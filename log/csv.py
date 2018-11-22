# -*- coding: utf-8 -*-

modos = ["iterativo", "threads", "fila", "concorrente", "php"]

dados_necessarios = ["Trans Rate"]

for modo in modos:
    tabela = {}
    tf = []
    gthreads = []
    with open("saida_" + modo + ".log") as f:
        cabecalho = map(str.strip, f.readline().split(","))

        while True:
            linha = f.readline()
            if len(linha) == 0:
                break

            axis = linha[5:-6].split(",")
            usuarios = axis[0]

            threads = 4
            if len(axis) > 2:
                threads = int(axis[2])

            tabela[threads] = {}

            tabela[threads][usuarios] = {}

            gthreads.append(threads)

            dados = map(str.strip, f.readline().split(","))
            for i in range(len(cabecalho)):
                tabela[threads][usuarios][cabecalho[i]] = dados[i]

            tf.append({"threads": threads, "dados": tabela[threads]})

        gthreads = sorted(list(set(gthreads)))

        with open("csv/" + modo + ".csv", "w") as f2:
            for gthread in gthreads:
                f2.write("threads;" + str(gthread) + "\nUsuários")
                for dado in dados_necessarios:
                    f2.write(";" + dado)
                f2.write("\n")
                for bla in tf:
                    if bla['threads'] == gthread:
                        usuarios = bla['dados'].keys()[0]
                        f2.write(usuarios + ";")
                        for dado in dados_necessarios:
                            f2.write(bla['dados'][usuarios][dado].replace(".", ",") + ";")
                        f2.write("\n")
                f2.write("\n")
        
