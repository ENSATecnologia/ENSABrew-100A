//*******************************************************************************************
// Arquivo C/C++ do Projeto Controlador de Patamares -- ENSABrewControll 
//-------------------------------------------------------------------------------------------
//
// Arquivo: ENSABrewDisplayRoutines.ino
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
// Função: startDisplay
// Data  : 15/11/2019 10:40
//WWWWWWWWWW*********************************************************************************

void startDisplay(void )
{
	// Usando LCD de barramento paralelo
	#if(DISPLAY_TYPE == DISPLAY_LCD_PRL)

		lcd.begin(16, 2);

	// Usando display I2c
	#elif(DISPLAY_TYPE == DISPLAY_LCD_I2C)

		lcd.begin();
		lcd.backlight(); 

	// Usando display OLED
	#elif(DISPLAY_TYPE == DISPLAY_OLED)

		oled.begin(&Adafruit128x64, I2C_OLED_ADDRESS);
		oled.setFont(ZevvPeep8x16);
		oled.set1X();

	#endif
}

//###########################################################################################
// 		  Funções de controle para os displays de LCD - barramento paralelo e I2C
//###########################################################################################

#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)

	//WWWWWWWWWW*********************************************************************************
	// Função: lcdInit
	// Data  : 
	//WWWWWWWWWW*********************************************************************************

	void lcdInit(void ) 
	{
	  lcdInitWrite();
	  lcdClear();
	}

	//WWWWWWWWWW*********************************************************************************
	// Função: lcdInitWrite
	// Data  : 
	//WWWWWWWWWW*********************************************************************************

	void lcdInitWrite(void ) 
	{
	  memcpy(telaAtual, telaBuffer, sizeof(telaAtual));
	}

	//WWWWWWWWWW*********************************************************************************
	// Função: lcdClear
	// Data  : 
	//WWWWWWWWWW*********************************************************************************

	void lcdClear(void ) 
	{
	  for (register int row = 0; row < 2; row++)
	    for (register int col = 0; col < 16; col++)
	      telaBuffer[row][col] = ' ';
	}

	//WWWWWWWWWW*********************************************************************************
	// Função: lcdPrint
	// Data  : 
	//WWWWWWWWWW*********************************************************************************

	void lcdPrint(int row, int col, String text) {
	  row = row - 1;
	  col = col - 1;
	  for (register int n = 0; n < text.length(); n++)
	    if (col < 16)
	      telaBuffer[row][col++] = text[n];
	}

	//WWWWWWWWWW*********************************************************************************
	// Função: lcdWrite
	// Data  : 
	//WWWWWWWWWW*********************************************************************************

	void lcdWrite(void ) 
	{
	  for (register int row = 0; row < 2; row++)
	    for (register int col = 0; col < 16; col++)
	      if (telaAtual[row][col] != telaBuffer[row][col])
		  {
	        char valor = telaBuffer[row][col];
	        lcd.setCursor(col, row);
	        lcd.print(valor);
	        telaAtual[row][col] = valor;
	      }
	}

//###########################################################################################
// 		  				    Funções de controle para o display OLED
//###########################################################################################

#elif(DISPLAY_TYPE == DISPLAY_OLED)

	//WWWWWWWWWW*********************************************************************************
	// Função: oledRestart
	// Data  : 16/01/2019 10:06
	//WWWWWWWWWW*********************************************************************************

	void oledRestart(void )
	{
	  if(passoAnterior != passoMaquina)
	  {
	    oled.clear();
	    oled.setCursor(0, 0);
	    wdtTimesDisplay = 0;
	    _rewriteDisplay = true;
	  }
	  passoAnterior = passoMaquina;
	}

	//WWWWWWWWWW*********************************************************************************
	// Função: resetDisplay
	// Data  : 18/01/2019 15:00
	//WWWWWWWWWW*********************************************************************************

	void resetDisplay(void )
	{
	  wdtRefreshDisplay = 0;
	  _rewriteDisplay = true;
	  _refreshDisplay = false;
	}

#endif

//WWWWWWWWWW*********************************************************************************
// Função: printMenuGlobal
// Data  : 18/11/2019 11:40
//WWWWWWWWWW*********************************************************************************

