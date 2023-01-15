package compiler.symbols.types;

import compiler.code_generator.MemoryCell;

public class CharType extends TypeFunctions{
    
    @Override
    public String setValue(MemoryCell value, MemoryCell to){
        String str ="lda " + value.address + "\n" +
                    "sta " + to.address + "\n";
        return str;
    }

}
