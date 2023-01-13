package compiler.intermediate.statements;

import compiler.intermediate.Id;
import compiler.intermediate.expressions.Expression;
import compiler.intermediate.operators.Arithmetic;
import compiler.intermediate.operators.ArrayAccess;
import compiler.intermediate.three_address.Label;
import compiler.lexer.Tag;
import compiler.lexer.Token;
import compiler.lexer.Word;
import compiler.symbols.Type;

// OperationAndSet implements an ID on the left side followed by an 
// +=, -=, *=, /= or %= and an expression on the right.
public class OperationAndSet extends Statement {
    public Expression id;
    public Token operator;
    public Expression expression;

    public OperationAndSet(Expression i, Token operator, Expression exp){
        this.id = i;
        if(!(i instanceof ArrayAccess) && !(i instanceof Id)){
            error("Cannot set this expression");
        }
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
        return  tag == Tag.ADDEQ || tag == Tag.SUBEQ || tag == Tag.MULTEQ || 
                tag == Tag.DIVEQ || tag == Tag.MODEQ || tag == Tag.ANDEQ  || 
                tag == Tag.OREQ  || tag == Tag.XOREQ;
    }

    public Type checkType(Type type1, Type type2){
        if(Type.isNumber(type1) && Type.isNumber(type2)) return type2;
        else if(type1 == Type.Bool && type2 == Type.Bool) return type2;
        else return null;
    }

    public void generate(Label beforeLabel, Label afterLabel){
        char operation = ((Word) operator).lexeme.charAt(0);
        Expression op = new Arithmetic(new Token(operation), id, expression);
        if(id instanceof Id){
            Set set = new Set((Id)id, op);
            set.generate(beforeLabel, afterLabel);
        }else if(id instanceof ArrayAccess){
            SetArray set = new SetArray((ArrayAccess)id, op);
            set.generate(beforeLabel, afterLabel);
        }

    }
}