void printMenuGlobal(void ) 
{
	register uint8_t posFinal; 
	register uint8_t posInicial;

	if (posicaoMenu >= tamanhoMenu)
		posicaoMenu = 0;
	else if (posicaoMenu < 0)
		posicaoMenu = tamanhoMenu - 1;

	#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)

		posFinal = posicaoMenu + 1;
		if (posFinal >= tamanhoMenu)
			posFinal = tamanhoMenu - 1;

		posInicial = posicaoMenu;
		if(posInicial > 0)
		{
			posFinal = posInicial;
			posInicial -= 1;
		}

		lcdInit();
	
	#elif(DISPLAY_TYPE == DISPLAY_OLED)

		static uint8_t posicaoMenuAnterior = 1;

		posFinal = tamanhoMenu;
		posInicial = posicaoMenu;

		if(tamanhoMenu <= 4)
		{
			posInicial = 0;
			posFinal = tamanhoMenu-1;
		}
		else
		{
			if(posInicial <= 3)
			{
				posInicial = 0;
				posFinal = 3;
			}

			else if (posInicial > 3) 
			{
				posFinal = posInicial;
				posInicial = posInicial - 3;
			}
		}

	#endif

	String st; st.reserve(17);
	
	char buffer[16];
	boolean stringProgmem = true;
	
	// Controle das linhas para escrita
	register uint8_t linha;
	#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)

		linha = 1;

	#elif(DISPLAY_TYPE == DISPLAY_OLED)

		linha = 0;

	#endif

	for (register uint8_t ind = posInicial; ind <= posFinal; ind++) 
	{
		if(passoMaquina == mMENU)
		  strcpy_P(buffer, (char *)pgm_read_word(&menu[ind]));
		else if(passoMaquina == mRECEITASEL)
		  strcpy_P(buffer, (char *)pgm_read_word(&menuReceitaSel[ind])); 
		else if(passoMaquina == mMENU_CONFIGURACAO)
		  strcpy_P(buffer, (char *)pgm_read_word(&menuConfig[ind]));
		else if(passoMaquina == mMENU_TEMPO_ENVIO)
		  strcpy_P(buffer, (char *)pgm_read_word(&menuTempoEnvio[ind]));
		else if(passoMaquina == mMENU_CAL_FACTOR)
		  strcpy_P(buffer, (char *)pgm_read_word(&menuCalFactor[ind]));
		else if(passoMaquina == mMENU_CONFIG_PID)
		  strcpy_P(buffer, (char *)pgm_read_word(&menuConfigPID[ind]));
		
		else if(passoMaquina == mRECEITAS) 
		{
		    stringProgmem = false;
		    st = menuReceita[ind];
		}

		if(stringProgmem == false) 
		{
			if (ind == posicaoMenu) 
			{
			    if(opcaoAtual != st) 
			    {
			    	opcaoAtual = st;
			    	if(_connectedAPP)
			    		enviaTela(true);
			    }
			    
			    st = ">" + st;
			}

			#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)
				
				lcdPrint(linha, 1, st);
			
			#elif(DISPLAY_TYPE == DISPLAY_OLED)

				oled.setCursor(0, linha);
        		oled.print(st);

			#endif
		}
		else if(stringProgmem == true) 
		{
			if (ind == posicaoMenu) 
			{
			    if(opcaoAtual != String(buffer)) 
			    {
			    	opcaoAtual = String(buffer);
			    	if(_connectedAPP)
			    		enviaTela(true);
			    }
		    
		    	st = ">" + String(buffer);
		    	
		    	#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)
		    		
		    		lcdPrint(linha, 1, st);
				
				#elif(DISPLAY_TYPE == DISPLAY_OLED)

		    		oled.setCursor(0, linha);
          			oled.print(st);
				
				#endif
		  	}

			else
		    {
		    	#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)
		    		
		    		lcdPrint(linha, 1, buffer);
				
				#elif(DISPLAY_TYPE == DISPLAY_OLED)

		    		oled.setCursor(0, linha);
          			oled.print(buffer);

				#endif
		    }
		}

		#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)

			linha = linha + 1;

		#elif(DISPLAY_TYPE == DISPLAY_OLED)

			linha = linha + 2;

		#endif
		
	}
	
	#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)
	
		lcdWrite();

	#endif

	if(passoAnterior != passoMaquina)
		passoAnterior = passoMaquina;

	#if(DISPLAY_TYPE == DISPLAY_OLED)
	
		posicaoMenuAnterior = posicaoMenu;

	#endif
}

