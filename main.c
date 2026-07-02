/* ============================================================================
 * Trabalho Pratico 2 - SCC0503 (Algoritmos e Estruturas de Dados II) - ICMC/USP
 *
 * Mateus Cardoso Vargas Saracuza - NUSP 13674087
 * Rafael Mendonça Duarte – NUSP 16817608 
 *
 * main.c - Ponto de entrada: le o identificador da funcionalidade na entrada
 *          padrao e despacha para a rotina correspondente.
 * ============================================================================ */

#include <stdio.h>
#include "funcionalidades.h"

/* Tamanho maximo do nome do arquivo de entrada. */
#define TAM_ARQUIVO 256

int main(void) {
    int funcionalidade;
    char arquivo[TAM_ARQUIVO];

    /* a primeira entrada da entrada padrao e sempre o identificador */
    if (scanf("%d", &funcionalidade) != 1) return 0;

    switch (funcionalidade) {
        case 10:
            /* [10] arquivoEntrada.bin */
            if (scanf("%255s", arquivo) != 1) return 0;
            funcionalidade10(arquivo);
            break;

        case 11:
            /* [11] arquivoEntrada.bin nomeEstacaoOrigem "Origem"
             *      nomeEstacaoDestino "Destino" */
            if (scanf("%255s", arquivo) != 1) return 0;
            funcionalidade11(arquivo);
            break;

        case 12:
            /* TODO [12]: arvore geradora minima + busca em profundidade */
            break;

        case 13:
            /* TODO [13]: contagem de ciclos simples a partir da origem */
            break;

        default:
            /* identificador nao reconhecido */
            break;
    }

    return 0;
}
