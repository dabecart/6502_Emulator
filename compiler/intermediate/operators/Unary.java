package compiler.intermediate.operators;

import compiler.intermediate.expressions.Expression;
import compiler.lexer.Token;
import compiler.symbols.Type;

public class Unary extends Operator {
    public Expression exp;

    public Unary(Token tok, Expression exp){
        super(tok, null);
        type = Type.typeConversion(Type.Int, exp.type);
        if(type == null) error("Type conversion error");
    }

    public Expression generate(){
        return new Unary(op, exp.reduce());
    }

    public String toString(){
        return op.toString() + " " + exp.toString();
    }
}
