#include "grafo.h"
#include "lista.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>


typedef struct VerticeStruct{
    char* id;
    double x;
    double y;
    int indice;              
    Lista arestasAdj;
}VerticeStruct;


typedef struct ArestaStruct{
    Vertice origem;
    Vertice destino;
    char* ldir;
    char* lesq;
    char* nomeRua;
    double comprimento;
    double velMedia;
    bool ativa;
}ArestaStruct;


typedef struct GrafoStruct{
    Lista vertices;
    Lista arestas;
    int numVertices;
    int numArestas;
}GrafoStruct;


static char* copiarString(const char* str){
    if(str == NULL) return NULL;
    char* copia = malloc(strlen(str) + 1);
    if(copia == NULL) return NULL;
    
    strcpy(copia, str);
    return copia;
}


Grafo criaGrafo(void){
    GrafoStruct* g = malloc(sizeof(GrafoStruct));
    if(g == NULL) return NULL;

    g->vertices = criaLista();
    g->arestas = criaLista();

    if(g->vertices == NULL || g->arestas == NULL){
        if(g->vertices != NULL) deletaLista(g->vertices);
        if(g->arestas != NULL) deletaLista(g->arestas);

        free(g);
        return NULL;
    }

    g->numVertices = 0;
    g->numArestas = 0;
    return g;
}


void deletaGrafo(Grafo g){
    if(g == NULL) return;
    
    GrafoStruct* grafo = (GrafoStruct*) g;
    
    No noAresta = primeiroNo(grafo->arestas);
    while(noAresta != NULL){
        No prox = proximoNo(noAresta);
        ArestaStruct* a = (ArestaStruct*) getInfo(noAresta);
        if(a != NULL){
            free(a->ldir);
            free(a->lesq);
            free(a->nomeRua);
            free(a);
            setInfo(noAresta, NULL);
        }
        noAresta = prox;
    }
    deletaLista(grafo->arestas);
    
    No noVertice = primeiroNo(grafo->vertices);
    while(noVertice != NULL){
        No prox = proximoNo(noVertice);
        VerticeStruct* v = (VerticeStruct*) getInfo(noVertice);
        if(v != NULL){
            free(v->id);
            deletaLista(v->arestasAdj);
            free(v);
            setInfo(noVertice, NULL);
        }
        noVertice = prox;
    }
    deletaLista(grafo->vertices);
    
    free(grafo);
}


Vertice adicionarVertice(Grafo g, const char* id, double x, double y){
    assert(g != NULL);
    assert(id != NULL);
    
    if(buscarVertice(g, id) != NULL) return NULL;

    GrafoStruct* grafo = (GrafoStruct*) g;
    VerticeStruct* v = malloc(sizeof(VerticeStruct));

    if(v == NULL) return NULL;

    v->id = copiarString(id);

    if(v->id == NULL){
        free(v);
        return NULL;
    }

    v->x = x;
    v->y = y;
    v->indice = grafo->numVertices;  
    v->arestasAdj = criaLista();

    if(v->arestasAdj == NULL){
        free(v->id);
        free(v);
        return NULL;
    }

    inserirFim(grafo->vertices, v);
    grafo->numVertices++;
    return v;
}


Vertice buscarVertice(Grafo g, const char* id){
    assert(g != NULL);
    assert(id != NULL);

    GrafoStruct* grafo = (GrafoStruct*) g;

    for(No n = primeiroNo(grafo->vertices);
        n != NULL; n = proximoNo(n)){

        VerticeStruct* v = (VerticeStruct*) getInfo(n);
        if(v == NULL) continue;
        if(strcmp(v->id, id) == 0) return v;
    }

    return NULL;
}


Vertice getVerticeIndice(Grafo g, int indice){
    assert(g != NULL);
    assert(indice >= 0);
    
    GrafoStruct* grafo = (GrafoStruct*) g;
    
    if (indice >= grafo->numVertices) return NULL;
    
    No no = primeiroNo(grafo->vertices);
    for (int i = 0; i < indice; i++) {
        no = proximoNo(no);
        if (no == NULL) return NULL;
    }
    
    return getInfo(no);
}


double getVerticeX(Vertice v){
    assert(v != NULL);
    return ((VerticeStruct*) v)->x;
}


double getVerticeY(Vertice v){
    assert(v != NULL);
    return ((VerticeStruct*) v)->y;
}


const char* getVerticeId(Vertice v){
    assert(v != NULL);
    return ((VerticeStruct*) v)->id;
}


int getIndiceVertice(Vertice v){
    assert(v != NULL);
    return ((VerticeStruct*) v)->indice;
}


