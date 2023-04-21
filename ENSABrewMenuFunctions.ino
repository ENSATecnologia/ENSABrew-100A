//*******************************************************************************************
// Arquivo C/C++ do Projeto Controlador de Patamares -- ENSABrewControll
//-------------------------------------------------------------------------------------------
//
// Arquivo: ENSABrewMenuFunctions.ino
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
// Função: senhaMenuConfig
// Data  : 13/12/2019 14:15
//WWWWWWWWWW*********************************************************************************

void senhaMenuConfig(void )
{
  // Evita o incremento contínuo
  // clearIncrement();

  if(_keyPress && _KEY_ESC)
  {
    // Reseta os parâmetros de escrita
    resetDataInput(false);
    // Retorna ao menu
    passoMaquina = mMENU;
  }

  // Digitando a senha
  entradaString(F(""));

  // Se a senha já foi digitada
  if (_endString) 
  {
    // Reseta a flag de escrita
    _endString = false;
    // Recebe a senha digitada
    String stringSenha = stDigitado;
    // Limpa a flag de escrita
    stChar = F("_");

    // Verifica se é a senha MASTER e vai para o menu principal
    if (stringSenha == String(SENHA_MASTER)) 
    { bipAcerto(); passoMaquina = mMENU_CONFIGURACAO; actualMenu = mMENU_CONFIGURACAO; }
    // Verifica se é a senha USER e vai para o menu do usuário 
    else if (stringSenha == String(SENHA_USER)) 
    { bipAcerto(); passoMaquina = mMENU_USER_CONFIG; actualMenu = mMENU_USER_CONFIG; }
    // Se errou a senha, retorna com mensagem de erro
    else 
    { bipAtencao(); printDisplay(F("      ERRO!     "), F(" Senha Invalida "), TEMPO_ALARME);}
  }
}


//WWWWWWWWWW*********************************************************************************
// Função: executaMenu
// Data  : 04/04/2017 13:45
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
      
      // Verifica onde está e para onde deve voltar
      switch(passoMaquina)
      {
        case mMENU:           
          
          passoMaquina = mSTANDBY; 

        break;
        
        case mRECEITAS:
        case mMENU_USER_CONFIG:
        case mMENU_CONFIGURACAO: 
          
          passoMaquina = mMENU; 

        break;

        case mRECEITASEL:

          passoMaquina = mRECEITAS;

        break;

        case mMENU_CAL_FACTOR:
        case mMENU_CONFIG_PID:
        case mMENU_TEMPO_ENVIO:

          passoMaquina = mMENU_CONFIGURACAO; 

        break;
      }
    }
  }
}

//WWWWWWWWWW*********************************************************************************
// Função: executaBoasVindas
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void executaBoasVindas(void ) 
{
  if (_keyPress && _KEY_SELECT) 
  { passoMaquina = mMENU; posicaoMenu = 0; return; }
}

//WWWWWWWWWW*********************************************************************************
// Função: executaMenuInicial
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void executaMenuInicial(void ) 
{
  // Evita o incremento contínuo
  clearIncrement();

  desligaLeds();

  int8_t opcMenu = executaMenu(LENGTH_MENU);
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
      passoMaquina = mSENHA_CONFIGURACAO;
  }
}

//WWWWWWWWWW*********************************************************************************
// Função: menuGeneralConfig
// Data  : 13/12/2019 13:15
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
         if (opcMenu == 0) passoMaquina = mMENU;
    else if (opcMenu == 1) passoMaquina = mSET_MAX_PATAMAR;
    else if (opcMenu == 2) passoMaquina = mSET_GRAU_MINUTO;
    else if (opcMenu == 3) passoMaquina = mSET_PWM_SETPOINT;
    else if (opcMenu == 4) passoMaquina = mMENU_CONFIG_PID;
    else if (opcMenu == 5) passoMaquina = mSET_SAMPLE_TEMP;
    else if (opcMenu == 6) passoMaquina = mMENU_TEMPO_ENVIO;
    else if (opcMenu == 7) passoMaquina = mMENU_CAL_FACTOR;
    else if (opcMenu == 8) passoMaquina = mLIMPA_EEPROM;
  }
}

