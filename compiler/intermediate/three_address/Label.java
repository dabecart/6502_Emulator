package compiler.intermediate.three_address;

import compiler.intermediate.expressions.Expression;
import compiler.lexer.Token;
import compiler.symbols.Type;

public class Label extends Expression{
    
    public int label;

    public Label(int label){
        super(new Token(label), Type.Label);
        this.label = label;
    }

    public String toString(){
        return "L" + label;
    }

}
