# ENSABrew-100A

## 🇧🇷 PT-BR

Projeto ENSA Tecnologia para controle de brassagem/produção, com firmware ATMEL/Arduino, protocolo serial e logger Node.js.

### Visão Geral

O repositório reúne firmware, bibliotecas, protocolo, projeto mecânico, PCB e logger operacional para o equipamento ENSABrew-100A.

### Funcionalidades

- Firmware embarcado para controle de processo.
- Comunicação serial por protocolo próprio.
- Logger Node.js para registros operacionais.
- Documentação de fluxo e protocolo.
- Materiais de hardware, mecânica e PCB.

### Estrutura

| Caminho | Descrição |
| --- | --- |
| `ENSABrewFirmware/` | Firmware ativo |
| `ENSABrewLibraries/` | Bibliotecas de apoio |
| `LoggerBuffaloBrew/` | Logger operacional Node.js |
| `ENSABrewPCB/` | Arquivos de PCB |
| `ENSABrewMechanicalProject/` | Projeto mecânico |
| `Protocol/` e `ENSABrewProtocol/` | Documentação de protocolo |

### Pontos de Atenção

- Refatorar recepção serial para máquina de estados não bloqueante.
- Evitar `DynamicJsonDocument` e uso extensivo de `String` em AVR.
- Revisar senhas hardcoded antes de produção.
- Documentar tamanho máximo de pacote serial.

### Licença

Consulte `LICENSE`.

---

## 🇺🇸 English

ENSA Tecnologia project for brewing/production control, with ATMEL/Arduino firmware, serial protocol and Node.js logger.

### Overview

This repository gathers firmware, libraries, protocol, mechanical project, PCB and operational logger for the ENSABrew-100A equipment.

### Features

- Embedded firmware for process control.
- Custom serial protocol communication.
- Node.js logger for operational records.
- Flow and protocol documentation.
- Hardware, mechanical and PCB materials.

### Structure

| Path | Description |
| --- | --- |
| `ENSABrewFirmware/` | Active firmware |
| `ENSABrewLibraries/` | Support libraries |
| `LoggerBuffaloBrew/` | Operational Node.js logger |
| `ENSABrewPCB/` | PCB files |
| `ENSABrewMechanicalProject/` | Mechanical project |
| `Protocol/` and `ENSABrewProtocol/` | Protocol documentation |

### Attention Points

- Refactor serial reception into a non-blocking state machine.
- Avoid `DynamicJsonDocument` and extensive `String` usage on AVR.
- Review hardcoded passwords before production.
- Document maximum serial packet size.

### License

See `LICENSE`.
