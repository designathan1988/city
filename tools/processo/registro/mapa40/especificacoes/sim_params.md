# sim:params

Grupo: sim. Entrada: subsistema. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/sim/params.ts`
- Unidades próprias (só esta funcionalidade alcança): 2 em 1 arquivo(s), 2 linhas
- Alcance total: 38 unidades; usa 4 compartilhadas de outras; é dona principal de 32 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#syncPopulationShare@306` (2)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: -
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/render/agentsParked.spec.ts`, `tests/render/citizenLocomotion.spec.ts`, `tests/render/vehicleSignals.spec.ts`, `tests/sim/admissionGroup.spec.ts`, `tests/sim/agents/citySpin.spec.ts`, `tests/sim/agents/lotDoors.spec.ts`, `tests/sim/agents/lotParked.spec.ts`, `tests/sim/agents/parkedInLot.spec.ts`, `tests/sim/agents/population.spec.ts`, `tests/sim/ambientMeso.spec.ts`, `tests/sim/boxPedestrians.spec.ts`, `tests/sim/bulb.spec.ts`, `tests/sim/busesAway.spec.ts`, `tests/sim/cityClock.spec.ts`, `tests/sim/classJoin.spec.ts`, `tests/sim/clockBudget.spec.ts`, `tests/sim/collisions.spec.ts`, `tests/sim/curvature.spec.ts`, `tests/sim/destinationRouting.spec.ts`, `tests/sim/drive/convoy.spec.ts`, `tests/sim/drive/entryRate.spec.ts`, `tests/sim/drivers.spec.ts`, `tests/sim/fourWay.spec.ts`, `tests/sim/junctionRules.spec.ts`, `tests/sim/kerb.spec.ts`, `tests/sim/kerbStops.spec.ts`, `tests/sim/laneConnectors.spec.ts`, `tests/sim/lotPlaces.spec.ts`, `tests/sim/meso/boundedTick.spec.ts`, `tests/sim/meso/window.spec.ts`, `tests/sim/passKerbStop.spec.ts`, `tests/sim/pedHold.spec.ts`, `tests/sim/populationFollowsCity.spec.ts`, `tests/sim/pose.spec.ts`, `tests/sim/priorityBox.spec.ts`, `tests/sim/queuedSwing.spec.ts`, `tests/sim/reservationsChecked.spec.ts`, `tests/sim/reset.spec.ts`, `tests/sim/resetLikeNew.spec.ts`, `tests/sim/roundabout.spec.ts`, `tests/sim/shortLinkBox.spec.ts`, `tests/sim/signalRest.spec.ts`, `tests/sim/signals.spec.ts`, `tests/sim/traffic.spec.ts`, `tests/sim/uturn.spec.ts`, `tests/sim/walkHeight.spec.ts`, `tests/sim/zipperMerge.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `traffic`: já existia 2

## Código próprio (linhas — declaração — comentário)

### src/sim/params.ts
- 123-123 `NARROW_SCREEN_SHARE` (variable): Share of the ceiling allowed on a narrow screen.
- 125-125 `NARROW_SCREEN_WIDTH` (variable): Viewport width, CSS pixels, under which a screen counts as narrow.
