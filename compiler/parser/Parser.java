package compiler.parser;

import java.io.IOException;

import compiler.code_generator.SystemOperators;
import compiler.intermediate.Id;
import compiler.intermediate.expressions.Constant;
import compiler.intermediate.expressions.Expression;
import compiler.intermediate.expressions.logical.And;
import compiler.intermediate.expressions.logical.Comparator;
import compiler.intermediate.expressions.logical.Not;
import compiler.intermediate.expressions.logical.Or;
import compiler.intermediate.operators.Arithmetic;
import compiler.intermediate.operators.ArrayAccess;
import compiler.intermediate.operators.Cast;
import compiler.intermediate.operators.Unary;
import compiler.intermediate.statements.Break;
import compiler.intermediate.statements.Case;
import compiler.intermediate.statements.Continue;
import compiler.intermediate.statements.DoWhile;
import compiler.intermediate.statements.For;
import compiler.intermediate.statements.If;
import compiler.intermediate.statements.IfElse;
import compiler.intermediate.statements.OperationAndSet;
import compiler.intermediate.statements.Set;
import compiler.intermediate.statements.SetArray;
import compiler.intermediate.statements.Statement;
import compiler.intermediate.statements.StatementBundle;
import compiler.intermediate.statements.Switch;
import compiler.intermediate.statements.While;
import compiler.intermediate.three_address.Intermediate;
import compiler.intermediate.three_address.Label;
import compiler.lexer.Lexer;
import compiler.lexer.Num;
import compiler.lexer.Tag;
import compiler.lexer.Token;
import compiler.lexer.Word;
import compiler.symbols.Array;
import compiler.symbols.Enviroment;
import compiler.symbols.Type;

public class Parser {
    private Token peekToken;
    Enviroment topEnviroment = null;
    int usedDeclarations = 0;

    Lexer lexer;

    public Parser(Lexer lexer) throws IOException{
        this.lexer = lexer;
        move();
    }

    private void error(String message){
        throw new Error(message + " (Line " + Lexer.line + ") ");
    }

    private void match(int c) throws IOException{
        if(peekToken.tag == c) move();
        else error("Syntax error: expected '" + Character.toString((char)c) + "', found '" + peekToken.toString() + "'");
    }

    private void move() throws IOException{
        peekToken = lexer.scan();
    }

    public void program() throws IOException{
        Statement s = block();
        Label begin = Statement.newLabel();
        Label after = Statement.newLabel();
        s.printLabel(begin);
        s.generate(begin, after);
        s.printLabel(after);
        Intermediate.setOperation(SystemOperators.GOTO);
        Intermediate.setResult(after);
        Intermediate.next();

        Intermediate.optimize();
    }

    private Statement block() throws IOException{
        match('{');
        Enviroment temporalEnviroment = topEnviroment;
        topEnviroment = new Enviroment(topEnviroment);
        Statement s = statements();
        match('}');
        topEnviroment = temporalEnviroment;
        return s;
    }

    // Declaration are made as:     D -> type ID;
    // For example,                 int variable;
    private Statement declarations() throws IOException{
        StatementBundle statement = StatementBundle.Null;
        boolean declarationHasStatements = false;

        while(peekToken.tag == Tag.BASIC){
            Type variableType = (Type) peekToken;
            match(Tag.BASIC);

            boolean commaFound;
            do{ // For declarations with comma i.e. int i,j,k=2;
                commaFound = false;

                Token tok = peekToken;
                match(Tag.ID);
                if(topEnviroment.getTokenId(tok) != null){
                    error("Duplicate variable " + tok.toString());
                }

                if(peekToken.tag == '['){  // Array
                    variableType = array(variableType);
                }

                usedDeclarations += variableType.byteSize;
                Id id = new Id((Word)tok, variableType, usedDeclarations);
                topEnviroment.addToken(tok, id);

                if(peekToken.tag == '='){   // Variable assignment
                    if(variableType instanceof Array) error("Cannot initialize array");
                    declarationHasStatements = true;
                    move();
                    statement = new StatementBundle(statement, new Set(id, boolExpression())); 
                }

                commaFound = peekToken.tag == ',';
                if(commaFound) move();
            } while(commaFound);
            match(';');
        }
        return declarationHasStatements ? statement : null;
    }
    
    // Create an array of type p:           Array -> type ID [ NUM ]
    private Type array(Type p) throws IOException{
        match('[');
        Token tok = peekToken;  // Size of array
        match(Tag.NUM);
        match(']');
        if(peekToken.tag == '[') p = array(p);
        return new Array(((Num)tok).value, p);
    }

    private Statement statements() throws IOException{
        if(peekToken.tag == '}') return Statement.Null;
        else return new StatementBundle(statement(), statements());
    }

