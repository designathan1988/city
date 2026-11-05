# editor:growthFeed

Grupo: editor. Entrada: ferramenta. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/editor/growthFeed.ts`
- Unidades próprias (só esta funcionalidade alcança): 41 em 1 arquivo(s), 543 linhas
- Alcance total: 664 unidades; usa 609 compartilhadas de outras; é dona principal de 14 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#growthAnswered@1998` (7), `src/main.ts#growZones@2597` (2), `src/main.ts#IfStatement@4036` (2), `src/main.ts#growth@1994` (1), `src/main.ts#growthPool@1993` (1)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `world:lot` (26), `world:doc` (19), `world:network` (19), `editor:growthReplica` (19), `world:buildings` (17), `worker:conflictTable` (14), `world:growth` (12), `world:changes` (10), `world:open` (9), `world:paving` (5), `world:derived` (4), `world:ids` (4)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `RoadDoc.buildings`, `RoadDoc.changes`, `RoadDoc.lotStore`, `RoadSegment.structure`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/editor/growthWorker.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `landscape`: não se aplica 4

## Código próprio (linhas — declaração — comentário)

### src/editor/growthFeed.ts
- 20-20 `grow` (function): A rectangle grown by `pad` on every side.
- 25-33 `GrowthHost` (interface): Where the growth reads the document: the page's document, network and ground.
- 42-44 `GrowthOutcome` (type): What an answer came to: the buildings written (their ids, in order), the lots set aside, and whether the rest was let go to be asked for again (the document moved under it, `ask`); or nothing (`stale`: not the ask waited for).
- 47-47 `REGION_HALF` (variable): Half the side of the region round the lot asked for whose open lots the growth goes on to in the worker (`GrowAsk.region`).
- 49-49 `READ_PAD` (variable): How far past the region its lots read the document: their frame, its margin (`growthReachOf`), a lot's size.
- 52-52 `MOVES` (variable): The changes a building made off the page may have missed: its neighbours, its roads, its ground, its lot.
- 54-54 `MISS_MARGIN` (variable): More read round a lot each time the worker asked outside what it was sent (`GrowthMiss`), world units.
- 56-56 `MOST_MISSES` (variable): Misses before a lot is set aside: a lot that reads farther than this is not one the growth can build.
- 58-58 `MOST_REFUSALS` (variable): Times a lot's building may be refused on the page, the document having moved, before the lot is set aside.
- 60-70 `Asked` (interface)
- 72-333 `GrowthSession` (class)
- 73-73 `GrowthSession.nextAsk` (field)
- 75-75 `GrowthSession.seed` (field): The seed of the lots' streams, and the most lots an ask grows, as last asked with.
- 76-76 `GrowthSession.most` (field)
- 77-77 `GrowthSession.asked` (field)
- 79-79 `GrowthSession.sentBuildings` (field): The buildings the worker holds; those of them the diary named since they were sent; whether it is to forget them all.
- 80-80 `GrowthSession.staleBuildings` (field)
- 81-81 `GrowthSession.resetBuildings` (field)
- 82-82 `GrowthSession.buildingsRead` (field)
- 84-84 `GrowthSession.sentRoads` (field): The roads and junctions the worker holds, those of them the network changed since, and its state they were last read at.
- 85-85 `GrowthSession.sentKerbs` (field)
- 86-86 `GrowthSession.staleRoads` (field)
- 87-87 `GrowthSession.staleKerbs` (field)
- 88-88 `GrowthSession.resetRoads` (field)
- 89-89 `GrowthSession.roadsOf` (field)
- 90-90 `GrowthSession.roadsState` (field)
- 92-92 `GrowthSession.moves` (field): What changed while a building was being made (`MOVES`): read when it is asked for and when it comes.
- 94-94 `GrowthSession.refusals` (field): Lots whose building was refused on the page, how many times.
- 96-96 `GrowthSession.constructor` (method)
- 99-101 `GrowthSession.busy` (method): Whether a building is being made.
- 104-115 `GrowthSession.ask` (method): Asks the worker for the next building (`pickLot`); false when no lot is open (`refused` set aside) - or true, already asking, while one is made.
- 118-126 `GrowthSession.restart` (method): The worker started again with nothing (it failed, `growthPool.ts`): all is sent again as it is asked for, and the building being made asked for again.
- 129-191 `GrowthSession.receive` (method): The worker's answer, its buildings written in order (`onGrown` told of each as it is, before the next) while each still stands where it was made - checked again first when the document moved meanwhile; the rest let go at the first that does
- 194-197 `GrowthSession.lotOpen` (method): Whether `lot` is still the document's, zoned, with no building standing and not set aside.
- 200-216 `GrowthSession.readChanges` (method): What changed of what the worker holds, from the buildings' diary and the network's changes: those it holds marked to be sent again, everything when they cannot say (another map, entries forgotten, a load).
- 218-224 `GrowthSession.forgetRoads` (method)
- 227-274 `GrowthSession.send` (method): The ask for `lot` (picked here, `pickLot`, its stream `rng`) and the lots the growth may go on to in the worker: the open lots round it, with what they all read - the buildings, roads and ground round them, the paving there - sent once and 
- 277-297 `GrowthSession.syncBuildings` (method): The buildings the worker holds that changed (`readChanges`), and those round this lot it does not hold yet: each as its row in a shared table (`refOf`).
- 300-332 `GrowthSession.syncRoads` (method): The roads and junctions the worker holds that changed (`readChanges`), and those filed round this lot it does not hold yet.
- 336-341 `roadPiece` (function): A road as the worker keeps it (`RoadPiece`), or null: no ribbon filed for it.
- 344-353 `junctionPiece` (function): A junction's kerb ring as the worker keeps it (`JunctionPiece`), or null: none filed.
