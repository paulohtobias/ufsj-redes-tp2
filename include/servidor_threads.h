#ifndef SERVIDOR_THREADS_H
#define SERVIDOR_THREADS_H

#include "servidor.h"

void *servidor_threads_processar_conexao(void *args);

int servidor_threads(int sfd);

#endif //SERVIDOR_THREADS_H