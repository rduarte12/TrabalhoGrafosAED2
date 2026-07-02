#ifndef FORNECIDAS_H
#define FORNECIDAS_H

/* Le todo o arquivo binario 'arquivo' e imprime na saida padrao um valor de verificacao 
 * O arquivo deve ter sido previamente fechado com fclose. */
void BinarioNaTela(char *arquivo);

/* Le da entrada padrao uma string delimitada por aspas duplas (") e a
 * armazena em 'str' (ja alocada) sem as aspas. Trata o valor "NULO"
 * copiando a string vazia. */
void ScanQuoteString(char *str);

#endif /* FORNECIDAS_H */
