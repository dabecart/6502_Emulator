package compiler.code_generator.types;

import compiler.code_generator.MemoryCell;

public interface TypeInterface {
    public String setValue(MemoryCell value, MemoryCell to);
}
