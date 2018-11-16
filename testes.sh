#!/bin/bash

url=$1
saida_sufixo=$2

#usuários: 1-1000 passo 10
base=10
topo=1000
passo=10

#tempo: 20s
tempo=20S

#threads fila: 2 - 1024 paso 2^x

usuarios=1
saida=log/saida_$saida_sufixo.log
saida_erros=log/erros/$saida_sufixo-$usuarios

siege -c $usuarios -t $tempo --log=$saida --mark="$usuarios;$tempo" $url > /dev/null 2>$saida_erros
for i in $(seq $base $passo $topo); do
	usuarios=$i
	saida_erros=log/erros/$saida_sufixo-$usuarios
	echo Testando com $usuarios usuarios
	siege -c $usuarios -t $tempo --log=$saida --mark="$usuarios;$tempo" $url > /dev/null 2>$saida_erros
	es=$?
	if [ $es -ne 0 ]; then
		echo "erro no siege com $i usuários"
		echo "erro no siege com $i usuários" >> $saida_erros
		break
	fi
done
