#ifndef __messaging_service_handler__
#define __messaging_service_handler__

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <string>

class ProtocolParser;

class Handler {
  public:
    Handler(int client)
      : m_client(client), m_client_connected(true) {}

    void start();

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

  private:
    bool m_client_connected;
    std::string m_remaining_buffer;
    int m_client;

};

#endif