// ===============================================================================( * FIM * )
// ===============================================================================( * FIM * )
// ===============================================================================( * FIM * )
// ===============================================================================( * FIM * )
// ================================(  MENSAGENS  )================================( * FIM * )
// ================================(     DO      )================================( * FIM * )
// ================================(   DISPLAY   )================================( * FIM * )
// ===============================================================================( * FIM * )
// ===============================================================================( * FIM * )
// ===============================================================================( * FIM * )
// ===============================================================================( * FIM * )

//WWWWWWWWWW*********************************************************************************
// Função: printVersao
// Data  : 18/11/2019 10:25
//WWWWWWWWWW*********************************************************************************

void centralWritingScreen(String auxString1, String auxString2)
{
	#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)

		lcdClear();
		lcdPrint(1, 1, auxString1);
		lcdPrint(2, 1, auxString2);
		lcdWrite();

	#elif(DISPLAY_TYPE == DISPLAY_OLED)

		oled.clear();
	    oled.setCursor(0, LINHA_2);
	    oled.println(auxString1);
    	oled.println(auxString2);

	#endif
}

//WWWWWWWWWW*********************************************************************************
// Função: printDisplay
// Data  : 18/11/2019 13:45
//WWWWWWWWWW*********************************************************************************

void printDisplay(String prMsgLinha1, String prMsgLinha2, unsigned long prTempoMsg) 
{
	#if(DISPLAY_TYPE == DISPLAY_OLED)

		oled.clear();
		passoAnterior = passoMaquina;
	
	#endif
	
	// Verifica se o tempo da mensagem está configurado
	if(prTempoMsg > 1)
	{
		// Carrega o tempo da mensagem
		wdtTimesDisplay = prTempoMsg;
		do 
		{
			#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)

				lcdClear();
				lcdPrint(1, 1, prMsgLinha1);
				lcdPrint(2, 1, prMsgLinha2);
				lcdWrite();

			#elif(DISPLAY_TYPE == DISPLAY_OLED)
			
				oled.setCursor(0, LINHA_2);
			  	oled.println(prMsgLinha1);
			  	oled.println(prMsgLinha2);

			#endif
		  
		} while (wdtTimesDisplay > 0); 
		
		#if(DISPLAY_TYPE == DISPLAY_OLED)

			oled.clear();
			_refreshDisplay = true;

		#endif
	}
	else
	{
		#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)

			lcdClear();
			lcdPrint(1, 1, prMsgLinha1);
			lcdPrint(2, 1, prMsgLinha2);
			lcdWrite();

		#elif(DISPLAY_TYPE == DISPLAY_OLED)
			
			oled.setCursor(0, LINHA_2);
		  	oled.println(prMsgLinha1);
		  	oled.println(prMsgLinha2);

		#endif
	}
}

//WWWWWWWWWW*********************************************************************************
// Função: printVersao
// Data  : 18/11/2019 09:40
//WWWWWWWWWW*********************************************************************************

void printVersao(void ) 
{
  if (!_flag3000ms) 
  	centralWritingScreen(SISTEMA, VERSAO);
  else 
  { passoMaquina = mSTANDBY; return; }
}

//WWWWWWWWWW*********************************************************************************
// Função: printBoasVindas
// Data  : 18/11/2019 09:45
//WWWWWWWWWW*********************************************************************************

void printBoasVindas(void ) 
{
	// Auxilia na escrita do display
	boolean _auxWriteDisplay = false;

	// Carrega as mensagens
	String auxString[2];
	auxString[0].reserve(17); 
  	auxString[1].reserve(17);

	if (_msgConvite) 
	{
		_msgConvite = false;
		piscaLed(LED_AMARELO);

		if(_connectedAPP)
			auxString[0] = F("* Sua Cerveja  *");
		else
			auxString[0] = F("  Sua Cerveja   "); 

		auxString[1] = F("Nossa Tecnologia");
		
		_auxWriteDisplay = true;
	}
	else if (_msgENSABrew) 
	{
		_msgENSABrew = false;
		piscaLed(LED_VERMELHO1);

		if(_connectedAPP)
			auxString[0] = F("*ENSABrew-100B*"); 
		else
			auxString[0] = F(" ENSABrew-100B ");
		
		auxString[1] = F(" versao: 1.0.00 ");

		_auxWriteDisplay = true;
	}
	else if (_msgTemperatura) 
	{
		_msgTemperatura = false;
		piscaLed(LED_VERMELHO1);

		if(_connectedAPP)
			auxString[0] = F("*ENSABrew-100B*"); 
		else
			auxString[0] = F(" ENSABrew-100B "); 
		auxString[1] = "     " + String((float)(mediaTemperature), 1) + String((char)223) + "C";

		_auxWriteDisplay = true;
	}

	// Apaga os LEDs que foram acesos
	desligaLeds();

	// Escreve no centro da tela
	if(_auxWriteDisplay)
		centralWritingScreen(auxString[0], auxString[1]);
}

