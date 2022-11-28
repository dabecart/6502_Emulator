#include "instruction.h"

uint8_t Instruction::nextInstructionCycleIncrease = 0;
uint8_t Instruction::subroutineJumps = 0;

void throwAddressingModeError(Instruction* instr){
    char msg[60];
    std::sprintf(msg, "Instruction %s does not support %d addressing mode!", instr->name.c_str(), instr->addressingMode);
    throw std::invalid_argument(msg);
}

void AND(Instruction* instr, CPU& cpu){
    instr->name = "and";

    cpu.a &= instr->args;
    // FLAGS:
    // Set if MSB of result is 1.
    cpu.n = cpu.a>>7;
    // Set if value is 0
    cpu.z = cpu.a==0;
}

void INC(Instruction* instr, CPU& cpu){
    instr->name = "inc";

    uint8_t result = 0;
    switch (instr->addressingMode) {
    case ACCUMULATOR:
        result = ++cpu.a;
        break;

    case ABSOLUTE:
        cpu.writeRAM(instr->args, result = cpu.readRAM(instr->args)+1);
        break;

    default:
        throwAddressingModeError(instr);
        break;
    }
    
    // FLAGS:
    // Set if MSB of result is 1.
    cpu.n = result>>7;
    // Set if value is 0
    cpu.z = result==0;
}

void INX(Instruction* instr, CPU& cpu){
    instr->name = "inx";

    cpu.x++;

    // FLAGS:
    // Set if MSB of result is 1.
    cpu.n = cpu.x>>7;
    // Set if value is 0
    cpu.z = cpu.x==0;
}

void INY(Instruction* instr, CPU& cpu){
    instr->name = "iny";

    cpu.y++;

    // FLAGS:
    // Set if MSB of result is 1.
    cpu.n = cpu.y>>7;
    // Set if value is 0
    cpu.z = cpu.y==0;
}

void JMP(Instruction* instr, CPU& cpu){
    instr->name = "jmp";

    // As a cheat, I have to remove the bytes required for the JMP because they will later
    // be added to the PC in the simulation. 
    cpu.pc = instr->args - instr->byteLength;
}

void JSR(Instruction* instr, CPU& cpu){
    instr->name = "jsr";

    if(cpu.stackPointer == 0) std::cout << "Stack overflow JSR 0! ";
    // Temporal value to point to the next byte after the instruction argument bytes.
    cpu.pc += instr->byteLength-1;
    // First store the high byte of the address.
    cpu.writeRAM(cpu.stackPointer--, cpu.pc>>8);

    if(cpu.stackPointer == 0) std::cout << "Stack overflow JSR 1! ";
    // Then the low byte.
    cpu.writeRAM(cpu.stackPointer--, cpu.pc);
    
    // As a cheat, I have to remove the bytes required for the JSR because they will later
    // be added to the PC in the simulation. 
    // In the real system, the address stored in the STACK is the CURRENT PC, later it will
    // be added one more to get the next address.
    cpu.pc = instr->args - instr->byteLength;

    instr->subroutineJumps++;
}

void LDA(Instruction* instr, CPU& cpu){
    instr->name = "lda";

    cpu.a = instr->args;
    // FLAGS:
    // Set if MSB of loaded value is 1.
    cpu.n = instr->args>>7;
    // Set if value is 0
    cpu.z = instr->args==0;
}

void LDX(Instruction* instr, CPU& cpu){
    instr->name = "ldx";

    cpu.x = instr->args;
    // FLAGS:
    // Set if MSB of loaded value is 1.
    cpu.n = instr->args>>7;
    // Set if value is 0
    cpu.z = instr->args==0;
}

void LDY(Instruction* instr, CPU& cpu){
    instr->name = "ldy";

    cpu.y = instr->args;
    // FLAGS:
    // Set if MSB of loaded value is 1.
    cpu.n = instr->args>>7;
    // Set if value is 0
    cpu.z = instr->args==0;
}

void ORA(Instruction* instr, CPU& cpu){
    instr->name = "ora";

    cpu.a |= instr->args;
    // FLAGS:
    // Set if MSB of result is 1.
    cpu.n = cpu.a>>7;
    // Set if value is 0
    cpu.z = cpu.a==0;
}

void ROL(Instruction* instr, CPU& cpu){
    instr->name = "rol";

    cpu.a = (cpu.a << 1) | cpu.c;

    // FLAGS:
    // Set if MSB of result is 1.
    cpu.n = cpu.a>>7;
    // MSB of address becomes the carry.
    cpu.c = instr->args>>7;
    cpu.z = cpu.a == 0;
}

