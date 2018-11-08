#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>

int diretorio(const char *caminho);

void *carregar_arquivo(FILE *in, int *tamanho_arquivo);

#endif //UTILS_H