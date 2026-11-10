# editor:bulldozer

Grupo: editor. Entrada: ferramenta. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/editor/bulldozer.ts`
- Unidades próprias (só esta funcionalidade alcança): 15 em 1 arquivo(s), 222 linhas
- Alcance total: 819 unidades; usa 804 compartilhadas de outras; é dona principal de 0 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#showInspector@3573` (2), `src/main.ts#gameKeys@2479` (1), `src/main.ts#bulldozer@1105` (1), `src/main.ts#currentGesture@1056` (1), `src/main.ts#gestureInProgress@1117` (1), `src/main.ts#cancelGestures@1041` (1), `src/main.ts#pointerDown@1217` (1), `src/main.ts#pointerMove@1404` (1), `src/main.ts#endPointer@1484` (1), `src/main.ts#drawOverlayScreen@2942` (1)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `worker:conflictTable` (36), `world:doc` (16), `editor:snap` (8), `world:buildings` (5), `world:utilities` (4), `world:landscape` (3), `world:ribbons` (3), `world:network` (2), `editor:roads-economy` (2), `editor:barriers` (2), `world:lot` (2), `world:barriers` (2)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `RoadDoc.buildings`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/editor/bulldozer.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `demolition`: portado 2
- porte `economy`: portado 2
- porte `street-furniture`: portado 2

## Código próprio (linhas — declaração — comentário)

### src/editor/bulldozer.ts
- 13-25 `BulldozerHost` (interface): What the bulldozer reads of the game and does to it.
- 28-36 `removeRoads` (function): Roads taken away as the player demolishes them, by any tool: their power line and furniture with them, a share of their price back (`editor/roads/economy.ts`).
- 39-39 `CLICK_PIXELS` (variable): A press that moved less than this many screen pixels is a click.
- 42-150 `Bulldozer` (class): THE BULLDOZER: a click removes the one thing under the pointer
- 44-44 `Bulldozer.drag` (field): The box being dragged (screen pixels in the canvas, and the ground under its corners), or the press while it stays a click.
- 46-46 `Bulldozer.constructor` (method)
- 48-50 `Bulldozer.gesture` (method)
- 52-54 `Bulldozer.inProgress` (method)
- 56-58 `Bulldozer.cancel` (method)
- 61-63 `Bulldozer.down` (method): A press at screen point `screen` over `world`, `anchor` what is under it.
- 66-72 `Bulldozer.move` (method): @perInput The pointer moved; true when it was this tool's drag.
- 75-84 `Bulldozer.up` (method): The pointer let go: the click or the box is carried out (`commit` false: dropped).
- 87-91 `Bulldozer.box` (method): The box on the map while it is dragged (no longer a click), for the overlay.
- 94-122 `Bulldozer.click` (method): @perInput The bulldozer's click: the one thing under the pointer.
- 125-149 `Bulldozer.boxed` (method): Everything inside the box removed in one undo step: roads
