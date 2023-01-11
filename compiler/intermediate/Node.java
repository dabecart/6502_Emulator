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
    public static int newLabel(){
        return ++labels;
    }

    public void printLabel(int labelNumber){
        System.out.print("L" + labelNumber + ":");
        Intermediate.setLabel(labelNumber);
    }

    public void gotoLabel(int labelNumber){
        Intermediate.setOperation(SystemOperators.GOTO);
        Intermediate.setResult(new Label(labelNumber));
        Intermediate.next();
        System.out.println("\tgoto L" + labelNumber);
    }

    public void print(String s){
        System.out.println("\t" + s);
    }

}
