#ifndef DISJOINT_H
#define DISJOINT_H

#include "grafo.h"
#include <stdbool.h>

/*
   Módulo auxiliar de Union-Find (Disjoint Set Union - DSU).
   Será uilizado pelo módulo AGM em alguma de suas funções.
*/

typedef void* Conjunto;

Conjunto criarConjunto(Grafo g);
/*
   Função que cria uma estrutura Union-Find a partir de um grafo.
   Cada vértice do grafo inicia como um conjunto separado.
   Primeiro parâmetro é o grafo contendo os vértices.
   Retorna um ponteiro para a estrutura Union-Find.
*/

void deletaConjunto(Conjunto c);
/*
   Função que deleta a estrutura Union-Find.
   Recebe como parâmetro a estrutura a ser deletada.
   Libera toda a memória alocada.
*/

int find(Conjunto c, Vertice v);
/*
   Função que encontra a raiz do conjunto do vértice.
   Primeiro parâmetro é a estrutura Union-Find.
   Segundo parâmetro é o vértice desejado.
   Retorna o índice do representante do conjunto.
   ATENÇÃO: Aplica compressão de caminho (path compression).
*/

void unionSet(Conjunto c, Vertice v1, Vertice v2);
/*
   Função que une os conjuntos dos vértices v1 e v2.
   Primeiro parâmetro é a estrutura Union-Find.
   Segundo parâmetro é o primeiro vértice.
   Terceiro parâmetro é o segundo vértice.
   ATENÇÃO: Aplica união por rank.
*/

bool sameSet(Conjunto c, Vertice v1, Vertice v2);
/*
   Função que verifica se dois vértices pertencem ao mesmo conjunto.
   Primeiro parâmetro é a estrutura Union-Find.
   Segundo parâmetro é o primeiro vértice.
   Terceiro parâmetro é o segundo vértice.
   Retorna true se estiverem no mesmo conjunto, false caso contrário.
*/

int numConjuntos(Conjunto c);
/*
   Função que retorna o número de conjuntos distintos.
   Recebe como parâmetro a estrutura Union-Find.
   Retorna a quantidade de conjuntos.
*/

#endif
