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

void *servidor_processar_conexao(void *args) {
	int cliente_sfd = *((int *) args);
	free(args);

	char buff[BUFF_LEN] = {0};
	int retval;

	//TO-DO: temporário.
	char resposta[] =
		//Cabeçalho
		"HTTP/1.1 200 OK\r\n\r\n"

		//Dados
		"<html>"
			"<head>"
			"<meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\"/>"
				"<title>Página Inicial do meu Site</title>"
			"</head>"
			"<body>"
				"<h1>Título</h1>"
					"<h2>Sub-título</h2>"
						"<h3>blablabla</h3>"
			"</body>"
		"</html>"
	;

	//TO-DO: talvez fazer um loop pra caso o cliente faça vários pedidos?

	while (1) {
		//Lê o pedido do cliente.
		retval = read(cliente_sfd, buff, BUFF_LEN - 1);
		if (retval == -1) {
			handle_error(1, "servidor_processar_conexao-read");
		} else if (retval == 0) {
			return NULL;
		}

		//Mostra a requisição na tela.
		if (gverbose) {
			printf("Reqest:\n<%s>\n", buff);
		}

		//TO-DO: Faz um parse e processa o pedido.

		//Envia resposta ao cliente.
		write(cliente_sfd, resposta, strlen(resposta) + 1);

		close(cliente_sfd);
		break;
	}

	return NULL;
}
