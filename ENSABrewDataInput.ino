//*******************************************************************************************
// Arquivo C/C++ do Projeto Controlador de Patamares -- ENSABrewControll
//-------------------------------------------------------------------------------------------
//
// Arquivo: ENSABrewDataInput.ino
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
// Função: entradaInteger
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void entradaInteger(String prTexto, String prDigitado)
{
  if (!_digitarInteger)
  {
    _digitarInteger = true;
    stDigitado = prDigitado;
  }
  if (_keyPress)
  {
    if (_KEY_SELECT)
    {
      if (stChar != F("_"))
        stDigitado = stDigitado + stChar;
      if (stDigitado != "")
      {
        _digitarInteger = false;
        _endInteger = true;
        stChar = F("");
        numLetra = -1;
        
        entradaAtual = stDigitado;
        if(_connectedAPP)
          enviaTela(true);
        entradaAtual = F("Void");
        return;
      }
    }
    else if (_KEY_UP)
    {
      numLetra++;
      if (numLetra >= MAX_NUMEROS)
        numLetra = 0;
      stChar = (char *)pgm_read_word(&numeros[numLetra]);
    }
    else if (_KEY_DOWN)
    {
      numLetra--;
      if (numLetra < 0)
        numLetra = MAX_NUMEROS - 1;
      stChar = (char *)pgm_read_word(&numeros[numLetra]);
    }
    else if (_KEY_RIGHT) 
    {
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
      numLetra = -1;
      String st = stDigitado;
      st = st.substring(0, st.length() - 1);
      stDigitado = st;
      stChar = F("_");
    }
  }
}

//WWWWWWWWWW*********************************************************************************
// Função: entradaString
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void entradaString(String prDigitado)
{
  if (!_digitarString)
  {
    _digitarString = true;
    stDigitado = prDigitado;
  }
  if (_keyPress)
  {
    if (_KEY_SELECT)
    {
      if (stChar != "_")
        stDigitado = stDigitado + stChar;
      if (stDigitado != "")
      {
        _digitarString = false;
        _endString = true;
        stChar = F("");
        numLetra = -1;
        
        entradaAtual = stDigitado;
        if(_connectedAPP)
          enviaTela(true);
        entradaAtual = F("Void");
        return;
      }
    }
    else if (_KEY_UP)
    {
      numLetra++;
      if (numLetra >= MAX_ALFABETO)
        numLetra = 0;
      stChar = (char *)pgm_read_word(&alfabeto[numLetra]);
    }
    else if (_KEY_DOWN)
    {
      numLetra--;
      if (numLetra < 0)
        numLetra = MAX_ALFABETO - 1;
      stChar = (char *)pgm_read_word(&alfabeto[numLetra]);
    }
    else if (_KEY_RIGHT)
    {
      numLetra = -1;
      if (stChar == F("_"))
        stChar = F(" ");
      stDigitado = stDigitado + stChar;
      stChar = F("_");
    }
    else if (_KEY_LEFT) 
    {
      numLetra = -1;
      String st = stDigitado;
      
      if(stChar != F("_"))
        st = st.substring(0, st.length());
      else st = st.substring(0, st.length() - 1);

      stDigitado = st;
      stChar = F("_");
    }
  }
}

//WWWWWWWWWW*********************************************************************************
// Função: entradaValorGeral
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void entradaValorGeral(float prValorDefault, int prValorMin, int prValorMax, boolean usarDec)
{
  if (!_digitarValue)
  {
    _digitarValue = true;
    valorDigitado = prValorDefault;

    // Correção dos valores em máximos e mínimos
    if(valorDigitado > prValorMax)
      valorDigitado = prValorMax;
    else if(valorDigitado < prValorMin)
      valorDigitado = prValorMin;
  }
  if (_keyPress)
  {
    if (_KEY_SELECT)
    {
      _endValue = true;
      _digitarValue = false;
      
      entradaAtual = String(valorDigitado);
      if(_connectedAPP)
        enviaTela(true);
      entradaAtual = F("Void");
    }
    else if (_KEY_UP)
    {
      if (usarDec)
        valorDigitado = valorDigitado + 0.1;
      else
        valorDigitado = valorDigitado + 1;

      if(valorDigitado > prValorMax)
        valorDigitado = prValorMin;
    }
    else if (_KEY_DOWN)
    {
      if (usarDec)
        valorDigitado = valorDigitado - 0.1;
      else
        valorDigitado = valorDigitado - 1;

      if(valorDigitado < prValorMin)
        valorDigitado = prValorMax;
    }
  }

  stDigitado = getValorInt(valorDigitado);
  if (usarDec)
    stDigitado = getValor1Dec(valorDigitado);
}

//WWWWWWWWWW*********************************************************************************
// Função: entradaTempoManual
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void entradaTempoManual(String prTexto, int prValorDefault, int prValorMin, int prValorMax)
{
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
// Função: entradaSetPointManual
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void entradaSetPointManual(String prTexto, int prValorDefault, int prValorMin, int prValorMax)
{
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
// Função: entradaPwmManual
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void entradaPwmManual(String prTexto, float prValorDefault, int prValorMin, int prValorMax)
{
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

//WWWWWWWWWW*********************************************************************************
// Função: resetDataInput
// Data  : 26/02/2020
//WWWWWWWWWW*********************************************************************************

void resetDataInput(boolean auxEndValue)
{
  _endString = false; _digitarString = false; 
  _endInteger = false; _digitarInteger = false; 
  _endValue = auxEndValue; _digitarValue = false;
  stChar = F("_"); stDigitado = F(""); numLetra = -1;
}