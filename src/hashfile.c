#include "hashfile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM_CHAVE_MAX 64
#define TAM_NOMEBASE_MAX 256

typedef size_t sizeT;

typedef struct {
    char chave[TAM_CHAVE_MAX];
    uint32_t offset_dados;
    uint32_t tamDados;
    uint8_t ocupado;
} RegistroBucket;

typedef struct BucketStruct {
    int profLocal;
    int numRegistros;
    RegistroBucket *registros;
    int modificado;
    long offset_arquivo;
} Bucket;

struct HashFileStruct {
    FILE *arqBuckets;
    FILE *arqDados;
    FILE *arqCabecalho;
    char nomeBase[TAM_NOMEBASE_MAX];
    int profGlobal;
    int tamBucket;
    Bucket **diretorio;
    sizeT tamDiretorio;
    int modificado;
    long offset_proximo_bucket;
    long *offsets_buckets;
    int num_buckets;
    int cap_buckets;
};

static uint32_t hashString(const char *str) {
    uint32_t hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash;
}

static int getBitsHash(uint32_t hash, int prof) {
    if (prof <= 0) return 0;
    if (prof >= 32) return (int)hash;
    return (int)(hash & ((1u << prof) - 1));
}

static Bucket* criarBucket(int nivel, int tamBucket) {
    Bucket *b = (Bucket*)malloc(sizeof(Bucket));
    if (!b) return NULL;
    b->profLocal = nivel;
    b->numRegistros = 0;
    b->registros = (RegistroBucket*)calloc(tamBucket, sizeof(RegistroBucket));
    if (!b->registros) {
        free(b);
        return NULL;
    }
    b->modificado = 1;
    b->offset_arquivo = -1;
    return b;
}

static void freeBucket(Bucket *b) {
    if (b) {
        free(b->registros);
        free(b);
    }
}

static int saveCabecalho(HashFile *hf) {
    if (!hf->arqCabecalho) return 0;
    rewind(hf->arqCabecalho);
    fwrite(&hf->profGlobal, sizeof(int), 1, hf->arqCabecalho);
    fwrite(&hf->tamBucket, sizeof(int), 1, hf->arqCabecalho);
    fwrite(&hf->tamDiretorio, sizeof(sizeT), 1, hf->arqCabecalho);
    fwrite(&hf->offset_proximo_bucket, sizeof(long), 1, hf->arqCabecalho);
    fwrite(&hf->num_buckets, sizeof(int), 1, hf->arqCabecalho);
    fwrite(hf->offsets_buckets, sizeof(long), hf->num_buckets, hf->arqCabecalho);
    fflush(hf->arqCabecalho);
    return 1;
}

static int loadCabecalho(HashFile *hf) {
    if (!hf->arqCabecalho) return 0;
    rewind(hf->arqCabecalho);
    if (fread(&hf->profGlobal, sizeof(int), 1, hf->arqCabecalho) != 1) return 0;
    if (fread(&hf->tamBucket, sizeof(int), 1, hf->arqCabecalho) != 1) return 0;
    if (fread(&hf->tamDiretorio, sizeof(sizeT), 1, hf->arqCabecalho) != 1) return 0;
    if (fread(&hf->offset_proximo_bucket, sizeof(long), 1, hf->arqCabecalho) != 1) return 0;
    if (fread(&hf->num_buckets, sizeof(int), 1, hf->arqCabecalho) != 1) return 0;
    return 1;
}

static int adicionarOffsetBucket(HashFile *hf, long offset) {
    for (int i = 0; i < hf->num_buckets; i++) {
        if (hf->offsets_buckets[i] == offset) return 1;
    }
    
    if (hf->num_buckets >= hf->cap_buckets) {
        int nova_cap = hf->cap_buckets * 2;
        if (nova_cap == 0) nova_cap = 4;
        long *novo = (long*)realloc(hf->offsets_buckets, nova_cap * sizeof(long));
        if (!novo) return 0;
        hf->offsets_buckets = novo;
        hf->cap_buckets = nova_cap;
    }
    
    hf->offsets_buckets[hf->num_buckets++] = offset;
    return 1;
}

