#ifndef INSTRUCTION_h
#define INSTRUCTION_h

#include "constants.h"
#include "CPU.h"
#include <functional>

#define IMMEDIATE       0
#define IMPLIED         1
#define ACCUMULATOR     2
#define ABSOLUTE        3
#define DP              4
#define ABS_INDEXED_X   5
#define ABS_INDEXED_Y   6
#define DP_INDEXED_X    7
#define DP_INDIRECT_INDEXED_X 8
#define DP_INDIRECT_INDEXED_Y 9

#include <string>
#include <sstream>

#define MAX_SUBROUTINE_JUMPS 5 //For debug purposes

class Instruction {

    public: 
    static const Instruction INSTRUCTIONS[];
    static void fetchInstruction(CPU* cpu);
    static uint8_t nextInstructionCycleIncrease;

    Instruction(std::function<void(Instruction*, CPU*)> function, uint8_t opcode, uint8_t addressingMode, uint8_t byteLength, uint8_t numberOfCycles, bool needsInput = false);
    uint8_t opcode;
    uint8_t addressingMode;
    uint8_t byteLength;
    uint8_t numberOfCycles;
    std::function<void(Instruction*, CPU*)> function;
    bool needsInput; // True if to run this instruction, first other chips must do some other work.

    uint16_t args;
    std::string name;

    void printDecodedInstruction(CPU*);
    // Used for spacing in debugging
    static uint8_t subroutineJumps;

};

#endif