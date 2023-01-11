package compiler.code_generator.instructions;

import compiler.code_generator.SystemOperators;
import compiler.intermediate.three_address.Quadruple;

public class ConditionalInstruction extends Quadruple{
    public ConditionalInstruction(Quadruple q) {
        super(q);
        if(q != null && !checkOperator(this.op)){
            throw new Error("Cannot create Conditional Instruction from operator " + this.op);
        }
    }

    public static boolean checkOperator(int op){
        switch(op){
            case SystemOperators.IF:
            case SystemOperators.IFNOT:{
                return true;
            }

            default: return false;
        }
    }

    public String toString(){
        return getLabelString() + 
    }
}
