# render:quality

Grupo: render. Entrada: desenho. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/quality.ts`
- Unidades próprias (só esta funcionalidade alcança): 27 em 1 arquivo(s), 118 linhas
- Alcance total: 40 unidades; usa 4 compartilhadas de outras; é dona principal de 9 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/graphicsWiring.ts#createGraphicsWiring@118` (5), `src/graphicsWiring.ts#isQualityPreset@13` (1), `src/main.ts#lab@2574` (1)

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
- `tests/core/weakMachineTier.spec.ts`, `tests/perf/budgets.spec.ts`, `tests/perf/geometryBudget.spec.ts`, `tests/perf/largeOpening.spec.ts`, `tests/perf/noHoles.spec.ts`, `tests/render/qualityGovernor.spec.ts`, `tests/render/vehicleCalls.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `interface`: portado 62, não se aplica 10

## Código próprio (linhas — declaração — comentário)

### src/render/quality.ts
- 118-118 `NETWORK_FAR_MB` (variable): The network's far level a city of 25,000 buildings keeps for good (`networkFarRoads.ts`), MB, with room: what `ROAD_BUDGET_MB` counts it at.
- 120-120 `roadBudgetMB` (function): The roads' surfaces' whole on each tier, MB: their near and far levels' budgets and the network's far level (see `GEOMETRY_BUDGETS`).
- 268-269 `isQualityLevel` (function)
- 272-272 `WINDOW` (variable): Frames read per window: a p90 over a second and a half at 60 Hz.
- 274-274 `SETTLE` (variable): Frames left out after a tier changes: its programs are built and its targets made, and the GPU's timings come back a few frames late.
- 276-276 `DOWN_WINDOWS` (variable): A tier is dropped after this many windows in a row whose p90 missed the frame's budget (one slow window is a road built, a city opened).
- 278-278 `UP_WINDOWS` (variable): A tier is raised after this many windows in a row whose p90 stayed under `UP_SHARE` of the budget: rising wants more room than falling (the hysteresis).
- 279-279 `UP_SHARE` (variable)
- 281-281 `UNDONE_WITHIN` (variable): A raise undone within this long, s: the machine cannot hold that tier; none tried again for `HOLD` s, twice as long each time it happens again.
- 282-282 `HOLD` (variable)
- 284-284 `STALL` (variable): A frame longer than this is a stall (a rebuild, the tab hidden), not a frame rate, s.
- 287-343 `QualityGovernor` (class): THE AUTOMATIC TIER: the 90th percentile of each window's frame cost (the GPU's time, or the frame's own where no timer answers) against the frame's budget; down after `DOWN_WINDOWS` missed windows, up after `UP_WINDOWS` roomy ones, never ab
- 288-288 `QualityGovernor.costs` (field)
- 289-289 `QualityGovernor.sorted` (field)
- 290-290 `QualityGovernor.count` (field)
- 291-291 `QualityGovernor.skip` (field)
- 292-292 `QualityGovernor.missed` (field)
- 293-293 `QualityGovernor.roomy` (field)
- 294-294 `QualityGovernor.clock` (field)
- 295-295 `QualityGovernor.lastRaise` (field)
- 296-296 `QualityGovernor.holdUntil` (field)
- 297-297 `QualityGovernor.hold` (field)
- 298-298 `QualityGovernor.ceiling` (field)
- 300-302 `QualityGovernor.constructor` (method)
- 304-306 `QualityGovernor.current` (method)
- 309-315 `QualityGovernor.set` (method): The tier changed (by this governor or by the player): the windows start again once it has settled.
- 318-342 `QualityGovernor.sample` (method): One frame: `deltaSeconds` since the last, `costMs` what it cost, `budgetMs` the frame it must fit (the refresh, or the player's cap). The tier to switch to, or null to stay.
