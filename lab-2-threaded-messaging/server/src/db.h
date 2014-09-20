#ifndef __messaging_service_db__
#define __messaging_service_db__

#include <string>
#include <vector>
#include <unordered_map>

#include "types.h"
#include "logger.h"

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
        Logger::info("Adding user: " + username);
        m_data[username] = message_vector_t();
      }

      m_data[username].push_back( message );
    }

    message_vector_t get( std::string username, int64_t index ) {
      if( !is_user(username) ) {
        Logger::info("Attempting to get user that doesn't exist: " + username);
        return message_vector_t();
      }

      Logger::info("Accessing data size " + std::to_string(m_data[username].size() )
          + " at " + std::to_string(index) );

      // Normalize the index from the 1-base protocol
      --index;

      if( m_data[username].size() <= index ) {
        Logger::info("Index " + std::to_string(index) + " is beyond range of message size "
            + std::to_string(m_data[username].size() ) );

        return message_vector_t();
      }

      auto message_obj = m_data[username].at( index );

      Logger::info("Getting message with subject: " + message_obj.subject);

      return { message_obj };
    }

    std::vector<std::string> list( std::string username ) {

      std::vector<std::string> subjects;

      Logger::info("Getting list of subjects for user: " + username);

      if( !is_user(username) ) {
        Logger::info("Cannot find username: \"" + username + "\"");
        return subjects;
      }

      Logger::info("Data size for list: " + std::to_string( m_data[username].size() ) );

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