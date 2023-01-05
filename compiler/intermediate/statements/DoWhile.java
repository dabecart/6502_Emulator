package compiler.intermediate.statements;

import compiler.intermediate.expressions.Expression;
import compiler.symbols.Type;

public class DoWhile extends Statement{
    Expression expression;
    Statement statement;

    public DoWhile() {
        expression = null;
        statement = null;
    }

    public void start(Expression exp, Statement st){
        this.expression = exp;
        this.statement = st;
        if(expression.type != Type.Bool){
            expression.error("Statement is not boolean inside DOWHILE");
        }
    }

    public void generate(int beforeLabel, int afterLabel){
        savedAfterLabel = afterLabel;
        int label = newLabel();
        statement.generate(beforeLabel, label);
        printLabel(label);
        expression.jump(beforeLabel, 0);
    }
}
