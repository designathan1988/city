# sim:world

Grupo: sim. Entrada: subsistema. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/sim/world.ts`
- Unidades próprias (só esta funcionalidade alcança): 5 em 1 arquivo(s), 19 linhas
- Alcance total: 1521 unidades; usa 1320 compartilhadas de outras; é dona principal de 196 compartilhadas

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
- Usa: `world:lanelets` (10), `sim:agents` (1), `world:junction` (1), `sim:vehicles` (1)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `?.active`, `?.admitted`, `?.cos`, `?.deps`, `?.detail`, `?.dx`, `?.dy`, `?.ex`, `?.ey`, `?.failed`, `?.far`, `?.fresh`, `?.lanelet`, `?.next`, `?.nodes`, `?.offset`, `?.r`, `?.rate`, `?.reserved`, `?.reservedLength`, `?.route`, `?.routeLength`, `?.s`, `?.score`, `?.serial`, `?.signalised`, `?.topology`, `?.turns`, `?.vehicle`, `?.view`, `?.waiting`, `?.x`, `?.y`, `ConnectorClearance.connector`, `CrossingState.demand`, `CrossingState.longestWait`, `CrossingState.occupants`, `CrossingState.waitingFrom`, `CrossingState.waitingTo`, `LaneletRuntime.ghost`, `LaneletRuntime.ghostSince`, `LaneletRuntime.id`, `LaneletRuntime.order`, `LaneletRuntime.shadows`, `PlanInputs.conflicts`, `PlanInputs.connectors`, `PlanInputs.crossings`, `PlanInputs.groups`, `PlanInputs.settings`, `PlanInputs.signalised`, `RoadDoc.changes`, `RoadNode.control`, `RoadNode.crossing`, `RoadNode.incident`, `RoadNode.signal`, `RoadNode.x`, `RoadNode.y`, `RoadSegment.a`, `RoadSegment.b`, `SimStreams.ambient` … (+118)
- Escreve: `?.failed`, `?.turns`, `LaneletRuntime.ghost`, `LaneletRuntime.ghostSince`, `SimWorld.completedTrips`, `SimWorld.controlAdvisor`, `SimWorld.entryDemandLost`, `SimWorld.flowStats`, `SimWorld.keptSignalDeps`, `SimWorld.microWindow`, `SimWorld.ordered`, `SimWorld.pedEngine`, `SimWorld.prepGraph`, `SimWorld.prepReached`, `SimWorld.rebindAll`, `SimWorld.signalLog`, `SimWorld.signalLogBase`, `SimWorld.streams`, `SimWorld.topologyRevision`, `SimWorld.vehicleSpawnClock`, `SimWorld.vehicleTopologyRevision`, `SimWorld.wakeAll`, `SimWorld.walkSeen`, `SimWorld.windowFleet`, `SimWorld.windowTick`, `SimWorld.windowWanted`, `Vehicle.greenDenied`, `Vehicle.greenStall`, `Vehicle.lanelet`, `Vehicle.shadow`, `VersionedMap.ordered`, `VersionedMap.orderedKeys`

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/editor/roadPriority.spec.ts`, `tests/editor/roundabout.spec.ts`, `tests/perf/simScale.spec.ts`, `tests/scale/buildingEdit.spec.ts`, `tests/scale/edit.spec.ts`, `tests/sim/advisorReach.spec.ts`, `tests/sim/agents/baysIncremental.spec.ts`, `tests/sim/agents/baysWorkers.spec.ts`, `tests/sim/agents/citySpin.spec.ts`, `tests/sim/agents/lotDoors.spec.ts`, `tests/sim/agents/lotGate.spec.ts`, `tests/sim/agents/lotParked.spec.ts`, `tests/sim/agents/parkedInLot.spec.ts`, `tests/sim/agents/population.spec.ts`, `tests/sim/ambientMeso.spec.ts`, `tests/sim/bodyRouteEligibility.spec.ts`, `tests/sim/busesAway.spec.ts`, `tests/sim/cityClock.spec.ts`, `tests/sim/crossingSpansCache.spec.ts`, `tests/sim/crossingSpansSame.spec.ts`, `tests/sim/crossingSpansUpdate.spec.ts`, `tests/sim/curvature.spec.ts`, `tests/sim/cycleUpdate.spec.ts`, `tests/sim/doLess.spec.ts`, `tests/sim/drive/convoy.spec.ts`, `tests/sim/drive/cycles.spec.ts`, `tests/sim/drive/shortTail.spec.ts`, `tests/sim/drive/tactical.spec.ts`, `tests/sim/drivers.spec.ts`, `tests/sim/fourWay.spec.ts`, `tests/sim/heightTopology.spec.ts`, `tests/sim/indexes/baysByDiary.spec.ts`, `tests/sim/indexes/cyclesHeap.spec.ts`, `tests/sim/indexes/footwayIndex.spec.ts`, `tests/sim/indexes/levelCrossings.spec.ts`, `tests/sim/indexes/lorries.spec.ts`, `tests/sim/indexes/lotGridSame.spec.ts`, `tests/sim/issuesSince.spec.ts`, `tests/sim/kerb.spec.ts`, `tests/sim/laneChangeIntent.spec.ts`, `tests/sim/laneChangeRouteFiling.spec.ts`, `tests/sim/laneReachUpdate.spec.ts`, `tests/sim/laneletGraphCache.spec.ts`, `tests/sim/lorryTraffic.spec.ts`, `tests/sim/lotPlaces.spec.ts`, `tests/sim/meso/boundedTick.spec.ts`, `tests/sim/meso/window.spec.ts`, `tests/sim/microWindowCells.spec.ts`, `tests/sim/passKerbStop.spec.ts`, `tests/sim/populationFollowsCity.spec.ts`, `tests/sim/pose.spec.ts`, `tests/sim/rebindReach.spec.ts`, `tests/sim/reset.spec.ts`, `tests/sim/resetLikeNew.spec.ts`, `tests/sim/restingRears.spec.ts`, `tests/sim/signalMovementMatrix.spec.ts`, `tests/sim/signalPlanWindow.spec.ts`, `tests/sim/signalPosts.spec.ts`, `tests/sim/signalWake.spec.ts`, `tests/sim/signals.spec.ts`, `tests/sim/simResync.spec.ts`, `tests/sim/stageSeven.spec.ts`, `tests/sim/topologyOffPage.spec.ts`, `tests/sim/traffic.spec.ts`, `tests/sim/trafficBuildings.spec.ts`, `tests/sim/transitLoop.spec.ts`, `tests/sim/transitStops.spec.ts`, `tests/sim/trapLanes.spec.ts`, `tests/sim/trapUpdate.spec.ts`, `tests/sim/vehiclesNear.spec.ts`, `tests/sim/vehiclesNearWork.spec.ts`, `tests/sim/walkArrivals.spec.ts`, `tests/sim/walkHeight.spec.ts`, `tests/sim/walkSpine.spec.ts`, `tests/sim/yardSlices.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `demolition`: já existia 2
- porte `people`: já existia 12
- porte `traffic`: já existia 2
- porte `transit`: já existia 2

## Código próprio (linhas — declaração — comentário)

### src/sim/world.ts
- 184-184 `SimWorld.driveModel` (field): The vehicle model: Drive v2 (`drive/*`), the only one (the plain-IDM legacy model was taken out on 2026-10-08).
- 235-235 `SimWorld.laneGrid` (field): The lanes by place, kept by what each build of the lane graph changed (`agents/laneGrid.ts`).
- 365-369 `SimWorld.requireLanelet` (method)
- 690-697 `SimWorld.warmTopologyPrep` (method): Builds the preparation graph with a map's load, so its first edit does not build it from nothing.
- 1237-1240 `SimWorld.laneTail` (method): The vehicle nearest the entry of a lanelet, or undefined.
