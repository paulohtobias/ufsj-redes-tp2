#ifndef SERVIDOR_FILA_H
#define SERVIDOR_FILA_H

#include "servidor.h"

///Quantidade de threads.
int threads_fila_qtd;

void *thread_fila(void *args);

int servidor_fila(int sfd);

#endif //SERVIDR_FILA_H