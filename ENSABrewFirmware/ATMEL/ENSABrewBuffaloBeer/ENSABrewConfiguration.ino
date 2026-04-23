//*******************************************************************************************
// Arquivo C/C++ do Projeto Controlador de Patamares -- ENSABrewControll
//-------------------------------------------------------------------------------------------
//
// Arquivo: ENSABrewConfiguration.ino
//
// Autor  : Cláudio B. Pimenta, A.S
// Autor  : Jânio Anselmo, Eng. Me.
// Autor  : Thiago Anselmo, Tecgº
// Autor  : Gabriel da Silva Caetano
//
// CPU    : ATMega2561 - MegaCore
// Clock  : 16MHz
//
//*******************************************************************************************


//WWWWWWWWWW*********************************************************************************
// Função: configSendTimeAC
// Data  : 13/12/2019 13:45
//WWWWWWWWWW*********************************************************************************

void configSendTimeAC(void )
{
	if (_keyPress && (_KEY_LEFT || _KEY_ESC)) 
    {
    	// Reseta a entrada de dados
       	resetDataInput(false);
       	// Altera o passo da máquina
       	passoMaquina = mMENU_TEMPO_ENVIO;
       	// Retorna
       	return;
    }
    	
    // Digita o valor à ser configurado
    entradaValorGeral(configGeral.timeSendReadAC, 0, 255, false);

    // Se finalizou a configuração
    if (_endValue) 
    {
    	// Reseta a entrada de dados
		resetDataInput(false);
        // Configura o valor em questão
        configGeral.timeSendReadAC = valorDigitado;
        // Salva a configuração
        salvaConfigGeral(_CONFIG_MENU);
        // Altera o passo da máquina
        passoMaquina = mMENU_TEMPO_ENVIO;
        // Retorna
        return;
    }
}

//WWWWWWWWWW*********************************************************************************
// Função: configSendTimeTemperature
// Data  : 13/12/2019 13:45
//WWWWWWWWWW*********************************************************************************

void configSendTimeTemperature(void )
{
	if (_keyPress && (_KEY_LEFT || _KEY_ESC)) 
    {  
    	// Reseta a entrada de dados
		resetDataInput(false);
		// Altera o passo da máquina
		passoMaquina = mMENU_TEMPO_ENVIO;
		// Retorna
		return;
    }
    	
    // Digita o valor à ser configurado
    entradaValorGeral(configGeral.timeSendTemp, 0, 255, false);

    // Se finalizou a configuração
    if (_endValue) 
    {
    	// Reseta a entrada de dados
		resetDataInput(false);
        // Configura o valor em questão
        configGeral.timeSendTemp = valorDigitado;
        // Salva a configuração
        salvaConfigGeral(_CONFIG_MENU);
        // Altera o passo da máquina
        passoMaquina = mMENU_TEMPO_ENVIO;
        // Retorna
        return;
    }
}
   	
//WWWWWWWWWW*********************************************************************************
// Função: configMaxPatamares
// Data  : 13/12/2019 13:50
//WWWWWWWWWW*********************************************************************************

void configMaxPatamares(void )
{
	if (_keyPress && (_KEY_LEFT || _KEY_ESC)) 
    {
    	// Reseta a entrada de dados
		resetDataInput(false);
		// Altera o passo da máquina
		passoMaquina = mMENU_CONFIGURACAO;
		// Retorna
		return;
    }

    // Digita o valor à ser configurado
    entradaValorGeral(configGeral.qtdeMaxPatamar, 1, MAX_PATAMAR, false);

    // Se finalizou a configuração
    if (_endValue) 
    {
    	// Reseta a entrada de dados
		resetDataInput(false);
		// Configura o valor em questão
		configGeral.qtdeMaxPatamar = valorDigitado;
		// Salva a configuração
		salvaConfigGeral(_CONFIG_MENU);
		// Altera o passo da máquina
		passoMaquina = mMENU_CONFIGURACAO;
		// Retorna
		return;
    }
}

//WWWWWWWWWW*********************************************************************************
// Function: configOffSet
// Date    : 30/05/2020 - 15:15
//WWWWWWWWWW*********************************************************************************

void configOffSet(void )
{
  	if (_keyPress && (_KEY_LEFT || _KEY_ESC)) 
    {
    	// Reseta a entrada de dados
		resetDataInput(false);
		// Altera o passo da máquina
		passoMaquina = mMENU_CONFIGURACAO;
		// Retorna
		return;
    }

    if(!_endValue)
    {
    	float auxOffSet = CONV_OFFSET_TEMP_FLOAT(configGeral.offSetTemp);
		entradaValorGeral(auxOffSet, MIN_OFFSET_TEMP, MAX_OFFSET_TEMP, true);
    }

    // Se finalizou a configuração
    if (_endValue) 
    {
    	// Reseta a entrada de dados
		resetDataInput(false);
		// Configura o valor em questão
		configGeral.offSetTemp = CONV_OFFSET_TEMP_INT(valorDigitado);
		// Salva a configuração
		salvaConfigGeral(_CONFIG_MENU);
		// Altera o passo da máquina
		passoMaquina = mMENU_CONFIGURACAO;
		// Retorna
		return;
    }
}

