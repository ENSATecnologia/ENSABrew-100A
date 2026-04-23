#### Cabeçalho padrão ENSA Tecnologia

	Cabeçalhos de equipamentos ENSA Tecnologia
		4E

	Cabeçalhos de segmento
		0001 - Postos de combustíveis
		0002 - Módulos IoT voltado ao condomínio

	Identificador de módulo
		ZZZZZZZZZ é 09 bytes, sendo [a-zA-Z0-9]

	Tipo do módulo
		D001 - Portas i/o Raspberry PI3
		D002 - Módulo 433Mhz TCP/IP v4
		D003 - Módulo 433Mhz TCP/IP v4 OFFLINE
		D004 - Módulo Wiegand TCP/IP v4
		D005 - Módulo Wiegand TCP/IP v4 OFFLINE
		D006 - Módulo de leitura de bombas 

#### * Observação: Protocolo baseado em Json

	  Exemplo - início do cabeçalho:

				{"header":"4E",
				 "product":"0002",
				 "type":"D002",
				 "id":"1020ABC01",
				 "typeCmd":"...",
				 "cmd":"...",...}

#### CC - Cabeçalho de comando

###### Faixa de cabeçalhos CC

| Nome                   | Tipo ("typeCmd") | Faixa ("cmd")|
|------------------------|------------------|--------------|
| Conexão                | CC   	 	    | 00      	   |
| Receitas               | CC   	 	    | 01 à 04      |
| Teclado Virtual        | CC   	 	    | 05           |
| Configurações          | CC   	 	    | 06 à 09      |
| Funções administrativas| CC   	 	    | 0A à 0E      |
 
###### Conexão
	
	00 - Subcabeçalho para confirmar a conexão:
      
	- Exemplo: do APP para o equipamento
     
			{"header":"4E",
			 "product":"0002",
			 "type":"D002",
			 "id":"1020ABC01",
			 "typeCmd":"CC",
			 "cmd":"00"}

	- Exemplo: do equipamento para o APP  
     
			{"header":"4E",
			 "product":"0002",
			 "type":"D002",
			 "id":"1020ABC01",
			 "typeCmd":"CC",
			 "cmd":"00",
			 "connection:":"ack"}

###### Receitas

	01 - Subcabeçalho para CRIAR uma receita:
     
	-Parâmetros:
     
		-> Nome da receita:                            
			
			"receita":["nome da receita"],
     
		-> Quantidade de patamares e suas temperaturas: (em °C)
			
			"tempPatamar":[50],
       
	*Obs: o número de patamares será definido de acordo com a quantidade de temperaturas que forem definidas dentro dos colchetes.
     
		-> Tempo dos patamares: (em minutos) 
			
			"minPatamar":[5],
          
	*Obs: a quantidade de tempo dentro dos colchetes deve corresponder à quantidade de patamares definidos anteriormente.
      
		-> Temperatura de fervura:  (em °C)
			
			"tempFervura":100,
      
		-> Tempo em fervura: (em minutos)
			
			"minFervura":3,
     
		-> Quantidade e nomes das adições:	  
			
			"nomeAdicao":["Nome adicao"],    
       
	*Obs: o número de adições será definido de acordo com a quantidade de nomes que forem definidos dentro dos colchetes.
     
		-> Quantidade de adição: (em gramas)
			
			"gramasAdicao":[100],
     
    *Obs: a quantidade de adição dentro dos colchetes deve corresponder àquantidade de adições definidos anteriormente.
     
		-> Tempo de cada adição: (em minutos)
			
			"minAdicao":[5],
     
    *Obs: a quantidade de tempo dentro dos colchetes deve corresponder àquantidade de adições definidos anteriormente.
     
		-> Quantidade e nome dos Whirpools: 
			
			"nomeWhirpool":["Nome do Whirpool"],
     
    *Obs: o número de whirpools será definido de acordo com a quantidade de nomes que forem definidos dentro dos colchetes.  
     
		-> Tempo em cada Whirpool: (em minutos)
			
			"minWhirpool":[10],
     
    *Obs: a quantidade de tempo dentro dos colchetes deve corresponder àquantidade de whirpools definidos anteriormente.
     
		-> Tempo de descanso: (em minutos)
			
			"minDescanso":30}
     
	- Exemplo:
     
			{"header":"4E",
			 "product":"0002",
			 "type":"D002",
			 "id":"1020ABC01",
			 "typeCmd":"CC",
			 "cmd":"01",
			 "receita":[1,"Receita Exemplo"],
			 "tempPatamar":[75],
			 "minPatamar":[15],
			 "tempFervura":150,
			 "minFervura":10,
			 "nomeAdicao":["Adicao Exemplo"],
			 "gramasAdicao":[500],
			 "minAdicao":[2],
			 "nomeWhirpool":["Whirpool Exemplo"],
			 "minWhirpool":[20],
			 "minDescanso":15}

