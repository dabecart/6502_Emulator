package compiler.code_generator;

import compiler.symbols.Type;

public class MemoryCell {
    
    public Type type;
    public int address;
    public byte value;

    public MemoryCell(Type type, int address){
        this.type = type;
        this.address = address;
    }


}
