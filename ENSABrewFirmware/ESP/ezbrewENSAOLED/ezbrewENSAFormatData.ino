//*******************************************************************************************
// Arquivo C/C++ do Projeto Controlador de Patamares -- EZBrewControll
//-------------------------------------------------------------------------------------------
// Arquivo  : ezbrewENSAFormatData.ino
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
// Funcao   : converteParaBCD - Converte o número de decimal para BCD
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

byte converteParaBCD(byte val) {
  return ( (val / 10 * 16) + (val % 10) );
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : converteparaDecimal - Converte de BCD para decimal
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

byte converteparaDecimal(byte val) {
  return ( (val / 16 * 10) + (val % 16) );
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : hexToDec
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

long hexToDec(String hexString) {
  long decValue = 0;
  int nextInt;
  for (register int i = 0; i < hexString.length(); i++) {
    nextInt = int(hexString.charAt(i));
    if (nextInt >= 48 && nextInt <= 57) nextInt = map(nextInt, 48, 57, 0, 9);
    if (nextInt >= 65 && nextInt <= 70) nextInt = map(nextInt, 65, 70, 10, 15);
    if (nextInt >= 97 && nextInt <= 102) nextInt = map(nextInt, 97, 102, 10, 15);
    nextInt = constrain(nextInt, 0, 15);
    decValue = (decValue * 16) + nextInt;
  }
  return decValue;
}
