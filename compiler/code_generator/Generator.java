package compiler.code_generator;

import java.util.HashMap;
import java.util.List;

import compiler.intermediate.operators.ArrayAccess;
import compiler.intermediate.three_address.Intermediate;
import compiler.intermediate.three_address.Quadruple;
import compiler.symbols.Array;
import compiler.symbols.Type;

public class Generator {
    
    public static Generator gen;

    CPU cpu = new CPU();
    List<Quadruple> intermediateCode;
    
    StringBuilder out = new StringBuilder();

    HashMap<String, MemoryCell> memory = new HashMap<>();
    int memoryCounter = 0;

    public Generator(){
        this.intermediateCode = Intermediate.intermediateCode;
        gen = this;
    }

    public void generate(){
        for(Quadruple q : intermediateCode){
            if(q.result instanceof ArrayAccess){
                Array arr = (Array) ((ArrayAccess)q.result).array.type;
                int startArrayAddress = memoryCounter;
                for(int i = 0; i < arr.size; i++){
                    addToMemory(arr.toString() + "[" + i + "]", arr.of, startArrayAddress, i);
                }
            }else{
                addToMemory(q.toString(), q.result.type);
            }
            
            out.append(q + "\n");
        }
    }

    public void print(){
        System.out.println("*************************************************");
        System.out.println(out);
    }

    public MemoryCell addToMemory(String name, Type type){
        MemoryCell mem = new MemoryCell(type, memoryCounter);
        memory.put(name, mem);
        memoryCounter += type.byteSize;
        return mem;
    }

    public MemoryCell addToMemory(String name, Type type, int startAddress, int arrayIndex){
        ArrayCell mem = new ArrayCell(type, memoryCounter, startAddress, arrayIndex);
        memory.put(name, mem);
        memoryCounter += type.byteSize;
        return mem;
    }

    public MemoryCell getMemoryCell(String name){
        return memory.get(name);
    }

}
