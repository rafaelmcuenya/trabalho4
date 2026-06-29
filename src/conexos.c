#include "conexos.h"
#include "lista.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <float.h>
#include <math.h>

static const char* CORES[] = {
    "#FF0000",  // Vermelho
    "#00FF00",  // Verde
    "#0000FF",  // Azul
    "#FFFF00",  // Amarelo
    "#FF00FF",  // Magenta
    "#00FFFF",  // Ciano
    "#FFA500",  // Laranja
    "#800080",  // Roxo
    "#008080",  // Teal
    "#FF69B4",  // Rosa
    "#A52A2A",  // Marrom
    "#2E8B57",  // Verde marinho
    "#FFD700",  // Dourado
    "#DC143C",  // Carmim
    "#00CED1",  // Turquesa
    "#9400D3",  // Violeta
    "#FF4500",  // Vermelho alaranjado
    "#32CD32",  // Lima
    "#1E90FF",  // Azul dodger
    "#FF1493"   // Rosa profundo
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
    
    Aresta a = primeiraArestaAdj(v);
    while (a != NULL) {
        if (arestaAtiva(a)) {
            Vertice destino = getDestino(a);
            int idxDestino = getIndiceVertice(destino);
            
            if (!ctx->visitado[idxDestino]) {
                dfs(destino, ctx);
            }
        }
        a = proximaArestaAdj(v, a);
    }
}

ListaComponentes encontrarComponentes(Grafo g, double vl) {
    assert(g != NULL);
    assert(vl >= 0);
    
    int numVertices = quantVertices(g);
    if (numVertices == 0) return NULL;
    
    Lista arestasDesativadas = criaLista();
    if (arestasDesativadas == NULL) return NULL;
    
    Aresta a = primeiraArestaAdj(primeiroVertice(g));
    Vertice v = primeiroVertice(g);
  
    while (v != NULL) {
        Aresta aTemp = primeiraArestaAdj(v);
      
        while (aTemp != NULL) {
            if (arestaAtiva(aTemp) && getVelocidadeMedia(aTemp) < vl) {
                offAresta(aTemp);
                inserirFim(arestasDesativadas, aTemp);
            }
            aTemp = proximaArestaAdj(v, aTemp);
        }
        v = proximoVertice(g, v);
    }
    
    bool* visitado = calloc(numVertices, sizeof(bool));
  
    if (visitado == NULL) {
        No no = primeiroNo(arestasDesativadas);
      
        while (no != NULL) {
            Aresta aRest = (Aresta) getInfo(no);
          
            if (aRest != NULL) onAresta(aRest);
            no = proximoNo(no);
        }
        deletaLista(arestasDesativadas);
        return NULL;
    }
    
    Lista componentes = criaLista();
    if (componentes == NULL) {
        free(visitado);
        No no = primeiroNo(arestasDesativadas);
      
        while (no != NULL) {
            Aresta aRest = (Aresta) getInfo(no);
          
            if (aRest != NULL) onAresta(aRest);
            no = proximoNo(no);
        }
        deletaLista(arestasDesativadas);
        return NULL;
    }
    
    DFSContext ctx;
    ctx.g = g;
    ctx.visitado = visitado;
    
    int numComponentes = 0;
    Vertice vertice = primeiroVertice(g);
  
    while (vertice != NULL) {
        int idx = getIndiceVertice(vertice);
        
        if (!visitado[idx]) {
            ComponenteStruct* comp = criarComponente();
          
            if (comp == NULL) {
                free(visitado);
                deletaLista(componentes);
                No no = primeiroNo(arestasDesativadas);
              
                while (no != NULL) {
                    Aresta aRest = (Aresta) getInfo(no);
                  
                    if (aRest != NULL) onAresta(aRest);
                    no = proximoNo(no);
                }
                deletaLista(arestasDesativadas);
                return NULL;
            }
            
            ctx.componenteAtual = comp;
            dfs(vertice, &ctx);
            
            finalizarBoundingBox(comp);
            atribuirCor(comp, numComponentes);            
            inserirFim(componentes, comp);
            numComponentes++;
        }
        
        vertice = proximoVertice(g, vertice);
    }
    
    No no = primeiroNo(arestasDesativadas);
  
    while (no != NULL) {
        Aresta aRest = (Aresta) getInfo(no);
      
        if (aRest != NULL) onAresta(aRest);
        no = proximoNo(no);
    }
    deletaLista(arestasDesativadas);
    
    free(visitado);
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
