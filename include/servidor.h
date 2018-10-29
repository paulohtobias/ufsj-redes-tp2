#ifndef SERVIDOR
#define SERVIDOR

#define handle_error(cod, msg)\
	perror(msg); exit(cod);

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <pthread.h>
#include <unistd.h>
#include <stdarg.h>
#include <arpa/inet.h>
#include "utils.h"

#define PORTA 2222
#define DEFAULT_BACKLOG 5
#define BUFF_LEN 2048

extern int gverbose;

///Cria um novo socket para servidor em modo listen.
int criar_socket_servidor(in_addr_t endereco, in_port_t porta, int backlog);

///Cria uma nova conexão TCP e retorna o socket.
int servidor_accept(int ssfd);

void servidor_processar_conexao_simples(int cliente_sfd);

char *servidor_processar_pedido(const char *pedido, int tamanho_pedido, int *tamanho_resposta);

#endif //SERVIDOR
