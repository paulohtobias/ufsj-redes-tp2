#include "servidor_concorrente.h"

int servidor_concorrente(int sfd) {
	if (!gquiet) {
		puts("######## MODO SERVIDOR CONCORRENTE ########");
		printf("\033[0;32m");
	}
	
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
			handle_error(errno, "select");
		}

		if (!gquiet) {
			printf("select\n");
		}

		for (i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &read_fds)) {
				if (i == sfd) {
					cliente_sfd = servidor_accept(sfd);

					if (cliente_sfd != -1) {
						FD_SET(cliente_sfd, &master);
						if (cliente_sfd > fdmax) {
							fdmax = cliente_sfd;
						}
						if (!gquiet) {
							printf("===============================================\nConexão aceita: %d\n", cliente_sfd);
						}
					}
				} else {
					if (!gquiet) {
						printf("Tratando %d\n", i);
					}
					servidor_processar_conexao_simples(i);
					FD_CLR(i, &master);
				}
			}
		}
	}
}
