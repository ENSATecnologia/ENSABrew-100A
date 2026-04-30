//*******************************************************************************************
// Arquivo C/C++ do Projeto Controlador de Patamares -- ENSABrewControll
//-------------------------------------------------------------------------------------------
//
// Arquivo: ENSABrewClock.ino
//
// Autor  : Cláudio B. Pimenta, A.S
// Autor  : Jânio Anselmo, Eng. Me.
// Autor  : Thiago Anselmo, Tecgº
// Autor  : Gabriel da Silva Caetano
//
// CPU    : ATMega2561 - MegaCore
// Clock  : 16MHz
//
//*****************************************************************************************

// Se o uso das funções do relógio estiver habilitada, gera e utiliza as funções
#ifdef FUNCTIONS_CLOCK

//WWWWWWWWWW*********************************************************************************
// Função: startClock
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void startClock(int prTempoMsg)
{
  byte zero = 0x00;

  wdtTimesDisplay = prTempoMsg;
  do {
    if (_keyPress) 
    {
      if (_KEY_SELECT) 
      {
        break;
      }
    }
    if (_flag1000ms) 
    {
      _flag1000ms = false;

      Wire.beginTransmission(ADDRESS_DS1307);
      Wire.write(zero);
      Wire.endTransmission();
      Wire.requestFrom(ADDRESS_DS1307, 7);

      char segundos[3];
      char minutos[3];
      char horas[3];
      snprintf(segundos, sizeof(segundos), "%02u", converteparaDecimal(Wire.read()));
      snprintf(minutos, sizeof(minutos), "%02u", converteparaDecimal(Wire.read()));
      snprintf(horas, sizeof(horas), "%02u", converteparaDecimal(Wire.read() & 0b111111));
      int diadasemana = converteparaDecimal(Wire.read());
      const char* diadasemanaString = "";
      switch (diadasemana)
      {
        case 1: diadasemanaString = "Seg"; break;
        case 2: diadasemanaString = "Ter"; break;
        case 3: diadasemanaString = "Qua"; break;
        case 4: diadasemanaString = "Qui"; break;
        case 5: diadasemanaString = "Sex"; break;
        case 6: diadasemanaString = "Sab"; break;
        case 7: diadasemanaString = "Dom"; break;
      }
      char diadomes[3];
      char mes[3];
      char ano[3];
      snprintf(diadomes, sizeof(diadomes), "%02u", converteparaDecimal(Wire.read()));
      snprintf(mes, sizeof(mes), "%02u", converteparaDecimal(Wire.read()));
      snprintf(ano, sizeof(ano), "%02u", converteparaDecimal(Wire.read()));

      #if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)

        lcdClear();
        char dataAtual[17];
        char horaAtual[17];
        snprintf(dataAtual, sizeof(dataAtual), "%s/%s/%s %s", diadomes, mes, ano, diadasemanaString);
        snprintf(horaAtual, sizeof(horaAtual), "%s:%s:%s", horas, minutos, segundos);
        lcdPrint(1, 3, dataAtual);
        lcdPrint(2, 5, horaAtual);
        lcdWrite();

      #endif
    }
  } while (wdtTimesDisplay > 0);
}

//WWWWWWWWWW*********************************************************************************
// Função: setDateClock - Set date and clock - DS1307
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void setDateClock(byte diadomes, byte mes, byte ano, byte horas, byte diadasemana, byte minutos, byte segundos) 
{
  byte zero = 0x00;
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

  bipAcerto();
  printDisplay(F("Configuracao"), F("Salva, Sucesso!"), TEMPO_CONF);
}

//WWWWWWWWWW*********************************************************************************
// Função: setClock
// Data  : 20/11/2018 14:20
//WWWWWWWWWW*********************************************************************************

