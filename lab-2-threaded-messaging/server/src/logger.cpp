#include "logger.h"

// Init static variables
Logger Logger::singleton_logger;
std::mutex Logger::m_logger_mutex;