//*******************************************************************************************
// Arquivo C/C++ do Projeto Controlador de Patamares -- ENSABrewControll
//-------------------------------------------------------------------------------------------
//
// Arquivo: ENSABrewReceita.ino
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
// Função: criacaoDaReceita
// Data  : 13/12/2019 10:00
//WWWWWWWWWW*********************************************************************************

void criacaoDaReceita(void )
{
  // ------------------------------------------------------- //
  // Verifica o ESC

  if(_keyPress && _KEY_ESC)
  {
    // Reseta os parâmetros de escrita
    resetDataInput(false);
    
    // Altera o passo da máquina
    if(prOper == _CRIAR_RECEITA)
      passoMaquina = mMENU;
    else if(prOper == _EDITAR_RECEITA)
      passoMaquina = mRECEITASEL;

    // Retorna
    return;
  }

  // ------------------------------------------------------- //
  // Inserir o nome da nova receita

  if(passoMaquina == mNOME_RECEITA)
  {
    if (_keyPress) 
    {
      if(stDigitado == F("") && stChar == F("_"))
      {
        if (_KEY_LEFT) 
        {
          // Reseta os parâmetros de escrita
          resetDataInput(false);
          
          // Altera o passo da máquina
          if(prOper == _CRIAR_RECEITA)
            passoMaquina = mMENU;
          else if(prOper == _EDITAR_RECEITA)
            passoMaquina = mRECEITASEL;

          // Retorna
          return;
        }
      }
    }

    // Se ainda não finalizou a inserção do dado, prossegue com a rotina de inserção
    if(!_endString)
    {
      // Faz o controle do número de caracteres
      if(controlNumberCharacters())
      {
        // Retorna para evitar qualquer lixo na escrita
        return;
      }
      // Escrita do nome
      entradaString(prReceita.nome);
    }

    // Se já finalizou a inserção do dado
    if (_endString) 
    {
      // Reseta a flag de escrita
      _endString = false; stChar = F("_");
    
      // Verifica se a string (nome) está vazio, se estiver, retorna para escrever
      if(checkVoidString()) return;

      // Atualiza o nome da receita
      String nomeReceita = stDigitado;
      nomeReceita.toCharArray(prReceita.nome, MAX_NOME+1);

      // Verifica o nome digitado
      boolean _nomeExistente = false;
      for (register uint8_t auxInd = 0; auxInd < qtdeReceita; auxInd++) 
      {
        // Evita verificar a própria receita
        if((prOper == _CRIAR_RECEITA && auxInd != qtdeReceita) ||     // Na criação de receita
           (prOper == _EDITAR_RECEITA && auxInd != prReceita.codigo)) // Na edição da receita
        {
          // Atualiza a receita à ser verificada
          Receita receita = regReceita[auxInd];
          
          // Verifica se a receita existe
          if (receita.codigo < 0)
            return;

          String auxReceita = String(receita.nome);
          String auxPrReceita = String(prReceita.nome);
          
          // Iguala o tamanho dos nomes para comparar
          if(auxPrReceita.length() > auxReceita.length())
          {
            register uint8_t auxLength = auxPrReceita.length() - auxReceita.length();
            for(register uint8_t i = 0; i < auxLength; i++)
              auxReceita += F(" ");
          } 
          else if(auxReceita.length() > auxPrReceita.length())
          {
            register uint8_t auxLength = auxReceita.length() - auxPrReceita.length();
            for(register uint8_t i = 0; i < auxLength; i++)
              auxPrReceita += F(" ");
          }

          // Verifica se o nome é o mesmo
          if(auxPrReceita == auxReceita)
          {
            // Sinaliza que o nome em questão já existe
            _nomeExistente = true;
            // Sai do laço
            break;
          }
        }
      }

      // Se o nome em questão já existe
      if (_nomeExistente) 
      {
        // Aviso sonoro e tela de nome já existente
        bipAtencao(); printDisplay(F("Nome Ja Existe!"), F("Digite Novamente"), TEMPO_CONF);
        // Limpa o nome
        nomeReceita = F(""); nomeReceita.toCharArray(prReceita.nome, MAX_NOME+1);
        // Reseta a inserção de dado
        resetDataInput(false);
        // Retorna
        return;
      }

      // Salva o nome da receita
      nomeReceita.toCharArray(prReceita.nome, MAX_NOME+1);
      // Altera o passo da máquina
      passoMaquina = mQT_PATAMAR;
      // Reinicia o temporizador de troca de mensagens 
      loadMsgChangeTimer();
      // Reseta a inserção de dado
      resetDataInput(false);
      // Retorna
      return;
    }
  }

  // ------------------------------------------------------- //
  // Inserir quantidade de patamares

  else if(passoMaquina == mQT_PATAMAR)
  {
    if (_keyPress) 
    {
      if (_KEY_LEFT) 
      {
        // Reseta a escrita
        resetDataInput(false);
        // Volta para o passo anterior
        passoMaquina--;
        // Reinicia o temporizador de troca de mensagens 
        loadMsgChangeTimer();
        // Retorna
        return;
      }
      // Passa para o próximo dado à ser inserido
      else if (_KEY_RIGHT) 
      { ind = 0; resetDataInput(true); }
    }

    // Se ainda não finalizou a inserção do dado, prossegue com a rotina de inserção
    if(!_endValue)
    {
      if(prReceita.qtdePatamar > configGeral.qtdeMaxPatamar)
        prReceita.qtdePatamar = configGeral.qtdeMaxPatamar;

      entradaValorGeral(prReceita.qtdePatamar, MIN_PATAMAR, configGeral.qtdeMaxPatamar, false);
    }
    
    // Se já finalizou a inserção do dado
    if (_endValue) 
    {
      // Reseta a entrada de dados
      resetDataInput(false);
      // Armazena o valor digitado
      qtdePatamar = valorDigitado;
      prReceita.qtdePatamar = valorDigitado;
      // Reseta o índice auxiliar das receitas
      ind = 0;
      // Altera o passo da máquina
      passoMaquina = mTEMPMAX_PATAMAR;
      // Reinicia o temporizador de troca de mensagens 
      loadMsgChangeTimer();
      // Retorna
      return;
    }
  }

  // ------------------------------------------------------- //
  // Inserir a temperatura máxima de cada patamar

  else if(passoMaquina == mTEMPMAX_PATAMAR)
  {
    // // Se já definiu a temperatura para todos os patamares
    // if (ind >= qtdePatamar) 
    // {
    //   // Reseta o índice auxiliar das receitas
    //   ind = 0;
    //   // Finaliza/reseta a inserção de dados
    //   resetDataInput(false);
    //   // Altera o passo da máquina
    //   passoMaquina = mSALVA_RECEITA;
    //   // Reinicia o temporizador de troca de mensagens 
    //   loadMsgChangeTimer();
    //   // Retorna
    //   return;
    // }

    if (_keyPress) 
    {
      // Sinaliza um retorno
      if (_KEY_LEFT) 
      {
        // Decrementa o índice auxiliar
        ind--;
        // Retorna ao patarmar anterior
        if (ind >= 0)
          passoMaquina++;
        // Retorna à inserção da quantidade de patamares
        else 
        {
          // Reseta o índice auxiliar das receitas
          ind = 0;
          // Altera o passo da máquina
          passoMaquina = mQT_PATAMAR;
        }
          
        // Finaliza/reseta a inserção de dados
        resetDataInput(false);
        // Reinicia o temporizador de troca de mensagens 
        loadMsgChangeTimer();
        // Retorna
        return;
      }

      // Passa para o próximo dado à ser inserido
      else if (_KEY_RIGHT) 
        resetDataInput(true);
    }

    // Se ainda não finalizou a inserção do dado, prossegue com a rotina de inserção
    if (!_endValue)
      entradaValorGeral(prReceita.patamar[ind].temperaturaMax,
                        MIN_TEMPERATURA_PATAMAR, MAX_TEMPERATURA_PATAMAR, false);
    
    // Se já finalizou a inserção do dado
    if (_endValue) 
    {
      // Reseta a entrada de dados
      resetDataInput(false);
      // Armazena o valor inserido
      prReceita.patamar[ind].temperaturaMax = valorDigitado;
      // Altera o passo da máquina
      passoMaquina++;
      // Reinicia o temporizador de troca de mensagens 
      loadMsgChangeTimer();
      // Retorna
      return;
    }
  }

  // ------------------------------------------------------- //
  // Inserir o tempo de cada patamar

  else if(passoMaquina == mMINUTO_PATAMAR)
  {
    if (_keyPress) 
    {
      if (_KEY_LEFT) 
      {
        // Finaliza/reseta a inserção de dados
        resetDataInput(false);
        // Altera o passo da máquina
        passoMaquina = mTEMPMAX_PATAMAR;
        // Reinicia o temporizador de troca de mensagens 
        loadMsgChangeTimer();
        // Retorna
        return;
      }
      // Passa para o próximo dado à ser inserido
      else if (_KEY_RIGHT) 
        resetDataInput(true);
    }
    // Se ainda não finalizou a inserção do dado, prossegue com a rotina de inserção
    if (!_endValue)
      entradaValorGeral(prReceita.patamar[ind].tempo,
                        MIN_MINUTO_PATAMAR, MAX_MINUTO_PATAMAR, false);
    
    // Se já finalizou a inserção do dado
    if (_endValue) 
    {
      // Reseta a entrada de dados
      resetDataInput(false);
      // Armazena o valor inserido
      prReceita.patamar[ind].tempo = valorDigitado;

      // Incrementa o índice auxiliar para ir para o próximo patamar
      ind++;

      // Se já definiu a temperatura para todos os patamares
      if (ind >= qtdePatamar) 
      {
        // Reseta o índice auxiliar das receitas
        ind = 0;
        // Altera o passo da máquina
        passoMaquina = mSALVA_RECEITA;
      }
      else
      {
        // Altera o passo da máquina
        passoMaquina--;
      }

      // Reinicia o temporizador de troca de mensagens 
      loadMsgChangeTimer();
      // Retorna
      return;
    }
  }

  // ------------------------------------------------------- //
  // Finaliza/armazena a receita

  else if(passoMaquina == mSALVA_RECEITA)
  {
    // Envia a tela atual
    enviaTelaStatus(F("Receita Salva"));
    
    // Atualiza e armazena todos dos dados da receita em questão
    prReceita.qtdePatamar = qtdePatamar;

    // Se for uma nova receita, atualiza a quantidade de receitas armazenadas
    if (prOper == _CRIAR_RECEITA) 
    { prReceita.codigo = qtdeReceita; qtdeReceita = qtdeReceita + 1; }

    // Armazena a posição da receita
    regReceita[prReceita.codigo] = prReceita;

    // Salva a receita
    salvaReceita(prReceita);

    // Aviso sonoro e tela de receita salva com sucesso
    bipAcerto();  printDisplay(F(" Receita Salva "), F("  Com Sucesso!  "), TEMPO_CONF);

    // Altera o passo da máquina
    if (code == F("A1")) 
      passoMaquina = mSTANDBY;
    else 
      passoMaquina = mMENU;
    
    // Retorna
    return;
  }
}

