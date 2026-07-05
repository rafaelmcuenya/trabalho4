#include "conexos.h"
#include "lista.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <float.h>
#include <math.h>

static const char* CORES[] = {
    "#FF0000", "#00FF00", "#0000FF", "#FFFF00", "#FF00FF",
    "#00FFFF", "#FFA500", "#800080", "#008080", "#FF69B4",
    "#A52A2A", "#2E8B57", "#FFD700", "#DC143C", "#00CED1",
    "#9400D3", "#FF4500", "#32CD32", "#1E90FF", "#FF1493"
};
#define NUM_CORES (sizeof(CORES) / sizeof(CORES[0]))

typedef struct ComponenteStruct {
    Lista vertices;
    double minX, minY;
    double maxX, maxY;
    double bboxX, bboxY;
    double bboxW, bboxH;
    char* cor;
} ComponenteStruct;

typedef struct {
    bool* visitado;
    Grafo g;
    ComponenteStruct* componenteAtual;
    Lista* adjacenciaNaoDirecionada;
} DFSContext;    



static ComponenteStruct* criarComponente(void) {
    ComponenteStruct* c = malloc(sizeof(ComponenteStruct));
    if (c == NULL) return NULL;
    
    c->vertices = criaLista();
    if (c->vertices == NULL) {
        free(c);
        return NULL;
    }
    
    c->minX = DBL_MAX;
    c->minY = DBL_MAX;
    c->maxX = -DBL_MAX;
    c->maxY = -DBL_MAX;
    c->bboxX = 0.0;
    c->bboxY = 0.0;
    c->bboxW = 0.0;
    c->bboxH = 0.0;
    c->cor = NULL;
    
    return c;
}

static void adicionarVerticeComponente(ComponenteStruct* c, Vertice v) {
    assert(c != NULL);
    assert(v != NULL);
    
    inserirFim(c->vertices, v);
    double x = getVerticeX(v);
    double y = getVerticeY(v);
    
    if (x < c->minX) c->minX = x;
    if (y < c->minY) c->minY = y;
    if (x > c->maxX) c->maxX = x;
    if (y > c->maxY) c->maxY = y;
}

static void finalizarBoundingBox(ComponenteStruct* c) {
    assert(c != NULL);
    
    c->bboxX = c->minX;
    c->bboxY = c->minY;
    c->bboxW = c->maxX - c->minX;
    c->bboxH = c->maxY - c->minY;
    
    double marginX = c->bboxW * 0.1;
    double marginY = c->bboxH * 0.1;
    
    if (marginX < 1.0) marginX = 1.0;
    if (marginY < 1.0) marginY = 1.0;
    
    c->bboxX -= marginX;
    c->bboxY -= marginY;
    c->bboxW += 2 * marginX;
    c->bboxH += 2 * marginY;
}

static void atribuirCor(ComponenteStruct* c, int indice) {
    assert(c != NULL);
    assert(indice >= 0);
    
    int corIndex = indice % NUM_CORES;
    c->cor = malloc(strlen(CORES[corIndex]) + 1);
    if (c->cor != NULL) {
        strcpy(c->cor, CORES[corIndex]);
    }
}

static void dfs(Vertice v, DFSContext* ctx) {
    int idx = getIndiceVertice(v);
    if (ctx->visitado[idx]) return;
    
    ctx->visitado[idx] = true;
    adicionarVerticeComponente(ctx->componenteAtual, v);
    
    Lista adj = ctx->adjacenciaNaoDirecionada[idx];
    No no = primeiroNo(adj);
    while (no != NULL) {
        Vertice vizinho = (Vertice) getInfo(no);
        int idxVizinho = getIndiceVertice(vizinho);
        if (!ctx->visitado[idxVizinho]) {
            dfs(vizinho, ctx);
        }
        no = proximoNo(no);
    }
}

static Lista* construirAdjacenciaNaoDirecionada(Grafo g, double vl) {
    int numVertices = quantVertices(g);
    
    if (numVertices == 0) return NULL;
    
    Lista* adj = calloc(numVertices, sizeof(Lista));
    if (adj == NULL) return NULL;
    
    for (int i = 0; i < numVertices; i++) {
        adj[i] = criaLista();
        if (adj[i] == NULL) {
            for (int j = 0; j < i; j++) {
                deletaLista(adj[j]);
            }
            free(adj);
            return NULL;
        }
    }
    
    Vertice v = primeiroVertice(g);
    while (v != NULL) {
        Aresta a = primeiraArestaAdj(v);
        while (a != NULL) {
            if (arestaAtiva(a) && getVelocidadeMedia(a) >= vl) {
                Vertice origem = getOrigem(a);
                Vertice destino = getDestino(a);
                int idxOrigem = getIndiceVertice(origem);
                int idxDestino = getIndiceVertice(destino);
                
                inserirFim(adj[idxOrigem], destino);
                inserirFim(adj[idxDestino], origem);
            }
            a = proximaArestaAdj(v, a);
        }
        v = proximoVertice(g, v);
    }
    
    return adj;
}

