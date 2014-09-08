#include "client_parser.h"
#include "repl.h"

std::string ClientParser::read(std::string command) {
  return command;
}

void ClientParser::send(std::string command) {
  std::cout << "- Type your message. End with a blank line -" << std::endl;
  
  std::string message;

  while(true) {
    std::string buffer;
    getline(std::cin, buffer);
    
    if( buffer.empty() ) {
      break;
    }
    
    message += buffer += "\n";
  }
  
}

std::string ClientParser::list(std::string command) {
  return command;
}

void ClientParser::quit() {
  REPL::release();
}