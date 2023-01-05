package compiler.intermediate.statements;

public class Break extends Statement{

    Statement statement;

    public Break(){
        if(Statement.Enclosing == null){
            error("Break outside loop");
        }
        statement = Statement.Enclosing;
    }

    public void generate(int beforeLabel, int afterLabel){
        print("goto L" + statement.savedAfterLabel);
    }
    
}
