//*******************************************************************************************
// Arquivo C/C++ do Projeto Controlador de Patamares -- ENSABrewControll
//-------------------------------------------------------------------------------------------
//
// Arquivo: ENSABrewProducao.ino
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
// Função: loopProducao
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void loopProducao(void ) 
{ 
  // Mede velocidade
  calculaGrauPorMinuto(mediaTemperature); 
  setStatusFase(mediaTemperature);

  if(passoMaquina != mLOOP_WHIRLPOOL && passoMaquina != mLOOP_DESCANSO)
  {
    tempoRestante = (patamar[numPatamar].tempo * 60) + (valorTempoManual * 60) - tempoInicial;
    if (tempoInicial > 0) 
    {
      if (tempoRestante <= 0) 
      {
        // Salva patamar como concluido
        receitaSel.patamar[numPatamar].concluido = true; 
        salvaReceita(receitaSel);
        numPatamar++;
        _temperaturaMaxEntrou = false;
        if (numPatamar >= qtdePatamar) 
        {
          desligaAquecimento();
          if (passoMaquina == mLOOP_FERVURA) 
          {
            bipOK(); printDisplay(F(" Fim da FERVURA "), F("  Com Sucesso!  "), TEMPO_ALARME);
            
            // Se houver Whirlpool
            if(receitaSel.qtdeWhirlpool != 0)
            {
              passoMaquina = mMENU_WHIRLPOOL;
              receitaSel.statusBrassagem = _WHIRLPOOL; 
            }
            // Se houver Descanso 
            else if(receitaSel.qtdeDescanso != 0)
            {
              passoMaquina = mMENU_DESCANSO;
              receitaSel.statusBrassagem = _DESCANSO; 
            }
            // Se não, finaliza a produção
            else { fimProducao(); return; }
            
            salvaReceita(receitaSel); 
          }
          else 
          {
            bipOK();
            printDisplay(F(" Fim PATAMARES "), F("  Com Sucesso!  "), TEMPO_ALARME);
            passoMaquina = mMENU_FERVURA;
          }
        }
        else 
        {
          bipOK();
          printDisplay(F(" Inicio de Novo "), " PATAMAR N: " + String(numPatamar + 1), TEMPO_ALARME);
          valorTempoManual = 0;
          _loadTimerPwm = false;
          _pwmFull = false;
        }
        desligaLeds();
        valorTempoManual = 0;
        _contaTempo = false; //Quando "false" paraliza o "tempoDecorrido" durante chegada no patamar
        tempoInicial = 0;
        tempoProximo = 0;

        if(receitaSel.statusBrassagem != _STOP)
          wdtDadosBrassagem = configGeral.tempoEnvioBrassagem;

        return;
      }
    }
  }

  char tempoAtual[17];
  char tempoIncremental[17];
  char minutoAtual[17];
  char segundoAtual[17];

  char auxBufferTempo[17];

  sprintf(tempoAtual, "%02u:%02u", tempoRestante / 60, tempoRestante % 60);
  sprintf(tempoIncremental, "%02u:%02u", tempoProximo / 60, tempoProximo % 60);

  sprintf(minutoAtual, "%02u", tempoRestante / 60);
  sprintf(segundoAtual, "%02u", tempoRestante % 60);

  if (passoMaquina == mLOOP_FERVURA) 
  { 
    //Adicao fervura 
    int tempoRestanteAdicao;

    if(qtdeAdicao == 0)
      tempoRestanteAdicao = (receitaSel.qtdeMinFervura * 60) - tempoProximo;
    else if (numAdicao == 0)
      tempoRestanteAdicao = ((receitaSel.qtdeMinFervura * 60) - (adicaoFervura[numAdicao].tempo * 60)) - tempoProximo;
    else if (numAdicao < qtdeAdicao)
    {
      if( adicaoFervura[numAdicao - 1].tempo == adicaoFervura[numAdicao].tempo )
        tempoRestanteAdicao = (adicaoFervura[numAdicao].tempo * 60) - tempoProximo;
      else
        tempoRestanteAdicao = ((adicaoFervura[numAdicao - 1].tempo * 60) - (adicaoFervura[numAdicao].tempo * 60)) - tempoProximo;
    }
    else
      tempoRestanteAdicao = (adicaoFervura[numAdicao - 1].tempo * 60) - tempoProximo;
    if (tempoRestanteAdicao <= 0)
      tempoRestanteAdicao = 0;

    sprintf(auxBufferTempo, "%02u:%02u", tempoRestanteAdicao / 60, tempoRestanteAdicao % 60);
    sprintf(minutoAtual, "%02u",tempoRestanteAdicao / 60);
    processaFervura(tempoAtual);

    if (_sendDataApp && _connectedAPP) 
    {
      _sendDataApp = false;

      // Informações sobre o dado (tipo do cabeçalho, tipo do dado e tamanho do dado)
      uint8_t auxInfoData[3] = {_HEADER_GET, _LOOP_FERVURA, 1};
      // Dado em si, com o número do comando
      String auxSendData[6];

      // Número do comando
      auxSendData[0] = F("2");
      auxSendData[1] = String(tempoRestanteAdicao);
      auxSendData[2] = String(auxBufferTempo);
      auxSendData[3] = String(numAdicao);
      auxSendData[4] = String((getValorPwm()*100)/1024);
      auxSendData[5] = String(tempoIncremental);

      // Envia o dado ao APP
      sendDataAPP(&auxInfoData[0], &auxSendData[0]);
    }
  }

  else if (passoMaquina == mLOOP_WHIRLPOOL) 
  { 
    //Whirlpool
    int tempoRestanteWhirlpool;
    tempoRestanteWhirlpool = (whirlpoolFervura[numWhirlpool].tempo * 60) - tempoProximo;
    if (tempoRestanteWhirlpool <= 0)
      tempoRestanteWhirlpool = 0;

    sprintf(auxBufferTempo, "%02u:%02u", tempoRestanteWhirlpool / 60, tempoRestanteWhirlpool % 60);
    sprintf(minutoAtual, "%02u",tempoRestanteWhirlpool / 60);
    
    // processaWhirlpool(tempoIncremental); // Esse é o tempo referência para o ALARME (Compara com a EEPROM)
    processaWhirlpool(tempoRestanteWhirlpool);

    if (_sendDataApp && _connectedAPP) 
    {
      _sendDataApp = false;

      // Informações sobre o dado (tipo do cabeçalho, tipo do dado e tamanho do dado)
      uint8_t auxInfoData[3] = {_HEADER_GET, _LOOP_WHIRLPOOL, 1};
      // Dado em si, com o número do comando
      String auxSendData[5];

      // Número do comando
      auxSendData[0] = F("2");
      auxSendData[1] = String(tempoRestanteWhirlpool);
      auxSendData[2] = String(auxBufferTempo);
      auxSendData[3] = String(numWhirlpool);
      auxSendData[4] = String(tempoIncremental);
      
      // Envia o dado ao APP
      sendDataAPP(&auxInfoData[0], &auxSendData[0]);
    }
  }

  else if (passoMaquina == mLOOP_DESCANSO) 
  { 
    //Descanso
    int tempoRestanteDescanso;
    tempoRestanteDescanso = (descansoFervura[numDescanso].tempo * 60) - tempoProximo;
    if (tempoRestanteDescanso <= 0)
      tempoRestanteDescanso = 0;

    sprintf(auxBufferTempo, "%02u:%02u", tempoRestanteDescanso / 60, tempoRestanteDescanso % 60);
    sprintf(minutoAtual, "%02u",tempoRestanteDescanso / 60);
    // processaDescanso(tempoIncremental); // Esse é o tempo referência para o ALARME (Compara com a EEPROM)
    processaDescanso(tempoRestanteDescanso);

    if (_sendDataApp && _connectedAPP) 
    {
      _sendDataApp = false;

      // Informações sobre o dado (tipo do cabeçalho, tipo do dado e tamanho do dado)
      uint8_t auxInfoData[3] = {_HEADER_GET, _LOOP_DESCANSO, 1};
      // Dado em si, com o número do comando
      String auxSendData[5];

      // Número do comando
      auxSendData[0] = F("2");
      auxSendData[1] = String(tempoRestanteDescanso);
      auxSendData[2] = String(auxBufferTempo);
      auxSendData[3] = String(numDescanso);
      auxSendData[4] = String(tempoIncremental);

      // Envia o dado ao APP
      sendDataAPP(&auxInfoData[0], &auxSendData[0]);
    }
  }

  else 
  {
    sprintf(auxBufferTempo, "%02u:%02u", tempoRestante / 60, tempoRestante % 60);
    
    if (_sendDataApp && _connectedAPP) 
    {
      _sendDataApp = false;
      
      // Informações sobre o dado (tipo do cabeçalho, tipo do dado e tamanho do dado)
      uint8_t auxInfoData[3] = {_HEADER_GET, _LOOP_PATAMARES, 2};
      // Dado em si, com o número do comando
      String auxSendData[7];

      // Número do comando
      auxSendData[0] = F("2");
      auxSendData[1] = String((float)(mediaTemperature), 2);
      auxSendData[2] = String(statusFase);
      auxSendData[3] = String((getValorPwm()*100)/1024);
      auxSendData[4] = String((float)(speedAquecimento), 2);
      auxSendData[5] = String(numPatamar + 1);
      auxSendData[6] = String(auxBufferTempo);

      // Envia o dado ao APP
      sendDataAPP(&auxInfoData[0], &auxSendData[0]);
    }
  }

  // Carrega as mensagens
  String auxString[2];

  if (passoMaquina == mLOOP_FERVURA) 
  {
    auxString[0] = String(getStatusFase() + " " + String(tempoRestante / 60) + " " + String(valorPwm) + "%");
    auxString[1] = String(String((float)(mediaTemperature), 1) + "/" + String(patamar[numPatamar].temperaturaMax) + "C " + auxBufferTempo);
  }
  else if (passoMaquina == mLOOP_WHIRLPOOL) 
  {
    String actualWhirlpool;
    if(numWhirlpool == 0)
      actualWhirlpool = F("LENTO");
    else
      actualWhirlpool = F("RAPIDO");

    auxString[0] = String(getStatusFase() + " " + actualWhirlpool);
    auxString[1] = String(String((float)(mediaTemperature), 1) + String(char(223)) + "C / " + auxBufferTempo);
  }
  else if (passoMaquina == mLOOP_DESCANSO) 
  { 
    auxString[0] = F("    DESCANSO    ");
    auxString[1] = String(String((float)(mediaTemperature), 1) + String(char(223)) + "C / " + auxBufferTempo);
  }
  else 
  {
    auxString[0] = String(getStatusFase() + " - " + String(numPatamar + 1) + "/" + String(qtdePatamar));
    if (String(tempoAtual) == F("00:00"))
      sprintf(tempoAtual, "%02u:%02u", patamar[numPatamar].tempo, 0);
    auxString[1] = String(String((float)(mediaTemperature), 1) + "/" + String(patamar[numPatamar].temperaturaMax) + "C " + tempoAtual);
  }

  // Apresenta as mensagens na tela
  centralWritingScreen(auxString[0], auxString[1]);
}

