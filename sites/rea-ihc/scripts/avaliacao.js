/* Classes */
class Ponteiro{
	constructor(nome, variavel, endereço){
		this.ponteiro = nome;
		this.referencia = variavel;
		this.endereço = endereço;
	}
}

/* Enumerações */
var AtributoEndereço = {
	ENDERECO: 0,
	FUNCAO: 1,
	NOME_DA_VARIAVEL: 2,
	VALOR: 3,
	VALOR_NO_ENDERECO: 4
}

/* Globais */
var requisitos = "";
var linha_atual = 1;
var instrução_atual = 0;
var memória = [];

var ponteiros;

/* Script do javascript carrega antes do html, 
logo é necessário essa função carregar logo após a página 
ser carregada. Assim ela é chamada onload() */
function start_onload(){
	gera_tabela();
}

function resetar(){
	location.reload(true);	
}

/* Função que gera as linhas laterais do código */
function insere_numeros_das_linhas(quantidade){
	/* Pega a referência do output e reseta seu valor */
	document.getElementById("LineNumbers").innerHTML = "";

	for (let i = 1; i <= quantidade; i++) {
		/* Para cada index é criado um div, seu valor é o index */
		var linha = document.createElement("div");
		linha.innerHTML = i;
		linha.id = "numero_" + i;

		/* Insere a nova div como filho do output */
		document.getElementById("LineNumbers").appendChild(linha);
	}

	//Identificando a primeira linha.
	console.log(requisitos);
	instrução_atual = 0;
	linha_atual = requisitos["requisitos"][instrução_atual]["linha"];
	setar_instrução(linha_atual);
}

/* Função que gera a tabela de endereço */
function __inteiro_para_endereço(inteiro){
	return ("0000" + inteiro).slice(-4);
}

function __id_endereço(endereço) {
	return "tabela_endereço_" + __inteiro_para_endereço(endereço);
}

function __gera_stack_frame(funçãoChamadora, inicioContagem, qntLinhas, cor) {
	
	var qntColunas = document.getElementById("MemHeader").children.length;

  	for (let i = 0; i < qntLinhas; i++) {
  		/* Cria o elemento pai que receberá as colunas */
  		var tr_linha = document.createElement("tr");
		  tr_linha.id = __id_endereço(i + inicioContagem);
		  tr_linha.style = "background-color: hsl(" + cor + ",100%,90%) !important;";
  		tr_linha.className = funçãoChamadora;

  		/* Gerando a coluna com o endereço */
  		var td_endereço = document.createElement("td");
  		td_endereço.className = "table-dark MCol1";
  		td_endereço.innerHTML = __inteiro_para_endereço(i + inicioContagem);

  		/* Gerando a coluna com a função */
  		var td_função = document.createElement("td");

  		td_função.innerHTML = funçãoChamadora;
  		td_função.className = "MCol2";

  		tr_linha.appendChild(td_endereço);
  		tr_linha.appendChild(td_função);

  		/* Adicionando outras colunas */
  		/* <Nome da variavel>;<valor>;<Valor no endereço>*/
  		for (let colunas = 3; colunas < qntColunas; colunas++) {
  			var td_col = document.createElement("td");
  			td_col.className = "MCol" + (colunas);

  			/*Criando o input que estará dentro da coluna */
  			var input = document.createElement("input");
  			input.type = "text";
  			input.className = "border-below";

  			td_col.appendChild(input);
  			tr_linha.appendChild(td_col);
  		}

  		var valor_no_endereço = document.createElement("td");
  		valor_no_endereço.className = "MCol5";

  		tr_linha.appendChild(valor_no_endereço);

  		document.getElementById("MemTabl").appendChild(tr_linha);
  	}
}

function gera_tabela(){
	var cor = 180; //Atributo hue na escala HSL
	var funções = requisitos["funções"];
	var qnt_total_de_funções = Object.keys(funções).length;

	Object.keys(funções).forEach(função => {
		var inicio = funções[função]["início"];
		var fim = funções[função]["fim"];
		__gera_stack_frame(função, inicio, fim-inicio, cor);
		cor += (360/qnt_total_de_funções)%360;
	});

	memória = carregar_tabela();
	ponteiros = [];
}

