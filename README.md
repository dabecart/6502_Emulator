# 6502_Emulator

The 6502 Emulator consists on both emulator and compiler (work in progress) for my own 6502 PC.

The emulator works with all the most used instructions of the 6502. It keeps count of the PC, flag bits, address and data bus, registers and stack pointer. It also simulates some of the most common
peripherals, like the 6522 VIA (Versatile Interface Adapter), ROM and RAM chips, as well as a PS/2 keyboard. For more info on those, take a look at Ben Eater's Youtube channel, most of the inspiration 
came from him. 

All the emulator files are inside `src`.

The compiler part of this project is still on the works. At the moment of writing this, it reads basic C++ code (loops, arrays, integer, byte and boolean variables, arythmetic and binary operations) and compiles 
it to **intermediary** code. The final assembly code is a WIP.

I highly recommend taking a read of _Aho's Compiler Guidebook_ to understand what is happenning under the hood.

All the compiler files are inside `compiler`.

This work is still in alpha, so use it with a pinch of salt.
