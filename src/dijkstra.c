#include "dijkstra.h"
#include "lista.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <float.h>
#include <math.h>

#define INFINITO DBL_MAX

typedef struct CaminhoStruct {
    Lista arestas;          
    double custoTotal;      
} CaminhoStruct;

typedef struct {
    double dist;
    Vertice predecessor;
    Aresta arestaPredecessora;
    bool visitado;
} InfoDijkstra;

static Vertice encontrarMenorDistancia(InfoDijkstra* info, Grafo g) {
    double menor = INFINITO;
    Vertice escolhido = NULL;
    
    Vertice v = primeiroVertice(g);
    while (v != NULL) {
        int idx = getIndiceVertice(v);
        if (!info[idx].visitado && info[idx].dist < menor) {
            menor = info[idx].dist;
            escolhido = v;
        }
        v = proximoVertice(g, v);
    }
    
    return escolhido;
}

static InfoDijkstra* executarDijkstra(Grafo g, Vertice origem, bool porTempo) {
    assert(g != NULL);
    assert(origem != NULL);
    
    int numVertices = quantVertices(g);
    if (numVertices == 0) return NULL;
    
    InfoDijkstra* info = calloc(numVertices, sizeof(InfoDijkstra));
    if (info == NULL) return NULL;
    
    Vertice v = primeiroVertice(g);
    while (v != NULL) {
        int idx = getIndiceVertice(v);
        info[idx].dist = INFINITO;
        info[idx].predecessor = NULL;
        info[idx].arestaPredecessora = NULL;
        info[idx].visitado = false;
        v = proximoVertice(g, v);
    }
    
    int idxOrigem = getIndiceVertice(origem);
    info[idxOrigem].dist = 0.0;
    
    for (int i = 0; i < numVertices; i++) {
        Vertice u = encontrarMenorDistancia(info, g);
        if (u == NULL) break; 
        
        int idxU = getIndiceVertice(u);
        info[idxU].visitado = true;
        
        Aresta a = primeiraArestaAdj(u);
        while (a != NULL) {
            if (arestaAtiva(a)) {
                Vertice destino = getDestino(a);
                int idxV = getIndiceVertice(destino);
                
                if (!info[idxV].visitado) {
                    double peso;
                    if (porTempo) {
                        peso = getComprimento(a) / getVelocidadeMedia(a);
                    } else {
                        peso = getComprimento(a);
                    }
                    
                    double novaDist = info[idxU].dist + peso;
                    if (novaDist < info[idxV].dist) {
                        info[idxV].dist = novaDist;
                        info[idxV].predecessor = u;
                        info[idxV].arestaPredecessora = a;
                    }
                }
            }
            a = proximaArestaAdj(u, a);
        }
    }  
    return info;
}

static Caminho reconstruirCaminho(InfoDijkstra* info, Grafo g, Vertice destino, double custoTotal) {
    assert(info != NULL);
    assert(destino != NULL);
    
    int idxDestino = getIndiceVertice(destino);
    if (info[idxDestino].dist >= INFINITO) return NULL; 
    
    CaminhoStruct* caminho = malloc(sizeof(CaminhoStruct));
    if (caminho == NULL) return NULL;
    
    caminho->arestas = criaLista();
    if (caminho->arestas == NULL) {
        free(caminho);
        return NULL;
    }
    
    caminho->custoTotal = custoTotal;
    Vertice atual = destino;
    Lista reverso = criaLista();
  
    if (reverso == NULL) {
        deletaLista(caminho->arestas);
        free(caminho);
        return NULL;
    }
    
    while (atual != NULL) {
        int idx = getIndiceVertice(atual);
        Aresta a = info[idx].arestaPredecessora;
        if (a != NULL) {
            inserirInicio(reverso, a);
        }
        atual = info[idx].predecessor;
    }
    
    No no = primeiroNo(reverso);
    while (no != NULL) {
        Aresta a = (Aresta) getInfo(no);
        inserirFim(caminho->arestas, a);
        no = proximoNo(no);
    }
    
    deletaLista(reverso);
    return caminho;
}


Caminho menorCaminho(Grafo g, const char* origem, const char* destino) {
    assert(g != NULL);
    assert(origem != NULL);
    assert(destino != NULL);
    
    Vertice vOrigem = buscarVertice(g, origem);
    Vertice vDestino = buscarVertice(g, destino);
    
    if (vOrigem == NULL || vDestino == NULL) return NULL;
    
    InfoDijkstra* info = executarDijkstra(g, vOrigem, false);
    if (info == NULL) return NULL;
    
    int idxDestino = getIndiceVertice(vDestino);
    Caminho caminho = reconstruirCaminho(info, g, vDestino, info[idxDestino].dist);
    
    free(info);
    return caminho;
}


Caminho caminhoMaisRapido(Grafo g, const char* origem, const char* destino) {
    assert(g != NULL);
    assert(origem != NULL);
    assert(destino != NULL);
    
    Vertice vOrigem = buscarVertice(g, origem);
    Vertice vDestino = buscarVertice(g, destino);
    
    if (vOrigem == NULL || vDestino == NULL) return NULL;
    
    InfoDijkstra* info = executarDijkstra(g, vOrigem, true);
    if (info == NULL) return NULL;
    
    int idxDestino = getIndiceVertice(vDestino);
    Caminho caminho = reconstruirCaminho(info, g, vDestino, info[idxDestino].dist);
    
    free(info);
    return caminho;
}


void deletaCaminho(Caminho c) {
    if (c == NULL) return;
    CaminhoStruct* caminho = (CaminhoStruct*) c;
    deletaLista(caminho->arestas);
    free(caminho);
}


double getCustoTotal(Caminho c) {
    assert(c != NULL);
    CaminhoStruct* caminho = (CaminhoStruct*) c;
    return caminho->custoTotal;
}


int tamanhoCaminho(Caminho c) {
    assert(c != NULL);
    CaminhoStruct* caminho = (CaminhoStruct*) c;
    return tamanhoLista(caminho->arestas);
}


Aresta getArestaCaminho(Caminho c, int indice) {
    assert(c != NULL);
    assert(indice >= 0);
    
    CaminhoStruct* caminho = (CaminhoStruct*) c;   
    if (indice >= tamanhoLista(caminho->arestas)) return NULL;
    
    No no = primeiroNo(caminho->arestas);
    for (int i = 0; i < indice; i++) {
        no = proximoNo(no);
        if (no == NULL) return NULL;
    }
    
    return (Aresta) getInfo(no);
}
