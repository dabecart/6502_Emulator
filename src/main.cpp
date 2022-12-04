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

    cpu.addChild(&via);
    via.addChild(&lcd);

    cpu.reset();

    std::cout << "Ready\n";

    for(;;){
        cpu.run();
        //std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    std::cout << "Bye!";

}