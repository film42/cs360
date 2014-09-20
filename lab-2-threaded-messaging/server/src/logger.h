#ifndef messaging_service_logger_h
#define messaging_service_logger_h

#include <iostream>
#include <atomic>
#include <string>
#include <thread>

enum class LogLevel {
  INFO,
  WARN,
  ERROR,
  FAIL
};

class Logger {
public:
  Logger() {
    set_log_level( LogLevel::WARN );
  }

  static void info( std::string message ) {
    std::lock_guard< std::mutex > lock(m_logger_mutex);
    
    singleton_logger.log_message( "[INFO] " + message , LogLevel::INFO );
  }

  static void warn( std::string message ) {
    std::lock_guard< std::mutex > lock(m_logger_mutex);
    
    singleton_logger.log_message( "[WARN] " + message , LogLevel::WARN );
  }

  static void error( std::string message ) {
    std::lock_guard< std::mutex > lock(m_logger_mutex);
    
    singleton_logger.log_message( "[ERROR] " + message , LogLevel::ERROR );
  }

  static void fail( std::string message ) {
    std::lock_guard< std::mutex > lock(m_logger_mutex);
    
    singleton_logger.log_message( "[FAILED] " + message , LogLevel::FAIL );
  }

  static void set_log_level( LogLevel level ) { singleton_logger.m_level = level; }

private:

  static Logger singleton_logger;
  
  static std::mutex m_logger_mutex;

  std::atomic< LogLevel > m_level;

  void log_message( std::string message, LogLevel level ) {
    // Chech if level severity is lower than desired
    if( m_level > level ) {
      return;
    }

    std::cout << message << std::endl;
  }
};

#endif