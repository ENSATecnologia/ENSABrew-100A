# ENSABrew-100A

## PT-BR

Projeto ENSA Tecnologia para controle de brassagem e producao, com firmware ATMEL/Arduino, protocolo serial e logger Node.js.

### Visao geral

O repositorio reune firmware, bibliotecas, protocolo, projeto mecanico, PCB e logger operacional para o equipamento ENSABrew-100A.

### Estrutura

| Caminho | Descricao |
| --- | --- |
| `ENSABrewFirmware/` | Firmware principal e variantes historicas |
| `ENSABrewLibraries/` | Bibliotecas compartilhadas |
| `LoggerBuffaloBrew/` | Logger operacional Node.js |
| `ENSABrewPCB/` | Arquivos de PCB |
| `ENSABrewMechanicalProject/` | Projeto mecanico |
| `Protocol/` e `ENSABrewProtocol/` | Documentacao de protocolo |

### Diretriz de manutencao

`ENSABrew/` e a base ativa de manutencao. `ENSABrewBuffaloBeer/` deve ser tratada como arvore legada/historica, mantida apenas para referencia e comparacao. Novas mudancas devem entrar em `ENSABrew/`, salvo necessidade explicita de suporte a essa variante antiga.

### Resultado da auditoria

- A recepcao serial foi corrigida para nao deixar interrupcoes desligadas ao final de um ciclo incompleto de leitura. A protecao de buffer tambem ficou consistente nas duas arvores de firmware.
- O uso de `DynamicJsonDocument` e `String` em AVR continua presente em pontos criticos de serial e comunicacao APP.
- Senhas e identificadores sensiveis continuam hardcoded, inclusive `SENHA_MASTER`, `SENHA_USER` e o `DEFAULT_ID_MODULE`.
- O tamanho maximo do pacote serial existe no codigo (`DATA_UTIL_SERIAL = 512`), mas nao esta descrito de forma clara no protocolo.
- Existem duas arvores de firmware com divergencias funcionais. A documentacao precisa tratar `ENSABrewBuffaloBeer/` como historica para evitar manutencao dupla.

### Ajustes adicionais que ainda valem

- Centralizar as constantes do protocolo e documentar formato, tamanho e validacao do pacote em um unico lugar.
- Trocar credenciais fixas por configuracao persistida, provisionamento ou leitura segura de EEPROM.
- Reduzir o uso de `String` no firmware principal, especialmente nas rotinas de serial, exibicao e montagem de JSON.
- Tratar explicitamente pacote incompleto, pacote excedido e erro de autenticao, sem depender de comportamento implcito do loop.
- Separar melhor codigo do projeto e dependencias vendorizadas, para facilitar manutencao e auditoria.
- Remover ambiguidade entre o firmware ativo e a arvore legada, para que o caminho recomendado de manutencao fique obvio.

### Observacao

Os pontos acima foram inferidos por leitura estatica do codigo e da documentacao, nao por teste em hardware.

### Licenca

Consulte `LICENSE`.

---

## English

ENSA Tecnologia project for brewing/production control, with ATMEL/Arduino firmware, serial protocol and Node.js logger.

### Overview

This repository gathers firmware, libraries, protocol, mechanical project, PCB and the operational logger for the ENSABrew-100A equipment.

### Structure

| Path | Description |
| --- | --- |
| `ENSABrewFirmware/` | Main firmware and historical variants |
| `ENSABrewLibraries/` | Shared libraries |
| `LoggerBuffaloBrew/` | Node.js operational logger |
| `ENSABrewPCB/` | PCB files |
| `ENSABrewMechanicalProject/` | Mechanical project |
| `Protocol/` and `ENSABrewProtocol/` | Protocol documentation |

### Maintenance direction

`ENSABrew/` is the active maintenance base. `ENSABrewBuffaloBeer/` should be treated as a legacy/historical tree, kept only for reference and comparison. New changes should land in `ENSABrew/` unless there is an explicit need to support that older variant.

### Audit result

- Serial reception has been corrected so it no longer leaves interrupts disabled after an incomplete read cycle. Buffer protection is now consistent across both firmware trees.
- `DynamicJsonDocument` and `String` usage is still present in critical AVR serial and APP communication paths.
- Sensitive values are still hardcoded, including `SENHA_MASTER`, `SENHA_USER` and `DEFAULT_ID_MODULE`.
- The maximum serial packet size exists in code (`DATA_UTIL_SERIAL = 512`), but it is not clearly documented in the protocol.
- There are two firmware trees with functional divergence. The documentation should classify `ENSABrewBuffaloBeer/` as historical to avoid maintaining two active code paths.

### Additional code improvements worth doing

- Centralize protocol constants and document packet format, size and validation in one place.
- Replace fixed credentials with persisted configuration, provisioning or secure EEPROM-backed settings.
- Reduce `String` usage in the main firmware, especially in serial, display and JSON assembly paths.
- Handle incomplete packets, oversized packets and authentication failures explicitly instead of relying on loop behavior.
- Separate project code from vendored dependencies more clearly to simplify maintenance and auditability.
- Remove ambiguity between the active firmware and the legacy tree so the recommended maintenance path stays obvious.

### Note

The points above were inferred from static code and documentation review, not from hardware testing.

### License

See `LICENSE`.