//WWWWWWWWWW*********************************************************************************
// Função: continuaProducao
// Data  : 16/12/2019 10:00
//WWWWWWWWWW*********************************************************************************

void continuaProducao(void )
{
  if (receitaSel.statusBrassagem != _STOP) 
  { //Brassagem já iniciada
    perguntaSimNao();
    if (_endString) 
    {
      _endString = false;
      if (stResp == ST_RESP_NAO) 
      {
        receitaSel.statusBrassagem = _STOP; //Reseta o "statusBrassagem"
        for (register int ind = 0; ind < qtdePatamar; ind++) //Reseta todos os Patamares
          receitaSel.patamar[ind].concluido = false;
        salvaReceita(receitaSel);
      }
      passoMaquina = mEXECUTA_PRODUCAO; return;
    }
  }
  else 
  { passoMaquina = mEXECUTA_PRODUCAO; return; }
}

//WWWWWWWWWW*********************************************************************************
// Função: checkESC
// Data  : 19/11/2019 11:04
//WWWWWWWWWW*********************************************************************************

boolean checkESC(void )
{
  // Verifica se é saída do processo de brassagem
  if (_KEY_ESC) 
  {
    auxKeyEsc = true;
    passoMaquina = mLOOP_DESCANSO;
    return(true);
  }
  return(false);
}

