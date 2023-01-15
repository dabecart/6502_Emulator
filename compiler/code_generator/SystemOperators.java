package compiler.code_generator;

import compiler.lexer.Tag;

public class SystemOperators {
                            // Numerical operators
    public static final int ADD = '+',
                            SUB = '-',
                            MUL = '*',
                            DIV = '/',
                            MOD = '%',
                            NEG = Tag.MINUS,

                            SET = '=',

                            // Bitwise operators
                            NOT = '~',
                            AND = '&',
                            OR = '|',
                            XOR = '^',

                            // Boolean operators
                            lAND = Tag.AND, // Logical AND &&
                            lOR = Tag.OR,   // Logical OR ||
                            EQ = Tag.EQUAL,
                            NEQ = Tag.NEQUAL,
                            GEQ = Tag.GEQUAL,
                            LEQ = Tag.LEQUAL,
                            LES = '<',
                            GRE = '>',

                            // Conditional jump
                            IF = Tag.IF,    // if op1 goto result
                            IFNOT = Tag.ELSE,

                            // Unconditional jump
                            GOTO = Tag.GOTO,

                            //Cast instruction
                            CAST = Tag.CAST;
}
