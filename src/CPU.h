#ifndef CPU_h
#define CPU_h

#include "constants.h"
#include <fstream>
#include <iterator>

class CPU{

    public:
    CPU();

    uint16_t pc;
    
    bool n = 0; // Negative         1 = negative
    bool v = 0; // Overflow         1 = overflow
    bool b = 0; // Break instruct   1 = Break caused interrupt
    bool d = 0; // Decimal mode     1 = Decimal mode
    bool i = 0; // IRQ Disable      1 = disabled
    bool z = 0; // Zero             1 = result 0 
    bool c = 0; // Carry            1 = carry

    uint8_t x = 0, y = 0, a = 0;
    uint16_t stackPointer;

    uint32_t cycleCounter = 0;

    void reset();
    void clearBus();
    uint8_t readROM(uint16_t add);
    uint8_t readRAM(uint16_t add);
    void writeRAM(uint16_t add, uint8_t data);
    
    bool writeToBus = false;
    uint8_t dataBus;
    uint16_t addressBus;

    private:
    uint8_t ROM[ROM_SIZE];
    uint8_t RAM[RAM_SIZE];

};

#endif