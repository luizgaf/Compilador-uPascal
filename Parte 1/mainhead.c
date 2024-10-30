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
    // Operadores
    OP_EQ, OP_GE, OP_MUL, OP_NE, OP_LE, OP_DIV, OP_GT, OP_AD, OP_ASS, OP_LT, OP_MIN,

    // Símbolos
    SMB_OBC, SMB_COM, SMB_CBC, SMB_SEM, SMB_OPA, SMB_CPA,

    // ID, Numero Inteiro, Numero FLutuante, UNKNOWN
    ID, NUM_INT, NUM_FLT, UNKNOWN,

    // Keys
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
        case KW_INTEGER: tokenName = "KW_INTEGER"; break;
        case KW_BEGIN: tokenName = "KW_BEGIN"; break;
        case KW_REAL: tokenName = "KW_REAL"; break;
        case KW_END: tokenName = "KW_END"; break;
        case KW_IF: tokenName = "KW_IF"; break;
        case KW_THEN: tokenName = "KW_THEN"; break;
        case KW_ELSE: tokenName = "KW_ELSE"; break;
        case KW_WHILE: tokenName = "KW_WHILE"; break;
        case KW_DO: tokenName = "KW_DO"; break;

        case NUM_INT: tokenName = "NUM_INT"; break;
        case NUM_FLT: tokenName = "NUM_FLT"; break;

        case SMB_OBC: tokenName = "SMB_OBC"; break;
        case SMB_COM: tokenName = "SMB_COM"; break;
        case SMB_CBC: tokenName = "SMB_CBC"; break;
        case SMB_SEM: tokenName = "SMB_SEM"; break;
        case SMB_OPA: tokenName = "SMB_OPA"; break;
        case SMB_CPA: tokenName = "SMB_CPA"; break;

        case OP_EQ: tokenName = "OP_EQ"; break;
        case OP_GE: tokenName = "OP_GE"; break;
        case OP_MUL: tokenName = "OP_MUL"; break;
        case OP_NE: tokenName = "OP_NE"; break;
        case OP_LE: tokenName = "OP_LE"; break;
        case OP_DIV: tokenName = "OP_DIV"; break;
        case OP_GT: tokenName = "OP_GT"; break;
        case OP_AD: tokenName = "OP_AD"; break;
        case OP_ASS: tokenName = "OP_ASS"; break;
        case OP_LT: tokenName = "OP_LT"; break;
        case OP_MIN: tokenName = "OP_MIN"; break;

        case ID: tokenName = "ID"; addSymbolToTable(token); break;  // Adiciona identificadores à tabela
        case UNKNOWN: tokenName = "UNKNOWN"; break;
        default: tokenName = "UNKNOWN"; break;
    }
    fprintf(file, "<%s, %s, %d, %d>\n", tokenName, token.lexeme, token.line, token.column);
}

// Função auxiliar para verificar se é um identificador válido
int is_identifier(char *lexeme) {
    // O primeiro caractere deve ser uma letra ou sublinhado
    if (!isalpha(lexeme[0]) && lexeme[0] != '_') {
        return 0;
    }

    // Os demais caracteres podem ser letras, dígitos ou sublinhados
    for (int i = 1; i < strlen(lexeme); i++) {
        if (!isalnum(lexeme[i]) && lexeme[i] != '_') {
            return 0;
        }
    }
    return 1;
}

// Função auxiliar para verificar se é um número inteiro válido
int is_integer(char *lexeme) {
    for (int i = 0; i < strlen(lexeme); i++) {
        if (!isdigit(lexeme[i])) {
            return 0;
        }
    }
    return 1;
}

// Função auxiliar para verificar se é um número flutuante válido
int is_float(char *lexeme) {
    int dotCount = 0;

    for (int i = 0; i < strlen(lexeme); i++) {
        if (lexeme[i] == '.') {
            dotCount++;
        } else if (!isdigit(lexeme[i])) {
            return 0;
        }

        if (dotCount > 1) {
            return 0;  // Mais de um ponto decimal
        }
    }
    return (dotCount == 1);  // Deve haver exatamente um ponto decimal
}

