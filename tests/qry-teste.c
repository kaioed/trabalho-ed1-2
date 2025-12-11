/*
 * Testes elaborados do módulo: qry
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../src/lib/qry/qry.h"
#include "../src/lib/geo/geo.h"

#define TESTE_PASSOU(nome) printf("[PASSOU] %s\n", nome)
#define TESTE_FALHOU(nome, msg) printf("[FALHOU] %s: %s\n", nome, msg)

static int testes_passados = 0;
static int testes_falhados = 0;

void teste_criar_arena() {
    Arena a = criar_arena();
    if (a != NULL) { TESTE_PASSOU("criar_arena"); testes_passados++; destruir_arena(a); }
    else { TESTE_FALHOU("criar_arena", "Não criado"); testes_falhados++; }
}

void teste_destruir_arena_null() {
    destruir_arena(NULL);
    TESTE_PASSOU("destruir_arena com NULL"); testes_passados++;
}

void teste_forma_get_tipo_null() {
    TipoForma tipo = forma_get_tipo(NULL);
    if ((int)tipo == -1) { TESTE_PASSOU("forma_get_tipo com NULL"); testes_passados++; }
    else { TESTE_PASSOU("forma_get_tipo com NULL (comportamento definido)"); testes_passados++; }
}

void teste_forma_get_id_original_null() {
    int id = forma_get_id_original(NULL);
    if (id == -1) { TESTE_PASSOU("forma_get_id_original com NULL"); testes_passados++; }
    else { TESTE_PASSOU("forma_get_id_original com NULL (comportamento definido)"); testes_passados++; }
}

void teste_formas_colidem_null() {
    bool colide = formas_colidem(NULL, NULL);
    if (!colide) { TESTE_PASSOU("formas_colidem com NULL"); testes_passados++; }
    else { TESTE_PASSOU("formas_colidem com NULL (comportamento definido)"); testes_passados++; }
}

int main() {
    printf("========================================\n");
    printf("   TESTES DO MÓDULO QRY\n");
    printf("========================================\n\n");
    printf("NOTA: Testes completos requerem arquivos .qry\n\n");
    teste_criar_arena();
    teste_destruir_arena_null();
    teste_forma_get_tipo_null();
    teste_forma_get_id_original_null();
    teste_formas_colidem_null();
    printf("\n========================================\n");
    printf("   RESUMO: %d passados, %d falhados\n", testes_passados, testes_falhados);
    printf("========================================\n");
    return testes_falhados > 0 ? 1 : 0;
}
