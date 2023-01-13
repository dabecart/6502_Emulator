package compiler.symbols;

import java.util.Hashtable;

import compiler.intermediate.Id;
import compiler.lexer.Token;

public class Enviroment {
    private Hashtable<Token, Id> table;
    public Enviroment previousEnv;
    public int enviromentId;

    public static int enviromentCount = 0;

    public Enviroment(Enviroment previousEnv){
        this.table = new Hashtable<>();
        this.previousEnv = previousEnv;
        this.enviromentId = enviromentCount++;
    }

    public void addToken(Token t, Id id){
        table.put(t, id);
    }

    // Searches for the variable ID inside and outside of the enveloping enviroments.
    public Id getTokenId(Token t){
        for(Enviroment e = this; e != null; e = e.previousEnv){
            Id found = e.table.get(t);
            if(found != null) return found;
        }
        return null;
    }

}
