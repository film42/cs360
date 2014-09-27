#include "thread_pool.h"
#include "handler.h"

//
// Free Method to generate our handlers
//
void * handle_generator(void * context) {

  auto thread_pool = static_cast<ThreadPool *>(context);

  // force handler to loop on pop()
  for(;;) {

    thread_pool->wait_for_handler();

    int client_socket = thread_pool->pop();

    // Bad client socket, or nothing available
    if(client_socket < 0) {
      continue;
    }

    Logger::info("Thread handling new client");

    Handler handle( client_socket );

    // Read from socket
    handle.start();
  }

}

//
// Private Method
//
void ThreadPool::init() {

  srand(time(NULL));

  Logger::info("Setting up Semaphores");

  // Setup the queue mutex
  sem_unlink("msg_thread_queue_mutex");
  m_queue_semaphore = sem_open("msg_thread_queue_mutex", O_CREAT, 0644, 1);

  if (m_queue_semaphore == SEM_FAILED) {
    perror("sem_open failed for queue mutex semaphore");
    return exit(-1);
  }

  // Setup the buffer semaphore with size of capacity
  sem_unlink("msg_buffer_counting_sem");
  m_buffer_semaphore = sem_open("msg_buffer_counting_sem", O_CREAT, 0644, m_buffer_size);

  if (m_buffer_semaphore == SEM_FAILED) {
    perror("sem_open failed for buffer semaphore");
    return exit(-1);
  }

  // Setup threads running in a thing
  sem_unlink("msg_thread_pool_mutex");
  m_thread_pool_semaphore = sem_open("msg_thread_pool_mutex", O_CREAT, 0644, 1);

  if (m_thread_pool_semaphore == SEM_FAILED) {
    perror("sem_open failed for thread pool semaphore");
    return exit(-1);
  }


  Logger::info("Setting up thread pool");

  // Block the thread_pool until `enqueue` signals
  sem_wait(m_thread_pool_semaphore);

  // Setup Thread Pool
  for( int i = 0; i < m_capacity; ++i ) {
    pthread_t new_thread;

    // How bad is this?
    pthread_create(&new_thread, NULL, &handle_generator, this);

    // Add new thread to thread pool
    m_thread_pool.push_back( new_thread );
  }

  Logger::info("Thread pool running");

}
