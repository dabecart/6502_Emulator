package compiler.intermediate.operators;

import compiler.intermediate.expressions.Expression;
import compiler.intermediate.expressions.TemporalExpression;
import compiler.lexer.Token;
import compiler.symbols.Type;

public class Operator extends Expression{
    public Operator(Token tok, Type type){
        super(tok, type);
    }

    public Expression reduce(){
        Expression x = generate();
        TemporalExpression t = new TemporalExpression(type);
        print(t.toString() + " = " + x.toString());
        return t;
    }
}