//WWWWWWWWWW*********************************************************************************
// Function: configHistereseFactor
// Date    : 30/05/2020 - 15:15
//WWWWWWWWWW*********************************************************************************

void configHistereseFactor(void )
{
  	if (_keyPress && (_KEY_LEFT || _KEY_ESC)) 
    {
    	// Reseta a entrada de dados
		resetDataInput(false);
		// Altera o passo da máquina
		passoMaquina = mMENU_CONFIGURACAO;
		// Retorna
		return;
    }

    if(!_endValue)
    {
    	float auxHistereseFactor = CONV_HIST_FT_FLOAT(configGeral.histereseFactor);
		entradaValorGeral(auxHistereseFactor, MIN_HISTERESE_FACTOR, MAX_HISTERESE_FACTOR, true);
    }

    // Se finalizou a configuração
    if (_endValue) 
    {
    	// Reseta a entrada de dados
		resetDataInput(false);
		// Configura o valor em questão
		configGeral.histereseFactor = CONV_HIST_FT_INT(valorDigitado);
		// Salva a configuração
		salvaConfigGeral(_CONFIG_MENU);
		// Altera o passo da máquina
		passoMaquina = mMENU_CONFIGURACAO;
		// Retorna
		return;
    }
}

//WWWWWWWWWW*********************************************************************************
// Função: configGrauPorMinuto
// Data  : 13/12/2019 13:50
//WWWWWWWWWW*********************************************************************************

void configGrauPorMinuto(void )
{
	if (_keyPress && (_KEY_LEFT || _KEY_ESC))  
    {
    	// Reseta a entrada de dados
		resetDataInput(false);
		// Altera o passo da máquina
		passoMaquina = mMENU_CONFIGURACAO;
		// Retorna
		return;
    }

    // Digita o valor à ser configurado
    entradaValorGeral(configGeral.grauPorMinuto, 1, 2, true);

    // Se finalizou a configuração
    if (_endValue) 
    {
    	// Reseta a entrada de dados
		resetDataInput(false);
		// Configura o valor em questão
		configGeral.grauPorMinuto = valorDigitado;
		// Salva a configuração
		salvaConfigGeral(_CONFIG_MENU);
		// Altera o passo da máquina
		passoMaquina = mMENU_CONFIGURACAO;
		// Retorna
		return;
    }
}

//WWWWWWWWWW*********************************************************************************
// Função: configPWMSetPoint
// Data  : 13/12/2019 13:50
//WWWWWWWWWW*********************************************************************************

void configPWMSetPoint(void )
{
	if (_keyPress && (_KEY_LEFT || _KEY_ESC)) 
	{
		// Reseta a entrada de dados
		resetDataInput(false);
		// Altera o passo da máquina
		passoMaquina = mMENU_CONFIGURACAO;
		// Retorna
		return;
	}

	// Digita o valor à ser configurado
	entradaValorGeral(configGeral.pwmSetpoint, 10, 100, false);

	// Se finalizou a configuração
	if (_endValue) 
	{
		// Reseta a entrada de dados
		resetDataInput(false);
		// Configura o dado em questão
		configGeral.pwmSetpoint = valorDigitado;
		// Salva a configuração
		salvaConfigGeral(_CONFIG_MENU);
		// Altera o passo da máquina
		passoMaquina = mMENU_CONFIGURACAO;
		// Retorna
		return;
	}
} 

//WWWWWWWWWW*********************************************************************************
// Função: configSampleTemperature
// Data  : 13/12/2019 13:55
//WWWWWWWWWW*********************************************************************************

