#include "instruction.h"

uint8_t Instruction::nextInstructionCycleIncrease = 0;
uint8_t Instruction::subroutineJumps = 0;

void throwAddressingModeError(Instruction* instr){
    char msg[60];
    std::sprintf(msg, "Instruction %s does not support %d addressing mode!", instr->name.c_str(), instr->addressingMode);
    throw std::invalid_argument(msg);
}

void AND(Instruction* instr, CPU* cpu){
    instr->name = "and";

    cpu->a &= instr->args;
    // FLAGS:
    // Set if MSB of result is 1.
    cpu->n = cpu->a>>7;
    // Set if value is 0
    cpu->z = cpu->a==0;
}

// Branch iz z = 0
void BNE(Instruction *instr, CPU* cpu){
    instr->name = "bne";
    
    uint8_t arg = instr->args;
    int8_t jumpDistance;
    memcpy(&jumpDistance, &arg, 1);

    if(!cpu->z){
        cpu->pc += jumpDistance - 2; // Substract the current BNE byte length.
        // If branch is taken, it takes a cycle more.
        instr->nextInstructionCycleIncrease++;
    }
}

void INC(Instruction* instr, CPU* cpu){
    instr->name = "inc";

    uint8_t result = 0;
    switch (instr->addressingMode) {
    case ACCUMULATOR:
        result = ++cpu->a;
        break;

    case ABSOLUTE:
    // TODO: substitute with r_w = false and update children.
        cpu->writeRAM(instr->args, result = cpu->readRAM(instr->args)+1);
        break;

    default:
        throwAddressingModeError(instr);
        break;
    }
    
    // FLAGS:
    // Set if MSB of result is 1.
    cpu->n = result>>7;
    // Set if value is 0
    cpu->z = result==0;
}

void INX(Instruction* instr, CPU* cpu){
    instr->name = "inx";

    cpu->x++;

    // FLAGS:
    // Set if MSB of result is 1.
    cpu->n = cpu->x>>7;
    // Set if value is 0
    cpu->z = cpu->x==0;
}

void INY(Instruction* instr, CPU* cpu){
    instr->name = "iny";

    cpu->y++;

    // FLAGS:
    // Set if MSB of result is 1.
    cpu->n = cpu->y>>7;
    // Set if value is 0
    cpu->z = cpu->y==0;
}

void JMP(Instruction* instr, CPU* cpu){
    instr->name = "jmp";

    // As a cheat, I have to remove the bytes required for the JMP because they will later
    // be added to the PC in the simulation. 
    cpu->pc = instr->args - instr->byteLength;
}

void JSR(Instruction* instr, CPU* cpu){
    instr->name = "jsr";

    // Temporal value to point to the next byte after the instruction argument bytes.
    cpu->pc += instr->byteLength-1;

    // First store the high byte of the address.
    cpu->pushToStack(cpu->pc>>8);
    // Then the low byte.
    cpu->pushToStack(cpu->pc);
    
    // As a cheat, I have to remove the bytes required for the JSR because they will later
    // be added to the PC in the simulation. 
    // In the real system, the address stored in the STACK is the CURRENT PC, later it will
    // be added one more to get the next address.
    cpu->pc = instr->args - instr->byteLength;

    instr->subroutineJumps++;
}

void LDA(Instruction* instr, CPU* cpu){
    instr->name = "lda";

    cpu->a = instr->args;
    // FLAGS:
    // Set if MSB of loaded value is 1.
    cpu->n = instr->args>>7;
    // Set if value is 0
    cpu->z = instr->args==0;
}

void LDX(Instruction* instr, CPU* cpu){
    instr->name = "ldx";

    cpu->x = instr->args;
    // FLAGS:
    // Set if MSB of loaded value is 1.
    cpu->n = instr->args>>7;
    // Set if value is 0
    cpu->z = instr->args==0;
}

void LDY(Instruction* instr, CPU* cpu){
    instr->name = "ldy";

    cpu->y = instr->args;
    // FLAGS:
    // Set if MSB of loaded value is 1.
    cpu->n = instr->args>>7;
    // Set if value is 0
    cpu->z = instr->args==0;
}

void PHA(Instruction* instr, CPU* cpu){
    instr->name = "pha";
    cpu->pushToStack(cpu->a);
}

void PHX(Instruction* instr, CPU* cpu){
    instr->name = "phx";
    cpu->pushToStack(cpu->x);
}

