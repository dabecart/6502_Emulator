package compiler.intermediate.statements;

import compiler.intermediate.Node;
import compiler.intermediate.three_address.Label;
import compiler.lexer.Tag;

public class Statement extends Node {
    // Saves the Tag of the function parenting this statement. Used for special 
    // keywords like "break" and "continue" that must be inside loops, or "case" 
    // inside switches.
    public int parentingFunctionCall;
    
    public Statement(int parentFunctionTag){
        this.parentingFunctionCall = parentFunctionTag;
    }

    public Statement(){
        this.parentingFunctionCall = -1;
    }

    public static Statement Null = new Statement();

    public void generate(Label beginLabel, Label afterLabel){}

    // The beginning label of the statement. Used for loops
    public Label continueLabel;
    // The ending label of the statement.
    public Label breakLabel;
    public static Statement Enclosing = Statement.Null;

    public boolean isLoopStatement(){
        switch(parentingFunctionCall){
            case Tag.WHILE:
            case Tag.FOR:
            case Tag.DO:{
                return true;
            }
        }
        return false;
    }
}
