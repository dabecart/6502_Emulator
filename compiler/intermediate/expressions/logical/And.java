package compiler.intermediate.expressions.logical;

import compiler.intermediate.expressions.Expression;
import compiler.intermediate.three_address.Label;
import compiler.lexer.Token;

public class And extends Logical {
    public And(Token tok, Expression exp1, Expression exp2){
        super(tok, exp1, exp2);
    }

    public void jump(Label trueLabel, Label falseLabel){
        Label label = falseLabel!=null ? falseLabel : newLabel();
        exp1.jump(null, label);
        exp2.jump(trueLabel, falseLabel);

        if(falseLabel == null) printLabel(label);
    }
}
