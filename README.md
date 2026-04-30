# ENSABrew-100A

Projeto ENSA Tecnologia para controle de brassagem e producao, com firmware ATMEL/Arduino, protocolo serial e logger Node.js.

## Visao geral

O repositorio reune firmware, bibliotecas, protocolo, projeto mecanico, PCB e logger operacional para o equipamento ENSABrew-100A.

## Estrutura

| Caminho | Descricao |
| --- | --- |
| `ENSABrewFirmware/` | Firmware ativo e variantes historicas |
| `ENSABrewLibraries/` | Bibliotecas de apoio |
| `LoggerBuffaloBrew/` | Logger operacional Node.js |
| `ENSABrewPCB/` | Arquivos de PCB |
| `ENSABrewMechanicalProject/` | Projeto mecanico |
| `Protocol/` e `ENSABrewProtocol/` | Documentacao de protocolo |

## Pontos de atencao

Status verificado no codigo, considerando o firmware ativo em `ENSABrewFirmware/ATMEL/ENSABrew/`:

- Recepcao serial em maquina de estados nao bloqueante: parcialmente resolvida. Existe controle por buffer e fim de pacote em `ENSABrewSerial.ino`, mas ainda ha uso de interrupcoes desabilitadas durante a leitura e a variante `ENSABrewBuffaloBeer` continua sem a mesma protecao de limite.
- `DynamicJsonDocument` e uso extensivo de `String` em AVR: nao resolvido. O firmware ativo ainda usa `DynamicJsonDocument` em pontos criticos de serial, JSON e comunicacao APP.
- Senhas hardcoded: nao resolvido. `SENHA_MASTER` e `SENHA_USER` continuam definidos no codigo, e o protocolo documenta uma chave fixa de autenticacao.
- Tamanho maximo de pacote serial: parcialmente resolvido. O codigo define `DATA_UTIL_SERIAL = 512`, mas esse limite ainda nao esta documentado de forma explicita no protocolo.

## Observacao

As conclusoes acima sao de leitura estatica do codigo, nao de teste em hardware.

## Licenca

Consulte `LICENSE`.
