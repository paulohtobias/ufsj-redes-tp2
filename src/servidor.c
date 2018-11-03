#include "servidor.h"

int gverbose = 1;

int criar_socket_servidor(in_port_t porta, int backlog) {
	int retval;
	
	int sfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sfd == -1) {
		handle_error(sfd, "criar_socket_servidor-socket");
	}

	int enable = 1;
	retval = setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
	if (retval < 0) {
		handle_error(retval, "criar_socket_servidor-setsockopt(SO_REUSEADDR)");
	}

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(porta);

	retval = bind(sfd, (struct sockaddr *) &server_addr, sizeof(server_addr));
	if (retval == -1) {
		handle_error(retval, "criar_socket_servidor-bind");
	}

	retval = listen(sfd, backlog);
	if (retval == -1) {
		handle_error(retval, "criar_socket_servidor-listen");
	}

	return sfd;
}

int servidor_accept(int ssfd) {
	int client_socket_fd;
	socklen_t client_len;
	struct sockaddr_in client_addr;

	client_len = sizeof client_addr;
	client_socket_fd = accept(ssfd, (struct sockaddr *) &client_addr, &client_len);
	if (client_socket_fd == -1) {
		handle_error(client_socket_fd, "servidor_accept-accept");
	}

	return client_socket_fd;
}

void servidor_processar_conexao_simples(int cliente_sfd) {
	char buff[BUFF_LEN] = {0};
	char *resposta = NULL;
	int tamanho_resposta;
	int retval;

	//Lê o pedido do cliente.
	if (gverbose) {
		printf("Esperando pedido...\n");
	}
	retval = read(cliente_sfd, buff, BUFF_LEN - 1);

	if (retval == -1) {
		close(cliente_sfd);
		handle_error(1, "iterativo_processar_conexao-read");
	} else if (retval == 0) {
		printf("Cliente fechou a conxão\n");
		close(cliente_sfd);
		return;
	}

	//Mostra a requisição na tela.
	if (gverbose) {
		printf("\033[0m%d: Reqest (%d):\n<%s>\033[0;31m\n", cliente_sfd, retval, buff);
	}

	resposta = servidor_processar_pedido(buff, retval, &tamanho_resposta);

	//Envia resposta ao cliente.
	write(cliente_sfd, resposta, tamanho_resposta);
	free(resposta);

	//Fecha a conexão com o cliente
	close(cliente_sfd);
}

char *servidor_processar_pedido(const char *pedido, int tamanho_pedido, int *tamanho_resposta) {
	char caminho[PATH_MAX] = ".";
	char *metodo = NULL;
	char *argumentos = "";
	
	int caminho_tamanho = 1;
	int metodo_tamanho = 0;
	int argumentos_tamanho = 0;
	
	int i;
	//Método GET.
	if (pedido[0] == 'G') {
		metodo = "GET";
		metodo_tamanho = 3;

		//Identifica o arquivo.
		for (i = 0; pedido[i + metodo_tamanho + 1] != ' '; i++) {
			caminho[i + 1] = pedido[i + metodo_tamanho + 1];
		}
		caminho[i + 1] = '\0';
		
		//Pegando valores na url, caso exista algum.
		for (i = 0; caminho[i] != '\0' && caminho[i] != '&'; i++);

		//Verifica se há argumentos na url.
		if (caminho[i] == '&') {
			caminho[i] = '\0';
			argumentos = &caminho[i + 1];

			argumentos_tamanho = strlen(argumentos);
		}
		caminho_tamanho = i;
	} else {
		metodo = "POST";
		metodo_tamanho = 4;

		//Identifica o arquivo.
		for (i = 0; pedido[i + metodo_tamanho + 1] != ' '; i++) {
			caminho[i + 1] = pedido[i + metodo_tamanho + 1];
		}
		caminho[i + 1] = '\0';
		caminho_tamanho = strlen(caminho);
		
		//Pegando os argumentos vindos do POST.
		argumentos = strstr(pedido, "\r\n\r\n") + 4;
		argumentos_tamanho = strlen(argumentos);
	}

	//Verifica se é a página inicial.
	if (strcmp(caminho, "./") == 0) {
		//Se o usuário especificou uma página inicial, então ela será usada.
		if (pagina_inicial[0] != '\0') {
			strcat(caminho, pagina_inicial);
		} else {
			//Caso contrário, tenta abrir primeiro index.php e depois index.html
			if(access("index.php", F_OK) != -1) {
				strcat(caminho, "index.php");
			} else {
				strcat(caminho, "index.html");
			}
		}
	}

	if (gverbose) {
		printf("Arquivo: '%s'\nMetodo: '%s'\nArgumentos: '%s'\n", caminho, metodo, argumentos);
	}

	FILE *in = NULL;
	uint8_t *dados = NULL;
	if (access(caminho, F_OK) != -1) {
		//Verifica se é uma página html. Nesse caso, é preciso executar o php.
		if (strstr(pedido, "text/html") != NULL) {
			in = servidor_executar_php(caminho, metodo, argumentos, caminho_tamanho + metodo_tamanho + argumentos_tamanho);
		} else {
			//Se não for, basta abrir o arquivo normalmente.
			in = fopen(caminho, "r");
		}

		//Carrega o arquivo.
		dados = carregar_arquivo(in, tamanho_resposta);
	}
	
	//Monta o cabeçalho.
	char resposta_cabecalho[2048];
	if (dados != NULL) {
		strcpy(resposta_cabecalho, "HTTP/1.0 200 OK\r\n");
	} else {
		strcpy(resposta_cabecalho, "HTTP/1.0 404 Not Found\r\n");

		sprintf(caminho, "%s/404.html", raiz);
		in = fopen(caminho, "r");
		dados = carregar_arquivo(in, tamanho_resposta);
	}

	//Content-Length.
	char content_length[32];
	sprintf(content_length, "Content-Length: %d\r\n", *tamanho_resposta);
	strcat(resposta_cabecalho, content_length);

	//Fim do cabeçalho.
	strcat(resposta_cabecalho, "\r\n");

	int tamanho_cabecalho = strlen(resposta_cabecalho);
	
	char *resposta = malloc(tamanho_cabecalho + (*tamanho_resposta));
	strcpy(resposta, resposta_cabecalho);
	memcpy(resposta + tamanho_cabecalho, dados, *tamanho_resposta);

	free(dados);

	(*tamanho_resposta) += tamanho_cabecalho;

	return resposta;
}

//TO-DO: fazer um pipe+fork e trocar o stdout do filho (php) pelo fd do arquivo temporário.??
FILE *servidor_executar_php(const char *caminho, const char *metodo, const char *argumentos, int tamanho_comando) {
	char comando[PATH_MAX + tamanho_comando + 20];

	//Cria um arquivo temporário.
	FILE *ftemp = tmpfile();
	if (ftemp == NULL) {
		handle_error(errno, "servidor_executar_php-tmpfile");
	}

	//Executa o php
	sprintf(comando, "php %s/wrapper.php %s %s?%s", raiz, metodo, caminho, argumentos);
	FILE *pin = popen(comando, "r");

	//Copia a saída do php para o arquivo temporário.
	char buff[BUFF_LEN];
	int nbytes;
	while (!feof(pin)) {
		nbytes = fread(buff, 1, BUFF_LEN, pin);
		fwrite(buff, 1, nbytes, ftemp);
	}
	pclose(pin);

	rewind(ftemp);
	
	return ftemp;
}
