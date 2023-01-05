package compiler.intermediate.statements;

import compiler.intermediate.expressions.Expression;
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

    public void generate(int beforeLabel, int afterLabel){
        int label1 = newLabel();
        int label2 = newLabel();
        expression.jump(0, label2);
        
        printLabel(label1);
        statement1.generate(label1, afterLabel);
        print("goto L" + afterLabel);   // End of if statement, jump to end of else
        
        printLabel(label2);
        statement2.generate(label2, afterLabel);
    }
}
