#include "CPU.h"

Chip::Chip(const string chipName, uint8_t pinCount, uint64_t IO, uint64_t defaultPinoutSignal, bool irq){
    this->pinCount = pinCount;
    this->IO = IO;
    this->chipName = chipName;
    this->pinoutSignals = defaultPinoutSignal;
    this->interruptIfISR = irq;
}

void Chip::addChild(Chip* chip){
    children.push_back(chip);
    chip->parent = this;
}

void Chip::setPinLevel(uint8_t pinNumber, bool level){
    if(!pinNumber || pinNumber > pinCount) throwException("Pin number %d of %s not valid. No level can be set!\n", pinNumber, chipName);
    
    // If expectsData = false, the caller of this function must be the chip itself, so its not possible to write to an INPUT
    if(!expectsData && !updateChildren && getBitAt(IO, pinNumber)) throwException("Pin %d of %s is an INPUT, not output\n",pinNumber, chipName);
    
    // If expectsData = true, the caller of this function must be the child, so its not possible to write to an OUTPUT
    // It would also be possible that the children is outputing something that will be read later by the parent
    if((expectsData || updateChildren) && !getBitAt(IO, pinNumber)) throwException("Pin %d of %s is an OUTPUT, not input\n",pinNumber, chipName);
    if(getBitAt(setPins, pinNumber)) throwException("Pin %d of %s has a value not read", pinNumber, chipName);

    setBitAt(setPins, pinNumber, true); // Sets the pin as set. It will store a value to be read.
    setBitAt(pinoutSignals, pinNumber, level); // Sets it on the level required.
}

void Chip::setPinIO(uint8_t pinNumber, bool i_o){
    if(!pinNumber || pinNumber > pinCount) throwException("Pin %d of %s not valid. Cannot set IO\n", pinNumber, chipName);

    setBitAt(IO, pinNumber, i_o);
    setBitAt(setPins, pinNumber, false);
}

bool Chip::getPinLevel(uint8_t pinNumber){
    if(!pinNumber || pinNumber > pinCount) throwException("Pin %d of %s not valid. Cannot get pin level\n", pinNumber, chipName);

    // Multiple reads can be allowed, so the setPins variable just gets cleared.
    setBitAt(setPins, pinNumber, false);
    return getBitAt(pinoutSignals, pinNumber);
}

uint8_t Chip::getByte(uint8_t LSB, uint8_t MSB){
    if(!LSB || LSB > pinCount) throwException("Pin %d of %s not valid. Cannot getByte\n", LSB, chipName);
    if(!MSB || MSB > pinCount) throwException("Pin %d of %s not valid. Cannot getByte\n", MSB, chipName);
    uint8_t differenceBetweenLSBandMSB = 0;
    if(MSB>LSB) differenceBetweenLSBandMSB = MSB-LSB;
    else differenceBetweenLSBandMSB = LSB-MSB;
    if(differenceBetweenLSBandMSB > 7) throwException("%s LSB-MSB > 7. Cannot getByte", chipName);

    uint8_t result = 0;
    uint8_t pos = LSB;
    uint8_t bitPosition = 0;
    do{
        result |= getPinLevel(pos)<<bitPosition;
        if(LSB<MSB) pos++;
        else pos--;
        bitPosition++;
    }while(pos != MSB);
    result |= getPinLevel(pos)<<differenceBetweenLSBandMSB;
    return result;
}

void Chip::setIOByte(uint8_t LSB, uint8_t MSB, uint8_t IO_data){
    if(!LSB || LSB > pinCount) throwException("Pin %d of %s not valid. Cannot set as IObyte\n", LSB, chipName);
    if(!MSB || MSB > pinCount) throwException("Pin %d of %s not valid. Cannot set as IObyte\n", MSB, chipName);
    uint8_t differenceBetweenLSBandMSB = 0;
    if(MSB>LSB) differenceBetweenLSBandMSB = MSB-LSB;
    else differenceBetweenLSBandMSB = LSB-MSB;
    if(differenceBetweenLSBandMSB > 7) throwException("%s LSB-MSB > 7. Cannot set IOByte", chipName);

    uint8_t pos = LSB;
    uint8_t bitPosition = 0;
    do{
        setPinIO(pos, getBitAt(IO_data, bitPosition));
        if(LSB<MSB) pos++;
        else pos--;
        bitPosition++;
    }while(pos != MSB);
    setPinIO(pos, getBitAt(IO_data, differenceBetweenLSBandMSB));
}

