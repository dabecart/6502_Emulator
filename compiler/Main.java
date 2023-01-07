package compiler;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;

import compiler.lexer.Lexer;
import compiler.parser.Parser;

public class Main {
    static void boo(){}

    public static void main(String[] args) throws IOException{
        if(args.length == 0) {
            System.err.println("No input file found!");
            return;
        } 

        FileReader fr = new FileReader(args[0]);
        BufferedReader br = new BufferedReader(fr);

        Lexer lexer = new Lexer(br);
        Parser parser = new Parser(lexer);
        parser.program();
        System.out.println();
    }
}
