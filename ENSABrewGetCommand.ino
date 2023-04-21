//*******************************************************************************************
// Arquivo C/C++ do Projeto Controlador de Patamares -- ENSABrewControll
//-------------------------------------------------------------------------------------------
//
// Arquivo: ENSABrewGetCommand.ino
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
// Funcão: replyCommand
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void replyCommand(int type, byte cmd, JsonObject dataObject) 
{
  JsonObject replyObject;
  if(_derrubaMalte) 
  {
    code = F("E14");
    setJsonData(type, cmd);
  }
  else 
  {
    switch (cmd) 
    {
      case 0x01:
        respondeReceitaRemota(type, cmd, dataObject);
        break;
      case 0x02:
        getConfiguracao(/*type, cmd, dataObject*/);
        break;
      case 0x03:
        getTelaAtual(type, cmd, dataObject);
        break;
      case 0x09:
        // getDateTime(type, cmd, dataObject);
        break;
      case 0x0D:
        getCalFactor(/*type, cmd, dataObject*/);
        break;
      case 0x0E:
        getConfigSendReadAC(/*type, cmd, dataObject*/);
        break;
      case 0x0F:
        getConfigSendTemp(/*type, cmd, dataObject*/);
        break;
      case 0x10:
        getConfigPID(/*type, cmd, dataObject*/);
        break;
      case 0xFF:
        getIdModule(/*type, cmd, dataObject*/);
        break;
      default:
        passoMaquina = mSTANDBY;
        code = F("E9"); // replyCommand: Unknown
        break;
    }
  }
}


//WWWWWWWWWW*********************************************************************************
// Funcão: getStatusFase
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

String getStatusFase(void ) 
{
  if (statusFase == RAMPA)
    return F("RAMPA");
  else if (statusFase == PATAMAR)
    return F("PATAMAR");
  else if (statusFase == FERVURA)
    return F("FERVURA");
  else if (statusFase == WHIRLPOOL)
    return F("WHIRLPOOL");
  else if (statusFase == DESCANSO)
    return F("DESCANSO");
  else
    return F("SEM SENSOR");
}

//WWWWWWWWWW*********************************************************************************
// Funcão: getValorPwm - http://playground.arduino.cc/Main/TimerPWMCheatsheet
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

float getValorPwm(void ) 
{
  return((valorPwm / (float)100) * (float)1024);
}

//WWWWWWWWWW*********************************************************************************
// Funcão: getValor1Dec
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

String getValor1Dec(float prValor) 
{
  String st = String(prValor);
  st = st.substring(0, st.indexOf('.') + 2);
  return st;
}

//WWWWWWWWWW*********************************************************************************
// Funcão: getValorInt
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

String getValorInt(float prValor) 
{
  String st = String(prValor);
  st = st.substring(0, st.indexOf('.'));
  return st;
}

//WWWWWWWWWW*********************************************************************************
// Funcão: getConfiguracao
// Data  : 13/02/2019 10:00
//WWWWWWWWWW*********************************************************************************

void getConfiguracao(void ) 
{
  // Informações sobre o dado (tipo do cabeçalho, tipo do dado e tamanho do dado)
  uint8_t auxInfoData[3] = {_HEADER_GET, _GET_CONFIG, 1};
  // Dado em si, com o número do comando
  String auxSendData[5];

  // Número do comando
  auxSendData[0] = F("02");
  auxSendData[1] = String(configGeral.qtdeMaxPatamar);
  auxSendData[2] = String(configGeral.pwmSetpoint);
  auxSendData[3] = String(configGeral.tempoEnvioBrassagem);
  auxSendData[4] = String(configGeral.grauPorMinuto);
  
  // Envia o dado ao APP
  sendDataAPP(&auxInfoData[0], &auxSendData[0]);
}

//WWWWWWWWWW*********************************************************************************
// Funcão: getIdModule
// Data  : 12/03/2019 09:50
//WWWWWWWWWW*********************************************************************************

void getIdModule(void ) 
{
  // Informações sobre o dado (tipo do cabeçalho, tipo do dado e tamanho do dado)
  uint8_t auxInfoData[3] = {_HEADER_GET, _GET_ID, 1};
  // Número do comando
  String auxSendData = F("FF");
  // Envia o dado ao APP
  sendDataAPP(&auxInfoData[0], &auxSendData);
}

//WWWWWWWWWW*********************************************************************************
// Funcão: getTelaAtual
// Data  : 12/03/2019 09:50
//WWWWWWWWWW*********************************************************************************

