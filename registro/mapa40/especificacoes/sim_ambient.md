# sim:ambient

Grupo: sim. Entrada: subsistema. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/sim/ambient/ambient.ts`, `src/sim/ambient/ambientIndex.ts`, `src/sim/ambient/demand.ts`, `src/sim/ambient/lotPlaces.ts`, `src/sim/ambient/playerId.ts`
- Unidades próprias (só esta funcionalidade alcança): 5 em 3 arquivo(s), 21 linhas
- Alcance total: 1727 unidades; usa 1481 compartilhadas de outras; é dona principal de 241 compartilhadas

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
- Usa: `render:buildings` (8)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `?.cos`, `?.detail`, `?.dx`, `?.dy`, `?.ex`, `?.ey`, `?.far`, `?.r`, `?.view`, `?.x`, `?.y`, `FreePose.x`, `FreePose.y`, `RoadDoc.changes`, `RoadDoc.nodes`, `RoadNode.id`, `RoadNode.x`, `RoadNode.y`, `RoadSegment.a`, `RoadSegment.b`, `SimStreams.ambient`, `SimStreams.spawnPeds`, `SimStreams.spawnVehicles`, `SimWorld.city`, `SimWorld.clock`, `SimWorld.edgeTraffic`, `SimWorld.focus`, `SimWorld.followCity`, `SimWorld.graph`, `SimWorld.meso`, `SimWorld.nextVehicleId`, `SimWorld.pedEngine`, `SimWorld.pedViews`, `SimWorld.pedestrianCount`, `SimWorld.pedestrianIntensity`, `SimWorld.populationShare`, `SimWorld.topologyLog`, `SimWorld.trafficCount`, `SimWorld.vehicles`, `StandingCar.archetype`, `StandingCar.color`, `StandingCar.id`, `Vehicle.archetype`, `Vehicle.destination`, `Vehicle.free`, `Vehicle.lanelet`, `Vehicle.route`
- Escreve: `Vehicle.destination`, `Vehicle.free`, `Vehicle.seats`

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/perf/trafficBuildingsBench.spec.ts`, `tests/scale/render/cityMemory.spec.ts`, `tests/sim/agents/lotParked.spec.ts`, `tests/sim/ambient/parkedLog.spec.ts`, `tests/sim/ambientIndex.spec.ts`, `tests/sim/lotPlaces.spec.ts`, `tests/sim/populationFollowsCity.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `demolition`: não se aplica 2
- porte `people`: não se aplica 346, portado 114
- porte `traffic`: portado 3, não se aplica 3, de outra funcionalidade 2

## Código próprio (linhas — declaração — comentário)

### src/sim/ambient/ambient.ts
- 115-117 `AmbientWorld.counts` (method): Walkers and drivers this world made, for the status bar.

### src/sim/ambient/ambientIndex.ts
- 66-68 `SightWalls.constructor` (method)
- 230-235 `BuildingMix.constructor` (method)

### src/sim/ambient/lotPlaces.ts
- 16-18 `Fenwick.get` (method)
- 193-198 `LotPlaces.wayOf` (method): A building's way as a place (worked out now if it was not yet); undefined for a building not filed.
