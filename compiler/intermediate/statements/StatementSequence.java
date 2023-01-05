package compiler.intermediate.statements;

public class StatementSequence extends Statement {
    Statement st1, st2;

    public StatementSequence(Statement st1, Statement st2){
        this.st1 = st1;
        this.st2 = st2;
    }

    public void generate(int beforeLabel, int afterLabel){
        if(st1 == Statement.Null) st2.generate(beforeLabel, afterLabel);
        else if (st2 == Statement.Null) st1.generate(beforeLabel, afterLabel);
        else{
            int label = newLabel();
            st1.generate(beforeLabel, label);
            printLabel(label);
            st2.generate(label, afterLabel);
        }
    }
}
