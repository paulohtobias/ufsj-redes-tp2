#include "utils.h"

void *carregar_arquivo(FILE *in, int *tamanho_arquivo) {
	if (in == NULL) {
		perror("carregar_arquivo-abrir");
		*tamanho_arquivo = 0;
		return NULL;
	}
	uint8_t *dados = NULL;

	fseek(in, 0, SEEK_END);
	*tamanho_arquivo = ftell(in);
	rewind(in);
	dados = malloc(*tamanho_arquivo);
	if (dados == NULL) {
		perror("carregar_arquivo-malloc");
		*tamanho_arquivo = 0;
		fclose(in);
		return NULL;
	}
	fread(dados, 1, *tamanho_arquivo, in);
	fclose(in);

	return dados;
}
