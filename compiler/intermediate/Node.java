package compiler.intermediate;

import compiler.code_generator.SystemOperators;
import compiler.intermediate.three_address.Intermediate;
import compiler.intermediate.three_address.Label;
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
    public static Label newLabel(){
        return new Label(++labels);
    }

    public void printLabel(Label l){
        Intermediate.setLabel(l);
        System.out.print(l.toString() + ":");
    }

    public void gotoLabel(Label labelNumber){
        Intermediate.setOperation(SystemOperators.GOTO);
        Intermediate.setResult(labelNumber);
        Intermediate.next();
        System.out.println("\tgoto L" + labelNumber);
    }

    public void print(String s){
        System.out.println("\t" + s);
    }

}
