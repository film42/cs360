#ifndef __messaging_service__repl__
#define __messaging_service__repl__

#include <iostream>

#include "client_parser.h"

class REPL {
  public:
  
    // Needs a better name, but this essentially binds the console to the REPL
    static void bind() {
      // Setup
      auto repl = REPL( ClientParser()  );
      // Start
      repl._start();
    }
  
    // Releases repl from terminal
    static void release() {
      m_lock = false;
    }
  
    // Constructor
    REPL(ClientParser evaluator) : m_evaluator(evaluator) {}
  
    // Members
    std::string read() {
      std::cout << "% ";
      
      std::string buffer;
      getline(std::cin, buffer);
      return buffer;
    }
  
  private:
    ClientParser m_evaluator;
  
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
        auto response = m_evaluator.evaluate(input);
        // PRINT
        std::cout << response << std::endl;
      }
    }
  
};

#endif /* defined(__messaging_service__repl__) */
