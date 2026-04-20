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
  if (hexString.length() == 0)
    return -1;

  long decValue = 0;
  int nextInt;
  for (register int i = 0; i < hexString.length(); i++)
  {
    nextInt = int(hexString.charAt(i));
    if (nextInt >= '0' && nextInt <= '9')      nextInt = nextInt - '0';
    else if (nextInt >= 'A' && nextInt <= 'F') nextInt = nextInt - 'A' + 10;
    else if (nextInt >= 'a' && nextInt <= 'f') nextInt = nextInt - 'a' + 10;
    else return -1;

    decValue = (decValue << 4) + nextInt;
  }
  return decValue;
}
