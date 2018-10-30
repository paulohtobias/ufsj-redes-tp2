#ifndef SERVIDOR_CONCORRENTE_H
#define SERVIDOR_CONCORRENTE_H

#include "servidor.h"
#include <sys/select.h>

int servidor_concorrente(int sfd);

#endif //SERVIDOR_CONCORRENTE_H
