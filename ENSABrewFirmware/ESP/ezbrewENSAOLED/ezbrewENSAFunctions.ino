//*******************************************************************************************
// Arquivo C/C++ do Projeto Controlador de Patamares -- EZBrewControll
//-------------------------------------------------------------------------------------------
// Arquivo  : ezbrewENSAFunctions.ino
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
// Funcao   : executaBoasVindas
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void executaBoasVindas() 
{
  if (_keyPress) 
  {
    if (_KEY_SELECT) 
    {
      passoMaquina = mMENU;
      return;
    }
  }
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : executaMenuInicial
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void executaMenuInicial() 
{
  desligaLeds();
  uint8_t opcMenu = executaMenu(4);
  if (_analisaOpcMenu) 
  {
    passoAnterior = passoMaquina;
    _analisaOpcMenu = false;
    if (opcMenu == 0) 
    {
      if (qtdeReceita > 0)
        passoMaquina = mRECEITAS;

      else 
      {
        bipAtencao();
        printDisplay(F("Nenhuma receita "), F("  Cadastrada  "), TEMPO_ALARME);
        passoMaquina = mMENU;
      }
    }
    else if (opcMenu == 1)
    {
      prOper = F("I");
      passoMaquina = mCRIAR_RECEITA;
    }
    else if (opcMenu == 2)
      passoMaquina = mADICIONAR_AGUA;
    else if (opcMenu == 3) 
      passoMaquina = mSENHA_CONFIGURACAO;
  }
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : executaMenu
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

uint8_t executaMenu(uint8_t prTamanhoMax) 
{
  tamanhoMenu = prTamanhoMax;
  if (_keyPress) 
  {
    if (_KEY_SELECT) 
    {
      _analisaOpcMenu = true;
      uint8_t opcaoMenu = posicaoMenu;
      posicaoMenu = 0;
      return opcaoMenu;
    }
    else if (_KEY_DOWN)
      posicaoMenu = posicaoMenu + 1;
    else if (_KEY_UP)
      posicaoMenu = posicaoMenu - 1;

    wdt3000ms = 0;
    _refreshDisplay = false;
    _rewriteDisplay = true;
  }
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : completaEspacos
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

String completaEspacos(String prStr, int prTamMax) {
  for (register int ind = prStr.length(); ind <= prTamMax; ind++) {
    prStr = prStr + ' ';
  } return prStr;
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : perguntaSimNao
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void perguntaSimNao(String prTexto) 
{
  oledRestart();
  if (_keyPress) 
  {
    if (_KEY_SELECT) 
    {
      if (stResp != F("_")) 
      {
        _endString = true;
        return;
      }
    }
    else if (_KEY_UP)
      stResp = F("S");
    else if (_KEY_DOWN)
      stResp = F("N");
  
    resetDisplay();
  }

  if(_rewriteDisplay == true || _refreshDisplay)
  {
    _refreshDisplay = false;
    _rewriteDisplay = false;
    oled.clear();
    oled.setCursor(0, LINHA_1);
    oled.println(prTexto);
    oled.println("S=Sim / N=Nao: " + stResp); 
  }
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : acInterruption - Interruption TIMER1 - 1ms
// Data     : 09/09/2016 09:55
// Revisão  :
//WWWWWWWWWW*********************************************************************************

void acInterruption() {
  pulseAC++;
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : callPulse - Interruption INT SENSOR FLOW
// Data     : 09/09/2016 09:55
// Revisão  :
//WWWWWWWWWW*********************************************************************************

void callPulse() {
  Npulse++;
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : calculaFluxo - Calcula o fluxo e volume de entrada
// Data     : 09/09/2016 09:55
// Revisão  :
//WWWWWWWWWW*********************************************************************************

void calculaFluxo(int alarmNivel) {

  if (_keyPress) {
    if (_KEY_SELECT) {
      digitalWrite(PINO_RELAY1, LOW);
      passoMaquina = mMENU;
      return;
    }
  }
  if (_flag1000ms) {
    _flag1000ms = false;
    detachInterrupt(digitalPinToInterrupt(PINO_FLUXO));
    flow = (Npulse / calFactor);
    milli = (flow / 60);
    liters = liters + milli;
    Npulse = 0;
    attachInterrupt(digitalPinToInterrupt(PINO_FLUXO), callPulse, RISING);
    if ((liters > 0) && (alarmNivel > 0)) {
      if (liters >= alarmNivel) {
        digitalWrite(PINO_RELAY1, LOW);
        if (_keyPress) {
          if (_KEY_SELECT) {
            passoMaquina = mMENU;
            return;
          }
        }
        if (_flag3000ms)
          bipAlarme();
      }
    }
  }
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : contandoTempo
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

boolean contandoTempo() {
  if (tempoInicial == 0)
    return false;
  else
    return true;
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : respondeBrassagemRemota
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void respondeBrassagemRemota() {
  // "typeCmd = FF"
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : calculaSpeed
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void calculaSpeed(int prTemperatura) {
  if (tempInicialAquecimento > - 1000 )
    speedAquecimento = (prTemperatura - tempInicialAquecimento);
  else
    speedAquecimento = 0;
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : calculaGrauPorMinuto
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void calculaGrauPorMinuto(float prTemperaturaFloat) {
  int tempo = 0;
  if (tempoInicialVelocidade == 0) {
    temperaturaFloatInicial = prTemperaturaFloat;
    tempo++;
  }
  if (tempo >= 60) { //Segundos
    grauPorMinuto = temperaturaFloatInicial - prTemperaturaFloat;
    if (grauPorMinuto < 0)
      grauPorMinuto = 0;
    tempoInicialVelocidade = 0;
  }
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : enviaTela
// Data     : 13/02/19 11:30
//WWWWWWWWWW*********************************************************************************

void enviaTela() 
{
  String telaAtual = F("void");

  switch(passoMaquina)
  {
    case mSTANDBY:
      telaAtual = F("Stand By");
      break;
      
    case mMENU:
      telaAtual = F("Menu Principal");
      break;

    case mRECEITASEL:
      telaAtual = F("Menu da receita");
      break;
    
    case mMENU_CONFIGURACAO:
      telaAtual = F("Menu Configuracao");
      break;
    
    case mSET_DATA_HORA:
      telaAtual = F("Configurar data e hora");
      break;
    
    case mRECEITAS:
      telaAtual = F("Menu Receitas");
      break;

    case mNOME_RECEITA:
      if(prOper == F("I"))
        telaAtual = F("Criar receita");
      else if(prOper == F("E"))
        telaAtual = F("Editar receitas");
      break;
    
    case mLIMPA_EEPROM:
      telaAtual = F("Reset Defaults");
    break;
      
    case mADICIONAR_AGUA:
      telaAtual = F("Adicionar Agua");
      break;

    default:
      break;
  }

  if(telaAtual != F("void"))
  {
    const size_t bufferSize = JSON_OBJECT_SIZE(7);
    DynamicJsonDocument jsonSendBuffer(bufferSize);
    JsonObject enviaTela = jsonSendBuffer.to<JsonObject>();

    enviaTela[F("header")] = F("4E");
    enviaTela[F("product")] = F("0002");
    enviaTela[F("type")] = F("D002");
    enviaTela[F("id")] = String(configGeral.idModule);
    enviaTela[F("tela")] = telaAtual;

    serializeJsonPretty(enviaTela, Serial1);
    enviaTela.clear();
  }
}


//----------------------------------------------------------

// void interruptTireButton()
// {
//   int readingTireButton = digitalRead(PINO_TIRE_BUTTON);

//   if (readingTireButton != lastTireButtonState)
//   {
//     lastDebounceTimeTireButton = millis();
//   }
//   if ((millis() - lastDebounceTimeTireButton) > debounceDelay)
//   {
//     if (readingTireButton != tireButtonState) 
//     {
//       tireButtonState = readingTireButton;

//       if (tireButtonState == LOW/*HIGH*/) 
//       {
//         _KEY_UP = true;
//         _keyPress = true;
//       }
//     }
//   }
//   lastTireButtonState = readingTireButton;
// }

// //WWWWWWWWWW*********************************************************************************
// // Funcao   : interruptCleanerButton
// // Data     : 28/01/19 09:00
// //WWWWWWWWWW*********************************************************************************

// void interruptCleanerButton()
// {
//   int readingCleanerButton = digitalRead(PINO_CLEANER_BUTTON);

//   if (readingCleanerButton != lastCleanerButtonState)
//   {
//     lastDebounceTimeCleanerButton = millis();
//   }
//   if ((millis() - lastDebounceTimeCleanerButton) > debounceDelay)
//   {
//         if (readingCleanerButton != cleanerButtonState) 
//     {
//       cleanerButtonState = readingCleanerButton;

//       if (cleanerButtonState == LOW/*HIGH*/) 
//       {
//         _KEY_SELECT = true;
//         _keyPress = true;
//       }
//     }
//   }
//   lastCleanerButtonState = readingCleanerButton;
// }

// //WWWWWWWWWW*********************************************************************************
// // Funcao   : interruptButton
// // Data     : 04/04/2017 13:45
// //WWWWWWWWWW*********************************************************************************

// void interruptButton() {

//   /*--------------------------------+
//     |  BUTTON HOLE - DEBOUNCING     |
//     +--------------------------------*/

//   int readingButton = digitalRead(PINO_BUTTON);

//   if (readingButton != lastButtonState) 
//   {
//     lastDebounceTimeButton = millis();
//   }
//   if ((millis() - lastDebounceTimeButton) > debounceDelay)
//   {
//     if (readingButton != buttonState)
//     {
//       buttonState = readingButton;
//       if (buttonState == LOW/*HIGH*/)
//       {
//         _KEY_DOWN = true;
//         _keyPress = true;
//       }
//     }
//   }

//   lastButtonState = readingButton;
// }