--------------------------------------------------------------------------
	
	02 - Subcabeçalho para EDITAR uma receita:
	 
	*Obs: as regras de criação valem para edição.   
	  
	-Parâmetros:
     
		-> Nome e número da receita: 
			
			..."receita":[1, "nome da receita editado"],
	     
		-> Quantidade de patamares e suas temperaturas: (em °C)
		   	
		   	"tempPatamar":[25],
	      
		-> Tempo dos patamares: (em minutos)  
		   	
		   	"minPatamar":[10],
	          
		-> Temperatura de fervura:  (em °C)    
		   	
		   	"tempFervura":200,
	     
		-> Tempo em fervura: (em minutos)   
		   	
		   	"minFervura":5,
	     
		-> Quantidade e nomes das adições:  
		   	
		   	"nomeAdicao":["Nome adicao editado"],    
	     
		-> Quantidade de adição: (em gramas)
		   	
		   	"gramasAdicao":[250],
	     
		-> Tempo de cada adição: (em minutos)
		   	
		   	"minAdicao":[20],
	     
		-> Quantidade e nome dos Whirpools: 
		   	
		   	"nomeWhirpool":["Nome do Whirpool editado"],
	     
		-> Tempo em cada Whirpool: (em minutos)
		   	
		   	"minWhirpool":[9],
	     
		-> Tempo de descanso: (em minutos)
		   	
		   	"minDescanso":30}
     
	- Exemplo:
     
			{"header":"4E",
			 "product":"0002",
			 "type":"D002",
			 "id":"1020ABC01",
			 "typeCmd":"CC",
			 "cmd":"02",
			 "receita":[1,"Receita Exemplo"],
			 "tempPatamar":[25],
			 "minPatamar":[10],
			 "tempFervura":80,
			 "minFervura":8,
			 "nomeAdicao":["Adicao Exemplo"],
			 "gramasAdicao":[250],
			 "minAdicao":[5],
			 "nomeWhirpool":["Whirpool Exemplo"],
			 "minWhirpool":[12],
			 "minDescanso":25}

--------------------------------------------------------------------------
	
	03 - Subcabeçalho para APAGAR uma receita:
      
		- Necessário apenas o número da receita à ser apagada: 
			
			..."receita":[1, ""]}	
     
	- Exemplo:
     
			{"header":"4E",
			 "product":"0002",
			 "type":"D002",
			 "id":"1020ABC01",
			 "typeCmd":"CC",
			 "cmd":"03",
			 "receita":[1,""]}

--------------------------------------------------------------------------

	04 - Subcabeçalho para BRASSAR uma receita:
      
		- Necessário apenas o número da receita à ser brassada: 
			
			..."receita":[1, ""]}
     
	- Exemplo:
     
			{"header":"4E",
			 "product":"0002",
			 "type":"D002",
			 "id":"1020ABC01",
			 "typeCmd":"CC",
			 "cmd":"04",
			 "receita":[1,""]}	

###### Teclado Virtual

	05 - Subcabeçalho para uso do "teclado virtual":
      
	- As teclas são definidas por seus respectivos nomes:
     
		-> UP
		-> DOWN
		-> LEFT
		-> RIGHT
		-> ENTER
		-> ESC
     
    	- Necessário apenas o nome da tecla à ser "pressionada":
        
    		..."vKey":"UP"}
     
	- Exemplo:
     
			{"header":"4E",
			 "product":"0002",
			 "type":"D002",
			 "id":"1020ABC01",
			 "typeCmd":"CC",
			 "cmd":"05",
			 "vKey":"UP"}

###### Configurações

	06 - Subcabeçalho para configurar o máximo de patamares:
     
    	- Necessário apenas o máximo de patamares desejado:
     
			..."maxPatamar":"10"}
     
	- Exemplo:
     
			{"header":"4E",
			 "product":"0002",
			 "type":"D002",
			 "id":"1020ABC01",
			 "typeCmd":"CC",
			 "cmd":"06",
			 "maxPatamar":"10"}

