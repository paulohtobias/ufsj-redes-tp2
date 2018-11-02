<html>
	<head>
		<meta charset="UTF8">
		<title>Visualizador de Ponteiros</title>
		<meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
		<!-- Pretty Print -->
		<link rel="stylesheet" href="frameworks/prettify/prettify.css">
		<link rel="stylesheet" href="frameworks/bootstrap-4.0.0/css/bootstrap.min.css">
		<link rel="stylesheet" type="text/css" href="frameworks/gliphycons/bootstrap.css">
		<link rel="stylesheet" href="css/style.css" type="text/css">
		
		<script src="scripts/avaliacao.js"></script>
		<script src="frameworks/prettify/prettify.js"></script>
		<script src="frameworks/jquery/jquery-3.3.1.min.js"></script>
		<script src="frameworks/bootstrap-4.0.0/js/bootstrap.min.js"></script>
	</head>

	<body onload="start_onload();">
		<div class="container-fluid">
			<div class="row h-100">
				<aside class="col-12 col-md-2 p-0 bg-dark">
					<nav class="navbar navbar-expand navbar-dark bg-dark flex-row">
						<div class="collapse navbar-collapse">
							<div class="flex-md-column flex-row navbar-nav w-100 justify-content-between">
								<ul class="list-unstyled components w-100">
									<li class="nav-item">
										<a class="nav-link text-center" href="index.php">
											<span class="glyphicon glyphicon-home"></span> Home
										</a>
									</li>
									
									<li class="nav-item">
										<a class="nav-link text-center" href="NovoExercicio.html">
											<span class="glyphicon glyphicon-floppy-save"></span> Criar exercício
										</a>
									</li>
									
									<li><hr/></li>
									
									<li>
										<button class="btn btn-info w-100 mt-1 mb-1" onclick="resolver()">
											<span class="glyphicon glyphicon-ok pull-left"> </span> Resolução
										</button>
									</li>
									<li>
										<button class="btn btn-info w-100 mt-1 mb-1" onclick="avaliar();" accesskey="n">
											<span class="glyphicon glyphicon-check pull-left"> </span> Verificar
										</button>
									</li>
									<li>
										<button class="btn btn-danger w-100 mt-1 mb-1" onclick="resetar();" accesskey="r">
											<span class="glyphicon glyphicon-repeat"> </span> Resetar
										</button>
									</li>
								</ul>
							</div>
						</div>
					</nav>
				</aside>
				<main class="row col-12 col-md-10" style="overflow: auto;">
					
					<div class="container m-0 p-0 col-12 col-xl-5">
						
						<div id="LineNumbers" class="Numbers Dark-Base d-inline float-left"></div>

						<div id="Code" class="Code d-inline"></div>
					</div>

					<div class="h-100 m-0 p-0 d-none d-xl-block" style="
						border-left: solid #b3b3b3 0.1em; 
						border-right: solid #b3b3b3 0.1em; 
						width: 1%;">
					</div>

					<div class="col-12 m-0 p-0 col-xl-6">
						<table class="table table-striped"> 
							<thead>
								<tr id="MemHeader"> 
									<th class="table-dark MCol1">Endereço</th> 
									<th class="MCol2">Função</th> 
									<th class="MCol3">Nome da Variável</th> 
									<th class="MCol4">Valor</th> 
									<th class="MCol5">Valor no Endereço</th> 
								</tr> 
							</thead> 
							<tbody id="MemTabl"> 
							</tbody> 
						</table> 
					</div>
				</main>
			</div>
		</div>
	</body>
</html>

<?php
	$exercicio = NULL;
	if(!empty($_POST)){
		$arquivo = "./exercicios/".$_POST['arquivo'];

		$executavel = substr( $arquivo,0 , strpos($arquivo,".c"));
		system("gcc -g ".$arquivo." -o ".$executavel);
		
		if(!file_exists($executavel.".json")){
			system("python3 parser.py ".$executavel);
		}
		
		$exercicio = fopen($arquivo,'r');
		$JSON = file_get_contents($executavel.".json");
		echo
		"<script>
			requisitos = JSON.parse('".str_replace("'","\'",$JSON)."');
		</script>";
	}else{
		echo "Não carregou o post";
	}
	if($exercicio != NULL){
		$i = 0;
		for($i = 1; !feof($exercicio); $i++){
			
			if(fscanf($exercicio,"%[^\n]\n",$leitura)==0){
				$leitura = '';
			}
			
			$instrucao = " ".str_replace('<', '&lt;', $leitura);
			$instrucao = str_replace("\\n", "\\\\n", $instrucao);
			$instrucao = str_replace('"', '\\"', $instrucao);
			//echo $instrucao;
			echo 
			'<script>
				var pre = document.createElement("pre");
				pre.className = "prettyprint prettyprinted m-0 p-0";
				pre.id = "linha_" + '.$i.';
		
				pre.innerHTML = PR.prettyPrintOne("'.$instrucao.'", "C", true);
				document.getElementById("Code").appendChild(pre);
				
			</script>';
		}
		$i--;
		echo 
		'<script>
			insere_numeros_das_linhas('.$i.');
		</script>';
	}
?>