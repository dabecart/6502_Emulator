package compiler.lexer;

import java.io.BufferedReader;
import java.io.IOException;
import java.util.Hashtable;

import compiler.symbols.Type;

public class Lexer {
    public static int line = 1;
    public char peek = ' ';
    public static Hashtable<String, Word> words = new Hashtable<>();

    BufferedReader br;

    public Lexer(BufferedReader br){
        this.br = br;
        for(Word w : Word.reservedWords){
            putWord(w);
        }
        for(Word w : Type.reservedTypes){
            putWord(w);
        }
    }

    public static void putWord(Word w){
        words.put(w.lexeme, w);
    }

    private void readChar() throws IOException{
        peek = (char) br.read();
    }

    private boolean readChar(char c) throws IOException{
        readChar();
        if(peek != c) return false;
        peek = ' ';
        return true;
    }

    public Token scan() throws IOException{
        for(;;readChar()){
            // Ignore spaces and tabs
            if(peek == ' ' || peek == '\t' || peek == '\r' /*Carriage return*/) continue;
            else if(peek == '\n') line++;
            else break;
        }

        char peekSave = peek;

        // Takes care of double and single operators
        switch(peek){
            case '&':{
                if(readChar('&')) return Word.and;
                else return new Token(peekSave);
            }

            case '|':{
                if(readChar('|')) return Word.or;
                else return new Token(peekSave);
            }

            case '=':{
                if(readChar('=')) return Word.eq;
                else return new Token(peekSave);
            }

            case '!':{
                if(readChar('=')) return Word.neq;
                else return new Token(peekSave);
            }

            case '<':{
                if(readChar('=')) return Word.leq;
                else return new Token(peekSave);
            }

            case '>':{
                if(readChar('=')) return Word.geq;
                else return new Token(peekSave);
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
