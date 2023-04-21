//*******************************************************************************************
// Arquivo C/C++ do Projeto Controlador de Patamares -- ENSABrewControll
//-------------------------------------------------------------------------------------------
//
// Arquivo: ENSABrewFormatData.ino
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
// Função: converteParaBCD - Converte o número de decimal para BCD
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

byte converteParaBCD(byte val) 
{
  return ( (val / 10 * 16) + (val % 10) );
}

//WWWWWWWWWW*********************************************************************************
// Função: converteparaDecimal - Converte de BCD para decimal
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

byte converteparaDecimal(byte val) 
{
  return ( (val / 16 * 10) + (val % 16) );
}

//WWWWWWWWWW*********************************************************************************
// Função: hexToDec
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

long hexToDec(String hexString) 
{
  long decValue = 0;
  int nextInt;
  for (register int i = 0; i < hexString.length(); i++) 
  {
    nextInt = int(hexString.charAt(i));
    if (nextInt >= 48 && nextInt <= 57) nextInt = map(nextInt, 48, 57, 0, 9);
    if (nextInt >= 65 && nextInt <= 70) nextInt = map(nextInt, 65, 70, 10, 15);
    if (nextInt >= 97 && nextInt <= 102) nextInt = map(nextInt, 97, 102, 10, 15);
    nextInt = constrain(nextInt, 0, 15);
    decValue = (decValue * 16) + nextInt;
  }
  return decValue;
}
