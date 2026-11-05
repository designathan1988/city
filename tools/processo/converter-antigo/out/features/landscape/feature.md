# Feature: src/editor/terrainBrush.ts, src/editor/streetscapeTool.ts, src/editor/cloudTool.ts, src/world/terrain.ts, src/world/terrainPaint.ts, src/world/landscape.ts, src/world/trees.ts, src/world/ecology.ts, src/world/weather.ts, src/world/clouds.ts, src/world/fogPaint.ts, src/world/groundPatch.ts, src/world/elevation.ts, src/world/gullies.ts, src/render/terrain.ts, src/render/terrainGrid.ts, src/render/terrainLevels.ts, src/render/terrainLod.ts, src/render/terrainShaping.ts, src/render/terrainLightCompute.ts, src/render/terrainLightPool.ts, src/render/terrainLight.worker.ts, src/render/water.ts, src/render/grass.ts, src/render/groundCover.ts, src/render/groundShape.ts, src/render/groundChanges.ts, src/render/groundShear.ts, src/render/natureTrees.ts, src/render/natureNoise.ts, src/render/lowPolyTrees.ts, src/render/stillTree.ts, src/render/farForest.ts, src/render/farWoods.ts, src/render/farGround.ts, src/render/woodsBase.ts, src/render/plantImpostors.ts, src/render/plantCells.ts, src/render/impostorShaders.ts, src/render/ecologyPool.ts, src/render/ecology.worker.ts, src/render/ecologyKept.ts, src/render/rain.ts, src/render/wind.ts, src/render/lightning.ts, src/render/fogLayer.ts, src/render/sunCascades.ts, src/render/canyonLight.ts, src/render/environment.ts, src/render/lightLevels.ts, src/render/cloudShadowStamp.ts, src/render/scenery.ts, src/render/bake/terrainRecipes.ts, src/render/bake/detailRecipes.ts, src/render/mesh/detailLayer.ts, src/render/elements.ts

The program starts from `src/main.ts`, `src/peopleLab/main.ts`. A declaration is the feature's own when the program reaches it only through the feature; in a declaration of the program that names the feature, its statements are a slice (what names it, what depends on that, what that controls).

- Own: 1402 declarations
- Slices of shared code: 412
- Code: 21191 lines in 148 files (source/)
- Resources: 347 (162 entry, 184 style, 1 import; 105 only the feature uses)
- Uses: 411 shared declarations (interface/: 1564 declarations in 292 files, the types they name included); hooks: 412; external APIs: 599

## Code

- src/actionsWiring.ts — part, 10 lines
- src/core/aabb.ts — part, 6 lines
- src/core/rerootMap.ts — whole, 105 lines
- src/editor/bulldozer.ts — part, 15 lines
- src/editor/chunkedSave.ts — part, 6 lines
- src/editor/cityGenerator.ts — part, 5 lines
- src/editor/cloudTool.ts — whole, 93 lines
- src/editor/commit.ts — part, 101 lines
- src/editor/editRules.ts — part, 21 lines
- src/editor/growthFeed.ts — part, 5 lines
- src/editor/growthReplica.ts — part, 13 lines
- src/editor/history.ts — part, 3 lines
- src/editor/persistence.ts — part, 19 lines
- src/editor/poles.ts — part, 8 lines
- src/editor/roads/streetKit.ts — part, 6 lines
- src/editor/roadTool.ts — part, 11 lines
- src/editor/smallCityMap.ts — part, 8 lines
- src/editor/streetObjects.ts — part, 7 lines
- src/editor/streetscapeTool.ts — whole, 155 lines
- src/editor/terrainBrush.ts — whole, 266 lines
- src/jobs/jobPool.ts — part, 17 lines
- src/lab/catalog/layers.ts — part, 12 lines
- src/lab/catalog/layout.ts — part, 11 lines
- src/main.ts — part, 330 lines
- src/render/agents.ts — part, 19 lines
- src/render/bake/detailRecipes.ts — whole, 262 lines
- src/render/bake/terrainRecipes.ts — whole, 309 lines
- src/render/buildings/buildingMesh.ts — part, 13 lines
- src/render/buildings/flagLayer.ts — part, 13 lines
- src/render/buildings/kit.ts — part, 15 lines
- src/render/canyonLight.ts — whole, 379 lines
- src/render/cellInstances.ts — part, 16 lines
- src/render/clearingCells.ts — part, 48 lines
- src/render/cloudShadowStamp.ts — whole, 55 lines
- src/render/coverRaster.ts — part, 57 lines
- src/render/coverSweep.ts — part, 70 lines
- src/render/densityCars.ts — part, 13 lines
- src/render/ecology.worker.ts — whole, 38 lines
- src/render/ecologyKept.ts — whole, 87 lines
- src/render/ecologyPool.ts — whole, 104 lines
- src/render/elements.ts — whole, 358 lines
- src/render/environment.ts — whole, 339 lines
- src/render/exhaust.ts — part, 7 lines
- src/render/farForest.ts — whole, 359 lines
- src/render/farGround.ts — whole, 66 lines
- src/render/farWoods.ts — whole, 196 lines
- src/render/flatAtlas.ts — part, 478 lines
- src/render/flatGround.worker.ts — part, 14 lines
- src/render/flatGroundPool.ts — part, 3 lines
- src/render/fogLayer.ts — whole, 205 lines
- src/render/gardens.ts — part, 49 lines
- src/render/gpu/twins.ts — part, 18 lines
- src/render/grass.ts — whole, 98 lines
- src/render/groundChanges.ts — whole, 80 lines
- src/render/groundCover.ts — whole, 323 lines
- src/render/groundShape.ts — whole, 267 lines
- src/render/groundShape.worker.ts — part, 14 lines
- src/render/groundShapePool.ts — part, 39 lines
- src/render/groundShear.ts — whole, 36 lines
- src/render/impostorShaders.ts — whole, 315 lines
- src/render/instanceRuns.ts — part, 9 lines
- src/render/landStroke.ts — part, 61 lines
- src/render/landStrokePool.ts — part, 1 lines
- src/render/lightLevels.ts — whole, 62 lines
- src/render/lightning.ts — whole, 139 lines
- src/render/lowPolyTrees.ts — whole, 646 lines
- src/render/materials.ts — part, 12 lines
- src/render/mesh/detailLayer.ts — whole, 123 lines
- src/render/mesh/textureBaker.ts — part, 22 lines
- src/render/natureNoise.ts — whole, 26 lines
- src/render/natureTrees.ts — whole, 332 lines
- src/render/optimizationsApply.ts — part, 14 lines
- src/render/plantCells.ts — whole, 175 lines
- src/render/plantImpostors.ts — whole, 512 lines
- src/render/plateMesh.ts — part, 11 lines
- src/render/platePaint.ts — part, 24 lines
- src/render/plateTexels.ts — part, 53 lines
- src/render/poleCells.ts — part, 3 lines
- src/render/poleLayer.ts — part, 13 lines
- src/render/postprocess.ts — part, 94 lines
- src/render/propCells.ts — part, 52 lines
- src/render/propGeometry.ts — part, 348 lines
- src/render/rain.ts — whole, 174 lines
- src/render/renderer.ts — part, 1268 lines
- src/render/riggedCitizens.ts — part, 10 lines
- src/render/roadStations.ts — part, 7 lines
- src/render/roadStations.worker.ts — part, 8 lines
- src/render/roadSurfacePass.ts — part, 8 lines
- src/render/roadSurfaceRings.ts — part, 4 lines
- src/render/roadSurfaces.ts — part, 213 lines
- src/render/scenery.ts — whole, 377 lines
- src/render/sharedGround.ts — part, 12 lines
- src/render/signals.ts — part, 9 lines
- src/render/signLayer.ts — part, 92 lines
- src/render/signs.ts — part, 138 lines
- src/render/stagedTexels.ts — part, 6 lines
- src/render/stillTree.ts — whole, 42 lines
- src/render/streetFurnitureLayer.ts — part, 180 lines
- src/render/structureCells.ts — part, 4 lines
- src/render/structures.ts — part, 239 lines
- src/render/sunCascades.ts — whole, 1172 lines
- src/render/terrain.ts — whole, 2855 lines
- src/render/terrainGrid.ts — whole, 13 lines
- src/render/terrainLevels.ts — whole, 162 lines
- src/render/terrainLight.worker.ts — whole, 25 lines
- src/render/terrainLightCompute.ts — whole, 97 lines
- src/render/terrainLightPool.ts — whole, 79 lines
- src/render/terrainLod.ts — whole, 325 lines
- src/render/terrainShaping.ts — whole, 95 lines
- src/render/torusWindow.ts — part, 5 lines
- src/render/transit.ts — part, 13 lines
- src/render/utilities.ts — part, 50 lines
- src/render/water.ts — whole, 406 lines
- src/render/wind.ts — whole, 210 lines
- src/render/woodsBase.ts — whole, 79 lines
- src/ui/toolChoices.ts — part, 26 lines
- src/ui/v2/shell.ts — part, 38 lines
- src/world/buildings/cameraSolids.ts — part, 21 lines
- src/world/buildings/pads.ts — part, 12 lines
- src/world/cityGen/field.ts — part, 5 lines
- src/world/cityGen/plan.ts — part, 12 lines
- src/world/cityGen/squares.ts — part, 17 lines
- src/world/clouds.ts — whole, 111 lines
- src/world/doc.ts — part, 449 lines
- src/world/ecology.ts — whole, 447 lines
- src/world/elements.ts — part, 26 lines
- src/world/elevation.ts — whole, 1541 lines
- src/world/fogPaint.ts — whole, 89 lines
- src/world/groundPatch.ts — whole, 66 lines
- src/world/gullies.ts — whole, 24 lines
- src/world/junction/bulb.ts — part, 7 lines
- src/world/junction/levels.ts — part, 3 lines
- src/world/landscape.ts — whole, 415 lines
- src/world/network.ts — part, 17 lines
- src/world/poleLines.ts — part, 8 lines
- src/world/roadField.ts — part, 84 lines
- src/world/roads/buildMode.ts — part, 10 lines
- src/world/roads/derivedSigns.ts — part, 17 lines
- src/world/roads/furnitureCatalog.ts — part, 4 lines
- src/world/roads/furnitureSets.ts — part, 74 lines
- src/world/roads/streetNames.ts — part, 15 lines
- src/world/solids.ts — part, 20 lines
- src/world/streetFurniture.ts — part, 50 lines
- src/world/structures.ts — part, 1 lines
- src/world/terrain.ts — whole, 296 lines
- src/world/terrainPaint.ts — whole, 38 lines
- src/world/trees.ts — whole, 89 lines
- src/world/weather.ts — whole, 55 lines

## Slices of shared code

- `src/actionsWiring.ts#createActions.explodeAt` — 10 of 158 lines
- `src/editor/bulldozer.ts#Bulldozer.boxed` — 7 of 25 lines
- `src/editor/bulldozer.ts#Bulldozer.click` — 8 of 29 lines
- `src/editor/chunkedSave.ts#KEYED` — 6 of 35 lines
- `src/editor/cityGenerator.ts#greenCity` — 5 of 17 lines
- `src/editor/commit.ts#DraftResult` — 3 of 14 lines
- `src/editor/commit.ts#boreDeepCuts` — 19 of 38 lines
- `src/editor/commit.ts#commitRoadPath` — 44 of 97 lines
- `src/editor/commit.ts#draftStations` — 9 of 24 lines
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
- `src/editor/growthFeed.ts#GrowthSession.send` — 5 of 48 lines
- `src/editor/growthReplica.ts#GrowAsk` — 3 of 30 lines
- `src/editor/growthReplica.ts#GrowthReplica.handle` — 10 of 39 lines
- `src/editor/history.ts#openedGenerated` — 3 of 4 lines
- `src/editor/persistence.ts#isSerializedDoc` — 19 of 98 lines
- `src/editor/poles.ts#inLanding` — 8 of 9 lines
- `src/editor/roadTool.ts#RoadToolHost` — 4 of 36 lines
- `src/editor/roadTool.ts#landscapeAlong` — 7 of 10 lines
- `src/editor/roads/streetKit.ts#carried` — 1 of 1 lines
- `src/editor/roads/streetKit.ts#kitOf` — 5 of 19 lines
- `src/editor/smallCityMap.ts#growLots` — 7 of 17 lines
- `src/editor/smallCityMap.ts#growLots.pavedAt` — 1 of 1 lines
- `src/editor/streetObjects.ts#commitUturn` — 7 of 34 lines
- `src/lab/catalog/layers.ts#signItems` — 7 of 19 lines
- `src/lab/catalog/layers.ts#signItems.one` — 3 of 9 lines
- `src/lab/catalog/layout.ts#surfacesOf` — 11 of 26 lines
- `src/main.ts#<ExpressionStatement>@1674` — 1 of 1 lines
- `src/main.ts#<ExpressionStatement>@192` — 1 of 1 lines
- `src/main.ts#<ExpressionStatement>@195` — 1 of 1 lines
- `src/main.ts#<IfStatement>@1703` — 7 of 9 lines
- `src/main.ts#<IfStatement>@4036` — 9 of 196 lines
- `src/main.ts#BrushMode` — 1 of 1 lines
- `src/main.ts#LANDFORMS` — 6 of 6 lines
- `src/main.ts#MAP_ACTIONS` — 11 of 20 lines
- `src/main.ts#NEW_RELIEF` — 1 of 1 lines
- `src/main.ts#SLIDERS` — 8 of 57 lines
- `src/main.ts#bulldozer` — 3 of 10 lines
- `src/main.ts#cloudTool` — 14 of 14 lines
- `src/main.ts#currentGesture` — 5 of 18 lines
- `src/main.ts#dabSettings` — 2 of 15 lines
- `src/main.ts#drawOverlayScreen` — 102 of 469 lines
- `src/main.ts#drawStreetscapeHover` — 33 of 40 lines
- `src/main.ts#endPointer` — 8 of 30 lines
- `src/main.ts#endTerrainStroke` — 7 of 11 lines
- `src/main.ts#frame` — 4 of 102 lines
- `src/main.ts#gestureInProgress` — 3 of 3 lines
- `src/main.ts#growth` — 3 of 3 lines
- `src/main.ts#mapBiome` — 3 of 3 lines
- `src/main.ts#newMap` — 5 of 13 lines
- `src/main.ts#newNature` — 1 of 1 lines
- `src/main.ts#openCity` — 3 of 15 lines
- `src/main.ts#openCityNow` — 7 of 26 lines
- `src/main.ts#pointerDown` — 13 of 180 lines
- `src/main.ts#pointerMove` — 15 of 72 lines
- `src/main.ts#restoreBootSave` — 7 of 29 lines
- `src/main.ts#setMapBiome` — 5 of 10 lines
- `src/main.ts#setNodeHeightMetres` — 16 of 26 lines
- `src/main.ts#streetscapeTool` — 12 of 12 lines
- `src/main.ts#terrainBrush` — 11 of 11 lines
- `src/main.ts#weatherSlider` — 2 of 2 lines
- `src/render/agents.ts#LAMP_GAIN` — 7 of 7 lines
- `src/render/agents.ts#createAgentMeshes.sync` — 12 of 421 lines
- `src/render/buildings/buildingMesh.ts#LAWN_LATTICE` — 1 of 1 lines
- `src/render/buildings/buildingMesh.ts#emitLots` — 12 of 179 lines
- `src/render/buildings/flagLayer.ts#createFlagLayer.set` — 8 of 46 lines
- `src/render/buildings/flagLayer.ts#material` — 5 of 46 lines
- `src/render/buildings/kit.ts#createBuildingKit` — 7 of 256 lines
- `src/render/buildings/kit.ts#createBuildingKit.setNight` — 8 of 21 lines
- `src/render/cellInstances.ts#createCellInstances.append` — 5 of 34 lines
- `src/render/cellInstances.ts#createCellInstances.visit` — 11 of 91 lines
- `src/render/clearingCells.ts#ClearingCells.cells` — 1 of 1 lines
- `src/render/clearingCells.ts#ClearingCells.file` — 7 of 8 lines
- `src/render/clearingCells.ts#ClearingCells.filed` — 1 of 1 lines
- `src/render/clearingCells.ts#ClearingCells.follow` — 16 of 21 lines
- `src/render/clearingCells.ts#ClearingCells.has` — 5 of 6 lines
- `src/render/clearingCells.ts#ClearingCells.over` — 5 of 5 lines
- `src/render/clearingCells.ts#ClearingCells.refile` — 4 of 6 lines
- `src/render/clearingCells.ts#ClearingCells.unfile` — 9 of 10 lines
- `src/render/coverSweep.ts#Candidate` — 3 of 8 lines
- `src/render/coverSweep.ts#forestIn` — 13 of 21 lines
- `src/render/coverSweep.ts#insidePlate` — 6 of 8 lines
- `src/render/coverSweep.ts#latticeOver` — 6 of 6 lines
- `src/render/coverSweep.ts#rocksIn` — 12 of 19 lines
- `src/render/coverSweep.ts#scrubIn` — 14 of 18 lines
- `src/render/coverSweep.ts#shoreIn` — 16 of 22 lines
- `src/render/densityCars.ts#TILE_RADIUS` — 1 of 1 lines
- `src/render/densityCars.ts#createDensityCars.tileAt` — 12 of 19 lines
- `src/render/exhaust.ts#createExhaust` — 7 of 207 lines
- `src/render/flatAtlas.ts#FarResident` — 3 of 5 lines
- `src/render/flatAtlas.ts#Plate` — 5 of 23 lines
- `src/render/flatAtlas.ts#PlateSource` — 12 of 15 lines
- `src/render/flatAtlas.ts#PlateSource.paintSources` — 1 of 1 lines
- `src/render/flatAtlas.ts#PlateSource.stampSources` — 1 of 1 lines
- `src/render/flatAtlas.ts#PlateSource.terrainPaint` — 1 of 1 lines
- `src/render/flatAtlas.ts#PlateSource.terrainStamps` — 1 of 1 lines
- `src/render/flatAtlas.ts#SIDE` — 1 of 1 lines
- `src/render/flatAtlas.ts#createFlatAtlas` — 175 of 730 lines
- `src/render/flatAtlas.ts#createFlatAtlas.ask` — 4 of 4 lines
- `src/render/flatAtlas.ts#createFlatAtlas.askCorners` — 5 of 16 lines
- `src/render/flatAtlas.ts#createFlatAtlas.bareAt` — 1 of 1 lines
- `src/render/flatAtlas.ts#createFlatAtlas.buildFar` — 8 of 8 lines
- `src/render/flatAtlas.ts#createFlatAtlas.buildSlab` — 3 of 8 lines
- `src/render/flatAtlas.ts#createFlatAtlas.decode` — 5 of 5 lines
- `src/render/flatAtlas.ts#createFlatAtlas.drawFar` — 6 of 12 lines
- `src/render/flatAtlas.ts#createFlatAtlas.encode` — 4 of 4 lines
- `src/render/flatAtlas.ts#createFlatAtlas.ensure` — 43 of 43 lines
- `src/render/flatAtlas.ts#createFlatAtlas.fill` — 9 of 9 lines
- `src/render/flatAtlas.ts#createFlatAtlas.followFar` — 20 of 30 lines
- `src/render/flatAtlas.ts#createFlatAtlas.groundOn` — 11 of 13 lines
- `src/render/flatAtlas.ts#createFlatAtlas.keepWithinBudget` — 7 of 11 lines
- `src/render/flatAtlas.ts#createFlatAtlas.letGo` — 4 of 14 lines
- `src/render/flatAtlas.ts#createFlatAtlas.measureTop` — 3 of 3 lines
- `src/render/flatAtlas.ts#createFlatAtlas.one` — 3 of 5 lines
- `src/render/flatAtlas.ts#createFlatAtlas.plateHolds` — 9 of 9 lines
- `src/render/flatAtlas.ts#createFlatAtlas.plateHolds.reach` — 1 of 1 lines
- `src/render/flatAtlas.ts#createFlatAtlas.readContent` — 21 of 21 lines
- `src/render/flatAtlas.ts#createFlatAtlas.readWholeContent` — 19 of 19 lines
- `src/render/flatAtlas.ts#createFlatAtlas.refresh` — 5 of 31 lines
- `src/render/flatAtlas.ts#createFlatAtlas.shaperFor` — 4 of 4 lines
- `src/render/flatAtlas.ts#createFlatAtlas.show` — 3 of 6 lines
- `src/render/flatAtlas.ts#createFlatAtlas.sync` — 45 of 45 lines
- `src/render/flatAtlas.ts#createFlatAtlas.take` — 8 of 12 lines
- `src/render/flatAtlas.ts#createFlatAtlas.takePaint` — 8 of 8 lines
- `src/render/flatAtlas.ts#createFlatAtlas.takeStamps` — 8 of 8 lines
- `src/render/flatAtlas.ts#createFlatAtlas.writeFar` — 3 of 6 lines
- `src/render/flatAtlas.ts#reaches` — 2 of 2 lines
- `src/render/flatAtlas.ts#writeFarNode` — 3 of 38 lines
- `src/render/flatAtlas.ts#writeFarPatch` — 3 of 3 lines
- `src/render/flatGround.worker.ts#Ask` — 1 of 1 lines
- `src/render/flatGround.worker.ts#serve` — 13 of 23 lines
- `src/render/flatGroundPool.ts#Job` — 3 of 15 lines
- `src/render/gardens.ts#createGardenCells` — 7 of 29 lines
- `src/render/gardens.ts#createGardenCells.dispose` — 3 of 4 lines
- `src/render/gardens.ts#gardenKinds` — 7 of 19 lines
- `src/render/gardens.ts#gardenKinds.kind` — 7 of 11 lines
- `src/render/gardens.ts#gardenPack` — 24 of 80 lines
- `src/render/gardens.ts#gardenPictures` — 1 of 1 lines
- `src/render/gpu/twins.ts#LandLodData` — 3 of 4 lines
- `src/render/gpu/twins.ts#ground` — 13 of 53 lines
- `src/render/gpu/twins.ts#ground.point` — 1 of 1 lines
- `src/render/gpu/twins.ts#ground.texel` — 1 of 1 lines
- `src/render/groundShape.worker.ts#serve` — 14 of 17 lines
- `src/render/groundShapePool.ts#GroundShapePool.digest` — 3 of 3 lines
- `src/render/groundShapePool.ts#GroundShapePool.pads` — 3 of 3 lines
- `src/render/groundShapePool.ts#GroundShapePool.run` — 5 of 12 lines
- `src/render/groundShapePool.ts#GroundShapePool.shape` — 3 of 3 lines
- `src/render/groundShapePool.ts#GroundShapePool.start` — 17 of 33 lines
- `src/render/groundShapePool.ts#GroundWorkers` — 5 of 10 lines
- `src/render/groundShapePool.ts#Lane` — 3 of 8 lines
- `src/render/landStroke.ts#GRID` — 1 of 1 lines
- `src/render/landStroke.ts#measureLevels` — 7 of 7 lines
- `src/render/landStroke.ts#runLandStroke` — 7 of 13 lines
- `src/render/landStroke.ts#shoreLevelsInto` — 28 of 32 lines
- `src/render/landStroke.ts#woodsBlock` — 9 of 16 lines
- `src/render/landStroke.ts#woodsBlock.land` — 1 of 1 lines
- `src/render/materials.ts#createMaterials` — 9 of 114 lines
- `src/render/materials.ts#createMaterials.setDetail` — 3 of 3 lines
- `src/render/optimizationsApply.ts#OptimizerDeps` — 3 of 7 lines
- `src/render/optimizationsApply.ts#createOptimizer` — 3 of 185 lines
- `src/render/optimizationsApply.ts#createOptimizer.set` — 8 of 27 lines
- `src/render/plateMesh.ts#buildPlateMesh` — 11 of 26 lines
- `src/render/platePaint.ts#dabInRects` — 2 of 2 lines
- `src/render/platePaint.ts#dabKey` — 1 of 1 lines
- `src/render/platePaint.ts#plateDabs` — 21 of 22 lines
- `src/render/poleCells.ts#PoleCells.constructor` — 3 of 11 lines
- `src/render/poleLayer.ts#buildPolePreview` — 6 of 22 lines
- `src/render/poleLayer.ts#buildUtilities` — 7 of 11 lines
- `src/render/postprocess.ts#PostChain` — 3 of 30 lines
- `src/render/postprocess.ts#createPostChain` — 64 of 493 lines
- `src/render/postprocess.ts#createPostChain.disposeAll` — 3 of 18 lines
- `src/render/postprocess.ts#createPostChain.followTown` — 3 of 3 lines
- `src/render/postprocess.ts#createPostChain.mapLost` — 1 of 1 lines
- `src/render/postprocess.ts#createPostChain.render` — 3 of 35 lines
- `src/render/postprocess.ts#createPostChain.setAtmosphere` — 3 of 12 lines
- `src/render/postprocess.ts#layClouds` — 14 of 16 lines
- `src/render/propCells.ts#PropLayerSpec` — 3 of 21 lines
- `src/render/propCells.ts#createPropLayer.batches` — 27 of 127 lines
- `src/render/propCells.ts#createPropLayer.cull` — 3 of 5 lines
- `src/render/propCells.ts#createPropLayer.note` — 9 of 12 lines
- `src/render/propCells.ts#createPropLayer.show` — 6 of 42 lines
- `src/render/propCells.ts#nearSquareOf` — 4 of 5 lines
- `src/render/propGeometry.ts#TREE_KIND` — 3 of 3 lines
- `src/render/renderer.ts#NaturePlate` — 3 of 7 lines
- `src/render/renderer.ts#SURFACE_SETS` — 1 of 1 lines
- `src/render/renderer.ts#SceneHandle` — 5 of 170 lines
- `src/render/renderer.ts#Shaping` — 5 of 11 lines
- `src/render/renderer.ts#createSceneRenderer` — 704 of 3774 lines
- `src/render/renderer.ts#createSceneRenderer.applyQuality` — 4 of 17 lines
- `src/render/renderer.ts#createSceneRenderer.blockRead` — 9 of 9 lines
- `src/render/renderer.ts#createSceneRenderer.blocksOver` — 15 of 15 lines
- `src/render/renderer.ts#createSceneRenderer.bringPlateLand` — 7 of 9 lines
- `src/render/renderer.ts#createSceneRenderer.buildWorld` — 16 of 78 lines
- `src/render/renderer.ts#createSceneRenderer.builtGroundAt` — 1 of 1 lines
- `src/render/renderer.ts#createSceneRenderer.compareSolves` — 74 of 74 lines
- `src/render/renderer.ts#createSceneRenderer.completeSolve` — 12 of 24 lines
- `src/render/renderer.ts#createSceneRenderer.coverCellsOf` — 10 of 10 lines
- `src/render/renderer.ts#createSceneRenderer.coverPlain` — 1 of 1 lines
- `src/render/renderer.ts#createSceneRenderer.deckHeight` — 4 of 8 lines
- `src/render/renderer.ts#createSceneRenderer.elementLayerOf` — 8 of 8 lines
- `src/render/renderer.ts#createSceneRenderer.farCellRect` — 5 of 5 lines
- `src/render/renderer.ts#createSceneRenderer.fileNature` — 10 of 26 lines
- `src/render/renderer.ts#createSceneRenderer.landGroundDigest` — 5 of 6 lines
- `src/render/renderer.ts#createSceneRenderer.landMovedSince` — 9 of 9 lines
- `src/render/renderer.ts#createSceneRenderer.landShapes` — 5 of 12 lines
- `src/render/renderer.ts#createSceneRenderer.levelGroundDigest` — 6 of 8 lines
- `src/render/renderer.ts#createSceneRenderer.loadEffects` — 5 of 18 lines
- `src/render/renderer.ts#createSceneRenderer.natureOpen` — 6 of 11 lines
- `src/render/renderer.ts#createSceneRenderer.natureStands` — 4 of 5 lines
- `src/render/renderer.ts#createSceneRenderer.natureSweep` — 74 of 89 lines
- `src/render/renderer.ts#createSceneRenderer.networkExtent` — 11 of 11 lines
- `src/render/renderer.ts#createSceneRenderer.noteSway` — 5 of 11 lines
- `src/render/renderer.ts#createSceneRenderer.padsAskOf` — 12 of 12 lines
- `src/render/renderer.ts#createSceneRenderer.paintedForestOf` — 9 of 9 lines
- `src/render/renderer.ts#createSceneRenderer.plantAll` — 9 of 10 lines
- `src/render/renderer.ts#createSceneRenderer.plantedForestOf` — 10 of 10 lines
- `src/render/renderer.ts#createSceneRenderer.plantedIn` — 1 of 1 lines
- `src/render/renderer.ts#createSceneRenderer.platesUnder` — 4 of 7 lines
- `src/render/renderer.ts#createSceneRenderer.readOf` — 5 of 5 lines
- `src/render/renderer.ts#createSceneRenderer.rebuildUtilities` — 3 of 7 lines
- `src/render/renderer.ts#createSceneRenderer.regionRect` — 1 of 1 lines
- `src/render/renderer.ts#createSceneRenderer.requestNetworkFar` — 4 of 4 lines
- `src/render/renderer.ts#createSceneRenderer.shapeGround` — 47 of 47 lines
- `src/render/renderer.ts#createSceneRenderer.startNatureTrees` — 25 of 26 lines
- `src/render/renderer.ts#createSceneRenderer.stationsAsked` — 2 of 2 lines
- `src/render/renderer.ts#createSceneRenderer.strikeAt` — 4 of 7 lines
- `src/render/renderer.ts#createSceneRenderer.sweepAllCovers` — 5 of 8 lines
- `src/render/renderer.ts#createSceneRenderer.switchedOff` — 16 of 16 lines
- `src/render/renderer.ts#createSceneRenderer.takeEditedCells` — 20 of 26 lines
- `src/render/renderer.ts#createSceneRenderer.terrainRegion` — 1 of 1 lines
- `src/render/renderer.ts#createSceneRenderer.warmPreviewShaders` — 9 of 38 lines
- `src/render/renderer.ts#createSceneRenderer.warmRoadShaders` — 15 of 39 lines
- `src/render/renderer.ts#createSceneRenderer.watchRoads` — 14 of 27 lines
- `src/render/renderer.ts#createSceneRenderer.waterMeshes` — 3 of 5 lines
- `src/render/renderer.ts#createSceneRenderer.worldSteps` — 24 of 105 lines
- `src/render/renderer.ts#isLevelLand` — 1 of 1 lines
- `src/render/riggedCitizens.ts#createRiggedCitizens.draw` — 6 of 92 lines
- `src/render/riggedCitizens.ts#createRiggedCitizens.emit` — 4 of 15 lines
- `src/render/roadStations.ts#Hint` — 2 of 4 lines
- `src/render/roadStations.ts#RoadStations.current` — 2 of 3 lines
- `src/render/roadStations.ts#RoadStations.waiting` — 3 of 30 lines
- `src/render/roadStations.worker.ts#serve` — 8 of 13 lines
- `src/render/roadSurfacePass.ts#GROUND_MAX_EDGE` — 1 of 1 lines
- `src/render/roadSurfacePass.ts#RAISED_MAX_EDGE` — 1 of 1 lines
- `src/render/roadSurfacePass.ts#followsGround` — 1 of 1 lines
- `src/render/roadSurfacePass.ts#passDesc` — 5 of 19 lines
- `src/render/roadSurfaceRings.ts#medianOf.back` — 4 of 5 lines
- `src/render/roadSurfaces.ts#buildRoadSurfaces` — 9 of 14 lines
- `src/render/roadSurfaces.ts#roadSurfaceSteps` — 204 of 512 lines
- `src/render/sharedGround.ts#GroundShare.relief` — 1 of 1 lines
- `src/render/sharedGround.ts#GroundShare.setRelief` — 4 of 5 lines
- `src/render/sharedGround.ts#SharedGroundData` — 3 of 12 lines
- `src/render/sharedGround.ts#sharedGroundReaders.bare` — 1 of 1 lines
- `src/render/sharedGround.ts#sharedGroundReaders.read` — 3 of 9 lines
- `src/render/signLayer.ts#SignLayerKept` — 3 of 14 lines
- `src/render/signLayer.ts#SignProduct.allKeys` — 3 of 7 lines
- `src/render/signLayer.ts#SignProduct.dirty` — 12 of 30 lines
- `src/render/signLayer.ts#SignSource` — 3 of 5 lines
- `src/render/signLayer.ts#Stand` — 3 of 8 lines
- `src/render/signLayer.ts#Standing` — 3 of 4 lines
- `src/render/signLayer.ts#createSignLayer.putQuad` — 24 of 24 lines
- `src/render/signLayer.ts#isSignItem` — 1 of 1 lines
- `src/render/signLayer.ts#itemSigns` — 40 of 42 lines
- `src/render/signals.ts#createSignalHeads` — 9 of 626 lines
- `src/render/signs.ts#PLATE` — 12 of 12 lines
- `src/render/signs.ts#Sheet` — 3 of 21 lines
- `src/render/signs.ts#SignSheets.drop` — 20 of 21 lines
- `src/render/signs.ts#SignSheets.plateCell` — 17 of 17 lines
- `src/render/signs.ts#SignSheets.take` — 6 of 6 lines
- `src/render/signs.ts#onTopOfPost` — 1 of 1 lines
- `src/render/signs.ts#plateCanvas` — 75 of 87 lines
- `src/render/signs.ts#plateFaces` — 4 of 5 lines
- `src/render/streetFurnitureLayer.ts#FurnitureSource` — 3 of 6 lines
- `src/render/streetFurnitureLayer.ts#MEADOW_STORES` — 4 of 4 lines
- `src/render/streetFurnitureLayer.ts#PlacedProduct.allKeys` — 3 of 6 lines
- `src/render/streetFurnitureLayer.ts#PlacedProduct.compute` — 4 of 4 lines
- `src/render/streetFurnitureLayer.ts#PlacedProduct.constructor` — 4 of 9 lines
- `src/render/streetFurnitureLayer.ts#PlacedProduct.dirty` — 9 of 19 lines
- `src/render/streetFurnitureLayer.ts#ROAD_REACH` — 1 of 1 lines
- `src/render/streetFurnitureLayer.ts#StreetFurnitureLayer` — 3 of 17 lines
- `src/render/streetFurnitureLayer.ts#createStreetFurniture` — 22 of 71 lines
- `src/render/streetFurnitureLayer.ts#createStreetFurniture.dispose` — 3 of 5 lines
- `src/render/streetFurnitureLayer.ts#furniturePlace` — 59 of 65 lines
- `src/render/streetFurnitureLayer.ts#furnitureStores` — 26 of 51 lines
- `src/render/streetFurnitureLayer.ts#furnitureStores.plant` — 7 of 11 lines
- `src/render/streetFurnitureLayer.ts#isFurniture` — 1 of 1 lines
- `src/render/streetFurnitureLayer.ts#meadowInto` — 5 of 9 lines
- `src/render/streetFurnitureLayer.ts#meadowInto.add` — 4 of 4 lines
- `src/render/streetFurnitureLayer.ts#meadowProduct` — 10 of 10 lines
- `src/render/streetFurnitureLayer.ts#placeInto` — 10 of 11 lines
- `src/render/streetFurnitureLayer.ts#stillGrowing` — 2 of 2 lines
- `src/render/structureCells.ts#StructureCells` — 4 of 8 lines
- `src/render/structures.ts#buildStructureDetails` — 5 of 14 lines
- `src/render/structures.ts#retainingWalls` — 44 of 58 lines
- `src/render/structures.ts#structureParts` — 187 of 205 lines
- `src/render/structures.ts#structureRibbons` — 3 of 35 lines
- `src/render/transit.ts#createTransit` — 4 of 145 lines
- `src/render/transit.ts#createTransit.update` — 9 of 39 lines
- `src/render/utilities.ts#placePole` — 5 of 83 lines
- `src/render/utilities.ts#poleGroundAt` — 10 of 14 lines
- `src/render/utilities.ts#polePartSpecs` — 19 of 25 lines
- `src/render/utilities.ts#wireMaterials` — 16 of 31 lines
- `src/ui/toolChoices.ts#kind` — 1 of 1 lines
- `src/ui/toolChoices.ts#paint` — 1 of 1 lines
- `src/ui/toolChoices.ts#paintKind` — 2 of 3 lines
- `src/ui/toolChoices.ts#setPaintKind` — 4 of 4 lines
- `src/ui/toolChoices.ts#setStreetscapeKind` — 4 of 4 lines
- `src/ui/toolChoices.ts#setTreeBrush` — 4 of 4 lines
- `src/ui/toolChoices.ts#setTreeKind` — 1 of 1 lines
- `src/ui/toolChoices.ts#signChoice` — 1 of 1 lines
- `src/ui/toolChoices.ts#streetscapeKind` — 2 of 3 lines
- `src/ui/toolChoices.ts#treeBrush` — 1 of 1 lines
- `src/ui/toolChoices.ts#treeBrushNow` — 3 of 3 lines
- `src/ui/toolChoices.ts#treeKind` — 1 of 1 lines
- `src/ui/toolChoices.ts#treeKindNow` — 1 of 1 lines
- `src/ui/v2/shell.ts#PAINT_SWATCH` — 5 of 5 lines
- `src/ui/v2/shell.ts#mountShell.renderLandscape` — 30 of 145 lines
- `src/ui/v2/shell.ts#mountShell.textInput` — 3 of 10 lines
- `src/world/cityGen/field.ts#TensorField.major` — 5 of 7 lines
- `src/world/cityGen/plan.ts#planCity.inside` — 4 of 5 lines
- `src/world/cityGen/plan.ts#planCity.zoneAt` — 8 of 18 lines
- `src/world/cityGen/squares.ts#SquarePlan` — 4 of 6 lines
- `src/world/cityGen/squares.ts#planSquares` — 13 of 53 lines
- `src/world/doc.ts#RoadDoc.addClouds` — 11 of 14 lines
- `src/world/doc.ts#RoadDoc.addLandscape` — 3 of 3 lines
- `src/world/doc.ts#RoadDoc.addLandscapeMany` — 14 of 14 lines
- `src/world/doc.ts#RoadDoc.addPaintDab` — 7 of 9 lines
- `src/world/doc.ts#RoadDoc.addTerrainStamp` — 9 of 10 lines
- `src/world/doc.ts#RoadDoc.clearTerrain` — 4 of 8 lines
- `src/world/doc.ts#RoadDoc.clouds` — 1 of 1 lines
- `src/world/doc.ts#RoadDoc.countStreetName` — 5 of 5 lines
- `src/world/doc.ts#RoadDoc.deleteLandscapeRecord` — 3 of 8 lines
- `src/world/doc.ts#RoadDoc.fogDabs` — 1 of 1 lines
- `src/world/doc.ts#RoadDoc.fogSettings` — 1 of 1 lines
- `src/world/doc.ts#RoadDoc.fromJSON` — 42 of 187 lines
- `src/world/doc.ts#RoadDoc.gullyAuto` — 1 of 1 lines
- `src/world/doc.ts#RoadDoc.gullyDabs` — 1 of 1 lines
- `src/world/doc.ts#RoadDoc.landscape` — 1 of 1 lines
- `src/world/doc.ts#RoadDoc.landscapeAt` — 1 of 1 lines
- `src/world/doc.ts#RoadDoc.landscapeIn` — 2 of 3 lines
- `src/world/doc.ts#RoadDoc.landscapeInAny` — 2 of 3 lines
- `src/world/doc.ts#RoadDoc.landscapeMap` — 1 of 1 lines
- `src/world/doc.ts#RoadDoc.layOver` — 7 of 47 lines
- `src/world/doc.ts#RoadDoc.nature` — 1 of 1 lines
- `src/world/doc.ts#RoadDoc.plantTrees` — 9 of 10 lines
- `src/world/doc.ts#RoadDoc.putLandscapeRecord` — 7 of 8 lines
- `src/world/doc.ts#RoadDoc.removeCloud` — 5 of 6 lines
- `src/world/doc.ts#RoadDoc.removeLandscape` — 5 of 7 lines
- `src/world/doc.ts#RoadDoc.replaceContents` — 36 of 149 lines
- `src/world/doc.ts#RoadDoc.replaceWith` — 7 of 32 lines
- `src/world/doc.ts#RoadDoc.restore` — 16 of 105 lines
- `src/world/doc.ts#RoadDoc.restore.itemRects` — 5 of 11 lines
- `src/world/doc.ts#RoadDoc.restoreValue` — 9 of 27 lines
- `src/world/doc.ts#RoadDoc.savedGlobals` — 11 of 18 lines
- `src/world/doc.ts#RoadDoc.serialized` — 5 of 35 lines
- `src/world/doc.ts#RoadDoc.setFogSettings` — 5 of 5 lines
- `src/world/doc.ts#RoadDoc.setGullyAuto` — 5 of 5 lines
- `src/world/doc.ts#RoadDoc.setNature` — 4 of 4 lines
- `src/world/doc.ts#RoadDoc.setWeather` — 5 of 5 lines
- `src/world/doc.ts#RoadDoc.takeFork` — 14 of 49 lines
- `src/world/doc.ts#RoadDoc.terrainPaint` — 1 of 1 lines
- `src/world/doc.ts#RoadDoc.terrainRelief` — 1 of 1 lines
- `src/world/doc.ts#RoadDoc.terrainStamps` — 1 of 1 lines
- `src/world/doc.ts#RoadDoc.treeCells` — 5 of 7 lines
- `src/world/doc.ts#RoadDoc.treeClearings` — 1 of 1 lines
- `src/world/doc.ts#RoadDoc.treeGrid` — 1 of 1 lines
- `src/world/doc.ts#RoadDoc.trees` — 1 of 1 lines
- `src/world/doc.ts#RoadDoc.treesIn` — 3 of 5 lines
- `src/world/doc.ts#RoadDoc.valueOf` — 9 of 15 lines
- `src/world/doc.ts#RoadDoc.weather` — 1 of 1 lines
- `src/world/doc.ts#RoadDoc.withRoom` — 6 of 12 lines
- `src/world/doc.ts#RoadDoc.worldPart` — 11 of 26 lines
- `src/world/doc.ts#SerializedDoc` — 3 of 75 lines
- `src/world/doc.ts#TILE_CAPS` — 4 of 4 lines
- `src/world/doc.ts#itemDifferences` — 11 of 33 lines
- `src/world/doc.ts#landscapeRecord` — 8 of 11 lines
- `src/world/doc.ts#listBox` — 9 of 15 lines
- `src/world/doc.ts#placedCloud` — 2 of 2 lines
- `src/world/doc.ts#samePaint` — 7 of 8 lines
- `src/world/doc.ts#sameStamps` — 10 of 11 lines
- `src/world/doc.ts#savedLandscape` — 3 of 3 lines
- `src/world/doc.ts#stampDifferences` — 12 of 15 lines
- `src/world/junction/bulb.ts#clearBulbsOf` — 7 of 32 lines
- `src/world/poleLines.ts#PoleLines` — 3 of 10 lines
- `src/world/poleLines.ts#poleLinesOf` — 5 of 28 lines
- `src/world/roads/buildMode.ts#stationsAlong` — 10 of 26 lines
- `src/world/roads/furnitureCatalog.ts#FurnitureEntry` — 3 of 10 lines
- `src/world/roads/furnitureCatalog.ts#furnitureEntry` — 1 of 1 lines
- `src/world/roads/furnitureSets.ts#FurniturePlacement` — 3 of 4 lines
- `src/world/roads/furnitureSets.ts#Pattern` — 3 of 9 lines
- `src/world/roads/furnitureSets.ts#furnitureFor` — 47 of 94 lines
- `src/world/roads/furnitureSets.ts#furnitureFor.file` — 6 of 6 lines
- `src/world/roads/furnitureSets.ts#nearGrid` — 3 of 17 lines
- `src/world/roads/furnitureSets.ts#nearGrid.around` — 7 of 7 lines
- `src/world/roads/furnitureSets.ts#nearGrid.file` — 5 of 5 lines
- `src/world/roads/streetNames.ts#platesNear` — 9 of 11 lines
- `src/world/roads/streetNames.ts#streetNameOf` — 6 of 10 lines
- `src/world/solids.ts#FURNITURE` — 4 of 4 lines
- `src/world/solids.ts#createSolids` — 7 of 47 lines
- `src/world/solids.ts#solidsOfItem` — 9 of 38 lines
- `src/world/streetFurniture.ts#FURNITURE_OF` — 11 of 11 lines
- `src/world/streetFurniture.ts#furnitureOf` — 9 of 11 lines
- `src/world/streetFurniture.ts#standing` — 23 of 28 lines
- `src/world/streetFurniture.ts#streetFurniture` — 7 of 10 lines

