#ifndef AGM_H
#define AGM_H

#include "grafo.h"
#include "disjoint.h"

/*
   Módulo para cálculo da Árvore Geradora Mínima (AGM) utilizando o algoritmo de Kruskal.
   Utilizado pelo módulo leitor, para poder executar a função "exp".
*/

typedef void* ArvoreGeradoraMinima;

ArvoreGeradoraMinima calcularAGM(Grafo g, double vl);
/*
   Função que calcula a Árvore Geradora Mínima (AGM) do grafo.
   Primeiro parâmetro é o grafo a ser analisado.
   Segundo parâmetro é o valor limite de velocidade dado.
   Considera apenas arestas com velocidade média < vl para a AGM.
   Utiliza o comprimento das arestas como peso.
   Retorna uma estrutura contendo as arestas selecionadas pela AGM.
*/

int tamanhoAGM(ArvoreGeradoraMinima agm);
/*
   Função que retorna a quantidade de arestas na AGM.
   Recebe como parâmetro a estrutura da AGM.
   Retorna o número de arestas selecionadas.
*/

Aresta getArestaAGM(ArvoreGeradoraMinima agm, int indice);
/*
   Função que retorna uma aresta específica da AGM.
   Primeiro parâmetro é a estrutura da AGM.
   Segundo parâmetro é o índice da aresta desejada.
   Retorna a aresta na posição informada.
*/

double getCustoTotalAGM(ArvoreGeradoraMinima agm);
/*
   Função que retorna o custo total da AGM (soma dos comprimentos das arestas).
   Recebe como parâmetro a estrutura da AGM.
   Retorna o custo total.
*/

void deletaAGM(ArvoreGeradoraMinima agm);
/*
   Função que deleta a estrutura da AGM.
   Recebe como parâmetro a estrutura a ser deletada.
   Libera toda a memória alocada.
*/

#endif
