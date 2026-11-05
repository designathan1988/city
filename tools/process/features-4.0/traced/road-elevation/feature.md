# Feature: src/world/elevation.ts

The program starts from `src/main.ts`, `src/peopleLab/main.ts`. A declaration is the feature's own when the program reaches it only through the feature; in a declaration of the program that names the feature, its statements are a slice (what names it, what depends on that, what that controls).

- Own: 145 declarations
- Slices of shared code: 62
- Code: 3104 lines in 25 files (source/)
- Resources: 2 (2 entry, 0 style, 0 import; 0 only the feature uses)
- Uses: 100 shared declarations (interface/: 133 declarations in 31 files, the types they name included); hooks: 26; external APIs: 72

## Code

- src/core/aabb.ts — part, 6 lines
- src/core/rerootMap.ts — whole, 105 lines
- src/editor/commit.ts — part, 95 lines
- src/editor/editRules.ts — part, 21 lines
- src/editor/roadTool.ts — part, 4 lines
- src/editor/smallCityMap.ts — part, 8 lines
- src/lab/catalog/layers.ts — part, 3 lines
- src/lab/catalog/layout.ts — part, 11 lines
- src/main.ts — part, 97 lines
- src/render/grass.ts — part, 12 lines
- src/render/groundShape.ts — part, 68 lines
- src/render/renderer.ts — part, 518 lines
- src/render/roadStations.ts — part, 7 lines
- src/render/roadStations.worker.ts — part, 8 lines
- src/render/roadSurfaces.ts — part, 213 lines
- src/render/signLayer.ts — part, 6 lines
- src/render/streetFurnitureLayer.ts — part, 30 lines
- src/render/structureCells.ts — part, 4 lines
- src/render/structures.ts — part, 239 lines
- src/render/utilities.ts — part, 10 lines
- src/world/elevation.ts — whole, 1541 lines
- src/world/junction/levels.ts — part, 3 lines
- src/world/roadField.ts — part, 84 lines
- src/world/roads/buildMode.ts — part, 10 lines
- src/world/structures.ts — part, 1 lines

## Slices of shared code

