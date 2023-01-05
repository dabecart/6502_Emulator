package compiler.lexer;

import java.util.ArrayList;
import java.util.List;

/*
 * This class is used to manage lexemes for reserved words, identifiers
 * and composite tokens like && or ||.
 */
public class Word extends Token{
    public String lexeme = "";

    public Word(String s, int tag){
        super(tag);
        this.lexeme = s;
        Lexer.putWord(this);
    }

    public String toString(){
        return lexeme;
    }

    public static final Word 
        and = new Word("&&", Tag.AND),
        or = new Word("||", Tag.OR),
        eq = new Word("==", Tag.EQUAL),
        neq = new Word("!=", Tag.NEQUAL),
        leq = new Word("<=", Tag.LEQUAL),
        geq = new Word(">=", Tag.GEQUAL),
        minus = new Word("minus", Tag.MINUS),
        True = new Word("true", Tag.TRUE),
        False = new Word("false", Tag.FALSE),

        If = new Word("if", Tag.IF),
        Else = new Word("else", Tag.ELSE),
        While = new Word("while", Tag.WHILE),
        Do = new Word("do", Tag.DO),
        Break = new Word("break", Tag.BREAK),

        Temp = new Word("t", Tag.TEMP);

        static final List<Word> reservedWords = new ArrayList<Word>(){{
            add(and);
            add(or);
            add(eq);
            add(neq);
            add(leq);
            add(geq);
            add(minus);
            add(True);
            add(False);

            add(If);
            add(Else);
            add(While);
            add(Do);
            add(Break);

            add(Temp);
        }};


}
