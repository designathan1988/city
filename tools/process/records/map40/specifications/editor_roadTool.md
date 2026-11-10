# editor:roadTool

Grupo: editor. Entrada: ferramenta. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/editor/roadTool.ts`
- Unidades próprias (só esta funcionalidade alcança): 57 em 1 arquivo(s), 952 linhas
- Alcance total: 1602 unidades; usa 1545 compartilhadas de outras; é dona principal de 0 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#drawOverlayScreen@2942` (16), `src/main.ts#pointerMove@1404` (5), `src/main.ts#pointerDown@1217` (3), `src/main.ts#roadTool@458` (1), `src/main.ts#frame@2644` (1), `src/main.ts#currentGesture@1056` (1), `src/main.ts#pointerWorld@1025` (1), `src/main.ts#gestureInProgress@1117` (1), `src/main.ts#applySnapshot@915` (1), `src/main.ts#cancelGestures@1041` (1), `src/main.ts#openSession@2273` (1), `src/main.ts#afterRestore@2353` (1)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `editor:snap` (89), `worker:conflictTable` (75), `editor:commit` (34), `world:doc` (28), `editor:roadPath` (21), `editor:barriers` (15), `world:grid` (12), `world:roads` (10), `world:road` (9), `world:network` (7), `editor:buildings` (5), `world:landscape` (5)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `RoadDoc.changes`, `RoadSegment.direction`, `RoadSegment.id`, `RoadSegment.lanes`, `RoadSegment.parking`, `RoadSegment.section`, `RoadSegment.type`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/editor/longStroke.spec.ts`, `tests/editor/roadVerdict.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `economy`: portado 2
- porte `roads`: portado 50, não se aplica 20, em parte 22, não portado 22
- porte `street-furniture`: portado 2
- porte `traffic`: já existia 2

## Código próprio (linhas — declaração — comentário)