static int saveBucket(HashFile *hf, Bucket *b) {
    if (!b->modificado) return 1;
    if (b->offset_arquivo == -1) {
        b->offset_arquivo = hf->offset_proximo_bucket;
        hf->offset_proximo_bucket += sizeof(int) * 2 + (hf->tamBucket * sizeof(RegistroBucket));
        adicionarOffsetBucket(hf, b->offset_arquivo);
    }
    
    fseek(hf->arqBuckets, b->offset_arquivo, SEEK_SET);
    fwrite(&b->profLocal, sizeof(int), 1, hf->arqBuckets);
    fwrite(&b->numRegistros, sizeof(int), 1, hf->arqBuckets);
    fwrite(b->registros, sizeof(RegistroBucket), hf->tamBucket, hf->arqBuckets);
    fflush(hf->arqBuckets);
    
    b->modificado = 0;
    return 1;
}

static Bucket* loadBucket(HashFile *hf, long offset) {
    if (offset < 0) return NULL;
    
    Bucket *b = (Bucket*)malloc(sizeof(Bucket));
    if (!b) return NULL;
    
    b->registros = (RegistroBucket*)calloc(hf->tamBucket, sizeof(RegistroBucket));
    if (!b->registros) {
        free(b);
        return NULL;
    }
    
    fseek(hf->arqBuckets, offset, SEEK_SET);
    if (fread(&b->profLocal, sizeof(int), 1, hf->arqBuckets) != 1) {
        free(b->registros);
        free(b);
        return NULL;
    }
    if (fread(&b->numRegistros, sizeof(int), 1, hf->arqBuckets) != 1) {
        free(b->registros);
        free(b);
        return NULL;
    }
    if (fread(b->registros, sizeof(RegistroBucket), hf->tamBucket, hf->arqBuckets) != (size_t)hf->tamBucket) {
        free(b->registros);
        free(b);
        return NULL;
    }
    
    b->modificado = 0;
    b->offset_arquivo = offset;
    return b;
}

static int dobraDiretorio(HashFile *hf) {
    sizeT newTam = hf->tamDiretorio * 2;
    Bucket **newDir = (Bucket**)malloc(newTam * sizeof(Bucket*));
    if (!newDir) return 0;
    
    for (sizeT i = 0; i < hf->tamDiretorio; i++) {
        newDir[i] = hf->diretorio[i];
        newDir[i + hf->tamDiretorio] = hf->diretorio[i];
    }
    
    free(hf->diretorio);
    hf->diretorio = newDir;
    hf->tamDiretorio = newTam;
    hf->profGlobal++;
    hf->modificado = 1;
    saveCabecalho(hf);
    return 1;
}

static int bucketCheio(HashFile *hf, int indice) {
    if (indice < 0 || indice >= (int)hf->tamDiretorio) return -1;
    Bucket *b = hf->diretorio[indice];
    if (!b) return -1;
    return b->numRegistros >= hf->tamBucket;
}

static int splitBucket(HashFile *hf, int indice) {
    Bucket *oldB = hf->diretorio[indice];
    if (!oldB) return 0;
    
    int newProf = oldB->profLocal + 1;
    
    while (newProf > hf->profGlobal) {
        if (!dobraDiretorio(hf)) return 0;
    }
    
    Bucket *novoB0 = criarBucket(newProf, hf->tamBucket);
    Bucket *novoB1 = criarBucket(newProf, hf->tamBucket);
    if (!novoB0 || !novoB1) {
        if (novoB0) freeBucket(novoB0);
        if (novoB1) freeBucket(novoB1);
        return 0;
    }
    
    for (int i = 0; i < oldB->numRegistros; i++) {
        RegistroBucket *reg = &oldB->registros[i];
        if (!reg->ocupado) continue;
        
        uint32_t h = hashString(reg->chave);
        int bit_extra = (h >> (newProf - 1)) & 1;
        
        Bucket *destino = bit_extra ? novoB1 : novoB0;
        int pos = destino->numRegistros;
        if (pos < hf->tamBucket) {
            memcpy(&destino->registros[pos], reg, sizeof(RegistroBucket));
            destino->numRegistros++;
        }
    }
    
    saveBucket(hf, novoB0);
    saveBucket(hf, novoB1);
    
    for (sizeT i = 0; i < hf->tamDiretorio; i++) {
        if (hf->diretorio[i] == oldB) {
            int bit = (int)((i >> (newProf - 1)) & 1);
            hf->diretorio[i] = bit ? novoB1 : novoB0;
        }
    }
    
    freeBucket(oldB);
    hf->modificado = 1;
    return 1;
}

