package compiler.symbols.types;

import compiler.code_generator.MemoryCell;
import compiler.intermediate.expressions.Constant;
import compiler.intermediate.expressions.Expression;
import compiler.lexer.Num;
import compiler.symbols.Type;

public class CharType extends TypeFunctions{
    
    @Override
    public String setValue(MemoryCell value, MemoryCell to){
        String str ="lda " + value.address + "\n" +
                    "sta " + to.address + "\n";
        return str;
    }

    @Override
    public MemoryCell castToType(MemoryCell castedCell, Type toType){
        if(toType == Type.Int){
            
        }
        return null;
    }

}
