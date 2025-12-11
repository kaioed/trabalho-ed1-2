/*
 * Testes elaborados do módulo: arvore (tree)
 * Testa a árvore binária de busca com inserção, remoção, busca e iteração
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../src/lib/arvore/tree.h"

/* ===================== MACROS DE TESTE ===================== */
#define TESTE_PASSOU(nome) printf("[PASSOU] %s\n", nome)
#define TESTE_FALHOU(nome, msg) printf("[FALHOU] %s: %s\n", nome, msg)

static int testes_passados = 0;
static int testes_falhados = 0;

/* ===================== DADOS DE TESTE ===================== */
typedef struct {
    int valor;
    char nome[20];
} DadoTeste;

DadoTeste* criar_dado(int valor, const char* nome) {
    DadoTeste* d = malloc(sizeof(DadoTeste));
    d->valor = valor;
    strncpy(d->nome, nome, 19);
    d->nome[19] = '\0';
    return d;
}

/* ===================== CALLBACK PARA ITERAÇÃO ===================== */
static int contador_iteracao = 0;
static int soma_valores = 0;

void callback_contar(void* data, void* ctx) {
    (void)ctx;
    DadoTeste* d = (DadoTeste*)data;
    contador_iteracao++;
    soma_valores += d->valor;
}

/* ===================== TESTES ===================== */

/**
 * Teste 1: Inicialização da árvore
 */
void teste_iniciar_tree() {
    const char* nome = "iniciar_tree";
    TreeNode root;
    
    iniciar_tree(&root);
    
    if (root == NULL) {
        TESTE_PASSOU(nome);
        testes_passados++;
    } else {
        TESTE_FALHOU(nome, "Árvore deveria estar vazia após inicialização");
        testes_falhados++;
    }
    
    liberar_tree(&root);
}

/**
 * Teste 2: Inserção simples
 */
void teste_inserir_simples() {
    const char* nome = "inserir_tree simples";
    TreeNode root;
    iniciar_tree(&root);
    
    DadoTeste* d1 = criar_dado(10, "dado1");
    inserir_tree(&root, 5.0, d1);
    
    void* min = buscar_min_tree(root);
    
    if (min == d1) {
        TESTE_PASSOU(nome);
        testes_passados++;
    } else {
        TESTE_FALHOU(nome, "Elemento inserido não encontrado");
        testes_falhados++;
    }
    
    free(d1);
    liberar_tree(&root);
}

/**
 * Teste 3: Inserção múltipla e busca do mínimo
 */
void teste_buscar_min_multiplos() {
    const char* nome = "buscar_min_tree com múltiplos elementos";
    TreeNode root;
    iniciar_tree(&root);
    
    DadoTeste* d1 = criar_dado(30, "maior");
    DadoTeste* d2 = criar_dado(10, "menor");
    DadoTeste* d3 = criar_dado(20, "medio");
    
    inserir_tree(&root, 30.0, d1);
    inserir_tree(&root, 10.0, d2);
    inserir_tree(&root, 20.0, d3);
    
    void* min = buscar_min_tree(root);
    
    if (min == d2) {
        TESTE_PASSOU(nome);
        testes_passados++;
    } else {
        TESTE_FALHOU(nome, "Mínimo incorreto");
        testes_falhados++;
    }
    
    free(d1);
    free(d2);
    free(d3);
    liberar_tree(&root);
}

/**
 * Teste 4: Remoção de elemento
 */
void teste_remover_elemento() {
    const char* nome = "remover_tree elemento existente";
    TreeNode root;
    iniciar_tree(&root);
    
    DadoTeste* d1 = criar_dado(10, "dado1");
    DadoTeste* d2 = criar_dado(20, "dado2");
    
    inserir_tree(&root, 10.0, d1);
    inserir_tree(&root, 20.0, d2);
    
    bool removido = remover_tree(&root, 10.0, d1);
    void* min = buscar_min_tree(root);
    
    if (removido && min == d2) {
        TESTE_PASSOU(nome);
        testes_passados++;
    } else {
        TESTE_FALHOU(nome, "Remoção falhou ou mínimo incorreto após remoção");
        testes_falhados++;
    }
    
    free(d1);
    free(d2);
    liberar_tree(&root);
}

/**
 * Teste 5: Remoção de elemento inexistente
 */
