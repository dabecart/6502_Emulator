package compiler.intermediate.statements;

import compiler.intermediate.expressions.Constant;
import compiler.intermediate.expressions.Expression;
import compiler.intermediate.operators.ArrayAccess;
import compiler.intermediate.three_address.Intermediate;
import compiler.intermediate.three_address.Label;
import compiler.symbols.Array;
import compiler.symbols.Type;

public class SetArray extends Statement{
    public ArrayAccess array;
    public Expression indexExpression;
    public Expression expression;

    public SetArray(ArrayAccess arrAcc, Expression exp){
        this.array = arrAcc;
        this.indexExpression = arrAcc.index;
        this.expression = exp;
        if(this.expression instanceof Constant){
            this.expression = exp.castToType(array.type);
        }
        if(checkType(arrAcc.type, exp.type) == null) error("Type error");
    }

    public Type checkType(Type type1, Type type2){
        if(type1 instanceof Array || type2 instanceof Array) return null;
        else if(type1 == type2) return type2;
        else if(Type.isNumber(type1) && Type.isNumber(type2)) return type2;
        else return null;
    }

    public void generate(Label beforeLabel, Label afterLabel){
        Expression s1 = indexExpression.reduce();
        Expression s2 = expression.reduce();
        if(s2.type != array.type) s2 = array.castExpression(s2);

        Intermediate.setArgs(s2, s1);
        Intermediate.setResult(array);
        Intermediate.setOperation('=');
        Intermediate.next();
        print(array.getName() + "[" + s1 + "] = " + s2);
    }

}
