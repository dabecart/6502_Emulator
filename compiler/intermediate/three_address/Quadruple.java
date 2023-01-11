package compiler.intermediate.three_address;

import java.util.ArrayList;
import java.util.List;

import compiler.intermediate.expressions.Expression;

public class Quadruple {

    public int op = -1;
    public List<Integer> label;   // Without labels
    public Expression arg1, arg2, result;

    public Quadruple(){
        this.label = new ArrayList<>();
    }

    public Quadruple(int label, int op, Expression arg1, Expression arg2, Expression result){
        addLabel(label);
        this.op = op;
        this.arg1 = arg1;
        this.arg2 = arg2;
        this.result = result;
    }

    public Quadruple(int op, Expression arg1, Expression arg2, Expression result){
        this.op = op;
        this.arg1 = arg1;
        this.arg2 = arg2;
        this.result = result;
    }

    public void addLabel(int label){
        if(this.label == null) this.label = new ArrayList<>();
        if(label > 0 && !this.label.contains(label)) this.label.add(label);
    }

}
