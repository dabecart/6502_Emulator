package compiler.intermediate.three_address;

import java.util.ArrayList;
import java.util.List;
import java.util.ListIterator;

import compiler.code_generator.instructions.AssignInstruction;
import compiler.code_generator.instructions.ConditionalInstruction;
import compiler.code_generator.instructions.JumpInstruction;
import compiler.code_generator.instructions.OperateInstruction;
import compiler.code_generator.instructions.UnaryInstruction;
import compiler.intermediate.expressions.Expression;

public class Intermediate {

    public static List<Quadruple> intermediateCode = new ArrayList<>();
    public static Quadruple currentQuad = new Quadruple();

    public static void next(){
        intermediateCode.add(currentQuad);
        currentQuad = new Quadruple();
    }

    public static void add(Quadruple quad){
        quad = setQuadrupleSubClass(quad);
        if(!currentQuad.label.isEmpty()){
            quad.label = new ArrayList<>(currentQuad.label);
            for(Label li : quad.label) li.occurrences++;
            currentQuad.label.clear();
        }
        intermediateCode.add(quad);
    }

    public static void setOperation(int op){
        if(currentQuad.op != -1) throw new Error("Operation already set");
        currentQuad.op = op;
        currentQuad = setQuadrupleSubClass(currentQuad);
    }

    public static Quadruple setQuadrupleSubClass(Quadruple quad){
        if(AssignInstruction.checkOperator(quad.op)) return new AssignInstruction(quad);
        if(ConditionalInstruction.checkOperator(quad.op)) return new ConditionalInstruction(quad);
        if(JumpInstruction.checkOperator(quad.op)) return new JumpInstruction(quad);
        if(OperateInstruction.checkOperator(quad.op)) return new OperateInstruction(quad);
        if(UnaryInstruction.checkOperator(quad.op)) return new UnaryInstruction(quad);
        return quad;
    }

    public static void setLabel(Label label){
        label.occurrences++;
        currentQuad.addLabel(label);
    }

    public static void setArgs(Expression arg1, Expression arg2){
        if(arg1 != null){
            if(currentQuad.arg1 != null) throw new Error("First argument already set");
            currentQuad.arg1 = arg1;
        }
        if(arg2 != null){
            if(currentQuad.arg2 != null) throw new Error("Second argument already set");
            currentQuad.arg2 = arg2;
        }
    }

    public static void setResult(Expression result){
        if(currentQuad.result != null) throw new Error("Result is already set");
        if(result instanceof Label) ((Label) result).occurrences++;
        currentQuad.result = result;
    }

    public static void printAll(){
        System.out.println("\n*******************************************************");
        for(Quadruple q : intermediateCode) System.out.println(q);
    }

    public static void optimize(){
        optimizeLabels();
    }

    // Will remove single labels and will set a single label for each instruction with multiple
    // labels.
    public static void optimizeLabels(){
        // Erase single labels
        for(Quadruple q : intermediateCode){
            if(q.label == null || q.label.isEmpty()) continue;

            ListIterator<Label> lIterator = q.label.listIterator();
            while(lIterator.hasNext()){
                if(lIterator.next().occurrences <= 1) lIterator.remove();
            }
        }

        // Erase duplicated labels
        for(Quadruple q : intermediateCode){
            if(q.label == null || q.label.size() <= 1) continue;

            // This instruction has multiple labels, so remove all instances of the duplicated labels
            // and substitute them with the first of the bunch.
            Label quadLabel = q.label.get(0);
            for(int i = 1; i < q.label.size(); i++){
                Label duplicatedLabel = q.label.get(i);
                for(Quadruple qin : intermediateCode){
                    qin.substituteAllXLabelsWithY(duplicatedLabel, quadLabel);
                }
            }
            q.label.clear();
            q.label.add(quadLabel);
        }

    }

}