// Função para reconhecer operadores e símbolos
Token recognizeOperatorOrSymbol(char *input, int *position, int line, int column) {
    Token token;
    token.lexeme[0] = input[*position];
    token.lexeme[1] = '\0';

    switch (input[*position]) {
        case '=': token.token = OP_EQ; break;
        case '>':
            if (input[*position + 1] == '=') {
                token.lexeme[1] = input[*position + 1];
                token.lexeme[2] = '\0';
                token.token = OP_GE;
                (*position)++;  // Avança o cursor
            } else {
                token.token = OP_GT;
            }
            break;
        case '<':
            if (input[*position + 1] == '=') {
                token.lexeme[1] = input[*position + 1];
                token.lexeme[2] = '\0';
                token.token = OP_LE;
                (*position)++;
            } else if (input[*position + 1] == '>') {
                token.lexeme[1] = input[*position + 1];
                token.lexeme[2] = '\0';
                token.token = OP_NE;
                (*position)++;
            } else {
                token.token = OP_LT;
            }
            break;
        case '+': token.token = OP_AD; break;
        case '-': token.token = OP_MIN; break;
        case '*': token.token = OP_MUL; break;
        case '/': token.token = OP_DIV; break;
        case ':':
            if (input[*position + 1] == '=') {
                token.lexeme[1] = input[*position + 1];
                token.lexeme[2] = '\0';
                token.token = OP_ASS;
                (*position)++;
            }
            break;
        case '{': token.token = SMB_OBC; break;
        case '}': token.token = SMB_CBC; break;
        case ',': token.token = SMB_COM; break;
        case ';': token.token = SMB_SEM; break;
        case '(': token.token = SMB_OPA; break;
        case ')': token.token = SMB_CPA; break;
        default:
            token.token = UNKNOWN;
            break;
    }

    token.line = line;
    token.column = column;
    (*position)++;  // Avança o cursor
    return token;
}

// Função para reconhecer identificadores e palavras reservadas
Token recognizeIdentifier(char *input, int *position, int line, int column) {
    Token token;
    int length = 0;

    // Coleta o lexema do identificador ou palavra reservada
    while (isalnum(input[*position]) || input[*position] == '_') {  // Inclui underscore para identificadores
        token.lexeme[length++] = input[*position];
        (*position)++;
    }
    token.lexeme[length] = '\0';  // Termina a string

    // Verifica se é uma palavra reservada
    if (strcmp(token.lexeme, "program") == 0) {
        token.token = KW_PROGRAM;
    } else if (strcmp(token.lexeme, "var") == 0) {
        token.token = KW_VAR;
    } else if (strcmp(token.lexeme, "integer") == 0) {
        token.token = KW_INTEGER;
    } else if (strcmp(token.lexeme, "real") == 0) {
        token.token = KW_REAL;
    } else if (strcmp(token.lexeme, "begin") == 0) {
        token.token = KW_BEGIN;
    } else if (strcmp(token.lexeme, "end") == 0) {
        token.token = KW_END;
    } else if (strcmp(token.lexeme, "if") == 0) {
        token.token = KW_IF;
    } else if (strcmp(token.lexeme, "then") == 0) {
        token.token = KW_THEN;
    } else if (strcmp(token.lexeme, "else") == 0) {
        token.token = KW_ELSE;
    } else if (strcmp(token.lexeme, "while") == 0) {
        token.token = KW_WHILE;
    } else if (strcmp(token.lexeme, "do") == 0) {
        token.token = KW_DO;
    }
    // Se não for palavra reservada, verifica se é um identificador válido
    else if (is_identifier(token.lexeme)) {
        token.token = ID;
    } else {
        token.token = UNKNOWN;  // Identificador inválido
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
        } else if (isalpha(input[position]) || input[position] == '_') {
            Token token = recognizeIdentifier(input, &position, line, column);
            writeTokenToFile(file, token);
        } else if (isdigit(input[position])) {
            Token token = recognizeNumber(input, &position, line, column);
            writeTokenToFile(file, token);
        } else {
            Token token = recognizeOperatorOrSymbol(input, &position, line, column);
            if (token.token != UNKNOWN) {
                writeTokenToFile(file, token);
            } else {
                logLexicalError(file, input[position], line, column);
                position++;
            }
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