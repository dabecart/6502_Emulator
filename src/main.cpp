#include "constants.h"
#include "instruction.h"
#include <chrono>
#include <thread>

#include "peripheral.h"

CPU cpu;
VIA via({{15,1},{14,1},{13,0}}, 0x000F);
LCD lcd({10, 11, 12, {0,0,0,0,13,14,15,16}});

int main(){
    std::cout << "Initializing\n";

    cpu.reset();

    std::cout << "Ready\n";

    for(;;){
        // 1st: Update CPU
        Instruction::fetchInstruction(cpu);

        // 2nd: Update peripherals
        cpu.RAMListener();
        bool updateVIAChildren = via.process(cpu);

        // 3rd: Update other chips
        if(updateVIAChildren) lcd.updateLCD(via.pinoutSignals);

        // 4th: Check for peripherals answers
        if(cpu.expectsData) Instruction::fetchInstruction(cpu);

        //std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    std::cout << "Bye!";

}