void setClock(const String& parametro, byte valor)
{
  byte zero = 0x00;
  Wire.beginTransmission(ADDRESS_DS1307);
  Wire.write(zero);
  Wire.endTransmission();
  Wire.requestFrom(ADDRESS_DS1307, 7);

  char segundos[3];
  char minutos[3];
  char horas[3];
  snprintf(segundos, sizeof(segundos), "%02u", converteparaDecimal(Wire.read()));
  snprintf(minutos, sizeof(minutos), "%02u", converteparaDecimal(Wire.read()));
  snprintf(horas, sizeof(horas), "%02u", converteparaDecimal(Wire.read() & 0b111111));

  int diadasemana = converteparaDecimal(Wire.read());
  const char* diadasemanaString = "";
  switch (diadasemana)
  {
    case 1: diadasemanaString = "Seg"; break;
    case 2: diadasemanaString = "Ter"; break;
    case 3: diadasemanaString = "Qua"; break;
    case 4: diadasemanaString = "Qui"; break;
    case 5: diadasemanaString = "Sex"; break;
    case 6: diadasemanaString = "Sab"; break;
    case 7: diadasemanaString = "Dom"; break;
  }

  char diadomes[3];
  char mes[3];
  char ano[3];
  snprintf(diadomes, sizeof(diadomes), "%02u", converteparaDecimal(Wire.read()));
  snprintf(mes, sizeof(mes), "%02u", converteparaDecimal(Wire.read()));
  snprintf(ano, sizeof(ano), "%02u", converteparaDecimal(Wire.read()));

  if(parametro == F("hrs"))
    setDateClock(atoi(diadomes), atoi(mes), atoi(ano), valor, diadasemana, atoi(minutos), atoi(segundos));
  else if(parametro == F("min"))
    setDateClock(atoi(diadomes), atoi(mes), atoi(ano), atoi(horas), diadasemana, valor, atoi(segundos));
  else if(parametro == F("sec"))
    setDateClock(atoi(diadomes), atoi(mes), atoi(ano), atoi(horas), diadasemana, atoi(minutos), valor);
  else if(parametro == F("diaMes"))
    setDateClock(valor, atoi(mes), atoi(ano), atoi(horas), diadasemana, atoi(minutos), atoi(segundos));
  else if(parametro == F("diaSem"))
    setDateClock(atoi(diadomes), atoi(mes), atoi(ano), atoi(horas), valor, atoi(minutos), atoi(segundos));
  else if(parametro == F("mes"))
    setDateClock(atoi(diadomes), valor, atoi(ano), atoi(horas), diadasemana, atoi(minutos), atoi(segundos));
  else if(parametro == F("ano"))
    setDateClock(atoi(diadomes), atoi(mes), valor, atoi(horas), diadasemana, atoi(minutos), atoi(segundos));

}

//WWWWWWWWWW*********************************************************************************
// Função: getClock
// Data  : 20/11/2018 14:20
//WWWWWWWWWW*********************************************************************************

byte getClock(const String& parametro)
{
  byte zero = 0x00;
  Wire.beginTransmission(ADDRESS_DS1307);
  Wire.write(zero);
  Wire.endTransmission();
  Wire.requestFrom(ADDRESS_DS1307, 7);

  char segundos[3];
  char minutos[3];
  char horas[3];
  snprintf(segundos, sizeof(segundos), "%02u", converteparaDecimal(Wire.read()));
  snprintf(minutos, sizeof(minutos), "%02u", converteparaDecimal(Wire.read()));
  snprintf(horas, sizeof(horas), "%02u", converteparaDecimal(Wire.read() & 0b111111));

  int diadasemana = converteparaDecimal(Wire.read());
  const char* diadasemanaString = "";
  switch (diadasemana)
  {
    case 1: diadasemanaString = "Seg"; break;
    case 2: diadasemanaString = "Ter"; break;
    case 3: diadasemanaString = "Qua"; break;
    case 4: diadasemanaString = "Qui"; break;
    case 5: diadasemanaString = "Sex"; break;
    case 6: diadasemanaString = "Sab"; break;
    case 7: diadasemanaString = "Dom"; break;
  }

  char diadomes[3];
  char mes[3];
  char ano[3];
  snprintf(diadomes, sizeof(diadomes), "%02u", converteparaDecimal(Wire.read()));
  snprintf(mes, sizeof(mes), "%02u", converteparaDecimal(Wire.read()));
  snprintf(ano, sizeof(ano), "%02u", converteparaDecimal(Wire.read()));

       if(parametro == F("hrs"))    return(atoi(horas));
  else if(parametro == F("min"))    return(atoi(minutos));
  else if(parametro == F("sec"))    return(atoi(segundos));
  else if(parametro == F("diaMes")) return(atoi(diadomes));
  else if(parametro == F("diaSem")) return(diadasemana);
  else if(parametro == F("mes"))    return(atoi(mes));
  else if(parametro == F("ano"))    return(atoi(ano));

}

#endif
