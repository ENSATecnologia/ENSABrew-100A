//*******************************************************************************************
// Arquivo C/C++ do Projeto Controlador de Patamares -- ENSABrewControll
//-------------------------------------------------------------------------------------------
//
// Arquivo: ENSABrewProducao.ino
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
// Function: loopProducao
// Date    : 04/04/2017 - 13:45
//WWWWWWWWWW*********************************************************************************

void loopProducao(void ) 
{ 
  // Mede velocidade
  // calculaGrauPorMinuto(mediaTemperature); 
  // setStatusFase(mediaTemperature);
  
  // ------------------------------------------------------------------------------------- //
  // Tempo restante - comparação

  static int32_t auxTpRest = 0;

  // ------------------------------------------------------------------------------------- //
  // Calcula o tempo restante

  tempoRestante = (patamar[numPatamar].tempo * 60) + (valorTempoManual * 60) - tempoInicial;

  // Se estiver no patamar, controla o envio de acordo com o tempo
  if(statusFase == PATAMAR)
  {
    if(auxTpRest != tempoRestante)
    {
      _sendDataApp = true;
      auxTpRest = tempoRestante;
    }
    else _sendDataApp = false;
  }

  // ------------------------------------------------------------------------------------- //
  // Envio do dado

  // Dado: (RAMPA/PATAMAR) | (N° RAMPA/PATAMAR) | (SENSOR) | (SETPOINT) | TEMPO | RELÉ

  if (_sendDataApp && _connectedAPP && _flagAuxBrassagem)
  {
    _sendDataApp = false;

    String auxSendData; 
    auxSendData.reserve(47);

    auxSendData = String(receitaSel.nome) + "|";
    
    if(statusFase == RAMPA)
      auxSendData += F("R|");
    else if(statusFase == PATAMAR) 
      auxSendData += F("P|");

    auxSendData += String(numPatamar+1) + F("|");
    auxSendData += String((float)mediaTemperature, 1) + F("|");
    auxSendData += String((float)actualTempSetPoint, 1) + F("|");
    auxSendData += String(tempoRestante) + F("|");
    auxSendData += String(digitalRead(PINO_RELAY1));

    Serial.println(auxSendData); Serial1.println(auxSendData);
  }

  // ------------------------------------------------------------------------------------- //
  // Controle de subida do SetPoint
  
  if(statusFase == RAMPA)
    setpointRiseControl();

  // ------------------------------------------------------------------------------------- //
  // Se a temperatura já foi atingida, controla a histerese para manter a temperatura
  
  else if(statusFase == PATAMAR)
    histereseControl();
  
  // ------------------------------------------------------------------------------------- //
  // Avança o patamar caso tenha finalizado o tempo

  if (tempoInicial > 0 && tempoRestante <= 0) 
  {
    // Avança o patamar
    advancePatamar();
    // Retorna
    return;
  }

  // ------------------------------------------------------------------------------------- //
  // Mensagens de tela

  if(_flagAuxBrassagem)
  {
    _flagAuxBrassagem = false;

    char tempoAtual[7];
    
    if((tempoRestante/60) > 100)
      sprintf(tempoAtual, "%03u:%02u", tempoRestante / 60, tempoRestante % 60);
    else
      sprintf(tempoAtual, "%02u:%02u", tempoRestante / 60, tempoRestante % 60);
    
    // Carrega as mensagens
    String auxString[2]; auxString[0].reserve(16); auxString[1].reserve(16);

    // Temperatura atual na tela - default: máximo do patamar
    float actualTempShow = patamar[numPatamar].temperaturaMax;
    // Se estiver em RAMPA, mostra a temperatura do patamar subindo
    if(statusFase == RAMPA) 
      actualTempShow = actualTempSetPoint;

    // Carrega a mensagem
    // auxString[0] = String(getStatusFase() + " - " + String(numPatamar + 1) + "/" + String(qtdePatamar));
    // auxString[0] = String(getStatusFase() + ":" + formatNumber(numPatamar + 1) + "/" + formatNumber(qtdePatamar) + 
    //                       "  " + String((float)actualTempShow, 1) + "C");
    
    auxString[0] = formatDataScreen(String(getStatusFase() + ":" + 
                                    formatNumber(numPatamar + 1) + 
                                    String(F("/")) + formatNumber(qtdePatamar) + 
                                    String(F("X")) + String((float)actualTempShow, 1) +
                                    String(F("C"))));
    
    auxString[0] = formatDataScreen(auxString[0]);

    if (String(tempoAtual) == F("00:00"))
    {
      if((tempoRestante/60) > 100)
        sprintf(tempoAtual, "%03u:%02u", patamar[numPatamar].tempo, 0);
      else
        sprintf(tempoAtual, "%02u:%02u", patamar[numPatamar].tempo, 0);
    }

    // Carrega a mensagem
    // auxString[1] = String(String((float)(mediaTemperature), 1) + "/" + String((float)actualTempShow, 1) + "C " + tempoAtual);
    // auxString[1] = String(tempoAtual) + String(F("    ")) + String((float)(mediaTemperature), 1) + String("C");
    auxString[1] = formatDataScreen(String(tempoAtual) + String(F("X")) +  
                                    String((float)(mediaTemperature), 1) + 
                                    String("C"));

    // Apresenta as mensagens na tela
    centralWritingScreen(auxString[0], auxString[1]);
  }
}

