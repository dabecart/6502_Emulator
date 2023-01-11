package compiler.intermediate.three_address;

import java.util.ArrayList;
import java.util.List;

import compiler.intermediate.expressions.Expression;

public class Quadruple {

    public int op = -1;
    public List<Label> label;   // Without labels
    public Expression arg1, arg2, result;

    public Quadruple(){
        this.label = new ArrayList<>();
    }

    public Quadruple(Quadruple q){
        this.op = q.op;
        this.label = q.label;
        this.arg1 = q.arg1;
        this.arg2 = q.arg2;
        this.result = q.result;
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
        if(label > 0 && !containsLabel(label)) this.label.add(new Label(label));
    }

    public boolean containsLabel(int label){
        for(Label lab : this.label){
            if(lab.label == label) return true;
        }
        return false;
    }

    public String getLabelString(){
        StringBuilder str = new StringBuilder();
        if(this.label != null){
            for(Label l : this.label) str.append(l + ":");
        }
        str.append("\t");
        return str.toString();
    }

    public String toString(){
        StringBuilder str = new StringBuilder();
        str.append(getLabelString());

        if(this.arg1 != null) str.append(this.arg1.toString());
        if(this.arg2 != null) str.append(", " + this.arg2.toString());
        str.append(" -(");
        if(this.op < 255) str.append((char) this.op);
        else str.append(this.op);
        str.append(")- " + this.result.toString());

        return str.toString();
    }

}