void PHY(Instruction* instr, CPU* cpu){
    instr->name = "phy";
    cpu->pushToStack(cpu->y);
}

void PLA(Instruction* instr, CPU* cpu){
    instr->name = "pla";
    cpu->a = cpu->pullFromStack();

    // FLAGS:
    // Set if MSB of loaded value is 1.
    cpu->n = cpu->a>>7;
    // Set if value is 0
    cpu->z = cpu->a==0;
}

void PLX(Instruction* instr, CPU* cpu){
    instr->name = "plx";
    cpu->x = cpu->pullFromStack();

    // FLAGS:
    // Set if MSB of loaded value is 1.
    cpu->n = cpu->x>>7;
    // Set if value is 0
    cpu->z = cpu->x==0;
}

void PLY(Instruction* instr, CPU* cpu){
    instr->name = "ply";
    cpu->y = cpu->pullFromStack();

    // FLAGS:
    // Set if MSB of loaded value is 1.
    cpu->n = cpu->y>>7;
    // Set if value is 0
    cpu->z = cpu->y==0;
}

void ORA(Instruction* instr, CPU* cpu){
    instr->name = "ora";

    cpu->a |= instr->args;
    // FLAGS:
    // Set if MSB of result is 1.
    cpu->n = cpu->a>>7;
    // Set if value is 0
    cpu->z = cpu->a==0;
}

void ROL(Instruction* instr, CPU* cpu){
    instr->name = "rol";

    cpu->a = (cpu->a << 1) | cpu->c;

    // FLAGS:
    // Set if MSB of result is 1.
    cpu->n = cpu->a>>7;
    // MSB of address becomes the carry.
    cpu->c = instr->args>>7;
    cpu->z = cpu->a == 0;
}

void ROR(Instruction* instr, CPU* cpu){
    instr->name = "ror";

    cpu->a = (cpu->a >> 1) | (cpu->c << 7);

    // FLAGS:
    // Set if MSB of result is 1.
    cpu->n = cpu->a>>7;
    // LSB of address becomes the carry.
    cpu->c = instr->args>>7;
    cpu->z = cpu->a == 0;
}

void RTS(Instruction* instr, CPU* cpu){
    instr->name = "rts";
    uint16_t returnAdd = cpu->pullFromStack() | (cpu->pullFromStack()<<8);
    
    // It should be returnAdd++, but as it also happens with JSR: I have to remove the 
    // byte required for the RTS because it will later be added to the PC in the 
    // simulation. 
    cpu->pc = returnAdd;

    instr->subroutineJumps--;
}

void STA(Instruction* instr, CPU* cpu){
    instr->name = "sta";

    cpu->dataBusWritten = true;
    cpu->r_wb = false;
    cpu->addressBus = instr->args;
    cpu->writeDataBus(cpu->a);
    cpu->updateChildren = true;
}

void STX(Instruction* instr, CPU* cpu){
    instr->name = "stx";

    cpu->dataBusWritten = true;
    cpu->r_wb = false;
    cpu->addressBus = instr->args;
    cpu->writeDataBus(cpu->x);
    cpu->updateChildren = true;
}

void STY(Instruction* instr, CPU* cpu){
    instr->name = "sty";

    cpu->dataBusWritten = true;
    cpu->r_wb = false;
    cpu->addressBus = instr->args;
    cpu->writeDataBus(cpu->y);
    cpu->updateChildren = true;
}

void TAX(Instruction* instr, CPU* cpu){
    cpu->x = cpu->a;
    // FLAGS:
    // Set if MSB of loaded value is 1.
    cpu->n = cpu->x>>7;
    // Set if value is 0
    cpu->z = cpu->x==0;

    instr->name = "tax";
}

void TAY(Instruction* instr, CPU* cpu){
    cpu->y = cpu->a;
    // FLAGS:
    // Set if MSB of loaded value is 1.
    cpu->n = cpu->y>>7;
    // Set if value is 0
    cpu->z = cpu->y==0;

    instr->name = "tay";
}

void TSX(Instruction* instr, CPU* cpu){
    cpu->x = cpu->stackPointer;
    // FLAGS:
    // Set if MSB of loaded value is 1.
    cpu->n = cpu->x>>7;
    // Set if value is 0
    cpu->z = cpu->x==0;

    instr->name = "tsx";
}