//WWWWWWWWWW*********************************************************************************
// Function: setpointRiseControl
// Date    : 30/05/2020 - 19:00
//WWWWWWWWWW*********************************************************************************

void setpointRiseControl(void )
{
  // Se a temperatura do SetPoint atingiu a temperatura prograda
  if(actualTempSetPoint >= patamar[numPatamar].temperaturaMax)
  {
    // Reseta o temporizador se subida do SetPoint
    wtdRiseSetPoint = 0;
    // Fixa a temperatura do SetPoint na temperatura programada
    actualTempSetPoint = patamar[numPatamar].temperaturaMax;

    // Se a temperatura atual atingiu a temperatura programada
    if(mediaTemperature >= actualTempSetPoint)
    {
      // Aviso sonoro
      bipOK();
      // Inicializa a contagem do tempo
      _contaTempo = true;
      // Altera o status da fase
      statusFase = PATAMAR;
      // Desaciona o relé
      digitalWrite(PINO_RELAY1, LOW);
      // Retorna
      return;
    }
  }

  // Se o SetPoint está '0.3°C' mais que o sensor
  else if(actualTempSetPoint > (mediaTemperature + 0.3))
  {
    // Se atingiu a temperatura configura
    if(actualTempSetPoint == patamar[numPatamar].temperaturaMax)
    {
      // Reseta o temporizador se subida do SetPoint
      wtdRiseSetPoint = 0;
      // Altera o status da fase
      statusFase = PATAMAR;
      // Desaciona o relé
      digitalWrite(PINO_RELAY1, LOW);
      // Desliga o LED de aquecimento
      digitalWrite(LED_AMARELO, LOW);
      // Retorna
      return;
    }
    // Se ainda não atingiu, faz o controle
    else
    {
      // Aciona o relé para aquecer até atingir a diferença
      digitalWrite(PINO_RELAY1, HIGH);
      // Liga o LED de aquecimento
      digitalWrite(LED_AMARELO, HIGH);
      // Retorna
      return;
    }
  }

  // Se a temperatura do sensor já atingiu a temperatura do SetPoint
  else if(mediaTemperature >= actualTempSetPoint)
  {
    // Desaciona o relé para aguardar a diferença de temperatura
    digitalWrite(PINO_RELAY1, LOW);
    // Desliga o LED de aquecimento
    digitalWrite(LED_AMARELO, LOW);
    // Retorna
    return;
  }
}

//WWWWWWWWWW*********************************************************************************
// Function: loadControlSetPoint
// Date    : 30/05/2020 - 19:10
//WWWWWWWWWW*********************************************************************************

void loadControlSetPoint(void )
{
  // ------------------------------------------------------------------ //
  // Controle do SetPoint e temperatura
      
  // Status -> RAMPA
  statusFase = RAMPA;
  // Carrega a temperatura atual
  actualTempSetPoint = mediaTemperature;
  // Carrega 1 minuto para controle do SetPoint
  wtdRiseSetPoint = TIME_SET_POINT_RISE;
  // Desaciona o relé para aguardar a diferença de temperatura
  digitalWrite(PINO_RELAY1, LOW);

  // ------------------------------------------------------------------ //
}


