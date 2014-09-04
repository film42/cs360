#ifndef __messaging_service__context__
#define __messaging_service__context__

#include <functional>
#include <string>

class MessageParser {
  public:
  
    std::string evaluate(std::string input) {
    
      if( !_is_valid(input) ) {
        return "Error! Could not parse input";
      }
      
      auto method = input.substr(0, 4); // EX: send, read, list, quit
      
      // Check for quit
      if( method.find("quit") != std::string::npos ) {
        quit();
        return "";
      }

      // Check for command
      if( input.size() < 6 ) {
        return "Error! Could not parse input";
      }
      
      // Determine command
      if( method.find("read") != std::string::npos ) {
        return read( input.substr(5) );
      }
      
      if( method.find("send") != std::string::npos ) {
        send( input.substr(5) );
        return "";
      }
      
      if( method.find("list") != std::string::npos ) {
        return list( input.substr(5) );
      }
      
      return "Error! Could not parse input";
    }
  
    std::string read(std::string command);
    void send(std::string command);
    std::string list(std::string command);
    void quit();
  
  private:
  
    bool _is_valid( std::string input ) {
      return (input.size() >= 4);
    }
};

#endif /* defined(__messaging_service__context__) */
