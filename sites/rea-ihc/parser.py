# -*- coding: utf-8 -*-

import os
import sys
import subprocess
from time import sleep
import re
import json
from parser_regex import *

def parse_valor(valor):
	"""
	Retorna uma string correta do valor de uma variável.

	Key
	"""
	#Tenta extrair um char.
	match = char_pattern.search(valor)
	if match:
		return match.group("valor")
	else:
		#Tenta extrair um vetor.
		if valor.startswith("{"):
			return valor.strip().replace("{", "[").replace("}", "]").replace(", ", ",")
	
	return valor


i = 0
def pbuff(buff, sep = '=', l = 20):
	global i
	print(int(l/2) * sep + " " + str(i) + " " + int(l/2) * sep + "\n" + buff + "\n" + sep * l)
	i += 1

def main():
	#Entrada
	executavel = sys.argv[1]

	pipe_pai = os.pipe()
	pipe_filho = os.pipe()

	pid = os.fork()
	if(pid == -1):
		print("Erro no fork")
		exit(1)
	
	if(pid == 0):
		#Processo filho. Vai ser o gdb.
		#Escreve em pipe_pai[1] e lê em pipe_filho[0].

		#stdin -> pipe_filho[0]
		os.close(pipe_filho[1])
		os.dup2(pipe_filho[0], 0)

		#stdout -> pipe_pai[1]
		os.close(pipe_pai[0])
		os.dup2(pipe_pai[1], 1)

		err = open("/dev/null", "w")
		os.dup2(err.fileno(), 2)

		os.execl("/usr/bin/gdb", "/usr/bin/gdb", "-silent", executavel)
	else:
		#Processo pai. Vai enviar comandos pro gdb.
		#Escreve em pipe_filho[1] e lê em pipe_pai[0].

		os.close(pipe_filho[0])
		os.close(pipe_pai[1])

		pin = pipe_pai[0]
		pout = pipe_filho[1]

		def interage(comando, tempo_espera = 0.06, buffer_size = 2048, prep = True):
			"""Função que envia comando(s) para o gdb e retorna a saída."""
			os.write(pout, (comando + "\n").encode())
			sleep(tempo_espera)
			if(buffer_size > 0):
				buff = os.read(pin, buffer_size).decode("utf8")
				if prep:
					return buff.replace("(gdb) ", "")
				return buff
		
		def get_valor_no_endereco(nome, size = 0):
			"""Retorna o valor que está no endereço onde nome aponta.\n
			Em outras palavras: "print *nome".

			Keyword arguments:
			nome -- nome da variável que é um ponteiro
			size -- tamanho do ponteiro caso este seja um vetor (default = 0)
			"""
			comando = "p *" + nome
			if size > 0:
				comando += "@" + str(size)
			buff = interage(comando).strip()
			valor_endereco = ""
			if buff.startswith("$"):
				valor_endereco = buff[buff.find("=") + 2:]
			
			return valor_endereco
		
		def quit():
			"""Função que encerra o gdb."""
			return interage("q\n\y", buffer_size = 0)

		#Ignorando o lixo inicial.
		sleep(0.1)
		buff = os.read(pin, 2048)


		#----==== DADOS ====----
		codigo = {"funções": {}, "requisitos": [], "return status": {}}
		"""Dicionário de requisitos e funções"""

		endereco_limite = 0
		"""Guarda o endereço fim de uma função e o início da próxima."""

		#Salvam as instruções.
		instrucao = ""
		instrucao_nova = ""

		frames = {}
		"""Salva os valores reais das variáveis de cada função."""
		
		gponteiros = {}
		"""Tabela global de ponteiros."""
		#----===============----
		
		#Mostrando uma linha por list.
		interage("set listsize 1", 0)
		
		#Pegando os nomes das funções.
		buff = interage("info fun")

		results = re.finditer(re_funcao, buff)
		for match in results:
			funcao = match.group('funcao')

			#Procura pela linha da função.
			buff = interage("list " + funcao)

			#Caso a '{' esteja em outra linha.
			#while not funcao in buff:
			#	buff = interage("list -")
			
			#Insere a função no dicionário.
			codigo["funções"][funcao] = {}
			codigo["funções"][funcao]["início"] = 0
			codigo["funções"][funcao]["fim"] = 0
			codigo["funções"][funcao]["linha"] = pegar_linha_de_instrucao(buff)

			#A primeira instrução é a da função main.
			if funcao == "main":
				instrucao = buff.strip()

			#Insere o breakpoint na função.
			buff = interage("b " + funcao)
			
			print("Função '" + funcao + "' na linha: " + codigo["funções"][funcao]["linha"])
		
		buff = interage("r\ninfo locals\nend", 0.2)
		
		#Sempre inicia pela função main.
		funcao = "main"
		
		while (not ("__libc_start_main" in buff or "SIGSEGV" in buff)):
			#Separa a instrução em linha e código.
			isplit = instrucao.split("\t")
			instrucao_linha = isplit[0].strip()
			instrucao_codigo = isplit[-1].strip()

			#Salva a instrução
			instrucao_nova = instrucao_pattern.search(buff).group().strip()
			print(funcao + ": INSTRUÇÃO: '" + instrucao + "'")

			match = breakpoint_pattern.search(buff)
			if match:
				"""Procura por "Breakpoint", que é onde uma nova função é chamada.
				Percorre a lista das variáveis e gera o primeiro requisito da função.
				Identifica ponteiros para poder printar seus valores desreferenciados.
				"""
				#Pegando o nome da função.
				funcao_anterior = funcao
				funcao = match.group("funcao")

				#Encontra os parâmetros.
				buff = interage("info args")
				try:
					parametros = frames_pattern.search(buff).group().split("\n")
				except AttributeError:
					parametros = []
				
				#Encontra as variáveis locais.
				buff = interage("info locals")
				variaveis = frames_pattern.search(buff).group().split("\n")

				frames[funcao] = {}
				frames[funcao]["ponteiros"] = {}

				#Seta o endereço inicial da função.
				codigo["funções"][funcao]["início"] = endereco_limite

				#Novo requisito: inserir todas as variáveis locais na tabela.
				requisito = {}
				requisito["linha"] = codigo["funções"][funcao]["linha"]
				requisito["alterações"] = []

				if funcao != "main":
					#Identifica os parametros reais.
					#to-do: Fazer contagem de parenteses.
					parametros_reais = []
					pindicei = instrucao_codigo.find(funcao_anterior) + len(funcao_anterior) + 1
					pindicei += instrucao_codigo[pindicei:].find("(") + 1
					pindicef = pindicei + instrucao_codigo[pindicei:].find(")")

					parametros_reais = instrucao_codigo[pindicei:pindicef].replace(" ", "").split(",")
				
				pcount = 0
				for parametro in parametros:
					if "=" in parametro:
						endereco_limite += 1
						nome, valor = parametro.split(" = ")
						ponteiro = False

						#Zera o frame pra comparar com as próximas instruções.
						frames[funcao][nome] = valor

						#Se o parâmeto for um ponteiro, então atualize o valor.
						if valor.startswith("0x"):
							parametro_real = parametros_reais[pcount]
							if parametro_real.startswith("&"):
								ponteiro = True
								valor = funcao_anterior + ":" + parametro_real[1:]
								#Insere o ponteiro na lista global
								gponteiros[funcao + ":" + nome] = valor
							else:
								valor = get_valor_no_endereco(funcao_anterior + ":" + parametro_real)#frames[funcao_anterior][parametro_real]
								#Insere o ponteiro na lista global
								gponteiros[funcao + ":" + nome] = gponteiros[funcao_anterior + ":" + parametro_real]
								
								#Atualiza o valor no frame da função atual.
								frames[funcao][nome] = valor

							#Insere na lista de ponteiros da função
							valor_endereco = get_valor_no_endereco(nome)
							frames[funcao]["ponteiros"]["*" + nome] = {}
							frames[funcao]["ponteiros"]["*" + nome]["valor"] = valor_endereco
							frames[funcao]["ponteiros"]["*" + nome]["tamanho"] = 1
						
						alteracao = {}
						alteracao["Função"] = funcao
						alteracao["Nome da Variável"] = nome
						alteracao["Valor"] = valor
						alteracao["ponteiro"] = ponteiro

						requisito["alterações"].append(alteracao)

						pcount += 1

				for variavel in variaveis:
					if "=" in variavel:
						endereco_limite += 1
						nome, valor = variavel.split(" = ")

						#Zera o frame pra comparar com as próximas instruções.
						frames[funcao][nome] = valor

						#Verifica se é ponteiro, para observar o valor.
						if valor.startswith("0x"):
							valor = ""

							#Insere o ponteiro na lista global
							gponteiros[funcao + ":" + nome] = valor

							valor = get_valor_no_endereco(nome)

							#Insere na lista de ponteiros da função
							frames[funcao]["ponteiros"]["*" + nome] = {}
							frames[funcao]["ponteiros"]["*" + nome]["valor"] = valor
							frames[funcao]["ponteiros"]["*" + nome]["tamanho"] = 1


						alteracao = {}
						alteracao["Função"] = funcao
						alteracao["Nome da Variável"] = nome
						alteracao["Valor"] = ""
						alteracao["ponteiro"] = False

						requisito["alterações"].append(alteracao)
				
				print("BREAKPOINT")
				print(gponteiros)
				print(frames[funcao])
				input()

				#Seta o endereço final da função.
				codigo["funções"][funcao]["fim"] = endereco_limite
				
				#Insere o requisito na lista.
				codigo["requisitos"].append(requisito)
			
			else:
				if "vet[i]" in instrucao:
					print(frames[funcao])
					print("")
					input()
				#Se não entrou em uma nova função, é preciso verificar se
				#está saindo da função atual.
				match = retorno_pattern.search(buff)
				if match:
					#Atualiza o nome da função.
					funcao = match.group("funcao")
				else:
					#Se não for uma instrução de retorno de função.
					if "return" not in instrucao and instrucao.split("\t")[1].strip() != "}":
						buff = interage("info locals\ninfo args")

						linha = pegar_linha_de_instrucao(instrucao)

						variaveis = frames_pattern.search(buff).group().split("\n")

						requisito = {}
						requisito["linha"] = linha
						requisito["alterações"] = []
						
						#Guarda os vetores recentemente alocados.
						alocados = []
						#Procura por alterações nos valores das variáveis.
						for variavel in variaveis:
							if "=" in variavel:
								nome, valor = variavel.split(" = ")
								valor = parse_valor(valor)

								#Se o valor foi alterado, então gere um novo requisito.
								if valor != frames[funcao][nome]:
									#Atualiza a stack frame da função.
									frames[funcao][nome] = valor

									#Se for ponteiro, é preciso atualizar o valor
									ponteiro = False
									if valor.startswith("0x"):
										#1: (m|c|re)alloc
										#Para efeitos de abstração, alocações na heap não serão
										#representadas fielmente. Portanto, elas serão consideradas
										#como vetores normais da pilha.
										match = alloc_pattern.search(instrucao)
										if match:
											#Retira o ponteiro da lista.
											gponteiros.pop(funcao + ":" + nome, None)
											frames[funcao]["ponteiros"].pop("*" + nome)

											#Pega o tamanho
											tamanho = 1
											operadores = match.group("operadores").split("*")
											for operador in operadores:
												if "sizeof" not in operador:
													tamanho = operador.strip()
													try:
														tamanho = int(tamanho)
													except ValueError:
														buff = interage("p " + tamanho).strip()
														tamanho = int(parse_valor(buff.split(" = ")[1]))

											nome_ponteiro = "*" + nome + "@" + str(tamanho)
											
											valor = parse_valor(interage("p " + nome_ponteiro).strip().split(" = ")[1])

											#Atualiza na tabela de ponteiros da função.
											frames[funcao]["ponteiros"][nome_ponteiro] = {}
											frames[funcao]["ponteiros"][nome_ponteiro]["valor"] = valor
											frames[funcao]["ponteiros"][nome_ponteiro]["tamanho"] = tamanho

											alocados.append(nome_ponteiro)
										else:
											ponteiro = True

											#2: &
											# passa a regex de identifcador começando
											# em instrucao.find("&") e procura o cara no próprio frame.
											if "&" in instrucao:
												referencia = identificador_pattern.search(instrucao, instrucao.find("&")).group()
												
												#Atualiza o vetor global de ponteiros
												gponteiros[funcao + ":" + nome] = funcao + ":" + referencia

												valor = gponteiros[funcao + ":" + nome]

												#Atualiza a lista de ponteiros da função
												valor_endereco = get_valor_no_endereco(nome)
												frames[funcao]["ponteiros"]["*" + nome] = {}
												frames[funcao]["ponteiros"]["*" + nome]["valor"] = valor_endereco
												frames[funcao]["ponteiros"]["*" + nome]["tamanho"] = 1
											
											#3: atribuição de outro ponteiro.
											#Aqui é só um else: pega o identificador depois do '=' e procura
											#pra quem ele aponta na tabela global.
											else:
												novo_nome = identificador_pattern.search(instrucao, instrucao.find("=")).group()
												gponteiros[funcao + ":" + nome] = gponteiros[funcao + ":" + novo_nome]
												
												valor = gponteiros[funcao + ":" + novo_nome]

												#Atualiza a lista de ponteiros da função
												valor_endereco = get_valor_no_endereco(nome)
												frames[funcao]["ponteiros"]["*" + nome] = {}
												frames[funcao]["ponteiros"]["*" + nome]["valor"] = valor_endereco
												frames[funcao]["ponteiros"]["*" + nome]["tamanho"] = 1
									else:
										for gp in gponteiros.keys():
											if gponteiros[gp] == funcao + ":" + nome:
												fgp, ngp = gp.split(":")
												frames[fgp]["ponteiros"]["*" + ngp]["valor"] = valor

									alteracao = {}
									alteracao["Função"] = funcao
									alteracao["Nome da Variável"] = nome
									alteracao["Valor"] = parse_valor(valor)
									alteracao["ponteiro"] = ponteiro
									requisito["alterações"].append(alteracao)
						
						#Iterar por todos os ponteiros cujo valor não comece com '['
						#Em frames[funcao]["ponteiros"].
						#Chamar a função interagir("p " + frames[funcao]["ponteiros"][nome]).
						#Olhar a sáida e comparar com os dados que já tão lá. Se tiver mudado,
						#então gera um requisito, onde ponteiro=False, NomedaVariável=gponteiros[nome]
						print("ALOCADOS: " + str(alocados))
						print(frames[funcao]["ponteiros"])
						for ponteiro in frames[funcao]["ponteiros"].keys():
							valor_antigo = frames[funcao]["ponteiros"][ponteiro]["valor"]
							try:
								valor = parse_valor(interage("p " + ponteiro).strip().split(" = ")[-1])
								print("NOVO valor: " + valor)
								if valor != valor_antigo:
									#Atualiza o valor na lista de ponteiros.
									frames[funcao]["ponteiros"][ponteiro]["valor"] = valor

									nponteiro = ponteiro[1:]
									idx = nponteiro.find("@")
									if idx != -1:
										nponteiro = ponteiro[1:idx + 1]
										alteracao = {}
										alteracao["Função"] = funcao
										alteracao["Nome da Variável"] = nponteiro
										alteracao["Valor"] = valor
										alteracao["ponteiro"] = False
										requisito["alterações"].append(alteracao)
									else:
										afuncao, anome = gponteiros[funcao + ":" + nponteiro].split(":")
										alteracao = {}
										alteracao["Função"] = afuncao
										alteracao["Nome da Variável"] = anome
										alteracao["Valor"] = valor
										alteracao["ponteiro"] = False
										requisito["alterações"].append(alteracao)
							except:
								print(sys.exc_info()[0])
								exit(1)
								pass

						if len(requisito["alterações"]) > 0:
							codigo["requisitos"].append(requisito)
							#print(requisito)
							#input()
					
					print("INSTRUÇÃO NORMAL")
					print(gponteiros)
					print(frames[funcao])
					input()

			# print(gponteiros)
			# print("=" * 20)
			# print(frames["main"])
			# print("=" * 20)
			# print("")

			#input()

			buff = interage("n")
			instrucao = instrucao_nova
		
		#to-do: adicionar elifs com outros erros.
		if "SIGSEGV" in buff:
			print("FALHA DE SEGMENTAÇÃO NA LINHA " + pegar_linha_de_instrucao(instrucao_nova))
			codigo["return status"]["sucesso"] = False
			codigo["return status"]["motivo"] = "Falha de Segmentação na linha " + pegar_linha_de_instrucao(instrucao_nova)
		else:
			codigo["return status"]["sucesso"] = True
		
		quit()

		with open(executavel + ".json", "w") as f:
			f.write(json.dumps(codigo, sort_keys=True))
			print(json.dumps(codigo,indent=4, ensure_ascii=False))

		os.wait()
		exit(0)


if __name__ == "__main__":
	main()