//WWWWWWWWWW*********************************************************************************
// Função: printFluxo
// Data  : 18/11/2019 10:20
//WWWWWWWWWW*********************************************************************************

void printFluxo(void ) 
{
	// Carrega as mensagens
	String auxString[2];
	auxString[0].reserve(17); 
  	auxString[1].reserve(17);

	auxString[0] = "Fluxo:  " + String(flow) + "L/min";
	auxString[1] = "Litros: " + String(liters) + "L";

	// Verifica se o nível não está zerado e se o alarme está ativado
	if ((liters > 0) && (alarmNivel > 0)) 
	{
		// Verifica se atingiu o alarme
		if (liters >= alarmNivel) 
		{
			auxString[0] = F(" Alarme Volume  ");
			auxString[1] = F(" Atingiu Limite!");
		}
	}

	// Escreve no centro da tela
	centralWritingScreen(auxString[0], auxString[1]);
}

//WWWWWWWWWW*********************************************************************************
// Função: printEntradaString
// Data  : 18/11/2019 10:40
//WWWWWWWWWW*********************************************************************************

void printEntradaString(void ) 
{
	String auxStringDisplay;

	if(passoMaquina == mNOME_RECEITA)
	{
		#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)

			// Alterna as mensagens na tela
			if (_changeMsg) 
				auxStringDisplay = F("Nome da RECEITA ");
			else 
				auxStringDisplay = F(" 12 Caracteres ");

		#elif(DISPLAY_TYPE == DISPLAY_OLED)
		  
			if (_changeMsg) 
			{
				auxStringDisplay = String(F(" NOVA RECEITA \n")) +
				                   String(F(" Escolha o Nome \n")) +
				                   String(F("   da RECEITA   "));
			}
			else
			{
				auxStringDisplay = String(F(" NOVA RECEITA \n")) +
				                   String(F("Com um Maximo de\n")) +
				                   String(F(" 12 caracteres! "));
			}
		  
		#endif
	}

	#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)

		lcdClear();
		lcdPrint(1, 1, auxStringDisplay);
		lcdPrint(2, 1, completaEspacos(stDigitado + stChar, 16));
		lcdWrite();

	#elif(DISPLAY_TYPE == DISPLAY_OLED)

		if(_rewriteDisplay || _refreshDisplay)
    	{
    		_refreshDisplay = false;
      		_rewriteDisplay = false;
			oled.clear();
			oled.setCursor(0, LINHA_1);
			oled.println(auxStringDisplay);
		}

	#endif
}

//WWWWWWWWWW*********************************************************************************
// Função: printEntradaValorGeral
// Data  : 23/09/2019 10:13
//WWWWWWWWWW*********************************************************************************

