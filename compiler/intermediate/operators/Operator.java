package compiler.intermediate.operators;

import compiler.intermediate.expressions.Expression;
import compiler.intermediate.expressions.TemporalExpression;
import compiler.intermediate.three_address.Intermediate;
import compiler.lexer.Token;
import compiler.symbols.Type;

public class Operator extends Expression{
    public Operator(Token tok, Type type){
        super(tok, type);
    }

    public Expression reduce(){
        Expression x = generate();
        TemporalExpression t = new TemporalExpression(type);
        Intermediate.setResult(t);
        Intermediate.next();
        print(t.toString() + " = " + x.toString());
        return t;
    }
}
