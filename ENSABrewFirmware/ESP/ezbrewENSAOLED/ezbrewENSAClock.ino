//*******************************************************************************************
// Arquivo C/C++ do Projeto Controlador de Patamares -- EZBrewControll
//-------------------------------------------------------------------------------------------
// Arquivo  : ezbrewENSAClock.ino
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
//*****************************************************************************************


//WWWWWWWWWW*********************************************************************************
// Funcao   : startClock
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void startClock(int prTempoMsg) 
{
  byte zero = 0x00;

  wdtTimesDisplay = prTempoMsg;
  do 
  {
    delay(0);
    if (_keyPress) 
    {
      if (_KEY_SELECT) 
      {
        break;
      }
    }
    if (_flag1000ms) {
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
      
      oledRestart();
      oled.setCursor(1, 3);
      oled.print(diadomes + "/" + mes + "/" + ano + " " + diadasemanaString);
      oled.setCursor(2, 5);
      oled.print(horas + ":" + minutos + ":" + segundos);
    }
  } while (wdtTimesDisplay > 0);
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : setDateClock - Set date and clock - DS1307
// Data     : 04/04/2017 13:45
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
  printDisplay(F("Configuracao"), F("Salva c/ sucesso"), TEMPO_CONF);
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : setClock
// Data     : 20/11/2018 14:20
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
// Funcao   : getClock
// Data     : 20/11/2018 14:20
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

  if(parametro == F("hrs"))
    return(horas.toInt());
  else if(parametro == F("min"))
    return(minutos.toInt());
  else if(parametro == F("sec"))
    return(segundos.toInt());
  else if(parametro == F("diaMes"))
    return(diadomes.toInt());
  else if(parametro == F("diaSem"))
    return(diadasemana);
  else if(parametro == F("mes"))
    return(mes.toInt());
  else if(parametro == F("ano"))
    return(ano.toInt());
}