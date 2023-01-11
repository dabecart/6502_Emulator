package compiler.intermediate.operators;

import compiler.intermediate.expressions.Expression;
import compiler.intermediate.three_address.Intermediate;
import compiler.lexer.Token;
import compiler.symbols.Type;

public class Arithmetic extends Operator{
    public Expression exp1, exp2;

    public Arithmetic(Token operation, Expression exp1, Expression exp2){
        super(operation, null);
        this.exp1 = exp1;
        this.exp2 = exp2;
        type = Type.typeConversion(exp1.type, exp2.type);
        if(type == null) error("Type conversion error");
    }

    public Expression generate(){
        Expression red1 = exp1.reduce();
        Expression red2 = exp2.reduce();
        Arithmetic arth = new Arithmetic(op, red1, red2);
        Intermediate.setArgs(red1, red2);
        Intermediate.setOperation(this.op.tag);
        return arth;
    }

    public String toString(){
        return exp1.toString() + " " + op.toString() + " " + exp2.toString();
    }
}