--------------------------------------------------------------------------

	07 - Subcabeçalho para configurar os graus por minuto:
     
    	- Necessário apenas o grau por minuto desejado:
     
			..."grauPorMinuto":"1.5"}
     
	- Exemplo:
     
			{"header":"4E",
			 "product":"0002",
			 "type":"D002",
			 "id":"1020ABC01",
			 "typeCmd":"CC",
			 "cmd":"07",
			 "grauPorMinuto":"1.5"}

--------------------------------------------------------------------------

	08 - Subcabeçalho para configurar o PWM para Setpoint:
     
    	- Necessário apenas o PWM desejado:
     
			..."setPWM":"75"}
     
	- Exemplo:
     
			{"header":"4E",
			 "product":"0002",
			 "type":"D002",
			 "id":"1020ABC01",
			 "typeCmd":"CC",
			 "cmd":"08",
			 "setPWM":"75"}

--------------------------------------------------------------------------

	09 - Subcabeçalho para configurar Data e Hora:
     
    	- Configurar os segundos:
     
			..."segundos":"00",
     
    	- Configurar os minutos:
     
			"minutos":"30",
     
    	- Configurar as horas:
     
			"horas":"09",
     
    	- Configurar o dia da semana: 
      
    	(1-SEG)(2-TER)(3-QUA)(4-QUI)(5-SEX)(6-SAB)(7-DOM)
     
			"diaDaSemana":"2",	
     
    	- Configurar o dia do mês:
     
			"diaDoMes":"15",
     
    	- Configurar o mês:
     
			"mes":"01",
     
    	- Configurar o ano:
     
			"ano":"2019"}
        
	- Exemplo:
     
			{"header":"4E",
			 "product":"0002",
			 "type":"D002",
			 "id":"1020ABC01",
			 "typeCmd":"CC",
			 "cmd":"05",
			 "segundos":"00",
			 "minutos":"30",
			 "horas":"09",
			 "diaDaSemana":"2",	
			 "diaDoMes":"15",
			 "mes":"01",
			 "ano":"2019"}

	--------------------------------------------------------------------------

	10 - Subcabeçalho para configurar o PID:
     
    	- Configurar o status: 00 - desabilitado | 01 - habilitado
     
			..."status":"01",
     
    	- Configurar kp:
     
			"kp":"1.5",
     
    	- Configurar ki:
     
			"ki":"2.5",
     
    	- Configurar kd:
     
			"kd":"3.5",
        
	- Exemplo:
     
			{"header":"4E",
			 "product":"0002",
			 "type":"D002",
			 "id":"1020ABC01",
			 "typeCmd":"CC",
			 "cmd":"10",
			 "status":"01",
			 "kp":"1.5",
			 "ki":"2.5",
			 "kd":"3.5"}