## Resources

- shared entry `talk` — src/render/people/faceExpression.ts:45: `talk: { channel: 'faceJoy', amount: 0.25, talk: true }`
- shared entry `laugh` — src/render/people/faceExpression.ts:47: `laugh: { channel: 'faceJoy', amount: 1, laugh: true }`
- shared entry `cheer` — src/render/people/faceExpression.ts:48: `cheer: { channel: 'faceJoy', amount: 0.9, laugh: true }`
- shared entry `wave` — src/render/people/faceExpression.ts:49: `wave: { channel: 'faceJoy', amount: 0.6 }`
- shared entry `argue` — src/render/people/faceExpression.ts:50: `argue: { channel: 'faceAnger', amount: 0.8, talk: true }`
- shared entry `phone` — src/render/people/faceExpression.ts:52: `phone: { amount: 0, talk: true }`
- shared entry `dance` — src/render/people/faceExpression.ts:53: `dance: { channel: 'faceJoy', amount: 0.7 }`
- feature entry `hint.streetscape.rule.stop` — src/ui/i18n/en.ts:298: `'hint.streetscape.rule.stop': 'Stop sign: this road now stops at the junction'`
- feature entry `hint.streetscape.rule.yield` — src/ui/i18n/en.ts:299: `'hint.streetscape.rule.yield': 'Give-way sign: this road now gives way at the junction'`
- feature entry `hint.streetscape.row` — src/ui/i18n/en.ts:334: `'hint.streetscape.row': 'Row laid along the footway'`
- feature entry `hint.streetscape.removed` — src/ui/i18n/en.ts:335: `'hint.streetscape.removed': 'Item removed'`
- feature entry `hint.streetscape.offFootway` — src/ui/i18n/en.ts:336: `'hint.streetscape.offFootway': 'Footways only'`
- feature entry `hint.streetscape.narrow` — src/ui/i18n/en.ts:337: `'hint.streetscape.narrow': 'The footway is too narrow for that'`
- feature entry `hint.streetscape.occupied` — src/ui/i18n/en.ts:338: `'hint.streetscape.occupied': 'Something is already here'`
- feature entry `hint.streetscape.crossing` — src/ui/i18n/en.ts:339: `'hint.streetscape.crossing': 'Not in a crossing landing'`
- feature entry `streetscape.tree` — src/ui/i18n/en.ts:340: `'streetscape.tree': 'Tree'`
- feature entry `streetscape.shrub` — src/ui/i18n/en.ts:341: `'streetscape.shrub': 'Shrub'`
- feature entry `streetscape.bench` — src/ui/i18n/en.ts:342: `'streetscape.bench': 'Bench'`
- feature entry `streetscape.bin` — src/ui/i18n/en.ts:343: `'streetscape.bin': 'Bin'`
- feature entry `streetscape.lamp` — src/ui/i18n/en.ts:344: `'streetscape.lamp': 'Street light'`
- feature entry `streetscape.hydrant` — src/ui/i18n/en.ts:345: `'streetscape.hydrant': 'Hydrant'`
- feature entry `streetscape.postbox` — src/ui/i18n/en.ts:346: `'streetscape.postbox': 'Post box'`
- feature entry `streetscape.phone` — src/ui/i18n/en.ts:347: `'streetscape.phone': 'Public phone'`
- feature entry `streetscape.drain` — src/ui/i18n/en.ts:348: `'streetscape.drain': 'Kerb drain'`
- feature entry `streetscape.meadow` — src/ui/i18n/en.ts:349: `'streetscape.meadow': 'Long grass (on the lawn)'`
- feature entry `streetscape.sign` — src/ui/i18n/en.ts:350: `'streetscape.sign': 'Sign'`
- feature entry `streetscape.streetname` — src/ui/i18n/en.ts:351: `'streetscape.streetname': 'Street name'`
- feature entry `sign.type` — src/ui/i18n/en.ts:352: `'sign.type': 'Sign type'`
- feature entry `sign.type.stop` — src/ui/i18n/en.ts:353: `'sign.type.stop': 'Stop'`
- feature entry `sign.type.yield` — src/ui/i18n/en.ts:354: `'sign.type.yield': 'Give way'`
- feature entry `sign.type.speed` — src/ui/i18n/en.ts:355: `'sign.type.speed': 'Speed limit'`
- feature entry `sign.type.noParking` — src/ui/i18n/en.ts:356: `'sign.type.noParking': 'No parking'`
- feature entry `sign.type.noEntry` — src/ui/i18n/en.ts:357: `'sign.type.noEntry': 'No entry'`
- feature entry `sign.type.pedestrian` — src/ui/i18n/en.ts:358: `'sign.type.pedestrian': 'Pedestrians'`
- feature entry `sign.type.school` — src/ui/i18n/en.ts:359: `'sign.type.school': 'School'`
- feature entry `sign.type.direction` — src/ui/i18n/en.ts:360: `'sign.type.direction': 'Direction'`
- feature entry `sign.type.street` — src/ui/i18n/en.ts:361: `'sign.type.street': 'Street plate'`
- feature entry `sign.type.info` — src/ui/i18n/en.ts:362: `'sign.type.info': 'Information'`
- feature entry `sign.text` — src/ui/i18n/en.ts:363: `'sign.text': 'Sign text'`
- feature entry `sign.text.speed` — src/ui/i18n/en.ts:364: `'sign.text.speed': 'Speed (e.g. 40)'`
- feature entry `tree.kind.mixed` — src/ui/i18n/en.ts:490: `'tree.kind.mixed': 'Mixed'`
- feature entry `tree.kind.oak` — src/ui/i18n/en.ts:491: `'tree.kind.oak': 'Oak'`
- feature entry `tree.kind.cypress` — src/ui/i18n/en.ts:492: `'tree.kind.cypress': 'Cypress'`
- feature entry `tree.kind.palm` — src/ui/i18n/en.ts:493: `'tree.kind.palm': 'Palm'`
- feature entry `confirm.clearClouds` — src/ui/i18n/en.ts:526: `'confirm.clearClouds': 'Remove every cloud in the sky?'`
- feature entry `hint.cloud.full` — src/ui/i18n/en.ts:528: `'hint.cloud.full': 'The sky already holds the most clouds (24)'`
- shared entry `paint.kind.sand` — src/ui/i18n/en.ts:540: `'paint.kind.sand': 'Sand'`
- shared entry `paint.kind.soil` — src/ui/i18n/en.ts:541: `'paint.kind.soil': 'Soil'`
- shared entry `paint.kind.meadow` — src/ui/i18n/en.ts:542: `'paint.kind.meadow': 'Meadow'`
- shared entry `paint.kind.snow` — src/ui/i18n/en.ts:543: `'paint.kind.snow': 'Snow'`
- shared entry `paint.kind.gravel` — src/ui/i18n/en.ts:544: `'paint.kind.gravel': 'Gravel'`
- shared entry `paint.kind.asphalt` — src/ui/i18n/en.ts:545: `'paint.kind.asphalt': 'Asphalt'`
- shared entry `paint.kind.concrete` — src/ui/i18n/en.ts:546: `'paint.kind.concrete': 'Concrete'`
- shared entry `paint.kind.grass` — src/ui/i18n/en.ts:547: `'paint.kind.grass': 'Grass (erases)'`
- shared entry `paint.kind.forest` — src/ui/i18n/en.ts:548: `'paint.kind.forest': 'Forest'`
- shared entry `paint.kind.granite` — src/ui/i18n/en.ts:555: `'paint.kind.granite': 'Rock: granite'`
- shared entry `paint.kind.sandstone` — src/ui/i18n/en.ts:556: `'paint.kind.sandstone': 'Rock: sandstone'`
- shared entry `paint.kind.basalt` — src/ui/i18n/en.ts:557: `'paint.kind.basalt': 'Rock: basalt'`
- shared entry `paint.kind.cerrado` — src/ui/i18n/en.ts:558: `'paint.kind.cerrado': 'Cerrado'`
- shared entry `paint.kind.atlantic` — src/ui/i18n/en.ts:559: `'paint.kind.atlantic': 'Atlantic forest'`
- shared entry `paint.kind.amazon` — src/ui/i18n/en.ts:560: `'paint.kind.amazon': 'Amazon'`
- shared entry `paint.kind.caatinga` — src/ui/i18n/en.ts:561: `'paint.kind.caatinga': 'Caatinga'`
- shared entry `paint.kind.pampa` — src/ui/i18n/en.ts:562: `'paint.kind.pampa': 'Pampa'`
- shared entry `paint.kind.pantanal` — src/ui/i18n/en.ts:563: `'paint.kind.pantanal': 'Pantanal'`
- shared entry `paint.kind.scrub` — src/ui/i18n/en.ts:574: `'paint.kind.scrub': 'Low scrub'`
- shared entry `paint.kind.flowers` — src/ui/i18n/en.ts:575: `'paint.kind.flowers': 'Flowers'`
- shared entry `paint.kind.rocks` — src/ui/i18n/en.ts:576: `'paint.kind.rocks': 'Rocks'`
- feature entry `terrain.level` — src/ui/i18n/en.ts:580: `'terrain.level': 'Level to'`
- shared entry `hint.road.gradeLimited` — src/ui/i18n/en.ts:720: `'hint.road.gradeLimited': 'The road reached its safe grade; continue drawing to reach the selected height.'`
- feature entry `hint.tileFull` — src/ui/i18n/en.ts:925: `'hint.tileFull': 'This part of the map holds as many as it can: nothing was added, nothing was taken away'`
- feature entry `hint.terrain.capNear` — src/ui/i18n/en.ts:935: `'hint.terrain.capNear': 'The land is close to its sculpting limit: past it, the oldest strokes are replaced'`
- feature entry `hint.terrain.capReached` — src/ui/i18n/en.ts:936: `'hint.terrain.capReached': 'Sculpting limit reached: each new stroke replaces the oldest one'`
- feature entry `v2.row.biome` — src/ui/i18n/en.ts:1341: `'v2.row.biome': 'Biome'`
- feature entry `v2.row.sign` — src/ui/i18n/en.ts:1343: `'v2.row.sign': 'Sign'`
- shared entry `v2.row.text` — src/ui/i18n/en.ts:1344: `'v2.row.text': 'Text'`
- shared entry `v2.map` — src/ui/i18n/en.ts:1348: `'v2.map': 'Whole map'`
- feature entry `hint.streetscape.rule.stop` — src/ui/i18n/pt-BR.ts:297: `'hint.streetscape.rule.stop': 'Placa de Pare: esta via agora para no cruzamento'`
- feature entry `hint.streetscape.rule.yield` — src/ui/i18n/pt-BR.ts:298: `'hint.streetscape.rule.yield': 'Placa de Dê a preferência: esta via agora dá a preferência no cruzamento'`
- feature entry `hint.streetscape.row` — src/ui/i18n/pt-BR.ts:332: `'hint.streetscape.row': 'Fileira posta ao longo da calçada'`
- feature entry `hint.streetscape.removed` — src/ui/i18n/pt-BR.ts:333: `'hint.streetscape.removed': 'Item removido'`
- feature entry `hint.streetscape.offFootway` — src/ui/i18n/pt-BR.ts:334: `'hint.streetscape.offFootway': 'Só nas calçadas'`
- feature entry `hint.streetscape.narrow` — src/ui/i18n/pt-BR.ts:335: `'hint.streetscape.narrow': 'A calçada é estreita demais para isso'`
- feature entry `hint.streetscape.occupied` — src/ui/i18n/pt-BR.ts:336: `'hint.streetscape.occupied': 'Já há algo aqui'`
- feature entry `hint.streetscape.crossing` — src/ui/i18n/pt-BR.ts:337: `'hint.streetscape.crossing': 'Não na saída de uma faixa de pedestres'`
- feature entry `streetscape.tree` — src/ui/i18n/pt-BR.ts:338: `'streetscape.tree': 'Árvore'`
- feature entry `streetscape.shrub` — src/ui/i18n/pt-BR.ts:339: `'streetscape.shrub': 'Arbusto'`
- feature entry `streetscape.bench` — src/ui/i18n/pt-BR.ts:340: `'streetscape.bench': 'Banco'`
- feature entry `streetscape.bin` — src/ui/i18n/pt-BR.ts:341: `'streetscape.bin': 'Lixeira'`
- feature entry `streetscape.lamp` — src/ui/i18n/pt-BR.ts:342: `'streetscape.lamp': 'Iluminação'`
- feature entry `streetscape.hydrant` — src/ui/i18n/pt-BR.ts:343: `'streetscape.hydrant': 'Hidrante'`
- feature entry `streetscape.postbox` — src/ui/i18n/pt-BR.ts:344: `'streetscape.postbox': 'Caixa de correio'`
- feature entry `streetscape.phone` — src/ui/i18n/pt-BR.ts:345: `'streetscape.phone': 'Orelhão (telefone público)'`
- feature entry `streetscape.drain` — src/ui/i18n/pt-BR.ts:346: `'streetscape.drain': 'Boca de lobo'`
- feature entry `streetscape.meadow` — src/ui/i18n/pt-BR.ts:347: `'streetscape.meadow': 'Mato (no gramado)'`
- feature entry `streetscape.sign` — src/ui/i18n/pt-BR.ts:348: `'streetscape.sign': 'Placa'`
- feature entry `streetscape.streetname` — src/ui/i18n/pt-BR.ts:349: `'streetscape.streetname': 'Nome da rua'`
- feature entry `sign.type` — src/ui/i18n/pt-BR.ts:350: `'sign.type': 'Tipo de placa'`
- feature entry `sign.type.stop` — src/ui/i18n/pt-BR.ts:351: `'sign.type.stop': 'Pare'`
- feature entry `sign.type.yield` — src/ui/i18n/pt-BR.ts:352: `'sign.type.yield': 'Dê a preferência'`
- feature entry `sign.type.speed` — src/ui/i18n/pt-BR.ts:353: `'sign.type.speed': 'Velocidade máxima'`
- feature entry `sign.type.noParking` — src/ui/i18n/pt-BR.ts:354: `'sign.type.noParking': 'Proibido estacionar'`
- feature entry `sign.type.noEntry` — src/ui/i18n/pt-BR.ts:355: `'sign.type.noEntry': 'Sentido proibido'`
- feature entry `sign.type.pedestrian` — src/ui/i18n/pt-BR.ts:356: `'sign.type.pedestrian': 'Pedestres'`
- feature entry `sign.type.school` — src/ui/i18n/pt-BR.ts:357: `'sign.type.school': 'Escola'`
- feature entry `sign.type.direction` — src/ui/i18n/pt-BR.ts:358: `'sign.type.direction': 'Direção'`
- feature entry `sign.type.street` — src/ui/i18n/pt-BR.ts:359: `'sign.type.street': 'Placa de rua'`
- feature entry `sign.type.info` — src/ui/i18n/pt-BR.ts:360: `'sign.type.info': 'Informação'`
- feature entry `sign.text` — src/ui/i18n/pt-BR.ts:361: `'sign.text': 'Texto da placa'`
- feature entry `sign.text.speed` — src/ui/i18n/pt-BR.ts:362: `'sign.text.speed': 'Velocidade (ex.: 40)'`
- feature entry `tree.kind.mixed` — src/ui/i18n/pt-BR.ts:485: `'tree.kind.mixed': 'Mistas'`
- feature entry `tree.kind.oak` — src/ui/i18n/pt-BR.ts:486: `'tree.kind.oak': 'Carvalho'`
- feature entry `tree.kind.cypress` — src/ui/i18n/pt-BR.ts:487: `'tree.kind.cypress': 'Cipreste'`
- feature entry `tree.kind.palm` — src/ui/i18n/pt-BR.ts:488: `'tree.kind.palm': 'Palmeira'`
- feature entry `confirm.clearClouds` — src/ui/i18n/pt-BR.ts:521: `'confirm.clearClouds': 'Apagar todas as nuvens do céu?'`
- feature entry `hint.cloud.full` — src/ui/i18n/pt-BR.ts:523: `'hint.cloud.full': 'O céu já tem o máximo de nuvens (24)'`
- shared entry `paint.kind.sand` — src/ui/i18n/pt-BR.ts:535: `'paint.kind.sand': 'Areia'`
- shared entry `paint.kind.soil` — src/ui/i18n/pt-BR.ts:536: `'paint.kind.soil': 'Terra'`
- shared entry `paint.kind.meadow` — src/ui/i18n/pt-BR.ts:537: `'paint.kind.meadow': 'Mato'`
- shared entry `paint.kind.snow` — src/ui/i18n/pt-BR.ts:538: `'paint.kind.snow': 'Neve'`
- shared entry `paint.kind.gravel` — src/ui/i18n/pt-BR.ts:539: `'paint.kind.gravel': 'Cascalho'`
- shared entry `paint.kind.asphalt` — src/ui/i18n/pt-BR.ts:540: `'paint.kind.asphalt': 'Asfalto'`
- shared entry `paint.kind.concrete` — src/ui/i18n/pt-BR.ts:541: `'paint.kind.concrete': 'Concreto'`
- shared entry `paint.kind.grass` — src/ui/i18n/pt-BR.ts:542: `'paint.kind.grass': 'Grama (apaga)'`
- shared entry `paint.kind.forest` — src/ui/i18n/pt-BR.ts:543: `'paint.kind.forest': 'Floresta'`
- shared entry `paint.kind.granite` — src/ui/i18n/pt-BR.ts:550: `'paint.kind.granite': 'Rocha: granito'`
- shared entry `paint.kind.sandstone` — src/ui/i18n/pt-BR.ts:551: `'paint.kind.sandstone': 'Rocha: arenito'`
- shared entry `paint.kind.basalt` — src/ui/i18n/pt-BR.ts:552: `'paint.kind.basalt': 'Rocha: basalto'`
- shared entry `paint.kind.cerrado` — src/ui/i18n/pt-BR.ts:553: `'paint.kind.cerrado': 'Cerrado'`
- shared entry `paint.kind.atlantic` — src/ui/i18n/pt-BR.ts:554: `'paint.kind.atlantic': 'Mata Atlântica'`
- shared entry `paint.kind.amazon` — src/ui/i18n/pt-BR.ts:555: `'paint.kind.amazon': 'Amazônia'`
- shared entry `paint.kind.caatinga` — src/ui/i18n/pt-BR.ts:556: `'paint.kind.caatinga': 'Caatinga'`
- shared entry `paint.kind.pampa` — src/ui/i18n/pt-BR.ts:557: `'paint.kind.pampa': 'Pampa'`
- shared entry `paint.kind.pantanal` — src/ui/i18n/pt-BR.ts:558: `'paint.kind.pantanal': 'Pantanal'`
- shared entry `paint.kind.scrub` — src/ui/i18n/pt-BR.ts:569: `'paint.kind.scrub': 'Mata baixa'`
- shared entry `paint.kind.flowers` — src/ui/i18n/pt-BR.ts:570: `'paint.kind.flowers': 'Flores'`
- shared entry `paint.kind.rocks` — src/ui/i18n/pt-BR.ts:571: `'paint.kind.rocks': 'Pedras'`
- feature entry `terrain.level` — src/ui/i18n/pt-BR.ts:575: `'terrain.level': 'Nivelar em'`
- shared entry `hint.road.gradeLimited` — src/ui/i18n/pt-BR.ts:713: `'hint.road.gradeLimited': 'A via atingiu a inclinação segura; continue desenhando para chegar à altura escolhida.'`
- feature entry `hint.tileFull` — src/ui/i18n/pt-BR.ts:918: `'hint.tileFull': 'Este trecho do mapa já tem o máximo que cabe: nada foi acrescentado e nada foi apagado'`
- feature entry `hint.terrain.capNear` — src/ui/i18n/pt-BR.ts:928: `'hint.terrain.capNear': 'O terreno está perto do limite de escultura: além dele, os traços mais antigos são substituídos'`
- feature entry `hint.terrain.capReached` — src/ui/i18n/pt-BR.ts:929: `'hint.terrain.capReached': 'Limite de escultura atingido: cada novo traço substitui o mais antigo'`
- feature entry `v2.row.biome` — src/ui/i18n/pt-BR.ts:1331: `'v2.row.biome': 'Bioma'`
- feature entry `v2.row.sign` — src/ui/i18n/pt-BR.ts:1333: `'v2.row.sign': 'Placa'`
- shared entry `v2.row.text` — src/ui/i18n/pt-BR.ts:1334: `'v2.row.text': 'Texto'`
- shared entry `v2.map` — src/ui/i18n/pt-BR.ts:1338: `'v2.map': 'Mapa inteiro'`
- shared entry `ls_tree` — src/ui/v2/shell.ts:175: `ls_tree: '<path d="M12 21v-7"/><circle cx="12" cy="9" r="6"/><path d="M8 21h8"/>'`
- feature entry `ls_shrub` — src/ui/v2/shell.ts:176: `ls_shrub: '<path d="M4 19c0-4 3-7 8-7s8 3 8 7Z"/><path d="M8 13c0-3 2-5 4-5s4 2 4 5"/><path d="M3 21h18"/>'`
- shared entry `ls_bench` — src/ui/v2/shell.ts:177: `ls_bench: '<path d="M4 11h16"/><path d="M4 15h16"/><path d="M6 15v5M18 15v5M6 11V7h12v4"/>'`
- feature entry `ls_bin` — src/ui/v2/shell.ts:178: `ls_bin: '<path d="M6 7h12l-1 14H7Z"/><path d="M5 7h14M10 4h4"/><path d="M10 11v6M14 11v6"/>'`
- feature entry `ls_lamp` — src/ui/v2/shell.ts:179: `ls_lamp: '<path d="M7 21V5h7"/><path d="M12 5h5l1 2h-6Z"/><path d="M15 9l-1 3M17 9l1 3" stroke-dasharray="1.5 1.5"/><path d="M4 21h6"/>'`
- feature entry `ls_hydrant` — src/ui/v2/shell.ts:180: `ls_hydrant: '<path d="M8 21V10h8v11Z"/><path d="M8 10a4 4 0 0 1 8 0"/><path d="M5 14h3M16 14h3M12 4v2"/><path d="M6 21h12"/>'`
- feature entry `ls_postbox` — src/ui/v2/shell.ts:181: `ls_postbox: '<rect x="6" y="5" width="12" height="11" rx="5"/><path d="M9 9h6"/><path d="M12 16v5M8 21h8"/>'`
- feature entry `ls_phone` — src/ui/v2/shell.ts:182: `ls_phone: '<path d="M7 11a5 6 0 0 1 10 0v2H7Z"/><path d="M12 13v8M9 21h6"/><path d="M11 9h2v3h-2Z"/>'`
- feature entry `ls_meadow` — src/ui/v2/shell.ts:183: `ls_meadow: '<path d="M4 20c1-4 2-7 1-10M8 20c0-5 1-8 3-11M12 20c0-4-1-8-3-12M16 20c0-5 2-8 4-10M20 20c-1-3-1-6 0-8"/>'`
- feature entry `ls_sign` — src/ui/v2/shell.ts:184: `ls_sign: '<path d="M12 22V11"/><path d="M8 3h8l3 4-3 4H8L5 7Z"/>'`
- feature entry `ls_streetname` — src/ui/v2/shell.ts:185: `ls_streetname: '<path d="M12 22V8"/><rect x="3" y="3" width="18" height="5" rx="1"/><path d="M6 5.5h12" stroke-dasharray="2 1.5"/>'`
- feature entry `ls_drain` — src/ui/v2/shell.ts:186: `ls_drain: '<path d="M3 9h18"/><path d="M3 9v3h18V9"/><path d="M5 15h14v4H5Z"/><path d="M8 15v4M11 15v4M14 15v4M17 15v4"/>'`
- feature entry `tk_mixed` — src/ui/v2/shell.ts:230: `tk_mixed: '<circle cx="8" cy="10" r="4"/><path d="M8 14v6"/><path d="M16 4l4 10h-8Z"/><path d="M16 14v6"/><path d="M3 21h18"/>'`
- feature entry `tk_oak` — src/ui/v2/shell.ts:231: `tk_oak: '<circle cx="12" cy="10" r="6"/><path d="M12 16v5M9 13l3 3 3-3"/>'`
- feature entry `tk_cypress` — src/ui/v2/shell.ts:232: `tk_cypress: '<path d="M12 2c3 4 4 9 3 15H9C8 11 9 6 12 2Z"/><path d="M12 17v4"/>'`
- feature entry `tk_palm` — src/ui/v2/shell.ts:233: `tk_palm: '<path d="M12 21c0-5 1-9 0-12"/><path d="M12 9C9 6 5 6 3 8M12 9c3-3 7-3 9-1M12 9c-1-3-4-5-7-5M12 9c1-3 4-5 7-5"/>'`
- shared entry `complete` — src/world/roads/furnitureSets.ts:33: `complete: patternsOf('complete')`
- feature import `virtual:model-urls/elements?ext=gltf,bin,png` — src/render/elements.ts:25: `import URLS from 'virtual:model-urls/elements?ext=gltf,bin,png';`
- shared style `lab-code-viewer` — src/lab/code/code.css:2: `.lab-code-viewer pre { position: relative; max-height: 40vh; white-space: pre; }`
- shared style `lab-code-line` — src/lab/code/code.css:3: `.lab-code-line { min-height: 1.4em; }`
- shared style `lab-code-hit` — src/lab/code/code.css:4: `.lab-code-hit { background: color-mix(in srgb, var(--lab-accent) 24%, transparent); outline: 1px solid color-mix(in srgb, var(--lab-accent) 60%, transparent); }`
- shared style `lab-code-summary` — src/lab/code/code.css:5: `.lab-code-summary { display: flex; flex-wrap: wrap; gap: 4px 14px; margin: 4px 0 6px; color: var(--lab-muted); }`
- shared style `lab-code-summary` — src/lab/code/code.css:6: `.lab-code-summary b { font-weight: 600; color: var(--lab-text); }`
- shared style `lab-code-summary` — src/lab/code/code.css:7: `.lab-code-summary b.lab-warn { color: var(--lab-warn); }`
- shared style `lab-code-summary` — src/lab/code/code.css:8: `.lab-code-summary b.lab-good { color: var(--lab-good); }`
- shared style `lab-code-sections` — src/lab/code/code.css:9: `.lab-code-sections { display: flex; flex-wrap: wrap; gap: 4px; margin: 6px 0; }`
- shared style `lab-code-note` — src/lab/code/code.css:10: `.lab-code-note { margin: 4px 0; color: var(--lab-muted); }`
- shared style `lab-code-note` — src/lab/code/code.css:11: `.lab-code-note.lab-bad { color: var(--lab-bad); }`
- shared style `lab-code-sel` — src/lab/code/code.css:12: `.lab-code-sel strong { color: var(--lab-accent); }`
- shared style `lab-panel` — src/lab/code/code.css:13: `.lab-panel button .lab-icon { display: inline-block; vertical-align: -2px; }`
- shared style `lab-doctor` — src/lab/doctor/doctor.css:2: `.lab-doctor { display: flex; flex-direction: column; gap: 10px; max-width: 1100px; }`
- shared style `lab-doc-top` — src/lab/doctor/doctor.css:3: `.lab-doc-top { position: sticky; top: -10px; z-index: 2; background: var(--lab-bg); padding: 4px 0; margin: 0; }`
- shared style `lab-doc-primary` — src/lab/doctor/doctor.css:4: `.lab-doc-primary { background: var(--lab-accent) !important; color: var(--lab-accent-ink) !important; border-color: var(--lab-accent) !important; font-weight: 6`
- shared style `lab-doc-primary` — src/lab/doctor/doctor.css:5: `.lab-doc-primary:hover { filter: brightness(1.1); }`
- shared style `lab-doc-verdict` — src/lab/doctor/doctor.css:7: `.lab-doc-verdict { border-radius: 10px; padding: 10px 14px; border: 1px solid var(--lab-line2); background: var(--lab-bg2); }`
- shared style `lab-doc-verdict` — src/lab/doctor/doctor.css:8: `.lab-doc-verdict h2 { margin: 0 0 4px; font-size: 1.35em; }`
- shared style `lab-doc-verdict` — src/lab/doctor/doctor.css:9: `.lab-doc-verdict p { margin: 0; color: #c7d3d8; }`
- shared style `lab-doc-section` — src/lab/doctor/doctor.css:17: `.lab-doc-section h3 { margin: 4px 0 6px; }`
- shared style `lab-doc-stack` — src/lab/doctor/doctor.css:18: `.lab-doc-stack { display: flex; height: 22px; border-radius: 6px; overflow: hidden; background: var(--lab-bg3); }`
- shared style `lab-doc-stack` — src/lab/doctor/doctor.css:19: `.lab-doc-stack span { display: block; height: 100%; min-width: 2px; cursor: pointer; border-right: 1px solid rgb(0 0 0 / 0.35); }`
- shared style `lab-doc-stack` — src/lab/doctor/doctor.css:20: `.lab-doc-stack span:hover { filter: brightness(1.25); }`
- shared style `lab-doc-legend` — src/lab/doctor/doctor.css:21: `.lab-doc-legend { display: flex; flex-wrap: wrap; gap: 4px; margin-top: 6px; }`
- shared style `lab-doc-key` — src/lab/doctor/doctor.css:22: `.lab-doc-key { gap: 6px !important; }`
- shared style `lab-doc-key` — src/lab/doctor/doctor.css:23: `.lab-doc-key i { display: inline-block; width: 10px; height: 10px; border-radius: 2px; }`
- shared style `lab-doc-key` — src/lab/doctor/doctor.css:24: `.lab-doc-key strong { font-variant-numeric: tabular-nums; }`
- shared style `lab-doc-layers` — src/lab/doctor/doctor.css:26: `.lab-doc-layers { display: flex; flex-direction: column; gap: 3px; }`
- shared style `lab-doc-layer` — src/lab/doctor/doctor.css:27: `.lab-doc-layer { position: relative; display: grid !important; grid-template-columns: 1fr auto; gap: 8px; text-align: left; padding: 5px 8px !important; overflo`
- shared style `lab-doc-layer-name` — src/lab/doctor/doctor.css:28: `.lab-doc-layer-name, .lab-doc-layer-tris { position: relative; z-index: 1; overflow: hidden; text-overflow: ellipsis; }`
- shared style `lab-doc-layer-tris` — src/lab/doctor/doctor.css:29: `.lab-doc-layer-tris { color: var(--lab-muted); font-variant-numeric: tabular-nums; }`
- shared style `lab-doc-layer-bar` — src/lab/doctor/doctor.css:30: `.lab-doc-layer-bar { position: absolute; left: 0; top: 0; bottom: 0; background: rgb(47 194 171 / 0.22); }`
- shared style `lab-doc-card` — src/lab/doctor/doctor.css:32: `.lab-doc-card { border: 1px solid var(--lab-line2); border-left-width: 4px; border-radius: 8px; background: var(--lab-bg2); padding: 8px 12px; margin: 6px 0; }`
- shared style `lab-doc-card` — src/lab/doctor/doctor.css:37: `.lab-doc-card header { display: flex; align-items: baseline; gap: 8px; flex-wrap: wrap; }`
- shared style `lab-doc-card` — src/lab/doctor/doctor.css:38: `.lab-doc-card header strong { font-size: 1.06em; }`
- shared style `lab-doc-sev` — src/lab/doctor/doctor.css:39: `.lab-doc-sev, .lab-doc-area { font-size: 0.82em; text-transform: uppercase; letter-spacing: 0.05em; padding: 1px 6px; border-radius: 4px; background: var(--lab-`
- shared style `lab-doc-card` — src/lab/doctor/doctor.css:42: `.lab-doc-card ul { margin: 6px 0; padding-left: 18px; }`
- shared style `lab-doc-card` — src/lab/doctor/doctor.css:43: `.lab-doc-card li { margin: 2px 0; font-variant-numeric: tabular-nums; }`
- shared style `lab-doc-why` — src/lab/doctor/doctor.css:44: `.lab-doc-why { margin: 4px 0 6px; color: var(--lab-muted); }`
- shared style `lab-doc-ab` — src/lab/doctor/doctor.css:45: `.lab-doc-ab { margin-top: 8px; padding: 6px 8px; border-radius: 6px; background: var(--lab-bg3); }`
- shared style `lab-doc-ab-row` — src/lab/doctor/doctor.css:46: `.lab-doc-ab-row { display: grid; grid-template-columns: 90px 160px 1fr; gap: 8px; font-variant-numeric: tabular-nums; }`
- shared style `lab-panel` — src/lab/lab.css:2: `.lab-panel { --lab-bg: #0f161a; --lab-bg2: #151f25; --lab-bg3: #1c2830; --lab-line: rgb(255 255 255 / 0.09); --lab-line2: rgb(255 255 255 / 0.16); --lab-text: #`
- shared style `lab-panel` — src/lab/lab.css:10: `.lab-panel[hidden] { display: none; }`
- shared style `lab-panel` — src/lab/lab.css:13: `.lab-panel *, .lab-panel *::before, .lab-panel *::after { box-sizing: border-box; }`
- shared style `lab-panel` — src/lab/lab.css:16: `.lab-panel * { scrollbar-width: thin; scrollbar-color: #3a4c55 transparent; }`
- shared style `lab-panel` — src/lab/lab.css:17: `.lab-panel ::-webkit-scrollbar { width: 10px; height: 10px; }`
- shared style `lab-panel` — src/lab/lab.css:18: `.lab-panel ::-webkit-scrollbar-track { background: transparent; }`
- shared style `lab-panel` — src/lab/lab.css:19: `.lab-panel ::-webkit-scrollbar-thumb { background: #33444c; border-radius: 6px; border: 2px solid var(--lab-bg); }`
- shared style `lab-panel` — src/lab/lab.css:20: `.lab-panel ::-webkit-scrollbar-thumb:hover { background: #4a6270; }`
- shared style `lab-panel` — src/lab/lab.css:21: `.lab-panel ::-webkit-scrollbar-corner { background: transparent; }`
- shared style `lab-head` — src/lab/lab.css:24: `.lab-head { display: flex; align-items: center; gap: 8px; padding: 6px 8px; background: var(--lab-bg2); border-bottom: 1px solid var(--lab-line); cursor: move; `
- shared style `lab-title` — src/lab/lab.css:25: `.lab-title { display: flex; align-items: center; gap: 6px; font-size: 1.08em; white-space: nowrap; color: var(--lab-accent); }`
- shared style `lab-title` — src/lab/lab.css:26: `.lab-title span { color: var(--lab-text); }`
- shared style `lab-meters` — src/lab/lab.css:27: `.lab-meters { display: flex; gap: 4px; flex: 1; min-width: 0; overflow-x: auto; padding: 1px; }`
- shared style `lab-tools` — src/lab/lab.css:28: `.lab-tools { display: flex; gap: 2px; flex: none; cursor: default; }`
- shared style `lab-icon` — src/lab/lab.css:34: `.lab-icon { flex: none; display: block; }`
- shared style `lab-meter` — src/lab/lab.css:36: `.lab-meter { --meter: var(--lab-accent); display: grid; grid-template-columns: auto auto; grid-template-rows: auto 4px 16px; column-gap: 6px; row-gap: 2px; alig`
- shared style `lab-meter` — src/lab/lab.css:38: `.lab-meter:hover { border-color: var(--meter); }`
- shared style `lab-meter` — src/lab/lab.css:39: `.lab-meter[data-level="warn"] { --meter: var(--lab-warn); }`
- shared style `lab-meter` — src/lab/lab.css:40: `.lab-meter[data-level="bad"] { --meter: var(--lab-bad); }`
- shared style `lab-meter` — src/lab/lab.css:41: `.lab-meter[data-level="none"] { --meter: #5b6b72; }`
- shared style `lab-meter-name` — src/lab/lab.css:42: `.lab-meter-name { font-size: 0.86em; color: var(--lab-muted); font-weight: 600; letter-spacing: 0.03em; }`
- shared style `lab-meter-value` — src/lab/lab.css:43: `.lab-meter-value { justify-self: end; font-weight: 700; font-variant-numeric: tabular-nums; color: var(--meter); }`
- shared style `lab-meter-bar` — src/lab/lab.css:44: `.lab-meter-bar { grid-column: 1 / 3; height: 4px; background: rgb(255 255 255 / 0.08); border-radius: 2px; overflow: hidden; }`
- shared style `lab-meter-fill` — src/lab/lab.css:45: `.lab-meter-fill { display: block; height: 100%; width: 0; background: var(--meter); transition: width 0.25s; }`
- shared style `lab-meter-spark` — src/lab/lab.css:46: `.lab-meter-spark { grid-column: 1 / 3; width: 100%; height: 16px; }`
- shared style `lab-panel` — src/lab/lab.css:49: `.lab-panel button { display: inline-flex; align-items: center; gap: 5px; background: rgb(255 255 255 / 0.05); color: inherit; border: 1px solid var(--lab-line2)`
- shared style `lab-panel` — src/lab/lab.css:51: `.lab-panel button:hover { background: rgb(255 255 255 / 0.1); border-color: rgb(255 255 255 / 0.26); }`
- shared style `lab-panel` — src/lab/lab.css:52: `.lab-panel button.on, .lab-panel button[aria-pressed="true"] { background: var(--lab-accent); color: var(--lab-accent-ink); border-color: var(--lab-accent); }`
- shared style `lab-panel` — src/lab/lab.css:53: `.lab-panel button:disabled { opacity: 0.45; cursor: default; }`
- shared style `lab-panel` — src/lab/lab.css:54: `.lab-panel button:focus-visible, .lab-panel input:focus-visible, .lab-panel select:focus-visible { outline: 2px solid var(--lab-accent); outline-offset: 1px; }`
- shared style `lab-icon-button` — src/lab/lab.css:55: `.lab-icon-button { padding: 4px !important; border-color: transparent !important; background: transparent !important; }`
- shared style `lab-icon-button` — src/lab/lab.css:56: `.lab-icon-button:hover { background: rgb(255 255 255 / 0.1) !important; }`
- shared style `lab-icon-button` — src/lab/lab.css:57: `.lab-icon-button.on { background: rgb(47 194 171 / 0.2) !important; color: var(--lab-accent) !important; }`
- shared style `lab-close` — src/lab/lab.css:58: `.lab-close:hover { background: rgb(255 111 97 / 0.25) !important; color: #fff; }`
- shared style `lab-panel` — src/lab/lab.css:59: `.lab-panel input, .lab-panel select, .lab-panel textarea { background: var(--lab-bg3); color: inherit; border: 1px solid var(--lab-line2); border-radius: 5px; p`
- shared style `lab-panel` — src/lab/lab.css:60: `.lab-panel input[type=checkbox], .lab-panel input[type=radio] { accent-color: var(--lab-accent); }`
- shared style `lab-panel` — src/lab/lab.css:61: `.lab-panel input[type=range] { accent-color: var(--lab-accent); padding: 0; }`
- shared style `lab-main` — src/lab/lab.css:64: `.lab-main { flex: 1 1 auto; min-height: 0; display: flex; flex-direction: row; }`
- shared style `lab-tabs` — src/lab/lab.css:65: `.lab-tabs { flex: none; display: flex; flex-direction: column; gap: 1px; width: 176px; padding: 6px; overflow-y: auto; overflow-x: hidden; border-right: 1px sol`
- shared style `lab-tab-group` — src/lab/lab.css:66: `.lab-tab-group { margin: 8px 6px 2px; font-size: 0.78em; font-weight: 700; letter-spacing: 0.07em; text-transform: uppercase; color: #6f838c; white-space: nowra`
- shared style `lab-tab-group` — src/lab/lab.css:67: `.lab-tab-group:first-child { margin-top: 2px; }`
- shared style `lab-tab` — src/lab/lab.css:68: `.lab-tab { justify-content: flex-start; width: 100%; border-color: transparent !important; background: transparent !important; color: var(--lab-muted) !importan`
- shared style `lab-tab` — src/lab/lab.css:69: `.lab-tab:hover { color: var(--lab-text) !important; background: rgb(255 255 255 / 0.06) !important; }`
- shared style `lab-tab` — src/lab/lab.css:70: `.lab-tab.on { color: var(--lab-accent) !important; background: rgb(47 194 171 / 0.14) !important; box-shadow: inset 2px 0 0 var(--lab-accent); }`
- shared style `lab-body` — src/lab/lab.css:77: `.lab-body { flex: 1 1 auto; min-height: 0; min-width: 0; overflow: auto; padding: 10px 12px 14px; }`
- shared style `lab-body` — src/lab/lab.css:78: `.lab-body h3 { margin: 14px 0 6px; font-size: 0.92em; color: #a9bcc4; text-transform: uppercase; letter-spacing: 0.06em; display: flex; align-items: center; gap`
- shared style `lab-body` — src/lab/lab.css:79: `.lab-body h3:first-child { margin-top: 2px; }`
- shared style `lab-muted` — src/lab/lab.css:80: `.lab-muted { color: var(--lab-muted); }`
- shared style `lab-row` — src/lab/lab.css:81: `.lab-row { display: flex; flex-wrap: wrap; align-items: center; gap: 6px; margin: 6px 0; }`
- shared style `lab-card` — src/lab/lab.css:82: `.lab-card { background: var(--lab-bg2); border: 1px solid var(--lab-line); border-radius: 8px; padding: 8px 10px; margin: 6px 0; }`
- shared style `lab-grid` — src/lab/lab.css:83: `.lab-grid { display: grid; grid-template-columns: repeat(auto-fill, minmax(150px, 1fr)); gap: 6px; }`
- shared style `lab-panel` — src/lab/lab.css:84: `.lab-panel pre, .lab-code { background: #0a1013; border: 1px solid var(--lab-line); border-radius: 6px; padding: 6px 8px; overflow: auto; max-height: 50vh; font`
- shared style `lab-good` — src/lab/lab.css:86: `.lab-good { color: var(--lab-good); }`
- shared style `lab-bad` — src/lab/lab.css:87: `.lab-bad { color: var(--lab-bad); }`
- shared style `lab-warn` — src/lab/lab.css:88: `.lab-warn { color: var(--lab-warn); }`
- shared style `lab-status` — src/lab/lab.css:91: `.lab-status { flex: none; display: flex; align-items: center; gap: 10px; padding: 4px 10px; border-top: 1px solid var(--lab-line); background: var(--lab-bg2); f`
- shared style `lab-status-text` — src/lab/lab.css:92: `.lab-status-text { flex: 1; min-width: 0; display: flex; gap: 10px; overflow: hidden; white-space: nowrap; text-overflow: ellipsis; }`
- shared style `lab-rec` — src/lab/lab.css:93: `.lab-rec { color: var(--lab-bad); font-weight: 700; animation: lab-blink 1s steps(2) infinite; }`
- shared style `lab-panel` — src/lab/lab.css:97: `.lab-panel[data-mode="mini"] .lab-main, .lab-panel[data-mode="mini"] .lab-status { display: none; }`
- shared style `lab-panel` — src/lab/lab.css:98: `.lab-panel[data-mode="mini"] .lab-head { border-bottom: 0; }`
- shared style `lab-grip` — src/lab/lab.css:101: `.lab-grip { position: absolute; z-index: 2; }`
- shared style `lab-grip-n` — src/lab/lab.css:102: `.lab-grip-n, .lab-grip-s { left: 10px; right: 10px; height: 6px; cursor: ns-resize; }`
- shared style `lab-grip-n` — src/lab/lab.css:103: `.lab-grip-n { top: -3px; }`
- shared style `lab-grip-s` — src/lab/lab.css:103: `.lab-grip-s { bottom: -3px; }`
- shared style `lab-grip-e` — src/lab/lab.css:104: `.lab-grip-e, .lab-grip-w { top: 10px; bottom: 10px; width: 6px; cursor: ew-resize; }`
- shared style `lab-grip-e` — src/lab/lab.css:105: `.lab-grip-e { right: -3px; }`
- shared style `lab-grip-w` — src/lab/lab.css:105: `.lab-grip-w { left: -3px; }`
- shared style `lab-grip-ne` — src/lab/lab.css:106: `.lab-grip-ne, .lab-grip-nw, .lab-grip-se, .lab-grip-sw { width: 12px; height: 12px; }`
- shared style `lab-grip-ne` — src/lab/lab.css:107: `.lab-grip-ne { top: -3px; right: -3px; cursor: nesw-resize; }`
- shared style `lab-grip-sw` — src/lab/lab.css:107: `.lab-grip-sw { bottom: -3px; left: -3px; cursor: nesw-resize; }`
- shared style `lab-grip-nw` — src/lab/lab.css:108: `.lab-grip-nw { top: -3px; left: -3px; cursor: nwse-resize; }`
- shared style `lab-grip-se` — src/lab/lab.css:108: `.lab-grip-se { bottom: -3px; right: -3px; cursor: nwse-resize; }`
- shared style `lab-grip` — src/lab/lab.css:109: `.lab-grip:hover { background: rgb(47 194 171 / 0.35); border-radius: 3px; }`
- shared style `lab-panel` — src/lab/lab.css:110: `.lab-panel[data-mode="mini"] .lab-grip, .lab-panel[data-mode="max"] .lab-grip { display: none; }`
- shared style `lab-table` — src/lab/lab.css:113: `.lab-table { margin: 4px 0 8px; }`
- shared style `lab-table-bar` — src/lab/lab.css:114: `.lab-table-bar { display: flex; flex-wrap: wrap; align-items: center; gap: 6px; margin: 4px 0; }`
- shared style `lab-table-bar` — src/lab/lab.css:115: `.lab-table-bar input[type=search] { flex: 1; min-width: 120px; }`
- shared style `lab-table-scroll` — src/lab/lab.css:116: `.lab-table-scroll { max-height: min(62vh, 720px); overflow: auto; border: 1px solid var(--lab-line); border-radius: 8px; resize: vertical; }`
- shared style `lab-table` — src/lab/lab.css:117: `.lab-table table { width: 100%; border-collapse: collapse; font-variant-numeric: tabular-nums; }`
- shared style `lab-table` — src/lab/lab.css:118: `.lab-table th { position: sticky; top: 0; z-index: 1; background: var(--lab-bg3); text-align: left; padding: 5px 7px; cursor: pointer; white-space: nowrap; font`
- shared style `lab-table` — src/lab/lab.css:119: `.lab-table th:hover { color: var(--lab-text); }`
- shared style `lab-table` — src/lab/lab.css:120: `.lab-table td { padding: 3px 7px; border-top: 1px solid rgb(255 255 255 / 0.04); max-width: 380px; overflow: hidden; text-overflow: ellipsis; white-space: nowra`
- shared style `lab-table` — src/lab/lab.css:123: `.lab-table tbody tr:nth-child(even) { background: rgb(255 255 255 / 0.018); }`
- shared style `lab-table` — src/lab/lab.css:124: `.lab-table tbody tr:hover { background: rgb(47 194 171 / 0.12); }`
- shared style `lab-table` — src/lab/lab.css:125: `.lab-table tbody tr[data-selected="true"] { background: rgb(47 194 171 / 0.22); }`
- shared style `lab-main` — src/lab/lab.css:132: `.lab-main { position: relative; }`
- shared style `lab-investigate` — src/lab/lab.css:133: `.lab-investigate { position: absolute; top: 0; right: 0; bottom: 0; width: min(560px, 62%); min-width: 320px; z-index: 5; overflow: auto; padding: 10px 12px 16p`
- shared style `lab-investigate` — src/lab/lab.css:135: `.lab-investigate[hidden] { display: none; }`
- shared style `lab-inv-head` — src/lab/lab.css:136: `.lab-inv-head { display: flex; align-items: center; gap: 6px; position: sticky; top: -10px; background: var(--lab-bg); padding: 2px 0 6px; z-index: 1; }`
- shared style `lab-inv-title` — src/lab/lab.css:137: `.lab-inv-title { display: flex; align-items: center; gap: 6px; flex: 1; min-width: 0; color: var(--lab-accent); }`
- shared style `lab-inv-title` — src/lab/lab.css:138: `.lab-inv-title strong { color: var(--lab-text); font-size: 1.1em; overflow-wrap: anywhere; }`
- shared style `lab-chip` — src/lab/lab.css:139: `.lab-chip { display: inline-block; padding: 1px 8px; border-radius: 10px; background: var(--lab-bg3); border: 1px solid var(--lab-line); color: var(--lab-muted)`
- shared style `lab-inv-section` — src/lab/lab.css:140: `.lab-inv-section { margin-top: 10px; }`
- shared style `lab-inv-section` — src/lab/lab.css:141: `.lab-inv-section > button { margin: 2px 4px 2px 0; }`
- shared style `lab-inv-fields` — src/lab/lab.css:142: `.lab-inv-fields { display: grid; grid-template-columns: max-content 1fr; gap: 2px 12px; margin: 6px 0; padding: 6px 8px; background: var(--lab-bg2); border-radi`
- shared style `lab-inv-fields` — src/lab/lab.css:143: `.lab-inv-fields dt { color: var(--lab-muted); }`
- shared style `lab-inv-fields` — src/lab/lab.css:144: `.lab-inv-fields dd { margin: 0; overflow-wrap: anywhere; font-variant-numeric: tabular-nums; }`
- shared style `lab-inv-hit` — src/lab/lab.css:145: `.lab-inv-hit pre { margin: 6px 0 0; max-height: 240px; }`
- shared style `lab-inv-line` — src/lab/lab.css:146: `.lab-inv-line { white-space: pre; }`
- shared style `lab-inv-line` — src/lab/lab.css:147: `.lab-inv-line.on { background: rgb(47 194 171 / 0.22); }`
- shared style `lab-inv-no` — src/lab/lab.css:148: `.lab-inv-no { color: #5d7079; user-select: none; }`
- shared style `lab-inv-table` — src/lab/lab.css:149: `.lab-inv-table { width: 100%; border-collapse: collapse; font-variant-numeric: tabular-nums; }`
- shared style `lab-inv-table` — src/lab/lab.css:150: `.lab-inv-table th { text-align: left; color: var(--lab-muted); font-weight: 600; padding: 3px 6px; }`
- shared style `lab-inv-table` — src/lab/lab.css:151: `.lab-inv-table td { padding: 2px 6px; border-top: 1px solid var(--lab-line); max-width: 260px; overflow: hidden; text-overflow: ellipsis; white-space: nowrap; }`
- shared style `lab-inv-table` — src/lab/lab.css:153: `.lab-inv-table tr:hover td { background: rgb(47 194 171 / 0.12); cursor: pointer; }`
- shared style `lab-table` — src/lab/lab.css:154: `.lab-table td.lab-look { width: 24px; padding: 0 2px; }`
- shared style `lab-table` — src/lab/lab.css:155: `.lab-table td.lab-look button { padding: 2px 3px; border: 0; background: transparent; color: var(--lab-muted); opacity: 0.55; }`
- shared style `lab-table` — src/lab/lab.css:156: `.lab-table tbody tr:hover td.lab-look button { opacity: 1; color: var(--lab-accent); }`
- shared style `lab-table` — src/lab/lab.css:157: `.lab-table tbody tr { cursor: pointer; }`
- shared style `lab-investigate` — src/lab/lab.css:158: `.lab-investigate h3 { display: flex; align-items: center; gap: 6px; margin: 12px 0 6px; font-size: 0.92em; color: #a9bcc4; text-transform: uppercase; letter-spa`
- shared style `lab-investigate` — src/lab/lab.css:159: `.lab-investigate p { margin: 4px 0; }`
- shared style `labres-kpis` — src/lab/resources/resources.css:2: `.labres-kpis { display: grid; grid-template-columns: repeat(auto-fill, minmax(230px, 1fr)); gap: 6px; margin: 6px 0; }`
- shared style `labres-kpi` — src/lab/resources/resources.css:3: `.labres-kpi { background: var(--lab-bg2); border: 1px solid var(--lab-line); border-radius: 8px; padding: 6px 8px; }`
- shared style `labres-kpi-head` — src/lab/resources/resources.css:4: `.labres-kpi-head { display: flex; justify-content: space-between; align-items: baseline; gap: 6px; }`
- shared style `labres-kpi-head` — src/lab/resources/resources.css:5: `.labres-kpi-head strong { font-size: 14px; font-variant-numeric: tabular-nums; color: var(--lab-text); }`
- shared style `labres-kpi-sub` — src/lab/resources/resources.css:6: `.labres-kpi-sub { font-size: 11px; font-variant-numeric: tabular-nums; color: var(--lab-muted); }`
- shared style `labres-spark` — src/lab/resources/resources.css:7: `.labres-spark { display: block; width: 100%; height: 46px; margin: 3px 0; }`
- shared style `labres-verdict` — src/lab/resources/resources.css:8: `.labres-verdict { margin: 6px 0; padding: 6px 10px; border-radius: 8px; background: var(--lab-bg2); border: 1px solid var(--lab-line); border-left: 3px solid va`
- shared style `labres-bound-GPU` — src/lab/resources/resources.css:9: `.labres-bound-GPU { border-left-color: var(--lab-bad); }`
- shared style `labres-bound-CPU` — src/lab/resources/resources.css:10: `.labres-bound-CPU { border-left-color: var(--lab-warn); }`
- shared style `labres-bound-vsync` — src/lab/resources/resources.css:11: `.labres-bound-vsync { border-left-color: var(--lab-good); }`
- shared style `labres-bound-idle` — src/lab/resources/resources.css:12: `.labres-bound-idle, .labres-bound-mixed { border-left-color: var(--lab-accent); }`
- shared style `labres-pct` — src/lab/resources/resources.css:13: `.labres-pct { border-collapse: collapse; font-variant-numeric: tabular-nums; margin-bottom: 4px; }`
- shared style `labres-pct` — src/lab/resources/resources.css:14: `.labres-pct th, .labres-pct td { padding: 2px 10px 2px 0; text-align: right; }`
- shared style `labres-pct` — src/lab/resources/resources.css:15: `.labres-pct td:first-child, .labres-pct th:first-child { text-align: left; color: var(--lab-muted); }`
- shared style `labres-warnings` — src/lab/resources/resources.css:16: `.labres-warnings { margin: 4px 0; padding-left: 18px; }`
- shared style `labres-settings` — src/lab/resources/resources.css:17: `.labres-settings { color: var(--lab-muted); }`
- shared style `grid` — src/peopleLab/peopleLab.css:61: `.grid { display: grid; grid-template-columns: repeat(3, 1fr); gap: 6px; }`
- shared style `big` — src/peopleLab/peopleLab.css:78: `.big { display: flex; align-items: center; justify-content: center; gap: 7px; width: 100%; border: 1px solid var(--line); border-radius: 7px; padding: 8px 10px;`
- shared style `big` — src/peopleLab/peopleLab.css:79: `.big:hover { background: var(--panel-3); }`
- shared style `big` — src/peopleLab/peopleLab.css:82: `.big:disabled { opacity: .4; cursor: default; }`
- shared style `fine` — src/peopleLab/peopleLab.css:84: `details.fine { margin: 2px 0 6px; }`
- shared style `fine` — src/peopleLab/peopleLab.css:85: `details.fine summary { cursor: pointer; font-size: 12px; color: var(--ink-3); padding: 6px 0; list-style: none; }`
- shared style `fine` — src/peopleLab/peopleLab.css:86: `details.fine summary::-webkit-details-marker { display: none; }`
- shared style `fine` — src/peopleLab/peopleLab.css:87: `details.fine summary::before { content: '+ '; }`
- shared style `fine` — src/peopleLab/peopleLab.css:88: `details.fine[open] summary::before { content: '\2013 '; }`
- shared style `fine` — src/peopleLab/peopleLab.css:89: `details.fine summary:hover { color: var(--ink); }`
- shared style `top` — src/peopleLab/peopleLab.css:121: `.top { position: absolute; left: 14px; right: 14px; top: 12px; display: flex; align-items: center; gap: 10px; pointer-events: none; }`
- shared style `top` — src/peopleLab/peopleLab.css:122: `.top > * { pointer-events: auto; }`
- shared style `tip` — src/peopleLab/peopleLab.css:132: `.tip { position: absolute; pointer-events: none; background: rgba(13, 15, 18, .92); border: 1px solid var(--line); color: var(--ink); font-size: 12px; line-heig`
- shared style `tip` — src/peopleLab/peopleLab.css:133: `.tip.on { display: block; }`
- shared style `tip` — src/peopleLab/peopleLab.css:134: `.tip b { font-weight: 600; }`
- shared style `glass` — src/ui/app.css:57: `.glass { background: linear-gradient(145deg, var(--ui-hi), transparent 42%), var(--ui-bg); border: 1px solid var(--ui-border); border-radius: 14px; box-shadow: `

