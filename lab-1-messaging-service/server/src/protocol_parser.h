#ifndef __messaging_service_protocol_parser__
#define __messaging_service_protocol_parser__

#include <functional>
#include <string>

#include "templates.h"
#include "utils.h"

// Forward Declaration
class Server;

class ProtocolParser {
  public:

    ProtocolParser( int client, Server * server );

    // Protocol Spec:
    // - put
    // - list
    // - get
    // - reset
    std::string evaluate();
    std::string put(std::vector<std::string> arguments);
    std::string list(std::string name);
    std::string get(std::vector<std::string> arguments);
    std::string reset();

  private:
    int m_client;
    Server * m_server;

    std::vector<std::string> _explode_raw_request(std::string input) {
      std::string buffer;
      std::vector<std::string> arguments;

      for(auto c : input) {
        // Store the buffer if a space or new line
        if( (c == ' ') || (c == '\n') ) {
          // Add buffer only if it's not empty (mult delimiters)
          if( !buffer.empty() ) {
            arguments.push_back(buffer);
            buffer.clear();
          }
        }
        else {
          buffer.push_back( c );
        }
      }

      // Check for requests without trailing space or new line
      if( !buffer.empty() ) {
        arguments.push_back( buffer );
      }

      // Return
      return arguments;
    }

    bool _is_valid( std::string input ) {
      return (input.size() >= 4);
    }
};

#endif