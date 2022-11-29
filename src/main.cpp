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
        // 1st: Update CPU
        Instruction::fetchInstruction(cpu);

        // 2nd: Update peripherals

        //std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    std::cout << "Bye!";

}