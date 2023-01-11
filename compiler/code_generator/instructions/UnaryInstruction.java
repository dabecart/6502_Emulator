package compiler.code_generator.instructions;

import compiler.code_generator.SystemOperators;
import compiler.intermediate.three_address.Quadruple;

public class UnaryInstruction extends Quadruple{

    public UnaryInstruction(Quadruple q){
        super(q);
        if(q != null && !checkOperator(this.op)){
            throw new Error("Cannot create Unary Instruction from operator " + this.op);
        }
    }

    public static boolean checkOperator(int op){
        switch(op){
            case SystemOperators.NEG:
            case SystemOperators.NOT:{
                return true;
            }

            default: return false;
        }
    }
    
}
