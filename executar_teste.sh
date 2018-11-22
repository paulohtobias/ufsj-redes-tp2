#!/bin/bash

modo=$1
usuarios=$2
threads=$3
url=$4
site=$5
valendo=1

modos[0]="iterativo"
modos[1]="threads"
modos[2]="fila"
modos[3]="concorrente"
modos[4]="php"

modo_str=${modos[$modo]}

tempo=20S

saida=log/saida_$modo_str.log
saida_erros=log/erros-$modo_str
mkdir -p $saida_erros
saida_erros="$saida_erros/$usuarios"

echo "URL: $url"

#Abrindo o servidor
if [ $modo -lt 4 ]
then
	echo ./servidor -q -m $modo -c $threads -r $site
	./servidor -q -m $modo -c $threads -r $site &
	spid=$!
	sleep 0.1
else
	echo php -S $url -t $site
	php -S $url -t $site &
	spid=$!
	sleep 2
fi
echo "spid: "$spid

# Rodando o siege
echo Testando $modo_str com $usuarios usuarios

if [ $valendo -eq 1 ]
then
	siege -c $usuarios -t $tempo --log=$saida --mark="$usuarios,$tempo,$threads" $url > /dev/null 2>$saida_erros &
	siege_pid=$!
	sleep 22
	kill $siege_pid
	cat $saida
else
	siege -c $usuarios --reps=1 $url
fi

echo Fim teste $modo_str com $usuarios usuarios
kill $spid