- `src/editor/commit.ts#DraftResult` — 3 of 14 lines
- `src/editor/commit.ts#boreDeepCuts` — 14 of 38 lines
- `src/editor/commit.ts#commitRoadPath` — 44 of 97 lines
- `src/editor/commit.ts#draftStations` — 8 of 24 lines
- `src/editor/commit.ts#lastDryRun` — 6 of 6 lines
- `src/editor/commit.ts#layCopy` — 5 of 11 lines
- `src/editor/commit.ts#onTerrain` — 1 of 1 lines
- `src/editor/commit.ts#solveOver` — 8 of 9 lines
- `src/editor/commit.ts#terrainIndexOf` — 6 of 6 lines
- `src/editor/editRules.ts#flatSolves` — 4 of 7 lines
- `src/editor/editRules.ts#flatSolves.after` — 1 of 1 lines
- `src/editor/editRules.ts#flatSolves.beforeSolve` — 1 of 1 lines
- `src/editor/editRules.ts#grade` — 6 of 23 lines
- `src/editor/editRules.ts#tooSteep` — 9 of 19 lines
- `src/editor/roadTool.ts#RoadToolHost` — 4 of 36 lines
- `src/editor/smallCityMap.ts#growLots` — 7 of 17 lines
- `src/editor/smallCityMap.ts#growLots.pavedAt` — 1 of 1 lines
- `src/lab/catalog/layers.ts#signItems.one` — 3 of 9 lines
- `src/lab/catalog/layout.ts#surfacesOf` — 11 of 26 lines
- `src/main.ts#drawOverlayScreen` — 81 of 469 lines
- `src/main.ts#setNodeHeightMetres` — 16 of 26 lines
- `src/render/grass.ts#grassClump` — 12 of 57 lines
- `src/render/groundShape.ts#BlockDigestAsk` — 3 of 7 lines
- `src/render/groundShape.ts#BlockRoads` — 1 of 1 lines
- `src/render/groundShape.ts#GroundShapeAsk` — 3 of 5 lines
- `src/render/groundShape.ts#blockDigest` — 9 of 15 lines
- `src/render/groundShape.ts#digestBlocks` — 11 of 24 lines
- `src/render/groundShape.ts#shapeGroundAsk` — 41 of 45 lines
- `src/render/renderer.ts#SURFACE_SETS` — 1 of 1 lines
- `src/render/renderer.ts#SceneHandle` — 4 of 170 lines
- `src/render/renderer.ts#createSceneRenderer` — 342 of 3774 lines
- `src/render/renderer.ts#createSceneRenderer.blockRead` — 9 of 9 lines
- `src/render/renderer.ts#createSceneRenderer.buildWorld` — 12 of 78 lines
- `src/render/renderer.ts#createSceneRenderer.compareSolves` — 74 of 74 lines
- `src/render/renderer.ts#createSceneRenderer.completeSolve` — 12 of 24 lines
- `src/render/renderer.ts#createSceneRenderer.landGroundDigest` — 5 of 6 lines
- `src/render/renderer.ts#createSceneRenderer.levelGroundDigest` — 6 of 8 lines
- `src/render/renderer.ts#createSceneRenderer.readOf` — 5 of 5 lines
- `src/render/renderer.ts#createSceneRenderer.requestNetworkFar` — 4 of 4 lines
- `src/render/renderer.ts#createSceneRenderer.shapeGround` — 9 of 47 lines
- `src/render/renderer.ts#createSceneRenderer.stationsAsked` — 2 of 2 lines
- `src/render/renderer.ts#createSceneRenderer.takeEditedCells` — 18 of 26 lines
- `src/render/renderer.ts#createSceneRenderer.warmRoadShaders` — 15 of 39 lines
- `src/render/roadStations.ts#Hint` — 2 of 4 lines
- `src/render/roadStations.ts#RoadStations.current` — 2 of 3 lines
- `src/render/roadStations.ts#RoadStations.waiting` — 3 of 30 lines
- `src/render/roadStations.worker.ts#serve` — 8 of 13 lines
- `src/render/roadSurfaces.ts#buildRoadSurfaces` — 9 of 14 lines
- `src/render/roadSurfaces.ts#roadSurfaceSteps` — 204 of 512 lines
- `src/render/signLayer.ts#SignLayerKept` — 3 of 14 lines
- `src/render/signLayer.ts#SignSource` — 3 of 5 lines
- `src/render/streetFurnitureLayer.ts#FurnitureSource` — 3 of 6 lines
- `src/render/streetFurnitureLayer.ts#StreetFurnitureLayer` — 3 of 17 lines
- `src/render/streetFurnitureLayer.ts#furniturePlace` — 19 of 65 lines
- `src/render/streetFurnitureLayer.ts#meadowInto` — 5 of 9 lines
- `src/render/structureCells.ts#StructureCells` — 4 of 8 lines
- `src/render/structures.ts#buildStructureDetails` — 5 of 14 lines
- `src/render/structures.ts#retainingWalls` — 44 of 58 lines
- `src/render/structures.ts#structureParts` — 187 of 205 lines
- `src/render/structures.ts#structureRibbons` — 3 of 35 lines
- `src/render/utilities.ts#poleGroundAt` — 10 of 14 lines
- `src/world/roads/buildMode.ts#stationsAlong` — 10 of 26 lines

## Resources

- shared entry `hint.road.gradeLimited` — src/ui/i18n/en.ts:720: `'hint.road.gradeLimited': 'The road reached its safe grade; continue drawing to reach the selected height.'`
- shared entry `hint.road.gradeLimited` — src/ui/i18n/pt-BR.ts:713: `'hint.road.gradeLimited': 'A via atingiu a inclinação segura; continue desenhando para chegar à altura escolhida.'`

## Where the program plugs the feature in

### src/world/elevation.ts

