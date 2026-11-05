# editor:barriers

Grupo: editor. Entrada: ferramenta. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/editor/barriers.ts`
- Unidades próprias (só esta funcionalidade alcança): 18 em 1 arquivo(s), 221 linhas
- Alcance total: 151 unidades; usa 121 compartilhadas de outras; é dona principal de 12 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#drawBarrierPlan@2786` (2), `src/main.ts#barrierTool@515` (1), `src/main.ts#currentGesture@1056` (1), `src/main.ts#cancelGestures@1041` (1), `src/main.ts#pointerDown@1217` (1), `src/main.ts#pointerMove@1404` (1), `src/main.ts#gameKeys@2479` (1)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `worker:conflictTable` (57), `world:barriers` (10), `world:road` (8), `world:ribbons` (7), `world:network` (5), `world:buildings` (3), `editor:snap` (2), `world:doc` (2)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `RoadSegment.structure`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/editor/barrierShiftClick.spec.ts`, `tests/ui/visibleText.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `street-furniture`: portado 38

## Código próprio (linhas — declaração — comentário)

### src/editor/barriers.ts
- 12-12 `FOOTWAY_REACH` (variable): How far from the back of a footway a point is still put onto it, world units.
- 14-14 `MIN_BARRIER_RUN` (variable): Shortest length of a run worth building, world units.
- 16-37 `snapBarrierPoint` (function)
- 40-61 `barrierProblem` (function): Why a run cannot be built, or null: it reaches a road or its footway, as a building's own parts may not
- 64-73 `BarrierToolHost` (interface): What the walls tool reads of the game and does to it.
- 76-162 `BarrierTool` (class): THE WALLS TOOL: a wall, fence or hedge traced point by point
- 78-78 `BarrierTool.points` (field): The points put down so far; null when no run is being traced.
- 80-80 `BarrierTool.lastClick` (field): The last click, to tell a double click (which ends the run).
- 82-82 `BarrierTool.cursor` (field): The pointer over the map.
- 84-84 `BarrierTool.constructor` (method)
- 87-89 `BarrierTool.gesture` (method): The gesture in progress, as a dictionary key (the interface translates it), or null.
- 91-93 `BarrierTool.cancel` (method)
- 96-114 `BarrierTool.down` (method): @perInput A press at `world` (screen position `screenX/Y`, `detail` the click count the browser reports).
- 117-120 `BarrierTool.move` (method): @perInput Runs once a pointer move: nothing it reaches may walk the map.
- 123-134 `BarrierTool.key` (method): A key while a run is traced; true when the tool took it.
- 137-152 `BarrierTool.finish` (method): Builds the run traced so far, in one undo step, or says why it cannot be.
- 155-161 `BarrierTool.plan` (method): The run as it will stand, for the overlay: its points
- 165-174 `onCarriageway` (function): Whether any corner of a footprint is on a carriageway or its kerb (a kerb barrier may stand on the footway).
