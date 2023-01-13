package compiler.intermediate.expressions.logical;

import compiler.intermediate.expressions.Expression;
import compiler.intermediate.expressions.TemporalExpression;
import compiler.intermediate.three_address.Label;
import compiler.lexer.Token;
import compiler.symbols.Type;

public class Logical extends Expression {
    public Expression exp1, exp2;

    public Logical(Token tok, Expression exp1, Expression exp2){
        super(tok, null);
        this.exp1 = exp1;
        this.exp2 = exp2;

        this.type = checkType(exp1.type, exp2.type);
        if(type == null) error("Type conversion error. Cannot convert from " + exp1.type + " to " + exp2.type);
    }

    public Type checkType(Type t1, Type t2){
        if(t1 == Type.Bool && t2 == Type.Bool) return Type.Bool;
        else return null;
    }

    public Expression generate(){
        Label fromLabel = newLabel(), toLabel = newLabel();
        TemporalExpression branchOutput = new TemporalExpression(type);
        this.jump(null, fromLabel);
        print(branchOutput.toString() + " = true");
        gotoLabel(toLabel);
        
        printLabel(fromLabel);
        print(branchOutput.toString() + " = false");
        printLabel(toLabel);

        return branchOutput;
    }

    public String toString(){
        return exp1.toString() + " " + op.toString() + " " + exp2.toString();
    }
}
