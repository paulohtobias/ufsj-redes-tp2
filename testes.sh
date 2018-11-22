#!/bin/bash

ip="$(ifconfig wlp3s0 | grep 'inet end.: ' | cut -d ':' -f 2 | cut -d ' ' -f 2)"
porta=2222
url="$ip:$porta"
site=sites/rea-ihc/

for modo in $(seq 0 4); do
	threads=2
	while [ $threads -le 1024 ]; do
		# Teste com 1 usuário
		echo ./executar_teste.sh $modo 1 $threads $url $site
		./executar_teste.sh $modo 1 $threads $url $site

		# Teste com 10, 20, 30, 40 e 50 usuários simultâneos
		for usuarios in $(seq 10 10 50); do
			sleep 20
			echo ./executar_teste.sh $modo $usuarios $threads $url $site
			./executar_teste.sh $modo $usuarios $threads $url $site
		done

		# Teste com 100, 150, 200 e 250 usuários simultâneos
		for usuarios in $(seq 100 50 255); do
			sleep 20
			echo ./executar_teste.sh $modo $usuarios $threads $url $site
			./executar_teste.sh $modo $usuarios $threads $url $site
		done

		# Threads só é relevante p/ modo=2
		if [ $modo -ne 2 ]
		then
			break
		fi
		let threads=$threads*2
	done
done

