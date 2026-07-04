#include "agm.h"
#include "lista.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <float.h>
#include <math.h>

typedef struct {
    Aresta aresta;
    double peso;
} ArestaPonderada;

typedef struct AGMStruct {
    Lista arestas;         
    double custoTotal;    
} AGMStruct;

static int compararArestas(const void* a, const void* b) {
    const ArestaPonderada* ap1 = (const ArestaPonderada*) a;
    const ArestaPonderada* ap2 = (const ArestaPonderada*) b;
    
    if (ap1->peso < ap2->peso) return -1;
    if (ap1->peso > ap2->peso) return 1;
    return 0;
}

static int contarArestasViaveis(Grafo g, double vl) {
    int count = 0;
    
    Vertice v = primeiroVertice(g);
    while (v != NULL) {
        Aresta a = primeiraArestaAdj(v);
        while (a != NULL) {
            if (arestaAtiva(a) && getVelocidadeMedia(a) < vl) {
                const char* idOrigem = getVerticeId(getOrigem(a));
                const char* idDestino = getVerticeId(getDestino(a));
              
                if (strcmp(idOrigem, idDestino) < 0) {
                    count++;
                }
            }
            a = proximaArestaAdj(v, a);
        }
        v = proximoVertice(g, v);
    }
    return count;
}

static int preencherArestasViaveis(Grafo g, double vl, ArestaPonderada* arestas) {
    int index = 0;
    Vertice v = primeiroVertice(g);
  
    while (v != NULL) {
        Aresta a = primeiraArestaAdj(v);
        while (a != NULL) {
            if (arestaAtiva(a) && getVelocidadeMedia(a) < vl) {
                const char* idOrigem = getVerticeId(getOrigem(a));
                const char* idDestino = getVerticeId(getDestino(a));
              
                if (strcmp(idOrigem, idDestino) < 0) {
                    arestas[index].aresta = a;
                    arestas[index].peso = getComprimento(a);
                    index++;
                }
            }
            a = proximaArestaAdj(v, a);
        }
        v = proximoVertice(g, v);
    }
    return index;
}

static bool arestaJaSelecionada(Lista arestasSelecionadas, Aresta a) {
    No no = primeiroNo(arestasSelecionadas);
  
    while (no != NULL) {
        Aresta selecionada = (Aresta) getInfo(no);
        if (selecionada == a) return true;
        no = proximoNo(no);
    }
    return false;
}

ArvoreGeradoraMinima calcularAGM(Grafo g, double vl) {
    assert(g != NULL);
    assert(vl >= 0);
    
    int numVertices = quantVertices(g);
    if (numVertices == 0) return NULL;
    
    int numArestasViaveis = contarArestasViaveis(g, vl);
    if (numArestasViaveis == 0) return NULL;
    
    ArestaPonderada* arestasViaveis = malloc(numArestasViaveis * sizeof(ArestaPonderada));
    if (arestasViaveis == NULL) return NULL;
    
    int numArestas = preencherArestasViaveis(g, vl, arestasViaveis);
    if (numArestas == 0) {
        free(arestasViaveis);
        return NULL;
    }
    
    qsort(arestasViaveis, numArestas, sizeof(ArestaPonderada), compararArestas);
    Conjunto conjunto = criarConjunto(g);
  
    if (conjunto == NULL) {
        free(arestasViaveis);
        return NULL;
    }
    
    AGMStruct* agm = malloc(sizeof(AGMStruct));
    if (agm == NULL) {
        deletaConjunto(conjunto);
        free(arestasViaveis);
        return NULL;
    }
    
    agm->arestas = criaLista();
  
    if (agm->arestas == NULL) {
        free(agm);
        deletaConjunto(conjunto);
        free(arestasViaveis);
        return NULL;
    }
    
    agm->custoTotal = 0.0;
    int arestasSelecionadas = 0;
  
    for (int i = 0; i < numArestas && arestasSelecionadas < numVertices - 1; i++) {
        Aresta a = arestasViaveis[i].aresta;
        Vertice origem = getOrigem(a);
        Vertice destino = getDestino(a);
        
        if (!sameSet(conjunto, origem, destino)) {
            inserirFim(agm->arestas, a);
            agm->custoTotal += arestasViaveis[i].peso;
            arestasSelecionadas++;            
            unionSet(conjunto, origem, destino);
        }
    }
    
    deletaConjunto(conjunto);
    free(arestasViaveis);
    return agm;
}

int tamanhoAGM(ArvoreGeradoraMinima agm) {
    assert(agm != NULL);
    AGMStruct* agmStruct = (AGMStruct*) agm;
    return tamanhoLista(agmStruct->arestas);
}

Aresta getArestaAGM(ArvoreGeradoraMinima agm, int indice) {
    assert(agm != NULL);
    assert(indice >= 0);
    
    AGMStruct* agmStruct = (AGMStruct*) agm;
    
    if (indice >= tamanhoLista(agmStruct->arestas)) return NULL;
    No no = primeiroNo(agmStruct->arestas);
  
    for (int i = 0; i < indice; i++) {
        no = proximoNo(no);
        if (no == NULL) return NULL;
    }
    return (Aresta) getInfo(no);
}

double getCustoTotalAGM(ArvoreGeradoraMinima agm) {
    assert(agm != NULL);
    AGMStruct* agmStruct = (AGMStruct*) agm;
    return agmStruct->custoTotal;
}

void deletaAGM(ArvoreGeradoraMinima agm) {
    if (agm == NULL) return;
    AGMStruct* agmStruct = (AGMStruct*) agm;
    deletaLista(agmStruct->arestas);
    free(agmStruct);
}
