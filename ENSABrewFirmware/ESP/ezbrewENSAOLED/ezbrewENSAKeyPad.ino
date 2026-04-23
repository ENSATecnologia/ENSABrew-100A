//*******************************************************************************************
// Arquivo C/C++ do Projeto Controlador de Patamares -- EZBrewControll
//-------------------------------------------------------------------------------------------
// Arquivo  : ezbrewENSAKeyPad.ino
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
// Funcao   : keyPad - https://arduino-info.wikispaces.com/LCD-Pushbuttons
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void keyPadInterruption() 
{

  /*--------------------------------+
    |     TESTE DO TECLADO I         |
    +--------------------------------*/

  if (!_atendeTeclado)
    return;
  _atendeTeclado = false;

  if (_keyPress)
    return;

  boolean _defineADC = false; // false = adc1, true = adc2
  uint16_t adc1, adc2;
  adc1 = ads.readADC_SingleEnded(2);
  adc2 = ads.readADC_SingleEnded(3);

  if(adc1 < 26100)
  {
    _defineADC = false;
    ADTeclado += adc1;
    countADTeclado++;
  }

  else if(adc2 < 26100) 
  {
    _defineADC = true;
    ADTeclado += adc2;
    countADTeclado++;
  }
  
  else 
  {
    ADTecladoAux = 26200;
    countADTeclado = 0;
  }

  if (countADTeclado >= AMOSTRAS_TECLADO) 
  {
    countADTeclado = 0;
    ADTecladoAux = ADTeclado;
    ADTecladoAux /= AMOSTRAS_TECLADO;
    ADTeclado = 0;
    
    //Serial.println("ADTecladoAux: " + String(ADTecladoAux));
  }
  if (ADTecladoAux > MAX_AD_TECLADO) 
  {
    if (!_keyTask) 
    {
      bounceUP++;
      if (bounceUP >= BOUNCE_UP) 
      {
        bounceUP = 0;
        _keyTask = true;
      }
    }
    else bounceUP = 0;
    bounceADTeclado = RESET;
  }
  else 
  {
    bounceUP = 0;
    if (_keyTask) 
    {
      if (CopyADTecladoAux == ADTecladoAux) 
      {
        bounceADTeclado++;
        if (bounceADTeclado >= BOUNCE_AD) 
        {
          bounceADTeclado = 0;

          // ------------------------------------------------------------------------------------------
          // VALORES DO "AD" DO MICROCONTROLADOR - ATMEGA 2561
          // ------------------------------------------------------------------------------------------

          if (ADTecladoAux >= 0 && ADTecladoAux < 4000)//500)
          {
            if(!_defineADC)      // adc1
              _KEY_UP = true;
            else                 // adc2
              _KEY_SELECT = true;
          }
          else if (ADTecladoAux >= 4800 && ADTecladoAux < 9000)//5200) 
          {
             if(!_defineADC)    // adc1
              _KEY_RIGHT = true;
            else                // adc2
              _KEY_DOWN = true;
          }
          else if (ADTecladoAux >= 10800 && ADTecladoAux < 18000)//11400)
          { 
             if(!_defineADC)    // adc1
              _KEY_ESC = true;
            else                // adc2
              _KEY_LEFT = true;
          }
          
          _keyTask = false;
          _keyPress = true;
        }
      }
      else
        bounceADTeclado = 0;
      CopyADTecladoAux = ADTecladoAux;
    }
  }
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : virtualKeyPad
// Data     : 03/01/2019 09:30
//WWWWWWWWWW*********************************************************************************

void virtualKeyPad(JsonObject dataObject) 
{
  _keyPress = true;

  String vKey = dataObject[F("vKey")].as<String>();

  if(vKey == V_KEY_UP)
    _KEY_UP = true;
  else if (vKey == V_KEY_DOWN)
    _KEY_DOWN = true;
  else if (vKey == V_KEY_RIGHT)
    _KEY_RIGHT = true;
  else if (vKey == V_KEY_LEFT)
    _KEY_LEFT = true;
  else if (vKey == V_KEY_SELECT)
    _KEY_SELECT = true;
  else if (vKey == V_KEY_ESC)    
    _KEY_ESC = true;
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : clearKeyPad
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void clearKeyPad() {
  _keyPress = false;
  _KEY_RIGHT = false;
  _KEY_UP = false;
  _KEY_DOWN = false;
  _KEY_LEFT = false;
  _KEY_SELECT = false;
  _KEY_ESC = false;
  _KEY_NONE = false;
}