# editor:poles

Grupo: editor. Entrada: ferramenta. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/editor/poles.ts`
- Unidades próprias (só esta funcionalidade alcança): 20 em 1 arquivo(s), 225 linhas
- Alcance total: 591 unidades; usa 539 compartilhadas de outras; é dona principal de 32 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#drawPolePlan@2849` (5), `src/main.ts#IfStatement@4036` (3), `src/main.ts#poleTool@488` (1), `src/main.ts#poleReach@1480` (1), `src/main.ts#currentGesture@1056` (1), `src/main.ts#gestureInProgress@1117` (1), `src/main.ts#cancelGestures@1041` (1), `src/main.ts#frame@2644` (1), `src/main.ts#pointerDown@1217` (1), `src/main.ts#pointerMove@1404` (1), `src/main.ts#endPointer@1484` (1)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `worker:conflictTable` (14), `world:doc` (7), `world:utilities` (6), `world:network` (3), `world:pole` (1)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `RoadDoc.changes`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/editor/poleCrossing.spec.ts`, `tests/editor/poleNoPath.spec.ts`, `tests/editor/poleTool.spec.ts`, `tests/editor/poles.spec.ts`, `tests/editor/powerLinePlanner.spec.ts`, `tests/world/poleLinesCells.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `street-furniture`: portado 122

## Código próprio (linhas — declaração — comentário)

### src/editor/poles.ts
- 17-17 `POLE_PICK_PIXELS` (variable): Pick radius for an existing pole, in screen pixels.
- 50-52 `snapPole` (function): Snaps one end of a pole run: onto a pole already standing
- 268-273 `planPoleRunWhole` (function): `planPoleRun` routed over the lines of a region covering the whole map (every street within `ROUTE_MARGIN_MOST`)
- 368-370 `commitPoleRun` (function): Applies a plan to the document.
- 391-404 `PoleToolHost` (interface): What the pole tool reads of the game and does to it.
- 407-515 `PoleTool` (class): THE POLE TOOL: a distribution line traced as straight stretches, each a drag
- 409-409 `PoleTool.draft` (field): The stretch being dragged.
- 411-411 `PoleTool.chain` (field): The end of the last committed run, while the tool is still on it
- 413-413 `PoleTool.hover` (field): The pointer over the map while the tool is in hand, unsnapped.
- 414-414 `PoleTool.lastMode` (field)
- 416-416 `PoleTool.constructor` (method)
- 419-421 `PoleTool.reach` (method): Pick radius for a pole, in WORLD units at the current zoom
- 423-425 `PoleTool.gesture` (method)
- 427-429 `PoleTool.inProgress` (method)
- 431-434 `PoleTool.cancel` (method)
- 437-449 `PoleTool.plan` (method): What the current gesture would build, snapped: drawn and committed alike.
- 451-470 `PoleTool.down` (method)
- 473-485 `PoleTool.move` (method): @perInput The pointer moved; true when a stretch being dragged took it.
- 488-503 `PoleTool.up` (method): @perInput The pointer let go: the stretch is built (`commit`), or dropped.
- 506-514 `PoleTool.marks` (method): What the overlay marks besides the run drawn in 3D: with the Remove verb, the pole under the pointer
