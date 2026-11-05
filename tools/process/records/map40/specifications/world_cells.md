# world:cells

Grupo: world. Entrada: subsistema. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/world/cells.ts`
- Unidades próprias (só esta funcionalidade alcança): 2 em 1 arquivo(s), 6 linhas
- Alcance total: 50 unidades; usa 9 compartilhadas de outras; é dona principal de 39 compartilhadas

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
- `tests/editor/binarySave.spec.ts`, `tests/editor/chunkedSave.spec.ts`, `tests/world/cellFilingAny.spec.ts`, `tests/world/forgetCrossings.spec.ts`, `tests/world/poleLinesCells.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `roads`: não portado 2

## Código próprio (linhas — declaração — comentário)

### src/world/cells.ts
- 71-73 `CellFiling.size` (method)
- 115-117 `CellFiling.holds` (method): Whether `id` is filed for exactly `item`.
