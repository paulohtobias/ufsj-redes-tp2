#include "servidor_threads.h"

void *servidor_threads_processar_conexao(void *args) {
	int cliente_sfd = *((int *) args);
	free(args);

	servidor_processar_conexao_simples(cliente_sfd);

	pthread_exit(NULL);
}

int servidor_threads(int sfd) {
	int retval;

	if (gverbose) {
		puts("######## MODO SERVIDOR THREADS ########");
		printf("\033[0;32m");
	}

	while (1) {
		//Aceita a conexão.
		if (gverbose) {
			printf("Esperando conexão...\n");
		}
		int *cliente_sfd = malloc(sizeof(int));
		*cliente_sfd = servidor_accept(sfd);

		if (*cliente_sfd != -1) {
			if (gverbose) {
				printf("===============================================\nConexão aceita: %d\n", *cliente_sfd);
			}

			pthread_t thread;
			retval = pthread_create(&thread, NULL, servidor_threads_processar_conexao, cliente_sfd);
			pthread_detach(thread);
			if (retval != 0) {
				handle_error(retval, "servidor_threads-pthread_create");
			}
		} else {
			free(cliente_sfd);
		}
	}
}
