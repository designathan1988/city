# sim:intersections

Grupo: sim. Entrada: subsistema. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/sim/intersections/admission.ts`, `src/sim/intersections/claims.ts`, `src/sim/intersections/crossingSpans.ts`, `src/sim/intersections/cycles.ts`, `src/sim/intersections/spillback.ts`
- Unidades próprias (só esta funcionalidade alcança): 3 em 1 arquivo(s), 9 linhas
- Alcance total: 839 unidades; usa 685 compartilhadas de outras; é dona principal de 151 compartilhadas

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
- Lê: `?.connector`, `?.s`, `?.state`, `?.vehicle`, `Claim.connector`, `Claim.grantedTick`, `Claim.vehicle`, `ClaimTable.byPoint`, `ClaimTable.byVehicle`, `ConnectorClearance.connector`, `ConnectorClearance.distanceBeyondExit`, `CrossingOccupant.forward`, `CrossingOccupant.held`, `CrossingOccupant.s`, `CrossingOccupant.v`, `CrossingState.length`, `CrossingState.occupants`, `CrossingState.waitingFrom`, `CrossingState.waitingTo`, `HolderState.centre`, `HolderState.cls`, `LaneletRuntime.order`, `RoadNode.control`, `RoadNode.incident`, `RoadSegment.section`, `RoadSegment.type`, `SimWorld.city`, `SimWorld.claims`, `SimWorld.clock`, `SimWorld.conflicts`, `SimWorld.crossingSpans`, `SimWorld.crossingStates`, `SimWorld.doLess`, `SimWorld.graph`, `SimWorld.lastAdmission`, `SimWorld.mergeTurn`, `SimWorld.runtime`, `SimWorld.sidewalks`, `SimWorld.topologyLog`, `SimWorld.vehicles`, `Vehicle.admittedConnector`, `Vehicle.archetype`, `Vehicle.clearingConnectors`, `Vehicle.constraints`, `Vehicle.driver`, `Vehicle.firstRequestTick`, `Vehicle.id`, `Vehicle.lanelet`, `Vehicle.reservedConnectors`, `Vehicle.route`, `Vehicle.s`, `Vehicle.v`, `Vehicle.v0`, `Vehicle.waited`
- Escreve: `Vehicle.admittedConnector`, `Vehicle.claims`, `Vehicle.firstRequestTick`, `Vehicle.heldByPedestriansTick`, `Vehicle.lastMovedTick`, `Vehicle.reservedConnectors`

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/editor/roadPriority.spec.ts`, `tests/editor/roundabout.spec.ts`, `tests/sim/admissionGroup.spec.ts`, `tests/sim/crossingSpansCache.spec.ts`, `tests/sim/crossingSpansUpdate.spec.ts`, `tests/sim/cycleUpdate.spec.ts`, `tests/sim/drive/cycles.spec.ts`, `tests/sim/indexes/cyclesHeap.spec.ts`, `tests/sim/junctionRules.spec.ts`, `tests/sim/signals.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `people`: já existia 2
- porte `traffic`: já existia 6
- porte `transit`: portado 1

## Código próprio (linhas — declaração — comentário)

### src/sim/intersections/claims.ts
- 41-43 `ClaimTable.holder` (method): Holder of a conflict point, if any.
- 176-178 `ClaimTable.size` (method)
- 180-182 `ClaimTable.holders` (method)
