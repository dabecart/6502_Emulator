package compiler.intermediate;

import compiler.lexer.Lexer;

public class Node {
    int lexLine = 0;
    
    public Node() {
        lexLine = Lexer.line;
    }

    public void error(String str){
        throw new Error("In line" + lexLine + ": " + str);
    }

    static int labels = 0;
    public static int newLabel(){
        return ++labels;
    }

    public void printLabel(int labelNumber){
        System.out.print("L" + labelNumber + ":");
    }

    public void print(String s){
        System.out.println("\t" + s);
    }

}
