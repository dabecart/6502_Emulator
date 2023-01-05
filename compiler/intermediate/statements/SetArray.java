package compiler.intermediate.statements;

import compiler.intermediate.Id;
import compiler.intermediate.expressions.Expression;
import compiler.intermediate.operators.ArrayAccess;
import compiler.symbols.Array;
import compiler.symbols.Type;

public class SetArray extends Statement{
    public Id array;
    public Expression indexExpression;
    public Expression expression;

    public SetArray(ArrayAccess arrAcc, Expression exp){
        this.array = arrAcc.array;
        this.indexExpression = arrAcc.index;
        this.expression = exp;
        if(checkType(arrAcc.type, exp.type) == null) error("Type error");
    }

    public Type checkType(Type type1, Type type2){
        if(type1 instanceof Array || type2 instanceof Array) return null;
        else if(type1 == type2) return type2;
        else if(Type.isNumber(type1) && Type.isNumber(type2)) return type2;
        else return null;
    }

    public void generate(int beforeLabel, int afterLabel){
        String s1 = indexExpression.reduce().toString();
        String s2 = expression.reduce().toString();
        print(array.toString() + " [" + s1 + "] = " + s2);
    }

}
