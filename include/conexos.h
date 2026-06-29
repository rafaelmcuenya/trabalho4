#ifndef CONEXOS_H
#define CONEXOS_H

#include "grafo.h"
#include <stdbool.h>

/*
   Módulo para identificação de componentes conexos no grafo de Bitnópolis.
   Utilizado para encontrar zonas viárias na cidade baseado na velocidade média das vias.
*/

typedef void* Componente;
typedef void* ListaComponentes;

ListaComponentes encontrarComponentes(Grafo g, double vl);
/*
   Função que encontra os componentes conexos do grafo.
   Primeiro parâmetro é o grafo a ser analisado.
   Segundo parâmetro é o valor limite de velocidade (vl).
   Apenas arestas com velocidade média >= vl são consideradas para conexão.
   Retorna uma lista de componentes conexos.
*/

int quantComponentes(ListaComponentes lc);
/*
   Função que retorna a quantidade de componentes conexos.
   Recebe como parâmetro a lista de componentes.
   Retorna o número de componentes.
*/

Componente getComponente(ListaComponentes lc, int indice);
/*
   Função que retorna um componente específico da lista.
   Primeiro parâmetro é a lista de componentes.
   Segundo parâmetro é o índice do componente desejado.
   Retorna o componente na posição informada.
*/

int tamanhoComponente(Componente c);
/*
   Função que retorna a quantidade de vértices em um componente.
   Recebe como parâmetro o componente desejado.
   Retorna o número de vértices no componente.
*/

Vertice getVerticeComponente(Componente c, int indice);
/*
   Função que retorna um vértice específico do componente.
   Primeiro parâmetro é o componente desejado.
   Segundo parâmetro é o índice do vértice.
   Retorna o vértice na posição informada.
*/

double getBBoxX(Componente c);
/*
   Função que retorna a coordenada X do canto superior esquerdo do bounding box.
   Recebe como parâmetro o componente desejado.
   Retorna a coordenada X do bounding box.
*/

double getBBoxY(Componente c);
/*
   Função que retorna a coordenada Y do canto superior esquerdo do bounding box.
   Recebe como parâmetro o componente desejado.
   Retorna a coordenada Y do bounding box.
*/

double getBBoxW(Componente c);
/*
   Função que retorna a largura do bounding box.
   Recebe como parâmetro o componente desejado.
   Retorna a largura do bounding box.
*/

double getBBoxH(Componente c);
/*
   Função que retorna a altura do bounding box.
   Recebe como parâmetro o componente desejado.
   Retorna a altura do bounding box.
*/

const char* getCorComponente(Componente c);
/*
   Função que retorna a cor associada ao componente.
   Recebe como parâmetro o componente desejado.
   Retorna uma string com o nome da cor.
*/

void deletaListaComponentes(ListaComponentes lc);
/*
   Função que deleta a lista de componentes.
   Recebe como parâmetro a lista de componentes.
   Libera toda a memória alocada.
*/

#endif
