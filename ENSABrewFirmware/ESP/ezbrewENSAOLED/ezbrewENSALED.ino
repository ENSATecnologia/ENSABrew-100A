//*******************************************************************************************
// Arquivo C/C++ do Projeto Controlador de Patamares -- EZBrewControll
//-------------------------------------------------------------------------------------------
// Arquivo  : ezbrewENSALED.ino
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
// Funcao   : piscaLed
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void piscaLed(unsigned int pinLed) {
  boolean ledOn = digitalRead(pinLed);
  if (ledOn) digitalWrite(pinLed, LOW);
  else digitalWrite(pinLed, HIGH);
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : piscaLedsAlarme
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void piscaLedsAlarme() {
  boolean status1 = digitalRead(LED_VERMELHO1);
  if (status1) digitalWrite(LED_VERMELHO1, LOW);
  else digitalWrite(LED_VERMELHO1, HIGH);
  boolean status2 = digitalRead(LED_VERMELHO2);
  if (status2) digitalWrite(LED_VERMELHO2, LOW);
  else digitalWrite(LED_VERMELHO2, HIGH);
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : desligaLeds
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void desligaLeds() {
  digitalWrite(LED_AMARELO, LOW);
  digitalWrite(LED_VERMELHO1, LOW);
  digitalWrite(LED_VERMELHO2, LOW);
}
