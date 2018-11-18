#include "servidor_iterativo.h"
#include "servidor_threads.h"
#include "servidor_fila.h"
#include "servidor_concorrente.h"
#include "opcoes.h"
#include <ifaddrs.h>

int (*tecnicas[])(int) = {servidor_iterativo, servidor_threads, servidor_fila, servidor_concorrente};

int main(int argc, char *argv[]) {
	//Pegando as flags
	in_port_t porta;
	int backlog;
	int modo;

	opcao_t opcoes[] = {
		OPCAO_INIT('q', tipo_bool, &gquiet, "0", "Suprime todas as mensagens da saída padrão"),
		OPCAO_INIT('p', tipo_int, &porta, "PORTA="PORTA, "Servidor escutará requisições http na porta %s"),
		OPCAO_INIT('b', tipo_int, &backlog, "BACKLOG="DEFAULT_BACKLOG, "Quantidade de conexões enfileiradas"),
		OPCAO_INIT('m', tipo_int, &modo, "MODO", "0: servidor iterativo\n1: servidor com threads\n2: servidor com fila de requisições\n3: servidor não bloqueante"),
		OPCAO_INIT('t', tipo_int, &gtimeout, "TIMEOUT=5", "Define o timeout para conexões inativas"),
		OPCAO_INIT('r', tipo_str(PATH_MAX), raiz_site, "DIR=.", "Define o diretório raiz do servidor"),
		OPCAO_INIT('i', tipo_str(128), pagina_inicial, "ARQ=", "Define qual a página inicial do site caso este seja acessado diretamente. index.php e index.html serão tentados automaticamente"),
		OPCAO_INIT('c', tipo_int, &threads_fila_qtd, "THREADS=4", "Quantidade de threads consimidoras quando MODO=2"),
		OPCAO_INIT('l', tipo_int, &gmax_fds, "LIMITE=1024", "Altera a quantidade máxima de descritores de arquivo abertos para o processo")
	};

	parse_args(argc, argv, opcoes, sizeof opcoes / sizeof(opcao_t));

	//Checagem de erro
	if (modo < 0 || modo > 3) {
		fprintf(stderr, "modo deve estar entre 0 e 3.\n");
		exit(1);
	}

	//Muda o diretório de trabalho.
	if (chdir(raiz_site) == -1) {
		handle_error(errno, "chdir");
	}
	if (!gquiet) {
		printf("Diretório atual: '%s'\n", raiz_site);
	}
	
	if (!gquiet) {
		printf("Página inicial: '%s'\n", pagina_inicial);
	}

	//Altera a quantidade máxima de arquivos abertos.
	definir_limite_arquivos(gmax_fds);
	if (!gquiet) {
		exibir_limite_arquivos();
	}

	//Cria o socket.
	int sfd = criar_socket_servidor(porta, backlog);

	//Exibe o ip na tela.
	if (!gquiet) {
		puts("Escutando em:");
		struct ifaddrs *ifaddr, *tmp;
		getifaddrs(&ifaddr);
		for (tmp = ifaddr; tmp != NULL; tmp = tmp->ifa_next) {
			if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_INET) {
				struct sockaddr_in *addr = (struct sockaddr_in *) tmp->ifa_addr;
				printf("%s: http://%s:%d\n", tmp->ifa_name, inet_ntoa(addr->sin_addr), porta);
			}
		}
		freeifaddrs(ifaddr);
	}

	tecnicas[modo](sfd);
	return 0;
}