void ROR(Instruction* instr, CPU& cpu){
    instr->name = "ror";

    cpu.a = (cpu.a >> 1) | (cpu.c << 7);

    // FLAGS:
    // Set if MSB of result is 1.
    cpu.n = cpu.a>>7;
    // LSB of address becomes the carry.
    cpu.c = instr->args>>7;
    cpu.z = cpu.a == 0;
}

void RTS(Instruction* instr, CPU& cpu){
    if(cpu.stackPointer == 0xFE) std::cout << "Stack overflow JSR 0! ";
    if(cpu.stackPointer == 0xFF) std::cout << "Stack overflow JSR 1! ";
    uint16_t returnAdd = cpu.readRAM(++cpu.stackPointer) | (cpu.readRAM(++cpu.stackPointer)<<8);
    
    // It should be returnAdd++, but as it also happens with JSR: I have to remove the 
    // byte required for the RTS because it will later be added to the PC in the 
    // simulation. 
    cpu.pc = returnAdd;

    instr->subroutineJumps--;

    instr->name = "rts";
}

void STA(Instruction* instr, CPU& cpu){
    cpu.writeToBus = true;
    cpu.addressBus = instr->args;
    cpu.dataBus = cpu.a;

    instr->name = "sta";
}

void STX(Instruction* instr, CPU& cpu){
    cpu.writeToBus = true;
    cpu.addressBus = instr->args;
    cpu.dataBus = cpu.x;

    instr->name = "stx";
}

void STY(Instruction* instr, CPU& cpu){
    cpu.writeToBus = true;
    cpu.addressBus = instr->args;
    cpu.dataBus = cpu.y;

    instr->name = "sty";
}

void TAX(Instruction* instr, CPU& cpu){
    cpu.x = cpu.a;
    // FLAGS:
    // Set if MSB of loaded value is 1.
    cpu.n = cpu.x>>7;
    // Set if value is 0
    cpu.z = cpu.x==0;

    instr->name = "tax";
}

void TAY(Instruction* instr, CPU& cpu){
    cpu.y = cpu.a;
    // FLAGS:
    // Set if MSB of loaded value is 1.
    cpu.n = cpu.y>>7;
    // Set if value is 0
    cpu.z = cpu.y==0;

    instr->name = "tay";
}

void TSX(Instruction* instr, CPU& cpu){
    cpu.x = cpu.stackPointer;
    // FLAGS:
    // Set if MSB of loaded value is 1.
    cpu.n = cpu.x>>7;
    // Set if value is 0
    cpu.z = cpu.x==0;

    instr->name = "tsx";
}

void TXA(Instruction* instr, CPU& cpu){
    cpu.a = cpu.x;
    // FLAGS:
    // Set if MSB of loaded value is 1.
    cpu.n = cpu.a>>7;
    // Set if value is 0
    cpu.z = cpu.a==0;

    instr->name = "txa";
}

void TXS(Instruction* instr, CPU& cpu){
    cpu.stackPointer = cpu.x | 0x0100; // 8th bit in stack always 1

    instr->name = "txs";
}

void TYA(Instruction* instr, CPU& cpu){
    cpu.a = cpu.y;
    // FLAGS:
    // Set if MSB of loaded value is 1.
    cpu.n = cpu.y>>7;
    // Set if value is 0
    cpu.z = cpu.y==0;

    instr->name = "tya";
}

const Instruction Instruction::INSTRUCTIONS[] = {
    Instruction(AND, 0x29, IMMEDIATE, 2, 2),

    Instruction(INC, 0x1A, ACCUMULATOR, 1, 2),
    Instruction(INC, 0xEE, ABSOLUTE, 3, 6),

    Instruction(INX, 0xE8, IMPLIED, 1, 2),

    Instruction(INY, 0xC8, IMPLIED, 1, 2),
    
    Instruction(JMP, 0x4C, IMMEDIATE, 3, 3), //Technically ABSOLUTE, but it works the same in this case.

    Instruction(JSR, 0x20, IMMEDIATE, 3, 6), //Technically ABSOLUTE, but it works the same in this case.
    
    Instruction(LDA, 0xA9, IMMEDIATE, 2, 2),
    
    Instruction(LDX, 0xA2, IMMEDIATE, 2, 2),
    
    Instruction(LDY, 0xA0, IMMEDIATE, 2, 2),
    
    Instruction(ORA, 0x09, IMMEDIATE, 2, 2),
    
    Instruction(ROL, 0x2a, ACCUMULATOR, 1, 2),
    
    Instruction(ROR, 0x6a, ACCUMULATOR, 1, 2),
    
    Instruction(RTS, 0x60, IMPLIED, 1, 6),
    
    Instruction(STA, 0x8D, IMMEDIATE, 3, 4),
    
    Instruction(STX, 0x8E, IMMEDIATE, 3, 4),
    
    Instruction(STY, 0x8C, IMMEDIATE, 3, 4),
    
    Instruction(TAX, 0xAA, IMPLIED, 1, 2),
    
    Instruction(TAY, 0xA8, IMPLIED, 1, 2),
    
    Instruction(TSX, 0xBA, IMPLIED, 1, 2),
    
    Instruction(TXA, 0x8A, IMPLIED, 1, 2),
    
    Instruction(TXS, 0x9A, IMPLIED, 1, 2),
    
    Instruction(TYA, 0x98, IMPLIED, 1, 2),
};