    private Statement statement() throws IOException{
        // First check if it is a new variable that is being created.
        // It can be followed by its initialization so that would be a statement.
        Statement variableDeclaration = declarations();
        if(variableDeclaration != null) return variableDeclaration;
        
        Expression x;
        Statement s1, s2, s3;
        Statement savedStatement;

        switch(peekToken.tag){
            case ';':{
                move();
                return Statement.Null;
            }

            case Tag.IF:{
                match(Tag.IF);
                match('(');
                x = boolExpression();
                match(')');

                s1 = statement();
                if(peekToken.tag != Tag.ELSE) return new If(x, s1);
                match(Tag.ELSE);
                s2 = statement();
                return new IfElse(x, s1, s2);
            }

            case Tag.WHILE:{
                While whileNode = new While();
                savedStatement = Statement.Enclosing;
                Statement.Enclosing = whileNode;

                match(Tag.WHILE);
                match('(');
                x = boolExpression();
                match(')');

                s1 = statement();
                whileNode.start(x, s1);
                Statement.Enclosing = savedStatement;

                return whileNode;
            }

            case Tag.DO:{
                DoWhile dowhileNode = new DoWhile();
                savedStatement = Statement.Enclosing;
                Statement.Enclosing = dowhileNode;

                match(Tag.DO);

                s1 = statement();

                match(Tag.WHILE);
                match('(');
                x = boolExpression();
                match(')');
                match(';');
                
                dowhileNode.start(x, s1);
                Statement.Enclosing = savedStatement;

                return dowhileNode;
            }

            case Tag.FOR:{
                For forLoopNode = new For();
                savedStatement = Statement.Enclosing;
                Statement.Enclosing = forLoopNode;

                // A for loop creates an enviroment for the declarations on the first parameter.
                topEnviroment = new Enviroment(topEnviroment);

                match(Tag.FOR);
                match('(');
                if(peekToken.tag == ';'){
                    s1 = Statement.Null;
                    match(';');
                }else{
                    s1 = declarations();
                    if(s1 == null){
                        s1 = assignValue();
                    }
                }

                if(peekToken.tag != ';'){
                    x = boolExpression();
                }else{
                    x = null;
                }
                match(';');
                // The third parameter must be a SET function
                s2 = assignValue();
                match(')');

                s3 = statement();

                forLoopNode.start(s1, x, s2, s3);
                Statement.Enclosing = savedStatement;

                topEnviroment = topEnviroment.previousEnv;
                return forLoopNode;
            }

            case Tag.BREAK:{
                match(Tag.BREAK);
                match(';');
                return new Break();
            }

            case Tag.CONTINUE:{
                match(Tag.CONTINUE);
                match(';');
                return new Continue();
            }

            case Tag.SWITCH:{
                Switch switchNode = new Switch();
                savedStatement = Statement.Enclosing;
                Statement.Enclosing = switchNode;

                match(Tag.SWITCH);
                match('(');
                x = boolExpression();
                match(')');

                s1 = statement();
                switchNode.start(x, s1);
                Statement.Enclosing = savedStatement;
                return switchNode;
            }
            
            case Tag.CASE:{
                match(Tag.CASE);
                x = null;
                switch(peekToken.tag){
                    case Tag.NUM:{
                        x = new Constant(peekToken, Type.Int);
                        break;
                    }

                    case Tag.TRUE:{
                        x = Constant.True;
                        break;
                    }
        
                    case Tag.FALSE:{
                        x = Constant.False;
                        break;
                    }
        
                    default:{
                        error("Case must be constant");
                        break;
                    }
                }
                move();
                match(':');
                s1 = statement();
                return new Case((Constant) x, s1);
            }

            case Tag.DEFAULT:{
                match(Tag.DEFAULT);
                match(':');
                s1 = statement();
                return new Case(s1);
            }

            case '{':{
                return block();
            }

            default:{
                s1 = assignValue();
                match(';');
                return s1;
            }
        }
    }

    private Statement assignValue() throws IOException{
        Expression variable = null;
        Token operator = null;

        // Can be: op ID (aka. ++ID)  or ID op (aka. ID = ) and all the array variants.
        if(peekToken.tag == Tag.ID){
            variable = getVariable(); // May return an array
        }else{
            operator = peekToken;
            move();
        }
        
        if(peekToken.tag == Tag.ID){
            variable = getVariable();
        }else{
            operator = peekToken;
            move();
        }

        if(operator.tag == Tag.INC || operator.tag == Tag.DEC){ // ++ID and --ID
            Token innerOperator = new Token((operator.tag == Tag.INC) ? '+' : '-');
            Expression inc = new Arithmetic(innerOperator, variable, Constant.ONE);
            return new Set((Id)variable, inc);
        }

        if(operator.tag == '='){ 
            if(variable instanceof ArrayAccess){
                return new SetArray((ArrayAccess)variable, boolExpression());
            }else {
                return new Set((Id) variable, boolExpression());
            }
        } // else...  Either is wrong or is an operate and set (+=, -=...)
        
        return new OperationAndSet(variable, operator, boolExpression());
    }

