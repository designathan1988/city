# sim:vehicles

Grupo: sim. Entrada: subsistema. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/sim/vehicles/archetypes.ts`, `src/sim/vehicles/curvature.ts`, `src/sim/vehicles/cycleLane.ts`, `src/sim/vehicles/driver.ts`, `src/sim/vehicles/idm.ts`, `src/sim/vehicles/integrate.ts`, `src/sim/vehicles/kerbStops.ts`, `src/sim/vehicles/laneChange.ts`, `src/sim/vehicles/laneClosures.ts`, `src/sim/vehicles/leaderIndex.ts`, `src/sim/vehicles/obstacles.ts`, `src/sim/vehicles/spawn.ts`, `src/sim/vehicles/state.ts`, `src/sim/vehicles/vehicleId.ts`
- Unidades próprias (só esta funcionalidade alcança): 2 em 1 arquivo(s), 5 linhas
- Alcance total: 901 unidades; usa 688 compartilhadas de outras; é dona principal de 211 compartilhadas

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
- Usa: `sim:world` (1)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `?.at`, `?.clearAt`, `?.lanelet`, `?.next`, `?.offset`, `?.rate`, `?.s`, `?.trip`, `?.vehicle`, `?.waiting`, `ConnectorClearance.connector`, `ConnectorClearance.distanceBeyondExit`, `FreePose.angle`, `FreePose.lot`, `FreePose.pangle`, `FreePose.px`, `FreePose.py`, `FreePose.x`, `FreePose.y`, `Kinematics.lanelet`, `Kinematics.lateral`, `Kinematics.lateralSlope`, `Kinematics.rearPath`, `Kinematics.s`, `Kinematics.v`, `LaneletRuntime.ghost`, `LaneletRuntime.order`, `LaneletRuntime.shadows`, `RoadNode.incident`, `RoadSegment.structure`, `SimStreams.driver`, `SimStreams.occupancy`, `SimStreams.spawnVehicles`, `SimWorld.ambient`, `SimWorld.city`, `SimWorld.claims`, `SimWorld.clearancesAtStart`, `SimWorld.clock`, `SimWorld.completedTrips`, `SimWorld.conflicts`, `SimWorld.crossingSpans`, `SimWorld.crossingStates`, `SimWorld.demandMultiplier`, `SimWorld.edgeTraffic`, `SimWorld.entryDemand`, `SimWorld.entryDemandLost`, `SimWorld.followCity`, `SimWorld.graph`, `SimWorld.meso`, `SimWorld.nextVehicleId`, `SimWorld.occupied`, `SimWorld.pedEngine`, `SimWorld.populationShare`, `SimWorld.roadLength`, `SimWorld.runtime`, `SimWorld.sidewalks`, `SimWorld.topologyLog`, `SimWorld.trafficCount`, `SimWorld.trafficIntensity`, `SimWorld.vehicleSpawnClock` … (+57)
- Escreve: `?.next`, `?.rate`, `ConnectorClearance.distanceBeyondExit`, `Kinematics.lanelet`, `Kinematics.lateral`, `Kinematics.lateralSlope`, `Kinematics.rearPath`, `Kinematics.s`, `Kinematics.v`, `SimWorld.vehicleSpawnClock`, `Vehicle.accel`, `Vehicle.admittedConnector`, `Vehicle.age`, `Vehicle.claims`, `Vehicle.clearingConnectors`, `Vehicle.constraints`, `Vehicle.desiredLane`, `Vehicle.destination`, `Vehicle.doors`, `Vehicle.errand`, `Vehicle.firstRequestTick`, `Vehicle.heldUp`, `Vehicle.kerbQueue`, `Vehicle.kerbStop`, `Vehicle.laneChange`, `Vehicle.laneIntent`, `Vehicle.laneIntentSince`, `Vehicle.lanelet`, `Vehicle.lastLaneChangeAge`, `Vehicle.lastMovedTick`, `Vehicle.lastServiceAge`, `Vehicle.lateral`, `Vehicle.lateralLength`, `Vehicle.lateralSlope`, `Vehicle.lateralStart`, `Vehicle.lateralTravelled`, `Vehicle.movementIntent`, `Vehicle.prev`, `Vehicle.rearPath`, `Vehicle.rorCredit`, `Vehicle.rorStopped`, `Vehicle.route`, `Vehicle.s`, `Vehicle.seats`, `Vehicle.slain`, `Vehicle.v`, `Vehicle.v0`, `Vehicle.waited`

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/editor/roadPriority.spec.ts`, `tests/editor/roundabout.spec.ts`, `tests/render/agentsParked.spec.ts`, `tests/render/agentsResidency.spec.ts`, `tests/render/interiorSolids.spec.ts`, `tests/render/occupantFit.spec.ts`, `tests/render/pageGarbage.spec.ts`, `tests/render/riderFit.spec.ts`, `tests/render/vehicleCalls.spec.ts`, `tests/render/vehicleFrame.spec.ts`, `tests/render/vehicleLevels.spec.ts`, `tests/render/vehiclesInView.spec.ts`, `tests/render/windows.spec.ts`, `tests/sim/agents.spec.ts`, `tests/sim/agents/lotParked.spec.ts`, `tests/sim/agents/parkedInLot.spec.ts`, `tests/sim/agents/population.spec.ts`, `tests/sim/ambient/parkedLog.spec.ts`, `tests/sim/collisions.spec.ts`, `tests/sim/curvature.spec.ts`, `tests/sim/destinationRouting.spec.ts`, `tests/sim/drive/convoy.spec.ts`, `tests/sim/drive/shortTail.spec.ts`, `tests/sim/drive/tactical.spec.ts`, `tests/sim/drivers.spec.ts`, `tests/sim/indexes/lorries.spec.ts`, `tests/sim/kerb.spec.ts`, `tests/sim/laneChangeIntent.spec.ts`, `tests/sim/laneChangeRouteFiling.spec.ts`, `tests/sim/lorryTraffic.spec.ts`, `tests/sim/microWindowCells.spec.ts`, `tests/sim/populationFollowsCity.spec.ts`, `tests/sim/pose.spec.ts`, `tests/sim/restingRears.spec.ts`, `tests/sim/routing/searchBudget.spec.ts`, `tests/sim/seatAgeClass.spec.ts`, `tests/sim/simResync.spec.ts`, `tests/sim/stageSeven.spec.ts`, `tests/sim/vehiclesNear.spec.ts`, `tests/sim/vehiclesNearWork.spec.ts`, `tests/sim/walkSpine.spec.ts`, `tests/sim/yardSlices.spec.ts`, `tests/world/connectorFootprints.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `traffic`: portado 6, já existia 9
- porte `people`: já existia 2, não se aplica 7
- porte `transit`: portado 1

## Código próprio (linhas — declaração — comentário)

### src/sim/vehicles/spawn.ts
- 161-164 `spawnVehicle` (function): Spawns one vehicle at the first entry lane with room for it.
- 327-327 `EmptyStatement` (statement)