Instruction::Instruction(std::function<void(Instruction*, CPU&)> function, uint8_t opcode, uint8_t addressingMode, uint8_t byteLength, uint8_t numberOfCycles){
    this->function = function;
    this->opcode = opcode;
    this->addressingMode = addressingMode;
    this->byteLength = byteLength;
    this->numberOfCycles = numberOfCycles;
}

uint8_t parseByte(uint32_t romRead){
    return (romRead>>8)&0xFF;
}

uint16_t parseWord(uint32_t romRead){
    return ((romRead&0xFF)<<8) | ((romRead&0xFF00)>>8);
}

void Instruction::fetchInstruction(CPU &cpu){
    std::cout << std::hex << "PC: " << cpu.pc << "\t";

    uint32_t romRead = 0;
    for(uint8_t i = 0; i < 3; i++){
        romRead |= cpu.readROM(cpu.pc+i) << ((2-i)*8);
    }

    //Find the function by opcode
    uint8_t requiredOPCode = romRead>>16;
    Instruction* instr = NULL;
    for(Instruction i : INSTRUCTIONS){
        if(i.opcode == requiredOPCode){
            instr = &i;
            break;
        }
    }
    if(instr == NULL){
        char msg[60];
        std::sprintf(msg, "Instruction %02X not supported!", requiredOPCode);
        throw std::invalid_argument(msg);
    }

    // Fetch the required data
    uint16_t instrArguments = 0;
    // For timing sake, it takes count of how long the instruction being processed will take in the real processor.
    // For ABS_INDEXED and DP_INDEXED: In case of the adding index crossing a page boundary it will be incremented by one. 
    nextInstructionCycleIncrease = instr->numberOfCycles;
    switch (instr->addressingMode){
    case IMMEDIATE:
        if(instr->byteLength == 2) instrArguments = parseByte(romRead);
        else if(instr->byteLength == 3) instrArguments = parseWord(romRead);
        else{
            char msg[60];
            std::sprintf(msg, "Wrong byte length in instruction %02X (opcode: %d)!", instr->opcode, instr->addressingMode);
            throw std::invalid_argument(msg);
        }
        break;

    case ABSOLUTE:
        instrArguments = cpu.readRAM(parseWord(romRead));
        break;

    case ACCUMULATOR:
    case IMPLIED:
        instrArguments = 0;
        break;
    default:
        char msg[60];
        std::sprintf(msg, "Addressing mode %d not supported!", instr->addressingMode);
        throw std::invalid_argument(msg);
        break;
    }

    instr->args = instrArguments;

    // Multiply by 8 as the down line is 8 chars long
    for(uint8_t i = 0; i < (subroutineJumps-1)*8; i++) std::cout << " ";
    if(subroutineJumps>0) std::cout << " \\____ ";

    //Execute the function
    instr->function(instr, cpu);
    
    instr->printDecodedInstruction();

    //Finalize updating the cpu
    cpu.pc += instr->byteLength;
    cpu.cycleCounter += nextInstructionCycleIncrease;
}

template< typename T >
std::string int_to_hex( T i ) {
  char hexString[20];
  std::string format = "$";
  if(sizeof(T) == 1) format += "%02X";
  else if(sizeof(T) == 2) format += "%04X";
  else format += "%X";
  sprintf(hexString, format.c_str(), i);
  return std::string(hexString);
}

void Instruction::printDecodedInstruction(){
    std::string strArgs = "";
    switch (addressingMode){
    case IMMEDIATE:
        strArgs = "#";
        if(byteLength == 2) 
            strArgs += int_to_hex((uint8_t)args);
        else if(byteLength == 3) strArgs += int_to_hex(args);
        break;

    case ABSOLUTE:
        strArgs += int_to_hex(args);
        break;

    case ACCUMULATOR:
    case IMPLIED:
        break;

    default:
        char msg[60];
        std::sprintf(msg, "Addressing mode %d not supported for debug!", addressingMode);
        throw std::invalid_argument(msg);
        break;
    }

    char msg[60];
    std::sprintf(msg, "%s  %s\n", name.c_str(), strArgs.c_str());
    std::cout << msg;
}