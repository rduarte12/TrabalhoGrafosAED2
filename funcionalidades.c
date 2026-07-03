/* ============================================================================
 * Trabalho Pratico 2 - SCC0503 (Algoritmos e Estruturas de Dados II) - ICMC/USP
 *
 * Mateus Cardoso Vargas Saracuza - NUSP 13674087
 * Rafael Mendonça Duarte – NUSP 16817608 
 * 
 * funcionalidades.c - Implementacao das funcionalidades [10], [11], [12] e [13].
 * ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "funcionalidades.h"
#include "leitura.h"
#include "grafo.h"
#include "fornecidas.h"

/*
 * Separador usado entre TODOS os valores impressos em uma mesma linha.
 * Isolado em uma unica constante para permitir troca imediata
  */
#define SEP ", "

/* Mensagem de erro geral, exibida quando o grafo nao pode ser construido. */
#define MSG_FALHA "Falha na execução da funcionalidade.\n"

/* Mensagem exibida quando nao existe caminho entre origem e destino. */
#define MSG_SEM_CAMINHO "Não existe caminho entre as estações solicitadas.\n"

/* ===========================================================================
 * [10] Impressao do grafo
 * =========================================================================== */

void funcionalidade10(const char *arquivo) {
    Grafo *g;
    int n, i, j, k;

    g = construirGrafo(arquivo);
    if (!g) {
        printf(MSG_FALHA);
        return;
    }

    n = grafoGetNumVertices(g);
    for (i = 0; i < n; i++) {
        int numArestas;

        numArestas = grafoGetNumArestas(g, i);
        if (numArestas == 0) continue;

        /* nome do vertice (primeiro valor da linha) */
        printf("%s", grafoGetNomeVertice(g, i));

        /* lista de adjacencias, ja ordenada por destino e por linha */
        for (j = 0; j < numArestas; j++) {
            const Aresta *a = grafoGetAresta(g, i, j);
            int numLinhas = arestaGetNumLinhas(a);

            printf("%s%s", SEP, arestaGetDestino(a));      /* nome do destino  */
            printf("%s%d", SEP, arestaGetDistancia(a));    /* distancia        */
            for (k = 0; k < numLinhas; k++) {              /* nomes das linhas */
                printf("%s%s", SEP, arestaGetLinha(a, k));
            }
        }
        printf("\n");
    }

    grafoDestruir(g);
}

/* ===========================================================================
 * [11] Caminho mais curto
 * =========================================================================== */

/*
 * Executa o algoritmo de Dijkstra a partir de 'origem'. Preenche os vetores
 * 'dist' (distancia minima da origem a cada vertice) e 'pred' (predecessor de
 * cada vertice no caminho minimo, -1 se nao houver).
 */
static void dijkstra(const Grafo *g, int origem, int *dist, int *pred) {
    int n = grafoGetNumVertices(g);
    char *visitado;
    int it, i;

    /* inicializa antes de qualquer retorno para nunca deixar dist/pred sujos */
    for (i = 0; i < n; i++) {
        dist[i] = INT_MAX;   /* INT_MAX representa "infinito" */
        pred[i] = -1;
    }
    dist[origem] = 0;

    visitado = calloc(n, sizeof(char));
    if (!visitado) return;   /* sem memoria: dist permanece "infinito" */

    for (it = 0; it < n; it++) {
        int u = -1, numArestas, j;

        /* seleciona o vertice nao visitado de menor distancia; a ordem
         * crescente de indices garante o desempate por menor nome */
        for (i = 0; i < n; i++) {
            if (!visitado[i] && dist[i] != INT_MAX &&
                (u == -1 || dist[i] < dist[u])) {
                u = i;
            }
        }
        if (u == -1) break;      /* vertices restantes sao inalcancaveis */
        visitado[u] = 1;

        /* relaxa as arestas de u (ja ordenadas por destino e linha) */
        numArestas = grafoGetNumArestas(g, u);
        for (j = 0; j < numArestas; j++) {
            const Aresta *a = grafoGetAresta(g, u, j);
            int v = grafoBuscarVertice(g, arestaGetDestino(a));
            long nova;

            if (v < 0 || visitado[v]) continue;
            nova = (long)dist[u] + arestaGetDistancia(a);

            if (nova < dist[v]) {
                dist[v] = (int)nova;
                pred[v] = u;
            } else if (nova == dist[v]) {
                /* mantem o predecessor de menor nome */
                if (pred[v] == -1 ||
                    strcmp(grafoGetNomeVertice(g, u),
                           grafoGetNomeVertice(g, pred[v])) < 0) {
                    pred[v] = u;
                }
            }
        }
    }

    free(visitado);
}

void funcionalidade11(const char *arquivo) {
    char campo[64];                 /* rotulo do campo (ex.: nomeEstacaoOrigem) */
    char origem[TAM_NOME];          /* valor da estacao de origem               */
    char destino[TAM_NOME];         /* valor da estacao de destino              */
    Grafo *g;
    int iOrigem, iDestino;

    /* leitura dos parametros: <rotulo> "valor" <rotulo> "valor" */
    if (scanf("%63s", campo) != 1) return;
    ScanQuoteString(origem);
    if (scanf("%63s", campo) != 1) return;
    ScanQuoteString(destino);

    g = construirGrafo(arquivo);
    if (!g) {
        printf(MSG_FALHA);
        return;
    }

    iOrigem  = grafoBuscarVertice(g, origem);
    iDestino = grafoBuscarVertice(g, destino);

    /* estacao inexistente no grafo: tratado como ausencia de caminho */
    if (iOrigem < 0 || iDestino < 0) {
        printf(MSG_SEM_CAMINHO);
        grafoDestruir(g);
        return;
    }

    /* origem e destino sao a mesma estacao: 0 estacoes, distancia 0 */
    if (iOrigem == iDestino) {
        printf("Numero de estacoes que serao percorridas: 0\n");
        printf("Distancia que sera percorrida: 0\n");
        printf("%s\n", origem);
        grafoDestruir(g);
        return;
    }

    {
        int n = grafoGetNumVertices(g);
        int *dist = malloc(sizeof(int) * n);
        int *pred = malloc(sizeof(int) * n);

        if (!dist || !pred) {       /* falha de alocacao */
            free(dist); free(pred);
            printf(MSG_FALHA);
            grafoDestruir(g);
            return;
        }

        dijkstra(g, iOrigem, dist, pred);

        if (dist[iDestino] == INT_MAX) {
            /* destino inalcancavel a partir da origem */
            printf(MSG_SEM_CAMINHO);
        } else {
            int *caminho = malloc(sizeof(int) * n);
            int cnt = 0, cur = iDestino, p;

            /* reconstroi o caminho do destino ate a origem via predecessores */
            while (cur != -1) {
                caminho[cnt++] = cur;
                if (cur == iOrigem) break;
                cur = pred[cur];
            }

            /* x = numero de estacoes sem contar a origem = (vertices - 1) */
            printf("Numero de estacoes que serao percorridas: %d\n", cnt - 1);
            printf("Distancia que sera percorrida: %d\n", dist[iDestino]);

            /* imprime o caminho na ordem origem -> ... -> destino */
            for (p = cnt - 1; p >= 0; p--) {
                printf("%s", grafoGetNomeVertice(g, caminho[p]));
                if (p > 0) printf("%s", SEP);
            }
            printf("\n");

            free(caminho);
        }

        free(dist);
        free(pred);
    }

    grafoDestruir(g);
}
