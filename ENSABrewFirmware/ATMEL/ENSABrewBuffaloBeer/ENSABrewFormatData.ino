//*******************************************************************************************
// Arquivo C/C++ do Projeto Controlador de Patamares -- ENSABrewControll
//-------------------------------------------------------------------------------------------
//
// Arquivo: ENSABrewFormatData.ino
//
// Autor  : Cláudio B. Pimenta, A.S
// Autor  : Jânio Anselmo, Eng. Me.
// Autor  : Thiago Anselmo, Tecgº
// Autor  : Gabriel da Silva Caetano
//
// CPU    : ATMega2561 - MegaCore
// Clock  : 16MHz
//
//*******************************************************************************************


//WWWWWWWWWW*********************************************************************************
// Function: converteParaBCD - Converte o número de decimal para BCD
// Date    : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

byte converteParaBCD(register byte val) 
{
  return ( (val / 10 * 16) + (val % 10) );
}

//WWWWWWWWWW*********************************************************************************
// Function: converteparaDecimal - Converte de BCD para decimal
// Date    : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

byte converteparaDecimal(register byte val) 
{
  return ( (val / 16 * 10) + (val % 16) );
}

//WWWWWWWWWW*********************************************************************************
// Function: hexToDec
// Date    : 04/04/2017 13:45
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


//WWWWWWWWWW*********************************************************************************
// Function: formatNumber
// Date    : 26/11/2019 13:15
//WWWWWWWWWW*********************************************************************************

String formatNumber(register uint8_t auxNumber)
{
  String formattedNumber; formattedNumber.reserve(2);
  if(auxNumber < 10)
    formattedNumber = "0" + String(auxNumber);
  else formattedNumber = String(auxNumber);

  // Retorna o número formatado
  return(formattedNumber);
}


//WWWWWWWWWW*********************************************************************************
// Function: formatDataScreen
// Date    : 09/06/2020   16:45
//WWWWWWWWWW*********************************************************************************

String formatDataScreen(String auxData)
{
  // ----------------------------------------------- //
  // Índice das strings

  register uint8_t i = 0;

  // ----------------------------------------------- //
  // Vefica o tamanho do espaçamento

  String auxSt; auxSt.reserve(16);
  for(i = 0; i < auxData.length(); i++)
  {
    String actualSt; actualSt.reserve(1);
    actualSt = auxData.substring(i, i+1);
    if(actualSt != F("X"))
      auxSt += actualSt;
  }

  // ----------------------------------------------- //
  // Insere os espaços

  String auxStSpace; auxStSpace.reserve(16);
  for(i = 0; i < (16 - auxSt.length()); i++)
    auxStSpace += F(" ");

  // ----------------------------------------------- //
  // Monta o dado final com o espaçamento

  String auxStEnd; auxStEnd.reserve(16);
  for(i = 0; i < auxData.length(); i++)
  {
    String actualSt; actualSt.reserve(1);
    actualSt = auxData.substring(i, i+1);

    if(actualSt != F("X"))
      auxStEnd += actualSt;
    else
      auxStEnd += auxStSpace;
  }

  // ----------------------------------------------- //

  return(auxStEnd);

  // ----------------------------------------------- //

}