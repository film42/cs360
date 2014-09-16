#ifndef messaging_service_client_h
#define messaging_service_client_h

#include <iostream>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>

class Client {
  public:
  
    static void setup( std::string host, int port ) {
      m_instance = new Client( host, port );
    }
  
    static Client * get_instance() {
      return m_instance;
    }

    bool send_request(std::string request) {
      // prepare to send request
      const char * request_cstr = request.c_str();
      size_t bytes_left = request.length();
      size_t bytes_written;
      // loop to be sure it is all sent
      while (bytes_left) {
        if ( (bytes_written = send(m_server, request_cstr, bytes_left, 0) ) < 0) {
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

    // Read for n bytes
    std::string read_for(size_t length) {
    
      // Copy the remaining buffer before we read
      std::string response = m_remaining_buffer;
      
      // Read until response length exceeds desired length
      while( response.length() < length ) {
       
        // Read Once
        response.append( read_once() );
      }
     
      // Store any buffer beyond `length'
      if( response.length() > length) {
        m_remaining_buffer = response.substr(length + 1);
      } else {
        m_remaining_buffer = "";
      }
      
//      std::cout << "::" << m_remaining_buffer << "::" << std::endl;
      
      // Return `length' worth of bytes
      return response.substr(0, length);
    }

    // Reads at least up until the terminating char specified
    // Note: This can also read past it
    std::string read_until(char terminator) {
    
      // Copy the remaining buffer before we read
      std::string response = m_remaining_buffer;
      
      // Read until we see the terminating char
      while( response.find(terminator) == std::string::npos ) {
      
        // Read Once
        response.append( read_once() );
        
      }
      
      // Store any buffer beyond terminator
      size_t termination_index = response.find(terminator);
      
      m_remaining_buffer = response.substr(termination_index + 1);
      
//      std::cout << "::" << m_remaining_buffer << "::" << std::endl;
      
      // Return up until the terminator
      return response.substr(0, (termination_index + 1) );
    }
  
    // read_once reads from the socket a single time
    std::string read_once() {
      char buffer[1025];
      
      ssize_t bytes_read = recv(m_server , buffer, 1024, 0);
      if (bytes_read < 0) {
        if (errno == EINTR)
          // the socket call was interrupted -- try again
          return "";
        else
          // an error occurred, so break out
          return "";
      } else if (bytes_read == 0) {
        // the socket is closed
        return "";
      }
      
//      std::cout << "=================================" << std::endl;
//      std::cout << std::string(buffer, bytes_read) << std::endl;
//      std::cout << "=================================" << std::endl;
      
      return std::string(buffer, bytes_read);
    }

    bool is_socket_open() {
      return m_socket_open;
    }

  private:
    int m_server;
    bool m_socket_open;
    std::string m_remaining_buffer;

    static Client * m_instance;
  
    Client( std::string host, int port ) {
      create( host, port );
    }

    void create( std::string host, int port ) {

      struct sockaddr_in server_address;

      m_socket_open = false;

      // use DNS to get IP address
      struct hostent *host_entry;
      host_entry = gethostbyname( host.c_str() );
      if(!host_entry) {
        std::cout << "No such host name: " << host << std::endl;
        m_socket_open = false;
        exit(-1);
      }

      // setup socket address structure
      memset( &server_address, 0, sizeof(server_address) );
      server_address.sin_family = AF_INET;
      server_address.sin_port = htons( port );
      memcpy( &server_address.sin_addr, host_entry->h_addr_list[0], host_entry->h_length );

      // create socket
      m_server = socket( PF_INET, SOCK_STREAM, 0 );
      if(!m_server) {
        perror("socket");
        m_socket_open = false;
        exit(-1);
      }

      // connect to server
      if( connect(m_server, (const struct sockaddr *)&server_address, sizeof(server_address) ) < 0) {
        perror("connect");
        m_socket_open = false;
        exit(-1);
      }

      m_socket_open = true;
    }
};

#endif