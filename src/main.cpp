#include "constants.h"
#include "instruction.h"
#include <chrono>
#include <thread>

CPU cpu;

int main(){
    std::cout << "Initializing\n";

    cpu.reset();

    std::cout << "Ready\n";

    for(;;){
        cpu.clearBus();

        // 1st: Update CPU
        Instruction::fetchInstruction(cpu);

        // 2nd: Update peripherals

        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }

    std::cout << "Bye!";

}