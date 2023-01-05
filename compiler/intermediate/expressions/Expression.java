package compiler.intermediate.expressions;

import compiler.intermediate.Node;
import compiler.lexer.Token;
import compiler.symbols.Type;

public class Expression extends Node {
    public Token op;
    public Type type;

    public Expression(Token token, Type t){
        this.op = token;
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
            print("if " + testExpression + " goto L" + trueLabel);
            print("goto L" + falseLabel);
        }else if(trueLabel != 0){
            print("if " + testExpression + " goto L" + trueLabel);
        }else if(falseLabel != 0){
            print("ifnot " + testExpression + " goto L" + falseLabel);
        }
    }

    public String toString(){
        return op.toString();
    }
    
}
