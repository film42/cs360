#ifndef messaging_service_templates_h
#define messaging_service_templates_h

#include <string>
#include <sstream>
#include <vector>
#include "types.h"

namespace Response {
  // Response
  // OK\n
  static std::string ok() {
    return "OK\n";
  }

  // Response
  // error [description]\n
  static std::string error(std::string description) {
    return "error " + description + "\n";
  }

  // Response
  // message [subject] [length]\n[message]
  static std::string message(message_t msg) {
    std::stringstream ostream;

    ostream << "message " << msg.subject << " " << msg.message.length() << "\n" << msg.message;

    return ostream.str();
  }
  
  // Response
  // list [number]\n[index] [subject]\n...[index] [subject]\n
  static std::string list(std::vector< std::string > subjects) {
    std::stringstream ostream;
    
    ostream << "list " << subjects.size();
    
    for(int i = 0; i < subjects.size(); ++i) {
      // Account for the 1-base protocl, so we use (i + 1)
      ostream << "\n" << (i + 1) << " " << subjects.at(i);
    }

    ostream << "\n";
    
    return ostream.str();
  }
};

namespace Request {
  // Request
  // put [name] [subject] [length]\n[message]
  static std::string put(std::string name, std::string subject, std::string message) {
    std::stringstream ostream;

    ostream << "put " << name << " " << subject << " " << message.length() << "\n" << message;

    return ostream.str();
  }

  // Request
  // list [name]\n
  static std::string list(std::string name) {
    std::stringstream ostream;

    ostream << "list " << name << "\n";

    return ostream.str();
  }

  // Request
  // get [name] [index]\n
  static std::string get(std::string name, int64_t index) {
    std::stringstream ostream;

    ostream << "get " << name << " " << index  << "\n";

    return ostream.str();
  }

  // Request
  // reset\n
  static std::string reset() {
    return "reset\n";
  }
};

#endif