    // Parsing of general expressions (starting with boolean OR firts)
    private Expression boolExpression() throws IOException{
        Expression x = joinExpression();
        while(peekToken.tag == Tag.OR){
            Token tok = peekToken;
            move();
            x = new Or(tok, x, joinExpression());
        }
        return x;
    }

    // Parsing of AND expressions
    private Expression joinExpression() throws IOException{
        Expression x = equalityExpression();
        while(peekToken.tag == Tag.AND){
            Token tok = peekToken;
            move();
            x = new And(tok, x, equalityExpression());
        }
        return x;
    }

    private Expression equalityExpression() throws IOException{
        Expression x = comparativeExpression();
        while(peekToken.tag == Tag.EQUAL || peekToken.tag == Tag.NEQUAL){
            Token tok = peekToken;
            move();
            x = new Comparator(tok, x, comparativeExpression());
        }
        return x;
    }

    private Expression comparativeExpression() throws IOException{
        Expression x = logicalExpression();
        switch(peekToken.tag){
            case '<':
            case Tag.LEQUAL:
            case Tag.GEQUAL:
            case '>':{
                Token tok = peekToken;
                move();
                return new Comparator(tok, x, logicalExpression());
            }
            default:{
                return x;
            }
        }
    }

    private Expression logicalExpression() throws IOException{
        Expression x = expression();
        while(peekToken.tag == '|' || peekToken.tag == '&' || peekToken.tag == '^'){
            Token tok = peekToken;
            move();
            x = new Arithmetic(tok, x, expression());
        }
        return x;
    }

    private Expression expression() throws IOException{
        Expression x = term();
        while(peekToken.tag == '+' || peekToken.tag == '-'){
            Token tok = peekToken;
            move();
            x = new Arithmetic(tok, x, term());
        }
        return x;
    }

    private Expression term() throws IOException{
        Expression x = unary();
        while(peekToken.tag == '*' || peekToken.tag == '/' || peekToken.tag == '%'){
            Token tok = peekToken;
            move();
            x = new Arithmetic(tok, x, unary());
        }
        return x;
    }

    private Expression unary() throws IOException{
        if(peekToken.tag == '-'){
            move();
            return new Unary(Word.minus, unary());
        }else if(peekToken.tag == '~'){
            move();
            return new Unary(new Token('~'), unary());
        }else if(peekToken.tag == '!'){
            Token tok = peekToken;
            move();
            return new Not(tok, unary());
        }else if(peekToken.tag == '('){
            match('(');
            Token typeCast = peekToken;
            match(Tag.BASIC);
            match(')');
            return new Cast(typeCast, boolExpression());
        }else{
            return factor();
        }
    }

    private Expression factor() throws IOException{
        Expression x = null;
        switch(peekToken.tag){
            case '(':{
                move();
                x = boolExpression();
                match(')');
                return x;
            }

            case Tag.NUM:{
                x = new Constant(peekToken, Type.Int);
                move();
                return x;
            }

            case Tag.TRUE:{
                x = Constant.True;
                move();
                return x;
            }

            case Tag.FALSE:{
                x = Constant.False;
                move();
                return x;
            }

            case Tag.ID:{
                return getVariable();
            }

            default:{
                error("Syntax error, wrong " + peekToken);
                return x;
            }
        }
    }

    Expression getVariable() throws IOException{
        String s = peekToken.toString();
        Id id = topEnviroment.getTokenId(peekToken);
        if(id == null) error(s + " not declared");

        move();
        if(peekToken.tag != '[') return id;
        else return arrayOffset(id);
    }

    ArrayAccess arrayOffset(Id id) throws IOException{
        Expression indexExpression, widthExpression;
        Expression temp1, temp2;
        Expression location;    // Byte location in disk
        Type type = ((Array)id.type).of;

        match('[');
        indexExpression = boolExpression();
        match(']');

        // Calculate the direction for the array
        widthExpression = new Constant(type.byteSize);
        temp1 = new Arithmetic(new Token('*'), indexExpression, widthExpression);
        location = temp1;

        // Multi dimensional array
        while(peekToken.tag == '['){
            match('[');
            indexExpression = boolExpression();
            match(']');

            widthExpression = new Constant(type.byteSize);
            temp1 = new Arithmetic(new Token('*'), indexExpression, widthExpression);
            temp2 = new Arithmetic(new Token('+'),  location, temp1);
            location = temp2;
        }
        return new ArrayAccess(id, location, type);
    }


}
