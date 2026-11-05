# sim:routing

Grupo: sim. Entrada: subsistema. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/sim/routing/destination.ts`, `src/sim/routing/laneReach.ts`, `src/sim/routing/lowerBound.ts`, `src/sim/routing/routeIndex.ts`, `src/sim/routing/router.ts`, `src/sim/routing/searchScratch.ts`
- Unidades próprias (só esta funcionalidade alcança): 2 em 2 arquivo(s), 19 linhas
- Alcance total: 234 unidades; usa 116 compartilhadas de outras; é dona principal de 116 compartilhadas

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
- Usa: `world:lanelets` (5), `editor:commit` (2)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `LaneletRuntime.ghost`, `LaneletRuntime.order`, `SimStreams.route`, `SimWorld.graph`, `SimWorld.laneReaches`, `SimWorld.topologyLog`, `Vehicle.admittedConnector`, `Vehicle.archetype`, `Vehicle.clearingConnectors`, `Vehicle.destination`, `Vehicle.driver`, `Vehicle.heldUp`, `Vehicle.lanelet`, `Vehicle.movementIntent`, `Vehicle.route`, `Vehicle.s`
- Escreve: `Vehicle.desiredLane`, `Vehicle.destination`, `Vehicle.heldUp`, `Vehicle.movementIntent`, `Vehicle.route`

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/scale/edit.spec.ts`, `tests/sim/bodyRouteEligibility.spec.ts`, `tests/sim/destinationRouting.spec.ts`, `tests/sim/drive/convoy.spec.ts`, `tests/sim/drive/tactical.spec.ts`, `tests/sim/laneChangeIntent.spec.ts`, `tests/sim/laneChangeRouteFiling.spec.ts`, `tests/sim/laneReachUpdate.spec.ts`, `tests/sim/routing/denseSearch.spec.ts`, `tests/sim/routing/routeIndex.spec.ts`, `tests/sim/routing/searchBudget.spec.ts`, `tests/sim/trapLanes.spec.ts`, `tests/sim/trapUpdate.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `traffic`: já existia 8

## Código próprio (linhas — declaração — comentário)

### src/sim/routing/laneReach.ts
- 369-373 `LaneReaches.work` (method): Lanes looked at since made, over the body classes (the scale tests' count).

### src/sim/routing/lowerBound.ts
- 14-27 `tripLowerBound` (function): The bound to `goal` from the end of a lane, seconds, each lane's worked out once a search.
