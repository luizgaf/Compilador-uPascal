#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_SYMBOLS 100  // Tamanho máximo da Tabela de Símbolos

typedef struct {
    char lexeme[100];  // O nome do identificador ou palavra reservada
} Symbol;

Symbol symbolTable[MAX_SYMBOLS];  // Tabela de símbolos
int symbolCount = 0;              // Contador de símbolos

typedef enum {
    OP_EQ, OP_GE, OP_MUL, OP_NE, OP_LE, OP_DIV, OP_GT, OP_AD, OP_ASS, OP_LT, OP_MIN,
    SMB_OBC, SMB_COM, SMB_CBC, SMB_SEM, SMB_OPA, SMB_CPA, PLV_RSD, ID, NUM_INT, NUM_FLT,
    KW_PROGRAM, KW_VAR, KW_INTEGER, KW_REAL, KW_BEGIN, KW_END, KW_IF, KW_THEN, KW_ELSE, KW_WHILE, KW_DO
} TokenName;

typedef struct {
    TokenName token;  // Tipo de token
    char lexeme[100]; // O valor real do token (como o identificador ou número)
    int line;         // Linha do código onde o token foi encontrado
    int column;       // Coluna onde começa o token
} Token;

// Função para adicionar um símbolo à tabela
void addSymbolToTable(Token token) {
    for (int i = 0; i < symbolCount; i++) {
        if (strcmp(symbolTable[i].lexeme, token.lexeme) == 0) {
            return;  // Já está na tabela, não adiciona duplicata
        }
    }
    strcpy(symbolTable[symbolCount].lexeme, token.lexeme);
    symbolCount++;
}

// Função para exibir a Tabela de Símbolos
void writeSymbolTable(FILE *file) {
    fprintf(file, "\nTabela de Símbolos:\n");
    for (int i = 0; i < symbolCount; i++) {
        fprintf(file, "%s\n", symbolTable[i].lexeme);
    }
}

// Função para gravar os erros léxicos no arquivo
void logLexicalError(FILE *file, char invalidChar, int line, int column) {
    fprintf(file, "Erro lexico: Caractere invalido '%c' na linha %d, coluna %d\n", invalidChar, line, column);
}

// Função para gravar os tokens no arquivo de saída
void writeTokenToFile(FILE *file, Token token) {
    const char *tokenName;
    switch (token.token) {
        case KW_PROGRAM: tokenName = "KW_PROGRAM"; break;
        case KW_VAR: tokenName = "KW_VAR"; break;
        case ID: tokenName = "ID"; addSymbolToTable(token); break;  // Adiciona identificadores à tabela
        // Adicione mais palavras reservadas e operadores...
        default: tokenName = "UNKNOWN"; break;
    }
    fprintf(file, "<%s, %s, %d, %d>\n", tokenName, token.lexeme, token.line, token.column);
}

// Função para reconhecer identificadores e palavras reservadas
Token recognizeIdentifier(char *input, int *position, int line, int column) {
    Token token;
    int length = 0;

    while (isalnum(input[*position])) {  // Identifica letras e dígitos
        token.lexeme[length++] = input[*position];
        (*position)++;
    }
    token.lexeme[length] = '\0';  // Termina a string

    // Verifica se é uma palavra reservada ou um identificador
    if (strcmp(token.lexeme, "program") == 0) {
        token.token = KW_PROGRAM;
    } else if (strcmp(token.lexeme, "var") == 0) {
        token.token = KW_VAR;
    } else {
        token.token = ID;  // Identificador
    }
    token.line = line;
    token.column = column;
    return token;
}

// Função para reconhecer números (inteiros e reais)
Token recognizeNumber(char *input, int *position, int line, int column) {
    Token token;
    int length = 0;
    int isReal = 0;

    // Coleta a parte inteira do número
    while (isdigit(input[*position])) {
        token.lexeme[length++] = input[*position];
        (*position)++;
    }

    // Verifica se é um número real (com ponto decimal)
    if (input[*position] == '.') {
        isReal = 1;
        token.lexeme[length++] = input[*position];
        (*position)++;
        while (isdigit(input[*position])) {
            token.lexeme[length++] = input[*position];
            (*position)++;
        }
    }

    token.lexeme[length] = '\0';  // Termina o lexema
    token.token = isReal ? NUM_FLT : NUM_INT;
    token.line = line;
    token.column = column;
    return token;
}

// Função de análise léxica
void lexicalAnalysis(char *input, FILE *file) {
    int position = 0;
    int line = 1, column = 1;

    while (input[position] != '\0') {
        if (isspace(input[position])) {
            if (input[position] == '\n') {
                line++;
                column = 1;
            }
            position++;
        } else if (isalpha(input[position])) {
            Token token = recognizeIdentifier(input, &position, line, column);
            writeTokenToFile(file, token);
        } else if (isdigit(input[position])) {
            Token token = recognizeNumber(input, &position, line, column);
            writeTokenToFile(file, token);
        } else {
            logLexicalError(file, input[position], line, column);
            position++;
        }
        column++;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Por favor, forneca o nome do arquivo de entrada.\n");
        return 1;
    }

    FILE *inputFile = fopen(argv[1], "r");
    if (inputFile == NULL) {
        printf("Nao foi possivel abrir o arquivo %s.\n", argv[1]);
        return 1;
    }

    FILE *outputFile = fopen("saida.lex", "w");

    char buffer[1000];
    while (fgets(buffer, sizeof(buffer), inputFile)) {
        lexicalAnalysis(buffer, outputFile);
    }

    // Grava a Tabela de Símbolos no arquivo
    writeSymbolTable(outputFile);

    fclose(inputFile);
    fclose(outputFile);

    printf("Analise lexica concluida. Resultado gravado em 'saida.lex'.\n");

    return 0;
}
