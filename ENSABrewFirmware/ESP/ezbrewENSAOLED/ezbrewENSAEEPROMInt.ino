//*******************************************************************************************
// Arquivo C/C++ do Projeto Controlador de Patamares -- EZBrewControll
//-------------------------------------------------------------------------------------------
//
// Arquivo  : ezbrewENSAEEPROMInt.ino
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
// Funcao   : clearEeprom
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void clearEeprom() 
{
  startEEPROM();
  if (ERASE_EEPROM_USER) 
  {
    disableInterrupts(); // Disable interrupts
    for (register int i = RESET; i < END_CONFIG; i++)
      EEPROM.write(i, -1);
    EEPROM.commit();
    enableInterrupts(); // Enable interrupts
  }
  if (ERASE_EEPROM_CONFIG) {
    disableInterrupts(); // Disable interrupts
    for (register int i = END_CONFIG; i < EEPROM.length(); i++)
      EEPROM.write(i, -1);
    EEPROM.commit();
    enableInterrupts(); // Enable interrupts
  }
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : leConfigGeral
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void leConfigGeral() 
{
  startEEPROM();
  disableInterrupts(); // Disable interrupts
  EEPROM.get(END_CONFIG, configGeral);
  if (configGeral.qtdeMaxPatamar == 255) {
    configGeral.qtdeMaxPatamar = DEFAULT_MAX_PATAMAR;
    configGeral.grauPorMinuto = DEFAULT_GRAU_MINUTO;
    configGeral.pwmSetpoint = DEFAULT_PWM;
    configGeral.tempoEnvioBrassagem = 1;

    String idModule = DEFAULT_ID_MODULE;
    idModule.toCharArray(configGeral.idModule, 10);
    EEPROM.commit();
  }
  enableInterrupts(); // Enable interrupts
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : salvaConfigGeral
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void salvaConfigGeral() 
{
  startEEPROM();
  disableInterrupts(); // Disable interrupts
  EEPROM.put(END_CONFIG, configGeral);
  EEPROM.commit();
  enableInterrupts(); // Enable interrupts
  bipOK();
  printDisplay(F("Configuracao"), F("Salva c/ sucesso"), TEMPO_CONF);
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : limpaEeprom
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void limpaEeprom() 
{
  perguntaSimNao(F("--MENU CONFIG.--\nRestaurar padrao\n  de fabrica ?"));
  if (_endString) 
  {
    _endString = false;
    if (stResp == F("N")) {
      passoMaquina = mMENU_CONFIGURACAO;
      return;
    }
    else if (stResp == F("S")) 
    {
      bipAcerto();
      disableInterrupts(); // Disable interrupts
      passoAnterior = passoMaquina;
      oled.clear();
      oled.setCursor(0, LINHA_1);
      oled.println(F("-RESET DEFAULTS-\nAguarde..."));
      oled.print(F("Apagando..."));
      uint8_t auxColuna = 0;

      startEEPROM();
      for (register int i = RESET; i < EEPROM.length(); i++)
      {
        EEPROM.write(i, -1);
        if((i == (256*(auxColuna + 1))) || i == 4000)
        {
          oled.setCursor(auxColuna*8, LINHA_4);
          oled.print("*");
          auxColuna++;
        }
      }
      EEPROM.commit();
      enableInterrupts(); // Enable interrupts
      qtdeReceita = 0;
      totalDeReceitas = 0;
    }
    stResp = "_";
    passoMaquina = mSTANDBY;
    leConfigGeral();
  }
}


//WWWWWWWWWW*********************************************************************************
// Funcao   : remoteCleaningEeprom
// Data     : 03/01/2019 10:30
//WWWWWWWWWW*********************************************************************************

void remoteCleaningEeprom(int type, byte cmd) 
{
  passoMaquina = mLIMPA_EEPROM;
  enviaTela();
  startEEPROM();
  responseRemoteControl(type, cmd);
  bipAcerto();
  disableInterrupts(); // Disable interrupts
  passoAnterior = passoMaquina;
  oled.clear();
  oled.setCursor(0, LINHA_1);
  oled.println(F("-RESET DEFAULTS-\nAguarde..."));
  oled.print(F("Apagando..."));
      
  uint8_t auxColuna = 0;
  for (register int i = RESET; i < EEPROM.length(); i++)
  {
    EEPROM.write(i, -1);
    if((i == (256*(auxColuna + 1))) || i == 4000)
    {
      oled.setCursor(auxColuna*8, LINHA_4);
      oled.print("*");
      auxColuna++;
    }
  }
  EEPROM.commit();
  enableInterrupts(); // Enable interrupts
  leConfigGeral();
  qtdeReceita = 0;
  totalDeReceitas = 0;
  passoMaquina = mSTANDBY;
  wdt3000ms = 0;
  _msgConvite = true;
  _msgEzbrew = false;
}