## Where the program plugs the feature in

### src/editor/cloudTool.ts

- `src/editor/cloudTool.ts#CloudTool` (class, line 25) ← construct from `src/main.ts#cloudTool`
- `src/editor/cloudTool.ts#CloudTool.down` (method, line 38) ← call from `src/main.ts#pointerDown`
- `src/editor/cloudTool.ts#CloudTool.gesture` (method, line 34) ← call from `src/main.ts#currentGesture`
- `src/editor/cloudTool.ts#CloudTool.move` (method, line 74) ← call from `src/main.ts#pointerMove`
- `src/editor/cloudTool.ts#CloudTool.pointer` (getter, line 30) ← call from `src/main.ts#pointerMove`
- `src/editor/cloudTool.ts#CloudTool.up` (method, line 85) ← call from `src/main.ts#endPointer`

### src/editor/streetscapeTool.ts

- `src/editor/streetscapeTool.ts#StreetscapeTool` (class, line 28) ← construct from `src/main.ts#streetscapeTool`
- `src/editor/streetscapeTool.ts#StreetscapeTool.down` (method, line 42) ← call from `src/main.ts#pointerDown`
- `src/editor/streetscapeTool.ts#StreetscapeTool.hover` (field, line 30) ← read from `src/main.ts#drawStreetscapeHover`
- `src/editor/streetscapeTool.ts#StreetscapeTool.move` (method, line 87) ← call from `src/main.ts#pointerMove`
- `src/editor/streetscapeTool.ts#StreetscapeTool.reach` (method, line 37) ← call from `src/main.ts#bulldozer`
- `src/editor/streetscapeTool.ts#StreetscapeTool.row` (field, line 32) ← read from `src/main.ts#drawStreetscapeHover`
- `src/editor/streetscapeTool.ts#StreetscapeTool.row` (field, line 32) ← write from `src/main.ts#endPointer`
- `src/editor/streetscapeTool.ts#StreetscapeTool.up` (method, line 100) ← call from `src/main.ts#endPointer`

### src/editor/terrainBrush.ts

