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

    public String toString(){
        return op.toString();
    }
    
}
