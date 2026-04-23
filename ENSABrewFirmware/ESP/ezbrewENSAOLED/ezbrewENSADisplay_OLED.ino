//*******************************************************************************************
// Arquivo C/C++ do Projeto Controlador de Patamares -- EZBrewControll 
//-------------------------------------------------------------------------------------------
//
// Arquivo  : ezbrewENSAOLED.ino
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
// Funcao   : oledRestart
// Data     : 16/01/2019 10:06
//WWWWWWWWWW*********************************************************************************

void oledRestart(void )
{
  if(passoAnterior != passoMaquina)
  {
    wdtTimesDisplay = 0;
    _rewriteDisplay = true;
    oled.clear();
    oled.setCursor(0, 0);
  }
  passoAnterior = passoMaquina;
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : resetDisplay
// Data     : 18/01/2019 15:00
//WWWWWWWWWW*********************************************************************************

void resetDisplay(void )
{
  wdt3000ms = 0;
  _refreshDisplay = false;
  _rewriteDisplay = true;
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : printDisplay
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void printDisplay(String prMsgLinha1, String prMsgLinha2, unsigned long prTempoMsg) 
{
    /*--------------------------------+
    |   TRATAMENTO DAS MENSAGENS     |
    +--------------------------------*/
  passoAnterior = passoMaquina;
  oled.clear();

  if(prTempoMsg > 1)
  {
    wdtTimesDisplay = prTempoMsg;
    do 
    {
      oled.setCursor(0, LINHA_2);
      oled.println(prMsgLinha1);
      oled.println(prMsgLinha2);
      delay(0);
    } while (wdtTimesDisplay > 0); 
    oled.clear();
    _refreshDisplay = true;
  }
  else
  {
    oled.setCursor(0, LINHA_2);
    oled.println(prMsgLinha1);
    oled.println(prMsgLinha2);
  }
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : printClockDisplay
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void printClockDisplay(/*boolean initOrMenu,*/ uint8_t linha) 
{
  byte zero = 0x00;

  Wire.beginTransmission(ADDRESS_DS1307);
  Wire.write(zero);
  Wire.endTransmission();
  Wire.requestFrom(ADDRESS_DS1307, 7);

  String segundos = String(converteparaDecimal(Wire.read()));
  if (segundos.toInt() < 10)
    segundos = "0" + segundos;
  String minutos = String(converteparaDecimal(Wire.read()));
  if (minutos.toInt() < 10)
    minutos = "0" + minutos;
  String horas = String(converteparaDecimal(Wire.read() & 0b111111));
  if (horas.toInt() < 10)
    horas = "0" + horas;

  int diadasemana = converteparaDecimal(Wire.read());
  String diadasemanaString;
  switch (diadasemana) {
    case 1: diadasemanaString = F("Seg");
      break;
    case 2: diadasemanaString = F("Ter");
      break;
    case 3: diadasemanaString = F("Qua");
      break;
    case 4: diadasemanaString = F("Qui");
      break;
    case 5: diadasemanaString = F("Sex");
      break;
    case 6: diadasemanaString = F("Sab");
      break;
    case 7: diadasemanaString = F("Dom");
      break;
  }

  String diadomes = String(converteparaDecimal(Wire.read()));
  if (diadomes.toInt() < 10)
    diadomes = "0" + diadomes;
  String mes = String(converteparaDecimal(Wire.read()));
  if (mes.toInt() < 10)
    mes = "0" + mes;
  String ano = String(converteparaDecimal(Wire.read()));
  if (ano.toInt() < 10)
    ano = "0" + ano;

  String FormattedDate = "  " + diadomes + "/" + mes + "/" + ano + " " + diadasemanaString + "  ";
  String FormattedTime = "    " + horas + ":" + minutos + ":" + segundos + "    ";

  // if (initOrMenu) {

  //   wdtTimesDisplay = CLOCK_TIME;

  //   /* printDisplay(FormattedDate, FormattedTime, CLOCK_TIME); */
  //   oled.println(FormattedDate);
  //   oled.print(FormattedTime);

  //   do {
  //     oled.println();
  //   } while (wdtTimesDisplay > 0);

  // }
  // else 
  // {
    oled.setCursor(3, linha);
    oled.print(FormattedDate);
    oled.setCursor(5, linha+1);
    oled.print(FormattedTime);
  //}

}


// ===============================================================================( * FIM * )
// ===============================================================================( * FIM * )
// ===============================================================================( * FIM * )
// ===============================================================================( * FIM * )
// ================================(  MENSAGENS  )================================( * FIM * )
// ================================(     DO      )================================( * FIM * )
// ================================(   DISPLAY   )================================( * FIM * )
// ===============================================================================( * FIM * )
// ===============================================================================( * FIM * )
// ===============================================================================( * FIM * )
// ===============================================================================( * FIM * )

//WWWWWWWWWW*********************************************************************************
// Funcao   : printVersao
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void printVersao() 
{
  if (!_flag3000ms) 
  {
    bipAcerto();
    oled.setCursor(0, LINHA_2);
    oled.println(SISTEMA);
    oled.println(VERSAO);
  }
  else 
  {
    passoMaquina = mSTANDBY;
    return;
  }
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : printBoasVindas
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void printBoasVindas() 
{
  if (_msgConvite) 
  {
    _msgConvite = false;

    piscaLed(LED_AMARELO);

    oled.clear();
    oled.setCursor(0, LINHA_2);
    oled.println(F("Vamos Comecar a"));
    oled.println(F(" Fazer Cerveja? "));
    //printClockDisplay(3);
  }
  if (_msgEzbrew) 
  {
    _msgEzbrew = false;

    piscaLed(LED_VERMELHO1);

    oled.clear();
    oled.setCursor(0, LINHA_2);
    oled.println(F("EZBrew a Cerveja"));
    oled.println(F(" Feita por Voce "));
    //printClockDisplay(2);
  }
  desligaLeds();
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : printMenuGlobal
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void printMenuGlobal(uint8_t prTamanhoMax) 
{
  static uint8_t posicaoMenuAnterior = 1;

  if (posicaoMenu >= tamanhoMenu)
    posicaoMenu = 0;
  else if (posicaoMenu < 0)
    posicaoMenu = tamanhoMenu - 1;

  register uint8_t posFinal = prTamanhoMax;
  register uint8_t posInicial = posicaoMenu;

  if(prTamanhoMax <= 4)
  {
    posInicial = 0;
    posFinal = prTamanhoMax-1;
  }

  else
  {
    if(posInicial <= 3)
    {
      posInicial = 0;
      posFinal = 3;
    }

    else if (posInicial > 3) 
    {
      posFinal = posInicial;
      posInicial = posInicial - 3;
    }
  }

  if(posicaoMenuAnterior != posicaoMenu || passoAnterior != passoMaquina || _refreshDisplay)
  {
    _refreshDisplay = false;
    oled.clear();
    
    String buffer;
    register uint8_t linha = 0;
    for (register uint8_t ind = posInicial; ind <= posFinal; ind++) 
    {
      if(passoMaquina == mMENU)
        buffer += FPSTR(menu[ind]);
      
      else if(passoMaquina == mRECEITASEL)
        buffer += FPSTR(menuReceitaSel[ind]);
      
      else if(passoMaquina == mMENU_CONFIGURACAO)
        buffer += FPSTR(menuConfig[ind]);
      
      else if(passoMaquina == mSET_DATA_HORA)
        buffer += FPSTR(menuDataHora[ind]);

      else if(passoMaquina == mRECEITAS)
        buffer = menuReceita[ind];

      if (ind == posicaoMenu)
        buffer = ">" + buffer;
         
      oled.setCursor(0, linha);
      oled.print(buffer);
      linha = linha + 2;
      buffer = "";
    }
  }

  posicaoMenuAnterior = posicaoMenu;
  passoAnterior = passoMaquina;
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : printFluxo
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void printFluxo() 
{
  oledRestart();
  
  if(_refreshDisplay)
  {
    oled.clear();
    resetDisplay();
    oled.setCursor(0, LINHA_2);
    //oled.println(F("--EM BRASSAGEM--\n"));
    oled.println("Fluxo:  " + String(flow) + "L/min");
    oled.print("Litros: " + String(liters) + "L");
  }

  if ((liters > 0) && (alarmNivel > 0)) 
  {
    if (liters >= alarmNivel) 
      printDisplay(F(" Alarme VOLUME "),F(" Atingiu limite "),TEMPO_CONF);
  }
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : printNomeReceita
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void printNomeReceita() 
{
  oledRestart();
  stringDisplay = String(F("--NOVA RECEITA--\n")) +
                  String(F(" Digite um nome\n")) +
                  String(F(" para a receita"));
                  
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : printQtPatamar
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void printQtPatamar() 
{
  oledRestart();
  stringDisplay = String(F("--NOVA RECEITA--\n")) +
                  String(F("   Quantidade \n"))+
                  String(F("  de Patamares"));
}
//WWWWWWWWWW*********************************************************************************
// Funcao   : printTempPatamar
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void printTempPatamar() 
{
  oledRestart();
  stringDisplay = String(F("--NOVA RECEITA--\n")) +
                  String(F(" Temperatura do\n"))+
                  String(F("   Patamar: ")) + String(ind + 1);
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : printMinutoPatamar
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void printMinutoPatamar() 
{
  oledRestart();
  stringDisplay = String(F("--NOVA RECEITA--\n")) +
                  String(F("  Tempo (min.)\n"))+
                  String(F("   Patamar: ")) + String(ind + 1);
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : printTempFervura
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void printTempFervura() 
{
  oledRestart();
  stringDisplay = String(F("--NOVA RECEITA--\n")) +
                  String(F("   Temperatura\n"))+
                  String(F("   da Fervura "));
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : printMinutoFervura
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void printMinutoFervura() 
{
  oledRestart();
  stringDisplay = String(F("--NOVA RECEITA--\n")) +
                  String(F("  Tempo (min.) \n"))+
                  String(F("   da Fervura "));
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : printQtAdicao
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void printQtAdicao() 
{
  oledRestart();
  stringDisplay = String(F("--NOVA RECEITA--\n")) +
                  String(F("  Numero total\n"))+
                  String(F("   de Adicoes"));
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : printNomeAdicao
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void printNomeAdicao() 
{
  oledRestart();
  stringDisplay = String(F("--NOVA RECEITA--\n")) +
                  String(F(" Escolha o nome\n"))+
                  String(F("  da Adicao: ")) + String(ind + 1);
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : printGramasAdicao
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void printGramasAdicao() 
{
  oledRestart();
  stringDisplay = String(F("--NOVA RECEITA--\n")) +
                  String(F(" Quantidade (g)\n"))+
                  String(F("  da Adicao: ")) + String(ind + 1);
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : printMinutoAdicao
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void printMinutoAdicao() 
{
  oledRestart();
  stringDisplay = String(F("--NOVA RECEITA--\n")) +
                  String(F("  Tempo (min.)\n"))+
                  String(F("  da Adicao: ")) + String(ind + 1);
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : printQtWhirlpool
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void printQtWhirlpool() 
{
  oledRestart();
  stringDisplay = String(F("--NOVA RECEITA--\n")) +
                  String(F("  Numero total\n"))+
                  String(F("  de Whirpools"));
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : printNomeWhirlpool
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void printNomeWhirlpool() 
{
  oledRestart();
  stringDisplay = String(F("--NOVA RECEITA--\n")) +
                  String(F(" Escolha o nome\n"))+
                  String(F(" do Whirpool: ")) + String(ind + 1);
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : printMinutoWhirlpool
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void printMinutoWhirlpool() 
{
  oledRestart();
  stringDisplay = String(F("--NOVA RECEITA--\n")) +
                  String(F("  Tempo (min.)\n"))+
                  String(F(" do Whirpool: ")) + String(ind + 1);
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : printQtDescanso
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void printQtDescanso() 
{
  oledRestart();
  stringDisplay = String(F("--NOVA RECEITA--\n")) +
                  String(F("  Numero total\n"))+
                  String(F("  de Descansos"));
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : printNomeDescanso
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void printNomeDescanso() 
{
  oledRestart();
  stringDisplay = String(F("--NOVA RECEITA--\n")) +
                  String(F(" Escolha o nome\n"))+
                  String(F("  do Descanso"));
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : printMinutoDescanso
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void printMinutoDescanso() 
{
  oledRestart();
  stringDisplay = String(F("--NOVA RECEITA--\n")) +
                  String(F("  Tempo (min.)\n"))+
                  String(F("  do Descanso"));
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : printMenuMalte
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void printMenuMalte() 
{
  oledRestart();
  
  if(_refreshDisplay)
  {
    oled.clear();
    resetDisplay();
    oled.setCursor(0, LINHA_2);
    //oled.println(F("--EM BRASSAGEM--\n"));
    oled.println(F(" Vamos Derrubar "));
    oled.print(F("    o MALTE?   "));
  }
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : printMenuFervura
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void printMenuFervura() 
{
  oledRestart();
  
  if(_refreshDisplay)
  {
    oled.clear();
    resetDisplay();
    oled.setCursor(0, LINHA_2);
    //oled.println("--EM BRASSAGEM--\n");
    oled.println(F(" Start FERVURA? "));
    oled.print(F("Pressione ENTER"));
  }
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : printAdicao
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void printAdicao() 
{
   oledRestart();
  
  if(_refreshDisplay)
  {
    oled.clear();
    resetDisplay();
    oled.setCursor(0, LINHA_2);
    //oled.println(F("--EM BRASSAGEM--\n"));
    oled.println(F(" Colocar ADICAO "));
    oled.print(String((float)(adicaoFervura[numAdicao].qtde), 2) +
              "g - " + adicaoFervura[numAdicao].nome);
  }
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : printMenuWhirlpool
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void printMenuWhirlpool() 
{
  if(_refreshDisplay)
  {
    oled.clear();
    resetDisplay();
    oled.setCursor(0, LINHA_2);
    //oled.println(F("--EM BRASSAGEM--\n"));
    oled.println(F("Start WHIRLPOOL?"));
    oled.print(F("Pressione ENTER"));
  }
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : printMenuDescanso
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void printMenuDescanso() 
{
  if(_refreshDisplay)
  {
    oled.clear();
    resetDisplay();
    oled.setCursor(0, LINHA_2);
    //oled.println(F("--EM BRASSAGEM--\n"));
    oled.println(F(" Start DESCANSO? "));
    oled.print(F("Pressione ENTER"));
  }
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : printMenuDescanso
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void printConfigDataHora()
{
  if (opcMenuDataHora == 1) // Ajuste da HORA
  {
    stringDisplay = String(F("--MENU CONFIG.--\n")) +
                    String(F(" Ajuste da hora\n")) +
                    String(F(" do dispositivo\n"));
  }
      
  else if (opcMenuDataHora == 2) // Ajuste do MINUTO
  {
    stringDisplay = String(F("--MENU CONFIG.--\n")) +
                    String(F("Ajuste do minuto\n")) +
                    String(F(" do dispositivo\n"));
  }

  else if (opcMenuDataHora == 3) // Ajuste do SEGUNDO
  {
    stringDisplay = String(F("--MENU CONFIG.--\n")) +
                    String(F("Ajuste o segundo\n")) +
                    String(F(" do dispositivo\n"));
  }

  else if (opcMenuDataHora == 4) // Ajuste da HORA
  {
    stringDisplay = String(F("--MENU CONFIG.--\n")) +
                    String(F(" Ajuste da Data\n")) +
                    String(F("  Dia da Semana\n"));
  }
      
  else if (opcMenuDataHora == 5) // Ajuste do MINUTO
  {
    stringDisplay = String(F("--MENU CONFIG.--\n")) +
                    String(F(" Ajuste da Data\n")) +
                    String(F("   Dia do Mes\n"));
  }

  else if (opcMenuDataHora == 6) // Ajuste do SEGUNDO
  {
    stringDisplay = String(F("--MENU CONFIG.--\n")) +
                    String(F(" Ajuste da Data\n")) +
                    String(F("      Mes \n"));
  }

  else if (opcMenuDataHora == 7) // Ajuste do SEGUNDO
  {
    stringDisplay = String(F("--MENU CONFIG.--\n")) +
                    String(F(" Ajuste da Data\n")) +
                    String(F("      Ano\n"));
  }
}

