#ifndef FUNCIONALIDADES_H
#define FUNCIONALIDADES_H

/*
 * funcionalidades.h
 *
 * Declara as funcionalidades do programa que operam sobre o grafo de
 * estacoes construido a partir do arquivo binario de entrada.
 *
 * Cada funcionalidade constroi o grafo internamente (via construirGrafo) e
 * escreve o resultado na saida padrao, seguindo estritamente o formato
 * exigido pela especificacao.
 */

/*
 * [10] Recupera os dados do arquivo binario, gera o grafo e o imprime na
 * saida padrao (uma linha por vertice, com sua lista de adjacencias).
 *   arquivo: nome do arquivo binario de entrada.
 */
void funcionalidade10(const char *arquivo);

/*
 * [11] Determina o caminho mais curto (algoritmo de Dijkstra) entre a
 * estacao de origem e a de destino, lidas da entrada padrao (rotulo do
 * campo seguido do valor entre aspas). Imprime o numero de estacoes, a
 * distancia total e o caminho, ou a mensagem de ausencia de caminho.
 *   arquivo: nome do arquivo binario de entrada.
 */
void funcionalidade11(const char *arquivo);

#endif /* FUNCIONALIDADES_H */
