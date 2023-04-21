//*******************************************************************************************
// Arquivo C/C++ do Projeto Controlador de Patamares -- ENSABrewControll
//-------------------------------------------------------------------------------------------
//
// Arquivo: ENSABrewFunctions.ino
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
// Funcão: controlTemperatureTasks
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void controlTemperatureTasks(void ) 
{
  // ----------------------------------------------------------------
  // Rotina de leitura 

  if (_readTemperature) 
  {
    _readTemperature = false;
    for (register uint16_t i = 1; i < configGeral.sampleTemperature; i++)
      filtroTemperatura[i] = filtroTemperatura[i - 1];
    filtroTemperatura[0] = thermistor.getTemp();
    mediaTemperature = 0;
    for (register uint16_t j = 0; j < configGeral.sampleTemperature; j++)
      mediaTemperature += filtroTemperatura[j];  //somatório das amostras
    mediaTemperature /= configGeral.sampleTemperature; //média dos valores
  }
  
  // ----------------------------------------------------------------
  // Rotina de controle - PID

  // Se o PID estiver habilitado
  if(configGeral.statusPID == _ENABLE)
    controlPIDTasks();

  // ----------------------------------------------------------------
  // Rotina de envio

  // Se o envio da temperatura estiver habilitado e se já está liberado, envia
  if (configGeral.timeSendTemp != 0 && wdtSendTemp == 0) 
  {
    // Recarrega o temporizador
    switch (passoMaquina) 
    {

      /*
        Se estiver em brassagem (em processo de alcance de  
        temperatura), carrega o tempo para envio em brassagem
      */
      case mLOOP_BRASSAGEM:
      case mLOOP_FERVURA:
        
        wdtSendTemp = configGeral.tempoEnvioBrassagem;
      
      break;

      /*
        Se estiver em brassagem (com temperatura já alcançada), não 
        envia a temperatura, pois já será enviada em outro objeto
      */
      case mMENU_FERVURA:
      case mADICAO_FERVURA:
      case mEXECUTA_FERVURA:
      case mMENU_DESCANSO:
      case mLOOP_DESCANSO:
      case mEXECUTA_DESCANSO:
      case mLOOP_WHIRLPOOL:
      case mMENU_WHIRLPOOL:
      case mEXECUTA_WHIRLPOOL:
        
        return;

      break;

      // Se não, recarrega o temporizador
      default: wdtSendTemp = configGeral.timeSendTemp; break;
    }

    // Se já atingiu a temperatura, também não envia a mesma
    if(_contaTempo) return;

    // Se está conectado ao APP
    if(_connectedAPP)
    {
      // mediaTemperature = 25;
      // Informações sobre o dado (tipo do cabeçalho, tipo do dado e tamanho do dado)
      uint8_t auxInfoData[3] = {_HEADER_EVENT, _TEMPERATURE, 1};
      // Dado em si, com o número do comando
      String auxSendData[2] = {F("02"), String((float)(mediaTemperature), 2)};
      // Envia o dado ao APP
      sendDataAPP(&auxInfoData[0], &auxSendData[0]);
    }
  }

  // ----------------------------------------------------------------
}


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
// Função: calculaSpeed
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void calculaSpeed(int prTemperatura) 
{
  if (tempInicialAquecimento > - 1000 )
    speedAquecimento = (prTemperatura - tempInicialAquecimento);
  else
    speedAquecimento = 0;
}

