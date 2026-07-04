#include "disjoint.h"
#include <stdlib.h>
#include <assert.h>

typedef struct {
    int parent;    
    int rank;      
} NodeInfo;

typedef struct ConjuntoStruct {
    Grafo grafo;
    NodeInfo* nodes;     
    int numVertices;
    int numConjuntos;
} ConjuntoStruct;


static int getIndice(Vertice v) {
    assert(v != NULL);
    return getIndiceVertice(v);
}


Conjunto criarConjunto(Grafo g) {
    assert(g != NULL);
    int numVertices = quantVertices(g);
    if (numVertices == 0) return NULL;
    
    ConjuntoStruct* c = malloc(sizeof(ConjuntoStruct));
    if (c == NULL) return NULL;
    
    c->grafo = g;
    c->numVertices = numVertices;
    c->numConjuntos = numVertices;  
    c->nodes = malloc(numVertices * sizeof(NodeInfo));
  
    if (c->nodes == NULL) {
        free(c);
        return NULL;
    }
    
    for (int i = 0; i < numVertices; i++) {
        c->nodes[i].parent = i;   
        c->nodes[i].rank = 0;      
    }
    
    return c;
}


void deletaConjunto(Conjunto c) {
    if (c == NULL) return;
    
    ConjuntoStruct* conj = (ConjuntoStruct*) c;
    free(conj->nodes);
    free(conj);
}


int find(Conjunto c, Vertice v) {
    assert(c != NULL);
    assert(v != NULL);
    
    ConjuntoStruct* conj = (ConjuntoStruct*) c;
    int idx = getIndice(v);
    assert(idx >= 0 && idx < conj->numVertices);
    
    if (conj->nodes[idx].parent != idx) {
        Vertice parentVertice = getVerticeIndice(conj->grafo, conj->nodes[idx].parent);
        conj->nodes[idx].parent = find(c, parentVertice);
    }
    
    return conj->nodes[idx].parent;
}


void unionSet(Conjunto c, Vertice v1, Vertice v2) {
    assert(c != NULL);
    assert(v1 != NULL);
    assert(v2 != NULL);
    
    ConjuntoStruct* conj = (ConjuntoStruct*) c;
    
    int root1 = find(c, v1);
    int root2 = find(c, v2);
    if (root1 == root2) return;
    
    if (conj->nodes[root1].rank < conj->nodes[root2].rank) {
        conj->nodes[root1].parent = root2;
    } else if (conj->nodes[root1].rank > conj->nodes[root2].rank) {
        conj->nodes[root2].parent = root1;
    } else {
        conj->nodes[root2].parent = root1;
        conj->nodes[root1].rank++;
    }
    
    conj->numConjuntos--;
}


bool sameSet(Conjunto c, Vertice v1, Vertice v2) {
    assert(c != NULL);
    assert(v1 != NULL);
    assert(v2 != NULL);
    return find(c, v1) == find(c, v2);
}


int numConjuntos(Conjunto c) {
    assert(c != NULL);
    ConjuntoStruct* conj = (ConjuntoStruct*) c;
    return conj->numConjuntos;
}
