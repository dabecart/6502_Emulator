package compiler.intermediate.statements;

import compiler.intermediate.expressions.Expression;
import compiler.lexer.Tag;
import compiler.symbols.Type;

public class DoWhile extends Statement{
    Expression expression;
    Statement statement;

    public DoWhile() {
        this.parentingFunctionCall = Tag.DO;
        this.expression = null;
        this.statement = null;
    }

    public void start(Expression exp, Statement st){
        this.expression = exp;
        this.statement = st;
        if(expression.type != Type.Bool){
            expression.error("Statement is not boolean inside DOWHILE");
        }
    }

    public void generate(int beforeLabel, int afterLabel){
        this.savedBeforeLabel = beforeLabel;
        this.savedAfterLabel = afterLabel;
        int label = newLabel();
        statement.generate(beforeLabel, label);
        printLabel(label);
        expression.jump(beforeLabel, 0);
    }
}
