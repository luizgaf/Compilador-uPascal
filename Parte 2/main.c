#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_VARS 100   // Máximo de variáveis que podem ser declaradas
#define TAM_LEXEMA 100 // Tamanho máximo do lexema

// Estrutura para o token
typedef struct {
    char lexema[TAM_LEXEMA];
    int tipo;
    int linha;
} Token;

// Variáveis globais
Token tokenCorrente;
FILE *arquivo;
int linhaAtual = 1;
char variaveisDeclaradas[MAX_VARS][TAM_LEXEMA]; // Lista de variáveis declaradas
int contadorVariaveis = 0;                      // Contador de variáveis declaradas

// Protótipos das funções
void AnalisadorLexico();
void CasaToken(int tokenEsperado);
void Programa();
void DeclaracaoVariavel();
void BlocoComandos();
void Comando();
void Expressao();
void RegistrarVariavel(const char *nome);
int ValidarVariavel(const char *nome);

// Constantes para tipos de tokens
enum {
    TOKEN_ID,
    TOKEN_NUM,
    TOKEN_PONTO_VIRGULA,
    TOKEN_VIRGULA,
    TOKEN_DOIS_PONTOS,
    TOKEN_ATRIBUICAO,
    TOKEN_IGUAL,
    TOKEN_INICIO,
    TOKEN_FIM,
    TOKEN_PROGRAMA,
    TOKEN_VAR,
    TOKEN_PONTO,
    TOKEN_INTEGER,
    TOKEN_REAL,
    TOKEN_ERRO,
    TOKEN_FIM_ARQUIVO
};

// Função para verificar se uma string é uma palavra reservada
int palavraReservada(char *lexema) {
    if (strcmp(lexema, "program") == 0) return TOKEN_PROGRAMA;
    if (strcmp(lexema, "var") == 0) return TOKEN_VAR;
    if (strcmp(lexema, "begin") == 0) return TOKEN_INICIO;
    if (strcmp(lexema, "end") == 0) return TOKEN_FIM;
    if (strcmp(lexema, "integer") == 0) return TOKEN_INTEGER;
    if (strcmp(lexema, "real") == 0) return TOKEN_REAL;
    return TOKEN_ID;
}

// Função para ler o próximo token do arquivo
void AnalisadorLexico() {
    char lexema[TAM_LEXEMA];
    int c = fgetc(arquivo);

    // Ignora espaços e novas linhas
    while (isspace(c)) {
        if (c == '\n') linhaAtual++;
        c = fgetc(arquivo);
    }

    if (c == EOF) {
        tokenCorrente.tipo = TOKEN_FIM_ARQUIVO;
        strcpy(tokenCorrente.lexema, "EOF");
        tokenCorrente.linha = linhaAtual;
        return;
    }

    // Verifica se é uma letra para um identificador ou palavra reservada
    if (isalpha(c)) {
        int i = 0;
        while (isalnum(c)) {  // Permite letras e números no identificador
            lexema[i++] = c;
            c = fgetc(arquivo);
        }
        lexema[i] = '\0';
        ungetc(c, arquivo);  // Retorna o caractere que não faz parte do lexema

        // Verifica se o lexema é uma palavra reservada
        int tipoToken = palavraReservada(lexema);
        if (tipoToken != TOKEN_ID) {
            tokenCorrente.tipo = tipoToken;  // Palavra reservada
        } else {
            tokenCorrente.tipo = TOKEN_ID;   // Identificador (nome de variável)
        }
        strcpy(tokenCorrente.lexema, lexema);
        tokenCorrente.linha = linhaAtual;
    }
    // Verifica se é um número
    else if (isdigit(c)) {
        int i = 0;
        int pontoEncontrado = 0;
        
        // Processa a parte inteira
        while (isdigit(c)) {
            lexema[i++] = c;
            c = fgetc(arquivo);
        }
        
        // Verifica se é um ponto decimal
        if (c == '.') {
            pontoEncontrado = 1;
            lexema[i++] = c;
            c = fgetc(arquivo);
            
            // Processa a parte decimal
            while (isdigit(c)) {
                lexema[i++] = c;
                c = fgetc(arquivo);
            }
        }
        
        lexema[i] = '\0';
        ungetc(c, arquivo); // Retorna o último caractere que não faz parte do número

        // Define o tipo do token com base na presença de ponto decimal
        if (pontoEncontrado) {
            tokenCorrente.tipo = TOKEN_NUM;  // Use um tipo específico, ex.: TOKEN_REAL, se preferir
        } else {
            tokenCorrente.tipo = TOKEN_NUM;  // Ou TOKEN_INTEGER, se preferir
        }
        strcpy(tokenCorrente.lexema, lexema);
        tokenCorrente.linha = linhaAtual;
    }
    // Verifica operadores e pontuação
    else if (c == ';') {
        tokenCorrente.tipo = TOKEN_PONTO_VIRGULA;
        strcpy(tokenCorrente.lexema, ";");
        tokenCorrente.linha = linhaAtual;
    } else if (c == ',') {
        tokenCorrente.tipo = TOKEN_VIRGULA;
        strcpy(tokenCorrente.lexema, ",");
        tokenCorrente.linha = linhaAtual;
    } else if (c == ':') {
        // Verifica se é :=
        int next = fgetc(arquivo);
        if (next == '=') {
            tokenCorrente.tipo = TOKEN_ATRIBUICAO;  // Define := como operador de atribuição
            strcpy(tokenCorrente.lexema, ":=");
        } else {
            tokenCorrente.tipo = TOKEN_DOIS_PONTOS;
            strcpy(tokenCorrente.lexema, ":");
            ungetc(next, arquivo);  // Retorna o caractere que não faz parte do lexema
        }
        tokenCorrente.linha = linhaAtual;
    } else if (c == '=') {
        tokenCorrente.tipo = TOKEN_IGUAL;
        strcpy(tokenCorrente.lexema, "=");
        tokenCorrente.linha = linhaAtual;
    } else if (c == '.') {
        tokenCorrente.tipo = TOKEN_PONTO;
        strcpy(tokenCorrente.lexema, ".");
        tokenCorrente.linha = linhaAtual;
    } else {
        tokenCorrente.tipo = TOKEN_ERRO;
        strcpy(tokenCorrente.lexema, "ERRO");
        tokenCorrente.linha = linhaAtual;
    }
}

