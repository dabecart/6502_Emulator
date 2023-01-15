package compiler.code_generator.instructions;

import compiler.code_generator.Generator;
import compiler.code_generator.MemoryCell;
import compiler.code_generator.SystemOperators;
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
        MemoryCell from = gen.getMemoryCell(arg1.getName());
        MemoryCell to = gen.getMemoryCell(result.getName());
        return "PEPE"; //TODO: HERE I LEFT!
    }
}
