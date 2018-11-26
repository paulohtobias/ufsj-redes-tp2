# Segundo Trabalho Prático de Redes

## João Vitor Gonçalves e Paulo Henrique Tobias

- **Compilar:**
	- Para compilar: `make`

- **Executar:**
	- Para executar o servidor: `./servidor`
	- *Flags* suportadas pelo sistema:
		- `-q`	Suprime todas as mensagens da saída padrão.
		- `-p`	`PORTA`	Servidor escutará requisições http na porta `PORTA`. Padrão: 2222
		- `-b`	`BACKLOG`	Quantidade de conexões enfileiradas. Padrão: 5
		- `-m`	`MODO`
			- 0: servidor iterativo
			- 1: servidor com threads
			- 2: servidor com fila de requisições
			- 3: servidor não bloqueante.
		- `-t`	`TIMEOUT`	Define o timeout para conexões inativas. Padrão: 5
		- `-r`	`DIR`	Define o diretório raiz do servidor. Padrão: 'sites/site-simples'
		- `-i`	`ARQ`	Define qual a página inicial do site caso este seja acessado diretamente. index.php e index.html serão tentados automaticamente. Padrão: ''
		- `-c`	`THREADS`	Quantidade de threads consimidoras quando `MODO`=2. Padrão: 4
		- `-l`	`LIMITE`	Altera a quantidade máxima de descritores de arquivo abertos para o processo. Padrão: 1024
