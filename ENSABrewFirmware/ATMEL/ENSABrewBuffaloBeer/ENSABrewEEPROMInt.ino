//*******************************************************************************************
// Arquivo C/C++ do Projeto Controlador de Patamares -- ENSABrewControll
//-------------------------------------------------------------------------------------------
//
// Arquivo: ENSABrewEEPROMInt.ino
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
// Function: clearEeprom
// Date    : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void clearEeprom(void ) 
{
  if (ERASE_EEPROM_USER) 
  {
    // Disable interrupts
    disableInterrupts(); 

    for (register uint16_t i = NETEEPROM_END; i < END_CONFIG; i++)
      EEPROM.write(i, -1);

    // Enable interrupts
    enableInterrupts(); 
  }
  if (ERASE_EEPROM_CONFIG) 
  {
    // Disable interrupts
    disableInterrupts(); 

    for (register uint16_t i = END_CONFIG; i < EEPROM.length(); i++)

      EEPROM.write(i, -1);

    // Enable interrupts
    enableInterrupts(); 
  }
}

//WWWWWWWWWW*********************************************************************************
// Function: leConfigGeral
// Date    : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void leConfigGeral(void ) 
{
  // Disable interrupts
  disableInterrupts(); 

  EEPROM.get(END_CONFIG, configGeral);

  if (configGeral.qtdeMaxPatamar == 255) 
  {
    configGeral.qtdeMaxPatamar = DEFAULT_MAX_PATAMAR;
    configGeral.grauPorMinuto = DEFAULT_GRAU_MINUTO;
    configGeral.pwmSetpoint = DEFAULT_PWM;
    configGeral.tempoEnvioBrassagem = 1; // 1 segundo

    configGeral.auxPct = 50; // 50%
    configGeral.sampleTemperature = DEFAULT_SAMPLE_TEMP;
    configGeral.calFactor = 4.5; // Fator de calibração padrão
    configGeral.timeSendReadAC = 0; // Desabilitado por padrão
    configGeral.timeSendTemp = 0;

    // configGeral.statusPID = _DISABLE;
    // configGeral.kpPID = 2.0;
    // configGeral.kiPID = 5.0;
    // configGeral.kdPID = 1.0;

    configGeral.offSetTemp = 0.0;
    configGeral.histereseFactor = 3;

    String idModule = DEFAULT_ID_MODULE;
    // idModule.toCharArray(configGeral.idModule, 10);
  }

  // Enable interrupts
  enableInterrupts(); 
}

//WWWWWWWWWW*********************************************************************************
// Function: salvaConfigGeral
// Date    : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void salvaConfigGeral(boolean auxCommand) 
{
  // Disable interrupts
  disableInterrupts(); 
  // Salva as configurações
  EEPROM.put(END_CONFIG, configGeral);
  // Enable interrupts
  enableInterrupts(); 

  // Se for configuração por MENU, envia a tela 
  if(auxCommand == _CONFIG_MENU)
  {
    bipOK();
    // enviaTelaStatus(F("Configuracao salva"));
    printDisplay(F("  Configuracao  "), F("Salva, Sucesso!!"), TEMPO_CONF);
  }
}

//WWWWWWWWWW*********************************************************************************
// Function: limpaEeprom
// Date    : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void limpaEeprom(void ) 
{
  // Se pressionou o botão ESC
  if(_keyPress && _KEY_ESC)
  { stResp = F("_"); passoMaquina = mMENU_CONFIGURACAO; return; }

  // Atualiza os passos
  if(passoAnterior != passoMaquina)
    passoAnterior = passoMaquina;
  
  // Se confirmou que deseja resetar a EEPROM
  if(auxResp)
  {
    // Pergunta se realmente vai limpar as receitas
    perguntaSimNao();

    // Se já respondeu
    if (_endString) 
    {
      // Reseta a flag de resposta
      _endString = false;
      // Faz o reset
      resetEEPROM();
      // Reseta a flag auxiliar de resposta
      auxResp = false;
    }
  }

  // Se ainda não respondeu sobre a limpeza da EEPROM
  else
  {
    // Pergunta se realmente deseja resetar a EEPROM
    perguntaSimNao();
    
    // Se já respondeu
    if (_endString && !auxResp) 
    {
      // Reseta a flag de resposta
      _endString = false;

      // Caso a resposta for não, retorna ao menu
      if (stResp == ST_RESP_NAO) 
      { stResp = F("_"); passoMaquina = mMENU_CONFIGURACAO; return; }

      // Caso o resposta for sim
      else if (stResp == ST_RESP_SIM) 
      {
        // Se houverem receitas armazenadas
        if(qtdeReceita != 0)
        {
          // Armazena a resposta sim para reset da EEPROM
          auxResp = true;
          // Carrega o temporizador de troca de mensagens 
          loadMsgChangeTimer();
          // Envia tela atual ao APP
          if(_connectedAPP)
            enviaTela(true);
        }
        // Se não houverem receitas armazenadas, limpa a EEPROM sem perguntar
        else
        {
          // Faz o reset
          resetEEPROM();
        }

        // Reseta a resposta
        stResp = F("_");
      }
    }
  }
}

//WWWWWWWWWW*********************************************************************************
// Function: resetEEPROM
// Date    : 16/01/2020  09:15
//WWWWWWWWWW*********************************************************************************

