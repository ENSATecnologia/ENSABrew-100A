// ------------------------------------------------------------------------------------------------------------------------- //
//																		         						 					 //
// 				     								Logger ENSABrew - ENSATecnologia					  			 		 //
//																		          						 					 //
// ------------------------------------------------------------------------------------------------------------------------- //

// ------------------------------------------------------------------------------------------------------------------------- //
// Mensagens iniciais

const msgLine = "\n ****************************************************************************************** \n";

console.log("\n");
console.log(" ****************************************************************************************** ");
console.log(" *                                                                                        * ");
console.log(" *                            Logger ENSABrew - ENSA Tecnologia                           * ");
console.log(" *                                                                                        * ");
console.log(" ****************************************************************************************** \n");

// console.log(" ----------                   Pressione CTRL + C para finalizar                  ---------- ");
// Linha de separação
// console.log(msgLine); 

// ------------------------------------------------------------------------------------------------------------------------- //
// Importa o módulo de arquivos - inclui 'promises'

const fs = require('fs').promises;

// ------------------------------------------------------------------------------------------------------------------------- //
// Variáveis globais

let myFile = "";

// ------------------------------------------------------------------------------------------------------------------------- //
// Entrada de dados

const readline = require('readline').createInterface
({
	input: process.stdin,
	output: process.stdout
});

function inputFileName()
{
	readline.question(" -> Digite o nome do arquivo: ", (answer) => 
	{
		// Atualiza o nome do arquivo
		myFile = answer + ".txt"; 
		// Fecha a entrada de dados
	    readline.close();
	    // Linha de separação
		console.log(msgLine);

		// Log de Brassagem
		console.log(" ----------                     Log de Brassagem em Andamento                    ---------- ");
		// Saída
		// console.log(" ----------                   Pressione CTRL + C para finalizar                  ---------- ");
		// Linha de separação
		console.log(msgLine); 
	});
}

// ------------------------------------------------------------------------------------------------------------------------- //
// Conexão

// Importa o módulo 'net'
const net = require('net')

// Cria o servidor
let myServer = net.createServer(function (device) 
{  
	// --------------------------------------------------------------------------------------------------------------------- //
	// DEBUG de conexão

	console.log(" ----------                        ENSABrew está conectado                       ---------- ");
	// Linha de separação
	console.log(msgLine); 

	// --------------------------------------------------------------------------------------------------------------------- //
	// Insere o nome do arquivo

	inputFileName();

	// --------------------------------------------------------------------------------------------------------------------- //
	// Evento de dado

	device.on('data', function (msg) 
	{	
		// Se já inicializou o arquivo, libera o armazenamento do LOG
		if(myFile !== "")
		{
			// Pega o timestamp
			let timeStamp = actualTimeStamp();
			// Monta o dado para salvar no arquivo
	  		let auxData = msg.toString();
	  		// Formata o dado
	  		auxData = formatData(auxData);

	  		// Se houver erro no dado, ou for KeepAlive (KP), não envia nem armazena
	  		if(auxData !== "#ERRO" && auxData !== "#KP")
	  		{
		  		// DEBUG do dado em tela
		    	console.log(" -> " + timeStamp + auxData);
		    	// Armazena o dado do LOG
		    	storeLog(myFile, (timeStamp + auxData));
	  		}
		}
	});

	// --------------------------------------------------------------------------------------------------------------------- //
	// Arquivo de desconexão ou erro

	device.on('close', function()
	{    
		device.end();
		// Linha de separação
		console.log("\n" + msgLine);
		// DEBUG
		console.log(" ----------                         ENSABrew Desconectado                        ---------- ");
		// Linha de separação
		console.log(msgLine);
	});

	device.on("error", function(err)
	{
		device.end();    
	});

	// --------------------------------------------------------------------------------------------------------------------- //

});

// Porta de escuta do servidor
const auxServer = require("./conectionPort");
// Extrai a porta de conexão
const serverPort = auxServer.conectionPort;
// Servidor escutandoauxServerPort na porta 'serverPort'
myServer.listen({ port: serverPort });
// DEBUG
console.log(" ----------                   Servidor escutando na porta: " + serverPort + "                  ---------- ");
// Linha de separação
console.log(msgLine); 

// ------------------------------------------------------------------------------------------------------------------------- //

function formatNumber(number)
{
	number = parseInt(number);

	if(number < 10) number = "0" + number;
	return(number);
}

// ------------------------------------------------------------------------------------------------------------------------- // 

function actualTimeStamp()
{
	let time = new Date();
  	let currentTime = formatNumber(time.getHours())   + ":" + 
  					  formatNumber(time.getMinutes()) + ":" + 
  					  formatNumber(time.getSeconds());

  	let currentDate = formatNumber(time.getDate()) 		+ "/" + 
  					  formatNumber(time.getMonth() + 1) + "/" + 
  					  time.getFullYear();
	
	
	return (currentDate + " - " + currentTime + ": ");
		
}

// ------------------------------------------------------------------------------------------------------------------------- // 

function formatData(auxData)
{
	// Verifica se é keepAlive, se for, retorna
	if(auxData.substring(0, 2) === "kp")
		return("#KP");

	// Cria um vetor com os dados quebrados de acordo com o delimitador
	let auxSlipValues = auxData.split("|");

	// Verifica se o dado está vindo corretamente
	let testData = auxSlipValues.find(element => element === NaN);

	// Se o dado for "undefined", quer dizer que veio corretamente
	if(testData === undefined)
	{
		// Altera de P para SETPOINT
		if(auxSlipValues[1] === "P")
			auxSlipValues[1] = "SETPOINT";
		// Altera de R para RAMPA
		else auxSlipValues[1] = "RAMPA   ";

		// Formata o número do patamar
		auxSlipValues[2] = formatNumber(auxSlipValues[2]);
		// Converte de segundos para minutos
		auxSlipValues[5] = timeInMinutes(auxSlipValues[5]);
		// Formata o estado do relé
		auxSlipValues[6] = formatNumber(auxSlipValues[6]);
		
		// Retorna o dado formatado
		return(auxSlipValues.join("|"));
	}

	// Se não retorna #ERRO
	else return("#ERRO");
}

async function storeLog(auxMyFile, dataSend)
{
	try
	{
		// Salva o dado no arquivo
		await fs.appendFile(("FileLogs/" + auxMyFile), (dataSend + "\r\n"), { enconding:'utf-8', flag: 'a+' })	
	}
	catch(err)
	{
		console.log(" # ERRO # -> Há algum problema com o arquivo!");
	}
}

// ------------------------------------------------------------------------------------------------------------------------- //

const convertSec2Hrs = (auxHrs) => { return(auxHrs/3600) };
const convertSec2Min = (auxSec) => { return(auxSec/60)   };

function timeInMinutes(seconds)
{
	return(formatNumber(convertSec2Min(seconds)) + ":" + formatNumber((seconds%60)))
}

// ------------------------------------------------------------------------------------------------------------------------- //
