package compiler.symbols;

import compiler.lexer.Tag;

public class Array extends Type {
    public Type of;         // Type of the array
    public int size = 1;

    public Array(int sz, Type p){
        super("[]", Tag.ARRAY, sz*p.byteSize);
        this.size = sz;
        this.of = p;
    }

    public String toString() {
        return "[" + size + "]" + of.toString();
    }
}
