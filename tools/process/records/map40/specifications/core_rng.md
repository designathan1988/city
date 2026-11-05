# core:rng

Grupo: core. Entrada: subsistema. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`; a lista de código abaixo é rastreável: arquivo e linhas de cada declaração própria.

- Sementes: `src/core/rng.ts`
- Unidades próprias (só esta funcionalidade alcança): 4 em 1 arquivo(s), 28 linhas
- Alcance total: 20 unidades; usa 1 compartilhadas de outras; é dona principal de 15 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Interage com: `core:scalar` (1)

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/core/axisCut.spec.ts`, `tests/core/isSimple.spec.ts`, `tests/core/mulberry32.spec.ts`, `tests/core/weightedSlots.spec.ts`, `tests/editor/history.spec.ts`, `tests/editor/pickAlongRay.spec.ts`, `tests/people/hairPinning.spec.ts`, `tests/perf/trafficBuildingsBench.spec.ts`, `tests/render/gardenCull.spec.ts`, `tests/render/gardenFrame.spec.ts`, `tests/render/instanceRuns.spec.ts`, `tests/render/lawnGround.spec.ts`, `tests/render/plateFit.spec.ts`, `tests/render/ragdollAxes.spec.ts`, `tests/render/signLayer.spec.ts`, `tests/render/streetFurnitureLayer.spec.ts`, `tests/render/terrainSlope.spec.ts`, `tests/render/vehiclesInView.spec.ts`, `tests/sim/agents/lotParked.spec.ts`, `tests/sim/agents/reedsShepp.spec.ts`, `tests/sim/ambientIndex.spec.ts`, `tests/sim/drivers.spec.ts`, `tests/sim/fleetGrid.spec.ts`, `tests/sim/indexes/audit.spec.ts`, `tests/sim/indexes/baysByDiary.spec.ts`, `tests/sim/indexes/cyclesHeap.spec.ts`, `tests/sim/indexes/footwayIndex.spec.ts`, `tests/sim/indexes/levelCrossings.spec.ts`, `tests/sim/indexes/lorries.spec.ts`, `tests/sim/indexes/lotGridSame.spec.ts`, `tests/sim/lotPlaces.spec.ts`, `tests/sim/meso/model.spec.ts`, `tests/sim/microWindowCells.spec.ts`, `tests/sim/resetLikeNew.spec.ts`, `tests/sim/routing/denseSearch.spec.ts`, `tests/sim/routing/routeIndex.spec.ts`, `tests/sim/signalPosts.spec.ts`, `tests/sim/trafficBuildings.spec.ts`, `tests/sim/vehiclesNearWork.spec.ts`, `tests/sim/walkSpine.spec.ts`, `tests/sim/walkingGolden.spec.ts`, `tests/sim/yardSlices.spec.ts`, `tests/world/architecture.spec.ts`, `tests/world/changeSweep.spec.ts`, `tests/world/crossingLandings.spec.ts`, `tests/world/elevationLocal.spec.ts`, `tests/world/junctionConnectors.spec.ts`, `tests/world/landscapeBatch.spec.ts`, `tests/world/lotStore.spec.ts`, `tests/world/nearSearch.spec.ts`, `tests/world/openLots.spec.ts`, `tests/world/roadFieldPrune.spec.ts`, `tests/world/roadValueLedger.spec.ts`, `tests/world/transitNear.spec.ts`, `tests/world/trees.spec.ts`, `tests/world/variety.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `build`: não se aplica 4
- porte `people`: não se aplica 4

## Código próprio (arquivo:linhas — declaração — comentário)

### src/core/rng.ts
- 12-27 `Rng.constructor` (method)
- 71-73 `Rng.bool` (method)
- 75-78 `Rng.pick` (method)
- 81-85 `Rng.normal` (method): Box-Muller normal deviate.
