//*******************************************************************************************
// Arquivo C/C++ do Projeto Controlador de Patamares -- EZBrewControll
//-------------------------------------------------------------------------------------------
// Arquivo  : ezbrewENSAJson.ino
// Inicio   : 29/04/2013 10:23
// Revisão  : 11/05/2016 18:31
//
// AutorI   : Cláudio B. Pimenta, A.S
// AutorII  : Jânio Anselmo, Eng. Me.
// AutorIIII : Gabriel da Silva Caetano, Estágio.
//
// CPU:     : ESP8266 - WEMOS D1 MINI Pro
// Clock:   : 16MHz
//
// Inicio   : 29/04/2013 10:23
// Revisão  : 11/06/2016 09:28
//
// Versão da biblioteca Json: 6.5.0
//
//*******************************************************************************************

//WWWWWWWWWW*********************************************************************************
// Funcao   : setJsonData
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void setJsonData(int type, byte cmd) {
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

  if(code != F("E15")) // Se o numero de serie nao for invalido, envia
  {
    dataReceita[F("id")] = String(configGeral.idModule);
    dataReceita[F("typeCmd")] = typeCommandUpper;
    dataReceita[F("cmd")] = commandUpper;
  }

  JsonArray error = dataReceita.createNestedArray(F("code"));
  error.add(code);

  serializeJsonPretty(dataReceita, Serial1);
  dataReceita.clear();
  if ((code != F("A1")) || (code != F("A2"))) bipAtencao();
  else bipAcerto();
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : getJsonData
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void getJsonData(JsonObject dataSerialJson) {
  serializeJsonPretty(dataSerialJson, Serial);
  dataSerialJson.clear();
  if ((code != F("A1")) || (code != F("A2"))) bipAtencao();
  else bipAcerto();
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : jsonMerge
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void jsonMerge(JsonObject dest, const JsonObject src) {
  disableInterrupts(); // Disable interrupts
  for (auto kvp : src)
    dest[kvp.key().c_str()] = kvp.value().as<char*>();
  enableInterrupts(); // Enable interrupts
}