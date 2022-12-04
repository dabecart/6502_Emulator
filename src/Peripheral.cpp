#include "Peripheral.h"

Peripheral::Peripheral(const char chipName[], uint8_t pinCount, uint64_t IO, AddressList address) : Chip(chipName, pinCount, IO){
    this->address = address;
}

bool Peripheral::isBeingAdressed(){
    // TODO: It is considered that the CPU is the only one that can address, but that may change.
    CPU* cpu = dynamic_cast<CPU*>(parent);
    for(AddressPin ad : address){
        if(getBitAt(cpu->addressBus, ad.pinNumber) != ad.value) return false;
    }
    return true;
}

void Peripheral::process(){
    if(isBeingAdressed()) updatePeripheral();
}

VIA::VIA(AddressList add, uint16_t regConnection) : Peripheral(VIA_NAME, VIA_PIN_COUNT, 0, add){
    this->regConnection = regConnection;
}

void VIA::updatePeripheral(){
    CPU* cpu = dynamic_cast<CPU*>(parent);

    // Fetch the received register code.
    uint16_t temp = regConnection;
    uint16_t receivedReg = cpu->addressBus & regConnection;
    while(!temp&0x01){
        temp = temp >> 1;
        receivedReg = receivedReg >> 1;
    }

    switch (receivedReg){
    case 0:{ // Output/Input to register B
        if(cpu->r_wb){ //Reading (inputing)
            //TODO
        }else{  // Writing (outputing)
            pinoutSignals &= ~(0xFF<<VIA_PB_PIN); // Convert only this port to 0.
            for(uint8_t i = 0; i < 8; i++){
                // Only output something is the bus is set to OUTPUT in the DDRB.
                if(getBitAt(dataDirectionRegisterB, i)){
                    pinoutSignals |= (getBitAt(cpu->getDataBus(), i)<<(VIA_PB_PIN+i));
                }
            }
            updateChildren = true;
        }
        break;
    }

    case 1:{ // Output/Input to register A
        if(cpu->r_wb){ //Reading (inputing)
            //TODO
        }else{  // Writing (outputing)
            pinoutSignals &= ~(0xFF<<VIA_PA_PIN); // Convert only this port to 0.
            for(uint8_t i = 0; i < 8; i++){
                // Only output something is the bus is set to OUTPUT in the DDRA.
                if(getBitAt(dataDirectionRegisterA, i)) pinoutSignals |= (getBitAt(cpu->getDataBus(), i)<<(VIA_PA_PIN+i));
            }
            updateChildren = true;
        }
        break;
    }
    
    case 2:{ // Set DDRB
        dataDirectionRegisterB = cpu->getDataBus();
        break;
    }

    case 3:{ // Set DDRA
        dataDirectionRegisterA = cpu->getDataBus();
        break;
    }

    default:
        throwException("Register number %d in VIA 65C22 not supported", receivedReg);
        break;
    }

    cpu->clearBus(VIA_NAME); // Data received so it is cleared for the next round.
}

void VIA::postProcess(){}

LCD::LCD(LCD_Connection lcd_conn) : Chip("LCD"){
    this->lcd_connections = lcd_conn;
}

void LCD::process(){
    uint64_t viaData = parent->pinoutSignals;
    LCD_Connection input;
    fetchDataFromVIA(viaData, input);
    
    bool enable = input.E;
    // Data has to be set previous to the enable signal. In some cases, it will happen that
    // the bus' data is set but the enable signal hasn't turned on yet.
    if(!enable) return;


    bool rs = input.RS;
    bool rw = input.R_W;
    uint8_t data = 0;
    for(uint8_t i = 0; i < 8; i++) data |= input.DB[i]<<i;

    if(!dl){ // If 4 bit mode is set, then the data is constructed in two rounds.
        static uint8_t previousData = 0xFF; // Using only 4 bits, this value cannot be reached.
        if(previousData == 0xFF){
            previousData = data&0xF0;
            return;
        }else{
            data = previousData | (data>>4);
            previousData = 0xFF;
        }
    }

    if(!rs && !rw){
        uint8_t MSB_position = 7;
        // TODO: check this
        while(!getBitAt(data, MSB_position) && MSB_position>0){MSB_position--;}

        switch (MSB_position) {
        case 0:{ // Clear display
            clearDisplay();
            break;
        }

        case 1:{ // Return home
            cursorAddress = 0;
            displayShift = 0;
            break;
        }
        
        case 2:{ // Entry mode set
            i_d = getBitAt(data, 1);
            s = getBitAt(data, 0);
            break;
        }

        case 3:{ // Display on/off control
            d = getBitAt(data, 0);
            c = getBitAt(data, 1);
            b = getBitAt(data, 2);
            break;
        }

        case 4:{ // Cursor or display shift
            bool disp_cursor = getBitAt(data, 3);
            bool right_left = getBitAt(data, 2);
            int increment = right_left?1:-1;
            if(disp_cursor){
                displayShift+=increment;
                displayShift%=40;
            }else{
                cursorAddress+=increment;
                cursorAddress%=80;
            }
            break;
        }

        case 5:{ // Function set
            dl = getBitAt(data, 4);
            n = getBitAt(data, 3);
            f = getBitAt(data, 2);
            break;
        }

        // case 6: // Set CGRAM address not supported

        case 7:{ // Set cursor address (DDRAM)
            cursorAddress = data&0x7F;
            break;        
        }

        default:{
            throwException("Instruction %d not supported in LCD", data);
            break;
        }
        }
    } else if(rs && !rw){
        // Write character to the display memory.
        displayMemory[cursorAddress] = data;
        if(i_d) cursorAddress++;
        else cursorAddress--;

        if(s) displayShift++;

        cursorAddress%=80;
        displayShift%=40;

        recalculateDisplayedText();
    }else{
        throwException("RS and RW configuration not supported in LCD");
    }
    printDisplay();
}

void LCD::postProcess(){}

void LCD::clearDisplay(){
    std::fill_n(displayMemory, std::size(displayMemory), 0);
    recalculateDisplayedText();
}

void LCD::recalculateDisplayedText(){
    if(n){
        for(int i = 0; i < 16; i++){
            displayedText[0][i] = displayMemory[(displayShift+i)%40];
            displayedText[1][i] = displayMemory[0x28 + (displayShift+i)%40];
        }
    }else{
        for(int i = 0; i < 16; i++){
            displayedText[0][i] = displayMemory[(displayShift+i)%80];
        }
    }
}

void LCD::fetchDataFromVIA(uint64_t viaData, LCD_Connection &data){
    data.RS = getBitAt(viaData, lcd_connections.RS);
    data.R_W = getBitAt(viaData, lcd_connections.R_W);
    data.E = getBitAt(viaData, lcd_connections.E);
    for(uint8_t i = 0; i < 8; i++) data.DB[i] = getBitAt(viaData, lcd_connections.DB[i]);
}

void LCD::printDisplay(){
    cout << "\t";
    for(int i = 0; i < 16; i++){
        char c = displayedText[0][i];
        if(c < ' ') c = ' ';
        cout << c << ' ';
    }
    cout << "--- ";
    for(int i = 0; i < 16; i++){
        char c = displayedText[1][i];
        if(c < ' ') c = ' ';
        cout << c << ' ';
    }
}