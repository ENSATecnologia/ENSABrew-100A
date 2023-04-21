//*******************************************************************************************
// Arquivo C/C++ do Projeto Controlador de Patamares -- ENSABrewControll
//-------------------------------------------------------------------------------------------
//
// Arquivo: ENSABrewAPPCommunication.ino
//
//
// Autor  : Jânio Anselmo, Eng. Me.
// Autor  : Thiago Anselmo, Tecgº
// Autor  : Gabriel da Silva Caetano
//
// CPU    : ATMega2561 - MegaCore
// Clock  : 16MHz
//
//*******************************************************************************************

//WWWWWWWWWW*********************************************************************************
// Funcão: communicationAPPTasks
// Data  : 22/01/2020	12:15
//WWWWWWWWWW*********************************************************************************

void communicationAPPTasks(void )
{
	// Se a flag de envio do keep alive estiver habilitada, faz o envio
	if(_flagSendKeepAlive)	
		controlKeepAlive(_SEND);
}

//WWWWWWWWWW*********************************************************************************
// Funcão: confirmConnectionAPP
// Data  : 22/01/2020	12:00
//WWWWWWWWWW*********************************************************************************

void confirmConnectionAPP(void )
{
	// Informações sobre o dado (tipo do cabeçalho, tipo do dado e tamanho do dado)
    uint8_t auxInfoData[3] = {_HEADER_SET, _CONNECTION_APP, 1};
    // Dado em si, com o número do comando
    String auxSendData[2] = {F("00"), F("ack")};
    // Envia o dado ao APP
    sendDataAPP(&auxInfoData[0], &auxSendData[0]);

	// Confirma APP conectado
	_connectedAPP = true;
	// Carrega o envio do keep alive
	controlKeepAlive(_LOAD);
}

//WWWWWWWWWW*********************************************************************************
// Funcão: controlKeepAlive
// Data  : 22/01/2020	12:05
//WWWWWWWWWW*********************************************************************************

void controlKeepAlive(uint8_t auxTypeAction)
{
	// Reseta a flag de envio do keep alive
	_flagSendKeepAlive = false;

	switch(auxTypeAction)
	{
		case _LOAD:
			
			// Limpa os temporizadores e flags
			controlKeepAlive(_CLEAR);
			// Carrega o temporizador
			wdtSendKeepAlive = TIME_SEND_KEEP_ALIVE;

		break;

		case _LOAD_WAIT:

			// Carrega o temporizador de aguardo de resposta
			wdtWaitRespKeepAlive = TIME_WAIT_RESP_KEEP_ALIVE;

			break;

		case _CLEAR:

			// Reseta o temporizador de envio
			wdtSendKeepAlive = 0;
			// Reseta o temporizador de aguardo
			wdtWaitRespKeepAlive = 0;

		break;

		case _SEND:

			// Carrega o temporizador de aguardo de resposta do keep alive
			controlKeepAlive(_LOAD_WAIT);

			// Informações sobre o dado (tipo do cabeçalho, tipo do dado e tamanho do dado)
		    uint8_t auxInfoData[3] = {_HEADER_EVENT, NULL, 1};
		    // Dado em si, com o número do comando
		    String auxSendData = F("00");
		    // Envia o dado ao APP
		    sendDataAPP(&auxInfoData[0], &auxSendData);

		break;

	}
}

//WWWWWWWWWW*********************************************************************************
// Funcão: sendDataAPP
// Data  : 22/01/2020	10:00
//WWWWWWWWWW*********************************************************************************

