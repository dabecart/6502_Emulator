package compiler.symbols.types;

import compiler.code_generator.MemoryCell;
import compiler.intermediate.expressions.Constant;

public abstract class TypeFunctions {
    
    // Loads a "value" from memory and stores it in "to"
    public abstract String setValue(MemoryCell value, MemoryCell to);
    // Loads a "value" address plus "access" from memory and stores it in "to"
    public abstract String setValue(MemoryCell value, MemoryCell access, MemoryCell to);
    // Loads a constant "value" and stores it in "to"
    public abstract String setValue(Constant value, MemoryCell to);

    // Loads a "value" from memory and stores it in "to" plus an "access" index
    public abstract String setArray(MemoryCell value, MemoryCell to, MemoryCell access);
    // Loads a constant "value" and stores it in "to" address plus an "access" index
    public abstract String setArray(Constant value, MemoryCell to, MemoryCell access);

}
