package compiler.code_generator.instructions;

import compiler.code_generator.SystemOperators;
import compiler.intermediate.three_address.Quadruple;

public class OperateInstruction extends Quadruple{

    public OperateInstruction(Quadruple q){
        super(q);
        if(q != null && !checkOperator(this.op)){
            throw new Error("Cannot create Operate Instruction from operator " + this.op);
        }
    }

    public static boolean checkOperator(int op){
        switch(op){
            case SystemOperators.ADD:
            case SystemOperators.SUB:
            case SystemOperators.MUL:
            case SystemOperators.DIV:
            case SystemOperators.MOD:

            case SystemOperators.AND:
            case SystemOperators.OR:
            case SystemOperators.XOR:
            
            case SystemOperators.lAND:
            case SystemOperators.lOR:
            case SystemOperators.EQ:
            case SystemOperators.NEQ:
            case SystemOperators.GEQ:
            case SystemOperators.LEQ:
            case SystemOperators.LES:
            case SystemOperators.GRE:{
                return true;
            }

            default: return false;
        }
    }
    
    public String getOperatorString(){
        switch(op){
            case SystemOperators.ADD:
            case SystemOperators.SUB:
            case SystemOperators.MUL:
            case SystemOperators.DIV:
            case SystemOperators.MOD:
            case SystemOperators.AND:
            case SystemOperators.OR:
            case SystemOperators.XOR:
            case SystemOperators.LES:
            case SystemOperators.GRE: {
                return Character.toString((char) op);
            }
            
            case SystemOperators.lAND:{
                return "&&";
            }
            case SystemOperators.lOR:{
                return "||";
            }
            case SystemOperators.EQ:{
                return "==";
            }
            case SystemOperators.NEQ:{
                return"!=";
            }
            case SystemOperators.GEQ:{
                return ">=";
            }
            case SystemOperators.LEQ:{
                return "<=";
            }

            default: return "";
        }
    }

    public String toString(){
        return getLabelString() + result.toString() + " = " + arg1.toString() + " " + getOperatorString() + " " + arg2.toString();
    }

}