- `src/world/elevation.ts#buildRoadElevation` (function, line 279) ← call from `src/editor/commit.ts#solveOver`, `src/editor/editRules.ts#flatSolves.after`, `src/editor/editRules.ts#flatSolves.beforeSolve`, `src/editor/smallCityMap.ts#growLots`, …
- `src/world/elevation.ts#buildRoadElevation` (function, line 279) ← implementation from `src/editor/commit.ts#boreDeepCuts`, `src/editor/editRules.ts#grade`, `src/editor/editRules.ts#refuseRoadEdit.deck`, `src/editor/smallCityMap.ts#growLots.pavedAt`, …
- `src/world/elevation.ts#buildRoadElevation` (function, line 279) ← indirectCall from `src/world/roadField.ts#roadField.blend`, `src/world/roadField.ts#roadField.onSegment`
- `src/world/elevation.ts#CUT_WALL_REACH` (variable, line 222) ← read from `src/render/structures.ts#retainingWalls`
- `src/world/elevation.ts#FLAT_GROUND` (function, line 951) ← read from `src/editor/editRules.ts#flatSolves.after`, `src/editor/editRules.ts#flatSolves.beforeSolve`, `src/editor/smallCityMap.ts#growLots`, `src/render/renderer.ts#createSceneRenderer.buildWorld`, …
- `src/world/elevation.ts#GROUND_ONLY` (variable, line 208) ← read from `src/editor/smallCityMap.ts#growLots.pavedAt`, `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#SURFACE_SETS`, `src/render/utilities.ts#poleGroundAt`
- `src/world/elevation.ts#MAX_AUTHORED_GRADE` (variable, line 68) ← read from `src/editor/commit.ts#commitRoadPath`, `src/main.ts#drawOverlayScreen`, `src/main.ts#setNodeHeightMetres`
- `src/world/elevation.ts#pendingOrder` (function, line 196) ← call from `src/render/renderer.ts#createSceneRenderer.completeSolve`
- `src/world/elevation.ts#RoadElevation` (interface, line 135) ← call from `src/editor/smallCityMap.ts#growLots.pavedAt`, `src/render/groundShape.ts#blockDigest`, `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.compareSolves`, …
- `src/world/elevation.ts#RoadElevation` (interface, line 135) ← read from `src/render/groundShape.ts#blockDigest`, `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.buildWorld`, `src/render/renderer.ts#createSceneRenderer.compareSolves`, …
- `src/world/elevation.ts#RoadElevation` (interface, line 135) ← reference from `src/render/groundShape.ts#shapeGroundAsk`, `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.compareSolves`, `src/render/roadSurfaces.ts#roadSurfaceSteps`
- `src/world/elevation.ts#RoadElevation` (interface, line 135) ← typeRef from `src/editor/commit.ts#boreDeepCuts`, `src/editor/commit.ts#commitRoadPath`, `src/editor/commit.ts#DraftResult`, `src/editor/commit.ts#draftStations`, …
- `src/world/elevation.ts#RoadShapeCut` (interface, line 840) ← typeRef from `src/render/groundShape.ts#BlockDigestAsk`, `src/render/groundShape.ts#GroundShapeAsk`
- `src/world/elevation.ts#roadShaper` (function, line 888) ← call from `src/render/groundShape.ts#digestBlocks`, `src/render/groundShape.ts#shapeGroundAsk`
- `src/world/elevation.ts#roadShaper` (function, line 888) ← implementation from `src/render/grass.ts#grassClump`, `src/render/groundShape.ts#blockDigest`, `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.deckHeight`, …
- `src/world/elevation.ts#roadShaper` (function, line 888) ← indirectCall from `src/world/roadField.ts#roadField.blend`, `src/world/roadField.ts#roadField.onSegment`
- `src/world/elevation.ts#roadShaper.shapeAt` (method, line 894) ← implementation from `src/render/groundShape.ts#blockDigest`, `src/render/groundShape.ts#shapeGroundAsk`
- `src/world/elevation.ts#roadStations` (function, line 240) ← call from `src/render/roadStations.worker.ts#serve`
- `src/world/elevation.ts#roadsToRead` (function, line 248) ← call from `src/render/renderer.ts#createSceneRenderer.buildWorld`, `src/render/renderer.ts#createSceneRenderer.completeSolve`
- `src/world/elevation.ts#roadsToRead` (function, line 248) ← typeRef from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.stationsAsked`
- `src/world/elevation.ts#SolveScope` (interface, line 169) ← read from `src/render/renderer.ts#createSceneRenderer.buildWorld`
- `src/world/elevation.ts#SolveScope` (interface, line 169) ← typeRef from `src/render/renderer.ts#createSceneRenderer.buildWorld`, `src/render/renderer.ts#createSceneRenderer.completeSolve`
- `src/world/elevation.ts#StationHint` (interface, line 232) ← inherit from `src/render/roadStations.ts#Hint`
- `src/world/elevation.ts#StationHint` (interface, line 232) ← read from `src/render/roadStations.ts#RoadStations.waiting`
- `src/world/elevation.ts#StationHint` (interface, line 232) ← typeRef from `src/render/roadStations.ts#RoadStations.current`
- `src/world/elevation.ts#SURFACE_READ_REACH` (variable, line 88) ← read from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.takeEditedCells`, `src/render/roadSurfaces.ts#roadSurfaceSteps`

