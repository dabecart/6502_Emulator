package compiler.intermediate.expressions;

import compiler.intermediate.three_address.Label;
import compiler.lexer.Num;
import compiler.lexer.Token;
import compiler.lexer.Word;
import compiler.symbols.Type;

public class Constant extends Expression {

    public static final Constant 
        True = new Constant(Word.True, Type.Bool),
        False = new Constant(Word.False, Type.Bool),
        ONE = new Constant(1);

    public Constant(Token tok, Type type){
        super(tok, type);
    }

    public Constant(int num){
        super(new Num(num), Type.Int);
    }

    public void jump(Label trueLabel, Label falseLabel){
        if(this == True && trueLabel != null) gotoLabel(trueLabel);
        else if(this == False && falseLabel != null) gotoLabel(falseLabel);
    }

}
