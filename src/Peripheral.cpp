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
    this->setPinIO(VIA_CA1, true);
    this->setPinIO(VIA_CB1, true);
    this->setPinIO(VIA_CA2, true);
    this->setPinIO(VIA_CB2, true);
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
            uint8_t dataRead = getByte(VIA_PB_PIN, VIA_PB_PIN+7);
            cpu->writeDataBus(dataRead);
            cout << "VIA: Reading PB = " << int_to_hex(dataRead);
            cout << "\n\t\t";
        }else{  // Writing (outputing)
            for(uint8_t i = 0; i < 8; i++){
                // Only output something is the bus is set to OUTPUT in the DDRB.
                if(getBitAt(dataDirectionRegisterB, i)){
                    setPinLevel(VIA_PB_PIN+i, getBitAt(cpu->getDataBus(), i));
                }
            }
            updateChildren = true;
        }

        // When read or write to ORB, CB1 always gets cleared.
        setBitAt(interruptFlagRegister, VIA_CB1_BITPOS, false);
        // In case of CB2, it depends if it is selected as independent.
        if(cb2_control != 1 && cb2_control != 3) setBitAt(interruptFlagRegister, VIA_CB2_BITPOS, false);
        break;
    }

    case 1:{ // Output/Input to register A
        if(cpu->r_wb){ //Reading (inputing)
            uint8_t dataRead = getByte(VIA_PA_PIN, VIA_PA_PIN+7);
            cpu->writeDataBus(dataRead);
            cout << "VIA: Reading PA = " << int_to_hex(dataRead);
            cout << "\n\t\t";
        }else{  // Writing (outputing)
            for(uint8_t i = 0; i < 8; i++){
                // Only output something is the bus is set to OUTPUT in the DDRA.
                if(getBitAt(dataDirectionRegisterA, i)){
                    setPinLevel(VIA_PA_PIN+i, getBitAt(cpu->getDataBus(), i));
                }
            }
            updateChildren = true;
        }
        // When read or write to ORB, CA1 always gets cleared.
        setBitAt(interruptFlagRegister, VIA_CA1_BITPOS, false);
        // In case of CA2, it depends if it is selected as independent.
        if(ca2_control != 1 && ca2_control != 3) setBitAt(interruptFlagRegister, VIA_CA2_BITPOS, false);
        break;
    }
    
    case 2:{ // Get/Set DDRB
        if(cpu->r_wb){
            cpu->writeDataBus(dataDirectionRegisterB);
        }else{
            dataDirectionRegisterB = cpu->getDataBus();
            // Beware that for the VIA 0 means input and 1 is output.
            setIOByte(VIA_PB_PIN, VIA_PB_PIN+7, ~dataDirectionRegisterB);
            cout << "VIA: DDRB = " << int_to_hex(dataDirectionRegisterB);
        }
        break;
    }

    case 3:{ // Get/Set DDRA
        if(cpu->r_wb){
            cpu->writeDataBus(dataDirectionRegisterA);
        }else{
            dataDirectionRegisterA = cpu->getDataBus();
            // Beware that for the VIA 0 means input and 1 is output.
            setIOByte(VIA_PA_PIN, VIA_PA_PIN+7, ~dataDirectionRegisterA);
            cout << "VIA: DDRA = " << int_to_hex(dataDirectionRegisterA);
        }
        break;
    }

    case 0xC: { // Set the Peripheral Control Register
        // CB2 control
        uint8_t temp = (cpu->getDataBus()>>5)&7;
        if(temp != cb2_control){
            cb2_control = temp;
            setCB2andCA2Lines(true, cb2_control);
        }

        //CB1 control
        bool tempBool = getBitAt(cpu->getDataBus(), 4);
        if(tempBool != cb1_control){
            cb1_control = tempBool;
            if(cb1_control){
                cout << "VIA: CB1 control set to Positive Active Edge";
            }else{
                cout << "VIA: CB1 control set to Negative Active Edge";
            }
        }

        // CA2 control
        temp = (cpu->getDataBus()>>1)&7;
        if(temp != ca2_control){
            ca2_control = temp;
            setCB2andCA2Lines(false, ca2_control);
        }

        //CA1 control
        tempBool = getBitAt(cpu->getDataBus(), 0);
        if(tempBool != ca1_control){
            ca1_control = tempBool;
            if(ca1_control){
                cout << "VIA: CA1 control set to Positive Active Edge";
            }else{
                cout << "VIA: CA1 control set to Negative Active Edge";
            }
        }
        break;
    }

    case 0xD:{  // Read the Interrupt Flag Register, aka. what caused the interrupt?
        // The IFR contains all the current exceptions, even those who are not enable in the IER.
        // Set the IRQ bit if there are any interrupts
        if(interruptFlagRegister&0x7F > 0) interruptFlagRegister |= 0x80;
        cpu->writeDataBus(interruptFlagRegister);
        cout << "VIA: Interrupt Flag Register = " << int_to_hex(interruptFlagRegister);
        break;
    }

    case 0xE:{  // Write the Interrupt Enable Register
        uint8_t data = cpu->getDataBus();
        // The bit at position 7 tells if the vector is for enabling or disabling interrupts.
        // The next bits tells which interrupt will be enabled/disable, depending on the 7th bit.
        bool enable = data>>7;
        for(uint8_t i = 0; i < 7; i++){
            if(getBitAt(data, i)) setBitAt(interruptEnableRegister, i, enable);
        }
        cout << "VIA: Interrupt Enable Register set to " << int_to_hex(interruptEnableRegister);
        break;
    }

    default:
        throwException("Register number %X in VIA 65C22 not supported", receivedReg);
        break;
    }
}

