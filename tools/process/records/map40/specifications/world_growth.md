# world:growth

Grupo: world. Entrada: subsistema. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/world/growth/grownRows.ts`, `src/world/growth/lotBoundaryRuns.ts`, `src/world/growth/lotPlan.ts`, `src/world/growth/rows.ts`, `src/world/growth/version.ts`, `src/world/growth/zoning.ts`
- Unidades próprias (só esta funcionalidade alcança): 8 em 2 arquivo(s), 83 linhas
- Alcance total: 700 unidades; usa 607 compartilhadas de outras; é dona principal de 85 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- nenhuma chamada direta da raiz: chega por outra funcionalidade

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): `recebe:flats`, `recebe:house`, `recebe:industry`, `recebe:shop`, `recebe:tower`
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `world:doc` (10), `world:buildings` (7), `world:lot` (4), `worker:conflictTable` (2), `editor:editStream` (1), `editor:growthReplica` (1), `world:lots` (1)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `RoadDoc.buildings`, `TileRecords.buildings`, `TileRecords.grown`, `TileRecords.lots`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/editor/growthWorker.spec.ts`, `tests/editor/lotBoundaryRunsEquivalence.spec.ts`, `tests/render/cellPaving.spec.ts`, `tests/render/cellWork.spec.ts`, `tests/render/facadeLevel.spec.ts`, `tests/render/partCells.spec.ts`, `tests/render/terrainShaping.spec.ts`, `tests/sim/agents/baysIncremental.spec.ts`, `tests/sim/agents/baysWorkers.spec.ts`, `tests/sim/agents/citySpin.spec.ts`, `tests/sim/agents/lotDoors.spec.ts`, `tests/sim/agents/lotGate.spec.ts`, `tests/sim/agents/lotParked.spec.ts`, `tests/sim/agents/parkedInLot.spec.ts`, `tests/sim/agents/population.spec.ts`, `tests/sim/ambientIndex.spec.ts`, `tests/sim/indexes/baysByDiary.spec.ts`, `tests/sim/indexes/lorries.spec.ts`, `tests/sim/indexes/lotGridSame.spec.ts`, `tests/sim/lorryTraffic.spec.ts`, `tests/sim/lotPlaces.spec.ts`, `tests/sim/populationFollowsCity.spec.ts`, `tests/sim/trafficBuildings.spec.ts`, `tests/sim/yardSlices.spec.ts`, `tests/world/buildingFloors.spec.ts`, `tests/world/buildingStoreOwnership.spec.ts`, `tests/world/facadeOverlap.spec.ts`, `tests/world/garageUnder.spec.ts`, `tests/world/levelFlights.spec.ts`, `tests/world/lorryDocks.spec.ts`, `tests/world/lotPhysics.spec.ts`, `tests/world/shopFloor.spec.ts`, `tests/world/splitLevel.spec.ts`, `tests/world/steppedYards.spec.ts`, `tests/world/streetDoors.spec.ts`, `tests/world/terrace.spec.ts`, `tests/world/unzone.spec.ts`, `tests/world/variety.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- nenhuma cobertura de porte cita os arquivos desta funcionalidade

## Código próprio (linhas — declaração — comentário)

### src/world/growth/grownRows.ts
- 10-10 `FLAT` (function): The flat land a generated tile is grown on (`wholeMapTile.ts`): what its buildings are made again on.
- 13-30 `packGrown` (function): Buildings grown, each with its lot and its choices, packed in columns.
- 33-46 `choiceOf` (function): Row `k`'s choices.
- 48-48 `warned` (variable)
- 51-71 `grownRecords` (function): The rows made records again on their lots, stamped as the generator stamped them (built at the city's first minute, new); a row of another generator, or whose lot is not among `lots`, is left out (said once).
- 74-76 `tileBuildingsOf` (function): Every building of a tile: its records and its grown rows made again.
- 79-83 `withGrownBuildings` (function): The tile with its grown rows made records (for whoever reads `buildings` whole: a write's bakes, a tile prepared for the page).

### src/world/growth/zoning.ts
- 453-472 `replayOnLot` (function): The record `designOnLot` made on a lot, made again from its choices (`GrownChoice`) alone: the same try with the same stream, the same candidate kept, the same pieces left out - no neighbour, road or paving read. `ground`: the land it was m
