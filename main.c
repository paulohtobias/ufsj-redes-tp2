#include "servidor_iterativo.h"
#include "servidor_threads.h"
#include "servidor_fila.h"
#include "servidor_concorrente.h"
#include "opcoes.h"

int (*tecnicas[])(int) = {servidor_iterativo, servidor_threads, servidor_fila, servidor_concorrente};

int main(int argc, char *argv[]) {
	//Pegando as flags
	in_port_t porta;
	int backlog;
	int modo;

	opcao_t opcoes[] = {
		OPCAO_INIT('v', tipo_bool, &gverbose, "0", "Imprime várias informações na tela sobre o estado do servidor"),
		OPCAO_INIT('p', tipo_int, &porta, "PORTA=2222", "Servidor escutará requisições http na porta %s"),
		OPCAO_INIT('b', tipo_int, &backlog, "BACKLOG=0", "Quantidade de conexões enfileiradas"),
		OPCAO_INIT('m', tipo_int, &modo, "MODO", "0: servidor iterativo\n1: servidor com threads\n2: servidor com fila de requisições\n3: servidor não bloqueante"), //to-do: melhorar a descrição
		OPCAO_INIT('r', tipo_str(PATH_MAX), raiz_site, "DIR=.", "Define o diretório raiz do servidor"),
		OPCAO_INIT('i', tipo_str(128), pagina_inicial, "ARQ=", "Define qual a página inicial do site caso este seja acessado diretamente. index.php e index.html serão tentados automaticamente")
	};

	parse_args(argc, argv, opcoes, sizeof opcoes / sizeof(opcao_t));

	//Checagem de erro
	if (modo < 0 || modo > 3) {
		fprintf(stderr, "modo deve estar entre 0 e 3.\n");
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
	
	if (gverbose) {
		printf("Página inicial: '%s'\n", pagina_inicial);
	}

	//Cria o socket.
	int sfd = criar_socket_servidor(porta, backlog);

	tecnicas[modo](sfd);
	return 0;
}
