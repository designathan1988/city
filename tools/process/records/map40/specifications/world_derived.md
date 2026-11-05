# world:derived

Grupo: world. Entrada: subsistema. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/world/derived.ts`
- Unidades próprias (só esta funcionalidade alcança): 4 em 1 arquivo(s), 12 linhas
- Alcance total: 60 unidades; usa 30 compartilhadas de outras; é dona principal de 26 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- nenhuma chamada direta da raiz: chega por outra funcionalidade

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: -
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/world/changeGuards.spec.ts`, `tests/world/changeSweep.spec.ts`, `tests/world/journalReaders.spec.ts`, `tests/world/landscapeBatch.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `traffic`: já existia 2

## Código próprio (linhas — declaração — comentário)

### src/world/derived.ts
- 11-13 `JournalCursor.seenSerial` (method): The serial last read (`JournalReader`): its diary keeps what came after it for it.
- 84-86 `Derived.entries` (method)
- 88-90 `Derived.size` (method)
- 139-141 `ChangeSweep.seenSerial` (method): What it has read up to (`JournalReader`): its diary keeps what came after it until the next `advance`.
