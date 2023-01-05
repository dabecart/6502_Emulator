package compiler.intermediate.operators;

import compiler.intermediate.Id;
import compiler.intermediate.expressions.Expression;
import compiler.lexer.Tag;
import compiler.lexer.Word;
import compiler.symbols.Type;

public class ArrayAccess extends Operator {

    public Id array;
    public Expression index;

    public ArrayAccess(Id a, Expression exp, Type type){
        super(new Word("[]", Tag.INDEX), type);
        array = a;
        index = exp;
    }

    public Expression generate(){
        return new ArrayAccess(array, index.reduce(), type);
    }

    public void jump(int trueLabel, int falseLabel){
        printJump(reduce().toString(), trueLabel, falseLabel);
    }

    public String toString(){
        return array.toString() + "[" + index.toString() + "]";
    }

}