- `src/editor/terrainBrush.ts#DabSettings` (interface, line 28) ← typeRef from `src/main.ts#dabSettings`
- `src/editor/terrainBrush.ts#TERRAIN_MIN_MS` (variable, line 15) ← read from `src/main.ts#terrainBrush`
- `src/editor/terrainBrush.ts#TerrainBrush` (class, line 170) ← construct from `src/main.ts#terrainBrush`
- `src/editor/terrainBrush.ts#TerrainBrush.at` (getter, line 187) ← call from `src/main.ts#drawOverlayScreen`
- `src/editor/terrainBrush.ts#TerrainBrush.begin` (method, line 196) ← call from `src/main.ts#pointerDown`
- `src/editor/terrainBrush.ts#TerrainBrush.end` (method, line 255) ← call from `src/main.ts#endTerrainStroke`
- `src/editor/terrainBrush.ts#TerrainBrush.level` (getter, line 191) ← call from `src/main.ts#drawOverlayScreen`
- `src/editor/terrainBrush.ts#TerrainBrush.paint` (method, line 222) ← call from `src/main.ts#pointerMove`
- `src/editor/terrainBrush.ts#TerrainBrush.pointer` (getter, line 178) ← call from `src/main.ts#endPointer`, `src/main.ts#pointerMove`
- `src/editor/terrainBrush.ts#TerrainBrush.stroking` (getter, line 182) ← call from `src/main.ts#currentGesture`, `src/main.ts#frame`, `src/main.ts#gestureInProgress`, `src/main.ts#pointerMove`
- `src/editor/terrainBrush.ts#TerrainBrush.tick` (method, line 209) ← call from `src/main.ts#frame`

### src/render/bake/detailRecipes.ts

- `src/render/bake/detailRecipes.ts#bake` (function, line 91) ← indirectCall from `src/render/mesh/textureBaker.ts#fbm`

### src/render/bake/terrainRecipes.ts

- `src/render/bake/terrainRecipes.ts#streakNoise` (function, line 220) ← indirectCall from `src/render/mesh/textureBaker.ts#fbm`
- `src/render/bake/terrainRecipes.ts#terrainRecipes` (function, line 237) ← indirectCall from `src/render/mesh/textureBaker.ts#fbm`

### src/render/canyonLight.ts

- `src/render/canyonLight.ts#CanyonLight` (interface, line 37) ← call from `src/render/postprocess.ts#createPostChain`, `src/render/postprocess.ts#createPostChain.disposeAll`, `src/render/postprocess.ts#createPostChain.followTown`, `src/render/postprocess.ts#createPostChain.setAtmosphere`
- `src/render/canyonLight.ts#CanyonLight` (interface, line 37) ← read from `src/render/postprocess.ts#createPostChain`, `src/render/postprocess.ts#createPostChain.render`
- `src/render/canyonLight.ts#createCanyonLight` (function, line 198) ← call from `src/render/postprocess.ts#createPostChain`
- `src/render/canyonLight.ts#createCanyonLight.dispose` (method, line 372) ← implementation from `src/render/postprocess.ts#createPostChain.disposeAll`
- `src/render/canyonLight.ts#createCanyonLight.follow` (method, line 325) ← implementation from `src/render/postprocess.ts#createPostChain.followTown`
- `src/render/canyonLight.ts#createCanyonLight.setOcclusion` (method, line 361) ← implementation from `src/render/postprocess.ts#createPostChain`
- `src/render/canyonLight.ts#createCanyonLight.setSun` (method, line 366) ← implementation from `src/render/postprocess.ts#createPostChain.setAtmosphere`

### src/render/cloudShadowStamp.ts

- `src/render/cloudShadowStamp.ts#CloudShadowStamp` (class, line 14) ← construct from `src/render/postprocess.ts#createPostChain`
- `src/render/cloudShadowStamp.ts#CloudShadowStamp.changed` (method, line 20) ← call from `src/render/postprocess.ts#createPostChain`
- `src/render/cloudShadowStamp.ts#CloudShadowStamp.invalidate` (method, line 51) ← call from `src/render/postprocess.ts#createPostChain.mapLost`

### src/render/ecology.worker.ts

- `src/render/ecology.worker.ts#serve` (function, line 8) ← callbackAssign from `src/bays.worker.ts#serve`, `src/cityGen.worker.ts#serve`, `src/conflictTable.worker.ts#serve`, `src/editor/save.worker.ts#serve`, …
- `src/render/ecology.worker.ts#serve` (function, line 8) ← moduleLoad from `src/job.worker.ts#LOADERS`

### src/render/ecologyPool.ts

- `src/render/ecologyPool.ts#onEcologyRead` (function, line 91) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/ecologyPool.ts#readFarWoods` (function, line 97) ← reference from `src/render/flatAtlas.ts#createFlatAtlas`

### src/render/elements.ts

- `src/render/elements.ts#createElementLayer` (function, line 285) ← call from `src/render/renderer.ts#createSceneRenderer.elementLayerOf`
- `src/render/elements.ts#createElementLayer.cull` (method, line 342) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/elements.ts#createElementLayer.dispose` (method, line 350) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/elements.ts#createElementLayer.emit` (method, line 343) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/elements.ts#createElementLayer.sync` (method, line 323) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/elements.ts#ElementKit` (interface, line 168) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/elements.ts#ElementKit` (interface, line 168) ← typeRef from `src/render/renderer.ts#createSceneRenderer`
- `src/render/elements.ts#ElementLayer` (interface, line 235) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/elements.ts#ElementLayer` (interface, line 235) ← read from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.elementLayerOf`, `src/render/renderer.ts#createSceneRenderer.switchedOff`
- `src/render/elements.ts#ElementLayer` (interface, line 235) ← typeRef from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.elementLayerOf`
- `src/render/elements.ts#loadElementKit` (function, line 174) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/elements.ts#loadElementKit.dispose` (method, line 220) ← implementation from `src/render/renderer.ts#createSceneRenderer`

### src/render/environment.ts

- `src/render/environment.ts#createEnvironment` (function, line 118) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/environment.ts#createEnvironment.directShare` (method, line 286) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/environment.ts#createEnvironment.dispose` (method, line 302) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/environment.ts#createEnvironment.follow` (method, line 218) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/environment.ts#createEnvironment.remakeShadowMaps` (method, line 298) ← implementation from `src/render/optimizationsApply.ts#createOptimizer.set`
- `src/render/environment.ts#createEnvironment.setCasters` (method, line 226) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/environment.ts#createEnvironment.setFarCascadeEvery` (method, line 295) ← implementation from `src/render/optimizationsApply.ts#createOptimizer`, `src/render/optimizationsApply.ts#createOptimizer.set`
- `src/render/environment.ts#createEnvironment.setMovers` (method, line 225) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/environment.ts#createEnvironment.setQuality` (method, line 299) ← implementation from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.applyQuality`
- `src/render/environment.ts#createEnvironment.setSmog` (method, line 293) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/environment.ts#createEnvironment.setTimeOfDay` (method, line 228) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/environment.ts#createEnvironment.setWeather` (method, line 294) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/environment.ts#createEnvironment.shadowCover` (method, line 296) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/environment.ts#createEnvironment.shadowMaps` (method, line 227) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/environment.ts#createEnvironment.shadowNearTexel` (method, line 297) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/environment.ts#createEnvironment.shadowPass` (method, line 223) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/environment.ts#createEnvironment.shadowStats` (method, line 224) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/environment.ts#SceneEnvironment` (interface, line 36) ← call from `src/render/optimizationsApply.ts#createOptimizer`, `src/render/optimizationsApply.ts#createOptimizer.set`, `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.applyQuality`
- `src/render/environment.ts#SceneEnvironment` (interface, line 36) ← read from `src/render/renderer.ts#createSceneRenderer`
- `src/render/environment.ts#SceneEnvironment` (interface, line 36) ← typeRef from `src/render/optimizationsApply.ts#OptimizerDeps`
- `src/render/environment.ts#sunShadowChunks` (function, line 318) ← call from `src/render/renderer.ts#createSceneRenderer`

### src/render/farForest.ts

- `src/render/farForest.ts#createFarForest` (function, line 201) ← call from `src/render/flatAtlas.ts#createFlatAtlas`
- `src/render/farForest.ts#createFarForest.dispose` (method, line 351) ← implementation from `src/render/flatAtlas.ts#createFlatAtlas`
- `src/render/farForest.ts#createFarForest.update` (method, line 320) ← implementation from `src/render/flatAtlas.ts#createFlatAtlas`
- `src/render/farForest.ts#FarForest` (interface, line 189) ← call from `src/render/flatAtlas.ts#createFlatAtlas`
- `src/render/farForest.ts#FarForest` (interface, line 189) ← read from `src/render/flatAtlas.ts#createFlatAtlas`, `src/render/flatAtlas.ts#createFlatAtlas.followFar`, `src/render/flatAtlas.ts#createFlatAtlas.sync`
- `src/render/farForest.ts#FarForest` (interface, line 189) ← write from `src/render/flatAtlas.ts#createFlatAtlas.followFar`
- `src/render/farForest.ts#FarWoodsBook.add` (method, line 82) ← call from `src/render/flatAtlas.ts#createFlatAtlas.followFar`
- `src/render/farForest.ts#FarWoodsBook.drop` (method, line 90) ← call from `src/render/flatAtlas.ts#createFlatAtlas.followFar`
- `src/render/farForest.ts#FarWoodsBook.setFull` (method, line 157) ← call from `src/render/flatAtlas.ts#createFlatAtlas`
- `src/render/farForest.ts#FarWoodsBook.setVegetation` (method, line 137) ← call from `src/render/flatAtlas.ts#createFlatAtlas`
- `src/render/farForest.ts#FarWoodsBook.sync` (method, line 102) ← call from `src/render/flatAtlas.ts#createFlatAtlas.sync`

### src/render/farGround.ts

- `src/render/farGround.ts#FarNode` (interface, line 15) ← read from `src/render/flatAtlas.ts#createFlatAtlas`, `src/render/flatAtlas.ts#createFlatAtlas.drawFar`, `src/render/flatAtlas.ts#createFlatAtlas.followFar`, `src/render/flatAtlas.ts#createFlatAtlas.writeFar`
- `src/render/farGround.ts#FarNode` (interface, line 15) ← typeRef from `src/render/flatAtlas.ts#FarResident`
- `src/render/farGround.ts#farNodesFor` (function, line 39) ← call from `src/render/flatAtlas.ts#createFlatAtlas.followFar`

### src/render/fogLayer.ts

- `src/render/fogLayer.ts#createFogField` (function, line 52) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/fogLayer.ts#createFogField.follow` (method, line 195) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/fogLayer.ts#createFogField.update` (method, line 185) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/fogLayer.ts#createFogField.whole` (function, line 174) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/fogLayer.ts#createFogTexture` (function, line 28) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/fogLayer.ts#FogField` (interface, line 43) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/fogLayer.ts#FogLayer` (interface, line 17) ← read from `src/render/renderer.ts#createSceneRenderer`
- `src/render/fogLayer.ts#FogLayer` (interface, line 17) ← typeRef from `src/render/renderer.ts#createSceneRenderer`

### src/render/grass.ts

- `src/render/grass.ts#Clump` (interface, line 32) ← read from `src/render/streetFurnitureLayer.ts#meadowInto`
- `src/render/grass.ts#CLUMP_READ_REACH` (variable, line 19) ← read from `src/render/streetFurnitureLayer.ts#meadowProduct`
- `src/render/grass.ts#grassClump` (function, line 41) ← call from `src/render/streetFurnitureLayer.ts#meadowInto`
- `src/render/grass.ts#Tuft` (interface, line 21) ← read from `src/render/streetFurnitureLayer.ts#meadowInto.add`
- `src/render/grass.ts#Tuft` (interface, line 21) ← typeRef from `src/render/streetFurnitureLayer.ts#meadowInto.add`

### src/render/groundChanges.ts

- `src/render/groundChanges.ts#GroundChanges` (class, line 17) ← construct from `src/render/flatAtlas.ts#createFlatAtlas`
- `src/render/groundChanges.ts#GroundChanges.mark` (method, line 29) ← call from `src/render/flatAtlas.ts#createFlatAtlas`
- `src/render/groundChanges.ts#GroundChanges.rectsSince` (method, line 56) ← call from `src/render/flatAtlas.ts#createFlatAtlas`, `src/render/renderer.ts#createSceneRenderer`
- `src/render/groundChanges.ts#GroundChanges.version` (getter, line 24) ← call from `src/render/flatAtlas.ts#createFlatAtlas`, `src/render/renderer.ts#createSceneRenderer`
- `src/render/groundChanges.ts#GroundRecord` (interface, line 10) ← read from `src/render/renderer.ts#createSceneRenderer`
- `src/render/groundChanges.ts#GroundRecord` (interface, line 10) ← typeRef from `src/render/renderer.ts#createSceneRenderer`
- `src/render/groundChanges.ts#Rect` (type, line 4) ← typeRef from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.blocksOver`, `src/render/renderer.ts#createSceneRenderer.farCellRect`, `src/render/renderer.ts#createSceneRenderer.landMovedSince`, …
- `src/render/groundChanges.ts#unionRect` (function, line 77) ← call from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.landMovedSince`

### src/render/groundCover.ts

- `src/render/groundCover.ts#COVER_POSE` (variable, line 283) ← implementation from `src/render/cellInstances.ts#createCellInstances.append`, `src/render/cellInstances.ts#drawnMatrices`
- `src/render/groundCover.ts#createGroundCoverKit` (function, line 172) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/groundCover.ts#createGroundCoverKit.dispose` (method, line 199) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/groundCover.ts#FOREST_SPECIES` (variable, line 35) ← read from `src/render/renderer.ts#createSceneRenderer.startNatureTrees`
- `src/render/groundCover.ts#ForestSpecies` (type, line 34) ← typeRef from `src/render/coverSweep.ts#Candidate`
- `src/render/groundCover.ts#GroundCoverKit` (interface, line 48) ← call from `src/render/renderer.ts#createSceneRenderer`

### src/render/groundShape.ts

- `src/render/groundShape.ts#blockDigest` (function, line 157) ← call from `src/render/renderer.ts#createSceneRenderer.blockRead`
- `src/render/groundShape.ts#BlockDigestAsk` (interface, line 187) ← typeRef from `src/render/groundShapePool.ts#GroundShapePool.digest`, `src/render/groundShapePool.ts#GroundWorkers`, `src/render/renderer.ts#createSceneRenderer.compareSolves`
- `src/render/groundShape.ts#cornerBoxes` (function, line 77) ← call from `src/render/renderer.ts#createSceneRenderer.shapeGround`
- `src/render/groundShape.ts#GroundJob` (type, line 253) ← typeRef from `src/render/groundShape.worker.ts#serve`, `src/render/groundShapePool.ts#GroundShapePool.run`
- `src/render/groundShape.ts#GroundJobAnswer` (type, line 255) ← typeRef from `src/render/groundShapePool.ts#GroundShapePool.run`, `src/render/groundShapePool.ts#GroundShapePool.start`, `src/render/groundShapePool.ts#Lane`
- `src/render/groundShape.ts#GroundShapeAsk` (interface, line 31) ← typeRef from `src/render/groundShapePool.ts#GroundShapePool.shape`, `src/render/groundShapePool.ts#GroundWorkers`, `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.shapeGround`, …
- `src/render/groundShape.ts#NaturalPlate` (interface, line 174) ← typeRef from `src/render/flatAtlas.ts#createFlatAtlas`
- `src/render/groundShape.ts#PadsAsk` (interface, line 224) ← typeRef from `src/render/groundShapePool.ts#GroundShapePool.pads`, `src/render/groundShapePool.ts#GroundWorkers`, `src/render/renderer.ts#createSceneRenderer.padsAskOf`
- `src/render/groundShape.ts#PlateShapeAnswer` (interface, line 38) ← typeRef from `src/render/groundShapePool.ts#GroundShapePool.shape`, `src/render/groundShapePool.ts#GroundWorkers`, `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#Shaping`
- `src/render/groundShape.ts#runGroundJob` (function, line 258) ← call from `src/render/groundShape.worker.ts#serve`
- `src/render/groundShape.ts#shapeGroundAsk` (function, line 107) ← call from `src/render/renderer.ts#createSceneRenderer.landShapes`
- `src/render/groundShape.ts#ShapeScratch` (class, line 93) ← construct from `src/render/groundShape.worker.ts#serve`

### src/render/groundShear.ts

- `src/render/groundShear.ts#Gradient` (interface, line 4) ← typeRef from `src/render/riggedCitizens.ts#createRiggedCitizens.draw`, `src/render/riggedCitizens.ts#createRiggedCitizens.emit`
- `src/render/groundShear.ts#groundGradient` (function, line 13) ← call from `src/render/agents.ts#createAgentMeshes.sync`
- `src/render/groundShear.ts#shearMatrix` (function, line 24) ← call from `src/render/riggedCitizens.ts#createRiggedCitizens.emit`

### src/render/lightLevels.ts

- `src/render/lightLevels.ts#lightPoolTexture` (function, line 39) ← call from `src/render/buildings/kit.ts#createBuildingKit`
- `src/render/lightLevels.ts#luminanceOf` (function, line 28) ← call from `src/render/buildings/kit.ts#createBuildingKit.setNight`
- `src/render/lightLevels.ts#NIGHT_LUMINANCE` (variable, line 9) ← read from `src/render/agents.ts#LAMP_GAIN`, `src/render/buildings/kit.ts#createBuildingKit.setNight`
- `src/render/lightLevels.ts#nightGain` (function, line 34) ← call from `src/render/agents.ts#LAMP_GAIN`

### src/render/lightning.ts

- `src/render/lightning.ts#createLightning` (function, line 65) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/lightning.ts#createLightning.dispose` (method, line 127) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/lightning.ts#createLightning.strike` (method, line 100) ← implementation from `src/render/renderer.ts#createSceneRenderer.strikeAt`
- `src/render/lightning.ts#createLightning.update` (method, line 107) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/lightning.ts#Lightning` (interface, line 12) ← call from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.strikeAt`
- `src/render/lightning.ts#Lightning` (interface, line 12) ← read from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.switchedOff`

### src/render/lowPolyTrees.ts

- `src/render/lowPolyTrees.ts#LowPolyKind` (type, line 31) ← typeRef from `src/render/propGeometry.ts#TREE_KIND`

### src/render/mesh/detailLayer.ts

- `src/render/mesh/detailLayer.ts#applyDetail` (function, line 69) ← call from `src/render/materials.ts#createMaterials`
- `src/render/mesh/detailLayer.ts#detailSwitch` (variable, line 26) ← read from `src/render/materials.ts#createMaterials.setDetail`
- `src/render/mesh/detailLayer.ts#detailSwitch` (variable, line 26) ← write from `src/render/materials.ts#createMaterials.setDetail`

### src/render/natureTrees.ts

- `src/render/natureTrees.ts#buildNatureForest` (function, line 232) ← call from `src/render/renderer.ts#createSceneRenderer.startNatureTrees`
- `src/render/natureTrees.ts#buildNatureForest.dispose` (method, line 327) ← implementation from `src/render/renderer.ts#createSceneRenderer.startNatureTrees`
- `src/render/natureTrees.ts#loadNatureTrees` (function, line 88) ← call from `src/render/renderer.ts#createSceneRenderer.startNatureTrees`
- `src/render/natureTrees.ts#loadNatureTrees.dispose` (method, line 108) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/natureTrees.ts#NatureForest` (interface, line 119) ← call from `src/render/renderer.ts#createSceneRenderer.startNatureTrees`
- `src/render/natureTrees.ts#NatureForest` (interface, line 119) ← read from `src/render/renderer.ts#createSceneRenderer.startNatureTrees`
- `src/render/natureTrees.ts#NatureTreeKit` (interface, line 43) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/natureTrees.ts#NatureTreeKit` (interface, line 43) ← typeRef from `src/render/renderer.ts#createSceneRenderer`
- `src/render/natureTrees.ts#seedOfKind` (function, line 23) ← call from `src/render/coverSweep.ts#forestIn`, `src/render/renderer.ts#createSceneRenderer.natureSweep`
- `src/render/natureTrees.ts#TREE_POSE` (variable, line 199) ← implementation from `src/render/cellInstances.ts#createCellInstances.append`, `src/render/cellInstances.ts#drawnMatrices`

### src/render/plantCells.ts

- `src/render/plantCells.ts#createCoverCells` (function, line 125) ← call from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.coverCellsOf`
- `src/render/plantCells.ts#createTreeCells` (function, line 80) ← call from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.paintedForestOf`, `src/render/renderer.ts#createSceneRenderer.plantedForestOf`
- `src/render/plantCells.ts#PlantCells` (interface, line 15) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/plantCells.ts#PlantCells` (interface, line 15) ← read from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.coverCellsOf`, `src/render/renderer.ts#createSceneRenderer.paintedForestOf`, `src/render/renderer.ts#createSceneRenderer.plantedForestOf`, …
- `src/render/plantCells.ts#PlantCells` (interface, line 15) ← typeRef from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.coverCellsOf`, `src/render/renderer.ts#createSceneRenderer.paintedForestOf`, `src/render/renderer.ts#createSceneRenderer.plantedForestOf`
- `src/render/plantCells.ts#plantLayer.cull` (method, line 60) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/plantCells.ts#plantLayer.dispose` (method, line 62) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/plantCells.ts#plantLayer.sync` (method, line 33) ← implementation from `src/render/renderer.ts#createSceneRenderer`

### src/render/plantImpostors.ts

- `src/render/plantImpostors.ts#createPlantImpostors.attach` (method, line 444) ← implementation from `src/render/gardens.ts#gardenKinds.kind`, `src/render/propCells.ts#createPropLayer.batches`, `src/render/streetFurnitureLayer.ts#furnitureStores.plant`
- `src/render/plantImpostors.ts#createPlantImpostors.ready` (method, line 443) ← implementation from `src/render/gardens.ts#gardenKinds.kind`, `src/render/propCells.ts#createPropLayer.show`, `src/render/streetFurnitureLayer.ts#furnitureStores.plant`
- `src/render/plantImpostors.ts#createPlantImpostors.setView` (method, line 439) ← implementation from `src/render/gardens.ts#gardenKinds.kind`, `src/render/propCells.ts#createPropLayer.cull`, `src/render/streetFurnitureLayer.ts#furnitureStores.plant`
- `src/render/plantImpostors.ts#impostorPick` (function, line 50) ← call from `src/render/cellInstances.ts#createCellInstances.append`
- `src/render/plantImpostors.ts#PICTURE_PX` (variable, line 467) ← read from `src/render/gardens.ts#gardenKinds.kind`, `src/render/propCells.ts#nearSquareOf`, `src/render/streetFurnitureLayer.ts#furnitureStores.plant`
- `src/render/plantImpostors.ts#PlantImpostors` (interface, line 255) ← call from `src/render/gardens.ts#gardenKinds.kind`, `src/render/propCells.ts#createPropLayer.cull`, `src/render/propCells.ts#createPropLayer.show`, `src/render/streetFurnitureLayer.ts#furnitureStores.plant`
- `src/render/plantImpostors.ts#PlantImpostors` (interface, line 255) ← read from `src/render/gardens.ts#gardenKinds.kind`, `src/render/propCells.ts#createPropLayer.batches`, `src/render/propCells.ts#createPropLayer.note`, `src/render/streetFurnitureLayer.ts#furnitureStores.plant`
- `src/render/plantImpostors.ts#PlantImpostors` (interface, line 255) ← reference from `src/render/gardens.ts#gardenKinds.kind`, `src/render/propCells.ts#createPropLayer.batches`, `src/render/streetFurnitureLayer.ts#furnitureStores.plant`
- `src/render/plantImpostors.ts#PlantImpostors` (interface, line 255) ← typeRef from `src/render/propCells.ts#PropLayerSpec`
- `src/render/plantImpostors.ts#sceneryPictures` (function, line 487) ← call from `src/render/gardens.ts#createGardenCells`, `src/render/streetFurnitureLayer.ts#createStreetFurniture`
- `src/render/plantImpostors.ts#SceneryPictures` (interface, line 470) ← call from `src/render/gardens.ts#createGardenCells.dispose`, `src/render/gardens.ts#gardenKinds`, `src/render/gardens.ts#gardenPictures`, `src/render/streetFurnitureLayer.ts#createStreetFurniture`, …
- `src/render/plantImpostors.ts#SceneryPictures` (interface, line 470) ← read from `src/render/gardens.ts#createGardenCells`, `src/render/gardens.ts#gardenKinds`, `src/render/streetFurnitureLayer.ts#createStreetFurniture`, `src/render/streetFurnitureLayer.ts#furnitureStores`
- `src/render/plantImpostors.ts#SceneryPictures` (interface, line 470) ← typeRef from `src/render/gardens.ts#gardenKinds`, `src/render/gardens.ts#gardenPictures`, `src/render/streetFurnitureLayer.ts#furnitureStores`
- `src/render/plantImpostors.ts#sceneryPictures.bushAt` (method, line 502) ← implementation from `src/render/gardens.ts#gardenKinds`, `src/render/gardens.ts#gardenPictures`, `src/render/streetFurnitureLayer.ts#createStreetFurniture`, `src/render/streetFurnitureLayer.ts#furnitureStores`
- `src/render/plantImpostors.ts#sceneryPictures.release` (method, line 503) ← implementation from `src/render/gardens.ts#createGardenCells.dispose`, `src/render/streetFurnitureLayer.ts#createStreetFurniture.dispose`
- `src/render/plantImpostors.ts#sceneryPictures.treeAt` (method, line 501) ← implementation from `src/render/gardens.ts#gardenKinds`, `src/render/gardens.ts#gardenPictures`, `src/render/streetFurnitureLayer.ts#createStreetFurniture`, `src/render/streetFurnitureLayer.ts#furnitureStores`
- `src/render/plantImpostors.ts#thinWant` (function, line 64) ← call from `src/render/cellInstances.ts#createCellInstances.visit`

### src/render/rain.ts

- `src/render/rain.ts#createRain` (function, line 84) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/rain.ts#createRain.dispose` (method, line 168) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/rain.ts#createRain.update` (method, line 160) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/rain.ts#Rain` (interface, line 10) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/rain.ts#Rain` (interface, line 10) ← read from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.switchedOff`

### src/render/scenery.ts

- `src/render/scenery.ts#createSceneryKit` (function, line 175) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/scenery.ts#createSceneryKit.dispose` (method, line 307) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/scenery.ts#createSceneryKit.setHaze` (method, line 303) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/scenery.ts#createSceneryKit.setNight` (method, line 293) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/scenery.ts#foliageTint` (function, line 373) ← call from `src/render/gardens.ts#gardenPack`, `src/render/streetFurnitureLayer.ts#furniturePlace`
- `src/render/scenery.ts#LAMP_BEAM_RADIUS` (variable, line 66) ← read from `src/render/streetFurnitureLayer.ts#furniturePlace`, `src/render/utilities.ts#placePole`
- `src/render/scenery.ts#LAMP_POOL_RADIUS` (variable, line 61) ← read from `src/render/streetFurnitureLayer.ts#furniturePlace`, `src/render/utilities.ts#placePole`
- `src/render/scenery.ts#PLANT_MAP_ZOOM` (variable, line 370) ← read from `src/render/renderer.ts#createSceneRenderer`
- `src/render/scenery.ts#plantSamples` (function, line 315) ← call from `src/render/renderer.ts#createSceneRenderer.warmPreviewShaders`
- `src/render/scenery.ts#plantSamples` (function, line 315) ← implementation from `src/render/renderer.ts#createSceneRenderer.warmPreviewShaders`
- `src/render/scenery.ts#plantSamples` (function, line 315) ← read from `src/render/renderer.ts#createSceneRenderer.warmPreviewShaders`
- `src/render/scenery.ts#plantSamples.dispose` (method, line 343) ← implementation from `src/render/renderer.ts#createSceneRenderer.warmPreviewShaders`
- `src/render/scenery.ts#POOL_ALONG` (variable, line 63) ← read from `src/render/streetFurnitureLayer.ts#furniturePlace`
- `src/render/scenery.ts#SceneryKit` (interface, line 131) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/scenery.ts#SceneryKit` (interface, line 131) ← read from `src/render/gardens.ts#gardenKinds`, `src/render/streetFurnitureLayer.ts#createStreetFurniture`, `src/render/streetFurnitureLayer.ts#furnitureStores`, `src/render/streetFurnitureLayer.ts#MEADOW_STORES`, …
- `src/render/scenery.ts#SceneryKit` (interface, line 131) ← typeRef from `src/render/gardens.ts#createGardenCells`, `src/render/gardens.ts#gardenKinds`, `src/render/poleCells.ts#PoleCells.constructor`, `src/render/poleLayer.ts#buildPolePreview`, …
- `src/render/scenery.ts#SHADOW_REACH` (variable, line 363) ← read from `src/render/gardens.ts#createGardenCells`, `src/render/streetFurnitureLayer.ts#createStreetFurniture`

### src/render/stillTree.ts

- `src/render/stillTree.ts#holdStill` (function, line 13) ← call from `src/render/renderer.ts#createSceneRenderer`, `src/render/signals.ts#createSignalHeads`
- `src/render/stillTree.ts#updateMoving` (function, line 35) ← call from `src/render/renderer.ts#createSceneRenderer`

### src/render/sunCascades.ts

- `src/render/sunCascades.ts#Movers` (interface, line 94) ← typeRef from `src/render/renderer.ts#createSceneRenderer`
- `src/render/sunCascades.ts#SunCascades` (interface, line 123) ← typeRef from `src/render/renderer.ts#SceneHandle`

### src/render/terrain.ts

- `src/render/terrain.ts#createTerrainSurface` (function, line 1330) ← call from `src/render/flatAtlas.ts#createFlatAtlas.ensure`
- `src/render/terrain.ts#createTerrainSurface` (function, line 1330) ← callbackAssign from `src/render/flatAtlas.ts#createFlatAtlas.ensure`, `src/render/renderer.ts#createSceneRenderer`
- `src/render/terrain.ts#createTerrainSurface` (function, line 1330) ← implementation from `src/render/flatAtlas.ts#createFlatAtlas`, `src/render/flatAtlas.ts#createFlatAtlas.ensure`, `src/render/flatAtlas.ts#createFlatAtlas.keepWithinBudget`, `src/render/flatAtlas.ts#createFlatAtlas.letGo`, …
- `src/render/terrain.ts#createTerrainSurface.coverAt` (function, line 2150) ← implementation from `src/render/flatAtlas.ts#createFlatAtlas`, `src/render/renderer.ts#createSceneRenderer`
- `src/render/terrain.ts#createTerrainSurface.naturalRenderedHeightAt` (function, line 1592) ← implementation from `src/render/flatAtlas.ts#createFlatAtlas`, `src/render/renderer.ts#createSceneRenderer.blockRead`, `src/render/renderer.ts#createSceneRenderer.builtGroundAt`
- `src/render/terrain.ts#createTerrainSurface.renderedHeightAt` (function, line 1591) ← implementation from `src/render/flatAtlas.ts#createFlatAtlas`, `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.buildWorld`, `src/render/renderer.ts#createSceneRenderer.coverPlain`, …
- `src/render/terrain.ts#createTerrainSurface.setSun` (function, line 2126) ← implementation from `src/render/flatAtlas.ts#createFlatAtlas`, `src/render/flatAtlas.ts#createFlatAtlas.ensure`, `src/render/renderer.ts#createSceneRenderer`
- `src/render/terrain.ts#createTerrainSurface.take` (method, line 2351) ← implementation from `src/render/flatAtlas.ts#createFlatAtlas.take`, `src/render/renderer.ts#createSceneRenderer.shapeGround`
- `src/render/terrain.ts#createTerrainSurface.updatePaint` (function, line 2153) ← implementation from `src/render/flatAtlas.ts#createFlatAtlas`, `src/render/flatAtlas.ts#createFlatAtlas.ensure`, `src/render/renderer.ts#createSceneRenderer`
- `src/render/terrain.ts#createTerrainSurface.wetAt` (function, line 1693) ← implementation from `src/render/flatAtlas.ts#createFlatAtlas`
- `src/render/terrain.ts#farLandMaterial` (function, line 495) ← call from `src/render/flatAtlas.ts#createFlatAtlas`
- `src/render/terrain.ts#farLandMaterial` (function, line 495) ← implementation from `src/render/flatAtlas.ts#createFlatAtlas`
- `src/render/terrain.ts#farLandMaterial` (function, line 495) ← read from `src/render/flatAtlas.ts#createFlatAtlas`
- `src/render/terrain.ts#farLandMaterial.dispose` (method, line 504) ← implementation from `src/render/flatAtlas.ts#createFlatAtlas`
- `src/render/terrain.ts#PlateSurface` (interface, line 219) ← call from `src/render/flatAtlas.ts#createFlatAtlas.ensure`, `src/render/flatAtlas.ts#createFlatAtlas.keepWithinBudget`, `src/render/flatAtlas.ts#createFlatAtlas.letGo`, `src/render/flatAtlas.ts#createFlatAtlas.refresh`
- `src/render/terrain.ts#PlateSurface` (interface, line 219) ← read from `src/render/renderer.ts#createSceneRenderer.natureSweep`
- `src/render/terrain.ts#PlateSurface` (interface, line 219) ← typeRef from `src/render/flatAtlas.ts#Plate`
- `src/render/terrain.ts#ShapeAsking` (interface, line 213) ← call from `src/render/flatAtlas.ts#createFlatAtlas.take`
- `src/render/terrain.ts#ShapeAsking` (interface, line 213) ← read from `src/render/flatAtlas.ts#createFlatAtlas`, `src/render/flatAtlas.ts#createFlatAtlas.take`, `src/render/renderer.ts#createSceneRenderer.shapeGround`
- `src/render/terrain.ts#ShapeAsking` (interface, line 213) ← reference from `src/render/renderer.ts#createSceneRenderer.shapeGround`
- `src/render/terrain.ts#ShapeAsking` (interface, line 213) ← typeRef from `src/render/flatAtlas.ts#createFlatAtlas`
- `src/render/terrain.ts#slabGeometry` (function, line 1159) ← call from `src/render/flatAtlas.ts#createFlatAtlas.buildSlab`
- `src/render/terrain.ts#strataTexture` (function, line 1091) ← indirectCall from `src/render/mesh/textureBaker.ts#fbm`
- `src/render/terrain.ts#TERRAIN_BASE` (variable, line 85) ← read from `src/render/flatAtlas.ts#createFlatAtlas`, `src/render/flatAtlas.ts#createFlatAtlas.askCorners`, `src/render/flatAtlas.ts#createFlatAtlas.bareAt`, `src/render/flatAtlas.ts#createFlatAtlas.buildSlab`, …
- `src/render/terrain.ts#TERRAIN_GRID` (variable, line 115) ← read from `src/render/renderer.ts#createSceneRenderer`
- `src/render/terrain.ts#TERRAIN_GRID` (variable, line 115) ← write from `src/render/renderer.ts#createSceneRenderer`
- `src/render/terrain.ts#TerrainPart` (interface, line 235) ← read from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.bringPlateLand`, `src/render/renderer.ts#createSceneRenderer.fileNature`, `src/render/renderer.ts#createSceneRenderer.natureSweep`, …
- `src/render/terrain.ts#TerrainPart` (interface, line 235) ← typeRef from `src/render/flatAtlas.ts#createFlatAtlas`, `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#NaturePlate`
- `src/render/terrain.ts#TerrainSource` (type, line 120) ← implements from `src/render/flatAtlas.ts#PlateSource`
- `src/render/terrain.ts#TerrainSource` (type, line 120) ← typeRef from `src/render/flatAtlas.ts#createFlatAtlas`, `src/render/flatAtlas.ts#createFlatAtlas.fill`, `src/render/flatAtlas.ts#createFlatAtlas.plateHolds`, `src/render/flatAtlas.ts#createFlatAtlas.readContent`, …
- `src/render/terrain.ts#TerrainSurface` (interface, line 125) ← call from `src/render/flatAtlas.ts#createFlatAtlas`, `src/render/flatAtlas.ts#createFlatAtlas.ensure`, `src/render/flatAtlas.ts#createFlatAtlas.letGo`, `src/render/flatAtlas.ts#createFlatAtlas.measureTop`, …
- `src/render/terrain.ts#TerrainSurface` (interface, line 125) ← read from `src/render/flatAtlas.ts#createFlatAtlas`, `src/render/flatAtlas.ts#createFlatAtlas.ensure`, `src/render/flatAtlas.ts#createFlatAtlas.show`, `src/render/renderer.ts#createSceneRenderer`, …
- `src/render/terrain.ts#TerrainSurface` (interface, line 125) ← reference from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.blockRead`, `src/render/renderer.ts#createSceneRenderer.buildWorld`, `src/render/renderer.ts#createSceneRenderer.padsAskOf`, …
- `src/render/terrain.ts#TerrainSurface` (interface, line 125) ← typeRef from `src/render/flatAtlas.ts#createFlatAtlas`, `src/render/flatAtlas.ts#createFlatAtlas.ask`, `src/render/flatAtlas.ts#createFlatAtlas.ensure`, `src/render/renderer.ts#createSceneRenderer`
- `src/render/terrain.ts#TerrainSurface` (interface, line 125) ← write from `src/render/flatAtlas.ts#createFlatAtlas.ensure`, `src/render/renderer.ts#createSceneRenderer`
- `src/render/terrain.ts#wallMaterial` (function, line 1220) ← call from `src/render/flatAtlas.ts#createFlatAtlas`

