# render:buildings

Grupo: render. Entrada: desenho. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/buildings/buildingMesh.ts`, `src/render/buildings/cellArrays.ts`, `src/render/buildings/cellPool.ts`, `src/render/buildings/cellRequests.ts`, `src/render/buildings/cellWork.ts`, `src/render/buildings/chunk.ts`, `src/render/buildings/facadeKit.ts`, `src/render/buildings/facadeLevel.ts`, `src/render/buildings/facadePropModels.ts`, `src/render/buildings/facadeProps.ts`, `src/render/buildings/fade.ts`, `src/render/buildings/farParts.ts`, `src/render/buildings/finishes.ts`, `src/render/buildings/flagLayer.ts`, `src/render/buildings/fracture.ts`, `src/render/buildings/frameBudget.ts`, `src/render/buildings/furnitureKit.ts`, `src/render/buildings/glassRoom.ts`, `src/render/buildings/gpuBatchedMesh.ts`, `src/render/buildings/kit.ts`, `src/render/buildings/landmarks.ts`, `src/render/buildings/lawnGround.ts`, `src/render/buildings/layer.ts`, `src/render/buildings/layerApi.ts`, `src/render/buildings/layerCells.ts`, `src/render/buildings/levelKeys.ts`, `src/render/buildings/lightSlots.ts`, `src/render/buildings/louvreSlats.ts`, `src/render/buildings/massBase.ts`, `src/render/buildings/massCells.ts` … (+19)
- Unidades próprias (só esta funcionalidade alcança): 52 em 14 arquivo(s), 765 linhas
- Alcance total: 2476 unidades; usa 1014 compartilhadas de outras; é dona principal de 1410 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/buildingsWiring.ts#createBuildingWiring@81` (28)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): `recebe:airHandler`, `recebe:bin`, `recebe:bollard`, `recebe:chimney`, `recebe:condenser`, `recebe:dish`, `recebe:gate`, `recebe:hatch`, `recebe:lamp`, `recebe:liftRoom`, `recebe:mast`, `recebe:skylight`, `recebe:solar`, `recebe:tank`, `recebe:tankBox`, `recebe:vent`
- Workers: -
- Ganchos do three.js: `customDepthMaterial`, `onBeforeCompile`
- Timers: 5
- Usa: `world:buildings` (74), `editor:cloudTool` (3), `worker:conflictTable` (3), `worker:job` (2), `render:mesh` (1)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `RoadDoc.buildings`, `RoadDoc.changes`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: `earcut`, `meshoptimizer`, `polygon-clipping`, `three`, `three/addons/loaders/GLTFLoader.js`, `three/addons/utils/BufferGeometryUtils.js`, `three/examples/jsm/utils/BufferGeometryUtils.js`

