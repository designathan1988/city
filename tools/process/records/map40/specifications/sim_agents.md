# sim:agents

Grupo: sim. Entrada: subsistema. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/sim/agents/bay.ts`, `src/sim/agents/buildingOrder.ts`, `src/sim/agents/hybridAStar.ts`, `src/sim/agents/kerbBays.ts`, `src/sim/agents/laneGrid.ts`, `src/sim/agents/lorryDocks.ts`, `src/sim/agents/lorryTraffic.ts`, `src/sim/agents/lotBays.ts`, `src/sim/agents/lotDoors.ts`, `src/sim/agents/lotNav.ts`, `src/sim/agents/lotTraffic.ts`, `src/sim/agents/manoeuvre.ts`, `src/sim/agents/parking.ts`, `src/sim/agents/reedsShepp.ts`, `src/sim/agents/trafficBuildings.ts`, `src/sim/agents/walk.ts`
- Unidades próprias (só esta funcionalidade alcança): 52 em 9 arquivo(s), 376 linhas
- Alcance total: 2833 unidades; usa 1874 compartilhadas de outras; é dona principal de 907 compartilhadas

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
- Usa: `sim:vehicles` (22), `world:buildings` (18), `sim:world` (16), `worker:conflictTable` (12), `render:buildings` (8), `sim:people` (6), `world:changes` (2), `sim:params` (2), `editor:editStream` (1), `world:lanelets` (1), `sim:clock` (1), `sim:dt` (1)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `?.at`, `?.lanelet`, `?.s`, `?.vehicle`, `CrossingState.length`, `CrossingState.longestWait`, `CrossingState.occupants`, `CrossingState.waitingFrom`, `CrossingState.waitingTo`, `FreePose.angle`, `FreePose.x`, `FreePose.y`, `RoadDoc.buildings`, `RoadDoc.changes`, `RoadDoc.lotStore`, `RoadDoc.nodes`, `RoadDoc.segments`, `RoadDoc.transit`, `RoadSegment.a`, `RoadSegment.b`, `RoadSegment.direction`, `SimStreams.driver`, `SimStreams.lorries`, `SimStreams.lots`, `SimWorld.ambient`, `SimWorld.city`, `SimWorld.clock`, `SimWorld.crossingStates`, `SimWorld.graph`, `SimWorld.nextPersonId`, `SimWorld.nextVehicleId`, `SimWorld.pedEngine`, `SimWorld.pedViewById`, `SimWorld.pedViews`, `SimWorld.sidewalks`, `SimWorld.signalPosts`, `SimWorld.topologyRevision`, `SimWorld.vehicles`, `StandingCar.archetype`, `StandingCar.free`, `Vehicle.archetype`, `Vehicle.color`, `Vehicle.commute`, `Vehicle.destination`, `Vehicle.driver`, `Vehicle.errand`, `Vehicle.free`, `Vehicle.id`, `Vehicle.kerbStop`, `Vehicle.lanelet`, `Vehicle.s`, `Vehicle.v`
- Escreve: `CrossingState.demand`, `CrossingState.longestWait`, `CrossingState.waitingFrom`, `CrossingState.waitingTo`, `FreePose.angle`, `FreePose.pangle`, `FreePose.px`, `FreePose.py`, `FreePose.x`, `FreePose.y`, `Vehicle.commute`, `Vehicle.destination`, `Vehicle.free`, `Vehicle.prev`, `Vehicle.s`, `Vehicle.seats`, `Vehicle.v`

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/perf/trafficBuildingsBench.spec.ts`, `tests/scale/buildingEdit.spec.ts`, `tests/scale/render/cityMemory.spec.ts`, `tests/sim/agents/baysIncremental.spec.ts`, `tests/sim/agents/baysWorkers.spec.ts`, `tests/sim/agents/citySpin.spec.ts`, `tests/sim/agents/lotDistance.spec.ts`, `tests/sim/agents/lotDoors.spec.ts`, `tests/sim/agents/lotGate.spec.ts`, `tests/sim/agents/lotParked.spec.ts`, `tests/sim/agents/parkedInLot.spec.ts`, `tests/sim/agents/population.spec.ts`, `tests/sim/agents/reedsShepp.spec.ts`, `tests/sim/ambientIndex.spec.ts`, `tests/sim/doLess.spec.ts`, `tests/sim/indexes/audit.spec.ts`, `tests/sim/indexes/baysByDiary.spec.ts`, `tests/sim/indexes/lorries.spec.ts`, `tests/sim/kerb.spec.ts`, `tests/sim/lorryTraffic.spec.ts`, `tests/sim/lotPlaces.spec.ts`, `tests/sim/lotPlanWorker.spec.ts`, `tests/sim/meso/stepGarbage.spec.ts`, `tests/sim/populationFollowsCity.spec.ts`, `tests/sim/reset.spec.ts`, `tests/sim/simInitBake.spec.ts`, `tests/sim/simResync.spec.ts`, `tests/sim/trafficBuildings.spec.ts`, `tests/sim/transitStops.spec.ts`, `tests/sim/vehiclesNearWork.spec.ts`, `tests/sim/walkArrivals.spec.ts`, `tests/sim/walkHeight.spec.ts`, `tests/sim/walkSpine.spec.ts`, `tests/sim/walkingGolden.spec.ts`, `tests/sim/yardSlices.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `traffic`: portado 15, não se aplica 4, já existia 5, de outra funcionalidade 1
- porte `people`: não se aplica 41, portado 1, já existia 398
- porte `transit`: portado 3, não se aplica 2
- porte `demolition`: não portado (pendência) 2
- porte `street-furniture`: já existia 2

## Código próprio (linhas — declaração — comentário)

### src/sim/agents/buildingOrder.ts
- 21-23 `StoreOrder.constructor` (method)
- 115-115 `BuildingsInOrder.kinds` (field)
- 117-119 `BuildingsInOrder.seenSerial` (method): What it has read up to (`JournalReader`): its diary keeps the buildings' entries after it until it reads them.

### src/sim/agents/hybridAStar.ts
- 41-47 `clearanceOf` (function): The clearance field of a grid of solid cells: an exact Euclidean distance transform.

### src/sim/agents/lorryDocks.ts
- 165-168 `Yard.solid` (method): The yard's grid, while it is kept (`plan` lets it go once every dock is planned).
- 459-467 `yardOf` (function): The yard of a building as it stands now, made to the end at once if it must be (probes and specs).

### src/sim/agents/lorryTraffic.ts
- 110-115 `LorryTraffic.view` (method): For probes: each lorry's phase, building, dock and where its body is.
- 141-159 `LorryTraffic.sendNow` (method): Sends a lorry to a dock now, standing on its lane at the stop before the gate (for probes and specs)

### src/sim/agents/lotNav.ts
- 64-64 `RingCover.px` (field)
- 65-65 `RingCover.py` (field)
- 71-86 `RingCover.constructor` (method)

### src/sim/agents/lotTraffic.ts
- 187-189 `LotTraffic.view` (method): The cars of the lots, for probes: id, phase, the bay's building, where the body stands.

### src/sim/agents/parking.ts
- 32-34 `collectBays` (function): Every stall of every parking element, with the way out of its lot and the lane it is reached from
- 84-89 `BakedSource` (interface): One lot's or street's bays as a bake keeps them (`bakeBays`): the record's digest (0 for a street) and the sources'.
- 139-141 `tileBaysWanted` (function): A tile's baked bays are being read (`simHost.ts` tileBuildings): its own streets wait for them (`KerbReads.waits`), as a lot waits for the workers.
- 144-155 `tileBaysRead` (function): A tile's read landed: its baked bays (null: none stored, of another format, or not read) offered while that read is the tile's last, its streets looked at again.
- 158-162 `tileBaysGone` (function): A tile let go: its baked bays dropped, its streets no longer wait.
- 165-168 `tileBaysReset` (function): Another map put in: every tile's baked bays and reads of the old one dropped.
- 170-176 `dropBaked` (function)
- 179-182 `release` (function): The streets held for a tile's bays looked at again at the next follow (those whose tile still waits held again).
- 214-227 `bakeBays` (function): @rebuild Every lot's and street's bays of a world worked out here and now (no workers), each with what it was worked out from: what a tile's bake keeps (`sim/tileBays.ts`).
- 315-324 `townBays` (function): @rebuild The town's list of bays, put together again only after a change (`collectBays`).

### src/sim/agents/trafficBuildings.ts
- 79-79 `trafficBuilding` (variable): DO NOT keep it per record on the page (`identityMemo`): a copy of every building of the map held for as long as the map, ~6 KB each; worked out when it is sent, kept by the worker alone.
- 272-272 `TrafficBuildingTable.mark` (field)
- 302-304 `TrafficBuildingTable.size` (method)
- 306-309 `TrafficBuildingTable.get` (method)
- 311-313 `TrafficBuildingTable.all` (method)
- 315-320 `TrafficBuildingTable.ids` (method)
- 322-325 `TrafficBuildingTable.facts` (method)
- 327-333 `TrafficBuildingTable.entrances` (method)
- 335-337 `TrafficBuildingTable.eachFacts` (method)
- 339-341 `TrafficBuildingTable.eachEntrances` (method)
- 343-345 `TrafficBuildingTable.eachFootprints` (method)
- 347-360 `TrafficBuildingTable.footprints` (method)
- 414-416 `TrafficBuildingTable.near` (method)
- 418-420 `TrafficBuildingTable.nearIds` (method)
- 422-431 `TrafficBuildingTable.nearAny` (method)
- 448-456 `TrafficBuildingTable.factsAt` (method)
- 544-546 `TrafficBuildingTable.inOrder` (method)
- 573-587 `TrafficBuildingTable.record` (method): A slot's traffic record: held, still alive elsewhere, or worked out again from its row.
- 670-688 `TrafficBuildingTable.meeting` (method): The slots whose filed rectangle meets `rect` (touching counts), each once.

### src/sim/agents/walk.ts
- 145-145 `ParkedZoneIndex.pass` (field)
- 189-220 `ParkedZoneIndex.sync` (method): Brings the cells up to `cars`, the cars standing now (`AmbientWorld.parked`).
- 404-408 `traceWalkers` (function): Traces these walkers' changes into `sink` (an empty list stops it).
- 1999-2001 `walkersNear` (function): The walkers within `radius` of a point, out on the street.
- 2010-2021 `takeWalker` (function): Puts somebody walking under the player's hand: they stop following their route.
- 2024-2046 `addPlayerWalker` (function): A body for the player to move, out at a point (stepping out of a door or a car).
- 2049-2055 `movePlayerWalker` (function): The player's body moved: where it is now, the way it faces, how fast it goes.
- 2103-2110 `walkerAct` (function): Somebody stops a while to do something (talk, fall down), facing a point.
- 2207-2213 `sendRunning` (function): Somebody sent running towards a point (a police officer after the player), their own route on the walkways.
- 2251-2258 `releaseWalker` (function): Gives a body back from the player's hand, walking on to `to` (their home, say); null: off the street.
- 2261-2275 `walkerState` (function): One walker as a test bench reads them (the weapons lab)
