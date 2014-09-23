#include <iostream>
#include <unistd.h>

#include "server.h"
#include "logger.h"
#include "db.h"

// CLI Output
void error__print_arguments_list() {
  std::cout << "Argument           Definition" << std::endl;
  std::cout << "---------------------------------------------------------" << std::endl;
  std::cout << "-p [port]          port number of the messaging server" << std::endl;
  std::cout << "-t [thread count]  thread pool size: default 10" << std::endl;
  std::cout << "-b [client buffer] client buffer size: default 10" << std::endl;
  std::cout << "-d                 print debugging information" << std::endl;
}

int main(int argc, char **argv) {

  if( argc < 2 ) {
    error__print_arguments_list();
    exit(-1);
  }

  int option = -1;
  int port = -1;
  int threads = 10;
  int buffer_size = 10;

  Logger::set_log_level( LogLevel::FAIL );

  while( (option = getopt(argc, argv, "p:t:b:d")) != -1 ) {
    // Check options
    switch(option) {
      case 'p':
        port = std::atoi(optarg);
        break;
        
      case 'd':
        Logger::set_log_level( LogLevel::INFO );
        break;

      case 't':
        threads = std::atoi(optarg);
        break;

      case 'b':
        buffer_size = std::atoi(optarg);
        break;

      default:
        error__print_arguments_list();
        exit(-1);
    }
  }

  Logger::info( "Starting server on port " + std::to_string(port) );


  DB::get_instance();
  Logger::info("Databse loaded");

  Server server;

  // Run the server!
  server.run( port, threads /* Run server with default of 10 threads */, buffer_size /* Client buffer size */);

  return 0;
}
