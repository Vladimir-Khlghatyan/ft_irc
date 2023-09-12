#include "debugger.hpp"

void errorLogger(const char* fileName, const char* functionName, int lineNumber) 
{
    (void)fileName;
    (void)functionName;
    (void)lineNumber;
   
    #ifdef ERRORLOGGER
    std::cerr << RED << "DEBUGGER:";
    std::cerr << YELLOW << "   FILE: " << WHITE << fileName;
    std::cerr << YELLOW << "   FUNC: " << WHITE << functionName;
    std::cerr << YELLOW << "   LINE: " << WHITE << lineNumber;
    std::cerr << RESET << std::endl;
    #endif
}
