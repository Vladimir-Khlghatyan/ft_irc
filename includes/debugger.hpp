#pragma once

#include <iostream>

#define RED     "\33[1;31m"
#define GREEN	"\33[1;32m"
#define YELLOW	"\33[1;33m"
#define BLUE	"\33[1;34m"
#define PINK	"\33[1;35m"
#define CYAN	"\33[1;36m"
#define WHITE	"\33[1;37m"
#define RESET	"\33[0;m"

#define ERRORLOGGER
#undef  ERRORLOGGER

#define DEBUGGER() errorLogger(__FILE__, __func__, __LINE__)

void errorLogger(const char* fileName, const char* functionName, int lineNumber);

