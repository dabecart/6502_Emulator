#ifndef PERIPHERAL_h
#define PERIPHERAL_h

#include "constants.h"
#include "CPU.h"

class Peripheral : public Chip{
    public:
    Peripheral(const char chipName[], uint8_t pinCount, uint64_t IO, AddressList address);
    
    AddressList address;
    bool isBeingAdressed(CPU cpu);

    bool process(CPU &cpu);

    protected:
    virtual bool update(CPU &cpu) = 0;
};

// The chip 65C22
class VIA : public Peripheral {
    #define VIA_NAME "65C22"
    #define VIA_PIN_COUNT 40

    #define VIA_PA_PIN 2    // PA0 pin, total of eight, from PA0 to PA7.
    #define VIA_PB_PIN 10   // Pb0 pin, total of eight, from PB0 to PB7
    #define VIA_CA1 39  // Interrupt pins
    #define VIA_CA2 40
    #define VIA_CB1 18
    #define VIA_CB2 19

    public:
    VIA(AddressList address, uint16_t regConnection);

    uint16_t regConnection;

    private:
    uint8_t dataDirectionRegisterB = 0; // DDRB
    uint8_t dataDirectionRegisterA = 0; // DDRA
    bool update(CPU &cpu) override;

};

typedef struct{
    uint8_t RS, R_W, E;
    uint8_t DB[8]; //LSB to MSB
}LCD_Connection;

// A generic LCD display
class LCD{
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

    // First line goes from AC = 0x00 to 0x27, second line from 0x40 to 0x67. Total of 80 chars.
    uint8_t cursorAddress = 0;  // Where the cursor is located. 7 bit long.
    uint8_t displayShift = 0;   // The entire display can be shifted. Max 40.
    char displayedText[2][16];  // The text that is shown in the display.
    char displayMemory[80];    // The chars saved in the CGRAM, there are more here than in display!

    void updateLCD(uint64_t viaData);
    void clearDisplay();
    void recalculateDisplayedText();
    void fetchDataFromVIA(uint64_t viaData, LCD_Connection &data);
    void printDisplay();
};

#endif