void printEntradaValorGeral(void ) 
{
	boolean _auxDisable = false;
	String stValorMin; stValorMin.reserve(5);
	String stValorMax; stValorMax.reserve(5);
	String auxStringDisplay; auxStringDisplay.reserve(16);

	// Verifica o passo da máquina e define as mensagens de acordo com os mesmos
	if(passoMaquina == mQT_PATAMAR) 
	{
		#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)

			// Alterna as mensagens na tela
			if (_changeMsg) 
				auxStringDisplay = F("   Quantidade   ");
			else
				auxStringDisplay = F("  de SetPoints  ");

		#elif(DISPLAY_TYPE == DISPLAY_OLED)

			auxStringDisplay = String(F("   Quantidade   \n")) +
							   String(F("  de SetPoints  "));

		#endif

		// Carrega os valores máximos e mínimos
		stValorMin = String(MIN_PATAMAR); 
		stValorMax = String(configGeral.qtdeMaxPatamar);
	}

	else if(passoMaquina == mTEMPMAX_PATAMAR) 
	{
		#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)
		
			// Alterna as mensagens na tela
			if (_changeMsg) 
				auxStringDisplay = F(" Temperatura do ");
			else
				auxStringDisplay = "  SetPoint: " + formatNumber(ind + 1);

		#elif(DISPLAY_TYPE == DISPLAY_OLED)

			auxStringDisplay = String(F("   Temperatura   \n")) +
							   String("  do SetPoint ") + String(ind + 1);

		#endif

		// Carrega os valores máximos e mínimos
		stValorMin = String(MIN_TEMPERATURA_PATAMAR); 
		stValorMax = String(MAX_TEMPERATURA_PATAMAR);
	}

	else if(passoMaquina == mMINUTO_PATAMAR) 
	{
		#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)
		
			// Alterna as mensagens na tela
			if (_changeMsg) 
				auxStringDisplay = F("   Minutos do   ");
			else
				auxStringDisplay = "  SetPoint: " + formatNumber(ind + 1);

		#elif(DISPLAY_TYPE == DISPLAY_OLED)

			auxStringDisplay = String(F("Tempo em MINUTOS\n")) +
							   String("  do SetPoint ") + String(ind + 1);

		#endif

		// Carrega os valores máximos e mínimos
		stValorMin = String(MIN_MINUTO_PATAMAR); 
		stValorMax = String(MAX_MINUTO_PATAMAR);
	}

	else if(passoMaquina == mADICIONAR_AGUA) 
	{
		#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)
		
			// Alterna as mensagens na tela
			if (_changeMsg) 
				auxStringDisplay = F("Insira o Volume ");
			else
				auxStringDisplay = F("    em Litros   ");

		#elif(DISPLAY_TYPE == DISPLAY_OLED)

			auxStringDisplay = String(F(" Insira o valor \n")) +
							   String(F("do VOLUME em (L)"));

		#endif

		// Carrega os valores máximos e mínimos
		stValorMin = "1"; 
		// Se estiver em calibração
		if(_auxAdjustCalFactor)
			stValorMax = "10";
		else 
			stValorMax = String(MAX_LITROS);
	}

	else if(passoMaquina == mSET_MAX_PATAMAR) 
	{
		#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)
		
			// Carrega a mensagem
			auxStringDisplay = F(" Total SetPoint ");

		#elif(DISPLAY_TYPE == DISPLAY_OLED)

			auxStringDisplay = String(F("Quantidade Total\n")) +
							   String(F("  de SetPoints  "));

		#endif

		// Carrega os valores máximos e mínimos
		stValorMin = "1";
		stValorMax = String(MAX_PATAMAR);
	}

	else if(passoMaquina == mSET_OFFSET_TEMP) 
	{
		// Carrega a mensagem
		auxStringDisplay = F("OFFSET: ");

		// Carrega os valores máximos e mínimos
		stValorMin = String(MIN_OFFSET_TEMP);
		stValorMax = String(MAX_OFFSET_TEMP);
	}

	else if(passoMaquina == mSET_HISTERESE_FACTOR) 
	{
		// Carrega a mensagem
		auxStringDisplay = F("   Histerese    ");

		// Carrega os valores máximos e mínimos
		stValorMin = String(MIN_HISTERESE_FACTOR);
		stValorMax = String(MAX_HISTERESE_FACTOR);
	}

	else if(passoMaquina == mSET_GRAU_MINUTO) 
	{
		#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)

			// Carrega a mensagem
			auxStringDisplay = F("Graus por Minuto");

		#elif(DISPLAY_TYPE == DISPLAY_OLED)

			auxStringDisplay = String(F("   Valor dos    \n")) +
							   String(F("Graus por Minuto"));

		#endif

		// Carrega os valores máximos e mínimos
		stValorMin = "1";
		stValorMax = String(F("2"));
	}

	else if(passoMaquina == mSET_PWM_SETPOINT) 
	{
		#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)

			// Carrega a mensagem
			auxStringDisplay = F("PWM SetPoint (%)");

		#elif(DISPLAY_TYPE == DISPLAY_OLED)

			auxStringDisplay = String(F("  Valor do PWM  \n")) +
							   String(F(" do SetPoint (%)"));

		#endif

		// Carrega os valores máximos e mínimos
		stValorMin = "10"; stValorMax = "100";
	}

	else if(passoMaquina == mSET_SAMPLE_TEMP) 
	{
		#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)

			// Carrega a mensagem
			auxStringDisplay = F("Sensibilidade(%)");

		#elif(DISPLAY_TYPE == DISPLAY_OLED)

			auxStringDisplay = String(F(" Fator da media \n")) +
							   String(F(" do Sensor NTC "));

		#endif

		// Carrega os valores máximos e mínimos
		stValorMin = "1"; stValorMax = "100";
	}

	else if(passoMaquina == mSET_SEND_TIME_AC) 
	{
		#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)

		 	auxStringDisplay = F("Tempo - Segundos");

		#elif(DISPLAY_TYPE == DISPLAY_OLED)

			auxStringDisplay = String(F("Tempo (s) envio\n")) +
							   String(F(" da Leitura AC  "));

		#endif

		// Carrega os valores máximos e mínimos
		stValorMin = "1"; stValorMax = "255";

		// Sinaliza que foi desabilitado
		if(stDigitado == "0")
			_auxDisable = true;
	}

	else if(passoMaquina == mSET_SEND_TIME_TEMP) 
	{
		#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)
		
			auxStringDisplay = F("Tempo - Segundos");

		#elif(DISPLAY_TYPE == DISPLAY_OLED)

			auxStringDisplay = String(F("Tempo (s) envio\n")) +
							   String(F(" da Temperatura "));

		#endif

		// Carrega os valores máximos e mínimos
		stValorMin = "1"; stValorMax = "255";

		// Sinaliza que foi desabilitado
		if(stDigitado == "0")
			_auxDisable = true;
	}

	else if(passoMaquina == mAUTO_CAL_FACTOR) 
	{
		#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)
			
			// Alterna as mensagens na tela
			if (_changeMsg) 
				auxStringDisplay = F("Valor dos Litros");
			else
				auxStringDisplay = F("Inseridos - Real");

		#elif(DISPLAY_TYPE == DISPLAY_OLED)

			auxStringDisplay = String(F("Valor dos Litros\n")) +
							   String(F("Inseridos - Real"));

		#endif

		// Carrega os valores máximos e mínimos
		stValorMin = "1"; 
		stValorMax = String(MAX_LITROS);
	}

	else if(passoMaquina == mMANUAL_CAL_FACTOR) 
	{
		#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)

			// Alterna as mensagens na tela
			if (_changeMsg) 
				auxStringDisplay = F(" Valor do Fator ");
			else
				auxStringDisplay = F("  de Calibracao ");

		#elif(DISPLAY_TYPE == DISPLAY_OLED)

			auxStringDisplay = String(F(" Valor do Fator \n")) +
							   String(F("  de Calibracao "));

		#endif

		// Carrega os valores máximos e mínimos
		stValorMin = "1"; 
		stValorMax = String(MAX_CAL_FACTOR);
	}

	else if(passoMaquina == mCONFIG_KP_PID)
	{
		// Carrega os valores máximos e mínimos
		stValorMin = "0.1"; stValorMax = "100";

		#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)

					// Alterna as mensagens na tela
			if (_changeMsg) 
				auxStringDisplay = F("Fator do PID: Kp");
			else
				auxStringDisplay = "Min:" + stValorMin + "|Max:" + stValorMax;

		#elif(DISPLAY_TYPE == DISPLAY_OLED)

			auxStringDisplay = String(F(" Valor do Fator \n")) +
							   String(F("   do PID: Kp   "));

		#endif
	}

	else if(passoMaquina == mCONFIG_KI_PID)
	{
		// Carrega os valores máximos e mínimos
		stValorMin = "0.1"; stValorMax = "100";

		#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)
			
			// Alterna as mensagens na tela
			if (_changeMsg) 
				auxStringDisplay = F("Fator do PID: Ki");
			else
				auxStringDisplay = "Min:" + stValorMin + "|Max:" + stValorMax;

		#elif(DISPLAY_TYPE == DISPLAY_OLED)

			auxStringDisplay = String(F(" Valor do Fator \n")) +
							   String(F("   do PID: Ki   "));

		#endif
	}

	else if(passoMaquina == mCONFIG_KD_PID)
	{
		// Carrega os valores máximos e mínimos
		stValorMin = "0.1"; stValorMax = "100";

		#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)
			
			// Alterna as mensagens na tela
			if (_changeMsg) 
				auxStringDisplay = F("Fator do PID: Kd");
			else
				auxStringDisplay = "Min:" + stValorMin + "|Max:" + stValorMax;

		#elif(DISPLAY_TYPE == DISPLAY_OLED)

			auxStringDisplay = String(F(" Valor do Fator \n")) +
							   String(F("   do PID: Kd   "));

		#endif
	}

	#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)

		// Mostra na tela as mensagens       
		lcdClear();
		lcdPrint(1, 1, auxStringDisplay);

		// Se algo foi desabilitado
		if(_auxDisable)
			lcdPrint(2, 1, F("  Desabilitado  "));
		else 
		{
			if (passoMaquina == mADICIONAR_AGUA)//  || 
				// passoMaquina == mSET_OFFSET_TEMP || 
				// passoMaquina == mSET_HISTERESE_FACTOR)
				lcdPrint(2, 1, stValorMin + " a " + stValorMax + ": " + stDigitado);
			else if(passoMaquina == mCONFIG_KP_PID || passoMaquina == mCONFIG_KI_PID || passoMaquina == mCONFIG_KD_PID)
				lcdPrint(2, 1, "Valor: " + stDigitado);

			// Controle de OFFSET
			else if(passoMaquina == mSET_OFFSET_TEMP)
			{
				lcdPrint(1, 1, String("Atual   : " + String((float)mediaTemperature, 1)) + "C");
				float auxValue = mediaTemperature + stDigitado.toFloat();
				lcdPrint(2, 1, String("Ajustado: " + String((float)auxValue, 1)) + "C");
			}

			else
				lcdPrint(2, 1, "De " + stValorMin + " a " + stValorMax + ": " + stDigitado);
		}

		lcdWrite();

	#elif(DISPLAY_TYPE == DISPLAY_OLED)

		if(_rewriteDisplay || _refreshDisplay)
    	{
    		_refreshDisplay = false;
      		_rewriteDisplay = false;
			oled.clear();
		    oled.setCursor(0, LINHA_1);
		    oled.println(auxStringDisplay);

	    	oled.setCursor(0, LINHA_4);
	    	// Se algo foi desabilitado
			if(_auxDisable)
				oled.println(F("  Desabilitado  "));
			else 
			{
				if (passoMaquina == mADICIONAR_AGUA)
					oled.println(" " + stValorMin + " a " + stValorMax + ": " + stDigitado);
				else
					oled.println("De " + stValorMin + " a " + stValorMax + ": " + stDigitado);
			}
		}

	#endif
}

