package compiler.code_generator.instructions;

import compiler.code_generator.SystemOperators;
import compiler.intermediate.three_address.Quadruple;

public class AssignInstruction extends Quadruple{

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
        return getLabelString() +  result.toString() + " = " + arg1.toString();
    }
}
