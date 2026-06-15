#include "trataNomeArquivo.h"
#include <sys/stat.h>
#include <string.h>

#ifdef _WIN32
#include <direct.h>
#define mkdir _mkdir
#endif

void criarDiretorioRecursivo(const char* path){
    if (!path || strlen(path) == 0) return;
    
    char tmp[PATH_LEN];
    char *p = NULL;
    size_t len;

    snprintf(tmp, sizeof(tmp), "%s", path);
    len = strlen(tmp);
    
    if (len > 0 && tmp[len - 1] == '/'){
        tmp[len - 1] = 0;
    }

    for (p = tmp + 1; *p; p++){
        if (*p == '/'){
            *p = 0;
            #ifdef _WIN32
                _mkdir(tmp);
            #else
                mkdir(tmp, 0700);
            #endif
            *p = '/';
        }
    }

    #ifdef _WIN32
        _mkdir(tmp);
    #else
        mkdir(tmp, 0700);
    #endif
}

void extrairNomeBase(const char* caminhoCompleto, char* nomeBase){
    if (!caminhoCompleto || !nomeBase) return;
    
    const char* ultimaBarra = strrchr(caminhoCompleto, '/');
    const char* inicioNome = (ultimaBarra != NULL) ? ultimaBarra + 1 : caminhoCompleto;
    
    strcpy(nomeBase, inicioNome);
    
    char* ponto = strrchr(nomeBase, '.');
    if (ponto != NULL){
        *ponto = '\0';
    }
}

void gerarNomeGeoSvg(const char* nomeBase, const char* outputDir, char* caminhoCompleto){
    if (!nomeBase || !outputDir || !caminhoCompleto) return;
    
    criarDiretorioRecursivo(outputDir);
    
    if (strlen(outputDir) > 0){
        snprintf(caminhoCompleto, PATH_LEN, "%s/%s.svg", outputDir, nomeBase);
    } else{
        snprintf(caminhoCompleto, PATH_LEN, "%s.svg", nomeBase);
    }
}

void gerarNomeTxt(const char* nomeBase, const char* outputDir, char* caminhoCompleto){
    if (!nomeBase || !outputDir || !caminhoCompleto) return;
    
    criarDiretorioRecursivo(outputDir);
    
    if (strlen(outputDir) > 0){
        snprintf(caminhoCompleto, PATH_LEN, "%s/%s.txt", outputDir, nomeBase);
    } else{
        snprintf(caminhoCompleto, PATH_LEN, "%s.txt", nomeBase);
    }
}

void gerarNomeQrySvg(const char* nomeBaseGeo, const char* nomeBaseQry, const char* outputDir, char* caminhoCompleto){
    if (!nomeBaseGeo || !nomeBaseQry || !outputDir || !caminhoCompleto) return;
    
    criarDiretorioRecursivo(outputDir);
    
    if (strlen(outputDir) > 0){
        snprintf(caminhoCompleto, PATH_LEN, "%s/%s-%s.svg", outputDir, nomeBaseGeo, nomeBaseQry);
    } else{
        snprintf(caminhoCompleto, PATH_LEN, "%s-%s.svg", nomeBaseGeo, nomeBaseQry);
    }
}

void gerarNomeQryTxt(const char* nomeBaseGeo, const char* nomeBaseQry, const char* outputDir, char* caminhoCompleto){
    if (!nomeBaseGeo || !nomeBaseQry || !outputDir || !caminhoCompleto) return;
    
    criarDiretorioRecursivo(outputDir);
    
    if (strlen(outputDir) > 0){
        snprintf(caminhoCompleto, PATH_LEN, "%s/%s-%s.txt", outputDir, nomeBaseGeo, nomeBaseQry);
    } else{
        snprintf(caminhoCompleto, PATH_LEN, "%s-%s.txt", nomeBaseGeo, nomeBaseQry);
    }
}

int criarDiretorioSeNecessario(const char* caminho){
    if (!caminho) return 0;
    criarDiretorioRecursivo(caminho);
    return 1;
}