void VIA::postProcess(){}

void VIA::processIRQ(){
    static bool lastCA1 = false, lastCA2 = false, lastCB1 = false, lastCB2 = false;
    bool CA1_level = getPinLevel(VIA_CA1);
    bool CA2_level = getPinLevel(VIA_CA2);
    bool CB1_level = getPinLevel(VIA_CB1);
    bool CB2_level = getPinLevel(VIA_CB2);

    bool launchIRQ = false;
    if(getBitAt(interruptEnableRegister, VIA_CA1_BITPOS) && lastCA1 != CA1_level){
        if(CA1_level && ca1_control || !CA1_level && !ca1_control){
            setBitAt(interruptFlagRegister, VIA_CA1_BITPOS, true);
            launchIRQ = true;
            cout << endl <<  "CA1 caused an interrupt" << endl;
        }
    }

    if(getBitAt(interruptEnableRegister, VIA_CB1_BITPOS) && lastCB1 != CB1_level){
        if(CB1_level && cb1_control || !CB1_level && !cb1_control){
            setBitAt(interruptFlagRegister, VIA_CB1_BITPOS, true);
            launchIRQ = true;
            cout << endl <<  "CB1 caused an interrupt" << endl;
        }
    }

    if(getBitAt(interruptEnableRegister, VIA_CB2_BITPOS) && lastCB2 != CB2_level){
        handleCB2andCA2Lines(true, CB2_level, launchIRQ);
    }

    if(getBitAt(interruptEnableRegister, VIA_CA2_BITPOS) && lastCA2 != CA2_level){
        handleCB2andCA2Lines(false, CA2_level, launchIRQ);
    }

    parent->setPinLevelFromIRQ(CPU_IRQB, !launchIRQ /*Because its IRQB (inverted!)*/);
    parent->launchIRQ(); // The CPU will acknowledge the IRQB pin level.
}

void VIA::setCB2andCA2Lines(bool isCB2, uint8_t code){
    string viaName = isCB2 ? "CB2" : "CA2";
    cout << "VIA: " << viaName << " control set to: ";
    switch (code){
    case 0:{
        cout << "Input-negative active edge";
        break;
    }

    case 1:{
        cout << "Independent interrupt input-negative edge";
        break;
    }

    case 2:{
        cout << "Input-positive active edge";
        break;
    }

    case 3:{
        cout << "Independent interrupt input-positive edge";
        break;
    }
    
    default:
        throwException("VIA: %s control code %d not supported", viaName, code);
    }
}

