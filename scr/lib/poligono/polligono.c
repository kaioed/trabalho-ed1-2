#include "poligono.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define INF 1e18
#define TIPO_INICIO 0
#define TIPO_FIM    1

static int LIMIAR_INSERTION_SORT = 10;

void definir_limiar_ordenacao(int limiar) {
    if (limiar > 0) {
        LIMIAR_INSERTION_SORT = limiar;
    }
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

typedef struct NoAVL {
    TSegmento seg;
    double chave;
    int h;
    struct NoAVL *esq, *dir;
} NoAVL;

typedef struct {
    NoAVL *raiz;
} TSetAVL;

typedef struct {
    TPonto v;
    double ang;
    int tipo; 
    TSegmento seg;
} EventoVarredura;


static double dist2(TPonto a, TPonto b) {
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    return dx*dx + dy*dy;
}

static int Intersec(TSegmento s1, TSegmento s2, TPonto *resp) {
    double x1 = s1.a.x, y1 = s1.a.y;
    double x2 = s1.b.x, y2 = s1.b.y;
    double x3 = s2.a.x, y3 = s2.a.y;
    double x4 = s2.b.x, y4 = s2.b.y;
    double den = (x1-x2)*(y3-y4) - (y1-y2)*(x3-x4);
    if (fabs(den) < 1e-12) return 0;
    double px = ((x1*y2 - y1*x2)*(x3 - x4) - (x1-x2)*(x3*y4 - y3*x4)) / den;
    double py = ((x1*y2 - y1*x2)*(y3 - y4) - (y1-y2)*(x3*y4 - y3*x4)) / den;
    double eps = 1e-9;
    if (px < fmin(x1,x2)-eps || px > fmax(x1,x2)+eps) return 0;
    if (py < fmin(y1,y2)-eps || py > fmax(y1,y2)+eps) return 0;
    if (px < fmin(x3,x4)-eps || px > fmax(x3,x4)+eps) return 0;
    if (py < fmin(y3,y4)-eps || py > fmax(y3,y4)+eps) return 0;
    resp->x = px; resp->y = py;
    return 1;
}

static int h(NoAVL *n) { return n ? n->h : 0; }
static int max_int(int a, int b) { return (a > b) ? a : b; }

static NoAVL* rotDir(NoAVL *y) {
    NoAVL *x = y->esq; NoAVL *T2 = x->dir;
    x->dir = y; y->esq = T2;
    y->h = max_int(h(y->esq), h(y->dir)) + 1;
    x->h = max_int(h(x->esq), h(x->dir)) + 1;
    return x;
}

static NoAVL* rotEsq(NoAVL *x) {
    NoAVL *y = x->dir; NoAVL *T2 = y->esq;
    y->esq = x; x->dir = T2;
    x->h = max_int(h(x->esq), h(x->dir)) + 1;
    y->h = max_int(h(y->esq), h(y->dir)) + 1;
    return y;
}

static int getBalance(NoAVL *n) { return n ? h(n->esq) - h(n->dir) : 0; }

static NoAVL* avlIns(NoAVL *n, TSegmento s, double chave) {
    if (!n) {
        NoAVL *x = malloc(sizeof(NoAVL));
        x->seg = s; x->chave = chave; x->h = 1; x->esq = x->dir = NULL;
        return x;
    }
    if (chave < n->chave) n->esq = avlIns(n->esq, s, chave);
    else n->dir = avlIns(n->dir, s, chave);
    n->h = max_int(h(n->esq), h(n->dir)) + 1;
    int bal = getBalance(n);
    if (bal > 1 && chave < n->esq->chave) return rotDir(n);
    if (bal < -1 && chave > n->dir->chave) return rotEsq(n);
    if (bal > 1 && chave > n->esq->chave) { n->esq = rotEsq(n->esq); return rotDir(n); }
    if (bal < -1 && chave < n->dir->chave) { n->dir = rotDir(n->dir); return rotEsq(n); }
    return n;
}

static NoAVL* avlMin(NoAVL *n) { while (n && n->esq) n = n->esq; return n; }

static NoAVL* avlRemove(NoAVL *root, double chave) {
    if (!root) return NULL;
    if (chave < root->chave) root->esq = avlRemove(root->esq, chave);
    else if (chave > root->chave) root->dir = avlRemove(root->dir, chave);
    else {
        if (!root->esq || !root->dir) {
            NoAVL *tmp = root->esq ? root->esq : root->dir;
            free(root); return tmp;
        }
        NoAVL *temp = avlMin(root->dir);
        root->chave = temp->chave;
        root->seg = temp->seg;
        root->dir = avlRemove(root->dir, temp->chave);
    }
    if (!root) return NULL;
    root->h = max_int(h(root->esq), h(root->dir)) + 1;
    int bal = getBalance(root);
    if (bal > 1 && getBalance(root->esq) >= 0) return rotDir(root);
    if (bal > 1 && getBalance(root->esq) < 0) { root->esq = rotEsq(root->esq); return rotDir(root); }
    if (bal < -1 && getBalance(root->dir) <= 0) return rotEsq(root);
    if (bal < -1 && getBalance(root->dir) > 0) { root->dir = rotDir(root->dir); return rotEsq(root); }
    return root;
}

static void avlDestruir(NoAVL *n) {
    if (!n) return;
    avlDestruir(n->esq); avlDestruir(n->dir); free(n);
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
    int i = l;
    int j = m + 1;
    int k = l;

    // Intercala
    while (i <= m && j <= r) {
        if (compararEventos(arr[i], arr[j]) <= 0) {
            temp[k++] = arr[i++];
        } else {
            temp[k++] = arr[j++];
        }
    }

    // Copia restantes
    while (i <= m) temp[k++] = arr[i++];
    while (j <= r) temp[k++] = arr[j++];

    // Copia de volta para o vetor original
    for (i = l; i <= r; i++) {
        arr[i] = temp[i];
    }
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
        
        eventos[2*i].v = s.a;
        eventos[2*i].ang = atan2(s.a.y - origem.y, s.a.x - origem.x);
        eventos[2*i].tipo = TIPO_INICIO;
        eventos[2*i].seg = s;

        eventos[2*i+1].v = s.b;
        eventos[2*i+1].ang = atan2(s.b.y - origem.y, s.b.x - origem.x);
        eventos[2*i+1].tipo = TIPO_FIM;
        eventos[2*i+1].seg = s;
    }

    EventoVarredura *temp = malloc(sizeof(EventoVarredura) * numEventos);
    if (temp) {
        mergeSortHibrido(eventos, temp, 0, numEventos - 1);
        free(temp);
    } else {
      
    }
  

    TSetAVL ativos;
    ativos.raiz = NULL;
    TPonto biombo = eventos[0].v; 

    for (int i = 0; i < numEventos; i++) {
        EventoVarredura ev = eventos[i];
        double d = dist2(origem, ev.v);

        if (ev.tipo == TIPO_INICIO) {
            ativos.raiz = avlIns(ativos.raiz, ev.seg, d);
            
            NoAVL *maisProximo = avlMin(ativos.raiz);
            TPonto inter;
            TSegmento raio = { origem, ev.v };

            if (maisProximo && Intersec(raio, maisProximo->seg, &inter)) {
                if (dist2(inter, ev.v) > 1e-9) {
                     VisAddPonto(vis, biombo); VisAddPonto(vis, inter); biombo = inter;
                } else {
                     VisAddPonto(vis, biombo); VisAddPonto(vis, ev.v); biombo = ev.v;
                }
            } else {
                VisAddPonto(vis, biombo); VisAddPonto(vis, ev.v); biombo = ev.v;
            }

        } else { // TIPO_FIM
            ativos.raiz = avlRemove(ativos.raiz, d);
            NoAVL *maisProximo = avlMin(ativos.raiz);
            TPonto inter;
            TSegmento raio = { origem, ev.v };
            
            if (maisProximo && Intersec(raio, maisProximo->seg, &inter)) {
                VisAddPonto(vis, biombo); VisAddPonto(vis, inter); biombo = inter;
            } else {
                VisAddPonto(vis, biombo); VisAddPonto(vis, ev.v); biombo = ev.v;
            }
        }
    }

    free(eventos);
    avlDestruir(ativos.raiz);
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