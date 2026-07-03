#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "grafo.h"

enum { 
    ERRO = -1,
    SUCCESSO = 0 
};

typedef struct Aresta {
    char destino[TAM_NOME];     // nome do vertice de destino
    int distancia;              // peso da aresta (distancia entre as estacoes)
    char **linhas;              // lista com os nomes das linhas que conectam origem->destino
    int numLinhas;              // quantidade de linhas cadastradas
    int capLinhas;              // capacidade do array de linhas (para realocacao dinamica)
    struct Aresta *prox;        // ponteiro para a proxima aresta na lista encadeada do vertice de origem
} Aresta;

typedef struct Vertice {
    char nome[TAM_NOME];          // nome do vertice
    struct Aresta *listaArestas;  // cabeça da lista encadeada
} Vertice;

typedef struct Grafo {
    struct Vertice *vertices;      // vetor dinâmico, ordenado por nome
    int numVertices;               // quantidade de vertices atualmente no grafo
    int capacidade;                // capacidade do vetor de vertices (para realocacao dinamica)
} Grafo;

#define CAP_INICIAL_PADRAO 8
#define CAP_LINHAS_PADRAO  4


/* Cria uma aresta nova, ja com o array de
 * linhas alocado. Retorna NULL em erro. 
 */
static Aresta *arestaCriar(const char *destino, int distancia) {
    Aresta *a = malloc(sizeof(Aresta));
    if (!a) return NULL; // alocacao falhou

    strncpy(a->destino, destino, TAM_NOME - 1); // garante que a string seja terminada em '\0'
    a->destino[TAM_NOME - 1] = '\0';            // usamos strncpy para evitar buffer overflow
    a->distancia = distancia;

    a->numLinhas = 0;
    a->capLinhas = CAP_LINHAS_PADRAO;
    a->linhas = malloc(sizeof(char *) * a->capLinhas);
    if (!a->linhas) {
        free(a);
        return NULL;
    }

    a->prox = NULL;
    return a;
}

/* Libera uma aresta e todas as strings de linha que ela guarda. */
static void arestaDestruir(Aresta *a) {
    int i;
    if (!a) return;                       // alocacao falhou,
    for (i = 0; i < a->numLinhas; i++) {  // loop em que se percorre o array de linhas e libera cada string
        free(a->linhas[i]);               // libera cada string
    }
    free(a->linhas);
    free(a);
}

/* Insere nomeLinha no array de linhas da aresta 'a', mantendo a
 * ordem crescente. Se a linha ja estiver presente, nao duplica.
 * Retorna SUCCESSO em sucesso, ERRO em erro de alocacao. */
static int arestaAdicionarLinha(Aresta *a, const char *nomeLinha) {
    int i, pos;

    for (i = 0; i < a->numLinhas; i++) {            // loop em que se percorre o array de linhas da aresta
        int cmp = strcmp(nomeLinha, a->linhas[i]);  // compara o nome da linha a ser inserida com o nome da linha atual
        if (cmp == 0) return SUCCESSO;              // linha ja cadastrada
        if (cmp < 0) break;                         // achou a posicao de insercao
    }                                               // se terminar fora do break, indica que deve ser inserido no final
    pos = i;

    if (a->numLinhas == a->capLinhas) {             // precisa realocar o array de linhas
        int novaCap = a->capLinhas * 2;             // dobra a capacidade 
        char **novo = realloc(a->linhas, sizeof(char *) * novaCap); // realoca o array de linhas
        if (!novo) return ERRO;                     // alocacao falhou
        a->linhas = novo;
        a->capLinhas = novaCap;
    }

    for (i = a->numLinhas; i > pos; i--) {          // abre espaco na posicao 'pos' para a nova linha
        a->linhas[i] = a->linhas[i - 1];
    }

    a->linhas[pos] = malloc(strlen(nomeLinha) + 1); // aloca memoria para a nova linha
    if (!a->linhas[pos]) return ERRO;               // alocacao falhou
    strcpy(a->linhas[pos], nomeLinha);              // copia o nome da linha para a nova posicao
    a->numLinhas++;                                 // incrementa o numero de linhas cadastradas

    return SUCCESSO;
}

// ===================================================================================================
// GRAFO

/**
 * Cria um grafo vazio com capacidade inicial para capacidadeInicial vertices.
 * Retorna NULL em caso de falha de alocacao.
 */