###### Funções Administrativas

	0A - Subcabeçalho para limpar a EEPROM:
     
    	- Necessário apenas o comando:
       
    		..."cmd":"0A"} 
     
	- Exemplo:
     
			{"header":"4E",
			 "product":"0002",
			 "type":"D002",
			 "id":"1020ABC01",
			 "typeCmd":"CC",
			 "cmd":"0A"}

	0B - Subcabeçalho para alterar o tempo de envio de dados da brassagem:
     
    - Necessário apenas o tempo (em segundos) desejado:
     
			..."setTime":"1"}
       
	- Exemplo:
     
			{"header":"4E",
			 "product":"0002",
			 "type":"D002",
			 "id":"1020ABC01",
			 "typeCmd":"CC",
			 "cmd":"0B",
			 "setTime":"5"}

	0C - Controle dos solenóides
     
    - Necessário definir o solenóide em que será feito o controle (1 (00) ou 2 (01))
     
			..."solenoid":"00"} 

	- Necessário definir a ação - acionar (01) ou desacionar (00)
     
			..."action":"01"}      

    - Necessário definir o tempo:
     
			..."operatingTime":"10"}

	- Necessário definir o tipo do temporizador (mili segundos (00), segundos (01) ou minutos (02)):
     
			..."timerType":"01"}

	*OBS: para acionamento sem temporização, colocar 0 em ambos campos acima

	- Exemplo: acionar o solenóide 1 por 10 segundos
     
			{"header":"4E",
			 "product":"0002",
			 "type":"D002",
			 "id":"1020ABC01",
			 "typeCmd":"CC",
			 "cmd":"0C",
			 "solenoid":"00",
			 "action":"01",
			 "operatingTime":"10",
			 "timerType":"01"}

	0D - Subcabeçalho para alterar o fator de calibração:
     
    - Caso queira inserir diretamento o fator de calibração, coloque algum dado no campo "calFactor" e zera o campo realLiters":
     		
     		..."calFactor":"4.5"}
     		..."realLiters":"0.0"}

    - Caso queira que o equipamento calcule o novo fator de calibrãção à partir dos litros reais inseridos, deixe o campo "calFactor" zerado e preencha o campo "realLiters":

    		..."calFactor":"0.0"}
			..."realLiters":"5.5"}
       
	- Exemplo 1: inserindo direto o fator 
     
			{"header":"4E",
			 "product":"0002",
			 "type":"D002",
			 "id":"1020ABC01",
			 "typeCmd":"CC",
			 "cmd":"0D",
			 "calFactor":"4.5",
			 "realLiters":"0.0"}

	- Exemplo 2: cálculo automático do fator
     
			{"header":"4E",
			 "product":"0002",
			 "type":"D002",
			 "id":"1020ABC01",
			 "typeCmd":"CC",
			 "cmd":"0D",
			 "calFactor":"0.0",
			 "realLiters":"5.5"}

	0E - Subcabeçalho para configurar o envio da leitura AC:
     
    - Para desabilitar a leitura AC, colocar "0" no campo "time"
     		
     		..."time":"0"}
      
    - Para configurar um tempo e habilitar o envio nesse tempo, basta colocar um valor entre 1 e 255 (segundos) no campo "time"
      
    		..."time":"10"}

    - Exemplo:  
     
			{"header":"4E",
			 "product":"0002",
			 "type":"D002",
			 "id":"1020ABC01",
			 "typeCmd":"CC",
			 "cmd":"0E",
			 "time":"10"}

	0F - Subcabeçalho para configurar o envio da temperatura:
     
    - Para desabilitar o envio, colocar "0" no campo "time"
     		
     		..."time":"0"}
      
    - Para configurar um tempo e habilitar o envio nesse tempo, basta colocar um valor entre 1 e 255 (segundos) no campo "time"
      
    		..."time":"10"}
    			
    - Exemplo:  
     
			{"header":"4E",
			 "product":"0002",
			 "type":"D002",
			 "id":"1020ABC01",
			 "typeCmd":"CC",
			 "cmd":"0F",
			 "time":"10"}

#### FF - Cabeçalho de solicitação

###### Faixa de cabeçalhos FF

| Nome                   | Tipo ("typeCmd") | Faixa ("cmd")|
|------------------------|------------------|--------------|
| Receitas               | FF   	 	    | 01           |
| Configurações          | FF               | 02, 0D à FF  |
| Tela atual			 | FF				| 03		   |

###### Receitas

	01 - Subcabeçalho para solicitar uma receita:
      
		- Necessário apenas o número da receita à ser solicitada: 
			
			..."receita":[1, ""]}	
     
	- Exemplo:
     
			{"header":"4E",
			 "product":"0002",
			 "type":"D002",
			 "id":"1020ABC01",
			 "typeCmd":"FF",
			 "cmd":"01",
			 "receita":[1,""]}

###### Configurações

	02 - Subcabeçalho para solicitar as configurações do dispositivo:	
     
	- Exemplo de solicitação:
     
			{"header":"4E",
			 "product":"0002",
			 "type":"D002",
			 "id":"1020ABC01",
			 "typeCmd":"FF",
			 "cmd":"02"}
     
	- Exemplo de resposta:
      
			{"header":"4E",
			 "product":"0002",
			 "type":"D002",
			 "id":"1020ABC01",
			 "typeCmd":"FF",
			 "cmd":"02",
			 "qtdeMaxPatamar":"7",
			 "pwmSetpoint":"95",
			 "tempoEnvioBrassagem":"1",
			 "grauPorMinuto":"1.0",
			 "code":["A2"]}

	0D - Subcabeçalho para solicitar o fator de calibração:
     
	- Exemplo:
     
			{"header":"4E",
			 "product":"0002",
			 "type":"D002",
			 "typeCmd":"FF",
			 "cmd":"0D",
			 "calFactor":"4.5",
			 "code":["A2"]}

	0E - Subcabeçalho para solicitar o tempo de envio da leitura AC:
     
	- Exemplo:
     
			{"header":"4E",
			 "product":"0002",
			 "type":"D002",
			 "typeCmd":"FF",
			 "cmd":"0E",
			 "time":"10",
			 "code":["A2"]}

	0F - Subcabeçalho para solicitar o tempo de envio da temperatura:
     
	- Exemplo:
     
			{"header":"4E",
			 "product":"0002",
			 "type":"D002",
			 "typeCmd":"FF",
			 "cmd":"0F",
			 "time":"10",
			 "code":["A2"]}

	10 - Subcabeçalho para solicitar a configuração do PID:
			
	- Exemplo:
     
			{"header":"4E",
			 "product":"0002",
			 "type":"D002",
			 "typeCmd":"FF",
			 "cmd":"10",
			 "status":"01",
			 "kp":"1.5",
			 "ki":"2.5",
			 "kd":"3.5",
			 "code":["A2"]}

	FF - Subcabeçalho para solicitar o número de série/Id:
     
	- Exemplo:
     
			{"header":"4E",
			 "product":"0002",
			 "type":"D002",
			 "typeCmd":"FF",
			 "cmd":"FF"}

