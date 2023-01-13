package compiler.code_generator;

// Implementation of a realy basic 6502 CPU.
public class CPU {

    public byte a = 0, x = 0, y = 0;
    public byte[] stack = new byte[256];
    public byte stackPointer = (byte)255;
    
    public int memoryPointer = 0;

    public CPU(){}
    
}