### src/render/terrainGrid.ts

- `src/render/terrainGrid.ts#TERRAIN_CELL` (variable, line 10) ← read from `src/main.ts#growth`, `src/render/buildings/buildingMesh.ts#LAWN_LATTICE`, `src/render/flatAtlas.ts#createFlatAtlas`, `src/render/flatAtlas.ts#createFlatAtlas.askCorners`, …
- `src/render/terrainGrid.ts#TERRAIN_HALF` (variable, line 12) ← read from `src/render/buildings/buildingMesh.ts#LAWN_LATTICE`, `src/render/coverSweep.ts#forestIn`, `src/render/coverSweep.ts#insidePlate`, `src/render/coverSweep.ts#latticeOver`, …
- `src/render/terrainGrid.ts#TERRAIN_SEGMENTS` (variable, line 8) ← read from `src/render/landStroke.ts#GRID`

### src/render/terrainLevels.ts

- `src/render/terrainLevels.ts#LandLevels` (class, line 18) ← construct from `src/render/landStroke.ts#measureLevels`, `src/render/plateMesh.ts#buildPlateMesh`
- `src/render/terrainLevels.ts#LandLevels.data` (field, line 20) ← read from `src/render/landStroke.ts#measureLevels`, `src/render/plateMesh.ts#buildPlateMesh`
- `src/render/terrainLevels.ts#LandLevels.markAll` (method, line 68) ← call from `src/render/plateMesh.ts#buildPlateMesh`
- `src/render/terrainLevels.ts#LandLevels.markBox` (method, line 54) ← call from `src/render/landStroke.ts#measureLevels`
- `src/render/terrainLevels.ts#LandLevels.measure` (method, line 73) ← call from `src/render/landStroke.ts#measureLevels`, `src/render/plateMesh.ts#buildPlateMesh`
- `src/render/terrainLevels.ts#LandLevels.take` (method, line 43) ← call from `src/render/landStroke.ts#measureLevels`
- `src/render/terrainLevels.ts#LOD_PATCH` (variable, line 6) ← read from `src/render/gpu/twins.ts#ground`

### src/render/terrainLight.worker.ts

- `src/render/terrainLight.worker.ts#serve` (function, line 5) ← callbackAssign from `src/bays.worker.ts#serve`, `src/cityGen.worker.ts#serve`, `src/conflictTable.worker.ts#serve`, `src/editor/save.worker.ts#serve`, …
- `src/render/terrainLight.worker.ts#serve` (function, line 5) ← moduleLoad from `src/job.worker.ts#LOADERS`

### src/render/terrainLod.ts

- `src/render/terrainLod.ts#LandGrid` (interface, line 28) ← read from `src/render/gpu/twins.ts#ground.point`, `src/render/gpu/twins.ts#ground.texel`
- `src/render/terrainLod.ts#LandGrid` (interface, line 28) ← typeRef from `src/render/gpu/twins.ts#LandLodData`

### src/render/terrainShaping.ts

- `src/render/terrainShaping.ts#sampleCorners` (function, line 78) ← call from `src/render/landStroke.ts#woodsBlock.land`, `src/render/sharedGround.ts#sharedGroundReaders.read`
- `src/render/terrainShaping.ts#shapeBoth` (function, line 67) ← call from `src/render/renderer.ts#createSceneRenderer.shapeGround`
- `src/render/terrainShaping.ts#TerrainRegion` (type, line 7) ← typeRef from `src/render/flatAtlas.ts#createFlatAtlas`, `src/render/flatAtlas.ts#createFlatAtlas.decode`, `src/render/flatAtlas.ts#createFlatAtlas.encode`, `src/render/renderer.ts#createSceneRenderer.regionRect`, …
- `src/render/terrainShaping.ts#TerrainShaper` (interface, line 10) ← call from `src/render/flatAtlas.ts#createFlatAtlas.shaperFor`
- `src/render/terrainShaping.ts#TerrainShaper` (interface, line 10) ← typeRef from `src/render/flatAtlas.ts#createFlatAtlas.shaperFor`

### src/render/water.ts

- `src/render/water.ts#WaterLook` (interface, line 46) ← read from `src/render/flatAtlas.ts#createFlatAtlas`
- `src/render/water.ts#WaterLook` (interface, line 46) ← typeRef from `src/render/flatAtlas.ts#createFlatAtlas`
- `src/render/water.ts#waterNormalTexels` (function, line 77) ← indirectCall from `src/render/mesh/textureBaker.ts#fbm`

### src/render/wind.ts

- `src/render/wind.ts#advanceWind` (function, line 135) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/wind.ts#applyPartSway` (function, line 170) ← call from `src/render/signals.ts#createSignalHeads`
- `src/render/wind.ts#applyStructureSway` (function, line 161) ← call from `src/render/signals.ts#createSignalHeads`
- `src/render/wind.ts#setWindWeather` (function, line 22) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/wind.ts#sways` (function, line 107) ← call from `src/render/renderer.ts#createSceneRenderer.noteSway`
- `src/render/wind.ts#windUniforms` (variable, line 13) ← read from `src/render/buildings/flagLayer.ts#createFlagLayer.set`, `src/render/buildings/flagLayer.ts#material`, `src/render/exhaust.ts#createExhaust`, `src/render/utilities.ts#wireMaterials`
- `src/render/wind.ts#WIRE_WIND_UNIFORMS_GLSL` (variable, line 144) ← read from `src/render/utilities.ts#wireMaterials`
- `src/render/wind.ts#wireWindGlsl` (function, line 151) ← call from `src/render/utilities.ts#wireMaterials`

### src/render/woodsBase.ts

- `src/render/woodsBase.ts#cornerLand` (function, line 66) ← call from `src/render/flatGround.worker.ts#serve`
- `src/render/woodsBase.ts#onPlate` (function, line 47) ← call from `src/render/landStroke.ts#woodsBlock.land`
- `src/render/woodsBase.ts#WOODS_SPACING` (variable, line 5) ← read from `src/render/renderer.ts#createSceneRenderer`
- `src/render/woodsBase.ts#woodsBase` (function, line 10) ← call from `src/render/flatGround.worker.ts#serve`
- `src/render/woodsBase.ts#woodsBaseInto` (function, line 18) ← call from `src/render/landStroke.ts#woodsBlock`
- `src/render/woodsBase.ts#woodsSpan` (function, line 33) ← call from `src/render/landStroke.ts#woodsBlock`

### src/world/clouds.ts

- `src/world/clouds.ts#CLOUD_SPAN` (variable, line 6) ← read from `src/main.ts#<IfStatement>@1703`, `src/main.ts#MAP_ACTIONS`
- `src/world/clouds.ts#driftedCloud` (function, line 101) ← call from `src/render/postprocess.ts#layClouds`
- `src/world/clouds.ts#driftedCloud` (function, line 101) ← read from `src/render/postprocess.ts#layClouds`
- `src/world/clouds.ts#MAX_PLACED_CLOUDS` (variable, line 24) ← read from `src/world/doc.ts#RoadDoc.addClouds`, `src/world/doc.ts#RoadDoc.fromJSON`
- `src/world/clouds.ts#PlacedCloud` (interface, line 8) ← read from `src/main.ts#MAP_ACTIONS`, `src/render/postprocess.ts#layClouds`, `src/world/doc.ts#listBox`, `src/world/doc.ts#placedCloud`, …
- `src/world/clouds.ts#PlacedCloud` (interface, line 8) ← typeRef from `src/render/postprocess.ts#createPostChain`, `src/render/postprocess.ts#layClouds`, `src/render/postprocess.ts#PostChain`, `src/world/doc.ts#listBox`, …
- `src/world/clouds.ts#readCloud` (function, line 36) ← call from `src/world/doc.ts#RoadDoc.fromJSON`
- `src/world/clouds.ts#scatterClouds` (function, line 51) ← call from `src/main.ts#<IfStatement>@1703`, `src/main.ts#MAP_ACTIONS`

### src/world/ecology.ts

- `src/world/ecology.ts#DEFAULT_REGION` (variable, line 9) ← read from `src/main.ts#newNature`, `src/main.ts#openCityNow`
- `src/world/ecology.ts#EcologyField` (interface, line 42) ← read from `src/render/renderer.ts#createSceneRenderer.natureSweep`
- `src/world/ecology.ts#isNatureSettings` (function, line 18) ← call from `src/editor/persistence.ts#isSerializedDoc`, `src/world/doc.ts#RoadDoc.fromJSON`
- `src/world/ecology.ts#isRegionId` (function, line 10) ← call from `src/main.ts#setMapBiome`
- `src/world/ecology.ts#NatureSettings` (interface, line 14) ← read from `src/main.ts#mapBiome`, `src/main.ts#setMapBiome`, `src/world/doc.ts#RoadDoc.fromJSON`
- `src/world/ecology.ts#NatureSettings` (interface, line 14) ← typeRef from `src/main.ts#newNature`, `src/world/doc.ts#RoadDoc.nature`, `src/world/doc.ts#RoadDoc.restoreValue`, `src/world/doc.ts#RoadDoc.setNature`

### src/world/elevation.ts

- `src/world/elevation.ts#buildRoadElevation` (function, line 279) ← call from `src/editor/commit.ts#solveOver`, `src/editor/editRules.ts#flatSolves.after`, `src/editor/editRules.ts#flatSolves.beforeSolve`, `src/editor/smallCityMap.ts#growLots`, …
- `src/world/elevation.ts#buildRoadElevation` (function, line 279) ← implementation from `src/editor/commit.ts#boreDeepCuts`, `src/editor/editRules.ts#grade`, `src/editor/editRules.ts#refuseRoadEdit.deck`, `src/editor/smallCityMap.ts#growLots.pavedAt`, …
- `src/world/elevation.ts#buildRoadElevation` (function, line 279) ← indirectCall from `src/world/roadField.ts#roadField.blend`, `src/world/roadField.ts#roadField.onSegment`
- `src/world/elevation.ts#CUT_WALL_REACH` (variable, line 222) ← read from `src/render/structures.ts#retainingWalls`
- `src/world/elevation.ts#FLAT_GROUND` (function, line 951) ← read from `src/editor/editRules.ts#flatSolves.after`, `src/editor/editRules.ts#flatSolves.beforeSolve`, `src/editor/smallCityMap.ts#growLots`, `src/render/renderer.ts#createSceneRenderer.buildWorld`, …
- `src/world/elevation.ts#GROUND_ONLY` (variable, line 208) ← read from `src/editor/smallCityMap.ts#growLots.pavedAt`, `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#SURFACE_SETS`, `src/render/utilities.ts#poleGroundAt`
- `src/world/elevation.ts#MAX_AUTHORED_GRADE` (variable, line 68) ← read from `src/editor/commit.ts#commitRoadPath`, `src/main.ts#drawOverlayScreen`, `src/main.ts#setNodeHeightMetres`
- `src/world/elevation.ts#pendingOrder` (function, line 196) ← call from `src/render/renderer.ts#createSceneRenderer.completeSolve`
- `src/world/elevation.ts#RoadElevation` (interface, line 135) ← call from `src/editor/smallCityMap.ts#growLots.pavedAt`, `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.compareSolves`, `src/render/renderer.ts#createSceneRenderer.completeSolve`, …
- `src/world/elevation.ts#RoadElevation` (interface, line 135) ← read from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.buildWorld`, `src/render/renderer.ts#createSceneRenderer.compareSolves`, `src/render/renderer.ts#createSceneRenderer.completeSolve`, …
- `src/world/elevation.ts#RoadElevation` (interface, line 135) ← reference from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.compareSolves`, `src/render/roadSurfaces.ts#roadSurfaceSteps`
- `src/world/elevation.ts#RoadElevation` (interface, line 135) ← typeRef from `src/editor/commit.ts#boreDeepCuts`, `src/editor/commit.ts#commitRoadPath`, `src/editor/commit.ts#DraftResult`, `src/editor/commit.ts#draftStations`, …
- `src/world/elevation.ts#roadShaper` (function, line 888) ← implementation from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.deckHeight`, `src/render/roadSurfaceReads.ts#passSurfaces`, `src/render/roadSurfaceReads.ts#passSurfaces.asphaltSection`, …
- `src/world/elevation.ts#roadShaper` (function, line 888) ← indirectCall from `src/world/roadField.ts#roadField.blend`, `src/world/roadField.ts#roadField.onSegment`
- `src/world/elevation.ts#roadStations` (function, line 240) ← call from `src/render/roadStations.worker.ts#serve`
- `src/world/elevation.ts#roadsToRead` (function, line 248) ← call from `src/render/renderer.ts#createSceneRenderer.buildWorld`, `src/render/renderer.ts#createSceneRenderer.completeSolve`
- `src/world/elevation.ts#roadsToRead` (function, line 248) ← typeRef from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.stationsAsked`
- `src/world/elevation.ts#SolveScope` (interface, line 169) ← read from `src/render/renderer.ts#createSceneRenderer.buildWorld`
- `src/world/elevation.ts#SolveScope` (interface, line 169) ← typeRef from `src/render/renderer.ts#createSceneRenderer.buildWorld`, `src/render/renderer.ts#createSceneRenderer.completeSolve`
- `src/world/elevation.ts#StationHint` (interface, line 232) ← inherit from `src/render/roadStations.ts#Hint`
- `src/world/elevation.ts#StationHint` (interface, line 232) ← read from `src/render/roadStations.ts#RoadStations.waiting`
- `src/world/elevation.ts#StationHint` (interface, line 232) ← typeRef from `src/render/roadStations.ts#RoadStations.current`
- `src/world/elevation.ts#SURFACE_READ_REACH` (variable, line 88) ← read from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.takeEditedCells`, `src/render/roadSurfaces.ts#roadSurfaceSteps`

### src/world/fogPaint.ts

- `src/world/fogPaint.ts#DEFAULT_FOG` (variable, line 24) ← read from `src/world/doc.ts#RoadDoc.fogSettings`, `src/world/doc.ts#RoadDoc.worldPart`
- `src/world/fogPaint.ts#FogDab` (interface, line 4) ← typeRef from `src/world/doc.ts#RoadDoc.fogDabs`
- `src/world/fogPaint.ts#FogSettings` (interface, line 19) ← read from `src/main.ts#SLIDERS`, `src/render/renderer.ts#createSceneRenderer`
- `src/world/fogPaint.ts#FogSettings` (interface, line 19) ← typeRef from `src/world/doc.ts#RoadDoc.fogSettings`, `src/world/doc.ts#RoadDoc.restoreValue`, `src/world/doc.ts#RoadDoc.setFogSettings`
- `src/world/fogPaint.ts#MAX_FOG_DABS` (variable, line 30) ← read from `src/world/doc.ts#TILE_CAPS`
- `src/world/fogPaint.ts#readFogDab` (function, line 48) ← call from `src/world/doc.ts#RoadDoc.fromJSON`
- `src/world/fogPaint.ts#readFogSettings` (function, line 42) ← call from `src/world/doc.ts#RoadDoc.fromJSON`, `src/world/doc.ts#RoadDoc.setFogSettings`

### src/world/groundPatch.ts

- `src/world/groundPatch.ts#cellDiagonal` (function, line 4) ← call from `src/render/plateMesh.ts#buildPlateMesh`
- `src/world/groundPatch.ts#GroundPatch` (interface, line 30) ← typeRef from `src/editor/growthReplica.ts#GrowAsk`
- `src/world/groundPatch.ts#patchGround` (function, line 58) ← call from `src/editor/growthReplica.ts#GrowthReplica.handle`
- `src/world/groundPatch.ts#PatchMiss` (class, line 51) ← reference from `src/editor/growthReplica.ts#GrowthReplica.handle`
- `src/world/groundPatch.ts#sampleGroundPatch` (function, line 41) ← call from `src/editor/growthFeed.ts#GrowthSession.send`

### src/world/gullies.ts

- `src/world/gullies.ts#DEFAULT_GULLY_AUTO` (variable, line 16) ← read from `src/world/doc.ts#RoadDoc.gullyAuto`, `src/world/doc.ts#RoadDoc.setGullyAuto`, `src/world/doc.ts#RoadDoc.worldPart`
- `src/world/gullies.ts#GullyDab` (interface, line 3) ← typeRef from `src/world/doc.ts#RoadDoc.gullyDabs`
- `src/world/gullies.ts#MAX_GULLY_DABS` (variable, line 14) ← read from `src/world/doc.ts#TILE_CAPS`
- `src/world/gullies.ts#readGullyDab` (function, line 18) ← call from `src/world/doc.ts#RoadDoc.fromJSON`

### src/world/landscape.ts

- `src/world/landscape.ts#ACCESS_REACH` (variable, line 278) ← read from `src/render/streetFurnitureLayer.ts#ROAD_REACH`, `src/world/roads/furnitureSets.ts#furnitureFor`
- `src/world/landscape.ts#CrossingAccess` (interface, line 227) ← read from `src/editor/poles.ts#inLanding`
- `src/world/landscape.ts#CrossingAccess` (interface, line 227) ← typeRef from `src/editor/poles.ts#inLanding`, `src/world/poleLines.ts#PoleLines`
- `src/world/landscape.ts#crossingAccesses` (function, line 238) ← call from `src/world/streetFurniture.ts#streetFurniture`
- `src/world/landscape.ts#crossingAccessesAt` (function, line 251) ← call from `src/world/poleLines.ts#poleLinesOf`, `src/world/roads/furnitureSets.ts#furnitureFor`
- `src/world/landscape.ts#crossingAccessTester` (function, line 296) ← call from `src/world/roads/furnitureSets.ts#furnitureFor`, `src/world/streetFurniture.ts#streetFurniture`
- `src/world/landscape.ts#footwayAt` (function, line 94) ← call from `src/render/signLayer.ts#itemSigns`, `src/world/streetFurniture.ts#standing`
- `src/world/landscape.ts#FootwayHit` (interface, line 76) ← read from `src/render/signLayer.ts#itemSigns`, `src/world/roads/furnitureSets.ts#furnitureFor`, `src/world/streetFurniture.ts#standing`
- `src/world/landscape.ts#GROW_MINUTES` (variable, line 30) ← read from `src/render/streetFurnitureLayer.ts#stillGrowing`
- `src/world/landscape.ts#isLandscapeKind` (function, line 52) ← call from `src/world/doc.ts#landscapeRecord`
- `src/world/landscape.ts#isSignType` (function, line 44) ← call from `src/world/doc.ts#landscapeRecord`
- `src/world/landscape.ts#LANDSCAPE_KINDS` (variable, line 13) ← read from `src/ui/toolChoices.ts#kind`, `src/ui/v2/shell.ts#mountShell.renderLandscape`, `src/world/doc.ts#landscapeRecord`
- `src/world/landscape.ts#LANDSCAPE_RADIUS` (variable, line 57) ← read from `src/main.ts#drawStreetscapeHover`, `src/world/streetFurniture.ts#standing`
- `src/world/landscape.ts#LandscapeItem` (interface, line 16) ← read from `src/actionsWiring.ts#createActions.explodeAt`, `src/editor/bulldozer.ts#Bulldozer.boxed`, `src/editor/bulldozer.ts#Bulldozer.click`, `src/editor/roads/streetKit.ts#kitOf`, …
- `src/world/landscape.ts#LandscapeItem` (interface, line 16) ← typeRef from `src/editor/chunkedSave.ts#KEYED`, `src/editor/roadTool.ts#landscapeAlong`, `src/render/signLayer.ts#isSignItem`, `src/render/streetFurnitureLayer.ts#furniturePlace`, …
- `src/world/landscape.ts#LandscapeItem` (interface, line 16) ← write from `src/world/doc.ts#landscapeRecord`
- `src/world/landscape.ts#LandscapeKind` (type, line 14) ← typeRef from `src/editor/roads/streetKit.ts#carried`, `src/ui/toolChoices.ts#kind`, `src/ui/toolChoices.ts#setStreetscapeKind`, `src/ui/toolChoices.ts#streetscapeKind`, …
- `src/world/landscape.ts#landscapeNearIn` (function, line 398) ← call from `src/editor/bulldozer.ts#Bulldozer.click`
- `src/world/landscape.ts#LandscapeSnap` (type, line 315) ← read from `src/main.ts#drawStreetscapeHover`, `src/world/roads/furnitureSets.ts#furnitureFor`
- `src/world/landscape.ts#MAX_ITEM_RADIUS` (variable, line 395) ← read from `src/render/streetFurnitureLayer.ts#ROAD_REACH`, `src/world/roads/furnitureSets.ts#furnitureFor`
- `src/world/landscape.ts#MEDIAN_UTURN_OPENING` (variable, line 158) ← read from `src/editor/streetObjects.ts#commitUturn`
- `src/world/landscape.ts#medianAt` (function, line 161) ← call from `src/world/streetFurniture.ts#standing`
- `src/world/landscape.ts#MedianHit` (interface, line 127) ← read from `src/world/roads/furnitureSets.ts#furnitureFor`, `src/world/streetFurniture.ts#standing`
- `src/world/landscape.ts#medianNose` (function, line 148) ← call from `src/render/roadSurfaceRings.ts#medianOf.back`
- `src/world/landscape.ts#nearCrossingAccess` (function, line 281) ← call from `src/world/streetFurniture.ts#furnitureOf`
- `src/world/landscape.ts#plantedMedian` (function, line 139) ← call from `src/world/roads/furnitureSets.ts#furnitureFor`
- `src/world/landscape.ts#plantGrowth` (function, line 35) ← call from `src/render/streetFurnitureLayer.ts#placeInto`
- `src/world/landscape.ts#SIGN_HAS_TEXT` (variable, line 48) ← read from `src/lab/catalog/layers.ts#signItems`, `src/ui/v2/shell.ts#mountShell.renderLandscape`
- `src/world/landscape.ts#SIGN_TEXT_MAX` (variable, line 50) ← read from `src/ui/v2/shell.ts#mountShell.textInput`, `src/world/doc.ts#landscapeRecord`, `src/world/doc.ts#RoadDoc.addLandscapeMany`
- `src/world/landscape.ts#SIGN_TYPES` (variable, line 42) ← read from `src/lab/catalog/layers.ts#signItems`, `src/ui/v2/shell.ts#mountShell.renderLandscape`
- `src/world/landscape.ts#SignType` (type, line 43) ← typeRef from `src/render/signLayer.ts#createSignLayer.putQuad`, `src/render/signLayer.ts#Stand`, `src/render/signLayer.ts#Standing`, `src/render/signs.ts#onTopOfPost`, …
- `src/world/landscape.ts#snapLandscape` (function, line 326) ← call from `src/world/roads/furnitureSets.ts#furnitureFor`
- `src/world/landscape.ts#SnapLocal` (interface, line 320) ← typeRef from `src/world/roads/furnitureSets.ts#furnitureFor`

### src/world/terrain.ts

- `src/world/terrain.ts#isTerrainMode` (function, line 294) ← call from `src/editor/persistence.ts#isSerializedDoc`
- `src/world/terrain.ts#isTerrainProfile` (function, line 30) ← call from `src/editor/persistence.ts#isSerializedDoc`
- `src/world/terrain.ts#MAX_TERRAIN_STAMPS` (variable, line 36) ← read from `src/main.ts#endTerrainStroke`, `src/world/doc.ts#TILE_CAPS`
- `src/world/terrain.ts#readRelief` (function, line 92) ← call from `src/editor/persistence.ts#isSerializedDoc`, `src/world/doc.ts#RoadDoc.fromJSON`
- `src/world/terrain.ts#RELIEF_FLAT` (variable, line 90) ← read from `src/editor/history.ts#openedGenerated`, `src/main.ts#NEW_RELIEF`, `src/render/renderer.ts#isLevelLand`, `src/render/roadSurfacePass.ts#followsGround`
- `src/world/terrain.ts#RELIEF_LEGACY` (variable, line 88) ← read from `src/world/doc.ts#RoadDoc.fromJSON`, `src/world/doc.ts#RoadDoc.savedGlobals`, `src/world/doc.ts#RoadDoc.terrainRelief`, `src/world/doc.ts#RoadDoc.worldPart`
- `src/world/terrain.ts#reliefHeight` (function, line 100) ← call from `src/render/flatAtlas.ts#createFlatAtlas.buildSlab`, `src/render/flatAtlas.ts#createFlatAtlas.sync`, `src/render/flatAtlas.ts#createFlatAtlas.writeFar`, `src/render/flatGround.worker.ts#serve`, …
- `src/world/terrain.ts#ReliefVersion` (type, line 87) ← typeRef from `src/render/flatAtlas.ts#createFlatAtlas`, `src/render/flatAtlas.ts#createFlatAtlas.buildFar`, `src/render/flatGround.worker.ts#Ask`, `src/render/flatGround.worker.ts#serve`, …
- `src/world/terrain.ts#sampleTerrainHeight` (function, line 218) ← call from `src/editor/commit.ts#boreDeepCuts`, `src/editor/commit.ts#draftStations`
- `src/world/terrain.ts#TERRAIN_MAX_HEIGHT` (variable, line 39) ← read from `src/render/densityCars.ts#createDensityCars.tileAt`, `src/render/densityCars.ts#TILE_RADIUS`, `src/render/renderer.ts#createSceneRenderer`, `src/render/transit.ts#createTransit`, …
- `src/world/terrain.ts#TERRAIN_MIN_HEIGHT` (variable, line 38) ← read from `src/render/densityCars.ts#createDensityCars.tileAt`, `src/render/densityCars.ts#TILE_RADIUS`, `src/render/renderer.ts#createSceneRenderer`, `src/render/transit.ts#createTransit`, …
- `src/world/terrain.ts#TerrainIndex` (class, line 161) ← construct from `src/editor/commit.ts#terrainIndexOf`
- `src/world/terrain.ts#TerrainIndex` (class, line 161) ← typeRef from `src/editor/commit.ts#onTerrain`, `src/editor/commit.ts#solveOver`, `src/editor/commit.ts#terrainIndexOf`
- `src/world/terrain.ts#TerrainIndex.relief` (field, line 167) ← read from `src/editor/commit.ts#terrainIndexOf`
- `src/world/terrain.ts#TerrainIndex.stamps` (field, line 163) ← read from `src/editor/commit.ts#terrainIndexOf`
- `src/world/terrain.ts#TerrainMode` (type, line 4) ← typeRef from `src/main.ts#BrushMode`
- `src/world/terrain.ts#TerrainStamp` (interface, line 6) ← read from `src/editor/commit.ts#boreDeepCuts`, `src/render/flatAtlas.ts#createFlatAtlas.plateHolds`, `src/render/flatAtlas.ts#createFlatAtlas.readWholeContent`, `src/world/doc.ts#RoadDoc.addTerrainStamp`, …
- `src/world/terrain.ts#TerrainStamp` (interface, line 6) ← typeRef from `src/render/flatAtlas.ts#createFlatAtlas.takeStamps`, `src/render/flatAtlas.ts#Plate`, `src/render/flatAtlas.ts#PlateSource.stampSources`, `src/render/flatAtlas.ts#PlateSource.terrainStamps`, …
- `src/world/terrain.ts#valueNoise` (function, line 72) ← call from `src/world/cityGen/field.ts#TensorField.major`, `src/world/cityGen/plan.ts#planCity.inside`, `src/world/cityGen/plan.ts#planCity.zoneAt`

### src/world/terrainPaint.ts

- `src/world/terrainPaint.ts#CoverKind` (type, line 10) ← typeRef from `src/render/flatAtlas.ts#createFlatAtlas`
- `src/world/terrainPaint.ts#GeologyKind` (type, line 15) ← typeRef from `src/main.ts#LANDFORMS`, `src/render/flatAtlas.ts#createFlatAtlas`
- `src/world/terrainPaint.ts#isCoverKind` (function, line 11) ← call from `src/render/renderer.ts#createSceneRenderer.sweepAllCovers`
- `src/world/terrainPaint.ts#isPaintKind` (function, line 32) ← call from `src/world/doc.ts#RoadDoc.fromJSON`
- `src/world/terrainPaint.ts#MAX_PAINT_DABS` (variable, line 37) ← read from `src/world/doc.ts#TILE_CAPS`
- `src/world/terrainPaint.ts#PAINT_KINDS` (variable, line 4) ← read from `src/ui/toolChoices.ts#paint`, `src/ui/v2/shell.ts#mountShell.renderLandscape`
- `src/world/terrainPaint.ts#PaintDab` (interface, line 23) ← read from `src/render/flatAtlas.ts#createFlatAtlas.plateHolds`, `src/render/flatAtlas.ts#createFlatAtlas.readWholeContent`, `src/render/platePaint.ts#dabInRects`, `src/render/platePaint.ts#dabKey`, …
- `src/world/terrainPaint.ts#PaintDab` (interface, line 23) ← typeRef from `src/render/flatAtlas.ts#createFlatAtlas.takePaint`, `src/render/flatAtlas.ts#Plate`, `src/render/flatAtlas.ts#PlateSource.paintSources`, `src/render/flatAtlas.ts#PlateSource.terrainPaint`, …
- `src/world/terrainPaint.ts#PaintKind` (type, line 6) ← typeRef from `src/ui/toolChoices.ts#paint`, `src/ui/toolChoices.ts#paintKind`, `src/ui/toolChoices.ts#setPaintKind`, `src/ui/v2/shell.ts#mountShell.renderLandscape`, …

### src/world/trees.ts

- `src/world/trees.ts#DEFAULT_TREE_BRUSH` (variable, line 38) ← read from `src/ui/toolChoices.ts#treeBrushNow`
- `src/world/trees.ts#MAX_PLANTED_TREES` (variable, line 40) ← read from `src/world/doc.ts#TILE_CAPS`
- `src/world/trees.ts#MAX_TREE_CLEARINGS` (variable, line 41) ← read from `src/world/doc.ts#TILE_CAPS`
- `src/world/trees.ts#PlantedTree` (interface, line 10) ← read from `src/render/flatAtlas.ts#createFlatAtlas.readWholeContent`, `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.plantAll`, `src/render/renderer.ts#createSceneRenderer.plantedForestOf`, …
- `src/world/trees.ts#PlantedTree` (interface, line 10) ← typeRef from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.plantAll`, `src/render/renderer.ts#createSceneRenderer.plantedIn`, `src/world/cityGen/squares.ts#planSquares`, …
- `src/world/trees.ts#readPlantedTree` (function, line 45) ← call from `src/world/doc.ts#RoadDoc.fromJSON`
- `src/world/trees.ts#readTreeClearing` (function, line 51) ← call from `src/world/doc.ts#RoadDoc.fromJSON`
- `src/world/trees.ts#TREE_KINDS` (variable, line 4) ← read from `src/ui/v2/shell.ts#mountShell.renderLandscape`
- `src/world/trees.ts#TreeBrush` (interface, line 27) ← read from `src/main.ts#SLIDERS`
- `src/world/trees.ts#TreeBrush` (interface, line 27) ← typeRef from `src/ui/toolChoices.ts#setTreeBrush`, `src/ui/toolChoices.ts#treeBrush`, `src/ui/toolChoices.ts#treeBrushNow`
- `src/world/trees.ts#TreeClearing` (interface, line 21) ← read from `src/render/clearingCells.ts#ClearingCells.has`, `src/render/clearingCells.ts#ClearingCells.over`
- `src/world/trees.ts#TreeClearing` (interface, line 21) ← typeRef from `src/render/clearingCells.ts#ClearingCells.cells`, `src/render/clearingCells.ts#ClearingCells.file`, `src/render/clearingCells.ts#ClearingCells.filed`, `src/render/clearingCells.ts#ClearingCells.follow`, …
- `src/world/trees.ts#TreeKind` (type, line 5) ← typeRef from `src/ui/toolChoices.ts#setTreeKind`, `src/ui/toolChoices.ts#treeKind`, `src/ui/toolChoices.ts#treeKindNow`
- `src/world/trees.ts#treeSeed` (function, line 58) ← call from `src/world/cityGen/squares.ts#planSquares`

### src/world/weather.ts

- `src/world/weather.ts#DEFAULT_WEATHER` (variable, line 22) ← read from `src/world/doc.ts#RoadDoc.savedGlobals`, `src/world/doc.ts#RoadDoc.weather`, `src/world/doc.ts#RoadDoc.worldPart`
- `src/world/weather.ts#readWeather` (function, line 38) ← call from `src/world/doc.ts#RoadDoc.fromJSON`, `src/world/doc.ts#RoadDoc.setWeather`
- `src/world/weather.ts#Weather` (interface, line 3) ← read from `src/main.ts#<ExpressionStatement>@1674`, `src/render/renderer.ts#createSceneRenderer`
- `src/world/weather.ts#Weather` (interface, line 3) ← typeRef from `src/world/doc.ts#RoadDoc.restoreValue`, `src/world/doc.ts#RoadDoc.setWeather`, `src/world/doc.ts#RoadDoc.weather`
- `src/world/weather.ts#windVector` (function, line 51) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/world/weather.ts#windVector` (function, line 51) ← read from `src/render/renderer.ts#createSceneRenderer`

