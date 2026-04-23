//*******************************************************************************************
// Arquivo C/C++ do Projeto Controlador de Patamares -- EZBrewControll
//-------------------------------------------------------------------------------------------
// Arquivo  : ezbrewENSAGetCommand.ino
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
// Funcao   : replyCommand
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void replyCommand(int type, byte cmd, JsonObject dataObject) {
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
        getConfiguracao(type, cmd, dataObject);
        break;
      case 0xFF:
        getIdModule(type, cmd, dataObject);
        break;
      default:
        passoMaquina = mSTANDBY;
        code = F("E9"); // replyCommand: Unknown
        break;
    }
  }
}


//WWWWWWWWWW*********************************************************************************
// Funcao   : getStatusFase
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

String getStatusFase() {
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
// Funcao   : getValorPwm - http://playground.arduino.cc/Main/TimerPWMCheatsheet
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

float getValorPwm() {
  float dutyCycle = (valorPwm / (float)100) * (float)1024;
  return dutyCycle;
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : getValor1Dec
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

String getValor1Dec(float prValor) {
  String st = String(prValor);
  st = st.substring(0, st.indexOf('.') + 2);
  return st;
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : getValorInt
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

String getValorInt(float prValor) {
  String st = String(prValor);
  st = st.substring(0, st.indexOf('.'));
  return st;
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : getTemperatura
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void getTemperatura() {
  if (_readTemperature) {
    _readTemperature = false;
    realTemperature += thermistor.getTemp();
    countTemperature++;
    if (countTemperature >= MEDIA_TEMPERATURE) {
      countTemperature = 0;
      mediaTemperature = realTemperature / MEDIA_TEMPERATURE;
      realTemperature = 0;
    }
  }
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : getConfiguracao
// Data     : 13/02/2019 10:00
//WWWWWWWWWW*********************************************************************************

void getConfiguracao(int type, byte cmd, JsonObject dataObject) 
{
  String typeCommandUpper = String(type, HEX);
  typeCommandUpper.toUpperCase();
  String commandUpper = String(cmd, HEX);
  commandUpper.toUpperCase();

  const size_t bufferSize = JSON_OBJECT_SIZE(7);
  DynamicJsonDocument jsonSendBuffer(bufferSize);
  JsonObject dataConfiguracao = jsonSendBuffer.to<JsonObject>();

  dataConfiguracao[F("header")] = F("4E");
  dataConfiguracao[F("product")] = F("0002");
  dataConfiguracao[F("type")] = F("D002");
  dataConfiguracao[F("id")] = String(configGeral.idModule);
  dataConfiguracao[F("typeCmd")] = typeCommandUpper;
  dataConfiguracao[F("cmd")] = commandUpper;
  dataConfiguracao[F("qtdeMaxPatamar")] = String(configGeral.qtdeMaxPatamar);
  dataConfiguracao[F("pwmSetpoint")] = String(configGeral.pwmSetpoint);
  dataConfiguracao[F("tempoEnvioBrassagem")] = String(configGeral.tempoEnvioBrassagem);
  dataConfiguracao[F("grauPorMinuto")] = String(configGeral.grauPorMinuto);
  
  code = F("A2"); // Success!

  JsonArray error = dataConfiguracao.createNestedArray(F("code"));
  error.add(code);

  getJsonData(dataConfiguracao);
  dataConfiguracao.clear();
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : getIdModule
// Data     : 12/03/2019 09:50
//WWWWWWWWWW*********************************************************************************

void getIdModule(int type, byte cmd, JsonObject dataObject)
{
  String typeCommandUpper = String(type, HEX);
  typeCommandUpper.toUpperCase();
  String commandUpper = String(cmd, HEX);
  commandUpper.toUpperCase();

  const size_t bufferSize = JSON_OBJECT_SIZE(7);
  DynamicJsonDocument jsonSendBuffer(bufferSize);
  JsonObject dataConfiguracao = jsonSendBuffer.to<JsonObject>();

  dataConfiguracao[F("header")] = F("4E");
  dataConfiguracao[F("product")] = F("0002");
  dataConfiguracao[F("type")] = F("D002");
  dataConfiguracao[F("id")] = String(configGeral.idModule);
  dataConfiguracao[F("typeCmd")] = typeCommandUpper;
  dataConfiguracao[F("cmd")] = commandUpper;

  code = F("A2"); // Success!

  JsonArray error = dataConfiguracao.createNestedArray(F("code"));
  error.add(code);

  getJsonData(dataConfiguracao);
  dataConfiguracao.clear();
}