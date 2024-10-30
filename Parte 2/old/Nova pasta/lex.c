#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "lex.h"
#include "tokens.h"

const char *getFileNameFromArgs(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <nome_do_arquivo>\n", argv[0]);
        exit(1);  
    }
    return argv[1];  
}

int main(int argc, char *argv[]) {
    ensureOutputDirectoryExists();

    const char *fileName = getFileNameFromArgs(argc, argv);

    FILE *file = fopen(fileName, "r");
    FILE *fileExit = fopen("./output/output.lex", "w");

    if (file == NULL) {
        printf("Erro ao abrir o arquivo de entrada.\n");
        return 1;
    }
    if (fileExit == NULL) {
        printf("Erro ao abrir o arquivo de saida.\n");
        fclose(file);
        return 1;
    }

    Token *token = NULL;
    Token *initialToken = NULL;
    Table *table = NULL;

    // Inicializa a leitura dos tokens
    do {
        token = getToken(file, token); // Recebe o proximo token
        if (token->previous == NULL)
            initialToken = token; // Salva o primeiro token como inicial
    } while (token->type != END_OF_FILE);

    // Imprime tokens no arquivo de saida e na tabela
    printFile(initialToken, fileExit);
    printTable(initialToken, &table);
    printf("\nO resultado do analisador lexico pode ser encontrado na pasta output\n\n");

    // Fecha os arquivos abertos
    fclose(file);
    fclose(fileExit);

    return 0;
}