void configSampleTemperature (void )
{
	if (_keyPress && (_KEY_LEFT || _KEY_ESC))  
	{
		// Reseta a entrada de dados
		resetDataInput(false);
		// Altera o passo da máquina
		passoMaquina = mMENU_CONFIGURACAO;
		// Retorna
		return;
	}

	// Entrada do dado
	entradaValorGeral(configGeral.auxPct, 1, 100, false);

	// Se finalizou a configuração
	if (_endValue) 
	{
		// Reseta a entrada de dados
		resetDataInput(false);
		
		// -------------------------------------------------------------------------------------- //
		// Configura o dado em questão
		
		// Configura o percentual
		configGeral.auxPct = (uint8_t)valorDigitado;

		// -------------------------------------------------------------------------------------- //
		// Cálculo da quantidade de amostras baseado no percentual de sensibilidade

		float factor = (MAX_SAMPLE_TEMPERATURE/100.0);
		uint16_t auxSampleTemp = configGeral.sampleTemperature;
		auxSampleTemp = factor*configGeral.auxPct;
		auxSampleTemp = (MAX_SAMPLE_TEMPERATURE - auxSampleTemp) + factor;

		// Ajusta o mínimo valor em 2 amostras e o máximo em MAX_SAMPLE_TEMPERATURE
		configGeral.sampleTemperature = constrain(auxSampleTemp, 2, MAX_SAMPLE_TEMPERATURE);

		// -------------------------------------------------------------------------------------- //

		// DEBUG
		// Serial.println("SP_2: " + String(configGeral.sampleTemperature));

		// Reseta a temperatura atual
		mediaTemperature = 0;
		// Salva a configuração
		salvaConfigGeral(_CONFIG_MENU);
		// Altera o passo da máquina
		passoMaquina = mMENU_CONFIGURACAO;
		// Retorna
		return;
	}
}

//WWWWWWWWWW*********************************************************************************
// Função: generalConfigPID
// Data  : 16/12/2019 09:30
//WWWWWWWWWW*********************************************************************************

void generalConfigPID (void )
{
	// // ----------------------------------------------------------- //
	// // Verifica se não está saindo da opção em questão

	// if (_keyPress && (_KEY_LEFT || _KEY_ESC))
	// {
	// 	// Reseta a entrada de dados
	// 	resetDataInput(false);
	// 	// Retorna ao menu de configuração do PID
	// 	passoMaquina = mMENU_CONFIG_PID;
	// 	// Retorna
	// 	return;
	// }

	// // ----------------------------------------------------------- //
	// // Configurando o status do PID
	
	// if(passoMaquina == mSTATUS_PID)
	// {
	// 	if (_keyPress)  
	// 	{
	// 		// Altera a opção: habilitar/desabilitar
	// 		if(_KEY_UP || _KEY_DOWN)
	// 			auxTempStatusPID = !auxTempStatusPID;
	// 		// Confirma a opção
	// 		else if(_KEY_SELECT)
	// 		{
	// 			// Atualiza o status do PID
	// 			configGeral.statusPID = auxTempStatusPID;

	// 			// Se habilitou o PID, inicializa o controlador
	// 			if(configGeral.statusPID == _ENABLE)
	// 			{
	// 				Input = mediaTemperature;
	// 				Setpoint = 100;

	// 				//turn the PID on
	// 				myPID.SetMode(AUTOMATIC);
	// 			}
	// 		}
	// 	}
	// }

	// // ----------------------------------------------------------- //
	// // Configurando o fator KP

	// else if(passoMaquina == mCONFIG_KP_PID)
	// {	
	// 	// Se ainda não confirmou, segue a rotina de alteração do valor em questão
	// 	if(!_endValue)
	// 		entradaValorGeral(configGeral.kpPID, 0.1, 100, true);
	// }

	// // ----------------------------------------------------------- //
	// // Configurando o fator KI

	// else if(passoMaquina == mCONFIG_KI_PID)
	// {	
	// 	// Se ainda não confirmou, segue a rotina de alteração do valor em questão
	// 	if(!_endValue)
	// 		entradaValorGeral(configGeral.kiPID, 0.1, 100, true);
	// }

	// // ----------------------------------------------------------- //
	// // Configurando o fator KD

	// else if(passoMaquina == mCONFIG_KD_PID)
	// {	
	// 	// Se ainda não confirmou, segue a rotina de alteração do valor em questão
	// 	if(!_endValue)
	// 		entradaValorGeral(configGeral.kdPID, 0.1, 100, true);
	// }

	// // ----------------------------------------------------------- //
	// // Confirmação do dado - armazená-lo

	// if(_KEY_SELECT || _endValue)
	// {
	// 	if(passoMaquina == mCONFIG_KP_PID)
	// 		configGeral.kpPID = valorDigitado;
	// 	else if(passoMaquina == mCONFIG_KI_PID)
	// 		configGeral.kiPID = valorDigitado;
	// 	else if(passoMaquina == mCONFIG_KD_PID)
	// 		configGeral.kdPID = valorDigitado;
		
	// 	// Reseta a entrada de dados
	// 	resetDataInput(false);
	// 	// Armazena a configuração
	// 	salvaConfigGeral(_CONFIG_MENU);
	// 	// Volta ao menu de configuração do PID
	// 	passoMaquina = mMENU_CONFIG_PID;
	// 	// Retorna
	// 	return;
	// }

	// // ----------------------------------------------------------- //
}