package compiler.code_generator;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import compiler.intermediate.three_address.Intermediate;
import compiler.intermediate.three_address.Quadruple;

public class Generator {
    
    CPU cpu = new CPU();
    List<Quadruple> intermediateCode;

    StringBuilder out = new StringBuilder();

    HashMap<String, MemoryCell> memory = new HashMap<>();

    public Generator(){
        this.intermediateCode = Intermediate.intermediateCode;
    }

    public void generate(){
        for(Quadruple q : intermediateCode) out.append(q.generate(this));
    }

    public void print(){
        System.out.println(out);
    }

}
