#include "iterativo.h"
#include "servidor_threads.h"
#include "servidor_fila.h"
#include <ctype.h>

int (*tecnicas[])(int) = {servidor_iterativo, servidor_threads, servidor_fila};

int main(int argc, char *argv[]) {
	//Pegando as flags
	in_addr_t endereco = INADDR_ANY;
	in_port_t porta = PORTA;
	int backlog = 0;
	gverbose = 0;
	int modo = -1;

	int opcao;
	opterr = 0;
	while ((opcao = getopt(argc, argv, "ve:p:b:m:")) != -1) {
		switch (opcao) {
			case 'v':
				gverbose = 1;
				break;
			case 'e':
				inet_pton(AF_INET, optarg, &endereco);
				break;
			case 'p':
				porta = atoi(optarg);
				break;
			case 'b':
				backlog = atoi(optarg);
				break;
			case 'm':
				modo = atoi(optarg);
				break;
			case '?':
				if (optopt == 'e' || optopt == 'p') {
					fprintf(stderr, "Opção -%c precisa do endereço do servidor.\n", optopt);
				} else if (isprint (optopt)) {
					fprintf(stderr, "Opção -%c desconhecida.\n", optopt);
				} else {
					fprintf(stderr, "Caractere '\\x%x' de opção desconhecido.\n", optopt);
				}
				exit(1);
		}
	}
	
	if (modo == -1) {
		printf("É preciso especificar o modo: -m MODO\n");
		exit(1);
	}

	//Cria o socket.
	int sfd = criar_socket_servidor(endereco, porta, backlog);

	system("ifconfig");

	tecnicas[modo](sfd);
	return 0;
}
