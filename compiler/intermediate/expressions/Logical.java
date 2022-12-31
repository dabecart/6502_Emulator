package compiler.intermediate.expressions;

import compiler.lexer.Token;
import compiler.symbols.Type;

public class Logical extends Expression {
    public Expression exp1, exp2;

    public Logical(Token tok, Expression exp1, Expression exp2){
        super(tok, null);
        this.exp1 = exp1;
        this.exp2 = exp2;

        Type type = null;
        if(exp1.type == Type.Bool && exp2.type == Type.Bool) type = Type.Bool;
        if(type == null) error("Type conversion error");
    }

    public Expression generate(){
        return null;
    }
}
