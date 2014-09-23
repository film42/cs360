#ifndef __messaging_service_thread_pool__
#define __messaging_service_thread_pool__

#include <semaphore.h>
#include <pthread.h>
#include <queue>
#include <vector>
#include <sys/socket.h>
#include <unistd.h>

#include "logger.h"

class ThreadPool {
  public:
    ThreadPool() {} /* Do nothing */

    ThreadPool(int capacity, int buffer_size)
      : m_capacity(capacity), m_buffer_size(buffer_size) { init(); }

    void enqueue(int client) {
      // Only fill the wait queue
      sem_wait(m_buffer_semaphore);
      Logger::info("Buffer size: " + std::to_string( m_queue.size() ) );

      sem_wait(m_queue_semaphore);

      Logger::info("Enqueued new client: " + std::to_string(client) );

      m_queue.push( client );

      Logger::info("Size of queue: " + std::to_string( m_queue.size() ) );
      sem_post(m_queue_semaphore);

      // Tell the thread pool to react
      Logger::info("Signaling a handle...");
      m_capacity = 400;

      signal_handler();
    }

    int pop() {
      sem_wait(m_queue_semaphore);

      Logger::info("Accessing queue: pop and front: " + std::to_string( m_queue.size()));
      int next = -1;
      if( m_queue.size() > 0 ) {
        next = m_queue.front();
        m_queue.pop();
      }
      sem_post(m_queue_semaphore);

      Logger::info("Dequeuing a client: " + std::to_string(next) );

      // Signal the buffer semaphore
      sem_post(m_buffer_semaphore);

      return next;
    }
  
    void signal_handler() {
      sem_post(m_thread_pool_semaphore);
    }

    void wait_for_handler() {
      sem_wait(m_thread_pool_semaphore);
    }

    int size() {
      sem_wait(m_queue_semaphore);
      int count = m_queue.size();
      sem_post(m_queue_semaphore);
      return count;
    }

    int capacity() { return m_capacity; }

    // Shutdown the thread pool
    void shutdown() {

      for(auto thread : m_thread_pool) {
        pthread_join(thread, NULL);
      }
    }

  private:
    int m_capacity;
    int m_buffer_size;
    std::queue<int> m_queue;
    std::vector<pthread_t> m_thread_pool;
    sem_t * m_queue_semaphore;
    sem_t * m_thread_pool_semaphore;
    sem_t * m_buffer_semaphore;

    void init();
};

#endif