package compiler.symbols.types;

import compiler.code_generator.MemoryCell;
import compiler.intermediate.expressions.Constant;

public class CharType extends TypeFunctions{
    
    @Override
    public String setValue(MemoryCell value, MemoryCell to){
        String str ="lda " + value.address + "\n" +
                    "sta " + to.address + "\n";
        return str;
    }

    @Override
    public String setValue(MemoryCell value, MemoryCell access, MemoryCell to){
        String str ="ldx " + access.address + "\n" +
                    "lda " + value.address + ",x\n" +
                    "sta " + to.address + "\n";
        return str;
    }

    @Override
    public String setValue(Constant value, MemoryCell to){
        String str ="lda #" + value.op.tag + "\n" +
                    "sta " + to.address + "\n";
        return str;
    }

    @Override
    public String setArray(MemoryCell value, MemoryCell to, MemoryCell access){
        String str ="lda " + value.address + "\n" +
                    "ldx " + access.address + "\n" + 
                    "sta " + to.address + ",x\n";
        return str;
    }

    @Override
    public String setArray(Constant value, MemoryCell to, MemoryCell access){
        String str ="lda #" + value.op.tag + "\n" +
                    "ldx " + access.address + "\n" + 
                    "sta " + to.address + ",x\n";
        return str;
    }

}