## Shared declarations the feature uses

### src/bays.worker.ts

- `src/bays.worker.ts#serve` (function, line 5) — callbackAssign by 2: `src/render/terrainLight.worker.ts#serve`, `src/render/ecology.worker.ts#serve`

### src/cityGen.worker.ts

- `src/cityGen.worker.ts#serve` (function, line 19) — callbackAssign by 2: `src/render/terrainLight.worker.ts#serve`, `src/render/ecology.worker.ts#serve`

### src/conflictTable.worker.ts

- `src/conflictTable.worker.ts#serve` (function, line 13) — callbackAssign by 2: `src/render/terrainLight.worker.ts#serve`, `src/render/ecology.worker.ts#serve`

### src/core/aabb.ts

- `src/core/aabb.ts#Aabb` (interface, line 3) — read by 8: `src/world/landscape.ts#footwayAt`, `src/world/landscape.ts#medianAt`, `src/world/elevation.ts#buildRoadElevation.take`, `src/world/elevation.ts#ProfileIndex.cells`, …
- `src/core/aabb.ts#Aabb` (interface, line 3) — typeRef by 10: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#Profile`, `src/world/elevation.ts#ProfileIndex.cells`, `src/world/elevation.ts#RoadElevation`, …

### src/core/derivedCache.ts

- `src/core/derivedCache.ts#derivedCacheOpen` (function, line 45) — reference by 1: `src/render/ecologyKept.ts#derivedEcologyCache`
- `src/core/derivedCache.ts#forgetOtherDerived` (function, line 327) — call by 1: `src/render/ecology.worker.ts#serve`
- `src/core/derivedCache.ts#keepDerivedQueuedMany` (function, line 175) — call by 1: `src/render/ecologyKept.ts#derivedEcologyCache.put`
- `src/core/derivedCache.ts#readDerived` (function, line 53) — call by 1: `src/render/ecologyKept.ts#derivedEcologyCache.get`
- `src/core/derivedCache.ts#WriteBudget` (class, line 200) — construct by 1: `src/render/ecologyKept.ts#derivedEcologyCache`

### src/core/digest.ts

- `src/core/digest.ts#Digest` (class, line 6) — construct by 9: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#buildRoadElevation.keyOf`, `src/world/elevation.ts#buildRoadElevation.localDigest`, `src/world/elevation.ts#lookDigest`, …
- `src/core/digest.ts#Digest.add` (method, line 11) — call by 7: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#buildRoadElevation.localDigest`, `src/world/elevation.ts#lookDigest`, `src/world/elevation.ts#ProfileIndex.digest`, …
- `src/core/digest.ts#Digest.addAll` (method, line 19) — call by 3: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#buildRoadElevation.keyOf`, `src/render/ecologyKept.ts#ecologyDigest`
- `src/core/digest.ts#Digest.addPlain` (method, line 31) — call by 4: `src/world/elevation.ts#buildRoadElevation.keyOf`, `src/world/elevation.ts#lookDigest`, `src/render/ecologyKept.ts#ecologyDigest`, `src/render/ecologyKept.ts#keptWoods`
- `src/core/digest.ts#Digest.addText` (method, line 25) — call by 2: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#buildRoadElevation.localDigest`
- `src/core/digest.ts#Digest.value` (method, line 59) — call by 9: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#buildRoadElevation.keyOf`, `src/world/elevation.ts#buildRoadElevation.localDigest`, `src/world/elevation.ts#lookDigest`, …

### src/core/hitch.ts

- `src/core/hitch.ts#measureHitch` (function, line 2) — call by 5: `src/world/elevation.ts#buildRoadElevation`, `src/render/terrain.ts#createTerrainSurface`, `src/render/terrain.ts#createTerrainSurface.applyLight`, `src/render/terrain.ts#createTerrainSurface.readOnPage`, …

### src/core/polyline.ts

- `src/core/polyline.ts#Frame` (interface, line 7) — read by 6: `src/editor/streetscapeTool.ts#rowPieces`, `src/world/landscape.ts#footwayAt`, `src/world/landscape.ts#medianAt`, `src/world/elevation.ts#readStations`, …
- `src/core/polyline.ts#Polyline` (class, line 36) — reference by 1: `src/world/elevation.ts#roadStations`
- `src/core/polyline.ts#Polyline` (class, line 36) — typeRef by 4: `src/world/elevation.ts#Profile`, `src/world/elevation.ts#readStations`, `src/world/elevation.ts#sameLine`, `src/world/elevation.ts#ShapingRoad`
- `src/core/polyline.ts#Polyline.bbox` (getter, line 116) — call by 3: `src/world/landscape.ts#footwayAt`, `src/world/landscape.ts#medianAt`, `src/world/elevation.ts#buildRoadElevation`
- `src/core/polyline.ts#Polyline.closestInto` (method, line 347) — call by 5: `src/world/landscape.ts#footwayAt`, `src/world/landscape.ts#medianAt`, `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#buildRoadElevation.localDigest`, …
- `src/core/polyline.ts#Polyline.closestPoint` (method, line 330) — call by 1: `src/world/roads/derivedSigns.ts#signApproach`
- `src/core/polyline.ts#Polyline.closestPoint` (method, line 330) — read by 1: `src/world/roads/derivedSigns.ts#signApproach`
- `src/core/polyline.ts#Polyline.distanceTo` (method, line 359) — call by 1: `src/world/landscape.ts#onAnyRoad`
- `src/core/polyline.ts#Polyline.fromFlat` (method, line 65) — call by 1: `src/world/elevation.ts#roadStations`
- `src/core/polyline.ts#Polyline.length` (getter, line 123) — call by 6: `src/editor/streetscapeTool.ts#rowPieces`, `src/world/landscape.ts#footwayAt`, `src/world/landscape.ts#medianAt`, `src/world/elevation.ts#buildRoadElevation`, …
- `src/core/polyline.ts#Polyline.sampleAt` (method, line 152) — call by 6: `src/editor/streetscapeTool.ts#rowPieces`, `src/world/landscape.ts#footwayAt`, `src/world/landscape.ts#medianAt`, `src/world/elevation.ts#readStations`, …
- `src/core/polyline.ts#Polyline.xy` (field, line 38) — read by 5: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#buildRoadElevation.localDigest`, `src/world/elevation.ts#roadsToRead`, `src/world/elevation.ts#sameLine`, …

### src/core/rng.ts

- `src/core/rng.ts#Rng` (class, line 6) — construct by 3: `src/render/grass.ts#grassClump`, `src/render/bake/detailRecipes.ts#bake`, `src/render/propGeometry.ts#grassTuftGeometry`
- `src/core/rng.ts#Rng` (class, line 6) — typeRef by 1: `src/render/scenery.ts#foliageTint`
- `src/core/rng.ts#Rng.float` (method, line 57) — call by 4: `src/render/grass.ts#grassClump`, `src/render/scenery.ts#foliageTint`, `src/render/bake/detailRecipes.ts#bake`, `src/render/propGeometry.ts#grassTuftGeometry`

### src/core/scalar.ts

- `src/core/scalar.ts#clamp` (function, line 58) — call by 1: `src/world/terrain.ts#sampleTerrainHeight`

### src/core/units.ts

- `src/core/units.ts#m` (function, line 6) — call by 135: `src/editor/terrainBrush.ts#NOISE_ORIGIN_REACH`, `src/editor/terrainBrush.ts#TerrainBrush.paint`, `src/editor/streetscapeTool.ts#MEADOW_CLEAR`, `src/editor/streetscapeTool.ts#rowPieces`, …
- `src/core/units.ts#perM` (function, line 9) — call by 6: `src/render/terrain.ts#landShader`, `src/render/wind.ts#installStructureSway`, `src/render/wind.ts#WIND_GLSL`, `src/render/wind.ts#wireWindGlsl`, …
- `src/core/units.ts#TUNED_UNIT` (variable, line 12) — read by 2: `src/world/terrain.ts#baseRelief`, `src/world/terrain.ts#naturalRelief`
- `src/core/units.ts#UNITS_PER_METER` (variable, line 3) — read by 3: `src/editor/terrainBrush.ts#stampTerrain`, `src/editor/cloudTool.ts#CloudTool.down`, `src/render/lowPolyTrees.ts#nearLeaves`

### src/core/vec2.ts

- `src/core/vec2.ts#Vec2` (interface, line 4) — read by 17: `src/editor/terrainBrush.ts#noiseOrigin`, `src/editor/terrainBrush.ts#stampTerrain`, `src/editor/terrainBrush.ts#TerrainBrush.begin`, `src/editor/terrainBrush.ts#TerrainBrush.paint`, …
- `src/core/vec2.ts#Vec2` (interface, line 4) — typeRef by 26: `src/editor/terrainBrush.ts#noiseOrigin`, `src/editor/terrainBrush.ts#stampTerrain`, `src/editor/terrainBrush.ts#Stroke`, `src/editor/terrainBrush.ts#TerrainBrush.at`, …

### src/editor/commit.ts

- `src/editor/commit.ts#solveOver` (function, line 204) — indirectCall by 1: `src/world/elevation.ts#buildRoadElevation`

### src/editor/editStream.ts

- `src/editor/editStream.ts#editRandom` (function, line 10) — call by 1: `src/editor/terrainBrush.ts#stampTerrain`

### src/editor/save.worker.ts

- `src/editor/save.worker.ts#serve` (function, line 12) — callbackAssign by 2: `src/render/terrainLight.worker.ts#serve`, `src/render/ecology.worker.ts#serve`

### src/editor/snap.ts

- `src/editor/snap.ts#ribbonsNear` (function, line 74) — call by 1: `src/editor/streetscapeTool.ts#itemsAround`

### src/globals.d.ts

- `src/globals.d.ts#urls` (variable, line 5) — read by 1: `src/render/elements.ts#url`

### src/growth.worker.ts

- `src/growth.worker.ts#serve` (function, line 5) — callbackAssign by 2: `src/render/terrainLight.worker.ts#serve`, `src/render/ecology.worker.ts#serve`

### src/job.worker.ts

- `src/job.worker.ts#<ExpressionStatement>` (statement, line 64) — callbackAssign by 2: `src/render/terrainLight.worker.ts#serve`, `src/render/ecology.worker.ts#serve`
- `src/job.worker.ts#<ExpressionStatement>` (statement, line 64) — implementation by 2: `src/render/terrainLight.worker.ts#serve`, `src/render/ecology.worker.ts#serve`
- `src/job.worker.ts#adopt` (function, line 40) — callbackAssign by 2: `src/render/terrainLight.worker.ts#serve`, `src/render/ecology.worker.ts#serve`
- `src/job.worker.ts#adopt` (function, line 40) — implementation by 2: `src/render/terrainLight.worker.ts#serve`, `src/render/ecology.worker.ts#serve`

### src/jobs/jobPool.ts

- `src/jobs/jobPool.ts#canQueueJobs` (function, line 534) — call by 1: `src/render/landStrokePool.ts#landStrokeOffPage`
- `src/jobs/jobPool.ts#Host` (interface, line 208) — read by 1: `src/jobs/jobPool.ts#JobPort.constructor`
- `src/jobs/jobPool.ts#Host` (interface, line 208) — write by 1: `src/jobs/jobPool.ts#JobPort.constructor`
- `src/jobs/jobPool.ts#jobPort` (function, line 669) — call by 2: `src/render/terrainLightPool.ts#start`, `src/render/ecologyPool.ts#start`
- `src/jobs/jobPool.ts#JobPort` (class, line 593) — reference by 1: `src/jobs/jobPool.ts#postSetup`
- `src/jobs/jobPool.ts#JobPort.host` (field, line 599) — read by 1: `src/jobs/jobPool.ts#JobPort.constructor`
- `src/jobs/jobPool.ts#JobPort.host` (field, line 599) — write by 1: `src/jobs/jobPool.ts#JobPort.constructor`
- `src/jobs/jobPool.ts#JobPort.worker` (field, line 597) — read by 1: `src/jobs/jobPool.ts#JobPort.postSetup`
- `src/jobs/jobPool.ts#JobPort.worker` (field, line 597) — write by 1: `src/jobs/jobPool.ts#JobPort.constructor`
- `src/jobs/jobPool.ts#JobSystem.nextPort` (field, line 233) — read by 1: `src/jobs/jobPool.ts#JobPort.id`
- `src/jobs/jobPool.ts#JobSystem.nextPort` (field, line 233) — write by 1: `src/jobs/jobPool.ts#JobPort.id`
- `src/jobs/jobPool.ts#JobSystem.ports` (field, line 232) — read by 1: `src/jobs/jobPool.ts#JobPort.constructor`
- `src/jobs/jobPool.ts#JobSystem.ports` (field, line 232) — write by 1: `src/jobs/jobPool.ts#JobPort.constructor`
- `src/jobs/jobPool.ts#JobTicket` (interface, line 61) — call by 3: `src/render/terrain.ts#createTerrainSurface`, `src/render/terrain.ts#createTerrainSurface.levelsMeasuredWhole`, `src/render/terrain.ts#createTerrainSurface.woodsMadeWhole`
- `src/jobs/jobPool.ts#JobTicket` (interface, line 61) — typeRef by 1: `src/render/terrain.ts#createTerrainSurface`
- `src/jobs/jobPool.ts#place` (function, line 559) — call by 1: `src/jobs/jobPool.ts#JobPort.constructor`
- `src/jobs/jobPool.ts#submitJob.cancel` (method, line 488) — implementation by 3: `src/render/terrain.ts#createTerrainSurface`, `src/render/terrain.ts#createTerrainSurface.levelsMeasuredWhole`, `src/render/terrain.ts#createTerrainSurface.woodsMadeWhole`
- `src/jobs/jobPool.ts#system` (variable, line 249) — read by 2: `src/jobs/jobPool.ts#JobPort.constructor`, `src/jobs/jobPool.ts#JobPort.id`

### src/jobs/jobScope.ts

- `src/jobs/jobScope.ts#JobScope` (interface, line 2) — call by 2: `src/render/terrainLight.worker.ts#serve`, `src/render/ecology.worker.ts#serve`
- `src/jobs/jobScope.ts#JobScope` (interface, line 2) — typeRef by 2: `src/render/terrainLight.worker.ts#serve`, `src/render/ecology.worker.ts#serve`
- `src/jobs/jobScope.ts#JobScope` (interface, line 2) — write by 2: `src/render/terrainLight.worker.ts#serve`, `src/render/ecology.worker.ts#serve`
- `src/jobs/jobScope.ts#JobService` (type, line 8) — typeRef by 1: `src/jobs/jobPool.ts#JobPort.constructor`
- `src/jobs/jobScope.ts#ToJob` (interface, line 13) — typeRef by 1: `src/jobs/jobPool.ts#JobPort.postSetup`

### src/lab/catalog/layers.ts

- `src/lab/catalog/layers.ts#farNetworkItems.flat` (function, line 200) — indirectCall by 1: `src/world/elevation.ts#buildRoadElevation`
- `src/lab/catalog/layers.ts#signItems.flat` (function, line 77) — indirectCall by 1: `src/world/elevation.ts#buildRoadElevation`

### src/lab/catalog/layout.ts

- `src/lab/catalog/layout.ts#layoutItems.flat` (function, line 166) — indirectCall by 1: `src/world/elevation.ts#buildRoadElevation`
- `src/lab/catalog/layout.ts#surfacesOf.flat` (function, line 112) — indirectCall by 1: `src/world/elevation.ts#buildRoadElevation`

### src/main.ts

- `src/main.ts#cloudTool` (variable, line 1137) — implementation by 2: `src/editor/cloudTool.ts#CloudTool.down`, `src/editor/cloudTool.ts#CloudTool.move`
- `src/main.ts#streetscapeTool` (variable, line 502) — implementation by 4: `src/editor/streetscapeTool.ts#StreetscapeTool.down`, `src/editor/streetscapeTool.ts#StreetscapeTool.move`, `src/editor/streetscapeTool.ts#StreetscapeTool.reach`, `src/editor/streetscapeTool.ts#StreetscapeTool.up`
- `src/main.ts#terrainBrush` (variable, line 579) — implementation by 3: `src/editor/terrainBrush.ts#TerrainBrush.begin`, `src/editor/terrainBrush.ts#TerrainBrush.paint`, `src/editor/terrainBrush.ts#TerrainBrush.tick`

### src/networkBake.worker.ts

- `src/networkBake.worker.ts#serve` (function, line 32) — callbackAssign by 2: `src/render/terrainLight.worker.ts#serve`, `src/render/ecology.worker.ts#serve`

### src/render/agents.ts

- `src/render/agents.ts#createAgentMeshes.sync` (method, line 2153) — indirectCall by 1: `src/render/groundShear.ts#groundGradient`

### src/render/buildings/cell.worker.ts

- `src/render/buildings/cell.worker.ts#serve` (function, line 5) — callbackAssign by 2: `src/render/terrainLight.worker.ts#serve`, `src/render/ecology.worker.ts#serve`

### src/render/buildings/cellPool.ts

- `src/render/buildings/cellPool.ts#CellPool.onPage` (method, line 345) — callbackAssign by 1: `src/render/groundShape.ts#padsOfAsk`
- `src/render/buildings/cellPool.ts#CellPool.onPage` (method, line 345) — implementation by 1: `src/render/groundShape.ts#padsOfAsk`

### src/render/buildings/cellWork.ts

- `src/render/buildings/cellWork.ts#runCellJob` (function, line 119) — callbackAssign by 1: `src/render/groundShape.ts#padsOfAsk`
- `src/render/buildings/cellWork.ts#workerReads` (function, line 100) — callbackAssign by 1: `src/render/groundShape.ts#padsOfAsk`
- `src/render/buildings/cellWork.ts#workerReads` (function, line 100) — implementation by 1: `src/render/groundShape.ts#padsOfAsk`

### src/render/buildings/fracture.worker.ts

- `src/render/buildings/fracture.worker.ts#serve` (function, line 5) — callbackAssign by 2: `src/render/terrainLight.worker.ts#serve`, `src/render/ecology.worker.ts#serve`

### src/render/buildings/layer.ts

- `src/render/buildings/layer.ts#createBuildingLayer.update` (method, line 730) — implementation by 1: `src/render/groundShape.ts#padsOfAsk`

### src/render/candidateKeep.ts

- `src/render/candidateKeep.ts#KEEP_CELL` (variable, line 7) — read by 1: `src/render/elements.ts#createElementLayer.emit`
- `src/render/candidateKeep.ts#keepCellKey` (function, line 10) — call by 3: `src/render/plantCells.ts#plantLayer.sync`, `src/render/plantCells.ts#treesRound`, `src/render/elements.ts#createElementLayer.emit`
- `src/render/candidateKeep.ts#keepCellOf` (function, line 13) — call by 2: `src/render/plantCells.ts#plantLayer.sync`, `src/render/plantCells.ts#treesRound`

### src/render/cellInstances.ts

- `src/render/cellInstances.ts#CellInstances` (interface, line 124) — call by 1: `src/render/instanceRuns.ts#createInstanceRuns.setMap`
- `src/render/cellInstances.ts#createCellInstances.setMap` (method, line 1052) — implementation by 1: `src/render/instanceRuns.ts#createInstanceRuns.setMap`
- `src/render/cellInstances.ts#KindLevels` (interface, line 70) — typeRef by 1: `src/render/instanceRuns.ts#leastPixel`
- `src/render/cellInstances.ts#posedMaterial` (function, line 179) — call by 2: `src/render/natureTrees.ts#loadNatureTrees`, `src/render/scenery.ts#createSceneryKit`
- `src/render/cellInstances.ts#SlotFormat` (interface, line 96) — read by 3: `src/render/plantCells.ts#coverInto`, `src/render/plantCells.ts#treesInto`, `src/render/instanceRuns.ts#RunBuilder.add`
- `src/render/cellInstances.ts#SlotFormat` (interface, line 96) — typeRef by 2: `src/render/groundCover.ts#COVER_POSE`, `src/render/natureTrees.ts#TREE_POSE`

### src/render/cellKeeps.ts

- `src/render/cellKeeps.ts#keepCellAt` (function, line 7) — call by 2: `src/render/elements.ts#allByCell`, `src/render/elements.ts#createElementLayer.sync`
- `src/render/cellKeeps.ts#keepCellRect` (function, line 10) — call by 1: `src/render/elements.ts#createElementLayer.sync`

### src/render/changeBoxes.ts

- `src/render/changeBoxes.ts#ChangeBox` (type, line 19) — read by 1: `src/render/sunCascades.ts#createSunCascades.rectsOf`
- `src/render/changeBoxes.ts#ChangeBox` (type, line 19) — typeRef by 1: `src/render/sunCascades.ts#createSunCascades.rectsOf`

### src/render/citizenBake.worker.ts

- `src/render/citizenBake.worker.ts#serve` (function, line 89) — callbackAssign by 2: `src/render/terrainLight.worker.ts#serve`, `src/render/ecology.worker.ts#serve`

### src/render/depthConvention.ts

- `src/render/depthConvention.ts#DEPTH_GLSL` (variable, line 6) — read by 1: `src/render/canyonLight.ts#FRAGMENT`
- `src/render/depthConvention.ts#pullTowardEye` (function, line 28) — call by 1: `src/render/scenery.ts#createSceneryKit.poolMaterial`

### src/render/derivedWriter.worker.ts

- `src/render/derivedWriter.worker.ts#serve` (function, line 23) — callbackAssign by 2: `src/render/terrainLight.worker.ts#serve`, `src/render/ecology.worker.ts#serve`

### src/render/exhaust.ts

- `src/render/exhaust.ts#createExhaust.burst` (method, line 219) — implementation by 1: `src/render/elements.ts#createElementLayer.give`
- `src/render/exhaust.ts#Exhaust` (interface, line 22) — call by 1: `src/render/elements.ts#createElementLayer.give`
- `src/render/exhaust.ts#Exhaust` (interface, line 22) — typeRef by 2: `src/render/elements.ts#createElementLayer.give`, `src/render/elements.ts#ElementLayer`
- `src/render/exhaust.ts#PuffKind` (type, line 20) — typeRef by 1: `src/render/elements.ts#EFFECTS`

### src/render/flatAtlas.ts

- `src/render/flatAtlas.ts#createFlatAtlas` (function, line 231) — callbackAssign by 1: `src/render/terrain.ts#createTerrainSurface.levelsLanded`
- `src/render/flatAtlas.ts#createFlatAtlas` (function, line 231) — implementation by 3: `src/render/terrain.ts#createTerrainSurface`, `src/render/terrain.ts#createTerrainSurface.levelsLanded`, `src/render/terrain.ts#createTerrainSurface.paintStep`
- `src/render/flatAtlas.ts#createFlatAtlas.ensure` (function, line 431) — callbackAssign by 1: `src/render/terrain.ts#createTerrainSurface.levelsLanded`
- `src/render/flatAtlas.ts#createFlatAtlas.shaperFor` (function, line 701) — implementation by 2: `src/render/terrainShaping.ts#shapeCorners`, `src/render/terrainShaping.ts#shapeCorners.visit`

### src/render/flatGround.worker.ts

- `src/render/flatGround.worker.ts#serve` (function, line 7) — callbackAssign by 2: `src/render/terrainLight.worker.ts#serve`, `src/render/ecology.worker.ts#serve`

### src/render/gpuLedger.ts

- `src/render/gpuLedger.ts#geometryCost` (function, line 73) — call by 1: `src/render/terrain.ts#createTerrainSurface`
- `src/render/gpuLedger.ts#geometryCost` (function, line 73) — read by 1: `src/render/terrain.ts#createTerrainSurface`

### src/render/groundShape.worker.ts

- `src/render/groundShape.worker.ts#serve` (function, line 5) — callbackAssign by 2: `src/render/terrainLight.worker.ts#serve`, `src/render/ecology.worker.ts#serve`

### src/render/instanceRuns.ts

- `src/render/instanceRuns.ts#createInstanceRuns` (function, line 201) — call by 2: `src/render/plantCells.ts#plantLayer`, `src/render/elements.ts#createElementLayer`
- `src/render/instanceRuns.ts#createInstanceRuns` (function, line 201) — read by 1: `src/render/instanceRuns.ts#createInstanceRuns.setMap`
- `src/render/instanceRuns.ts#createInstanceRuns.cull` (method, line 214) — implementation by 2: `src/render/plantCells.ts#plantLayer.cull`, `src/render/elements.ts#createElementLayer.cull`
- `src/render/instanceRuns.ts#createInstanceRuns.dispose` (method, line 215) — implementation by 2: `src/render/plantCells.ts#plantLayer.dispose`, `src/render/elements.ts#createElementLayer.dispose`
- `src/render/instanceRuns.ts#createInstanceRuns.update` (method, line 209) — implementation by 2: `src/render/plantCells.ts#plantLayer.sync`, `src/render/elements.ts#createElementLayer.sync`
- `src/render/instanceRuns.ts#InstanceRuns` (interface, line 170) — call by 7: `src/render/plantCells.ts#plantLayer.cull`, `src/render/plantCells.ts#plantLayer.dispose`, `src/render/plantCells.ts#plantLayer.setMap`, `src/render/plantCells.ts#plantLayer.sync`, …
- `src/render/instanceRuns.ts#InstanceRuns` (interface, line 170) — read by 2: `src/render/plantCells.ts#plantLayer`, `src/render/elements.ts#createElementLayer`
- `src/render/instanceRuns.ts#PoseFormat` (type, line 16) — typeRef by 1: `src/render/plantCells.ts#plantLayer`
- `src/render/instanceRuns.ts#Run` (type, line 100) — typeRef by 5: `src/render/plantCells.ts#plantLayer`, `src/render/plantCells.ts#plantLayer.sync`, `src/render/elements.ts#createElementLayer`, `src/render/elements.ts#createElementLayer.cellOf`, …
- `src/render/instanceRuns.ts#RunBuilder` (class, line 111) — construct by 2: `src/render/plantCells.ts#plantLayer.sync`, `src/render/elements.ts#createElementLayer.cellOf`
- `src/render/instanceRuns.ts#RunBuilder` (class, line 111) — typeRef by 4: `src/render/plantCells.ts#coverInto`, `src/render/plantCells.ts#plantLayer`, `src/render/plantCells.ts#treesInto`, `src/render/elements.ts#elementsInto`
- `src/render/instanceRuns.ts#RunBuilder.build` (method, line 137) — call by 2: `src/render/plantCells.ts#plantLayer.sync`, `src/render/elements.ts#createElementLayer.cellOf`
- `src/render/instanceRuns.ts#RunBuilder.constructor` (constructor, line 115) — read by 1: `src/render/instanceRuns.ts#RunBuilder.add`
- `src/render/instanceRuns.ts#RunBuilder.listOf` (method, line 117) — call by 1: `src/render/instanceRuns.ts#RunBuilder.add`
- `src/render/instanceRuns.ts#RunBuilder.turned` (method, line 132) — call by 1: `src/render/elements.ts#elementsInto`
- `src/render/instanceRuns.ts#RunStore` (interface, line 69) — typeRef by 5: `src/render/plantCells.ts#createCoverCells`, `src/render/plantCells.ts#createCoverCells.shown`, `src/render/plantCells.ts#createTreeCells`, `src/render/plantCells.ts#plantLayer`, …
- `src/render/instanceRuns.ts#Tint` (interface, line 108) — typeRef by 1: `src/render/instanceRuns.ts#RunBuilder.add`
- `src/render/instanceRuns.ts#YAW_POSE` (variable, line 42) — read by 1: `src/render/scenery.ts#createSceneryKit`

### src/render/landStroke.ts

- `src/render/landStroke.ts#LevelsJob` (interface, line 12) — read by 1: `src/render/terrain.ts#createTerrainSurface.measureMoved`
- `src/render/landStroke.ts#LevelsJob` (interface, line 12) — typeRef by 1: `src/render/terrain.ts#createTerrainSurface.measureMoved`
- `src/render/landStroke.ts#NO_WATER` (variable, line 60) — read by 4: `src/render/terrain.ts#createTerrainSurface`, `src/render/terrain.ts#createTerrainSurface.wetCorners`, `src/render/terrain.ts#farLandMaterial`, `src/render/terrain.ts#landShader`
- `src/render/landStroke.ts#ShoreJob` (interface, line 46) — read by 1: `src/render/terrain.ts#createTerrainSurface.askShore`
- `src/render/landStroke.ts#ShoreJob` (interface, line 46) — typeRef by 1: `src/render/terrain.ts#createTerrainSurface.askShore`
- `src/render/landStroke.ts#shoreLevelsInto` (function, line 65) — call by 1: `src/render/terrain.ts#createTerrainSurface.askShore`
- `src/render/landStroke.ts#StrokeBox` (type, line 9) — typeRef by 4: `src/render/terrain.ts#createTerrainSurface`, `src/render/terrain.ts#createTerrainSurface.levelsLanded`, `src/render/terrain.ts#createTerrainSurface.measureOnPage`, `src/render/landStroke.ts#unionBox`
- `src/render/landStroke.ts#WoodsBlock` (interface, line 43) — read by 1: `src/render/landStroke.ts#writeWoodsBlock`
- `src/render/landStroke.ts#WoodsBlock` (interface, line 43) — typeRef by 1: `src/render/landStroke.ts#writeWoodsBlock`
- `src/render/landStroke.ts#woodsBlock.land` (function, line 116) — indirectCall by 1: `src/render/woodsBase.ts#woodsBaseInto`
- `src/render/landStroke.ts#WoodsJob` (interface, line 25) — read by 1: `src/render/terrain.ts#createTerrainSurface.woodsMoved`
- `src/render/landStroke.ts#WoodsJob` (interface, line 25) — typeRef by 1: `src/render/terrain.ts#createTerrainSurface.woodsMoved`

### src/render/landStroke.worker.ts

- `src/render/landStroke.worker.ts#serve` (function, line 5) — callbackAssign by 2: `src/render/terrainLight.worker.ts#serve`, `src/render/ecology.worker.ts#serve`

### src/render/landStrokePool.ts

- `src/render/landStrokePool.ts#askLandStroke` (function, line 10) — call by 3: `src/render/terrain.ts#createTerrainSurface.askShore`, `src/render/terrain.ts#createTerrainSurface.measureMoved`, `src/render/terrain.ts#createTerrainSurface.woodsMoved`

### src/render/lotOverlay.ts

- `src/render/lotOverlay.ts#applyLotOverlay` (function, line 208) — call by 1: `src/render/terrain.ts#terrainMaterial`

### src/render/mesh/cylinder.ts

- `src/render/mesh/cylinder.ts#cylinderWith` (function, line 16) — call by 5: `src/render/propGeometry.ts#binGeometry`, `src/render/propGeometry.ts#hydrantGeometry`, `src/render/propGeometry.ts#lampFarGeometry`, `src/render/propGeometry.ts#lampGeometry`, …

### src/render/mesh/gpuOnly.ts

- `src/render/mesh/gpuOnly.ts#fillArrays` (function, line 37) — call by 1: `src/render/farForest.ts#createFarForest.update`
- `src/render/mesh/gpuOnly.ts#gpuOnlyAttributes` (function, line 22) — call by 1: `src/render/farForest.ts#createFarForest.write`

### src/render/mesh/hiddenFaces.ts

- `src/render/mesh/hiddenFaces.ts#dropHiddenFaces` (function, line 22) — call by 1: `src/render/propGeometry.ts#merge`

### src/render/mesh/textureBaker.ts

- `src/render/mesh/textureBaker.ts#fbm` (function, line 32) — call by 11: `src/render/terrain.ts#strataTexture`, `src/render/water.ts#waterNormalTexels`, `src/render/bake/terrainRecipes.ts#ROCK_RECIPES.basaltFace`, `src/render/bake/terrainRecipes.ts#ROCK_RECIPES.basaltTop`, …
- `src/render/mesh/textureBaker.ts#makeNoise` (function, line 4) — call by 5: `src/render/terrain.ts#strataTexture`, `src/render/water.ts#waterNormalTexels`, `src/render/bake/terrainRecipes.ts#ROCK_RECIPES`, `src/render/bake/terrainRecipes.ts#terrainRecipes`, …
- `src/render/mesh/textureBaker.ts#SurfaceRecipe` (interface, line 81) — typeRef by 3: `src/render/bake/terrainRecipes.ts#ROCK_RECIPES`, `src/render/bake/terrainRecipes.ts#ROCK_RECIPES.layer`, `src/render/bake/terrainRecipes.ts#terrainRecipes`
- `src/render/mesh/textureBaker.ts#SurfaceRecipe` (interface, line 81) — write by 1: `src/render/bake/terrainRecipes.ts#terrainRecipes`

### src/render/networkFarRoads.worker.ts

- `src/render/networkFarRoads.worker.ts#serve` (function, line 5) — callbackAssign by 2: `src/render/terrainLight.worker.ts#serve`, `src/render/ecology.worker.ts#serve`

### src/render/people/lodWorker.ts

- `src/render/people/lodWorker.ts#serve` (function, line 82) — callbackAssign by 2: `src/render/terrainLight.worker.ts#serve`, `src/render/ecology.worker.ts#serve`

### src/render/plateMesh.ts

- `src/render/plateMesh.ts#landNormalsInto` (function, line 19) — call by 1: `src/render/plateTexels.ts#landTexelsInto`
- `src/render/plateMesh.ts#PlateMesh` (interface, line 5) — read by 1: `src/render/terrain.ts#createTerrainSurface`
- `src/render/plateMesh.ts#PlateMesh` (interface, line 5) — typeRef by 1: `src/render/terrain.ts#createTerrainSurface`

### src/render/propGeometry.ts

