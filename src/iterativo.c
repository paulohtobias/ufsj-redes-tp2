#include "iterativo.h"

int servidor_iterativo(int sfd) {
	while (1) {
		//Aceita a conexão.
		int *cliente_sfd = malloc(sizeof(int));
		*cliente_sfd = servidor_accept(sfd);

		if (gverbose) {
			printf("Conexão aceita: %d\n", *cliente_sfd);
		}

		servidor_processar_conexao(cliente_sfd);
	}
}
