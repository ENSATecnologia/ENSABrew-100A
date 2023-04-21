//*******************************************************************************************
// Arquivo C/C++ do Projeto Controlador de Patamares -- ENSABrewControll
//-------------------------------------------------------------------------------------------
//
// Arquivo: ENSABrewBeep.ino
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
// Função: buzzerInterruption - Interruption TIMER4 - 10ms
// Data  : 09/09/2016 09:55
//WWWWWWWWWW*********************************************************************************

void buzzerInterruption(void ) 
{
  machineBuzzer();
}

//WWWWWWWWWW*********************************************************************************
// Função: machineBuzzer
// Data  : 15/11/2007 09:55
//WWWWWWWWWW*********************************************************************************

void machineBuzzer(void ) 
{
  switch (passoBuzzer) 
  {
    case gmBUZZER_OFF:
      noTone(PINO_BUZZER);
      break;

    case gmBIP_GLOBAL: {
        timerBuzzer++;
        if (timerBuzzer >= periodoBuzzer) 
        {
          timerBuzzer = 0;
          boolean _inverteBuzzer = digitalRead(PINO_BUZZER);
          if (_inverteBuzzer) noTone(PINO_BUZZER);
          else tone(PINO_BUZZER, F_TONE);
          if (nBips) 
          {
            if (--nBips == 0)
              passoBuzzer = gmBUZZER_OFF;
          }
        }
      }
      break;

    case gmBIP_ALARME: {
        periodoBuzzer++;
        if (periodoBuzzer >= BUZZER_PERIODO) 
        { // DEFAULT: 10
          periodoBuzzer = 0;
          if (nBips) 
          {
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
// Função: bipInicial
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void bipInicial(void ) 
{
  passoBuzzer = gmBIP_GLOBAL;
  timerBuzzer = 0;
  nBips = 6;
  periodoBuzzer = 12;
}

//WWWWWWWWWW*********************************************************************************
// Função: bipShutdown
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void bipShutdown(void ) 
{
  passoBuzzer = gmBIP_GLOBAL;
  timerBuzzer = 0;
  nBips = 1;
  periodoBuzzer = 1;
}

//WWWWWWWWWW*********************************************************************************
// Função: bipServerError
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void bipServerError(void ) 
{
  passoBuzzer = gmBIP_GLOBAL;
  timerBuzzer = 0;
  nBips = 2;
  periodoBuzzer = 4;
}

//WWWWWWWWWW*********************************************************************************
// Função: bipTeclado
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void bipTeclado(void ) 
{
  passoBuzzer = gmBIP_GLOBAL;
  timerBuzzer = 0;
  nBips = 1;
  periodoBuzzer = 3;
}

//WWWWWWWWWW*********************************************************************************
// Função: bipAlarme
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void bipAlarme(void ) 
{
  passoBuzzer = gmBIP_ALARME;
  tone(PINO_BUZZER, F_TONE);
  timerBuzzer = 0;
  nBips = 5;
  periodoBuzzer = 0;
}

//WWWWWWWWWW*********************************************************************************
// Função: bipOK
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void bipOK(void ) 
{
  passoBuzzer = gmBIP_GLOBAL;
  timerBuzzer = 0;
  nBips = 6;
  periodoBuzzer = 6;
}

//WWWWWWWWWW*********************************************************************************
// Função: bipAtencao
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void bipAtencao(void ) 
{
  passoBuzzer = gmBIP_GLOBAL;
  timerBuzzer = 0;
  nBips = 10;
  periodoBuzzer = 8;
}

//WWWWWWWWWW*********************************************************************************
// Função: bipAcerto
// Data  : 09/04/2010 15:46
//WWWWWWWWWW*********************************************************************************

void bipAcerto(void ) 
{
  passoBuzzer = gmBIP_GLOBAL;
  timerBuzzer = 0;
  nBips = 4;
  periodoBuzzer = 8;
}

#else
#warning PROGMEM is not supported on this platform
#endif