#include "message_parser.h"
#include "repl.h"

std::string MessageParser::read(std::string command) {
  return command;
}

void MessageParser::send(std::string command) {
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

std::string MessageParser::list(std::string command) {
  return command;
}

void MessageParser::quit() {
  REPL::release();
}