//WWWWWWWWWW*********************************************************************************
// Function: histereseControl
// Date    : 31/05/2020 - 12:30
//WWWWWWWWWW*********************************************************************************

void histereseControl(void )
{  
  // Se a temperatura atual for cair '0.3' da temperatura do SetPoint
  if(mediaTemperature < patamar[numPatamar].temperaturaMax - CONV_HIST_FT_FLOAT(configGeral.histereseFactor))
  {
    // Aciona o relé para aquecimento
    digitalWrite(PINO_RELAY1, HIGH);
    // Liga o LED de aquecimento
    digitalWrite(LED_AMARELO, HIGH);
    // Liga o LED de temperatura
    digitalWrite(LED_VERMELHO1, HIGH);
  }
  
  else // Se não
  {
    // Se o relé estiver acionado
    if(digitalRead(PINO_RELAY1))
    {
      // Desaciona
      digitalWrite(PINO_RELAY1, LOW);
      // Desliga o LED de aquecimento
      digitalWrite(LED_AMARELO, LOW);
      // Desliga o LED de temperatura
      digitalWrite(LED_VERMELHO1, LOW);
    }
  } 

  // Se a temperatura atual for maior ou igual à temperatura do patamar somada
  // ao fator de histerese, desaciona o relé para liberar o aquecimento
  // if(mediaTemperature >= patamar[numPatamar].temperaturaMax + CONV_HIST_FT_FLOAT(configGeral.histereseFactor))
    // digitalWrite(PINO_RELAY1, LOW);
}

//WWWWWWWWWW*********************************************************************************
// Function: startProduction
// Date    : 30/05/2020 - 12:30
//WWWWWWWWWW*********************************************************************************

void resetSetPointParameters(void )
{
  // Desaciona o relé
  digitalWrite(PINO_RELAY1, LOW);
  // Reseta o temporizador
  wtdRiseSetPoint = 0;
  // Reseta a temperatura
  actualTempSetPoint = 0;
}

//WWWWWWWWWW*********************************************************************************
// Function: startProduction
// Date    : 30/05/2020 - 12:30
//WWWWWWWWWW*********************************************************************************

void startProduction(void )
{
  // --------------------------------------------------------- //
  // Inicia produção
  
  passoMaquina = mEXECUTA_PRODUCAO; return;

  // --------------------------------------------------------- //

}

//WWWWWWWWWW*********************************************************************************
// Function: checkESC
// Date    : 19/11/2019 - 11:04
//WWWWWWWWWW*********************************************************************************

boolean checkESC(void )
{
  // Verifica se é saída do processo de brassagem
  if (_KEY_ESC) 
  {
    auxKeyEsc = true;
    return(true);
  }
  return(false);
}

//WWWWWWWWWW*********************************************************************************
// Function: loopBrassagem
// Date    : 19/11/2019 - 11:04
//WWWWWWWWWW*********************************************************************************

