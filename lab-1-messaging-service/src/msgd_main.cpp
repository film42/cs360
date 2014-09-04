#include <iostream>
#include <unistd.h>

#include "some.h"

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

  int option = 23;
  int port = -1;
  std::string host;
  
  while( (option = getopt(argc, argv, "p:d")) != -1 ) {
    // Check options
    switch(option) {
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

  return 0;
}