// Função para registrar uma variável
void RegistrarVariavel(const char *nome) {
    if (contadorVariaveis < MAX_VARS) {
        strcpy(variaveisDeclaradas[contadorVariaveis], nome);
        contadorVariaveis++;
    } else {
        fprintf(stderr, "Erro: numero maximo de variaveis declaradas excedido.\n");
        exit(1);
    }
}

// Função para validar se uma variável foi declarada
int ValidarVariavel(const char *nome) {
    for (int i = 0; i < contadorVariaveis; i++) {
        if (strcmp(variaveisDeclaradas[i], nome) == 0) {
            return 1; // Variável encontrada
        }
    }
    return 0; // Variável não encontrada
}

// Função que compara o token atual com o esperado
void CasaToken(int tokenEsperado) {
    if (tokenCorrente.tipo == tokenEsperado) {
        AnalisadorLexico(); // Lê o próximo token
    } else {
        printf("%d:token nao esperado [%s].\n", tokenCorrente.linha, tokenCorrente.lexema);
        exit(1);
    }
}

// Função para a regra inicial do programa
void Programa() {
    CasaToken(TOKEN_PROGRAMA);
    CasaToken(TOKEN_ID);            // Nome do programa
    CasaToken(TOKEN_PONTO_VIRGULA);
    DeclaracaoVariavel();
    CasaToken(TOKEN_INICIO);
    BlocoComandos();
    CasaToken(TOKEN_FIM);
    CasaToken(TOKEN_PONTO);
}

// Função para declaração de variáveis
void DeclaracaoVariavel() {
    if (tokenCorrente.tipo == TOKEN_VAR) {
        CasaToken(TOKEN_VAR);
        do {
            CasaToken(TOKEN_ID);  // Primeiro identificador
            RegistrarVariavel(tokenCorrente.lexema); // Registra a variável
            while (tokenCorrente.tipo == TOKEN_VIRGULA) {  // Se houver uma vírgula, espera mais um identificador
                CasaToken(TOKEN_VIRGULA);
                CasaToken(TOKEN_ID);  // Próximo identificador
                RegistrarVariavel(tokenCorrente.lexema); // Registra a variável
            }
            CasaToken(TOKEN_DOIS_PONTOS);
            if (tokenCorrente.tipo == TOKEN_INTEGER || tokenCorrente.tipo == TOKEN_REAL) {
                CasaToken(tokenCorrente.tipo);  // Consome o tipo da variável
            } else {
                printf("%d:tipo de variavel invalido [%s].\n", tokenCorrente.linha, tokenCorrente.lexema);
                exit(1);
            }
            CasaToken(TOKEN_PONTO_VIRGULA);
        } while (tokenCorrente.tipo == TOKEN_ID);  // Continua enquanto houver mais declarações de variáveis
    }
}

// Função para bloco de comandos (para permitir múltiplos comandos)
void BlocoComandos() {
    while (tokenCorrente.tipo == TOKEN_ID) {
        Comando();
    }
}

// Função para comando
void Comando() {
    if (tokenCorrente.tipo == TOKEN_ID) {
        if (!ValidarVariavel(tokenCorrente.lexema)) {
            printf("%d:variavel nao declarada [%s].\n", tokenCorrente.linha, tokenCorrente.lexema);
            exit(1);
        }
        CasaToken(TOKEN_ID);
        CasaToken(TOKEN_ATRIBUICAO);  // Atribuição deve ser :=
        Expressao();
        CasaToken(TOKEN_PONTO_VIRGULA);
    } else {
        printf("%d:comando inesperado [%s].\n", tokenCorrente.linha, tokenCorrente.lexema);
        exit(1);
    }
}

// Função para expressão (exemplo simples)
void Expressao() {
    if (tokenCorrente.tipo == TOKEN_ID) {
        if (!ValidarVariavel(tokenCorrente.lexema)) {
            printf("%d:variavel nao declarada [%s].\n", tokenCorrente.linha, tokenCorrente.lexema);
            exit(1);
        }
        CasaToken(TOKEN_ID);
    } else if (tokenCorrente.tipo == TOKEN_NUM) {
        CasaToken(TOKEN_NUM);
    } else {
        printf("%d:token nao esperado [%s].\n", tokenCorrente.linha, tokenCorrente.lexema);
        exit(1);
    }
}

// Função principal
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <arquivo.pas>\n", argv[0]);
        return 1;
    }
    
    // Abre o arquivo para leitura
    arquivo = fopen(argv[1], "r");
    if (!arquivo) {
        fprintf(stderr, "Erro ao abrir o arquivo %s\n", argv[1]);
        return 1;
    }
    
    AnalisadorLexico(); // Lê o primeiro token
    Programa();         // Chama o símbolo inicial da gramática
    
    if (tokenCorrente.tipo != TOKEN_FIM_ARQUIVO) {
        printf("%d:fim de arquivo nao esperado.\n", tokenCorrente.linha);
    } else {
        printf("Análise sintática concluída com sucesso.\n");
    }
    
    fclose(arquivo);
    return 0;
}
