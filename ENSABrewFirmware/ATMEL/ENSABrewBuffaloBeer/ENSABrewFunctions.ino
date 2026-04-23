//*******************************************************************************************
// Arquivo C/C++ do Projeto Controlador de Patamares -- ENSABrewControll
//-------------------------------------------------------------------------------------------
//
// Arquivo: ENSABrewFunctions.ino
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
// Função: contandoTempo
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

boolean contandoTempo(void ) 
{
  if (tempoInicial == 0) return false;
  else return true;
}

//WWWWWWWWWW*********************************************************************************
// Função: respondeBrassagemRemota
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void respondeBrassagemRemota(void ) 
{
  // "typeCmd = FF"
}

//WWWWWWWWWW*********************************************************************************
// Função: enviaTela
// Data  : 13/02/19 11:30
//WWWWWWWWWW*********************************************************************************

String enviaTela(boolean auxSendData) 
{
  String telaAtual = F("Void");

  switch(passoMaquina) 
  {
    case mSTANDBY:
      telaAtual = F("Stand-By");
      opcaoAtual = F("Void");
      break;
      
    case mMENU:
      telaAtual = F("Menu Principal");
      break;

    case mRECEITASEL:
      telaAtual = String(F("Menu da Receita: ")) + String(receitaSel.nome);
      break;
    
    case mMENU_CONFIGURACAO:
      telaAtual = F("Menu Configuracao");
      break;
    
    case mSET_DATA_HORA:
      telaAtual = F("Configurar Data e Hora");
      break;

    // ------------------------------------------------------------------------

    case mVISUALIZA_RECEITA:

      switch(auxVisualizaReceita)
      {
        case mNOME_RECEITA:
          telaAtual = String(F("Nome: ")) + String(prReceita.nome);
        break;

        case mQT_PATAMAR:
          telaAtual = String(F("Quantidade de PATAMARES: ")) + String(prReceita.qtdePatamar);
        break;

        case mTEMPMAX_PATAMAR:
          telaAtual = String(F("Temperatura do PATAMAR ")) + 
                      String(ind + 1) + String(F(": ")) + 
                      String(prReceita.patamar[ind].temperaturaMax) + String(F("C"));
        break;

        case mMINUTO_PATAMAR:
          telaAtual = String(F("Tempo PATAMAR ")) + String(ind + 1) + String(F(": ")) +
                      String(prReceita.patamar[ind].tempo) + String(F(" minutos"));
        break;
      }

      break;

    // ------------------------------------------------------------------------
    
    case mRECEITAS:
      telaAtual = F("Menu Receitas");
      break;

     case mNOME_RECEITA:
      telaAtual = F("Nome da Receita");
      opcaoAtual = F("Void");
      break;

    case mQT_PATAMAR:
      telaAtual = F("Quantidade de Patamares");
      opcaoAtual = F("Void");
      break;

    case mSALVA_RECEITA:
      if (prOper == _CRIAR_RECEITA)
        telaAtual = F("Fim da Criacao da Receita");
      else if(prOper == _EDITAR_RECEITA)
        telaAtual = F("Fim da Edicao da Receita");
      break;

    case mADICIONAR_AGUA:
      telaAtual = F("Adicionar Agua");
      break;

    case mLIMPA_EEPROM:
      if(!auxResp)
        telaAtual = F("Reset Defaults?");
      else 
        telaAtual = F("Apagar receitas?");
      if(opcaoAtual != ST_RESP_SIM && opcaoAtual != ST_RESP_NAO)
        opcaoAtual = F("Void");
    break;

    case mSET_MAX_PATAMAR:
      telaAtual = F("Maximo de Patamares");
      opcaoAtual = F("Void");
      break;

    case mSET_GRAU_MINUTO:
      telaAtual = F("Grau por Minuto");
      opcaoAtual = F("Void");
      break;

    case mSET_PWM_SETPOINT:
      telaAtual = F("PWM p/ Setpoint");
      opcaoAtual = F("Void");
      break;

    case mSET_SAMPLE_TEMP:
      telaAtual = F("Media de Temperatura");
      opcaoAtual = F("Void");
      break;

    case mMENU_CAL_FACTOR:
      telaAtual = F("Menu de Ajuste de Volume");
      break;
    
    case mAUTO_CAL_FACTOR:
      telaAtual = F("Ajuste Automatico de Volume");
      opcaoAtual = F("Void");
      break;
    
    case mMANUAL_CAL_FACTOR:
      telaAtual = F("Ajuste Manual de Volume");
      opcaoAtual = F("Void");
      break;

    case mMENU_TEMPO_ENVIO:
      telaAtual = F("Menu de Configuracao dos Tempos de Envio");
      break;
    
    case mSET_SEND_TIME_AC:
      telaAtual = F("Envio da Leitura AC");
      opcaoAtual = F("Void");
      break;

    case mSET_SEND_TIME_TEMP:
      telaAtual = F("Envio da Temperatura");
      opcaoAtual = F("Void");
      break;
    
    case mMENU_CONFIG_PID:
      telaAtual = F("Menu de Configuracao do PID");
      break;

    case mCONFIG_KP_PID:
      telaAtual = F("Configurar PID: KP");
      opcaoAtual = F("Void");
      break;

    case mCONFIG_KI_PID:
      telaAtual = F("Configurar PID: KI");
      opcaoAtual = F("Void");
      break;

    case mCONFIG_KD_PID:
      telaAtual = F("Configurar PID: KD");
      opcaoAtual = F("Void");
      break;

    default: break;
  }

  // Se está liberado para envio
  if(telaAtual != F("Void") && auxSendData) 
  {
    // Informações sobre o dado (tipo do cabeçalho, tipo do dado e tamanho do dado)
    register uint8_t auxInfoData[3] = {_HEADER_SCREEN, NULL, 1};
    // Envia o dado ao APP
    // sendDataAPP(&auxInfoData[0], &telaAtual);
  }
  else
  {
    // Retorna a tela atual
    return(telaAtual);
  }
}

