# world:changes

Grupo: world. Entrada: subsistema. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/world/changes.ts`
- Unidades próprias (só esta funcionalidade alcança): 2 em 1 arquivo(s), 12 linhas
- Alcance total: 57 unidades; usa 0 compartilhadas de outras; é dona principal de 55 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#IfStatement@4004` (3), `src/main.ts#healthWatch@173` (1), `src/main.ts#wireProfiler@699` (1), `src/main.ts#drawChanges@2912` (1), `src/main.ts#healthPanel@3987` (1)

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
- `tests/render/agentsParked.spec.ts`, `tests/render/agentsResidency.spec.ts`, `tests/render/coverIndex.spec.ts`, `tests/render/elementCells.spec.ts`, `tests/render/lotOverlay.spec.ts`, `tests/render/pageGarbage.spec.ts`, `tests/render/plateNoWater.spec.ts`, `tests/render/plateWoods.spec.ts`, `tests/render/shoreTextures.spec.ts`, `tests/render/signLayer.spec.ts`, `tests/render/streetFurnitureLayer.spec.ts`, `tests/render/vehicleCalls.spec.ts`, `tests/render/vehiclesInView.spec.ts`, `tests/scale/buildingEdit.spec.ts`, `tests/sim/ambientIndex.spec.ts`, `tests/world/cellFilingAny.spec.ts`, `tests/world/changeGuards.spec.ts`, `tests/world/changeSweep.spec.ts`, `tests/world/changes.spec.ts`, `tests/world/diaryPlaces.spec.ts`, `tests/world/docFork.spec.ts`, `tests/world/docTiles.spec.ts`, `tests/world/forgetCrossings.spec.ts`, `tests/world/journalBound.spec.ts`, `tests/world/journalReaders.spec.ts`, `tests/world/lotRaster.spec.ts`, `tests/world/lotStore.spec.ts`, `tests/world/poleLinesKept.spec.ts`, `tests/world/roadEntries.spec.ts`, `tests/world/tileEntries.spec.ts`, `tests/world/unboundedPlates.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `build`: a portar 2
- porte `roads`: não portado 2

## Código próprio (linhas — declaração — comentário)

### src/world/changes.ts
- 367-374 `ChangeJournal.latest` (method): The last `n` entries, newest first.
- 377-380 `ChangeJournal.get` (method): An entry by its serial, while it is kept.