Aresta adicionarAresta(Grafo g, const char* origem, const char* destino,
                       const char* ldir, const char* lesq, double comp,
                       double velMedia, const char* nomeRua){
    assert(g != NULL);
    assert(origem != NULL);
    assert(destino != NULL);
    assert(comp > 0);
    assert(velMedia > 0);

    GrafoStruct* grafo = (GrafoStruct*) g;
    Vertice vOrigem = buscarVertice(g, origem);
    Vertice vDestino = buscarVertice(g, destino);

    if(vOrigem == NULL || vDestino == NULL) return NULL;
    if(buscarAresta(g, origem, destino) != NULL) return NULL;
    
    ArestaStruct* a = malloc(sizeof(ArestaStruct));
    if(a == NULL) return NULL;

    a->origem = vOrigem;
    a->destino = vDestino;
    a->ldir = copiarString(ldir);
    a->lesq = copiarString(lesq);
    a->nomeRua = copiarString(nomeRua);
    a->comprimento = comp;
    a->velMedia = velMedia;
    a->ativa = true;

    if((ldir != NULL && a->ldir == NULL) || (lesq != NULL && a->lesq == NULL) ||
        (nomeRua != NULL && a->nomeRua == NULL)){

        free(a->ldir);
        free(a->lesq);
        free(a->nomeRua);
        free(a);
        return NULL;
    }

    inserirFim(grafo->arestas, a);
    inserirFim(((VerticeStruct*)vOrigem)->arestasAdj, a);
    grafo->numArestas++;
    return a;
}


Aresta buscarAresta(Grafo g, const char* origem, const char* destino){
    assert(g != NULL);
    assert(origem != NULL);
    assert(destino != NULL);

    GrafoStruct* grafo = (GrafoStruct*) g;

    for(No n = primeiroNo(grafo->arestas);
        n != NULL; n = proximoNo(n)){

        ArestaStruct* a = (ArestaStruct*) getInfo(n);

        if(a == NULL) continue;

        if(strcmp(getVerticeId(a->origem), origem) == 0 &&
            strcmp(getVerticeId(a->destino), destino) == 0){
            return a;
        }
    }

    return NULL;
}


void offAresta(Aresta a){
    assert(a != NULL);
    ((ArestaStruct*)a)->ativa = false;
}


void onAresta(Aresta a){
    assert(a != NULL);
    ((ArestaStruct*)a)->ativa = true;
}


bool arestaAtiva(Aresta a){
    assert(a != NULL);
    return ((ArestaStruct*)a)->ativa;
}


Vertice getOrigem(Aresta a){
    assert(a != NULL);
    return ((ArestaStruct*)a)->origem;
}


Vertice getDestino(Aresta a){
    assert(a != NULL);
    return ((ArestaStruct*)a)->destino;
}


double getComprimento(Aresta a){
    assert(a != NULL);
    return ((ArestaStruct*)a)->comprimento;
}


double getVelocidadeMedia(Aresta a){
    assert(a != NULL);
    return ((ArestaStruct*)a)->velMedia;
}


void setVelocidadeMedia(Aresta a, double novaVel){
    assert(a != NULL);
    assert(novaVel >= 0);

    ((ArestaStruct*)a)->velMedia = novaVel;
}


const char* getNomeRua(Aresta a){
    assert(a != NULL);
    return ((ArestaStruct*)a)->nomeRua;
}


const char* getLdir(Aresta a){
    assert(a != NULL);
    return ((ArestaStruct*)a)->ldir;
}


const char* getLesq(Aresta a){
    assert(a != NULL);
    return ((ArestaStruct*)a)->lesq;
}


int quantVertices(Grafo g){
    assert(g != NULL);
    return ((GrafoStruct*)g)->numVertices;
}


int quantArestas(Grafo g){
    assert(g != NULL);
    return ((GrafoStruct*)g)->numArestas;
}


Vertice primeiroVertice(Grafo g){
    assert(g != NULL);
    
    GrafoStruct* grafo = (GrafoStruct*) g;
    No n = primeiroNo(grafo->vertices);
    
    if(n == NULL) return NULL;
    return getInfo(n);
}


Vertice proximoVertice(Grafo g, Vertice atual){
    assert(g != NULL);
    assert(atual != NULL);
    
    GrafoStruct* grafo = (GrafoStruct*) g;

    for(No n = primeiroNo(grafo->vertices);
        n != NULL; n = proximoNo(n)){

        if(getInfo(n) == atual){
            n = proximoNo(n);
            if(n == NULL) return NULL;
            return getInfo(n);
        }
    }
    return NULL;
}


Aresta primeiraArestaAdj(Vertice v){
    assert(v != NULL);
    
    VerticeStruct* vertice = (VerticeStruct*) v;
    No n = primeiroNo(vertice->arestasAdj);

    if(n == NULL) return NULL;
    return getInfo(n);
}


Aresta proximaArestaAdj(Vertice v, Aresta atual){
    assert(v != NULL);
    assert(atual != NULL);
    
    VerticeStruct* vertice = (VerticeStruct*) v;

    for(No n = primeiroNo(vertice->arestasAdj);
        n != NULL; n = proximoNo(n)){

        if(getInfo(n) == atual){
            n = proximoNo(n);
            if(n == NULL) return NULL;
            return getInfo(n);
        }
    }
    return NULL;
}
