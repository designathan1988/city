# render:renderer

Grupo: render. Entrada: desenho. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/renderer.ts`
- Unidades próprias (só esta funcionalidade alcança): 43 em 4 arquivo(s), 4216 linhas
- Alcance total: 7354 unidades; usa 7309 compartilhadas de outras; é dona principal de 2 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: `ouve:childadded`, `ouve:childremoved`, `ouve:webglcontextrestored`
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#IfStatement@4036` (53), `src/actionsWiring.ts#createActions@68` (29), `src/effectsWiring.ts#createEffectsWiring@108` (16), `src/buildingsWiring.ts#createBuildingWiring@81` (12), `src/graphicsWiring.ts#createGraphicsWiring@118` (11), `src/main.ts#frame@2644` (9), `src/main.ts#drawOverlayScreen@2942` (8), `src/main.ts#wireProfiler@699` (6), `src/main.ts#roadTool@458` (6), `src/main.ts#terrainBrush@579` (4), `src/main.ts#pointerDown@1217` (3), `src/main.ts#scene@671` (2)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: `customDepthMaterial`, `onBeforeRender`
- Timers: 2
- Usa: `render:terrain` (172), `world:network` (121), `world:doc` (107), `render:buildings` (103), `render:iso` (100), `world:buildings` (77), `render:cover` (73), `render:road` (72), `world:changes` (69), `editor:barriers` (64), `world:elevation` (61), `render:quality` (59)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `RoadDoc.buildings`, `RoadDoc.changes`, `RoadDoc.clouds`, `RoadDoc.fogDabs`, `RoadDoc.fogSettings`, `RoadDoc.gullyAuto`, `RoadDoc.gullyDabs`, `RoadDoc.nodes`, `RoadDoc.segments`, `RoadDoc.terrainPaint`, `RoadDoc.terrainRelief`, `RoadDoc.terrainStamps`, `RoadDoc.treeClearings`, `RoadDoc.trees`, `RoadDoc.weather`, `RoadNode.id`, `RoadNode.x`, `RoadNode.y`, `RoadSegment.structure`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: `three`

