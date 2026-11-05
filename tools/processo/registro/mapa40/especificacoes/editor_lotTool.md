# editor:lotTool

Grupo: editor. Entrada: ferramenta. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/editor/lotTool.ts`
- Unidades próprias (só esta funcionalidade alcança): 94 em 1 arquivo(s), 1488 linhas
- Alcance total: 512 unidades; usa 418 compartilhadas de outras; é dona principal de 0 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#drawOverlayScreen@2942` (10), `src/main.ts#gameState@393` (2), `src/main.ts#pointerMove@1404` (2), `src/main.ts#lotTool@530` (1), `src/main.ts#growZones@2597` (1), `src/main.ts#IfStatement@4036` (1), `src/main.ts#currentGesture@1056` (1), `src/main.ts#gestureInProgress@1117` (1), `src/main.ts#cancelGestures@1041` (1), `src/main.ts#setZone@1617` (1), `src/main.ts#pointerDown@1217` (1), `src/main.ts#endPointer@1484` (1)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `worker:conflictTable` (222), `world:lot` (72), `world:lots` (30), `world:doc` (19), `editor:barriers` (18), `world:buildings` (11), `world:network` (8), `world:zones` (8), `world:cells` (4), `world:derived` (4), `world:nearest` (4), `world:changes` (3)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `RoadDoc.buildings`, `RoadDoc.changes`, `RoadDoc.lotStore`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/editor/cutNarrowLot.spec.ts`, `tests/editor/lotOverlayKey.spec.ts`, `tests/editor/lotProposalPlace.spec.ts`, `tests/editor/lotSnapOnce.spec.ts`, `tests/editor/zoneStroke.spec.ts`, `tests/scale/lotEdit.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- nenhuma cobertura de porte cita os arquivos desta funcionalidade

## Código próprio (linhas — declaração — comentário)

