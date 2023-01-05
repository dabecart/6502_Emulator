package compiler.intermediate.expressions.logical;

import compiler.intermediate.expressions.Expression;
import compiler.lexer.Token;

public class Or extends Logical {
    public Or(Token tok, Expression exp1, Expression exp2){
        super(tok, exp1, exp2);
    }

    public void jump(int trueLabel, int falseLabel){
        int label = trueLabel!=0 ? trueLabel : newLabel();
        exp1.jump(label, 0);
        exp2.jump(trueLabel, falseLabel);

        if(trueLabel == 0) printLabel(label);
    }
}
