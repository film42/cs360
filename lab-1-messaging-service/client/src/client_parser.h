#ifndef __messaging_service__context__
#define __messaging_service__context__

#include <vector>
#include <string>
#include <iostream>

class ClientParser {
  public:

    void evaluate(std::string input) {

      if( !_is_valid(input) ) {
        std::cout << "Error! Could not parse input" << std::endl;
        return;
      }

      auto method = input.substr(0, 4); // EX: send, read, list, quit

      // Check for quit
      if( method.find("quit") != std::string::npos ) {
        quit();
        return;
      }

      // Check for command
      if( input.size() < 6 ) {
        std::cout << "Error! Could not parse input" << std::endl;
        return;
      }

      // Determine command
      if( method.find("read") != std::string::npos ) {
        read( input.substr(5) );
        return;
      }

      if( method.find("send") != std::string::npos ) {
        send( input.substr(5) );
        return;
      }

      if( method.find("list") != std::string::npos ) {
        list( input.substr(5) );
        return;
      }

      std::cout << "Error! Could not parse input" << std::endl;
    }

    void read(std::string command);
    void send(std::string command);
    void list(std::string command);
    void quit();

  private:

    bool is_error( std::string response ) {

      if( response.find("error") != std::string::npos ) {
        return true;
      }

      return false;
    }

    // Safe string to int
    // This will attempt to get stoi to always word
    int safe_stoi( std::string n ) {
      int _n = 0;
      try {
        _n = std::atoi( n.c_str() );
      } catch (...) {
        return -1;
      }

      return _n;
    }

    // Splits the string at white spaces
    // Warning: This will dissolve sequential whitespace
    std::vector<std::string> split( std::string input ) {
      std::vector<std::string> output;

      std::string buffer;
      for(int i = 0; i < input.length(); ++i) {
        // We have a sequence of chars at an empty space
        // Or we have reached the end of the input stream
        if( (input[i] == ' ' ) && !buffer.empty() ) {
          output.push_back( buffer );
          buffer.clear();
        }
        // Otherwise, load characters into buffer
        else {
          buffer.push_back( input[i] );
        }
      }

      // Check for buffer after string ends
      if( !buffer.empty() ) {
        output.push_back( buffer );
      }

      return output;
    }

    bool _is_valid( std::string input ) {
      return (input.size() >= 4);
    }

};

#endif /* defined(__messaging_service__context__) */