###### Receitas

	03 - Subcabeçalho para solicitar a tela atual:	
     
	- Exemplo:
     
			{"header":"4E",
			 "product":"0002",
			 "type":"D002",
			 "id":"1020ABC01",
			 "typeCmd":"FF",
			 "cmd":"01",
			 "tela":"Menu Principal",
			 "opcao":"Escolhe receita"}

#### Respostas à comandos e solicitações

###### Tabela de respostas

| Código   | Descrição                                          |
|----------|----------------------------------------------------|
| A1       | Código de comando aceito e deserealização completa |
| A2       | Código de solicitação aceita                       |
| E1       | Código de entrada serial incompleta                |
| E2       | Código de entrada inválida                         |
| E3       | Código de memória insuficiente                     |
| E4       | Código de comando não suportado                    | 
| E5       | Código de "Too Deep"                               |
| E6       | Código de falha na deserealização                  |
| E7       | Código de cabeçalho desconhecido                   |
| E8       | Código de comando desconhecido                     |
| E9       | Código de solicitação desconhecida                 |
| E10      | Código de memória cheia                            |
| E11      | Código de memória vazia                            |
| E12      | Código de receita inexistente                      |
| E13      | Código de nome de receita já existente             |
| E14      | Código de brassagem: aguardar o fim para comandos  |
| E15      | Código de número de série inválido					|

#### Dados enviados por serial durante brassagem

* À cada 1 segundo, durante a brassagem, o equipamento envia via serial os dados da brassagem.

-> Patamares:
	                        	        
	- Exemplo:
            
			{"header":"4E",
			 "product":"0002",
			 "type":"D002",
			 "typeCmd":"FF",
			 "cmd":2,
			 "temp":25,
			 "status":"PATAMAR",
			 "pwm":80,
			 "speed":100,
			 "patamar":1,
			 "tempo":60, (segundos)
			 "cmd":"A1"}

-> Fervura/adições:
	                        	        
	- Exemplo:
            
			{"header":"4E",
			 "product":"0002",
			 "type":"D002",
			 "typeCmd":"FF",
			 "cmd":2,
			 "restante":5000,       (tempo restante, segundos)
			 "proximo":60,          (tempo adição, segundos)
			 "nAdicao":1,
			 "tempo":60,            (segundos)
			 "cmd":"A1"}

-> Whirpool:
	                        	        
	                        	        
	- Exemplo:
            
			{"header":"4E",
			 "product":"0002",
			 "type":"D002",
			 "typeCmd":"FF",
			 "cmd":2,
			 "restante":5000,       (tempo restante, segundos)
			 "proximo":60,          (tempo whirpool, segundos)
			 "nWhirlpool":1,
			 "tempo":60,            (segundos)
			 "cmd":"A1"}

-> Descanso:
	                        	        
	                        	        
	- Exemplo:
            
			{"header":"4E",
			 "product":"0002",
			 "type":"D002",
			 "typeCmd":"FF",
			 "cmd":2,
			 "restante":5000,       (tempo restante, segundos)
			 "proximo":60,          (tempo descanso, segundos)
			 "nDescanso":1,
			 "tempo":60,            (segundos)
			 "cmd":"A1"}

#### EE - Cabeçalho de evento

###### Faixa de cabeçalhos EE

