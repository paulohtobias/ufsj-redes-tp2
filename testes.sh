#!/bin/bash

modo=$1
ip="$(ifconfig wlp3s0 | grep 'inet end.: ' | cut -d ':' -f 2 | cut -d ' ' -f 2)"

modos[0]="iterativo"
modos[1]="threads"
modos[2]="fila"
modos[3]="concorrente"

modo_str=${modos[$modo]}

#usuários: 1-1000 passo 10
base=10
topo=20
passo=10

#tempo: 20s
tempo=20S

#threads fila: 2 - 1024 paso 2^x

saida=log/saida_$modo_str.log

executar_teste()
{
	#Variáveis
	usuarios=$1
	saida_erros=log/erros/$modo_str-$usuarios
	porta=$((usuarios+1100))
	url="$ip:$porta"

	echo "URL: $url"
	# Abrindo o servidor
	./servidor -m $modo -p $porta -r sites/site-simples/ &
	spid=$!

	# Rodando o siege
	echo Testando com $usuarios usuarios
	siege -c $usuarios -t $tempo --log=$saida --mark="$usuarios;$tempo" $url > /dev/null 2>$saida_erros
	es=$?
	echo "Saida $usuarios: $es"
	if [ $es -ne 0 ]; then
		echo "erro no siege com $i usuários"
		echo "erro no siege com $i usuários" >> $saida_erros
		exit
	fi
	kill $spid
}

# Testes
executar_teste 1
for i in $(seq $base $passo $topo); do
	executar_teste $i
done
