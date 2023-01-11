package compiler.intermediate.three_address;

import java.util.ArrayList;
import java.util.List;

import compiler.intermediate.expressions.Expression;

public class Intermediate {

    public static List<Quadruple> intermediateCode = new ArrayList<>();
    public static Quadruple currentQuad = new Quadruple();

    public static void next(){
        intermediateCode.add(currentQuad);
        currentQuad = new Quadruple();
    }

    public static void add(Quadruple quad){
        intermediateCode.add(quad);
    }

    public static void setOperation(int op){
        if(currentQuad.op != -1) throw new Error("Operation already set");
        currentQuad.op = op; 
    }

    public static void setLabel(int label){
        currentQuad.addLabel(label);
    }

    public static void setArgs(Expression arg1, Expression arg2){
        if(arg1 != null){
            if(currentQuad.arg1 != null) throw new Error("First argument already set");
            currentQuad.arg1 = arg1;
        }
        if(arg2 != null){
            if(currentQuad.arg2 != null) throw new Error("Second argument already set");
            currentQuad.arg2 = arg2;
        }
    }

    public static void setResult(Expression result){
        if(currentQuad.result != null) throw new Error("Result is already set");
        currentQuad.result = result;
    }

    public static void printAll(){
        System.out.println("\n*******************************************************");
        for(Quadruple q : intermediateCode){
            if(q.label != null){
                for(int i : q.label){
                    System.out.print("L" + i + ":");
                }
            }
            System.out.print("\t");

            if(q.arg1 != null)System.out.print(q.arg1.toString());
            if(q.arg2 != null) System.out.print(", " + q.arg2.toString());
            System.out.print(" -(");
            if(q.op < 255) System.out.print((char) q.op);
            else System.out.print(q.op);
            System.out.println(")- " + q.result.toString());
        }
    }
    
}
