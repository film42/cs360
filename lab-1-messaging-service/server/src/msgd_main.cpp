#include <iostream>
#include <unistd.h>

#include "server.h"
#include "logger.h"

// CLI Output
void error__print_arguments_list() {
  std::cout << "Argument           Definition" << std::endl;
  std::cout << "---------------------------------------------------------" << std::endl;
  std::cout << "-p [port]          port number of the messaging server" << std::endl;
  std::cout << "-d                 print debugging information" << std::endl;
}

int main(int argc, char **argv) {

  if( argc < 2 ) {
    error__print_arguments_list();
  }

  int option = -1;
  int port = -1;

  Logger::set_log_level( LogLevel::FAIL );

  while( (option = getopt(argc, argv, "p:d")) != -1 ) {
    // Check options
    switch(option) {
      case 'p':
        port = std::atoi(optarg);
        break;
        
      case 'd':
        Logger::set_log_level( LogLevel::INFO );
        break;
        
      default:
        error__print_arguments_list();
        exit(-1);
    }
  }

  Logger::info( "Starting server on port " + std::to_string(port) );

  Server server;

  // Run the server!
  server.run( port );

  return 0;
}
