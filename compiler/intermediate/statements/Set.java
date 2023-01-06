package compiler.intermediate.statements;

import compiler.intermediate.Id;
import compiler.intermediate.expressions.Expression;
import compiler.symbols.Type;

// Set implements an ID on the left side followed by an equal and an expression
// on the right.
public class Set extends Statement {
    public Id id;
    public Expression expression;

    public Set(Id i, Expression exp){
        this.id = i;
        this.expression = exp;
        if(checkType(id.type, exp.type) == null){
            error("Type error");
        }
    }

    public Type checkType(Type type1, Type type2){
        if(Type.isNumber(type1) && Type.isNumber(type2)) return type2;
        else if(type1 == Type.Bool && type2 == Type.Bool) return type2;
        else return null;
    }

    public void generate(int beforeLabel, int afterLabel){
        print(id.toString() + " = " + expression.generate().toString());
    }
}