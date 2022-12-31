package compiler.intermediate.expressions;

import compiler.lexer.Num;
import compiler.lexer.Token;
import compiler.lexer.Word;
import compiler.symbols.Type;

public class Constant extends Expression {
    public Constant(Token tok, Type type){
        super(tok, type);
    }

    public Constant(int num){
        super(new Num(num), Type.Int);
    }

    public static final Constant 
        True = new Constant(Word.True, Type.Bool),
        False = new Constant(Word.False, Type.Bool);

    
}
