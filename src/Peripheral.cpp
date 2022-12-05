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
            // When reading from the BUS it is supposed that the data from the children has been set on the
            // previous instructions.
            cpu->writeDataBus(getByte(VIA_PB_PIN, VIA_PB_PIN+7));
        }else{  // Writing (outputing)
            for(uint8_t i = 0; i < 8; i++){
                // Only output something is the bus is set to OUTPUT in the DDRB.
                if(getBitAt(dataDirectionRegisterB, i)){
                    setPinLevel(VIA_PB_PIN+i, getBitAt(cpu->getDataBus(), i));
                }
            }
            updateChildren = true;
        }
        break;
    }

    case 1:{ // Output/Input to register A
        if(cpu->r_wb){ //Reading (inputing)
            cpu->writeDataBus(getByte(VIA_PA_PIN, VIA_PA_PIN+7));
        }else{  // Writing (outputing)
            for(uint8_t i = 0; i < 8; i++){
                // Only output something is the bus is set to OUTPUT in the DDRA.
                if(getBitAt(dataDirectionRegisterA, i)){
                    setPinLevel(VIA_PA_PIN+i, getBitAt(cpu->getDataBus(), i));
                }
            }
            updateChildren = true;
        }
        break;
    }
    
    case 2:{ // Set DDRB
        dataDirectionRegisterB = cpu->getDataBus();
        // Beware that for the VIA 0 means input and 1 is output.
        setIOByte(VIA_PB_PIN, VIA_PB_PIN+7, ~dataDirectionRegisterB);
        break;
    }

    case 3:{ // Set DDRA
        dataDirectionRegisterA = cpu->getDataBus();
        // Beware that for the VIA 0 means input and 1 is output.
        setIOByte(VIA_PA_PIN, VIA_PA_PIN+7, ~dataDirectionRegisterA);
        break;
    }

    default:
        throwException("Register number %d in VIA 65C22 not supported", receivedReg);
        break;
    }

    cpu->clearBus(VIA_NAME); // Data received so it is cleared for the next round.
}

void VIA::postProcess(){}

LCD::LCD(LCD_Connection lcd_conn) : Chip(LCD_NAME){
    this->lcd_connections = lcd_conn;
}

void LCD::process(){
    LCD_Connection input;
    fetchDataFromVIA(input);
    
    bool enable = input.E;
    // Data has to be set previous to the enable signal. In some cases, it will happen that
    // the bus' data is set but the enable signal hasn't turned on yet.
    if(!enable) return;

    bool rs = input.RS;
    bool rw = input.R_W;
    uint8_t data = 0;
    for(uint8_t i = 0; i < 8; i++) data |= input.DB[i]<<i;

    if(!dl){ // If 4 bit mode is set, then the data is constructed in two rounds.
        static bool lastRS = rs, lastRW = rw;
        static uint8_t previousData = 0xFF; // Using only 4 bits, this value cannot be reached.
        if(previousData == 0xFF){
            lastRS = rs;
            lastRW = rw;
            previousData = data&0xF0;
            return;
        }else{
            if(lastRS != rs || lastRW != rw) throwException("LCD: If 4bit mode selected, data must come in two packages. RS/RW signals changed after one packet!");
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
            cout << "LCD: display clear";
            break;
        }

        case 1:{ // Return home
            cursorAddress = 0;
            displayShift = 0;
            cout << "LCD: Return home";
            break;
        }
        
        case 2:{ // Entry mode set
            i_d = getBitAt(data, 1);
            s = getBitAt(data, 0);
            cout << "LCD: Cursor move (i_d): " << i_d << ". Display shift (s): " << s << ".";
            break;
        }

        case 3:{ // Display on/off control
            d = getBitAt(data, 2);
            c = getBitAt(data, 1);
            b = getBitAt(data, 0);
            cout << "LCD: Display on/off (d): " << d << ". Cursor on/off (c): " << c << ". Blink cursor on/off (b): " << b << ".";
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
            printDisplay();
            break;
        }

        case 5:{ // Function set
            dl = getBitAt(data, 4);
            n = getBitAt(data, 3);
            f = getBitAt(data, 2);
            cout << "LCD: Data length (8bit/4bit): " << dl << " LCD display lines (2/1): " << n << " Font: " << f << ".";
            break;
        }

        // case 6: // Set CGRAM address not supported

        case 7:{ // Set cursor address (DDRAM)
            cursorAddress = data&0x7F;
            cout << "LCD: Cursor set to " << cursorAddress; 
            break;        
        }

        default:{
            throwException("Instruction %d not supported in LCD", data);
            break;
        }
        }
    }else if(rs && !rw){
        // Write character to the display memory.
        displayMemory[cursorAddress] = data;
        if(i_d) cursorAddress++;
        else cursorAddress--;

        if(s) displayShift++;

        cursorAddress%=80;
        displayShift%=40;

        recalculateDisplayedText();
        printDisplay();
    }else if(!rs && rw){
        if(dl){ // 8 bit mode
            for(uint8_t i = 0; i < 7; i++){
                if(lcd_connections.DB[i] != 0){ // Because it boots up in 8 bit mode, maybe the 4 lower pins are connected (4 bit mode).
                    parent->setPinLevel(lcd_connections.DB[i], getBitAt(cursorAddress, i));
                }
            }
            parent->setPinLevel(lcd_connections.DB[7], BF);
        }else{  // 4 bit mode
            static bool firstRead = true;
            if(firstRead){  // Upper read
                parent->setPinLevel(lcd_connections.DB[7], BF);
                for(uint8_t i = 4; i < 7; i++){
                    parent->setPinLevel(lcd_connections.DB[i], getBitAt(cursorAddress, /*6-(i-4)*/ 10-i));
                }
            }else{  // Lower read
                for(uint8_t i = 4; i < 8; i++){
                    parent->setPinLevel(lcd_connections.DB[i], getBitAt(cursorAddress, i-4));
                }
            }
            firstRead = !firstRead;
        }
    }else{
        throwException("RS and RW configuration not supported in LCD");
    }
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

void LCD::fetchDataFromVIA(LCD_Connection &data){
    data.RS = parent->getPinLevel(lcd_connections.RS);
    data.R_W = parent->getPinLevel(lcd_connections.R_W);
    data.E = parent->getPinLevel(lcd_connections.E);
    for(uint8_t i = 0; i < 8; i++){
        if(lcd_connections.DB[i] == 0){ // Pin is not connected (4 bit mode)
            data.DB[i] = 0;
        }else{
            data.DB[i] = parent->getPinLevel(lcd_connections.DB[i]);
        }
    }
}

void LCD::printDisplay(){
    if(!d){
        cout << "LCD is OFF";
        return;
    }
    cout << "\tLCD: ";
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