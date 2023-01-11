package compiler.intermediate.expressions;

import compiler.code_generator.SystemOperators;
import compiler.intermediate.Node;
import compiler.intermediate.three_address.Intermediate;
import compiler.intermediate.three_address.Label;
import compiler.lexer.Token;
import compiler.symbols.Type;

public class Expression extends Node {
    public Token op;
    public Type type;

    public Expression(Token operation, Type t){
        this.op = operation;
        this.type = t;
    }

    public Expression generate(){
        return this;
    }

    public Expression reduce(){
        return this;
    }

    public void jump(int trueLabel, int falseLabel){
        printJump(toString(), trueLabel, falseLabel);
    }

    public void printJump(String testExpression, int trueLabel, int falseLabel){
        if(trueLabel != 0 && falseLabel != 0){
            Intermediate.setResult(new Label(trueLabel));
            Intermediate.setOperation(SystemOperators.IF);
            Intermediate.next();
            print("if " + testExpression + " goto L" + trueLabel);
            gotoLabel(falseLabel);
        }else if(trueLabel != 0){
            Intermediate.setResult(new Label(trueLabel));
            Intermediate.setOperation(SystemOperators.IF);
            Intermediate.next();
            print("if " + testExpression + " goto L" + trueLabel);
        }else if(falseLabel != 0){
            Intermediate.setResult(new Label(falseLabel));
            Intermediate.setOperation(SystemOperators.IFNOT);
            Intermediate.next();
            print("ifnot " + testExpression + " goto L" + falseLabel);
        }
    }

    public String toString(){
        return op.toString();
    }
    
}
