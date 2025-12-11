#include "poligono.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <float.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define EPSILON 1e-5

typedef struct {
    double x, y;
} TPontoInternal;

typedef struct {
    TPontoInternal a, b;
    int id; 
} TSegmentoInternal;

typedef struct {
    TSegmentoInternal *S;
    int n, cap;
} TPoligonoInternal;

typedef struct {
    TPontoInternal *pontos;
    int n, cap;
} TVisibilidadeInternal;

typedef enum {
    EVENTO_INICIO,
    EVENTO_FIM
} TipoEvento;

typedef struct {
    TPontoInternal ponto;
    double angulo;
    double distancia;
    TipoEvento tipo;
    TSegmentoInternal *segmento;
} Evento;

typedef struct NoAtivo {
    TSegmentoInternal *seg;
    struct NoAtivo *prox;
} NoAtivo;

static double dist_sq(double x1, double y1, double x2, double y2) {
    return (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2);
}

static double ponto_dist(TPontoInternal p1, TPontoInternal p2) {
    return sqrt(dist_sq(p1.x, p1.y, p2.x, p2.y));
}

static int ponto_igual(TPontoInternal p1, TPontoInternal p2) {
    return (fabs(p1.x - p2.x) < EPSILON && fabs(p1.y - p2.y) < EPSILON);
}

static double ponto_angulo(TPontoInternal origem, TPontoInternal p) {
    double dx = p.x - origem.x;
    double dy = p.y - origem.y;
    double ang = atan2(dy, dx);
    if (ang < 0) ang += 2.0 * M_PI;
    return ang;
}

static double distancia_raio_segmento(TPontoInternal origem, double angulo, TSegmentoInternal *seg) {
    double ox = origem.x, oy = origem.y;
    double dx = cos(angulo), dy = sin(angulo);
    double sx1 = seg->a.x, sy1 = seg->a.y;
    double sx2 = seg->b.x, sy2 = seg->b.y;
    
    double segx = sx2 - sx1;
    double segy = sy2 - sy1;
    double denom = dx * segy - dy * segx;
    
    if (fabs(denom) < EPSILON) return DBL_MAX;
    
    double t = ((sx1 - ox) * segy - (sy1 - oy) * segx) / denom;
    double u = ((sx1 - ox) * dy - (sy1 - oy) * dx) / denom;
    
    if (t >= -EPSILON && u >= -EPSILON && u <= 1.0 + EPSILON) {
        return (t < 0) ? 0.0 : t; 
    }
    return DBL_MAX;
}

static int intersecao_raio_seg(TPontoInternal origem, TPontoInternal direcao, TSegmentoInternal *seg, TPontoInternal *result) {
    double ox = origem.x, oy = origem.y;
    double dx = direcao.x - ox, dy = direcao.y - oy;
    double sx1 = seg->a.x, sy1 = seg->a.y;
    double sx2 = seg->b.x, sy2 = seg->b.y;
    
    double segx = sx2 - sx1;
    double segy = sy2 - sy1;
    double denom = dx * segy - dy * segx;
    
    if (fabs(denom) < EPSILON) return 0;
    
    double t = ((sx1 - ox) * segy - (sy1 - oy) * segx) / denom;
    double u = ((sx1 - ox) * dy - (sy1 - oy) * dx) / denom;
    
    if (t >= -EPSILON && u >= -EPSILON && u <= 1.0 + EPSILON) {
        result->x = ox + t * dx;
        result->y = oy + t * dy;
        return 1;
    }
    return 0;
}

static int comparar_eventos(const void *a, const void *b) {
    Evento *e1 = (Evento*)a;
    Evento *e2 = (Evento*)b;
    
    if (fabs(e1->angulo - e2->angulo) > EPSILON) {
        return (e1->angulo < e2->angulo) ? -1 : 1;
    }
    
    if (e1->tipo != e2->tipo) {
        return (e1->tipo == EVENTO_INICIO) ? -1 : 1;
    }
    
    if (fabs(e1->distancia - e2->distancia) > EPSILON) {
        return (e1->distancia < e2->distancia) ? -1 : 1;
    }
    return 0;
}

static void lista_ativos_inserir(NoAtivo **head, TSegmentoInternal *seg) {
    NoAtivo *curr = *head;
    while(curr) {
        if(curr->seg == seg) return;
        curr = curr->prox;
    }
    NoAtivo *novo = malloc(sizeof(NoAtivo));
    novo->seg = seg;
    novo->prox = *head;
    *head = novo;
}

static void lista_ativos_remover(NoAtivo **head, TSegmentoInternal *seg) {
    NoAtivo *atual = *head;
    NoAtivo *ant = NULL;
    while (atual) {
        if (atual->seg == seg) {
            if (ant) ant->prox = atual->prox;
            else *head = atual->prox;
            free(atual);
            return;
        }
        ant = atual;
        atual = atual->prox;
    }
}

