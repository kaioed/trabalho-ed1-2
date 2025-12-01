#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "qry.h"
#include "../lib/geo/geo.h"
#include "../lib/formas/circulo/circulo.h"
#include "../lib/formas/retangulo/retangulo.h"
#include "../lib/formas/linha/linha.h"
#include "../lib/formas/texto/texto.h"

typedef struct
{
    int id_original;
    TipoForma tipo;
    void *dados_forma;
    bool foi_destruida;
    bool foi_clonada;
    float x_landed;
    float y_landed;
} FormaStruct;

static int proximo_id_clone = 10000;
static int total_disparos = 0;
static int total_formas_esmagadas = 0;
static int total_formas_clonadas = 0;
static double pontuacao_final = 0.0;

static float calcular_area_forma(FormaStruct *f)
{
    if (!f || !f->dados_forma)
        return 0.0f;
    switch (f->tipo)
    {
    case TIPO_CIRCULO:
        return area_circulo(f->dados_forma);
    case TIPO_RETANGULO:
        return area_retangulo(f->dados_forma);
    case TIPO_LINHA:
        return area_linha(f->dados_forma);
    case TIPO_TEXTO:
        return area_texto(f->dados_forma);
    default:
        return 0.0f;
    }
}

TipoForma forma_get_tipo(Forma forma)
{
    if (!forma)
        return (TipoForma)-1;
    return ((FormaStruct *)forma)->tipo;
}
int forma_get_id_original(Forma forma)
{
    if (!forma)
        return -1;
    return ((FormaStruct *)forma)->id_original;
}

void forma_set_destruida(Forma forma, bool status)
{
    if (!forma)
        return;
    ((FormaStruct *)forma)->foi_destruida = status;
}

static FormaStruct *clonar_forma(FormaStruct *original, float x, float y, const char *nova_cor_borda, bool trocar_cores)
{
    if (!original || !original->dados_forma)
        return NULL;

    int id_clone = proximo_id_clone++;
    void *dados_clonados = NULL;

    switch (original->tipo)
    {
    case TIPO_CIRCULO:
    {
        Circulo *orig_c = (Circulo *)original->dados_forma;
        const char *cor_b_orig = get_corBorda_circulo(orig_c);
        const char *cor_p_orig = get_corPreenchimento_circulo(orig_c);
        if (!cor_b_orig || !cor_p_orig)
            return NULL;

        const char *cor_b = trocar_cores ? cor_p_orig : (nova_cor_borda ? nova_cor_borda : cor_b_orig);
        const char *cor_p = trocar_cores ? cor_b_orig : cor_p_orig;
        dados_clonados = criar_circulo(x, y, get_raio(orig_c), (char *)cor_p, (char *)cor_b, id_clone);
        break;
    }
    case TIPO_RETANGULO:
    {
        Retangulo *orig_r = (Retangulo *)original->dados_forma;
        const char *cor_b_orig = get_corBorda_retangulo(orig_r);
        const char *cor_p_orig = get_corPreenchimento_retangulo(orig_r);
        if (!cor_b_orig || !cor_p_orig)
            return NULL;

        const char *cor_b = trocar_cores ? cor_p_orig : (nova_cor_borda ? nova_cor_borda : cor_b_orig);
        const char *cor_p = trocar_cores ? cor_b_orig : cor_p_orig;
        dados_clonados = criar_retangulo(x, y, get_largura(orig_r), get_altura(orig_r), (char *)cor_p, (char *)cor_b, id_clone);
        break;
    }
    case TIPO_LINHA:
    {
        Linha *orig_l = (Linha *)original->dados_forma;
        const char *cor_original = get_cor_linha(orig_l);
        if (!cor_original)
            return NULL;

        const char *nova_cor = nova_cor_borda ? nova_cor_borda : cor_original;

        float x1_orig = get_x1_linha(orig_l);
        float y1_orig = get_y1_linha(orig_l);
        float x2_orig = get_x2_linha(orig_l);
        float y2_orig = get_y2_linha(orig_l);
        float dx = x2_orig - x1_orig;
        float dy = y2_orig - y1_orig;

        dados_clonados = criar_linha(x, y, x + dx, y + dy, (char *)nova_cor, id_clone);
        break;
    }
    case TIPO_TEXTO:
    {
        Texto *orig_t = (Texto *)original->dados_forma;
        const char *orig_borda = get_corBorda_texto(orig_t);
        const char *orig_preench = get_corPreenchimento_texto(orig_t);
        const char *orig_conteudo = get_conteudo_texto(orig_t);
        if (!orig_borda || !orig_preench || !orig_conteudo)
            return NULL;

        char orig_anchor = get_anchor_texto(orig_t);

        const char *cor_b = trocar_cores ? orig_preench : (nova_cor_borda ? nova_cor_borda : orig_borda);
        const char *cor_p = trocar_cores ? orig_borda : orig_preench;

        dados_clonados = criar_texto(x, y, (char *)cor_b, (char *)cor_p, orig_anchor, (char *)orig_conteudo, NULL, id_clone);
        break;
    }
    }

    if (!dados_clonados)
        return NULL;

    FormaStruct *wrapper_clone = malloc(sizeof(FormaStruct));
    if (!wrapper_clone)
    {
        switch (original->tipo)
        {
        case TIPO_CIRCULO:
            if (dados_clonados)
                liberar_circulo(dados_clonados);
            break;
        case TIPO_RETANGULO:
            if (dados_clonados)
                liberar_retangulo(dados_clonados);
            break;
        case TIPO_LINHA:
            if (dados_clonados)
                liberar_linha(dados_clonados);
            break;
        case TIPO_TEXTO:
            if (dados_clonados)
                liberar_texto(dados_clonados);
            break;
        }
        fprintf(stderr, "Falha ao alocar wrapper para clone ID %d\n", id_clone);
        return NULL;
    }

    wrapper_clone->id_original = id_clone;
    wrapper_clone->tipo = original->tipo;
    wrapper_clone->dados_forma = dados_clonados;
    wrapper_clone->foi_clonada = true;
    wrapper_clone->foi_destruida = false;
    wrapper_clone->x_landed = x;
    wrapper_clone->y_landed = y;
    total_formas_clonadas++;
    return wrapper_clone;
}

