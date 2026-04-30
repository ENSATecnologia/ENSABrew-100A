// ------------------------------------------------------------------------------------------------------------------------- //
// Logger ENSABrew - ENSA Tecnologia
// ------------------------------------------------------------------------------------------------------------------------- //

const msgLine = "\n ****************************************************************************************** \n";

console.log("\n");
console.log(" ****************************************************************************************** ");
console.log(" *                                                                                        * ");
console.log(" *                            Logger ENSABrew - ENSA Tecnologia                           * ");
console.log(" *                                                                                        * ");
console.log(" ****************************************************************************************** \n");

const fs = require("fs").promises;
const path = require("path");
const net = require("net");
const readline = require("readline").createInterface({
	input: process.stdin,
	output: process.stdout
});

let myFile = "";
let pendingData = "";

function inputFileName()
{
	readline.question(" -> Digite o nome do arquivo: ", (answer) =>
	{
		myFile = path.basename(answer).replace(/[<>:"/\\|?*]/g, "_") + ".txt";
		readline.close();

		console.log(msgLine);
		console.log(" ----------                     Log de Brassagem em Andamento                    ---------- ");
		console.log(msgLine);
	});
}

function formatNumber(number)
{
	number = parseInt(number, 10);
	if(number < 10) number = "0" + number;
	return number;
}

function actualTimeStamp()
{
	let time = new Date();
	let currentTime = formatNumber(time.getHours()) + ":" +
					  formatNumber(time.getMinutes()) + ":" +
					  formatNumber(time.getSeconds());
	let currentDate = formatNumber(time.getDate()) + "/" +
					  formatNumber(time.getMonth() + 1) + "/" +
					  time.getFullYear();

	return currentDate + " - " + currentTime + ": ";
}

function timeInMinutes(seconds)
{
	return formatNumber(seconds / 60) + ":" + formatNumber(seconds % 60);
}

function formatData(auxData)
{
	auxData = (auxData || "").trim();

	if(auxData.substring(0, 2) === "kp")
		return "#KP";

	let auxSlipValues = auxData.split("|");
	let validData = auxSlipValues.length >= 7 && auxSlipValues.every((element) => element !== undefined && element !== "");

	if(!validData)
		return "#ERRO";

	if(auxSlipValues[1] === "P")
		auxSlipValues[1] = "SETPOINT";
	else
		auxSlipValues[1] = "RAMPA   ";

	auxSlipValues[2] = formatNumber(auxSlipValues[2]);
	auxSlipValues[5] = timeInMinutes(auxSlipValues[5]);
	auxSlipValues[6] = formatNumber(auxSlipValues[6]);

	return auxSlipValues.join("|");
}

async function storeLog(auxMyFile, dataSend)
{
	try
	{
		await fs.appendFile(path.join("FileLogs", auxMyFile), dataSend + "\r\n", { encoding: "utf-8", flag: "a+" });
	}
	catch(err)
	{
		console.log(" # ERRO # -> Ha algum problema com o arquivo!");
	}
}

const myServer = net.createServer(function (device)
{
	console.log(" ----------                        ENSABrew esta conectado                       ---------- ");
	console.log(msgLine);

	inputFileName();

	device.on("data", function (msg)
	{
		if(myFile === "")
			return;

		pendingData += msg.toString("utf8");

		if(pendingData.length > 4096)
		{
			console.log(" # ERRO # -> Pacote excedeu o limite do logger!");
			pendingData = "";
			return;
		}

		let chunks = pendingData.split(/\r?\n/);
		pendingData = chunks.pop();

		chunks.forEach((chunk) =>
		{
			let auxData = formatData(chunk);
			if(auxData === "#ERRO" || auxData === "#KP")
				return;

			let timeStamp = actualTimeStamp();
			console.log(" -> " + timeStamp + auxData);
			storeLog(myFile, timeStamp + auxData);
		});
	});

	device.on("close", function()
	{
		device.end();
		pendingData = "";
		console.log("\n" + msgLine);
		console.log(" ----------                         ENSABrew Desconectado                        ---------- ");
		console.log(msgLine);
	});

	device.on("error", function()
	{
		device.end();
	});
});

const auxServer = require("./conectionPort");
const serverPort = auxServer.conectionPort;

myServer.listen({ port: serverPort });
console.log(" ----------                   Servidor escutando na porta: " + serverPort + "                  ---------- ");
console.log(msgLine);
