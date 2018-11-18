#include "utils.h"

int diretorio(const char *caminho) {
	struct stat sb;

	if (stat(caminho, &sb) == 0) {
		return S_ISDIR(sb.st_mode);
	}

	return 0;
}

void *carregar_arquivo(FILE *in, int *tamanho_arquivo) {
	if (in == NULL) {
		handle_error(0, "carregar_arquivo-abrir");
		*tamanho_arquivo = 0;
		return NULL;
	}
	uint8_t *dados = NULL;

	fseek(in, 0, SEEK_END);
	*tamanho_arquivo = ftell(in);
	rewind(in);
	dados = malloc(*tamanho_arquivo);
	if (dados == NULL) {
		handle_error(0, "carregar_arquivo-malloc");
		*tamanho_arquivo = 0;
		fclose(in);
		return NULL;
	}
	fread(dados, 1, *tamanho_arquivo, in);
	fclose(in);

	return dados;
}

void exibir_limite_arquivos() {
	struct rlimit fdlimit;
	getrlimit(RLIMIT_NOFILE, &fdlimit);

	printf("Máx. descritores: %lu/%lu\n", fdlimit.rlim_cur, fdlimit.rlim_max);
}

void definir_limite_arquivos(rlim_t limite) {
	struct rlimit fdlimit;
	getrlimit(RLIMIT_NOFILE, &fdlimit);
	fdlimit.rlim_cur = limite;
	if (setrlimit(RLIMIT_NOFILE, &fdlimit) == -1) {
		handle_error(errno, "definir_limite_arquivos");
	}
}
