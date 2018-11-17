#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <errno.h>

#define handle_error(cod, msg)\
	fprintf(stderr, "%d: ", errno); perror(msg); exit(cod);

int diretorio(const char *caminho);

void *carregar_arquivo(FILE *in, int *tamanho_arquivo);

void exibir_limite_arquivos();

void definir_limite_arquivos(rlim_t limite);

#endif //UTILS_H