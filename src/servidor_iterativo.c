#include "servidor_iterativo.h"

int servidor_iterativo(int sfd) {
	int cliente_sfd;

	if (!gquiet) {
		puts("######## MODO SERVIDOR ITERATIVO ########");
		printf("\033[0;32m");
	}

	while (1) {
		//Aceita a conexão.
		if (!gquiet) {
			printf("Esperando conexão...\n");
		}
		cliente_sfd = servidor_accept(sfd);

		if (cliente_sfd != -1) {
			if (!gquiet) {
				printf("===============================================\nConexão aceita: %d\n", cliente_sfd);
			}

			servidor_processar_conexao_simples(cliente_sfd);
		}
	}
}
