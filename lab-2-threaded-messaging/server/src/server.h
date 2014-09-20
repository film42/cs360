#ifndef messaging_service_server_h
#define messaging_service_server_h

#include <iostream>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>

#include "templates.h"
#include "protocol_parser.h"
#include "logger.h"

class Server {

  public:

    void run( int port ) {

      // No client connected at `run()'
      m_client_connected = false;

      // create and run the server
      create( port );

      Logger::info("Socket connection established");

      serve();
    }

    void serve() {
      // setup client
      int client;
      struct sockaddr_in client_address;
      socklen_t client_length = sizeof(client_address);

      Logger::info("Waiting for incoming connection");

      // accept clients
      while ( (client = accept(m_server, (struct sockaddr *)&client_address, &client_length) ) > 0) {
        Logger::info("Received new connection");
        m_client_connected = true;
        handle(client);
      }

      Logger::info("Client accept loop has broken");

      close_socket();
    }

    void handle(int client) {
      Logger::info("Entered into the client handle loop");
      // loop to handle all requests
      while( true ) {
        // get a response
        std::string response = ProtocolParser( client, this ).evaluate();
        // break if client is done or an error occurred
        if (response.empty())
          break;
        // send response
        bool success = send_response( client, response );

        Logger::info("Sent response: " + response);

        // break if an error occurred
        if ( !success )
          break;

        Logger::info("Response sent to client");
      }

      close(client);

      Logger::info("Client disconnected: Connection closed");
    }

    bool send_response(int client, std::string request) {
      // prepare to send request
      const char * request_cstr = request.c_str();
      size_t bytes_left = request.length();
      ssize_t bytes_written;
      // loop to be sure it is all sent
      while (bytes_left) {
        if ( (bytes_written = send(client, request_cstr, bytes_left, 0) ) < 0) {
          if (errno == EINTR) {
            // the socket call was interrupted -- try again
            continue;
          } else {
            // an error occurred, so break out
            perror("write");
            return false;
          }
        } else if (bytes_written == 0) {
          // the socket is closed
          return false;
        }
        bytes_left -= bytes_written;
        request_cstr += bytes_written;
      }
      return true;
    }

    //
    // PARSING FROM SOCKET
    //

    // Read for n bytes
    std::string read_for(int client, size_t length) {

      // Copy the remaining buffer before we read
      std::string response = m_remaining_buffer;

      // Read until response length exceeds desired length
      while( response.length() < length ) {

        if( !m_client_connected ) break;

        // Read Once
        response.append( read_once( client ) );
      }

      // Store any buffer beyond `length'
      if( response.length() > length) {
        m_remaining_buffer = response.substr(length + 1);
      } else {
        m_remaining_buffer = "";
      }

      // Return `length' worth of bytes
      return response.substr(0, length);
    }

    // Reads at least up until the terminating char specified
    // Note: This can also read past it
    std::string read_until(int client, char terminator) {

      // Copy the remaining buffer before we read
      std::string response = m_remaining_buffer;

      // Read until we see the terminating char
      while( response.find(terminator) == std::string::npos ) {

        if( !m_client_connected ) break;

        // Read Once
        response.append( read_once( client ) );

      }

      // Store any buffer beyond terminator
      size_t termination_index = response.find(terminator);

      m_remaining_buffer = response.substr(termination_index + 1);

      // Return up until the terminator
      return response.substr(0, (termination_index + 1) );
    }

    // read_once reads from the socket a single time
    std::string read_once(int client) {
      char buffer[1025];

      ssize_t bytes_read = recv(client , buffer, 1024, 0);
      if (bytes_read < 0) {
        if (errno == EINTR) {
          // the socket call was interrupted -- try again
          return "";
        } else {
          // an error occurred, so break out
          m_client_connected = false;
          return "";
        }

      } else if (bytes_read == 0) {
        // the socket is closed
        m_client_connected = false;
        return "";
      }

      return std::string(buffer, bytes_read);
    }

    void close_socket() {
      close( m_server );
    }

  private:
    int m_server;
    bool m_client_connected;
    std::string m_remaining_buffer;

    void create(int port) {
      struct sockaddr_in server_address;

      // setup socket address structure
      memset(&server_address, 0, sizeof(server_address));
      server_address.sin_family = AF_INET;
      server_address.sin_port = htons(port);
      server_address.sin_addr.s_addr = INADDR_ANY;

      // create socket
      m_server  = socket(PF_INET, SOCK_STREAM, 0);
      if (!m_server ) {
        perror("socket");
        exit(-1);
      }

      // set socket to immediately reuse port when the application closes
      int reuse = 1;
      if (setsockopt(m_server , SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        perror("setsockopt");
        exit(-1);
      }

      // call bind to associate the socket with our local address and
      // port
      if (bind(m_server, (const struct sockaddr *)&server_address, sizeof(server_address) ) < 0) {
        perror("bind");
        exit(-1);
      }

      // convert the socket to listen for incoming connections
      if (listen(m_server ,SOMAXCONN) < 0) {
        perror("listen");
        exit(-1);
      }
    }
};

#endif