void loopBrassagem(void )
{
  if (_keyPress) 
  {
    if (_KEY_RIGHT) 
    {
      // Avança o patamar
      advancePatamar();
    }
    else if (_KEY_LEFT) 
    {
      // Volta um patamar
      numPatamar--;
      // Reseta o patamar
      // patamar[numPatamar].concluido = false;
      // Reseta das dos PWM
      _pwmFull = false; _loadTimerPwm = false;
      // Reseta a temporização
      _contaTempo = false; tempoInicial = 0; tempoProximo = 0;
        
      // Reseta o envio de dados
      if(_connectedAPP)
      {
        _sendDataApp = false;
        wdtDadosBrassagem = configGeral.tempoEnvioBrassagem;
      }
        
      // Se é a primeira receita
      if (numPatamar == 0) 
      {
        // --------------------------------------------------------- //
        // Reseta os patamares

        qtdePatamar = receitaSel.qtdePatamar;
        for (register uint8_t ind = 0; ind < qtdePatamar; ind++) 
        {
          patamar[ind] = receitaSel.patamar[ind];
          // patamar[ind].concluido = false;
        }
        
        // salvaReceita(receitaSel);

        // --------------------------------------------------------- //
        // Reinicia a produção

        loadControlSetPoint();
        // executaProducao();

        // --------------------------------------------------------- //
        // Aviso sonoro e mensagem

        bipOK(); 
        printDisplay(F("   Inicio do   "), "  SetPoint: " + 
                     formatNumber(numPatamar + 1), TEMPO_ALARME);

        // --------------------------------------------------------- //

      }
      // Se for a última receita, finaliza a produção
      else if (numPatamar == 255) 
        fimProducao(false);
      // Carrega o controle do SetPoint
      else
      {
        // --------------------------------------------------------- //
        // Reinicia a produção

        loadControlSetPoint();
        // executaProducao();

        // --------------------------------------------------------- //
        // Aviso sonoro e mensagem

        bipOK(); 
        printDisplay(F("   Inicio do   "), "  SetPoint: " + 
                     formatNumber(numPatamar + 1), TEMPO_ALARME);

        // --------------------------------------------------------- //
      }

    }
    // Altera o valor do tempo
    else if ((_KEY_UP || _KEY_DOWN) && contandoTempo()) 
    {
      // Entrada de tempo
      entradaTempoManual(F("Tempo"), valorTempoManual, -500, 500);
      // Atualiza o valor do tempo
      valorTempoManual = valorDigitado;
      // Retorna
      return;
    }
    // Verifica se é saída do processo de brassagem
    else if (checkESC()) 
      fimProducao(false);
  }

  // ----------------------------------// 
  // Loop de produção

  loopProducao();

  // ----------------------------------// 
}

//WWWWWWWWWW*********************************************************************************
// Function: executaProducao
// Date    : 04/04/2017 - 13:45
//WWWWWWWWWW*********************************************************************************

void executaProducao(void ) 
{
  // --------------------------------------------------------- //
  // Aviso sonoro

  bipAcerto();
  // --------------------------------------------------------- //
  // Resets
  
  _temperaturaMaxEntrou = false;

  // Reseta a temporização
  _contaTempo = false; tempoInicial = 0; tempoProximo = 0;

  // --------------------------------------------------------- //
  // Reseta os patamares

  // Reseta o número do patar
  numPatamar = 0;
  // Atualiza a quantidade de patamares
  qtdePatamar = receitaSel.qtdePatamar;
  for (register uint8_t ind = 0; ind < qtdePatamar; ind++) 
  {
    patamar[ind] = receitaSel.patamar[ind];
    // patamar[ind].concluido = false;
  }
    
  // Armazena o dado
  // salvaReceita(receitaSel);

  // --------------------------------------------------------- //

  // for (ind = 0; ind < qtdePatamar; ind++) 
  // {
  //   patamar[ind] = receitaSel.patamar[ind]; 
  //   if (patamar[ind].concluido == true) 
  //     numPatamar = ind + 1;
  // }

  // --------------------------------------------------------- //
  // Reseta envio de dados

  if(_connectedAPP)
  {
    _sendDataApp = false;
    wdtDadosBrassagem = configGeral.tempoEnvioBrassagem;
  }

  // --------------------------------------------------------- //
  // Carrega o controle do SetPoint
  
  loadControlSetPoint();

  // --------------------------------------------------------- //
  // Executa brassagem

  passoMaquina = mLOOP_BRASSAGEM;

  // --------------------------------------------------------- //
}


//WWWWWWWWWW*********************************************************************************
// Function: fimProducao
// Date    : 20/02/2020 -   10:30
//WWWWWWWWWW*********************************************************************************

void advancePatamar(void )
{
  // Apaga os LEDs
  desligaLeds();
  // Reseta flag
  _temperaturaMaxEntrou = false;
  // Salva patamar como concluido
  // receitaSel.patamar[numPatamar].concluido = true; 
  // Reseta temporização
  _contaTempo = false; tempoInicial = 0; tempoProximo = 0; valorTempoManual = 0;

  // Salva receita
  // salvaReceita(receitaSel);
  
  // Incrementa o patamar
  numPatamar++;

  // Verifica se finalizou a produção
  if (numPatamar >= qtdePatamar) 
    fimProducao(true);
  else 
  {
    // Reseta aquecimento
    _pwmFull = false; _loadTimerPwm = false;
    // Carrega o controle do SetPoint
    loadControlSetPoint();

    // Aviso sonoro e mensagem
    bipOK(); 
    printDisplay(F("   Inicio do   "), "  SetPoint: " + formatNumber(numPatamar + 1), TEMPO_ALARME);
  }

  // Retorna
  return;
}

