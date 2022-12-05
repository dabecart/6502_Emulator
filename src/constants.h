#ifndef CONSTANTS_h
#define CONSTANTS_h

#include <cstdint>
#include <iostream>
#include <string>

using namespace std;

// For eliptic arguments
#include <stdarg.h>

#define ROM_SIZE 8192
#define ROM_START 0xE000

#define RAM_SIZE 16384
#define RAM_START 0x0000

inline bool getBitAt(uint32_t data, uint8_t position){
    return (data>>position)&0x01;
}

inline void throwException(const char format[], ...){
    cout << endl;
    char msg[200];
    va_list args;
    va_start(args, format);
    std::sprintf(msg, format, args);
    va_end(args);
    throw std::invalid_argument(msg);
}

#endif