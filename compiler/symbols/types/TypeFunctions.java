package compiler.symbols.types;

import compiler.code_generator.MemoryCell;
import compiler.symbols.Type;

public abstract class TypeFunctions {
    
    public abstract String setValue(MemoryCell value, MemoryCell to);
    public abstract MemoryCell castToType(MemoryCell castedCell, Type toType);

}
