#include "servidor_concorrente.h"

int servidor_concorrente(int sfd) {
	puts("######## MODO SERVIDOR THREADS ########");
	printf("\033[0;31m");
	
	fd_set master;
	fd_set read_fds;

	int fdmax;
	int cliente_sfd;

	int i;

	FD_ZERO(&master);
	FD_ZERO(&read_fds);

	FD_SET(sfd, &master);

	fdmax = sfd;

	while (1) {
		read_fds = master;

		if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
			handle_error(1, "select");
		}
		printf("Server-select() is OK...\n");

		for (i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &read_fds)) {
				if (i == sfd) {
					cliente_sfd = servidor_accept(sfd);

					FD_SET(cliente_sfd, &master);
					if (cliente_sfd > fdmax) {
						fdmax = cliente_sfd;
					}
					printf("New connection on socket %d\n", cliente_sfd);
				} else {
					printf("Tratando %d\n", i);
					servidor_processar_conexao_simples(i);
					FD_CLR(i, &master);
				}
			}
		}
	}
}
