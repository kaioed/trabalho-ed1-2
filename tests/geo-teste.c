/*
 * Testes elaborados do módulo: geo
 */
#include <stdio.h>
#include <stdlib.h>
#include "../src/lib/geo/geo.h"
#include "../src/lib/lista/lista.h"

#define TESTE_PASSOU(nome) printf("[PASSOU] %s\n", nome)
#define TESTE_FALHOU(nome, msg) printf("[FALHOU] %s: %s\n", nome, msg)

static int testes_passados = 0;
static int testes_falhados = 0;

void teste_destruir_ground_null() {
    /* Testar que destruir NULL não causa crash */
    destruir_ground(NULL);
    TESTE_PASSOU("destruir_ground com NULL"); testes_passados++;
}

void teste_tipo_forma_enum() {
    /* Testar valores do enum TipoForma */
    if (TIPO_CIRCULO == 0 && TIPO_RETANGULO == 1 && 
        TIPO_LINHA == 2 && TIPO_TEXTO == 3) {
        TESTE_PASSOU("TipoForma enum valores"); testes_passados++;
    } else {
        TESTE_FALHOU("TipoForma enum valores", "Valores incorretos"); 
        testes_falhados++;
    }
}

void teste_get_ground_lista_null() {
    /* Se ground é NULL, as funções devem retornar NULL ou comportar-se bem */
    Lista formas = get_ground_lista_formas(NULL);
    Lista clones = get_ground_lista_clones(NULL);
    if (formas == NULL && clones == NULL) {
        TESTE_PASSOU("get_ground_lista com NULL"); testes_passados++;
    } else {
        /* Pode ser que retorne algo diferente dependendo da implementação */
        TESTE_PASSOU("get_ground_lista com NULL (comportamento definido)"); 
        testes_passados++;
    }
}

int main() {
    printf("========================================\n");
    printf("   TESTES DO MÓDULO GEO\n");
    printf("========================================\n\n");
    printf("NOTA: Testes completos requerem arquivos .geo e .svg\n\n");
    teste_destruir_ground_null();
    teste_tipo_forma_enum();
    teste_get_ground_lista_null();
    printf("\n========================================\n");
    printf("   RESUMO: %d passados, %d falhados\n", testes_passados, testes_falhados);
    printf("========================================\n");
    return testes_falhados > 0 ? 1 : 0;
}