//WWWWWWWWWW*********************************************************************************
// Função: printSimNao
// Data  : 12/11/2019 12:55
//WWWWWWWWWW*********************************************************************************

void printSimNao(void ) 
{
	String auxStringDisplay; auxStringDisplay.reserve(17);

	if(passoMaquina == mLIMPA_EEPROM)
	{
		#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)

			// Se ainda não respondeu se deseja ou não apagar a EEPROM
			if(!auxResp)
			{
			  	// Alterna as mensagens na tela
			    if (_changeMsg) 
			    	auxStringDisplay = F("Deseja Restaurar");
			    else
			    	auxStringDisplay = F("Padrao/Fabrica? ");
			}
			// Se já respondeu, verifica se deseja, ou não apagar as receitas
			else
			{
				// Alterna as mensagens na tela
			    if (_changeMsg) 
			    	auxStringDisplay = F("Deseja Apagar as");
			    else
			    	auxStringDisplay = F("Receitas Salvas?");
			}
		
		#elif(DISPLAY_TYPE == DISPLAY_OLED)

			// Se ainda não respondeu se deseja ou não apagar a EEPROM
			if(!auxResp)
			{
				auxStringDisplay = String(F("Deseja Restaurar\n")) +
								   String(F("Padrao/Fabrica? "));
			}

		#endif

	}

	else if(passoMaquina == mAPAGA_RECEITA)
	{
		Receita receita = regReceita[idReceitaSel];

		#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)

		    // Alterna as mensagens na tela
		    if (_changeMsg) 
		    	auxStringDisplay = F("Apagar a Receita");
		    else
		    	auxStringDisplay = String(receita.nome) + "?";

		#elif(DISPLAY_TYPE == DISPLAY_OLED)

			auxStringDisplay = String(F("Apagar a Receita\n")) +
							   String(String(receita.nome) + "?");

		#endif
	}

	#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)

		lcdClear();
		lcdPrint(1, 1, auxStringDisplay);
		lcdPrint(2, 1, "  Up/Down: " + stResp);
		lcdWrite();

	#elif(DISPLAY_TYPE == DISPLAY_OLED)

		if(_rewriteDisplay || _refreshDisplay)
    	{
    		_refreshDisplay = false;
      		_rewriteDisplay = false;

			oled.clear();
		    oled.setCursor(0, LINHA_1);
		    oled.println(auxStringDisplay);

	    	oled.setCursor(0, LINHA_4);
	    	oled.println("  Up/Down: " + stResp);
	    }

	#endif
}

