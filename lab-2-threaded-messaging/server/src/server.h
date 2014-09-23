#ifndef messaging_service_server_h
#define messaging_service_server_h

#include <iostream>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>

#include "templates.h"
#include "protocol_parser.h"
#include "logger.h"
#include "thread_pool.h"

class Server {

  public:
  
    ~Server() { if(m_thread_pool) delete m_thread_pool; }

    void run( int port, int thread_pool_size, int buffer_size ) {

      // create and run the server
      create( port );

      Logger::info("Socket connection established");

      m_thread_pool = new ThreadPool( thread_pool_size, buffer_size );

      Logger::info("Thread Pool running at size: " +  std::to_string(thread_pool_size) );

      serve();
    }

    void serve() {
      // setup client
      int client;
      struct sockaddr_in client_address;
      socklen_t client_length = sizeof(client_address);

      Logger::info("Waiting for incoming connection");

      // accept clients
      while ( (client = accept(m_server, (struct sockaddr *)&client_address, &client_length) ) > 0) {
        Logger::info("Received new connection");
        m_thread_pool->enqueue( client );
        Logger::info("Added to queue");
      }

      Logger::info("Client accept loop has broken");

      close_socket();
    }

    void close_socket() {
      close( m_server );
    }

  private:
    int m_server;
    ThreadPool * m_thread_pool;

    void create(int port) {
      struct sockaddr_in server_address;

      // setup socket address structure
      memset(&server_address, 0, sizeof(server_address));
      server_address.sin_family = AF_INET;
      server_address.sin_port = htons(port);
      server_address.sin_addr.s_addr = INADDR_ANY;

      // create socket
      m_server  = socket(PF_INET, SOCK_STREAM, 0);
      if (!m_server ) {
        perror("socket");
        exit(-1);
      }

      // set socket to immediately reuse port when the application closes
      int reuse = 1;
      if (setsockopt(m_server , SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        perror("setsockopt");
        exit(-1);
      }

      // call bind to associate the socket with our local address and
      // port
      if (bind(m_server, (const struct sockaddr *)&server_address, sizeof(server_address) ) < 0) {
        perror("bind");
        exit(-1);
      }

      // convert the socket to listen for incoming connections
      if (listen(m_server ,SOMAXCONN) < 0) {
        perror("listen");
        exit(-1);
      }
    }
};

#endif