//WWWWWWWWWW*********************************************************************************
// Função: calculaGrauPorMinuto
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void calculaGrauPorMinuto(float prTemperaturaFloat) 
{
  int tempo = 0;
  if (tempoInicialVelocidade == 0) 
  {
    temperaturaFloatInicial = prTemperaturaFloat;
    tempo++;
  }
  if (tempo >= 60) 
  { //Segundos
    grauPorMinuto = temperaturaFloatInicial - prTemperaturaFloat;
    if (grauPorMinuto < 0)
      grauPorMinuto = 0;
    tempoInicialVelocidade = 0;
  }
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

    case mMENU_USER_CONFIG:
      telaAtual = F("Menu Configuracao Usuario");
      break;
    
    case mSENHA_CONFIGURACAO: 
      telaAtual = F("Digitar a Senha");
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

        case mTEMPMAX_FERVURA:
          telaAtual = String(F("Temperatura da FERVURA: ")) + 
                String(prReceita.tempFervura) + String(F("C"));
        break;

        case mMAXMIN_FERVURA:
          telaAtual = String(F("Tempo da FERVURA: ")) + String(prReceita.qtdeMinFervura) + String(F(" minutos"));
        break;

        case mQT_ADICAO:
          telaAtual = String(F("Quantidade de ADICOES: ")) + String(prReceita.qtdeAdicao);
        break;

        case mNOME_ADICAO:
          telaAtual = String(F("Nome da ADICAO ")) + String(ind + 1) + 
                      String(F(": ")) + String(prReceita.adicao[ind].nome);
        break;

        case mGRAMAS_ADICAO:
          telaAtual = String(F("Quantidade da ADICAO ")) + String(ind + 1) + String(F(": ")) +
                String(prReceita.adicao[ind].qtde) + String(F(" gramas"));
        break;

        case mMINUTOS_ADICAO:
          telaAtual = String(F("Tempo ADICAO: ")) + String(ind + 1) + String(F(": ")) +
                    String(prReceita.adicao[ind].tempo) + String(F(" minutos"));
        break;

            case mQT_WHIRLPOOL:
          telaAtual = String(F("Quantidade de WHIRLPOOL: ")) + String(prReceita.qtdeWhirlpool);
        break;

            case mMINUTO_WHIRLPOOL:
          telaAtual = String(F("Tempo do WHIRLPOOL   ")) + actualWhirlpool(ind) + String(F(": ")) +
                  String(prReceita.whirlpool[ind].tempo) + String(F(" minutos"));
        break;

            case mQT_DESCANSO:
          telaAtual = String(F("Quantidade de DESCANSO  ")) + String(prReceita.qtdeDescanso);
        break;

            case mMINUTO_DESCANSO:
            telaAtual = String(F(" Tempo DESCANSO: ")) +
                        String(prReceita.descanso[ind].tempo) + String(F(" minutos"));
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

    case mTEMPMAX_PATAMAR:
      if(qtdePatamar < ind+1)
        telaAtual = F("Void");
      else
        telaAtual = "Temperatura MAX Patamar " + String(ind + 1);
        opcaoAtual = F("Void");
      break;

    case mMINUTO_PATAMAR:
      if(qtdePatamar < ind+1)
        telaAtual = F("Void");
      else
        telaAtual = "Tempo do Patamar " + String(ind + 1);
        opcaoAtual = F("Void");
      break;

    case mTEMPMAX_FERVURA:
      telaAtual = F("Temperatura de Fervura");
      opcaoAtual = F("Void");
      break;

    case mMAXMIN_FERVURA:
      telaAtual = F("Tempo de Fervura");
      opcaoAtual = F("Void");
      break;

    case mQT_ADICAO:
      telaAtual = F("Quantidade de Adicao");
      opcaoAtual = F("Void");
      break;

    case mNOME_ADICAO:
      if(qtdeAdicao < ind+1)
        telaAtual = F("Void");
      else
        telaAtual = "Nome da Adicao " + String(ind + 1);
        opcaoAtual = F("Void");
      break;

    case mGRAMAS_ADICAO:
      if(qtdeAdicao < ind+1)
        telaAtual = F("Void");
      else
        telaAtual = "Gramas da Adicao " + String(ind + 1);
        opcaoAtual = F("Void");
      break;

    case mMINUTOS_ADICAO:
      if(qtdeAdicao < ind+1)
        telaAtual = F("Void");
      else
        telaAtual = "Tempo da Adicao " + String(ind + 1);
        opcaoAtual = F("Void");
      break;

    case mQT_WHIRLPOOL:
      telaAtual = F("Quantidade de Whirpool");
      opcaoAtual = F("Void");
      break;

    case mMINUTO_WHIRLPOOL:
      if(qtdeWhirlpool < ind+1)
        telaAtual = F("Void");
      else
        telaAtual = "Tempo do Whirpool " + String(ind + 1);
        opcaoAtual = F("Void");
      break;

    case mQT_DESCANSO:
      telaAtual = F("Quantidade de descanso");
      opcaoAtual = F("Void");
      break;

    case mMINUTO_DESCANSO:
      telaAtual = F("Tempo de Descanso");
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

    case mDERRUBAR_MALTE:
      telaAtual = F("Derrubar Malte?");
      resetEnviosTela();
      break;

    case mMENU_FERVURA:
      telaAtual = F("Iniciar Fervura?");
      resetEnviosTela();
      break;

    case mMENU_WHIRLPOOL:
      telaAtual = F("Iniciar Whirpool?");
      resetEnviosTela();
      break;

    case mADICAO_FERVURA:
      telaAtual = F("Colocar Adicao?");
      opcaoAtual = (String((float)(adicaoFervura[numAdicao].qtde), 2) +
                   "g - " + adicaoFervura[numAdicao].nome);
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
    uint8_t auxInfoData[3] = {_HEADER_SCREEN, NULL, 1};
    // Envia o dado ao APP
    sendDataAPP(&auxInfoData[0], &telaAtual);
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
  // enviaTela(true);
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
  Input = mediaTemperature; myPID.Compute();
  Timer3.pwm(PINO_SSR, Output, TIMER3);
}