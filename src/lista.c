#include "lista.h"
#include <stdlib.h>

typedef struct No{
    void* info;
    struct No* ant;
    struct No* prox;
} No;

typedef struct Lista{
    No* primeiro;
    No* ultimo;
    int tamanho;
} Lista;


Lista criaLista(void){
    Lista* l = malloc(sizeof(Lista));

    if(l == NULL)return NULL;

    l->primeiro = NULL;
    l->ultimo = NULL;
    l->tamanho = 0;
    return l;
}


void inserirInicio(Lista l, void* info){
    if(l == NULL)return;

    Lista* lista = (Lista*) l;
    No* novo = malloc(sizeof(No));

    if(novo == NULL)return;

    novo->info = info;
    novo->ant = NULL;
    novo->prox = lista->primeiro;

    if(lista->primeiro != NULL){
        lista->primeiro->ant = novo;
    }

    lista->primeiro = novo;

    if(lista->ultimo == NULL){
        lista->ultimo = novo;
    }
    
    lista->tamanho++;
}


void inserirFim(Lista l, void* info){
    if(l == NULL)return;

    Lista* lista = (Lista*) l;
    No* novo = malloc(sizeof(No));

    if(novo == NULL)return;

    novo->info = info;
    novo->prox = NULL;
    novo->ant = lista->ultimo;

    if(lista->ultimo != NULL){
        lista->ultimo->prox = novo;
    }

    lista->ultimo = novo;

    if(lista->primeiro == NULL){
        lista->primeiro = novo;
    }

    lista->tamanho++;
}


void removerNo(Lista l, No n){
    if(l == NULL || n == NULL)return;

    Lista* lista = (Lista*) l;
    No* no = (No*) n;

    if(no->ant != NULL){
        no->ant->prox = no->prox;
    }else{
        lista->primeiro = no->prox;
    }

    if(no->prox != NULL){
        no->prox->ant = no->ant;
    }else{
        lista->ultimo = no->ant;
    }

    free(no);
    lista->tamanho--;
}


int tamanhoLista(Lista l){
    if(l == NULL)return 0;
    return ((Lista*) l)->tamanho;
}


bool listaVazia(Lista l){
    return tamanhoLista(l) == 0;
}

No primeiroNo(Lista l){
    if(l == NULL)return NULL;
    return ((Lista*) l)->primeiro;
}

No ultimoNo(Lista l){
    if(l == NULL)return NULL;
    return ((Lista*) l)->ultimo;
}

No proximoNo(No n){
    if(n == NULL)return NULL;
    return ((No*) n)->prox;
}

No anteriorNo(No n){
    if(n == NULL)return NULL;
    return ((No*) n)->ant;
}

No buscarNo(Lista l, void* info){
    if(l == NULL)
        return NULL;

    for(No n = primeiroNo(l);
        n != NULL;
        n = proximoNo(n)){

        if(getInfo(n) == info)
            return n;
    }

    return NULL;
}

void* getInfo(No n){
    if(n == NULL)return NULL;
    return ((No*) n)->info;
}

void setInfo(No n, void* info){
    if(n == NULL)return;
    ((No*) n)->info = info;
}

void deletaLista(Lista l){
    if(l == NULL)return;

    Lista* lista = (Lista*) l;
    No* atual = lista->primeiro;

    while(atual != NULL){
        No* prox = atual->prox;
        free(atual);
        atual = prox;
    }

    free(lista);
}
