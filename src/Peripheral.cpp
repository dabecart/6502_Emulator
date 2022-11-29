#include "Peripheral.h"

VIA::VIA(uint16_t add, uint16_t regConnection) : Peripheral(add){
    this->regConnection = regConnection;
}

void VIA::updateFunction(CPU &cpu){
    // Fetch the received register code.
    uint16_t temp = regConnection;
    uint16_t receivedReg = cpu.addressBus & regConnection;
    while(!temp&0x01){
        temp = temp >> 1;
        receivedReg = receivedReg >> 1;
    }

    switch (receivedReg){
    case 0:
        if(cpu.r_wb){ //Reading
            //TODO
        }else{

        }
        break;
    
    default:
        char msg[60];
        std::sprintf(msg, "Register number %d in VIA 65C22 not supported", receivedReg);
        throw std::invalid_argument(msg);
        break;
    }

    cpu.clearBus();
}

LCD::LCD(LCD_Connection lcd_conn){
    this->lcd_connections = lcd_conn;
}

void LCD::updateLCD(uint64_t viaData){
    LCD_Connection input;
    fetchDataFromVIA(viaData, input);
    
    bool enable = input.E;
    bool rs = input.RS;
    bool rw = input.R_W;
    uint8_t data = 0;
    for(uint8_t i = 0; i < 8; i++) data |= input.DB[i]<<i;

    // Data has to be set previous to the enable signal. In some cases, it will happen that
    // the bus' data is set but the enable signal hasn't turned on yet.
    if(!enable) return;

    if(!rs && !rw){
        if(!dl){ // If 4 bit mode is set, then the data is constructed in two rounds.
            static uint8_t previousData = 0xFF; // Using only 4 bits, this value cannot be reached.
            if(previousData == 0xFF){
                previousData = data<<4;
                return;
            }else{
                data = previousData | (data&0x0F);
                previousData = 0xFF;
            }
        }

        uint8_t MSB_position = 7;
        // TODO: check this
        while(!getBitAt(data, MSB_position)){MSB_position--;}

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
            char msg[60];
            std::sprintf(msg, "Instruction %d not supported in LCD", data);
            throw std::invalid_argument(msg);
            break;
        }
        }
    } else if(rs && !rw){
        // Write character to the display memory.
        displayMemory[cursorAddress] = data;
    }else{
        char msg[60];
        std::sprintf(msg, "RS and RW configuration not supported in LCD", data);
        throw std::invalid_argument(msg);
    }
}

void LCD::clearDisplay(){
    for(uint8_t i = 0; i < 2; i++){
        for (uint8_t j = 0; j < 16; j++){
            displayedText[i][j] = 0;
        }
    }
    std::fill_n(displayMemory, std::size(displayMemory), 0);
}

void LCD::recalculateDisplayedText(){
    //TODO: check
    for(int i = 0; i < 16; i++){
        displayedText[0][i] = displayMemory[(displayShift+i)%40];
        displayedText[1][i] = displayMemory[0x27 + (displayShift+i)%40];
    }
}

void LCD::fetchDataFromVIA(uint64_t viaData, LCD_Connection &data){
    data.RS = getBitAt(viaData, lcd_connections.RS);
    data.R_W = getBitAt(viaData, lcd_connections.R_W);
    data.E = getBitAt(viaData, lcd_connections.E);
    for(uint8_t i = 0; i < 8; i++) data.DB[i] = getBitAt(viaData, lcd_connections.DB[i]);
}

Peripheral::Peripheral(uint16_t add){
    this->address = add;
}

bool Peripheral::isBeingAdressed(CPU cpu){
    return (cpu.addressBus & address) == address;
}

void Peripheral::process(CPU &cpu){
    if(isBeingAdressed(cpu)) updateFunction(cpu);
}