//WWWWWWWWWW*********************************************************************************
// Function: fimProducao
// Date    : 20/02/2020 -   10:30
//WWWWWWWWWW*********************************************************************************

void fimProducao(boolean auxMoment)
{
  // Carrega a quantidade de patamares 
  qtdePatamar = receitaSel.qtdePatamar;
  // //Reseta todos os Patamares
  // for (register uint8_t ind = 0; ind < qtdePatamar; ind++) 
  //   receitaSel.patamar[ind].concluido = false;
    
  // Salva a receita e dá aviso sonoro
  bipOK(); // salvaReceita(receitaSel); 
  
  if(auxMoment)
    printDisplay(F("Fim do PROCESSO"), F("  Com Sucesso!  "), TEMPO_ALARME);
  else
    printDisplay(F(" Etapa Abortada "), F("  Saida Remota  "), TEMPO_ALARME);

  // Apaga os LEDs
  desligaLeds();
  
  // Reseta temporização
  _contaTempo = false; tempoInicial = 0; tempoProximo = 0; valorTempoManual = 0;

  // Desliga tempo
  desligaAquecimento();

  // Reseta os processos do SetPoint
  resetSetPointParameters();

  // Reseta o envio da dados
  if(_connectedAPP)
  {
    _sendDataApp = false;
    wdtDadosBrassagem = 0;
  }

  // Altera o passo da máquina 
  passoMaquina = mSTANDBY;
}

//WWWWWWWWWW*********************************************************************************
// Function: disparaAquecimento
// Date    : 04/04/2017 - 13:45
//WWWWWWWWWW*********************************************************************************

void disparaAquecimento(int prTemperatura) 
{
  // Usa biblioteca - TIMER3
  Timer3.pwm(PINO_SSR, getValorPwm(), TIMER3); 
  calculaSpeed(prTemperatura);
}

//WWWWWWWWWW*********************************************************************************
// Function: desligaAquecimento
// Date    : 04/04/2017 - 13:45
//WWWWWWWWWW*********************************************************************************

void desligaAquecimento(void ) 
{
  // Desligar
  digitalWrite(PINO_SSR, LOW); 
}

//WWWWWWWWWW*********************************************************************************
// Function: calculaGrauPorMinuto
// Date    : 04/04/2017 - 13:45
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
// Function: calculaSpeed
// Date    : 04/04/2017 - 13:45
//WWWWWWWWWW*********************************************************************************

void calculaSpeed(int prTemperatura) 
{
  if (tempInicialAquecimento > - 1000 )
    speedAquecimento = (prTemperatura - tempInicialAquecimento);
  else
    speedAquecimento = 0;
}

//WWWWWWWWWW*********************************************************************************
// Funcão: getStatusFase
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

String getStatusFase(void ) 
{
  if (statusFase == RAMPA)
    return F("RP"); // "RAMPA");
  else if (statusFase == PATAMAR)
    return F("SP"); // "SETPOINT");
  else
    return F("SS"); // "SEM SENSOR");
}

//WWWWWWWWWW*********************************************************************************
// Function: setStatusFase
// Date    : 04/04/2017 - 13:45
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
      wdtDadosBrassagem = configGeral.tempoEnvioBrassagem;    
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
      bipAlarme();
      printDisplay(F("Temperatura"), String((float)(patamar[numPatamar].temperaturaMax), 2) + "C Atingida", TEMPO_ALARME);
      if (numPatamar == 0) 
      {
        _contaTempo = false;
        disparaAquecimento(prTemperatura);
        passoMaquina = mLOOP_BRASSAGEM;
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
      wdtDadosBrassagem = configGeral.tempoEnvioBrassagem;     
    }
    _contaTempo = true;
    valorPwm = 50;
    statusFase = PATAMAR;
    if (difTemp >= 3.0)
      valorPwm = MAX_PWM;
    disparaAquecimento(prTemperatura);
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