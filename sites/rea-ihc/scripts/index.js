function listarArquivos() {
	/* Pega a lista de exercícios */
	document.getElementById("ListaDeExercicios").innerHTML = ""; //Limpa o campo
	var arquivos = document.getElementById("CarregarArquivos").files;

	for(var i = 0; i < arquivos.length; i++){
		if(arquivos[i].name.endsWith(".c")){
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