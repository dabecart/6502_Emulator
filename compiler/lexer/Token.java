package compiler.lexer;

public class Token {
    public int tag;
    private boolean isChar;

    public Token(int t){
        this.tag = t;
        this.isChar = false;
    }

    public Token(char t){
        this.tag = t;
        this.isChar = true;
    }

    public String toString(){
        return isChar ? Character.toString((char)tag) : Integer.toString(tag);
    }
}
