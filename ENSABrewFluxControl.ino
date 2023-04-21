//*******************************************************************************************
// Arquivo C/C++ do Projeto Controlador de Patamares -- ENSABrewControll
//-------------------------------------------------------------------------------------------
//
// Arquivo: ENSABrewFluxControl.ino
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
// Função: adicionarAgua
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void adicionarAgua(void ) 
{
  // Sinaliza contagem do fluxo habilitada
  _stopCountFlux = false;

  if (_keyPress && (_KEY_LEFT || _KEY_ESC)) 
  {
    // Reseta a entrada de dados
    resetDataInput(false);

    // Se é calibração
    if(_auxAdjustCalFactor) 
    {
      // Limpa a flag de sinalização de calibração
      _auxAdjustCalFactor = false;
      // Cálcula o fator automaticamente
      passoMaquina = mMENU_CAL_FACTOR;
      // Retorna
      return;
    }
    else { passoMaquina = mMENU; return; }
  }

  // Se for calibração
  if(_auxAdjustCalFactor)
    entradaValorGeral(5, 1, 10, true);
  else 
    entradaValorGeral(30, 1, MAX_LITROS, true);
  if (_endValue) 
  {
    _endValue = false;
    volumeDigitado = valorDigitado;
    digitalWrite(PINO_RELAY1, HIGH);
    
    wdtResetFluxo = 0;
    _flagResetFluxo = false;
    
    passoMaquina = mCALCULA_FLUXO;
  }
}

//WWWWWWWWWW*********************************************************************************
// Função: callPulse - Interruption INT SENSOR FLOW
// Data  : 09/09/2016 09:55
// Revisão  :
//WWWWWWWWWW*********************************************************************************

void callPulse(void ) 
{
  Npulse++;
}

//WWWWWWWWWW*********************************************************************************
// Função: calculaFluxo - Calcula o fluxo e volume de entrada
// Data  : 09/09/2016 09:55
// Revisão  :
//WWWWWWWWWW*********************************************************************************

void calculaFluxo(float alarmNivel) 
{
  if (_keyPress) 
  {
    if (_KEY_ESC) 
    {
      // Carrega os 5 segundos para zerar o fluxo
      if(wdtResetFluxo == 0 && !_flagResetFluxo) 
      {
        wdtResetFluxo = 5;
        _flagResetFluxo = true;
      }
    }
    else if (_KEY_SELECT) 
    {
      if(_auxAdjustCalFactor) 
      {
        // Desatraca o relé
        digitalWrite(PINO_RELAY1, LOW);
        // Limpa a flag de sinalização de calibração
        _auxAdjustCalFactor = false;
        // Cálcula o fator automaticamente
        passoMaquina = mAUTO_CAL_FACTOR;
          return;
      }
      // Se não for calibração, simplesmente para a contagem
      else {
        Npulse = 0;
        _stopCountFlux = !_stopCountFlux;
        digitalWrite(PINO_RELAY1, !_stopCountFlux);  
      }
    }
  }
  else if(_flagResetFluxo) 
  {
    uint8_t auxReadADC = analogRead(PINO_TECLADO);
    
    /*
      Verifica se o botão ESC não está pressionado, se já soltou,
      volta para o menu, se não, continua contando para zerar
    */

    if(auxReadADC > 50) 
    {
      wdtResetFluxo = 0;
      _flagResetFluxo = false;
      digitalWrite(PINO_RELAY1, LOW);
      
      // Se é calibração
      if(_auxAdjustCalFactor) 
      {
        // Limpa a flag de sinalização de calibração
        _auxAdjustCalFactor = false;
        // Cálcula o fator automaticamente
        passoMaquina = mMENU_CAL_FACTOR;
      }
      else passoMaquina = mMENU;
      return;
    }
  }

  else if (_flag1000ms && !_stopCountFlux) 
  {
    _flag1000ms = false;
    detachInterrupt(digitalPinToInterrupt(PINO_FLUXO));
    flow = (Npulse / configGeral.calFactor);
    milli = (flow / 60);
    liters = liters + milli;
    Npulse = 0;
    attachInterrupt(digitalPinToInterrupt(PINO_FLUXO), callPulse, RISING);
    if ((liters > 0) && (alarmNivel > 0)) 
    {
      if (liters >= alarmNivel) 
      {
        digitalWrite(PINO_RELAY1, LOW);

        // Se era calibração
        if(_auxAdjustCalFactor) 
        { 
          printDisplay(F(" Volume Inserido"), F("  Foi Atingido! "), TEMPO_CONF);
          // Limpa a flag de sinalização de calibração
          _auxAdjustCalFactor = false;
          // Cálcula o fator automaticamente
          passoMaquina = mAUTO_CAL_FACTOR;
          // Retorna
          return;
        }
        else if (_keyPress) 
        {
          if (_KEY_ESC) 
          { passoMaquina = mMENU; return; }
        }
        if (_flag3000ms)
          bipInicial();
      }
    }

    // Envia os dados do fluxo
    if(_connectedAPP)
      sendDataFlux();
  }
}