### src/editor/roadTool.ts
- 33-33 `VERDICT_DELAY` (variable): How long a draft rests before the preview judges it (`RoadTool.verdict`), ms.
- 36-36 `RefusalReason` (type): A refusal the preview names: the editing rules', or a crossing at the wrong height.
- 38-38 `REFUSALS` (variable)
- 39-41 `isRefusal` (function)
- 44-53 `RoadDraft` (interface): A road being drawn: a drag, or a stretch chained from the last road's end.
- 56-62 `CurvePending` (interface): A curve with its end put down, waiting for the click that sets its bend.
- 65-100 `RoadToolHost` (interface): What the road tool reads of the game and does to it.
- 103-569 `RoadTool` (class): THE ROAD TOOL: a road drawn by a drag
- 105-105 `RoadTool.draft` (field): The drag in progress.
- 107-107 `RoadTool.chain` (field): The end of the last stretch, while the road is being chained.
- 108-108 `RoadTool.chainHeight` (field)
- 109-109 `RoadTool.chainPreview` (field)
- 110-110 `RoadTool.curvePending` (field)
- 112-112 `RoadTool.pointerScreen` (field): The pointer on the screen while the tool is in hand (the height step carries the stroke from it).
- 114-114 `RoadTool.shift` (field): How far a drag's stroke is carried, in plan, by the heights changed during it
- 116-116 `RoadTool.heightEdited` (field): The height was set by hand since the stroke began (a start on a road does not take that road's height then).
- 118-118 `RoadTool.settling` (field): The road just committed, drawn as previewed until the world with it is built.
- 120-120 `RoadTool.constructor` (method)
- 122-128 `RoadTool.gesture` (method)
- 131-133 `RoadTool.bending` (method): A curve's end is down and its bend follows the pointer.
- 135-137 `RoadTool.inProgress` (method)
- 139-144 `RoadTool.cancel` (method)
- 147-149 `RoadTool.dropCurve` (method): The plan is no longer a curve: a curve waiting for its bend is dropped.
- 152-155 `RoadTool.reset` (method): A new map: drawn at the land's own height again.
- 158-161 `RoadTool.heightChosen` (method): A height chosen from outside the tool (the console).
- 164-166 `RoadTool.pointerAt` (method): The pointer over the map, on the screen.
- 169-171 `RoadTool.anchorHeight` (method): Height of an authored connection; open ground takes the height being drawn at.
- 174-176 `RoadTool.atHeight` (method): A nearby road at another height is a crossing, not an accidental junction.
- 179-195 `RoadTool.bend` (method): A press at `world` while a curve waits for its bend: the curve is laid bent there. True when there was one.
- 198-237 `RoadTool.down` (method): A press at `world`, `anchor` what is under it: a stroke begins (from the chain's end when chaining).
- 240-255 `RoadTool.move` (method): @perInput The pointer moved; true when the curve's bend or the drag took it.
- 258-271 `RoadTool.hover` (method): @perInput The pointer over the map with no drag: the next stretch of a chain, previewed.
- 274-304 `RoadTool.up` (method): @perInput The pointer let go: a click starts a chain, a curve waits for its bend, a drag is laid (`commit` false: dropped).
- 307-344 `RoadTool.stepHeight` (method): PageUp/PageDown or the height buttons: the road is drawn a metre higher or lower, landing on whole metres
- 347-353 `RoadTool.ending` (method): The end a draft is laid to (`chosenEnd`, or what is under its end), its height and its pieces.
- 356-369 `RoadTool.verdict` (method): Why the road in hand would be refused (`editRules.ts`, or a crossing at the wrong height), or null
- 371-372 `RoadTool.judged` (field)
- 375-378 `RoadTool.stations` (method): The road in hand as it would be built: the dry run's stations
- 381-384 `RoadTool.cost` (method): What the road in hand would cost (`world/economy.ts`), judged with the verdict on the same dry run, or null until it is
- 386-386 `RoadTool.judgeAt` (field): When the draft in hand will have rested long enough to be judged (`verdict`); null: nothing waiting.
- 389-399 `RoadTool.draftKey` (method): DO NOT key the draft on the whole map's revision (every tile that came in judged it again) nor make its key a text each frame: its numbers, compared as they are, and `mapAround` for the map.
- 400-400 `RoadTool.lastKey` (field)
- 403-415 `RoadTool.mapAround` (method): What the dry run reads of the map moved near the draft: a serial, moved by a change of the diary whose place meets the draft's box (or the balance, or the whole map).
- 416-416 `RoadTool.mapSeen` (field)
- 417-417 `RoadTool.mapSerial` (field)
- 419-431 `RoadTool.judge` (method)
- 434-437 `RoadTool.commit` (method): Lays the road `d` ends at (`chosenEnd`, or what is under its end), one undo step; false when refused.
- 440-514 `RoadTool.layPart` (method): One part of a road laid from `start` to `end`, one undo step (joined into `group`'s); `last`: the gesture ends with it. False when refused.
- 517-531 `RoadTool.preview` (method): What the overlay draws: the curve waiting for its bend, the drag, the chain's next stretch
- 534-544 `RoadTool.pieces` (method): The pieces a draft is laid as: one straight or curved, or the freehand path's.
- 547-568 `RoadTool.curveOf` (method): The curve of a draft: through its control point, or bent as far as the drag went off the chord.
- 572-581 `landscapeAlong` (function): The placed items the furniture of `laid` can meet
- 584-584 `FURNITURE_CELL` (variable): The side of `furnitureFor`'s cells (`world/roads/furnitureSets.ts` CELL).
- 587-587 `DraftKey` (type): A draft's numbers as it is judged (`RoadTool.draftKey`): the same array while none of them moved.
- 590-597 `pushSample` (function): Every sample of a stroke kept as it comes up to `DENSE_SAMPLES`; past them, one a metre or a height changed, up to `MOST_SAMPLES` (EDT-11: past the first 256 every sample was dropped, and a long stroke ran straight from its 256th to its end
- 598-598 `DENSE_SAMPLES` (variable)
- 599-599 `MOST_SAMPLES` (variable)
