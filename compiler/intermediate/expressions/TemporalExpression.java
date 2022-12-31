package compiler.intermediate.expressions;

import compiler.lexer.Word;
import compiler.symbols.Type;

public class TemporalExpression extends Expression{
    static int count = 0;
    int number = 0;

    public TemporalExpression(Type p){
        super(Word.Temp, p);
        number = count++;
    }

    public String toString(){
        return "t" + number;
    }
}