//WWWWWWWWWW*********************************************************************************
// Função: sendDataFlux
// Data  : 10/09/2019 15:30
//WWWWWWWWWW*********************************************************************************

void sendDataFlux(void ) 
{
  // Informações sobre o dado (tipo do cabeçalho, tipo do dado e tamanho do dado)
  uint8_t auxInfoData[3] = {_HEADER_EVENT, _COUNT_FLUX, 1};
  // Dado em si, com o número do comando
  String auxSendData[3];

  // Número do comando
  auxSendData[0] = F("03");
  auxSendData[1] = String(flow);
  auxSendData[2] = String(liters);
  
  // Envia o dado ao APP
  sendDataAPP(&auxInfoData[0], &auxSendData[0]);
}

//WWWWWWWWWW*********************************************************************************
// Função: adjustCalFactor
// Data  : 19/09/2019 14:46
//WWWWWWWWWW*********************************************************************************

void adjustCalFactor(float auxAdjustCalFactor, boolean auxTypeAdjust, boolean auxCommand) 
{
  // Se for ajuste automático, faz o cálculo, sendo auxAdjustCalFactor os litros inseridos
  if(auxTypeAdjust == _AUTO_ADJUSTMENT)
    configGeral.calFactor = ((auxAdjustCalFactor * configGeral.calFactor) / liters);
  // Se for ajuste manual, atualiza o valor diretamente
  else if(auxTypeAdjust == _MANUAL_ADJUSTMENT)
    configGeral.calFactor = auxAdjustCalFactor;

  // Fixa o valor máximo em 10
  if(configGeral.calFactor > 10)
    configGeral.calFactor = 10;
  
  // Salva a configuração
  salvaConfigGeral(auxCommand);
}

//WWWWWWWWWW*********************************************************************************
// Função: manualCalFactor
// Data  : 13/12/2019 14:00
//WWWWWWWWWW*********************************************************************************

void manualCalFactor(void )
{
  if (_keyPress && (_KEY_LEFT || _KEY_ESC)) 
  {
    // Reseta a entrada de dados
    resetDataInput(false);
    // Volta ao menu
    passoMaquina = mMENU_CAL_FACTOR;
    // Retorna
    return;
  } 
  
  // Configura o dado em questão
  entradaValorGeral(configGeral.calFactor, 1, MAX_CAL_FACTOR, true);
  // Se finalizou a configuração
  if (_endValue) 
  {
    // Reseta a entrada de dados
    resetDataInput(false);
    // Configura o valor
    adjustCalFactor(valorDigitado, _MANUAL_ADJUSTMENT, _CONFIG_MENU);
    // Retorna ao menu
    passoMaquina = mMENU_CAL_FACTOR;
    // Retorna
    return;
  }
}

//WWWWWWWWWW*********************************************************************************
// Função: autoCalFactor
// Data  : 13/12/2019 14:05
//WWWWWWWWWW*********************************************************************************

void autoCalFactor(void )
{
  // Configura o novo fator
  adjustCalFactor(volumeDigitado, _AUTO_ADJUSTMENT, _CONFIG_MENU);
  
  // Apresenta na tela o novo fator
  printDisplay(F(" Valor do Fator "), String("Ajustado: " + String(configGeral.calFactor, 1)), TEMPO_CONF+1);
  
  // Retorna ao menu
  passoMaquina = mMENU_CAL_FACTOR;

  // Reseta o fluxo
  volumeDigitado = 0;
  flow = 0; liters = 0;
  milli = 0; Npulse = 0;
}

