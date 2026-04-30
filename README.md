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

### Estado atual

- A recepcao serial foi corrigida para nao deixar interrupcoes desligadas ao final de um ciclo incompleto de leitura.
- O caminho principal de JSON foi reduzido para `StaticJsonDocument` em vez de `DynamicJsonDocument` nos fluxos mais importantes de serial, APP e resposta remota.
- As chaves de autenticacao e identificadores sensiveis foram movidos para configuracao persistida em EEPROM, com valores de bootstrap definidos no firmware.
- O protocolo serial agora documenta o tamanho maximo do pacote (`DATA_UTIL_SERIAL = 512`) e os campos principais esperados.
- O logger Node.js passou a lidar com framings por linha e limite de buffer, reduzindo erros de leitura parcial.
- As rotinas de display e relogio reduziram copias desnecessarias de `String` nos caminhos mais usados.
- A arvore `ENSABrewBuffaloBeer/` segue documentada como legado, para evitar manutencao dupla.

### Ajustes adicionais que ainda valem

- Reduzir o uso de `String` nas rotinas de receita e nas comparacoes de nome ainda espalhadas pelo firmware.
- Reduzir o uso restante de `String` nas telas de configuracao, edicao de receita e mensagens dinamicas do display.
- Tratar explicitamente pacote incompleto, pacote excedido e erro de autenticacao, sem depender de comportamento implcito do loop.
- Separar melhor codigo do projeto e dependencias vendorizadas, para facilitar manutencao e auditoria.
- Manter o contrato do protocolo centralizado no arquivo `Protocol/protocolo ENSABrew.md`.

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

### Current state

- Serial reception has been corrected so it no longer leaves interrupts disabled after an incomplete read cycle.
- The main JSON path has been reduced to `StaticJsonDocument` instead of `DynamicJsonDocument` in the main serial, APP and remote-reply flows.
- Authentication keys and sensitive identifiers were moved into persisted EEPROM-backed configuration, with firmware bootstrap defaults.
- The serial protocol now documents the maximum packet size (`DATA_UTIL_SERIAL = 512`) and the main expected fields.
- The Node.js logger now handles line-based framing and a bounded buffer to reduce partial-read errors.
- Display and clock routines now avoid unnecessary `String` copies in the most-used paths.
- The `ENSABrewBuffaloBeer/` tree remains documented as legacy to avoid maintaining two active code paths.

### Additional code improvements worth doing

- Reduce `String` usage in recipe routines and in the remaining name comparison helpers in the firmware.
- Reduce the remaining `String` usage in configuration screens, recipe editing and dynamic display messages.
- Handle incomplete packets, oversized packets and authentication failures explicitly instead of relying on loop behavior.
- Separate project code from vendored dependencies more clearly to simplify maintenance and auditability.
- Keep the protocol contract centralized in `Protocol/protocolo ENSABrew.md`.

### Note

The points above were inferred from static code and documentation review, not from hardware testing.

### License

See `LICENSE`.
