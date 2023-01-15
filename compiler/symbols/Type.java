package compiler.symbols;

import java.util.ArrayList;
import java.util.List;

import compiler.lexer.Tag;
import compiler.lexer.Word;
import compiler.symbols.types.CharType;
import compiler.symbols.types.TypeFunctions;

public class Type extends Word {
    public int byteSize = 0;
    public TypeFunctions functions;

    public Type(String str, int tag, int size, TypeFunctions functions){
        super(str, tag);
        this.byteSize = size;
        this.functions = functions;
    }

    public static final Type 
        Int = new Type("int", Tag.BASIC, 4, null),
        Char = new Type("char", Tag.BASIC, 1, new CharType()),
        Bool = new Type("bool", Tag.BASIC, 1, null),
        Label = new Type("L", Tag.LABEL, 0, null);

    public static final List<Type> reservedTypes = new ArrayList<Type>(){{
        add(Int);
        add(Char);
        add(Bool);
    }};

    public static Type getTypeByName(String name){
        for(Type t : reservedTypes){
            if(t.lexeme.equals(name)) return t;
        }
        return null;
    }

    public static boolean isNumber(Type p){
        return p == Type.Char || p == Type.Int;
    }

    public static Type typeConversion(Type t1, Type t2){
        if(t1 == t2) return t1;
        if(!isNumber(t1) || !isNumber(t2)) return null;
        
        if(t1 == Type.Int || t2 == Type.Int) return Type.Int;
        else if(t1 == Type.Char || t2 == Type.Char) return Type.Char;
        else throw new Error("Type conversion between " + t1.toString() + " and " + t2.toString() + " not possible");
    }
}
