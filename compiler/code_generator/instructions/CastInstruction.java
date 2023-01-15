package compiler.code_generator.instructions;

import compiler.code_generator.SystemOperators;
import compiler.intermediate.three_address.Quadruple;

public class CastInstruction extends Quadruple{
    
    public CastInstruction(Quadruple q){
        super(q);
        if(q != null && !checkOperator(this.op)){
            throw new Error("Cannot create Cast Instruction from operator " + this.op);
        }
    }

    public static boolean checkOperator(int op){
        switch(op){
            case SystemOperators.CAST:{
                return true;
            }
            default: return false;
        }
    }

    public String toString(){
        return getLabelString() + result.getName() + " = (" + result.type + ") " + arg1.toString();
    }

}
