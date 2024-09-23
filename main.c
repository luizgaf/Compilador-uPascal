#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef enum {
    OP_EQ, OP_GE, OP_MUL, OP_NE, OP_LE, OP_DIV, OP_GT, OP_AD, OP_ASS, OP_LT, OP_MIN,
    SMB_OBC, SMB_COM, SMB_CBC, SMB_SEM, SMB_OPA, SMB_CPA, PAL_RES, ID, NUM_INT, NUM_FLT,
    KW_PROGRAM, KW_VAR, KW_INTEGER, KW_REAL, KW_BEGIN, KW_END, KW_IF, KW_THEN, KW_ELSE, KW_WHILE, KW_DO
} TokenName;

// enum que guarda os tokens

typedef struct {
    TokenName token;   
    char lexeme[100];  
    int line;          
    int column;        
} Token;

// struct que guarda um token, tipo, lexema, linha, coluna


Token recognizeID(char *input, int *position, int line, int column){
    Token token;
    int len = 0;
    while (isalnum(input[*position]))
    {
        /* code */
    }
    

    
}