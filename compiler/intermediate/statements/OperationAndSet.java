package compiler.intermediate.statements;

import compiler.intermediate.Id;
import compiler.intermediate.expressions.Constant;
import compiler.intermediate.expressions.Expression;
import compiler.intermediate.operators.Arithmetic;
import compiler.lexer.Tag;
import compiler.lexer.Token;
import compiler.lexer.Word;
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
        if(!isOperatorAndEqual()){
            error("Unrecognised operator " + operator.toString());
        }
        this.expression = exp;
        if(checkType(id.type, exp.type) == null){
            error("Type error");
        }
    }

    public boolean isOperatorAndEqual(){
        int tag = this.operator.tag;
        return tag == Tag.ADDEQ || tag == Tag.SUBEQ || tag == Tag.MULTEQ || tag == Tag.DIVEQ || tag == Tag.MODEQ;
    }

    public Type checkType(Type type1, Type type2){
        if(Type.isNumber(type1) && Type.isNumber(type2)) return type2;
        else if(type1 == Type.Bool && type2 == Type.Bool) return type2;
        else return null;
    }

    public void generate(int beforeLabel, int afterLabel){
        char operation = 0;
        switch(operator.tag){
            case Tag.ADDEQ: {operation = '+'; break;}
            case Tag.SUBEQ: {operation = '-'; break;}
            case Tag.MULTEQ:{operation = '*'; break;}
            case Tag.DIVEQ: {operation = '/'; break;}
            case Tag.MODEQ: {operation = '%'; break;}
        }
        Expression op = new Arithmetic(new Token(operation), id, expression);
        Set set = new Set(id, op);
        set.generate(beforeLabel, afterLabel);
    }
}