//WWWWWWWWWW*********************************************************************************
// Função: loopBrassagem
// Data  : 19/11/2019 11:04
//WWWWWWWWWW*********************************************************************************

void loopBrassagem(void )
{
  if (_keyPress) 
  {
    if (_KEY_RIGHT) 
    {
      numPatamar++;
      _contaTempo = false;
      _pwmFull = false;
      _loadTimerPwm = false;
      tempoInicial = 0;
      tempoProximo = 0;

      if(_connectedAPP)
        wdtDadosBrassagem = configGeral.tempoEnvioBrassagem;
     
      if (numPatamar >= qtdePatamar) 
      {
        numPatamar = qtdePatamar - 1;
        passoMaquina = mMENU_FERVURA;
        return;
      }
    }
    else if (_KEY_LEFT) 
    {
      numPatamar--;
      _contaTempo = false;
      _pwmFull = false;
      _loadTimerPwm = false;
      tempoInicial = 0;
      tempoProximo = 0;
      
      if(_connectedAPP)
        wdtDadosBrassagem = configGeral.tempoEnvioBrassagem;
      
      if (numPatamar == 0) 
      {
        _temperaturaMaxEntrou = false;
        _contaTempo = false;
        numPatamar = 0;
        tempoInicial = 0;
        tempoProximo = 0;
        
        if(_connectedAPP)
          wdtDadosBrassagem = configGeral.tempoEnvioBrassagem;
        
        qtdePatamar = receitaSel.qtdePatamar;

        for (register int ind = 0; ind < qtdePatamar; ind++) 
        {
          patamar[ind] = receitaSel.patamar[ind];
          if (receitaSel.statusBrassagem == _STOP) //Parada
            patamar[ind].concluido = false;
        }
        salvaReceita(receitaSel);
      }

      //Atenção: "numPatamar": declarado como: "byte"
      if (numPatamar == 255) 
      { 
        numPatamar = 0;
        tempoInicial = 0;
        _derrubaMalte = false;
        
        if(_connectedAPP)
          wdtDadosBrassagem = configGeral.tempoEnvioBrassagem;
        
        receitaSel.statusBrassagem = _STOP;
        qtdePatamar = receitaSel.qtdePatamar;

        //Reseta todos os Patamares
        for (register int ind = 0; ind < qtdePatamar; ind++) 
          receitaSel.patamar[ind].concluido = false;
        
        salvaReceita(receitaSel);
        desligaAquecimento();
        passoMaquina = mMENU;
        return;
      }
    }
    // Altera o valor do tempo
    else if ((_KEY_UP || _KEY_DOWN) && contandoTempo()) 
    {
      entradaTempoManual(F("Tempo"), valorTempoManual, -500, 500);
      valorTempoManual = valorDigitado;
      return;
    }
    // Verifica se é saída do processo de brassagem
    else if (checkESC()) 
      return;
  }

  // Loop de produção
  loopProducao();
}

//WWWWWWWWWW*********************************************************************************
// Função: loopBrassagem
// Data  : 19/11/2019 11:35
//WWWWWWWWWW*********************************************************************************

