//*******************************************************************************************
// Arquivo C/C++ do Projeto Controlador de Patamares -- ENSABrewControll
//-------------------------------------------------------------------------------------------
//
// Arquivo: ENSABrewReceita.ino
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
            uint8_t auxLength = auxPrReceita.length() - auxReceita.length();
            for(register uint8_t i = 0; i < auxLength; i++)
              auxReceita += F(" ");
          } 
          else if(auxReceita.length() > auxPrReceita.length())
          {
            uint8_t auxLength = auxReceita.length() - auxPrReceita.length();
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
    // Se já definiu a temperatura para todos os patamares
    if (ind >= qtdePatamar) 
    {
      // Reseta o índice auxiliar das receitas
      ind = 0;
      // Finaliza/reseta a inserção de dados
      resetDataInput(false);
      // Altera o passo da máquina
      passoMaquina = mTEMPMAX_FERVURA;
      // Reinicia o temporizador de troca de mensagens 
      loadMsgChangeTimer();
      // Retorna
      return;
    }

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
      // Altera o passo da máquina
      passoMaquina--;
      // Reinicia o temporizador de troca de mensagens 
      loadMsgChangeTimer();
      // Retorna
      return;
    }
  }

  // ------------------------------------------------------- //
  // Inserir a temperatura da fervura

  else if(passoMaquina == mTEMPMAX_FERVURA)
  {
    if (_keyPress) 
    {
      if (_KEY_LEFT) 
      {
        // Finaliza/reseta a inserção de dados
        resetDataInput(false);
        // Altera o índice auxiliar de acordo com a quantidade de patamares
        ind = qtdePatamar - 1;
        // Altera o passo da máquina
        passoMaquina--;
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
    if(!_endValue)
    entradaValorGeral(prReceita.tempFervura,
                      MIN_TEMPERATURA_FERVURA, MAX_TEMPERATURA_FERVURA, false);

    // Se já finalizou a inserção do dado
    if (_endValue) 
    {
      // Reseta a inserção do dado
      resetDataInput(false);
      // Armazena o valor inserido
      tempFervura = valorDigitado; prReceita.tempFervura = valorDigitado;
      // Altera o passo da máquina
      passoMaquina = mMAXMIN_FERVURA;
      // Reinicia o temporizador de troca de mensagens 
      loadMsgChangeTimer();
      // Retorna
      return;
    }
  }

  // ------------------------------------------------------- //
  // Inserir o tempo de fervura

  else if(passoMaquina == mMAXMIN_FERVURA)
  {
    if (_keyPress) 
    {
      if (_KEY_LEFT) 
      {
        // Finaliza/reseta a inserção de dados
        resetDataInput(false);
        // Altera o passo da máquina
        passoMaquina--;
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
      entradaValorGeral(prReceita.qtdeMinFervura, MIN_MINUTO_FERVURA, 
                        MAX_MINUTO_FERVURA, false);

    // Se já finalizou a inserção do dado
    if (_endValue) 
    {
      // Finaliza/reseta a inserção de dados
      resetDataInput(false);
      // Armazena o valor inserido
      qtdeMinFervura = valorDigitado;
      prReceita.qtdeMinFervura = valorDigitado;
      // Altera o passo da máquina
      passoMaquina = mQT_ADICAO;
      // Reinicia o temporizador de troca de mensagens 
      loadMsgChangeTimer();
      // Retorna
      return;
    }
  }

  // ------------------------------------------------------- //
  // Inserir a quantidade de adições

  else if(passoMaquina == mQT_ADICAO)
  {
    if (_keyPress) 
    {
      if (_KEY_LEFT) 
      {
        // Finaliza/reseta a inserção de dados
        resetDataInput(false);
        // Altera o passo da máquina
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
      entradaValorGeral(prReceita.qtdeAdicao, MIN_ADICAO, MAX_ADICAO, false);

    // Se já finalizou a inserção do dado
    if (_endValue) 
    {
      // Finaliza/reseta a inserção de dados
      resetDataInput(false);
      // Armazena o valor inserido
      qtdeAdicao = valorDigitado;
      prReceita.qtdeAdicao = valorDigitado;
      qtdeMin = prReceita.qtdeMinFervura;
      // Altera o passo da máquina
      passoMaquina = mNOME_ADICAO;
      // Reinicia o temporizador de troca de mensagens 
      loadMsgChangeTimer();
      // Retorna
      return;
    }
  }
  
  // ------------------------------------------------------- //
  // Inserir o nome da adição

  else if(passoMaquina == mNOME_ADICAO)
  {
    // Se já inseriu o nome de todas as adições
    if (ind >= qtdeAdicao) 
    {
      // Reseta o índice auxiliar das receitas
      ind = 0;
      // Finaliza/reseta a inserção de dados
      resetDataInput(false);
      // Altera o passo da máquina
      passoMaquina = mQT_WHIRLPOOL;
      // Reinicia o temporizador de troca de mensagens 
      loadMsgChangeTimer();
      // Retorna
      return;
    }

    // Voltar ao passo anterior sem escrever o nome
    if (_keyPress && (stDigitado == F("") && stChar == F("_"))) 
    {
      if (_KEY_LEFT && stDigitado == F("")) 
      {
        // Finaliza e reseta todos os parâmetros da inserção de dados
        resetDataInput(false);
        // Altera o passo da máquina
        passoMaquina--;
        // Reinicia o temporizador de troca de mensagens 
        loadMsgChangeTimer();

        // Decrementa o índice
        ind--;
        // Volta para quantidade de adições
        if (ind == -1) 
        {
          // Reseta o índice
          ind = 0;
          // Altera o passo da máquina
          passoMaquina = mQT_ADICAO;
        }
        // Volta para tempo das adições
        else 
          passoMaquina = mMINUTOS_ADICAO;

        // Retorna
        return;
      }
    }

    // Se ainda não finalizou a inserção do dado, prossegue com a rotina de inserção
    if (!_endString)
    {
      // Faz o controle do número de caracteres
      if(controlNumberCharacters())
      {
        // Retorna para evitar qualquer lixo na escrita
        return;
      }
      // Escrita do nome
      entradaString(prReceita.adicao[ind].nome);
    }

    // Se já finalizou a inserção do dado
    if (_endString) 
    {
      // Reseta a flag de escrita
      _endString = false; stChar = F("_");

      // Verifica se a string (nome) está vazio, se estiver, retorna para escrever
      if(checkVoidString()) return;

      // Armazena o dado inserido
      String nomeAdicao = stDigitado; 
      nomeAdicao.toCharArray(prReceita.adicao[ind].nome, MAX_NOME+1);
      // Altera o passo da máquina
      passoMaquina++;
      // Reinicia o temporizador de troca de mensagens 
      loadMsgChangeTimer();
      // Reseta a inserção do dado
      resetDataInput(false);
      // Retorna
      return;
    }
  }

  // ------------------------------------------------------- //
  // Inserir a quantidade - em gramas - de cada adição

  else if(passoMaquina == mGRAMAS_ADICAO)
  {
    if (_keyPress) 
    {
      if (_KEY_LEFT) 
      {
        // Reseta os dados de inserção
        resetDataInput(false);
        // Altera o passo da máquina
        passoMaquina--;
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
    entradaValorGeral(prReceita.adicao[ind].qtde,
                      MIN_GRAMAS_ADICAO, MAX_GRAMAS_ADICAO, false);

    // Se já finalizou a inserção do dado
    if (_endValue) 
    {
      // Reseta a entrada de dados
      resetDataInput(false);
      // Armazena o dado inserido
      prReceita.adicao[ind].qtde = valorDigitado;
      // Altera o passo da máquina
      passoMaquina++;
      // Reinicia o temporizador de troca de mensagens 
      loadMsgChangeTimer();
      // Retorna
      return;
    }
  }

  // ------------------------------------------------------- //
  // Inserir o tempo para cada adição - em minutos -

  else if(passoMaquina == mMINUTOS_ADICAO)
  {
    if (_keyPress) 
    {
      if (_KEY_LEFT) 
      {
        // Finaliza/reseta a inserção de dados
        resetDataInput(false);
        // Altera o passo da máquina
        passoMaquina--;
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
    {
      // Se está em edição da receita
      if (prOper == _EDITAR_RECEITA)
      {
        if (ind == 0)
          entradaValorGeral(prReceita.adicao[ind].tempo,
                            MIN_MINUTOS_ADICAO, prReceita.qtdeMinFervura, false);
        else
        {
          if(prReceita.adicao[ind - 1].tempo != MIN_MINUTOS_ADICAO)
            entradaValorGeral(qtdeMin, MIN_MINUTOS_ADICAO, prReceita.adicao[ind - 1].tempo, false);
          else
            entradaValorGeral(qtdeMin, MIN_MINUTOS_ADICAO, MIN_MINUTOS_ADICAO, false);
        }
         
      }
      // Se está na criação da receita
      else 
      {
        if (ind == 0)
          entradaValorGeral(qtdeMin, MIN_MINUTOS_ADICAO, prReceita.qtdeMinFervura, false);
        else
        {
          if(prReceita.adicao[ind - 1].tempo != MIN_MINUTOS_ADICAO)
            entradaValorGeral(qtdeMin, MIN_MINUTOS_ADICAO, prReceita.adicao[ind - 1].tempo, false);
          else
            entradaValorGeral(qtdeMin, MIN_MINUTOS_ADICAO, MIN_MINUTOS_ADICAO, false);
        }
      }
    }

    // Se já finalizou a inserção do dado
    if (_endValue) 
    {
      // Reseta a entrada de dados
      resetDataInput(false);
      // Armazena o dado inserido
      prReceita.adicao[ind].tempo = valorDigitado;

      // Ajusta o valor de tempo
      if(prReceita.adicao[ind].tempo == MIN_MINUTOS_ADICAO)
        qtdeMin = MIN_MINUTOS_ADICAO;
      else
        qtdeMin = prReceita.adicao[ind].tempo - 1;

      // Incrementa o índice auxiliar para ir para a próxima adição
      ind++;
      // Altera o passo da máquina
      passoMaquina = mNOME_ADICAO;
      // Reinicia o temporizador de troca de mensagens 
      loadMsgChangeTimer();
      // Retorna
      return;
    }
  }

  // ------------------------------------------------------- //
  // Inserir a quantidade de whirlpools

  else if(passoMaquina == mQT_WHIRLPOOL)
  {
    if (_keyPress) 
    {
      if (_KEY_LEFT) 
      {
        // Finaliza/reseta a inserção de dados
        resetDataInput(false);
        
        // Altera o passo da máquina de acordo com o número de adições
        if(prReceita.qtdeAdicao == 0)
        {
          ind = 0;
          passoMaquina = mQT_ADICAO;
        }
        else
        {
          passoMaquina--;
          ind = prReceita.qtdeAdicao - 1;
          if(ind == 0)
            qtdeMin = prReceita.qtdeMinFervura;
          else
          {
            qtdeMin = prReceita.adicao[ind].tempo + 1;
            if(qtdeMin > prReceita.qtdeMinFervura)
              qtdeMin = prReceita.qtdeMinFervura;
          }
        }

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
    if(!_endValue)
      entradaValorGeral(prReceita.qtdeWhirlpool,
                        MIN_WHIRLPOOL, MAX_WHIRLPOOL, false);
    
    // Se já finalizou a inserção do dado
    if (_endValue) 
    {   
      // Zera o índice auxiliar
      ind = 0;
      // Reseta a entrada de dados
      resetDataInput(false);
      // Armazena o valor inserido
      qtdeWhirlpool = valorDigitado;
      prReceita.qtdeWhirlpool = valorDigitado;

      // Altera o passo da máquina
      if(qtdeWhirlpool == 0)
        passoMaquina = mQT_DESCANSO;
      else
        passoMaquina = mMINUTO_WHIRLPOOL;

      // Reinicia o temporizador de troca de mensagens 
      loadMsgChangeTimer();
      // Retorna
      return;
    }
  }

  // ------------------------------------------------------- //
  // Inserir os tempos para os whirlpools

  else if(passoMaquina == mMINUTO_WHIRLPOOL)
  {
    if (_keyPress) 
    {
      if (_KEY_LEFT) 
      {
        if (ind <= 0)
        {
          // Zera o índice auxiliar
          ind = 0;
          // Altera o passo da máquina
          passoMaquina = mQT_WHIRLPOOL;
        }
        // Decrementa o índice auxiliar
        else ind--;
        
        // Reseta a entrada de dados
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
    entradaValorGeral(prReceita.whirlpool[ind].tempo,
                      MIN_MINUTOS_WHIRLPOOL, MAX_MINUTOS_WHIRLPOOL, false);

    // Se já finalizou a inserção do dado
    if (_endValue) 
    {
      // Armazena o dado inserido
      prReceita.whirlpool[ind].tempo = valorDigitado;
      qtdeMinWhirlpool = prReceita.whirlpool[ind].tempo;
      // Reseta a entrada de dados
      resetDataInput(false); 
      // Incrementa o índice auxiliar para o próximo whirlpool
      ind++;
      // Altera o passo da máquina
      // passoMaquina--;
      if (ind >= qtdeWhirlpool) 
      {
        // Reseta o índice auxiliar
        ind = 0;
        // Altra o passo da máquina
        passoMaquina = mQT_DESCANSO;
      }
      // Reinicia o temporizador de troca de mensagens 
      loadMsgChangeTimer();
      // Retorna
      return;
    }
  }

  // ------------------------------------------------------- //
  // Inserir a quantidade de descansos

  else if(passoMaquina == mQT_DESCANSO)
  {
    if (_keyPress) 
    {
      if (_KEY_LEFT) 
      {
        // Se não houver whirlpools
        if(prReceita.qtdeWhirlpool == 0)
        {
          // Zera o índice auxiliar
          ind = 0;
          // Retorna direto para quantidade
          passoMaquina = mQT_WHIRLPOOL;
        }
        else
        {
          // Atualiza o índice auxiliar de acordo com a quantidade de whirlpool
          ind = qtdeWhirlpool - 1;
          // Altera o passo da máquina
          passoMaquina--;
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
      entradaValorGeral(prReceita.qtdeDescanso,
                        MIN_DESCANSO, MAX_DESCANSO, false);
    
    // Se já finalizou a inserção do dado
    if (_endValue) 
    {
      // Zera o índice auxiliar
      ind = 0;
      // Reseta a entrada de dados
      resetDataInput(false);
      // Armazena o dado inserido
      qtdeDescanso = valorDigitado;
      prReceita.qtdeDescanso = valorDigitado;

      // Se não houver descanso
      if(prReceita.qtdeDescanso == 0)
      {
        // Vai direto para fim da receita
        passoMaquina = mSALVA_RECEITA;
      }
      else
      {
        // Altera o passo da máquina
        passoMaquina = mMINUTO_DESCANSO;
      }
      // Reinicia o temporizador de troca de mensagens 
      loadMsgChangeTimer();
      // Retorna
      return;
    }
  }

  // ------------------------------------------------------- //
  // Inserir os tempos dos descansos

  else if(passoMaquina == mMINUTO_DESCANSO)
  {
    if (_keyPress) 
    {       
      if (_KEY_LEFT) 
      {
        // Altera o passo da máquina
        passoMaquina = mQT_DESCANSO;
        // Reinicia o temporizador de troca de mensagens 
        loadMsgChangeTimer();
        // Reseta a escrita
        resetDataInput(false);
        // Retorna
        return;
      }
      // Passa para o próximo dado à ser inserido
      else if (_KEY_RIGHT) 
        resetDataInput(true);
    }

    // Se ainda não finalizou a inserção do dado, prossegue com a rotina de inserção
    if (!_endValue) 
      entradaValorGeral(prReceita.descanso[ind].tempo,
                        MIN_MINUTOS_DESCANSO, MAX_MINUTOS_DESCANSO, false);

    // Se já finalizou a inserção do dado
    if (_endValue) 
    {
      // Reseta a entrada de dados
      resetDataInput(false);
      // Armazena o valor inserido
      prReceita.descanso[ind].tempo = valorDigitado;
      qtdeMinDescanso = prReceita.descanso[ind].tempo;
      // Incrementa o índice auxiliar para ir ao próximo descanso
      ind++;

      // Se chegou ao fim dos descansos
      if (ind >= qtdeDescanso) 
      {
        // Reseta o índice auxiliar
        ind = 0;
          
        // Limpa o display
        #if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)
          lcdClear();
        #endif

        // Vai para o passo de finalização/armazenamento da receita
        passoMaquina = mSALVA_RECEITA;
        // Retorna
        return;
      }

      // Altera o passo da máquina
      passoMaquina--;
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
    prReceita.qtdeAdicao = qtdeAdicao;
    prReceita.qtdeMinFervura = qtdeMinFervura;
    prReceita.tempFervura = tempFervura;
    prReceita.qtdeWhirlpool = qtdeWhirlpool;
    prReceita.qtdeMinWhirlpool = qtdeMinWhirlpool; 

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

void executaMenuReceitaSel() 
{
  int opcMenu = executaMenu(LENGTH_MENU_RECEITA_SEL);
  if (_analisaOpcMenu) 
  {
    // Reinicia o temporizador de troca de mensagens 
    loadMsgChangeTimer();

    _analisaOpcMenu = false;
    if (opcMenu == 0) //Voltar
      passoMaquina = mRECEITAS;
    else if(opcMenu == 1)  //Visualizar
      initVisualizaReceita(idReceitaSel);
    else if (opcMenu == 2) //Produção
      passoMaquina = mCONTINUA_PRODUCAO;
    else if (opcMenu == 3) //Editar
      editaReceita(idReceitaSel);
    else if (opcMenu == 4) //Apagar
      passoMaquina = mAPAGA_RECEITA;
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
            auxVisualizaReceita = mTEMPMAX_FERVURA;
          }
          else
          {
            // Visualiza a temperatura do patamar em questão
            auxVisualizaReceita = mTEMPMAX_PATAMAR;
          }

        break;

        case mTEMPMAX_FERVURA:

          // Visualiza o tempo de fervura
          auxVisualizaReceita = mMAXMIN_FERVURA;

        break;

        case mMAXMIN_FERVURA:

          // Visualiza a quantidade de adições
          auxVisualizaReceita = mQT_ADICAO;

        break;

        case mQT_ADICAO:

          // Se não houverem adições
          if(prReceita.qtdeAdicao == 0)
          {
            // Visualiza a quantidade de whirlpools
            auxVisualizaReceita = mQT_WHIRLPOOL;
          }
          else
          {
            // Visualiza os nomes das adições
            auxVisualizaReceita = mNOME_ADICAO;
          }

        break;

        case mNOME_ADICAO:

          // Visualiza a quantidade (em gramas) da adição
          auxVisualizaReceita = mGRAMAS_ADICAO;

        break;

        case mGRAMAS_ADICAO:

          // Visualiza o tempo da adição
          auxVisualizaReceita = mMINUTOS_ADICAO;

        break;

        case mMINUTOS_ADICAO:

          // Incrementa o índice auxiliar para visualizar a próxima adição
          ind++;
          // Verifica se já visualizou todas as adições
          if(ind >= prReceita.qtdeAdicao)
          {
            // Zera o índice auxiliar
            ind = 0;
            // Visualiza a quantidade de whirlpools
            auxVisualizaReceita = mQT_WHIRLPOOL;
          }
          else
          {
            // Visualiza o nome da próxima adição
            auxVisualizaReceita = mNOME_ADICAO;
          }

        break;

        case mQT_WHIRLPOOL:

          // Se não houverem whirlpools
          if(prReceita.qtdeWhirlpool == 0)
          {
            // Visualiza a quantidade de whirlpools
            auxVisualizaReceita = mQT_DESCANSO;
          }
          else
          {
            // Visualiza o tempo do whirlpool em questão
            auxVisualizaReceita = mMINUTO_WHIRLPOOL; 
          }

        break;

        case mMINUTO_WHIRLPOOL:

          // Incrementa o índice auxiliar para visualizar o próximo whirlpool
          ind++;
          // Verifica se já visualizou todos os whirlpools
          if(ind >= prReceita.qtdeWhirlpool)
          {
            // Zera o índice auxiliar
            ind = 0;
            // Visualiza a quantidade de descanso
            auxVisualizaReceita = mQT_DESCANSO;
          }

        break;

        case mQT_DESCANSO:

          // Se não houver descanso, 
          if(prReceita.qtdeDescanso == 0)
            fimVisualizacao();
          // Visualiza o tempo do descanso --- nome do descanso
          else auxVisualizaReceita = mMINUTO_DESCANSO; 

        break;

        case mMINUTO_DESCANSO:

          // Incrementa o índice auxiliar para visualizar o próximo whirlpool
          ind++;
          // Verifica se já visualizou todos os descansos, finaliza a visualização
          if(ind >= prReceita.qtdeDescanso)
            fimVisualizacao();

        break;

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

        case mTEMPMAX_FERVURA:

          // Altera o índice auxiliar de acordo com a quantidade de patamares
          ind = prReceita.qtdePatamar - 1;
          // Volta à visualizar o tempo do último patamar
          auxVisualizaReceita--;

        break;

        case mMAXMIN_FERVURA:

          // Volta à visualizar a temperatura de fervura
          auxVisualizaReceita = mTEMPMAX_FERVURA;

        break;

        case mQT_ADICAO:

          // Volta à visualizar o tempo da fervura
          auxVisualizaReceita = mMAXMIN_FERVURA;

        break;

        case mNOME_ADICAO:

          // Decrementa o índice
          ind--;
          // Volta para quantidade de adições
          if (ind == -1) 
          {
            // Reseta o índice
            ind = 0;
            // Volta à visualizar a quantidade de adições
            auxVisualizaReceita = mQT_ADICAO;
          }
          // Volta para tempo das adições
          else 
          {
            // Volta à visualizar o tempo da adição anterior
            auxVisualizaReceita = mMINUTOS_ADICAO;
          }

        break;

        case mGRAMAS_ADICAO:

          // Visualiza o nome da adição em questão
          auxVisualizaReceita = mNOME_ADICAO;

        break;

        case mMINUTOS_ADICAO:

          // Visualiza a quantidade da adição em questão
          auxVisualizaReceita = mGRAMAS_ADICAO;

        break;

        case mQT_WHIRLPOOL:

          // Se a quantidade de adições for igual à zero
          if(prReceita.qtdeAdicao == 0)
          {
            // Volta à visualizar a quantidade de adições
            auxVisualizaReceita = mQT_ADICAO;
          }
          else
          {
            // Altera o índice auxiliar de acordo com a quantidade de adições
            ind = prReceita.qtdeAdicao - 1;
            // Volta à visualizar o tempo da última adição
            auxVisualizaReceita = mMINUTOS_ADICAO;
          }

        break;

        case mMINUTO_WHIRLPOOL:

          // Decrementa o índice
          ind--;
          if (ind == -1) 
          {
            // Reseta o índice
            ind = 0;
            // Volta à visualizar a quantidade de whirlpools
            auxVisualizaReceita = mQT_WHIRLPOOL;
          }
        break;

        case mQT_DESCANSO:

          // Se a quantidade de whirlpools for igual à zero
          if(prReceita.qtdeWhirlpool == 0)
          {
            // Volta à visualizar a quantidade de whirlpools
            auxVisualizaReceita = mQT_WHIRLPOOL;
          }
          else
          {
            // Altera o índice auxiliar de acordo com a quantidade de whirlpools
            ind = prReceita.qtdeWhirlpool - 1;
            // Volta à visualizar o tempo da última adição
            auxVisualizaReceita = mMINUTO_WHIRLPOOL;
          }

        break;

        case mMINUTO_DESCANSO:

          // Decrementa o índice
          ind--;
          if (ind == -1) 
          {
            // Reseta o índice
            ind = 0;
            // Volta à visualizar a quantidade de descansos
            auxVisualizaReceita = mQT_DESCANSO;
          }
          // Volta para tempo das adições
          else 
          {
            // Volta à visualizar o tempo do descanso anterior
            auxVisualizaReceita = mMINUTO_DESCANSO;
          }
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
// Função: criaEditaReceitaRemota
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void criaEditaReceitaRemota(int type, byte cmd, JsonObject dataObject, boolean auxPrOper) 
{
  carregaReceitas();
  
  if ((qtdeReceita >= MAX_RECEITA) && (auxPrOper == _CRIAR_RECEITA)) 
  {
    qtdeReceita = MAX_RECEITA;
    passoMaquina = mSTANDBY;
    code = F("E10"); // Memory-Full
    bipAtencao();
  }
  else if ((qtdeReceita <= RESET) && (auxPrOper == _EDITAR_RECEITA)) 
  {
    qtdeReceita = RESET;
    passoMaquina = mSTANDBY;
    code = F("E11"); // Empty Memory
    bipAtencao();
  }
  else 
  {

    //------------------------------------------------------------------------------------
    // O recebimento da receita será dividido em 3 partes ([PT1], [PT2] e [PT3]) por 
    // questões de compatibilidade com a capacidade de recebimento do módulo WiFi.
    //
    // Divisões por partes:
    //
    // [PT1]: Cabeçalho padrão (header - ... - cmd) + receita +
    //      tempPatamar + minPatamar tempFervura + minFervura 
    //
    // [PT2]: Cabeçalho padrão (header - ... - cmd) + receita +
    //      nomeAdicao + gramasAdicao + minAdicao
    //
    // [PT3]: Cabeçalho padrão (header - ... - cmd) + receita +
    //      nomeWhirlpool + minWhirlpool + minDescanso
    //------------------------------------------------------------------------------------
    // Recebendo o nome da receita
        
    JsonArray nomeReceitaArray = dataObject[F("receita")];
    int idxReceita = nomeReceitaArray[RESET].as<int>();
    if (prOper == _EDITAR_RECEITA)
      prReceita = regReceita[idxReceita - 1];
    String nomeReceita = nomeReceitaArray[RESET + 1].as<String>();
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
          uint8_t auxLength = auxPrReceita.length() - auxReceita.length();
          for(register uint8_t i = 0; i < auxLength; i++)
            auxReceita += F(" ");
        } 
        else if(auxReceita.length() > auxPrReceita.length())
        {
          uint8_t auxLength = auxReceita.length() - auxPrReceita.length();
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

    if (_nomeExistente) 
    {
      passoMaquina = mSTANDBY;
      code = F("E13"); // Nome já existente
      bipAtencao();
      setJsonData(type, cmd);
      return;
    }

    //------------------------------------------------------------------------------------
    // Recebimento [PT1]

    // Se possuir o objeto "tempPatamar", quer dizer que está recebendo a parte 1 da receita
    // if(dataObject[F("tempPatamar")]). Se não recebeu nenhuma das partes da receita,
    // inicia o tratamento do recebimento da parte 1

    if (contadorPartesReceita == 0) 
    {
      JsonArray tempPatamar = dataObject[F("tempPatamar")];
      qtdePatamar = tempPatamar.size();
      prReceita.qtdePatamar = qtdePatamar;
      JsonArray minPatamar = dataObject[F("minPatamar")];
      for (register int ind = 0; ind < qtdePatamar; ind++) 
      {
        prReceita.patamar[ind].temperaturaMax = tempPatamar[ind].as<int>();
        prReceita.patamar[ind].tempo = minPatamar[ind].as<int>();
      }

      tempFervura = dataObject[F("tempFervura")].as<int>();
      prReceita.tempFervura = tempFervura;
      qtdeMinFervura = dataObject[F("minFervura")].as<int>();
      prReceita.qtdeMinFervura = qtdeMinFervura;

      contadorPartesReceita++; // Recebeu a parte 1 da receita  
    }
    
    // Fim do recebimento [PT1]
    //------------------------------------------------------------------------------------
    // Recebimento [PT2]

    // Se possuir o objeto "nomeAdicao", quer dizer que está recebendo a parte 2 da receita
    // else if(dataObject[F("nomeAdicao")]). Se já recebeu a parte 1 da receita, 
    // inicia o tratamento do recebimento da parte 2

    else if (contadorPartesReceita == 1) 
    {
      JsonArray nomeAdicaoArray = dataObject[F("nomeAdicao")];
      qtdeAdicao = nomeAdicaoArray.size();
      prReceita.qtdeAdicao = qtdeAdicao;
      JsonArray gramasAdicao = dataObject[F("gramasAdicao")];
      JsonArray minAdicao = dataObject[F("minAdicao")];
      for (register int ind = 0; ind < qtdeAdicao; ind++) 
      {
        String nomeAdicao = nomeAdicaoArray[ind].as<String>();
        nomeAdicao.toCharArray(prReceita.adicao[ind].nome, MAX_NOME+1);
        prReceita.adicao[ind].qtde = gramasAdicao[ind].as<int>();
        prReceita.adicao[ind].tempo = minAdicao[ind].as<int>();
      }

      contadorPartesReceita++; // Recebeu a parte 2 da receita
    }

    // Fim do recebimento [PT2]
    //------------------------------------------------------------------------------------
    // Recebimento [PT3]

    // Se possuir o objeto "nomeWhirlpool", quer dizer que está recebendo a parte 3 da receita
    // else if(dataObject[F("nomeWhirlpool")]). Se já recebeu a parte 1 e 2 da receita, 
    // inicia o tratamento do recebimento da parte 3

    else if (contadorPartesReceita == 2) 
    {
      JsonArray minWhirlpool = dataObject[F("minWhirlpool")];

      qtdeDescanso = 1;
      uint8_t minDescanso = dataObject[F("minDescanso")].as<int>();
      prReceita.descanso[RESET].tempo = minDescanso;

      contadorPartesReceita++; // Recebeu a parte 3 da receita
    }

    // Se é criação de receitas e já recebeu todas as partes da receita, faz os incrementos de índice
    // Se já recebeu todas as partes da receita, salva

    if (contadorPartesReceita == 3) 
    {
      if (prOper == _CRIAR_RECEITA && contadorPartesReceita >= 3) 
      {
        prReceita.codigo = qtdeReceita;
        qtdeReceita = qtdeReceita + 1;
      }
      contadorPartesReceita = 0;
      regReceita[prReceita.codigo] = prReceita;
      passoMaquina = mSALVA_RECEITA;
      code = F("A1"); // Success!
      // bipOK(); // Descomentar para que, a cada receita recebida temos o "bip"
    }

    // Fim do recebimento [PT3]
    //------------------------------------------------------------------------------------

  }

  setJsonData(type, cmd);

}

//WWWWWWWWWW*********************************************************************************
// Função: respondeReceitaRemota
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void respondeReceitaRemota(int type, byte cmd, JsonObject dataObject) 
{
  carregaReceitas();
  
  if (qtdeReceita <= RESET) 
  {
    qtdeReceita = RESET;
    passoMaquina = mSTANDBY;
    code = F("E11"); // Empty Memory
    bipAtencao();
    setJsonData(type, cmd);
  }

  else 
  {
    JsonArray idxReceitaArray = dataObject[F("receita")];
    int idxReceita = idxReceitaArray[RESET].as<int>();
    prReceita = regReceita[idxReceita - 1];

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

    JsonArray minWhirlpoolArray = dataReceita.createNestedArray(F("minWhirlpool"));
    for (register int ind = 0; ind < prReceita.qtdeWhirlpool; ind++)
      minWhirlpoolArray.add(prReceita.whirlpool[ind].tempo);

    dataReceita[F("minDescanso")] = prReceita.descanso[ind].tempo;

    code = F("A2"); // Success!

    JsonArray error = dataReceita.createNestedArray(F("code"));
    error.add(code);

    getJsonData(dataReceita);

    if (type == 0xFF && (idxReceita == qtdeReceita)) 
    {
      passoMaquina = mSTANDBY;
      code = F("E12"); // Fim das receitas
      bipOK();
    }
  }
}

//WWWWWWWWWW*********************************************************************************
// Função: apagaReceitaRemota
// Data  : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void apagaReceitaRemota(int type, byte cmd, JsonObject dataObject) 
{
  JsonArray nomeReceitaArray = dataObject[F("receita")];
  int idxReceita = nomeReceitaArray[RESET].as<int>();
  
  if (qtdeReceita <= RESET) 
  {
    passoMaquina = mSTANDBY;
    code = F("E11"); // Empty Memory
    bipAtencao();
  }
  else 
  {
    // Reinicia o temporizador de troca de mensagens 
    loadMsgChangeTimer();

    passoMaquina = mAPAGA_RECEITA;
    idReceitaSel = idxReceita - 1;
    if (idReceitaSel <= RESET)
      idReceitaSel = RESET;
    _endString = true;
    stResp = F("S");
  }

  setJsonData(type, cmd);
}

//WWWWWWWWWW*********************************************************************************
// Função: actualWhirlpool
// Data  : 28/02/2020   09:50
//WWWWWWWWWW*********************************************************************************

String actualWhirlpool(uint8_t auxActualWhirlpool)
{
  if(auxActualWhirlpool == 0)
    return(F("LENTO"));
  else return(F("RAPIDO"));
}