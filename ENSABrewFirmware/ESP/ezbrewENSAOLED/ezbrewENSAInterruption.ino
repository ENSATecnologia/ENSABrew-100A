//*******************************************************************************************
// Arquivo C/C++ do Projeto Controlador de Patamares -- EZBrewControll
//-------------------------------------------------------------------------------------------
// Arquivo  : ezbrewENSAInterruption
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
// Funcao   : globalInterruption - Interruption TIMER1 - 1ms
// Data     : 09/09/2016 09:55
// Revisão  :
//WWWWWWWWWW*********************************************************************************

void globalInterruption() 
{
  _atendeMaquina = true;
  _atendeDisplay = true;
  _atendeTeclado = true;
  _atendeSocket = true;

  wdt50ms++;
  if (wdt50ms >= 50) 
  {
    wdt50ms = 0;
    _flag50ms = true;
    freqAC = pulseAC;
    if (freqAC <= 2) {
      _faltouAC = true;
    }
    pulseAC = 0;
  }

  wdtTimesTemperatura++;
  if (wdtTimesTemperatura >= 100) {
    wdtTimesTemperatura = 0;
    _readTemperature = true;
  }

  wdt1000ms++;
  if (wdt1000ms >= 1000) 
  {
    wdt1000ms = 0;
    _flag1000ms = true;
    
    if (_contaTempo) 
    {
      tempoInicial++; // Quando "false" paraliza o "tempoDecorrido" - PATAMAR
      tempoProximo++;

      wdtDadosBrassagem++;
      if(wdtDadosBrassagem >= configGeral.tempoEnvioBrassagem)
      {
        wdtDadosBrassagem = 0;
        _printDebug = true;
      }
    }
  }

  wdtTimesReceita++;
  if (wdtTimesReceita >= 1000) {
    wdtTimesReceita = 0;
    _changeMsg = !_changeMsg;
    if (_changeMsg) _msgDisplay1 = true;
    else _msgDisplay2 = true;
  }

  wdt3000ms++;
  if (wdt3000ms >= 3000) 
  {
    wdt3000ms = 0;

    _flag3000ms = !_flag3000ms;
    if (_flag3000ms) _msgConvite = true;
    else _msgEzbrew = true;

    _refreshDisplay = true;
  }

  if (passoMaquina == mLOOP_BRASSAGEM ||
      passoMaquina == mLOOP_FERVURA) {
    int difPwm = MAX_PWM - valorPwm;
    if (difPwm >= MAX_PWM)
      digitalWrite(LED_AMARELO, LOW);
    else {
      wdtTimesBlink++;
      if (wdtTimesBlink >= difPwm * 10) {
        wdtTimesBlink = 0;
        digitalWrite(LED_AMARELO, HIGH);
      }
      else digitalWrite(LED_AMARELO, LOW);
    }
  }
  
  wdtTimesDisplay--;
  if (wdtTimesDisplay <= 0)
    wdtTimesDisplay = 0;
  
  if (_loadTimerPwm) {
    wdtPwmFull++;
    if (wdtPwmFull >= TEMPO_PWM) {
      wdtPwmFull = 0;
      _pwmFull = true;
    }
  }


}

//WWWWWWWWWW*********************************************************************************
// Funcao   : disableInterrupts
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void disableInterrupts()
{
  TimerGlobal.detach();
  TimerBuzzer.detach();
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : enableInterrupts
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void enableInterrupts() 
{
  startTimers();
}