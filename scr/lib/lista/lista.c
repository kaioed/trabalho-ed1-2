#include <stdlib.h>
#include <stdio.h>
#include "lista.h"

// Estrutura do Nó (Elemento)
typedef struct elemento {
    void* valor;
    struct elemento* proximo;
    struct elemento* anterior;
} Elemento;

// Estrutura da Lista (Descritor)
typedef struct lista_impl {
    Elemento* inicio;
    Elemento* fim;
    int tamanho;
} ListaStruct;


void iniciar_lista(Lista* list) {
    ListaStruct* l = (ListaStruct*) malloc(sizeof(ListaStruct));
    if (l) {
        l->inicio = NULL;
        l->fim = NULL;
        l->tamanho = 0;
    }
    *list = (Lista)l;
}

void liberar_lista(Lista* list) {
    if (!list || !*list) return;
    
    ListaStruct* l = (ListaStruct*)*list;
    Elemento* atual = l->inicio;
    
    while (atual != NULL) {
        Elemento* temp = atual;
        atual = atual->proximo;
        free(temp);
    }
    free(l);
    *list = NULL;
}

void inserir_lista(Lista* list, void* value) {
    if (!list || !*list) return;
    ListaStruct* l = (ListaStruct*)*list;
    
    Elemento* novo = (Elemento*) malloc(sizeof(Elemento));
    novo->valor = value;
    novo->proximo = l->inicio;
    novo->anterior = NULL;
    
    if (l->inicio != NULL) {
        l->inicio->anterior = novo;
    } else {
        l->fim = novo;
    }
    l->inicio = novo;
    l->tamanho++;
}

void inserir_fim_lista(Lista* list, void* value) {
    if (!list || !*list) return;
    ListaStruct* l = (ListaStruct*)*list;

    Elemento* novo = (Elemento*) malloc(sizeof(Elemento));
    novo->valor = value;
    novo->proximo = NULL;
    novo->anterior = l->fim;

    if (l->fim != NULL) {
        l->fim->proximo = novo;
    } else {
        l->inicio = novo;
    }
    l->fim = novo;
    l->tamanho++;
}

bool remover_inicio_lista(Lista* list, void** out_value) {
    if (!list || !*list) return false;
    ListaStruct* l = (ListaStruct*)*list;
    
    if (l->inicio == NULL) return false;
    
    Elemento* removido = l->inicio;
    l->inicio = removido->proximo;
    
    if (l->inicio != NULL) {
        l->inicio->anterior = NULL;
    } else {
        l->fim = NULL;
    }
    
    if (out_value) *out_value = removido->valor;
    free(removido);
    l->tamanho--;
    return true;
}

int tamanho_lista(Lista list) {
    if (!list) return 0;
    return ((ListaStruct*)list)->tamanho;
}

Posic get_primeiro_lista(Lista list) {
    if (!list) return NULL;
    return (Posic)((ListaStruct*)list)->inicio;
}

Posic get_proximo_lista(Lista list, Posic p) {
    if (!list || !p) return NULL;
    return (Posic)((Elemento*)p)->proximo;
}

Posic get_ultimo_lista(Lista list) {
    if (!list) return NULL;
    return (Posic)((ListaStruct*)list)->fim;
}

Posic get_anterior_lista(Lista list, Posic p) {
    if (!list || !p) return NULL;
    return (Posic)((Elemento*)p)->anterior;
}

void* get_valor_lista(Lista list, Posic p) {
    if (!list || !p) return NULL;
    return ((Elemento*)p)->valor;
}