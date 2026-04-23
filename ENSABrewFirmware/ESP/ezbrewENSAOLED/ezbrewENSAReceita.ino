//*******************************************************************************************
// Arquivo C/C++ do Projeto Controlador de Patamares -- EZBrewControll
//-------------------------------------------------------------------------------------------
// Arquivo  : ezbrewENSAReceita.ino
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
// Funcao   : criaReceita
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void criaReceita() {
  prOper = F("I");
  Receita receita;
  carregaReceitas();
  mantemReceita(receita);
  totalDeReceitas++;
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : editaReceita
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void editaReceita(int prIdReceita) {
  prOper = F("E");
  mantemReceita(regReceita[prIdReceita]);
  carregaReceitas();
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : apagaReceita
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void apagaReceita(int prIdReceita) 
{
  Receita receita = regReceita[prIdReceita];
  perguntaSimNao(F("\nApagar receita?")/*String(receita.nome)*/);
  if (_endString) 
  {
    _endString = false;
    if (stResp == F("N")) 
    {
      passoMaquina = mMENU;
      return;
    }
    else if (stResp == F("S")) 
    {
      disableInterrupts(); // Disable interrupts
      
      for (register int ind = receita.codigo; ind < qtdeReceita; ind++) 
      {
        regReceita[ind] = regReceita[ind + 1];
        
        Receita receita = regReceita[ind];
        receita.codigo = receita.codigo - 1;
        
        EEPROM.put((ind * sizeof(receita)), receita);
        EEPROM.commit();
      }
      EEPROM.put(((qtdeReceita - 1) * sizeof(receita)), -1);
      EEPROM.commit();
      enableInterrupts(); // Enable interrupts

      qtdeReceita--;
      totalDeReceitas--;
      
      enableInterrupts(); // Enable interrupts
      if (qtdeReceita <= 0)
        qtdeReceita = 0;
      bipAcerto();
      printDisplay(F("Receita Apagada"), F("  Com Sucesso!  "), TEMPO_CONF);
      carregaReceitas();
    }
    stResp = "_";
    passoMaquina = mMENU;
  }
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : executaMenuReceitaSel
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void executaMenuReceitaSel() 
{
  
  uint8_t opcMenu = executaMenu(4);
  if (_analisaOpcMenu) {
    _analisaOpcMenu = false;
    if (opcMenu == 0) //Voltar
      passoMaquina = mRECEITAS;
    else if (opcMenu == 1) //Produção
      passoMaquina = mCONTINUA_PRODUCAO;
    else if (opcMenu == 2) //Editar
      editaReceita(idReceitaSel);
    else if (opcMenu == 3) //Apagar
      passoMaquina = mAPAGA_RECEITA;
  }
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : executaMenuReceitas
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void executaMenuReceitas() 
{
  int opcMenu = executaMenu(qtdeReceita + 1);
  if (_analisaOpcMenu) {
    _analisaOpcMenu = false;
    if (opcMenu == 0)
      passoMaquina = mMENU;
    else {
      idReceitaSel = opcMenu - 1;
      receitaSel = regReceita[idReceitaSel];
      passoMaquina = mRECEITASEL;
    }
  }
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : carregaReceitas
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void carregaReceitas() 
{
  menuReceita[0] = F("Voltar");
  qtdeReceita = 0;
  disableInterrupts(); // Disable interrupts

  if(totalDeReceitas == 0)
  {
    for (register int ind = 0; ind < MAX_RECEITA; ind++) 
    {
      Receita receita;
      int ender = (ind * sizeof(receita));
      EEPROM.get(ender, receita);

      if(receita.codigo < 0)
        break;
      else 
        totalDeReceitas++;
    }
  }

  for (register int ind = 0; ind < MAX_RECEITA; ind++) 
  {
    Receita receita;
    int ender = (ind * sizeof(receita));
    EEPROM.get(ender, receita);
    
    if (receita.codigo < 0)
      break;

    qtdeReceita = qtdeReceita + 1;
    if (qtdeReceita >= MAX_RECEITA)
      qtdeReceita = MAX_RECEITA;

    menuReceita[ind + 1] = String(receita.codigo + 1) + "." + String(receita.nome);
    regReceita[ind] = receita;
  }
  enableInterrupts(); // Enable interrupts
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : mantemReceita
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void mantemReceita(Receita mantemReceita) {
  if ((qtdeReceita >= MAX_RECEITA) && (prOper == F("I"))) {
    bipAtencao();
    printDisplay(F(" Memoria cheia! "), F(" Atingiu limite "), TEMPO_ALARME);
    passoMaquina = mMENU;
    return;
  }
  if (prOper == F("I")) {
    Receita newReceita;
    prReceita = newReceita;
  }
  else if (prOper == F("E"))
    prReceita = mantemReceita;
  passoMaquina = mNOME_RECEITA;
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : salvaReceita
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void salvaReceita(Receita prReceita) {
  disableInterrupts(); // Disable interrupts
  int num = prReceita.codigo;
  int ender = (num * sizeof(prReceita));
  if (ender >= END_CONFIG)
    ender = RESET;
  EEPROM.put(ender, prReceita);
  EEPROM.commit();
  enableInterrupts(); // Enable interrupts
  carregaReceitas();
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : adicionarAgua
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void adicionarAgua() {
  entradaInteger(F("Volume em [L]"), F("30"));
  if (_endInteger) {
    _endInteger = false;
    volumeDigitado = stDigitado.toInt();
    digitalWrite(PINO_RELAY1, HIGH);
    passoMaquina = mCALCULA_FLUXO;
  }
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : criaEditaReceitaRemota
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void criaEditaReceitaRemota(int type, byte cmd, JsonObject dataObject, String prOper) {
  carregaReceitas();
  
  if ((qtdeReceita >= MAX_RECEITA) && (prOper == F("I"))) {
    qtdeReceita = MAX_RECEITA;
    passoMaquina = mSTANDBY;
    code = F("E10"); // Memory full
  }
  else if ((qtdeReceita <= RESET) && (prOper == F("E"))) {
    qtdeReceita = RESET;
    passoMaquina = mSTANDBY;
    code = F("E11"); // Empty Memory
  }
  
  else 
  {
    JsonArray nomeReceitaArray = dataObject[F("receita")];
    int idxReceita = nomeReceitaArray[RESET].as<int>();
    if (prOper == F("E"))
      prReceita = regReceita[idxReceita - 1];
    nomeReceita = nomeReceitaArray[RESET + 1].as<String>();
    nomeReceita.toCharArray(prReceita.nome, MAX_NOME);

    boolean nomeExistente = false;
    for (register int ind = 0; ind < MAX_RECEITA; ind++) 
    {
      Receita receita;
      int ender = (ind * sizeof(receita));
      EEPROM.get(ender, receita);
    
      if (receita.codigo < 0)
          break;

      if(strcmp(prReceita.nome, receita.nome) == 0)
      {
        nomeExistente = true;
        break;  
      }
    }

    if(nomeExistente)
    {
      passoMaquina = mSTANDBY;
      code = F("E13"); // Nome já existente 
      setJsonData(type, cmd);
      return;
    }

    JsonArray tempPatamar = dataObject[F("tempPatamar")];
    qtdePatamar = tempPatamar.size();
    prReceita.qtdePatamar = qtdePatamar;
    JsonArray minPatamar = dataObject[F("minPatamar")];
    for (register int ind = 0; ind < qtdePatamar; ind++) {
      prReceita.patamar[ind].temperaturaMax = tempPatamar[ind].as<int>();
      prReceita.patamar[ind].tempo = minPatamar[ind].as<int>();
    }

    tempFervura = dataObject[F("tempFervura")].as<int>();
    prReceita.tempFervura = tempFervura;
    qtdeMinFervura = dataObject[F("minFervura")].as<int>();
    prReceita.qtdeMinFervura = qtdeMinFervura;

    JsonArray nomeAdicaoArray = dataObject[F("nomeAdicao")];
    qtdeAdicao = nomeAdicaoArray.size();
    prReceita.qtdeAdicao = qtdeAdicao;
    JsonArray gramasAdicao = dataObject[F("gramasAdicao")];
    JsonArray minAdicao = dataObject[F("minAdicao")];
    for (register int ind = 0; ind < qtdeAdicao; ind++) {
      nomeAdicao = nomeAdicaoArray[ind].as<String>();
      nomeAdicao.toCharArray(prReceita.adicao[ind].nome, MAX_NOME);
      prReceita.adicao[ind].qtde = gramasAdicao[ind].as<int>();
      prReceita.adicao[ind].tempo = minAdicao[ind].as<int>();
    }

    JsonArray nomeWhirlpoolArray = dataObject[F("nomeWhirlpool")];
    qtdeWhirlpool = nomeWhirlpoolArray.size();
    prReceita.qtdeWhirlpool = qtdeWhirlpool;
    JsonArray minWhirlpool = dataObject[F("minWhirlpool")];
    for (register int ind = 0; ind < qtdeWhirlpool; ind++) 
    {
      nomeWhirlpool = nomeWhirlpoolArray[ind].as<String>();
      nomeWhirlpool.toCharArray(prReceita.whirlpool[ind].nome, MAX_NOME);
      prReceita.whirlpool[ind].tempo = minWhirlpool[ind].as<int>();
    }

    nomeDescanso = F("DESCANSO");
    qtdeDescanso = 1;
    int minDescanso = dataObject[F("minDescanso")].as<int>();
    nomeDescanso.toCharArray(prReceita.descanso[RESET].nome, MAX_NOME);
    prReceita.descanso[RESET].tempo = minDescanso;

    if (prOper == F("I")) {
      prReceita.codigo = qtdeReceita;
      qtdeReceita = qtdeReceita + 1;
      totalDeReceitas++;
    }
    regReceita[prReceita.codigo] = prReceita;
    passoMaquina = mSALVA_RECEITA;

    code = F("A1"); // Success!
  }
  setJsonData(type, cmd);
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : respondeReceitaRemota
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void respondeReceitaRemota(int type, byte cmd, JsonObject dataObject) 
{
  JsonArray idxReceitaArray = dataObject[F("receita")];
  int idxReceita = idxReceitaArray[RESET].as<int>();
  prReceita = regReceita[idxReceita - 1];
  
  if(type == 0xFF && (idxReceita > totalDeReceitas || idxReceita == 0))
  {
    qtdeReceita = totalDeReceitas;
    passoMaquina = mSTANDBY;
    code = F("E12");
    setJsonData(type, cmd);
  }

  else
  {
    carregaReceitas();

    if (qtdeReceita <= RESET) 
    {
      qtdeReceita = RESET;
      passoMaquina = mSTANDBY;
      code = F("E11"); // Empty Memory
    }
    else
    {
      /*JsonArray idxReceitaArray = dataObject[F("receita")];
      int idxReceita = idxReceitaArray[RESET].as<int>();
      prReceita = regReceita[idxReceita - 1];*/

      String typeCommandUpper = String(type, HEX);
      typeCommandUpper.toUpperCase();
      String commandUpper = String(cmd, HEX);
      commandUpper.toUpperCase();

      const size_t bufferSize = JSON_OBJECT_SIZE(7);
      DynamicJsonDocument jsonSendBuffer(bufferSize);
      JsonObject dataReceita = jsonSendBuffer.to<JsonObject>();

      dataReceita[F("header")] = F("4E");
      dataReceita[F("product")] = F("0002");
      dataReceita[F("type")] = F("D002");
      dataReceita[F("id")] = String(configGeral.idModule);
      dataReceita[F("typeCmd")] = typeCommandUpper;
      dataReceita[F("cmd")] = commandUpper;

      JsonArray nomeReceitaArray = dataReceita.createNestedArray(F("receita"));
      nomeReceitaArray.add(prReceita.codigo + 1);
      nomeReceitaArray.add(prReceita.nome);

      JsonArray tempPatamarArray = dataReceita.createNestedArray(F("tempPatamar"));
      for (register int ind = 0; ind < prReceita.qtdePatamar; ind++)
        tempPatamarArray.add(prReceita.patamar[ind].temperaturaMax);

      JsonArray minPatamarArray = dataReceita.createNestedArray(F("minPatamar"));
      for (register int ind = 0; ind < prReceita.qtdePatamar; ind++)
        minPatamarArray.add(prReceita.patamar[ind].tempo);

      dataReceita[F("tempFervura")] = prReceita.tempFervura;
      dataReceita[F("minFervura")] = prReceita.qtdeMinFervura;

      JsonArray nomeAdicaoArray = dataReceita.createNestedArray(F("nomeAdicao"));
      for (register int ind = 0; ind < prReceita.qtdeAdicao; ind++)
        nomeAdicaoArray.add(prReceita.adicao[ind].nome);

      JsonArray gramasAdicaoArray = dataReceita.createNestedArray(F("gramasAdicao"));
      for (register int ind = 0; ind < prReceita.qtdeAdicao; ind++)
        gramasAdicaoArray.add(prReceita.adicao[ind].qtde);

      JsonArray minAdicaoArray = dataReceita.createNestedArray(F("minAdicao"));
      for (register int ind = 0; ind < prReceita.qtdeAdicao; ind++)
        minAdicaoArray.add(prReceita.adicao[ind].tempo);

      JsonArray nomeWhirlpoolArray = dataReceita.createNestedArray(F("nomeWhirlpool"));
      for (register int ind = 0; ind < prReceita.qtdeWhirlpool; ind++)
        nomeWhirlpoolArray.add(prReceita.whirlpool[ind].nome);

      JsonArray minWhirlpoolArray = dataReceita.createNestedArray(F("minWhirlpool"));
      for (register int ind = 0; ind < prReceita.qtdeWhirlpool; ind++)
        minWhirlpoolArray.add(prReceita.whirlpool[ind].tempo);

      dataReceita[F("minDescanso")] = prReceita.descanso[ind].tempo;

      code = F("A2"); // Success!

      JsonArray error = dataReceita.createNestedArray(F("code"));
      error.add(code);

      getJsonData(dataReceita);
    }
  }
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : apagaReceitaRemota
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void apagaReceitaRemota(int type, byte cmd, JsonObject dataObject) {
  JsonArray nomeReceitaArray = dataObject[F("receita")];
  int idxReceita = nomeReceitaArray[RESET].as<int>();
  if (qtdeReceita <= RESET) {
    passoMaquina = mSTANDBY;
    code = F("E11"); // Empty Memory
  }
  else {
    passoMaquina = mAPAGA_RECEITA;
    idReceitaSel = idxReceita - 1;
    if (idReceitaSel <= RESET)
      idReceitaSel = RESET;
    _endString = true;
    stResp = F("S");
  }
  setJsonData(type, cmd);
}