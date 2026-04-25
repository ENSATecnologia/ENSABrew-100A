# ENSABrew-100A

<!-- ENSA-AUDIT-README:START -->

---

## 🔍 Visao Geral Tecnica

Projeto com firmware ATMEL/Arduino para controle de brassagem/producao e logger Node.js para registros operacionais.

## 🛠 Status da Auditoria

- ✅ Firmware e logger analisados.
- ✅ Correcoes seguras aplicadas no parser serial e no logger.
- ⚠️ Ainda exige revisao arquitetural da recepcao serial com interrupcoes desabilitadas.

## ✅ Correcoes Aplicadas

- Adicionado descarte defensivo para pacote serial maior que o buffer em `ENSABrewSerial.ino`.
- Corrigida validacao de campos no `loggerBrew.js`.
- Corrigido typo `enconding` para `encoding`.
- Sanitizado nome de arquivo informado pelo usuario no logger.

## ⚠️ Pontos de Atencao

- Evitar `DynamicJsonDocument` e uso extensivo de `String` em AVR quando possivel.
- Rever senhas curtas e hardcoded (`SENHA_MASTER`, `SENHA_USER`).
- Documentar protocolo serial e tamanho maximo de pacote.

## 🚀 Proximos Passos

1. Refatorar recepcao serial para maquina de estados nao bloqueante.
2. Separar variantes antigas, bibliotecas externas e firmware ativo.
3. Adicionar `package.json` para o logger Node.

<!-- ENSA-AUDIT-README:END -->