## Shared declarations the feature uses

### src/core/aabb.ts

- `src/core/aabb.ts#Aabb` (interface, line 3) — read by 5: `src/world/elevation.ts#buildRoadElevation.take`, `src/world/elevation.ts#ProfileIndex.cells`, `src/world/roadField.ts#SpatialIndex.constructor`, `src/core/aabb.ts#expand`, …
- `src/core/aabb.ts#Aabb` (interface, line 3) — typeRef by 6: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#Profile`, `src/world/elevation.ts#ProfileIndex.cells`, `src/world/elevation.ts#RoadElevation`, …

### src/core/digest.ts

- `src/core/digest.ts#Digest` (class, line 6) — construct by 5: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#buildRoadElevation.keyOf`, `src/world/elevation.ts#buildRoadElevation.localDigest`, `src/world/elevation.ts#lookDigest`, …
- `src/core/digest.ts#Digest.add` (method, line 11) — call by 4: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#buildRoadElevation.localDigest`, `src/world/elevation.ts#lookDigest`, `src/world/elevation.ts#ProfileIndex.digest`
- `src/core/digest.ts#Digest.addAll` (method, line 19) — call by 2: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#buildRoadElevation.keyOf`
- `src/core/digest.ts#Digest.addPlain` (method, line 31) — call by 2: `src/world/elevation.ts#buildRoadElevation.keyOf`, `src/world/elevation.ts#lookDigest`
- `src/core/digest.ts#Digest.addText` (method, line 25) — call by 2: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#buildRoadElevation.localDigest`
- `src/core/digest.ts#Digest.value` (method, line 59) — call by 5: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#buildRoadElevation.keyOf`, `src/world/elevation.ts#buildRoadElevation.localDigest`, `src/world/elevation.ts#lookDigest`, …

### src/core/hitch.ts

- `src/core/hitch.ts#measureHitch` (function, line 2) — call by 1: `src/world/elevation.ts#buildRoadElevation`

### src/core/polyline.ts

- `src/core/polyline.ts#Frame` (interface, line 7) — read by 2: `src/world/elevation.ts#readStations`, `src/world/elevation.ts#stationsAt`
- `src/core/polyline.ts#Polyline` (class, line 36) — reference by 1: `src/world/elevation.ts#roadStations`
- `src/core/polyline.ts#Polyline` (class, line 36) — typeRef by 4: `src/world/elevation.ts#Profile`, `src/world/elevation.ts#readStations`, `src/world/elevation.ts#sameLine`, `src/world/elevation.ts#ShapingRoad`
- `src/core/polyline.ts#Polyline.bbox` (getter, line 116) — call by 1: `src/world/elevation.ts#buildRoadElevation`
- `src/core/polyline.ts#Polyline.closestInto` (method, line 347) — call by 3: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#buildRoadElevation.localDigest`, `src/world/elevation.ts#roadShapeAt`
- `src/core/polyline.ts#Polyline.fromFlat` (method, line 65) — call by 1: `src/world/elevation.ts#roadStations`
- `src/core/polyline.ts#Polyline.length` (getter, line 123) — call by 2: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#roadStations`
- `src/core/polyline.ts#Polyline.sampleAt` (method, line 152) — call by 2: `src/world/elevation.ts#readStations`, `src/world/elevation.ts#stationsAt`
- `src/core/polyline.ts#Polyline.xy` (field, line 38) — read by 5: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#buildRoadElevation.localDigest`, `src/world/elevation.ts#roadsToRead`, `src/world/elevation.ts#sameLine`, …

