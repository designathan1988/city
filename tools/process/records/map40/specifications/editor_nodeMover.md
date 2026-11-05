# editor:nodeMover

Grupo: editor. Entrada: ferramenta. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/editor/nodeMover.ts`
- Unidades próprias (só esta funcionalidade alcança): 14 em 1 arquivo(s), 183 linhas
- Alcance total: 1425 unidades; usa 1411 compartilhadas de outras; é dona principal de 0 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#frame@2644` (4), `src/main.ts#mover@569` (1), `src/main.ts#gestureInProgress@1117` (1), `src/main.ts#growthAnswered@1998` (1), `src/main.ts#growZones@2597` (1), `src/main.ts#currentGesture@1056` (1), `src/main.ts#pointerDown@1217` (1), `src/main.ts#pointerMove@1404` (1), `src/main.ts#cancelMove@1129` (1), `src/main.ts#endPointer@1484` (1)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `world:doc` (19), `editor:commit` (8), `world:ids` (4), `worker:conflictTable` (4), `world:network` (3), `editor:roads-streetKit` (1)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `RoadNode.incident`, `RoadNode.x`, `RoadNode.y`, `RoadSegment.curve`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- nenhum spec importa os arquivos desta funcionalidade

## Estado no jogo (lido de ports/*/coverage.md)
- porte `roads`: portado 28

## Código próprio (linhas — declaração — comentário)

### src/editor/nodeMover.ts
- 11-20 `NodeMoverHost` (interface): What the node mover reads of the game and does to it.
- 23-23 `PREVIEW_MIN_MS` (variable): Shortest interval between geometry rebuilds while a node is dragged
- 26-120 `NodeMover` (class): DO NOT keep the whole document (`toJSON` at the press, `replaceFromJSON` at the release)
- 27-27 `NodeMover.drag` (field)
- 28-28 `NodeMover.lastPreview` (field)
- 30-30 `NodeMover.constructor` (method)
- 32-34 `NodeMover.dragging` (method)
- 37-39 `NodeMover.gesture` (method): The gesture in progress, as a dictionary key (the interface translates it), or null.
- 42-53 `NodeMover.grab` (method): A press on `node`: the drag begins (false when there is no such node).
- 56-61 `NodeMover.move` (method): @perInput The pointer moved: the node follows it, live.
- 64-68 `NodeMover.previewDue` (method): Whether the network may be rebuilt for the preview at `now` (ms): no sooner than the last rebuild's own cost allows.
- 71-77 `NodeMover.cancel` (method): The drag cut short: the node and its roads as they were before it.
- 80-109 `NodeMover.drop` (method): The pointer let go: the move recorded as one undo step (`commit` false: dropped).
- 112-119 `NodeMover.putBack` (method): The node back where the drag found it, each of its roads with the curve it had then; the network after them.
