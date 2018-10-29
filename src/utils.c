#include "utils.h"

void *carregar_arquivo(const char *nome_arquivo, int *tamanho_arquivo) {
	uint8_t *dados = NULL;
    FILE *in = fopen(nome_arquivo, "rb");
    if (in == NULL) {
        *tamanho_arquivo = 0;
        return NULL;
	}

    fseek(in, 0, SEEK_END);
    *tamanho_arquivo = ftell(in) + 1;
    rewind(in);
    dados = malloc(*tamanho_arquivo);
	if (dados == NULL) {
        *tamanho_arquivo = 0;
        fclose(in);
		return NULL;
	}
    fread(dados, 1, *tamanho_arquivo, in);
    fclose(in);

    return dados;
}
