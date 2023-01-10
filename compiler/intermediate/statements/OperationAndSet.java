package compiler.intermediate.statements;

import compiler.intermediate.Id;
import compiler.intermediate.expressions.Expression;
import compiler.lexer.Tag;
import compiler.lexer.Token;
import compiler.symbols.Type;

// OperationAndSet implements an ID on the left side followed by an 
// +=, -=, *=, /= or %= and an expression on the right.
public class OperationAndSet extends Statement {
    public Id id;
    public Token operator;
    public Expression expression;

    public OperationAndSet(Id i, Token operator, Expression exp){
        this.id = i;
        this.operator = operator;
        this.expression = exp;
        if(checkType(id.type, exp.type) == null){
            error("Type error");
        }
    }

    public static boolean isOperatorAndEqual(Token tok){
        int tag = tok.tag;
        return tag == Tag.ADDEQ || tag == Tag.SUBEQ || tag == Tag.MULTEQ || tag == Tag.DIVEQ || tag == Tag.MODEQ;
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
