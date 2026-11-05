# sim:host

Grupo: sim. Entrada: subsistema. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/sim/host/fleetGrid.ts`, `src/sim/host/protocol.ts`, `src/sim/host/simAnswers.ts`, `src/sim/host/simClient.ts`, `src/sim/host/simHost.ts`
- Unidades próprias (só esta funcionalidade alcança): 1 em 1 arquivo(s), 1 linhas
- Alcance total: 4694 unidades; usa 4576 compartilhadas de outras; é dona principal de 117 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- nenhuma chamada direta da raiz: chega por outra funcionalidade

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): `recebe:act`, `recebe:ask`, `recebe:delta`, `recebe:frame`, `recebe:getUp`, `recebe:greenWave`, `recebe:impact`, `recebe:init`, `recebe:junction`, `recebe:laneLost`, `recebe:lanes`, `recebe:load`, `recebe:lotPlan`, `recebe:nodeLanes`, `recebe:removeVehicle`, `recebe:removeVehiclesNear`, `recebe:reset`, `recebe:resync`, `recebe:segment`, `recebe:shot`, `recebe:strand`, `recebe:window`
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: -
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `?.act`, `?.all`, `?.ambient`, `?.answer`, `?.ask`, `?.bake`, `?.bays`, `?.buildings`, `?.casualties`, `?.code`, `?.conflicts`, `?.controls`, `?.cos`, `?.crawl`, `?.delta`, `?.detail`, `?.dx`, `?.dy`, `?.ex`, `?.ey`, `?.far`, `?.frames`, `?.fromX`, `?.fromY`, `?.heading`, `?.hold`, `?.id`, `?.junctions`, `?.kill`, `?.kind`, `?.layout`, `?.map`, `?.meso`, `?.message`, `?.node`, `?.nodes`, `?.parked`, `?.part`, `?.ped`, `?.peds`, `?.r`, `?.result`, `?.scare`, `?.seconds`, `?.seed`, `?.segments`, `?.serial`, `?.service`, `?.signals`, `?.slot`, `?.stats`, `?.subject`, `?.tick`, `?.trains`, `?.type`, `?.vehicles`, `?.wall`, `?.whole`, `?.why`, `?.wounds` … (+103)
- Escreve: `SimWorld.auditEnabled`, `SimWorld.auditEvery`, `SimWorld.auditLevel`, `SimWorld.demandMultiplier`, `SimWorld.focus`, `SimWorld.followCity`, `SimWorld.meso`, `SimWorld.pedestrianIntensity`, `SimWorld.populationShare`, `SimWorld.topologyDeferred`, `SimWorld.trafficIntensity`

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/render/agentsParked.spec.ts`, `tests/render/agentsResidency.spec.ts`, `tests/render/densityCars.spec.ts`, `tests/render/pageGarbage.spec.ts`, `tests/render/signalBatches.spec.ts`, `tests/render/vehicleCalls.spec.ts`, `tests/render/vehiclesInView.spec.ts`, `tests/sim/fleetGrid.spec.ts`, `tests/sim/lotPlanWorker.spec.ts`, `tests/sim/meso/stepGarbage.spec.ts`, `tests/sim/simClientAnswers.spec.ts`, `tests/sim/simClientDecode.spec.ts`, `tests/sim/simClientSignalsReset.spec.ts`, `tests/sim/simClientStart.spec.ts`, `tests/sim/simInitBake.spec.ts`, `tests/sim/simResync.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `traffic`: não se aplica 6
- porte `demolition`: não se aplica 3
- porte `people`: já existia 3
- porte `interface`: outra sessão 1, portado 1
- porte `roads`: não se aplica 1
- porte `transit`: portado 1

## Código próprio (linhas — declaração — comentário)

### src/sim/host/fleetGrid.ts
- 29-29 `FleetGrid.size` (method)
