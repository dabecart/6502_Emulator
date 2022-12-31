package compiler.parser;

import java.io.IOException;

import compiler.lexer.Lexer;
import compiler.lexer.Token;
import compiler.symbols.Enviroment;

public class Parser {
    private Token peekToken;
    Enviroment topEnviroment = null;
    int usedDeclarations = 0;

    public Parser() throws IOException{
        move();
    }

    private void error(String message){
        throw new Error("Error in line " + Lexer.line + ": " + message);
    }

    private void move() throws IOException{
        Lexer.scan();
    }

    
}
