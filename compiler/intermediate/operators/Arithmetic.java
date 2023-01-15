package compiler.intermediate.operators;

import compiler.intermediate.expressions.Constant;
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

        if(exp1 instanceof Constant){
            this.exp1 = exp1.castToType(exp2.type);
        } else if(exp2 instanceof Constant){
            this.exp2 = exp2.castToType(exp1.type);
        }

        this.type = Type.typeConversion(this.exp1.type, this.exp2.type);
        if(type == null) error("Type conversion error");
    }

    public Expression generate(){
        Expression red1 = exp1.reduce();
        if(red1.type != this.type) red1 = castExpression(red1);

        Expression red2 = exp2.reduce();
        if(red2.type != this.type) red2 = castExpression(red2);

        Arithmetic arth = new Arithmetic(op, red1, red2);
        Intermediate.setArgs(red1, red2);
        Intermediate.setOperation(this.op.tag);
        return arth;
    }

    public String toString(){
        return exp1.toString() + " " + op.toString() + " " + exp2.toString();
    }
}
