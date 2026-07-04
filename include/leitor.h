#ifndef LEITOR_H
#define LEITOR_H

#include <stdio.h>
#include <stdbool.h>

/*
   O leitor é responsável por processar arquivos de comando (.geo, .via e .qry) que definem as operações do projeto.
*/

void inicializarSistema(const char* nomeBase, const char* outputDir);
/*
   Função que inicializa o sistema, criando as estruturas de dados.
   Primeiro parâmetro é o nome base dos arquivos (sem extensão).
   Segundo parâmetro é o diretório de saída.
*/

void finalizarSistema(void);
/*
   Função que finaliza o sistema, liberando toda a memória alocada.
   Persiste os dados em disco e fecha os arquivos abertos.
*/

void processarArquivoGeo(const char* caminho, const char* inputDir,
                         const char* nomeBase, const char* outputDir);
/*
   Função que processa o arquivo .geo.
   Primeiro parâmetro é o nome do arquivo .geo.
   Segundo parâmetro é o diretório de entrada.
   Terceiro parâmetro é o nome base dos arquivos.
   Quarto parâmetro é o diretório de saída.
*/

void processarArquivoVia(const char* caminho, const char* inputDir);
/*
   Função que processa o arquivo .via.
   Primeiro parâmetro é o nome do arquivo .via.
   Segundo parâmetro é o diretório de entrada.
   Constrói o grafo viário a partir do arquivo.
*/

void processarArquivoQry(const char* caminho, const char* inputDir,
                         const char* nomeBase, const char* outputDir);
/*
   Função que processa o arquivo .qry.
   Primeiro parâmetro é o nome do arquivo .qry.
   Segundo parâmetro é o diretório de entrada.
   Terceiro parâmetro é o nome base dos arquivos.
   Quarto parâmetro é o diretório de saída.
   Gera os arquivos .svg e .txt de saída.
*/

#endif