static int searchRegistro(Bucket *b, const char *chave) {
    for (int i = 0; i < b->numRegistros; i++) {
        if (b->registros[i].ocupado && strcmp(b->registros[i].chave, chave) == 0) {
            return i;
        }
    }
    return -1;
}

static int searchPosLivre(Bucket *b, int tamBucket) {
    for (int i = 0; i < b->numRegistros; i++) {
        if (!b->registros[i].ocupado) {
            return i;
        }
    }
    if (b->numRegistros < tamBucket) {
        return b->numRegistros;
    }
    return -1;
}

HashFile* criarHF(const char *nomeBase, int tamBucket, int profInicial) {
    if (!nomeBase || tamBucket <= 0 || profInicial <= 0) return NULL;
    
    HashFile *hf = (HashFile*)malloc(sizeof(HashFile));
    if (!hf) return NULL;
    
    memset(hf, 0, sizeof(HashFile));
    strncpy(hf->nomeBase, nomeBase, TAM_NOMEBASE_MAX - 1);
    
    char nome_arq[TAM_NOMEBASE_MAX + 10];
    
    snprintf(nome_arq, sizeof(nome_arq), "%s.hfb", nomeBase);
    hf->arqBuckets = fopen(nome_arq, "w+b");
    
    snprintf(nome_arq, sizeof(nome_arq), "%s.hfd", nomeBase);
    hf->arqDados = fopen(nome_arq, "w+b");
    
    snprintf(nome_arq, sizeof(nome_arq), "%s.hfc", nomeBase);
    hf->arqCabecalho = fopen(nome_arq, "w+b");
    
    if (!hf->arqBuckets || !hf->arqDados || !hf->arqCabecalho) {
        if (hf->arqBuckets) fclose(hf->arqBuckets);
        if (hf->arqDados) fclose(hf->arqDados);
        if (hf->arqCabecalho) fclose(hf->arqCabecalho);
        free(hf);
        return NULL;
    }
    
    hf->profGlobal = profInicial;
    hf->tamBucket = tamBucket;
    hf->tamDiretorio = 1u << profInicial;
    hf->offset_proximo_bucket = 0;
    hf->num_buckets = 0;
    hf->cap_buckets = 4;
    hf->offsets_buckets = (long*)malloc(hf->cap_buckets * sizeof(long));
    if (!hf->offsets_buckets) {
        fclose(hf->arqBuckets);
        fclose(hf->arqDados);
        fclose(hf->arqCabecalho);
        free(hf);
        return NULL;
    }
    
    hf->diretorio = (Bucket**)malloc(hf->tamDiretorio * sizeof(Bucket*));
    if (!hf->diretorio) {
        free(hf->offsets_buckets);
        fclose(hf->arqBuckets);
        fclose(hf->arqDados);
        fclose(hf->arqCabecalho);
        free(hf);
        return NULL;
    }
    
    Bucket *b_inicial = criarBucket(profInicial, tamBucket);
    if (!b_inicial) {
        free(hf->diretorio);
        free(hf->offsets_buckets);
        fclose(hf->arqBuckets);
        fclose(hf->arqDados);
        fclose(hf->arqCabecalho);
        free(hf);
        return NULL;
    }
    
    saveBucket(hf, b_inicial);
    
    for (sizeT i = 0; i < hf->tamDiretorio; i++) {
        hf->diretorio[i] = b_inicial;
    }
    
    hf->modificado = 1;
    saveCabecalho(hf);
    return hf;
}