### src/core/units.ts

- `src/core/units.ts#m` (function, line 6) — call by 19: `src/world/elevation.ts#BESIDE_DROP`, `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#CUT_SHOULDER`, `src/world/elevation.ts#ORDER_CELL`, …

### src/core/vec2.ts

- `src/core/vec2.ts#Vec2` (interface, line 4) — read by 2: `src/world/elevation.ts#readStations`, `src/world/elevation.ts#stationsAt`

### src/editor/commit.ts

- `src/editor/commit.ts#solveOver` (function, line 204) — indirectCall by 1: `src/world/elevation.ts#buildRoadElevation`

### src/lab/catalog/layers.ts

- `src/lab/catalog/layers.ts#farNetworkItems.flat` (function, line 200) — indirectCall by 1: `src/world/elevation.ts#buildRoadElevation`
- `src/lab/catalog/layers.ts#signItems.flat` (function, line 77) — indirectCall by 1: `src/world/elevation.ts#buildRoadElevation`

### src/lab/catalog/layout.ts

- `src/lab/catalog/layout.ts#layoutItems.flat` (function, line 166) — indirectCall by 1: `src/world/elevation.ts#buildRoadElevation`
- `src/lab/catalog/layout.ts#surfacesOf.flat` (function, line 112) — indirectCall by 1: `src/world/elevation.ts#buildRoadElevation`

### src/render/renderer.ts

- `src/render/renderer.ts#createSceneRenderer.buildWorld` (function, line 2317) — indirectCall by 1: `src/world/elevation.ts#buildRoadElevation`
- `src/render/renderer.ts#createSceneRenderer.completeSolve` (function, line 2225) — indirectCall by 1: `src/world/elevation.ts#buildRoadElevation`
- `src/render/renderer.ts#createSceneRenderer.warmRoadShaders` (function, line 2506) — indirectCall by 1: `src/world/elevation.ts#buildRoadElevation`

### src/world/cells.ts

- `src/world/cells.ts#CellFiling.idsMeeting` (method, line 131) — call by 1: `src/world/elevation.ts#scopeIds`
- `src/world/cells.ts#CellFiling.idsMeetingAny` (method, line 169) — call by 2: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#roadsToRead`
- `src/world/cells.ts#CellFiling.rectOf` (method, line 120) — call by 1: `src/world/elevation.ts#pendingOrder`

### src/world/changes.ts

- `src/world/changes.ts#ChangeRect` (type, line 4) — typeRef by 3: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#roadsToRead`, `src/world/elevation.ts#SolveScope`

### src/world/doc.ts

- `src/world/doc.ts#RoadDoc.node` (method, line 416) — call by 4: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#buildRoadElevation.incident`, `src/world/elevation.ts#buildRoadElevation.keyOf`, `src/world/elevation.ts#roadsToRead`
- `src/world/doc.ts#RoadDoc.segment` (method, line 420) — call by 2: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#roadsToRead`
- `src/world/doc.ts#RoadNode` (interface, line 89) — read by 4: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#buildRoadElevation.incident`, `src/world/elevation.ts#buildRoadElevation.keyOf`, `src/world/elevation.ts#roadsToRead`
- `src/world/doc.ts#RoadSegment` (interface, line 124) — read by 1: `src/world/elevation.ts#buildRoadElevation`

### src/world/ids.ts

- `src/world/ids.ts#NodeId` (type, line 7) — typeRef by 12: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#buildRoadElevation.capOf`, `src/world/elevation.ts#buildRoadElevation.gradeOf`, `src/world/elevation.ts#buildRoadElevation.incident`, …
- `src/world/ids.ts#SegmentId` (type, line 8) — typeRef by 13: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#buildRoadElevation.incident`, `src/world/elevation.ts#buildRoadElevation.mine`, `src/world/elevation.ts#buildRoadElevation.profileOf`, …

### src/world/junction/levels.ts

- `src/world/junction/levels.ts#bit` (function, line 169) — call by 1: `src/world/junction/levels.ts#JunctionLevels.has`
- `src/world/junction/levels.ts#JunctionLevels.constructor` (constructor, line 186) — read by 1: `src/world/junction/levels.ts#JunctionLevels.has`
- `src/world/junction/levels.ts#JunctionLevels.legsAt` (method, line 269) — call by 1: `src/world/elevation.ts#buildRoadElevation`

