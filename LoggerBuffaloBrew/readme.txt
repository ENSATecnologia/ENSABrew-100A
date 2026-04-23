-> Descrição do dado: (NOME DA RECEITA) | (RAMPA/SETPOINT) | (N° RAMPA/SETPOINT) | (TEMP. SENSOR) | (TEMP. SETPOINT) | (TEMPO ATUAL) | (ESTADO RELÉ)
	
	- (NOME DA RECEITA): Nome da receita atual;
	
	- (RAMPA/SETPOINT): Se refere a etapa em si, em qual delas está, ou RAMPA (subindo a temperatura) ou SETPOINT (contando tempo e mantendo a temperatura);
	
	- (N° RAMPA/SETPOINT): Se refere ao número/ID da RAMPA ou do SETPOINT, ambos serão os mesmos;

	- (TEMP. SENSOR): Temperatura atual do SENSOR;
	
	- (TEMP. SETPOINT): Temperatura atual do SETPOINT (será fixa na contagem de tempo);

	- (TEMPO ATUAL): tempo total do SETPOINT em questão (em minutos). Enquanto estiver em RAMPA, se mantém travado até chegar ao SETPOINT e chegando ao mesmo, faz uma contagem decrescente;

	- (ESTADO RELÉ): Estado atual do relé: (1) acionado | (0) desacionado



* OBSERVAÇÃO:
	
	-> Caso queira alterar a PORTA de conexão com o servidor, acesse o arquivo 'conectionPort.js' e altere diretamente o valor;