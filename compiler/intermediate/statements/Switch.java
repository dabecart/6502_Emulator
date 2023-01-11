package compiler.intermediate.statements;

import java.util.Hashtable;
import java.util.Set;
import java.util.Map.Entry;

import compiler.intermediate.expressions.Constant;
import compiler.intermediate.expressions.Expression;
import compiler.lexer.Tag;

public class Switch extends Statement{
    Expression expression;
    Statement statement;

    int defaultCase = 0;

    public Hashtable<Constant, Integer> cases = new Hashtable<>();

    public Switch() {
        this.parentingFunctionCall = Tag.SWITCH;
        this.expression = null;
        this.statement = null;
    }

    public void start(Expression exp, Statement st){
        this.expression = exp;
        this.statement = st;
    }

    public void generate(int beforeLabel, int afterLabel){
        this.breakLabel = afterLabel;

        int testLabel = newLabel();
        String switchVariable = expression.reduce().toString();
        gotoLabel(testLabel); // Go to test condition area
        statement.generate(beforeLabel, afterLabel);
        printLabel(testLabel);
        
        Set<Entry<Constant, Integer>> entries = cases.entrySet();
        for(Entry<Constant, Integer> entry : entries){
            print("case " + switchVariable + " " + entry.getKey().toString() + " L" + entry.getValue());
        }
        if(defaultCase != 0){
            gotoLabel(defaultCase);
        }
    }

    public void addCase(Constant c){
        int label = newLabel();
        if(c != null){
            cases.put(c, label);
        }else{
            this.defaultCase = label;
        }
        printLabel(label);
    }
}
