package compiler.symbols;

import java.util.ArrayList;
import java.util.List;

import compiler.lexer.Tag;
import compiler.lexer.Word;

public class Type extends Word {
    public int byteSize = 0;

    public Type(Type type){
        super(type.lexeme, type.tag);
        this.byteSize = type.byteSize;
    }

    public Type(String str, int tag, int size){
        super(str, tag);
        byteSize = size;
    }

    public static final Type 
        Int = new Type("int", Tag.BASIC, 4),
        Char = new Type("char", Tag.BASIC, 1),
        Bool = new Type("bool", Tag.BASIC, 1),
        Label = new Type("L", Tag.LABEL, 0);

    public static final List<Type> reservedTypes = new ArrayList<Type>(){{
        add(Int);
        add(Char);
        add(Bool);
    }};

    public static boolean isNumber(Type p){
        return p == Type.Char || p == Type.Int;
    }

    public static Type typeConversion(Type t1, Type t2){
        if(t1 == t2) return t1;
        if(!isNumber(t1) || !isNumber(t2)) return null;
        
        if(t1 == Type.Int || t2 == Type.Int) return Type.Int;
        else if(t1 == Type.Char || t2 == Type.Char) return Type.Char;
        else{
            try {
                throw new Exception("Type conversion between " + t1.toString() + " and " + t2.toString() + " not possible");
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
        return null;
    }
}
