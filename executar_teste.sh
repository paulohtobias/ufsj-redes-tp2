#!/bin/bash

modo=$1
usuarios=$2
threads=$3
url=$4

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

# Rodando o siege
echo Testando $modo_str com $usuarios usuarios
siege -c $usuarios -t $tempo --log=$saida --mark="$usuarios,$tempo,$threads" $url > /dev/null 2>$saida_erros
cat $saida
echo Fim teste $modo_str com $usuarios usuarios