void teste_remover_inexistente() {
    const char* nome = "remover_tree elemento inexistente";
    TreeNode root;
    iniciar_tree(&root);
    
    DadoTeste* d1 = criar_dado(10, "dado1");
    DadoTeste* d2 = criar_dado(99, "fantasma");
    
    inserir_tree(&root, 10.0, d1);
    
    bool removido = remover_tree(&root, 99.0, d2);
    
    if (!removido) {
        TESTE_PASSOU(nome);
        testes_passados++;
    } else {
        TESTE_FALHOU(nome, "Não deveria remover elemento inexistente");
        testes_falhados++;
    }
    
    free(d1);
    free(d2);
    liberar_tree(&root);
}

/**
 * Teste 6: Buscar mínimo em árvore vazia
 */
void teste_buscar_min_vazia() {
    const char* nome = "buscar_min_tree em árvore vazia";
    TreeNode root;
    iniciar_tree(&root);
    
    void* min = buscar_min_tree(root);
    
    if (min == NULL) {
        TESTE_PASSOU(nome);
        testes_passados++;
    } else {
        TESTE_FALHOU(nome, "Deveria retornar NULL para árvore vazia");
        testes_falhados++;
    }
    
    liberar_tree(&root);
}

/**
 * Teste 7: Iteração sobre a árvore
 */
void teste_tree_iterar() {
    const char* nome = "tree_iterar sobre elementos";
    TreeNode root;
    iniciar_tree(&root);
    
    DadoTeste* d1 = criar_dado(1, "um");
    DadoTeste* d2 = criar_dado(2, "dois");
    DadoTeste* d3 = criar_dado(3, "tres");
    
    inserir_tree(&root, 1.0, d1);
    inserir_tree(&root, 2.0, d2);
    inserir_tree(&root, 3.0, d3);
    
    contador_iteracao = 0;
    soma_valores = 0;
    tree_iterar(root, callback_contar, NULL);
    
    if (contador_iteracao == 3 && soma_valores == 6) {
        TESTE_PASSOU(nome);
        testes_passados++;
    } else {
        TESTE_FALHOU(nome, "Iteração não visitou todos os elementos corretamente");
        testes_falhados++;
    }
    
    free(d1);
    free(d2);
    free(d3);
    liberar_tree(&root);
}

/**
 * Teste 8: Chaves duplicadas
 */
void teste_chaves_duplicadas() {
    const char* nome = "inserir_tree com chaves duplicadas";
    TreeNode root;
    iniciar_tree(&root);
    
    DadoTeste* d1 = criar_dado(1, "primeiro");
    DadoTeste* d2 = criar_dado(2, "segundo");
    
    inserir_tree(&root, 5.0, d1);
    inserir_tree(&root, 5.0, d2);
    
    contador_iteracao = 0;
    tree_iterar(root, callback_contar, NULL);
    
    if (contador_iteracao == 2) {
        TESTE_PASSOU(nome);
        testes_passados++;
    } else {
        TESTE_FALHOU(nome, "Deveria permitir chaves duplicadas");
        testes_falhados++;
    }
    
    free(d1);
    free(d2);
    liberar_tree(&root);
}

/**
 * Teste 9: Liberação da árvore
 */
void teste_liberar_tree() {
    const char* nome = "liberar_tree";
    TreeNode root;
    iniciar_tree(&root);
    
    DadoTeste* d1 = criar_dado(10, "dado1");
    DadoTeste* d2 = criar_dado(20, "dado2");
    
    inserir_tree(&root, 10.0, d1);
    inserir_tree(&root, 20.0, d2);
    
    liberar_tree(&root);
    
    if (root == NULL) {
        TESTE_PASSOU(nome);
        testes_passados++;
    } else {
        TESTE_FALHOU(nome, "Raiz deveria ser NULL após liberação");
        testes_falhados++;
    }
    
    free(d1);
    free(d2);
}

/* ===================== MAIN ===================== */
int main() {
    printf("========================================\n");
    printf("   TESTES DO MÓDULO ARVORE (TREE)\n");
    printf("========================================\n\n");
    
    teste_iniciar_tree();
    teste_inserir_simples();
    teste_buscar_min_multiplos();
    teste_remover_elemento();
    teste_remover_inexistente();
    teste_buscar_min_vazia();
    teste_tree_iterar();
    teste_chaves_duplicadas();
    teste_liberar_tree();
    
    printf("\n========================================\n");
    printf("   RESUMO: %d passados, %d falhados\n", testes_passados, testes_falhados);
    printf("========================================\n");
    
    return testes_falhados > 0 ? 1 : 0;
}
