#include "servidor_iterativo.h"
#include "servidor_threads.h"
#include "servidor_fila.h"
#include "servidor_concorrente.h"
#include <ctype.h>

int (*tecnicas[])(int) = {servidor_iterativo, servidor_threads, servidor_fila, servidor_concorrente};

int main(int argc, char *argv[]) {
	//Pegando as flags
	in_addr_t endereco = INADDR_ANY;
	in_port_t porta = PORTA;
	int backlog = 0;
	gverbose = 0;
	int modo = -1;
	strcpy(raiz_site, ".");
	strcpy(pagina_inicial, "");

	int opcao;
	opterr = 0;
	while ((opcao = getopt(argc, argv, "ve:p:b:m:r:h:")) != -1) {
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
			case 'r':
				strncpy(raiz_site, optarg, PATH_MAX);
				break;
			case 'h':
				strncpy(pagina_inicial, optarg, 128);
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
		fprintf(stderr, "É preciso especificar o modo: -m MODO\n");
		exit(1);
	}

	//Exibe o ip na tela.
	if (gverbose) {
		system("ifconfig");
	}

	//Caminho absoluto do wrapper para executar o php.
	if (getcwd(raiz, PATH_MAX - 64) == NULL) {
		handle_error(errno, "getcwd");
	}

	//Muda o diretório de trabalho.
	if (chdir(raiz_site) == -1) {
		handle_error(errno, "chdir");
	}
	if (gverbose) {
		printf("Diretório atual: '%s'\n", raiz_site);
	}
	
	system("pwd");
	printf("path_max: %d\n", PATH_MAX);

	//Cria o socket.
	int sfd = criar_socket_servidor(endereco, porta, backlog);

	tecnicas[modo](sfd);
	return 0;
}
