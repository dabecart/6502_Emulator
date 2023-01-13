package compiler;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;

import compiler.code_generator.Generator;
import compiler.lexer.Lexer;
import compiler.parser.Parser;

public class Main {
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
        Generator gen = new Generator();
        gen.generate();
        gen.print();
        System.out.println();
    }
}
