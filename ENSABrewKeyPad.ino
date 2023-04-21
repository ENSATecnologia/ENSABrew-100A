//*******************************************************************************************
// Arquivo C/C++ do Projeto Controlador de Patamares -- ENSABrewControll
//-------------------------------------------------------------------------------------------
//
// Arquivo: ENSABrewKeyPad.ino
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
// Função: keyPad - https://arduino-info.wikispaces.com/LCD-Pushbuttons
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void keyPad(void ) 
{

  /*--------------------------------+
    |     TECLADO ANALÓGICO AD0     |
    +--------------------------------*/

  if (!_atendeTeclado) return;
  _atendeTeclado = false;
  
  if (_keyPress) return;

  disableInterrupts();

  // Faz a leitura do AD
  uint16_t auxReadADCTeclado = analogRead(PINO_TECLADO);
  
  ADTeclado += auxReadADCTeclado;
  countADTeclado++;
  if (countADTeclado >= AMOSTRAS_TECLADO) 
  {
    countADTeclado = 0;
    ADTecladoAux = ADTeclado;
    ADTecladoAux /= AMOSTRAS_TECLADO;
    ADTeclado = 0;
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

               if (ADTecladoAux >= 0   && ADTecladoAux < 50 ) _KEY_ESC    = true;
          else if (ADTecladoAux >= 100 && ADTecladoAux < 200) _KEY_RIGHT  = true;
          else if (ADTecladoAux >= 300 && ADTecladoAux < 400) _KEY_UP     = true;
          else if (ADTecladoAux >= 450 && ADTecladoAux < 550) _KEY_LEFT   = true;
          else if (ADTecladoAux >= 700 && ADTecladoAux < 800) _KEY_SELECT = true;
          else if (ADTecladoAux >= 885 && ADTecladoAux < 940) _KEY_DOWN   = true;  

          // Se for UP ou DOWN, inicializa os parâmetros para incremento
          if(_KEY_UP || _KEY_DOWN) 
          {
            // Flag de verificação de incremento limpa
            _flagCheckIncrement = false; 
            // Flag para temporizar a verificação do incremento habilitada
            _flagWdtIncrement = true; 
            // Inicializando tempo de incremento
            timeWdtIncrement = INIT_TIME_INCREMENT; 
            // Inicializando temporizador de incremento
            wdtIncrement = timeWdtIncrement;
          }

          _keyTask = false; _keyPress = true;

          // Se estiver usando o display OLED, atualiza a escrita ao se pressionar um botão
          #if(DISPLAY_TYPE == DISPLAY_OLED)
            resetDisplay();
          #endif
        }
      }
      else
        bounceADTeclado = 0;

      CopyADTecladoAux = ADTecladoAux;
    }
  }

  enableInterrupts();
}

//WWWWWWWWWW*********************************************************************************
// Função: virtualKeyPad
// Data  : 03/01/2019 09:30
//WWWWWWWWWW*********************************************************************************

void virtualKeyPad(JsonObject dataObject) 
{
  _keyPress = true;

  String vKey = dataObject[F("vKey")].as<String>();

       if (vKey == V_KEY_UP)     _KEY_UP = true;
  else if (vKey == V_KEY_DOWN)   _KEY_DOWN = true;
  else if (vKey == V_KEY_RIGHT)  _KEY_RIGHT = true;
  else if (vKey == V_KEY_LEFT)   _KEY_LEFT = true;
  else if (vKey == V_KEY_SELECT) _KEY_SELECT = true;
  else if (vKey == V_KEY_ESC)    _KEY_ESC = true;
}

//WWWWWWWWWW*********************************************************************************
// Função: clearKeyPad
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void clearKeyPad(void ) 
{
  _keyPress = false;
  _KEY_RIGHT = false;
  _KEY_UP = false;
  _KEY_DOWN = false;
  _KEY_LEFT = false;
  _KEY_SELECT = false;
  _KEY_ESC = false;
  _KEY_NONE = false;
}

//WWWWWWWWWW*********************************************************************************
// Função: CheckIncrement
// Data  : 30/07/2019 08:57
//WWWWWWWWWW*********************************************************************************

void checkIncrement(void ) 
{
  // Limpa a flag de verificação de incremento
  _flagCheckIncrement = false;

  // Faz o leitura do AD do teclado para ver se UP ou DOWN estão pressionados
  uint16_t auxReadADC = analogRead(PINO_TECLADO);

  // Faixa de leitura para UP
  if (auxReadADC >= 300 && auxReadADC < 400) 
    _KEY_UP = true;

  // Faixa de leitura para DOWN
  else if (auxReadADC >= 885 && auxReadADC < 940) 
    _KEY_DOWN = true;

  // Se pressionou algum dos botões
  if(_KEY_UP || _KEY_DOWN)
  {
    // Sinaliza botão pressionado
    _keyPress = true;

    // Vai diminuindo o tempo de verificação de incremento, 
    // para poder aumentar a velocidade de incremento
    // Tempo definido por: timeWdtIncrement*50ms
    if(timeWdtIncrement == INIT_TIME_INCREMENT)
      timeWdtIncrement = 11;
    else if(timeWdtIncrement == 11)
      timeWdtIncrement = 7;
    else if(timeWdtIncrement == 7)
      timeWdtIncrement = 5;
    else if(timeWdtIncrement == 5)
      timeWdtIncrement = 4;
    else if(timeWdtIncrement == 4)
      timeWdtIncrement = 3;
    else if(timeWdtIncrement == 3)
      timeWdtIncrement = 2;
  }
  // Caso nenhum estiver pressionado, limpa o temporizador
  else 
  { 
    timeWdtIncrement = INIT_TIME_INCREMENT;
    wdtIncrement = timeWdtIncrement;
  }
}

//WWWWWWWWWW*********************************************************************************
// Função: clearIncrement
// Data  : 19/09/2019 09:37
//WWWWWWWWWW*********************************************************************************

void clearIncrement(void ) 
{
  // Evita incremento contínuo
  _flagWdtIncrement = false;
  _flagCheckIncrement = false;
  wdtIncrement = 0;
}