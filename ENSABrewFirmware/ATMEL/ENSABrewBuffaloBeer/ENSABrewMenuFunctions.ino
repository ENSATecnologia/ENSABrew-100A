//*******************************************************************************************
// Arquivo C/C++ do Projeto Controlador de Patamares -- ENSABrewControll
//-------------------------------------------------------------------------------------------
//
// Arquivo: ENSABrewMenuFunctions.ino
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
// Function: executaMenu
// Date    : 04/04/2017 - 13:45
//WWWWWWWWWW*********************************************************************************

int8_t executaMenu(int prTamanhoMax) 
{
  // Evita o incremento contínuo
  clearIncrement();

  tamanhoMenu = prTamanhoMax;
  if (_keyPress) 
  {
    if (_KEY_SELECT) 
    {
      _analisaOpcMenu = true;
      int opcaoMenu = posicaoMenu;
      posicaoMenu = 0;
      return opcaoMenu;
    }
    else if (_KEY_DOWN)
      posicaoMenu++;
    
    else if (_KEY_UP)
      posicaoMenu--;

    else if(_KEY_ESC)
    {
      // Reseta os parâmetros de escrita
      resetDataInput(false);
      // Reseta a posição do MENU
      posicaoMenu = 0;
      
      // Verifica onde está e para onde deve voltar
      switch(passoMaquina)
      {
        case mMENU:           
          
          passoMaquina = mSTANDBY; 

        break;
        
        case mRECEITAS:
        case mMENU_CONFIGURACAO: 
          
          passoMaquina = mMENU; 

        break;

        case mRECEITASEL:

          passoMaquina = mRECEITAS;

        break;

        // case mMENU_CONFIG_PID:
        case mMENU_CAL_FACTOR:
        case mMENU_TEMPO_ENVIO:

          passoMaquina = mMENU_CONFIGURACAO; 

        break;
      }
    }
  }
}

//WWWWWWWWWW*********************************************************************************
// Function: executaBoasVindas
// Date    : 04/04/2017 - 13:45
//WWWWWWWWWW*********************************************************************************

void executaBoasVindas(void ) 
{
  if (_keyPress && _KEY_SELECT) 
  { passoMaquina = mMENU; posicaoMenu = 0; return; }
}

//WWWWWWWWWW*********************************************************************************
// Function: executaMenuInicial
// Date    : 04/04/2017 - 13:45
//WWWWWWWWWW*********************************************************************************

void executaMenuInicial(void ) 
{
  // Apaga os LEDs
  desligaLeds();
  // Evita o incremento contínuo
  clearIncrement();

  // Controle de menu de acordo com o tamanho do menu principal
  int8_t opcMenu = executaMenu(LENGTH_MENU);
    
  // Se definiu a 
  if (_analisaOpcMenu) 
  {
    _analisaOpcMenu = false;

    // Reseta os parâmetros de escrita
    resetDataInput(false);
    
    if (opcMenu == 0) 
    {
      if (qtdeReceita > 0)
        passoMaquina = mRECEITAS;
      else 
      {
        bipAtencao();
        printDisplay(F("Nenhuma Receita "), F("  Cadastrada  "), TEMPO_ALARME);
        passoMaquina = mMENU;
      }
    }
    else if (opcMenu == 1)
      passoMaquina = mCRIAR_RECEITA;
    else if (opcMenu == 2)
      passoMaquina = mADICIONAR_AGUA;
    else if (opcMenu == 3) 
      passoMaquina = mMENU_CONFIGURACAO;
  }
}

//WWWWWWWWWW*********************************************************************************
// Function: menuGeneralConfig
// Date    : 13/12/2019 - 13:15
//WWWWWWWWWW*********************************************************************************