//WWWWWWWWWW*********************************************************************************
// Função: enviaTelaStatus
// Data  : 05/04/2019 11:35
//WWWWWWWWWW*********************************************************************************

void enviaTelaStatus(String typeStatus) 
{
  statusAtual = typeStatus;
  enviaTela(true);
  statusAtual = F("Void");
}

//WWWWWWWWWW*********************************************************************************
// Função: resetEnviosTela
// Data  : 08/04/2019 09:20
//WWWWWWWWWW*********************************************************************************

void resetEnviosTela(void ) 
{
  opcaoAtual = F("Void");
  statusAtual = F("Void");
  entradaAtual = F("Void");
}

//WWWWWWWWWW*********************************************************************************
// Função: clearTimers
// Data  : 08/04/2019 09:20
//WWWWWWWWWW*********************************************************************************

void clearTimers(void ) 
{
  _contaTempo = false;

  valorTempoManual = 0;
  tempoRestante = 0;
  tempoInicial = 0;
  tempoProximo = 0;
}

//WWWWWWWWWW*********************************************************************************
// Função: loadTimers
// Data  : 26/06/2019 11:43
//WWWWWWWWWW*********************************************************************************

void loadTimers(void )
{
  // Carrega o temporizador de envio de leitura AC
  wdtSendReadAC = configGeral.timeSendReadAC;
  // Carrega o temporizador de envio de temperatura
  wdtSendTemp = configGeral.timeSendTemp;
}

//WWWWWWWWWW*********************************************************************************
// Função: loadMsgChangeTimer
// Data  : 04/11/2019 08:30
//WWWWWWWWWW*********************************************************************************

void loadMsgChangeTimer(void ) 
{
  #ifdef DISPLAY_LCD_PRL || DISPLAY_LCD_I2C
    // Reinicia o temporizador de troca de mensagens 
    wdtChangeMsg = TIME_CHANGE_MSG; _changeMsg = true;
  #endif
}

//WWWWWWWWWW*********************************************************************************
// Função: controlPIDTasks
// Data  : 16/12/2019 11:25
//WWWWWWWWWW*********************************************************************************

void controlPIDTasks(void )
{
  // Input = mediaTemperature; myPID.Compute();
  // Timer3.pwm(PINO_SSR, Output, TIMER3);
}

//WWWWWWWWWW*********************************************************************************
// Funcão: getValorPwm - http://playground.arduino.cc/Main/TimerPWMCheatsheet
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

float getValorPwm(void ) 
{
  return((valorPwm / (float)100) * (float)1024);
}

//WWWWWWWWWW*********************************************************************************
// Funcão: getValor1Dec
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

String getValor1Dec(float prValor) 
{
  String st = String(prValor);
  st = st.substring(0, st.indexOf('.') + 2);
  return st;
}

//WWWWWWWWWW*********************************************************************************
// Funcão: getValorInt
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

String getValorInt(float prValor) 
{
  String st = String(prValor);
  st = st.substring(0, st.indexOf('.'));
  return st;
}