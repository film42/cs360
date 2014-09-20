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

    ThreadPool(int capacity)
      : m_capacity(capacity) { init(); }

    void enqueue(int client) {
      sem_wait(m_queue_semaphore);

      Logger::info("Enqueued new client: " + std::to_string(client) );

      m_queue.push( client );

      Logger::info("Front of queue: " + std::to_string( m_queue.front()) );
      sem_post(m_queue_semaphore);

      // Tell the thread pool to react
      Logger::info("Signaling a handle...");
      m_capacity = 400;
      handle_signal();
    }

    int pop() {
      sem_wait(m_queue_semaphore);
      std::cout <<  m_capacity << std::endl;

      Logger::info("Accessing queue: pop and front: " + std::to_string( m_queue.size()));
      int next = -1;
      if( m_queue.size() > 0 ) {
        next = m_queue.front();
        m_queue.pop();
      }
      sem_post(m_queue_semaphore);

      Logger::info("Dequeuing a client: " + std::to_string(next) );

      return next;
    }
  
    void handle_signal() {
      sem_post(m_thread_pool_semaphore);
    }

    void handle_wait() {
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
    std::queue<int> m_queue;
    std::vector<pthread_t> m_thread_pool;
    sem_t * m_queue_semaphore;
    sem_t * m_thread_pool_semaphore;

    void init();
};

#endif