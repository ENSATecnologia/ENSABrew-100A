//*******************************************************************************************
// Arquivo C/C++ do Projeto Controlador de Patamares -- EZBrewControll
//-------------------------------------------------------------------------------------------
// Arquivo  : ezbrewENSAProducao.ino
// Inicio   : 29/04/2013 10:23
// Revisão  : 11/05/2016 18:31
//
// AutorI   : Cláudio B. Pimenta, A.S
// AutorII  : Jânio Anselmo, Eng. Me.
// AutorIII : Thiago Anselmo, Tecgº
// AutorIIII : Gabriel da Silva Caetano, Estágio.
//
// CPU:     : ESP8266 - WEMOS D1 MINI Pro
// Clock:   : 16MHz
//*******************************************************************************************


//WWWWWWWWWW*********************************************************************************
// Funcao   : executaProducao
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void executaProducao() {
  _temperaturaMaxEntrou = false;
  numPatamar = 0;
  tempoInicial = 0;
  tempoProximo = 0;
  _contaTempo = false;
  qtdePatamar = receitaSel.qtdePatamar;
  bipAcerto();

  for (register int ind = 0; ind < qtdePatamar; ind++) {
    patamar[ind] = receitaSel.patamar[ind];
    if (receitaSel.statusBrassagem == RESET) //Parada
      patamar[ind].concluido = false;
    else if (receitaSel.statusBrassagem == 1) { //Em produção
      if (patamar[ind].concluido == true) {
        numPatamar = ind + 1;
      }
    }
  }
  if (receitaSel.statusBrassagem < 2) {
    receitaSel.statusBrassagem = 1; //Em producao
    salvaReceita(receitaSel);
    passoMaquina = mLOOP_BRASSAGEM;
    return;
  }
  else if (receitaSel.statusBrassagem == 2) { //Fervura
    desligaAquecimento();
    passoMaquina = mMENU_FERVURA;
    return;
  }
  else if (receitaSel.statusBrassagem == 3) { //Whirlpool
    desligaAquecimento();
    passoMaquina = mMENU_WHIRLPOOL;
    return;
  }
  else if (receitaSel.statusBrassagem == 4) { //Descanso
    desligaAquecimento();
    passoMaquina = mMENU_DESCANSO;
    return;
  }
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : executaFervura
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void executaFervura() {
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
  valorPwmManual = MAX_PWM;
  qtdeAdicao = receitaSel.qtdeAdicao;

  for (register int ind = 0; ind < qtdeAdicao; ind++)
    adicaoFervura[ind] = receitaSel.adicao[ind];
  receitaSel.statusBrassagem = 2; //Fervura
  salvaReceita(receitaSel);
  passoMaquina = mLOOP_FERVURA;
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : executaWhirlpool
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void executaWhirlpool() {
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
  receitaSel.statusBrassagem = 3; //Whirlpool
  salvaReceita(receitaSel);
  passoMaquina = mLOOP_WHIRLPOOL;
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : executaDescanso
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void executaDescanso() {
  desligaLeds();
  _temperaturaMaxEntrou = false;
  tempoInicial = 0;
  tempoProximo = 0;
  numPatamar = 0;
  patamar[0].tempo = receitaSel.qtdeDescanso;
  qtdePatamar = 1;
  numDescanso = 0;
  valorPwmManual = MAX_PWM;
  qtdeDescanso = receitaSel.qtdeDescanso;

  for (register int ind = 0; ind < qtdeDescanso; ind++)
    descansoFervura[ind] = receitaSel.descanso[ind];
  receitaSel.statusBrassagem = 4; //Descanso
  salvaReceita(receitaSel);
  passoMaquina = mLOOP_DESCANSO;
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : processaFervura
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void processaFervura(String prTempoAtual) { // "prTempoAtual" - Esse tempo é decrescente!
  if (numAdicao < qtdeAdicao && contandoTempo()) {
    char stFervChar[17]; // (adicaoFervura[numAdicao].tempo) + ":00";
    String stFerv, stPreFerv;
    if (numAdicao == 0 && adicaoFervura[numAdicao].tempo == receitaSel.qtdeMinFervura)
      stFerv = prTempoAtual;
    else {
      sprintf(stFervChar, "%02u:%02u", adicaoFervura[numAdicao].tempo, 0);
      stFerv = stFervChar;
      sprintf(stFervChar, "%02u:%02u", adicaoFervura[numAdicao].tempo + 1, 0); // +1 decrescente
      stPreFerv = stFervChar;
    }
    if (prTempoAtual == stPreFerv) {
      bipAtencao();
      printDisplay(F(" Para ADICAO: "), F("   +1 Minuto   "), TEMPO_ALARME);
    }
    if (prTempoAtual == stFerv) {
      disparaAquecimento(mediaTemperature);
      tempoProximo = 0;
      passoMaquina = mADICAO_FERVURA;
      return;
    }
  }
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : processaWhirlpool
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void processaWhirlpool(String prTempoAtual) { // "prTempoAtual" - Esse tempo é crescente!
  if (numWhirlpool < qtdeWhirlpool && contandoTempo()) {
    char stWhirChar[17]; // (whirlpoolFervura[numAdicao].tempo) + ":00";
    String stWhir, stPreWhir;
    if (numWhirlpool == 0)
      stWhir = prTempoAtual;
    else {
      sprintf(stWhirChar, "%02u:%02u", whirlpoolFervura[numWhirlpool - 1].tempo, 0);
      stWhir = stWhirChar;
      sprintf(stWhirChar, "%02u:%02u", whirlpoolFervura[numWhirlpool - 1].tempo - 1, 0); // -1 crescente
      stPreWhir = stWhirChar;
    }
    if (prTempoAtual == stPreWhir) {
      bipAtencao();
      printDisplay(F("Para WHIRLPOOL:"), F("   +1 Minuto   "), TEMPO_ALARME);
    }
    if (prTempoAtual == stWhir) {
      tempoProximo = 0;
      bipAtencao();
      printDisplay(F("Start WHIRLPOOL:"), String(whirlpoolFervura[numWhirlpool].nome), TEMPO_ALARME);
      numWhirlpool = numWhirlpool + 1;
      desligaLeds();
    }
  }
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : processaDescanso
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void processaDescanso(String prTempoAtual) { // "prTempoAtual" - Esse tempo é crescente!
  if (numDescanso < qtdeDescanso && contandoTempo()) {
    char stDescChar[17]; // (DescansoFervura[numAdicao].tempo) + ":00";
    String stDesc, stPreDesc;
    if (numDescanso == 0)
      stDesc = prTempoAtual;
    else {
      sprintf(stDescChar, "%02u:%02u", descansoFervura[numDescanso - 1].tempo, 0);
      stDesc = stDescChar;
      sprintf(stDescChar, "%02u:%02u", descansoFervura[numDescanso - 1].tempo - 1, 0); // -1 crescente
      stPreDesc = stDescChar;
    }
    if (prTempoAtual == stPreDesc) {
      bipAtencao();
      printDisplay(F("Para DESCANSO:"), F("   +1 Minuto   "), TEMPO_ALARME);
    }
    if (prTempoAtual == stDesc) {
      tempoProximo = 0;
      bipAtencao();
      printDisplay(F("Start DESCANSO:"), String(descansoFervura[numDescanso].nome), TEMPO_ALARME);
      numDescanso = numDescanso + 1;
      desligaLeds();
    }
  }
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : executaBrassagemRemota
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void executaBrassagemRemota(int type, byte cmd, JsonObject dataObject) {
  JsonArray nomeReceitaArray = dataObject[F("receita")];
  int idxReceita = nomeReceitaArray[RESET].as<int>();

  if (qtdeReceita <= 0) {
    passoMaquina = mSTANDBY;
    code = F("E11"); // Empty Memory
  }
  else if(idxReceita > qtdeReceita)
  {
    passoMaquina = mSTANDBY;
    code = F("E12"); // Empty Memory
  }
  else {
    if (idxReceita <= 1)
      idxReceita = 1;
    receitaSel = regReceita[idxReceita - 1];
    receitaSel.statusBrassagem = RESET;
    passoMaquina = mEXECUTA_PRODUCAO;
    code = F("A1"); // Success!
  }
  setJsonData(type, cmd);
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : disparaAquecimento
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void disparaAquecimento(int prTemperatura) 
{
  analogWriteFreq(FREQ_PWM); // Definindo frequência PWM
  analogWrite(PINO_SSR, getValorPwm());
  calculaSpeed(prTemperatura);
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : desligaAquecimento
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void desligaAquecimento() {
  analogWrite(PINO_SSR, 1023); //Desligar
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : setStatusFase
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void setStatusFase(float prTemperatura) {
  float difTemp = patamar[numPatamar].temperaturaMax - prTemperatura; //Desaceleração do setpoint
  float tempMax = patamar[numPatamar].temperaturaMax;
  if (prTemperatura < tempMax && passoMaquina == mLOOP_BRASSAGEM && !contandoTempo()) {
    desligaLeds();
    statusFase = RAMPA;
    if (numPatamar == 0)
      valorPwm = MAX_PWM;
    else {
      if (difTemp >= 0.0 && difTemp <= 3.0) {
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
  else if (prTemperatura >= tempMax && passoMaquina == mLOOP_BRASSAGEM) {
    piscaLedsAlarme();
    
    if(!_contaTempo)
    {
      wdtDadosBrassagem = 0;
      _printDebug = true;
    }
    
    _contaTempo = true;
    _loadTimerPwm = false;
    _pwmFull = false;
    valorPwm = RESET;
    statusFase = PATAMAR;
    desligaAquecimento();
    if (!_temperaturaMaxEntrou) {
      _temperaturaMaxEntrou = true;
      bipAlarme();
      printDisplay(F("Temperatura"), String((float)(patamar[numPatamar].temperaturaMax), 2) + "C Atingida", TEMPO_ALARME);
      if (numPatamar == 0 && !_derrubaMalte) {
        _contaTempo = false;
        disparaAquecimento(prTemperatura);
        passoMaquina = mDERRUBAR_MALTE;
        return;
      }
    }
  }
  else if (prTemperatura < tempMax && passoMaquina == mLOOP_BRASSAGEM && tempoInicial > 0) {
    desligaLeds();
    
    if(!_contaTempo)
    {
      wdtDadosBrassagem = 0;
      _printDebug = true;
    }
    
    _contaTempo = true;
    valorPwm = 50;
    statusFase = PATAMAR;
    if (difTemp >= 3.0)
      valorPwm = MAX_PWM;
    disparaAquecimento(prTemperatura);
  }
  else if (prTemperatura < tempMax && passoMaquina == mLOOP_FERVURA && tempoInicial >= 0) {
    desligaLeds();
    valorPwm = valorPwmManual;
    statusFase = FERVURA;
    disparaAquecimento(prTemperatura);
  }
  else if (prTemperatura >= tempMax && passoMaquina == mLOOP_FERVURA) {
    piscaLedsAlarme();
    
    if(!_contaTempo)
    {
      wdtDadosBrassagem = 0;
      _printDebug = true;
    }
    
    _contaTempo = true;
    valorPwm = valorPwmManual;
    statusFase = FERVURA;
    disparaAquecimento(prTemperatura);
    if (!_temperaturaMaxEntrou) {
      _temperaturaMaxEntrou = true;
      bipAlarme();
    }
  }
  else if (prTemperatura >= RESET && passoMaquina == mLOOP_WHIRLPOOL) {
    desligaLeds();
    
    if(!_contaTempo)
    {
      wdtDadosBrassagem = 0;
      _printDebug = true;
    }
    
    _contaTempo = true;
    valorPwm = RESET;
    statusFase = WHIRLPOOL;
    desligaAquecimento();
  }
  else if (prTemperatura >= RESET && passoMaquina == mLOOP_DESCANSO) {
    desligaLeds();
    
    if(!_contaTempo)
    {
      wdtDadosBrassagem = 0;
      _printDebug = true;
    }
    
    _contaTempo = true;
    valorPwm = RESET;
    statusFase = DESCANSO;
    desligaAquecimento();
  }
  else {
    desligaLeds();
    _contaTempo = false;
    valorPwm = RESET;
    statusFase = FINAL; //SEM SENSOR
    desligaAquecimento();
  }
}

