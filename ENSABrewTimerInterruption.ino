//*******************************************************************************************
// Arquivo C/C++ do Projeto Controlador de Patamares -- ENSABrewControll
//-------------------------------------------------------------------------------------------
//
// Arquivo: ENSABrewTimerInterruption.ino
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
// Função: defineTimers
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void defineTimers(void ) 
{
  
  Timer1.initialize(TIMER1);
  Timer1.attachInterrupt(globalInterruption);

  Timer3.initialize(TIMER3);

  Timer4.initialize(TIMER4);
  Timer4.attachInterrupt(buzzerInterruption);

  attachInterrupt(digitalPinToInterrupt(PINO_AC), acInterruption, RISING);
  attachInterrupt(digitalPinToInterrupt(PINO_FLUXO), callPulse, RISING);

}

//WWWWWWWWWW*********************************************************************************
// Função: startTimers
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void startTimers(void ) 
{

  Timer1.start();
  Timer3.start();
  Timer4.start();
}

//WWWWWWWWWW*********************************************************************************
// Função: globalInterruption - Interruption TIMER1 - 1ms
// Data  : 09/09/2016 09:55
// Revisão  :
//WWWWWWWWWW*********************************************************************************

void globalInterruption(void ) 
{
  // Flag para temporização de 1 segundos
  boolean _auxFlag1000ms = false;
    
  // Temporizador para 1 minuto
  static uint8_t auxWdt1min = 60;
  // Flag para temporização de 1 minuto
  boolean _auxFlag1min = false;

  _atendeMaquina = true;
  _atendeDisplay = true;
  _atendeTeclado = true;

  //------------------------------------------------------
  // Temporizadores principais - base de contagem
  
  wdt50ms--;
  if (wdt50ms == 0) 
  {
    _flag50ms = true;
    wdt50ms = TIMER_50ms;
    
    // Se o envio e leitura do AC estiver desabilitado
    if(configGeral.timeSendReadAC != 0) 
    {
      if (pulseAC <= 2)
        _faltouAC = true;
      pulseAC = 0;
    }
    
    wdtTimesTemperatura--;
    if (wdtTimesTemperatura == 0) 
    {
      _readTemperature = true;
      wdtTimesTemperatura = TIME_READ_TEMPERATURE;
    }
  }

  if(_flag50ms) 
  {
    _flag50ms = false;

    if(_flagWdtIncrement) 
    {
      if(wdtIncrement != 0)
      {
        wdtIncrement--;
        if(wdtIncrement == 0) 
        {
          _flagCheckIncrement = true;
          wdtIncrement = timeWdtIncrement;
        }
      }
    }

    wdt1000ms--;
    if (wdt1000ms == 0) // (20 * 50ms)
    {
      _flag1000ms = true;
      _auxFlag1000ms = true;
      wdt1000ms = TIMER_1000ms;
    }

    // Alterna mensagens na tela
    wdtChangeMsg--;
    if(wdtChangeMsg == 0) 
    {
      _changeMsg = !_changeMsg;
      wdtChangeMsg = TIME_CHANGE_MSG;
    }
  }
  
  // Temporização de 1 segundo
  if(_auxFlag1000ms) 
  {
    // Temporização de refresh do display OLED
    #if(DISPLAY_TYPE == DISPLAY_OLED)
    {
      wdtRefreshDisplay--;
      if(wdtRefreshDisplay == 0)
      {
        _refreshDisplay = true;
        wdtRefreshDisplay = TIME_REFRESH_DISPLAY;
      }
    }
    #endif
    
    // Se o envio e leitura do AC estiver desabilitado
    if(configGeral.timeSendReadAC != 0) 
    {
      if(_faltouAC) 
      {
        wdtSendReadAC--;
        if(wdtSendReadAC == 0) 
        {
          _faltouAC = false;
          _sendDataAC = true;
          wdtSendReadAC = configGeral.timeSendReadAC;
        }
      }
    }

    // Contador para 1 minutos
    auxWdt1min--;
    if(auxWdt1min == 0) 
    {
      auxWdt1min = 60;
      _auxFlag1min = true;
    }

    // Temporização de envio do keep alive
    if(wdtSendKeepAlive != 0)
    {
      wdtSendKeepAlive--;
      if(wdtSendKeepAlive == 0)
        _flagSendKeepAlive = true;
    }

    // Temporização de aguardo de resposta do keep alive
    if(wdtWaitRespKeepAlive != 0)
    {
      wdtWaitRespKeepAlive--;
      if(wdtWaitRespKeepAlive == 0)
        _connectedAPP = false;
    }

    // Temporização para zera fluxo com botão ESC
    if(passoMaquina == mCALCULA_FLUXO) 
    {
      if(wdtResetFluxo != 0 && _flagResetFluxo) 
      {
        wdtResetFluxo--;
        if(wdtResetFluxo == 0) 
        {
          flow = 0; liters = 0;
          milli = 0; Npulse = 0;
          _flagResetFluxo = false;
        }
      }
    }

    // Temporização de 3 segundos
    wdt3000ms--;
    if (wdt3000ms == 0) // (3 * 1000ms) 
    { 
      wdt3000ms = TIMER_3000ms;
      _flag3000ms = !_flag3000ms;

      _msgConvite = false;
      _msgENSABrew = false;
      _msgTemperatura = false;

      if(countChangeMsg == 0)
        _msgConvite = true;
      else if(countChangeMsg == 1)
        _msgENSABrew = true;
      else if(countChangeMsg == 2)
        _msgTemperatura = true;

      countChangeMsg++;
      if(countChangeMsg == 3)
        countChangeMsg = 0;
    }

    // Se o envio da temperatura estiver habilitado, temporiza
    if(configGeral.timeSendTemp != 0 && wdtSendTemp != 0)
      wdtSendTemp--;

  }
  
  // Fim dos Temporizadores principais - base de contagem
  //------------------------------------------------------
  // Temporizadores gerais - ações no dispositivo

  if(_auxFlag1000ms) 
  {
    if (_contaTempo) 
    {
      tempoInicial++; // Quando "false" paraliza o "tempoDecorrido" - PATAMAR
      tempoProximo++;

      wdtDadosBrassagem--;
      if(wdtDadosBrassagem == 0) 
      {
        _sendDataApp = true;
        wdtDadosBrassagem = configGeral.tempoEnvioBrassagem;
      }
    }
  }

  if (passoMaquina == mLOOP_BRASSAGEM || passoMaquina == mLOOP_FERVURA) 
  {
    int difPwm = MAX_PWM - valorPwm;
    if (difPwm >= MAX_PWM)
      digitalWrite(LED_AMARELO, LOW);
    else 
    {
      wdtTimesBlink++;
      if (wdtTimesBlink >= difPwm * 10) 
      {
        wdtTimesBlink = 0;
        digitalWrite(LED_AMARELO, HIGH);
      }
      else digitalWrite(LED_AMARELO, LOW);
    }
  }
  
  wdtTimesDisplay--;
  if (wdtTimesDisplay <= 0)
    wdtTimesDisplay = 0;

  if (_loadTimerPwm) 
  {
    wdtPwmFull++;
    if (wdtPwmFull >= TEMPO_PWM) 
    {
      wdtPwmFull = 0;
      _pwmFull = true;
    }
  }

  //--------------------------------------------
  // Temporizações acionamento dos solenóides

  // Habilita o timer de acordo com o tipo de temporização
  boolean _flagEnableTimer[2] = {false, false};

  // Solenóide 1
  if(wdtActionSolenoid[SOLENOID1] != 0) 
  {
    // Se for temporizador de milisegundos
    if(timerTypeSolenoid[SOLENOID1] == TIMER_IN_MILISECONDS)
      _flagEnableTimer[SOLENOID1] = true;

    // Se for temporizador de milisegundos
    else if(timerTypeSolenoid[SOLENOID1] == TIMER_IN_SECONDS) 
    {
      // Se já se passou 1 segundo, habilita a temporização
      if(_auxFlag1000ms)
        _flagEnableTimer[SOLENOID1] = true;
    }

    else if(timerTypeSolenoid[SOLENOID1] == TIMER_IN_MINUTES) 
    {
      // Se já se passou 1 minuto, habilita a temporização
      if(_auxFlag1min)
        _flagEnableTimer[SOLENOID1] = true;
    }

    // Se a temporização estiver habilitada
    if(_flagEnableTimer[SOLENOID1]) 
    {
      wdtActionSolenoid[SOLENOID1]--;
      if(wdtActionSolenoid[SOLENOID1] == 0)
        digitalWrite(PINO_SOLENOIDE1, LOW);
    }
  }

  // Solenóide 2
  if(wdtActionSolenoid[SOLENOID2] != 0) 
  {
    // Se for temporizador de milisegundos
    if(timerTypeSolenoid[SOLENOID2] == TIMER_IN_MILISECONDS)
      _flagEnableTimer[SOLENOID2] = true;
    // Se for temporizador de milisegundos
    else if(timerTypeSolenoid[SOLENOID2] == TIMER_IN_SECONDS) 
    {
      // Se já se passou 1 segundo, habilita a temporização
      if(_auxFlag1000ms)
        _flagEnableTimer[SOLENOID2] = true;
    }
    else if(timerTypeSolenoid[SOLENOID2] == TIMER_IN_MINUTES) 
    {
      // Se já se passou 1 minuto, habilita a temporização
      if(_auxFlag1min)
        _flagEnableTimer[SOLENOID2] = true;
    }
    // Se a temporização estiver habilitada
    if(_flagEnableTimer[SOLENOID2]) 
    {
      wdtActionSolenoid[SOLENOID2]--;
      if(wdtActionSolenoid[SOLENOID2] == 0)
        digitalWrite(PINO_SOLENOIDE2, LOW);
    }
  }
}

//WWWWWWWWWW*********************************************************************************
// Função: disableInterrupts
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void disableInterrupts(void ) 
{
  Timer1.detachInterrupt(); // Disable interrupts
  Timer3.detachInterrupt(); // Disable interrupts
  Timer4.detachInterrupt(); // Disable interrupts
}

//WWWWWWWWWW*********************************************************************************
// Função: enableInterrupts
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void enableInterrupts(void ) 
{
  Timer1.attachInterrupt(globalInterruption);
  // Timer1.start();
  Timer3.start();
  Timer4.attachInterrupt(buzzerInterruption);
  // Timer4.start();
}