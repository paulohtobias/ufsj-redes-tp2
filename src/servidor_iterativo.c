#include "servidor_iterativo.h"

int servidor_iterativo(int sfd) {
	int cliente_sfd;

	puts("######## MODO SERVIDOR ITERATIVO ########");
	printf("\033[0;31m");
	
	while (1) {
		//Aceita a conexão.
		cliente_sfd = servidor_accept(sfd);

		if (gverbose) {
			printf("Conexão aceita: %d\n", cliente_sfd);
		}

		servidor_processar_conexao_simples(cliente_sfd);
	}
}
