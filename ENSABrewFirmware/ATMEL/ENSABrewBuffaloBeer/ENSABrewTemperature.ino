//*******************************************************************************************
// Arquivo C/C++ do Projeto Controlador de Patamares -- ENSABrewControll
//-------------------------------------------------------------------------------------------
//
// Arquivo: ENSABrewFTemperature.ino
//
// Autor  : Cláudio B. Pimenta, A.S
// Autor  : Jânio Anselmo, Eng. Me.
// Autor  : Thiago Anselmo, Tecgº
// Autor  : Gabriel da Silva Caetano
//
// CPU    : ATMega2561 - MegaCore
// Clock  : 16MHz
//
//*******************************************************************************************

//WWWWWWWWWW*********************************************************************************
// Funcão: controlTemperatureTasks
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void controlTemperatureTasks(void ) 
{
  // -------------------------------------------------------------------------------- //
  // Desabilita as interrupções

  disableInterrupts();

  // -------------------------------------------------------------------------------- //
  // Armazena a média de temperatura

  static uint16_t actualInd = 0;

  // -------------------------------------------------------------------------------- //
  // Rotina de leitura 

  if (_readTemperature) 
  {
    // Reseta a flag de leitura
    _readTemperature = false;

    // Se atingiu o máximo de amostras, reseta o índice
    if(actualInd >= configGeral.sampleTemperature)
      actualInd = 0;

    // Realiza a leitura, passando-a para inteiro
    tempFilter[actualInd] = thermistor.getTemp()*100;

    // Incrementa o índice de leitura
    actualInd++;

    // Se for a primeira leitura ou uma reconfiguração 
    // preenche o buffer com o valor atual obtido
    if(mediaTemperature == 0)
    {
      // Preenche o buffer com a leitura
      for(register uint16_t j = 1; j < configGeral.sampleTemperature; j++)
        tempFilter[j] = tempFilter[0];
    }

    // Reseta a média de temperatura
    mediaTemperature = 0;
    // Somatório das amostras (passando para float)
    for(register uint16_t i = 0; i < configGeral.sampleTemperature; i++)
      mediaTemperature += (tempFilter[i] / 100.0);

    // Realiza a média
    mediaTemperature /= configGeral.sampleTemperature;

    // Acrescenta o valor do OffSet
    mediaTemperature += CONV_OFFSET_TEMP_FLOAT(configGeral.offSetTemp);
  }

  // -------------------------------------------------------------------------------- //
  // Habilita as interrupções

  enableInterrupts(); return;
  
  // -------------------------------------------------------------------------------- //
  // Rotina de controle - PID

  // Se o PID estiver habilitado
  // if(configGeral.statusPID == _ENABLE)
  //   controlPIDTasks();

  // -------------------------------------------------------------------------------- //
  // Rotina de envio

  // Se o envio da temperatura estiver habilitado e se já está liberado, envia
  // if (configGeral.timeSendTemp != 0 && wdtSendTemp == 0) 
  // {
  //   // Recarrega o temporizador
  //   switch (passoMaquina) 
  //   {

  //     /*
  //       Se estiver em brassagem (em processo de alcance de  
  //       temperatura), carrega o tempo para envio em brassagem
  //     */
  //     case mLOOP_BRASSAGEM:
        
  //       wdtSendTemp = configGeral.tempoEnvioBrassagem;
      
  //     break;

  //     // Se não, recarrega o temporizador
  //     default: wdtSendTemp = configGeral.timeSendTemp; break;
  //   }

  //   // Se já atingiu a temperatura, também não envia a mesma
  //   if(_contaTempo) return;

  //   // Se está conectado ao APP
  //   if(_connectedAPP)
  //   {
  //     // mediaTemperature = 25;
  //     // Informações sobre o dado (tipo do cabeçalho, tipo do dado e tamanho do dado)
  //     uint8_t auxInfoData[3] = {_HEADER_EVENT, _TEMPERATURE, 1};
  //     // Dado em si, com o número do comando
  //     String auxSendData[2] = {F("02"), String((float)(mediaTemperature), 2)};
  //     // Envia o dado ao APP
  //     sendDataAPP(&auxInfoData[0], &auxSendData[0]);
  //   }
  // }

  // -------------------------------------------------------------------------------- //
}