void Chip::setPinLevelFromIRQ(uint8_t pinNumber, bool level){
    updateChildren = true;
    setPinLevel(pinNumber, level);
}

bool Chip::getPinLevelFromIRQ(uint8_t pinNumber){
    updateChildren = true;
    return getPinLevel(pinNumber);
}

void Chip::launchIRQ(){
    if(interruptIfISR){
        processIRQ();
    }
}

void Chip::run(){
    expectsData = false;
    updateChildren = false;
    process();
    if(expectsData || updateChildren){
        // This will ensure that children get the data and furthermore, answer the call from the parent.
        postProcess();
        runChildren();
        if(expectsData){
            process();
            if(updateChildren){ // Specially made for INC operation
                postProcess();
                runChildren();
            }
        }
    }
}

void Chip::runChildren(){
    for(Chip* child : children){
        child->run();
    }
}

CPU::CPU() : Chip("65C02", 40, 1<<CPU_IRQB | 1<<CPU_NMIB, 1<<CPU_IRQB | 1<<CPU_NMIB, false){
    RAM_address = {{15,0}, {14,0}};
    ROM_address = {{15,1}, {14,1}, {13,1}};
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
        cout << "RAM: Read " << int_to_hex(dataBus) << " from " << int_to_hex(addressBus);
        cout << "\n\t\t";
    }else{
        writeRAM(addressBus, dataBus);
        cout << "RAM: Write " << int_to_hex(dataBus) << " to " << int_to_hex(addressBus);
    }
}

void CPU::ROMListener(){
    for(AddressPin ad : ROM_address){
        if(getBitAt(addressBus, ad.pinNumber) != ad.value) return;
    }
    if(r_wb){
        dataBus = readROM(addressBus);
        cout << "ROM: Read " << int_to_hex(dataBus) << " from " << int_to_hex(addressBus);
        cout << "\n\t\t";
    }else{
        throwException("Something tried to write to ROM!");
    }
    
}

void CPU::pushToStack(uint8_t data){
    if(stackPointer == 0x100){
        std::cout << "Stack overflow! ";
        stackPointer = 0x1FF;
    }else{
        stackPointer--;
    }
    writeRAM(stackPointer, data);
}

uint8_t CPU::pullFromStack(){
    uint8_t data = readRAM(stackPointer);
    if(stackPointer == 0x1FF){
        std::cout << "Stack overflow ";
        stackPointer = 0x100;
    }else{
        stackPointer++;
    }
    return data;
}

void CPU::postProcess(){
    RAMListener();
    ROMListener();
}

void CPU::processIRQ(){
    if(!i && !getPinLevel(CPU_IRQB)){
        i = 1;  // Disable interrupts until handled correctly
        pushToStack(pc>>8);
        pushToStack(pc);
        pushToStack(getStatusRegister());

        pc = readROM(0xFFFE) | (readROM(0xFFFF)<<8); 
        cout << "\n **************** IRQ Routine ***************";
    }
    if(!getPinLevel(CPU_NMIB)){
        i = 1;  // Disable interrupts until handled correctly
        pushToStack(pc>>8);
        pushToStack(pc);
        pushToStack(getStatusRegister());

        pc = readROM(0xFFFA) | (readROM(0xFFFB)<<8); 
    }
}

uint8_t CPU::getStatusRegister(){
    bool statReg[7] = {n,v,b,d,i,z,c};
    uint8_t statInt = 0;
    for(uint8_t i = 0; i < 7; i++) statInt |= statReg[i]<<i;
    return statInt;
}

void CPU::setStatusRegister(uint8_t st){
    n = getBitAt(st, 0);
    v = getBitAt(st, 1);
    b = getBitAt(st, 2);
    d = getBitAt(st, 3);
    i = getBitAt(st, 4);
    z = getBitAt(st, 5);
    c = getBitAt(st, 6);
}


// This method can be found in instruction.cpp
/*void CPU::process(){
    Instruction::fetchInstruction(this);
}*/