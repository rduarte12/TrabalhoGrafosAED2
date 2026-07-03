#ifndef LEITURA_H
#define LEITURA_H

/*
 * leitura.h
 *
 * Modulo responsavel pela leitura do arquivo binario de estacoes (no
 * mesmo formato gerado pelo trabalho introdutorio e pelo trabalho
 * pratico 1) e pela construcao do grafo direcionado ponderado a partir
 * dos registros nao removidos.
 *
 * Formato do arquivo (little-endian):
 *   - Registro de cabecalho: 17 bytes = status (1 char) + 4 inteiros.
 *     O campo status vale '1' quando o arquivo esta consistente.
 *   - Registros de dados: tamanho fixo de 80 bytes cada. Campos inteiros
 *     de tamanho fixo seguidos dos campos string (nome da estacao e nome
 *     da linha), cada um precedido por seu tamanho (inteiro). O espaco
 *     nao utilizado do registro e preenchido com o caractere '$'.
 *     Valores inteiros nulos sao representados por -1 e o campo removido
 *     vale '1' para registros logicamente removidos e '0' caso contrario.
 */

#include "grafo.h"

/* ---- Constantes do formato binario ---- */
#define TAM_REGISTRO      80    /* tamanho fixo, em bytes, de um registro de dados */
#define TAM_CABECALHO     17    /* status (1 byte) + 4 inteiros (16 bytes)         */
#define STATUS_CONSISTENTE '1'  /* valor do status para um arquivo consistente     */
#define REGISTRO_REMOVIDO  '1'  /* marcador de registro logicamente removido       */
#define CAMPO_NULO        (-1)  /* sentinela de valor nulo para campos inteiros     */

/* Nome exato da linha usada nas arestas de integracao (restricao da spec). */
#define LINHA_INTEGRACAO "Integração"

/*
 * Registro de dados de uma estacao. Representa um segmento direcionado
 * "estacao atual -> proxima estacao" em uma determinada linha.
 */
typedef struct {
    char removido;              /* '0' presente, '1' removido logicamente        */
    int  encadeamento;          /* RRN do proximo removido (-1 se nao ha)         */
    int  codEstacao;            /* codigo da estacao atual                        */
    int  codLinha;              /* codigo da linha                                */
    int  codProxEstacao;        /* codigo da proxima estacao (-1 = nulo)          */
    int  distProxEstacao;       /* distancia ate a proxima estacao (-1 = nulo)    */
    int  codLinhaIntegra;       /* codigo da linha de integracao (-1 = nulo)      */
    int  codEstacaoIntegra;     /* codigo da estacao de integracao (-1 = nulo)    */
    char nomeEstacao[TAM_NOME]; /* nome da estacao atual                          */
    char nomeLinha[TAM_LINHA];  /* nome da linha                                  */
} RegistroEstacao;

/*
 * Le o arquivo binario 'nomeArquivo' e constroi o grafo direcionado
 * ponderado de estacoes. Registros removidos sao ignorados. Retorna o
 * grafo construido (que deve ser liberado com grafoDestruir) ou NULL em
 * caso de erro (falha ao abrir o arquivo ou cabecalho inconsistente).
 */
Grafo *construirGrafo(const char *nomeArquivo);

#endif /* LEITURA_H */
