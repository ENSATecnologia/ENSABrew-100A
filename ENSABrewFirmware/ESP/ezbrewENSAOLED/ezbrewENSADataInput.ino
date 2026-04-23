//*******************************************************************************************
// Arquivo C/C++ do Projeto Controlador de Patamares -- EZBrewControll
//-------------------------------------------------------------------------------------------
// Arquivo  : ezbrewENSADataInput.ino
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
// Funcao   : entradaInteger
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void entradaInteger(String prTexto, String prDigitado) 
{
  if (!_digitarInteger) {
    _digitarInteger = true;
    stDigitado = prDigitado;
  }
  if (_keyPress) 
  {
    if (_KEY_SELECT) 
    {
      if (stChar != F("_"))
        stDigitado = stDigitado + stChar;
      if (stDigitado != "") {
        _digitarInteger = false;
        _endInteger = true;
        stChar = F("");
        numLetra = -1;
        resetDisplay();
        return;
      }
    }
    else if (_KEY_UP) 
    {
      numLetra++;
      if (numLetra >= MAX_NUMEROS)
        numLetra = 0;
      stChar = FPSTR(numeros[numLetra]);
    }
    else if (_KEY_DOWN) 
    {
      numLetra--;
      if (numLetra < 0)
        numLetra = MAX_NUMEROS - 1;
      stChar = FPSTR(numeros[numLetra]);
    }
    else if (_KEY_RIGHT) {
      numLetra = -1;
      if (stChar == F("_"))
        stChar = F(" ");
      if (stChar == F(" ")) {
        if (stDigitado == F(""))
          stChar = F("");
        else if (stDigitado.substring(stDigitado.length() - 1) == F(" "))
          stChar = F("");
      }
      stDigitado = stDigitado + stChar;
      stChar = F("_");
    }
    else if (_KEY_LEFT) {
      numLetra = -1;
      String st = stDigitado;
      st = st.substring(0, st.length() - 1);
      stDigitado = st;
      stChar = F("_");
    }
    else if (_KEY_ESC) {
      passoMaquina = mMENU;
      _digitarInteger = false;
      _endInteger = false;
      stChar = F("");
      stDigitado = F("");
      numLetra = -1;
    }
    
    resetDisplay();
  }
  if(_rewriteDisplay == true || _refreshDisplay)
  {
    _refreshDisplay = false;
    _rewriteDisplay = false;
    oled.clear();
    oled.setCursor(0, LINHA_1);
    oled.println(prTexto);
    oled.print(completaEspacos(stDigitado + stChar, 16));
  }
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : entradaString
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void entradaString(String prTexto, String prDigitado) 
{
  static uint8_t contCaracteres;

  if (!_digitarString) {
    _digitarString = true;
    stDigitado = prDigitado;
  }
  if (_keyPress) 
  {
    if (_KEY_SELECT) 
    {
      if (stChar != "_")
        stDigitado = stDigitado + stChar;
      if (stDigitado != "") {
        _digitarString = false;
        _endString = true;
        stChar = F("");
        numLetra = -1;
        contCaracteres = 0;
        resetDisplay();
        return;
      }
    }
    else if (_KEY_UP) 
    {
      numLetra++;
      if (numLetra >= MAX_ALFABETO)
        numLetra = 0;
      stChar = FPSTR(alfabeto[numLetra]);
    }
    else if (_KEY_DOWN) {
      numLetra--;
      if (numLetra < 0)
        numLetra = MAX_ALFABETO - 1;
      stChar = FPSTR(alfabeto[numLetra]);
    }
    else if (_KEY_RIGHT) 
    {
      contCaracteres++; 
      if(contCaracteres >= MAX_NOME)
      {
        stDigitado = stDigitado + stChar;
        stChar = F("_");

        _digitarString = false;
        _endString = true;
        stChar = F("");
        numLetra = -1;
        contCaracteres = 0;
        resetDisplay();
        return;
      }

      numLetra = -1;
      if (stChar == F("_"))
        stChar = F(" ");
      if (stChar == F(" ")) 
      {
        if (stDigitado == F(""))
          stChar = F("");
        else if (stDigitado.substring(stDigitado.length() - 1) == F(" "))
          stChar = F("");
      }

      stDigitado = stDigitado + stChar;
      stChar = F("_");
    }
    else if (_KEY_LEFT) 
    {
      contCaracteres--;
      numLetra = -1;
      String st = stDigitado;
      st = st.substring(0, st.length() - 1);
      stDigitado = st;
      stChar = F("_");
    }
    else if (_KEY_ESC) {
      passoMaquina = mMENU;
      stChar = F("_");
      stDigitado = F("");
      numLetra = -1;
      _endString = false;
      _digitarString = false;
      contCaracteres = 0;
    }
    
    resetDisplay();
  }

  if(_rewriteDisplay == true || _refreshDisplay)
  {
    _refreshDisplay = false;
    _rewriteDisplay = false;
    oled.clear();
    oled.setCursor(0, LINHA_1);
    oled.println(prTexto);
    oled.print("-> " + completaEspacos(stDigitado + stChar, 16));
  }
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : entradaValorGeral
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void entradaValorGeral(String prTexto, float prValorDefault, int prValorMin, int prValorMax, boolean usarDec) 
{
  if (!_digitarValue) {
    _digitarValue = true;
    valorDigitado = prValorDefault;
  }
  if (_keyPress) 
  {
    if (_KEY_SELECT) 
    {
      _endValue = true;
      _digitarValue = false;
      resetDisplay();
      return;
    }
    else if (_KEY_UP) 
    {
      if (usarDec)
        valorDigitado = valorDigitado + 0.1;
      else
        valorDigitado = valorDigitado + 1;

      if(valorDigitado > prValorMax)// Alteração - Gabriel
        valorDigitado = prValorMin;
    }
    else if (_KEY_DOWN) 
    {
      if (usarDec)
        valorDigitado = valorDigitado - 0.1;
      else
        valorDigitado = valorDigitado - 1;

      if(valorDigitado < prValorMin)// Alteração - Gabriel
        valorDigitado = prValorMax;
    }
    else if (_KEY_ESC) 
    {
      passoMaquina = mMENU;
      _endValue = false;
      _digitarValue = false;
      _flagAjusteDataHora = true;
      opcMenuDataHora = 0;
    }

    resetDisplay();
  }

  if (valorDigitado > prValorMax)
    valorDigitado = prValorMax;
  else if (valorDigitado < prValorMin)
    valorDigitado = prValorMin;

  String st = getValorInt(valorDigitado);
  if (usarDec)
    st = getValor1Dec(valorDigitado);

  if(_rewriteDisplay == true || _refreshDisplay)
  {
    _refreshDisplay = false;
    _rewriteDisplay = false;
    oled.clear();
    oled.setCursor(0, LINHA_1);
    switch(passoMaquina)
    {
      case mSET_MAX_PATAMAR:
      case mSET_GRAU_MINUTO:
      case mSET_PWM_SETPOINT:
        oled.println(F("--MENU CONFIG.--"));
      break;
      default:
        break;
    }
    oled.println(prTexto);

    if(passoMaquina == mSET_DATA_HORA)
    {
      if(opcMenuDataHora == 4)
      {
        if(valorDigitado == 1)
          st = F("Seg");
        else if(valorDigitado == 2)
          st = F("Ter");
        else if(valorDigitado == 3)
          st = F("Qua");
        else if(valorDigitado == 4)
          st = F("Qui");
        else if(valorDigitado == 5)
          st = F("Sex");
        else if(valorDigitado == 6)
          st = F("Sab");
        else if(valorDigitado == 7)
          st = F("Dom");
      }
    }
    
    oled.print("De " + String(prValorMin) + " a " + String(prValorMax) + ": " + st);
  }
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : entradaTempoManual
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void entradaTempoManual(String prTexto, int prValorDefault, int prValorMin, int prValorMax) {
  valorDigitado = prValorDefault;
  if (_KEY_UP)
    valorDigitado = valorDigitado + 1;
  else if (_KEY_DOWN)
    valorDigitado = valorDigitado - 1;
  else if (_KEY_NONE)
    valorDigitado = prValorDefault;
  if (valorDigitado >= prValorMax)
    valorDigitado = prValorMax;
  else if (valorDigitado <= prValorMin)
    valorDigitado = prValorMin;
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : entradaPwmManual
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void entradaPwmManual(String prTexto, float prValorDefault, int prValorMin, int prValorMax) {
  valorDigitado = prValorDefault;
  if (_KEY_UP)
    valorDigitado = valorDigitado + 5;
  else if (_KEY_DOWN)
    valorDigitado = valorDigitado - 5;
  else if (_KEY_NONE)
    valorDigitado = prValorDefault;
  if (valorDigitado >= prValorMax)
    valorDigitado = prValorMax;
  else if (valorDigitado <= prValorMin)
    valorDigitado = prValorMin;
}
