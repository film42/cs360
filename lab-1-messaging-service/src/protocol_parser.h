#ifndef __messaging_service_protocol_parser__
#define __messaging_service_protocol_parser__

#include <functional>
#include <string>

#include "templates.h"

class ProtocolParser {
  public:

    // Protocol Spec:
    // - put
    // - list
    // - get
    // - reset
    std::string evaluate(std::string input) {
      if( !_is_valid(input) ) {
        return Response::error("Error! Could not parse request");
      }

      if( input.find("put ") == 0 ) {
        auto arguments = _explode_raw_request(input.substr(4));
        return put( arguments );
      }

      if( input.find("list ") == 0 ) {
        return list( input.substr(5) );
      }

      if( input.find("get ") == 0 ) {
        auto arguments = _explode_raw_request(input.substr(4));
        return get( arguments );
      }

      if( input == "reset\n" ) {
        return reset();
      }

      return Response::error("Error! Could not parse request");
    }

    std::string put(std::vector<std::string> arguments);
    std::string list(std::string name);
    std::string get(std::vector<std::string> arguments);
    std::string reset();

  private:

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