//*******************************************************************************************
// Arquivo C/C++ do Projeto Controlador de Patamares -- ENSABrewControll
//-------------------------------------------------------------------------------------------
//
// Arquivo: ENSABrewLED.ino
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
// Função: piscaLed
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void piscaLed(unsigned int pinLed) 
{
  boolean ledOn = digitalRead(pinLed);
  if (ledOn) digitalWrite(pinLed, LOW);
  else digitalWrite(pinLed, HIGH);
}

//WWWWWWWWWW*********************************************************************************
// Função: piscaLedsAlarme
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void piscaLedsAlarme(void ) 
{
  boolean status1 = digitalRead(LED_VERMELHO1);
  if (status1) digitalWrite(LED_VERMELHO1, LOW);
  else digitalWrite(LED_VERMELHO1, HIGH);
  boolean status2 = digitalRead(LED_VERMELHO2);
  if (status2) digitalWrite(LED_VERMELHO2, LOW);
  else digitalWrite(LED_VERMELHO2, HIGH);
}

//WWWWWWWWWW*********************************************************************************
// Função: desligaLeds
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void desligaLeds(void ) 
{
  digitalWrite(LED_AMARELO, LOW);
  digitalWrite(LED_VERMELHO1, LOW);
  digitalWrite(LED_VERMELHO2, LOW);
}