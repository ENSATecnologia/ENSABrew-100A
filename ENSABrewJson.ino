//*******************************************************************************************
// Arquivo C/C++ do Projeto Controlador de Patamares -- ENSABrewControll
//-------------------------------------------------------------------------------------------
//
// Arquivo: ENSABrewJson.ino
//
//
// Autor  : Jânio Anselmo, Eng. Me.
// Autor  : Thiago Anselmo, Tecgº
// Autor  : Gabriel da Silva Caetano
//
// CPU    : ATMega2561 - MegaCore
// Clock  : 16MHz
//
// Versão da biblioteca Json: 6.5.0
//
//*******************************************************************************************


//WWWWWWWWWW*********************************************************************************
// Função: setJsonData
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void setJsonData(int type, byte cmd) 
{
  String typeCommandUpper = String(type, HEX);
  typeCommandUpper.toUpperCase();
  String commandUpper = String(cmd, HEX);
  commandUpper.toUpperCase();

  const size_t bufferSize = JSON_OBJECT_SIZE(7);
  DynamicJsonDocument jsonSendBuffer(bufferSize);
  JsonObject dataReceita = jsonSendBuffer.to<JsonObject>();

  dataReceita[F("header")] = F("4E");
  dataReceita[F("product")] = F("0002");
  dataReceita[F("type")] = F("D002");
  
  // Se o numero de serie nao for invalido, envia
  if(code != F("E15")) 
  {
    dataReceita[F("id")] = String(configGeral.idModule);
    dataReceita[F("typeCmd")] = typeCommandUpper;
    dataReceita[F("cmd")] = commandUpper;
  }

  JsonArray error = dataReceita.createNestedArray(F("code"));
  error.add(code);

  // Envia dados pelo módulo Wi-Fi/Bluetooth
  serializeJsonPretty(dataReceita, Serial1);    
  dataReceita.clear();

}

//WWWWWWWWWW*********************************************************************************
// Função: getJsonData
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void getJsonData(JsonObject dataSerialJson) 
{
  // Envia dados pelo módulo Wi-Fi/Bluetooth
  serializeJsonPretty(dataSerialJson, Serial1);   
  dataSerialJson.clear();
}