### src/world/junction/types.ts

- `src/world/junction/types.ts#Leg` (interface, line 14) — read by 1: `src/world/elevation.ts#buildRoadElevation`

### src/world/network.ts

- `src/world/network.ts#Network` (class, line 46) — typeRef by 7: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#everyRoad`, `src/world/elevation.ts#flatSolves`, `src/world/elevation.ts#flatSolvesOf`, …
- `src/world/network.ts#Network.changesSince` (method, line 143) — call by 2: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#roadsToRead`
- `src/world/network.ts#Network.constructor` (constructor, line 100) — read by 4: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#buildRoadElevation.incident`, `src/world/elevation.ts#buildRoadElevation.keyOf`, `src/world/elevation.ts#roadsToRead`
- `src/world/network.ts#Network.forkedFrom` (field, line 84) — read by 1: `src/world/elevation.ts#flatSolvesOf`
- `src/world/network.ts#Network.isBuilt` (method, line 170) — call by 2: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#roadsToRead`
- `src/world/network.ts#Network.junctions` (field, line 54) — read by 1: `src/world/elevation.ts#buildRoadElevation`
- `src/world/network.ts#Network.ribbons` (field, line 55) — read by 4: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#everyRoad`, `src/world/elevation.ts#roadsToRead`, `src/world/elevation.ts#scopeIds`
- `src/world/network.ts#Network.segmentCells` (field, line 57) — read by 4: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#pendingOrder`, `src/world/elevation.ts#roadsToRead`, `src/world/elevation.ts#scopeIds`
- `src/world/network.ts#Network.state` (getter, line 148) — call by 1: `src/world/elevation.ts#buildRoadElevation`
- `src/world/network.ts#Network.transitions` (field, line 79) — read by 1: `src/world/elevation.ts#buildRoadElevation`
- `src/world/network.ts#Network.trims` (field, line 75) — read by 1: `src/world/elevation.ts#buildRoadElevation`

### src/world/networkDelta.ts

- `src/world/networkDelta.ts#NetworkDelta` (interface, line 5) — read by 2: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#roadsToRead`
- `src/world/networkDelta.ts#NetworkDelta` (interface, line 5) — typeRef by 1: `src/world/elevation.ts#buildRoadElevation`

### src/world/ribbons.ts

- `src/world/ribbons.ts#SegmentRibbon` (interface, line 25) — read by 2: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#roadsToRead`
- `src/world/ribbons.ts#SegmentTrims` (interface, line 18) — read by 1: `src/world/elevation.ts#buildRoadElevation`

### src/world/roadField.ts

