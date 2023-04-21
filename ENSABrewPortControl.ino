//*******************************************************************************************
// Arquivo C/C++ do Projeto Controlador de Patamares -- ENSABrewControll
//-------------------------------------------------------------------------------------------
//
// Arquivo: ENSABrewPortControl.ino
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
// Função: readPortTasks
// Data  : 23/01/2020 08:40
//WWWWWWWWWW*********************************************************************************

void readPortTasks(void )
{
  // Leitura das entradas
  readPort1(); readPort2();
}

//WWWWWWWWWW*********************************************************************************
// Função: readPort1
// Data  : 20/08/2019 08:20
//WWWWWWWWWW*********************************************************************************

void readPort1(void ) 
{
  boolean readingPort_1 = digitalRead(PINO_NIVEL1);

  if (readingPort_1 != lastStatePort_1) 
    lastDebounceTimePort_1 = millis();

  if ((millis() - lastDebounceTimePort_1) > debounceDelay) 
  {
    if (readingPort_1 != statePort_1) 
    {
      statePort_1 = readingPort_1;
      
      // Se estiver conectado ao APP
      if(_connectedAPP)
      {
        // Indica envio de evento de leitura do PORT 1
        sendDataReadPort(1); 
      }
    }
  }

  lastStatePort_1 = readingPort_1;
}

//WWWWWWWWWW*********************************************************************************
// Função: readPort2
// Data  : 20/08/2019 08:20
//WWWWWWWWWW*********************************************************************************

void readPort2(void ) 
{
  boolean readingPort_2 = digitalRead(PINO_NIVEL2);

  if (readingPort_2 != lastStatePort_2) 
    lastDebounceTimePort_2 = millis();

  if ((millis() - lastDebounceTimePort_2) > debounceDelay) 
  {
    if (readingPort_2 != statePort_2) 
    {
      statePort_2 = readingPort_2;
      
      // Se estiver conectado ao APP
      if(_connectedAPP)
      {
        // Indica envio de evento de leitura do PORT 2
        sendDataReadPort(2);
      }
    }
  }

  lastStatePort_2 = readingPort_2;
}

//WWWWWWWWWW*********************************************************************************
// Função: sendDataReadPort
// Data  : 04/09/2019 10:05
//WWWWWWWWWW*********************************************************************************

void sendDataReadPort(uint8_t auxPort) 
{
  // Informações sobre o dado (tipo do cabeçalho, tipo do dado e tamanho do dado)
  uint8_t auxInfoData[3] = {_HEADER_EVENT, _READ_PORT, 1};
  // Dado em si, com o número do comando
  String auxSendData[3];

  // Número do comando
  auxSendData[0] = F("01");

  // Verifica o PORT em que o evento ocorreu e o estado do mesmo
  boolean auxStatePort;
  if(auxPort == 1) 
  {
    auxStatePort = statePort_1;
    auxSendData[1] = F("01");
  }
  else if(auxPort == 2) 
  {
    auxStatePort = statePort_2;
    auxSendData[1] = F("02");
  }

  // Define o estado para envio
  if(auxStatePort == HIGH)
    auxSendData[2] = F("HIGH");
  else if(auxStatePort == LOW)
    auxSendData[2] = F("LOW");

  // Envia o dado ao APP
  sendDataAPP(&auxInfoData[0], &auxSendData[0]);
}