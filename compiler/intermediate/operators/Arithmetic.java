package compiler.intermediate.operators;

import compiler.intermediate.expressions.Expression;
import compiler.lexer.Token;
import compiler.symbols.Type;

public class Arithmetic extends Operator{
    public Expression exp1, exp2;

    public Arithmetic(Token tok, Expression exp1, Expression exp2){
        super(tok, null);
        this.exp1 = exp1;
        this.exp2 = exp2;
        type = Type.typeConversion(exp1.type, exp2.type);
        if(type == null) error("Type conversion error");
    }

    public Expression generate(){
        return new Arithmetic(op, exp1.reduce(), exp2.reduce());
    }

    public String toString(){
        return exp1.toString() + " " + op.toString() + " " + exp2.toString();
    }
}
