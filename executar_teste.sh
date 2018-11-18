#!/bin/bash

modo=$1
usuarios=$2
threads=$3

ip="$(ifconfig wlp3s0 | grep 'inet end.: ' | cut -d ':' -f 2 | cut -d ' ' -f 2)"
porta=2222
url="$ip:$porta"

modos[0]="iterativo"
modos[1]="threads"
modos[2]="fila"
modos[3]="concorrente"

modo_str=${modos[$modo]}

#tempo: 20s
tempo=20S

#threads fila: 2 - 1024 paso 2^x

saida=log/saida_$modo_str.log
saida_erros=log/erros-$modo_str/$usuarios

echo "URL: $url"
# Abrindo o servidor
./servidor -m $modo -p $porta -c $threads -q -r sites/site-simples/ &
spid=$!

# Rodando o siege
echo Testando com $usuarios usuarios
siege -c $usuarios -t $tempo --log=$saida --mark="$usuarios;$tempo;$threads" $url > /dev/null 2>$saida_erros
kill $spid

