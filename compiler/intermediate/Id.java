package compiler.intermediate;

import compiler.intermediate.expressions.Expression;
import compiler.lexer.Word;
import compiler.symbols.Type;

public class Id extends Expression{
    public int offset;  // Relative address for the variable

    public Id(Word id, Type type, int offset){
        super(id, type);
        this.offset = offset;
    }
}