void menuGeneralConfig(void )
{ 
  // Verifica a opção do menu
  int8_t opcMenu = executaMenu(LENGTH_MENU_CONFIGURACAO);

  // Se definiu alguma das opção do menu, verifica qual é
  if (_analisaOpcMenu) 
  {
    // Reseta a flag de opção do menu
    _analisaOpcMenu = false;
    // Reinicia o temporizador de troca de mensagens 
    loadMsgChangeTimer();

    // Verifica e seleciona a opção escolhida
         if (opcMenu == 0)  passoMaquina = mMENU;
    else if (opcMenu == 1)  passoMaquina = mSET_MAX_PATAMAR;
    else if (opcMenu == 2)  passoMaquina = mSET_OFFSET_TEMP;
    else if (opcMenu == 3)  passoMaquina = mSET_HISTERESE_FACTOR;
    else if (opcMenu == 4)  passoMaquina = mSET_SAMPLE_TEMP;
    // else if (opcMenu == 5)  passoMaquina = mSET_GRAU_MINUTO;
    else if (opcMenu == 5)  passoMaquina = mSET_PWM_SETPOINT;
    // else if (opcMenu == 7)  passoMaquina = mMENU_CONFIG_PID;
    // else if (opcMenu == 6)  passoMaquina = mMENU_TEMPO_ENVIO;
    else if (opcMenu == 6)  passoMaquina = mMENU_CAL_FACTOR;
    else if (opcMenu == 7)  passoMaquina = mLIMPA_EEPROM;
    
    // --------------------------------------------------------------------------------------- //
    // Reconexão WIFI
    
    else if (opcMenu == 8)
    {
      // Reseta o Serial
      Serial.end();  Serial1.end(); 
      // Restart Serial
      startSerial();

      // Reseta o Keep alive
      _flagSendKeepAlive = false; wdtSendKeepAlive = TIME_SEND_KEEP_ALIVE;

      bipOK(); printDisplay(F("  Tentativa de  "), F(" Reconexao Wifi "), TEMPO_CONF);
    }

    // --------------------------------------------------------------------------------------- //

  }
}

//WWWWWWWWWW*********************************************************************************
// Function: menuTimeConfig
// Date    : 13/12/2019 - 13:30
//WWWWWWWWWW*********************************************************************************

void menuTimeConfig(void )
{
  // Verifica a opção do menu
  int8_t opcMenu = executaMenu(LENGTH_MENU_TIME_CONFIG);
  // Se definiu alguma das opção do menu, verifica qual é
  if (_analisaOpcMenu) 
  {
    // Reseta a flag de opção do menu
    _analisaOpcMenu = false;

    // Verifica e seleciona a opção escolhida
         if (opcMenu == 0) passoMaquina = mMENU_CONFIGURACAO;
    else if (opcMenu == 1) passoMaquina = mSET_SEND_TIME_AC;
    else if (opcMenu == 2) passoMaquina = mSET_SEND_TIME_TEMP;
  }
}

//WWWWWWWWWW*********************************************************************************
// Function: menuGeneralConfigPID
// Date    : 16/12/2019 - 09:30
//WWWWWWWWWW*********************************************************************************

void menuGeneralConfigPID(void )
{
  // // Verifica a opção do menu
  // int8_t opcMenu = executaMenu(LENGTH_MENU_PID);
  // // Se definiu alguma das opção do menu, verifica qual é
  // if (_analisaOpcMenu) 
  // {
  //   // Reseta a flag de opção do menu
  //   _analisaOpcMenu = false;

  //   // Verifica e seleciona a opção escolhida
  //        if (opcMenu == 0) passoMaquina = mMENU_CONFIGURACAO;
  //   else if (opcMenu == 1) passoMaquina = mSTATUS_PID;     
  //   else if (opcMenu == 2) passoMaquina = mCONFIG_KP_PID;
  //   else if (opcMenu == 3) passoMaquina = mCONFIG_KI_PID;
  //   else if (opcMenu == 4) passoMaquina = mCONFIG_KD_PID;

  //   // Carrega o status temporário do PID
  //   if(passoMaquina == mSTATUS_PID)
  //     auxTempStatusPID = configGeral.statusPID;
  // }
}

//WWWWWWWWWW*********************************************************************************
// Function: executaMenuCalFactor
// Date    : 13/12/2019 - 14:10
//WWWWWWWWWW*********************************************************************************