void getTelaAtual(int type, byte cmd, JsonObject dataObject) 
{
  String typeCommandUpper = String(type, HEX);
  typeCommandUpper.toUpperCase();
  String commandUpper = String(cmd, HEX);
  commandUpper.toUpperCase();

  const size_t bufferSize = JSON_OBJECT_SIZE(7);
  DynamicJsonDocument jsonSendBuffer(bufferSize);
  JsonObject enviaTelaObjJson = jsonSendBuffer.to<JsonObject>();

  enviaTelaObjJson[F("header")] = F("4E");
  enviaTelaObjJson[F("product")] = F("0002");
  enviaTelaObjJson[F("type")] = F("D002");
  enviaTelaObjJson[F("id")] = String(configGeral.idModule);
  enviaTelaObjJson[F("typeCmd")] = typeCommandUpper;
  enviaTelaObjJson[F("cmd")] = commandUpper;
  enviaTelaObjJson[F("tela")] = enviaTela(false);
    
  if(passoMaquina == mSENHA_CONFIGURACAO)
    enviaTelaObjJson[F("digitado")] = entradaAtual;
  else if(opcaoAtual != F("void"))
    enviaTelaObjJson[F("opcao")] = opcaoAtual;

  code = F("A2"); // Success!

  JsonArray error = enviaTelaObjJson.createNestedArray(F("code"));
  error.add(code);

  getJsonData(enviaTelaObjJson);
  enviaTelaObjJson.clear();
  jsonSendBuffer.clear();
}

//WWWWWWWWWW*********************************************************************************
// Funcão: getCalFactor
// Data  : 12/03/2019 09:50
//WWWWWWWWWW*********************************************************************************

void getCalFactor(void ) 
{
  // Informações sobre o dado (tipo do cabeçalho, tipo do dado e tamanho do dado)
  uint8_t auxInfoData[3] = {_HEADER_GET, _GET_CAL_FACTOR, 1};
  // Dado em si, com o número do comando
  String auxSendData[2];

  // Número do comando
  auxSendData[0] = F("0D");
  auxSendData[1] = String(configGeral.calFactor, 2);
  
  // Envia o dado ao APP
  sendDataAPP(&auxInfoData[0], &auxSendData[0]);
}

//WWWWWWWWWW*********************************************************************************
// Funcão: getConfigPID
// Data  : 12/03/2019 09:50
//WWWWWWWWWW*********************************************************************************

void getConfigPID(void ) 
{
  // Informações sobre o dado (tipo do cabeçalho, tipo do dado e tamanho do dado)
  uint8_t auxInfoData[3] = {_HEADER_GET, _GET_CONFIG_PID, 1};
  // Dado em si, com o número do comando
  String auxSendData[5];

  // Número do comando
  auxSendData[0] = F("10");
  auxSendData[1] = String(configGeral.statusPID);
  auxSendData[2] = String(configGeral.kpPID, 2);
  auxSendData[3] = String(configGeral.kiPID, 2);
  auxSendData[4] = String(configGeral.kdPID, 2);
  
  // Envia o dado ao APP
  sendDataAPP(&auxInfoData[0], &auxSendData[0]);
}

//WWWWWWWWWW*********************************************************************************
// Funcão: getConfigSendReadAC
// Data  : 26/09/2019
//WWWWWWWWWW*********************************************************************************

void getConfigSendReadAC(void ) 
{
  // Informações sobre o dado (tipo do cabeçalho, tipo do dado e tamanho do dado)
  uint8_t auxInfoData[3] = {_HEADER_GET, _GET_SEND_TIME_AC, 1};
  // Dado em si, com o número do comando
  String auxSendData[2];

  // Número do comando
  auxSendData[0] = F("0E");
  auxSendData[1] = String(configGeral.timeSendReadAC);
  
  // Envia o dado ao APP
  sendDataAPP(&auxInfoData[0], &auxSendData[0]);
}

//WWWWWWWWWW*********************************************************************************
// Funcão: getConfigSendTemp
// Data  : 26/09/2019 12:06
//WWWWWWWWWW*********************************************************************************

void getConfigSendTemp(void ) 
{
  // Informações sobre o dado (tipo do cabeçalho, tipo do dado e tamanho do dado)
  uint8_t auxInfoData[3] = {_HEADER_GET, _GET_SEND_TIME_TEMP, 1};
  // Dado em si, com o número do comando
  String auxSendData[2];

  // Número do comando
  auxSendData[0] = F("0F");
  auxSendData[1] = String(configGeral.timeSendTemp);
  
  // Envia o dado ao APP
  sendDataAPP(&auxInfoData[0], &auxSendData[0]);
}