package compiler.intermediate.statements;

import compiler.intermediate.Node;

public class Statement extends Node {
    public Statement(){}

    public static Statement Null = new Statement();

    public void generate(int beginLabel, int afterLabel){}

    public int savedAfterLabel = 0;
    public static Statement Enclosing = Statement.Null;
}
