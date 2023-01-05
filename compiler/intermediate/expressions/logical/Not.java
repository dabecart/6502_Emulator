package compiler.intermediate.expressions.logical;

import compiler.intermediate.expressions.Expression;
import compiler.lexer.Token;

public class Not extends Logical {
    public Not(Token tok, Expression exp){
        super(tok, exp, exp);
    }

    public void jump(int trueLabel, int falseLabel){
        exp2.jump(falseLabel, trueLabel);
    }

    public String toString() {
        return op.toString() + " " + exp1.toString();
    }
}