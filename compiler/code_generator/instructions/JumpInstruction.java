package compiler.code_generator.instructions;

import compiler.code_generator.SystemOperators;
import compiler.intermediate.three_address.Quadruple;

public class JumpInstruction extends Quadruple{
    public JumpInstruction(Quadruple q){
        super(q);
        if(q != null && !checkOperator(this.op)){
            throw new Error("Cannot create Jump Instruction from operator " + this.op);
        }
    }

    public static boolean checkOperator(int op){
        switch(op){
            case SystemOperators.GOTO:{
                return true;
            }

            default: return false;
        }
    }

    public String toString(){
        return getLabelString() + "goto " + this.result.toString();
    }
}