/* Gera um vetor com todos os dados atuais da tabela na função atual. */
function carregar_tabela() {
	var cabeçalho = document.getElementById("MemHeader").children;
	var memória_atual = [];

	var função_atual = requisitos["requisitos"][0]["Função"];
	var linhas = document.getElementById("MemTabl").children.length;
	//A última coluna é ignorada pois contém o valor no endereço do ponteiro
	//E esta será atualizada automáticamente.
	var colunas = cabeçalho.length - 1;

	for (let i = 0; i < linhas; i++) {
		memória_atual[i] = {};
		var linha = document.getElementById("MemTabl").children[i].children;
		memória_atual[i][cabeçalho[0].innerHTML] = i;
		for (let j = 1; j < colunas; j++) {
			var valor = linha[j].innerHTML;
			if (linha[j].children.length > 0) {
				valor = linha[j].children[0].value;
			}
			memória_atual[i][cabeçalho[j].innerHTML] = valor;
		}
	}

	return memória_atual;	
}

/* Compara a memória atual com a memória antiga e identifica as alterações. */
function _identificar_alterações(memória_atual) {
	if (memória.length != memória_atual.length) {
		alert("Não era pra dar isso.");
	}
	
	var alterações = [];
	var cabeçalho = document.getElementById("MemHeader").children;

	for (let i = 0; i < memória.length; i++) {
		for (let j = 0; j < cabeçalho.length; j++) {
			var coluna = cabeçalho[j].innerHTML;
			
			if (memória_atual[i][coluna] != memória[i][coluna]) {
				alterações.push(memória_atual[i]);
				break;
			}
		}
	}

	return alterações;
}

/* Função que remove trechos da tabela com o nome de uma 
função associada: Precisa ser testada */
function _limpa_trecho_tabela(função){
	var linhas = document.getElementById("MemTabl").children;
	var qnt_colunas = document.getElementById("MemHeader").children.length;
	var qnt_linhas = linhas.children.length;	
		
	for(let linha = 0; linha < qnt_linhas; linha++){
		if(!linhas[linha].classList.contains(função)){
			continue;
		}
		for(let coluna = 0; coluna < qnt_colunas; coluna++){
			linhas[linha].children[coluna].firstChild.nodeValue = "";
		}
	}	
}

/* Funções que gerenciam as instruções do código */
function setar_instrução(linha, habilitar = true){
	var instrução = document.getElementById("linha_" + linha);
	var número = document.getElementById("numero_" + linha);
	
	instrução.classList.remove("Ativo");
	número.classList.remove("Ativo");

	if(habilitar){
		instrução.classList.add("Ativo");
		número.classList.add("Ativo");
	}		
}

