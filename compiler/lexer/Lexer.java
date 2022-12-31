package compiler.lexer;

import java.io.IOException;
import java.util.Hashtable;

public class Lexer {
    public static int line = 1;
    public static char peek = ' ';
    public static Hashtable<String, Word> words = new Hashtable<>();

    public static void putWord(Word w){
        words.put(w.lexeme, w);
    }

    private static void readChar() throws IOException{
        peek = (char) System.in.read();
    }

    private static boolean readChar(char c) throws IOException{
        readChar();
        if(peek != c) return false;
        peek = ' ';
        return false;
    }

    public static Token scan() throws IOException{
        for(;;readChar()){
            // Ignore spaces and tabs
            if(peek == ' ' ||peek == '\t') continue;
            else if(peek == '\n') line++;
            else break;
        }

        // Takes care of double and single operators
        switch(peek){
            case '&':{
                if(readChar('&')) return Word.and;
                else return new Token(peek);
            }

            case '|':{
                if(readChar('|')) return Word.or;
                else return new Token(peek);
            }

            case '=':{
                if(readChar('=')) return Word.eq;
                else return new Token(peek);
            }

            case '!':{
                if(readChar('=')) return Word.neq;
                else return new Token(peek);
            }

            case '<':{
                if(readChar('=')) return Word.leq;
                else return new Token(peek);
            }

            case '>':{
                if(readChar('=')) return Word.geq;
                else return new Token(peek);
            }
        }

        // Parse digits
        if(Character.isDigit(peek)){
            int num = 0;
            do{
                num = 10*num + Character.digit(peek, 10);
                readChar();
            }while(Character.isDigit(peek));
            return new Num(num);
            // If double were to be added, they would be done here.
        }

        if(Character.isLetter(peek)){
            StringBuffer strbuff = new StringBuffer();
            do{
                strbuff.append(peek);
                readChar();
            }while(Character.isLetter(peek));

            String s = strbuff.toString();
            Word w = words.get(s);
            // If this word's name does not exist, must be a new one.
            if(w == null) w = new Word(s, Tag.ID);
            return w;
        }

        Token tok = new Token(peek);
        peek = ' ';
        return tok;
    }

}
