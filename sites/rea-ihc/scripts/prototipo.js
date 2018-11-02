/* Variáveis globais */
var linhas_iniciais = 1;
var endereços_iniciais = 20;
var indentação_padrão = 4;
var memórias = [];
var linha_atual = 1;
var entradaRequisitos = "";
var instrução_atual = 0;

function Start() {
	//genLinesNum(linhas_iniciais);
	genAddressNum(endereços_iniciais);
}

/* Cria um numero hexadecimal e o formata para 4 digitos */
function int_to_endereço(i){
	var hexNum = (i).toString(10).toUpperCase();
	return ("0000" + hexNum).slice(-4);
}

/* Função que gera a tabela de endereços */
function genAddressNum(qntLin) {
	/* Calcula qual é a quantidade de colunas na tabela */
	var qntCol = document.getElementsByClassName("MemHeader")[0].children.length;
	
	for (let i = 0; i < qntLin; i++) {
		/* Cria uma div pai que receberá as colunas */
		var parent = document.createElement("tr");

		/* Primeira coluna, endereços gerados */
		var div = document.createElement("td");
		div.className = "MCol1 Dark-Base";
		div.innerHTML = ""+ int_to_endereço(i);
		/* Adiciona ao pai */
		parent.appendChild(div);

		/* Cria a segunda coluna até a quantidade de colunas 
		   Todas elas possui um filho input */
		for(let i = 2; i <= qntCol; i++){
			/* Cria a coluna */
			var div = document.createElement("td");
			div.className = "MCol"+(i);

			/* Cria o filho input */
			var input = document.createElement("input");
			input.type = "text";

			/* Adiciona o filho a coluna */
			div.appendChild(input);

			/* Adiciona a coluna ao pai */
			parent.appendChild(div);
		}

		/* Adiciona o pai à tabela de memória */
		document.getElementById("MemTabl").appendChild(parent);
	}
	memórias = [loadTableData()];
}

function loadLastTable(){
	var lastTable = memórias.pop();
	
}

function restartTable(){
	desativar_instrução(linha_atual);
	console.log(entradaRequisitos);
	linha_atual = entradaRequisitos["requisitos"][0]["linha"];
	console.log(linha_atual);
	ativar_instrução(linha_atual);
	instrução_atual = 0;

	var cols = [];
	for(let i = 0; i < document.getElementsByClassName("MemHeader")[0].children.length; i++){
		cols[i] = document.getElementsByClassName("MCol"+(i+1));
	}
	
	for(let i = 1; i < cols[0].length; i++){
		for(let j = 1; j < cols.length; j++){
			cols[j][i].children[0].value = "";		
		}
	}
	memórias = [loadTableData()];
}

/* TODO: Otimizar a função deixando-a genérica e legivel*/
/* Função que transforma os dados da tabela em JSON */
function loadTableData() {
	/* JSON da tabela */
	var tableJSON = [];

	/* Lista das possiveis colunas da tabela*/
	var cols = [];

	/* Cabeçalho da tabela */
	var tableHeader = document.getElementsByClassName("MemHeader")[0];
	/* Para cada coluna do cabeçalho, pegue todos os filhos e adicione a coluna correspondente*/
	for(let i = 0; i < tableHeader.children.length; i++){
		cols[i] = document.getElementsByClassName("MCol"+(i+1));
	}

	/* Para cada linha da tabela crie um array no JSON */
	for(let i = 1; i < cols[0].length; i++){
		var empty = 0;	/* Variavel que vê colunas vazias*/
		var values = [];/* Variavel que captura os valores*/
		/* Para cada uma das colunas, capture o respectivo valor e adicione a values */
		for(let j = 0; j < cols.length; j++){
			if(j == 0){ /* Endereço é valor direto no html*/
				values[j] = cols[j][i].innerHTML;
			}else{ /* Outras colunas possuem um filho input */
				values[j] = cols[j][i].children[0].value;
				/* contando colunas vazias */
				empty += values[j] == 0 ? 1: 0; 
			}
		}

		/* Guarda o index de referencia do atributo JSON */
		var index = values[0]; //Endereço
		
		/* Se houver determinada quantidade de colunas vazias, não insere no JSON 
			É cols.length-1, pois Endereço nunca é vazio*/
		//if(empty >= (cols.length-1))continue;

		/* Cria a linha do JSON com o nome da variável */
		tableJSON[index] = {};
		/* Adiciona todos os outros valores a linha */
		for(let j = 0; j < cols.length; j++){
			var attrName = cols[j][0].innerHTML;
			tableJSON[index][attrName] = values[j];
		}
	}
	
	/* Retorna o JSON com todas as informações */
	return tableJSON;
}

