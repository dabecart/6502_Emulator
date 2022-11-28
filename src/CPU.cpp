#include "CPU.h"

CPU::CPU(){}

void CPU::reset(){
    std::ifstream input(".\\a.out", std::ios::in | std::ios::binary);
    if(input.is_open()){
        input.seekg(0, std::ios::beg);
        input.read(reinterpret_cast<char *>(ROM), 8192);
    }else{
        std::cerr << "Could not read file a.out\n";
    }
    input.close();

    std::fill_n(RAM, std::size(RAM), 0);

    pc = readROM(0xFFFC) | (readROM(0xFFFD)<<8);
}

void CPU::clearBus(){
    writeToBus = false;
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