static TSegmentoInternal* obter_mais_proximo(NoAtivo *head, TPontoInternal origem, double angulo) {
    TSegmentoInternal *min_seg = NULL;
    double min_dist = DBL_MAX;
    
    NoAtivo *atual = head;
    while (atual) {
        double d = distancia_raio_segmento(origem, angulo, atual->seg);
        if (d < min_dist) {
            min_dist = d;
            min_seg = atual->seg;
        }
        atual = atual->prox;
    }
    return min_seg;
}

Poligono CriarPoligono(int n) {
    TPoligonoInternal *p = malloc(sizeof(TPoligonoInternal));
    p->cap = n + 4;
    p->n = 0;
    p->S = malloc(sizeof(TSegmentoInternal) * p->cap);
    return (Poligono)p;
}

void DestruirPoligono(Poligono P) {
    if (!P) return;
    TPoligonoInternal *p = (TPoligonoInternal*)P;
    if (p->S) free(p->S);
    free(p);
}

Ponto CriarPonto(double x, double y) {
    TPontoInternal *p = malloc(sizeof(TPontoInternal));
    p->x = x; p->y = y;
    return (Ponto)p;
}

Segmento CriarSegmento(Ponto a, Ponto b) {
    TSegmentoInternal *s = malloc(sizeof(TSegmentoInternal));
    if (a) s->a = *(TPontoInternal*)a;
    if (b) s->b = *(TPontoInternal*)b;
    s->id = -1;
    return (Segmento)s;
}

void PoligonoAdicionarSegmento(Poligono P, Segmento S) {
    TPoligonoInternal *p = (TPoligonoInternal*)P;
    TSegmentoInternal *seg = (TSegmentoInternal*)S;
    if (p->n >= p->cap) {
        p->cap *= 2;
        p->S = realloc(p->S, sizeof(TSegmentoInternal) * p->cap);
    }
    p->S[p->n++] = *seg;
}