/* Verifica se as alterações feitas estão certas de acordo com os requisitos. */
function avaliar(alert_sucesso = true) {
	if (instrução_atual >= requisitos["requisitos"].length) {
		alert("Todos os requisitos já foram cumpridos.");
		return;
	}

	var memória_alterada = carregar_tabela();
	var alterações_tabela = _identificar_alterações(memória_alterada);
	var alterações_requisitos = requisitos["requisitos"][instrução_atual]["alterações"];

	var mensagem_sucesso = "Muito bem! Para a próxima instrução.";
	var mensagem_erro = "A mudança que você fez não bate com o que está indicado no código-fonte, tente ver onde está diferente.";
	var mensagem_erro_dica = "Valores do tipo char (caracteres) são representados entre aspas simples.";
	

	//Caso trivial. A quantidade de alterações é diferente da quantidade de requisitos.
	console.log(alterações_tabela.length);
	console.log(alterações_requisitos.length);
	if (alterações_tabela.length != alterações_requisitos.length) {
		mensagem_erro_dica = "O número de alterações feitas na tabela (" + alterações_tabela.length + ") " +
		                     "é diferente do número de alterações necessárias.";
		
		alert(mensagem_erro + "\nDica: " + mensagem_erro_dica);
		return false;
	}

	var acerto;
	for (let i = 0; i < alterações_requisitos.length; i++) {
		acerto = false;
		for (let j = 0; j < alterações_requisitos.length; j++) {
			console.log(alterações_tabela[i]);
			console.log(alterações_requisitos[j]);
			if (alterações_tabela[i]["Nome da Variável"] == alterações_requisitos[j]["Nome da Variável"]){
				if(alterações_tabela[i]["Função"] == alterações_requisitos[j]["Função"]) {
					var valor_requisito = alterações_requisitos[j]["Valor"];
					var valor_tabela = alterações_tabela[i]["Valor"];
					
					//Se a variável for um ponteiro, então compare o nome da variável e da função.
					if (alterações_requisitos[i]["ponteiro"]) {
						//Atualizando o campo "Valor no Endereço".
						atualiza_valor_do_ponteiro(alterações_tabela[i]["Endereço"], AtributoEndereço.VALOR_NO_ENDERECO);

						var função_tabela = retorna_atributo_no_endereço(parseInt(valor_tabela), AtributoEndereço.FUNCAO);
						valor_tabela = retorna_atributo_no_endereço(parseInt(valor_tabela), AtributoEndereço.NOME_DA_VARIAVEL);
						
						let [função_requisito, valor_requisito_] = valor_requisito.split(":");
						valor_requisito = valor_requisito_;

						console.log("PONTEIROOOOO");
						console.log(função_tabela + " == " + função_requisito);
						console.log("=========================");

						//Se as funções forem diferentes, então é um erro.
						if (função_tabela != função_requisito) {
							mensagem_erro_dica = "'" + alterações_tabela[i]["Nome da Variável"] +
							                     "' aponta uma variável em outra função.";
							break;
						}

						var achou = false;

						for(let i = 0; i < ponteiros.length; i++) {
							var ponteiro = ponteiros[i];
							if(ponteiro.ponteiro == alterações_requisitos[j]["Função"] + ":" + alterações_requisitos[j]["Nome da Variável"]){
								ponteiro.referencia = alterações_requisitos[j]["Valor"];
								achou = true;
								break;
							}
						}

						if(achou == false){
							ponteiros.push(new Ponteiro(
								alterações_requisitos[j]["Função"] + ":" + alterações_requisitos[j]["Nome da Variável"],
								alterações_requisitos[j]["Valor"],
								__encontrar_endereço_variável(
									memória_alterada, 
									alterações_requisitos[j]["Nome da Variável"],
									alterações_requisitos[j]["Função"], 
									false
								)
							));
						}
					}

					console.log(alterações_tabela[i]["Nome da Variável"] + " == " + alterações_requisitos[j]["Nome da Variável"]);
					console.log(valor_tabela + " == " + valor_requisito);
					console.log("=========================");
					if (valor_tabela == valor_requisito) {
						acerto = true;
					} else {
						mensagem_erro_dica = "Tente mudar o valor da variável '" + alterações_tabela[i]["Nome da Variável"] + "'.";
					}
					break;
				} else {
					mensagem_erro_dica = "'" + alterações_tabela[i]["Nome da Variável"] + "' precisa ser alocada em outra função.";
				}
			}
		}
		if (!acerto) { // ou (j == alterações_requisitos.length)
			alert(mensagem_erro + "\nDica: " + mensagem_erro_dica);
			ponteiros.forEach(elemento=>{
				if (retorna_atributo_no_endereço(elemento.endereço, AtributoEndereço.VALOR)[0] != "[") {
					atualiza_valor_do_ponteiro(elemento.endereço);
				}
			});
			return false;
		}
	}

	setar_instrução(requisitos["requisitos"][instrução_atual]["linha"], false);
	
	/*console.log("+++++++++++ Tabela Ponteiros +++++++++++++++");
	console.log(ponteiros);
	console.log("++++++++++++++++++++++++++++++++++++++++++++");*/

	ponteiros.forEach(elemento=>{
		if (retorna_atributo_no_endereço(elemento.endereço, AtributoEndereço.VALOR)[0] != "[") {
			atualiza_valor_do_ponteiro(elemento.endereço);
		}
	});

	instrução_atual++;
	if (instrução_atual >= requisitos["requisitos"].length) {
		alert("Parabéns exercício finalizado com sucesso!!\n" +
		"Você será redirecionado para a tela de exercícios");
		window.location.replace("./index.php")
		return true;
	}

	setar_instrução(requisitos["requisitos"][instrução_atual]["linha"]);

	memória = memória_alterada;

	if (alert_sucesso) {
		alert(mensagem_sucesso);
	}
}

/* 
 * Retorna o endereço de uma variável em uma função. -1 se não encontrar.
 * Se livre == true, retorna o primeiro endereço livre na stackframe da função
 * caso a variável não seja encontrada.
 */
