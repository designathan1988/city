# world:ids

Grupo: world. Entrada: subsistema. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/world/ids.ts`
- Unidades próprias (só esta funcionalidade alcança): 1 em 1 arquivo(s), 3 linhas
- Alcance total: 18 unidades; usa 0 compartilhadas de outras; é dona principal de 17 compartilhadas

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
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/core/historyGuards.spec.ts`, `tests/editor/buildPreview.spec.ts`, `tests/editor/economy.spec.ts`, `tests/editor/poles.spec.ts`, `tests/editor/signRule.spec.ts`, `tests/editor/snapNear.spec.ts`, `tests/editor/turnBans.spec.ts`, `tests/editor/uturn.spec.ts`, `tests/perf/fillEdit.spec.ts`, `tests/render/poleLayer.spec.ts`, `tests/render/roadTileWorkers.spec.ts`, `tests/render/signLayer.spec.ts`, `tests/render/signalBatches.spec.ts`, `tests/render/streetFurnitureLayer.spec.ts`, `tests/scale/roadJudge.spec.ts`, `tests/scale/roadKeys.spec.ts`, `tests/sim/advisorReach.spec.ts`, `tests/sim/boxPedestrians.spec.ts`, `tests/sim/busesAway.spec.ts`, `tests/sim/junctionRules.spec.ts`, `tests/sim/laneConnectors.spec.ts`, `tests/sim/laneletGraphCache.spec.ts`, `tests/sim/lotPlanWorker.spec.ts`, `tests/sim/microWindowCells.spec.ts`, `tests/sim/rebindReach.spec.ts`, `tests/sim/signalPlanWindow.spec.ts`, `tests/sim/signalPosts.spec.ts`, `tests/sim/signalRest.spec.ts`, `tests/sim/simClientAnswers.spec.ts`, `tests/sim/simClientSignalsReset.spec.ts`, `tests/sim/simInitBake.spec.ts`, `tests/sim/simResync.spec.ts`, `tests/sim/transitStops.spec.ts`, `tests/sim/walkArrivals.spec.ts`, `tests/ui/connectorPanel.spec.ts`, `tests/world/carriedRoad.spec.ts`, `tests/world/conflictCache.spec.ts`, `tests/world/conflictWindow.spec.ts`, `tests/world/crossingLandings.spec.ts`, `tests/world/docFork.spec.ts`, `tests/world/elevationLocal.spec.ts`, `tests/world/incrementalRebuild.spec.ts`, `tests/world/junction.spec.ts`, `tests/world/junctionConnectors.spec.ts`, `tests/world/networkDelta.spec.ts`, `tests/world/nodeSettingsKept.spec.ts`, `tests/world/roadFieldPrune.spec.ts`, `tests/world/roadFurniture.spec.ts`, `tests/world/roadPaint.spec.ts`, `tests/world/roadProfile.spec.ts`, `tests/world/roadStations.spec.ts`, `tests/world/roadValueLedger.spec.ts`, `tests/world/solids.spec.ts`, `tests/world/tileIndex.spec.ts`, `tests/world/transitionLevels.spec.ts`, `tests/world/turnOffSurface.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- nenhuma cobertura de porte cita os arquivos desta funcionalidade

## Código próprio (linhas — declaração — comentário)

### src/world/ids.ts
- 22-24 `IdAllocator.constructor` (method)
