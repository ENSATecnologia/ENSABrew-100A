//*******************************************************************************************
// Arquivo C/C++ do Projeto Controlador de Patamares -- ENSABrewControll 
//-------------------------------------------------------------------------------------------
//
// Arquivo: ENSABrewDisplayRoutines.ino
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

	String st;
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
		else if(passoMaquina == mMENU_USER_CONFIG)
			strcpy_P(buffer, (char *)pgm_read_word(&userConfigMenu[ind]));
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
			auxString[0] = F("*ENSABrew-100A*"); 
		else
			auxString[0] = F(" ENSABrew-100A ");
		
		auxString[1] = F(" versao: 1.0.00 ");

		_auxWriteDisplay = true;
	}
	else if (_msgTemperatura) 
	{
		_msgTemperatura = false;
		piscaLed(LED_VERMELHO1);

		if(_connectedAPP)
			auxString[0] = F("*ENSABrew-100A*"); 
		else
			auxString[0] = F(" ENSABrew-100A "); 
		auxString[1] = "    " + String((float)(mediaTemperature), 2) + String((char)223) + "C";

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

	if(passoMaquina == mSENHA_CONFIGURACAO)
	{

		#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)
		  
		  	auxStringDisplay = F(" Digite a SENHA ");

		#elif(DISPLAY_TYPE == DISPLAY_OLED)

		  	auxStringDisplay = String(F(" Digite a Senha \n")) +
		                       String(F(" Para Acessar as \n")) +
		                       String(F(" Configuracoes "));

		#endif
	}

	else if(passoMaquina == mNOME_RECEITA)
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

	else if(passoMaquina == mNOME_ADICAO) 
	{

		#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)

			// Alterna as mensagens na tela
			if (_changeMsg) 
				auxStringDisplay = "Nome da ADICAO " + String(ind + 1);
			else
				auxStringDisplay = F(" 12 Caracteres ");

		#elif(DISPLAY_TYPE == DISPLAY_OLED)
		  
			if (_changeMsg) 
			{
				auxStringDisplay = String(F(" NOVA RECEITA \n")) +
				                   String(F(" Escolha o Nome \n")) +
				                   String(F("   da ADICAO ")) + String(ind + 1);
			}
			else
			{
				auxStringDisplay = String(F(" NOVA RECEITA \n")) +
				                   String(F("Com um Maximo de\n")) +
				                   String(F(" 12 Caracteres! "));
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
	String stValorMin;
	String stValorMax;
	String auxStringDisplay;

	// Verifica o passo da máquina e define as mensagens de acordo com os mesmos
	if(passoMaquina == mQT_PATAMAR) 
	{
		#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)

			// Alterna as mensagens na tela
			if (_changeMsg) 
				auxStringDisplay = F("   Quantidade   ");
			else
				auxStringDisplay = F("  de PATAMARES  ");

		#elif(DISPLAY_TYPE == DISPLAY_OLED)

			auxStringDisplay = String(F("   Quantidade   \n")) +
							   String(F("  de PATAMARES  "));

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
				auxStringDisplay = F("  Temperatura  ");
			else
				auxStringDisplay = "  do PATAMAR " + String(ind + 1);

		#elif(DISPLAY_TYPE == DISPLAY_OLED)

			auxStringDisplay = String(F("   Temperatura   \n")) +
							   String("  do PATAMAR ") + String(ind + 1);

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
				auxStringDisplay = "   PATAMAR " + String(ind + 1);

		#elif(DISPLAY_TYPE == DISPLAY_OLED)

			auxStringDisplay = String(F("Tempo em MINUTOS\n")) +
							   String("  do PATAMAR ") + String(ind + 1);

		#endif

		// Carrega os valores máximos e mínimos
		stValorMin = String(MIN_MINUTO_PATAMAR); 
		stValorMax = String(MAX_MINUTO_PATAMAR);
	}

	else if(passoMaquina == mTEMPMAX_FERVURA) 
	{
		#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)

			// Alterna as mensagens na tela
			if (_changeMsg) 
				auxStringDisplay = F("  Temperatura  ");
			else
				auxStringDisplay = F("   da FERVURA   ");

		#elif(DISPLAY_TYPE == DISPLAY_OLED)

			auxStringDisplay = String(F("  Temperatura  \n")) +
							   String(F("   da FERVURA   "));

		#endif

		// Carrega os valores máximos e mínimos
		stValorMin = String(MIN_TEMPERATURA_FERVURA); 
		stValorMax = String(MAX_TEMPERATURA_FERVURA);
	}

	else if(passoMaquina == mMAXMIN_FERVURA) 
	{
		#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)
		
			// Alterna as mensagens na tela
			if (_changeMsg) 
				auxStringDisplay = F("    Minutos    ");
			else
				auxStringDisplay = F("   da FERVURA   ");

		#elif(DISPLAY_TYPE == DISPLAY_OLED)

			auxStringDisplay = String(F("Tempo em MINUTOS\n")) +
							   String(F("   da FERVURA   "));

		#endif

		// Carrega os valores máximos e mínimos
		stValorMin = String(MIN_MINUTO_FERVURA); 
		stValorMax = String(MAX_MINUTO_FERVURA);
	}

	else if(passoMaquina == mQT_ADICAO) 
	{
		#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)

			// Alterna as mensagens na tela
			if (_changeMsg) 
				auxStringDisplay = F("  Numero total  ");
			else
				auxStringDisplay = F("   de ADICOES   ");

		#elif(DISPLAY_TYPE == DISPLAY_OLED)

			auxStringDisplay = String(F("  Numero total  \n")) +
							   String(F("   de ADICOES   "));

		#endif

		// Carrega os valores máximos e mínimos
		stValorMin = String(MIN_ADICAO); 
		stValorMax = String(MAX_ADICAO);
	}

	else if(passoMaquina == mGRAMAS_ADICAO) 
	{
		#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)

			// Alterna as mensagens na tela
			if (_changeMsg) 
				auxStringDisplay = F("Quantidade - (g)");
			else
				auxStringDisplay = "   da ADICAO " + String(ind + 1);

		#elif(DISPLAY_TYPE == DISPLAY_OLED)

			auxStringDisplay = String(F("Quantidade - (g)\n")) +
							   String("   da ADICAO " + String(ind + 1));

		#endif

		// Carrega os valores máximos e mínimos
		stValorMin = String(MIN_GRAMAS_ADICAO); 
		stValorMax = String(MAX_GRAMAS_ADICAO);
	}

	else if(passoMaquina == mMINUTOS_ADICAO) 
	{	
		#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)

			// Alterna as mensagens na tela
			if (_changeMsg) 
				auxStringDisplay = F("Tempo em MINUTOS");
			else
				auxStringDisplay = "   da ADICAO " + String(ind + 1);

		#elif(DISPLAY_TYPE == DISPLAY_OLED)

			auxStringDisplay = String(F("Tempo em MINUTOS\n")) +
							   String("   da ADICAO " + String(ind + 1));

		#endif

		// Carrega o valor mínimo
		stValorMin = String(MIN_MINUTOS_ADICAO); 

		// Carrega o valor máximo
		if (!_endValue) 
		{
			if (prOper == _EDITAR_RECEITA)
			{
				if (ind == 0)
					stValorMax = String(prReceita.qtdeMinFervura);
				else
					stValorMax = String(prReceita.adicao[ind - 1].tempo);
			}
			else 
			{
				if (ind == 0)
					stValorMax = String(prReceita.qtdeMinFervura);
				else
					stValorMax = String(prReceita.adicao[ind - 1].tempo);
			}
		} 
	}

	else if(passoMaquina == mQT_WHIRLPOOL) 
	{
		#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)

			// Alterna as mensagens na tela
			if (_changeMsg) 
				auxStringDisplay = F("   Quantidade   ");
			else
				auxStringDisplay = F("  de WHIRLPOOL  ");

		#elif(DISPLAY_TYPE == DISPLAY_OLED)

			auxStringDisplay = String(F("   Quantidade   \n")) +
							   String(F("  de WHIRLPOOL  "));

		#endif

		// Carrega os valores máximos e mínimos
		stValorMin = String(MIN_WHIRLPOOL); 
		stValorMax = String(MAX_WHIRLPOOL);
	}

	else if(passoMaquina == mMINUTO_WHIRLPOOL) 
	{
		#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)

			// Alterna as mensagens na tela
			if (_changeMsg) 
				auxStringDisplay = F("Tempo em MINUTOS");
			else
				auxStringDisplay = "WHIRLPOOL " + actualWhirlpool(ind);

		#elif(DISPLAY_TYPE == DISPLAY_OLED)

			auxStringDisplay = String(F("Tempo em MINUTOS\n")) +
							   String(" do WHIRLPOOL " + String(ind + 1));

		#endif

		// Carrega os valores máximos e mínimos
		stValorMin = String(MIN_MINUTOS_WHIRLPOOL); 
		stValorMax = String(MAX_MINUTOS_WHIRLPOOL);
	}

	else if(passoMaquina == mQT_DESCANSO) 
	{
		#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)

			// Alterna as mensagens na tela
			if (_changeMsg) 
				auxStringDisplay = F("   Quantidade   ");
			else
				auxStringDisplay = F("  de DESCANSO  ");

		#elif(DISPLAY_TYPE == DISPLAY_OLED)

			auxStringDisplay = String(F("   Quantidade   \n")) +
							   String(F("  de DESCANSO  "));

		#endif	

		// Carrega os valores máximos e mínimos
		stValorMin = String(MIN_DESCANSO); 
		stValorMax = String(MAX_DESCANSO);
	}

	else if(passoMaquina == mMINUTO_DESCANSO) 
	{
		#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)

			// Alterna as mensagens na tela
			if (_changeMsg) 
				auxStringDisplay = F("Tempo em MINUTOS");
			else
				auxStringDisplay = F("   do DESCANSO  ");

		#elif(DISPLAY_TYPE == DISPLAY_OLED)

			auxStringDisplay = String(F("Tempo em MINUTOS\n")) +
							   String(F("   do DESCANSO  "));

		#endif

		// Carrega os valores máximos e mínimos
		stValorMin = String(MIN_MINUTOS_DESCANSO); 
		stValorMax = String(MAX_MINUTOS_DESCANSO);
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
			auxStringDisplay = F(" Total Patamares ");

		#elif(DISPLAY_TYPE == DISPLAY_OLED)

			auxStringDisplay = String(F("Quantidade Total\n")) +
							   String(F("  de PATAMARES  "));

		#endif

		// Carrega os valores máximos e mínimos
		stValorMin = "1";
		stValorMax = String(MAX_PATAMAR);
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
			auxStringDisplay = F("Media Sensor NTC");

		#elif(DISPLAY_TYPE == DISPLAY_OLED)

			auxStringDisplay = String(F(" Fator da media \n")) +
							   String(F(" do Sensor NTC "));

		#endif

		// Carrega os valores máximos e mínimos
		stValorMin = "2"; 
		stValorMax = String(MAX_SAMPLE_TEMPERATURE);
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
			if (passoMaquina == mADICIONAR_AGUA)
				lcdPrint(2, 1, " " + stValorMin + " a " + stValorMax + ": " + stDigitado);
			else if(passoMaquina == mCONFIG_KP_PID || passoMaquina == mCONFIG_KI_PID || passoMaquina == mCONFIG_KD_PID)
				lcdPrint(2, 1, "Valor: " + stDigitado);
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
	String auxStringDisplay;

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

	else if(passoMaquina == mCONTINUA_PRODUCAO)
	{
		#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)

		    // Alterna as mensagens na tela
		    if (_changeMsg) 
		    	auxStringDisplay = F("Deseja Continuar");
		    else
		    	auxStringDisplay = F("  a Brassagem?  ");

			#elif(DISPLAY_TYPE == DISPLAY_OLED)

				auxStringDisplay = String(F("Deseja Continuar\n")) +
							   	   String(F("  a Brassagem?  "));

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
// Função: printMenuMalte
// Data  : 18/11/2019 11:10
//WWWWWWWWWW*********************************************************************************

void printMenuMalte(void )
{
	// Carrega as mensagens
	String auxString[2];
	auxString[0] = F(" Vamos Despejar ");
	auxString[1] = F("    o MALTE?    ");

	// Auxilia na apresentação das mensagens de tela em brassagem
	printBrassagem(auxString[0], auxString[1]);
}

//WWWWWWWWWW*********************************************************************************
// Função: printMenuFervura
// Data  : 18/11/2019 11:10
//WWWWWWWWWW*********************************************************************************

void printMenuFervura(void )
{
	// Carrega as mensagens
  	String auxString[2];
	auxString[0] = F(" Start FERVURA? ");
	auxString[1] = F("  Pressione OK  ");

	// Auxilia na apresentação das mensagens de tela em brassagem
	printBrassagem(auxString[0], auxString[1]);
}

//WWWWWWWWWW*********************************************************************************
// Função: printAdicao
// Data  : 18/11/2019 11:10
//WWWWWWWWWW*********************************************************************************

void printAdicao(void )
{
	// Carrega as mensagens
	String auxString[2];
	auxString[0] = F(" Inserir ADICAO ");  

	if (_changeMsg)
		auxString[1] = "-> " + String(adicaoFervura[numAdicao].nome);
	else
		auxString[1] = "-> " + String((float)(adicaoFervura[numAdicao].qtde), 2) + "g";

    // Auxilia na apresentação das mensagens de tela em brassagem
	printBrassagem(auxString[0], auxString[1]);
}

//WWWWWWWWWW*********************************************************************************
// Função: printMenuWhirlpool
// Data  : 18/11/2019 11:10
//WWWWWWWWWW*********************************************************************************

void printMenuWhirlpool(void )
{
	// Carrega as mensagens
  	String auxString[2];
	auxString[0] = F("Start WHIRLPOOL?");
	auxString[1] = F("  Pressione OK  ");

    // Auxilia na apresentação das mensagens de tela em brassagem
	printBrassagem(auxString[0], auxString[1]);
}

//WWWWWWWWWW*********************************************************************************
// Função: printMenuDescanso
// Data  : 18/11/2019 11:10
//WWWWWWWWWW*********************************************************************************

void printMenuDescanso(void )
{
  	// Carrega as mensagens
  	String auxString[2];
	auxString[0] = F("Start DESCANSO ?");
	auxString[1] = F("  Pressione OK  ");
	
    // Auxilia na apresentação das mensagens de tela em brassagem
	printBrassagem(auxString[0], auxString[1]);
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
					auxString[0] = F("  de PATAMARES  ");

				auxString[1] = String(F("-> ")) + String(prReceita.qtdePatamar);
			
			#elif(DISPLAY_TYPE == DISPLAY_OLED)
			
			#endif	

		break;

		case mTEMPMAX_PATAMAR:

			#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)
		
			// Alterna as mensagens na tela
			if (_changeMsg) 
				 auxString[0] = F("  Temperatura  ");
			else auxString[0] = String(F(" do PATAMAR: ")) + String(ind + 1); 

			auxString[1] = String(F("-> ")) + String(prReceita.patamar[ind].temperaturaMax) + String(char(223)) + String(F("C"));

			#elif(DISPLAY_TYPE == DISPLAY_OLED)

			#endif

		break;

		case mMINUTO_PATAMAR:

			#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)
		
				auxString[0] = String(F("Tempo PATAMAR ")) + String(ind + 1); 
				auxString[1] = String(F("-> ")) + String(prReceita.patamar[ind].tempo) + String(F(" minutos"));

			#elif(DISPLAY_TYPE == DISPLAY_OLED)

			#endif

		break;

		case mTEMPMAX_FERVURA:

			#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)
		
				// Alterna as mensagens na tela
				if (_changeMsg) 
					auxString[0] = F("  Temperatura  ");
				else
					auxString[0] = F("   da FERVURA   ");
				
				auxString[1] = String(F("-> ")) + String(prReceita.tempFervura) + String(char(223)) + String(F("C"));

			#elif(DISPLAY_TYPE == DISPLAY_OLED)

			#endif

		break;

		case mMAXMIN_FERVURA:

			#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)
		
				auxString[0] = F("Tempo da FERVURA"); 
				auxString[1] = String(F("-> ")) + String(prReceita.qtdeMinFervura) + String(F(" minutos"));

			#elif(DISPLAY_TYPE == DISPLAY_OLED)

			#endif

		break;

		case mQT_ADICAO:

			#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)
		
				// Alterna as mensagens na tela
				if (_changeMsg) 
					auxString[0] = F("   Quantidade   ");
				else
					auxString[0] = F("   de ADICOES   ");

				auxString[1] = String(F("-> ")) + String(prReceita.qtdeAdicao);

			#elif(DISPLAY_TYPE == DISPLAY_OLED)

			#endif

		break;

		case mNOME_ADICAO:

			#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)
		
				auxString[0] = String(F("Nome da ADICAO ")) + String(ind + 1); 
				auxString[1] = String(F("-> ")) + String(prReceita.adicao[ind].nome);

			#elif(DISPLAY_TYPE == DISPLAY_OLED)

			#endif

		break;

		case mGRAMAS_ADICAO:

			#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)
		
				// Alterna as mensagens na tela
				if (_changeMsg) 
					auxString[0] = F("   Quantidade   ");
				else
					auxString[0] = String(F("  da ADICAO: ")) + String(ind + 1);
 
				auxString[1] = String(F("-> ")) + String(prReceita.adicao[ind].qtde) + String(F(" gramas"));

			#elif(DISPLAY_TYPE == DISPLAY_OLED)

			#endif

		break;

		case mMINUTOS_ADICAO:

			#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)
		
				auxString[0] = String(F("Tempo ADICAO: ")) + String(ind + 1);
				auxString[1] = String(F("-> ")) + String(prReceita.adicao[ind].tempo) + String(F(" minutos"));

			#elif(DISPLAY_TYPE == DISPLAY_OLED)

			#endif

		break;

        case mQT_WHIRLPOOL:

        	#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)
		
				// Alterna as mensagens na tela
				if (_changeMsg) 
					auxString[0] = F("   Quantidade   ");
				else
					auxString[0] = F("  de WHIRLPOOL  ");

				auxString[1] = String(F("-> ")) + String(prReceita.qtdeWhirlpool);

			#elif(DISPLAY_TYPE == DISPLAY_OLED)

			#endif

		break;

        case mMINUTO_WHIRLPOOL:

        	#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)
		
				// Alterna as mensagens na tela
				if (_changeMsg) 
					auxString[0] = F("    Tempo do    ");
				else
					auxString[0] = String(F("WHIRLPOOL ")) + actualWhirlpool(ind); 
				
				auxString[1] = String(F("-> ")) + String(prReceita.whirlpool[ind].tempo) + String(F(" minutos"));

			#elif(DISPLAY_TYPE == DISPLAY_OLED)

			#endif

		break;

        case mQT_DESCANSO:

        	#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)
		
				// Alterna as mensagens na tela
				if (_changeMsg) 
					auxString[0] = F("   Quantidade   ");
				else
					auxString[0] = F("   de DESCANSO  ");

				auxString[1] = String(F("-> ")) + String(prReceita.qtdeDescanso);

			#elif(DISPLAY_TYPE == DISPLAY_OLED)

			#endif

		break;

        case mMINUTO_DESCANSO:

        	#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)
		
				auxString[0] = F(" Tempo DESCANSO ");
				auxString[1] = String(F("-> ")) + String(prReceita.descanso[ind].tempo) + String(F(" minutos"));

			#elif(DISPLAY_TYPE == DISPLAY_OLED)

			#endif

		break;
	}

	// Caso for escrita do display LCD
	centralWritingScreen(auxString[0], auxString[1]);
}