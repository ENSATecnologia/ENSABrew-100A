//*******************************************************************************************
// Arquivo C/C++ do Projeto Controlador de Patamares -- ENSABrewControll
//-------------------------------------------------------------------------------------------
//
// Arquivo: ENSABrewClock.ino
//
//
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
      switch (diadasemana) 
      {
        case 1: diadasemanaString = F("Seg"); break;
        case 2: diadasemanaString = F("Ter"); break;
        case 3: diadasemanaString = F("Qua"); break;
        case 4: diadasemanaString = F("Qui"); break;
        case 5: diadasemanaString = F("Sex"); break;
        case 6: diadasemanaString = F("Sab"); break;
        case 7: diadasemanaString = F("Dom"); break;
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
      
      #if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)
      
        lcdClear();
        lcdPrint(1, 3, diadomes + "/" + mes + "/" + ano + " " + diadasemanaString);
        lcdPrint(2, 5, horas + ":" + minutos + ":" + segundos);
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

void setClock(String parametro, byte valor) 
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
  switch (diadasemana) 
  {
    case 1: diadasemanaString = F("Seg"); break;
    case 2: diadasemanaString = F("Ter"); break;
    case 3: diadasemanaString = F("Qua"); break;
    case 4: diadasemanaString = F("Qui"); break;
    case 5: diadasemanaString = F("Sex"); break;
    case 6: diadasemanaString = F("Sab"); break;
    case 7: diadasemanaString = F("Dom"); break;
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

  if(parametro == F("hrs"))
    setDateClock(diadomes.toInt(), mes.toInt(), ano.toInt(), valor, diadasemana, minutos.toInt(), segundos.toInt()); 
  else if(parametro == F("min"))
    setDateClock(diadomes.toInt(), mes.toInt(), ano.toInt(), horas.toInt(), diadasemana, valor, segundos.toInt()); 
  else if(parametro == F("sec"))
    setDateClock(diadomes.toInt(), mes.toInt(), ano.toInt(), horas.toInt(), diadasemana, minutos.toInt(), valor); 
  else if(parametro == F("diaMes"))
    setDateClock(valor, mes.toInt(), ano.toInt(), horas.toInt(), diadasemana, minutos.toInt(), segundos.toInt()); 
  else if(parametro == F("diaSem"))
    setDateClock(diadomes.toInt(), mes.toInt(), ano.toInt(), horas.toInt(), valor, minutos.toInt(), segundos.toInt()); 
  else if(parametro == F("mes"))
    setDateClock(diadomes.toInt(), valor, ano.toInt(), horas.toInt(), diadasemana, minutos.toInt(), segundos.toInt()); 
  else if(parametro == F("ano"))
    setDateClock(diadomes.toInt(), mes.toInt(), valor, horas.toInt(), diadasemana, minutos.toInt(), segundos.toInt()); 

}

//WWWWWWWWWW*********************************************************************************
// Função: getClock
// Data  : 20/11/2018 14:20
//WWWWWWWWWW*********************************************************************************

byte getClock(String parametro) 
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
  switch (diadasemana) 
  {
    case 1: diadasemanaString = F("Seg"); break;
    case 2: diadasemanaString = F("Ter"); break;
    case 3: diadasemanaString = F("Qua"); break;
    case 4: diadasemanaString = F("Qui"); break;
    case 5: diadasemanaString = F("Sex"); break;
    case 6: diadasemanaString = F("Sab"); break;
    case 7: diadasemanaString = F("Dom"); break;
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

       if(parametro == F("hrs"))    return(horas.toInt());
  else if(parametro == F("min"))    return(minutos.toInt());
  else if(parametro == F("sec"))    return(segundos.toInt());
  else if(parametro == F("diaMes")) return(diadomes.toInt());
  else if(parametro == F("diaSem")) return(diadasemana);
  else if(parametro == F("mes"))    return(mes.toInt());
  else if(parametro == F("ano"))    return(ano.toInt());

}

#endif