package compiler.intermediate.expressions;

import compiler.code_generator.SystemOperators;
import compiler.intermediate.Node;
import compiler.intermediate.operators.Cast;
import compiler.intermediate.three_address.Intermediate;
import compiler.intermediate.three_address.Label;
import compiler.lexer.Tag;
import compiler.lexer.Token;
import compiler.symbols.Type;

public class Expression extends Node {
    public Token op;
    public Type type;

    public Expression(Token operation, Type t){
        this.op = operation;
        this.type = t;
    }

    public Expression castToType(Type toType){
        return null;
    }

    public Expression generate(){
        return this;
    }

    public Expression reduce(){
        return this;
    }

    public void jump(Label trueLabel, Label falseLabel){
        printJump(toString(), trueLabel, falseLabel);
    }

    public void printJump(String testExpression, Label trueLabel, Label falseLabel){
        if(trueLabel != null && falseLabel != null){
            Intermediate.setResult(trueLabel);
            Intermediate.setOperation(SystemOperators.IF);
            Intermediate.next();
            print("if " + testExpression + " goto " + trueLabel);
            gotoLabel(falseLabel);
        }else if(trueLabel != null){
            Intermediate.setResult(trueLabel);
            Intermediate.setOperation(SystemOperators.IF);
            Intermediate.next();
            print("if " + testExpression + " goto " + trueLabel);
        }else if(falseLabel !=  null){
            Intermediate.setResult(falseLabel);
            Intermediate.setOperation(SystemOperators.IFNOT);
            Intermediate.next();
            print("ifnot " + testExpression + " goto " + falseLabel);
        }
    }

    public String toString(){
        return op.toString();
    }

    public Expression castExpression(Expression exp){
        Cast c = new Cast(type, exp);   // Used only for the print
        TemporalExpression t = new TemporalExpression(type);
        Intermediate.setArgs(exp, null);
        Intermediate.setResult(t);
        Intermediate.setOperation(Tag.CAST);
        Intermediate.next();
        print(t.toString() + " = " + c.toString());
        return t;
    }
    
}
