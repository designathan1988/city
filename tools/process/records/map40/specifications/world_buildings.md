# world:buildings

Grupo: world. Entrada: subsistema. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/world/buildings/architecture.ts`, `src/world/buildings/blocks.ts`, `src/world/buildings/blueprints.ts`, `src/world/buildings/cameraSolids.ts`, `src/world/buildings/cityBuildings.ts`, `src/world/buildings/decayWatch.ts`, `src/world/buildings/docks.ts`, `src/world/buildings/elements.ts`, `src/world/buildings/facadePatterns.ts`, `src/world/buildings/flags.ts`, `src/world/buildings/floors.ts`, `src/world/buildings/footprints.ts`, `src/world/buildings/foundation.ts`, `src/world/buildings/geometry.ts`, `src/world/buildings/grid.ts`, `src/world/buildings/handles.ts`, `src/world/buildings/interior.ts`, `src/world/buildings/intern.ts`, `src/world/buildings/items.ts`, `src/world/buildings/lotLibrary.ts`, `src/world/buildings/lotTemplate.ts`, `src/world/buildings/lots.ts`, `src/world/buildings/materials.ts`, `src/world/buildings/neighbours.ts`, `src/world/buildings/pads.ts`, `src/world/buildings/pick.ts`, `src/world/buildings/procedural.ts`, `src/world/buildings/recordTable.ts`, `src/world/buildings/reference.ts`, `src/world/buildings/roofPlant.ts` … (+12)
- Unidades próprias (só esta funcionalidade alcança): 84 em 15 arquivo(s), 441 linhas
- Alcance total: 1225 unidades; usa 178 compartilhadas de outras; é dona principal de 963 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/buildingsWiring.ts#createBuildingWiring@81` (3), `src/main.ts#decayLook@4253` (2), `src/main.ts#cityData@2056` (2), `src/main.ts#openedOf@2079` (1), `src/main.ts#cityJob@2109` (1), `src/desktopJobs.ts#desktopJobWorkers@15` (1), `src/actionsWiring.ts#createActions@68` (1)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `render:buildings` (10), `world:doc` (6), `editor:poleRoutes` (3), `world:derived` (3), `sim:agents` (3), `world:changes` (2), `worker:conflictTable` (2), `editor:buildings` (2)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `RoadDoc.buildings`, `RoadDoc.changes`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: `polygon-clipping`

