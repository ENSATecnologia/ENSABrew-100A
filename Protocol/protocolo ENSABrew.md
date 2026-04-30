# Protocolo ENSABrew / ENSABrew Protocol

## PT-BR

### Visao geral

O firmware ENSABrew usa mensagens JSON sobre a serial. O pacote de entrada deve respeitar o formato esperado pelo firmware principal em `ENSABrewFirmware/ATMEL/ENSABrew/`.

### Estrutura base

Todo pacote deve conter, no minimo:

- `header`
- `product`
- `type`
- `id`
- `typeCmd`
- `cmd`

Dependendo do comando, o pacote tambem pode conter:

- `key`
- `newId`
- `receita`
- `tempPatamar`
- `minPatamar`
- `tempFervura`
- `minFervura`
- `nomeAdicao`
- `gramasAdicao`
- `minAdicao`
- `minWhirlpool`
- `minDescanso`

### Tamanho maximo

- O buffer serial do firmware ativo e `DATA_UTIL_SERIAL = 512` bytes.
- Pacotes maiores que esse limite nao devem ser enviados.
- Pacotes incompletos ou truncados devem ser tratados como erro de leitura.

### Atualizacao de ID

Para alterar o `id` do modulo, o pacote deve usar:

- `typeCmd = "CC"`
- `cmd = "FF"`
- `key = "excENSA"`
- `newId = "<novo id>"`

O firmware compara `key` com a chave autenticada armazenada em configuracao persistida.

### Consulta de ID

Para consultar o `id` atual:

- `typeCmd = "FF"`
- `cmd = "FF"`

### Erros esperados

O firmware deve rejeitar:

- pacote sem campos obrigatorios
- `key` invalida
- `id` divergente do modulo configurado
- pacote maior que o limite
- pacote com JSON invalido

### Observacao

`ENSABrewBuffaloBeer/` deve ser tratado como arvore legada. O contrato de protocolo deve ser mantido a partir da base ativa `ENSABrew/`.

## EN

### Overview

The ENSABrew firmware exchanges JSON messages over serial. Incoming packets must match the format expected by the active firmware tree in `ENSABrewFirmware/ATMEL/ENSABrew/`.

### Base structure

Every packet must include, at minimum:

- `header`
- `product`
- `type`
- `id`
- `typeCmd`
- `cmd`

Depending on the command, the packet may also include:

- `key`
- `newId`
- `receita`
- `tempPatamar`
- `minPatamar`
- `tempFervura`
- `minFervura`
- `nomeAdicao`
- `gramasAdicao`
- `minAdicao`
- `minWhirlpool`
- `minDescanso`

### Maximum size

- The active firmware serial buffer is `DATA_UTIL_SERIAL = 512` bytes.
- Packets larger than that limit must not be sent.
- Incomplete or truncated packets must be treated as read errors.

### ID update

To change the module `id`, the packet must use:

- `typeCmd = "CC"`
- `cmd = "FF"`
- `key = "excENSA"`
- `newId = "<new id>"`

The firmware compares `key` against the authenticated key stored in persisted configuration.

### ID query

To query the current `id`:

- `typeCmd = "FF"`
- `cmd = "FF"`

### Expected errors

The firmware should reject:

- packets missing required fields
- invalid `key`
- `id` different from the configured module id
- packets above the size limit
- invalid JSON packets

### Note

`ENSABrewBuffaloBeer/` must be treated as a legacy tree. The protocol contract should be maintained from the active `ENSABrew/` branch of the firmware.
