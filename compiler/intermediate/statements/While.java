package compiler.intermediate.statements;

import compiler.intermediate.expressions.Expression;
import compiler.intermediate.three_address.Label;
import compiler.lexer.Tag;
import compiler.symbols.Type;

public class While extends Statement{
    Expression expression;
    Statement statement;

    public While() {
        expression = null;
        statement = null;
    }

    public void start(Expression exp, Statement st){
        this.parentingFunctionCall = Tag.FOR;
        this.expression = exp;
        this.statement = st;
        if(expression.type != Type.Bool){
            expression.error("Statement is not boolean inside WHILE");
        }
    }

    public void generate(Label beforeLabel, Label afterLabel){
        this.continueLabel = beforeLabel;
        this.breakLabel = afterLabel;
        expression.jump(null, afterLabel);
        Label label = newLabel();
        printLabel(label);
        statement.generate(beforeLabel, label);
        gotoLabel(beforeLabel);
    }
}