HashFile* abrirHF(const char *nomeBase) {
    if (!nomeBase) return NULL;
    
    HashFile *hf = (HashFile*)malloc(sizeof(HashFile));
    if (!hf) return NULL;
    
    memset(hf, 0, sizeof(HashFile));
    strncpy(hf->nomeBase, nomeBase, TAM_NOMEBASE_MAX - 1);
    
    char nome_arq[TAM_NOMEBASE_MAX + 10];
    
    snprintf(nome_arq, sizeof(nome_arq), "%s.hfb", nomeBase);
    hf->arqBuckets = fopen(nome_arq, "r+b");
    
    snprintf(nome_arq, sizeof(nome_arq), "%s.hfd", nomeBase);
    hf->arqDados = fopen(nome_arq, "r+b");
    
    snprintf(nome_arq, sizeof(nome_arq), "%s.hfc", nomeBase);
    hf->arqCabecalho = fopen(nome_arq, "r+b");
    
    if (!hf->arqBuckets || !hf->arqDados || !hf->arqCabecalho) {
        if (hf->arqBuckets) fclose(hf->arqBuckets);
        if (hf->arqDados) fclose(hf->arqDados);
        if (hf->arqCabecalho) fclose(hf->arqCabecalho);
        free(hf);
        return NULL;
    }
    
    if (!loadCabecalho(hf)) {
        fclose(hf->arqBuckets);
        fclose(hf->arqDados);
        fclose(hf->arqCabecalho);
        free(hf);
        return NULL;
    }
    
    hf->cap_buckets = hf->num_buckets > 0 ? hf->num_buckets : 4;
    hf->offsets_buckets = (long*)malloc(hf->cap_buckets * sizeof(long));
    if (!hf->offsets_buckets) {
        fclose(hf->arqBuckets);
        fclose(hf->arqDados);
        fclose(hf->arqCabecalho);
        free(hf);
        return NULL;
    }
    
    if (hf->num_buckets > 0) {
        fseek(hf->arqCabecalho, sizeof(int) * 2 + sizeof(sizeT) + sizeof(long) + sizeof(int), SEEK_SET);
        fread(hf->offsets_buckets, sizeof(long), hf->num_buckets, hf->arqCabecalho);
    }
    
    hf->diretorio = (Bucket**)malloc(hf->tamDiretorio * sizeof(Bucket*));
    if (!hf->diretorio) {
        free(hf->offsets_buckets);
        fclose(hf->arqBuckets);
        fclose(hf->arqDados);
        fclose(hf->arqCabecalho);
        free(hf);
        return NULL;
    }
    
    Bucket **buckets_carregados = (Bucket**)malloc(hf->num_buckets * sizeof(Bucket*));
    if (!buckets_carregados) {
        free(hf->diretorio);
        free(hf->offsets_buckets);
        fclose(hf->arqBuckets);
        fclose(hf->arqDados);
        fclose(hf->arqCabecalho);
        free(hf);
        return NULL;
    }
    
    for (int i = 0; i < hf->num_buckets; i++) {
        buckets_carregados[i] = loadBucket(hf, hf->offsets_buckets[i]);
    }
    
    for (sizeT i = 0; i < hf->tamDiretorio; i++) {
        int encontrado = 0;
        for (int j = 0; j < hf->num_buckets; j++) {
            if (buckets_carregados[j] && buckets_carregados[j]->numRegistros > 0) {
                for (int k = 0; k < buckets_carregados[j]->numRegistros; k++) {
                    if (buckets_carregados[j]->registros[k].ocupado) {
                        uint32_t h = hashString(buckets_carregados[j]->registros[k].chave);
                        int idx_registro = getBitsHash(h, hf->profGlobal);
                        if (idx_registro == (int)i) {
                            hf->diretorio[i] = buckets_carregados[j];
                            encontrado = 1;
                            break;
                        }
                    }
                }
                if (encontrado) break;
            }
        }
        if (!encontrado) {
            hf->diretorio[i] = buckets_carregados[0];
        }
    }
    
    free(buckets_carregados);
    hf->modificado = 0;
    return hf;
}

