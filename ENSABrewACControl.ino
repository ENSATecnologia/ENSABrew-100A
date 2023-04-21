//*******************************************************************************************
// Arquivo C/C++ do Projeto Controlador de Patamares -- ENSABrewControll
//-------------------------------------------------------------------------------------------
//
// Arquivo: ENSABrewACControl.ino
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
// Função: acInterruption - Interruption TIMER1 - 1ms
// Data  : 09/09/2016 09:55
//WWWWWWWWWW*********************************************************************************

void acInterruption() 
{
  pulseAC++;
}

//WWWWWWWWWW*********************************************************************************
// Função: readAC
// Data  : 10/09/2019 15:30
//WWWWWWWWWW*********************************************************************************

void readAC(void) 
{
  if(_sendDataAC) 
  {
    _sendDataAC = false;
    sendDataAC(false);
  }
}

//WWWWWWWWWW*********************************************************************************
// Função: sendDataAC
// Data  : 10/09/2019 15:30
//WWWWWWWWWW*********************************************************************************

void sendDataAC(boolean energyAC) 
{
  // Informações sobre o dado (tipo do cabeçalho, tipo do dado e tamanho do dado)
  uint8_t auxInfoData[3] = {_HEADER_EVENT, _READ_AC, 1};
  // Dado em si, com o número do comando
  String auxSendData[2];

  // Número do comando
  auxSendData[0] = F("04");
  auxSendData[1] = String(energyAC);
  
  // Envia o dado ao APP
  sendDataAPP(&auxInfoData[0], &auxSendData[0]);
}