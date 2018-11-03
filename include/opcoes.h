#ifndef OPCOES_H
#define OPCOES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <argp.h>
#include <ctype.h>

typedef struct tipo_t {
	enum {FT_BOOL, FT_CHAR, FT_INT, FT_DOUBLE, FT_STR} cod;
	size_t tamanho;
} tipo_t;
#define tipo_bool {FT_BOOL, sizeof(int)}
#define tipo_char {FT_CHAR, sizeof(char)}
#define tipo_int {FT_INT, sizeof(int)}
#define tipo_double {FT_DOUBLE, sizeof(double)}
#define tipo_str(max) {FT_STR, max}

#define DESCRICAO_MAX 256

typedef struct opcao_t {
	///Caractere usado na linha de comando para identificar esta opção.
	char flag;

	///Tipo da opção.
	tipo_t tipo;

	///Endereço da variável que armazenará o valor.
	void *valor;

	///Afeta como a opção aparecerá na ajuda.
	///Caso nome_valor seja da forma nome=valor_padrao,
	///valor_padrao será convertido e atribuído em valor
	///inicialmente.
	///Caso o tipo seja FT_BOOL e nome_valor seja != NULL,
	///então é assumido que nome_valor é o valor padrão (0 ou 1).
	const char *nome_valor;

	///Indica o índice no vetor de opcções se a opção é obrigatória.
	int __obrigatoria;

	///Descrição da opção. Use %s ao se referir a nome_valor.
	const char *descricao;
} opcao_t;

#define OPCAO_INIT(flag, tipo, valor, nome_valor, descricao)\
	{flag, tipo, valor, nome_valor, -1, descricao}

void parse_args(int arc, char * const argv[], opcao_t *opcoes, int qtd_opcoes);

#endif //OPCOES_H
