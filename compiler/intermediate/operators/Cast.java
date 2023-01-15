package compiler.intermediate.operators;

import compiler.intermediate.expressions.Expression;
import compiler.lexer.Token;
import compiler.lexer.Word;
import compiler.symbols.Type;

public class Cast extends Unary{

    public Type castTo;

    public Cast(Token castedTo, Expression exp){
        super(castedTo, exp);
        if(castedTo instanceof Word){
            this.castTo = Type.getTypeByName(((Word) castedTo).lexeme);
            if(this.castTo == null) throw new Error("Cannot find type " + castedTo);
        }else throw new Error(castedTo + " is not a recognised Word class component");
    }

    public Cast(Type castedTo, Expression exp){
        super(castedTo, exp);
        this.castTo = castedTo;
    }

    public Expression generate(){
        return new Cast(op, exp.reduce());
    }

    public String toString(){
        return "(" + castTo.lexeme + ") " + exp.toString();
    }
    
}