Grafo *grafoCriar(int capacidadeInicial) {
    Grafo *g = malloc(sizeof(Grafo));       // aloca memoria para o grafo
    if (!g) return NULL;                    // alocacao falhou

    if (capacidadeInicial <= 0) capacidadeInicial = CAP_INICIAL_PADRAO; // se a capacidade inicial for invalida, usa o padrao

    g->vertices = malloc(sizeof(Vertice) * capacidadeInicial);          // aloca memoria para o vetor de vertices
    if (!g->vertices) {                                                 // alocacao falhou
        free(g);
        return NULL;
    }

    g->numVertices = 0;
    g->capacidade = capacidadeInicial;
    return g;
}

/**
 * Libera a memoria do grafo
 */
void grafoDestruir(Grafo *g) {
    int i;
    if (!g) return;

    for (i = 0; i < g->numVertices; i++) {          // percorre todos os vertices do grafo
        Aresta *a = g->vertices[i].listaArestas;    // percorre a lista de arestas do vertice atual
        while (a != NULL) {
            Aresta *prox = a->prox;                 // guarda o proximo antes de destruir a aresta atual
            arestaDestruir(a);                      // libera a memoria da aresta atual
            a = prox;                               // avança para a proxima aresta
        }
    }

    free(g->vertices);
    free(g);
}

// ===================================================================================================
// Manipulacao de vertices


/**
 * Busca binaria pelo nome da estacao no vetor de vertices.
 * Retorna o indice do vertice, ou ERRO se nao encontrado.
 */
int grafoBuscarVertice(const Grafo *g, const char *nomeEstacao) {
    int ini, fim, meio, cmp; // busca binaria

    if (!g || !nomeEstacao) return ERRO;

    ini = 0;
    fim = g->numVertices - 1;  // subtrai um para ter o indice do ultimo elemento
    while (ini <= fim) {
        meio = ini + (fim - ini) / 2;
        cmp = strcmp(nomeEstacao, g->vertices[meio].nome);
        if (cmp == 0) return meio;
        else if (cmp < 0) fim = meio - 1;
        else ini = meio + 1;
    }
    return ERRO;
}

/**
 * Insere um vertice com o nome informado, mantendo o vetor ordenado crescentemente por nome. Se o vertice ja existir,
 * nao duplica (estacoes com mesmo nome sao a mesma estacao). Retorna o indice do vertice (novo ou existente), ou ERRO em erro.
 */
int grafoInserirVertice(Grafo *g, const char *nomeEstacao) {
    int ini, fim, meio, pos, i, cmp;

    if (!g || !nomeEstacao) return ERRO;

    /* busca binaria: acha o vertice (se existir) ou a posicao onde
     * ele deveria entrar para manter o vetor ordenado */
    ini = 0;
    fim = g->numVertices - 1;
    while (ini <= fim) {
        meio = ini + (fim - ini) / 2;
        cmp = strcmp(nomeEstacao, g->vertices[meio].nome);
        if (cmp == 0) return meio;          /* ja existe */
        else if (cmp < 0) fim = meio - 1;
        else ini = meio + 1;
    }
    pos = ini;

    if (g->numVertices == g->capacidade) { // se verdadeiro precisa realocar o vetor de vertices
        int novaCap = g->capacidade * 2;
        Vertice *novo = realloc(g->vertices, sizeof(Vertice) * novaCap);
        if (!novo) return ERRO;
        g->vertices = novo;
        g->capacidade = novaCap;
    }

    // abre espaco na posicao pos
    for (i = g->numVertices; i > pos; i--) { // loop em que se percorre o vetor de vertices de tras para frente, abrindo espaco na posicao pos
        g->vertices[i] = g->vertices[i - 1];
    }

    strncpy(g->vertices[pos].nome, nomeEstacao, TAM_NOME - 1); // copia o nome da estacao para o vertice 
    g->vertices[pos].nome[TAM_NOME - 1] = '\0';                 // na posicao pos, garantindo que a string seja terminada em '\0'
    g->vertices[pos].listaArestas = NULL;
    g->numVertices++;

    return pos;
}

/* =======================================================
 * Manipulacao de arestas
 */


/**
 * Insere uma aresta (nomeOrigem -> nomeDestino) com o peso e o nome de linha informados.
 * retorna SUCCESSO em sucesso, ERRO em erro.
 */
