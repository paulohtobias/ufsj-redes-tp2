#include "servidor_fila.h"

///Fila de espera.
int *gfila = NULL;
int tamanho_fila_max = 256;

///Threads
pthread_t *threads_fila;

///Variáveis de condição.
pthread_mutex_t mutex_fila = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_fila = PTHREAD_COND_INITIALIZER;
int tamanho_fila = 0;

void *thread_fila(void *args) {
	int cliente_sfd;
	while (1) {
		//Espera ter pedidos na fila.
		pthread_mutex_lock(&mutex_fila);
		while (tamanho_fila == 0) {
			pthread_cond_wait(&cond_fila, &mutex_fila);
		}

		//Retira um pedido da fila.
		cliente_sfd = gfila[--tamanho_fila];
		pthread_mutex_unlock(&mutex_fila);

		if (!gquiet) {
			printf("===============================================\nConexão aceita: %d\n", cliente_sfd);
		}

		servidor_processar_conexao_simples(cliente_sfd);
	}
}

int servidor_fila(int sfd) {
	int i;
	if (!gquiet) {
		puts("######## MODO SERVIDOR FILA ########");
		printf("\033[0;32m");
	}

	//Cria as threads.
	threads_fila = malloc(threads_fila_qtd * sizeof(pthread_t));
	for (i = 0; i < threads_fila_qtd; i++) {
		pthread_create(threads_fila + i, NULL, thread_fila, NULL);
	}

	//Aloca memória para a fila.
	gfila = malloc(tamanho_fila_max * sizeof(int));

	int cliente_sfd;
	while (1) {
		//Aceita a conexão.
		if (!gquiet) {
			printf("Esperando conexão...\n");
		}
		cliente_sfd = servidor_accept(sfd);

		if (cliente_sfd != -1) {
			pthread_mutex_lock(&mutex_fila);
			gfila[tamanho_fila++] = cliente_sfd;
			//Sinaliza as threads que há um novo pedido.
			if (tamanho_fila == 1) {
				pthread_cond_signal(&cond_fila);
			} else if (tamanho_fila == tamanho_fila_max) {
				tamanho_fila_max *= 2;
				gfila = realloc(gfila, tamanho_fila_max * sizeof(int));
				if (gfila == NULL) {
					fprintf(stderr, "AVISO: Não foi possível aumentar o tamanho da fila para %d.\n", tamanho_fila_max);
					exit(1);
				}
			}
			pthread_mutex_unlock(&mutex_fila);
		}
	}

	for (i = 0; i < threads_fila_qtd; i++) {
		pthread_join(threads_fila[i], NULL);
	}
}