//WWWWWWWWWW*********************************************************************************
// Função: criaReceita
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void criaReceita() 
{
  prOper = _CRIAR_RECEITA;
  Receita receita;
  carregaReceitas();
  mantemReceita(receita);
}

//WWWWWWWWWW*********************************************************************************
// Função: editaReceita
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void editaReceita(int prIdReceita) 
{
  prOper = _EDITAR_RECEITA;
  mantemReceita(regReceita[prIdReceita]);
  carregaReceitas();
}

//WWWWWWWWWW*********************************************************************************
// Função: initVisualizaReceita
// Data  : 14/01/2020 11:40
//WWWWWWWWWW*********************************************************************************

void initVisualizaReceita(int prIdReceita)
{
  prOper = _EDITAR_RECEITA;
  mantemReceita(regReceita[prIdReceita]);
  // carregaReceitas();
  auxVisualizaReceita = mNOME_RECEITA;
  passoMaquina = mVISUALIZA_RECEITA;
}

//WWWWWWWWWW*********************************************************************************
// Função: apagaReceita
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void apagaReceita(int prIdReceita) 
{
  Receita receita = regReceita[prIdReceita];
  perguntaSimNao();

  if (_endString) 
  {
    _endString = false;
    if (stResp == ST_RESP_NAO) 
    {
      passoMaquina = mRECEITASEL;
      return;
    }
    else if (stResp == ST_RESP_SIM) 
    {
      disableInterrupts(); // Disable interrupts
      for (register int ind = receita.codigo; ind < qtdeReceita; ind++) 
      {
        regReceita[ind] = regReceita[ind + 1];
        Receita receita = regReceita[ind];
        receita.codigo = receita.codigo - 1;
        EEPROM.put((ind * sizeof(receita)) + NETEEPROM_END, receita);
      }
      EEPROM.put(((qtdeReceita - 1) * sizeof(receita)) + NETEEPROM_END, -1);
      enableInterrupts(); // Enable interrupts
      qtdeReceita--;
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
// Função: executaMenuReceitaSel
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void executaMenuReceitaSel(void ) 
{
  // Controle do menu com base no tamanho do mesmo
  register uint8_t opcMenu = executaMenu(LENGTH_MENU_RECEITA_SEL);

  // Se confirmou a opção
  if (_analisaOpcMenu) 
  {
    // Reseta a flag de opção
    _analisaOpcMenu = false;
    // Reinicia o temporizador de troca de mensagens 
    loadMsgChangeTimer();

    // Verifica a opção do menu
    switch(opcMenu)
    {
      // Voltar
      case 0: passoMaquina = mRECEITAS; break;
      // Visualizar
      case 1: initVisualizaReceita(idReceitaSel); break;
      // Produção
      case 2: passoMaquina = mSTART_PRODUCTION; break;
      // Editar
      case 3: editaReceita(idReceitaSel); break;
      // Apagar
      case 4: passoMaquina = mAPAGA_RECEITA; break;
      // DEFAULT
      default: break;
    }
  }
}

//WWWWWWWWWW*********************************************************************************
// Função: executaMenuReceitas
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void executaMenuReceitas(void ) 
{
  int opcMenu = executaMenu(qtdeReceita + 1);
  if (_analisaOpcMenu) 
  {
    _analisaOpcMenu = false;
    if (opcMenu == 0)
      passoMaquina = mMENU;
    else 
    {
      idReceitaSel = opcMenu - 1;
      receitaSel = regReceita[idReceitaSel];
      passoMaquina = mRECEITASEL;
    }
  }
}

//WWWWWWWWWW*********************************************************************************
// Função: executaMenuReceitas
// Data  : 14/01/2020  11:30
//WWWWWWWWWW*********************************************************************************

void controleVisualizaReceita(void )
{
  if(_keyPress)
  {
    if(_KEY_ESC)
    { passoMaquina = mRECEITASEL; return; }

    else if(_KEY_RIGHT)
    {
      switch(auxVisualizaReceita)
      {
        case mNOME_RECEITA:
          
          // Visualiza a quantidade de patamares
          auxVisualizaReceita = mQT_PATAMAR;
          
        break;
      
        case mQT_PATAMAR:
          
          // Visualiza a temperatura do patamar em questão
          auxVisualizaReceita = mTEMPMAX_PATAMAR; ind = 0;

        break;

        case mTEMPMAX_PATAMAR:

          // Visualiza o tempo do patamar
          auxVisualizaReceita = mMINUTO_PATAMAR;

        break;

        case mMINUTO_PATAMAR:

          // Incrementa o índice auxiliar para ir para o próximo patamar
          ind++;

          // Se já visualizou todos os patamares
          if(ind >= prReceita.qtdePatamar)
          {
            // Reseta o índice auxiliar das receitas
            ind = 0;
            // Visualiza a temperatura de fervura  
            fimVisualizacao();
          }
          else
          {
            // Visualiza a temperatura do patamar em questão
            auxVisualizaReceita = mTEMPMAX_PATAMAR;
          }
      }   

      // Reinicia o temporizador de troca de mensagens 
      loadMsgChangeTimer();
    }

    else if(_KEY_LEFT)
    {
      switch(auxVisualizaReceita)
      {
        case mNOME_RECEITA:
        
          // Volta para o menu
          passoMaquina = mRECEITASEL;
          /*posicaoMenu = retornaPosicao(mVISUALIZA_RECEITA); */

        break;
      
        case mQT_PATAMAR:
          
          // Visualiza o nome da receita
          auxVisualizaReceita = mNOME_RECEITA;

        break;

        case mTEMPMAX_PATAMAR:

          // Decrementa o índice auxiliar
          ind--;
          // Retorna ao patarmar anterior
          if (ind >= 0)
            auxVisualizaReceita++;
          // Retorna à inserção da quantidade de patamares
          else 
          {
            // Reseta o índice auxiliar das receitas
            ind = 0;
            // Volta à visualiza a quantidade de patamares
            auxVisualizaReceita = mQT_PATAMAR;
          }

        break;

        case mMINUTO_PATAMAR:

          // Visualiza a temperatura do patamar em questão
          auxVisualizaReceita = mTEMPMAX_PATAMAR;

        break;
      }

      // Reinicia o temporizador de troca de mensagens 
      loadMsgChangeTimer();   
    }

    if(_connectedAPP)
      enviaTela(true);
  }
}

//WWWWWWWWWW*********************************************************************************
// Função: fimVisualizacao
// Data  : 06/02/2020 10:55
//WWWWWWWWWW*********************************************************************************

void fimVisualizacao(void )
{
  // Zera o índice auxiliar
  ind = 0;
  // Volta ao Menu
  passoMaquina = mRECEITAS;
  // Aviso sonoro e tela temporaria do fim de visualização da receita
  bipOK(); printDisplay(F(" Fim da Receita "), F(""), TEMPO_CONF);

  if(_connectedAPP)
     enviaTelaStatus(F("Fim da Vizualizacao da Receita"));
}

//WWWWWWWWWW*********************************************************************************
// Função: carregaReceitas
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void carregaReceitas() 
{
  menuReceita[0] = F("Voltar");
  qtdeReceita = 0;
  disableInterrupts(); // Disable interrupts
  for (register int auxInd = 0; auxInd < MAX_RECEITA; auxInd++) 
  {
    Receita receita;
    int ender = (auxInd * sizeof(receita)) + NETEEPROM_END;
    EEPROM.get(ender, receita);

    // Serial.println("receita.codigo: " + String(receita.codigo));
    if (receita.codigo < 0)
      break;
    qtdeReceita = qtdeReceita + 1;
    if (qtdeReceita >= MAX_RECEITA)
      qtdeReceita = MAX_RECEITA;
    menuReceita[auxInd + 1] = "(" + String(receita.codigo + 1) + ")" + String(receita.nome);
    regReceita[auxInd] = receita;
  }
  enableInterrupts(); // Enable interrupts
}

//WWWWWWWWWW*********************************************************************************
// Função: mantemReceita
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void mantemReceita(Receita mantemReceita) 
{
  if ((qtdeReceita >= MAX_RECEITA) && (prOper == _CRIAR_RECEITA)) 
  {
    bipAtencao();
    printDisplay(F(" Memoria cheia! "), F(" Atingiu limite "), TEMPO_ALARME);
    passoMaquina = mMENU;
    return;
  }
  if (prOper == _CRIAR_RECEITA) 
  {
    Receita newReceita;
    prReceita = newReceita;
  }
  else if (prOper == _EDITAR_RECEITA)
    prReceita = mantemReceita;

  // Reinicia o temporizador de troca de mensagens 
  loadMsgChangeTimer();
  passoMaquina = mNOME_RECEITA;
}

//WWWWWWWWWW*********************************************************************************
// Função: salvaReceita
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void salvaReceita(Receita prReceita) 
{
  disableInterrupts(); // Disable interrupts
  int num = prReceita.codigo;
  int ender = (num * sizeof(prReceita)) + NETEEPROM_END;
  if (ender >= END_CONFIG)
    ender = NETEEPROM_END;
  EEPROM.put(ender, prReceita);
  enableInterrupts(); // Enable interrupts
  carregaReceitas();
}

//WWWWWWWWWW*********************************************************************************
// Função: actualWhirlpool
// Data  : 28/02/2020   09:50
//WWWWWWWWWW*********************************************************************************

String actualWhirlpool(register uint8_t auxActualWhirlpool)
{
  if(auxActualWhirlpool == 0)
    return(F("LENTO"));
  else return(F("RAPIDO"));
}