function comparar_instrução(requisitos, entrada){
	var campos = ["Nome da Variavel", "Valor", "Valor Desreferenciado"];

	for(let i = 0; i < campos.length; i++){
		if(requisitos[campos[i]] != entrada[campos[i]]){
			return false;
		}
	}
	return true;
}

function avançar(){
	console.log("========================AVANÇAR=================");
	var table = loadTableData();
	console.log("TABELA ANTIGA:");
	console.log(memórias[memórias.length - 1]);
	console.log("\nTABELA NOVA:");
	console.log(table)
	
	//Comparando com a tabela anterior.
	var ultima_tabela = memórias[memórias.length - 1];
	var mudanças = []
	for(let i = 0; i < endereços_iniciais; i++){
		var endereço = "" + int_to_endereço(i);
		var colunas = ["Nome da Variavel", "Valor", "Valor Desreferenciado"];
		
		for(let j = 0; j < colunas.length; j++){
			if(table[endereço][colunas[j]] != ultima_tabela[endereço][colunas[j]]){
				console.log("Mudou " + colunas[j] + " no endereço " + endereço);
				console.log("mudou de " + ultima_tabela[endereço][colunas[j]] + " para " + table[endereço][colunas[j]]);
				mudanças.push({"Nome da Variavel": table[endereço]["Nome da Variavel"], "Valor": table[endereço]["Valor"], "linha": linha_atual});
				break;
			}
		}
	}

	if(mudanças.length != 1 || !comparar_instrução(entradaRequisitos["requisitos"][instrução_atual], mudanças[0])){
		alert("A mudança que você fez não bate com o que está indicado no código-fonte, tente ver onde está diferente.\nDica: valores do tipo char (caracteres) são representados entre aspas simples.");
		return;
	}

	alert("Muito bem! Para a próxima instrução.");
	desativar_instrução(linha_atual);
	instrução_atual++;
	if(instrução_atual >= entradaRequisitos["requisitos"].length){
		alert("Parabéns exercício finalizado com sucesso!!\nVocê será redirecionado para a tela de exercícios");
		window.location.replace("./index.php")
		return;
	}
	linha_atual = entradaRequisitos["requisitos"][instrução_atual]["linha"];
	ativar_instrução(linha_atual);
	
	memórias.push(table);
}

function voltar(){
	desativar_instrução(linha_atual);
	instrução_atual--;
	linha_atual = entradaRequisitos["requisitos"][instrução_atual]["linha"];
	ativar_instrução(linha_atual);
}

function ativar_instrução(linha){
	console.log("Ativando a linha " + linha);
	var instrução = document.getElementById("linha_" + linha);
	instrução.className += " Ativo";

	var número = document.getElementById("numero_" + linha);
	número.className += " Ativo";
}

function desativar_instrução(linha){
	console.log("Desativando a linha " + linha);
	var instrução = document.getElementById("linha_" + linha);
	instrução.className = instrução.className.replace(/\s?Ativo\s?/, "");

	var número = document.getElementById("numero_" + linha);
	número.className = número.className.replace(/\s?Ativo\s?/, "");
}

function listarArquivos() {
	document.getElementById("ListaDeExercicios").innerHTML = "";
	var arquivos = document.getElementById("CarregarArquivos").files;
	var nomes = [];
	for(var i = 0; i < arquivos.length; i++){
		if(arquivos[i].name.endsWith(".c")){
			nomes.push(arquivos[i].name);
			var novaLinha = document.createElement("tr");
			var nome = document.createElement("td");
			var dificuldade = document.createElement("td");
			
			/* Cria um link para o nome */
			var nameLink = document.createElement("a");
			nameLink.innerHTML = arquivos[i].name;
			nameLink.onclick = retornoClick(arquivos[i].name);
			nameLink.href = "#";
			
			/* Cria um link para a dificuldade */
			var dificultLink = document.createElement("a");
			var stars = "<span class='glyphicon glyphicon-star'></span>";
			for(var k = 0, j = Math.random()*10 % 4; k < j; k++){
				stars += "<span class='glyphicon glyphicon-star'></span>";
			}
			dificultLink.innerHTML = stars;
			dificultLink.onclick = retornoClick(arquivos[i].name);
			dificultLink.href = "#";
			
			/* Adiciona todos os filhos */
			nome.appendChild(nameLink);
			dificuldade.appendChild(dificultLink);
			novaLinha.appendChild(nome);
			novaLinha.appendChild(dificuldade);
			document.getElementById("ListaDeExercicios").appendChild(novaLinha);
		}
	}
}

function retornoClick(nome) {
	document.getElementById("NomeArquivo").value = nome;
	document.getElementById("loadExercicios").submit();
}