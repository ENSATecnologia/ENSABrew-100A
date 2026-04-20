//*******************************************************************************************
// Arquivo C/C++ do Projeto Controlador de Patamares -- ENSABrewControll
//-------------------------------------------------------------------------------------------
//
// Arquivo: ENSABrewSerial.ino
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
  const char startMarker = '{';
  const char endMarker = '}';
  const uint16_t maxPayloadLength = sizeof(receivedSerial) - 1;

  while (Serial1.available() > 0 && !_newDataSerial)
  {
    dataChar = Serial1.read();
    if (dataChar == startMarker)
    {
      _recvInProgress = true;
    }
    if (_recvInProgress)
    {
      // Evita estouro do buffer serial para manter o parse seguro.
      if (idx < maxPayloadLength)
      {
        receivedSerial[idx++] = dataChar;
      }
      else
      {
        idx = 0;
        _recvInProgress = false;
        _newDataSerial = false;
        _serialOverflow = true;
        memset(receivedSerial, 0, sizeof(receivedSerial));
        continue;
      }
    }
    if (dataChar == endMarker)
    {
      receivedSerial[idx] = '\0'; // Terminate the "String"
      idx = 0;
      _recvInProgress = false;
      _newDataSerial = true;
    }
  }
}

//WWWWWWWWWW*********************************************************************************
// Função: analyzesDataSerial
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void analyzesDataSerial(void )
{
  if (_serialOverflow)
  {
    _serialOverflow = false;
    code = F("E16"); // Buffer serial excedido
    setJsonData(0x00, 0x00);
  }

  if (_newDataSerial)
  {
    // https://arduinojson.org/v5/faq/how-to-reuse-a-jsonbuffer/
    _newDataSerial = false;
    const size_t bufferSize = JSON_OBJECT_SIZE(7) + 90;
    DynamicJsonDocument jsonReceivedBuffer(bufferSize);
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
    long parsedTypeCommand = -1;
    long parsedCommand = -1;
    bool isValidCommand = false;

    if (code == F("A1"))
    {
      if (dataSerialJson[F("header")] == F("4E") &&
          dataSerialJson[F("product")] == F("0002"))
      {
        // Recarrega o envio do keep alive
        controlKeepAlive(_LOAD);

        const char *typeCmd = dataSerialJson[F("typeCmd")] | "";
        const char *cmd = dataSerialJson[F("cmd")] | "";
        parsedTypeCommand = hexToDec(typeCmd);
        parsedCommand = hexToDec(cmd);
        if(parsedTypeCommand >= 0x00 && parsedTypeCommand <= 0xFF &&
           parsedCommand >= 0x00 && parsedCommand <= 0xFF)
        {
          typeCommand = parsedTypeCommand;
          command = parsedCommand;
          isValidCommand = true;

          // Garante que o getId seja executado
          const char *idModule = dataSerialJson[F("id")] | "";

          // Verifica o número de série
          if((strcmp(idModule, configGeral.idModule) != 0))
          {
            memset(receivedSerial, 0, sizeof(receivedSerial));
            code = F("E15"); // Número de série inválido
            setJsonData(0x00, 0x00);
            return;
          }
        }
        else
        {
          code = F("E17"); // Campo hex inválido
          setJsonData(0x00, 0x00);
        }
        if (isValidCommand)
        {
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
      else
      {
        code = F("E2"); // Invalid input
        setJsonData(0x00, 0x00);
      }
    }
    memset(receivedSerial, 0, sizeof(receivedSerial));
    dataSerialJson.clear();
    jsonReceivedBuffer.clear();
  }
}
