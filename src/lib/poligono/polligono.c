#include "poligono.h"
#include "../arvore/tree.h" 
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define TIPO_INICIO 0
#define TIPO_FIM    1

static int LIMIAR_INSERTION_SORT = 10;

void definir_limiar_ordenacao(int limiar) {
    if (limiar > 0) LIMIAR_INSERTION_SORT = limiar;
}

typedef struct {
    double x, y;
} TPonto;

typedef struct {
    TPonto a, b;
} TSegmento;

typedef struct {
    TSegmento *S;
    int n, cap;
} TPoligono;

typedef struct {
    TPonto *pontos;
    int n, cap;
} TVisibilidade;

typedef struct {
    TPonto v;
    double ang;
    int tipo; 
    TSegmento seg; 
    double dist_key; 
} EventoVarredura;

static double dist2(TPonto a, TPonto b) {
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    return dx*dx + dy*dy;
}

static double ObterT(TSegmento raio, TSegmento s2) {
    double x1 = raio.a.x, y1 = raio.a.y;
    double x2 = raio.b.x, y2 = raio.b.y;
    double x3 = s2.a.x, y3 = s2.a.y;
    double x4 = s2.b.x, y4 = s2.b.y;
    
    double den = (x1-x2)*(y3-y4) - (y1-y2)*(x3-x4);
    if (fabs(den) < 1e-12) return -1.0;
    
    double t = ((x1-x3)*(y3-y4) - (y1-y3)*(x3-x4)) / den;
    double u = -((x1-x2)*(y1-y3) - (y1-y2)*(x1-x3)) / den;

    if (u >= 0.0 && u <= 1.0) {
        return t;
    }
    return -1.0;
}

Poligono CriarPoligono(int n) {
    TPoligono *p = malloc(sizeof(TPoligono));
    if (!p) return NULL;
    p->cap = n; p->n = 0;
    p->S = malloc(sizeof(TSegmento) * n);
    return (Poligono)p;
}

void DestruirPoligono(Poligono P) {
    if (!P) return;
    TPoligono *p = (TPoligono*)P;
    if (p->S) free(p->S);
    free(p);
}

Ponto CriarPonto(double x, double y) {
    TPonto *p = malloc(sizeof(TPonto));
    if (!p) return NULL;
    p->x = x; p->y = y;
    return (Ponto)p;
}

Segmento CriarSegmento(Ponto a, Ponto b) {
    TSegmento *s = malloc(sizeof(TSegmento));
    if (!s) return NULL;
    if (a) s->a = *(TPonto*)a;
    if (b) s->b = *(TPonto*)b;
    return (Segmento)s;
}

void PoligonoAdicionarSegmento(Poligono P, Segmento S) {
    TPoligono *p = (TPoligono*)P;
    TSegmento *seg = (TSegmento*)S;
    if (p && seg && p->n < p->cap) p->S[p->n++] = *seg;
}

static void VisAddPonto(TVisibilidade *v, TPonto p) {
    if (v->n == v->cap) {
        v->cap = (v->cap == 0) ? 16 : v->cap * 2;
        v->pontos = realloc(v->pontos, v->cap * sizeof(TPonto));
    }
    v->pontos[v->n++] = p;
}

static int compararEventos(const EventoVarredura a, const EventoVarredura b) {
    if (a.ang < b.ang) return -1;
    if (a.ang > b.ang) return 1;
    if (a.tipo != b.tipo) {
        return (a.tipo == TIPO_INICIO) ? -1 : 1; 
    }
    return 0;
}

static void insertionSort(EventoVarredura *arr, int l, int r) {
    for (int i = l + 1; i <= r; i++) {
        EventoVarredura chave = arr[i];
        int j = i - 1;
        while (j >= l && compararEventos(arr[j], chave) > 0) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = chave;
    }
}

static void merge(EventoVarredura *arr, EventoVarredura *temp, int l, int m, int r) {
    int i = l, j = m + 1, k = l;
    while (i <= m && j <= r) {
        if (compararEventos(arr[i], arr[j]) <= 0) temp[k++] = arr[i++];
        else temp[k++] = arr[j++];
    }
    while (i <= m) temp[k++] = arr[i++];
    while (j <= r) temp[k++] = arr[j++];
    for (i = l; i <= r; i++) arr[i] = temp[i];
}

static void mergeSortHibrido(EventoVarredura *arr, EventoVarredura *temp, int l, int r) {
    if (r - l + 1 <= LIMIAR_INSERTION_SORT) {
        insertionSort(arr, l, r);
        return;
    }
    if (l < r) {
        int m = l + (r - l) / 2;
        mergeSortHibrido(arr, temp, l, m);
        mergeSortHibrido(arr, temp, m + 1, r);
        merge(arr, temp, l, m, r);
    }
}

typedef struct {
    TSegmento raio;
    double min_t;
    int found;
} BuscaCtx;

