package compiler.code_generator.instructions;

import compiler.code_generator.Generator;
import compiler.code_generator.MemoryCell;
import compiler.code_generator.SystemOperators;
import compiler.intermediate.expressions.Constant;
import compiler.intermediate.operators.ArrayAccess;
import compiler.intermediate.three_address.Quadruple;

public class AssignInstruction extends Quadruple{

    // Handles both assign and indexed asign (with array) assignments.
    public AssignInstruction(Quadruple q) {
        super(q);
        if(q != null && !checkOperator(this.op)){
            throw new Error("Cannot create Assign Instruction from operator " + this.op);
        }
    }

    public static boolean checkOperator(int op){
        switch(op){
            case SystemOperators.SET:{
                return true;
            }
            default: return false;
        }
    }

    public String toString(){
        String ret = getLabelString() +  result.getName();
        if(arg2 == null) return ret + " = " + arg1.toString();
        else{
            if(result instanceof ArrayAccess) return  ret + "[" + arg2.toString() + "] = " + arg1.toString();
            else return ret + " = " + arg1.toString() + "[" + arg2.toString() + "]";
        } 
    }

    public String generate(Generator gen){
        if(result instanceof ArrayAccess){
            MemoryCell to = gen.getMemoryCell(result.getName() + "[0]");
            MemoryCell index = gen.getMemoryCell(arg2.getName());
            if(arg1 instanceof Constant){
                return arg1.type.functions.setArray((Constant) arg1, to, index);
            }else{
                MemoryCell from = gen.getMemoryCell(arg1.getName());
                return arg1.type.functions.setArray(from,to, index);
            }
        } // else...

        MemoryCell to = gen.getMemoryCell(result.getName());
        if(arg1 instanceof Constant){
            return arg1.type.functions.setValue((Constant) arg1, to);
        }else{
            if(arg1 instanceof ArrayAccess){
                MemoryCell from = gen.getMemoryCell(arg1.getName() + "[0]");
                MemoryCell index = gen.getMemoryCell(arg2.getName());
                return arg1.type.functions.setValue(from,index,to);
            } // else...
            MemoryCell from = gen.getMemoryCell(arg1.getName());
            return arg1.type.functions.setValue(from,to);
        }
    }
}
