#ifndef CPU_h
#define CPU_h

#include "constants.h"
#include <fstream>
#include <iterator>
#include <vector>

typedef struct{
    uint8_t pinNumber;
    bool value;
} AddressPin;
typedef std::vector<AddressPin> AddressList;

class Chip {
    public:
    Chip(const char chipName[], uint8_t pinCount, uint64_t IO);

    const char *chipName;

    uint8_t pinCount = 0; // Number of pins of the chip.
    uint64_t pinoutSignals = 0; // Logic levels of the pins.

    // Keeps control of which of the pins have its value set anywhere in code (1). When readed it
    // clears to 0. This is a mean to beware of shortcircuits and bus contention.
    // If a pin level is set, and other chip tries to change the pin level, it
    // will trigger an exception.
    uint64_t setPins = 0;
    // 1 = Input. 0 = Output.
    uint64_t IO = 0;

    // If a chip is waiting for data it will be later processed in the queue.
    bool expectsData = false;

    void setPinLevel(uint8_t pinNumber, bool level);
    void setPinIO(uint8_t pinNumber, bool i_o);
    bool getPinLevel(uint8_t pinNumber);
};

class CPU : public Chip{

    #define CPU_DATA_BUS 33 //Goes from 33 (D0) to 26 (D7)

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

    void writeDataBus(uint8_t data);
    uint8_t getDataBus();
    // clearBus() has to be called whenever data is read, so the emulator acknowledges that there has not been an 
    // useless write call.
    void clearBus(const char chipName[]);
    uint8_t readROM(uint16_t add);

    void RAMListener();
    
    bool dataBusWritten = false; // Write to data bus
    uint16_t addressBus;
    bool r_wb = true;   // Read/Write Bar. Reading = 1, Writing = 0.

    uint8_t readRAM(uint16_t add);
    void writeRAM(uint16_t add, uint8_t data);

    private:
    uint8_t ROM[ROM_SIZE];
    uint8_t RAM[RAM_SIZE];

    AddressList RAM_address;

    uint8_t dataBus;

};

#endif