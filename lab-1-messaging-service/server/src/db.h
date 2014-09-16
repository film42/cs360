#ifndef __messaging_service_db__
#define __messaging_service_db__

#include <string>
#include <vector>
#include <unordered_map>

#include "types.h"

class DB {
  public:
    static DB * get_instance() {

      if( !m_instance ) {
        m_instance = new DB();
      }

      return m_instance;
    }

    // Cleanup on delete
    ~DB() { if(m_instance) delete m_instance; }

    bool is_user(std::string username) {
      return m_data.count(username) > 0;
    }

    void add_message(std::string username, message_t message) {
      if( !is_user(username) ) {
        m_data[username] = message_vector_t();
      }

      m_data[username].push_back( message );
    }

    message_vector_t get( std::string username, int64_t index ) {
      if( !is_user(username) ) {
        return message_vector_t();
      }

      // Normalize the index from the 1-base protocol
      index--;

      if( m_data[username].size() <= index ) {
        return message_vector_t();
      }

      return { m_data[username].at( index ) };
    }

    std::vector<std::string> list( std::string username ) {

      std::vector<std::string> subjects;

      if( !is_user(username) ) {
        return subjects;
      }

      subjects.reserve( m_data[username].size() );

      // Map over messages and return only subjects
      for( auto message : m_data[username] ) {
        subjects.push_back( message.subject );
      }

      return subjects;
    }

    void delete_all() {
      // Clear the data map
      m_data.clear();
    }

  private:
    static DB * m_instance;

    std::unordered_map<std::string, message_vector_t > m_data;

};

#endif