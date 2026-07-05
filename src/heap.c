#include "heap.h"
#include <stdlib.h>
#include <assert.h>
#include <float.h>

typedef struct {
    Vertice vertice;
    double distancia;
} ElementoHeap;

typedef struct HeapStruct {
    ElementoHeap* elementos;
    int capacidade;
    int tamanho;
} HeapStruct;

static void heapifyUp(HeapStruct* h, int idx) {
    while (idx > 0) {
        int pai = (idx - 1) / 2;
        if (h->elementos[idx].distancia >= h->elementos[pai].distancia) {
            break;
        }
        ElementoHeap temp = h->elementos[idx];
        h->elementos[idx] = h->elementos[pai];
        h->elementos[pai] = temp;
        idx = pai;
    }
}

static void heapifyDown(HeapStruct* h, int idx) {
    int menor = idx;
    int esquerdo = 2 * idx + 1;
    int direito = 2 * idx + 2;
    
    if (esquerdo < h->tamanho && h->elementos[esquerdo].distancia < h->elementos[menor].distancia) {
        menor = esquerdo;
    }
    if (direito < h->tamanho && h->elementos[direito].distancia < h->elementos[menor].distancia) {
        menor = direito;
    }
    
    if (menor != idx) {
        ElementoHeap temp = h->elementos[idx];
        h->elementos[idx] = h->elementos[menor];
        h->elementos[menor] = temp;
        heapifyDown(h, menor);
    }
}

Heap criarHeap(int capacidade) {
    assert(capacidade > 0);
    
    HeapStruct* h = malloc(sizeof(HeapStruct));
    if (h == NULL) return NULL;
    
    h->elementos = malloc(capacidade * sizeof(ElementoHeap));
    if (h->elementos == NULL) {
        free(h);
        return NULL;
    }
    
    h->capacidade = capacidade;
    h->tamanho = 0;
    
    return h;
}

void deletaHeap(Heap h) {
    if (h == NULL) return;
    
    HeapStruct* heap = (HeapStruct*) h;
    free(heap->elementos);
    free(heap);
}

void inserirHeap(Heap h, Vertice v, double distancia) {
    assert(h != NULL);
    assert(v != NULL);
    
    HeapStruct* heap = (HeapStruct*) h;
    
    if (heap->tamanho >= heap->capacidade) {
        int novaCapacidade = heap->capacidade * 2;
        ElementoHeap* novos = realloc(heap->elementos, novaCapacidade * sizeof(ElementoHeap));
        if (novos == NULL) return;
        heap->elementos = novos;
        heap->capacidade = novaCapacidade;
    }
    
    heap->elementos[heap->tamanho].vertice = v;
    heap->elementos[heap->tamanho].distancia = distancia;
    heap->tamanho++;
    
    heapifyUp(heap, heap->tamanho - 1);
}

Vertice extrairMin(Heap h) {
    assert(h != NULL);
    
    HeapStruct* heap = (HeapStruct*) h;
    if (heap->tamanho == 0) return NULL;
    
    Vertice min = heap->elementos[0].vertice;
    
    heap->tamanho--;
    if (heap->tamanho > 0) {
        heap->elementos[0] = heap->elementos[heap->tamanho];
        heapifyDown(heap, 0);
    }
    
    return min;
}

void atualizarDistancia(Heap h, Vertice v, double novaDistancia) {
    assert(h != NULL);
    assert(v != NULL);
    
    HeapStruct* heap = (HeapStruct*) h;
    
    for (int i = 0; i < heap->tamanho; i++) {
        if (heap->elementos[i].vertice == v) {
            if (novaDistancia < heap->elementos[i].distancia) {
                heap->elementos[i].distancia = novaDistancia;
                heapifyUp(heap, i);
            }
            return;
        }
    }
    
    inserirHeap(h, v, novaDistancia);
}

bool heapVazia(Heap h) {
    assert(h != NULL);
    HeapStruct* heap = (HeapStruct*) h;
    return heap->tamanho == 0;
}

int tamanhoHeap(Heap h) {
    assert(h != NULL);
    HeapStruct* heap = (HeapStruct*) h;
    return heap->tamanho;
}
