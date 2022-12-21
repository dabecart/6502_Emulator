#include "instruction.h"

uint8_t Instruction::nextInstructionCycleIncrease = 0;
uint8_t Instruction::subroutineJumps = 0;

void throwAddressingModeError(Instruction* instr){
    char msg[60];
    std::sprintf(msg, "Instruction %s does not support %d addressing mode!", instr->name.c_str(), instr->addressingMode);
    throw std::invalid_argument(msg);
}

void ADC(Instruction* instr, CPU* cpu){
    instr->name = "adc";

    uint8_t args = 0;
    if(instr->addressingMode == IMMEDIATE){
        args = instr->args;
    }else{
        args = cpu->getDataBus();
    }

    uint8_t prevA = cpu->a;
    cpu->a += args + cpu->c;

    // FLAGS:
    // Negative: Set if MSB of result is 1.
    cpu->n = cpu->a>>7;
    // Overflow: Set if signed overflow -> positive + positive = negative or negative + negative = positive
    bool a_neg = prevA>>7;
    bool arg_neg = args>>7;
    bool sum_neg = cpu->a>>7;
    cpu->v = (a_neg && arg_neg && !sum_neg) || (!a_neg && !arg_neg && sum_neg);
    // Zero: Set if value is 0
    cpu->z = cpu->a==0;
    // Carry: Set if unsigned overflow
    cpu->c = prevA>=cpu->a;
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

void ASL(Instruction* instr, CPU* cpu){
    instr->name = "asl";

    uint8_t result = 0;
    if(instr->addressingMode == ACCUMULATOR){
        cpu->c = cpu->a >> 7;
        cpu->a = cpu->a << 1;
        result = cpu->a;
    }else{
        result = cpu->getDataBus();
        cpu->c = result >> 7;
        result = result << 1;

        cpu->r_wb = false;
        cpu->addressBus = instr->args;
        cpu->writeDataBus(result);
        cpu->updateChildren = true;
    }
    
    // FLAGS:
    // Negative: if top bit is 1.
    cpu->n = result>>7;
    // Set if value is 0
    cpu->z = result==0;
}

// Branch if n = 1 (negative number)
void BMI(Instruction *instr, CPU* cpu){
    instr->name = "bmi";
    
    uint8_t arg = instr->args;
    int8_t jumpDistance;
    memcpy(&jumpDistance, &arg, 1);

    if(cpu->n){
        uint16_t temp = cpu->pc;
        cpu->pc += jumpDistance;
        // If branch is taken, it takes a cycle more.
        instr->nextInstructionCycleIncrease++;
        // Add one cycle more if branch taken crosses boundary.
        if(temp>>8 != (cpu->pc+2)>>8) instr->nextInstructionCycleIncrease++;
    }
}

// Branch iz c = 0
void BCC(Instruction *instr, CPU* cpu){
    instr->name = "bcc";
    
    uint8_t arg = instr->args;
    int8_t jumpDistance;
    memcpy(&jumpDistance, &arg, 1);

    if(!cpu->c){
        uint16_t temp = cpu->pc;
        cpu->pc += jumpDistance;
        // If branch is taken, it takes a cycle more.
        instr->nextInstructionCycleIncrease++;
        // Add one cycle more if branch taken crosses boundary.
        if(temp>>8 != (cpu->pc+2)>>8) instr->nextInstructionCycleIncrease++;
    }
}

// Branch iz c = 1
void BCS(Instruction *instr, CPU* cpu){
    instr->name = "bcs";
    
    uint8_t arg = instr->args;
    int8_t jumpDistance;
    memcpy(&jumpDistance, &arg, 1);

    if(cpu->c){
        uint16_t temp = cpu->pc;
        cpu->pc += jumpDistance;
        // If branch is taken, it takes a cycle more.
        instr->nextInstructionCycleIncrease++;
        // Add one cycle more if branch taken crosses boundary.
        if(temp>>8 != (cpu->pc+2)>>8) instr->nextInstructionCycleIncrease++;
    }
}

// Branch iz z = 1
void BEQ(Instruction *instr, CPU* cpu){
    instr->name = "beq";
    
    uint8_t arg = instr->args;
    int8_t jumpDistance;
    memcpy(&jumpDistance, &arg, 1);

    if(cpu->z){
        uint16_t temp = cpu->pc;
        cpu->pc += jumpDistance;
        // If branch is taken, it takes a cycle more.
        instr->nextInstructionCycleIncrease++;
        // Add one cycle more if branch taken crosses boundary.
        if(temp>>8 != (cpu->pc+2)>>8) instr->nextInstructionCycleIncrease++;
    }
    
}

// Branch if n = 0 (positive number)
void BPL(Instruction *instr, CPU* cpu){
    instr->name = "bpl";
    
    uint8_t arg = instr->args;
    int8_t jumpDistance;
    memcpy(&jumpDistance, &arg, 1);

    if(!cpu->n){
        uint16_t temp = cpu->pc;
        cpu->pc += jumpDistance;
        // If branch is taken, it takes a cycle more.
        instr->nextInstructionCycleIncrease++;
        // Add one cycle more if branch taken crosses boundary.
        if(temp>>8 != (cpu->pc+2)>>8) instr->nextInstructionCycleIncrease++;
    }
}

// Branch iz z = 0
void BNE(Instruction *instr, CPU* cpu){
    instr->name = "bne";
    
    uint8_t arg = instr->args;
    int8_t jumpDistance;
    memcpy(&jumpDistance, &arg, 1);

    if(!cpu->z){
        cpu->pc += jumpDistance;
        // If branch is taken, it takes a cycle more.
        instr->nextInstructionCycleIncrease++;
    }
}

void CLC(Instruction* instr, CPU* cpu){
    instr->name = "clc";
    cpu->c = false;
}

void CLI(Instruction* instr, CPU* cpu){
    instr->name = "cli";
    cpu->i = false;
}

void CMP(Instruction* instr, CPU* cpu){
    instr->name = "cmp";

    uint8_t operand;
    if(instr->addressingMode == IMMEDIATE){
        operand = instr->args;
    }else{
        operand = cpu->getDataBus();
    }
    uint8_t result = cpu->a - operand;

    // FLAGS:
    cpu->n = result>>7;
    cpu->z = result==0;
    // Carry set if no borrow required, aka. accumulator bigger or same as operand.
    cpu->c = cpu->a >= operand;
}

void DEC(Instruction* instr, CPU* cpu){
    instr->name = "dec";

    uint8_t result = 0;
    if(instr->addressingMode == ACCUMULATOR){
        result = --cpu->a;
    }else{
        result = cpu->getDataBus() - 1;
        cpu->r_wb = false;
        cpu->addressBus = instr->args;
        cpu->writeDataBus(result);
        cpu->updateChildren = true;
    }
    
    // FLAGS:
    // Set if MSB of result is 1.
    cpu->n = result>>7;
    // Set if value is 0
    cpu->z = result==0;
}

void DEX(Instruction* instr, CPU* cpu){
    instr->name = "dex";

    cpu->x--;

    // FLAGS:
    // Set if MSB of result is 1.
    cpu->n = cpu->x>>7;
    // Set if value is 0
    cpu->z = cpu->x==0;
}

void DEY(Instruction* instr, CPU* cpu){
    instr->name = "dey";

    cpu->y--;

    // FLAGS:
    // Set if MSB of result is 1.
    cpu->n = cpu->y>>7;
    // Set if value is 0
    cpu->z = cpu->y==0;
}

void EOR(Instruction* instr, CPU* cpu){
    instr->name = "eor";

    cpu->a ^= instr->args;
    // FLAGS:
    // Set if MSB of result is 1.
    cpu->n = cpu->a>>7;
    // Set if value is 0
    cpu->z = cpu->a==0;
}

void INC(Instruction* instr, CPU* cpu){
    instr->name = "inc";

    uint8_t result = 0;
    if(instr->addressingMode == ACCUMULATOR){
        result = ++cpu->a;
    }else{
        result = cpu->getDataBus() + 1;
        cpu->r_wb = false;
        cpu->addressBus = instr->args;
        cpu->writeDataBus(result);
        cpu->updateChildren = true;
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

    if(instr->addressingMode == IMMEDIATE){
        cpu->a = instr->args;
    }else{
        cpu->a = cpu->getDataBus();
    }
    
    // FLAGS:
    // Set if MSB of loaded value is 1.
    cpu->n = cpu->a>>7;
    // Set if value is 0
    cpu->z = cpu->a==0;
}

void LDX(Instruction* instr, CPU* cpu){
    instr->name = "ldx";

    if(instr->addressingMode == IMMEDIATE){
        cpu->x = instr->args;
    }else{
        cpu->x = cpu->getDataBus();
    }
    // FLAGS:
    // Set if MSB of loaded value is 1.
    cpu->n = cpu->x>>7;
    // Set if value is 0
    cpu->z = cpu->x==0;
}

void LDY(Instruction* instr, CPU* cpu){
    instr->name = "ldy";

    if(instr->addressingMode == IMMEDIATE){
        cpu->y = instr->args;
    }else{
        cpu->y = cpu->getDataBus();
    }
    // FLAGS:
    // Set if MSB of loaded value is 1.
    cpu->n = cpu->y>>7;
    // Set if value is 0
    cpu->z = cpu->y==0;
}

void LSR(Instruction* instr, CPU* cpu){
    instr->name = "lsr";

    uint8_t result = 0;
    if(instr->addressingMode == ACCUMULATOR){
        cpu->c = cpu->a & 0x01;
        cpu->a = cpu->a>>1;
        result = cpu->a;
    }else{
        result = cpu->getDataBus();
        cpu->c = result & 0x01;
        result = result>>1;

        cpu->r_wb = false;
        cpu->addressBus = instr->args;
        cpu->writeDataBus(result);
        cpu->updateChildren = true;
    }
    
    // FLAGS:
    // Negative: always 0.
    cpu->n = 0;
    // Set if value is 0
    cpu->z = result==0;
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

    uint8_t result = 0;
    if(instr->addressingMode == ACCUMULATOR){
        result = cpu->a;
        cpu->a = (cpu->a << 1) | cpu->c;
        cpu->c = result >> 7;
        result = cpu->a;
    }else{
        uint8_t data = cpu->getDataBus();
        result = (data << 1) | cpu->c;
        cpu->c = data >> 7;

        cpu->r_wb = false;
        cpu->addressBus = instr->args;
        cpu->writeDataBus(result);
        cpu->updateChildren = true;
    }
    
    // FLAGS:
    // Negative: if top bit is 1.
    cpu->n = result>>7;
    // Set if value is 0
    cpu->z = result==0;
}

void ROR(Instruction* instr, CPU* cpu){
    instr->name = "ror";

    uint8_t result = 0;
    if(instr->addressingMode == ACCUMULATOR){
        result = cpu->a;
        cpu->a = (cpu->a >> 1) | (cpu->c << 7);
        cpu->c = result & 1;
        result = cpu->a;
    }else{
        uint8_t data = cpu->getDataBus();
        result = (data >> 1) | (cpu->c << 7);
        cpu->c = data & 1;

        cpu->r_wb = false;
        cpu->addressBus = instr->args;
        cpu->writeDataBus(result);
        cpu->updateChildren = true;
    }
    
    // FLAGS:
    // Negative: if top bit is 1.
    cpu->n = result>>7;
    // Set if value is 0
    cpu->z = result==0;
}

void RTI(Instruction* instr, CPU* cpu){
    instr->name = "rti";

    cpu->setStatusRegister(cpu->pullFromStack());
    cpu->pc = cpu->pullFromStack() | (cpu->pullFromStack()<<8);
    cpu->pc--;  // Substract the byte that will later be added at the end of the fetchInstruction() function
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

/* // Old complicated method using signed numbers, there's no need to do that, it's better to use the bit trickery IMO
void SBC(Instruction* instr, CPU* cpu){
    instr->name = "sbc";

    uint8_t args = 0;
    if(instr->addressingMode == IMMEDIATE){
        args = instr->args;
    }else{
        args = cpu->getDataBus();
    }

    uint8_t prev_a = cpu->a;
    int8_t a_signed = 0, arg_signed = 0;
    memcpy(&a_signed, &cpu->a, 1);
    memcpy(&arg_signed, &args, 1);
    int8_t sum = a_signed - arg_signed - !cpu->c;

    memcpy(&cpu->a, &sum, 1);
    // FLAGS:
    // Negative: Set if MSB of result is 1.
    cpu->n = cpu->a>>7;
    // Overflow: Set if signed overflow -> positive + positive = negative or negative + negative = positive
    bool a_pos = a_signed>0;
    bool arg_pos = arg_signed>0;
    bool sum_pos = sum>0;
    cpu->v = (a_pos && arg_pos && !sum_pos) || (!a_pos && !arg_pos && sum_pos);
    // Zero: Set if value is 0
    cpu->z = cpu->a==0;
    // Carry: Set if unsigned borrow not required
    cpu->c = prev_a>=args;
}
*/

void SBC(Instruction* instr, CPU* cpu){
    instr->name = "sbc";

    uint8_t args = 0;
    if(instr->addressingMode == IMMEDIATE){
        args = instr->args;
    }else{
        args = cpu->getDataBus();
    }

    uint8_t prevA = cpu->a;
    // A bit of bit magic: so, subtracting in 2s-complement is the same as adding the 2s-complement
    // of the number, which literally is the negation of the number plus one. The 6502 instead of adding
    // that one by itself requires the programmer to set the carry first using SEC. When using multiple 
    // byte operations involving this instruction, the carry bit is used to signal that the next subtraction
    // needs a carry (if LB1 < LB2, it has to be subtracted one from the HB2, that is the same as NOT adding
    // that bit in the 2s-complement). So, for example: 
    // $0108 - $0008 = $0108 + $FFF7 + 1    -> LSB: 08 + F7 + 1 = 1 00  (carry is 1)    -> Result = $0100   (carry 1, meaning result is positive!)
    //                                      -> MSB: 01 + FF + 1 = 1 01  (carry is 1)------------------------^
    // Meanwhile:
    // $0A55 - $1111 = $0A55 + $EEEE + 1    -> LSB: 55 + EE + 1 = 1 44  (carry is 1)    -> Result = $F944   (carry 0, meaning result is negative!)
    //                                      -> MSB: 0A + EE + 1 = 0 F9  (carry is 0)------------------------^
    cpu->a += ~args + cpu->c;

    // FLAGS:
    // Negative: Set if MSB of result is 1.
    cpu->n = cpu->a>>7;
    // Overflow: Set if signed overflow -> positive + positive = negative or negative + negative = positive
    bool a_pos = !(prevA>>7);
    bool arg_pos = args>>7; // As the result is negated, no need to !
    bool sum_pos = !(cpu->a>>7);
    cpu->v = (a_pos && arg_pos && !sum_pos) || (!a_pos && !arg_pos && sum_pos);
    // Zero: Set if value is 0
    cpu->z = cpu->a==0;
    // Carry: Set if unsigned borrow not required
    cpu->c = prevA >= cpu->a;
}

void SEC(Instruction* instr, CPU* cpu){
    instr->name = "sec";
    cpu->c = true;
}

void SEI(Instruction* instr, CPU* cpu){
    instr->name = "sei";
    cpu->i = true;
}

void STA(Instruction* instr, CPU* cpu){
    instr->name = "sta";

    cpu->r_wb = false;
    cpu->addressBus = instr->args;
    cpu->writeDataBus(cpu->a);
    cpu->updateChildren = true;
}

void STX(Instruction* instr, CPU* cpu){
    instr->name = "stx";

    cpu->r_wb = false;
    cpu->addressBus = instr->args;
    cpu->writeDataBus(cpu->x);
    cpu->updateChildren = true;
}

void STY(Instruction* instr, CPU* cpu){
    instr->name = "sty";

    cpu->r_wb = false;
    cpu->addressBus = instr->args;
    cpu->writeDataBus(cpu->y);
    cpu->updateChildren = true;
}

void STZ(Instruction* instr, CPU* cpu){
    instr->name = "stz";

    cpu->r_wb = false;
    cpu->addressBus = instr->args;
    cpu->writeDataBus(0);
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

vector<Instruction> Instruction::INSTRUCTIONS = {
    Instruction(ADC, 0x69, IMMEDIATE, 2, 2),
    Instruction(ADC, 0x6D, ABSOLUTE, 3, 4, true),
    Instruction(ADC, 0x65, DP, 2, 3, true),
    Instruction(ADC, 0x75, DP_INDEXED_X, 2, 4, true),

    Instruction(AND, 0x29, IMMEDIATE, 2, 2),
    Instruction(AND, 0x2D, ABSOLUTE, 3, 4),
    Instruction(AND, 0x25, DP, 4, 5),
    Instruction(AND, 0x3D, ABS_INDEXED_X, 3, 4),
    Instruction(AND, 0x39, ABS_INDEXED_Y, 3, 4),
    Instruction(AND, 0x35, DP_INDEXED_X, 2, 4),

    Instruction(ASL, 0x0A, ACCUMULATOR, 1, 2),
    Instruction(ASL, 0x0E, ABSOLUTE, 3, 6, true),
    Instruction(ASL, 0x06, DP, 2, 5, true),
    Instruction(ASL, 0x1E, ABS_INDEXED_X, 3, 6 /*If page is crossed it'll add 1*/, true),
    Instruction(ASL, 0x16, DP_INDEXED_X, 2, 6, true),

    Instruction(BMI, 0x30, IMMEDIATE, 2, 2),

    Instruction(BCC, 0x90, IMMEDIATE, 2, 2),

    Instruction(BCS, 0xB0, IMMEDIATE, 2, 2),

    Instruction(BEQ, 0xF0, IMMEDIATE, 2, 2),

    Instruction(BPL, 0x10, IMMEDIATE, 2, 2),

    Instruction(BNE, 0xD0, IMMEDIATE, 2, 2),

    Instruction(CLC, 0x18, IMPLIED, 1, 2),
    Instruction(CLI, 0x58, IMPLIED, 1, 2),

    Instruction(CMP, 0xC9, IMMEDIATE, 2, 2),
    Instruction(CMP, 0xCD, ABSOLUTE, 3, 4, true),
    Instruction(CMP, 0xC5, DP, 2, 3, true),

    Instruction(DEC, 0x3A, ACCUMULATOR, 1, 2),
    Instruction(DEC, 0xCE, ABSOLUTE, 3, 6, true),
    Instruction(DEC, 0xC6, DP, 2, 5, true),
    Instruction(DEC, 0xDE, ABS_INDEXED_X, 2, 6, true),
    Instruction(DEC, 0xD6, DP_INDEXED_X, 2, 6, true),

    Instruction(DEX, 0xCA, IMPLIED, 1, 2),

    Instruction(DEY, 0x88, IMPLIED, 1, 2),

    Instruction(EOR, 0x49, IMMEDIATE, 2, 2),
    Instruction(EOR, 0x4D, ABSOLUTE, 3, 4),
    Instruction(EOR, 0x45, DP, 2, 3),

    Instruction(INC, 0x1A, ACCUMULATOR, 1, 2),
    Instruction(INC, 0xEE, ABSOLUTE, 3, 6, true),
    Instruction(INC, 0xE6, DP, 2, 5, true),
    Instruction(INC, 0xFE, ABS_INDEXED_X, 2, 6, true),
    Instruction(INC, 0xF6, DP_INDEXED_X, 2, 6, true),

    Instruction(INX, 0xE8, IMPLIED, 1, 2),

    Instruction(INY, 0xC8, IMPLIED, 1, 2),
    
    Instruction(JMP, 0x4C, IMMEDIATE, 3, 3), //Technically ABSOLUTE, but it works the same in this case.

    Instruction(JSR, 0x20, IMMEDIATE, 3, 6), //Technically ABSOLUTE, but it works the same in this case.
    
    Instruction(LDA, 0xA9, IMMEDIATE, 2, 2),
    Instruction(LDA, 0xAD, ABSOLUTE, 3, 4, true),
    Instruction(LDA, 0xA5, DP, 2, 3, true),
    Instruction(LDA, 0xB5, DP_INDEXED_X, 2, 4, true),
    Instruction(LDA, 0xBD, ABS_INDEXED_X, 3, 4, true),
    Instruction(LDA, 0xB9, ABS_INDEXED_Y, 3, 4, true),
    
    Instruction(LDX, 0xA2, IMMEDIATE, 2, 2),
    Instruction(LDX, 0xAE, ABSOLUTE, 3, 4, true),
    Instruction(LDX, 0xA6, DP, 2, 3, true),
    Instruction(LDX, 0xB6, DP_INDEXED_Y, 2, 4, true),
    Instruction(LDX, 0xBE, ABS_INDEXED_Y, 3, 4, true),
    
    Instruction(LDY, 0xA0, IMMEDIATE, 2, 2),
    Instruction(LDY, 0xAC, ABSOLUTE, 3, 4, true),
    Instruction(LDY, 0xA4, DP, 2, 3, true),
    Instruction(LDY, 0xB4, DP_INDEXED_X, 2, 4, true),
    Instruction(LDY, 0xBC, ABS_INDEXED_X, 3, 4, true),

    Instruction(LSR, 0x4A, ACCUMULATOR, 1, 2),
    Instruction(LSR, 0x4E, ABSOLUTE, 3, 6, true),
    Instruction(LSR, 0x46, DP, 2, 5, true),
    Instruction(LSR, 0x5E, ABS_INDEXED_X, 3, 6 /*It'll add 1 if page boundary is crossed*/, true),
    Instruction(LSR, 0x56, DP_INDEXED_X, 2, 6, true),
    
    Instruction(ORA, 0x09, IMMEDIATE, 2, 2),
    Instruction(ORA, 0x0D, ABSOLUTE, 3, 4),
    Instruction(ORA, 0x05, DP, 4, 5),
    Instruction(ORA, 0x1D, ABS_INDEXED_X, 3, 4),
    Instruction(ORA, 0x19, ABS_INDEXED_Y, 3, 4),
    Instruction(ORA, 0x15, DP_INDEXED_X, 2, 4),

    Instruction(PHA, 0x48, IMPLIED, 1, 3),
    Instruction(PHX, 0xDA, IMPLIED, 1, 3),
    Instruction(PHY, 0x5A, IMPLIED, 1, 3),

    Instruction(PLA, 0x68, IMPLIED, 1, 4),
    Instruction(PLX, 0xFA, IMPLIED, 1, 4),
    Instruction(PLY, 0x7A, IMPLIED, 1, 4),
    
    Instruction(ROL, 0x2A, ACCUMULATOR, 1, 2),
    Instruction(ROL, 0x2E, ABSOLUTE, 3, 6, true),
    Instruction(ROL, 0x26, DP, 2, 5, true),
    Instruction(ROL, 0x3E, ABS_INDEXED_X, 3, 6 /*If page boundary cross, it'll add 1*/, true),
    Instruction(ROL, 0x36, DP_INDEXED_X, 2, 6, true),
    
    Instruction(ROR, 0x6A, ACCUMULATOR, 1, 2, true),
    Instruction(ROR, 0x6E, ABSOLUTE, 3, 6, true),
    Instruction(ROR, 0x66, DP, 2, 5, true),
    Instruction(ROR, 0x7E, ABS_INDEXED_X, 3, 6 /*If page boundary cross, it'll add 1*/, true),
    Instruction(ROR, 0x76, DP_INDEXED_X, 2, 6, true),

    Instruction(RTI, 0x40, IMPLIED, 1, 6),

    Instruction(RTS, 0x60, IMPLIED, 1, 6),

    Instruction(SBC, 0xE9, IMMEDIATE, 2, 2),
    Instruction(SBC, 0xED, ABSOLUTE, 3, 4, true),
    Instruction(SBC, 0xE5, DP, 2, 3, true),
    Instruction(SBC, 0xF5, DP_INDEXED_X, 2, 4, true),

    Instruction(SEC, 0x38, IMPLIED, 1, 2),

    Instruction(SEI, 0x78, IMPLIED, 1, 2),
    
    Instruction(STA, 0x8D, ABSOLUTE, 3, 4),
    Instruction(STA, 0x85, DP, 2, 3),
    Instruction(STA, 0x9D, ABS_INDEXED_X, 3, 5),
    Instruction(STA, 0x99, ABS_INDEXED_Y, 3, 5),
    Instruction(STA, 0x95, DP_INDEXED_X, 2, 4),

    Instruction(STX, 0x8E, ABSOLUTE, 3, 4),
    Instruction(STX, 0x86, DP, 2, 3),
    Instruction(STX, 0x96, DP_INDEXED_Y, 2, 4),
    
    Instruction(STY, 0x8C, ABSOLUTE, 3, 4),
    Instruction(STY, 0x84, DP, 2, 3),
    Instruction(STY, 0x94, DP_INDEXED_Y, 2, 4),

    Instruction(STZ, 0x9C, ABSOLUTE, 3, 4),
    Instruction(STZ, 0x64, DP, 2, 3),
    Instruction(STZ, 0x9E, ABS_INDEXED_X, 3, 5),
    Instruction(STZ, 0x74, DP_INDEXED_X, 2, 4),
    
    Instruction(TAX, 0xAA, IMPLIED, 1, 2),
    
    Instruction(TAY, 0xA8, IMPLIED, 1, 2),
    
    Instruction(TSX, 0xBA, IMPLIED, 1, 2),
    
    Instruction(TXA, 0x8A, IMPLIED, 1, 2),
    
    Instruction(TXS, 0x9A, IMPLIED, 1, 2),
    
    Instruction(TYA, 0x98, IMPLIED, 1, 2),
};

void Instruction::sortInstructions(){
    sort(INSTRUCTIONS.begin(), INSTRUCTIONS.end(), [](const Instruction &in1, const Instruction &in2){
        return in1.opcode < in2.opcode;
    });
}

uint8_t Instruction::findInstructionIndex(uint8_t opcode){
    uint8_t first = 0;
    uint8_t last = INSTRUCTIONS.size() - 1;
    while(first <= last){
        uint8_t center = (first+last)/2;
        uint8_t currentOPCode = INSTRUCTIONS[center].opcode;
        if(currentOPCode == opcode) return center;
        
        if(currentOPCode > opcode) last = center - 1;
        else first = center + 1;
    }
    throwException("Instruction %02X not supported!", opcode);
    return -1;
}

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
    // The CPU only listens when the last instruction has been fully processed.
    cpu->processIRQ();

    // Begining of the CPU cycle
    if(!cpu->expectsData){
        cout << endl;
        cout << "PC: " << int_to_hex(cpu->pc) << "\t";
        cpu->r_wb = true;
        cpu->addressBus = cpu->pc;
        nextInstructionCycleIncrease = 0;
    }

    // Loads the next three bytes from the instruction byte (included) in the same order
    // it appears inside the ROM:  CodeOp + 1stByte + 2nd Byte
    uint32_t romRead = 0;
    for(uint8_t i = 0; i < 3; i++){
        romRead |= cpu->readROM(cpu->pc+i) << ((2-i)*8);
    }

    //Find the function by opcode
    uint8_t requiredOPCode = romRead>>16;
    Instruction* instr = &INSTRUCTIONS[findInstructionIndex(requiredOPCode)];

    uint8_t addrMode = instr->addressingMode;
    bool needsSubsequentProcess = !cpu->expectsData && instr->needsInput && addrMode!=IMMEDIATE;
    if(needsSubsequentProcess){
        cpu->expectsData = true;
        uint16_t args1 = parseWord(romRead);
        switch (addrMode) {
            case ABSOLUTE:{
                cpu->addressBus = args1;
                break;
            }

            case DP:{
                // Load the byte following the CodeOp.
                cpu->addressBus = parseByte(romRead);
                break;
            }

            case DP_INDEXED_X:{
                // Load the byte following the CodeOp.
                cpu->addressBus = parseByte(romRead) + cpu->x;
                break;
            }

            case DP_INDEXED_Y:{
                // Load the byte following the CodeOp.
                cpu->addressBus = parseByte(romRead) + cpu->y;
                break;
            }

            case DP_INDIRECT:{
                uint8_t dp = parseByte(romRead);
                uint16_t direction = cpu->readRAM(dp) | (cpu->readRAM(dp+1) << 8);
                cpu->addressBus = direction;
                break;
            }

            case ABS_INDEXED_X:{
                uint16_t temp = cpu->addressBus;
                cpu->addressBus = args1 + cpu->x;
                // Check if adding index crosses a page boundary
                if(temp>>8 != cpu->addressBus>>8) nextInstructionCycleIncrease--;
                break;
            }

            case ABS_INDEXED_Y:{
                uint16_t temp = cpu->addressBus;
                cpu->addressBus = args1 + cpu->y;
                // Check if adding index crosses a page boundary
                if(temp>>8 != cpu->addressBus>>8) nextInstructionCycleIncrease++;
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
    nextInstructionCycleIncrease += instr->numberOfCycles;
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
        instrArguments = parseWord(romRead); // i.e. STA
        break;
    }

    case DP:{
        instrArguments = parseByte(romRead);
        break;
    }

    case DP_INDEXED_X:{
        instrArguments = parseByte(romRead) + cpu->x;
        break;
    }

    case DP_INDEXED_Y:{
        instrArguments = parseByte(romRead) + cpu->y;
        break;
    }

    case ABS_INDEXED_X:{
        instrArguments = parseWord(romRead) + cpu->x;
        break;
    }

    case ABS_INDEXED_Y:{
        instrArguments = parseWord(romRead) + cpu->y;
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
    
    instr->args = instrArguments;   // To format correctly the debug log
    instr->printDecodedInstruction(cpu);

    //Finalize updating the cpu
    cpu->pc += instr->byteLength;
    cpu->cycleCounter += nextInstructionCycleIncrease;
}

void Instruction::printDecodedInstruction(CPU* cpu){
    std::string strArgs = "";
    switch (addressingMode){
    case IMMEDIATE:{
        strArgs = "#";
        if(byteLength == 2){
            strArgs += int_to_hex((uint8_t)args);
            strArgs += "  ";
        }else if(byteLength == 3){
            strArgs += int_to_hex(args);
        }
        break;
    }

    case ABSOLUTE:{
        strArgs += int_to_hex(args);
        break;
    }
    
    case DP:{
        uint8_t byteParam = args;
        strArgs += int_to_hex(byteParam);
        strArgs += "  ";
        break;
    }

    case DP_INDEXED_X:{
        uint8_t byteParam = args-cpu->x;
        strArgs += int_to_hex(byteParam);
        strArgs += ",x";
        break;
    }

    case DP_INDEXED_Y:{
        uint8_t byteParam = args-cpu->y;
        strArgs += int_to_hex(byteParam);
        strArgs += ",y";
        break;
    }

    case ABS_INDEXED_X:{
        strArgs += int_to_hex(args-cpu->x);
        strArgs += ",x";
        break;
    }

    case ABS_INDEXED_Y:{
        strArgs += int_to_hex(args-cpu->y);
        strArgs += ",y";
        break;
    }

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
    std::sprintf(msg, "n=%d v=%d b=%d d=%d i=%d z=%d c=%d  a=%02X x=%02X y=%02X cycle=%d\t", 
        cpu->n, cpu->v, cpu->b, cpu->d, cpu->i, cpu->z, cpu->c, 
        cpu->a, cpu->x, cpu->y,
        cpu->cycleCounter);
    std::cout << msg;

    if(name=="rti"){
        cout << "\n**************** Exit IRQ ***************\n";
    }
}

void CPU::process(){
    Instruction::fetchInstruction(this);
}