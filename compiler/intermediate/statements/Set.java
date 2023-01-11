package compiler.intermediate.statements;

import compiler.intermediate.Id;
import compiler.intermediate.expressions.Expression;
import compiler.intermediate.three_address.Intermediate;
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
        Expression red_exp = expression.generate();
        print(id.toString() + " = " + red_exp.toString());
        Intermediate.setResult(id);
        // If no operation is done, then its a copy sentence.
        if(Intermediate.currentQuad.op == -1){
            Intermediate.setOperation('=');
            Intermediate.setArgs(red_exp, null);
        }
        Intermediate.next();
    }
}
