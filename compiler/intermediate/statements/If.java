package compiler.intermediate.statements;

import compiler.intermediate.expressions.Expression;
import compiler.intermediate.three_address.Label;
import compiler.symbols.Type;

public class If extends Statement {
    Expression expression;
    Statement statement;

    public If(Expression exp, Statement st){
        this.expression = exp;
        this.statement = st;
        if(expression.type != Type.Bool){
            expression.error("Statement is not boolean inside IF");
        }
    }

    public void generate(Label beforeLabel, Label afterLabel){
        Label label = newLabel();
        expression.jump(null, afterLabel);
        printLabel(label);
        statement.generate(label, afterLabel);
    }
}
