/*
 * Testes elaborados do módulo: lista
 * Testa a lista duplamente encadeada com todas as operações
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../src/lib/lista/lista.h"

/* ===================== MACROS DE TESTE ===================== */
#define TESTE_PASSOU(nome) printf("[PASSOU] %s\n", nome)
#define TESTE_FALHOU(nome, msg) printf("[FALHOU] %s: %s\n", nome, msg)

static int testes_passados = 0;
static int testes_falhados = 0;

/* ===================== TESTES ===================== */

/**
 * Teste 1: Inicialização da lista
 */
void teste_iniciar_lista() {
    const char* nome = "iniciar_lista";
    Lista list;
    
    iniciar_lista(&list);
    
    int tam = tamanho_lista(list);
    Posic primeiro = get_primeiro_lista(list);
    
    if (tam == 0 && primeiro == NULL) {
        TESTE_PASSOU(nome);
        testes_passados++;
    } else {
        TESTE_FALHOU(nome, "Lista deveria estar vazia após inicialização");
        testes_falhados++;
    }
    
    liberar_lista(&list);
}

/**
 * Teste 2: Inserção no início
 */
void teste_inserir_inicio() {
    const char* nome = "inserir_lista (início)";
    Lista list;
    iniciar_lista(&list);
    
    int valor1 = 10;
    int valor2 = 20;
    
    inserir_lista(&list, &valor1);
    inserir_lista(&list, &valor2);
    
    int tam = tamanho_lista(list);
    Posic primeiro = get_primeiro_lista(list);
    int* val = (int*)get_valor_lista(list, primeiro);
    
    if (tam == 2 && *val == 20) {
        TESTE_PASSOU(nome);
        testes_passados++;
    } else {
        TESTE_FALHOU(nome, "Inserção no início incorreta");
        testes_falhados++;
    }
    
    liberar_lista(&list);
}

/**
 * Teste 3: Inserção no fim
 */
void teste_inserir_fim() {
    const char* nome = "inserir_fim_lista";
    Lista list;
    iniciar_lista(&list);
    
    int valor1 = 10;
    int valor2 = 20;
    
    inserir_fim_lista(&list, &valor1);
    inserir_fim_lista(&list, &valor2);
    
    Posic ultimo = get_ultimo_lista(list);
    int* val = (int*)get_valor_lista(list, ultimo);
    
    if (*val == 20) {
        TESTE_PASSOU(nome);
        testes_passados++;
    } else {
        TESTE_FALHOU(nome, "Inserção no fim incorreta");
        testes_falhados++;
    }
    
    liberar_lista(&list);
}

/**
 * Teste 4: Remoção do início
 */
void teste_remover_inicio() {
    const char* nome = "remover_inicio_lista";
    Lista list;
    iniciar_lista(&list);
    
    int valor1 = 10;
    int valor2 = 20;
    
    inserir_lista(&list, &valor1);
    inserir_lista(&list, &valor2);
    
    void* removido;
    bool sucesso = remover_inicio_lista(&list, &removido);
    
    int tam = tamanho_lista(list);
    
    if (sucesso && *(int*)removido == 20 && tam == 1) {
        TESTE_PASSOU(nome);
        testes_passados++;
    } else {
        TESTE_FALHOU(nome, "Remoção do início incorreta");
        testes_falhados++;
    }
    
    liberar_lista(&list);
}

/**
 * Teste 5: Remoção de lista vazia
 */
void teste_remover_lista_vazia() {
    const char* nome = "remover_inicio_lista em lista vazia";
    Lista list;
    iniciar_lista(&list);
    
    void* removido;
    bool sucesso = remover_inicio_lista(&list, &removido);
    
    if (!sucesso) {
        TESTE_PASSOU(nome);
        testes_passados++;
    } else {
        TESTE_FALHOU(nome, "Não deveria remover de lista vazia");
        testes_falhados++;
    }
    
    liberar_lista(&list);
}

/**
 * Teste 6: Tamanho da lista
 */
void teste_tamanho_lista() {
    const char* nome = "tamanho_lista";
    Lista list;
    iniciar_lista(&list);
    
    int valores[] = {1, 2, 3, 4, 5};
    
    for (int i = 0; i < 5; i++) {
        inserir_lista(&list, &valores[i]);
    }
    
    int tam = tamanho_lista(list);
    
    if (tam == 5) {
        TESTE_PASSOU(nome);
        testes_passados++;
    } else {
        TESTE_FALHOU(nome, "Tamanho incorreto");
        testes_falhados++;
    }
    
    liberar_lista(&list);
}

/**
 * Teste 7: Navegação com get_proximo
 */