void derrubarMalte(void )
{
  if (_keyPress) 
  {
    if (_KEY_SELECT) 
    {
      desligaLeds();
      _derrubaMalte = true;
      _contaTempo = true;
      
      if(_connectedAPP)
      {
        _sendDataApp = true;
        wdtSendTemp = configGeral.tempoEnvioBrassagem;
        wdtDadosBrassagem = configGeral.tempoEnvioBrassagem;
      }
      
      passoMaquina = mLOOP_BRASSAGEM;
      return;
    }
    // Verifica se é saída do processo de brassagem
    else if (checkESC()) 
      return;
  }
  if (_flag3000ms)
    bipInicial();
}

//WWWWWWWWWW*********************************************************************************
// Função: executaProducao
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void executaProducao(void ) 
{
  _temperaturaMaxEntrou = false;
  numPatamar = 0;
  tempoInicial = 0;
  tempoProximo = 0;
  _contaTempo = false;
  qtdePatamar = receitaSel.qtdePatamar;
  bipAcerto();

  for (register int ind = 0; ind < qtdePatamar; ind++) 
  {
    patamar[ind] = receitaSel.patamar[ind];
    if (receitaSel.statusBrassagem == _STOP) //Parada
      patamar[ind].concluido = false;
    else if (receitaSel.statusBrassagem == _PRODUCAO) 
    { 
      if (patamar[ind].concluido == true) 
      {
        numPatamar = ind + 1;
      }
    }
  }
  if (receitaSel.statusBrassagem < _FERVURA) 
  {
    receitaSel.statusBrassagem = _PRODUCAO; //Em producao
    salvaReceita(receitaSel);
    passoMaquina = mLOOP_BRASSAGEM;
    wdtSendTemp = configGeral.tempoEnvioBrassagem;
    return;
  }
  else if (receitaSel.statusBrassagem == _FERVURA) 
  {
    desligaAquecimento();
    passoMaquina = mMENU_FERVURA;
    return;
  }
  else if (receitaSel.statusBrassagem == _WHIRLPOOL) 
  {
    desligaAquecimento();
    passoMaquina = mMENU_WHIRLPOOL;
    return;
  }
  else if (receitaSel.statusBrassagem == _DESCANSO) 
  {
    desligaAquecimento();
    passoMaquina = mMENU_DESCANSO;
    return;
  }
}

//WWWWWWWWWW*********************************************************************************
// Função: menuExecutaFervura
// Data  : 19/11/2019 11:38
//WWWWWWWWWW*********************************************************************************

void menuExecutaFervura(void )
{
  if (_keyPress) 
  {
    if (_KEY_SELECT) 
    {
      passoMaquina = mEXECUTA_FERVURA;
      return;
    }
    // Verifica se é saída do processo de brassagem
    else if (checkESC()) 
      return;
  }
  if (_flag3000ms)
    bipInicial();
}

//WWWWWWWWWW*********************************************************************************
// Função: executaFervura
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void executaFervura(void ) 
{
  desligaLeds();
  _temperaturaMaxEntrou = false;
  tempoInicial = 0;
  tempoProximo = 0;
  numPatamar = 0;
  _contaTempo = false;
  patamar[0].temperaturaMax = receitaSel.tempFervura;
  patamar[0].tempo = receitaSel.qtdeMinFervura;
  qtdePatamar = 1;
  numAdicao = 0;
  valorPwmManual = patamar[0].temperaturaMax;
  qtdeAdicao = receitaSel.qtdeAdicao;

  for (register int ind = 0; ind < qtdeAdicao; ind++)
    adicaoFervura[ind] = receitaSel.adicao[ind];
  receitaSel.statusBrassagem = _FERVURA; //Fervura
  salvaReceita(receitaSel);
  passoMaquina = mLOOP_FERVURA;
  wdtSendTemp = configGeral.tempoEnvioBrassagem;

  // Verifica a saída da brassagem
  if (_keyPress) 
  {
    if(checkESC())
      return;
  }
}

//WWWWWWWWWW*********************************************************************************
// Função: loopFervura
// Data  : 19/11/2019 11:42
//WWWWWWWWWW*********************************************************************************

void loopFervura(void )
{
  if (_keyPress) 
  {
    if (_KEY_SELECT) 
    {
      alteraSetpoint = !alteraSetpoint; 
      if(_connectedAPP)
      {
        _sendDataApp = true;
        wdtDadosBrassagem = configGeral.tempoEnvioBrassagem;
      }
      return;
    }
    else if (_KEY_RIGHT)
    {
      if (receitaSel.qtdeWhirlpool == 0) 
      {
        passoMaquina = mMENU_DESCANSO;
        if (receitaSel.qtdeDescanso == 0)
          fimProducao();
        return;
      }
      else 
      {
        passoMaquina = mMENU_WHIRLPOOL;
        return;
      }
    }
    else if (_KEY_LEFT) 
    {
      _contaTempo = false;
      tempoInicial = 0;
      tempoProximo = 0;
      
      receitaSel = regReceita[idReceitaSel];
      receitaSel.statusBrassagem = _PRODUCAO;
      
      qtdePatamar = receitaSel.qtdePatamar;
      numPatamar = qtdePatamar - 1;
      
      passoMaquina = mLOOP_BRASSAGEM;
      
      if(_connectedAPP)
      {
        wdtSendTemp = configGeral.tempoEnvioBrassagem;
        wdtDadosBrassagem = configGeral.tempoEnvioBrassagem;
      }
      
      return;
    }
    else if (_KEY_UP || _KEY_DOWN) 
    {
      if (alteraSetpoint)
      {
        entradaSetPointManual(F("SetPoint"), valorSetpointManual, 0, MAX_SETPOINT);
        valorSetpointManual = valorDigitado;
        patamar[0].temperaturaMax = receitaSel.tempFervura + valorSetpointManual;
        return;
      }
      else
      {
        entradaPwmManual(F("% PWM"), valorPwmManual, 0, MAX_PWM);
        valorPwmManual = valorDigitado;
        return;
      }
    }
    // Verifica a saída da brassagem
    else if(checkESC())
      return;
  }
  
  loopProducao();
}