- `src/render/propGeometry.ts#BUSH_KINDS` (variable, line 158) — read by 4: `src/render/plantImpostors.ts#sceneryPictures`, `src/render/plantImpostors.ts#sceneryPictures.bushAt`, `src/render/scenery.ts#createSceneryKit`, `src/render/scenery.ts#plantSamples`
- `src/render/propGeometry.ts#BushKind` (type, line 157) — typeRef by 5: `src/render/plantImpostors.ts#PlantModels`, `src/render/plantImpostors.ts#SceneryPictures`, `src/render/scenery.ts#createSceneryKit`, `src/render/scenery.ts#SceneryKit`, …
- `src/render/propGeometry.ts#IRON` (variable, line 299) — read by 1: `src/render/propGeometry.ts#benchGeometry`
- `src/render/propGeometry.ts#LAMP_HEIGHT` (variable, line 233) — read by 3: `src/render/propGeometry.ts#lampFarGeometry`, `src/render/propGeometry.ts#lampGeometry`, `src/render/propGeometry.ts#lampLensGeometry`
- `src/render/propGeometry.ts#LAMP_METAL` (variable, line 236) — read by 2: `src/render/propGeometry.ts#lampFarGeometry`, `src/render/propGeometry.ts#lampGeometry`
- `src/render/propGeometry.ts#LAMP_OUTREACH` (variable, line 234) — read by 3: `src/render/propGeometry.ts#lampFarGeometry`, `src/render/propGeometry.ts#lampGeometry`, `src/render/propGeometry.ts#lampLensGeometry`
- `src/render/propGeometry.ts#LAMP_PLINTH` (variable, line 237) — read by 1: `src/render/propGeometry.ts#lampGeometry`
- `src/render/propGeometry.ts#rgb` (function, line 31) — call by 10: `src/render/propGeometry.ts#binGeometry`, `src/render/propGeometry.ts#drainGeometry`, `src/render/propGeometry.ts#grassTuftGeometry`, `src/render/propGeometry.ts#hydrantGeometry`, …
- `src/render/propGeometry.ts#Rgb` (type, line 29) — typeRef by 3: `src/render/propGeometry.ts#grassTuftGeometry`, `src/render/propGeometry.ts#part`, `src/render/propGeometry.ts#benchGeometry.slat`
- `src/render/propGeometry.ts#TREE_KIND` (variable, line 148) — read by 1: `src/render/propGeometry.ts#treeParts`
- `src/render/propGeometry.ts#TREE_SPECIES` (variable, line 145) — read by 5: `src/render/plantImpostors.ts#sceneryPictures`, `src/render/plantImpostors.ts#sceneryPictures.bushAt`, `src/render/plantImpostors.ts#sceneryPictures.treeAt`, `src/render/scenery.ts#createSceneryKit`, …
- `src/render/propGeometry.ts#TreeSpecies` (type, line 144) — typeRef by 5: `src/render/plantImpostors.ts#PlantModels`, `src/render/plantImpostors.ts#SceneryPictures`, `src/render/scenery.ts#createSceneryKit`, `src/render/scenery.ts#SceneryKit`, …
- `src/render/propGeometry.ts#WOOD` (variable, line 298) — read by 1: `src/render/propGeometry.ts#benchGeometry`

### src/render/renderer.ts

- `src/render/renderer.ts#createSceneRenderer` (function, line 432) — callbackAssign by 1: `src/render/terrain.ts#createTerrainSurface.levelsLanded`
- `src/render/renderer.ts#createSceneRenderer` (function, line 432) — implementation by 1: `src/render/sunCascades.ts#createSunCascades.pass`
- `src/render/renderer.ts#createSceneRenderer.buildWorld` (function, line 2317) — indirectCall by 1: `src/world/elevation.ts#buildRoadElevation`
- `src/render/renderer.ts#createSceneRenderer.completeSolve` (function, line 2225) — indirectCall by 1: `src/world/elevation.ts#buildRoadElevation`
- `src/render/renderer.ts#createSceneRenderer.warmRoadShaders` (function, line 2506) — indirectCall by 1: `src/world/elevation.ts#buildRoadElevation`

### src/render/roadStations.worker.ts

- `src/render/roadStations.worker.ts#serve` (function, line 15) — callbackAssign by 3: `src/render/terrainLight.worker.ts#serve`, `src/render/groundShape.ts#padsOfAsk`, `src/render/ecology.worker.ts#serve`

### src/render/roadTile.worker.ts

- `src/render/roadTile.worker.ts#serve` (function, line 102) — callbackAssign by 2: `src/render/terrainLight.worker.ts#serve`, `src/render/ecology.worker.ts#serve`

### src/render/sharedGround.ts

- `src/render/sharedGround.ts#GroundReaders` (interface, line 30) — read by 1: `src/render/groundShape.ts#padsOfAsk`
- `src/render/sharedGround.ts#PlateHeights` (interface, line 8) — read by 1: `src/render/terrain.ts#createTerrainSurface`
- `src/render/sharedGround.ts#PlateHeights` (interface, line 8) — typeRef by 1: `src/render/terrain.ts#createTerrainSurface`
- `src/render/sharedGround.ts#SharedGroundData` (interface, line 16) — typeRef by 1: `src/render/groundShape.ts#PadsAsk`
- `src/render/sharedGround.ts#sharedGroundReaders` (function, line 112) — call by 1: `src/render/groundShape.ts#padsOfAsk`
- `src/render/sharedGround.ts#sharedGroundReaders` (function, line 112) — callbackAssign by 1: `src/render/groundShape.ts#padsOfAsk`
- `src/render/sharedGround.ts#sharedGroundReaders` (function, line 112) — implementation by 1: `src/render/groundShape.ts#padsOfAsk`

### src/render/stagedTexels.ts

- `src/render/stagedTexels.ts#StagedTexels` (interface, line 52) — call by 7: `src/render/terrain.ts#createTerrainSurface`, `src/render/terrain.ts#createTerrainSurface.fitPaint`, `src/render/terrain.ts#createTerrainSurface.flushPaint`, `src/render/terrain.ts#createTerrainSurface.paintRaster`, …
- `src/render/stagedTexels.ts#StagedTexels` (interface, line 52) — read by 6: `src/render/terrain.ts#createTerrainSurface.applyLight`, `src/render/terrain.ts#createTerrainSurface.cornersMoved`, `src/render/terrain.ts#createTerrainSurface.ecologyUploaded`, `src/render/terrain.ts#createTerrainSurface.fitPaint`, …
- `src/render/stagedTexels.ts#StagedTexels` (interface, line 52) — typeRef by 1: `src/render/terrain.ts#createTerrainSurface.stagePaint`
- `src/render/stagedTexels.ts#stageTexels` (function, line 68) — call by 2: `src/render/terrain.ts#createTerrainSurface`, `src/render/terrain.ts#createTerrainSurface.stagePaint`
- `src/render/stagedTexels.ts#stageTexels` (function, line 68) — read by 1: `src/render/stagedTexels.ts#stageTexels.letGoWhenSent`
- `src/render/stagedTexels.ts#stageTexels` (function, line 68) — write by 2: `src/render/stagedTexels.ts#stageTexels.hold`, `src/render/stagedTexels.ts#stageTexels.letGoWhenSent`
- `src/render/stagedTexels.ts#TexelBox` (interface, line 10) — read by 4: `src/render/terrain.ts#createTerrainSurface.flushPaint`, `src/render/terrain.ts#unionTexels`, `src/render/plateTexels.ts#landTexelsInto`, `src/render/plateTexels.ts#groundCornersInto`
- `src/render/stagedTexels.ts#TexelBox` (interface, line 10) — typeRef by 5: `src/render/terrain.ts#createTerrainSurface`, `src/render/terrain.ts#paintTexelBox`, `src/render/terrain.ts#unionTexels`, `src/render/plateTexels.ts#landTexelsInto`, …
- `src/render/stagedTexels.ts#TexelPart` (interface, line 13) — read by 2: `src/render/terrain.ts#createTerrainSurface`, `src/render/terrain.ts#createTerrainSurface.stagePaint`

### src/render/streetFurnitureLayer.ts

- `src/render/streetFurnitureLayer.ts#meadowInto` (function, line 334) — indirectCall by 1: `src/render/grass.ts#grassClump`

### src/render/surfaceTextures.ts

- `src/render/surfaceTextures.ts#surfaceTexture` (function, line 77) — call by 2: `src/render/terrain.ts#terrainBakes`, `src/render/mesh/detailLayer.ts#detailTextures`

### src/render/torusWindow.ts

- `src/render/torusWindow.ts#TexelBand` (type, line 4) — typeRef by 2: `src/render/fogLayer.ts#createFogField.raster`, `src/render/torusWindow.ts#TorusWindow.bandOf`
- `src/render/torusWindow.ts#TorusWindow` (class, line 6) — construct by 1: `src/render/fogLayer.ts#createFogField`
- `src/render/torusWindow.ts#TorusWindow.constructor` (constructor, line 11) — read by 1: `src/render/torusWindow.ts#TorusWindow.bandOf`
- `src/render/torusWindow.ts#TorusWindow.follow` (method, line 46) — call by 1: `src/render/fogLayer.ts#createFogField.follow`
- `src/render/torusWindow.ts#TorusWindow.ox` (field, line 8) — read by 2: `src/render/fogLayer.ts#createFogField.layer`, `src/render/torusWindow.ts#TorusWindow.bandOf`
- `src/render/torusWindow.ts#TorusWindow.oy` (field, line 9) — read by 2: `src/render/fogLayer.ts#createFogField.layer`, `src/render/torusWindow.ts#TorusWindow.bandOf`
- `src/render/torusWindow.ts#TorusWindow.span` (getter, line 17) — call by 1: `src/render/fogLayer.ts#createFogField.layer`
- `src/render/torusWindow.ts#TorusWindow.texel` (method, line 22) — call by 1: `src/render/fogLayer.ts#createFogField.raster`
- `src/render/torusWindow.ts#TorusWindow.whole` (getter, line 41) — call by 1: `src/render/fogLayer.ts#createFogField.whole`

### src/render/uploads.ts

- `src/render/uploads.ts#cancelUploads` (function, line 17) — call by 1: `src/render/terrain.ts#createTerrainSurface.fitPaint`

### src/world/approach.ts

- `src/world/approach.ts#CROSSWALK_DEPTH` (variable, line 7) — read by 3: `src/world/landscape.ts#crossingAccessTester`, `src/world/landscape.ts#medianNose`, `src/world/landscape.ts#onCrossingAccess`

### src/world/bounds.ts

- `src/world/bounds.ts#PLATE_SIZE` (variable, line 9) — read by 3: `src/render/terrainGrid.ts#TERRAIN_SIZE`, `src/render/farGround.ts#farExtent`, `src/render/farGround.ts#farNodesFor.visit`
- `src/world/bounds.ts#plateOf` (function, line 51) — call by 2: `src/render/groundShape.ts#digestBlocks.naturalAt`, `src/render/farGround.ts#farNodesFor.visit`

### src/world/buildings/cameraSolids.ts

- `src/world/buildings/cameraSolids.ts#at` (function, line 78) — call by 1: `src/world/buildings/cameraSolids.ts#SolidFiling.visitCells`
- `src/world/buildings/cameraSolids.ts#CameraSolids` (interface, line 80) — call by 1: `src/render/canyonLight.ts#createCanyonLight.follow`
- `src/world/buildings/cameraSolids.ts#CameraSolids` (interface, line 80) — typeRef by 1: `src/render/canyonLight.ts#CanyonLight`
- `src/world/buildings/cameraSolids.ts#CELL` (variable, line 14) — read by 1: `src/world/buildings/cameraSolids.ts#SolidFiling.visitCells`
- `src/world/buildings/cameraSolids.ts#cellKey` (function, line 177) — call by 1: `src/world/buildings/cameraSolids.ts#SolidFiling.visitCells`
- `src/world/buildings/cameraSolids.ts#CLEARANCE` (variable, line 10) — read by 1: `src/world/buildings/cameraSolids.ts#SolidFiling.visitCells`
- `src/world/buildings/cameraSolids.ts#Rect` (type, line 102) — typeRef by 3: `src/render/canyonLight.ts#createCanyonLight.clearCells`, `src/render/canyonLight.ts#createCanyonLight.follow`, `src/world/buildings/cameraSolids.ts#SolidFiling.forEachInAny`
- `src/world/buildings/cameraSolids.ts#S_BX,S_BY,S_COS,S_SIN,S_X0,S_Y0,S_X1,S_Y1,S_FOOT,S_TOP` (variable, line 18) — read by 1: `src/world/buildings/cameraSolids.ts#SolidFiling.visitCells`
- `src/world/buildings/cameraSolids.ts#SolidFiling.cells` (field, line 200) — read by 1: `src/world/buildings/cameraSolids.ts#SolidFiling.visitCells`
- `src/world/buildings/cameraSolids.ts#SolidFiling.pages` (field, line 199) — read by 3: `src/world/buildings/cameraSolids.ts#SolidFiling.forEachIn`, `src/world/buildings/cameraSolids.ts#SolidFiling.forEachInAny`, `src/world/buildings/cameraSolids.ts#SolidFiling.visitCells`
- `src/world/buildings/cameraSolids.ts#SolidPages.page` (method, line 65) — call by 1: `src/world/buildings/cameraSolids.ts#SolidFiling.visitCells`
- `src/world/buildings/cameraSolids.ts#SolidPages.visitOnce` (method, line 53) — call by 1: `src/world/buildings/cameraSolids.ts#SolidFiling.visitCells`
- `src/world/buildings/cameraSolids.ts#SolidPages.walk` (method, line 47) — call by 2: `src/world/buildings/cameraSolids.ts#SolidFiling.forEachIn`, `src/world/buildings/cameraSolids.ts#SolidFiling.forEachInAny`
- `src/world/buildings/cameraSolids.ts#SolidVisit` (type, line 97) — typeRef by 4: `src/render/canyonLight.ts#createCanyonLight.layVolume`, `src/world/buildings/cameraSolids.ts#SolidFiling.forEachIn`, `src/world/buildings/cameraSolids.ts#SolidFiling.forEachInAny`, `src/world/buildings/cameraSolids.ts#SolidFiling.visitCells`

### src/world/buildings/pads.ts

- `src/world/buildings/pads.ts#BuildingPads` (interface, line 47) — typeRef by 1: `src/world/buildings/pads.ts#padShaper`
- `src/world/buildings/pads.ts#Grading` (class, line 330) — construct by 1: `src/world/buildings/pads.ts#padShaper`
- `src/world/buildings/pads.ts#Pad` (interface, line 32) — typeRef by 3: `src/render/groundShape.ts#GroundJobAnswer`, `src/render/groundShape.ts#padsOfAsk`, `src/world/buildings/pads.ts#padShaper`
- `src/world/buildings/pads.ts#padOf` (function, line 116) — call by 1: `src/render/groundShape.ts#padsOfAsk`
- `src/world/buildings/pads.ts#PadShapeCut` (interface, line 165) — read by 1: `src/world/buildings/pads.ts#padShaper`
- `src/world/buildings/pads.ts#PadShapeCut` (interface, line 165) — typeRef by 2: `src/render/groundShape.ts#GroundShapeAsk`, `src/world/buildings/pads.ts#padShaper`

### src/world/buildings/recordTable.ts

- `src/world/buildings/recordTable.ts#eachRecord` (function, line 508) — call by 1: `src/render/groundShape.ts#padsOfAsk`
- `src/world/buildings/recordTable.ts#RecordRef` (interface, line 468) — typeRef by 1: `src/render/groundShape.ts#PadsAsk`

### src/world/cells.ts

- `src/world/cells.ts#CellFiling.idsMeeting` (method, line 131) — call by 1: `src/world/elevation.ts#scopeIds`
- `src/world/cells.ts#CellFiling.idsMeetingAny` (method, line 169) — call by 2: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#roadsToRead`
- `src/world/cells.ts#CellFiling.rectOf` (method, line 120) — call by 1: `src/world/elevation.ts#pendingOrder`

### src/world/changes.ts

- `src/world/changes.ts#ChangeJournal` (class, line 156) — typeRef by 1: `src/render/terrain.ts#createTerrainSurface`
- `src/world/changes.ts#ChangeJournal.record` (method, line 263) — call by 8: `src/render/terrain.ts#createTerrainSurface.applyLight`, `src/world/doc.ts#RoadDoc.updateCloud`, `src/world/doc.ts#RoadDoc.addCloud`, `src/world/doc.ts#RoadDoc.addElements`, …
- `src/world/changes.ts#ChangeJournal.since` (method, line 309) — call by 1: `src/render/terrain.ts#createTerrainSurface`
- `src/world/changes.ts#ChangeJournal.version` (getter, line 198) — call by 1: `src/render/terrain.ts#createTerrainSurface.setSun`
- `src/world/changes.ts#ChangeRect` (type, line 4) — typeRef by 4: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#roadsToRead`, `src/world/elevation.ts#SolveScope`, `src/render/elements.ts#ElementSource`
- `src/world/changes.ts#rectAround` (function, line 147) — call by 2: `src/world/doc.ts#RoadDoc.updateCloud`, `src/world/doc.ts#RoadDoc.addCloud`

### src/world/cityGen/squares.ts

- `src/world/cityGen/squares.ts#planSquares` (function, line 100) — indirectCall by 1: `src/world/trees.ts#treeSeed`

### src/world/cityGen/wholeMap.ts

- `src/world/cityGen/wholeMap.ts#onMap` (function, line 22) — call by 1: `src/render/farWoods.ts#farWoods`

### src/world/doc.ts

- `src/world/doc.ts#circleRect` (function, line 3324) — call by 4: `src/world/doc.ts#RoadDoc.addFogDab`, `src/world/doc.ts#RoadDoc.addGullyDab`, `src/world/doc.ts#RoadDoc.cutTrees`, `src/world/doc.ts#RoadDoc.removeElements`
- `src/world/doc.ts#elementsBox` (function, line 3338) — call by 1: `src/world/doc.ts#RoadDoc.addElements`
- `src/world/doc.ts#ItemGrid.add` (method, line 3491) — call by 1: `src/world/doc.ts#RoadDoc.addElements`
- `src/world/doc.ts#ItemGrid.near` (method, line 3504) — call by 2: `src/world/doc.ts#RoadDoc.cutTrees`, `src/world/doc.ts#RoadDoc.removeElements`
- `src/world/doc.ts#ItemGrid.remove` (method, line 3496) — call by 2: `src/world/doc.ts#RoadDoc.cutTrees`, `src/world/doc.ts#RoadDoc.removeElements`
- `src/world/doc.ts#keepWhere` (function, line 3516) — call by 2: `src/world/doc.ts#RoadDoc.cutTrees`, `src/world/doc.ts#RoadDoc.removeElements`
- `src/world/doc.ts#placedCloud` (function, line 3314) — call by 1: `src/world/doc.ts#RoadDoc.addCloud`
- `src/world/doc.ts#RoadDoc` (class, line 153) — typeRef by 5: `src/editor/terrainBrush.ts#stampTerrain`, `src/editor/terrainBrush.ts#TerrainBrushHost`, `src/editor/streetscapeTool.ts#StreetscapeToolHost`, `src/editor/cloudTool.ts#CloudToolHost`, …
- `src/world/doc.ts#RoadDoc.addLandscape` (method, line 576) — call by 2: `src/editor/streetscapeTool.ts#StreetscapeTool.down`, `src/editor/streetscapeTool.ts#StreetscapeTool.up`
- `src/world/doc.ts#RoadDoc.addPaintDab` (method, line 1107) — call by 1: `src/editor/terrainBrush.ts#stampTerrain`
- `src/world/doc.ts#RoadDoc.addTerrainStamp` (method, line 1327) — call by 1: `src/editor/terrainBrush.ts#stampTerrain`
- `src/world/doc.ts#RoadDoc.allocatorMarks` (method, line 2229) — call by 1: `src/editor/terrainBrush.ts#TerrainBrush.begin`
- `src/world/doc.ts#RoadDoc.changes` (field, line 262) — read by 8: `src/render/terrain.ts#createTerrainSurface`, `src/world/doc.ts#RoadDoc.updateCloud`, `src/world/doc.ts#RoadDoc.addCloud`, `src/world/doc.ts#RoadDoc.addElements`, …
- `src/world/doc.ts#RoadDoc.clouds` (field, line 216) — read by 4: `src/editor/cloudTool.ts#CloudTool.down`, `src/editor/cloudTool.ts#CloudTool.move`, `src/world/doc.ts#RoadDoc.updateCloud`, `src/world/doc.ts#RoadDoc.addCloud`
- `src/world/doc.ts#RoadDoc.clouds` (field, line 216) — write by 2: `src/world/doc.ts#RoadDoc.updateCloud`, `src/world/doc.ts#RoadDoc.addCloud`
- `src/world/doc.ts#RoadDoc.elementCells` (getter, line 247) — call by 1: `src/world/doc.ts#RoadDoc.removeElements`
- `src/world/doc.ts#RoadDoc.elementGrid` (field, line 245) — read by 1: `src/world/doc.ts#RoadDoc.addElements`
- `src/world/doc.ts#RoadDoc.elementGrid` (field, line 245) — write by 1: `src/world/doc.ts#RoadDoc.addElements`
- `src/world/doc.ts#RoadDoc.elements` (field, line 243) — read by 2: `src/world/doc.ts#RoadDoc.addElements`, `src/world/doc.ts#RoadDoc.removeElements`
- `src/world/doc.ts#RoadDoc.elements` (field, line 243) — write by 1: `src/world/doc.ts#RoadDoc.addElements`
- `src/world/doc.ts#RoadDoc.elementsIn` (method, line 256) — call by 1: `src/editor/terrainBrush.ts#stampTerrain`
- `src/world/doc.ts#RoadDoc.fogDabs` (field, line 210) — read by 1: `src/world/doc.ts#RoadDoc.addFogDab`
- `src/world/doc.ts#RoadDoc.fogDabs` (field, line 210) — write by 1: `src/world/doc.ts#RoadDoc.addFogDab`
- `src/world/doc.ts#RoadDoc.gullyDabs` (field, line 213) — read by 1: `src/world/doc.ts#RoadDoc.addGullyDab`
- `src/world/doc.ts#RoadDoc.gullyDabs` (field, line 213) — write by 1: `src/world/doc.ts#RoadDoc.addGullyDab`
- `src/world/doc.ts#RoadDoc.keepCounts` (method, line 1101) — call by 4: `src/world/doc.ts#RoadDoc.addElements`, `src/world/doc.ts#RoadDoc.addFogDab`, `src/world/doc.ts#RoadDoc.addGullyDab`, `src/world/doc.ts#RoadDoc.cutTrees`
- `src/world/doc.ts#RoadDoc.landscapeIn` (method, line 605) — call by 1: `src/editor/streetscapeTool.ts#itemsAround`
- `src/world/doc.ts#RoadDoc.nature` (field, line 206) — read by 1: `src/render/terrain.ts#createTerrainSurface.updatePaint`
- `src/world/doc.ts#RoadDoc.natureRevision` (getter, line 381) — call by 1: `src/render/terrain.ts#createTerrainSurface.updatePaint`
- `src/world/doc.ts#RoadDoc.node` (method, line 416) — call by 8: `src/editor/streetscapeTool.ts#StreetscapeTool.down`, `src/world/landscape.ts#crossingAccessesAt`, `src/world/landscape.ts#medianNose`, `src/world/elevation.ts#buildRoadElevation`, …
- `src/world/doc.ts#RoadDoc.nodes` (field, line 154) — read by 1: `src/world/landscape.ts#crossingAccessesNow`
- `src/world/doc.ts#RoadDoc.ownList` (method, line 1969) — call by 7: `src/world/doc.ts#RoadDoc.updateCloud`, `src/world/doc.ts#RoadDoc.addCloud`, `src/world/doc.ts#RoadDoc.addElements`, `src/world/doc.ts#RoadDoc.addFogDab`, …
- `src/world/doc.ts#RoadDoc.paintRevision` (getter, line 382) — call by 1: `src/render/terrain.ts#createTerrainSurface.paintStep`
- `src/world/doc.ts#RoadDoc.plantTrees` (method, line 1168) — call by 1: `src/editor/terrainBrush.ts#stampTerrain`
- `src/world/doc.ts#RoadDoc.removeCloud` (method, line 1297) — call by 1: `src/editor/cloudTool.ts#CloudTool.down`
- `src/world/doc.ts#RoadDoc.removeLandscape` (method, line 596) — call by 1: `src/editor/streetscapeTool.ts#StreetscapeTool.down`
- `src/world/doc.ts#RoadDoc.requireSegment` (method, line 430) — call by 2: `src/world/landscape.ts#snapLandscape`, `src/world/network.ts#Network.crossingLandingAt`
- `src/world/doc.ts#RoadDoc.segment` (method, line 420) — call by 7: `src/world/landscape.ts#footwayAt`, `src/world/landscape.ts#medianAt`, `src/world/landscape.ts#nearCrossingAccess`, `src/world/landscape.ts#onCrossingAccess`, …
- `src/world/doc.ts#RoadDoc.setNodeApproachRules` (method, line 1027) — call by 1: `src/editor/streetscapeTool.ts#StreetscapeTool.down`
- `src/world/doc.ts#RoadDoc.setNodeControl` (method, line 1347) — call by 1: `src/editor/streetscapeTool.ts#StreetscapeTool.down`
- `src/world/doc.ts#RoadDoc.terrainPaint` (field, line 208) — read by 1: `src/render/terrain.ts#createTerrainSurface.paintStep`
- `src/world/doc.ts#RoadDoc.terrainRelief` (field, line 202) — read by 1: `src/render/terrain.ts#createTerrainSurface`
- `src/world/doc.ts#RoadDoc.terrainRevision` (getter, line 374) — call by 1: `src/render/terrain.ts#createTerrainSurface`
- `src/world/doc.ts#RoadDoc.terrainStamps` (field, line 200) — read by 1: `src/render/terrain.ts#createTerrainSurface`
- `src/world/doc.ts#RoadDoc.treeCells` (getter, line 226) — call by 1: `src/world/doc.ts#RoadDoc.cutTrees`
- `src/world/doc.ts#RoadDoc.treeClearings` (field, line 241) — read by 1: `src/world/doc.ts#RoadDoc.cutTrees`
- `src/world/doc.ts#RoadDoc.treeClearings` (field, line 241) — write by 1: `src/world/doc.ts#RoadDoc.cutTrees`
- `src/world/doc.ts#RoadDoc.trees` (field, line 222) — read by 1: `src/world/doc.ts#RoadDoc.cutTrees`
- `src/world/doc.ts#RoadDoc.treesIn` (method, line 235) — call by 1: `src/editor/terrainBrush.ts#stampTerrain`
- `src/world/doc.ts#RoadDoc.withRoom` (method, line 1087) — call by 4: `src/world/doc.ts#RoadDoc.addElements`, `src/world/doc.ts#RoadDoc.addFogDab`, `src/world/doc.ts#RoadDoc.addGullyDab`, `src/world/doc.ts#RoadDoc.cutTrees`
- `src/world/doc.ts#RoadNode` (interface, line 89) — read by 8: `src/editor/streetscapeTool.ts#StreetscapeTool.down`, `src/world/landscape.ts#crossingAccessesAt`, `src/world/landscape.ts#medianNose`, `src/world/elevation.ts#buildRoadElevation`, …
- `src/world/doc.ts#RoadSegment` (interface, line 124) — read by 8: `src/world/landscape.ts#footwayAt`, `src/world/landscape.ts#medianAt`, `src/world/landscape.ts#nearCrossingAccess`, `src/world/landscape.ts#onCrossingAccess`, …

### src/world/elements.ts

- `src/world/elements.ts#EFFECT_KINDS` (variable, line 4) — read by 1: `src/world/elements.ts#isEffectKind`
- `src/world/elements.ts#EFFECT_KINDS` (variable, line 4) — typeRef by 1: `src/world/elements.ts#EffectKind`
- `src/world/elements.ts#ElementBrush` (interface, line 29) — read by 2: `src/editor/terrainBrush.ts#stampTerrain`, `src/world/elements.ts#scatter`
- `src/world/elements.ts#ElementBrush` (interface, line 29) — typeRef by 2: `src/editor/terrainBrush.ts#DabSettings`, `src/world/elements.ts#scatter`
- `src/world/elements.ts#ElementItem` (interface, line 12) — read by 7: `src/editor/terrainBrush.ts#stampTerrain`, `src/render/elements.ts#allByCell`, `src/render/elements.ts#createElementLayer.give`, `src/render/elements.ts#createElementLayer.sync`, …
- `src/world/elements.ts#ElementItem` (interface, line 12) — typeRef by 8: `src/render/elements.ts#allByCell`, `src/render/elements.ts#createElementLayer.cellOf`, `src/render/elements.ts#createElementLayer.sync`, `src/render/elements.ts#elementsInto`, …
- `src/world/elements.ts#ElementKind` (type, line 6) — typeRef by 10: `src/editor/terrainBrush.ts#DabSettings`, `src/render/elements.ts#CASTS`, `src/render/elements.ts#createElementLayer`, `src/render/elements.ts#ElementKit`, …

### src/world/geometry.ts

- `src/world/geometry.ts#orientedPolyline` (function, line 33) — call by 1: `src/world/network.ts#Network.crossingLandingAt`
- `src/world/geometry.ts#PolylineCache.get` (method, line 70) — call by 1: `src/world/roads/derivedSigns.ts#signApproach`

### src/world/ids.ts

- `src/world/ids.ts#NodeId` (type, line 7) — typeRef by 17: `src/world/landscape.ts#crossingAccessesAt`, `src/world/landscape.ts#medianNose`, `src/world/landscape.ts#nearCrossingAccess`, `src/world/elevation.ts#buildRoadElevation`, …
- `src/world/ids.ts#SegmentId` (type, line 8) — typeRef by 22: `src/world/landscape.ts#crossingAccessTester`, `src/world/landscape.ts#FootwayHit`, `src/world/landscape.ts#MedianHit`, `src/world/landscape.ts#medianNose`, …

### src/world/junction/levels.ts

- `src/world/junction/levels.ts#bit` (function, line 169) — call by 1: `src/world/junction/levels.ts#JunctionLevels.has`
- `src/world/junction/levels.ts#JunctionLevels.constructor` (constructor, line 186) — read by 1: `src/world/junction/levels.ts#JunctionLevels.has`
- `src/world/junction/levels.ts#JunctionLevels.legsAt` (method, line 269) — call by 1: `src/world/elevation.ts#buildRoadElevation`

### src/world/junction/types.ts

- `src/world/junction/types.ts#Leg` (interface, line 14) — read by 1: `src/world/elevation.ts#buildRoadElevation`

### src/world/network.ts

- `src/world/network.ts#CrossingLanding` (interface, line 37) — typeRef by 1: `src/world/network.ts#Network.crossingLandingAt`
- `src/world/network.ts#Network` (class, line 46) — typeRef by 22: `src/editor/streetscapeTool.ts#StreetscapeToolHost`, `src/world/landscape.ts#accessMemo`, `src/world/landscape.ts#crossingAccesses`, `src/world/landscape.ts#crossingAccessesAt`, …
- `src/world/network.ts#Network.changesSince` (method, line 143) — call by 2: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#roadsToRead`
- `src/world/network.ts#Network.constructor` (constructor, line 100) — read by 14: `src/world/landscape.ts#crossingAccessesAt`, `src/world/landscape.ts#crossingAccessesNow`, `src/world/landscape.ts#footwayAt`, `src/world/landscape.ts#medianAt`, …
- `src/world/network.ts#Network.crossingLandings` (field, line 52) — read by 1: `src/world/network.ts#Network.crossingLandingAt`
- `src/world/network.ts#Network.crossingLandings` (field, line 52) — write by 1: `src/world/network.ts#Network.crossingLandingAt`
- `src/world/network.ts#Network.crosswalkDistanceAt` (method, line 324) — call by 2: `src/world/landscape.ts#medianNose`, `src/world/network.ts#Network.crossingLandingAt`
- `src/world/network.ts#Network.forkedFrom` (field, line 84) — read by 1: `src/world/elevation.ts#flatSolvesOf`
- `src/world/network.ts#Network.isBuilt` (method, line 170) — call by 2: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#roadsToRead`
- `src/world/network.ts#Network.junctions` (field, line 54) — read by 1: `src/world/elevation.ts#buildRoadElevation`
- `src/world/network.ts#Network.mouthDistance` (method, line 288) — call by 3: `src/world/landscape.ts#footwayAt`, `src/world/landscape.ts#medianAt`, `src/world/roads/derivedSigns.ts#signApproach`
- `src/world/network.ts#Network.polylines` (field, line 80) — read by 1: `src/world/roads/derivedSigns.ts#signApproach`
- `src/world/network.ts#Network.revision` (field, line 87) — read by 1: `src/world/landscape.ts#crossingAccesses`
- `src/world/network.ts#Network.ribbons` (field, line 55) — read by 5: `src/editor/streetscapeTool.ts#rowPieces`, `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#everyRoad`, `src/world/elevation.ts#roadsToRead`, …
- `src/world/network.ts#Network.ribbonsMeeting` (method, line 62) — call by 2: `src/world/landscape.ts#nearCrossingAccess`, `src/world/landscape.ts#ribbonsNear`
- `src/world/network.ts#Network.segmentCells` (field, line 57) — read by 4: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#pendingOrder`, `src/world/elevation.ts#roadsToRead`, `src/world/elevation.ts#scopeIds`
- `src/world/network.ts#Network.state` (getter, line 148) — call by 1: `src/world/elevation.ts#buildRoadElevation`
- `src/world/network.ts#Network.trafficRevision` (field, line 89) — read by 1: `src/world/landscape.ts#crossingAccesses`
- `src/world/network.ts#Network.transitions` (field, line 79) — read by 1: `src/world/elevation.ts#buildRoadElevation`
- `src/world/network.ts#Network.trims` (field, line 75) — read by 1: `src/world/elevation.ts#buildRoadElevation`

### src/world/networkDelta.ts

- `src/world/networkDelta.ts#NetworkDelta` (interface, line 5) — read by 2: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#roadsToRead`
- `src/world/networkDelta.ts#NetworkDelta` (interface, line 5) — typeRef by 1: `src/world/elevation.ts#buildRoadElevation`

### src/world/paving.ts

- `src/world/paving.ts#PavingData` (interface, line 27) — typeRef by 1: `src/render/groundShape.ts#PadsAsk`
- `src/world/paving.ts#pavingReader` (function, line 41) — call by 1: `src/render/groundShape.ts#padsOfAsk`

### src/world/pedestrianAccess.ts

- `src/world/pedestrianAccess.ts#carriesPedestrians` (function, line 4) — call by 1: `src/world/landscape.ts#footwayAt`

### src/world/ribbons.ts