typedef struct
{
    Lista formas_na_arena;
} ArenaStruct;

Arena criar_arena()
{
    ArenaStruct *a = malloc(sizeof(ArenaStruct));
    if (!a)
    {
        fprintf(stderr, "Falha ao alocar ArenaStruct\n");
        return NULL;
    }
    iniciar_lista(&a->formas_na_arena);
    if (!a->formas_na_arena)
    {
        fprintf(stderr, "Falha ao iniciar lista da arena\n");
        free(a);
        return NULL;
    }
    return a;
}

void arena_adicionar_forma(Arena a, Forma forma)
{
    ArenaStruct *arena = (ArenaStruct *)a;
    if (!arena || !arena->formas_na_arena || !forma)
    {
        fprintf(stderr, "Erro: Tentativa de adicionar forma NULL ou a arena/lista inválida.\n");
        return;
    }
    inserir_lista(&arena->formas_na_arena, forma);
}

void destruir_arena(Arena a)
{
    if (!a)
        return;
    ArenaStruct *arena = (ArenaStruct *)a;
    if (arena->formas_na_arena)
        liberar_lista(&arena->formas_na_arena);
    free(a);
}

typedef struct
{
    float minX, minY, maxX, maxY;
} Aabb;

static Aabb make_aabb_for_shape(FormaStruct *s)
{
    Aabb box = {0, 0, 0, 0};
    if (!s || !s->dados_forma)
        return box;

    float landed_x = s->x_landed;
    float landed_y = s->y_landed;

    switch (s->tipo)
    {
    case TIPO_CIRCULO:
    {
        float r = get_raio((Circulo *)s->dados_forma);
        box.minX = landed_x - r;
        box.maxX = landed_x + r;
        box.minY = landed_y - r;
        box.maxY = landed_y + r;
        break;
    }
    case TIPO_RETANGULO:
    {
        float w = get_largura((Retangulo *)s->dados_forma);
        float h = get_altura((Retangulo *)s->dados_forma);
        box.minX = landed_x;
        box.maxX = landed_x + w;
        box.minY = landed_y;
        box.maxY = landed_y + h;
        break;
    }
    case TIPO_TEXTO:
    {
        Texto *t = (Texto *)s->dados_forma;
        const char *txt_content = get_conteudo_texto(t);
        int len = txt_content ? (int)strlen(txt_content) : 0;
        float segLen = 10.0f * (float)len;
        char anchor = get_anchor_texto(t);
        float x1 = landed_x, y1 = landed_y, x2 = landed_x;

        if (anchor == 'i' || anchor == 'I' || anchor == 's' || anchor == 'S')
        {
            x2 = landed_x + segLen;
        }
        else if (anchor == 'f' || anchor == 'F' || anchor == 'e' || anchor == 'E')
        {
            x1 = landed_x - segLen;
        }
        else if (anchor == 'm' || anchor == 'M')
        {
            x1 = landed_x - segLen * 0.5f;
            x2 = landed_x + segLen * 0.5f;
        }
        else
        {
            x2 = landed_x + segLen;
        }

        box.minX = (x1 < x2 ? x1 : x2) - 1.0f;
        box.maxX = (x1 > x2 ? x1 : x2) + 1.0f;
        box.minY = y1 - 1.0f;
        box.maxY = y1 + 1.0f;
        break;
    }
    case TIPO_LINHA:
    {
        Linha *l = (Linha *)s->dados_forma;
        float x1 = get_x1_linha(l);
        float y1 = get_y1_linha(l);
        float x2 = get_x2_linha(l);
        float y2 = get_y2_linha(l);

        box.minX = (x1 < x2 ? x1 : x2) - 1.0f;
        box.maxX = (x1 > x2 ? x1 : x2) + 1.0f;
        box.minY = (y1 < y2 ? y1 : y2) - 1.0f;
        box.maxY = (y1 > y2 ? y1 : y2) + 1.0f;
        break;
    }
    }
    return box;
}

static bool aabb_overlap(Aabb a, Aabb b)
{
    if (a.maxX < b.minX)
        return false;
    if (b.maxX < a.minX)
        return false;
    if (a.maxY < b.minY)
        return false;
    if (b.maxY < a.minY)
        return false;
    return true;
}

bool formas_colidem(FormaStruct *f1, FormaStruct *f2)
{
    if (!f1 || !f2)
        return false;
    Aabb a = make_aabb_for_shape(f1);
    Aabb b = make_aabb_for_shape(f2);
    return aabb_overlap(a, b);
}

void process_qry(FILE *qry, FILE *svg, Ground ground, FILE *txt)
{
  
}