#include "CPU.h"

Chip::Chip(const char chipName[], uint8_t pinCount, uint64_t IO){
    this->pinCount = pinCount;
    this->IO = IO;
    this->chipName = chipName;
}

void Chip::addChild(Chip* chip){
    children.push_back(chip);
    chip->parent = this;
}

void Chip::setPinLevel(uint8_t pinNumber, bool level){
    if(!pinNumber || pinNumber > pinCount) throwException("Pin number %d of %s not valid. No level can be set!\n", pinNumber, chipName);
    if(!getBitAt(IO, pinNumber)) throwException("Pin %d of %s is not an OUTPUT\n", pinNumber, chipName);
    if(getBitAt(setPins, pinNumber)) throwException("Pin %d of %s has a value not read", pinNumber, chipName);

    uint64_t mask = 1<<pinNumber;
    setPins |= mask;        // Sets the pin as set. It will store a value to be read.
    pinoutSignals &= ~mask; // Clears the bit value
    if(level) pinoutSignals |= level<<pinNumber;  // Sets it on the level required.
}

void Chip::setPinIO(uint8_t pinNumber, bool i_o){
    if(!pinNumber || pinNumber > pinCount) throwException("Pin %d of %s not valid. Cannot set IO\n", pinNumber, chipName);

    uint64_t mask = ~(1<<pinNumber);
    IO &= mask;  // Clears the bit value
    if(i_o) IO |= i_o<<pinNumber;  // Sets it on the level required.
}

bool Chip::getPinLevel(uint8_t pinNumber){
    if(!pinNumber || pinNumber > pinCount) throwException("Pin %d of %s not valid. Cannot get pin level\n", pinNumber, chipName);

    // Multiple reads can be allowed, so the setPins variable just gets cleared.
    setPins &= ~(1<<pinNumber);
    return getBitAt(pinoutSignals, pinNumber);
}

void Chip::run(){
    expectsData = false;
    updateChildren = false;
    process();
    if(expectsData || updateChildren){
        // This will ensure that children get the data and furthermore, answer the call from the parent.
        postProcess();
        runChildren();
        if(expectsData) process();
    }
}

void Chip::runChildren(){
    for(Chip* child : children){
        child->run();
    }
}

CPU::CPU() : Chip("65C02", 40, 0){
    //this->IO = 0x0F<<CPU_DATA_BUS;
    RAM_address = {{15,0}, {14,0}};
}

void CPU::reset(){
    //std::ifstream input("C:\\Users\\dbeja\\Documents\\PlatformIO\\Projects\\6502_Emulator\\src\\a.out", std::ios::in | std::ios::binary);
    std::ifstream input(".\\a.out", std::ios::in | std::ios::binary);
    if(input.is_open()){
        input.seekg(0, std::ios::beg);
        input.read(reinterpret_cast<char *>(ROM), 8192);
    }else{
        char msg[60];
        std::sprintf(msg, "Could not read file a.out\n");
        throw std::invalid_argument(msg);
    }
    input.close();

    std::fill_n(RAM, std::size(RAM), 0);

    pc = readROM(0xFFFC) | (readROM(0xFFFD)<<8);
}

void CPU::writeDataBus(uint8_t data){
    this->dataBus = data;
    dataBusWritten = true;
}

uint8_t CPU::getDataBus(){
    return dataBus;
}

void CPU::clearBus(const char chipName[]){
    if(!dataBusWritten){
        throwException("CPU bus is empty and %s is trying to erase it!", chipName);
    }
    dataBusWritten = false;
    dataBus = 0;
}

uint8_t CPU::readROM(uint16_t add){
    return ROM[add - ROM_START];
}

uint8_t CPU::readRAM(uint16_t add){
    return RAM[add - RAM_START];
}

void CPU::writeRAM(uint16_t add, uint8_t data){
    RAM[add - RAM_START] = data;
}

void CPU::RAMListener(){
    for(AddressPin ad : RAM_address){
        if(getBitAt(addressBus, ad.pinNumber) != ad.value) return;
    }

    if(r_wb){
        dataBus = readRAM(addressBus);
    }else{
        writeRAM(addressBus, dataBus);
    }
}

// This method can be found in instruction.cpp
/*void CPU::process(){
    Instruction::fetchInstruction(this);
}*/

void CPU::postProcess(){
    RAMListener();
}