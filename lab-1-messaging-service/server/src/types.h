#ifndef __messaging_service_types__
#define __messaging_service_types__

#include <vector>
#include <unordered_map>

struct message_t {
  message_t( std::string _subject, std::string _message )
      : subject(_subject), message(_message) {}

  std::string subject;
  std::string message;
};

typedef std::vector<message_t > message_vector_t;

#endif