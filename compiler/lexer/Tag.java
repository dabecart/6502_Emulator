package compiler.lexer;

public class Tag {
    
    public final static int NUM = 257,      // Represents an integer digit
                            AND = 258,      // &&
                            OR = 259,       // ||
                            EQUAL = 260,    // ==
                            NEQUAL = 261,   // !=
                            LEQUAL = 262,   // <=
                            GEQUAL = 263,   // >=
                            MINUS = 264,    // The negative before the number
                            TRUE = 265,
                            FALSE = 266,

                            IF = 267,
                            ELSE = 268,
                            WHILE = 269,
                            DO = 270,
                            BREAK = 271,

                            ID = 512,       // Variable/Classes names
                            BASIC = 513,    // Bundle of basic variables
                            ARRAY = 514,
                            TEMP = 515;     // For temporal variables
}