## Testes do 4.0 que a exercitam
- `tests/editor/binarySave.spec.ts`, `tests/editor/blockLift.spec.ts`, `tests/editor/blueprintLibrary.spec.ts`, `tests/editor/builderMove.spec.ts`, `tests/editor/buildingCorner.spec.ts`, `tests/editor/buildingCreator.spec.ts`, `tests/editor/buildingFloors.spec.ts`, `tests/editor/buildingPlans.spec.ts`, `tests/editor/buildingProfile.spec.ts`, `tests/editor/buildings.spec.ts`, `tests/editor/elementHover.spec.ts`, `tests/editor/facadeZone.spec.ts`, `tests/editor/fromReference.spec.ts`, `tests/editor/ghostCopy.spec.ts`, `tests/editor/growthWorker.spec.ts`, `tests/editor/history.spec.ts`, `tests/editor/lotsCutByRoad.spec.ts`, `tests/editor/pickAlongRay.spec.ts`, `tests/editor/preparedCity.spec.ts`, `tests/editor/shapeDrag.spec.ts`, `tests/editor/snapNear.spec.ts`, `tests/editor/splitLevelTool.spec.ts`, `tests/editor/stairRun.spec.ts`, `tests/editor/weld.spec.ts`, `tests/perf/budgets.spec.ts`, `tests/perf/trafficBuildingsBench.spec.ts`, `tests/render/buildingMesh.spec.ts`, `tests/render/buildingShellMemory.spec.ts`, `tests/render/canyonLight.spec.ts`, `tests/render/cellArrays.spec.ts`, `tests/render/cellJobMemory.spec.ts`, `tests/render/cellPaving.spec.ts`, `tests/render/cellWork.spec.ts`, `tests/render/coplanar.spec.ts`, `tests/render/coverIndex.spec.ts`, `tests/render/detailCap.spec.ts`, `tests/render/editedCells.spec.ts`, `tests/render/facadeLevel.spec.ts`, `tests/render/facadeProps.spec.ts`, `tests/render/finishFar.spec.ts`, `tests/render/flightFoot.spec.ts`, `tests/render/frameBudget.spec.ts`, `tests/render/furnitureKit.spec.ts`, `tests/render/gardenLists.spec.ts`, `tests/render/glassDielectric.spec.ts`, `tests/render/gpuLedger.spec.ts`, `tests/render/gpuOnly.spec.ts`, `tests/render/groundDigest.spec.ts`, `tests/render/groundShapeRegions.spec.ts`, `tests/render/layerIncremental.spec.ts`, `tests/render/lotSignatures.spec.ts`, `tests/render/massBase.spec.ts`, `tests/render/massLevels.spec.ts`, `tests/render/massVertex.spec.ts`, `tests/render/occlusion.spec.ts`, `tests/render/partCells.spec.ts`, `tests/render/partPose.spec.ts`, `tests/render/partyWalls.spec.ts`, `tests/render/previewWriter.spec.ts`, `tests/render/roadTileWorkers.spec.ts`, `tests/render/shellFinishes.spec.ts`, `tests/render/shellWriterReuse.spec.ts`, `tests/render/terrainShaping.spec.ts`, `tests/scale/buildingEdit.spec.ts`, `tests/scale/cityGeneration.spec.ts`, `tests/scale/render/cityMemory.spec.ts`, `tests/scale/render/layerMemory.spec.ts`, `tests/sim/agents/baysIncremental.spec.ts`, `tests/sim/agents/baysWorkers.spec.ts`, `tests/sim/agents/citySpin.spec.ts`, `tests/sim/agents/lotDistance.spec.ts`, `tests/sim/agents/lotDoors.spec.ts`, `tests/sim/agents/lotGate.spec.ts`, `tests/sim/agents/parkedInLot.spec.ts`, `tests/sim/ambientIndex.spec.ts`, `tests/sim/indexes/baysByDiary.spec.ts`, `tests/sim/indexes/lorries.spec.ts`, `tests/sim/indexes/lotGridSame.spec.ts`, `tests/sim/lorryTraffic.spec.ts`, `tests/sim/lotPlaces.spec.ts`, `tests/sim/trafficBuildings.spec.ts`, `tests/ui/builderCatalog.spec.ts`, `tests/world/architecture.spec.ts`, `tests/world/blocksFrozen.spec.ts`, `tests/world/buildingBounds.spec.ts`, `tests/world/buildingFloors.spec.ts`, `tests/world/buildingForkGrid.spec.ts`, `tests/world/buildingNearAny.spec.ts`, `tests/world/buildingPlans.spec.ts`, `tests/world/buildingStoreOwnership.spec.ts`, `tests/world/buildingWear.spec.ts`, `tests/world/buildings.spec.ts`, `tests/world/cameraSolids.spec.ts`, `tests/world/changeGuards.spec.ts`, `tests/world/cityBuildings.spec.ts`, `tests/world/decayWatch.spec.ts`, `tests/world/defaultTown.spec.ts`, `tests/world/diaryPlaces.spec.ts`, `tests/world/docBinary.spec.ts`, `tests/world/docFork.spec.ts`, `tests/world/docMemory.spec.ts`, `tests/world/docTiles.spec.ts`, `tests/world/edgeFrame.spec.ts`, `tests/world/entranceMerge.spec.ts`, `tests/world/facadeOverlap.spec.ts`, `tests/world/floorTypes.spec.ts`, `tests/world/footprintsOverlap.spec.ts`, `tests/world/foundationDeck.spec.ts`, `tests/world/furnishing.spec.ts`, `tests/world/garageUnder.spec.ts`, `tests/world/intern.spec.ts`, `tests/world/jsonCopy.spec.ts`, `tests/world/levelFlights.spec.ts`, `tests/world/lorryDocks.spec.ts`, `tests/world/lotClash.spec.ts`, `tests/world/lotLibrary.spec.ts`, `tests/world/lotPhysics.spec.ts`, `tests/world/migrateOnce.spec.ts`, `tests/world/openLots.spec.ts`, `tests/world/openingOffPage.spec.ts`, `tests/world/padRingDistance.spec.ts`, `tests/world/pads.spec.ts`, `tests/world/pageWorldMemory.spec.ts`, `tests/world/recordTable.spec.ts`, `tests/world/recordTableReader.spec.ts`, `tests/world/reliefPushes.spec.ts`, `tests/world/rescale.spec.ts`, `tests/world/roofPlant.spec.ts`, `tests/world/shopFloor.spec.ts`, `tests/world/siteGrading.spec.ts`, `tests/world/spilledItems.spec.ts`, `tests/world/splitLevel.spec.ts`, `tests/world/steppedYards.spec.ts`, `tests/world/storeIntern.spec.ts`, `tests/world/streetDoors.spec.ts`, `tests/world/terrace.spec.ts`, `tests/world/tileEntries.spec.ts`, `tests/world/tileGates.spec.ts`, `tests/world/unzone.spec.ts`, `tests/world/variety.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `interior`: outro porte 37, já existia 3
- porte `interface`: portado 14, outra sessão 6
- porte `landscape`: já existia 12
- porte `traffic`: já existia 3, de outra funcionalidade 1
- porte `landmarks`: portado 1

## Código próprio (linhas — declaração — comentário)

### src/world/buildings/cameraSolids.ts
- 32-32 `SolidPages.epoch` (field)
- 47-50 `SolidPages.walk` (method): A new walk: no slot visited in it yet.
- 53-58 `SolidPages.visitOnce` (method): True the first time `slot` is met in this walk.
- 99-99 `smoothstep` (function)
- 205-207 `SolidFiling.constructor` (method)
- 273-276 `SolidFiling.forEachIn` (method)
- 278-281 `SolidFiling.forEachInAny` (method)
- 284-296 `SolidFiling.visitCells` (method): The slots filed over a rectangle not yet met in this walk.
- 298-313 `SolidFiling.forEachOccluder` (method)
- 315-317 `SolidFiling.floorAt` (method)
- 319-322 `SolidFiling.roofAt` (method)
- 326-344 `lowestEye` (function): The lowest the eye may stand at (x, y) over `list`, its floor easing down over `reach` round each volume.

### src/world/buildings/decayWatch.ts
- 8-58 `createDecayWatch` (function): DO NOT read `change.ids ?? []`: after a load the new town never ran down and the old one stayed due.

### src/world/buildings/flags.ts
- 17-20 `FLAG_COLOURS` (variable): The colours a flag's fields are picked from in the Builder.

### src/world/buildings/geometry.ts
- 53-57 `frameToLocal` (function): A world point in the building's frame, as `worldToLocal` gives it.
- 272-275 `inheritStoredBounds` (function): `to`'s boxes are `from`'s: the same building in every respect its box reads (its wear changed alone, `BuildingStore.wear`).

### src/world/buildings/interior.ts
- 298-298 `Plan.d` (method)

### src/world/buildings/intern.ts
- 25-29 `internedEntries` (function): Entries of the table now, and hashes: what the tests read.

### src/world/buildings/items.ts
- 18-20 `BuildingItems.GetAccessor` (method)
- 69-71 `BuildingItems.size` (method)
- 80-82 `BuildingItems.has` (method)
- 95-102 `BuildingItems.delete` (method)
- 105-111 `BuildingItems.clear` (method): Every record let go, the table with them.
- 114-117 `BuildingItems.keys` (method): DO NOT decode a record to list ids: the table's id column, then the ids written after it.
- 119-129 `BuildingItems.entries` (method)
- 131-133 `BuildingItems.values` (method)
- 135-137 `BuildingItems.MethodDeclaration` (method)
- 139-141 `BuildingItems.forEach` (method)

### src/world/buildings/neighbours.ts
- 72-72 `RecordNeighbour.top` (field)
- 73-73 `RecordNeighbour.constructor` (method)
- 74-74 `RecordNeighbour.id` (method)
- 75-75 `RecordNeighbour.x` (method)
- 76-76 `RecordNeighbour.y` (method)
- 77-77 `RecordNeighbour.bounds` (method)
- 78-78 `RecordNeighbour.reach` (method)
- 79-79 `RecordNeighbour.obstacles` (method)
- 80-80 `RecordNeighbour.footprints` (method)
- 81-81 `RecordNeighbour.projections` (method)
- 82-82 `RecordNeighbour.signature` (method)
- 83-83 `RecordNeighbour.storeys` (method)

### src/world/buildings/pads.ts
- 333-333 `Grading.ringAt` (field)

### src/world/buildings/recordTable.ts
- 23-27 `tableIn` (function): A table's bytes in a buffer of their own (shared where the thread is isolated, `allocate`): one read back from a store, which keeps plain buffers only (IndexedDB refuses shared memory, HTML "StructuredSerialize").
- 30-35 `tableBytes` (function): A table's bytes as a store keeps them: a plain buffer, copied when the table is in shared memory.
- 136-136 `TableWriter.pos` (field): Where `read` stands in the stream.
- 148-150 `TableWriter.idAt` (method): The id of a row written.
- 225-229 `TableWriter.read` (method): A row written, decoded: a fresh object of its own, its shared members its own too.
- 231-265 `TableWriter.value` (method)
- 295-299 `isRecordTable` (function): Whether a buffer holds a record table of this format: written once when encoded (`encodeRecords`), never again - a job process's thread keeps the copy it is sent (`jobs/processWorker.ts` KeptBuffer).
- 309-309 `RecordTable.rowOf` (field): The row of each id, made when first asked.
- 324-327 `RecordTable.row` (method): The row holding a building's id; -1 when it is not in the table.

### src/world/buildings/sections.ts
- 53-71 `eachSectionRecord` (function): Every record of the sections in the store's order, one at a time (a worker's copy: `sim/agents/trafficBuildings.ts`): DO NOT hold them all - what is worked out of each goes with it.

### src/world/buildings/spilledItems.ts
- 6-6 `RECENT` (variable): The records decoded from the table kept as objects, the newest read last: a grown building's neighbours are read again and again while its block grows (`growth/zoning.ts` neighbourStoreys, nearSignatures), each the same object while it is h
- 24-24 `SpilledItems.over` (field): Spilled rows given another record since (in place, as a `Map` keeps a key set again).
- 28-28 `SpilledItems.recent` (field): The rows decoded last, held (`RECENT`).
- 32-34 `SpilledItems.GetAccessor` (method)
- 42-45 `SpilledItems.constructor` (method): `shared`: what the records spilled have alike (`SharedMembers`), the batch ended by whoever passes it (`endBatch`) - the page's table shares it (`editor/preparedCity.ts`); none: its own, each spill a batch.
- 79-81 `SpilledItems.spilled` (method)
- 83-93 `SpilledItems.decoded` (method)
- 95-97 `SpilledItems.size` (method)
- 99-104 `SpilledItems.get` (method)
- 106-108 `SpilledItems.has` (method)
- 110-118 `SpilledItems.set` (method)
- 120-128 `SpilledItems.delete` (method)
- 130-136 `SpilledItems.clear` (method)
- 138-144 `SpilledItems.keys` (method)
- 146-148 `SpilledItems.entries` (method)
- 150-152 `SpilledItems.values` (method)
- 154-156 `SpilledItems.MethodDeclaration` (method)
- 158-160 `SpilledItems.forEach` (method)

### src/world/buildings/store.ts
- 110-113 `BuildingStore.table` (method): The record table the map was opened from (null: none, every record written here): its columns are read without decoding a record.
- 179-188 `BuildingStore.wear` (method): @perItem DO NOT write a wear change with `put`
- 256-262 `BuildingStore.clear` (method)

### src/world/buildings/tableRows.ts
- 27-38 `TableRows.constructor` (method)
- 40-42 `TableRows.GetAccessor` (method)
- 48-50 `TableRows.size` (method)
- 65-67 `TableRows.idAt` (method): The id of a row.
- 81-85 `TableRows.held` (method): How many rows' records are decoded and held somewhere now (the LRU of `get` among them): what the table saves is what is not.
- 102-105 `TableRows.keys` (method)
- 107-109 `TableRows.values` (method)
- 111-113 `TableRows.entries` (method)
- 115-117 `TableRows.MethodDeclaration` (method)
- 119-121 `TableRows.forEach` (method)

### src/world/buildings/types.ts
- 454-454 `DECAY_GRACE` (variable): City minutes a building stays as new before it starts to age: five days.
- 466-469 `decayOf` (function): How run-down a building built at `builtAt` is at `now`, in tenths.
