#ifndef __messaging_service_protocol_parser__
#define __messaging_service_protocol_parser__

#include <functional>
#include <string>

#include "templates.h"
#include "utils.h"
#include "handler.h"

// Forward Declaration
class Handler;

class ProtocolParser {
  public:

    ProtocolParser( int client, Handler * handler );

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
    Handler * m_handler;

    bool _is_valid( std::string input ) {
      return (input.size() >= 4);
    }
};

#endif