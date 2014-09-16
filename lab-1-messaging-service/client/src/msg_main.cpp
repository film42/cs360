#include <iostream>
#include <unistd.h>
#include <chrono>
#include <thread>

#include "repl.h"
#include "client.h"
#include "templates.h"

void error__print_arguments_list() {
  std::cout << "Argument           Definition" << std::endl;
  std::cout << "---------------------------------------------------------" << std::endl;
  std::cout << "-s [server]        machine name of the messaging server (e.g. hiking.cs.byu.edu)" << std::endl;
  std::cout << "-p [port]          port number of the messaging server" << std::endl;
  std::cout << "-d                 print debugging information" << std::endl;
}

int main(int argc, char **argv) {
  
  if( argc < 2 ) {
    error__print_arguments_list();
  }
  
  // DEFAULTS
  int port = 3001;
  std::string host = "localhost";
  
  // READ OPTIONS
  int option = -1;
  while( (option = getopt(argc, argv, "s:p:d")) != -1 ) {
    // Check options
    switch(option) {
      case 's':
        host = std::string(optarg);
        break;

      case 'p':
        port = std::atoi(optarg);
        break;
        
      case 'd':
        std::cout << "Alter log level" << std::endl;
        break;
        
      default:
        error__print_arguments_list();
        return -1;
    }
  }

  std::cout << host << " : " << port << std::endl;

  Client::setup(host, port);

  // Start the REPL
  REPL::bind();

  return 0;
}