- `src/world/ribbons.ts#SegmentRibbon` (interface, line 25) — read by 8: `src/editor/streetscapeTool.ts#itemsAround`, `src/editor/streetscapeTool.ts#rowPieces`, `src/world/landscape.ts#footwayAt`, `src/world/landscape.ts#medianAt`, …
- `src/world/ribbons.ts#SegmentRibbon` (interface, line 25) — typeRef by 1: `src/world/landscape.ts#ribbonsNear`
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
- `src/world/roadField.ts#roadField` (function, line 417) — implementation by 3: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#roadShaper`, `src/render/groundShape.ts#blockDigest`
- `src/world/roadField.ts#roadField.onSegment` (method, line 601) — implementation by 1: `src/world/elevation.ts#buildRoadElevation`
- `src/world/roadField.ts#roadField.remembering` (method, line 591) — implementation by 1: `src/world/elevation.ts#buildRoadElevation`
- `src/world/roadField.ts#roadField.roadAt` (method, line 624) — implementation by 4: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#roadShaper`, `src/render/grass.ts#grassClump`, `src/render/groundShape.ts#blockDigest`
- `src/world/roadField.ts#roadField.surfaceFrameAt` (method, line 642) — implementation by 1: `src/world/elevation.ts#buildRoadElevation`
- `src/world/roadField.ts#roadField.surfaceFrameInto` (method, line 645) — implementation by 1: `src/world/elevation.ts#buildRoadElevation`
- `src/world/roadField.ts#RoadReads` (interface, line 96) — call by 2: `src/render/grass.ts#grassClump`, `src/render/groundShape.ts#blockDigest`
- `src/world/roadField.ts#RoadReads` (interface, line 96) — inherit by 1: `src/world/elevation.ts#RoadElevation`
- `src/world/roadField.ts#RoadReads` (interface, line 96) — reference by 2: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#roadShaper`
- `src/world/roadField.ts#RoadSample` (interface, line 57) — read by 2: `src/render/grass.ts#grassClump`, `src/render/groundShape.ts#blockDigest`
- `src/world/roadField.ts#smoothstep` (function, line 125) — call by 3: `src/world/elevation.ts#roadShapeAt`, `src/world/elevation.ts#solveGround`, `src/world/elevation.ts#solveRaised`
- `src/world/roadField.ts#SpatialIndex` (class, line 153) — construct by 1: `src/world/elevation.ts#roadShaper`
- `src/world/roadField.ts#SpatialIndex` (class, line 153) — read by 1: `src/world/roadField.ts#SpatialIndex.constructor`
- `src/world/roadField.ts#SpatialIndex` (class, line 153) — typeRef by 7: `src/world/roadField.ts#SpatialIndex.near`, `src/world/roadField.ts#SpatialIndex.all`, `src/world/roadField.ts#SpatialIndex.buckets`, `src/world/roadField.ts#SpatialIndex.constructor`, …
- `src/world/roadField.ts#STRIDE` (variable, line 654) — read by 1: `src/world/roadField.ts#packField`
- `src/world/roadField.ts#unpackField` (function, line 714) — call by 1: `src/world/elevation.ts#roadShaper`

### src/world/roads/derivedSigns.ts

- `src/world/roads/derivedSigns.ts#APPROACH_REACH` (variable, line 64) — read by 1: `src/world/roads/derivedSigns.ts#signApproach`

### src/world/roads/furnitureCatalog.ts

- `src/world/roads/furnitureCatalog.ts#FURNITURE_CATALOG` (variable, line 17) — read by 1: `src/editor/streetscapeTool.ts#ROW_SPACING`
- `src/world/roads/furnitureCatalog.ts#FurnitureEntry` (interface, line 6) — read by 1: `src/editor/streetscapeTool.ts#ROW_SPACING`

### src/world/roads/furnitureSets.ts

- `src/world/roads/furnitureSets.ts#furnitureFor` (function, line 71) — callbackAssign by 2: `src/world/landscape.ts#snapLandscape`, `src/world/landscape.ts#snapLandscape.others`
- `src/world/roads/furnitureSets.ts#furnitureFor` (function, line 71) — implementation by 2: `src/world/landscape.ts#snapLandscape`, `src/world/landscape.ts#snapLandscape.others`

### src/world/roads/rules.ts

- `src/world/roads/rules.ts#ApproachRuleEntry` (interface, line 11) — read by 1: `src/editor/streetscapeTool.ts#StreetscapeTool.down`
- `src/world/roads/rules.ts#arrivesAt` (function, line 137) — call by 1: `src/world/roads/derivedSigns.ts#signApproach`

### src/world/roads/tuning.ts

- `src/world/roads/tuning.ts#ROAD_TUNING` (variable, line 4) — read by 7: `src/world/elevation.ts#DECK_GRADE`, `src/world/elevation.ts#DECK_TIE`, `src/world/elevation.ts#GROUND_GRADE`, `src/world/elevation.ts#LIFT_OFF`, …

### src/world/roadSection.ts

- `src/world/roadSection.ts#SectionMaterials` (interface, line 14) — read by 1: `src/world/landscape.ts#plantedMedian`

### src/world/roadTypes.ts

- `src/world/roadTypes.ts#CASING_BAND` (variable, line 95) — read by 1: `src/world/elevation.ts#roadShapeAt`
- `src/world/roadTypes.ts#casingHalf` (function, line 384) — call by 2: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#roadsToRead`
- `src/world/roadTypes.ts#CURB_BAND` (variable, line 90) — read by 1: `src/render/propGeometry.ts#drainGeometry`
- `src/world/roadTypes.ts#FOOTWAY_RISE` (variable, line 93) — read by 2: `src/world/elevation.ts#solveRaised`, `src/render/propGeometry.ts#drainGeometry`
- `src/world/roadTypes.ts#footwayOn` (function, line 356) — call by 1: `src/world/elevation.ts#buildRoadElevation`
- `src/world/roadTypes.ts#halfWidth` (function, line 339) — call by 1: `src/editor/streetscapeTool.ts#itemsAround`
- `src/world/roadTypes.ts#Level` (enum, line 10) — reference by 2: `src/editor/streetscapeTool.ts#itemsAround`, `src/world/elevation.ts#buildRoadElevation`
- `src/world/roadTypes.ts#roadProfile` (function, line 248) — call by 1: `src/world/network.ts#Network.crossingLandingAt`
- `src/world/roadTypes.ts#RoadSide` (type, line 353) — typeRef by 1: `src/world/landscape.ts#depthFor`
- `src/world/roadTypes.ts#RoadType` (interface, line 29) — read by 7: `src/world/landscape.ts#footwayAt`, `src/world/landscape.ts#medianAt`, `src/world/landscape.ts#onAnyRoad`, `src/world/landscape.ts#plantedMedian`, …
- `src/world/roadTypes.ts#RoadType` (interface, line 29) — typeRef by 4: `src/world/landscape.ts#depthFor`, `src/world/landscape.ts#FootwayHit`, `src/world/landscape.ts#plantedMedian`, `src/world/elevation.ts#lookDigest`
- `src/world/roadTypes.ts#SurfaceLevel` (type, line 25) — typeRef by 1: `src/world/junction/levels.ts#JunctionLevels.has`
- `src/world/roadTypes.ts#travelShift` (function, line 403) — call by 1: `src/world/landscape.ts#medianAt`

### src/world/section.ts

- `src/world/section.ts#Band` (interface, line 8) — read by 2: `src/world/landscape.ts#depthFor`, `src/world/landscape.ts#footwayAt`
- `src/world/section.ts#BENCH_ZONE` (variable, line 56) — read by 1: `src/world/landscape.ts#depthFor`
- `src/world/section.ts#LAMP_ZONE` (variable, line 54) — read by 1: `src/world/landscape.ts#depthFor`
- `src/world/section.ts#MIN_THROUGH` (variable, line 60) — read by 1: `src/world/landscape.ts#depthFor`
- `src/world/section.ts#sectionOf` (function, line 67) — call by 2: `src/world/landscape.ts#depthFor`, `src/world/landscape.ts#footwayAt`
- `src/world/section.ts#SideZones` (interface, line 14) — read by 2: `src/world/landscape.ts#depthFor`, `src/world/landscape.ts#footwayAt`
- `src/world/section.ts#TREE_KERB_SETBACK` (variable, line 50) — read by 1: `src/world/landscape.ts#depthFor`
- `src/world/section.ts#TREE_PIT` (variable, line 49) — read by 3: `src/world/landscape.ts#depthFor`, `src/world/landscape.ts#LANDSCAPE_RADIUS`, `src/render/propGeometry.ts#treePitGeometry`
- `src/world/section.ts#zonesOn` (function, line 80) — call by 2: `src/world/landscape.ts#depthFor`, `src/world/landscape.ts#footwayAt`

### src/world/streetFurniture.ts

- `src/world/streetFurniture.ts#FurnitureKind` (type, line 11) — typeRef by 1: `src/render/scenery.ts#SceneryKit`

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

- `es:Math` × 1171
- `es:Math.max` × 318
- `es:Math.min` × 256
- `es:Math.floor` × 176
- `es:Array.push` × 169
- `es:Float32Array` × 169
- `es:Array.length` × 164
- `three:Vector3` × 154
- `three:Vector3.y` × 111
- `es:Number.toFixed` × 87
- `es:Infinity` × 79
- `es:ReadonlyArray.length` × 76
- `three:BufferGeometry` × 73
- `es:Float64Array` × 71
- `es:Map.get` × 67
- `es:Uint8Array` × 66
- `es:Math.abs` × 65
- `three:Vector3.x` × 62
- `three:Vector3.z` × 59
- `es:Math.round` × 58
- `es:Math.PI` × 56
- `es:Map` × 54
- `es:Map.set` × 54
- `es:Set.add` × 53
- `es:String.replace` × 53
- `es:Math.hypot` × 49
- `three:Color` × 49
- `es:Array.map` × 47
- `es:Math.sin` × 46
- `three:BufferGeometry.setAttribute` × 44
- `es:Record` × 42
- `es:Math.ceil` × 41
- `three:MeshStandardMaterial` × 41
- `es:Math.cos` × 37
- `es:Set` × 37
- `three:BufferAttribute` × 37
- `es:ReadonlyArray.map` × 32
- `three:BufferGeometry.dispose` × 32
- `es:Math.imul` × 31
- `three:DirectionalLight.shadow` × 30
- `three:Matrix4` × 30
- `three:Material.dispose` × 28
- `es:Object` × 27
- `three:WebGLProgramParameters.fragmentShader` × 26
- `three:BoxGeometry` × 25
- `three:Vector3.set` × 25
- `three:Vector3.normalize` × 24
- `three:Object3D` × 23
- `three:Texture` × 23
- `three:DataTexture` × 22
- `three:Material.onBeforeCompile` × 22
- `three:Vector3.addScaledVector` × 22
- `es:ArrayLike` × 21
- `es:Number` × 21
- `es:Set.has` × 21
- `three:Light.intensity` × 21
- `three:Texture.needsUpdate` × 21
- `three:Vector3.clone` × 21
- `three:Vector3.copy` × 20
- `three:WebGLProgramParameters.vertexShader` × 20
- `three:BufferAttribute.count` × 19
- `es:ArrayBuffer` × 18
- `es:Error` × 18
- `es:Float64Array.length` × 18
- `three:Material.customProgramCacheKey` × 18
- `three:Object3D.visible` × 18
- `three:Box3` × 17
- `three:InstancedMesh` × 17
- `three:BufferGeometry.computeBoundingSphere` × 16
- `three:Mesh` × 16
- `three:Object3D.name` × 16
- `three:ShaderMaterial` × 16
- `es:Map.values` × 15
- `es:Promise` × 15
- `es:Readonly` × 15
- `three:Material` × 15
- `three:Object3D.add` × 15
- `three:ShaderChunk` × 15
- `es:CallableFunction.bind` × 14
- `es:Partial` × 14
- `three:BufferGeometry.getAttribute` × 14
- `three:DoubleSide` × 14
- `three:IUniform.value` × 14
- `three:Object3D.position` × 14
- `three:RenderTarget.depthTexture` × 14
- `es:Math.sqrt` × 13
- `three:Color.setRGB` × 13
- `three:CylinderGeometry` × 13
- `three:LightShadow.map` × 13
- `three:Mesh.geometry` × 13
- `three:Vector3.multiplyScalar` × 13
- `three:WebGLRenderTarget` × 13
- `dom:console` × 12
- `dom:MessageEvent.data` × 12
- `dom:performance` × 12
- `es:Array` × 12
- `es:Float32Array.length` × 12
- `es:Iterable` × 12
- `es:NumberConstructor.isFinite` × 12
- `es:ReadonlyArray.includes` × 12
- `three:Color.b` × 12
- `three:Color.g` × 12
- `three:Color.r` × 12
- `three:InstancedMesh.instanceColor` × 12
- `three:MeshDepthMaterial` × 12
- `three:NearestFilter` × 12
- `three:Texture.dispose` × 12
- `three:Vector2` × 12
- `three:Vector3.add` × 12
- `three:WebGLRenderer.clear` × 12
- `dom:Console.warn` × 11
- `es:Array.slice` × 11
- `es:Int32Array` × 11
- `es:Uint8Array.fill` × 11
- `three:Box3.min` × 11
- `three:Float32BufferAttribute` × 11
- `three:Group` × 11
- `three:InstancedBufferAttribute` × 11
- `three:Vector4.set` × 11
- `dom:Performance.now` × 10
- `es:Array.forEach` × 10
- `es:Float32Array.fill` × 10
- `es:Map.delete` × 10
- `es:NaN` × 10
- `three:Box3.max` × 10
- `three:BufferGeometry.boundingSphere` × 10
- `three:InstancedMesh.instanceMatrix` × 10
- `three:LightShadow.mapSize` × 10
- `three:LinearFilter` × 10
- `three:Object3D.castShadow` × 10
- `three:OrthographicCamera` × 10
- `three:RenderTarget.dispose` × 10
- `three:Scene` × 10
- `three:Texture.image` × 10
- `three:Vector2.x` × 10
- `three:Vector4` × 10
- `es:Array.filter` × 9
- `es:Array.sort` × 9
- `es:ArrayConstructor.from` × 9
- `es:Float64Array.buffer` × 9
- `es:Map.has` × 9
- `es:Map.keys` × 9
- `es:String.includes` × 9
- `es:Uint16Array` × 9
- `three:BufferGeometry.setIndex` × 9
- `three:Camera` × 9
- `three:Color.lerp` × 9
- `three:DirectionalLight` × 9
- `three:Object3D.frustumCulled` × 9
- `three:RenderTarget.textures` × 9
- `three:UnsignedByteType` × 9
- `three:Vector2.set` × 9
- `three:Vector3.fromBufferAttribute` × 9
- `dom:CanvasPath.moveTo` × 8
- `es:Float32Array.buffer` × 8
- `es:Float64Array.set` × 8
- `es:ObjectConstructor.assign` × 8
- `es:ObjectConstructor.values` × 8
- `es:ReadonlyArray.forEach` × 8
- `es:ReadonlySet` × 8
- `es:WeakMap.get` × 8
- `three:DataTexture.magFilter` × 8
- `three:DataTexture.minFilter` × 8
- `three:DataUtils` × 8
- `three:GLSL3` × 8
- `three:MaterialProperties.userData` × 8
- `three:MeshBasicMaterial` × 8
- `three:Object3D.updateMatrixWorld` × 8
- `three:RedFormat` × 8
- `three:RenderTarget.scissor` × 8
- `three:RepeatWrapping` × 8
- `three:RGBAFormat` × 8
- `three:WebGLProgramParametersWithUniforms.uniforms` × 8
- `three:WebGLRenderer` × 8
- `es:Array.includes` × 7
- `es:Pick` × 7
- `three:BufferGeometry.computeBoundingBox` × 7
- `three:DataTexture.generateMipmaps` × 7
- `three:FrontSide` × 7
- `three:Object3D.children` × 7
- `three:Object3D.matrixWorld` × 7
- `three:PerspectiveCamera` × 7
- `three:Vector3.dot` × 7
- `three:WebGLRenderer.setRenderTarget` × 7
- `three:WebGLRenderer.shadowMap` × 7
- `three:WebGLRenderer.state` × 7
- `dom:CanvasPath.quadraticCurveTo` × 6
- `dom:Worker` × 6
- `es:Array.every` × 6
- `es:Array.pop` × 6
- `es:Float64Array.fill` × 6
- `es:Int32Array.length` × 6
- `es:ReadonlyMap` × 6
- `es:Uint32Array` × 6
- `es:WeakMap` × 6
- `es:WeakMap.set` × 6
- `three:AdditiveBlending` × 6
- `three:Color.copy` × 6
- `three:DataArrayTexture.addLayerUpdate` × 6
- `three:Frustum` × 6
- `three:InstancedMesh.count` × 6
- `three:Object3D.receiveShadow` × 6
- `three:Object3D.updateMatrix` × 6
- `three:RenderTarget.scissorTest` × 6
- `three:RenderTarget.viewport` × 6
- `three:RenderTarget.width` × 6
- `three:SRGBColorSpace` × 6
- `three:Texture.clearUpdateRanges` × 6
- `three:Vector3.sub` × 6
- `dom:CanvasDrawPath.beginPath` × 5
- `dom:Worker.postMessage` × 5
- `es:Array.fill` × 5
- `es:Float32Array.set` × 5
- `es:Float32ArrayConstructor.from` × 5
- `es:Float64Array.slice` × 5
- `es:Map.clear` × 5
- `es:ObjectConstructor.fromEntries` × 5
- `es:ReadonlyArray.filter` × 5
- `es:ReadonlyArray.indexOf` × 5
- `es:Set.size` × 5
- `es:String.length` × 5
- `three:BufferAttribute.array` × 5
- `three:BufferGeometry.index` × 5
- `three:CanvasTexture` × 5
- `three:DataUtils.toHalfFloat` × 5
- `three:IcosahedronGeometry` × 5
- `three:InstancedBufferGeometry` × 5
- `three:InstancedBufferGeometry.instanceCount` × 5
- `three:Light` × 5
- `three:Matrix4.makeTranslation` × 5
- `three:Object3D.matrixAutoUpdate` × 5
- `three:OrthographicCamera.bottom` × 5
- `three:OrthographicCamera.top` × 5
- `three:ShaderMaterialProperties.uniforms` × 5
- `three:Sphere.radius` × 5
- `three:Texture.addUpdateRange` × 5
- `three:Texture.colorSpace` × 5
- `three:Texture.wrapS` × 5
- `three:Texture.wrapT` × 5
- `three:Vector3.crossVectors` × 5
- `three:Vector3.subVectors` × 5
- `three:Vector4.w` × 5
- `three:Vector4.y` × 5
- `three:Vector4.z` × 5
- `dom:CanvasDrawPath.stroke` × 4
- `dom:CanvasFillStrokeStyles.strokeStyle` × 4
- `dom:CanvasPath.lineTo` × 4
- `dom:CanvasPathDrawingStyles.lineWidth` × 4
- `dom:MessageEvent` × 4
- `es:Array.findIndex` × 4
- `es:Array.reduce` × 4
- `es:CallableFunction.call` × 4
- `es:Math.pow` × 4
- `es:ObjectConstructor.keys` × 4
- `es:Omit` × 4
- `es:ReadonlyArray.every` × 4
- `es:String.slice` × 4
- `es:Uint8Array.buffer` × 4
- `three:BufferAttribute.getX` × 4
- `three:BufferGeometry.boundingBox` × 4
- `three:Color.multiplyScalar` × 4
- `three:DataArrayTexture` × 4
- `three:DataArrayTextureImageData.width` × 4
- `three:DirectionalLight.target` × 4
- `three:Light.color` × 4
- `three:LineMaterial` × 4
- `three:LineSegments2` × 4
- `three:MaterialProperties.opacity` × 4
- `three:MaterialProperties.visible` × 4
- `three:Matrix4.copy` × 4
- `three:MeshStandardMaterialProperties.normalScale` × 4
- `three:Object3D.customDepthMaterial` × 4
- `three:Object3D.remove` × 4
- `three:Object3D.renderOrder` × 4
- `three:OrthographicCamera.left` × 4
- `three:OrthographicCamera.right` × 4
- `three:OrthographicCamera.updateProjectionMatrix` × 4
- `three:RenderTarget.texture` × 4
- `three:ShaderChunk.shadowmap_vertex` × 4
- `three:SphereGeometry` × 4
- `three:Texture.anisotropy` × 4
- `three:Vector3.length` × 4
- `three:Vector4.x` × 4
- `three:WebGLRenderer.copyTextureToTexture` × 4
- `three:WebGLState.bindFramebuffer` × 4
- `dom:CanvasDrawPath.fill` × 3
- `dom:CanvasFillStrokeStyles.fillStyle` × 3
- `dom:document` × 3
- `dom:Transferable` × 3
- `es:Array.flatMap` × 3
- `es:Array.splice` × 3
- `es:ArrayBuffer.slice` × 3
- `es:ArrayLike.length` × 3
- `es:Exclude` × 3
- `es:Float64Array.subarray` × 3
- `es:Float64ArrayConstructor.from` × 3
- `es:Math.exp` × 3
- `es:NumberConstructor.isNaN` × 3
- `es:ReadonlyArray.slice` × 3
- `es:ReadonlyArray.some` × 3
- `es:ReadonlyMap.get` × 3
- `es:Set.delete` × 3
- `es:Uint8Array.set` × 3
- `three:Box3.expandByScalar` × 3
- `three:BufferAttribute.addUpdateRange` × 3
- `three:BufferAttribute.clearUpdateRanges` × 3
- `three:BufferAttribute.getY` × 3
- `three:BufferAttribute.needsUpdate` × 3
- `three:BufferGeometry.clone` × 3
- `three:BufferGeometry.translate` × 3
- `three:DataUtils.fromHalfFloat` × 3
- `three:FloatType` × 3
- `three:Frustum.intersectsBox` × 3
- `three:LightShadow.needsUpdate` × 3
- `three:LinearMipmapLinearFilter` × 3
- `three:LineSegments2.geometry` × 3
- `three:Matrix4.makeScale` × 3
- `three:Matrix4.multiply` × 3
- `three:Mesh.count` × 3
- `three:Mesh.isMesh` × 3
- `three:MeshBasicMaterialProperties.color` × 3
- `three:MeshStandardMaterialProperties.map` × 3
- `three:Object3D.clear` × 3
- `three:Object3D.dispose` × 3
- `three:OrthographicCamera.far` × 3
- `three:OrthographicCamera.near` × 3
- `three:RenderTarget.height` × 3
- `three:ShaderChunk.shadowmap_pars_fragment` × 3
- `three:ShaderPass` × 3
- `three:Texture.onUpdate` × 3
- `three:Vector3.applyMatrix4` × 3
- `three:WebGLRenderer.autoClear` × 3
- `three:WebGLShadowMap.autoUpdate` × 3
- `dom:AbstractWorker.onerror` × 2
- `dom:CanvasState.restore` × 2
- `dom:CanvasState.save` × 2
- `dom:CanvasTransform.rotate` × 2
- `dom:CanvasTransform.translate` × 2
- `dom:Document.createElement` × 2
- `dom:ErrorEvent.message` × 2
- `dom:fetch` × 2
- `dom:HTMLCanvasElement.getContext` × 2
- `dom:HTMLCanvasElement.height` × 2
- `dom:HTMLCanvasElement.width` × 2
- `dom:MessageEventTarget.onmessage` × 2
- `dom:Performance.measure` × 2
- `dom:WebGL2RenderingContextBase.DRAW_FRAMEBUFFER` × 2
- `dom:WebGL2RenderingContextBase.READ_FRAMEBUFFER` × 2
- `dom:WebGLFramebuffer` × 2
- `es:Array.shift` × 2
- `es:Array.some` × 2
- `es:Float32Array.slice` × 2
- `es:Map.size` × 2
- `es:Math.atan` × 2
- `es:Math.atan2` × 2
- `es:Math.fround` × 2
- `es:Math.log` × 2
- `es:Math.log2` × 2
- `es:Math.SQRT2` × 2
- `es:Math.tan` × 2
- `es:Promise.then` × 2
- `es:PromiseConstructor.all` × 2
- `es:ReadonlyArray.flatMap` × 2
- `es:ReadonlyArray.reduce` × 2
- `es:ReadonlyMap.size` × 2
- `es:ReadonlySet.has` × 2
- `es:String` × 2
- `es:String.split` × 2
- `es:Symbol` × 2
- `es:Uint8Array.length` × 2
- `es:Uint8Array.slice` × 2
- `es:Uint8Array.subarray` × 2
- `three:BackSide` × 2
- `three:Box2.max` × 2
- `three:Box2.min` × 2
- `three:Box3.copy` × 2
- `three:Box3.expandByPoint` × 2
- `three:Box3.isEmpty` × 2
- `three:BufferAttribute.getZ` × 2
- `three:BufferAttribute.setUsage` × 2
- `three:BufferAttribute.setXYZ` × 2
- `three:BufferAttribute.start` × 2
- `three:BufferGeometry.applyMatrix4` × 2
- `three:BufferGeometry.computeVertexNormals` × 2
- `three:BufferGeometry.rotateX` × 2
- `three:BufferGeometry.toNonIndexed` × 2
- `three:Camera.matrixWorldInverse` × 2
- `three:Camera.projectionMatrix` × 2
- `three:Camera.projectionMatrixInverse` × 2
- `three:childadded.child` × 2
- `three:ClampToEdgeWrapping` × 2
- `three:Color.clone` × 2
- `three:Color.setHex` × 2
- `three:DataTextureImageData.data` × 2
- `three:DepthFormat` × 2
- `three:DepthTexture` × 2
- `three:DepthTexture.format` × 2
- `three:DepthTexture.magFilter` × 2
- `three:DepthTexture.minFilter` × 2
- `three:DynamicDrawUsage` × 2
- `three:GeometryGroup.start` × 2
- `three:InstancedMesh.dispose` × 2
- `three:LightShadow` × 2
- `three:LightShadow.bias` × 2
- `three:LightShadow.camera` × 2
- `three:LightShadow.normalBias` × 2
- `three:LightShadow.updateMatrices` × 2
- `three:LineSegments` × 2
- `three:LineSegmentsGeometry` × 2
- `three:LineSegmentsGeometry.setPositions` × 2
- `three:Material.clone` × 2
- `three:Matrix4.compose` × 2
- `three:Matrix4.getMaxScaleOnAxis` × 2
- `three:Matrix4.multiplyMatrices` × 2
- `three:Object3D.onAfterRender` × 2
- `three:Object3D.onBeforeShadow` × 2
- `three:Object3D.up` × 2
- `three:Object3D.userData` × 2
- `three:Pass.render` × 2
- `three:PlaneGeometry` × 2
- `three:Quaternion` × 2
- `three:Scene.environment` × 2
- `three:Scene.environmentIntensity` × 2
- `three:ShaderChunk.lights_fragment_begin` × 2
- `three:ShaderChunk.project_vertex` × 2
- `three:Sphere` × 2
- `three:Texture.generateMipmaps` × 2
- `three:Texture.internalFormat` × 2
- `three:Texture.name` × 2
- `three:UnsignedIntType` × 2
- `three:Vector2.y` × 2
- `three:Vector3.cross` × 2
- `three:Vector3.setFromMatrixPosition` × 2
- `three:Vector3.toArray` × 2
- `three:WebGLDepthBuffer.setMask` × 2
- `three:WebGLProgramParameters.defines` × 2
- `three:WebGLProgramParametersWithUniforms` × 2
- `three:WebGLProperties.get` × 2
- `three:WebGLRenderer.getRenderTarget` × 2
- `three:WebGLRenderer.initRenderTarget` × 2
- `three:WebGLRenderer.properties` × 2
- `three:WebGLRenderer.render` × 2
- `three:WebGLRenderer.setClearColor` × 2
- `three:WebGLShadowMap.needsUpdate` × 2
- `three:WebGLShadowMap.render` × 2
- `three:WebGLState.buffers` × 2
- `three:WebGLState.depth` × 2
- `dom:Body.arrayBuffer` × 1
- `dom:Body.json` × 1
- `dom:CanvasPath.closePath` × 1
- `dom:CanvasPathDrawingStyles.lineCap` × 1
- `dom:CanvasRect.clearRect` × 1
- `dom:Console.info` × 1
- `dom:Path2D` × 1
- `dom:WebGL2RenderingContext` × 1
- `dom:WebGL2RenderingContextBase.blitFramebuffer` × 1
- `dom:WebGLRenderingContextBase.DEPTH_BUFFER_BIT` × 1
- `dom:WebGLRenderingContextBase.NEAREST` × 1
- `es:Array.find` × 1
- `es:Array.join` × 1
- `es:Array.reverse` × 1
- `es:ArrayBufferConstructor.isView` × 1
- `es:Float32Array.subarray` × 1
- `es:Function.length` × 1
- `es:Int32Array.buffer` × 1
- `es:Int32Array.fill` × 1
- `es:Int32Array.slice` × 1
- `es:IterableIterator` × 1
- `es:Math.asin` × 1
- `es:Math.log10` × 1
- `es:Math.random` × 1
- `es:Math.sign` × 1
- `es:ObjectConstructor.entries` × 1
- `es:ObjectConstructor.freeze` × 1
- `es:Parameters` × 1
- `es:ReadonlyArray.find` × 1
- `es:ReadonlyMap.has` × 1
- `es:ReturnType` × 1
- `es:String.charCodeAt` × 1
- `es:String.indexOf` × 1
- `es:String.lastIndexOf` × 1
- `es:String.trim` × 1
- `es:Uint32ArrayConstructor.from` × 1
- `es:Uint8ArrayConstructor.from` × 1
- `es:Uint8ClampedArray` × 1
- `es:WeakMap.delete` × 1
- `es:WeakSet` × 1
- `es:WeakSet.add` × 1
- `es:WeakSet.has` × 1
- `three:AmbientLight` × 1
- `three:BasicShadowMap` × 1
- `three:Box2` × 1
- `three:Box3.distanceToPoint` × 1
- `three:Box3.getBoundingSphere` × 1
- `three:Box3.makeEmpty` × 1
- `three:BufferAttribute.copy` × 1
- `three:BufferAttribute.updateRanges` × 1
- `three:BufferGeometry.attributes` × 1
- `three:BufferGeometry.clearGroups` × 1
- `three:BufferGeometry.deleteAttribute` × 1
- `three:BufferGeometry.groups` × 1
- `three:BufferGeometry.scale` × 1
- `three:BufferGeometry.setDrawRange` × 1
- `three:Camera.reversedDepth` × 1
- `three:CircleGeometry` × 1
- `three:CompressedTexture` × 1
- `three:CompressedTexture.mipmaps` × 1
- `three:ConeGeometry` × 1
- `three:DataArrayTexture.generateMipmaps` × 1
- `three:DataArrayTexture.magFilter` × 1
- `three:DataArrayTexture.minFilter` × 1
- `three:DataArrayTextureImageData.data` × 1
- `three:DataArrayTextureImageData.height` × 1
- `three:DepthTexture.compareFunction` × 1
- `three:DodecahedronGeometry` × 1
- `three:Euler` × 1
- `three:EventDispatcher.addEventListener` × 1
- `three:EventDispatcher.hasEventListener` × 1
- `three:Frustum.setFromProjectionMatrix` × 1
- `three:GeometryGroup.count` × 1
- `three:GeometryGroup.materialIndex` × 1
- `three:GreaterEqualCompare` × 1
- `three:HalfFloatType` × 1
- `three:HemisphereLight` × 1
- `three:InstancedMesh.setColorAt` × 1
- `three:InstancedMesh.setMatrixAt` × 1
- `three:LessEqualCompare` × 1
- `three:LightShadow.autoUpdate` × 1
- `three:LightShadow.dispose` × 1
- `three:LightShadow.matrix` × 1
- `three:LightShadow.radius` × 1
- `three:LineMaterial.resolution` × 1
- `three:Loader.loadAsync` × 1
- `three:Material.defines` × 1
- `three:Material.needsUpdate` × 1
- `three:MaterialProperties.alphaTest` × 1
- `three:MaterialProperties.shadowSide` × 1
- `three:Matrix4.elements` × 1
- `three:Matrix4.premultiply` × 1
- `three:mergeGeometries` × 1
- `three:Mesh.material` × 1
- `three:MeshDepthMaterialProperties.map` × 1
- `three:Object3D.getWorldDirection` × 1
- `three:Object3D.getWorldPosition` × 1
- `three:Object3D.lookAt` × 1
- `three:Object3D.onBeforeRender` × 1
- `three:Object3D.scale` × 1
- `three:Object3DEventMap` × 1
- `three:OrthographicCamera.isOrthographicCamera` × 1
- `three:OrthographicCamera.zoom` × 1
- `three:Pass.dispose` × 1
- `three:PerspectiveCamera.far` × 1
- `three:PerspectiveCamera.fov` × 1
- `three:PerspectiveCamera.isPerspectiveCamera` × 1
- `three:PerspectiveCamera.near` × 1
- `three:PerspectiveCamera.zoom` × 1
- `three:PMREMGenerator` × 1
- `three:PMREMGenerator.dispose` × 1
- `three:PMREMGenerator.fromScene` × 1
- `three:Quaternion.setFromEuler` × 1
- `three:Quaternion.setFromUnitVectors` × 1
- `three:RGBAIntegerFormat` × 1
- `three:Scene.background` × 1
- `three:Scene.fog` × 1
- `three:ShaderChunk.normal_fragment_begin` × 1
- `three:ShaderMaterialProperties.linewidth` × 1
- `three:Texture.flipY` × 1
- `three:Texture.format` × 1
- `three:Texture.magFilter` × 1
- `three:Texture.minFilter` × 1
- `three:Texture.type` × 1
- `three:Texture.version` × 1
- `three:TextureLoader` × 1
- `three:UnsignedShortType` × 1
- `three:Vector2.copy` × 1
- `three:Vector2.normalize` × 1
- `three:Vector3.addVectors` × 1
- `three:Vector3.angleTo` × 1
- `three:Vector3.distanceTo` × 1
- `three:Vector3.divideScalar` × 1
- `three:Vector3.fromArray` × 1
- `three:Vector3.lerp` × 1
- `three:Vector3.multiply` × 1
- `three:Vector3.negate` × 1
- `three:Vector3.setScalar` × 1
- `three:Vector3.setY` × 1
- `three:WebGLCapabilities.reversedDepthBuffer` × 1
- `three:WebGLCoordinateSystem` × 1
- `three:WebGLRenderer.capabilities` × 1
- `three:WebGLRenderer.getActiveCubeFace` × 1
- `three:WebGLRenderer.getActiveMipmapLevel` × 1
- `three:WebGLRenderer.getClearAlpha` × 1
- `three:WebGLRenderer.getClearColor` × 1
- `three:WebGLRenderer.getContext` × 1
- `three:WebGLRenderer.getCurrentViewport` × 1
- `three:WebGLShadowMap.type` × 1
- `three:WebGLState.setScissorTest` × 1
- `vite:ImportMeta.env` × 1
- `vite:ImportMetaEnv.DEV` × 1
