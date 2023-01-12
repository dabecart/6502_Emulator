package compiler.intermediate.statements;

import java.util.Hashtable;
import java.util.Set;
import java.util.Map.Entry;

import compiler.code_generator.SystemOperators;
import compiler.intermediate.expressions.Constant;
import compiler.intermediate.expressions.Expression;
import compiler.intermediate.expressions.TemporalExpression;
import compiler.intermediate.three_address.Intermediate;
import compiler.intermediate.three_address.Label;
import compiler.intermediate.three_address.Quadruple;
import compiler.lexer.Tag;
import compiler.symbols.Type;

public class Switch extends Statement{
    Expression expression;
    Statement statement;

    Label defaultCase;

    public Hashtable<Constant, Label> cases = new Hashtable<>();

    public Switch() {
        this.parentingFunctionCall = Tag.SWITCH;
        this.expression = null;
        this.statement = null;
    }

    public void start(Expression exp, Statement st){
        this.expression = exp;
        this.statement = st;
    }

    public void generate(Label beforeLabel, Label afterLabel){
        this.breakLabel = afterLabel;

        Label testLabel = newLabel();

        Expression switchVariable = expression.reduce();
        gotoLabel(testLabel); // Go to test condition area
        statement.generate(beforeLabel, afterLabel);
        printLabel(testLabel);
        
        Set<Entry<Constant, Label>> entries = cases.entrySet();
        for(Entry<Constant, Label> entry : entries){
            TemporalExpression t = new TemporalExpression(Type.Bool);
            Quadruple q1 = new Quadruple(SystemOperators.EQ, switchVariable, entry.getKey(), t);
            Quadruple q2 = new Quadruple(SystemOperators.IF, t, null, entry.getValue());
            Intermediate.add(q1);
            Intermediate.add(q2);

            print("case " + switchVariable.toString() + " " + entry.getKey().toString() + " L" + entry.getValue());
        }
        if(defaultCase != null){
            gotoLabel(defaultCase);
        }
    }

    public void addCase(Constant c){
        Label label = newLabel();
        if(c != null){
            cases.put(c, label);
        }else{
            this.defaultCase = label;
        }
        printLabel(label);
    }
}
