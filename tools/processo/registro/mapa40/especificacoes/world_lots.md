# world:lots

Grupo: world. Entrada: subsistema. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/world/lots.ts`
- Unidades próprias (só esta funcionalidade alcança): 3 em 1 arquivo(s), 32 linhas
- Alcance total: 561 unidades; usa 434 compartilhadas de outras; é dona principal de 124 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#mutateBuilt@876` (1)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `world:lot` (12), `worker:conflictTable` (9), `world:doc` (5), `world:network` (3), `editor:barriers` (2), `world:changes` (1), `render:network` (1)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `RoadDoc.buildings`, `RoadDoc.changes`, `RoadDoc.lotStore`, `RoadDoc.segments`, `RoadNode.x`, `RoadNode.y`, `RoadSegment.structure`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/editor/bulldozer.spec.ts`, `tests/editor/growthWorker.spec.ts`, `tests/editor/lotOverlayKey.spec.ts`, `tests/editor/lotProposalPlace.spec.ts`, `tests/editor/lotSnapOnce.spec.ts`, `tests/editor/lotsCutByRoad.spec.ts`, `tests/editor/zoneStroke.spec.ts`, `tests/render/cellPaving.spec.ts`, `tests/render/cellWork.spec.ts`, `tests/render/facadeLevel.spec.ts`, `tests/render/partCells.spec.ts`, `tests/render/poleLayer.spec.ts`, `tests/render/terrainShaping.spec.ts`, `tests/scale/lotEdit.spec.ts`, `tests/scale/lotPlan.spec.ts`, `tests/sim/agents/baysIncremental.spec.ts`, `tests/sim/agents/baysWorkers.spec.ts`, `tests/sim/agents/citySpin.spec.ts`, `tests/sim/agents/lotDoors.spec.ts`, `tests/sim/agents/lotGate.spec.ts`, `tests/sim/agents/lotParked.spec.ts`, `tests/sim/agents/parkedInLot.spec.ts`, `tests/sim/agents/population.spec.ts`, `tests/sim/ambientIndex.spec.ts`, `tests/sim/indexes/baysByDiary.spec.ts`, `tests/sim/indexes/lorries.spec.ts`, `tests/sim/indexes/lotGridSame.spec.ts`, `tests/sim/lorryTraffic.spec.ts`, `tests/sim/lotPlaces.spec.ts`, `tests/sim/lotPlanWorker.spec.ts`, `tests/sim/populationFollowsCity.spec.ts`, `tests/sim/trafficBuildings.spec.ts`, `tests/sim/yardSlices.spec.ts`, `tests/world/buildingFloors.spec.ts`, `tests/world/buildingStoreOwnership.spec.ts`, `tests/world/cityGreen.spec.ts`, `tests/world/facadeOverlap.spec.ts`, `tests/world/garageUnder.spec.ts`, `tests/world/landscapeBatch.spec.ts`, `tests/world/levelFlights.spec.ts`, `tests/world/lorryDocks.spec.ts`, `tests/world/lotCorners.spec.ts`, `tests/world/lotPhysics.spec.ts`, `tests/world/lotRaster.spec.ts`, `tests/world/lotStore.spec.ts`, `tests/world/lots.spec.ts`, `tests/world/openLots.spec.ts`, `tests/world/shopFloor.spec.ts`, `tests/world/splitLevel.spec.ts`, `tests/world/steppedYards.spec.ts`, `tests/world/streetDoors.spec.ts`, `tests/world/terrace.spec.ts`, `tests/world/tileEntries.spec.ts`, `tests/world/unzone.spec.ts`, `tests/world/variety.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `people`: já existia 2

## Código próprio (linhas — declaração — comentário)

### src/world/lots.ts
- 225-225 `LandPaving.constructor` (method)
- 1066-1066 `CROSSED_AREA` (variable): Area a road takes off a lot under which it counts as crossing it: less is the paving's own outline round the lot, not a road through it.
- 1069-1098 `cutLotsByRoads` (function): A ROAD EDIT CUTS THE LOTS IT CROSSES (`main.ts` mutateBuilt, beside the buildings it razes): each lot the changed paving (`rects`; null, every lot) takes land from is replaced by the pieces of it left on land, each a lot of its zone where i
