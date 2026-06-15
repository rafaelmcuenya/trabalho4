#ifndef LISTA_H
#define LISTA_H

#include <stdio.h>
#include <stdbool.h>

/*
   Uma lista é uma estrutura de dados que organiza os elementos como uma fila, mas sem restrição de ordem
   de entrada/saída. Este módulo em si é utilizado em diversos outros módulos deste projeto.
*/

typedef void* Lista;
typedef void* No;

Lista criaLista(void);
/*
   Função para criar uma lista vazia.
   Retorna um ponteiro para a lista recém criada.
*/

void inserirInicio(Lista l, void* info);
/*
   Função para inserir uma informação no início da lista.
   Primeiro parâmetro é o ponteiro para a lista.
   Segundo parâmetro é a informação a ser inserida.
*/

void inserirFim(Lista l, void* info);
/*
   Função para inserir uma informação no final da lista.
   Primeiro parâmetro é o ponteiro para a lista.
   Segundo parâmetro é a informação a ser inserida.
*/

void removerNo(Lista l, No n);
/*
   Função para remover um nó da lista.
   Primeiro parâmetro é o ponteiro para a lista.
   Segundo parâmetro é o ponteiro do nó a ser removido.
   Não deleta a informação armazenada.
*/

int tamanhoLista(Lista l);
/*
   Função que calcula a quantidade de elementos da lista.
   Primeiro parâmetro é o ponteiro para a lista.
   Retorna o tamanho da lista.
*/

bool listaVazia(Lista l);
/*
   Função para verificar se a lista está vazia ou não.
   Primeiro parâmetro é o ponteiro para a lista.
   Retorna 1 se a lista estiver vazia, 0 caso contrário.
*/

No primeiroNo(Lista l);
/*
   Função para retornar o primeiro nó da lista.
   Primeiro parâmetro é o ponteiro para a lista.
   Retorna um ponteiro para o primeiro nó da lista.
*/

No ultimoNo(Lista l);
/*
   Função para retornar o último nó da lista.
   Primeiro parâmetro é o ponteiro para a lista.
   Retorna um ponteiro para o último nó da lista.
*/

No proximoNo(No n);
/*
   Função para pegar o próximo nó na lista
   Primeiro parâmetro é o ponteiro para o nó atual.
   Retorna um ponteiro pro próximo nó da lista.
*/

No anteriorNo(No n);
/*
   Função para pegar o nó anterior da lista
   Primeiro parâmetro é o ponteiro para o nó atual.
   Retorna um ponteiro pro nó anterior da lista.
*/

void* getInfo(No n);
/*
   Função para obter a informação armazenada de um nó.
   Primeiro parâmetro é o ponteiro para o determinado nó.
   Retorna a informação.
*/

void setInfo(No n, void* info);
/*
   Função para alterar a informação armazenada no nó.
   Primeiro parâmetro é o ponteiro para o nó.
   Segundo parâmetro é as informações a serem inseridas.
*/

void deletaLista(Lista l);
/*
   Deleta a estrutura da lista.
   Não deleta os dados armazenados nela.
   Primeiro parâmetro é o ponteiro para a lista.
*/

#endif
