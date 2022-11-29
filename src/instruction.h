#ifndef INSTRUCTION_h
#define INSTRUCTION_h

#include "constants.h"
#include "CPU.h"
#include <functional>

#define IMMEDIATE 0
#define ABSOLUTE 1
#define DP 2
#define ABS_INDEXED_X 3
#define ABS_INDEXED_Y 4
#define DP_INDEXED_X 5
#define DP_INDIRECT_INDEXED_X 6
#define DP_INDIRECT_INDEXED_Y 7
#define IMPLIED 8
#define ACCUMULATOR 9

#include <string>
#include <sstream>
#include <iomanip>

#define MAX_SUBROUTINE_JUMPS 2 //For debug purposes

class Instruction {

    public: 
    static const Instruction INSTRUCTIONS[];
    static void fetchInstruction(CPU &cpu);
    static uint8_t nextInstructionCycleIncrease;

    Instruction(std::function<void(Instruction*, CPU&)> function, uint8_t opcode, uint8_t addressingMode, uint8_t byteLength, uint8_t numberOfCycles);
    uint8_t opcode;
    uint8_t addressingMode;
    uint8_t byteLength;
    uint8_t numberOfCycles;
    std::function<void(Instruction*, CPU&)> function;

    uint16_t args;
    std::string name;

    void printDecodedInstruction(CPU);
    // Used for spacing in debugging
    static uint8_t subroutineJumps;

};

#endif