void freeHF(HashFile *hf) {
    if (!hf) return;
    
    if (hf->diretorio) {
        Bucket **buckets_liberados = (Bucket**)malloc(hf->tamDiretorio * sizeof(Bucket*));
        int num_liberados = 0;
        
        if (buckets_liberados) {
            for (sizeT i = 0; i < hf->tamDiretorio; i++) {
                if (hf->diretorio[i]) {
                    int ja_liberado = 0;
                    for (int j = 0; j < num_liberados; j++) {
                        if (buckets_liberados[j] == hf->diretorio[i]) {
                            ja_liberado = 1;
                            break;
                        }
                    }
                    if (!ja_liberado) {
                        saveBucket(hf, hf->diretorio[i]);
                        buckets_liberados[num_liberados++] = hf->diretorio[i];
                    }
                }
            }
            
            for (int i = 0; i < num_liberados; i++) {
                freeBucket(buckets_liberados[i]);
            }
            free(buckets_liberados);
        }
        free(hf->diretorio);
    }
    
    if (hf->modificado) {
        if (hf->arqCabecalho) {
            fseek(hf->arqCabecalho, sizeof(int) * 2 + sizeof(sizeT) + sizeof(long) + sizeof(int), SEEK_SET);
            fwrite(hf->offsets_buckets, sizeof(long), hf->num_buckets, hf->arqCabecalho);
        }
        saveCabecalho(hf);
    }
    
    if (hf->offsets_buckets) free(hf->offsets_buckets);
    if (hf->arqBuckets) fclose(hf->arqBuckets);
    if (hf->arqDados) fclose(hf->arqDados);
    if (hf->arqCabecalho) fclose(hf->arqCabecalho);
    free(hf);
}

int insertHF(HashFile *hf, const char *chave, const void *dado, sizeT tamDado) {
    if (!hf || !chave || !dado || tamDado == 0) return -1;
    
    uint32_t h = hashString(chave);
    int indice = getBitsHash(h, hf->profGlobal);
    
    Bucket *b = hf->diretorio[indice];
    if (!b) return -1;
    
    if (searchRegistro(b, chave) != -1) {
        return 0;
    }
    
    int tentativas = 0;
    while (bucketCheio(hf, indice) && tentativas < 10) {
        if (!splitBucket(hf, indice)) return -1;
        h = hashString(chave);
        indice = getBitsHash(h, hf->profGlobal);
        b = hf->diretorio[indice];
        tentativas++;
    }
    
    int pos = searchPosLivre(b, hf->tamBucket);
    if (pos == -1) return -1;
    
    strncpy(b->registros[pos].chave, chave, TAM_CHAVE_MAX - 1);
    b->registros[pos].tamDados = tamDado;
    b->registros[pos].ocupado = 1;
    
    fseek(hf->arqDados, 0, SEEK_END);
    b->registros[pos].offset_dados = ftell(hf->arqDados);
    fwrite(dado, 1, tamDado, hf->arqDados);
    fflush(hf->arqDados);
    
    if (pos == b->numRegistros) {
        b->numRegistros++;
    }
    
    b->modificado = 1;
    hf->modificado = 1;
    saveBucket(hf, b);
    return 1;
}

