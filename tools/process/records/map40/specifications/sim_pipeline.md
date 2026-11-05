# sim:pipeline

Grupo: sim. Entrada: subsistema. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/sim/pipeline.ts`
- Unidades próprias (só esta funcionalidade alcança): 1 em 1 arquivo(s), 1 linhas
- Alcance total: 3010 unidades; usa 2995 compartilhadas de outras; é dona principal de 14 compartilhadas

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
- Lê: `?.admitted`, `?.lanelet`, `?.reserved`, `?.reservedLength`, `?.route`, `?.routeLength`, `?.topology`, `ConnectorClearance.connector`, `LaneletRuntime.ghost`, `LaneletRuntime.order`, `SimWorld.ambient`, `SimWorld.auditEnabled`, `SimWorld.auditEvery`, `SimWorld.auditLevel`, `SimWorld.city`, `SimWorld.claims`, `SimWorld.clock`, `SimWorld.conflicts`, `SimWorld.doLess`, `SimWorld.ghostCandidates`, `SimWorld.graph`, `SimWorld.issues`, `SimWorld.meso`, `SimWorld.occupied`, `SimWorld.pedEngine`, `SimWorld.reservationsChecked`, `SimWorld.runtime`, `SimWorld.signalClock`, `SimWorld.topologyDeferred`, `SimWorld.topologyLog`, `SimWorld.topologyRevision`, `SimWorld.vehicles`, `Vehicle.admittedConnector`, `Vehicle.archetype`, `Vehicle.clearingConnectors`, `Vehicle.constraints`, `Vehicle.desiredLane`, `Vehicle.destination`, `Vehicle.driver`, `Vehicle.greenDenied`, `Vehicle.greenStall`, `Vehicle.id`, `Vehicle.lanelet`, `Vehicle.lastMovedTick`, `Vehicle.movementIntent`, `Vehicle.reservedConnectors`, `Vehicle.route`, `Vehicle.s`, `Vehicle.v`
- Escreve: `LaneletRuntime.ghost`, `LaneletRuntime.ghostSince`, `Vehicle.admittedConnector`, `Vehicle.claims`, `Vehicle.clearingConnectors`, `Vehicle.constraints`, `Vehicle.firstRequestTick`, `Vehicle.greenDenied`, `Vehicle.greenStall`, `Vehicle.reservedConnectors`, `Vehicle.route`, `Vehicle.s`

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/perf/simScale.spec.ts`, `tests/render/vehicleSignals.spec.ts`, `tests/sim/admissionGroup.spec.ts`, `tests/sim/agents/citySpin.spec.ts`, `tests/sim/agents/lotDoors.spec.ts`, `tests/sim/agents/lotParked.spec.ts`, `tests/sim/agents/parkedInLot.spec.ts`, `tests/sim/agents/population.spec.ts`, `tests/sim/boxPedestrians.spec.ts`, `tests/sim/bulb.spec.ts`, `tests/sim/busesAway.spec.ts`, `tests/sim/classJoin.spec.ts`, `tests/sim/collisions.spec.ts`, `tests/sim/curvature.spec.ts`, `tests/sim/destinationRouting.spec.ts`, `tests/sim/doLess.spec.ts`, `tests/sim/drive/convoy.spec.ts`, `tests/sim/drive/entryRate.spec.ts`, `tests/sim/drive/shortTail.spec.ts`, `tests/sim/drive/tactical.spec.ts`, `tests/sim/drivers.spec.ts`, `tests/sim/fourWay.spec.ts`, `tests/sim/heightTopology.spec.ts`, `tests/sim/indexes/audit.spec.ts`, `tests/sim/junctionRules.spec.ts`, `tests/sim/kerb.spec.ts`, `tests/sim/kerbStops.spec.ts`, `tests/sim/laneConnectors.spec.ts`, `tests/sim/laneletGraphCache.spec.ts`, `tests/sim/lotPlaces.spec.ts`, `tests/sim/meso/boundedTick.spec.ts`, `tests/sim/meso/window.spec.ts`, `tests/sim/passKerbStop.spec.ts`, `tests/sim/populationFollowsCity.spec.ts`, `tests/sim/pose.spec.ts`, `tests/sim/priorityBox.spec.ts`, `tests/sim/queuedSwing.spec.ts`, `tests/sim/rebindReach.spec.ts`, `tests/sim/reservationsChecked.spec.ts`, `tests/sim/reset.spec.ts`, `tests/sim/resetLikeNew.spec.ts`, `tests/sim/roundabout.spec.ts`, `tests/sim/routing/denseSearch.spec.ts`, `tests/sim/shortLinkBox.spec.ts`, `tests/sim/signalWake.spec.ts`, `tests/sim/signals.spec.ts`, `tests/sim/stageSeven.spec.ts`, `tests/sim/topologyDeferred.spec.ts`, `tests/sim/traffic.spec.ts`, `tests/sim/transitStops.spec.ts`, `tests/sim/uturn.spec.ts`, `tests/sim/vehiclesNear.spec.ts`, `tests/sim/vehiclesNearWork.spec.ts`, `tests/sim/walkArrivals.spec.ts`, `tests/sim/walkHeight.spec.ts`, `tests/sim/walkSpine.spec.ts`, `tests/sim/zipperMerge.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `people`: já existia 2
- porte `traffic`: já existia 2

## Código próprio (linhas — declaração — comentário)

### src/sim/pipeline.ts
- 452-452 `EmptyStatement` (statement)