void VIA::handleCB2andCA2Lines(bool isCB2, bool level, bool &launchIRQ){
    uint8_t control = isCB2 ? cb2_control : ca2_control;
    string viaName = isCB2 ? "CB2" : "CA2";
    bool hasThisChannelCausedAnInterrupt = false;
    switch (control){
    case 0:
    case 1:{
        if(!level){
            hasThisChannelCausedAnInterrupt = true;
        }
        break;
    }

    case 2:
    case 3:{
        if(level){
            hasThisChannelCausedAnInterrupt = true;
        }
        break;
    }

    default:
        throwException("VIA: %s code for handling %d interrupts still not supported", viaName, control);
    }

    if(hasThisChannelCausedAnInterrupt){
        launchIRQ = true;
        cout << endl <<  viaName << " caused an interrupt" << endl;
        setBitAt(interruptFlagRegister, isCB2?VIA_CB2_BITPOS:VIA_CA2_BITPOS, true);
    }
}

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

    if(!rs && rw){    // Read the busy flag and address vector.
        if(dl){ // 8 bit mode
            BF = !BF;   //Flip BF to test
            for(uint8_t i = 0; i < 7; i++){
                if(lcd_connections.DB[i] != 0){ // Because it boots up in 8 bit mode, maybe the 4 lower pins are connected (4 bit mode).
                    parent->setPinLevel(lcd_connections.DB[i], getBitAt(cursorAddress, i));
                }
            }
            parent->setPinLevel(lcd_connections.DB[7], BF);
        }else{  // 4 bit mode
            static bool firstRead = true;
            if(firstRead){  // Upper read
                BF = !BF;   //Flip BF to test
                parent->setPinLevel(lcd_connections.DB[7], BF);
                for(uint8_t i = 4; i < 7; i++){
                    parent->setPinLevel(lcd_connections.DB[i], getBitAt(cursorAddress, /*6-(i-4)*/ 10-i));
                }
            }else{  // Lower read
                for(uint8_t i = 4; i < 8; i++){
                    parent->setPinLevel(lcd_connections.DB[i], getBitAt(cursorAddress, i-4));
                }
                cout << "VIA: Busy flag: " << BF;
            }
            firstRead = !firstRead;
        }
        return;
    }

    if(BF) throwException("Busy flag is not zero!");

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
        if(BF) throwException("Busy flag is not zero!");

        // Write character to the display memory.
        displayMemory[cursorAddress] = data;
        if(i_d) cursorAddress++;
        else cursorAddress--;

        if(s) displayShift++;

        cursorAddress%=80;
        displayShift%=40;

        recalculateDisplayedText();
        printDisplay();
    }else{
        throwException("RS and RW configuration not supported in LCD");
    }
}

void LCD::postProcess(){}

void LCD::processIRQ(){}

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
    cout << "LCD: ";
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

string keymap =     "????????????? `?" //00-0F
                    "?????q1???zsaw2?" //10-1F
                    "?cxde43?? vftr5?" //20-2F
                    "?nbhgy6???mju78?" //30-3F
                    "?,kio09??./l;p-?" //40-4F
                    "??'?[=????\n]?\\??" //50-5F
                    "?????????1?47???" //60-6F
                    "0.2568???+3-*9??" //70-7F
                    "????????????????" //80-8F
                    "????????????????" //90-9F
                    "????????????????" //A0-AF
                    "????????????????" //B0-BF
                    "????????????????" //C0-CF
                    "????????????????" //D0-DF
                    "????????????????" //E0-EF
                    "????????????????"; //F0-FF

Keyboard::Keyboard(Chip *cc, uint8_t pinCons[8], uint8_t irq){
    this->parent = cc;
    memcpy(&pinConnections[0], &pinCons[0], 8);
    this->IRQ_pin = irq;
}

void Keyboard::pressKey(char c){
    uint8_t keyCode = 0;
    for(keyCode = 0; keyCode < 0xFF; keyCode++){
        if(keymap[keyCode] == c) break;
    }
    sendScanCode(keyCode);
}

void Keyboard::sendScanCode(uint8_t scanCode){
    for(uint8_t i = 0; i < 8; i++){
        parent->setPinLevelFromIRQ(pinConnections[i], getBitAt(scanCode, i));
    }
    parent->setPinLevelFromIRQ(IRQ_pin, true);
    parent->launchIRQ();
}

void Keyboard::releaseKey(char c){
    sendScanCode(0xF0);

}