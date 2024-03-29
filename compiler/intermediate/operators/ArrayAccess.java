package compiler.intermediate.operators;

import compiler.intermediate.Id;
import compiler.intermediate.expressions.Expression;
import compiler.intermediate.three_address.Intermediate;
import compiler.intermediate.three_address.Label;
import compiler.lexer.Tag;
import compiler.lexer.Word;
import compiler.symbols.Array;
import compiler.symbols.Type;

public class ArrayAccess extends Operator {

    public Id array;
    public Expression index;

    public ArrayAccess(Id a, Expression exp, Type type){
        super(new Word("[]", Tag.INDEX), null);
        if(type instanceof Array) this.type = ((Array)type).of;
        else this.type = type; 
        array = a;
        index = exp;
    }

    public Expression generate(){
        Expression indexReduced = index.reduce();
        Intermediate.setArgs(array, indexReduced);
        return new ArrayAccess(array, indexReduced, type);
    }

    public void jump(Label trueLabel, Label falseLabel){
        printJump(reduce().toString(), trueLabel, falseLabel);
    }

    public String getName(){
        return array.toString();
    }

    public String toString(){
        return array.toString() + "[" + index.toString() + "]";
    }

}
