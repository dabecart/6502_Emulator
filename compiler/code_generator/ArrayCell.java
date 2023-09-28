package compiler.code_generator;

import compiler.symbols.Type;

public class ArrayCell extends MemoryCell{
    
    public int startAddress;
    public int arrayIndex;

    public ArrayCell(Type type, int address, int startAddress, int arrayIndex){
        super(type, address);
        this.startAddress = startAddress;
        this.arrayIndex = arrayIndex;
    }


}
