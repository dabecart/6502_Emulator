package compiler.intermediate.statements;

import compiler.intermediate.expressions.Expression;
import compiler.lexer.Tag;
import compiler.symbols.Type;

public class For extends Statement{
    // First statement is always a declaration or an assign value.
    // StepStatement is the 3rd param, must be an assign value.
    // TODO: Add the posibility to call a function as 3rd param.
    Statement initialStatement, stepStatement;
    // 2nd param is the condition to keep the loop running.
    Expression condition;
    Statement innerStatement;

    public For(){
        this.initialStatement = null;
        this.condition = null;
        this.stepStatement = null;
        this.innerStatement = null;
        this.parentingFunctionCall = Tag.FOR;
    }

    public void start(Statement initSt, Expression condition, Statement stepStatement, Statement innerStatement){
        this.initialStatement = initSt;
        this.condition = condition;
        this.stepStatement = stepStatement;
        this.innerStatement = innerStatement;
        if(condition!=null && condition.type != Type.Bool){
            condition.error("Conditional statement (2nd arg) of FOR loop not boolean");
        }
    }

    public void generate(int beforeLabel, int afterLabel){
        initialStatement.generate(beforeLabel, afterLabel);
        int innerLoopLabel = newLabel();    // After first statement of for loop
        int continueLabel = newLabel();     // When continue is called, it executes the third param and goes to innerLoopLabel

        this.continueLabel = continueLabel;
        this.breakLabel = afterLabel;

        printLabel(innerLoopLabel);
        innerStatement.generate(innerLoopLabel, afterLabel);
        
        printLabel(continueLabel);
        stepStatement.generate(0, 0);

        if(condition == null) gotoLabel(innerLoopLabel);
        else condition.jump(innerLoopLabel, 0);
    }

}
