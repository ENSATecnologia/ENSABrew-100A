//*******************************************************************************************
// Arquivo C/C++ do Projeto Controlador de Patamares -- EZBrewControll
//-------------------------------------------------------------------------------------------
//
// Arquivo  : ezbrewENSABeep.ino
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
// Funcao   : buzzerInterruption - Interruption TIMER4 - 10ms
// Data     : 09/09/2016 09:55
// Revisão  :
//WWWWWWWWWW*********************************************************************************

void buzzerInterruption() {
  machineBuzzer();
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : machineBuzzer
// Data     : 15/11/2007 09:55
//WWWWWWWWWW*********************************************************************************

void machineBuzzer() {
  switch (passoBuzzer) {

    case gmBUZZER_OFF:
      noTone(PINO_BUZZER);
      break;

    case gmBIP_GLOBAL: {
        timerBuzzer++;
        if (timerBuzzer >= periodoBuzzer) {
          timerBuzzer = 0;
          boolean _inverteBuzzer = digitalRead(PINO_BUZZER);
          if (_inverteBuzzer) noTone(PINO_BUZZER);
          else tone(PINO_BUZZER, F_TONE);
          if (nBips) {
            if (--nBips == 0)
              passoBuzzer = gmBUZZER_OFF;
          }
        }
      }
      break;

    case gmBIP_ALARME: {
        periodoBuzzer++;
        if (periodoBuzzer >= 10) {
          periodoBuzzer = 0;
          if (nBips) {
            if (--nBips == 0)
              passoBuzzer = gmBUZZER_OFF;
          }
        }
      }
      break;
  }
}

// ===============================================================================( * FIM * )
// ===============================================================================( * FIM * )
// ===============================================================================( * FIM * )
// ===============================================================================( * FIM * )
// ================================(   FUNCOES   )================================( * FIM * )
// ================================(     DO      )================================( * FIM * )
// ================================(   BUZZER    )================================( * FIM * )
// ===============================================================================( * FIM * )
// ===============================================================================( * FIM * )
// ===============================================================================( * FIM * )
// ===============================================================================( * FIM * )

#ifdef PROGMEM
//WWWWWWWWWW*********************************************************************************
// Funcao   : bipInicial
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void bipInicial() {
  passoBuzzer = gmBIP_GLOBAL;
  timerBuzzer = 0;
  nBips = 6;
  periodoBuzzer = 12;
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : bipShutdown
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void bipShutdown() {
  passoBuzzer = gmBIP_GLOBAL;
  timerBuzzer = 0;
  nBips = 1;
  periodoBuzzer = 1;
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : bipServerError
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void bipServerError() {
  passoBuzzer = gmBIP_GLOBAL;
  timerBuzzer = 0;
  nBips = 2;
  periodoBuzzer = 4;
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : bipTeclado
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void bipTeclado() {
  passoBuzzer = gmBIP_GLOBAL;
  timerBuzzer = 0;
  nBips = 1;
  periodoBuzzer = 3;
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : bipAlarme
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void bipAlarme() {
  passoBuzzer = gmBIP_ALARME;
  tone(PINO_BUZZER, F_TONE);
  timerBuzzer = 0;
  nBips = 5;
  periodoBuzzer = 0;
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : bipOK
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void bipOK() {
  passoBuzzer = gmBIP_GLOBAL;
  timerBuzzer = 0;
  nBips = 6;
  periodoBuzzer = 6;
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : bipAtencao
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void bipAtencao() {
  passoBuzzer = gmBIP_GLOBAL;
  timerBuzzer = 0;
  nBips = 10;
  periodoBuzzer = 8;
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : bipAcerto
// Data     : 09/04/2010 15:46
//WWWWWWWWWW*********************************************************************************

void bipAcerto() {
  passoBuzzer = gmBIP_GLOBAL;
  timerBuzzer = 0;
  nBips = 4;
  periodoBuzzer = 8;
}

#else
#warning PROGMEM is not supported on this platform
#endif