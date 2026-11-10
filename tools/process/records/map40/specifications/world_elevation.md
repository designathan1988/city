# world:elevation

Grupo: world. Entrada: subsistema. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/world/elevation.ts`
- Unidades próprias (só esta funcionalidade alcança): 0 em 0 arquivo(s), 0 linhas
- Alcance total: 359 unidades; usa 244 compartilhadas de outras; é dona principal de 115 compartilhadas

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
- Lê: `RoadNode.heightOffset`, `RoadNode.incident`, `RoadNode.smooth`, `RoadNode.x`, `RoadNode.y`, `RoadSegment.a`, `RoadSegment.b`, `RoadSegment.cutWalls`, `RoadSegment.structure`, `RoadSegment.type`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/editor/buildPreview.spec.ts`, `tests/editor/freeRoad.spec.ts`, `tests/editor/growthWorker.spec.ts`, `tests/perf/budgets.spec.ts`, `tests/render/cellPaving.spec.ts`, `tests/render/grassPlacement.spec.ts`, `tests/render/pedSlope.spec.ts`, `tests/render/pierOnRoad.spec.ts`, `tests/render/roadPasses.spec.ts`, `tests/render/roadStandIn.spec.ts`, `tests/render/roadTileWorkers.spec.ts`, `tests/render/roadTiles.spec.ts`, `tests/render/roadsRead.spec.ts`, `tests/render/signLayer.spec.ts`, `tests/render/streetFurnitureLayer.spec.ts`, `tests/render/surfaceInputsResidency.spec.ts`, `tests/render/surfaceReads.spec.ts`, `tests/render/terrainShaping.spec.ts`, `tests/render/vehicleFrame.spec.ts`, `tests/render/warmLoose.spec.ts`, `tests/scale/census/derivedBound.spec.ts`, `tests/scale/roadJudge.spec.ts`, `tests/scale/roadKeys.spec.ts`, `tests/world/buildingFloors.spec.ts`, `tests/world/cutWalls.spec.ts`, `tests/world/elevation.spec.ts`, `tests/world/elevationDifferences.spec.ts`, `tests/world/elevationGround.spec.ts`, `tests/world/elevationLocal.spec.ts`, `tests/world/elevationScope.spec.ts`, `tests/world/incrementalRebuild.spec.ts`, `tests/world/pageWorldMemory.spec.ts`, `tests/world/roadProfile.spec.ts`, `tests/world/roadStations.spec.ts`, `tests/world/roadTuning.spec.ts`, `tests/world/shopFloor.spec.ts`, `tests/world/steppedYards.spec.ts`, `tests/world/verticalCurves.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `road-elevation`: a portar 116
- porte `roads`: não se aplica 116

## Código próprio (linhas — declaração — comentário)