int grafoInserirAresta(Grafo *g, const char *nomeOrigem,
                        const char *nomeDestino, int distancia,
                        const char *nomeLinha) {
    int iOrigem, iDestino;  // indices dos vertices de origem e destino
    Aresta *cur, *ant, *nova; // ponteiros current, anterior e nova aresta

    if (!g || !nomeOrigem || !nomeDestino || !nomeLinha) return ERRO; // verifica se os ponteiros sao validos

    iOrigem = grafoBuscarVertice(g, nomeOrigem);      // busca o indice do vertice de origem
    iDestino = grafoBuscarVertice(g, nomeDestino);     // busca o indice do vertice de destino
    if (iOrigem == -1 || iDestino == -1) return ERRO;   // se algum dos vertices nao existir, retorna erro

    ant = NULL;
    cur = g->vertices[iOrigem].listaArestas;  // ponteiro para a lista de arestas do vertice de origem


    while (cur != NULL) {
        int cmpDestino = strcmp(nomeDestino, cur->destino); // compara o nome do destino da aresta atual com o nome do destino da arestaa ser inseirdo

        if (cmpDestino < 0) {
            break; /* posicao de insercao encontrada, antes de cur */
        }

        if (cmpDestino == 0) {
            if (cur->distancia == distancia) {
                /* mesmo par origem->destino com a mesma distancia:
                 * so acrescenta a linha na aresta existente */
                return arestaAdicionarLinha(cur, nomeLinha);
            }
            /* mesmo destino, mas distancia diferente: duas arestas
             * distintas para o mesmo par. Desempata pelo nome da
             * linha (comparando com a menor linha ja cadastrada
             * em cur) para decidir a ordem entre elas. */
            if (cur->numLinhas > 0 && strcmp(nomeLinha, cur->linhas[0]) < 0) {
                break;
            }
        }

        ant = cur;
        cur = cur->prox;
    }

    /* nao encontrou aresta equivalente: cria um novo no e insere
     * entre 'ant' e 'cur' */
    nova = arestaCriar(nomeDestino, distancia);
    if (!nova) return ERRO;

    if (arestaAdicionarLinha(nova, nomeLinha) != SUCCESSO) {
        arestaDestruir(nova);
        return ERRO;
    }

    nova->prox = cur;
    if (ant == NULL) {
        g->vertices[iOrigem].listaArestas = nova;
    } else {
        ant->prox = nova;
    }

    return SUCCESSO;
}

/* =======================================================
 * Getters - vertices
 * ======================================================= */

 // Retorna o numero total de vertices do grafo.
int grafoGetNumVertices(const Grafo *g) {
    if (!g) return 0;
    return g->numVertices;
}
// Retorna o nome do vertice na posicao indice do vetor ordenado (0 <= indice < grafoGetNumVertices). Retorna NULL se indice invalido.
const char *grafoGetNomeVertice(const Grafo *g, int indice) {
    if (!g || indice < 0 || indice >= g->numVertices) return NULL;
    return g->vertices[indice].nome;
}

/* =======================================================
 * Getters - arestas
 * ======================================================= */

int grafoGetNumArestas(const Grafo *g, int indiceVertice) {
    Aresta *a;
    int count = 0;

    if (!g || indiceVertice < 0 || indiceVertice >= g->numVertices) return 0;

    a = g->vertices[indiceVertice].listaArestas;
    while (a != NULL) {
        count++;
        a = a->prox;
    }
    return count;
}

const Aresta *grafoGetAresta(const Grafo *g, int indiceVertice, int indiceAresta) {
    Aresta *a;
    int i;

    if (!g || indiceVertice < 0 || indiceVertice >= g->numVertices) return NULL;
    if (indiceAresta < 0) return NULL;

    a = g->vertices[indiceVertice].listaArestas;
    for (i = 0; i < indiceAresta && a != NULL; i++) {
        a = a->prox;
    }

    return a; // NULL se indiceAresta for >= numero de arestas 
}

const char *arestaGetDestino(const Aresta *a) {
    if (!a) return NULL;
    return a->destino;
}

int arestaGetDistancia(const Aresta *a) {
    if (!a) return -1;
    return a->distancia;
}

int arestaGetNumLinhas(const Aresta *a) {
    if (!a) return 0;
    return a->numLinhas;
}

const char *arestaGetLinha(const Aresta *a, int indice) {
    if (!a || indice < 0 || indice >= a->numLinhas) return NULL;
    return a->linhas[indice];
}