- `src/world/roadField.ts#BLEND_TAU` (variable, line 12) — read by 1: `src/world/elevation.ts#SURFACE_READ_REACH`
- `src/world/roadField.ts#EMPTY` (variable, line 267) — read by 1: `src/world/roadField.ts#SpatialIndex.near`
- `src/world/roadField.ts#FieldData` (interface, line 657) — read by 2: `src/world/elevation.ts#roadShaper`, `src/world/roadField.ts#packField`
- `src/world/roadField.ts#FieldData` (interface, line 657) — typeRef by 3: `src/world/elevation.ts#RoadElevation`, `src/world/elevation.ts#RoadShapeCut`, `src/world/roadField.ts#packField`
- `src/world/roadField.ts#FieldProfile` (interface, line 33) — read by 2: `src/world/elevation.ts#roadShaper`, `src/world/roadField.ts#packField`
- `src/world/roadField.ts#FieldProfile` (interface, line 33) — typeRef by 1: `src/world/roadField.ts#packField`
- `src/world/roadField.ts#heightAtArc` (function, line 131) — call by 1: `src/world/elevation.ts#roadShapeAt`
- `src/world/roadField.ts#INDEX_CELL` (variable, line 20) — read by 6: `src/world/elevation.ts#buildRoadElevation.take`, `src/world/elevation.ts#ProfileIndex.cells`, `src/world/elevation.ts#ProfileIndex.digest`, `src/world/elevation.ts#ProfileIndex.meeting`, …
- `src/world/roadField.ts#OVERSIZED_CELLS` (variable, line 145) — read by 2: `src/world/elevation.ts#ProfileIndex.cells`, `src/world/roadField.ts#SpatialIndex.constructor`
- `src/world/roadField.ts#PROFILE_FADE` (variable, line 18) — read by 1: `src/world/elevation.ts#buildRoadElevation`
- `src/world/roadField.ts#PROFILE_REACH` (variable, line 16) — read by 1: `src/world/elevation.ts#SURFACE_READ_REACH`
- `src/world/roadField.ts#roadField` (function, line 417) — call by 2: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#roadShaper`
- `src/world/roadField.ts#roadField` (function, line 417) — implementation by 2: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#roadShaper`
- `src/world/roadField.ts#roadField.onSegment` (method, line 601) — implementation by 1: `src/world/elevation.ts#buildRoadElevation`
- `src/world/roadField.ts#roadField.remembering` (method, line 591) — implementation by 1: `src/world/elevation.ts#buildRoadElevation`
- `src/world/roadField.ts#roadField.roadAt` (method, line 624) — implementation by 2: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#roadShaper`
- `src/world/roadField.ts#roadField.surfaceFrameAt` (method, line 642) — implementation by 1: `src/world/elevation.ts#buildRoadElevation`
- `src/world/roadField.ts#roadField.surfaceFrameInto` (method, line 645) — implementation by 1: `src/world/elevation.ts#buildRoadElevation`
- `src/world/roadField.ts#RoadReads` (interface, line 96) — inherit by 1: `src/world/elevation.ts#RoadElevation`
- `src/world/roadField.ts#RoadReads` (interface, line 96) — reference by 2: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#roadShaper`
- `src/world/roadField.ts#smoothstep` (function, line 125) — call by 3: `src/world/elevation.ts#roadShapeAt`, `src/world/elevation.ts#solveGround`, `src/world/elevation.ts#solveRaised`
- `src/world/roadField.ts#SpatialIndex` (class, line 153) — construct by 1: `src/world/elevation.ts#roadShaper`
- `src/world/roadField.ts#SpatialIndex` (class, line 153) — read by 1: `src/world/roadField.ts#SpatialIndex.constructor`
- `src/world/roadField.ts#SpatialIndex` (class, line 153) — typeRef by 7: `src/world/roadField.ts#SpatialIndex.near`, `src/world/roadField.ts#SpatialIndex.all`, `src/world/roadField.ts#SpatialIndex.buckets`, `src/world/roadField.ts#SpatialIndex.constructor`, …
- `src/world/roadField.ts#STRIDE` (variable, line 654) — read by 1: `src/world/roadField.ts#packField`
- `src/world/roadField.ts#unpackField` (function, line 714) — call by 1: `src/world/elevation.ts#roadShaper`

### src/world/roads/tuning.ts

- `src/world/roads/tuning.ts#ROAD_TUNING` (variable, line 4) — read by 7: `src/world/elevation.ts#DECK_GRADE`, `src/world/elevation.ts#DECK_TIE`, `src/world/elevation.ts#GROUND_GRADE`, `src/world/elevation.ts#LIFT_OFF`, …

### src/world/roadTypes.ts

- `src/world/roadTypes.ts#CASING_BAND` (variable, line 95) — read by 1: `src/world/elevation.ts#roadShapeAt`
- `src/world/roadTypes.ts#casingHalf` (function, line 384) — call by 2: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#roadsToRead`
- `src/world/roadTypes.ts#FOOTWAY_RISE` (variable, line 93) — read by 1: `src/world/elevation.ts#solveRaised`
- `src/world/roadTypes.ts#footwayOn` (function, line 356) — call by 1: `src/world/elevation.ts#buildRoadElevation`
- `src/world/roadTypes.ts#Level` (enum, line 10) — reference by 1: `src/world/elevation.ts#buildRoadElevation`
- `src/world/roadTypes.ts#RoadType` (interface, line 29) — read by 2: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#lookDigest`
- `src/world/roadTypes.ts#RoadType` (interface, line 29) — typeRef by 1: `src/world/elevation.ts#lookDigest`
- `src/world/roadTypes.ts#SurfaceLevel` (type, line 25) — typeRef by 1: `src/world/junction/levels.ts#JunctionLevels.has`