//WWWWWWWWWW*********************************************************************************
// Função: menuAdicaoFervura
// Data  : 19/11/2019 11:50
//WWWWWWWWWW*********************************************************************************

void menuAdicaoFervura(void )
{
  if (_keyPress) 
  {
    if (_KEY_SELECT) 
    {
      // Passa para a próxima adição
      numAdicao++;

      // Auxilia na saída do menu em questão
      boolean _sairMenuAdicao = false;
      // Se já atingiu a quantidade de adições, sai do menu
      if(numAdicao >= qtdeAdicao)
        _sairMenuAdicao = true;
      // Se o tempo da adição atual for diferente do tempo da adição anterior, sai do menu
      else if(adicaoFervura[numAdicao].tempo != adicaoFervura[numAdicao - 1].tempo)
        _sairMenuAdicao = true;

      // Se a saída do menu está sinalizada
      if(_sairMenuAdicao)
      {
        desligaLeds();
        passoMaquina = mLOOP_FERVURA;
        wdtSendTemp = configGeral.tempoEnvioBrassagem;
      }

      return;
    }
    // Verifica a saída da brassagem
    else if(checkESC())
      return;
  }
  if (_flag3000ms)
    bipInicial();
  if (_flag1000ms) 
  {
    _flag1000ms = false;
    tempoInicial++;
    tempoProximo++;
  }
}

//WWWWWWWWWW*********************************************************************************
// Função: menuExecutaWhirlpool
// Data  : 19/11/2019 11:53
//WWWWWWWWWW*********************************************************************************

void menuExecutaWhirlpool(void )
{
  if (_keyPress) 
  {
    if (_KEY_SELECT) 
    {
      passoMaquina = mEXECUTA_WHIRLPOOL;
      return;
    }
    // Verifica a saída da brassagem
    else if(checkESC())
      return;
  }
  if (_flag3000ms)
    bipInicial();
}

//WWWWWWWWWW*********************************************************************************
// Função: executaWhirlpool
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void executaWhirlpool(void ) 
{
  desligaLeds();
  _temperaturaMaxEntrou = false;
  tempoInicial = 0;
  tempoProximo = 0;
  numPatamar = 0;
  _contaTempo = false;
  patamar[0].tempo = receitaSel.qtdeMinWhirlpool;
  qtdePatamar = 1;
  numWhirlpool = 0;
  valorPwmManual = MAX_PWM;
  qtdeWhirlpool = receitaSel.qtdeWhirlpool;

  for (register int ind = 0; ind < qtdeWhirlpool; ind++)
    whirlpoolFervura[ind] = receitaSel.whirlpool[ind];
  receitaSel.statusBrassagem = _WHIRLPOOL; //Whirlpool
  salvaReceita(receitaSel);
  passoMaquina = mLOOP_WHIRLPOOL;

  if(_connectedAPP)
    wdtSendTemp = configGeral.tempoEnvioBrassagem;

  bipAtencao();
  printDisplay(F("Iniciando Etapa"), ("WHIRLPOOL " + actualWhirlpool(numWhirlpool)), TEMPO_ALARME);

  // Verifica a saída da brassagem
  if (_keyPress) 
  {
    if(checkESC())
      return;
  }
}

//WWWWWWWWWW*********************************************************************************
// Função: loopWhirlpool
// Data  : 19/11/2019 11:57
//WWWWWWWWWW*********************************************************************************

void loopWhirlpool(void ) 
{
  if (_keyPress) 
  {
    if ((_KEY_UP || _KEY_DOWN) && contandoTempo()) 
    {
      // entradaTempoManual(F("Tempo"), valorTempoManual, -500, 500);
      // valorTempoManual = valorDigitado;
      if (_KEY_UP)
        tempoProximo = tempoProximo - 60;
      else if (_KEY_DOWN)
        tempoProximo = tempoProximo + 60;
      return;
    }

    else if (_KEY_RIGHT) 
    {
      tempoInicial = 0;
      tempoProximo = 0;
      valorTempoManual = 0;

      numWhirlpool++;
      if(numWhirlpool >= qtdeWhirlpool)
        passoMaquina = mEXECUTA_DESCANSO;
      else
      {
        bipAtencao();
        printDisplay(F("Iniciando Etapa"), ("WHIRLPOOL " + actualWhirlpool(numWhirlpool)), TEMPO_ALARME);
        desligaLeds();
      } 
      
      if(_connectedAPP)
        wdtDadosBrassagem = configGeral.tempoEnvioBrassagem;
      
      return;
    }
    else if (_KEY_LEFT) 
    {
      receitaSel.statusBrassagem = _FERVURA;
      _contaTempo = false;
      tempoInicial = 0;
      tempoProximo = 0;
      passoMaquina = mEXECUTA_FERVURA;
      
      if(_connectedAPP)
        wdtDadosBrassagem = configGeral.tempoEnvioBrassagem;
      
      return;
    }
    // Verifica a saída da brassagem
    else if(checkESC())
      return;
  }
  loopProducao();
}