//WWWWWWWWWW*********************************************************************************
// Função: printBrassagem
// Data  : 18/11/2019 11:20
//WWWWWWWWWW*********************************************************************************

void printBrassagem(String auxString1, String auxString2)
{
	// Caso for escrita do display OLED
	#if(DISPLAY_TYPE == DISPLAY_OLED)

		oledRestart();
  
		if(_refreshDisplay)
		{
			resetDisplay();
			centralWritingScreen(auxString1, auxString2);
			return;
		}

	#endif

	// Caso for escrita do display LCD
	centralWritingScreen(auxString1, auxString2);
}

//WWWWWWWWWW*********************************************************************************
// Função: printConfigStatusPID
// Data  : 13/12/2019 14:25
//WWWWWWWWWW*********************************************************************************

void printConfigStatusPID(void )
{	
	// Carrega as mensagens
  	String auxString[2];

  	auxString[0].reserve(17); 
  	auxString[1].reserve(17);

	auxString[0] = F("   Status PID   ");

	if(auxTempStatusPID)
		auxString[1] = F("   HABILITADO   ");
	else
		auxString[1] = F("  DESABILITADO  ");
	
   	// Caso for escrita do display LCD
	centralWritingScreen(auxString[0], auxString[1]);

}

//WWWWWWWWWW*********************************************************************************
// Função: printVisualizaReceita
// Data  : 14/01/2020 11:45
//WWWWWWWWWW*********************************************************************************

