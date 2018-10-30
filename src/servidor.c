#include "servidor.h"

int gverbose = 1;

int criar_socket_servidor(in_addr_t endereco, in_port_t porta, int backlog) {
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
	server_addr.sin_addr.s_addr = endereco;
	server_addr.sin_port = htons(porta);

	retval = bind(sfd, (struct sockaddr *) &server_addr, sizeof(server_addr));
	if (retval == -1) {
		handle_error(retval, "criar_socket_servidor-bind");
	}

	if (backlog < 1) {
		backlog = DEFAULT_BACKLOG;
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
		close(cliente_sfd);
		return;
	}

	//Mostra a requisição na tela.
	if (gverbose) {
		printf("\033[0mReqest (%d):\n<%s>\033[0;31m\n", retval, buff);
	}

	resposta = servidor_processar_pedido(buff, retval, &tamanho_resposta);

	//Envia resposta ao cliente.
	write(cliente_sfd, resposta, tamanho_resposta);
	free(resposta);

	//Fecha a conexão com o cliente
	close(cliente_sfd);
}

char *servidor_processar_pedido(const char *pedido, int tamanho_pedido, int *tamanho_resposta) {
	//TO-DO: temporário.
	char resposta_cabecalho[] =
		"HTTP/1.1 200 OK\r\n\r\n";
	int tamanho_cabecalho = strlen(resposta_cabecalho);

	char caminho[256] = "testes";
	sscanf(&pedido[4], "%s", caminho + strlen(caminho));

	if (strcmp(caminho, "testes/") == 0) {
		strcat(caminho, "index.html");
	}

	uint8_t *dados = carregar_arquivo(caminho, tamanho_resposta);

	char *resposta = malloc(tamanho_cabecalho + (*tamanho_resposta));
	strcpy(resposta, resposta_cabecalho);
	memcpy(resposta + tamanho_cabecalho, dados, *tamanho_resposta);

	free(dados);

	(*tamanho_resposta) += tamanho_cabecalho;

	return resposta;
}