void executaMenuCalFactor(void )
{
  // Verifica a opção do menu
  int8_t opcMenu = executaMenu(LENGTH_MENU_CAL_FACTOR);
  // Se definiu alguma das opção do menu, verifica qual é
  if (_analisaOpcMenu) 
  {
    // Reseta a flag de opção do menu
    _analisaOpcMenu = false;

    // Verifica e seleciona a opção escolhida
    if (opcMenu == 0)
      passoMaquina = mMENU_CONFIGURACAO;
    else if (opcMenu == 1) 
    {
      // Sinaliza ajuste automático
      _auxAdjustCalFactor = true;
      passoMaquina = mADICIONAR_AGUA;
    }
    else if (opcMenu == 2) 
      passoMaquina = mMANUAL_CAL_FACTOR;
  }
}

//WWWWWWWWWW*********************************************************************************
// Function: completaEspacos
// Date    : 04/04/2017 - 13:45
//WWWWWWWWWW*********************************************************************************

String completaEspacos(String prStr, int prTamMax) 
{
  for (register int ind = prStr.length(); ind <= prTamMax; ind++) 
  {
    prStr = prStr + ' ';
  } return prStr;
}

//WWWWWWWWWW*********************************************************************************
// Function: perguntaSimNao
// Date    : 04/04/2017 - 13:45
//WWWWWWWWWW*********************************************************************************

void perguntaSimNao(void ) 
{
  // Evita o incremento contínuo
  clearIncrement();

  if (_keyPress) 
  {
    if (_KEY_SELECT)
    {
      if(stResp == ST_RESP_SIM)
        opcaoAtual = ST_RESP_SIM;
      else
        opcaoAtual = ST_RESP_NAO;

      if(auxResp)
        auxResp = false;
      
      if(_connectedAPP)
        enviaTela(true);
      opcaoAtual = F("Void");

      if (stResp != F("_")) 
      {
        _endString = true;
        return;
      }
    }
    else if (_KEY_UP)
      stResp = ST_RESP_SIM;
    else if (_KEY_DOWN)
      stResp = ST_RESP_NAO;
  }
}

//WWWWWWWWWW*********************************************************************************
// Function: controlNumberCharacters
// Date    : 12/11/2019 - 11:00
//WWWWWWWWWW*********************************************************************************

boolean controlNumberCharacters(void )
{
 // Extrai o tamanho da string
  register uint8_t auxStDigitado = stDigitado.length();

  // Se atingiu o máximo de caracteres
  if( ((auxStDigitado >= MAX_NOME-1) && _KEY_RIGHT) ||
      ((auxStDigitado  > MAX_NOME-1) && (_KEY_UP || _KEY_DOWN)) )
  {
    // ------ Limpa a tecla que foi pressionada ------ //
    
    if(_KEY_UP) _KEY_UP = false;
    else if(_KEY_DOWN) _KEY_DOWN = false;
    else if(_KEY_RIGHT) _KEY_RIGHT = false;

    // ------ Limpa a tecla que foi pressionada ------ //

    // Apaga o último caracter digitado
    stDigitado = stDigitado.substring(0, auxStDigitado);
    // Avisa que o máximo de caracteres já foi atingido
    bipAtencao(); printDisplay(F(" Atingido o MAX "), F(" de Caracteres! "), TEMPO_CONF);
    // Sinaliza que pode retornar para evitar lixos na escrita
    return(true);
  }

  // Se não, retorna falso
  return(false);
}

//WWWWWWWWWW*********************************************************************************
// Function: checkVoidString
// Date    : 31/01/2020 - 14:15
//WWWWWWWWWW*********************************************************************************

boolean checkVoidString(void )
{
  for(register uint8_t i = 0; i < stDigitado.length(); i++)
  {
    if(stDigitado.substring(i, i+1) != F(" "))
      return(false);
  }

  bipAtencao();
  printDisplay(F("   Nome Vazio   "), F(" Nao Permitido! "), TEMPO_CONF);
  return(true);
}