static void check_intersection(void* data, void* ctx) {
    TSegmento* seg = (TSegmento*)data;
    BuscaCtx* bc = (BuscaCtx*)ctx;
    
    double t = ObterT(bc->raio, *seg);
    if (t > 1e-9) {
        if (bc->found == 0 || t < bc->min_t) {
            bc->min_t = t;
            bc->found = 1;
        }
    }
}

Visibilidade CalcularVisibilidade(Poligono P, Ponto X) {
    TPoligono *poly = (TPoligono*)P;
    TPonto origem = *(TPonto*)X;

    if (!poly || poly->n == 0) return NULL;

    TVisibilidade *vis = malloc(sizeof(TVisibilidade));
    vis->n = 0; vis->cap = 0; vis->pontos = NULL;

    int N = poly->n;
    int numEventos = 2 * N;
    EventoVarredura *eventos = malloc(sizeof(EventoVarredura) * numEventos);

    for (int i = 0; i < N; i++) {
        TSegmento s = poly->S[i];
        
        double ang1 = atan2(s.a.y - origem.y, s.a.x - origem.x);
        double ang2 = atan2(s.b.y - origem.y, s.b.x - origem.x);
        
        TPonto pInicio, pFim;
        double angInicio, angFim;
        double distKey;

        if (ang1 < ang2) {
            pInicio = s.a; angInicio = ang1;
            pFim = s.b;    angFim = ang2;
            distKey = dist2(origem, s.a);
        } else {
            pInicio = s.b; angInicio = ang2;
            pFim = s.a;    angFim = ang1;
            distKey = dist2(origem, s.b);
        }
        
        if (fabs(ang1 - ang2) > M_PI) {
             if (ang1 < ang2) { 
                pInicio = s.b; angInicio = ang2;
                pFim = s.a;    angFim = ang1;
                distKey = dist2(origem, s.b);
             } else {
                pInicio = s.a; angInicio = ang1;
                pFim = s.b;    angFim = ang2;
                distKey = dist2(origem, s.a);
             }
        }

        eventos[2*i].v = pInicio;
        eventos[2*i].ang = angInicio;
        eventos[2*i].tipo = TIPO_INICIO;
        eventos[2*i].seg = s; 
        eventos[2*i].dist_key = distKey;

        eventos[2*i+1].v = pFim;
        eventos[2*i+1].ang = angFim;
        eventos[2*i+1].tipo = TIPO_FIM;
        eventos[2*i+1].seg = s;
        eventos[2*i+1].dist_key = distKey;
    }

    EventoVarredura *temp = malloc(sizeof(EventoVarredura) * numEventos);
    if (temp) {
        mergeSortHibrido(eventos, temp, 0, numEventos - 1);
        free(temp);
    }

    TreeNode raiz = NULL;
    iniciar_tree(&raiz);

    TPonto biombo = eventos[0].v; 

    for (int i = 0; i < numEventos; i++) {
        EventoVarredura ev = eventos[i];
        TSegmento* segPtr = malloc(sizeof(TSegmento));
        *segPtr = ev.seg;

        if (ev.tipo == TIPO_INICIO) {
            inserir_tree(&raiz, ev.dist_key, segPtr);
        } else {
            remover_tree(&raiz, ev.dist_key, NULL);
            free(segPtr);
        }

        BuscaCtx ctx;
        ctx.raio.a = origem;
        ctx.raio.b = ev.v;
        ctx.min_t = 1.0;
        ctx.found = 0;

        tree_iterar(raiz, check_intersection, &ctx);

        if (ctx.found && ctx.min_t < 0.999999999) {
            TPonto inter;
            inter.x = origem.x + ctx.min_t * (ev.v.x - origem.x);
            inter.y = origem.y + ctx.min_t * (ev.v.y - origem.y);
            
            VisibilidadeGetPonto(vis, 0); 
            VisAddPonto(vis, biombo); 
            VisAddPonto(vis, inter); 
            biombo = inter;
        } else {
            VisAddPonto(vis, biombo); 
            VisAddPonto(vis, ev.v); 
            biombo = ev.v;
        }
    }

    liberar_tree(&raiz); 
    free(eventos);
    return (Visibilidade)vis;
}

int VisibilidadeNumPontos(Visibilidade VV) {
    if (!VV) return 0;
    return ((TVisibilidade*)VV)->n;
}

Ponto VisibilidadeGetPonto(Visibilidade VV, int i) {
    TVisibilidade *v = (TVisibilidade*)VV;
    if (!v || i < 0 || i >= v->n) return NULL;
    return CriarPonto(v->pontos[i].x, v->pontos[i].y);
}

double PontoX(Ponto P) { return P ? ((TPonto*)P)->x : 0.0; }
double PontoY(Ponto P) { return P ? ((TPonto*)P)->y : 0.0; }

void DestruirVisibilidade(Visibilidade V) {
    if (!V) return;
    TVisibilidade *v = (TVisibilidade*)V;
    if (v->pontos) free(v->pontos);
    free(v);
}