### src/editor/lotTool.ts
- 21-21 `REFUSED_REACH` (variable): How far from a road edit a refused lot is tried again: its frontage and setbacks read the roads that near, u.
- 22-22 `STROKE_STEP` (variable)
- 27-27 `ZoneMode` (type): Brush paints the cells under the pointer; the other modes edit the lots.
- 29-29 `LotSplitKind` (type): How the split tool cuts (`LotCut`): across the front, parallel to it, or along a drawn line.
- 32-46 `LotToolHost` (interface): What the tool reads of the game and does to it.
- 49-56 `LotOverlay` (interface): What the scene draws of the lots, a group at a time (the shape of `render/lotOverlay.ts` `LotOverlayShapes`), and the labels.
- 58-58 `ZONE_COLOURS` (variable)
- 60-60 `PROPOSAL_CELL` (variable): Side of the grid the proposed lots are filed in for the pointer (`proposedAt`): a few lots wide, as the planner files them.
- 62-62 `PROPOSAL_RANK` (variable): Drawing ranks of the overlay's groups (`render/lotOverlay.ts`, under 2^29): the lots by their order, then the proposal, then the gesture.
- 63-63 `GESTURE_RANK` (variable)
- 65-65 `NO_PROPOSAL` (variable): No proposal drawn (out of the tool, or not worked out for the land as it is).
- 67-67 `DRAG_REACH` (variable): How near a dragged corner the corners it moves are (`drawLot`).
- 70-79 `LotDecor` (interface): What a lot's group is drawn with besides the lot itself (`LotTool.drawLot`): -1 for no lot.
- 82-84 `LotOverlayGroup` (interface): One group of the overlay: its shapes and its rank in the drawing order (`render/lotOverlay.ts` `LotOverlayGroup`).
- 87-114 `LotOverlayBook` (class): DO NOT hand the scene every lot again for a pointer move: the overlay by group, each put again only when it changed
- 89-89 `LotOverlayBook.groups` (field): Each lot's (`l` + id), each proposed lot's (`p` + a number) and the gesture's (`g`).
- 91-91 `LotOverlayBook.labels` (field): The labels for the 2D layer: a lot's size, a side's length.
- 93-93 `LotOverlayBook.reader` (field): The one reader the changes are kept for; another one is told everything.
- 94-94 `LotOverlayBook.changed` (field)
- 97-101 `LotOverlayBook.put` (method): A group put (null: taken away).
- 104-113 `LotOverlayBook.changedFor` (method): The groups put or taken away since `reader` last asked; null: all of them (another reader than the last).
- 116-857 `LotTool` (class)
- 118-118 `LotTool.hover` (field): The pointer over the map while the tool is in hand.
- 120-120 `LotTool.refused` (field): Lots on which nothing would grow, skipped by the growth until the land or the roads change.
- 122-122 `LotTool.refusedNet` (field): The network's state the refused lots were last brought up to (`refusedNow`).
- 125-125 `LotTool.polygon` (field): The corners of a lot being drawn point by point (the polygon mode).
- 127-127 `LotTool.cutLine` (field): A drawn cut line, and a side being curved.
- 128-128 `LotTool.curve` (field)
- 131-131 `LotTool.stroke` (field): A zone stroke: the lots it passed, the proposed lots it passed
- 133-133 `LotTool.waiting` (field): A stroke let go before the proposal it waits on was worked out: made in the frame the proposal is done
- 135-135 `LotTool.proposal` (field): The land along the streets cut into lots while the Zoning tool is in hand, as Cities
- 137-137 `LotTool.proposalFiling` (field): The proposed lots by place (`proposedAt`), filed for the list they were filed from.
- 138-138 `LotTool.corner` (field)
- 139-139 `LotTool.drawn` (field)
- 140-140 `LotTool.joinFirst` (field)
- 142-142 `LotTool.erase` (field): A stroke of the delete mode: the lots and buildings it has passed over, removed on release as one undo step.
- 144-144 `LotTool.constructor` (method)
- 147-147 `LotTool.zonedIds` (field): The zoned lots, kept by the lots the diary names (`anyZoned`).
- 148-148 `LotTool.zonedCursor` (field)
- 151-161 `LotTool.anyZoned` (method): Whether any lot is zoned: the faint zones are shown with the other tools. DO NOT walk every lot when one changes.
- 164-164 `LotTool.held` (field): Whether the quiet overlay is held as it is while lots keep changing (`holdOverlay`).
- 166-168 `LotTool.holdOverlay` (method): Holds the quiet overlay as it is (a city growing, a lot a slice); released, the next frame lays it again.
- 170-170 `LotTool.doc` (method)
- 171-171 `LotTool.net` (method)
- 174-184 `LotTool.refusedNow` (method): DO NOT let every refused lot of the town go for an edit
- 187-197 `LotTool.gesture` (method): What the player is in the middle of doing with it, named (`main.ts` `currentGesture`).
- 200-211 `LotTool.cancel` (method): Every gesture dropped without committing it.
- 214-217 `LotTool.modeChanged` (method): Another mode picked: a polygon half drawn or a join half chosen is dropped.
- 222-222 `LotTool.proposalNet` (field): The network's state and the diary's lot entries the proposal key last saw (`proposalKey`).
- 223-223 `LotTool.proposalCursor` (field)
- 224-224 `LotTool.proposalStamp` (field)
- 226-233 `LotTool.proposalKey` (method): What the proposal was worked out from, as what its planner reads (`LotPlanner.update`): moves when the roads or the lots changed.
- 236-236 `LotTool.asking` (field): The plan asked for and its key, until it is answered.
- 239-255 `LotTool.advanceProposal` (method): The proposal brought up to the land as it is: when what it stands on changed, the plan asked of the host (planned in the simulation's worker, incrementally: DO NOT plan it on the page, the first plan of a region floods its land for long) an
- 258-264 `LotTool.takeWaiting` (method): The stroke let go before the proposal was in, made now with it.
- 267-281 `LotTool.proposedAt` (method): The proposed lot under a point (its index), or -1; only once worked out for the land as it is now.
- 284-288 `LotTool.readPending` (method): A stroke's points that waited on the proposal, read once it is ready.
- 291-310 `LotTool.commitStroke` (method): A stroke made: the proposed lots it passed laid, and zoned with the stored ones, in one undo step.
- 313-322 `LotTool.brushProposed` (method): The zone brush over the street land at `at`: the proposed lot under it, or the point kept until the proposal is ready
- 325-328 `LotTool.proposalReady` (method): Whether the proposal for the land as it is now is ready (probes).
- 333-333 `LotTool.snapReach` (method): The snap of lot points to the footways, the blocks' corners and the other lots' corners.
- 334-334 `LotTool.snap` (method)
- 336-336 `LotTool.lotAt` (method): DO NOT walk `doc.lots`: this runs for each of up to 400 steps of a pointer move.
- 338-338 `LotTool.lotsNear` (method): The lots whose box comes within `reach` of a point (a superset: test each).
- 341-352 `LotTool.sideNear` (method): The side of a lot nearest a point: its two corners.
- 355-367 `LotTool.sideAt` (method): The side of a lot nearest a point within reach: the lot and the side's index (corner i to i + 1).
- 370-373 `LotTool.snapExcept` (method): A dragged corner's snap: onto the streets and other corners, never onto itself.
- 376-385 `LotTool.frontSideOf` (method): Which side of a lot is its front: the longest of those against a street
- 388-391 `LotTool.landLot` (method): A lot as drawn, cut back to the footways (`onLand`), with its front; null when nothing is left on land.
- 394-400 `LotTool.streetAngleNear` (method): The direction of the street nearest a point, for a lot drawn there.
- 403-415 `LotTool.eraseUnder` (method): What the delete stroke takes at a point: the lot there, the building there.
- 420-426 `LotTool.adoptProposal` (method): The street land's proposed lots made real, to be edited as lots
- 429-499 `LotTool.down` (method): @perInput Runs once an input on one thing (a press, a release, a click): nothing it reaches may walk the map.
- 502-538 `LotTool.move` (method): @perInput The pointer moved; true when one of this tool's gestures took it.
- 541-616 `LotTool.up` (method): @perInput The pointer let go: the gesture it held is committed (`commit`), or dropped.
- 621-633 `LotTool.overlay` (method): The lots as the scene draws them (`render/lotOverlay.ts`), brought up to date: in the Zoning tool (`editing`), or the faint zones
- 636-636 `LotTool.book` (field): The overlay by group (`overlay`), and what its lots' groups were last made with.
- 637-637 `LotTool.drawnWith` (field)
- 638-638 `LotTool.lotsCursor` (field)
- 640-640 `LotTool.builtCursor` (field): The buildings' entries: a lot built on or cleared is drawn otherwise.
- 642-642 `LotTool.painted` (field): The lots drawn as painted by the stroke, and whether it was taking the zone off.
- 643-643 `LotTool.paintedRemove` (field)
- 645-645 `LotTool.drawnCorner` (field): The dragged corner the lots near it were drawn with.
- 647-647 `LotTool.lotsSeen` (field): Counts the times the lots' groups changed: the gesture reads the lots too (`refreshGesture`).
- 650-686 `LotTool.refreshLots` (method): The lots' groups made again where what they are drawn from changed: the diary, the pointer, the stroke, the dragged corner; the count of times any did.
- 687-687 `LotTool.drawnFrom` (field)
- 690-693 `LotTool.drawAllLots` (method): @rebuild Every lot's group made again: the first frame, another mode or zone colour, a map loaded, a change the diary cannot place.
- 696-735 `LotTool.drawLot` (method): One lot's group: its outline and fill, its front and arrow, its corners (none where nothing of it is drawn).
- 738-740 `LotTool.shownProposal` (field): The proposal drawn: its list, the zone colour, each lot's group name and look (bit 1 painted, 2 under the pointer), the pointer's lot, the stroke's.
- 741-741 `LotTool.nextProposalId` (field)
- 744-788 `LotTool.refreshProposal` (method): The street land proposed as lots, in every mode of the tool (they vanished from the Lots modes): faint outlines, one group each
- 791-855 `LotTool.refreshGesture` (method): The gesture's own group (lines being drawn, a cut, a curve, the snap) and the labels: made again when what it reads changed.
- 856-856 `LotTool.gestureKey` (field)
- 860-874 `segmentCrossesLot` (function): Whether the segment a-b passes through the inside of a lot.
