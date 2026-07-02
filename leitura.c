/* ============================================================================
 * leitura.c - Leitura do arquivo binario de estacoes e construcao do grafo.
 * ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "leitura.h"
#include "grafo.h"

/* ===========================================================================
 * Indice codigo -> nome
 *
 * A spec pede que o nome referente a codProxEstacao / codEstacaoIntegra seja
 * obtido por meio de uma "pesquisa no arquivo de dados". Para isso montamos,
 * em memoria, um vetor de pares (codigo, nome) a partir dos registros nao
 * removidos, ordenado por codigo, permitindo busca binaria.
 * =========================================================================== */

typedef struct {
    int         cod;    /* codigo da estacao                     */
    const char *nome;   /* aponta para o nome no vetor de registros */
} ParCodNome;

/* Comparador por codigo, usado pelo qsort. */
static int cmpParCod(const void *a, const void *b) {
    int ca = ((const ParCodNome *)a)->cod;
    int cb = ((const ParCodNome *)b)->cod;
    return (ca > cb) - (ca < cb);
}

/* Resolve um codigo de estacao para o seu nome via busca binaria no indice.
 * Retorna NULL se o codigo for nulo (-1) ou nao existir no indice. */
static const char *resolveNome(const ParCodNome *idx, int n, int cod) {
    int ini, fim, meio;

    if (cod == CAMPO_NULO) return NULL;

    ini = 0;
    fim = n - 1;
    while (ini <= fim) {
        meio = ini + (fim - ini) / 2;
        if (idx[meio].cod == cod)      return idx[meio].nome;
        else if (idx[meio].cod < cod)  ini = meio + 1;
        else                           fim = meio - 1;
    }
    return NULL;
}

/* ===========================================================================
 * Parsing de registros
 * =========================================================================== */

/* Le um inteiro de 4 bytes (little-endian nativo) a partir de 'buf'. */
static int lerIntBuf(const unsigned char *buf) {
    int v;
    memcpy(&v, buf, sizeof(int));
    return v;
}

/* Le um campo string de tamanho variavel (tamanho inteiro seguido do
 * conteudo) a partir de 'buf', copiando ate 'capacidade'-1 caracteres para
 * 'destino' (sempre terminado em '\0'). Retorna quantos bytes o campo ocupou
 * no registro (4 + tamanho armazenado), para avancar o cursor de leitura. */
static int lerStringVar(const unsigned char *buf, char *destino, int capacidade) {
    int tam = lerIntBuf(buf);
    int copiar = tam;

    if (copiar < 0) copiar = 0;
    if (copiar > capacidade - 1) copiar = capacidade - 1;

    memcpy(destino, buf + 4, copiar);
    destino[copiar] = '\0';

    return 4 + (tam > 0 ? tam : 0);
}

/* Preenche 'r' com os campos do registro de 80 bytes contido em 'buf'. */
static void parseRegistro(const unsigned char *buf, RegistroEstacao *r) {
    int off = 0;

    r->removido          = (char)buf[off]; off += 1;
    r->encadeamento      = lerIntBuf(buf + off); off += 4;
    r->codEstacao        = lerIntBuf(buf + off); off += 4;
    r->codLinha          = lerIntBuf(buf + off); off += 4;
    r->codProxEstacao    = lerIntBuf(buf + off); off += 4;
    r->distProxEstacao   = lerIntBuf(buf + off); off += 4;
    r->codLinhaIntegra   = lerIntBuf(buf + off); off += 4;
    r->codEstacaoIntegra = lerIntBuf(buf + off); off += 4;

    off += lerStringVar(buf + off, r->nomeEstacao, TAM_NOME);
    off += lerStringVar(buf + off, r->nomeLinha,   TAM_LINHA);
    /* o restante do registro e apenas preenchimento ('$') e e ignorado */
    (void)off;
}

/* ===========================================================================
 * Construcao do grafo
 * =========================================================================== */

Grafo *construirGrafo(const char *nomeArquivo) {
    FILE *f;
    unsigned char cabecalho[TAM_CABECALHO];
    unsigned char buf[TAM_REGISTRO];
    RegistroEstacao *regs = NULL;
    ParCodNome *idx = NULL;
    Grafo *g = NULL;
    int nReg = 0, cap = 0, i;

    /* 1. abertura e validacao do cabecalho */
    f = fopen(nomeArquivo, "rb");
    if (!f) return NULL;

    if (fread(cabecalho, 1, TAM_CABECALHO, f) != (size_t)TAM_CABECALHO) {
        fclose(f);
        return NULL;
    }
    if (cabecalho[0] != STATUS_CONSISTENTE) {   /* arquivo inconsistente */
        fclose(f);
        return NULL;
    }

    /* 2. leitura de todos os registros de dados, guardando os nao removidos */
    while (fread(buf, 1, TAM_REGISTRO, f) == (size_t)TAM_REGISTRO) {
        RegistroEstacao r;
        parseRegistro(buf, &r);
        if (r.removido == REGISTRO_REMOVIDO) continue;   /* ignora removidos */

        if (nReg == cap) {                               /* cresce o vetor */
            int novaCap = cap ? cap * 2 : 64;
            RegistroEstacao *tmp = realloc(regs, sizeof(RegistroEstacao) * novaCap);
            if (!tmp) { free(regs); fclose(f); return NULL; }
            regs = tmp;
            cap = novaCap;
        }
        regs[nReg++] = r;
    }
    fclose(f);

    /* 3. indice codigo -> nome (ordenado por codigo para busca binaria) */
    idx = malloc(sizeof(ParCodNome) * (nReg > 0 ? nReg : 1));
    if (!idx) { free(regs); return NULL; }
    for (i = 0; i < nReg; i++) {
        idx[i].cod  = regs[i].codEstacao;
        idx[i].nome = regs[i].nomeEstacao;
    }
    qsort(idx, nReg, sizeof(ParCodNome), cmpParCod);

    /* 4. criacao do grafo e insercao dos vertices (o TAD deduplica nomes) */
    g = grafoCriar(nReg > 0 ? nReg : 8);
    if (!g) { free(idx); free(regs); return NULL; }
    for (i = 0; i < nReg; i++) {
        grafoInserirVertice(g, regs[i].nomeEstacao);
    }

    /* 5. insercao das arestas */
    for (i = 0; i < nReg; i++) {
        RegistroEstacao *r = &regs[i];
        const char *nomeProx, *nomeIntegra;

        /* aresta para a proxima estacao (se codProxEstacao nao for nulo) */
        nomeProx = resolveNome(idx, nReg, r->codProxEstacao);
        if (nomeProx) {
            grafoInserirAresta(g, r->nomeEstacao, nomeProx,
                               r->distProxEstacao, r->nomeLinha);
        }

        /* aresta de integracao (se codEstacaoIntegra nao for nulo e o nome
         * da estacao de integracao for diferente do nome da estacao atual) */
        nomeIntegra = resolveNome(idx, nReg, r->codEstacaoIntegra);
        if (nomeIntegra && strcmp(r->nomeEstacao, nomeIntegra) != 0) {
            grafoInserirAresta(g, r->nomeEstacao, nomeIntegra, 0, LINHA_INTEGRACAO);
        }
    }

    free(idx);
    free(regs);
    return g;
}