static void liberarAdjacenciaNaoDirecionada(Lista* adj, int numVertices) {
    if (adj == NULL) return;
    for (int i = 0; i < numVertices; i++) {
        deletaLista(adj[i]);
    }
    free(adj);
}

ListaComponentes encontrarComponentes(Grafo g, double vl) {
    assert(g != NULL);
    assert(vl >= 0);
    
    int numVertices = quantVertices(g);
    printf("[DEBUG] encontrarComponentes: %d vértices, vl=%.2f\n", numVertices, vl);
    if (numVertices == 0) return NULL;
    
    Lista* adj = construirAdjacenciaNaoDirecionada(g, vl);
    if (adj == NULL) return NULL;
    
    bool* visitado = calloc(numVertices, sizeof(bool));
    if (visitado == NULL) {
        liberarAdjacenciaNaoDirecionada(adj, numVertices);
        return NULL;
    }
    
    Lista componentes = criaLista();
    if (componentes == NULL) {
        free(visitado);
        liberarAdjacenciaNaoDirecionada(adj, numVertices);
        return NULL;
    }
    
    DFSContext ctx;
    ctx.g = g;
    ctx.visitado = visitado;
    ctx.adjacenciaNaoDirecionada = adj;
    
    int numComponentes = 0;
    Vertice vertice = primeiroVertice(g);
    
    while (vertice != NULL) {
        int idx = getIndiceVertice(vertice);
        
        if (!visitado[idx]) {
            ComponenteStruct* comp = criarComponente();
            if (comp == NULL) {
                free(visitado);
                deletaLista(componentes);
                liberarAdjacenciaNaoDirecionada(adj, numVertices);
                return NULL;
            }
            
            ctx.componenteAtual = comp;
            dfs(vertice, &ctx);

            printf("[DEBUG] Componente %d: %d vértices, bbox=(%.2f,%.2f) %.2fx%.2f\n",
            numComponentes, 
            tamanhoLista(comp->vertices),
            comp->bboxX, comp->bboxY, comp->bboxW, comp->bboxH);
            
            finalizarBoundingBox(comp);
            atribuirCor(comp, numComponentes);
            inserirFim(componentes, comp);
            numComponentes++;
        }
        
        vertice = proximoVertice(g, vertice);
    }
    
    free(visitado);
    liberarAdjacenciaNaoDirecionada(adj, numVertices);
    
    return componentes;
}


int quantComponentes(ListaComponentes lc) {
    if (lc == NULL) return 0;
    return tamanhoLista((Lista) lc);
}

Componente getComponente(ListaComponentes lc, int indice) {
    assert(lc != NULL);
    assert(indice >= 0);
    
    Lista lista = (Lista) lc;
    if (indice >= tamanhoLista(lista)) return NULL;
    
    No no = primeiroNo(lista);
    for (int i = 0; i < indice; i++) {
        no = proximoNo(no);
        if (no == NULL) return NULL;
    }
    return (Componente) getInfo(no);
}

int tamanhoComponente(Componente c) {
    assert(c != NULL);
    ComponenteStruct* comp = (ComponenteStruct*) c;
    return tamanhoLista(comp->vertices);
}

Vertice getVerticeComponente(Componente c, int indice) {
    assert(c != NULL);
    assert(indice >= 0);
    
    ComponenteStruct* comp = (ComponenteStruct*) c;
    if (indice >= tamanhoLista(comp->vertices)) return NULL;
    No no = primeiroNo(comp->vertices);
    for (int i = 0; i < indice; i++) {
        no = proximoNo(no);
        if (no == NULL) return NULL;
    }
    return (Vertice) getInfo(no);
}

double getBBoxX(Componente c) {
    assert(c != NULL);
    ComponenteStruct* comp = (ComponenteStruct*) c;
    return comp->bboxX;
}

double getBBoxY(Componente c) {
    assert(c != NULL);
    ComponenteStruct* comp = (ComponenteStruct*) c;
    return comp->bboxY;
}

double getBBoxW(Componente c) {
    assert(c != NULL);
    ComponenteStruct* comp = (ComponenteStruct*) c;
    return comp->bboxW;
}

double getBBoxH(Componente c) {
    assert(c != NULL);
    ComponenteStruct* comp = (ComponenteStruct*) c;
    return comp->bboxH;
}

const char* getCorComponente(Componente c) {
    assert(c != NULL);
    ComponenteStruct* comp = (ComponenteStruct*) c;
    return comp->cor;
}

void deletaListaComponentes(ListaComponentes lc) {
    if (lc == NULL) return;
    
    Lista lista = (Lista) lc;
    No no = primeiroNo(lista);
    while (no != NULL) {
        ComponenteStruct* comp = (ComponenteStruct*) getInfo(no);
        if (comp != NULL) {
            deletaLista(comp->vertices);
            free(comp->cor);
            free(comp);
            setInfo(no, NULL);
        }
        no = proximoNo(no);
    }
    deletaLista(lista);
}
