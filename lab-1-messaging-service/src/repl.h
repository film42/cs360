#ifndef __messaging_service__repl__
#define __messaging_service__repl__

#include <iostream>

#include "message_parser.h"

class REPL {
  public:
  
    // Needs a better name, but this essentially binds the console to the REPL
    static void bind(MessageParser evaluator) {
      // Setup
      auto repl = REPL(evaluator);
      // Start
      repl._start();
    }
  
    // Releases repl from terminal
    static void release() {
      m_lock = false;
    }
  
    // Constructor
    REPL(MessageParser evaluator) : m_evaluator(evaluator) {}
  
    // Members
    std::string read() {
      std::cout << "% ";
      
      std::string buffer;
      getline(std::cin, buffer);
      return buffer;
    }
  
  private:
    MessageParser m_evaluator;
  
    // A hacky `mutex`
    static bool m_lock;
  
    void _start() {
      // Lock the session
      m_lock = true;
      // Loop until we release
      while(m_lock) {
        // READ
        auto input = read();
        // EVALUATE
        auto respone = m_evaluator.evaluate(input);
        // PRINT
        std::cout << respone << std::endl;
      }
    }
  
};

#endif /* defined(__messaging_service__repl__) */