### src/world/structures.ts

- `src/world/structures.ts#isRaised` (function, line 68) — call by 3: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#buildRoadElevation.solve`, `src/world/elevation.ts#roadShapeAt`
- `src/world/structures.ts#ROAD_GROUND_CLEARANCE` (variable, line 53) — read by 2: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#readStations`
- `src/world/structures.ts#roadStructure` (function, line 55) — call by 3: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#buildRoadElevation.wantedOf`, `src/world/elevation.ts#solveRaised`
- `src/world/structures.ts#RoadStructure` (type, line 5) — typeRef by 9: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#buildRoadElevation.count`, `src/world/elevation.ts#GROUND_ONLY`, `src/world/elevation.ts#Profile`, …
- `src/world/structures.ts#RoadStructureSpec` (interface, line 17) — read by 3: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#buildRoadElevation.wantedOf`, `src/world/elevation.ts#solveRaised`
- `src/world/structures.ts#TUNNEL_BORE` (variable, line 38) — read by 1: `src/world/elevation.ts#roadShapeAt`
- `src/world/structures.ts#TUNNEL_GRADE` (variable, line 42) — read by 1: `src/world/elevation.ts#solveSunken`
- `src/world/structures.ts#TUNNEL_ROOF` (variable, line 37) — read by 1: `src/world/elevation.ts#roadShapeAt`

## External APIs

- `es:Math` × 133
- `es:Math.max` × 53
- `es:Array.length` × 42
- `es:Set.add` × 35
- `es:Math.min` × 28
- `es:Map.get` × 26
- `es:Math.floor` × 26
- `es:Set` × 25
- `es:Map.set` × 22
- `es:Map` × 19
- `es:Set.has` × 15
- `es:Array.push` × 14
- `es:Float64Array` × 14
- `es:ReadonlyArray.length` × 11
- `es:Array.slice` × 9
- `es:Array.map` × 8
- `es:Math.abs` × 8
- `es:Array.sort` × 6
- `es:Float64Array.length` × 6
- `es:Map.has` × 6
- `es:Math.round` × 6
- `es:Infinity` × 5
- `es:Error` × 4
- `es:Iterable` × 4
- `es:Map.keys` × 4
- `es:ReadonlyMap` × 4
- `es:ReadonlySet` × 4
- `es:WeakMap.get` × 4
- `es:ArrayBuffer` × 3
- `es:ArrayLike.length` × 3
- `es:Map.delete` × 3
- `es:Math.ceil` × 3
- `es:Math.hypot` × 3
- `dom:performance` × 2
- `es:Array` × 2
- `es:Math.exp` × 2
- `es:Pick` × 2
- `es:ReadonlyArray.filter` × 2
- `es:ReadonlyMap.get` × 2
- `es:Set.size` × 2
- `es:Uint32Array` × 2
- `es:Uint8Array` × 2
- `es:WeakMap` × 2
- `es:WeakMap.set` × 2
- `dom:Performance.measure` × 1
- `dom:Performance.now` × 1
- `es:Array.every` × 1
- `es:Array.fill` × 1
- `es:Array.filter` × 1
- `es:Array.pop` × 1
- `es:ArrayLike` × 1
- `es:Float64Array.buffer` × 1
- `es:Float64Array.set` × 1
- `es:Float64ArrayConstructor.from` × 1
- `es:IterableIterator` × 1
- `es:Map.size` × 1
- `es:Map.values` × 1
- `es:Math.cos` × 1
- `es:Math.log` × 1
- `es:Math.PI` × 1
- `es:Math.sin` × 1
- `es:Number` × 1
- `es:NumberConstructor.isFinite` × 1
- `es:Object` × 1
- `es:ObjectConstructor.freeze` × 1
- `es:ReadonlyArray.find` × 1
- `es:ReadonlyArray.forEach` × 1
- `es:ReadonlyArray.map` × 1
- `es:ReadonlyMap.has` × 1
- `es:ReadonlySet.has` × 1
- `es:Symbol` × 1
- `es:Uint8ArrayConstructor.from` × 1