int buscaHF(HashFile *hf, const char *chave, void *dadoSaida, sizeT *tamSaida) {
    if (!hf || !chave) return -1;
    
    uint32_t h = hashString(chave);
    int indice = getBitsHash(h, hf->profGlobal);
    
    Bucket *b = hf->diretorio[indice];
    if (!b) return -1;
    
    int pos = searchRegistro(b, chave);
    if (pos == -1) return 0;
    
    if (dadoSaida && tamSaida) {
        if (*tamSaida < b->registros[pos].tamDados) {
            return -1;
        }
        
        fseek(hf->arqDados, b->registros[pos].offset_dados, SEEK_SET);
        fread(dadoSaida, 1, b->registros[pos].tamDados, hf->arqDados);
        *tamSaida = b->registros[pos].tamDados;
    }
    return 1;
}

int deletarItemHF(HashFile *hf, const char *chave) {
    if (!hf || !chave) return -1;
    
    uint32_t h = hashString(chave);
    int indice = getBitsHash(h, hf->profGlobal);
    
    Bucket *b = hf->diretorio[indice];
    if (!b) return -1;
    
    int pos = searchRegistro(b, chave);
    if (pos == -1) return 0;
    
    b->registros[pos].ocupado = 0;
    
    if (pos < b->numRegistros - 1 && b->registros[b->numRegistros - 1].ocupado) {
        memcpy(&b->registros[pos], &b->registros[b->numRegistros - 1], sizeof(RegistroBucket));
        b->registros[b->numRegistros - 1].ocupado = 0;
    }
    
    if (b->numRegistros > 0) {
        b->numRegistros--;
    }
    
    b->modificado = 1;
    hf->modificado = 1;
    saveBucket(hf, b);
    return 1;
}

int refreshHF(HashFile *hf, const char *chave, const void *novoDado, sizeT tamDado) {
    if (!hf || !chave || !novoDado || tamDado == 0) return -1;
    
    uint32_t h = hashString(chave);
    int indice = getBitsHash(h, hf->profGlobal);
    
    Bucket *b = hf->diretorio[indice];
    if (!b) return -1;
    
    int pos = searchRegistro(b, chave);
    if (pos == -1) return 0;
    
    fseek(hf->arqDados, b->registros[pos].offset_dados, SEEK_SET);
    fwrite(novoDado, 1, tamDado, hf->arqDados);
    fflush(hf->arqDados);
    
    b->registros[pos].tamDados = tamDado;
    b->modificado = 1;
    hf->modificado = 1;
    saveBucket(hf, b);
    return 1;
}

int gerarDumpHF(HashFile *hf) {
    if (!hf) return 0;
    
    char nome_dump[TAM_NOMEBASE_MAX + 10];
    snprintf(nome_dump, sizeof(nome_dump), "%s.hfd", hf->nomeBase);
    
    FILE *dump = fopen(nome_dump, "w");
    if (!dump) return 0;
    
    fprintf(dump, "=== DUMP DO HASHFILE ===\n");
    fprintf(dump, "Arquivo base: %s\n", hf->nomeBase);
    fprintf(dump, "Profundidade global: %d\n", hf->profGlobal);
    fprintf(dump, "Capacidade do bucket: %d\n", hf->tamBucket);
    fprintf(dump, "Tamanho do diretorio: %zu\n", hf->tamDiretorio);
    fprintf(dump, "Total de registros: %zu\n", totalRegistrosHF(hf));
    fprintf(dump, "\n");
    
    Bucket **buckets_impressos = (Bucket**)malloc(hf->tamDiretorio * sizeof(Bucket*));
    if (!buckets_impressos) {
        fclose(dump);
        return 0;
    }
    int num_impressos = 0;
    
    for (sizeT i = 0; i < hf->tamDiretorio; i++) {
        Bucket *b = hf->diretorio[i];
        
        int ja_impresso = 0;
        for (int j = 0; j < num_impressos; j++) {
            if (buckets_impressos[j] == b) {
                ja_impresso = 1;
                break;
            }
        }
        
        if (!ja_impresso) {
            buckets_impressos[num_impressos++] = b;
            fprintf(dump, "Bucket (profLocal=%d, registros=%d, offset=%ld):\n", 
                    b->profLocal, b->numRegistros, b->offset_arquivo);
            
            for (int j = 0; j < b->numRegistros; j++) {
                if (b->registros[j].ocupado) {
                    fprintf(dump, "  [%d] chave='%s', tam=%u, offset_dados=%u\n", 
                            j, b->registros[j].chave, b->registros[j].tamDados, b->registros[j].offset_dados);
                }
            }
            
            fprintf(dump, "  Apontado por indices: ");
            for (sizeT j = 0; j < hf->tamDiretorio; j++) {
                if (hf->diretorio[j] == b) {
                    fprintf(dump, "%zu ", j);
                }
            }
            fprintf(dump, "\n\n");
        }
    }
    
    free(buckets_impressos);
    fclose(dump);
    return 1;
}

