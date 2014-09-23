#include "handler.h"
#include "protocol_parser.h"
#include "logger.h"

void Handler::start() {
  Logger::info("Entered into the client handle loop");
  // loop to handle all requests
  while( true ) {
    // get a response
    std::string response = ProtocolParser( m_client, this ).evaluate();
    // break if client is done or an error occurred
    if( response.empty() )
      break;
    // send response
    bool success = send_response( m_client, response );

    Logger::info("Sent response!");

    // break if an error occurred
    if ( !success )
      break;

    Logger::info("Response sent to client");
  }

  close(m_client);

  Logger::info("Client disconnected: Connection closed");
}