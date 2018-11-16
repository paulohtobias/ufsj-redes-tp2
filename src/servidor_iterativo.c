#include "servidor_iterativo.h"

int servidor_iterativo(int sfd) {
	int cliente_sfd;

	if (gverbose) {
		puts("######## MODO SERVIDOR ITERATIVO ########");
		printf("\033[0;32m");
	}

	while (1) {
		//Aceita a conexão.
		if (gverbose) {
			printf("Esperando conexão...\n");
		}
		cliente_sfd = servidor_accept(sfd);

		if (gverbose) {
			printf("===============================================\nConexão aceita: %d\n", cliente_sfd);
		}

		servidor_processar_conexao_simples(cliente_sfd);
	}
}
