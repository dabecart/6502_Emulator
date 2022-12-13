#ifndef PERIPHERAL_h
#define PERIPHERAL_h

#include "constants.h"
#include "CPU.h"

class Peripheral : public Chip{
    public:
    Peripheral(const char chipName[], uint8_t pinCount, uint64_t IO, AddressList address);
    
    AddressList address;
    bool isBeingAdressed();

    void process() override;
    // postProcess() will be handled but the abstracting classes.
    // processIRQ()  same...

    protected:
    virtual void updatePeripheral() = 0;
};

// The chip 65C22
class VIA : public Peripheral {
    #define VIA_NAME "65C22"
    #define VIA_PIN_COUNT 40

    #define VIA_PA_PIN 2    // PA0 pin, total of eight, from PA0 to PA7.
    #define VIA_PB_PIN 10   // Pb0 pin, total of eight, from PB0 to PB7
    #define VIA_CA1 40  // Interrupt pins
    #define VIA_CA2 39
    #define VIA_CB1 18
    #define VIA_CB2 19

    public:
    VIA(AddressList address, uint16_t regConnection);

    uint16_t regConnection;

    private:
    uint8_t dataDirectionRegisterB = 0; // DDRB Reg 2
    uint8_t dataDirectionRegisterA = 0; // DDRA Reg 3

    //uint8_t peripheralControlRegister = 0;  // PCR Reg C
    uint8_t cb2_control = 0, ca2_control = 0;
    bool cb1_control = false, ca1_control = false;  // 0 = Negative active edge, 1 = Positive active edge

    uint8_t interruptFlagRegister = 0;      // IFR Reg D

    // Stores which interrupt sources are enable. 1=Enable.
    // 7->IRQ, 6->Timer1, 5->Timer2, 4->CB1, 3->CB2, 2->Shift Reg, 1->CA1, 0->CA2
    #define VIA_IRQ_BITPOS 7
    #define VIA_TIMER1_BITPOS 6
    #define VIA_TIMER2_BITPOS 5
    #define VIA_CB1_BITPOS 4
    #define VIA_CB2_BITPOS 3
    #define VIA_SHIFT_REG_BITPOS 2
    #define VIA_CA1_BITPOS 1
    #define VIA_CA2_BITPOS 0
    uint8_t interruptEnableRegister = 0;    // IER Reg E

    // Because they do the same in different lines, this is put in a function to be called twice.
    void setCB2andCA2Lines(bool isCB2, uint8_t code /*From 0 to 7*/);
    void handleCB2andCA2Lines(bool isCB2, bool level, bool &launchIRQ);

    void updatePeripheral() override;
    void postProcess() override;
    void processIRQ() override;
};

typedef struct{
    uint8_t RS, R_W, E;
    uint8_t DB[8]; //LSB to MSB
}LCD_Connection;

// A generic LCD display
class LCD : public Chip{
    #define LCD_NAME "LCD"

    public:
    // To create an LCD, it is necessary to pass the number of the pins connected to the VIA to form
    // the connections. When connecting to the VIA to fetch the data, it will return an uint64_t with 
    // the correspondent output of its pins. The LCD will fetch the information attending the pinout inside
    // the LCD_Connection. PINS START AT 1 (as in all datasheets). 0 means NO CONNECTION.
    LCD(LCD_Connection);
    LCD_Connection lcd_connections;

    bool i_d = 0;   // Cursor move direction. I/D=1 increment, I/D=0 decrement. 
    bool s = 0;     // Display shift
    bool d = 0;     // Display on/off. D = 1 on
    bool c = 0;     // Cursor on/off. C = 1 on
    bool b = 0;     // Blink cursor on/off. B = 1 on
    bool dl = 1;    // Data length (mode of operation). DL=1->8 bit mode. DL=0->4 bit mode.
    bool n = 0;     // Number of display lines. N=1->2 lines. N=0->1 line.
    bool f = 0;     // Character font. F=1->5x10 dots. F=0->5x8 dots.

    bool BF = 0;    // Busy flag. 1=Busy

    // First line goes from AC = 0x00 to 0x27, second line from 0x40 to 0x67. Total of 80 chars.
    uint8_t cursorAddress = 0;  // Where the cursor is located. 7 bit long.
    uint8_t displayShift = 0;   // The entire display can be shifted. Max 40.
    char displayedText[2][16];  // The text that is shown in the display.
    char displayMemory[80];    // The chars saved in the CGRAM, there are more here than in display!

    void process() override;
    void postProcess() override;
    void processIRQ() override;
    void clearDisplay();
    void recalculateDisplayedText();
    void fetchDataFromVIA(LCD_Connection &data);
    void printDisplay();
};

typedef struct{
    char symbol;
    uint8_t scanCode;
}Key;

class Keyboard{
    #define KEYBOARD_INTERKEY_TIME_MS 2
    #define KEYBOARD_RELEASE_TIME_MS 2
    #define KEYBOARD_INTERVAL_F0_KEYCODE 2 // Interval between 0xF0 and the keyscan released

    public:
    Keyboard(Chip *connectedToChip, uint8_t pinConnections[8], uint8_t IRQ);

    string keySequence;
    uint8_t currentKeyIndex = 0; // Index of last character sent of keySequence. 

    Chip *parent;
    uint8_t pinConnections[8];
    uint8_t IRQ_pin;

    void setKeySequence(string str);
    void pressKey(char c);
    void sendScanCode(uint8_t scanCode);

    void typeKeySequence(uint64_t time_ms, uint64_t startTypingTime);

    bool keySequenceFinished();

};

#endif