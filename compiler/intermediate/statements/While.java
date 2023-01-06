package compiler.intermediate.statements;

import compiler.intermediate.expressions.Expression;
import compiler.symbols.Type;

public class While extends Statement{
    Expression expression;
    Statement statement;

    public While() {
        expression = null;
        statement = null;
    }

    public void start(Expression exp, Statement st){
        this.expression = exp;
        this.statement = st;
        if(expression.type != Type.Bool){
            expression.error("Statement is not boolean inside WHILE");
        }
    }

    public void generate(int beforeLabel, int afterLabel){
        this.savedBeforeLabel = beforeLabel;
        this.savedAfterLabel = afterLabel;
        expression.jump(0, afterLabel);
        int label = newLabel();
        printLabel(label);
        statement.generate(beforeLabel, label);
        print("goto L" + beforeLabel);
    }
}
