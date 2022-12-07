#include "constants.h"
#include "instruction.h"
#include <chrono>
#include <thread>

#include "peripheral.h"

CPU cpu;
VIA via({{15,1},{14,1},{13,0}}, 0x000F);
LCD lcd({10, 11, 12, {0,0,0,0,13,14,15,16}});

uint8_t keyConnections[8] = {2,3,4,5,6,7,8,9};
Keyboard keyboard(dynamic_cast<Chip*>(&via), keyConnections, VIA_CA1);

int main(){
    std::cout << "Initializing\n";

    cpu.addChild(&via);
    via.addChild(&lcd);

    cpu.reset();

    std::cout << "Ready\n";

    for(;;){
        if(cpu.cycleCounter > 3600){
            keyboard.pressKey('a');
        }
        cpu.run();
        std::this_thread::sleep_for(std::chrono::microseconds(1));
    }

    std::cout << "Bye!";

}