void printVisualizaReceita(void )
{
	// Carrega as mensagens
  	String auxString[2];
  	
  	auxString[0].reserve(17); 
  	auxString[1].reserve(17);

  	// Verifica o ponto de visualização
	switch(auxVisualizaReceita)
	{
		case mNOME_RECEITA:

			#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)
			
				auxString[0] = F("Nome da RECEITA");
				auxString[1] = String(F("-> ")) + String(prReceita.nome);
			
			#elif(DISPLAY_TYPE == DISPLAY_OLED)

			#endif

		break;

		case mQT_PATAMAR:

			#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)
			
				// Alterna as mensagens na tela
				if (_changeMsg) 
					auxString[0] = F("   Quantidade   ");
				else
					auxString[0] = F("  de SetPoints  ");

				auxString[1] = String(F("-> ")) + String(prReceita.qtdePatamar);
			
			#elif(DISPLAY_TYPE == DISPLAY_OLED)
			
			#endif	

		break;

		case mTEMPMAX_PATAMAR:

			#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)
		
			// Alterna as mensagens na tela
			if (_changeMsg) 
				 auxString[0] = F(" Temperatura do ");
			else auxString[0] = String(F("  SetPoint: ")) + formatNumber(ind + 1); 

			auxString[1] = String(F("-> ")) + String(prReceita.patamar[ind].temperaturaMax) + String(char(223)) + String(F("C"));

			#elif(DISPLAY_TYPE == DISPLAY_OLED)

			#endif

		break;

		case mMINUTO_PATAMAR:

			#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)
		
				if (_changeMsg) 
				 auxString[0] = F("   Minutos do   ");
				else auxString[0] = String(F("  SetPoint: ")) + formatNumber(ind + 1); 
				auxString[1] = String(F("-> ")) + String(prReceita.patamar[ind].tempo) + String(F(" minutos"));

			#elif(DISPLAY_TYPE == DISPLAY_OLED)

			#endif

		break;
	}

	// Caso for escrita do display LCD
	centralWritingScreen(auxString[0], auxString[1]);
}