sizeT totalRegistrosHF(const HashFile *hf) {
    if (!hf) return 0;
    
    sizeT total = 0;
    Bucket **buckets_contados = (Bucket**)malloc(hf->tamDiretorio * sizeof(Bucket*));
    if (!buckets_contados) return 0;
    int num_contados = 0;
    
    for (sizeT i = 0; i < hf->tamDiretorio; i++) {
        Bucket *b = hf->diretorio[i];
        
        int ja_contado = 0;
        for (int j = 0; j < num_contados; j++) {
            if (buckets_contados[j] == b) {
                ja_contado = 1;
                break;
            }
        }
        
        if (!ja_contado && b) {
            buckets_contados[num_contados++] = b;
            for (int j = 0; j < b->numRegistros; j++) {
                if (b->registros[j].ocupado) {
                    total++;
                }
            }
        }
    }
    free(buckets_contados);
    return total;
}

int getProfGlobalHF(const HashFile *hf) {
    if (!hf) return -1;
    return hf->profGlobal;
}

int getTamBucketHF(const HashFile *hf) {
    if (!hf) return -1;
    return hf->tamBucket;
}

int getInfoBucketHF(const HashFile *hf, int indiceBucket, int *qtdRegistros, int *profLocal) {
    if (!hf || indiceBucket < 0 || indiceBucket >= (int)hf->tamDiretorio) return 0;
    
    Bucket *b = hf->diretorio[indiceBucket];
    if (!b) return 0;
    
    if (qtdRegistros) *qtdRegistros = b->numRegistros;
    if (profLocal) *profLocal = b->profLocal;
    return 1;
}

int iterarHF(HashFile *hf, hf_Callback callback, void *contexto) {
    if (!hf || !callback) return -1;
    
    int processados = 0;
    void *buffer = NULL;
    sizeT buffer_tam = 0;
    
    Bucket **buckets_processados = (Bucket**)malloc(hf->tamDiretorio * sizeof(Bucket*));
    if (!buckets_processados) return -1;
    int num_processados = 0;
    
    for (sizeT i = 0; i < hf->tamDiretorio; i++) {
        Bucket *b = hf->diretorio[i];
        
        int ja_processado = 0;
        for (int j = 0; j < num_processados; j++) {
            if (buckets_processados[j] == b) {
                ja_processado = 1;
                break;
            }
        }   
        if (!ja_processado && b) {
            buckets_processados[num_processados++] = b;
            
            for (int j = 0; j < b->numRegistros; j++) {
                if (b->registros[j].ocupado) {
                    if (b->registros[j].tamDados > buffer_tam) {
                        void *new_buffer = realloc(buffer, b->registros[j].tamDados);
                        if (!new_buffer) {
                            free(buffer);
                            free(buckets_processados);
                            return -1;
                        }
                        buffer = new_buffer;
                        buffer_tam = b->registros[j].tamDados;
                    }
                    fseek(hf->arqDados, b->registros[j].offset_dados, SEEK_SET);
                    fread(buffer, 1, b->registros[j].tamDados, hf->arqDados);
                    callback(b->registros[j].chave, buffer, b->registros[j].tamDados, contexto);
                    processados++;
                }
            }
        }
    }
    free(buffer);
    free(buckets_processados);
    return processados;
}
