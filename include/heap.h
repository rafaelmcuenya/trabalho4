#ifndef HEAP_H
#define HEAP_H

#include "grafo.h"
#include <stdbool.h>

/*
   Módulo para fila de prioridade (heap mínima) utilizada pelo algoritmo de Dijkstra.
   Armazena vértices com suas respectivas distâncias.
*/

typedef void* Heap;

Heap criarHeap(int capacidade);
/*
   Função que cria uma fila de prioridade vazia.
   Primeiro parâmetro é a capacidade inicial da heap.
   Retorna um ponteiro para a heap criada.
*/

void deletaHeap(Heap h);
/*
   Função que deleta a fila de prioridade.
   Recebe como parâmetro a heap a ser deletada.
*/

void inserirHeap(Heap h, Vertice v, double distancia);
/*
   Função que insere um vértice na fila de prioridade.
   Primeiro parâmetro é a heap.
   Segundo parâmetro é o vértice.
   Terceiro parâmetro é a distância associada ao vértice.
*/

Vertice extrairMin(Heap h);
/*
   Função que extrai o vértice com menor distância.
   Recebe como parâmetro a heap.
   Retorna o vértice com menor distância ou NULL se vazia.
*/

void atualizarDistancia(Heap h, Vertice v, double novaDistancia);
/*
   Função que atualiza a distância de um vértice na heap.
   Primeiro parâmetro é a heap.
   Segundo parâmetro é o vértice.
   Terceiro parâmetro é a nova distância.
*/

bool heapVazia(Heap h);
/*
   Função que verifica se a heap está vazia.
   Recebe como parâmetro a heap.
   Retorna true se vazia, false caso contrário.
*/

int tamanhoHeap(Heap h);
/*
   Função que retorna o número de elementos na heap.
   Recebe como parâmetro a heap.
   Retorna a quantidade de elementos.
*/

#endif
