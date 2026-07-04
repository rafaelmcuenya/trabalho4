#ifndef HASHFILE_H
#define HASHFILE_H

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

/*
   Este TAD contém a documentação do HashFile extensível. 
   Um HashFile extensível é um método de armazenamento em dispositivos de memória secundários, tendo como exemplo mais notável, o disco rígido.
   Permite gerenciar de forma dinâmica o espalhamento a depender se necessita diminuir/aumentar o espaço reservado, de forma automática.

   Por meio da função criarHF, cria-se um HashFile, com as especificações descritas em tal função.
   Pode ser encerrado com a função freeHF.
   Possui demais operações importantes (inserir, procurar item, deletar item, etc).
*/

typedef size_t sizeT;

typedef struct HashFileStruct HashFile;

HashFile* criarHF(const char *nomeBase, int tamBucket, int profInicial);
/*
Função para criar ou abrir um HashFile existente.
Primeiro parâmetro é o nome base para os arquivos (sem extensão).
Segundo parâmetro é o número máximo de elementos por bucket.
Terceiro parâmetro é a profundidade global inicial.
Retorna um ponteiro para o HashFile, ou NULL em caso de erro.
*/

HashFile* abrirHF(const char *nomeBase);
/*
Função para abrur um arquivo HashFile baseado no nome repassado pelo parâmetro.
Retorna um ponteiro para o HashFile (?)
*/

void freeHF(HashFile *hf);
/*
Função para fechar o HashFile e liberar toda a memória alocada.
Persiste todas as alterações pendentes em disco.
Primeiro parâmetro é o ponteiro para o HashFile.
*/

int insertHF(HashFile *hf, const char *chave, const void *dado, sizeT tamDado);
/*
Função para inserir um registro no HashFile.
Primeiro parâmetro é o ponteiro para o HashFile.
Segundo parâmetro é a chave alfanumérica (string) para indexação.
Terceiro parâmetro é um ponteiro para os dados a serem armazenados.
Quarto parâmetro é o tamanho em bytes do dado.
Retorna 1 caso a operação tenha sido bem sucedida, 0 se a chave já existe, -1 em caso de erro.
*/

int buscaHF(HashFile *hf, const char *chave, void *dadoSaida, sizeT *tamSaida);
/*
Função para procurar um registro específico por meio de sua chave.
Primeiro parâmetro é o ponteiro para o HashFile.
Segundo parâmetro é a chave a ser buscada.
Terceiro parâmetro é um ponteiro para o buffer onde os dados serão copiados (pode ser NULL).
Quarto parâmetro é o tamanho do buffer (entrada) / tamanho real do dado (saída).
Retorna 1 caso encontre, 0 caso contrário, -1 em caso de erro.
*/

int deletarItemHF(HashFile *hf, const char *chave);
/*
Função para remover um registro por meio da chave.
Primeiro parâmetro é o ponteiro para o HashFile.
Segundo parâmetro é a chave a ser removida.
Retorna 1 caso sucedido, 0 se não encontrado, -1 em caso de erro.
*/

int refreshHF(HashFile *hf, const char *chave, const void *novoDado, sizeT tamDado);
/*
Função para atualizar um registro existente.
Primeiro parâmetro é o ponteiro para o HashFile.
Segundo parâmetro é a chave do registro a ser atualizado.
Terceiro parâmetro é um ponteiro para os novos dados.
Quarto parâmetro é o tamanho em bytes dos novos dados.
Retorna 1 caso a operação tenha sido bem sucedida, 0 se não encontrado, -1 em caso de erro.
*/

int gerarDumpHF(HashFile *hf);
/*
Função para gerar o arquivo de dump legível (.hfd) mostrando a estrutura do HashFile.
Registra as expansões dos buckets e o estado atual do diretório.
Primeiro parâmetro é o ponteiro para o HashFile.
Retorna 1 caso tenha sido gerado com sucesso, 0 em caso de erro.
*/

sizeT totalRegistrosHF(const HashFile *hf);
/*
Função para obter o número total de registros armazenados no HashFile.
Primeiro parâmetro é o ponteiro para o HashFile.
Retorna o total de registros, ou 0 em caso de erro.
*/

int getProfGlobalHF(const HashFile *hf);
/*
Função para obter a profundidade global atual do diretório.
Primeiro parâmetro é o ponteiro para o HashFile.
Retorna a profundidade global, ou -1 em caso de erro.
*/

int getTamBucketHF(const HashFile *hf);
/*
Função para obter a capacidade máxima de registros por bucket.
Primeiro parâmetro é o ponteiro para o HashFile.
Retorna a capacidade do bucket, ou -1 em caso de erro.
*/

int getInfoBucketHF(const HashFile *hf, int indiceBucket, int *qtdRegistros, int *profLocal);
/*
Função para obter informações de um bucket específico.
Primeiro parâmetro é o ponteiro para o HashFile.
Segundo parâmetro é o índice do bucket no diretório.
Terceiro parâmetro é um ponteiro para int onde a quantidade de registros será armazenada.
Quarto parâmetro é um ponteiro para int onde a profundidade local será armazenada.
Retorna 1 caso sucesso, 0 se índice inválido, -1 em caso de erro.
*/

typedef void (*hf_Callback)(const char *chave, const void *dado, sizeT tamDado, void *contexto);
/*
Tipo para função de callback usada na iteração sobre os registros do HashFile.
Primeiro parâmetro é a chave do registro.
Segundo parâmetro é um ponteiro para os dados do registro.
Terceiro parâmetro é o tamanho em bytes dos dados.
Quarto parâmetro é um ponteiro para dados de contexto fornecidos pelo usuário.
*/

int iterarHF(HashFile *hf, hf_Callback callback, void *contexto);
/*
Função para iterar sobre todos os registros do HashFile.
Primeiro parâmetro é o ponteiro para o HashFile.
Segundo parâmetro é a função de callback que será chamada para cada registro.
Terceiro parâmetro é um ponteiro para dados de contexto (pode ser NULL).
Retorna o número de registros processados, ou -1 em caso de erro.
*/

#endif