void teste_get_proximo() {
    const char* nome = "get_proximo_lista";
    Lista list;
    iniciar_lista(&list);
    
    int valores[] = {1, 2, 3};
    
    inserir_fim_lista(&list, &valores[0]);
    inserir_fim_lista(&list, &valores[1]);
    inserir_fim_lista(&list, &valores[2]);
    
    Posic p = get_primeiro_lista(list);
    int soma = 0;
    
    while (p != NULL) {
        int* val = (int*)get_valor_lista(list, p);
        soma += *val;
        p = get_proximo_lista(list, p);
    }
    
    if (soma == 6) {
        TESTE_PASSOU(nome);
        testes_passados++;
    } else {
        TESTE_FALHOU(nome, "Navegação com get_proximo incorreta");
        testes_falhados++;
    }
    
    liberar_lista(&list);
}

/**
 * Teste 8: Navegação com get_anterior
 */
void teste_get_anterior() {
    const char* nome = "get_anterior_lista";
    Lista list;
    iniciar_lista(&list);
    
    int valores[] = {1, 2, 3};
    
    inserir_fim_lista(&list, &valores[0]);
    inserir_fim_lista(&list, &valores[1]);
    inserir_fim_lista(&list, &valores[2]);
    
    Posic p = get_ultimo_lista(list);
    int soma = 0;
    
    while (p != NULL) {
        int* val = (int*)get_valor_lista(list, p);
        soma += *val;
        p = get_anterior_lista(list, p);
    }
    
    if (soma == 6) {
        TESTE_PASSOU(nome);
        testes_passados++;
    } else {
        TESTE_FALHOU(nome, "Navegação com get_anterior incorreta");
        testes_falhados++;
    }
    
    liberar_lista(&list);
}

/**
 * Teste 9: get_primeiro e get_ultimo
 */
void teste_primeiro_ultimo() {
    const char* nome = "get_primeiro_lista e get_ultimo_lista";
    Lista list;
    iniciar_lista(&list);
    
    int valor1 = 10;
    int valor2 = 20;
    int valor3 = 30;
    
    inserir_fim_lista(&list, &valor1);
    inserir_fim_lista(&list, &valor2);
    inserir_fim_lista(&list, &valor3);
    
    Posic primeiro = get_primeiro_lista(list);
    Posic ultimo = get_ultimo_lista(list);
    
    int* val_primeiro = (int*)get_valor_lista(list, primeiro);
    int* val_ultimo = (int*)get_valor_lista(list, ultimo);
    
    if (*val_primeiro == 10 && *val_ultimo == 30) {
        TESTE_PASSOU(nome);
        testes_passados++;
    } else {
        TESTE_FALHOU(nome, "Primeiro ou último incorreto");
        testes_falhados++;
    }
    
    liberar_lista(&list);
}

/**
 * Teste 10: Liberação da lista
 */
void teste_liberar_lista() {
    const char* nome = "liberar_lista";
    Lista list;
    iniciar_lista(&list);
    
    int valores[] = {1, 2, 3};
    
    for (int i = 0; i < 3; i++) {
        inserir_lista(&list, &valores[i]);
    }
    
    liberar_lista(&list);
    
    /* Após liberar, a lista deve estar resetada */
    if (list == NULL) {
        TESTE_PASSOU(nome);
        testes_passados++;
    } else {
        TESTE_FALHOU(nome, "Lista deveria ser NULL após liberação");
        testes_falhados++;
    }
}

/**
 * Teste 11: Inserção de muitos elementos
 */
void teste_muitos_elementos() {
    const char* nome = "inserir_lista com muitos elementos";
    Lista list;
    iniciar_lista(&list);
    
    int valores[100];
    for (int i = 0; i < 100; i++) {
        valores[i] = i;
        inserir_fim_lista(&list, &valores[i]);
    }
    
    int tam = tamanho_lista(list);
    Posic ultimo = get_ultimo_lista(list);
    int* val_ultimo = (int*)get_valor_lista(list, ultimo);
    
    if (tam == 100 && *val_ultimo == 99) {
        TESTE_PASSOU(nome);
        testes_passados++;
    } else {
        TESTE_FALHOU(nome, "Inserção de muitos elementos falhou");
        testes_falhados++;
    }
    
    liberar_lista(&list);
}

/* ===================== MAIN ===================== */
int main() {
    printf("========================================\n");
    printf("   TESTES DO MÓDULO LISTA\n");
    printf("========================================\n\n");
    
    teste_iniciar_lista();
    teste_inserir_inicio();
    teste_inserir_fim();
    teste_remover_inicio();
    teste_remover_lista_vazia();
    teste_tamanho_lista();
    teste_get_proximo();
    teste_get_anterior();
    teste_primeiro_ultimo();
    teste_liberar_lista();
    teste_muitos_elementos();
    
    printf("\n========================================\n");
    printf("   RESUMO: %d passados, %d falhados\n", testes_passados, testes_falhados);
    printf("========================================\n");
    
    return testes_falhados > 0 ? 1 : 0;
}