## Testes do 4.0 que a exercitam
- `tests/core/cityBudget.spec.ts`, `tests/core/stepBudget.spec.ts`, `tests/core/writeBudget.spec.ts`, `tests/perf/budgets.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `demolition`: portado 2
- porte `landscape`: portado 108
- porte `people`: já existia 2
- porte `street-furniture`: portado 2
- porte `traffic`: já existia 2
- porte `transit`: portado 2
- porte `interface`: portado 1
- porte `build`: não se aplica 2
- porte `interior`: não se aplica 1

## Código próprio (linhas — declaração — comentário)

### src/core/derivedCache.ts
- 292-306 `forgetDerivedKinds` (function): Forgets every entry of these kinds (`kind:` + anything), without waiting: one readwrite transaction, a ranged `delete` a kind (the bakes of a product no longer made, left in an old player's store)
- 309-324 `forgetDerivedKeys` (function): Forgets these entries (each `prefix` + key), without waiting: one readwrite transaction, a `delete` per key

### src/core/engine.ts
- 4-21 `EngineInput` (class): An input of the engine that is not the diary (the view, the clock, the quality): a serial moved when it changes.
- 6-6 `EngineInput.epoch` (field): Moved by every input's `touch`: part of the engine's stamp (`Engine.watch`).
- 24-31 `Product` (interface): A product brought up to date by the engine.
- 34-45 `ProductWork` (interface): What one product did.
- 48-48 `Phase` (type): Where in the frame a product is brought up to date: `frame` by the frame loop between the simulation and the drawing
- 50-137 `Engine` (class)
- 51-51 `Engine.products` (field)
- 53-53 `Engine.stamp` (field): What says an input moved (the diary's serial, the inputs' serials); none: every computing product is taken as needed.
- 55-55 `Engine.stampAt` (field): Each phase's stamp at its last refresh.
- 57-57 `Engine.onRefresh` (field): Told each product's refresh: its time, keys and whether it computed for nothing (the profiler).
- 64-64 `Engine.phaseOf` (field)
- 65-65 `Engine.orders` (field)
- 67-67 `Engine.ledger` (field): Each product's work, by name: the ledger.
- 69-69 `Engine.lastBusy` (field): The names of the products that computed in each phase's last refresh.
- 72-80 `Engine.register` (method): Registers a product (once); it is refreshed after those it reads.
- 93-114 `Engine.refresh` (method): Every product of a phase brought up to date, in the order of what each reads; the names of those that computed.
- 122-136 `Engine.sorted` (method): The products in an order where each comes after those it reads (a depth-first walk); a cycle is an error.

### src/core/perfBudget.ts
- 40-40 `CityView` (type): THE CITY PLAN'S BUDGETS: what the whole-map city may cost, from the plan
- 44-48 `VIEW_BUDGETS` (variable): Per view: triangles and draw calls (colour + shadow) of one frame, GPU ms (p95), fps (a floor). DO NOT set the triangles under what the view draws at full facade level on a dedicated GPU (district 13 M, street 5.4 M, map 4.9 M measured 202

### src/render/renderer.ts
- 143-143 `ROOM_LIGHTS` (variable): Room lights kept in the scene for the floors cut open (`roomLamps.ts`).
- 144-144 `__ROAD_TILES_HASH__` (variable)
- 146-146 `ROAD_TILES_HASH` (variable): The fingerprint of the road tiles' code (`cook-plugin.ts`): their tiles are kept under it between sessions.
- 148-148 `KEPT_TILES_WAIT_MS` (variable): How long the opening's roads wait for the kept tiles to be read, ms: past it they are built.
- 150-150 `STREET_ZOOM` (variable): The view a zoom belongs to, for its budgets (`core/perfBudget.ts` `VIEW_BUDGETS`): the street from 8 px a metre, the district from 0.8, the map under.
- 151-151 `DISTRICT_ZOOM` (variable)
- 152-152 `viewOf` (function)
- 183-183 `SkyMode` (type): How the sky is kept: `cycle` follows the residents' clock.
- 199-217 `RenderStats` (interface)
- 219-222 `DrawOptions` (interface)
- 225-225 `OCCLUSION_RETRY_FRAMES` (variable): How many frames the buildings' occlusion rests after a frame it hid nothing in (`occlusionIdle`).
- 227-396 `SceneHandle` (interface)
- 399-399 `SURFACE_SETS` (variable): Roads at grade, and the decks above them, as `surfaceHeightAt` asks them.
- 402-402 `OPENING_SOLVE_MARGIN` (variable): How far past a scoped opening's view its roads are solved (`buildWorld`, `SolveScope`): a tile meeting the view (76.8 m), what a tile reads round it (`SURFACE_READ_REACH`) and a junction or two whose heights reach in. `tests/world/elevation
- 404-404 `FAR_ROAD_CELL` (variable): The far roads' cells made from the network at a scoped opening (`networkFarRoads.ts`): a road block's side (`roadSurfaces.ts` CHUNK tiles).
- 406-406 `GOVERN_FRAMES` (variable): Frames between two reads of the scene's geometry against the tier's budget (`governGeometry`): a walk of the scene's objects, ~1-3 ms.
- 408-408 `FILL_CELLS` (variable): The far level's cells the opening's fill takes at a step past those under the view (`fillStep`): a step's build has a fixed cost (every pass walked), a few cells share it.
- 410-410 `SOLVE_SHARE` (variable): The roads the opening's solve left, solved a share a frame once the town is shown (`completeSolve`): ~15-20 ms of a frame on level ground (the 16,708 roads of a city of the largest radius: 1.5 roads solved a road taken, ~0.04 ms each, Node,
- 412-412 `isLevelLand` (function): Level ground (a new map's `RELIEF_FLAT`, no stamp): every station's ground is the same, nothing is read under a road (`FLAT_GROUND`). A stroke or a height stamp makes it real ground again.
- 415-415 `isLeafCards` (function): A plant's leaf cards (`scenery.ts` `plantMeshes`, `gardens.ts`): `...-trees-<species>-cards`, `...-bushes-<kind>-cards`.
- 418-430 `firstDrawnVertices` (function): The vertices of the first mesh drawn under `root` (visible itself), 0 when none is: stops at the first, never counts them all.
- 432-4205 `createSceneRenderer` (function)