Visibilidade CalcularVisibilidade(Poligono P, Ponto X) {
    TPoligonoInternal *poly = (TPoligonoInternal*)P;
    TPontoInternal origem = *(TPontoInternal*)X;
    
    int qtd_segs_orig = poly->n;
    TSegmentoInternal *segs_proc = malloc(sizeof(TSegmentoInternal) * (qtd_segs_orig + 4) * 2);
    int n_segs = 0;
    
    for (int i=0; i<qtd_segs_orig; i++) {
        TSegmentoInternal s = poly->S[i];
        double a1 = ponto_angulo(origem, s.a);
        double a2 = ponto_angulo(origem, s.b);
        
        if (fabs(a1 - a2) > M_PI) {
            TPontoInternal dir = {origem.x + 10000, origem.y};
            TPontoInternal inter;
            TSegmentoInternal s_temp = s;
            if (intersecao_raio_seg(origem, dir, &s_temp, &inter)) {
                segs_proc[n_segs++] = (TSegmentoInternal){s.a, inter, s.id};
                segs_proc[n_segs++] = (TSegmentoInternal){inter, s.b, s.id};
                continue;
            }
        }
        segs_proc[n_segs++] = s;
    }
    
    int n_eventos = n_segs * 2;
    Evento *eventos = malloc(sizeof(Evento) * n_eventos);
    int idx_ev = 0;
    
    for (int i=0; i<n_segs; i++) {
        TSegmentoInternal *s = &segs_proc[i];
        double ang1 = ponto_angulo(origem, s->a);
        double ang2 = ponto_angulo(origem, s->b);
        
        if (ang1 > ang2) {
            double temp = ang1; ang1 = ang2; ang2 = temp;
            TPontoInternal tp = s->a; s->a = s->b; s->b = tp;
        }
        
        double d1 = ponto_dist(origem, s->a);
        double d2 = ponto_dist(origem, s->b);
        
        eventos[idx_ev++] = (Evento){s->a, ang1, d1, EVENTO_INICIO, s};
        eventos[idx_ev++] = (Evento){s->b, ang2, d2, EVENTO_FIM, s};
    }
    
    qsort(eventos, idx_ev, sizeof(Evento), comparar_eventos);
    
    NoAtivo *ativos = NULL;
    double angulo_inicial = 0.0;
    for (int i=0; i<n_segs; i++) {
        TSegmentoInternal *s = &segs_proc[i];
        if (distancia_raio_segmento(origem, angulo_inicial, s) < DBL_MAX) {
            lista_ativos_inserir(&ativos, s);
        }
    }
    
    TVisibilidadeInternal *vis = malloc(sizeof(TVisibilidadeInternal));
    vis->n = 0; vis->cap = 32;
    vis->pontos = malloc(sizeof(TPontoInternal) * vis->cap);
    
    TSegmentoInternal *biombo = obter_mais_proximo(ativos, origem, angulo_inicial);
    TPontoInternal ultimo_ponto = {0,0};
    int tem_ultimo = 0;
    
    if (biombo) {
        TPontoInternal dir = {origem.x + 10000, origem.y};
        TPontoInternal inter;
        if (intersecao_raio_seg(origem, dir, biombo, &inter)) {
            vis->pontos[vis->n++] = inter;
            ultimo_ponto = inter;
            tem_ultimo = 1;
        }
    }
    
    for (int i=0; i<idx_ev; i++) {
        Evento ev = eventos[i];
        TSegmentoInternal *antigo_biombo = biombo;
        
        if (ev.tipo == EVENTO_INICIO) {
            lista_ativos_inserir(&ativos, ev.segmento);
        } else {
            lista_ativos_remover(&ativos, ev.segmento);
        }
        
        biombo = obter_mais_proximo(ativos, origem, ev.angulo);
        
        if (antigo_biombo != biombo) {
            if (antigo_biombo) {
                TPontoInternal inter;
                if (intersecao_raio_seg(origem, ev.ponto, antigo_biombo, &inter)) {
                     if (!tem_ultimo || !ponto_igual(ultimo_ponto, inter)) {
                        if (vis->n == vis->cap) { vis->cap *= 2; vis->pontos = realloc(vis->pontos, sizeof(TPontoInternal)*vis->cap); }
                        vis->pontos[vis->n++] = inter;
                        ultimo_ponto = inter;
                        tem_ultimo = 1;
                    }
                }
            }
            
            if (!tem_ultimo || !ponto_igual(ultimo_ponto, ev.ponto)) {
                 double d_ev = ev.distancia;
                 double d_biombo = biombo ? distancia_raio_segmento(origem, ev.angulo, biombo) : DBL_MAX;
                 
                 if (d_ev <= d_biombo + EPSILON) {
                    if (vis->n == vis->cap) { vis->cap *= 2; vis->pontos = realloc(vis->pontos, sizeof(TPontoInternal)*vis->cap); }
                    vis->pontos[vis->n++] = ev.ponto;
                    ultimo_ponto = ev.ponto;
                    tem_ultimo = 1;
                 }
            }

            if (biombo) {
                TPontoInternal inter;
                if (intersecao_raio_seg(origem, ev.ponto, biombo, &inter)) {
                    if (!tem_ultimo || !ponto_igual(ultimo_ponto, inter)) {
                        if (vis->n == vis->cap) { vis->cap *= 2; vis->pontos = realloc(vis->pontos, sizeof(TPontoInternal)*vis->cap); }
                        vis->pontos[vis->n++] = inter;
                        ultimo_ponto = inter;
                        tem_ultimo = 1;
                    }
                }
            }
        }
    }
    
    while(ativos) {
        NoAtivo *t = ativos;
        ativos = ativos->prox;
        free(t);
    }
    free(segs_proc);
    free(eventos);
    
    return (Visibilidade)vis;
}

int VisibilidadeNumPontos(Visibilidade VV) {
    if (!VV) return 0;
    return ((TVisibilidadeInternal*)VV)->n;
}

Ponto VisibilidadeGetPonto(Visibilidade VV, int i) {
    TVisibilidadeInternal *v = (TVisibilidadeInternal*)VV;
    if (!v || i < 0 || i >= v->n) return NULL;
    return CriarPonto(v->pontos[i].x, v->pontos[i].y);
}

double PontoX(Ponto P) { return P ? ((TPontoInternal*)P)->x : 0.0; }
double PontoY(Ponto P) { return P ? ((TPontoInternal*)P)->y : 0.0; }

void DestruirVisibilidade(Visibilidade V) {
    if (!V) return;
    TVisibilidadeInternal *v = (TVisibilidadeInternal*)V;
    if (v->pontos) free(v->pontos);
    free(v);
}

void definir_limiar_ordenacao(int limiar) {
    (void)limiar;
}

double CalcularAreaVisibilidade(Visibilidade V) {
    if (!V) return 0.0;
    TVisibilidadeInternal *vis = (TVisibilidadeInternal*)V;
    double area = 0.0;
    int n = vis->n;
    if (n < 3) return 0.0;

    for (int i = 0; i < n; i++) {
        double x1 = vis->pontos[i].x;
        double y1 = vis->pontos[i].y;
        double x2 = vis->pontos[(i + 1) % n].x;
        double y2 = vis->pontos[(i + 1) % n].y;
        area += (x1 * y2 - x2 * y1);
    }
    return fabs(area) / 2.0;
}