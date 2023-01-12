package compiler.intermediate.statements;

import compiler.intermediate.three_address.Label;

public class StatementBundle extends Statement {
    Statement st1, st2;

    public StatementBundle(Statement st1, Statement st2){
        this.st1 = st1;
        this.st2 = st2;
    }

    public void generate(Label beforeLabel, Label afterLabel){
        if(this == Null) return;
        
        if(st1 == Statement.Null) st2.generate(beforeLabel, afterLabel);
        else if (st2 == Statement.Null) st1.generate(beforeLabel, afterLabel);
        else{
            Label label = newLabel();
            st1.generate(beforeLabel, label);
            printLabel(label);
            st2.generate(label, afterLabel);
        }
    }

    public static final StatementBundle Null 
    = new StatementBundle(Statement.Null, Statement.Null);
}
