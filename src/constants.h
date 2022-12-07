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

inline void throwException(const char format[], ...){
    cout << endl;
    char msg[200];
    va_list args;
    va_start(args, format);
    std::sprintf(msg, format, args);
    va_end(args);
    throw std::invalid_argument(msg);
}

inline bool getBitAt(uint64_t data, uint8_t position){
    return (data>>position)&1LL;
}

inline void setBitAt(uint8_t &data, uint8_t position, bool level){
    if(position > 7) throwException("Cannot set bit position bigger than 7");
    uint8_t mask = ~(1<<position);
    data &= mask;
    if(level) data |= (1<<position);
}

inline void setBitAt(uint16_t &data, uint8_t position, bool level){
    if(position > 15) throwException("Cannot set bit position bigger than 15");
    uint16_t mask = ~(1<<position);
    data &= mask;
    if(level) data |= (1<<position);
}

inline void setBitAt(uint64_t &data, uint8_t position, bool level){
    if(position > 63) throwException("Cannot set bit position bigger than 63");
    uint64_t mask = ~(1LL<<position);
    data &= mask;
    if(level) data |= (1LL<<position);
}

template< typename T >
inline std::string int_to_hex( T i ) {
  char hexString[20];
  std::string format = "$";
  if(sizeof(T) == 1) format += "%02X";
  else if(sizeof(T) == 2) format += "%04X";
  else format += "%X";
  sprintf(hexString, format.c_str(), i);
  return std::string(hexString);
}

#endif