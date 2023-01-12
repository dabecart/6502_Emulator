package compiler.intermediate.statements;

import compiler.intermediate.three_address.Label;

public class Continue extends Statement{

    Statement statement;

    public Continue(){
        if(!Statement.Enclosing.isLoopStatement()){
            error("Continue outside loop");
        }
        this.statement = Statement.Enclosing;
    }

    public void generate(Label beforeLabel, Label afterLabel){
        gotoLabel(statement.continueLabel);
    }
    
}