//WWWWWWWWWW*********************************************************************************
// Função: menuExecutaDescanso
// Data  : 19/11/2019 11:59
//WWWWWWWWWW*********************************************************************************

void menuExecutaDescanso(void )
{
  if (_keyPress) 
  {
    if (_KEY_SELECT) 
    {
      passoMaquina = mEXECUTA_DESCANSO;
      return;
    }
    // Verifica a saída da brassagem
    else if(checkESC())
      return;
  }
  if (_flag3000ms)
    bipInicial();
}

//WWWWWWWWWW*********************************************************************************
// Função: executaDescanso
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void executaDescanso(void ) 
{
  desligaLeds();
  _temperaturaMaxEntrou = false;
  tempoInicial = 0;
  tempoProximo = 0;
  numPatamar = 0;
  patamar[0].tempo = receitaSel.qtdeMinDescanso;
  qtdePatamar = 1;
  numDescanso = 0;
  valorPwmManual = MAX_PWM;
  qtdeDescanso = receitaSel.qtdeDescanso;

  for (register int ind = 0; ind < qtdeDescanso; ind++)
    descansoFervura[ind] = receitaSel.descanso[ind];
  receitaSel.statusBrassagem = _DESCANSO; //Descanso
  salvaReceita(receitaSel);

  if(qtdeDescanso == 0)
    fimProducao();
  else
  {
    passoMaquina = mLOOP_DESCANSO;
    // Aviso sonoro
    bipAtencao();
    // Mostra na tela o ínicio do próximo descanso
    printDisplay(F(" Iniciando Etapa"), F("   de DESCANSO  "), TEMPO_ALARME);
  } 

  // Verifica a saída da brassagem
  if(checkESC())
    return;
}

//WWWWWWWWWW*********************************************************************************
// Função: loopDescanso
// Data  : 
//WWWWWWWWWW*********************************************************************************

void loopDescanso(void )
{
  boolean auxFimProcesso = false;
  if (_keyPress || auxKeyEsc) 
  {
    if (_KEY_RIGHT || auxKeyEsc) 
    {
      _KEY_RIGHT = false;
      _KEY_ESC = true;
      auxFimProcesso = true;
    }
    else if (_KEY_LEFT) 
    {
      _contaTempo = false;
      tempoInicial = 0;
      tempoProximo = 0;

      if(_connectedAPP)
        wdtDadosBrassagem = configGeral.tempoEnvioBrassagem;

      if (receitaSel.qtdeWhirlpool == 0) 
      {
        receitaSel.statusBrassagem = _FERVURA;
        passoMaquina = mMENU_FERVURA;
        return;
      }
      else 
      {
        receitaSel.statusBrassagem = _WHIRLPOOL;
        passoMaquina = mEXECUTA_WHIRLPOOL;
        return;
      }
    }
    else if ((_KEY_UP || _KEY_DOWN) && contandoTempo()) 
    {
      if (_KEY_UP)
        tempoProximo = tempoProximo - 60;
      else if (_KEY_DOWN)
        tempoProximo = tempoProximo + 60;
      return;
    }
    if (_KEY_ESC || auxKeyEsc) 
    {
      tempoInicial = 0;
      _derrubaMalte = false;
      receitaSel.statusBrassagem = _STOP;
      qtdePatamar = receitaSel.qtdePatamar;
      //Reseta todos os Patamares
      for (register int ind = 0; ind < qtdePatamar; ind++) 
        receitaSel.patamar[ind].concluido = false;
      
      salvaReceita(receitaSel); bipInicial();

      if (auxKeyEsc || _KEY_ESC)
        printDisplay(F(" Etapa Abortada "), F("  Saida Remota  "), TEMPO_ALARME);
      else if (auxFimProcesso)
        printDisplay(F("Fim do Processo"), F("Etapas Completas"), TEMPO_ALARME);
      
      desligaAquecimento();
      passoMaquina = mSTANDBY;
      auxKeyEsc = false;

      // Recarrega o envio da temperatura
      wdtSendTemp = configGeral.timeSendTemp; // TIME_SEND_TEMPERATURE;
      // Retorna
      return;
    }
  }
  loopProducao();
}
//WWWWWWWWWW*********************************************************************************
// Função: processaFervura
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void processaFervura(String prTempoAtual) 
{ 
  // "prTempoAtual" - Esse tempo é decrescente!
  if (numAdicao < qtdeAdicao && contandoTempo()) 
  {
    char stFervChar[17]; // (adicaoFervura[numAdicao].tempo) + ":00";
    String stFerv, stPreFerv;
    if (numAdicao == 0 && adicaoFervura[numAdicao].tempo == receitaSel.qtdeMinFervura)
      stFerv = prTempoAtual;
    else 
    {
      sprintf(stFervChar, "%02u:%02u", adicaoFervura[numAdicao].tempo, 0);
      stFerv = stFervChar;
      sprintf(stFervChar, "%02u:%02u", adicaoFervura[numAdicao].tempo + 1, 0); // +1 decrescente
      stPreFerv = stFervChar;
    }
    if (prTempoAtual == stPreFerv) 
    {
      bipAtencao();
      printDisplay(F(" Para ADICAO: "), F("   +1 Minuto   "), TEMPO_ALARME);
    }
    if (prTempoAtual == stFerv) 
    {
      disparaAquecimento(mediaTemperature);
      tempoProximo = 0;
      passoMaquina = mADICAO_FERVURA;
      return;
    }
  }
}

