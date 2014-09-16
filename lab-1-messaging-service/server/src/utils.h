#ifndef messaging_service_utils_h
#define messaging_service_utils_h

#include <string>
#include <vector>
#include <iostream>

namespace utils {
  // Safe string to int
  // This will attempt to get stoi to always word
  static int safe_stoi(std::string n) {
      int _n = 0;
      try {
        _n = std::atoi(n.c_str());
      } catch (...) {
        return -1;
      }

      return _n;
    }

  // Splits the string at white spaces
  // Warning: This will dissolve sequential whitespace
  static std::vector <std::string> split(std::string input) {
    std::vector <std::string> output;

    std::string buffer;
    for (int i = 0; i < input.length(); ++i) {
      // We have a sequence of chars at an empty space
      // Or we have reached the end of the input stream
      if ((input[i] == ' ') && !buffer.empty()) {
        output.push_back(buffer);
        buffer.clear();
      }
        // Otherwise, load characters into buffer
      else {
        buffer.push_back(input[i]);
      }
    }

    // Check for buffer after string ends
    if (!buffer.empty()) {
      output.push_back(buffer);
    }

    return output;
  }

}

#endif