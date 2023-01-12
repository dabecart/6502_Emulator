package compiler.intermediate.statements;

import compiler.intermediate.three_address.Label;
import compiler.lexer.Tag;

public class Break extends Statement{

    Statement statement;

    public Break(){
        // Break must be either inside a loop or a switch statement.
        boolean isBreakCorrect = Statement.Enclosing.isLoopStatement() || 
                                 Statement.Enclosing.parentingFunctionCall == Tag.SWITCH;
        if(!isBreakCorrect){
            error("Break is unenclosed");
        }
        this.statement = Statement.Enclosing;
    }

    public void generate(Label beforeLabel, Label afterLabel){
        gotoLabel(statement.breakLabel);
    }
    
}
