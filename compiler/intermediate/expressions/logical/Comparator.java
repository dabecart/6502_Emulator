package compiler.intermediate.expressions.logical;

import compiler.intermediate.expressions.Expression;
import compiler.intermediate.expressions.TemporalExpression;
import compiler.intermediate.three_address.Intermediate;
import compiler.intermediate.three_address.Label;
import compiler.intermediate.three_address.Quadruple;
import compiler.lexer.Token;
import compiler.symbols.Array;
import compiler.symbols.Type;

public class Comparator extends Logical {
    public Comparator(Token tok, Expression exp1, Expression exp2){
        super(tok, exp1, exp2);
    }

    public Type checkType(Type t1, Type t2){
        if(t1 instanceof Array || t2 instanceof Array) return null;
        else if(t1 == t2) return Type.Bool;
        else return null;
    }

    public void jump(Label trueLabel, Label falseLabel){
        Expression a = exp1.reduce();
        Expression b = exp2.reduce();

        TemporalExpression t = new TemporalExpression(Type.Bool);
        Quadruple quad = new Quadruple(op.tag, a, b, t);
        Intermediate.add(quad);

        Intermediate.setArgs(t, null);

        String testStr = a.toString() + " " + op.toString() + " " + b.toString();
        printJump(testStr, trueLabel, falseLabel);
    }
}