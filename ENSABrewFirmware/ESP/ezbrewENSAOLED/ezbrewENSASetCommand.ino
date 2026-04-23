//*******************************************************************************************
// Arquivo C/C++ do Projeto Controlador de Patamares -- EZBrewControll
//-------------------------------------------------------------------------------------------
// Arquivo  : ezbrewENSASetCommand.ino
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
// Funcao   : requestCommand
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void requestCommand(int type, byte cmd, JsonObject dataObject) 
{
  if(_derrubaMalte && cmd != 0x05)
  {
    code = F("E14");
    setJsonData(type, cmd);
  }
  else
  {
    switch (cmd) 
    {
      case 0x01:
        criaEditaReceitaRemota(type, cmd, dataObject, F("I"));
        break;

      case 0x02:
        criaEditaReceitaRemota(type, cmd, dataObject, F("E"));
        break;

      case 0x03:
        apagaReceitaRemota(type, cmd, dataObject);
        break;

      case 0x04:
        executaBrassagemRemota(type, cmd, dataObject);
        break;

      case 0x05: // VIRTUAL KEY
        virtualKeyPad(dataObject);
        responseRemoteControl(type, cmd);
        break;

      case 0x06:
        remoteMaxPatamar(dataObject);
        responseRemoteControl(type, cmd);
        break;

      case 0x07:
        remoteGrauPorMinuto(dataObject);
        responseRemoteControl(type, cmd);
        break;

      case 0x08:
        remoteSetPWM(dataObject);
        responseRemoteControl(type, cmd);
        break;

      case 0x09:
        remoteSetDateAndTime(dataObject);
        responseRemoteControl(type, cmd);
        break;

      case 0x0A:
        remoteCleaningEeprom(type, cmd);
        break;

      case 0x0B:
        remoteSetTempoEnvioBrassagem(dataObject);
        responseRemoteControl(type, cmd);
        break;

      case 0xFF:
        setIdModule(dataObject);
        break;

      default:
        passoMaquina = mSTANDBY;
        code = F("E8"); // requestCommand: Unknown
        break;
    }
  }
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : remoteMaxPatar
// Data     : 03/01/2019 10:30
//WWWWWWWWWW*********************************************************************************

void remoteMaxPatamar(JsonObject dataObject)
{
  configGeral.qtdeMaxPatamar = dataObject[F("maxPatamar")].as<int>();
  salvaConfigGeral();
  passoMaquina = mMENU;
  code = F("A1");
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : remoteGrauPorMinuto
// Data     : 03/01/2019 10:30
//WWWWWWWWWW*********************************************************************************

void remoteGrauPorMinuto(JsonObject dataObject)
{
  configGeral.grauPorMinuto = dataObject[F("grauPorMinuto")].as<float>();
  salvaConfigGeral();
  passoMaquina = mMENU;
  code = F("A1");
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : remoteSetPWM
// Data     : 03/01/2019 10:30
//WWWWWWWWWW*********************************************************************************

void remoteSetPWM(JsonObject dataObject)
{
  configGeral.pwmSetpoint = dataObject[F("setPWM")].as<byte>();
  salvaConfigGeral();
  passoMaquina = mMENU;
  code = F("A1");
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : remoteSetDateAndTime
// Data     : 03/01/2019 10:30
//WWWWWWWWWW*********************************************************************************

void remoteSetDateAndTime(JsonObject dataObject)
{
  #ifdef RTC_OFF

  passoMaquina = mSET_DATA_HORA;
  code = F("NOT RTC");
  bipAtencao();

  #else

  byte zero = 0x00;
  byte segundos = dataObject[F("segundos")].as<byte>();            
  byte minutos = dataObject[F("minutos")].as<byte>();             
  byte horas = dataObject[F("horas")].as<byte>();            
  byte diadasemana = dataObject[F("diaDaSemana")].as<byte>();       
  byte diadomes = dataObject[F("diaDoMes")].as<byte>();         
  byte mes = dataObject[F("mes")].as<byte>();              
  byte ano = dataObject[F("ano")].as<byte>();              

  Wire.beginTransmission(ADDRESS_DS1307);
  Wire.write(zero);
  Wire.write(converteParaBCD(segundos));
  Wire.write(converteParaBCD(minutos));
  Wire.write(converteParaBCD(horas));
  Wire.write(converteParaBCD(diadasemana));
  Wire.write(converteParaBCD(diadomes));
  Wire.write(converteParaBCD(mes));
  Wire.write(converteParaBCD(ano));
  Wire.write(zero);               //Start no CI
  Wire.endTransmission();

  passoMaquina = mMENU;
  code = F("A1");

  #endif
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : remoteSetTempoEnvioBrassagem
// Data     : 12/02/2019 10:45
//WWWWWWWWWW*********************************************************************************

void remoteSetTempoEnvioBrassagem(JsonObject dataObject)
{
  configGeral.tempoEnvioBrassagem = dataObject[F("setTime")].as<int>();
  salvaConfigGeral();
  passoMaquina = mMENU;
  code = F("A1");
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : setIdModule
// Data     : 12/03/2019 09:40
//WWWWWWWWWW*********************************************************************************

void setIdModule(JsonObject dataObject)
{
  String auxKey = dataObject[F("key")].as<String>();
  if(auxKey == F("excENSA"))
  {
    String auxIdModule = dataObject[F("newId")].as<String>();
    auxIdModule.toCharArray(configGeral.idModule, 10);
    salvaConfigGeral();
    passoMaquina = mMENU;
    code = F("A1");
    responseRemoteControl(0xCC, 0xFF);
  }
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : respondeRemoteControl
// Data     : 03/01/2019 10:30
//WWWWWWWWWW*********************************************************************************

void responseRemoteControl(int type, byte cmd)
{
  String typeCommandUpper = String(type, HEX);
  typeCommandUpper.toUpperCase();
  String commandUpper = String(cmd, HEX);
  commandUpper.toUpperCase();

  const size_t bufferSize = JSON_OBJECT_SIZE(7);
  DynamicJsonDocument jsonSendBuffer(bufferSize);
  JsonObject responseCommand = jsonSendBuffer.to<JsonObject>();

  responseCommand[F("header")] = F("4E");
  responseCommand[F("product")] = F("0002");
  responseCommand[F("type")] = F("D002");
  responseCommand[F("id")] = String(configGeral.idModule);
  responseCommand[F("typeCmd")] = typeCommandUpper;
  responseCommand[F("cmd")] = commandUpper;

  JsonArray error = responseCommand.createNestedArray(F("code"));
  error.add(code);

  serializeJsonPretty(responseCommand, Serial1);
  responseCommand.clear();
}