//*******************************************************************************************
// Arquivo C/C++ do Projeto Controlador de Patamares -- EZBrewControll
//-------------------------------------------------------------------------------------------
// Arquivo  : ezbrewENSASerial.ino
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
// Funcao   : serialReceiver
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void serialReceiver() {

  char startMarker = '{';
  char endMarker = '}';

  while (Serial1.available() > 0 && !_newDataSerial) {
    dataChar = Serial1.read();
    if (dataChar == startMarker) {
      _recvInProgress = true;
      disableInterrupts(); // Disable interrupts
    }
    if (_recvInProgress)
      receivedSerial[idx++] = dataChar;
    if (dataChar == endMarker) {
      receivedSerial[idx] = '\0'; // Terminate the "String"
      idx = 0;
      _recvInProgress = false;
      _newDataSerial = true;
      enableInterrupts(); // Enable interrupts
    }
  }
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : analyzesDataSerial
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void analyzesDataSerial() {

  /* Static & Dynamic JSON
    StaticJsonDocument<DATA_UTIL_SERIAL> jsonReceivedBuffer;
    const size_t bufferSize = JSON_OBJECT_SIZE(7) + 90;
    DynamicJsonDocument jsonReceivedBuffer(bufferSize);
  */

  if (_newDataSerial) { //https://arduinojson.org/v5/faq/how-to-reuse-a-jsonbuffer/
    _newDataSerial = false;
    const size_t bufferSize = JSON_OBJECT_SIZE(7) + 90;
    DynamicJsonDocument jsonReceivedBuffer(bufferSize);
    DeserializationError jsonError = deserializeJson(jsonReceivedBuffer, receivedSerial);
    JsonObject dataSerialJson = jsonReceivedBuffer.as<JsonObject>();
 
    switch (jsonError) { // https://arduinojson.org/v6/api/misc/deserializationerror/
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
        typeCommand = hexToDec(dataSerialJson[F("typeCmd")]);
        command = hexToDec(dataSerialJson[F("cmd")]);

        if(typeCommand != 0xFF && command != 0xFF) // garante que o getId seja executado
        {
          String auxIdModule = dataSerialJson[F("id")].as<String>();
          char idModuleChar[10];
          auxIdModule.toCharArray(idModuleChar, 10);
          if((strcmp(idModuleChar, configGeral.idModule) != 0))// Verifica o numero de serie
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
          default:
            passoMaquina = mSTANDBY;
            code = F("E7"); // typeCommand: Unknown
            break;
        }
      }
    }
    memset(receivedSerial, 0, sizeof(receivedSerial));
    dataSerialJson.clear();
    jsonReceivedBuffer.clear();
  }
}