//WWWWWWWWWW*********************************************************************************
// Função: menuUserConfig
// Data  : 20/02/2020 08:45
//WWWWWWWWWW*********************************************************************************

void menuUserConfig(void )
{ 
  // Verifica a opção do menu
  int8_t opcMenu = executaMenu(LENGTH_MENU_USER_CONFIG);

  // Se definiu alguma das opção do menu, verifica qual é
  if (_analisaOpcMenu) 
  {
    // Reseta a flag de opção do menu
    _analisaOpcMenu = false;
    // Reinicia o temporizador de troca de mensagens 
    loadMsgChangeTimer();

    // Verifica e seleciona a opção escolhida
         if (opcMenu == 0) passoMaquina = mMENU;
    else if (opcMenu == 1) passoMaquina = mSET_MAX_PATAMAR;
    else if (opcMenu == 2) passoMaquina = mSET_GRAU_MINUTO;
    else if (opcMenu == 3) passoMaquina = mSET_PWM_SETPOINT;
  }
}

//WWWWWWWWWW*********************************************************************************
// Função: menuTimeConfig
// Data  : 13/12/2019 13:30
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
// Função: menuGeneralConfigPID
// Data  : 16/12/2019 09:30
//WWWWWWWWWW*********************************************************************************

void menuGeneralConfigPID(void )
{
  // Verifica a opção do menu
  int8_t opcMenu = executaMenu(LENGTH_MENU_PID);
  // Se definiu alguma das opção do menu, verifica qual é
  if (_analisaOpcMenu) 
  {
    // Reseta a flag de opção do menu
    _analisaOpcMenu = false;

    // Verifica e seleciona a opção escolhida
         if (opcMenu == 0) passoMaquina = mMENU_CONFIGURACAO;
    else if (opcMenu == 1) passoMaquina = mSTATUS_PID;     
    else if (opcMenu == 2) passoMaquina = mCONFIG_KP_PID;
    else if (opcMenu == 3) passoMaquina = mCONFIG_KI_PID;
    else if (opcMenu == 4) passoMaquina = mCONFIG_KD_PID;

    // Carrega o status temporário do PID
    if(passoMaquina == mSTATUS_PID)
      auxTempStatusPID = configGeral.statusPID;
  }
}

//WWWWWWWWWW*********************************************************************************
// Função: executaMenuCalFactor
// Data  : 13/12/2019 14:10
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
// Função: completaEspacos
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

String completaEspacos(String prStr, int prTamMax) 
{
  for (register int ind = prStr.length(); ind <= prTamMax; ind++) 
  {
    prStr = prStr + ' ';
  } return prStr;
}

//WWWWWWWWWW*********************************************************************************
// Função: perguntaSimNao
// Data  : 04/04/2017 13:45
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
// Função: controlNumberCharacters
// Data  : 12/11/2019 11:00
//WWWWWWWWWW*********************************************************************************

boolean controlNumberCharacters(void )
{
  // Se atingiu o máximo de caracteres
  if( ((stDigitado.length() >= MAX_NOME-1) && _KEY_RIGHT) ||
      ((stDigitado.length()  > MAX_NOME-1) && (_KEY_UP || _KEY_DOWN)) )
  {
    // Limpa a tecla que foi pressionada
    if(_KEY_RIGHT)
      _KEY_RIGHT = false;
    else if(_KEY_UP)
      _KEY_UP = false;
    else if(_KEY_DOWN)
      _KEY_DOWN = false;

    // Apaga o último caracter digitado
    stDigitado = stDigitado.substring(0, stDigitado.length());
    // Avisa que o máximo de caracteres já foi atingido
    bipAtencao(); printDisplay(F(" Atingido o MAX "), F(" de Caracteres! "), TEMPO_CONF);
    // Sinaliza que pode retornar para evitar lixos na escrita
    return(true);
  }

  // Se não, retorna falso
  return(false);
}

//WWWWWWWWWW*********************************************************************************
// Função: checkVoidString
// Data  : 31/01/2020 14:15
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