void resetEEPROM(void )
{
  // Envia tela de status e emite aviso sonoro
  enviaTelaStatus(F("Resetando EEPROM")); bipAcerto();
    
  #if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)
   
    lcdClear(); lcdPrint(1,1,F("Aguarde...")); lcdWrite();
  
  #endif

  // Índice de limpeza da EEPROM - incluindo receitas
  uint16_t auxIndClearEEPROM = NETEEPROM_END;

  // -------------------------------------------------------------------------- //
  // Carrega os dados das receitas, para não excluí-las

  // Se respondeu que deseja não deseja apagar as receitas
  if(stResp == ST_RESP_NAO)
  {
    // Atualiza o índice auxiliar das receitas
    ind = qtdeReceita;
    // Carrega a última receita
    Receita receita = regReceita[ind];
    // Extrai a posição da última receita
    auxIndClearEEPROM = ind*sizeof(receita) + NETEEPROM_END;
  }

  // -------------------------------------------------------------------------- //

  // Disable interrupts
  disableInterrupts(); 
  
  // Auxilia no desenho dos '*' de avanço do processo de limpeza
  register uint8_t auxColuna = 1;
    
  // Auxiliar no controle do avanço do desenho (248 = (4096 - 115)/ 16)
  uint16_t auxDesign = 0;
  if(auxIndClearEEPROM > 248)
    auxDesign = auxIndClearEEPROM - 248;

  // Laço para limpeza da EEPROM
  for (uint16_t i = auxIndClearEEPROM; i < EEPROM.length(); i++)
  {
    // Limpa a posição de memória em questão
    EEPROM.write(i, -1);

    // Algoritmo para desenhar '*' de avanço do processo de limpeza
    if(((i - auxDesign) == (248*auxColuna)) || (i - auxDesign) == 4050)
    {

      #if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)
        
        lcdPrint(2, auxColuna, F("*")); lcdWrite();
        
      #endif

      auxColuna++;
    }
  }

  // Limpa o índice auxiliar das receitas 
  ind = 0;

  // Se apagou as receitas
  if(stResp == ST_RESP_SIM)
  {
    // Limpa a quantidade de receitas
    qtdeReceita = 0;
  }
  
  // Enable interrupts
  enableInterrupts(); 

  // Reseta a resposta
  stResp = F("_");
  // Altera o passo da máquina
  passoMaquina = mSTANDBY;
  // Lê e carrega as configurações de fábrica
  leConfigGeral();
}

//WWWWWWWWWW*********************************************************************************
// Function: checkInitialReset
// Date    : 16/01/2020  09:15
//WWWWWWWWWW*********************************************************************************

void checkInitialReset(void )
{
  uint16_t i = 0, j = 0;
  boolean auxReset = false;
  
  while(i < 5000)
  {
    // Reset buzzer
    digitalWrite(PINO_BUZZER, LOW);
    // Lê o teclado
    uint16_t auxReadADCTeclado = analogRead(PINO_TECLADO);
    
    if(auxReset)
    {
      if(auxReadADCTeclado >= 0 && auxReadADCTeclado < 50)
      {
        j++; 
        if(j == 2500)
          break;
      }
      else auxReset = false;
    }
    
    // Verifica se a tecla ESC está pressionada
    if (auxReadADCTeclado >= 0 && auxReadADCTeclado < 50)
    {
      // Se estiver, sinaliza RESET e sai do laço
      auxReset = true; i = 0;
    } 

    // Incrementa o índice
    i++;
  }

  if(auxReset)
  {
    lcdClear(); 
    lcdPrint(1, 1, F("Padrao Fabrica ?")); 
    lcdPrint(2, 1, F("<-(SIM)  (NAO)->")); 
    lcdWrite();

    delay(1000);

    while(1)
    {
      // Reset buzzer
      digitalWrite(PINO_BUZZER, LOW);
      // Lê o teclado
      uint16_t auxReadADCTeclado = analogRead(PINO_TECLADO);

      // Se pressionou alguma tecla e não foi a tecla ESC
      if(auxReadADCTeclado != MAX_AD_TECLADO && 
        !(auxReadADCTeclado >= 0 && auxReadADCTeclado < 50))

      {
        // Se apertou DIREITA, sai
        if(auxReadADCTeclado >= 100 && auxReadADCTeclado < 200)
        {
          lcdClear(); 
          lcdPrint(1, 1, F("Inicializando...")); 
          lcdPrint(2, 1, F("Nao Restaurado!!")); 
          lcdWrite();
          return;
        }
        // Se apertou ESQUERDA, reseta
        else if (auxReadADCTeclado >= 450 && auxReadADCTeclado < 550)
          break;
      }
    }

    #if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)
   
    lcdClear(); lcdPrint(1, 1, F("Restaurar Padrao")); lcdWrite();
  
    #endif

    // Índice de limpeza da EEPROM - incluindo receitas
    uint16_t auxIndClearEEPROM = NETEEPROM_END;
    // Auxiliar no controle do avanço do desenho (248 = (4096 - 115)/ 16)
    uint16_t auxDesign = 0;
    if(auxIndClearEEPROM > 248)
      auxDesign = auxIndClearEEPROM - 248;

    // Auxilia no desenho dos '*' de avanço do processo de limpeza
    register uint8_t auxColuna = 1;
    // Laço para limpeza da EEPROM
    for (uint16_t i = auxIndClearEEPROM; i < EEPROM.length(); i++)
    {
      // Reset buzzer
      digitalWrite(PINO_BUZZER, LOW);
      // Limpa a posição de memória em questão
      EEPROM.write(i, -1);

      // Algoritmo para desenhar '*' de avanço do processo de limpeza
      if(((i - auxDesign) == (248*auxColuna)) || (i - auxDesign) == 4050)
      {

        #if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)
          
          lcdPrint(2, auxColuna, F("*")); lcdWrite();
          
        #endif

        auxColuna++;
      }
    }
    
    lcdPrint(2, 1, F("   Completo!!   "));  
    lcdWrite();
  }
}