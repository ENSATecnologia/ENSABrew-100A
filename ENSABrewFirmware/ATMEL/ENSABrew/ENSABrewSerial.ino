//*******************************************************************************************
// Arquivo C/C++ do Projeto Controlador de Patamares -- ENSABrewControll
//-------------------------------------------------------------------------------------------
//
// Arquivo: ENSABrewSerial.ino
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
// Função: startSerial
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void startSerial(void ) 
{
  Serial.begin(SERIAL_BAUD_RATE); //Para debug
  while (!Serial) {;}

  Serial1.begin(SERIAL_BAUD_RATE); //Para bluetooth
  while (!Serial1) {;}
}

//WWWWWWWWWW*********************************************************************************
// Função: serialReceiver
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void serialReceiver(void ) 
{
  char startMarker = '{';
  char endMarker = '}';

  while (Serial1.available() > 0 && !_newDataSerial) 
  {
    dataChar = Serial1.read();
    if (dataChar == startMarker) 
    {
      _recvInProgress = true;
      disableInterrupts(); // Disable interrupts
    }
    if (_recvInProgress && idx < (sizeof(receivedSerial) - 1))
      receivedSerial[idx++] = dataChar;
    else if (_recvInProgress)
    {
      idx = 0;
      _recvInProgress = false;
      memset(receivedSerial, 0, sizeof(receivedSerial));
      enableInterrupts(); // Enable interrupts
      return;
    }
    if (dataChar == endMarker) 
    {
      receivedSerial[idx] = '\0'; // Terminate the "String"
      idx = 0;
      _recvInProgress = false;
      _newDataSerial = true;
      enableInterrupts(); // Enable interrupts
    }
  }

  if (_recvInProgress)
    enableInterrupts();
}

//WWWWWWWWWW*********************************************************************************
// Função: analyzesDataSerial
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void analyzesDataSerial(void ) 
{
  if (_newDataSerial) 
  { 
    // https://arduinojson.org/v5/faq/how-to-reuse-a-jsonbuffer/
    _newDataSerial = false;
    StaticJsonDocument<JSON_OBJECT_SIZE(7) + 128> jsonReceivedBuffer;
    DeserializationError jsonError = deserializeJson(jsonReceivedBuffer, receivedSerial);
    JsonObject dataSerialJson = jsonReceivedBuffer.as<JsonObject>();
 
    switch (jsonError) 
    { // https://arduinojson.org/v6/api/misc/deserializationerror/
      case DeserializationError::Ok:
        code = F("A1"); // Deserialization succeeded
        break;
      case DeserializationError::IncompleteInput:
        code = F("E1"); // Incomplete Input
        break;
      case DeserializationError::InvalidInput:
        code = F("E2"); // Invalid input
        break;
      case DeserializationError::NoMemory:
        code = F("E3"); // Not enough memory
        break;
      case DeserializationError::NotSupported:
        code = F("E4"); /// Not Supported
        break;
      case DeserializationError::TooDeep:
        code = F("E5"); // Too Deep
        break;
      default:
        code = F("E6"); // Deserialization failed
        break;
    }

    int typeCommand = RESET;
    byte command = RESET;
     
    if (code == F("A1")) 
    {
      if (dataSerialJson[F("header")] == F("4E") &&
          dataSerialJson[F("product")] == F("0002")) 
      {
        // Recarrega o envio do keep alive
        controlKeepAlive(_LOAD);

        typeCommand = hexToDec(dataSerialJson[F("typeCmd")]);
        command = hexToDec(dataSerialJson[F("cmd")]);
        if(typeCommand != 0xFF && command <= 0xFF) 
        {  
          // Garante que o getId seja executado
          const char* auxIdModule = dataSerialJson[F("id")];
          char idModuleChar[10];
          strncpy(idModuleChar, auxIdModule != NULL ? auxIdModule : "", sizeof(idModuleChar) - 1);
          idModuleChar[sizeof(idModuleChar) - 1] = '\0';
            
          // Verifica o número de série
          if((strcmp(idModuleChar, configGeral.idModule) != 0)) 
          {
            memset(receivedSerial, 0, sizeof(receivedSerial));
            code = F("E15"); // Número de série inválido
            setJsonData(0x00, 0x00);
            return;
          }
        }
        switch (typeCommand) 
        {
          case 0xCC:
            
            requestCommand(typeCommand, command, dataSerialJson);
            
            break;
          
          case 0xFF:
            
            replyCommand(typeCommand, command, dataSerialJson);
            
            break;
          
          case 0xEE: break;
          
          default:
            
            passoMaquina = mSTANDBY;
            code = F("E7"); // typeCommand: Unknown
            
            break;
        }
      }
    }
    memset(receivedSerial, 0, sizeof(receivedSerial));
    dataSerialJson.clear();
  }
}
