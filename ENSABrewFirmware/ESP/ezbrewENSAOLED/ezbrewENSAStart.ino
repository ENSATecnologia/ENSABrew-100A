//*******************************************************************************************
// Arquivo C/C++ do Projeto Controlador de Patamares -- EZBrewControll
//-------------------------------------------------------------------------------------------
//
// Arquivo  : ezbrewENSAStart.ino
// Inicio   : 29/04/2013 10:23
// Revisão  : 11/05/2016 18:31
//
// AutorI   : Jânio Anselmo, Eng. Me.
// AutorII  : Thiago Anselmo, Tecgº.
// AutorIIII : Gabriel da Silva Caetano, Estágio.
//
// CPU:     : ESP8266 - WEMOS D1 MINI Pro
// Clock:   : 16MHz
//*******************************************************************************************


//WWWWWWWWWW*********************************************************************************
// Funcao   : startSerial
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void startSerial() {

  Serial.begin(SERIAL_BAUD_RATE); //Para debug
  while (!Serial) {
    ;
  }

  /*Serial1.begin(SERIAL_BAUD_RATE); //Para bluetooth
  while (!Serial1) {
    ;
  }*/
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : startI2c
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void startI2c()
{
   Wire.begin(); // ajustaDataHoraManual();
   Wire.setClock(CLOCK_EEPROM);  // Let's go fast! Set I2C clock to 400kHz
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : startOled
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void startOled() {

  oled.begin(&Adafruit128x64, I2C_ADDRESS);
  oled.setFont(ZevvPeep8x16);
  oled.set1X();
  // oled.setFont(Adafruit5x7); // Font DEFAULT

}


//WWWWWWWWWW*********************************************************************************
// Funcao   : startEEPROM
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void startEEPROM() {
  EEPROM.begin(MAX_BYTES_EEPROM);
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : startAds1115
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void startAds1115() {

  //Serial.println(F("Getting single-ended readings from AIN0...3"));
  //Serial.println(F("ADC Range: +/- 6.144V (1 bit = 3mV/ADS1015, 0.1875mV/ADS1115)"));

  /*

    The ADC input range (or gain) can be changed via the following
    functions, but be careful never to exceed VDD +0.3V max, or to
    exceed the upper and lower limits if you adjust the input range!
    Setting these values incorrectly may destroy your ADC!
                                                                  ADS1015  ADS1115
                                                                  -------  -------
  */

  // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  // ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV


  ads.setGain(GAIN_ONE);           // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  ads.begin();

}

//WWWWWWWWWW*********************************************************************************
// Funcao   : startPorts
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void startPorts() 
{

  /*pinMode(ETH_CS, OUTPUT);
  pinMode(SD_CS, OUTPUT);

  digitalWrite(ETH_CS, LOW);
  digitalWrite(SD_CS, HIGH);*/

  /*pinMode(PINO_TIRE_BUTTON, INPUT_PULLUP);
  pinMode(PINO_CLEANER_BUTTON, INPUT_PULLUP);
  pinMode(PINO_BUTTON, INPUT_PULLUP);*/

  pinMode(PINO_SSR, OUTPUT);
  digitalWrite(PINO_SSR, LOW);

  pinMode(PINO_RELAY1, OUTPUT);
  digitalWrite(PINO_RELAY1, LOW);

  pinMode(LED_AMARELO, OUTPUT);
  digitalWrite(LED_AMARELO, LOW);

  pinMode(LED_VERMELHO1, OUTPUT);
  digitalWrite(LED_VERMELHO1, LOW);

  pinMode(LED_VERMELHO2, OUTPUT);
  digitalWrite(LED_VERMELHO2, LOW);

  pinMode(PINO_FLUXO, INPUT);

}

//WWWWWWWWWW*********************************************************************************
// Funcao   : defineTimers
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void defineTimers() 
{
  /*Timer1.initialize(TIMER1);
  Timer1.attachInterrupt(globalInterruption);

  Timer3.initialize(TIMER3);

  Timer4.initialize(TIMER4);
  Timer4.attachInterrupt(buzzerInterruption);*/



  /*
    attachInterrupt(digitalPinToInterrupt(PINO_AC), acInterruption, RISING);
    attachInterrupt(digitalPinToInterrupt(PINO_FLUXO), callPulse, RISING);
  */

}

//WWWWWWWWWW*********************************************************************************
// Funcao   : startTimers
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void startTimers() 
{
  TimerGlobal.attach(TIME_GLOBAL, globalInterruption);
  TimerBuzzer.attach(TIME_BUZZER, buzzerInterruption);

  _sistemaUp = true;
}