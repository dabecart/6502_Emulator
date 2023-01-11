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
                            TRUE = 266,
                            FALSE = 267,

                            IF = 300,
                            ELSE = 301,
                            WHILE = 302,
                            DO = 303,
                            BREAK = 304,
                            CONTINUE = 305,
                            FOR = 306,
                            SWITCH = 307,
                            CASE = 308,
                            DEFAULT = 309,

                            ADDEQ = 277,    // +=
                            SUBEQ = 278,    // -=
                            MULTEQ = 279,   // *=
                            DIVEQ = 280,    // /=
                            MODEQ = 281,    // %=
                            INC = 282,      // ++
                            DEC = 283,      // --
                            ANDEQ = 284,    // &=
                            OREQ = 285,     // |=
                            XOREQ = 286,    // ^=

                            ID = 512,       // Variable/Classes names
                            BASIC = 513,    // Bundle of basic variables
                            ARRAY = 514,
                            TEMP = 515,     // For temporal/intermediary variables
                            INDEX = 512,    // For access to arrays (index)
                            LABEL = 513,

                            GOTO = 600;
}