//WWWWWWWWWW*********************************************************************************
// Função: processaWhirlpool
// Data  : 27/02/2020   14:15 --- 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void processaWhirlpool(int tempoRestanteWhirlpool) 
{ 
  // "prTempoAtual" - Esse tempo é crescente!
  if (numWhirlpool <= qtdeWhirlpool && contandoTempo()) 
  {
    // Faltando um minuto para acabar o whirlpool
    if(tempoRestanteWhirlpool == 60)
    {
      // Aviso sonoro
      bipAtencao();
      // Mostra na tela que falta um minuto para o fim do processo
      printDisplay(F("Para WHIRLPOOL:"), F("   +1 Minuto   "), TEMPO_ALARME);
    }
    else if(tempoRestanteWhirlpool == 0)
    {
      // Aviso sonoro
      bipAtencao();
      // Zera os temporizadores
      tempoInicial = 0; tempoProximo = 0;
      
      // Atualiza o whirlpool
      numWhirlpool++;
      // Se chegou ao fim do whirlpool
      if(numWhirlpool >= qtdeWhirlpool)
      {
        // Atualiza o estado atual da brassagem
        receitaSel.statusBrassagem = _DESCANSO; 
        // Salva o estado atual e emite aviso sonoro
        salvaReceita(receitaSel); bipOK();
        // Mostra na tela o fim do whirlpool
        printDisplay(F("Fim do WHIRLPOOL"), F("  Com Sucesso!  "), TEMPO_ALARME);
        // Altera o passo da máquina
        passoMaquina = mEXECUTA_DESCANSO;
      }
      // Se não chegou ao fim do whirlpool
      else
      {
        // Mostra na tela o ínicio do próximo whirlpool
        printDisplay(F("Iniciando Etapa"), ("WHIRLPOOL " + actualWhirlpool(numWhirlpool)), TEMPO_ALARME);
      }
      
      desligaLeds();
    }
  }
}

//WWWWWWWWWW*********************************************************************************
// Função: processaDescanso
// Data  : 27/02/2020   14:20 --- 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void processaDescanso(int tempoRestanteDescanso) 
{ 
  // 
  if (numDescanso < qtdeDescanso && contandoTempo()) 
  {
    // Faltando um minuto para acabar o whirlpool
    if(tempoRestanteDescanso == 60)
    {
      // Aviso sonoro
      bipAtencao();
      // Mostra na tela que falta um minuto para o fim do processo
     printDisplay(F("Para DESCANSO:"), F("   +1 Minuto   "), TEMPO_ALARME);
    }
    else if(tempoRestanteDescanso == 0)
    {
      // Aviso sonoro
      bipAtencao();
      // Zera os temporizadores
      tempoInicial = 0; tempoProximo = 0;
      
      // Atualiza o whirlpool
      numDescanso++;
      // Se chegou ao fim do Descanso
      if(numDescanso >= qtdeDescanso)
        fimProducao();
      // Se não chegou ao fim do whirlpool
      else
      {
        // Mostra na tela o ínicio do próximo descanso
        printDisplay(F(" Iniciando Etapa"), F("   de DESCANSO  "), TEMPO_ALARME);
      }
      
      desligaLeds();
    }
  }
}

//WWWWWWWWWW*********************************************************************************
// Função: fimProducao
// Data  : 20/02/2020   10:30
//WWWWWWWWWW*********************************************************************************

void fimProducao(void )
{
  _derrubaMalte = false;
  receitaSel.statusBrassagem = _STOP;
  qtdePatamar = receitaSel.qtdePatamar;
  for (register int ind = 0; ind < qtdePatamar; ind++) //Reseta todos os Patamares
    receitaSel.patamar[ind].concluido = false;
  salvaReceita(receitaSel); bipInicial();
  printDisplay(F("Fim do PROCESSO"), F("  Com Sucesso!  "), TEMPO_ALARME);
  executaBoasVindas(); passoMaquina = mSTANDBY;

  desligaLeds();
  valorTempoManual = 0;
  _contaTempo = false; //Quando "false" paraliza o "tempoDecorrido" durante chegada no patamar
  tempoInicial = 0;
  tempoProximo = 0;

  if(receitaSel.statusBrassagem != _STOP && _connectedAPP)
    wdtDadosBrassagem = configGeral.tempoEnvioBrassagem;
}

//WWWWWWWWWW*********************************************************************************
// Função: executaBrassagemRemota
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void executaBrassagemRemota(int type, byte cmd, JsonObject dataObject) 
{
  JsonArray nomeReceitaArray = dataObject[F("receita")];
  int idxReceita = nomeReceitaArray[RESET].as<int>();
  
  if (qtdeReceita <= 0) 
  {
    passoMaquina = mSTANDBY;
    code = F("E11"); // Empty Memory
    bipAtencao();
  }
  else if(idxReceita > qtdeReceita) 
  {
    passoMaquina = mSTANDBY;
    code = F("E12");
    bipAtencao();
  }
  else 
  {
    if (idxReceita <= 1)
      idxReceita = 1;
    receitaSel = regReceita[idxReceita - 1];
    receitaSel.statusBrassagem = _STOP;
    passoMaquina = mEXECUTA_PRODUCAO;
    code = F("A1"); // Success!
    bipAcerto();
  }
  setJsonData(type, cmd);
}

