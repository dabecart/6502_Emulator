#ifndef CONSTANTS_h
#define CONSTANTS_h

#include <cstdint>
#include <iostream>

#define ROM_SIZE 8192
#define ROM_START 0xE000

#define RAM_SIZE 16384
#define RAM_START 0x0000

inline bool getBitAt(uint32_t data, uint8_t position){
    return (data>>position)&0x01;
}

#endif