package compiler.intermediate.statements;

public class Continue extends Statement{

    Statement statement;

    public Continue(){
        if(!Statement.Enclosing.isLoopStatement()){
            error("Continue outside loop");
        }
        this.statement = Statement.Enclosing;
    }

    public void generate(int beforeLabel, int afterLabel){
        print("goto L" + statement.continueLabel);
    }
    
}