function __encontrar_endereço_variável(memória_atual, variável, função, livre) {
	var endereço = -1;
	var início = requisitos["funções"][função]["início"];
	var fim = requisitos["funções"][função]["fim"];

	for (let i = início; i < fim; i++) {
		if (memória_atual[i]["Nome da Variável"] == variável) {
			endereço = i;
			break;
		}
		if (livre && endereço == -1 && memória_atual[i]["Nome da Variável"] == "" && memória_atual[i]["Valor"] == "") {
			endereço = i;
		}
	}

	return endereço;
}

/* Preenche a tabela com os valores corretos. */
function resolver() {
	//Verifica se já foi concluído.
	if (instrução_atual >= requisitos["requisitos"].length) {
		alert("Todos os requisitos já foram cumpridos.");
		return;
	}

	var memória_atual = carregar_tabela();
	//var memória_atual = memória;

	console.log("====================\nMemória Atual");
	console.log(memória_atual);
	
	var alterações_requisitos = requisitos["requisitos"][instrução_atual]["alterações"];
	console.log("Instrução " + instrução_atual);
	console.log(alterações_requisitos);
	console.log("====================================");

	//Percorre todas as alterações.
	for (let i = 0; i < alterações_requisitos.length; i++) {
		var função = alterações_requisitos[i]["Função"];
		var variável = alterações_requisitos[i]["Nome da Variável"];
		var valor = alterações_requisitos[i]["Valor"];

		console.log("====================\nMemória Atual");
		console.log(memória_atual);

		//Procura o endereço da variável ou um endereço livre.
		//Em teoria, sempre haverá um endereço disponível, então isso não será checado.
		var endereço = __encontrar_endereço_variável(memória_atual, variável, função, true);

		console.log("Inserindo/Atualizando " + variável + " no endereço " + endereço);

		//Insere o nome da variável.
		atualiza_atributo_no_endereço(endereço, AtributoEndereço.NOME_DA_VARIAVEL, variável);
		//memória_atual[""]

		//Se for uma variável normal, insira o valor diretamente.
		if (alterações_requisitos[i]["ponteiro"] == false) {
			atualiza_atributo_no_endereço(endereço, AtributoEndereço.VALOR, valor);
		}

		//Se for um ponteiro, é preciso atualizar o valor para o endereço da variável que ele aponta.
		else {
			let [função_ponteiro, variável_ponteiro] = valor.split(":");

			valor = __encontrar_endereço_variável(memória_atual, variável_ponteiro, função_ponteiro, false);

			//Insere o endereço da variável que o ponteiro aponta.
			atualiza_atributo_no_endereço(endereço, AtributoEndereço.VALOR, valor);

			//Atualiza o campo Valor no Endereço
			atualiza_valor_do_ponteiro(endereço);
		}

		memória_atual = carregar_tabela();

	}

	//memória = carregar_tabela();

	avaliar(false);

	//instrução_atual++;
	//setar_instrução(requisitos["requisitos"][instrução_atual]["linha"], true);
} 

function retorna_atributo_no_endereço(endereço, atributo_endereço) {
	console.log(endereço);
	var atributo = 
		document.getElementById(
			__id_endereço(endereço)
		).children[atributo_endereço];

	console.log(atributo);
	if(atributo.children.length > 0){
		return atributo.firstChild.value;
	}
	return atributo.innerHTML;
}

function atualiza_atributo_no_endereço(endereço, atributo_endereço, valor) {
	var atributo = 
		document.getElementById(
			__id_endereço(endereço)
		).children[atributo_endereço];

	console.log(atributo);
	if(atributo.children.length > 0){
		atributo.firstChild.value = valor;
	}else{
		atributo.innerHTML = valor;
	}
}

/* Função que atualiza automaticamente o valor no endereço 
para variáveis do tipo ponteiro.
obs: É necessário o endereço de memória que o ponteiro está 
referênciando e a linha */
function atualiza_valor_do_ponteiro(endereço) {
	var linha = document.getElementById(__id_endereço(endereço));
	var valor_ponteiro = linha.children[3].firstChild.value;

	var valor_na_referência = 
		retorna_atributo_no_endereço(
			parseInt(valor_ponteiro), 
			AtributoEndereço.VALOR
		);
	
	linha.children[AtributoEndereço.VALOR_NO_ENDERECO].innerHTML = valor_na_referência;
}
