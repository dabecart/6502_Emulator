package compiler.intermediate.expressions.logical;

import compiler.intermediate.expressions.Expression;
import compiler.lexer.Token;

public class And extends Logical {
    public And(Token tok, Expression exp1, Expression exp2){
        super(tok, exp1, exp2);
    }

    public void jump(int trueLabel, int falseLabel){
        int label = falseLabel!=0 ? falseLabel : newLabel();
        exp1.jump(0, label);
        exp2.jump(trueLabel, falseLabel);

        if(falseLabel == 0) printLabel(label);
    }
}
