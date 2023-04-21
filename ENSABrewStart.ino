//*******************************************************************************************
// Arquivo C/C++ do Projeto Controlador de Patamares -- ENSABrewControll
//-------------------------------------------------------------------------------------------
//
// Arquivo: ENSABrewStart.ino
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
// Função: startI2c
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void startI2c(void) 
{
   Wire.begin(); // ajustaDataHoraManual();
}

//WWWWWWWWWW*********************************************************************************
// Função: startPorts
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void startPorts(void) 
{

  pinMode(ETH_CS, OUTPUT);
  digitalWrite(ETH_CS, LOW);
 
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);

  pinMode(PINO_SSR, OUTPUT);
  digitalWrite(PINO_SSR, LOW);

  pinMode(PINO_RELAY1, OUTPUT);
  digitalWrite(PINO_RELAY1, LOW);

  pinMode(PINO_SOLENOIDE1, OUTPUT);
  digitalWrite(PINO_SOLENOIDE1, LOW);

  pinMode(PINO_SOLENOIDE2, OUTPUT);
  digitalWrite(PINO_SOLENOIDE2, LOW);

  pinMode(LED_VERMELHO1, OUTPUT);
  digitalWrite(LED_VERMELHO1, LOW);

  pinMode(LED_VERMELHO2, OUTPUT);
  digitalWrite(LED_VERMELHO2, LOW);

  pinMode(LED_AMARELO, OUTPUT);
  digitalWrite(LED_AMARELO, LOW);

  pinMode(PINO_NIVEL1, INPUT);

  pinMode(PINO_NIVEL2, INPUT);

  pinMode(PINO_FLUXO, INPUT);

  pinMode(PINO_AC, INPUT);

}