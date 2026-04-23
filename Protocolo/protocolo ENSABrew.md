#### Protocolo exclusivo - ENSA 

CC/FF - Subcabeçalho para alterar o número de série/Id:
     
	- Exemplo:
     
			{"header":"4E",
			 "product":"0002",
			 "type":"D002",
			 "id":"1020ABC01"
			 "typeCmd":"CC",
			 "cmd":"FF",
			 "key":"excENSA",
			 "newId":"123456ABC"}

FF/FF - Subcabeçalho para solicitar o número de série/Id:
     
	- Exemplo:
     
			{"header":"4E",
			 "product":"0002",
			 "type":"D002",
			 "typeCmd":"FF",
			 "cmd":"FF"}