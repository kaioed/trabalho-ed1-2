/*
 * Testes elaborados do módulo: arg
 * Testa as funções de parsing de argumentos de linha de comando
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../src/lib/arg/args.h"

/* ===================== MACROS DE TESTE ===================== */
#define TESTE_PASSOU(nome) printf("[PASSOU] %s\n", nome)
#define TESTE_FALHOU(nome, msg) printf("[FALHOU] %s: %s\n", nome, msg)

static int testes_passados = 0;
static int testes_falhados = 0;

/* ===================== TESTES ===================== */

/**
 * Teste 1: obter_valor_opcao com opção existente
 */
void teste_obter_valor_opcao_existente() {
    const char* nome = "obter_valor_opcao com opção existente";
    char* argv[] = {"programa", "-e", "entrada.txt", "-o", "saida.txt"};
    int argc = 5;
    
    char* valor = obter_valor_opcao(argc, argv, "e");
    
    if (valor != NULL && strcmp(valor, "entrada.txt") == 0) {
        TESTE_PASSOU(nome);
        testes_passados++;
    } else {
        TESTE_FALHOU(nome, "Valor retornado incorreto para opção -e");
        testes_falhados++;
    }
}

/**
 * Teste 2: obter_valor_opcao com segunda opção
 */
void teste_obter_valor_opcao_segunda() {
    const char* nome = "obter_valor_opcao com segunda opção";
    char* argv[] = {"programa", "-e", "entrada.txt", "-o", "saida.txt"};
    int argc = 5;
    
    char* valor = obter_valor_opcao(argc, argv, "o");
    
    if (valor != NULL && strcmp(valor, "saida.txt") == 0) {
        TESTE_PASSOU(nome);
        testes_passados++;
    } else {
        TESTE_FALHOU(nome, "Valor retornado incorreto para opção -o");
        testes_falhados++;
    }
}

/**
 * Teste 3: obter_valor_opcao com opção inexistente
 */
void teste_obter_valor_opcao_inexistente() {
    const char* nome = "obter_valor_opcao com opção inexistente";
    char* argv[] = {"programa", "-e", "entrada.txt"};
    int argc = 3;
    
    char* valor = obter_valor_opcao(argc, argv, "x");
    
    if (valor == NULL) {
        TESTE_PASSOU(nome);
        testes_passados++;
    } else {
        TESTE_FALHOU(nome, "Deveria retornar NULL para opção inexistente");
        testes_falhados++;
    }
}

/**
 * Teste 4: obter_sufixo_comando com sufixo válido
 */
void teste_obter_sufixo_comando_valido() {
    const char* nome = "obter_sufixo_comando com sufixo válido";
    char* argv[] = {"programa", "-e", "entrada.txt", "sufixo123"};
    int argc = 4;
    
    char* sufixo = obter_sufixo_comando(argc, argv);
    
    if (sufixo != NULL && strcmp(sufixo, "sufixo123") == 0) {
        TESTE_PASSOU(nome);
        testes_passados++;
    } else {
        TESTE_FALHOU(nome, "Sufixo retornado incorreto");
        testes_falhados++;
    }
}

/**
 * Teste 5: obter_sufixo_comando sem sufixo
 */
void teste_obter_sufixo_comando_sem_sufixo() {
    const char* nome = "obter_sufixo_comando sem sufixo";
    char* argv[] = {"programa", "-e", "entrada.txt", "-o", "saida.txt"};
    int argc = 5;
    
    char* sufixo = obter_sufixo_comando(argc, argv);
    
    if (sufixo == NULL) {
        TESTE_PASSOU(nome);
        testes_passados++;
    } else {
        TESTE_FALHOU(nome, "Deveria retornar NULL quando não há sufixo");
        testes_falhados++;
    }
}

/**
 * Teste 6: obter_valor_opcao com múltiplas opções
 */
void teste_obter_valor_opcao_multiplas() {
    const char* nome = "obter_valor_opcao com múltiplas opções";
    char* argv[] = {"programa", "-a", "val_a", "-b", "val_b", "-c", "val_c"};
    int argc = 7;
    
    char* val_a = obter_valor_opcao(argc, argv, "a");
    char* val_b = obter_valor_opcao(argc, argv, "b");
    char* val_c = obter_valor_opcao(argc, argv, "c");
    
    if (val_a && strcmp(val_a, "val_a") == 0 &&
        val_b && strcmp(val_b, "val_b") == 0 &&
        val_c && strcmp(val_c, "val_c") == 0) {
        TESTE_PASSOU(nome);
        testes_passados++;
    } else {
        TESTE_FALHOU(nome, "Algum valor de opção incorreto");
        testes_falhados++;
    }
}

/**
 * Teste 7: argv vazio (apenas nome do programa)
 */
void teste_argv_vazio() {
    const char* nome = "obter_valor_opcao com argv vazio";
    char* argv[] = {"programa"};
    int argc = 1;
    
    char* valor = obter_valor_opcao(argc, argv, "e");
    
    if (valor == NULL) {
        TESTE_PASSOU(nome);
        testes_passados++;
    } else {
        TESTE_FALHOU(nome, "Deveria retornar NULL para argv vazio");
        testes_falhados++;
    }
}

/* ===================== MAIN ===================== */
int main() {
    printf("========================================\n");
    printf("   TESTES DO MÓDULO ARG\n");
    printf("========================================\n\n");
    
    teste_obter_valor_opcao_existente();
    teste_obter_valor_opcao_segunda();
    teste_obter_valor_opcao_inexistente();
    teste_obter_sufixo_comando_valido();
    teste_obter_sufixo_comando_sem_sufixo();
    teste_obter_valor_opcao_multiplas();
    teste_argv_vazio();
    
    printf("\n========================================\n");
    printf("   RESUMO: %d passados, %d falhados\n", testes_passados, testes_falhados);
    printf("========================================\n");
    
    return testes_falhados > 0 ? 1 : 0;
}
