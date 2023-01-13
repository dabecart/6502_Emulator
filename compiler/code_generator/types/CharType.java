package compiler.code_generator.types;

import compiler.code_generator.MemoryCell;

public class CharType implements TypeInterface{
    
    public String setValue(MemoryCell value, MemoryCell to){
        String str ="lda " + value.address + "\n" +
                    "sta " + to.address + "\n";
        return str;
    }

}
