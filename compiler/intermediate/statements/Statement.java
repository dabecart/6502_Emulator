package compiler.intermediate.statements;

import compiler.intermediate.Node;

public class Statement extends Node {
    public Statement(){}

    public static Statement Null = new Statement();

    public void generate(int beginLabel, int afterLabel){}

    // The beginning label of the statement. Used for loops
    public int savedBeforeLabel = 0;
    // The ending label of the statement.
    public int savedAfterLabel = 0;
    public static Statement Enclosing = Statement.Null;
}
