package compiler.intermediate.statements;

public class Continue extends Statement{

    Statement statement;

    public Continue(){
        if(Statement.Enclosing == null){
            error("Continue outside loop");
        }
        statement = Statement.Enclosing;
    }

    public void generate(int beforeLabel, int afterLabel){
        print("goto L" + statement.savedBeforeLabel);
    }
    
}
