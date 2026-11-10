# worker:sim

Grupo: worker. Entrada: worker. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/sim/host/simClient.ts`, `src/sim/sim.worker.ts`
- Unidades próprias (só esta funcionalidade alcança): 4 em 1 arquivo(s), 9 linhas
- Alcance total: 4697 unidades; usa 4575 compartilhadas de outras; é dona principal de 118 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- nenhuma chamada direta da raiz: chega por outra funcionalidade

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): `envia:failed`, `envia:message`, `recebe:acted`, `recebe:answer`, `recebe:failed`, `recebe:ready`, `recebe:tick`
- Workers: `src/sim/sim.worker.ts`
- Ganchos do three.js: -
- Timers: 0
- Usa: `sim:host` (4)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `?.all`, `?.answer`, `?.casualties`, `?.id`, `?.junctions`, `?.kind`, `?.layout`, `?.meso`, `?.message`, `?.node`, `?.parked`, `?.peds`, `?.result`, `?.signals`, `?.stats`, `?.trains`, `?.type`, `?.vehicles`, `?.wounds`, `ParkedChanges.cars`, `ParkedChanges.whole`, `RoadDoc.changes`, `RoadDoc.lotStore`, `RoadDoc.nodes`, `RoadDoc.segments`, `SharedFrames.control`, `SharedFrames.data`, `SignalLayout.groups`, `SignalLayout.node`, `SignalLights.node`, `SignalLights.stage`, `SignalLights.states`, `SimStats.issues`, `SimStats.topology`, `VehicleRich.archetype`, `VehicleRich.color`, `VehicleRich.doors`, `VehicleRich.kerbStop`, `VehicleRich.people`, `VehicleRich.peopleAge`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/render/agentsParked.spec.ts`, `tests/render/agentsResidency.spec.ts`, `tests/render/densityCars.spec.ts`, `tests/render/pageGarbage.spec.ts`, `tests/render/signalBatches.spec.ts`, `tests/render/vehicleCalls.spec.ts`, `tests/render/vehiclesInView.spec.ts`, `tests/sim/lotPlanWorker.spec.ts`, `tests/sim/simClientAnswers.spec.ts`, `tests/sim/simClientDecode.spec.ts`, `tests/sim/simClientSignalsReset.spec.ts`, `tests/sim/simClientStart.spec.ts`, `tests/sim/simInitBake.spec.ts`, `tests/sim/simResync.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `demolition`: não se aplica 1
- porte `interface`: outra sessão 1, portado 1
- porte `people`: já existia 1
- porte `roads`: não se aplica 1
- porte `traffic`: não se aplica 1

## Código próprio (linhas — declaração — comentário)

### src/sim/sim.worker.ts
- 5-5 `scope` (variable): The simulation's worker (`host/simHost.ts`): the page's messages answered in order, one at a time.
- 6-6 `host` (variable)
- 7-7 `queue` (variable)
- 8-13 `ExpressionStatement` (statement)
