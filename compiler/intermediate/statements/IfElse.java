package compiler.intermediate.statements;

import compiler.intermediate.expressions.Expression;
import compiler.intermediate.three_address.Label;
import compiler.symbols.Type;

public class IfElse extends Statement {
    Expression expression;
    Statement statement1, statement2;

    public IfElse(Expression exp, Statement st1, Statement st2){
        this.expression = exp;
        this.statement1 = st1;
        this.statement2 = st2;
        if(expression.type != Type.Bool){
            expression.error("Statement is not boolean inside IFELSE");
        }
    }

    public void generate(Label beforeLabel, Label afterLabel){
        Label label1 = newLabel();
        Label label2 = newLabel();
        expression.jump(null, label2);
        
        printLabel(label1);
        statement1.generate(label1, afterLabel);
        gotoLabel(afterLabel);   // End of if statement, jump to end of else
        
        printLabel(label2);
        statement2.generate(label2, afterLabel);
    }
}
