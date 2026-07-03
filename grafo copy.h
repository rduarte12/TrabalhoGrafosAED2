#ifndef GRAFO_H
#define GRAFO_H

/*
 * grafo.h
 *
 * Interface pública do TAD Grafo: representa um grafo direcionado
 * ponderado por meio de lista de adjacências (vetor de vértices,
 * cada um com um array dinâmico de arestas).
 *
 * Encapsulamento: os tipos sao opacos. A definicao completa dos structs
 * e as funcoes-helper ficam em grafo.c. O modulo que constroi o grafo
 * (main.c) nunca acessa campos diretamente, usando apenas as funcoes
 * (getters) abaixo. Isso evita inconsistencias (ex.: aresta fora de ordem)
 * e permite mudancas internas sem quebrar o main.
 *
 * Arestas sao armazenadas em arrays dinamicos, mantidas ordenadas por
 * destino (e por linha em caso de empate de destino).
 */

#define TAM_NOME 100   /* tamanho maximo do nome de uma estacao   */
#define TAM_LINHA 50   /* tamanho maximo do nome de uma linha     */

/* ---- Tipos opacos ---- */
typedef struct Aresta Aresta;
typedef struct Vertice Vertice;
typedef struct Grafo Grafo;

/* =======================================================
 * Ciclo de vida do grafo
 * ======================================================= */

/* Cria um grafo vazio. capacidadeInicial e uma dica de tamanho
 * para o vetor dinamico de vertices (evita realocacoes iniciais).
 * Retorna NULL em caso de falha de alocacao. */
Grafo *grafoCriar(int capacidadeInicial);

/* Libera toda a memoria do grafo: vetor de vertices, listas de
 * arestas de cada vertice e os nomes de linha armazenados em
 * cada aresta. */
void grafoDestruir(Grafo *g);

/* =======================================================
 * Manipulacao de vertices
 * ======================================================= */

/* Insere um vertice com o nome informado, mantendo o vetor
 * ordenado crescentemente por nome. Se o vertice ja existir,
 * nao duplica (estacoes com mesmo nome sao a mesma estacao).
 * Retorna o indice do vertice (novo ou existente), ou -1 em erro. */
int grafoInserirVertice(Grafo *g, const char *nomeEstacao);

/* Busca binaria pelo nome da estacao no vetor de vertices.
 * Retorna o indice do vertice, ou -1 se nao encontrado. */
int grafoBuscarVertice(const Grafo *g, const char *nomeEstacao);

/* =======================================================
 * Manipulacao de arestas
 * ======================================================= */

/* Insere uma aresta (nomeOrigem -> nomeDestino) com o peso e o
 * nome de linha informados. Ambos os vertices devem ja existir
 * no grafo (inseridos via grafoInserirVertice).
 *
 * Regra de negocio: se ja existir uma aresta nomeOrigem->nomeDestino
 * com a mesma distancia (mesmo par de estacoes conectado por mais
 * de uma linha), a implementacao deve apenas adicionar nomeLinha
 * a lista de linhas dessa aresta ja existente, mantendo os nomes
 * de linha ordenados crescentemente, em vez de criar um novo no.
 * Caso contrario, cria um novo no de aresta e o insere na lista
 * encadeada do vertice de origem, mantendo-a ordenada
 * crescentemente por nome do destino.
 *
 * Retorna 0 em sucesso, -1 em erro (ex.: vertice inexistente). */
int grafoInserirAresta(Grafo *g, const char *nomeOrigem,
                        const char *nomeDestino, int distancia,
                        const char *nomeLinha);

/* =======================================================
 * Getters - vertices
 * ======================================================= */

/* Numero total de vertices do grafo. */
int grafoGetNumVertices(const Grafo *g);

/* Nome do vertice na posicao 'indice' do vetor ordenado
 * (0 <= indice < grafoGetNumVertices). Retorna NULL se indice
 * invalido. */
const char *grafoGetNomeVertice(const Grafo *g, int indice);



/* =======================================================
 * Getters - arestas (acessam array de arestas do vertice)
 * ======================================================= */

/* Numero de arestas (vizinhos) do vertice em 'indice'. */
int grafoGetNumArestas(const Grafo *g, int indiceVertice);

/* Aresta 'i' do vertice em 'indiceVertice' (0 <= i < grafoGetNumArestas).
 * Arestas estao ordenadas por nome do destino (e por linha em caso de empate).
 * Retorna NULL se indices invalidos. */
const Aresta *grafoGetAresta(const Grafo *g, int indiceVertice, int indiceAresta);

/* Nome da estacao de destino da aresta. */
const char *arestaGetDestino(const Aresta *a);

/* Distancia (peso) da aresta. */
int arestaGetDistancia(const Aresta *a);

/* Quantidade de nomes de linha associados a aresta. */
int arestaGetNumLinhas(const Aresta *a);

/* Nome da linha na posicao 'indice' (0 <= indice < arestaGetNumLinhas),
 * ja ordenados crescentemente. Retorna NULL se indice invalido. */
const char *arestaGetLinha(const Aresta *a, int indice);

#endif /* GRAFO_H */