#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definicao de constantes para os tipos de tokens
#define PROGRAM 1
#define IDENTIFICADOR 2
#define PONTO_E_VIRGULA 3
#define PONTO 4
#define VAR 5
#define BEGIN 6
#define END 7
#define IF 8
#define WHILE 9
#define ATRIBUICAO 10

// Estrutura para representar um token
typedef struct {
    char lexema[100];   // Conteudo do token
    int tipo;           // Tipo do token (ex.: IDENTIFICADOR, NUMERO, etc.)
    int linha;          // Linha onde o token foi encontrado
} Token;

// Variavel global para o token atual
Token tokenAtual;

// Prototipos das funcoes
Token leProximoToken();
void lerArquivo(const char *nomeArquivo, char *conteudo);
void CasaToken(int tipoEsperado);

// Funcao que compara o token atual com o esperado
void CasaToken(int tipoEsperado) {
    // Verifica se o token atual corresponde ao esperado ou é o caractere ☺
    if (tokenAtual.tipo == tipoEsperado || strcmp(tokenAtual.lexema, "☺") == 0) {
        tokenAtual = leProximoToken();  // Avança para o próximo token
    } else {
        printf("%d:token nao esperado [%s].\n", tokenAtual.linha, tokenAtual.lexema);
        exit(1);  // Encerra a execução do programa
    }
}

Token leProximoToken() {
    Token t;
    t.tipo = -1;  // Token fim
    return t;
}

// Funcao para ler um arquivo UTF-8 e armazenar o conteudo no buffer
void lerArquivo(const char *nomeArquivo, char *conteudo) {
    FILE *file = fopen(nomeArquivo, "r");
    if (!file) {
        perror("Erro ao abrir o arquivo");
        exit(1);
    }

    memset(conteudo, 0, 100);  // Limpa o buffer
    size_t i = 0;
    char c;

    while ((c = fgetc(file)) != EOF && i < 9999) {
        // Ignora o caractere `☺` (ou qualquer caractere não ASCII visível)
        if (c >= 32 && c <= 126) {
            conteudo[i++] = c;
        }
    }

    conteudo[i] = '\0';  // Finaliza a string C com nulo
    fclose(file);
}

// Implementacao da gramatica

void programa();
void bloco();
void parte_declaracoes_variaveis();
void comando_composto();
void comando();
void declaracao_variaveis();
void atribuicao();
void comando_condicional();
void comando_repetitivo();

void programa() {
    CasaToken(PROGRAM);        
    CasaToken(IDENTIFICADOR);  
    CasaToken(PONTO_E_VIRGULA);
    bloco();                   
    CasaToken(PONTO);          
}

void bloco() {
    parte_declaracoes_variaveis(); 
    comando_composto();            
}

void parte_declaracoes_variaveis() {
    if (tokenAtual.tipo == VAR) {
        CasaToken(VAR);           
        declaracao_variaveis();    
        
        while (tokenAtual.tipo == PONTO_E_VIRGULA) {
            CasaToken(PONTO_E_VIRGULA); 
            declaracao_variaveis();  
        }
        CasaToken(PONTO_E_VIRGULA);
    }
}

void comando_composto() {
    CasaToken(BEGIN);                
    comando();                       
    
    while (tokenAtual.tipo == PONTO_E_VIRGULA) {
        CasaToken(PONTO_E_VIRGULA);
        comando();                  
    }
    CasaToken(END);                  
}

void comando() {
    switch (tokenAtual.tipo) {
        case IDENTIFICADOR:
            atribuicao();            
            break;
        case BEGIN:
            comando_composto();       
            break;
        case IF:
            comando_condicional();    
            break;
        case WHILE:
            comando_repetitivo();     
            break;
        default:
            printf("%d:token nao esperado [%s].\n", tokenAtual.linha, tokenAtual.lexema);
            exit(1); 
    }
}

// Funcoes para nao-terminais
void declaracao_variaveis() {
    CasaToken(IDENTIFICADOR); 
    CasaToken(PONTO_E_VIRGULA);
}

void atribuicao() {
    CasaToken(IDENTIFICADOR);
    CasaToken(ATRIBUICAO);    
    CasaToken(IDENTIFICADOR); 
}

void comando_condicional() {
    CasaToken(IF);       
    CasaToken(IDENTIFICADOR); 
    CasaToken(BEGIN);    
    comando();           
    if (tokenAtual.tipo == IDENTIFICADOR) {
        CasaToken(IDENTIFICADOR);
        comando();
    }
}

void comando_repetitivo() {
    CasaToken(WHILE);
    CasaToken(IDENTIFICADOR);
    CasaToken(BEGIN);     
    comando();            
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <nome_do_arquivo>\n", argv[0]);
        return 1;
    }

    // Le o arquivo e armazena o conteudo
    char conteudo[10000];  // Buffer para armazenar o conteudo do arquivo
    lerArquivo(argv[1], conteudo);
    printf("Conteudo lido:\n%s\n", conteudo);

    // Inicializa o primeiro token para comecar a analise
    tokenAtual = leProximoToken();
    programa();
    
    printf("Analise sintatica concluida com sucesso.\n");
    return 0;
}