## Testes do 4.0 que a exercitam
- `tests/census/worldCensus.spec.ts`, `tests/core/writeBudget.spec.ts`, `tests/render/buildingMesh.spec.ts`, `tests/render/buildingShadowSides.spec.ts`, `tests/render/buildingShellMemory.spec.ts`, `tests/render/cellArrays.spec.ts`, `tests/render/cellHoles.spec.ts`, `tests/render/cellJobMemory.spec.ts`, `tests/render/cellPaving.spec.ts`, `tests/render/cellPool.spec.ts`, `tests/render/cellPoolLazy.spec.ts`, `tests/render/cellPriority.spec.ts`, `tests/render/cellWork.spec.ts`, `tests/render/cheapAudit.spec.ts`, `tests/render/coplanar.spec.ts`, `tests/render/coverIndex.spec.ts`, `tests/render/detailCap.spec.ts`, `tests/render/editedCells.spec.ts`, `tests/render/facadeLevel.spec.ts`, `tests/render/facadeProps.spec.ts`, `tests/render/farParts.spec.ts`, `tests/render/finishFar.spec.ts`, `tests/render/flightFoot.spec.ts`, `tests/render/frameBudget.spec.ts`, `tests/render/furnitureKit.spec.ts`, `tests/render/glassDielectric.spec.ts`, `tests/render/gpuLedger.spec.ts`, `tests/render/gpuOnly.spec.ts`, `tests/render/groundDigest.spec.ts`, `tests/render/lawnGround.spec.ts`, `tests/render/layerIncremental.spec.ts`, `tests/render/lotSignatures.spec.ts`, `tests/render/macroNoise.spec.ts`, `tests/render/massBase.spec.ts`, `tests/render/massLevels.spec.ts`, `tests/render/massVertex.spec.ts`, `tests/render/massing.spec.ts`, `tests/render/occlusionHide.spec.ts`, `tests/render/parapets.spec.ts`, `tests/render/partCells.spec.ts`, `tests/render/partPose.spec.ts`, `tests/render/partyWalls.spec.ts`, `tests/render/previewWriter.spec.ts`, `tests/render/sharedPrograms.spec.ts`, `tests/render/shellBatch.spec.ts`, `tests/render/shellFinishes.spec.ts`, `tests/render/shellWriterReuse.spec.ts`, `tests/render/spaceSeed.spec.ts`, `tests/render/thinParts.spec.ts`, `tests/scale/buildingEdit.spec.ts`, `tests/scale/census/derivedBound.spec.ts`, `tests/scale/render/cityMemory.spec.ts`, `tests/scale/render/layerMemory.spec.ts`, `tests/world/floorTypes.spec.ts`, `tests/world/levelFlights.spec.ts`, `tests/world/splitLevel.spec.ts`, `tests/world/steppedYards.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `demolition`: não portado (pendência) 3, não se aplica 3
- porte `interior`: portado 14, outro porte 135, não se aplica 8
- porte `landscape`: já existia 12
- porte `street-furniture`: não se aplica 1
- porte `build`: não se aplica 2

## Código próprio (linhas — declaração — comentário)

### src/core/derivedCache.ts
- 107-126 `readDerivedAll` (function): Every entry whose key starts with `prefix`, by the rest of its key.

### src/render/buildings/buildingMesh.ts
- 661-663 `setShellTap` (function): The catalogue's tap on every face of a shell, or none.
- 798-802 `Shell.triangles` (method)
- 3721-3721 `ShellWriter.constructor` (method)

### src/render/buildings/cellPool.ts
- 51-51 `NEAR_TIER` (variable): The near products' band anchor (`nearTiles.ts` PRODUCT_TIER): after the buildings' masses (2-2.75). The roads' near level is no longer here: it goes in the cells' band (`farRoads.ts` farRoadPriority), the street under the eye stood in its f
- 120-124 `FloorColumns.constructor` (method)
- 167-169 `CellPool.coarseFirst` (method): DO NOT give the detail the queue first while the town is behind its curtain: the opening shows the masses and the far levels, the detail lands after (`renderer.ts` reveal), and the cells' detail took the queue's two workers while the coarse
- 171-182 `CellPool.coarseFirst` (method): DO NOT leave the detail asked behind the curtain in its band once the town shows: the queue ranks a job by the band it was asked in, and the cell at the camera after a reload waited behind the whole re-bake (conferencia 97, picture 51: neit
- 194-196 `CellPool.running` (method): Jobs asked and not answered.

### src/render/buildings/furnitureKit.ts
- 342-358 `furnitureParts` (function): What one furniture kind is built from, part by part, before its hidden faces are dropped (`scripts/model-sheet.mjs`): the primitive, its parameters, its colour, its edge rounding, its triangles and where it stands.

### src/render/buildings/gpuBatchedMesh.ts
- 47-57 `normalizer` (function): What a normalized value of 1 is stored as in `of` (three's MathUtils.normalize); a float array stores it as is.
- 59-67 `glTypeOf` (function)
- 82-82 `GpuBatchedMesh.flipped` (field): The instances `onBeforeRender` turned off for its listing, turned back on after it.
- 84-86 `GpuBatchedMesh.constructor` (method)
- 89-107 `GpuBatchedMesh.onBeforeRender` (method): three's listing of the multi-draw (frustum, visibility), with the instances hidden from this camera left out of it.
- 151-167 `GpuBatchedMesh._initializeGeometry` (method): three's arrays laid out on the first geometry; here their storage on the GPU.
- 169-237 `GpuBatchedMesh.setGeometryAt` (method)
- 374-376 `GpuBatchedMesh.raycast` (method): No hits: its vertices are on the GPU (the lab's inspector picks buildings by their records).
- 380-394 `GpuBatchedMesh.copy` (method)

### src/render/buildings/layerCells.ts
- 21-24 `groundDigest` (function): DO NOT work it out again here: every building the ground moved near turned its corners into the world again, 3 s of a 2

### src/render/buildings/massBase.ts
- 226-226 `baseBytes` (function): The bytes a base holds on the GPU: its vertices and indices.

### src/render/buildings/massing.ts
- 233-242 `emitMassingLevels` (function): A building's masses at two levels, `coarse` leaving out at least what `fine` does: DO NOT walk the building again for the coarser where the finer's walk was asked nothing between their `drop`s (and no pitched roof a level of boxes lays flat

### src/render/buildings/massLevels.ts
- 166-168 `MassLevels.asking` (method): Levels asked and not come.

### src/render/buildings/partCells.ts
- 115-115 `PartCell.runCount` (field)

### src/render/buildings/partFiling.ts
- 16-16 `IfStatement` (statement)

### src/render/buildings/parts.ts
- 25-25 `SIZE` (variable): Pictures of the things the galleries offer - a model, a window, a roof, a stair
- 27-27 `PER_FRAME` (variable): Pictures per frame: enough to fill a row, small enough to hold 60 fps.
- 29-29 `FRAME_SHARE_MS` (variable): Milliseconds of a frame the pictures may take, after the first of the frame.
- 31-31 `IDLE_MS` (variable): How long the second context is kept once the queue is empty.
- 34-38 `WALL_PARTS` (variable): Which BayComponent a gallery id stands for: most carry their own name.
- 40-42 `ROOF_PARTS` (variable)
- 44-44 `ELEMENT_PARTS` (variable)
- 47-53 `RUN_PARTS` (variable): The traced runs are their element, seen as the thing they lay.
- 55-55 `MODEL_KEYS` (variable)
- 57-61 `blank` (function)
- 64-120 `partSample` (function): The record a part is photographed from
- 122-131 `ThumbnailStudio` (interface)
- 133-133 `__BUILDING_KIT_HASH__` (variable)
- 135-135 `KIT_HASH` (variable): The fingerprint of the code the pictures are made by (`cook-plugin.ts`): pictures are kept under it.
- 137-322 `createThumbnailStudio` (function)

### src/render/buildings/referenceModel.ts
- 7-15 `ReferenceTarget` (interface): A reference model in the Builder: a real building's 3D model
- 17-21 `ReferenceInfo` (interface)
- 23-38 `ReferenceModel` (interface)
- 40-43 `ReferenceCapture` (interface)
- 46-46 `CAPTURE_DENSITY` (variable): Pixels per metre of the pictures a capture reads its colours from.
- 47-47 `CAPTURE_MAX` (variable)
- 50-54 `DEPTH_MATERIAL` (variable): Depth as two 8-bit channels (an orthographic camera's depth is linear).
- 56-56 `toSrgb` (function)
- 59-140 `captureModel` (function): Reads the triangles and the colours of a model (a copy of it in metres, three's axes).
- 142-246 `createReferenceModel` (function)

### src/render/buildings/townColumns.ts
- 85-98 `TownColumns.constructor` (method)
- 115-118 `TownColumns.known` (method): Whether an id's numbers are those of its record now (filed since the diary last named it).