void sendDataAPP(uint8_t *auxInfoData, String *auxSendData) 
{
	// ------------------------------------------------------- //
	// Formato e significado dos vetores
	
	// auxInfoData[0] -> tipo do cabeçalho
	// auxInfoData[1] -> tipo do comando
	// auxInfoData[2] -> tamanho do dado

	// ------------------------------------------------------- //
	// Definição do tamanho do buffer para envio

    const size_t bufferSize = JSON_OBJECT_SIZE(auxInfoData[2]);
    DynamicJsonDocument jsonSendData(bufferSize);
    JsonObject bufferSendDataAPP = jsonSendData.to<JsonObject>();

    // ------------------------------------------------------- // 
    // Cabeçalho padrão

    bufferSendDataAPP[F("header")] = F("4E");
    bufferSendDataAPP[F("product")] = F("0002");
    bufferSendDataAPP[F("type")] = F("D002");
    bufferSendDataAPP[F("id")] = String(configGeral.idModule);

    // ------------------------------------------------------- // 
    // Definição do tipo do cabeçalho

    // Gerencia adição de código A1 no dado à ser enviado
	boolean _auxCodeA1 = false;
	// Gerencia adição de código A2 no dado à ser enviado
	boolean _auxCodeA2 = false;

    // Se for envio de tela, monta o dado diretamente
    if(auxInfoData[0] == _HEADER_SCREEN)
    {
    	bufferSendDataAPP[F("tela")] = auxSendData[0];
    
	    if(entradaAtual != F("Void"))
	    	bufferSendDataAPP[F("digitado")] = entradaAtual;
	    else if(opcaoAtual != F("Void"))
	    	bufferSendDataAPP[F("opcao")] = opcaoAtual;
	    if(statusAtual != F("Void"))
	    	bufferSendDataAPP[F("status")] = statusAtual;
    }

    // Se forem outros tipos de envio
    else
    {
	    // Verifica o tipo do cabeçalho
		switch(auxInfoData[0])
		{	
			// Cabeçalho de configuração/set
			case _HEADER_SET:

				bufferSendDataAPP[F("typeCmd")] = F("CC");

				break;

			// Cabeçalho de solicitação/get
			case _HEADER_GET:

				bufferSendDataAPP[F("typeCmd")] = F("FF");

				break;

			// Cabeçalho de evento
			case _HEADER_EVENT:

				bufferSendDataAPP[F("typeCmd")] = F("EE");

				break;
		}

		// ------------------------------------------------------- // 
		// Definição e montagem dos dados do comando

		// Extrai o número do comando
		bufferSendDataAPP[F("cmd")] = auxSendData[0];

		// Monta os dados do relacionados ao comando
		switch(auxInfoData[1])
		{
			// Confirmação de conexão
			case _CONNECTION_APP:

				bufferSendDataAPP[F("connection")] = auxSendData[1];

				break;

			// Evento de envio de temperatura
			case _TEMPERATURE:

				bufferSendDataAPP[F("temp")] = auxSendData[1];

				break;

			// Evento de leitura de porta
			case _READ_PORT:

				bufferSendDataAPP[F("port")] = auxSendData[1];
				bufferSendDataAPP[F("state")] = auxSendData[2];

				break;

			// Fervura - brassagem
			case _LOOP_FERVURA:

				bufferSendDataAPP[F("restante")] = auxSendData[1];
				bufferSendDataAPP[F("proximo")] = auxSendData[2];
				bufferSendDataAPP[F("nAdicao")] = auxSendData[3];
				bufferSendDataAPP[F("pwm")] = auxSendData[4];
				bufferSendDataAPP[F("tempo")] = auxSendData[5];

				// Sinaliza adição de código ao dado
				_auxCodeA1 = true;

				break;

			// Whirlpool - brassagem
			case _LOOP_WHIRLPOOL:

				bufferSendDataAPP[F("restante")] = auxSendData[1];
				bufferSendDataAPP[F("proximo")] = auxSendData[2];
				bufferSendDataAPP[F("nWhirlpool")] = auxSendData[3];
				bufferSendDataAPP[F("tempo")] = auxSendData[4];

				// Sinaliza adição de código ao dado
				_auxCodeA1 = true;

				break;

			// Descanso - brassagem
			case _LOOP_DESCANSO:

				bufferSendDataAPP[F("restante")] = auxSendData[1];
				bufferSendDataAPP[F("proximo")] = auxSendData[2];
				bufferSendDataAPP[F("nDescanso")] = auxSendData[3];
				bufferSendDataAPP[F("tempo")] = auxSendData[4];

				// Sinaliza adição de código ao dado
				_auxCodeA1 = true;

				break;

			// Patamares - brassagem
			case _LOOP_PATAMARES:

				bufferSendDataAPP[F("temp")] = auxSendData[1];
				bufferSendDataAPP[F("status")] = auxSendData[2];
				bufferSendDataAPP[F("pwm")] = auxSendData[3];
				bufferSendDataAPP[F("speed")] = auxSendData[4];
				bufferSendDataAPP[F("patamar")] = auxSendData[5];
				bufferSendDataAPP[F("tempo")] = auxSendData[6];

				// Sinaliza adição de código ao dado
				_auxCodeA1 = true;

				break;

			// Contagem de fluxo
			case _COUNT_FLUX:

				bufferSendDataAPP[F("flow")] = auxSendData[1];
				bufferSendDataAPP[F("liters")] = auxSendData[2];

				break;

			// Evento de leitura AC
			case _READ_AC:

				bufferSendDataAPP[F("ac")] = auxSendData[1];

				break;

			// Solicitação dos dados de configuração
			case _GET_CONFIG:

				bufferSendDataAPP[F("qtdeMaxPatamar")] = auxSendData[1];
				bufferSendDataAPP[F("pwmSetpoint")] = auxSendData[2];
				bufferSendDataAPP[F("tempoEnvioBrassagem")] = auxSendData[3];
				bufferSendDataAPP[F("grauPorMinuto")] = auxSendData[4];

				// Sinaliza adição de código ao dado
				_auxCodeA2 = true;

				break;

			// Solicitação do fator de calibração
			case _GET_CAL_FACTOR:

				bufferSendDataAPP[F("calFactor")] = auxSendData[1];

				// Sinaliza adição de código ao dado
				_auxCodeA2 = true;

				break;

			// Solicitação dos dados de configuração do PID
			case _GET_CONFIG_PID:

				bufferSendDataAPP[F("status")] = auxSendData[1];
  				bufferSendDataAPP[F("kp")] = auxSendData[2];
  				bufferSendDataAPP[F("ki")] = auxSendData[3];
  				bufferSendDataAPP[F("kd")] = auxSendData[4];

				// Sinaliza adição de código ao dado
				_auxCodeA2 = true;
				
				break;

			// Solicitação dos dados de tempo de envio da leitura AC
			case _GET_SEND_TIME_AC:

				bufferSendDataAPP[F("time")] = auxSendData[1];

				// Sinaliza adição de código ao dado
				_auxCodeA2 = true;

				break;

			// Solicitação dos dados de tempo de envio da temperatura
			case _GET_SEND_TIME_TEMP:

				bufferSendDataAPP[F("time")] = auxSendData[1];

				// Sinaliza adição de código ao dado
				_auxCodeA2 = true;

				break;

			// Solicitação do ID do dispositivo
			case _GET_ID:

				// Sinaliza adição de código ao dado
				_auxCodeA2 = true;

				break;

			// Resposta para comandos de configuração
			case _SET_RESPONSE:

				// Sinaliza adição de código ao dado
				_auxCodeA1 = true;

				break;

			default: break;
		}
    }
	
	// ------------------------------------------------------- // 

	// Se houver código A1 à ser adicionado
	if(_auxCodeA1)
		code = F("A1");

	// Se houver código A2 à ser adicionado
	else if(_auxCodeA2)
		code = F("A2");
      
     // Adiciona o código a dado
	if(_auxCodeA1 || _auxCodeA2)
	{
		JsonArray error = bufferSendDataAPP.createNestedArray(F("code"));
      	error.add(code);
	}

	// ------------------------------------------------------- // 
	// Envia o dado ao APP

	// serializeJsonPretty(bufferSendDataAPP, Serial);
	serializeJsonPretty(bufferSendDataAPP, Serial1);
	// Faz a limpeza dos buffers
    bufferSendDataAPP.clear(); jsonSendData.clear(); 
    // Recarrega o envio do keep alive
    controlKeepAlive(_LOAD);

    // ------------------------------------------------------- // 
}

