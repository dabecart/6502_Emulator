package compiler.intermediate.expressions.logical;

import compiler.intermediate.expressions.Expression;
import compiler.intermediate.three_address.Label;
import compiler.lexer.Token;

public class Not extends Logical {
    public Not(Token tok, Expression exp){
        super(tok, exp, exp);
    }

    public void jump(Label trueLabel, Label falseLabel){
        exp2.jump(falseLabel, trueLabel);
    }

    public String toString() {
        return op.toString() + " " + exp1.toString();
    }
}