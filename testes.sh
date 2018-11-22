#!/bin/bash

ip="$(ifconfig wlp3s0 | grep 'inet end.: ' | cut -d ':' -f 2 | cut -d ' ' -f 2)"
porta=2222
url="$ip:$porta"
site=sites/site-simples/

for modo in $(seq 0 4); do
	threads=2
	while [ $threads -le 1024 ]; do
		usuarios=1
		while [ $usuarios -le 1000 ]; do
			#Abrindo o servidor
			spid=0
			if [ $modo -lt 4 ]
			then
				echo ./servidor -q -m $modo -p $porta -c $threads -r $site
				./servidor -q -m $modo -p $porta -c $threads -r $site &
				spid=$!
			else
				echo php -S $url -t $site
				php -S $url -t $site &
				spid=$!
			fi
			echo "spid: "$spid
			
			#Rodando o siege
			echo ./executar_teste.sh $modo $usuarios $threads $url
			./executar_teste.sh $modo $usuarios $threads $url
			
			kill $spid
			let usuarios=$usuarios*10
			sleep 120
		done
		if [ $modo -ne 2 ]
		then
			break
		fi
		let threads=$threads*2
	done
done