void TXA(Instruction* instr, CPU* cpu){
    cpu->a = cpu->x;
    // FLAGS:
    // Set if MSB of loaded value is 1.
    cpu->n = cpu->a>>7;
    // Set if value is 0
    cpu->z = cpu->a==0;

    instr->name = "txa";
}

void TXS(Instruction* instr, CPU* cpu){
    cpu->stackPointer = cpu->x | 0x0100; // 8th bit in stack always 1

    instr->name = "txs";
}

void TYA(Instruction* instr, CPU* cpu){
    cpu->a = cpu->y;
    // FLAGS:
    // Set if MSB of loaded value is 1.
    cpu->n = cpu->y>>7;
    // Set if value is 0
    cpu->z = cpu->y==0;

    instr->name = "tya";
}

const Instruction Instruction::INSTRUCTIONS[] = {
    Instruction(AND, 0x29, IMMEDIATE, 2, 2),

    Instruction(BNE, 0xD0, IMMEDIATE, 2, 2),

    Instruction(INC, 0x1A, ACCUMULATOR, 1, 2),
    Instruction(INC, 0xEE, ABSOLUTE, 3, 6),

    Instruction(INX, 0xE8, IMPLIED, 1, 2),

    Instruction(INY, 0xC8, IMPLIED, 1, 2),
    
    Instruction(JMP, 0x4C, IMMEDIATE, 3, 3), //Technically ABSOLUTE, but it works the same in this case.

    Instruction(JSR, 0x20, IMMEDIATE, 3, 6), //Technically ABSOLUTE, but it works the same in this case.
    
    Instruction(LDA, 0xA9, IMMEDIATE, 2, 2, true),
    Instruction(LDA, 0xAD, ABSOLUTE, 3, 4, true),
    
    Instruction(LDX, 0xA2, IMMEDIATE, 2, 2, true),
    Instruction(LDX, 0xAE, ABSOLUTE, 3, 4, true),
    
    Instruction(LDY, 0xA0, IMMEDIATE, 2, 2, true),
    Instruction(LDY, 0xAC, ABSOLUTE, 3, 4, true),
    
    Instruction(ORA, 0x09, IMMEDIATE, 2, 2),

    Instruction(PHA, 0x48, IMPLIED, 1, 3),
    Instruction(PHX, 0xDA, IMPLIED, 1, 3),
    Instruction(PHY, 0x5A, IMPLIED, 1, 3),

    Instruction(PLA, 0x68, IMPLIED, 1, 4),
    Instruction(PLX, 0xFA, IMPLIED, 1, 4),
    Instruction(PLY, 0x7A, IMPLIED, 1, 4),
    
    Instruction(ROL, 0x2a, ACCUMULATOR, 1, 2),
    
    Instruction(ROR, 0x6a, ACCUMULATOR, 1, 2),
    
    Instruction(RTS, 0x60, IMPLIED, 1, 6),
    
    Instruction(STA, 0x8D, ABSOLUTE, 3, 4),
    
    Instruction(STX, 0x8E, ABSOLUTE, 3, 4),
    
    Instruction(STY, 0x8C, ABSOLUTE, 3, 4),
    
    Instruction(TAX, 0xAA, IMPLIED, 1, 2),
    
    Instruction(TAY, 0xA8, IMPLIED, 1, 2),
    
    Instruction(TSX, 0xBA, IMPLIED, 1, 2),
    
    Instruction(TXA, 0x8A, IMPLIED, 1, 2),
    
    Instruction(TXS, 0x9A, IMPLIED, 1, 2),
    
    Instruction(TYA, 0x98, IMPLIED, 1, 2),
};

Instruction::Instruction(std::function<void(Instruction*, CPU*)> function, uint8_t opcode, uint8_t addressingMode, uint8_t byteLength, uint8_t numberOfCycles, bool needsInput){
    this->function = function;
    this->opcode = opcode;
    this->addressingMode = addressingMode;
    this->byteLength = byteLength;
    this->numberOfCycles = numberOfCycles;
    this->needsInput = needsInput;
}

uint8_t parseByte(uint32_t romRead){
    return (romRead>>8)&0xFF;
}

uint16_t parseWord(uint32_t romRead){
    return ((romRead&0xFF)<<8) | ((romRead&0xFF00)>>8);
}

