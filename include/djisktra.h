#ifndef DIJKSTRA_H
#define DIJKSTRA_H
#include "grafo.h"

/*
   O algoritmo de Dijkstra será utilizado para encontrar caminhos mínimos em um grafo.
   Neste projeto ele será utilizado tanto para encontrar o caminho mais curto quanto
   o caminho mais rápido entre dois vértices.
*/

typedef void* Caminho;

Caminho menorCaminho(Grafo g, const char* origem, const char* destino);
/*
   Função que calcula o menor caminho entre dois vértices.
   Primeiro parâmetro é o grafo onde será realizada a busca.
   Segundo parâmetro é o ID do vértice de origem.
   Terceiro parâmetro é o ID do vértice de destino.
   Utiliza o comprimento das arestas como peso.
   Retorna um ponteiro para o caminho encontrado.
*/

Caminho caminhoMaisRapido(Grafo g, const char* origem, const char* destino);
/*
   Função que calcula o caminho mais rápido entre dois vértices.
   Primeiro parâmetro é o grafo onde será realizada a busca.
   Segundo parâmetro é o ID do vértice de origem.
   Terceiro parâmetro é o ID do vértice de destino.
   Utiliza o tempo de percurso das arestas como peso.
   Retorna um ponteiro para o caminho encontrado.
*/

void deletaCaminho(Caminho c);
/*
   Função que deleta um caminho.
   Recebe como parâmetro o ponteiro para o caminho.
*/

double getCustoTotal(Caminho c);
/*
   Função que retorna o custo total de um caminho.
   Recebe como parâmetro o caminho desejado.
   Retorna o custo total calculado pelo algoritmo.
*/

int tamanhoCaminho(Caminho c);
/*
   Função que retorna a quantidade de arestas presentes no caminho.
   Recebe como parâmetro o caminho desejado.
   Retorna a quantidade de arestas do caminho.
*/

Aresta getArestaCaminho(Caminho c, int indice);
/*
   Função que retorna uma aresta pertencente ao caminho.
   Primeiro parâmetro é o caminho desejado.
   Segundo parâmetro é a posição da aresta no caminho.
   Retorna a aresta armazenada na posição informada.
*/

#endif
