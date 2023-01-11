package compiler.intermediate.statements;

import compiler.intermediate.expressions.Constant;
import compiler.lexer.Tag;

public class Case extends Statement{
    
    Constant constant;
    Statement statement;
    Switch parentSwitch;

    boolean isDefaultCase = false;

    public Case(Constant constant, Statement statement){
        this.constant = constant;
        this.statement = statement;
        if(Statement.Enclosing.parentingFunctionCall != Tag.SWITCH){
            error("Case outside loop");
        }
        this.parentSwitch = (Switch) Statement.Enclosing;
    }

    // Construct for the default keyword
    public Case(Statement statement){
        this.isDefaultCase = true;
        this.constant = null;
        this.statement = statement;
        if(Statement.Enclosing.parentingFunctionCall != Tag.SWITCH){
            error("Case outside loop");
        }
        this.parentSwitch = (Switch) Statement.Enclosing;
    }

    public void generate(int beforeLabel, int afterLabel){
        parentSwitch.addCase(constant);
        statement.generate(beforeLabel, afterLabel);
        gotoLabel(afterLabel); // Exit the switch
    }

}