void Instruction::fetchInstruction(CPU *cpu){
    if(!cpu->expectsData){
        cout << endl;
        cout << std::hex << "PC: " << cpu->pc << "\t";
        cpu->r_wb = true;
        cpu->addressBus = cpu->pc;
    } 

    uint32_t romRead = 0;
    for(uint8_t i = 0; i < 3; i++){
        romRead |= cpu->readROM(cpu->pc+i) << ((2-i)*8);
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
    if(instr == NULL) throwException("Instruction %02X not supported!", requiredOPCode);

    uint8_t addrMode = instr->addressingMode;
    if(!cpu->expectsData && instr->needsInput && addrMode!=IMMEDIATE){
        cpu->expectsData = true;
        uint16_t args1 = parseWord(romRead);
        switch (addrMode) {
            case ABSOLUTE:{
                cpu->addressBus = args1;
                break;
            }
            
            default:
                break;
        }
        return;
    }

    // Fetch the required data
    uint16_t instrArguments = 0;
    // For timing sake, it takes count of how long the instruction being processed will take in the real processor.
    // For ABS_INDEXED and DP_INDEXED: In case of the adding index crossing a page boundary it will be incremented by one. 
    nextInstructionCycleIncrease = instr->numberOfCycles;
    switch (addrMode){
    case IMMEDIATE:{
        if(instr->byteLength == 2) instrArguments = parseByte(romRead);
        else if(instr->byteLength == 3) instrArguments = parseWord(romRead);
        else{
            char msg[60];
            std::sprintf(msg, "Wrong byte length in instruction %02X (opcode: %d)!", instr->opcode, instr->addressingMode);
            throw std::invalid_argument(msg);
        }
        break;
    }

    case ACCUMULATOR:
    case IMPLIED:{
        break;
    }

    case ABSOLUTE:{
        if(instr->needsInput){
            instrArguments = cpu->getDataBus();
        }else{
            instrArguments = parseWord(romRead);
        }
        break;
    }

    default:
        throwException("Addressing mode %d not supported!", instr->addressingMode);
    }

    instr->args = instrArguments;

    // Multiply by 8 as the down line is 8 chars long
    for(uint8_t i = 0; i < (subroutineJumps-1)*8; i++) std::cout << " ";
    if(subroutineJumps>0) std::cout << " \\____ ";

    //Execute the function
    instr->function(instr, cpu);
    
    instr->printDecodedInstruction(cpu);

    //Finalize updating the cpu
    cpu->pc += instr->byteLength;
    cpu->cycleCounter += nextInstructionCycleIncrease;
    cpu->expectsData = false;
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

void Instruction::printDecodedInstruction(CPU* cpu){
    std::string strArgs = "";
    switch (addressingMode){
    case IMMEDIATE:
        strArgs = "#";
        if(byteLength == 2){
            strArgs += int_to_hex((uint8_t)args);
            strArgs += "  ";
        }else if(byteLength == 3){
            strArgs += int_to_hex(args);
        }
        break;

    case ABSOLUTE:
        strArgs += int_to_hex(args);
        break;

    case ACCUMULATOR:
        strArgs += "A";
    case IMPLIED:
        strArgs += "\t";
        break;

    default:
        throwException("Addressing mode %d not supported for debug!", addressingMode);
        break;
    }

    cout << name << " " << strArgs;

    // This is a quick fix. As the subroutineJumps variable is increased exactly during the JSR call
    // and decreased in the RTS call, to maintain all flags aligned on this two calls, this is done.
    static uint8_t tempSubroutineJumpCount = 0;
    // If the subroutineJumps has changed from the previous call to this method, it has to be one of 
    // the later calls.
    uint8_t tabCount = MAX_SUBROUTINE_JUMPS - subroutineJumps + (tempSubroutineJumpCount<subroutineJumps) - (tempSubroutineJumpCount>subroutineJumps);
    for(uint8_t i = 0; i < tabCount; i++) cout << "\t";
    if(tempSubroutineJumpCount!=subroutineJumps) tempSubroutineJumpCount = subroutineJumps;

    char msg[60];
    std::sprintf(msg, "n=%d v=%d b=%d d=%d i=%d z=%d c=%d  a=%02X x=%02X y=%02X cycle=%d", 
        cpu->n, cpu->v, cpu->b, cpu->d, cpu->i, cpu->z, cpu->c, 
        cpu->a, cpu->x, cpu->y,
        cpu->cycleCounter);
    std::cout << msg;
}

void CPU::process(){
    Instruction::fetchInstruction(this);
}