//WWWWWWWWWW*********************************************************************************
// Função: disparaAquecimento
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void disparaAquecimento(int prTemperatura) 
{
  Timer3.pwm(PINO_SSR, getValorPwm(), TIMER3); // Usa biblioteca - TIMER3
  calculaSpeed(prTemperatura);
}

//WWWWWWWWWW*********************************************************************************
// Função: desligaAquecimento
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void desligaAquecimento(void ) 
{
  digitalWrite(PINO_SSR, LOW); //Desligar
}

//WWWWWWWWWW*********************************************************************************
// Função: setStatusFase
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void setStatusFase(float prTemperatura) 
{
  float difTemp = patamar[numPatamar].temperaturaMax - prTemperatura; //Desaceleração do setpoint
  float tempMax = patamar[numPatamar].temperaturaMax;
  if (prTemperatura < tempMax && passoMaquina == mLOOP_BRASSAGEM && !contandoTempo()) 
  {
    desligaLeds();
    statusFase = RAMPA;
    if (numPatamar == 0)
      valorPwm = MAX_PWM;
    else 
    {
      if (difTemp >= 0.0 && difTemp <= 3.0) 
      {
        _loadTimerPwm = true;
        if (_pwmFull && (prTemperatura < tempMax))
          valorPwm = MAX_PWM;
        else
          valorPwm = configGeral.pwmSetpoint;
      }
      else
        valorPwm = MAX_PWM;
    }
    disparaAquecimento(prTemperatura);
  }
  else if (prTemperatura >= tempMax && passoMaquina == mLOOP_BRASSAGEM) 
  {
    piscaLedsAlarme();
    if(!_contaTempo && _connectedAPP) 
    {
      _sendDataApp = true;
      wdtDadosBrassagem = configGeral.tempoEnvioBrassagem;//0;      
    }
    _contaTempo = true;
    _loadTimerPwm = false;
    _pwmFull = false;
    valorPwm = RESET;
    statusFase = PATAMAR;
    desligaAquecimento();
    if (!_temperaturaMaxEntrou) 
    {
      _temperaturaMaxEntrou = true;
      bipInicial();
      printDisplay(F("Temperatura"), String((float)(patamar[numPatamar].temperaturaMax), 2) + "C Atingida", TEMPO_ALARME);
      if (numPatamar == 0 && !_derrubaMalte) 
      {
        _contaTempo = false;
        disparaAquecimento(prTemperatura);
        passoMaquina = mDERRUBAR_MALTE;
        return;
      }
    }
  }
  else if (prTemperatura < tempMax && passoMaquina == mLOOP_BRASSAGEM && tempoInicial > 0) 
  {
    desligaLeds();
    if(!_contaTempo && _connectedAPP) 
    {
      _sendDataApp = true;
      wdtDadosBrassagem = configGeral.tempoEnvioBrassagem;//0;      
    }
    _contaTempo = true;
    valorPwm = 50;
    statusFase = PATAMAR;
    if (difTemp >= 3.0)
      valorPwm = MAX_PWM;
    disparaAquecimento(prTemperatura);
  }
  else if (prTemperatura < tempMax && passoMaquina == mLOOP_FERVURA && tempoInicial >= 0) 
  {
    desligaLeds();
    valorPwm = valorPwmManual;
    statusFase = FERVURA;
    disparaAquecimento(prTemperatura);
  }
  else if (prTemperatura >= tempMax && passoMaquina == mLOOP_FERVURA) 
  {
    piscaLedsAlarme();
    if(!_contaTempo && _connectedAPP) 
    {
      _sendDataApp = true;
      wdtDadosBrassagem = configGeral.tempoEnvioBrassagem;//0;      
    }
    _contaTempo = true;
    valorPwm = valorPwmManual;
    statusFase = FERVURA;
    disparaAquecimento(prTemperatura);
    if (!_temperaturaMaxEntrou) 
    {
      _temperaturaMaxEntrou = true;
      bipInicial();
    }
  }
  else if (prTemperatura >= RESET && passoMaquina == mLOOP_WHIRLPOOL) 
  {
    desligaLeds();
    if(!_contaTempo && _connectedAPP) 
    {
      _sendDataApp = true;
      wdtDadosBrassagem = configGeral.tempoEnvioBrassagem;//0;      
    }
    _contaTempo = true;
    valorPwm = RESET;
    statusFase = WHIRLPOOL;
    desligaAquecimento();
  }
  else if (prTemperatura >= RESET && passoMaquina == mLOOP_DESCANSO) 
  {
    desligaLeds();
    if(!_contaTempo && _connectedAPP) 
    {
      _sendDataApp = true;
      wdtDadosBrassagem = configGeral.tempoEnvioBrassagem;//0;      
    }
    _contaTempo = true;
    valorPwm = RESET;
    statusFase = DESCANSO;
    desligaAquecimento();
  }
  else 
  {
    desligaLeds();
    _contaTempo = false;
    valorPwm = RESET;
    statusFase = FINAL; //SEM SENSOR
    desligaAquecimento();
  }
}