| Nome                   | Tipo ("typeCmd") | Faixa ("cmd")|
|------------------------|------------------|--------------|
| Keep Alive			 | EE 				| 00		   |
| Entradas digitais      | EE   	 	    | 01       	   |
| Leitura da temperatura | EE   	 	    | 02           |
| Fluxo			         | EE   	 	    | 03           |
| Leitura AC	         | EE   	 	    | 04           |

###### Keep Alive

	00 - Subcabeçalho para evento de keep alive:	

	- Exemplo: 
     
			{"header":"4E",
			 "product":"0002",
			 "type":"D002",
			 "id":"1020ABC01",
			 "typeCmd":"EE",
			 "cmd":"00"}

	*OBS: o dado enviado será exatamente igual, tanto de APP -> equipamento quanto de equipamento -> APP

###### Entradas digitais

	01 - Subcabeçalho para evento de leitura da porta digital:	
		
	- Exemplo:
     
			{"header":"4E",
			 "product":"0002",
			 "type":"D002",
			 "id":"1020ABC01",
			 "typeCmd":"EE",
			 "cmd":"01",
			 "port":"02",
			 "state":"HIGH"}

	- Temos duas portas, logo "entrada - port" varia entre 01 e 02;
	- Temos dois estados, logo "estado - state" varia entre LOW e HIGH

###### Leitura da temperatura

	02 - Subcabeçalho para evento de leitura de temperatura:	
			
	- Exemplo:
     
			{"header":"4E",
			 "product":"0002",
			 "type":"D002",
			 "id":"1020ABC01",
			 "typeCmd":"EE",
			 "cmd":"02",
			 "temp":"25.0"}

###### Fluxo

	03 - Subcabeçalho para evento de leitura de fluxo:	

	- Exemplo:
     
			{"header":"4E",
			 "product":"0002",
			 "type":"D002",
			 "id":"1020ABC01",
			 "typeCmd":"EE",
			 "cmd":"03",
			 "flow":"0.00",
			 "liters":"0.00"}

###### Leitura AC

	04 - Subcabeçalho para evento de leitura AC: 00 - não presente | 01 - presente	

	- Exemplo:
     
			{"header":"4E",
			 "product":"0002",
			 "type":"D002",
			 "id":"1020ABC01",
			 "typeCmd":"EE",
			 "cmd":"04",
			 "ac":"01"}

#### Envio das telas atuais do Hardware para o app

* À cada mudança de tela, o equipamento enviará a tela atual para o app.
	                        	        
	- Exemplo:
            
			{"header":"4E",
       		 "product":"0002",  
			 "type":"D002",                      
			 "tela": "Menu Principal"}

#### Tabela de descrição e especificação das telas

| Tela                    | Descrição 										             |
|-------------------------|--------------------------------------------------------------|
| Stand by                | Tela de apresentação, descanso						         |
| Menu Principal          | Escolher receita, criar receita, Adicionar água, Configuração|
| Menu receitas           | Receitas presentes na controladora              		     |
| Menu da receita         | Brassar, editar ou apagar a receita selecionada				 | 
| Criar receita           | Parâmetros de criação de uma nova receita					 |
| Editar receitas         | Parâmetros de edição de uma receita	existente				 |
| Configurar data e hora  | Configurar os parâmetros de data e hora do dispositivo		 |
| Adicionar água          | Adição de água automática, seleção da quantidade em litros	 |
  

#### Tabelas de máximos, mínimos e defaults dos parâmetros

| Patamares     | Máximo | Mínimo | Default |
|---------------|--------|--------|---------|
| Quantidade    |   10   |    1   |    2    |
| Tempo (min)   |  300   |    0   |    5    |
| Temperatura   |  100   |    1   |   25    |

| Fervura       | Máximo | Mínimo | Default |
|---------------|--------|--------|---------|
| Temperatura   |  200   |   25   |   100   |
| Tempo (min)   |  500   |    1   |    60   |

| Adições       | Máximo | Mínimo | Default |
|---------------|--------|--------|---------|
| Total         |    5   |    0   |     1   |
| Quantidade(g) |  500   |    1   |    10   |
| Tempo (min)   |   60   |    0   |    15   |

| Whirpool      | Máximo | Mínimo | Default |
|---------------|--------|--------|---------|
| Total         |    2   |    0   |     2   |
| Tempo (min)   |   60   |    0   |    10   |

| Descanso      | Máximo | Mínimo | Default |
|---------------|--------|--------|---------|
| Total         |    1   |    0   |     1   |
| Tempo (min)   |   60   |    0   |    10   |



