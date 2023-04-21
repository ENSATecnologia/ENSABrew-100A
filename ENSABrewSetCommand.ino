//*******************************************************************************************
// Arquivo C/C++ do Projeto Controlador de Patamares -- ENSABrewControll
//-------------------------------------------------------------------------------------------
//
// Arquivo: ENSABrewSetCommand.ino
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
// Função: requestCommand
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void requestCommand(int type, byte cmd, JsonObject dataObject) 
{
  if(_derrubaMalte && cmd != 0x05) 
  {
    code = F("E14");
    setJsonData(type, cmd);
  }
  else {
    switch (cmd) 
    {
      case 0x00:
        confirmConnectionAPP();
        break;
        
      case 0x01:
        criaEditaReceitaRemota(type, cmd, dataObject, _CRIAR_RECEITA);
        break;

      case 0x02:
        criaEditaReceitaRemota(type, cmd, dataObject, _EDITAR_RECEITA);
        break;

      case 0x03:
        apagaReceitaRemota(type, cmd, dataObject);
        break;

      case 0x04:
        executaBrassagemRemota(type, cmd, dataObject);
        break;

      case 0x05: // VIRTUAL KEY
        virtualKeyPad(dataObject);
        responseRemoteControl(cmd);
        break;

      case 0x06:
        remoteMaxPatamar(dataObject);
        responseRemoteControl(cmd);
        break;

      case 0x07:
        remoteGrauPorMinuto(dataObject);
        responseRemoteControl(cmd);
        break;

      case 0x08:
        remoteSetPWM(dataObject);
        responseRemoteControl(cmd);
        break;

      case 0x09:
          // Se o uso das funções do relógio estiver habilitada, gera e utiliza a função
          #ifdef FUNCTIONS_CLOCK
            remoteSetDateAndTime(dataObject);
            responseRemoteControl(cmd);
          #endif
        break;

      case 0x0A:
        remoteCleaningEeprom(type, cmd);
        break;

      case 0x0B:
        remoteSetTempoEnvioBrassagem(dataObject);
        responseRemoteControl(cmd);
        break;

      case 0x0C:
        remoteSolenoidAction(dataObject);
        responseRemoteControl(cmd);
        break;

      case 0x0D:
        remoteAdjustCalFactor(dataObject);
        responseRemoteControl(cmd);
        break;

      case 0x0E:
        remoteConfigReadAC(dataObject);
        responseRemoteControl(cmd);
        break;

      case 0x0F:
        remoteConfigSendTemp(dataObject);
        responseRemoteControl(cmd);
        break;

      case 0x10:
        remoteConfigPID(dataObject);
        responseRemoteControl(cmd);
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
// Função: remoteMaxPatar
// Data  : 03/01/2019 10:30
//WWWWWWWWWW*********************************************************************************

void remoteMaxPatamar(JsonObject dataObject) 
{
  configGeral.qtdeMaxPatamar = dataObject[F("maxPatamar")].as<int>();
  // Armazena a configuração
  salvaConfigGeral(_CONFIG_SOCKET);
  passoMaquina = mMENU;
}

//WWWWWWWWWW*********************************************************************************
// Função: remoteGrauPorMinuto
// Data  : 03/01/2019 10:30
//WWWWWWWWWW*********************************************************************************

void remoteGrauPorMinuto(JsonObject dataObject) 
{
  configGeral.grauPorMinuto = dataObject[F("grauPorMinuto")].as<float>();
  // Armazena a configuração
  salvaConfigGeral(_CONFIG_SOCKET);
  passoMaquina = mMENU;
}

//WWWWWWWWWW*********************************************************************************
// Função: remoteSetPWM
// Data  : 03/01/2019 10:30
//WWWWWWWWWW*********************************************************************************

void remoteSetPWM(JsonObject dataObject) 
{
  configGeral.pwmSetpoint = dataObject[F("setPWM")].as<byte>();
  // Armazena a configuração
  salvaConfigGeral(_CONFIG_SOCKET);
  passoMaquina = mMENU;
}

//WWWWWWWWWW*********************************************************************************
// Função: remoteSetDateAndTime
// Data  : 03/01/2019 10:30
//WWWWWWWWWW*********************************************************************************

// Se o uso das funções do relógio estiver habilitada, gera e utiliza a função
#ifdef FUNCTIONS_CLOCK

void remoteSetDateAndTime(JsonObject dataObject) 
{
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
}

#endif // #ifdef FUNCTIONS_CLOCK

//WWWWWWWWWW*********************************************************************************
// Função: remoteSetTempoEnvioBrassagem
// Data  : 12/02/2019 10:45
//WWWWWWWWWW*********************************************************************************

void remoteSetTempoEnvioBrassagem(JsonObject dataObject) 
{
  configGeral.tempoEnvioBrassagem = dataObject[F("setTime")].as<int>();
  // Armazena a configuração
  salvaConfigGeral(_CONFIG_SOCKET);
  passoMaquina = mMENU;
}

//WWWWWWWWWW*********************************************************************************
// Função: setIdModule
// Data  : 12/03/2019 09:40
//WWWWWWWWWW*********************************************************************************

void setIdModule(JsonObject dataObject) 
{
  String auxKey = dataObject[F("key")].as<String>();
  if(auxKey == F("excENSA")) 
  {
    String auxIdModule = dataObject[F("newId")].as<String>();
    auxIdModule.toCharArray(configGeral.idModule, 10);
    // Armazena a configuração
    salvaConfigGeral(_CONFIG_SOCKET);
    passoMaquina = mMENU;
    responseRemoteControl(0xFF);
  }
}

//WWWWWWWWWW*********************************************************************************
// Função: remoteSolenoidAction
// Data  : 03/01/2019 10:30
//WWWWWWWWWW*********************************************************************************

void remoteSolenoidAction(JsonObject dataObject) 
{
  // Verifica o solenóide à sofrer a ação
  boolean auxSolenoid = dataObject[F("solenoid")].as<int>(); 

  // Pino de acionamento do solenoid
  register uint8_t auxSolenoidPin;
  if(auxSolenoid == SOLENOID1)
    auxSolenoidPin = PINO_SOLENOIDE1;
  else if(auxSolenoid == SOLENOID2)
    auxSolenoidPin = PINO_SOLENOIDE2;

  // Verifica o tipo de temporizador
  timerTypeSolenoid[auxSolenoid] = dataObject[F("timerType")].as<int>(); 
  // Carrega o temporizador
  wdtActionSolenoid[auxSolenoid] = dataObject[F("operatingTime")].as<int>();
  
  if(dataObject[F("action")].as<int>() == HIGH)
    digitalWrite(auxSolenoidPin, HIGH);

  else if(dataObject[F("action")].as<int>() == LOW)
    digitalWrite(auxSolenoidPin, LOW);
  
}

//WWWWWWWWWW*********************************************************************************
// Função: remoteAdjustCalFactor
// Data  : 19/09/2019 14:49
//WWWWWWWWWW*********************************************************************************

void remoteAdjustCalFactor(JsonObject dataObject) 
{
  // Sinaliza o tipo de ajuste do fator: direto ou automático
  boolean auxTypeAdjust;

  // Recebe o valor direto do fator de calibração
  float auxAdjustCalFactor = dataObject[F("calFactor")].as<float>(); 
  
  /*
    Se o fator de calibração vier zerado, quer dizer que 
    o ajuste será feito automaticamente, logo, se faz a 
    leitura dos litros inseridos (real)
  */
  
  if (auxAdjustCalFactor == 0) 
  {
    // Sinaliza ajuste automático
    auxTypeAdjust = _AUTO_ADJUSTMENT;
    // Lê os litros reais inseridos
    auxAdjustCalFactor = dataObject[F("realLiters")].as<float>(); 
  }
  // Se não, quer dizer que o valor do fator será inserido diretamente
  else 
  {
    // Sinaliza ajuste direto
    auxTypeAdjust = _MANUAL_ADJUSTMENT;
  }
  
  // Ajusta o fator de calibração
  adjustCalFactor(auxAdjustCalFactor, auxTypeAdjust, _CONFIG_SOCKET);

}

//WWWWWWWWWW*********************************************************************************
// Função: remoteConfigReadAC
// Data  : 19/09/2019 14:49
//WWWWWWWWWW*********************************************************************************

void remoteConfigReadAC(JsonObject dataObject) 
{
  // Recebe o valor do tempo de envio da leitura AC
  configGeral.timeSendReadAC = dataObject[F("time")].as<int>();
  // Carrega o temporizador com o novo tempo
  wdtSendReadAC = configGeral.timeSendReadAC;
  // Armazena a configuração
  salvaConfigGeral(_CONFIG_SOCKET);
}

//WWWWWWWWWW*********************************************************************************
// Função: remoteConfigSendTemp
// Data  : 19/09/2019 14:49
//WWWWWWWWWW*********************************************************************************

void remoteConfigSendTemp(JsonObject dataObject) 
{
  // Recebe o valor do tempo de envio da temperatura
  configGeral.timeSendTemp = dataObject[F("time")].as<int>();
  // Carrega o temporizador com o novo tempo
  wdtSendTemp = configGeral.timeSendTemp;
  // Armazena a configuração
  salvaConfigGeral(_CONFIG_SOCKET);
}

//WWWWWWWWWW*********************************************************************************
// Função: remoteConfigPID
// Data  : 16/12/2019 12:10
//WWWWWWWWWW*********************************************************************************

void remoteConfigPID(JsonObject dataObject) 
{
  // Configura o PID
  configGeral.statusPID = dataObject[F("status")].as<int>();
  configGeral.kpPID = dataObject[F("kp")].as<float>();
  configGeral.kiPID = dataObject[F("ki")].as<float>();
  configGeral.kdPID = dataObject[F("kd")].as<float>();
  // Armazena a configuração
  salvaConfigGeral(_CONFIG_SOCKET);
}

//WWWWWWWWWW*********************************************************************************
// Função: respondeRemoteControl
// Data  : 03/01/2019 10:30
//WWWWWWWWWW*********************************************************************************

void responseRemoteControl(byte cmd) 
{
  // Informações sobre o dado (tipo do cabeçalho, tipo do dado e tamanho do dado)
  uint8_t auxInfoData[3] = {_HEADER_SET, _SET_RESPONSE, 1};
  // Número do comando
  String auxSendData = String(cmd, HEX);
  // Letra maiúscula
  auxSendData.toUpperCase();

  // Envia o dado ao APP
  sendDataAPP(&auxInfoData[0], &auxSendData);
}