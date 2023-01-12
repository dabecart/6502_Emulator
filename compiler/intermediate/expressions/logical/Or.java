package compiler.intermediate.expressions.logical;

import compiler.intermediate.expressions.Expression;
import compiler.intermediate.three_address.Label;
import compiler.lexer.Token;

public class Or extends Logical {
    public Or(Token tok, Expression exp1, Expression exp2){
        super(tok, exp1, exp2);
    }

    public void jump(Label trueLabel, Label falseLabel){
        Label label = trueLabel!=null ? trueLabel : newLabel();
        exp1.jump(label, null);
        exp2.jump(trueLabel, falseLabel);

        if(trueLabel == null) printLabel(label);
    }
}
