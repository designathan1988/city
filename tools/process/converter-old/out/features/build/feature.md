# Feature: src/buildingsWiring.ts, src/editor/blocks.ts, src/editor/blueprintLibrary.ts, src/editor/buildingFloors.ts, src/editor/buildingPlans.ts, src/editor/buildingProfile.ts, src/editor/buildingRoofs.ts, src/editor/buildingSnap.ts, src/editor/buildingTool.ts, src/editor/buildings.ts, src/editor/fromReference.ts, src/editor/growthFeed.ts, src/editor/growthReplica.ts, src/editor/lotTool.ts, src/growth.worker.ts, src/growthPool.ts, src/render/buildingSwitches.ts, src/render/buildings/buildingMesh.ts, src/render/buildings/cell.worker.ts, src/render/buildings/cellArrays.ts, src/render/buildings/cellPool.ts, src/render/buildings/cellRequests.ts, src/render/buildings/cellWork.ts, src/render/buildings/chunk.ts, src/render/buildings/facadeKit.ts, src/render/buildings/facadeLevel.ts, src/render/buildings/facadePropModels.ts, src/render/buildings/facadeProps.ts, src/render/buildings/fade.ts, src/render/buildings/farParts.ts, src/render/buildings/finishes.ts, src/render/buildings/flagLayer.ts, src/render/buildings/fracture.ts, src/render/buildings/fracture.worker.ts, src/render/buildings/frameBudget.ts, src/render/buildings/furnitureKit.ts, src/render/buildings/glassRoom.ts, src/render/buildings/gpuBatchedMesh.ts, src/render/buildings/kit.ts, src/render/buildings/landmarks.ts, src/render/buildings/lawnGround.ts, src/render/buildings/layer.ts, src/render/buildings/layerApi.ts, src/render/buildings/layerCells.ts, src/render/buildings/levelKeys.ts, src/render/buildings/lightSlots.ts, src/render/buildings/louvreSlats.ts, src/render/buildings/massBase.ts, src/render/buildings/massCells.ts, src/render/buildings/massLevels.ts, src/render/buildings/massing.ts, src/render/buildings/massingMaterial.ts, src/render/buildings/partCells.ts, src/render/buildings/partFiling.ts, src/render/buildings/partPose.ts, src/render/buildings/partWarm.ts, src/render/buildings/parts.ts, src/render/buildings/partyWalls.ts, src/render/buildings/referenceModel.ts, src/render/buildings/roofPlan.ts, src/render/buildings/roomBehind.ts, src/render/buildings/roomLamps.ts, src/render/buildings/shellBatch.ts, src/render/buildings/shellParts.ts, src/render/buildings/signature.ts, src/render/buildings/thinParts.ts, src/render/buildings/townColumns.ts, src/render/buildings/townIndex.ts, src/render/gardenLists.ts, src/render/gardens.ts, src/render/interiorSolids.ts, src/render/lotLattice.ts, src/render/lotOverlay.ts, src/render/mesh/farBlockBatches.ts, src/render/roomLamps.ts, src/render/torusWindow.ts, src/render/wear.ts, src/ui/buildStamp.ts, src/ui/builder/catalog.ts, src/ui/builder/icons.ts, src/ui/builder/workspace.ts, src/ui/overlay/builderGrid.ts, src/ui/overlay/buildingOverlay.ts, src/world/buildingGates.ts, src/world/buildings/architecture.ts, src/world/buildings/blocks.ts, src/world/buildings/blueprints.ts, src/world/buildings/cameraSolids.ts, src/world/buildings/cityBuildings.ts, src/world/buildings/decayWatch.ts, src/world/buildings/docks.ts, src/world/buildings/elements.ts, src/world/buildings/facadePatterns.ts, src/world/buildings/flags.ts, src/world/buildings/floors.ts, src/world/buildings/footprints.ts, src/world/buildings/foundation.ts, src/world/buildings/geometry.ts, src/world/buildings/grid.ts, src/world/buildings/handles.ts, src/world/buildings/interior.ts, src/world/buildings/intern.ts, src/world/buildings/items.ts, src/world/buildings/lotLibrary.ts, src/world/buildings/lotTemplate.ts, src/world/buildings/lots.ts, src/world/buildings/materials.ts, src/world/buildings/neighbours.ts, src/world/buildings/pads.ts, src/world/buildings/pick.ts, src/world/buildings/procedural.ts, src/world/buildings/recordTable.ts, src/world/buildings/reference.ts, src/world/buildings/roofPlant.ts, src/world/buildings/sections.ts, src/world/buildings/serialize.ts, src/world/buildings/siteGrading.ts, src/world/buildings/spaces.ts, src/world/buildings/spilledItems.ts, src/world/buildings/splitLevel.ts, src/world/buildings/store.ts, src/world/buildings/streetDoors.ts, src/world/buildings/tableRows.ts, src/world/buildings/towerKit.ts, src/world/buildings/types.ts, src/world/buildings/validate.ts, src/world/growth/grownRows.ts, src/world/growth/lotBoundaryRuns.ts, src/world/growth/lotPlan.ts, src/world/growth/rows.ts, src/world/growth/version.ts, src/world/growth/zoning.ts, src/world/lotRecord.ts, src/world/lotRings.ts, src/world/lotStore.ts, src/world/lots.ts, src/world/openLots.ts, src/world/zoneGrid.ts, src/world/zones.ts

The program starts from `src/main.ts`, `src/peopleLab/main.ts`. A declaration is the feature's own when the program reaches it only through the feature; in a declaration of the program that names the feature, its statements are a slice (what names it, what depends on that, what that controls).

- Own: 4178 declarations
- Slices of shared code: 360
- Code: 50205 lines in 224 files (source/)
- Resources: 1642 (1315 entry, 327 style, 0 import; 1257 only the feature uses)
- Uses: 407 shared declarations (interface/: 1403 declarations in 270 files, the types they name included); hooks: 433; external APIs: 736

## Code

- src/actionsWiring.ts — part, 121 lines
- src/buildingsWiring.ts — whole, 1233 lines
- src/core/derivedCache.ts — part, 32 lines
- src/core/forkMap.ts — part, 3 lines
- src/core/hypot.ts — part, 22 lines
- src/core/idSlots.ts — part, 10 lines
- src/core/minMax.ts — whole, 10 lines
- src/core/polyline.ts — part, 18 lines
- src/core/rng.ts — part, 9 lines
- src/core/scalar.ts — part, 1 lines
- src/desktopJobs.ts — part, 14 lines
- src/editor/barriers.ts — part, 23 lines
- src/editor/blocks.ts — whole, 50 lines
- src/editor/blueprintLibrary.ts — whole, 67 lines
- src/editor/buildingFloors.ts — whole, 43 lines
- src/editor/buildingPlans.ts — whole, 272 lines
- src/editor/buildingProfile.ts — whole, 108 lines
- src/editor/buildingRoofs.ts — whole, 44 lines
- src/editor/buildings.ts — whole, 1126 lines
- src/editor/buildingSnap.ts — whole, 226 lines
- src/editor/buildingTool.ts — whole, 2727 lines
- src/editor/bulldozer.ts — part, 9 lines
- src/editor/chunkedSave.ts — part, 20 lines
- src/editor/cityGenerator.ts — part, 21 lines
- src/editor/fromReference.ts — whole, 252 lines
- src/editor/growthFeed.ts — whole, 354 lines
- src/editor/growthReplica.ts — whole, 291 lines
- src/editor/history.ts — part, 4 lines
- src/editor/lotTool.ts — whole, 875 lines
- src/editor/persistence.ts — part, 3 lines
- src/editor/preparedCity.ts — part, 33 lines
- src/editor/regionFrames.ts — part, 13 lines
- src/editor/smallCityMap.ts — part, 34 lines
- src/editor/transitTools.ts — part, 5 lines
- src/growth.worker.ts — whole, 18 lines
- src/growthPool.ts — whole, 87 lines
- src/jobs/jobPool.ts — part, 3 lines
- src/lab/catalog/capture.ts — part, 6 lines
- src/lab/catalog/city.ts — part, 12 lines
- src/lab/catalog/engineBuildings.ts — part, 4 lines
- src/lab/recorder/report.ts — part, 7 lines
- src/main.ts — part, 287 lines
- src/render/bake/finishRecipes.ts — whole, 383 lines
- src/render/buildings/buildingMesh.ts — whole, 4197 lines
- src/render/buildings/cell.worker.ts — whole, 21 lines
- src/render/buildings/cellArrays.ts — whole, 579 lines
- src/render/buildings/cellPool.ts — whole, 397 lines
- src/render/buildings/cellRequests.ts — whole, 210 lines
- src/render/buildings/cellWork.ts — whole, 237 lines
- src/render/buildings/chunk.ts — whole, 137 lines
- src/render/buildings/facadeKit.ts — whole, 451 lines
- src/render/buildings/facadeLevel.ts — whole, 211 lines
- src/render/buildings/facadePropModels.ts — whole, 351 lines
- src/render/buildings/facadeProps.ts — whole, 193 lines
- src/render/buildings/fade.ts — whole, 44 lines
- src/render/buildings/farParts.ts — whole, 294 lines
- src/render/buildings/finishes.ts — whole, 367 lines
- src/render/buildings/flagLayer.ts — whole, 171 lines
- src/render/buildings/fracture.ts — whole, 418 lines
- src/render/buildings/fracture.worker.ts — whole, 20 lines
- src/render/buildings/frameBudget.ts — whole, 143 lines
- src/render/buildings/furnitureKit.ts — whole, 463 lines
- src/render/buildings/glassRoom.ts — whole, 452 lines
- src/render/buildings/gpuBatchedMesh.ts — whole, 407 lines
- src/render/buildings/kit.ts — whole, 730 lines
- src/render/buildings/landmarks.ts — whole, 422 lines
- src/render/buildings/lawnGround.ts — whole, 63 lines
- src/render/buildings/layer.ts — whole, 1243 lines
- src/render/buildings/layerApi.ts — whole, 112 lines
- src/render/buildings/layerCells.ts — whole, 314 lines
- src/render/buildings/levelKeys.ts — whole, 31 lines
- src/render/buildings/lightSlots.ts — whole, 30 lines
- src/render/buildings/louvreSlats.ts — whole, 108 lines
- src/render/buildings/massBase.ts — whole, 227 lines
- src/render/buildings/massCells.ts — whole, 1082 lines
- src/render/buildings/massing.ts — whole, 608 lines
- src/render/buildings/massingMaterial.ts — whole, 128 lines
- src/render/buildings/massLevels.ts — whole, 181 lines
- src/render/buildings/partCells.ts — whole, 527 lines
- src/render/buildings/partFiling.ts — whole, 210 lines
- src/render/buildings/partPose.ts — whole, 111 lines
- src/render/buildings/parts.ts — whole, 323 lines
- src/render/buildings/partWarm.ts — whole, 31 lines
- src/render/buildings/partyWalls.ts — whole, 115 lines
- src/render/buildings/referenceModel.ts — whole, 247 lines
- src/render/buildings/roofPlan.ts — whole, 84 lines
- src/render/buildings/roomBehind.ts — whole, 183 lines
- src/render/buildings/roomLamps.ts — whole, 63 lines
- src/render/buildings/shellBatch.ts — whole, 289 lines
- src/render/buildings/shellParts.ts — whole, 174 lines
- src/render/buildings/signature.ts — whole, 1804 lines
- src/render/buildings/thinParts.ts — whole, 99 lines
- src/render/buildings/townColumns.ts — whole, 179 lines
- src/render/buildings/townIndex.ts — whole, 131 lines
- src/render/buildingSwitches.ts — whole, 103 lines
- src/render/canyonLight.ts — part, 58 lines
- src/render/carBody.ts — part, 54 lines
- src/render/destruction.ts — part, 196 lines
- src/render/flatAtlas.ts — part, 9 lines
- src/render/fogLayer.ts — part, 91 lines
- src/render/gardenLists.ts — whole, 124 lines
- src/render/gardens.ts — whole, 275 lines
- src/render/gpuLedger.ts — part, 5 lines
- src/render/groundChanges.ts — part, 19 lines
- src/render/groundShape.ts — part, 56 lines
- src/render/groundShapePool.ts — part, 6 lines
- src/render/interiorSolids.ts — whole, 71 lines
- src/render/lotLattice.ts — whole, 447 lines
- src/render/lotOverlay.ts — whole, 518 lines
- src/render/materials.ts — part, 8 lines
- src/render/mesh/farBlockBatches.ts — whole, 199 lines
- src/render/mesh/hiddenFaces.ts — part, 31 lines
- src/render/mesh/roadResidency.ts — part, 8 lines
- src/render/networkFarRoads.ts — part, 24 lines
- src/render/occlusion.ts — part, 6 lines
- src/render/playEffects.ts — part, 50 lines
- src/render/postprocess.ts — part, 3 lines
- src/render/renderer.ts — part, 525 lines
- src/render/roadSurfaces.ts — part, 27 lines
- src/render/roomLamps.ts — whole, 46 lines
- src/render/scenery.ts — part, 15 lines
- src/render/sharedGround.ts — part, 9 lines
- src/render/surfaceTextures.ts — part, 6 lines
- src/render/terrain.ts — part, 3 lines
- src/render/torusWindow.ts — whole, 63 lines
- src/render/uploads.ts — part, 13 lines
- src/render/vehicleModels.ts — part, 59 lines
- src/render/wear.ts — whole, 188 lines
- src/sim/agents/bay.ts — part, 3 lines
- src/sim/agents/buildingOrder.ts — part, 126 lines
- src/sim/agents/kerbBays.ts — part, 63 lines
- src/sim/agents/lorryDocks.ts — part, 104 lines
- src/sim/agents/lorryTraffic.ts — part, 79 lines
- src/sim/agents/lotBays.ts — part, 184 lines
- src/sim/agents/lotDoors.ts — part, 45 lines
- src/sim/agents/lotNav.ts — part, 36 lines
- src/sim/agents/lotTraffic.ts — part, 4 lines
- src/sim/agents/parking.ts — part, 39 lines
- src/sim/agents/trafficBuildings.ts — part, 234 lines
- src/sim/ambient/ambientIndex.ts — part, 5 lines
- src/sim/ambient/demand.ts — part, 8 lines
- src/sim/host/simClient.ts — part, 50 lines
- src/sim/host/simHost.ts — part, 3 lines
- src/ui/about.ts — part, 10 lines
- src/ui/builder/catalog.ts — whole, 199 lines
- src/ui/builder/icons.ts — whole, 316 lines
- src/ui/builder/workspace.ts — whole, 372 lines
- src/ui/buildStamp.ts — whole, 47 lines
- src/ui/i18n/index.ts — part, 4 lines
- src/ui/materialSwatch.ts — part, 173 lines
- src/ui/overlay/builderGrid.ts — whole, 32 lines
- src/ui/overlay/buildingOverlay.ts — whole, 317 lines
- src/ui/planSwatch.ts — part, 39 lines
- src/ui/v2/shell.ts — part, 204 lines
- src/view/viewport.ts — part, 3 lines
- src/world/bounds.ts — part, 2 lines
- src/world/buildingGates.ts — whole, 45 lines
- src/world/buildings/architecture.ts — whole, 326 lines
- src/world/buildings/blocks.ts — whole, 135 lines
- src/world/buildings/blueprints.ts — whole, 331 lines
- src/world/buildings/cameraSolids.ts — whole, 345 lines
- src/world/buildings/cityBuildings.ts — whole, 342 lines
- src/world/buildings/decayWatch.ts — whole, 59 lines
- src/world/buildings/docks.ts — whole, 57 lines
- src/world/buildings/elements.ts — whole, 519 lines
- src/world/buildings/facadePatterns.ts — whole, 114 lines
- src/world/buildings/flags.ts — whole, 39 lines
- src/world/buildings/floors.ts — whole, 48 lines
- src/world/buildings/footprints.ts — whole, 182 lines
- src/world/buildings/foundation.ts — whole, 353 lines
- src/world/buildings/geometry.ts — whole, 803 lines
- src/world/buildings/grid.ts — whole, 189 lines
- src/world/buildings/handles.ts — whole, 155 lines
- src/world/buildings/interior.ts — whole, 915 lines
- src/world/buildings/intern.ts — whole, 242 lines
- src/world/buildings/items.ts — whole, 143 lines
- src/world/buildings/lotLibrary.ts — whole, 32 lines
- src/world/buildings/lots.ts — whole, 88 lines
- src/world/buildings/lotTemplate.ts — whole, 12 lines
- src/world/buildings/materials.ts — whole, 297 lines
- src/world/buildings/neighbours.ts — whole, 104 lines
- src/world/buildings/pads.ts — whole, 443 lines
- src/world/buildings/pick.ts — whole, 227 lines
- src/world/buildings/procedural.ts — whole, 706 lines
- src/world/buildings/recordTable.ts — whole, 516 lines
- src/world/buildings/reference.ts — whole, 10 lines
- src/world/buildings/roofPlant.ts — whole, 146 lines
- src/world/buildings/sections.ts — whole, 95 lines
- src/world/buildings/serialize.ts — whole, 399 lines
- src/world/buildings/siteGrading.ts — whole, 182 lines
- src/world/buildings/spaces.ts — whole, 150 lines
- src/world/buildings/spilledItems.ts — whole, 162 lines
- src/world/buildings/splitLevel.ts — whole, 308 lines
- src/world/buildings/store.ts — whole, 442 lines
- src/world/buildings/streetDoors.ts — whole, 42 lines
- src/world/buildings/tableRows.ts — whole, 123 lines
- src/world/buildings/towerKit.ts — whole, 22 lines
- src/world/buildings/types.ts — whole, 470 lines
- src/world/buildings/validate.ts — whole, 348 lines
- src/world/changes.ts — part, 5 lines
- src/world/cityGen/plan.ts — part, 21 lines
- src/world/cityGen/squares.ts — part, 82 lines
- src/world/doc.ts — part, 242 lines
- src/world/docBinary.ts — part, 17 lines
- src/world/groundPatch.ts — part, 40 lines
- src/world/growth/grownRows.ts — whole, 84 lines
- src/world/growth/lotBoundaryRuns.ts — whole, 59 lines
- src/world/growth/lotPlan.ts — whole, 1095 lines
- src/world/growth/rows.ts — whole, 45 lines
- src/world/growth/version.ts — whole, 3 lines
- src/world/growth/zoning.ts — whole, 641 lines
- src/world/lotRecord.ts — whole, 27 lines
- src/world/lotRings.ts — whole, 67 lines
- src/world/lots.ts — whole, 1330 lines
- src/world/lotStore.ts — whole, 452 lines
- src/world/nearestRibbon.ts — whole, 22 lines
- src/world/openLots.ts — whole, 287 lines
- src/world/parkingLayout.ts — part, 3 lines
- src/world/poleLines.ts — part, 94 lines
- src/world/rasterGrid.ts — whole, 32 lines
- src/world/rescale.ts — part, 8 lines
- src/world/solids.ts — part, 21 lines
- src/world/zoneGrid.ts — whole, 314 lines
- src/world/zones.ts — whole, 40 lines

## Slices of shared code

- `src/actionsWiring.ts#Actions` — 3 of 10 lines
- `src/actionsWiring.ts#RAY_ABOVE` — 1 of 1 lines
- `src/actionsWiring.ts#createActions` — 6 of 408 lines
- `src/actionsWiring.ts#createActions.breakOne` — 6 of 7 lines
- `src/actionsWiring.ts#createActions.buildingsUnderRay` — 4 of 6 lines
- `src/actionsWiring.ts#createActions.explodeAt` — 62 of 158 lines
- `src/actionsWiring.ts#createActions.rayOnBuilding` — 12 of 17 lines
- `src/actionsWiring.ts#createActions.strikeAt` — 9 of 14 lines
- `src/actionsWiring.ts#createActions.tick` — 18 of 24 lines
- `src/core/forkMap.ts#ForkMap.writable` — 3 of 11 lines
- `src/desktopJobs.ts#desktopJobWorkers` — 14 of 32 lines
- `src/editor/barriers.ts#barrierProblem` — 5 of 22 lines
- `src/editor/barriers.ts#snapBarrierPoint` — 18 of 22 lines
- `src/editor/bulldozer.ts#Bulldozer.boxed` — 9 of 25 lines
- `src/editor/chunkedSave.ts#KEYED` — 10 of 35 lines
- `src/editor/chunkedSave.ts#TiledSaves.write` — 10 of 89 lines
- `src/editor/cityGenerator.ts#greenCity` — 5 of 17 lines
- `src/editor/cityGenerator.ts#zoneCity` — 16 of 31 lines
- `src/editor/history.ts#openedGenerated` — 4 of 4 lines
- `src/editor/persistence.ts#isSerializedDoc` — 3 of 98 lines
- `src/editor/preparedCity.ts#PreparedCityWriter.add` — 11 of 12 lines
- `src/editor/preparedCity.ts#PreparedCityWriter.constructor` — 1 of 1 lines
- `src/editor/preparedCity.ts#PreparedCityWriter.endBatch` — 3 of 3 lines
- `src/editor/preparedCity.ts#PreparedCityWriter.finish` — 3 of 3 lines
- `src/editor/preparedCity.ts#PreparedCityWriter.table` — 1 of 1 lines
- `src/editor/preparedCity.ts#openedBuildings` — 3 of 3 lines
- `src/editor/preparedCity.ts#prepareCity` — 11 of 11 lines
- `src/editor/regionFrames.ts#readSquareFrame` — 5 of 10 lines
- `src/editor/regionFrames.ts#squareFrame` — 5 of 6 lines
- `src/editor/regionFrames.ts#wearFrame` — 3 of 5 lines
- `src/editor/smallCityMap.ts#buildCityTable` — 14 of 17 lines
- `src/editor/smallCityMap.ts#growCity` — 8 of 24 lines
- `src/editor/smallCityMap.ts#growLots` — 12 of 17 lines
- `src/editor/transitTools.ts#TransitTool.built` — 5 of 8 lines
- `src/lab/catalog/capture.ts#instanceMatrices` — 6 of 24 lines
- `src/lab/catalog/city.ts#GameProbe` — 12 of 18 lines
- `src/lab/recorder/report.ts#buildReport` — 7 of 161 lines
- `src/main.ts#<ExpressionStatement>@3923` — 3 of 8 lines
- `src/main.ts#<IfStatement>@4036` — 14 of 196 lines
- `src/main.ts#actions` — 3 of 16 lines
- `src/main.ts#afterRestore` — 3 of 11 lines
- `src/main.ts#buildings` — 47 of 47 lines
- `src/main.ts#bulldozer` — 3 of 10 lines
- `src/main.ts#cancelGestures` — 4 of 13 lines
- `src/main.ts#cityData` — 6 of 21 lines
- `src/main.ts#cityJob` — 9 of 19 lines
- `src/main.ts#currentGesture` — 3 of 18 lines
- `src/main.ts#decayLook` — 1 of 1 lines
- `src/main.ts#decayWatch` — 1 of 1 lines
- `src/main.ts#doubleClick` — 3 of 6 lines
- `src/main.ts#drawOverlayScreen` — 43 of 469 lines
- `src/main.ts#endPointer` — 4 of 30 lines
- `src/main.ts#frame` — 4 of 102 lines
- `src/main.ts#gameKeys` — 3 of 77 lines
- `src/main.ts#gameState` — 6 of 38 lines
- `src/main.ts#gestureInProgress` — 3 of 3 lines
- `src/main.ts#growZones` — 5 of 10 lines
- `src/main.ts#growth` — 3 of 3 lines
- `src/main.ts#growthAnswered` — 12 of 15 lines
- `src/main.ts#growthPool` — 1 of 1 lines
- `src/main.ts#hintKey` — 3 of 8 lines
- `src/main.ts#lotTool` — 14 of 14 lines
- `src/main.ts#mountInterface` — 5 of 88 lines
- `src/main.ts#mutateBuilt` — 6 of 36 lines
- `src/main.ts#openedOf` — 3 of 5 lines
- `src/main.ts#pointerDown` — 40 of 180 lines
- `src/main.ts#pointerMove` — 9 of 72 lines
- `src/main.ts#serializedDoc` — 5 of 8 lines
- `src/main.ts#setZone` — 5 of 14 lines
- `src/main.ts#showTool` — 3 of 5 lines
- `src/main.ts#singleClick` — 3 of 4 lines
- `src/main.ts#snapshotApplied` — 3 of 12 lines
- `src/main.ts#takeTool` — 4 of 12 lines
- `src/render/canyonLight.ts#CanyonLight` — 3 of 10 lines
- `src/render/canyonLight.ts#createCanyonLight.clearCells` — 16 of 16 lines
- `src/render/canyonLight.ts#createCanyonLight.follow` — 14 of 36 lines
- `src/render/canyonLight.ts#createCanyonLight.layVolume` — 25 of 25 lines
- `src/render/carBody.ts#buildCarModel` — 54 of 577 lines
- `src/render/destruction.ts#Destruction` — 3 of 13 lines
- `src/render/destruction.ts#Piece` — 3 of 15 lines
- `src/render/destruction.ts#Ruin` — 3 of 9 lines
- `src/render/destruction.ts#createDestruction` — 5 of 343 lines
- `src/render/destruction.ts#createDestruction.fail` — 4 of 7 lines
- `src/render/destruction.ts#createDestruction.hit` — 17 of 17 lines
- `src/render/destruction.ts#createDestruction.land` — 3 of 33 lines
- `src/render/destruction.ts#createDestruction.loosen` — 6 of 18 lines
- `src/render/destruction.ts#createDestruction.make` — 22 of 22 lines
- `src/render/destruction.ts#createDestruction.place` — 34 of 34 lines
- `src/render/destruction.ts#createDestruction.strike` — 46 of 55 lines
- `src/render/destruction.ts#createDestruction.unsupported` — 10 of 14 lines
- `src/render/destruction.ts#createDestruction.update` — 22 of 40 lines
- `src/render/destruction.ts#createDestruction.workerOf` — 18 of 26 lines
- `src/render/flatAtlas.ts#createFlatAtlas.readContent` — 5 of 21 lines
- `src/render/flatAtlas.ts#createFlatAtlas.readWholeContent` — 4 of 19 lines
- `src/render/fogLayer.ts#createFogField` — 3 of 153 lines
- `src/render/fogLayer.ts#createFogField.follow` — 5 of 8 lines
- `src/render/fogLayer.ts#createFogField.layer` — 3 of 6 lines
- `src/render/fogLayer.ts#createFogField.raster` — 71 of 71 lines
- `src/render/fogLayer.ts#createFogField.update` — 6 of 10 lines
- `src/render/fogLayer.ts#createFogField.whole` — 3 of 9 lines
- `src/render/groundShape.ts#GroundJobAnswer` — 1 of 1 lines
- `src/render/groundShape.ts#GroundShapeAsk` — 3 of 5 lines
- `src/render/groundShape.ts#PadsAsk` — 3 of 7 lines
- `src/render/groundShape.ts#padsOfAsk` — 8 of 18 lines
- `src/render/groundShape.ts#shapeGroundAsk` — 41 of 45 lines
- `src/render/groundShapePool.ts#GroundShapePool.pads` — 3 of 3 lines
- `src/render/groundShapePool.ts#GroundWorkers` — 3 of 10 lines
- `src/render/materials.ts#createMaterials` — 8 of 114 lines
- `src/render/mesh/roadResidency.ts#restoreCell` — 8 of 26 lines
- `src/render/networkFarRoads.ts#NetworkFarRoads.clearMeshes` — 5 of 9 lines
- `src/render/networkFarRoads.ts#NetworkFarRoads.meshesOf` — 1 of 1 lines
- `src/render/networkFarRoads.ts#NetworkFarRoads.place` — 18 of 39 lines
- `src/render/occlusion.ts#OcclusionBuffer.build` — 6 of 31 lines
- `src/render/playEffects.ts#PlayEffects` — 3 of 35 lines
- `src/render/playEffects.ts#PlayEffectsContext` — 7 of 17 lines
- `src/render/playEffects.ts#createPlayEffects` — 28 of 417 lines
- `src/render/playEffects.ts#createPlayEffects.groundChanged` — 5 of 10 lines
- `src/render/playEffects.ts#createPlayEffects.lotsAt` — 2 of 2 lines
- `src/render/playEffects.ts#createPlayEffects.strikeBuilding` — 5 of 7 lines
- `src/render/postprocess.ts#PostChain` — 3 of 30 lines
- `src/render/renderer.ts#SceneHandle` — 6 of 170 lines
- `src/render/renderer.ts#createSceneRenderer` — 324 of 3774 lines
- `src/render/renderer.ts#createSceneRenderer.applyGeometryBudget` — 3 of 8 lines
- `src/render/renderer.ts#createSceneRenderer.applyQuality` — 3 of 17 lines
- `src/render/renderer.ts#createSceneRenderer.bankBox` — 8 of 8 lines
- `src/render/renderer.ts#createSceneRenderer.buildingsOnGroundSince` — 9 of 9 lines
- `src/render/renderer.ts#createSceneRenderer.coverFree` — 1 of 1 lines
- `src/render/renderer.ts#createSceneRenderer.floorBox` — 8 of 8 lines
- `src/render/renderer.ts#createSceneRenderer.followFloors` — 1 of 1 lines
- `src/render/renderer.ts#createSceneRenderer.governGeometry` — 9 of 18 lines
- `src/render/renderer.ts#createSceneRenderer.levelGroundDigest` — 4 of 8 lines
- `src/render/renderer.ts#createSceneRenderer.loadEffects` — 9 of 18 lines
- `src/render/renderer.ts#createSceneRenderer.natureOpen` — 3 of 11 lines
- `src/render/renderer.ts#createSceneRenderer.occluderDrawn` — 7 of 7 lines
- `src/render/renderer.ts#createSceneRenderer.padsAskOf` — 12 of 12 lines
- `src/render/renderer.ts#createSceneRenderer.shapeBlocksSteps` — 37 of 54 lines
- `src/render/renderer.ts#createSceneRenderer.shapeGround` — 20 of 47 lines
- `src/render/renderer.ts#createSceneRenderer.switchedOff` — 16 of 16 lines
- `src/render/renderer.ts#createSceneRenderer.warmBlastShaders` — 14 of 34 lines
- `src/render/renderer.ts#createSceneRenderer.warmPreviewShaders` — 21 of 38 lines
- `src/render/renderer.ts#createSceneRenderer.worldSteps` — 10 of 105 lines
- `src/render/roadSurfaces.ts#SurfaceReuse` — 3 of 35 lines
- `src/render/roadSurfaces.ts#roadSurfaceSteps` — 24 of 512 lines
- `src/render/terrain.ts#terrainMaterial` — 3 of 26 lines
- `src/render/vehicleModels.ts#VehicleModel` — 3 of 33 lines
- `src/render/vehicleModels.ts#buildBusModel` — 37 of 284 lines
- `src/render/vehicleModels.ts#buildTruckModel` — 19 of 176 lines
- `src/sim/agents/bay.ts#Bay` — 3 of 22 lines
- `src/sim/agents/buildingOrder.ts#BuildingDelta` — 3 of 6 lines
- `src/sim/agents/buildingOrder.ts#BuildingsInOrder.file` — 4 of 4 lines
- `src/sim/agents/buildingOrder.ts#BuildingsInOrder.filedAt` — 1 of 1 lines
- `src/sim/agents/buildingOrder.ts#BuildingsInOrder.has` — 3 of 3 lines
- `src/sim/agents/buildingOrder.ts#BuildingsInOrder.ids` — 3 of 3 lines
- `src/sim/agents/buildingOrder.ts#BuildingsInOrder.list` — 1 of 1 lines
- `src/sim/agents/buildingOrder.ts#BuildingsInOrder.merge` — 23 of 25 lines
- `src/sim/agents/buildingOrder.ts#BuildingsInOrder.rank` — 3 of 3 lines
- `src/sim/agents/buildingOrder.ts#BuildingsInOrder.reread` — 6 of 9 lines
- `src/sim/agents/buildingOrder.ts#BuildingsInOrder.resort` — 4 of 6 lines
- `src/sim/agents/buildingOrder.ts#BuildingsInOrder.slot` — 8 of 8 lines
- `src/sim/agents/buildingOrder.ts#BuildingsInOrder.unfile` — 6 of 6 lines
- `src/sim/agents/buildingOrder.ts#BuildingsInOrder.update` — 16 of 37 lines
- `src/sim/agents/buildingOrder.ts#StoreOrder.apply` — 31 of 39 lines
- `src/sim/agents/buildingOrder.ts#StoreOrder.follow` — 3 of 9 lines
- `src/sim/agents/buildingOrder.ts#StoreOrder.has` — 3 of 3 lines
- `src/sim/agents/buildingOrder.ts#StoreOrder.place` — 4 of 5 lines
- `src/sim/agents/buildingOrder.ts#StoreOrder.rank` — 4 of 4 lines
- `src/sim/agents/kerbBays.ts#EntranceBook.box` — 8 of 11 lines
- `src/sim/agents/kerbBays.ts#EntranceBook.each` — 5 of 6 lines
- `src/sim/agents/kerbBays.ts#EntranceBook.file` — 12 of 16 lines
- `src/sim/agents/kerbBays.ts#EntranceBook.put` — 10 of 12 lines
- `src/sim/agents/kerbBays.ts#EntranceBook.same` — 9 of 11 lines
- `src/sim/agents/kerbBays.ts#KerbReads` — 3 of 12 lines
- `src/sim/agents/kerbBays.ts#NO_LOT` — 1 of 1 lines
- `src/sim/agents/kerbBays.ts#entrancesFacing` — 15 of 17 lines
- `src/sim/agents/lorryDocks.ts#SOLID` — 4 of 4 lines
- `src/sim/agents/lorryDocks.ts#Yard.constructor` — 26 of 48 lines
- `src/sim/agents/lorryDocks.ts#Yard.toWorld` — 4 of 4 lines
- `src/sim/agents/lorryDocks.ts#YardDock` — 3 of 12 lines
- `src/sim/agents/lorryDocks.ts#buildYard` — 31 of 69 lines
- `src/sim/agents/lorryDocks.ts#elementRing` — 6 of 6 lines
- `src/sim/agents/lorryDocks.ts#lotsNear` — 2 of 3 lines
- `src/sim/agents/lorryDocks.ts#nearestGate` — 7 of 8 lines
- `src/sim/agents/lorryDocks.ts#ownLot` — 9 of 9 lines
- `src/sim/agents/lorryDocks.ts#yardBox` — 6 of 9 lines
- `src/sim/agents/lorryDocks.ts#yardInputs` — 6 of 18 lines
- `src/sim/agents/lorryTraffic.ts#Lorry` — 3 of 24 lines
- `src/sim/agents/lorryTraffic.ts#LorryTraffic.call` — 26 of 45 lines
- `src/sim/agents/lorryTraffic.ts#LorryTraffic.plan` — 11 of 12 lines
- `src/sim/agents/lorryTraffic.ts#LorryTraffic.planning` — 1 of 1 lines
- `src/sim/agents/lorryTraffic.ts#LorryTraffic.readYards` — 4 of 7 lines
- `src/sim/agents/lorryTraffic.ts#LorryTraffic.reset` — 3 of 7 lines
- `src/sim/agents/lorryTraffic.ts#LorryTraffic.yardFor` — 14 of 22 lines
- `src/sim/agents/lorryTraffic.ts#LorryTraffic.yards` — 1 of 1 lines
- `src/sim/agents/lorryTraffic.ts#YardList.docks` — 1 of 1 lines
- `src/sim/agents/lorryTraffic.ts#YardList.gone` — 1 of 1 lines
- `src/sim/agents/lorryTraffic.ts#YardList.has` — 3 of 3 lines
- `src/sim/agents/lorryTraffic.ts#YardList.list` — 1 of 1 lines
- `src/sim/agents/lorryTraffic.ts#YardList.read` — 10 of 13 lines
- `src/sim/agents/lotBays.ts#BayShare` — 12 of 17 lines
- `src/sim/agents/lotBays.ts#LotBlock` — 1 of 1 lines
- `src/sim/agents/lotBays.ts#LotRecord` — 3 of 8 lines
- `src/sim/agents/lotBays.ts#WallRing` — 3 of 5 lines
- `src/sim/agents/lotBays.ts#bayRows` — 66 of 73 lines
- `src/sim/agents/lotBays.ts#carGates` — 3 of 3 lines
- `src/sim/agents/lotBays.ts#clearOfWalls` — 11 of 18 lines
- `src/sim/agents/lotBays.ts#exitsOf` — 15 of 27 lines
- `src/sim/agents/lotBays.ts#gateExits` — 22 of 27 lines
- `src/sim/agents/lotBays.ts#gatedProperty` — 8 of 12 lines
- `src/sim/agents/lotBays.ts#personGates` — 3 of 4 lines
- `src/sim/agents/lotBays.ts#shareOf` — 16 of 46 lines
- `src/sim/agents/lotBays.ts#stallRoom` — 15 of 32 lines
- `src/sim/agents/lotBays.ts#wallsNear` — 6 of 7 lines
- `src/sim/agents/lotDoors.ts#DoorWay` — 3 of 9 lines
- `src/sim/agents/lotDoors.ts#streetWay` — 8 of 12 lines
- `src/sim/agents/lotDoors.ts#workOut` — 34 of 48 lines
- `src/sim/agents/lotNav.ts#LotGridOptions` — 3 of 5 lines
- `src/sim/agents/lotNav.ts#LotPart` — 1 of 1 lines
- `src/sim/agents/lotNav.ts#PEOPLE_SOLID` — 4 of 4 lines
- `src/sim/agents/lotNav.ts#SOLID` — 4 of 4 lines
- `src/sim/agents/lotNav.ts#buildLotGrid` — 17 of 71 lines
- `src/sim/agents/lotNav.ts#elementCorners` — 7 of 7 lines
- `src/sim/agents/lotTraffic.ts#kindClass` — 4 of 4 lines
- `src/sim/agents/parking.ts#Asked` — 1 of 1 lines
- `src/sim/agents/parking.ts#BakedBays` — 4 of 9 lines
- `src/sim/agents/parking.ts#BayBook` — 5 of 32 lines
- `src/sim/agents/parking.ts#Walls.read` — 1 of 1 lines
- `src/sim/agents/parking.ts#baySources` — 3 of 7 lines
- `src/sim/agents/parking.ts#baysNow` — 4 of 43 lines
- `src/sim/agents/parking.ts#baysOfSource` — 4 of 6 lines
- `src/sim/agents/parking.ts#buildingWalls` — 11 of 13 lines
- `src/sim/agents/parking.ts#lotsReached` — 6 of 7 lines
- `src/sim/agents/trafficBuildings.ts#BuildingChange` — 4 of 5 lines
- `src/sim/agents/trafficBuildings.ts#BuildingSent` — 4 of 4 lines
- `src/sim/agents/trafficBuildings.ts#TrafficBlock` — 1 of 1 lines
- `src/sim/agents/trafficBuildings.ts#TrafficBuilding` — 7 of 23 lines
- `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.apply` — 10 of 22 lines
- `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.eachEntrances` — 3 of 3 lines
- `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.eachFootprints` — 3 of 3 lines
- `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.entrances` — 5 of 7 lines
- `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.facts` — 4 of 4 lines
- `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.factsAt` — 5 of 9 lines
- `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.footprints` — 10 of 14 lines
- `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.get` — 4 of 4 lines
- `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.ids` — 5 of 6 lines
- `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.keepRef` — 11 of 11 lines
- `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.load` — 13 of 26 lines
- `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.nearAny` — 4 of 10 lines
- `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.nearIds` — 3 of 3 lines
- `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.place` — 3 of 29 lines
- `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.readers` — 1 of 1 lines
- `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.record` — 12 of 15 lines
- `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.remove` — 3 of 11 lines
- `src/sim/agents/trafficBuildings.ts#TrafficBuildings` — 12 of 25 lines
- `src/sim/agents/trafficBuildings.ts#TrafficFacts` — 3 of 12 lines
- `src/sim/agents/trafficBuildings.ts#TrafficMap` — 4 of 6 lines
- `src/sim/agents/trafficBuildings.ts#TrafficPart` — 1 of 1 lines
- `src/sim/agents/trafficBuildings.ts#buildingChangesOf` — 6 of 8 lines
- `src/sim/agents/trafficBuildings.ts#docBuildings` — 3 of 47 lines
- `src/sim/agents/trafficBuildings.ts#docBuildings.eachEntrances` — 1 of 1 lines
- `src/sim/agents/trafficBuildings.ts#docBuildings.eachFacts` — 1 of 1 lines
- `src/sim/agents/trafficBuildings.ts#docBuildings.eachFootprints` — 1 of 1 lines
- `src/sim/agents/trafficBuildings.ts#docBuildings.entrances` — 4 of 4 lines
- `src/sim/agents/trafficBuildings.ts#docBuildings.facts` — 4 of 4 lines
- `src/sim/agents/trafficBuildings.ts#docBuildings.factsOfRecord` — 6 of 6 lines
- `src/sim/agents/trafficBuildings.ts#docBuildings.footprints` — 4 of 4 lines
- `src/sim/agents/trafficBuildings.ts#docBuildings.get` — 4 of 4 lines
- `src/sim/agents/trafficBuildings.ts#docBuildings.ids` — 1 of 1 lines
- `src/sim/agents/trafficBuildings.ts#docBuildings.near` — 1 of 1 lines
- `src/sim/agents/trafficBuildings.ts#docBuildings.nearAny` — 1 of 1 lines
- `src/sim/agents/trafficBuildings.ts#docBuildings.nearIds` — 1 of 1 lines
- `src/sim/agents/trafficBuildings.ts#docBuildings.trafficBuilding` — 5 of 5 lines
- `src/sim/agents/trafficBuildings.ts#frontageBound` — 22 of 26 lines
- `src/sim/agents/trafficBuildings.ts#partOf` — 3 of 3 lines
- `src/sim/agents/trafficBuildings.ts#project` — 14 of 14 lines
- `src/sim/agents/trafficBuildings.ts#takeTrafficMap` — 3 of 6 lines
- `src/sim/agents/trafficBuildings.ts#trafficBuilding` — 1 of 1 lines
- `src/sim/agents/trafficBuildings.ts#trafficChangesOf` — 4 of 7 lines
- `src/sim/agents/trafficBuildings.ts#trafficMapOf` — 7 of 14 lines
- `src/sim/agents/trafficBuildings.ts#traitsOf` — 2 of 2 lines
- `src/sim/ambient/ambientIndex.ts#SightWalls.sync` — 5 of 9 lines
- `src/sim/ambient/demand.ts#kindOf` — 8 of 8 lines
- `src/sim/host/simClient.ts#SimClient.keyChanges` — 15 of 17 lines
- `src/sim/host/simClient.ts#SimClient.lotPlan` — 3 of 6 lines
- `src/sim/host/simClient.ts#SimClient.markSent` — 4 of 8 lines
- `src/sim/host/simClient.ts#SimClient.sendChanges` — 23 of 65 lines
- `src/sim/host/simClient.ts#recordOf` — 5 of 8 lines
- `src/sim/host/simHost.ts#SimHost.lotPlan` — 3 of 6 lines
- `src/ui/about.ts#mountAbout` — 3 of 10 lines
- `src/ui/about.ts#mountAbout.paint` — 3 of 3 lines
- `src/ui/about.ts#openAbout` — 4 of 8 lines
- `src/ui/materialSwatch.ts#BASE` — 1 of 1 lines
- `src/ui/materialSwatch.ts#draw` — 165 of 176 lines
- `src/ui/materialSwatch.ts#materialSwatch` — 7 of 15 lines
- `src/ui/planSwatch.ts#OUTLINES` — 26 of 26 lines
- `src/ui/planSwatch.ts#planSwatch` — 13 of 52 lines
- `src/ui/v2/shell.ts#ShellDeps` — 3 of 72 lines
- `src/ui/v2/shell.ts#mountShell` — 29 of 1639 lines
- `src/ui/v2/shell.ts#mountShell.BARRIER_ICON` — 2 of 2 lines
- `src/ui/v2/shell.ts#mountShell.ask` — 5 of 5 lines
- `src/ui/v2/shell.ts#mountShell.picture` — 4 of 4 lines
- `src/ui/v2/shell.ts#mountShell.render` — 4 of 59 lines
- `src/ui/v2/shell.ts#mountShell.renderBuild` — 52 of 63 lines
- `src/ui/v2/shell.ts#mountShell.renderModels` — 22 of 45 lines
- `src/ui/v2/shell.ts#mountShell.renderModels.fill` — 3 of 7 lines
- `src/ui/v2/shell.ts#mountShell.renderSection` — 59 of 60 lines
- `src/ui/v2/shell.ts#mountShell.seeInside` — 1 of 1 lines
- `src/ui/v2/shell.ts#mountShell.sigOf` — 2 of 2 lines
- `src/ui/v2/shell.ts#mountShell.syncInside` — 5 of 6 lines
- `src/ui/v2/shell.ts#mountShell.toolButton` — 8 of 9 lines
- `src/ui/v2/shell.ts#mountShell.toolButton.run` — 1 of 1 lines
- `src/ui/v2/shell.ts#mountShell.updateHint` — 4 of 4 lines
- `src/world/cityGen/plan.ts#CityPlan` — 3 of 9 lines
- `src/world/cityGen/plan.ts#planCity.zoneAt` — 18 of 18 lines
- `src/world/cityGen/squares.ts#blocks` — 22 of 35 lines
- `src/world/cityGen/squares.ts#blocks.near` — 10 of 10 lines
- `src/world/cityGen/squares.ts#planSquares` — 50 of 53 lines
- `src/world/doc.ts#PreparedGrown` — 3 of 4 lines
- `src/world/doc.ts#RoadDoc.allocatorMarks` — 3 of 3 lines
- `src/world/doc.ts#RoadDoc.buildings` — 1 of 1 lines
- `src/world/doc.ts#RoadDoc.buildingsMovedIn` — 3 of 5 lines
- `src/world/doc.ts#RoadDoc.fromBinary` — 3 of 3 lines
- `src/world/doc.ts#RoadDoc.fromJSON` — 60 of 187 lines
- `src/world/doc.ts#RoadDoc.image` — 3 of 3 lines
- `src/world/doc.ts#RoadDoc.layOver` — 5 of 47 lines
- `src/world/doc.ts#RoadDoc.listOf` — 5 of 16 lines
- `src/world/doc.ts#RoadDoc.listen` — 4 of 13 lines
- `src/world/doc.ts#RoadDoc.listen.tell` — 3 of 3 lines
- `src/world/doc.ts#RoadDoc.lotStore` — 1 of 1 lines
- `src/world/doc.ts#RoadDoc.lots` — 3 of 3 lines
- `src/world/doc.ts#RoadDoc.lotsIn` — 3 of 3 lines
- `src/world/doc.ts#RoadDoc.lotsMovedIn` — 3 of 5 lines
- `src/world/doc.ts#RoadDoc.openLots` — 9 of 9 lines
- `src/world/doc.ts#RoadDoc.openLotsIndex` — 1 of 1 lines
- `src/world/doc.ts#RoadDoc.ownList` — 3 of 10 lines
- `src/world/doc.ts#RoadDoc.recordOf` — 6 of 12 lines
- `src/world/doc.ts#RoadDoc.replaceContents` — 10 of 149 lines
- `src/world/doc.ts#RoadDoc.reserveAllocators` — 3 of 8 lines
- `src/world/doc.ts#RoadDoc.restore` — 25 of 105 lines
- `src/world/doc.ts#RoadDoc.restoreValue` — 8 of 27 lines
- `src/world/doc.ts#RoadDoc.savedGlobals` — 5 of 18 lines
- `src/world/doc.ts#RoadDoc.serialized` — 9 of 35 lines
- `src/world/doc.ts#RoadDoc.takeFork` — 4 of 49 lines
- `src/world/doc.ts#RoadDoc.tileGrown` — 1 of 1 lines
- `src/world/doc.ts#RoadDoc.toText` — 4 of 11 lines
- `src/world/doc.ts#RoadDoc.valueOf` — 5 of 15 lines
- `src/world/doc.ts#RoadDoc.worldPart` — 7 of 26 lines
- `src/world/doc.ts#RoadDoc.zoneMarks` — 1 of 1 lines
- `src/world/doc.ts#RoadDoc.zones` — 1 of 1 lines
- `src/world/doc.ts#SerializedDoc` — 6 of 75 lines
- `src/world/doc.ts#TileRecords` — 4 of 26 lines
- `src/world/doc.ts#lotRecord` — 5 of 5 lines
- `src/world/docBinary.ts#DocImage` — 3 of 4 lines
- `src/world/docBinary.ts#imageBytes` — 5 of 5 lines
- `src/world/docBinary.ts#imageOf` — 3 of 12 lines
- `src/world/docBinary.ts#packImage` — 6 of 28 lines
- `src/world/parkingLayout.ts#entrancesNear` — 3 of 7 lines
- `src/world/rescale.ts#rescaleSerializedDoc` — 8 of 55 lines
- `src/world/solids.ts#POST_ELEMENTS` — 1 of 1 lines
- `src/world/solids.ts#createSolids` — 7 of 47 lines
- `src/world/solids.ts#solidsOfItem` — 13 of 38 lines

## Resources

- shared entry `street` — src/core/perfBudget.ts:45: `street: { triangles: 16_000_000, drawCalls: 700 + 300, gpuMs: 12, fps: 60 }`
- shared entry `district` — src/core/perfBudget.ts:46: `district: { triangles: 16_000_000, drawCalls: 800 + 300, gpuMs: 12, fps: 60 }`
- shared entry `map` — src/core/perfBudget.ts:47: `map: { triangles: 8_000_000, drawCalls: 300 + 100, gpuMs: 12, fps: 50 }`
- feature entry `hint.lot.front` — src/ui/i18n/en.ts:28: `'hint.lot.front': 'Front set: the building will face this side.'`
- feature entry `hint.lot.frontPick` — src/ui/i18n/en.ts:29: `'hint.lot.frontPick': 'Click near a side of a lot.'`
- feature entry `hint.lot.split` — src/ui/i18n/en.ts:34: `'hint.lot.split': 'Lot split in two (Shift: front and back).'`
- feature entry `hint.lot.join` — src/ui/i18n/en.ts:35: `'hint.lot.join': 'Lots joined.'`
- feature entry `hint.lot.joinPick` — src/ui/i18n/en.ts:36: `'hint.lot.joinPick': 'Now click the lot beside it.'`
- feature entry `hint.lot.joinFail` — src/ui/i18n/en.ts:37: `'hint.lot.joinFail': 'Those lots do not share a side.'`
- feature entry `hint.lot.added` — src/ui/i18n/en.ts:38: `'hint.lot.added': 'New lot added.'`
- feature entry `hint.lot.addFail` — src/ui/i18n/en.ts:39: `'hint.lot.addFail': 'No room for a lot there.'`
- shared entry `hint.lot.deleted` — src/ui/i18n/en.ts:40: `'hint.lot.deleted': 'Deleted.'`
- feature entry `hint.lot.splitFail` — src/ui/i18n/en.ts:46: `'hint.lot.splitFail': 'Nothing worth a lot is left there.'`
- feature entry `zone.lot.size` — src/ui/i18n/en.ts:47: `'zone.lot.size': '{w} x {d} m'`
- feature entry `hint.zone.removed` — src/ui/i18n/en.ts:56: `'hint.zone.removed': 'Zone removed'`
- feature entry `hint.zone.painted` — src/ui/i18n/en.ts:57: `'hint.zone.painted': 'Zone painted: buildings will grow on it'`
- feature entry `hint.zone.empty` — src/ui/i18n/en.ts:58: `'hint.zone.empty': 'No lot here: draw a lot to zone it'`
- feature entry `hint.zone.pending` — src/ui/i18n/en.ts:59: `'hint.zone.pending': 'Zoning as soon as the street lots are worked out…'`
- feature entry `build.stamp` — src/ui/i18n/en.ts:174: `'build.stamp': '{branch} · {hash} · {date}'`
- feature entry `hint.blocks.built` — src/ui/i18n/en.ts:401: `'hint.blocks.built': 'Blocks built'`
- shared entry `hint.road.invalid` — src/ui/i18n/en.ts:721: `'hint.road.invalid': 'This connection needs more distance or a different endpoint.'`
- feature entry `hint.strike.hit` — src/ui/i18n/en.ts:908: `'hint.strike.hit': 'Hit: keep striking to bring it down'`
- feature entry `hint.strike.down` — src/ui/i18n/en.ts:909: `'hint.strike.down': 'The building came down'`
- feature entry `hint.strike.deaths` — src/ui/i18n/en.ts:910: `'hint.strike.deaths': 'Hit: there were casualties'`
- feature entry `hint.strike.ground` — src/ui/i18n/en.ts:911: `'hint.strike.ground': 'Struck the ground'`
- feature entry `hint.building.renovated` — src/ui/i18n/en.ts:912: `'hint.building.renovated': 'Building renovated'`
- feature entry `building.preset.block` — src/ui/i18n/en.ts:1035: `'building.preset.block': 'Block'`
- feature entry `building.preset.house` — src/ui/i18n/en.ts:1036: `'building.preset.house': 'House'`
- feature entry `building.preset.rowhouse` — src/ui/i18n/en.ts:1037: `'building.preset.rowhouse': 'Row house'`
- feature entry `building.preset.apartments` — src/ui/i18n/en.ts:1038: `'building.preset.apartments': 'Apartments'`
- feature entry `building.preset.tower` — src/ui/i18n/en.ts:1039: `'building.preset.tower': 'Tower on podium'`
- feature entry `building.preset.shop` — src/ui/i18n/en.ts:1040: `'building.preset.shop': 'Shop'`
- feature entry `building.preset.office` — src/ui/i18n/en.ts:1041: `'building.preset.office': 'Office block'`
- feature entry `building.preset.mixed` — src/ui/i18n/en.ts:1042: `'building.preset.mixed': 'Mixed-use block'`
- feature entry `building.preset.courtyard` — src/ui/i18n/en.ts:1043: `'building.preset.courtyard': 'Courtyard building'`
- feature entry `building.preset.warehouse` — src/ui/i18n/en.ts:1044: `'building.preset.warehouse': 'Warehouse'`
- feature entry `building.preset.factory` — src/ui/i18n/en.ts:1045: `'building.preset.factory': 'Factory'`
- feature entry `building.element.noRoom` — src/ui/i18n/en.ts:1046: `'building.element.noRoom': 'No room to repeat it along the building'`
- feature entry `building.relief.pickFace` — src/ui/i18n/en.ts:1047: `'building.relief.pickFace': 'Click a facade (Shift+click or long press for more bays), then pull the yellow arrow'`
- feature entry `building.style.brick` — src/ui/i18n/en.ts:1048: `'building.style.brick': 'Brick'`
- feature entry `building.style.modern` — src/ui/i18n/en.ts:1049: `'building.style.modern': 'Modern'`
- feature entry `building.style.mediterranean` — src/ui/i18n/en.ts:1050: `'building.style.mediterranean': 'Mediterranean'`
- feature entry `building.style.industrial` — src/ui/i18n/en.ts:1051: `'building.style.industrial': 'Industrial'`
- feature entry `building.style.timber` — src/ui/i18n/en.ts:1052: `'building.style.timber': 'Timber'`
- feature entry `building.material.pickFace` — src/ui/i18n/en.ts:1053: `'building.material.pickFace': 'Click a facade to paint that face'`
- feature entry `building.finish.plaster` — src/ui/i18n/en.ts:1054: `'building.finish.plaster': 'Plaster'`
- feature entry `building.finish.ceramic` — src/ui/i18n/en.ts:1055: `'building.finish.ceramic': 'Porcelain tile'`
- feature entry `building.finish.brick` — src/ui/i18n/en.ts:1056: `'building.finish.brick': 'Brick'`
- feature entry `building.finish.stone` — src/ui/i18n/en.ts:1057: `'building.finish.stone': 'Stone'`
- feature entry `building.finish.concrete` — src/ui/i18n/en.ts:1058: `'building.finish.concrete': 'Concrete'`
- feature entry `building.finish.wood` — src/ui/i18n/en.ts:1059: `'building.finish.wood': 'Wood'`
- feature entry `building.finish.metal` — src/ui/i18n/en.ts:1060: `'building.finish.metal': 'Metal'`
- feature entry `building.finish.glass` — src/ui/i18n/en.ts:1061: `'building.finish.glass': 'Glass'`
- feature entry `building.finish.tile` — src/ui/i18n/en.ts:1062: `'building.finish.tile': 'Tiles'`
- feature entry `building.finish.roofing` — src/ui/i18n/en.ts:1063: `'building.finish.roofing': 'Roofing'`
- feature entry `building.blueprintName` — src/ui/i18n/en.ts:1064: `'building.blueprintName': 'Name for this blueprint'`
- feature entry `building.blueprintSaved` — src/ui/i18n/en.ts:1065: `'building.blueprintSaved': 'Blueprint saved'`
- feature entry `building.blueprintNotSaved` — src/ui/i18n/en.ts:1066: `'building.blueprintNotSaved': 'Blueprint not saved: browser storage is full or blocked'`
- feature entry `building.blueprintNotRemoved` — src/ui/i18n/en.ts:1067: `'building.blueprintNotRemoved': 'Blueprint not removed: browser storage is full or blocked'`
- feature entry `building.placed` — src/ui/i18n/en.ts:1068: `'building.placed': 'Building placed: drag the arrows to shape it'`
- feature entry `building.copied` — src/ui/i18n/en.ts:1069: `'building.copied': 'Building copied: Ctrl+V to place a copy'`
- feature entry `building.problem.size` — src/ui/i18n/en.ts:1070: `'building.problem.size': 'Size out of range'`
- feature entry `building.problem.overlap` — src/ui/i18n/en.ts:1071: `'building.problem.overlap': 'Two volumes would overlap'`
- feature entry `building.problem.support` — src/ui/i18n/en.ts:1072: `'building.problem.support': 'A volume would hang over nothing'`
- feature entry `building.problem.footprint` — src/ui/i18n/en.ts:1073: `'building.problem.footprint': 'Nothing would stand on the ground'`
- feature entry `building.problem.bounds` — src/ui/i18n/en.ts:1074: `'building.problem.bounds': 'Outside the map'`
- feature entry `building.problem.road` — src/ui/i18n/en.ts:1075: `'building.problem.road': 'Overlaps a road'`
- feature entry `building.problem.building` — src/ui/i18n/en.ts:1076: `'building.problem.building': 'Overlaps another building'`
- feature entry `building.problem.slope` — src/ui/i18n/en.ts:1077: `'building.problem.slope': 'The ground is too steep here'`
- feature entry `building.demolished.one` — src/ui/i18n/en.ts:1078: `'building.demolished.one': 'The road demolished 1 building'`
- feature entry `building.demolished.other` — src/ui/i18n/en.ts:1079: `'building.demolished.other': 'The road demolished {count} buildings'`
- feature entry `building.floors.one` — src/ui/i18n/en.ts:1080: `'building.floors.one': '{count} floor'`
- feature entry `building.floors.other` — src/ui/i18n/en.ts:1081: `'building.floors.other': '{count} floors'`
- feature entry `building.problem.outline` — src/ui/i18n/en.ts:1082: `'building.problem.outline': 'The outline crosses itself or is too small'`
- feature entry `building.problem.roofSpace` — src/ui/i18n/en.ts:1083: `'building.problem.roofSpace': 'Choose a clear spot fully inside a roof.'`
- feature entry `building.problem.cut` — src/ui/i18n/en.ts:1084: `'building.problem.cut': 'The cut must open into an exterior edge. Build a closed courtyard from adjacent masses.'`
- feature entry `building.problem.setback` — src/ui/i18n/en.ts:1085: `'building.problem.setback': 'Not enough room for an automatic setback. Sketch a smaller mass on the roof.'`
- feature entry `creator.dock.scope.bay` — src/ui/i18n/en.ts:1086: `'creator.dock.scope.bay': 'Bay'`
- feature entry `creator.dock.scope.column` — src/ui/i18n/en.ts:1087: `'creator.dock.scope.column': 'Vertical column'`
- feature entry `creator.dock.scope.row` — src/ui/i18n/en.ts:1088: `'creator.dock.scope.row': 'Row of the floor'`
- feature entry `creator.dock.scope.storey` — src/ui/i18n/en.ts:1089: `'creator.dock.scope.storey': 'Whole floor'`
- feature entry `creator.dock.scope.side` — src/ui/i18n/en.ts:1090: `'creator.dock.scope.side': 'Whole face'`
- feature entry `creator.dock.scope.volume` — src/ui/i18n/en.ts:1091: `'creator.dock.scope.volume': 'Whole block'`
- feature entry `creator.dock.scope.zone` — src/ui/i18n/en.ts:1092: `'creator.dock.scope.zone': 'Zone (drag)'`
- feature entry `building.reshapeRelief` — src/ui/i18n/en.ts:1094: `'building.reshapeRelief': 'Flatten face reliefs before changing this tier’s outline.'`
- feature entry `creator.pattern.residential` — src/ui/i18n/en.ts:1095: `'creator.pattern.residential': 'Residential'`
- feature entry `creator.pattern.storefront` — src/ui/i18n/en.ts:1096: `'creator.pattern.storefront': 'Storefront'`
- feature entry `creator.pattern.office` — src/ui/i18n/en.ts:1097: `'creator.pattern.office': 'Offices'`
- feature entry `creator.pattern.industrial` — src/ui/i18n/en.ts:1098: `'creator.pattern.industrial': 'Industrial'`
- feature entry `creator.pattern.arcade` — src/ui/i18n/en.ts:1099: `'creator.pattern.arcade': 'Arcade'`
- feature entry `creator.pattern.gallery` — src/ui/i18n/en.ts:1100: `'creator.pattern.gallery': 'Open gallery'`
- feature entry `creator.pattern.artDeco` — src/ui/i18n/en.ts:1101: `'creator.pattern.artDeco': 'Art Deco'`
- feature entry `creator.pattern.artDecoCrown` — src/ui/i18n/en.ts:1102: `'creator.pattern.artDecoCrown': 'Deco crown'`
- feature entry `creator.pattern.observation` — src/ui/i18n/en.ts:1103: `'creator.pattern.observation': 'Glazed lookout'`
- feature entry `builder.grid` — src/ui/i18n/en.ts:1107: `'builder.grid': 'Grid'`
- feature entry `builder.view.frame` — src/ui/i18n/en.ts:1108: `'builder.view.frame': 'Frame the selection'`
- feature entry `builder.hideOthers` — src/ui/i18n/en.ts:1109: `'builder.hideOthers': 'Hide others'`
- feature entry `builder.floor.title` — src/ui/i18n/en.ts:1121: `'builder.floor.title': 'Which floor the face tools are working on'`
- feature entry `builder.snap.label` — src/ui/i18n/en.ts:1122: `'builder.snap.label': 'Snap'`
- feature entry `builder.snap.auto` — src/ui/i18n/en.ts:1123: `'builder.snap.auto': 'Auto'`
- feature entry `builder.snap.grid` — src/ui/i18n/en.ts:1124: `'builder.snap.grid': 'Grid'`
- feature entry `builder.snap.edge` — src/ui/i18n/en.ts:1125: `'builder.snap.edge': 'Edge'`
- feature entry `builder.snap.face` — src/ui/i18n/en.ts:1126: `'builder.snap.face': 'Face'`
- feature entry `builder.snap.centre` — src/ui/i18n/en.ts:1127: `'builder.snap.centre': 'Centre'`
- feature entry `builder.snap.building` — src/ui/i18n/en.ts:1128: `'builder.snap.building': 'Building'`
- feature entry `builder.snap.road` — src/ui/i18n/en.ts:1129: `'builder.snap.road': 'Road'`
- feature entry `builder.snap.off` — src/ui/i18n/en.ts:1130: `'builder.snap.off': 'Off'`
- feature entry `builder.sel.title` — src/ui/i18n/en.ts:1133: `'builder.sel.title': 'Selection'`
- feature entry `builder.sel.volume` — src/ui/i18n/en.ts:1134: `'builder.sel.volume': 'Mass {n}'`
- feature entry `builder.sel.building` — src/ui/i18n/en.ts:1135: `'builder.sel.building': 'Building'`
- feature entry `builder.sel.face` — src/ui/i18n/en.ts:1136: `'builder.sel.face': 'Face'`
- feature entry `builder.sel.floor` — src/ui/i18n/en.ts:1137: `'builder.sel.floor': 'floor {n}'`
- feature entry `builder.sel.element` — src/ui/i18n/en.ts:1138: `'builder.sel.element': 'Element'`
- feature entry `builder.sel.detail` — src/ui/i18n/en.ts:1139: `'builder.sel.detail': 'Roof detail'`
- feature entry `builder.field.storeys` — src/ui/i18n/en.ts:1140: `'builder.field.storeys': 'Floors'`
- feature entry `builder.field.storeyHeight` — src/ui/i18n/en.ts:1141: `'builder.field.storeyHeight': 'Floor height'`
- feature entry `builder.field.area` — src/ui/i18n/en.ts:1143: `'builder.field.area': 'Footprint'`
- feature entry `builder.field.pitch` — src/ui/i18n/en.ts:1144: `'builder.field.pitch': 'Pitch'`
- feature entry `builder.field.depth` — src/ui/i18n/en.ts:1145: `'builder.field.depth': 'Depth'`
- feature entry `builder.field.width` — src/ui/i18n/en.ts:1153: `'builder.field.width': 'Width'`
- feature entry `builder.field.flag` — src/ui/i18n/en.ts:1155: `'builder.field.flag': 'Flag'`
- feature entry `builder.flag.none` — src/ui/i18n/en.ts:1156: `'builder.flag.none': 'None'`
- feature entry `builder.field.flagPattern` — src/ui/i18n/en.ts:1157: `'builder.field.flagPattern': 'Flag design'`
- feature entry `builder.flagPattern.solid` — src/ui/i18n/en.ts:1158: `'builder.flagPattern.solid': 'Plain'`
- feature entry `builder.flagPattern.bicolourH` — src/ui/i18n/en.ts:1159: `'builder.flagPattern.bicolourH': 'Two horizontal bands'`
- feature entry `builder.flagPattern.bicolourV` — src/ui/i18n/en.ts:1160: `'builder.flagPattern.bicolourV': 'Two vertical bands'`
- feature entry `builder.flagPattern.tricolourH` — src/ui/i18n/en.ts:1161: `'builder.flagPattern.tricolourH': 'Three horizontal bands'`
- feature entry `builder.flagPattern.tricolourV` — src/ui/i18n/en.ts:1162: `'builder.flagPattern.tricolourV': 'Three vertical bands'`
- feature entry `builder.flagPattern.cross` — src/ui/i18n/en.ts:1163: `'builder.flagPattern.cross': 'Cross'`
- feature entry `builder.flagPattern.nordic` — src/ui/i18n/en.ts:1164: `'builder.flagPattern.nordic': 'Nordic cross'`
- feature entry `builder.flagPattern.diagonal` — src/ui/i18n/en.ts:1165: `'builder.flagPattern.diagonal': 'Diagonal'`
- feature entry `builder.flagPattern.saltire` — src/ui/i18n/en.ts:1166: `'builder.flagPattern.saltire': 'Saltire'`
- feature entry `builder.flagPattern.canton` — src/ui/i18n/en.ts:1167: `'builder.flagPattern.canton': 'Canton'`
- feature entry `builder.flagPattern.stripes` — src/ui/i18n/en.ts:1168: `'builder.flagPattern.stripes': 'Stripes with canton'`
- feature entry `builder.flagPattern.border` — src/ui/i18n/en.ts:1169: `'builder.flagPattern.border': 'Border'`
- feature entry `builder.flagPattern.disc` — src/ui/i18n/en.ts:1170: `'builder.flagPattern.disc': 'Disc'`
- feature entry `builder.flagPattern.chevron` — src/ui/i18n/en.ts:1171: `'builder.flagPattern.chevron': 'Chevron'`
- feature entry `builder.flagColour.0` — src/ui/i18n/en.ts:1172: `'builder.flagColour.0': 'White'`
- feature entry `builder.flagColour.1` — src/ui/i18n/en.ts:1173: `'builder.flagColour.1': 'Black'`
- feature entry `builder.flagColour.2` — src/ui/i18n/en.ts:1174: `'builder.flagColour.2': 'Red'`
- feature entry `builder.flagColour.3` — src/ui/i18n/en.ts:1175: `'builder.flagColour.3': 'Vermilion'`
- feature entry `builder.flagColour.4` — src/ui/i18n/en.ts:1176: `'builder.flagColour.4': 'Amber'`
- feature entry `builder.flagColour.5` — src/ui/i18n/en.ts:1177: `'builder.flagColour.5': 'Yellow'`
- feature entry `builder.flagColour.6` — src/ui/i18n/en.ts:1178: `'builder.flagColour.6': 'Green'`
- feature entry `builder.flagColour.7` — src/ui/i18n/en.ts:1179: `'builder.flagColour.7': 'Dark green'`
- feature entry `builder.flagColour.8` — src/ui/i18n/en.ts:1180: `'builder.flagColour.8': 'Blue'`
- feature entry `builder.flagColour.9` — src/ui/i18n/en.ts:1181: `'builder.flagColour.9': 'Navy'`
- feature entry `builder.flagColour.10` — src/ui/i18n/en.ts:1182: `'builder.flagColour.10': 'Light blue'`
- feature entry `builder.flagColour.11` — src/ui/i18n/en.ts:1183: `'builder.flagColour.11': 'Purple'`
- feature entry `builder.flagColour.12` — src/ui/i18n/en.ts:1184: `'builder.flagColour.12': 'Maroon'`
- feature entry `builder.flagColour.13` — src/ui/i18n/en.ts:1185: `'builder.flagColour.13': 'Brown'`
- feature entry `builder.flagColour.14` — src/ui/i18n/en.ts:1186: `'builder.flagColour.14': 'Grey'`
- feature entry `builder.flagColour.15` — src/ui/i18n/en.ts:1187: `'builder.flagColour.15': 'Pink'`
- feature entry `builder.flag.plain` — src/ui/i18n/en.ts:1188: `'builder.flag.plain': 'Plain'`
- feature entry `builder.flag.saoPaulo` — src/ui/i18n/en.ts:1189: `'builder.flag.saoPaulo': 'City of São Paulo'`
- feature entry `builder.flag.saoPauloState` — src/ui/i18n/en.ts:1190: `'builder.flag.saoPauloState': 'State of São Paulo'`
- feature entry `builder.field.height` — src/ui/i18n/en.ts:1192: `'builder.field.height': 'Height'`
- feature entry `builder.field.material` — src/ui/i18n/en.ts:1193: `'builder.field.material': 'Material'`
- feature entry `builder.quick.duplicate` — src/ui/i18n/en.ts:1194: `'builder.quick.duplicate': 'Duplicate'`
- feature entry `builder.quick.mirror` — src/ui/i18n/en.ts:1195: `'builder.quick.mirror': 'Mirror'`
- feature entry `builder.quick.group` — src/ui/i18n/en.ts:1196: `'builder.quick.group': 'Group with the building beside it'`
- feature entry `builder.quick.delete` — src/ui/i18n/en.ts:1197: `'builder.quick.delete': 'Demolish'`
- feature entry `builder.styleCopied` — src/ui/i18n/en.ts:1198: `'builder.styleCopied': 'Style copied: the brush carries it'`
- feature entry `builder.roof.ridge.label` — src/ui/i18n/en.ts:1199: `'builder.roof.ridge.label': 'Ridge'`
- feature entry `builder.roof.side.label` — src/ui/i18n/en.ts:1200: `'builder.roof.side.label': 'Falls to'`
- feature entry `builder.roof.ridge.x` — src/ui/i18n/en.ts:1201: `'builder.roof.ridge.x': 'Ridge left–right'`
- feature entry `builder.roof.ridge.y` — src/ui/i18n/en.ts:1202: `'builder.roof.ridge.y': 'Ridge front–back'`
- feature entry `builder.roof.side.front` — src/ui/i18n/en.ts:1203: `'builder.roof.side.front': 'Falls front'`
- feature entry `builder.roof.side.right` — src/ui/i18n/en.ts:1204: `'builder.roof.side.right': 'Falls right'`
- feature entry `builder.roof.side.back` — src/ui/i18n/en.ts:1205: `'builder.roof.side.back': 'Falls back'`
- feature entry `builder.roof.side.left` — src/ui/i18n/en.ts:1206: `'builder.roof.side.left': 'Falls left'`
- feature entry `builder.plan.finish` — src/ui/i18n/en.ts:1207: `'builder.plan.finish': 'Finish'`
- feature entry `builder.plan.back` — src/ui/i18n/en.ts:1208: `'builder.plan.back': 'Step back'`
- feature entry `builder.plan.cancel` — src/ui/i18n/en.ts:1209: `'builder.plan.cancel': 'Cancel'`
- feature entry `builder.noGroup` — src/ui/i18n/en.ts:1210: `'builder.noGroup': 'No building close enough to group'`
- feature entry `builder.grouped` — src/ui/i18n/en.ts:1211: `'builder.grouped': 'Grouped into one building'`
- feature entry `building.selectFirst` — src/ui/i18n/en.ts:1212: `'building.selectFirst': 'Select a building first'`
- feature entry `builder.category.draw` — src/ui/i18n/en.ts:1214: `'builder.category.draw': 'Draw'`
- shared entry `inside.toggle` — src/ui/i18n/en.ts:1215: `'inside.toggle': 'See inside: the building in the middle of the view (or double-click a building)'`
- feature entry `inside.down` — src/ui/i18n/en.ts:1216: `'inside.down': 'Floor below'`
- feature entry `inside.up` — src/ui/i18n/en.ts:1217: `'inside.up': 'Floor above'`
- feature entry `inside.ground` — src/ui/i18n/en.ts:1218: `'inside.ground': 'Ground'`
- feature entry `hint.inside.none` — src/ui/i18n/en.ts:1219: `'hint.inside.none': 'No building in the middle of the view: point the view at one, or double-click it'`
- feature entry `inside.floor` — src/ui/i18n/en.ts:1220: `'inside.floor': 'Floor {n}'`
- feature entry `building.fn.house` — src/ui/i18n/en.ts:1221: `'building.fn.house': 'House'`
- feature entry `building.fn.townhouse` — src/ui/i18n/en.ts:1222: `'building.fn.townhouse': 'Townhouse'`
- feature entry `building.fn.apartments` — src/ui/i18n/en.ts:1223: `'building.fn.apartments': 'Apartments'`
- feature entry `building.fn.residentialTower` — src/ui/i18n/en.ts:1224: `'building.fn.residentialTower': 'Residential tower'`
- feature entry `building.fn.cityHall` — src/ui/i18n/en.ts:1225: `'building.fn.cityHall': 'City hall'`
- feature entry `building.fn.council` — src/ui/i18n/en.ts:1226: `'building.fn.council': 'City council'`
- feature entry `building.fn.courthouse` — src/ui/i18n/en.ts:1227: `'building.fn.courthouse': 'Courthouse'`
- feature entry `building.fn.postOffice` — src/ui/i18n/en.ts:1228: `'building.fn.postOffice': 'Post office'`
- feature entry `building.fn.police` — src/ui/i18n/en.ts:1229: `'building.fn.police': 'Police station'`
- feature entry `building.fn.fireStation` — src/ui/i18n/en.ts:1230: `'building.fn.fireStation': 'Fire station'`
- feature entry `building.fn.hospital` — src/ui/i18n/en.ts:1231: `'building.fn.hospital': 'Hospital'`
- feature entry `building.fn.clinic` — src/ui/i18n/en.ts:1232: `'building.fn.clinic': 'Clinic'`
- feature entry `building.fn.school` — src/ui/i18n/en.ts:1233: `'building.fn.school': 'School'`
- feature entry `building.fn.university` — src/ui/i18n/en.ts:1234: `'building.fn.university': 'University'`
- feature entry `building.fn.library` — src/ui/i18n/en.ts:1235: `'building.fn.library': 'Library'`
- feature entry `building.fn.museum` — src/ui/i18n/en.ts:1236: `'building.fn.museum': 'Museum'`
- feature entry `building.fn.prison` — src/ui/i18n/en.ts:1237: `'building.fn.prison': 'Prison'`
- feature entry `building.fn.church` — src/ui/i18n/en.ts:1238: `'building.fn.church': 'Church'`
- feature entry `building.fn.cemetery` — src/ui/i18n/en.ts:1239: `'building.fn.cemetery': 'Cemetery'`
- feature entry `building.fn.busStation` — src/ui/i18n/en.ts:1240: `'building.fn.busStation': 'Bus station'`
- feature entry `building.fn.shop` — src/ui/i18n/en.ts:1241: `'building.fn.shop': 'Shop'`
- feature entry `building.fn.supermarket` — src/ui/i18n/en.ts:1242: `'building.fn.supermarket': 'Supermarket'`
- feature entry `building.fn.mall` — src/ui/i18n/en.ts:1243: `'building.fn.mall': 'Shopping centre'`
- feature entry `building.fn.bank` — src/ui/i18n/en.ts:1244: `'building.fn.bank': 'Bank'`
- feature entry `building.fn.pharmacy` — src/ui/i18n/en.ts:1245: `'building.fn.pharmacy': 'Pharmacy'`
- feature entry `building.fn.bakery` — src/ui/i18n/en.ts:1246: `'building.fn.bakery': 'Bakery'`
- feature entry `building.fn.restaurant` — src/ui/i18n/en.ts:1247: `'building.fn.restaurant': 'Restaurant'`
- feature entry `building.fn.snackBar` — src/ui/i18n/en.ts:1248: `'building.fn.snackBar': 'Snack bar'`
- feature entry `building.fn.bar` — src/ui/i18n/en.ts:1249: `'building.fn.bar': 'Bar'`
- feature entry `building.fn.nightclub` — src/ui/i18n/en.ts:1250: `'building.fn.nightclub': 'Nightclub'`
- feature entry `building.fn.cinema` — src/ui/i18n/en.ts:1251: `'building.fn.cinema': 'Cinema'`
- feature entry `building.fn.hotel` — src/ui/i18n/en.ts:1252: `'building.fn.hotel': 'Hotel'`
- feature entry `building.fn.gym` — src/ui/i18n/en.ts:1253: `'building.fn.gym': 'Gym'`
- feature entry `building.fn.club` — src/ui/i18n/en.ts:1254: `'building.fn.club': 'Club'`
- feature entry `building.fn.gasStation` — src/ui/i18n/en.ts:1255: `'building.fn.gasStation': 'Petrol station'`
- feature entry `building.fn.office` — src/ui/i18n/en.ts:1256: `'building.fn.office': 'Offices'`
- feature entry `building.fn.factory` — src/ui/i18n/en.ts:1257: `'building.fn.factory': 'Factory'`
- feature entry `building.fn.warehouse` — src/ui/i18n/en.ts:1258: `'building.fn.warehouse': 'Warehouse'`
- feature entry `building.fn.park` — src/ui/i18n/en.ts:1259: `'building.fn.park': 'Park'`
- feature entry `building.fn.square` — src/ui/i18n/en.ts:1260: `'building.fn.square': 'Square'`
- feature entry `building.fn.playground` — src/ui/i18n/en.ts:1261: `'building.fn.playground': 'Playground'`
- feature entry `building.fn.sportsCourt` — src/ui/i18n/en.ts:1262: `'building.fn.sportsCourt': 'Sports court'`
- feature entry `builder.city.homes` — src/ui/i18n/en.ts:1263: `'builder.city.homes': 'Homes'`
- feature entry `builder.city.public` — src/ui/i18n/en.ts:1264: `'builder.city.public': 'Public services'`
- feature entry `builder.city.commerce` — src/ui/i18n/en.ts:1265: `'builder.city.commerce': 'Shops and services'`
- feature entry `builder.city.work` — src/ui/i18n/en.ts:1266: `'builder.city.work': 'Work'`
- feature entry `builder.city.leisure` — src/ui/i18n/en.ts:1267: `'builder.city.leisure': 'Leisure'`
- feature entry `builder.city.generic` — src/ui/i18n/en.ts:1268: `'builder.city.generic': 'Plain blocks'`
- feature entry `builder.city.lots` — src/ui/i18n/en.ts:1269: `'builder.city.lots': 'Lots'`
- feature entry `builder.lot.balconyMid` — src/ui/i18n/en.ts:1270: `'builder.lot.balconyMid': 'Balconies and timber'`
- feature entry `builder.lot.glassOffice` — src/ui/i18n/en.ts:1271: `'builder.lot.glassOffice': 'Glass office'`
- feature entry `builder.lot.glassBalcony` — src/ui/i18n/en.ts:1272: `'builder.lot.glassBalcony': 'Glass with balconies'`
- feature entry `builder.lot.beigeClassic` — src/ui/i18n/en.ts:1273: `'builder.lot.beigeClassic': 'Classic beige'`
- feature entry `builder.lot.darkGlass` — src/ui/i18n/en.ts:1274: `'builder.lot.darkGlass': 'Dark glass'`
- feature entry `builder.lot.whiteBalcony` — src/ui/i18n/en.ts:1275: `'builder.lot.whiteBalcony': 'White with balconies'`
- feature entry `builder.lot.brickFrame` — src/ui/i18n/en.ts:1276: `'builder.lot.brickFrame': 'Brick and glass'`
- feature entry `builder.lot.roundGlass` — src/ui/i18n/en.ts:1277: `'builder.lot.roundGlass': 'Round glass'`
- feature entry `builder.lot.darkGrid` — src/ui/i18n/en.ts:1278: `'builder.lot.darkGrid': 'Dark grid'`
- feature entry `builder.lot.artDeco` — src/ui/i18n/en.ts:1279: `'builder.lot.artDeco': 'Art deco'`
- feature entry `builder.lot.twistGreen` — src/ui/i18n/en.ts:1280: `'builder.lot.twistGreen': 'Twisted teal'`
- feature entry `builder.lot.hexTerracotta` — src/ui/i18n/en.ts:1281: `'builder.lot.hexTerracotta': 'Hexagonal terracotta'`
- feature entry `builder.lot.stepGarden` — src/ui/i18n/en.ts:1282: `'builder.lot.stepGarden': 'Terrace gardens'`
- feature entry `builder.lot.waveWhite` — src/ui/i18n/en.ts:1283: `'builder.lot.waveWhite': 'Wave balconies'`
- feature entry `builder.lot.cubeBalcony` — src/ui/i18n/en.ts:1284: `'builder.lot.cubeBalcony': 'Cube balconies'`
- feature entry `builder.lot.twinNavy` — src/ui/i18n/en.ts:1285: `'builder.lot.twinNavy': 'Navy twins'`
- feature entry `builder.lot.octCopper` — src/ui/i18n/en.ts:1286: `'builder.lot.octCopper': 'Octagonal copper'`
- feature entry `builder.lot.stackedBlocks` — src/ui/i18n/en.ts:1287: `'builder.lot.stackedBlocks': 'Stacked blocks'`
- feature entry `builder.lot.pinkCorner` — src/ui/i18n/en.ts:1288: `'builder.lot.pinkCorner': 'Pink corner'`
- feature entry `builder.lot.triangleDark` — src/ui/i18n/en.ts:1289: `'builder.lot.triangleDark': 'Dark triangle'`
- feature entry `builder.lot.cityHallBrick` — src/ui/i18n/en.ts:1290: `'builder.lot.cityHallBrick': 'Brick town hall'`
- feature entry `builder.lot.decoSpire` — src/ui/i18n/en.ts:1291: `'builder.lot.decoSpire': 'Deco spire'`
- feature entry `builder.lot.hospitalA` — src/ui/i18n/en.ts:1292: `'builder.lot.hospitalA': 'Hospital'`
- feature entry `builder.lot.loftBrick` — src/ui/i18n/en.ts:1293: `'builder.lot.loftBrick': 'Brick lofts'`
- feature entry `builder.lot.cityHallWhite` — src/ui/i18n/en.ts:1294: `'builder.lot.cityHallWhite': 'White town hall'`
- feature entry `builder.lot.forum` — src/ui/i18n/en.ts:1295: `'builder.lot.forum': 'Courthouse'`
- feature entry `builder.lot.church` — src/ui/i18n/en.ts:1296: `'builder.lot.church': 'Church'`
- feature entry `builder.lot.bank` — src/ui/i18n/en.ts:1297: `'builder.lot.bank': 'Bank'`
- feature entry `builder.lot.school` — src/ui/i18n/en.ts:1298: `'builder.lot.school': 'School'`
- feature entry `builder.lot.fireStation` — src/ui/i18n/en.ts:1299: `'builder.lot.fireStation': 'Fire station'`
- feature entry `builder.lot.police` — src/ui/i18n/en.ts:1300: `'builder.lot.police': 'Police station'`
- feature entry `builder.lot.hospitalB` — src/ui/i18n/en.ts:1301: `'builder.lot.hospitalB': 'Hospital with a wing'`
- feature entry `builder.lot.cemetery` — src/ui/i18n/en.ts:1302: `'builder.lot.cemetery': 'Cemetery'`
- feature entry `builder.lot.beauxArts` — src/ui/i18n/en.ts:1303: `'builder.lot.beauxArts': 'Beaux-Arts hotel'`
- feature entry `builder.lot.decoTower` — src/ui/i18n/en.ts:1304: `'builder.lot.decoTower': 'Art Deco tower'`
- feature entry `builder.lot.modernOffice` — src/ui/i18n/en.ts:1305: `'builder.lot.modernOffice': 'Modernist office'`
- feature entry `builder.lot.residentialTerrace` — src/ui/i18n/en.ts:1306: `'builder.lot.residentialTerrace': 'Terraced flats'`
- feature entry `builder.lot.glassSpire` — src/ui/i18n/en.ts:1307: `'builder.lot.glassSpire': 'Blue glass tower'`
- feature entry `builder.city.all` — src/ui/i18n/en.ts:1308: `'builder.city.all': 'All'`
- feature entry `v2.build.shape` — src/ui/i18n/en.ts:1387: `'v2.build.shape': 'Shape'`
- feature entry `v2.build.selectToEdit` — src/ui/i18n/en.ts:1388: `'v2.build.selectToEdit': 'Click a building to edit it'`
- feature entry `v2.build.floor` — src/ui/i18n/en.ts:1389: `'v2.build.floor': 'Floor'`
- feature entry `v2.build.plan` — src/ui/i18n/en.ts:1390: `'v2.build.plan': 'Plan'`
- feature entry `v2.build.material` — src/ui/i18n/en.ts:1391: `'v2.build.material': 'Material'`
- feature entry `v2.build.colour` — src/ui/i18n/en.ts:1392: `'v2.build.colour': 'Colour'`
- feature entry `builder.city.mine` — src/ui/i18n/en.ts:1393: `'builder.city.mine': 'Saved by me'`
- feature entry `builder.advanced.off` — src/ui/i18n/en.ts:1396: `'builder.advanced.off': 'Show advanced modelling'`
- feature entry `builder.advanced.on` — src/ui/i18n/en.ts:1397: `'builder.advanced.on': 'Hide advanced modelling'`
- feature entry `builder.field.function` — src/ui/i18n/en.ts:1398: `'builder.field.function': 'Function'`
- feature entry `building.fn.none` — src/ui/i18n/en.ts:1399: `'building.fn.none': 'None'`
- feature entry `builder.tool.clock` — src/ui/i18n/en.ts:1400: `'builder.tool.clock': 'Clock'`
- shared entry `hint.builder.clock` — src/ui/i18n/en.ts:1401: `'hint.builder.clock': 'Click a bay of the facade to hang a clock'`
- feature entry `builder.category.interior` — src/ui/i18n/en.ts:1402: `'builder.category.interior': 'Inside'`
- feature entry `builder.section.interiorView` — src/ui/i18n/en.ts:1403: `'builder.section.interiorView': 'Look inside'`
- feature entry `builder.section.cores` — src/ui/i18n/en.ts:1404: `'builder.section.cores': 'Circulation: stairs and lifts'`
- feature entry `builder.tool.interiorView` — src/ui/i18n/en.ts:1405: `'builder.tool.interiorView': 'Cut open'`
- feature entry `builder.tool.floorDown` — src/ui/i18n/en.ts:1406: `'builder.tool.floorDown': 'Floor below'`
- feature entry `builder.tool.floorUp` — src/ui/i18n/en.ts:1407: `'builder.tool.floorUp': 'Floor above'`
- feature entry `builder.tool.coreLift` — src/ui/i18n/en.ts:1408: `'builder.tool.coreLift': 'Lift'`
- feature entry `builder.tool.coreStair` — src/ui/i18n/en.ts:1409: `'builder.tool.coreStair': 'Stair'`
- feature entry `builder.tool.coreBoth` — src/ui/i18n/en.ts:1410: `'builder.tool.coreBoth': 'Stair + lift'`
- feature entry `builder.tool.furn_sofa` — src/ui/i18n/en.ts:1411: `'builder.tool.furn_sofa': 'Sofa'`
- feature entry `hint.builder.furn_sofa` — src/ui/i18n/en.ts:1412: `'hint.builder.furn_sofa': 'Click the floor to put down: sofa (R turns it)'`
- feature entry `builder.tool.furn_armchair` — src/ui/i18n/en.ts:1413: `'builder.tool.furn_armchair': 'Armchair'`
- feature entry `hint.builder.furn_armchair` — src/ui/i18n/en.ts:1414: `'hint.builder.furn_armchair': 'Click the floor to put down: armchair (R turns it)'`
- feature entry `builder.tool.furn_table` — src/ui/i18n/en.ts:1415: `'builder.tool.furn_table': 'Table'`
- feature entry `hint.builder.furn_table` — src/ui/i18n/en.ts:1416: `'hint.builder.furn_table': 'Click the floor to put down: table (R turns it)'`
- feature entry `builder.tool.furn_chair` — src/ui/i18n/en.ts:1417: `'builder.tool.furn_chair': 'Chair'`
- feature entry `hint.builder.furn_chair` — src/ui/i18n/en.ts:1418: `'hint.builder.furn_chair': 'Click the floor to put down: chair (R turns it)'`
- feature entry `builder.tool.furn_bed` — src/ui/i18n/en.ts:1419: `'builder.tool.furn_bed': 'Double bed'`
- feature entry `hint.builder.furn_bed` — src/ui/i18n/en.ts:1420: `'hint.builder.furn_bed': 'Click the floor to put down: double bed (R turns it)'`
- feature entry `builder.tool.furn_singleBed` — src/ui/i18n/en.ts:1421: `'builder.tool.furn_singleBed': 'Single bed'`
- feature entry `hint.builder.furn_singleBed` — src/ui/i18n/en.ts:1422: `'hint.builder.furn_singleBed': 'Click the floor to put down: single bed (R turns it)'`
- feature entry `builder.tool.furn_wardrobe` — src/ui/i18n/en.ts:1423: `'builder.tool.furn_wardrobe': 'Wardrobe'`
- feature entry `hint.builder.furn_wardrobe` — src/ui/i18n/en.ts:1424: `'hint.builder.furn_wardrobe': 'Click the floor to put down: wardrobe (R turns it)'`
- feature entry `builder.tool.furn_tv` — src/ui/i18n/en.ts:1425: `'builder.tool.furn_tv': 'TV'`
- feature entry `hint.builder.furn_tv` — src/ui/i18n/en.ts:1426: `'hint.builder.furn_tv': 'Click the floor to put down: tv (R turns it)'`
- feature entry `builder.tool.furn_desk` — src/ui/i18n/en.ts:1427: `'builder.tool.furn_desk': 'Desk'`
- feature entry `hint.builder.furn_desk` — src/ui/i18n/en.ts:1428: `'hint.builder.furn_desk': 'Click the floor to put down: desk (R turns it)'`
- feature entry `builder.tool.furn_officeChair` — src/ui/i18n/en.ts:1429: `'builder.tool.furn_officeChair': 'Office chair'`
- feature entry `hint.builder.furn_officeChair` — src/ui/i18n/en.ts:1430: `'hint.builder.furn_officeChair': 'Click the floor to put down: office chair (R turns it)'`
- feature entry `builder.tool.furn_bookshelf` — src/ui/i18n/en.ts:1431: `'builder.tool.furn_bookshelf': 'Bookcase'`
- feature entry `hint.builder.furn_bookshelf` — src/ui/i18n/en.ts:1432: `'hint.builder.furn_bookshelf': 'Click the floor to put down: bookcase (R turns it)'`
- feature entry `builder.tool.furn_shelf` — src/ui/i18n/en.ts:1433: `'builder.tool.furn_shelf': 'Shelf'`
- feature entry `hint.builder.furn_shelf` — src/ui/i18n/en.ts:1434: `'hint.builder.furn_shelf': 'Click the floor to put down: shelf (R turns it)'`
- feature entry `builder.tool.furn_counter` — src/ui/i18n/en.ts:1435: `'builder.tool.furn_counter': 'Counter'`
- feature entry `hint.builder.furn_counter` — src/ui/i18n/en.ts:1436: `'hint.builder.furn_counter': 'Click the floor to put down: counter (R turns it)'`
- feature entry `builder.tool.furn_fridge` — src/ui/i18n/en.ts:1437: `'builder.tool.furn_fridge': 'Fridge'`
- feature entry `hint.builder.furn_fridge` — src/ui/i18n/en.ts:1438: `'hint.builder.furn_fridge': 'Click the floor to put down: fridge (R turns it)'`
- feature entry `builder.tool.furn_stove` — src/ui/i18n/en.ts:1439: `'builder.tool.furn_stove': 'Cooker'`
- feature entry `hint.builder.furn_stove` — src/ui/i18n/en.ts:1440: `'hint.builder.furn_stove': 'Click the floor to put down: cooker (R turns it)'`
- feature entry `builder.tool.furn_sink` — src/ui/i18n/en.ts:1441: `'builder.tool.furn_sink': 'Sink'`
- feature entry `hint.builder.furn_sink` — src/ui/i18n/en.ts:1442: `'hint.builder.furn_sink': 'Click the floor to put down: sink (R turns it)'`
- feature entry `builder.tool.furn_bath` — src/ui/i18n/en.ts:1443: `'builder.tool.furn_bath': 'Bath'`
- feature entry `hint.builder.furn_bath` — src/ui/i18n/en.ts:1444: `'hint.builder.furn_bath': 'Click the floor to put down: bath (R turns it)'`
- feature entry `builder.tool.furn_toilet` — src/ui/i18n/en.ts:1445: `'builder.tool.furn_toilet': 'Toilet'`
- feature entry `hint.builder.furn_toilet` — src/ui/i18n/en.ts:1446: `'hint.builder.furn_toilet': 'Click the floor to put down: toilet (R turns it)'`
- feature entry `builder.tool.furn_plant` — src/ui/i18n/en.ts:1447: `'builder.tool.furn_plant': 'Plant'`
- feature entry `hint.builder.furn_plant` — src/ui/i18n/en.ts:1448: `'hint.builder.furn_plant': 'Click the floor to put down: plant (R turns it)'`
- feature entry `builder.tool.furn_ceilingLamp` — src/ui/i18n/en.ts:1449: `'builder.tool.furn_ceilingLamp': 'Ceiling light'`
- feature entry `hint.builder.furn_ceilingLamp` — src/ui/i18n/en.ts:1450: `'hint.builder.furn_ceilingLamp': 'Click the floor to put down: ceiling light (R turns it)'`
- feature entry `builder.tool.furn_floorLamp` — src/ui/i18n/en.ts:1451: `'builder.tool.furn_floorLamp': 'Floor lamp'`
- feature entry `hint.builder.furn_floorLamp` — src/ui/i18n/en.ts:1452: `'hint.builder.furn_floorLamp': 'Click the floor to put down: floor lamp (R turns it)'`
- feature entry `builder.tool.furn_tableLamp` — src/ui/i18n/en.ts:1453: `'builder.tool.furn_tableLamp': 'Table lamp'`
- feature entry `hint.builder.furn_tableLamp` — src/ui/i18n/en.ts:1454: `'hint.builder.furn_tableLamp': 'Click the floor to put down: table lamp (R turns it)'`
- feature entry `builder.tool.furnMove` — src/ui/i18n/en.ts:1455: `'builder.tool.furnMove': 'Move'`
- feature entry `hint.builder.furnMove` — src/ui/i18n/en.ts:1456: `'hint.builder.furnMove': 'Click a piece to pick it up, then where it goes (R turns it)'`
- feature entry `builder.tool.furnTurn` — src/ui/i18n/en.ts:1457: `'builder.tool.furnTurn': 'Turn'`
- feature entry `hint.builder.furnTurn` — src/ui/i18n/en.ts:1458: `'hint.builder.furnTurn': 'Turns the selected piece of furniture'`
- feature entry `builder.tool.furnRemove` — src/ui/i18n/en.ts:1459: `'builder.tool.furnRemove': 'Take away'`
- feature entry `hint.builder.furnRemove` — src/ui/i18n/en.ts:1460: `'hint.builder.furnRemove': 'Click a piece or a light to take it away'`
- feature entry `builder.section.furnishing` — src/ui/i18n/en.ts:1461: `'builder.section.furnishing': 'Arrange'`
- feature entry `builder.tool.coreRemove` — src/ui/i18n/en.ts:1462: `'builder.tool.coreRemove': 'Remove'`
- feature entry `hint.builder.interiorView` — src/ui/i18n/en.ts:1463: `'hint.builder.interiorView': 'The building is drawn cut open at the chosen floor: see the lobby, the stairs, the lifts'`
- feature entry `hint.builder.floorDown` — src/ui/i18n/en.ts:1464: `'hint.builder.floorDown': 'Shows the floor below'`
- feature entry `hint.builder.floorUp` — src/ui/i18n/en.ts:1465: `'hint.builder.floorUp': 'Shows the floor above'`
- feature entry `hint.builder.coreLift` — src/ui/i18n/en.ts:1466: `'hint.builder.coreLift': 'Click the floor: a lift through every floor'`
- feature entry `hint.builder.coreStair` — src/ui/i18n/en.ts:1467: `'hint.builder.coreStair': 'Click the floor: a stair through every floor'`
- feature entry `hint.builder.coreBoth` — src/ui/i18n/en.ts:1468: `'hint.builder.coreBoth': 'Click the floor: a stair and a lift together'`
- feature entry `hint.builder.coreRemove` — src/ui/i18n/en.ts:1469: `'hint.builder.coreRemove': 'Click a stair or a lift to take it out'`
- feature entry `builder.section.primitives` — src/ui/i18n/en.ts:1470: `'builder.section.primitives': 'Basic shapes: click, then drop on the building'`
- feature entry `builder.tool.primBox` — src/ui/i18n/en.ts:1471: `'builder.tool.primBox': 'Box'`
- feature entry `builder.tool.primCylinder` — src/ui/i18n/en.ts:1472: `'builder.tool.primCylinder': 'Cylinder'`
- feature entry `builder.tool.primOctagonal` — src/ui/i18n/en.ts:1473: `'builder.tool.primOctagonal': 'Octagon'`
- feature entry `builder.tool.primPrism` — src/ui/i18n/en.ts:1474: `'builder.tool.primPrism': 'Prism'`
- feature entry `builder.tool.primWedge` — src/ui/i18n/en.ts:1475: `'builder.tool.primWedge': 'Wedge'`
- feature entry `builder.tool.primPyramid` — src/ui/i18n/en.ts:1476: `'builder.tool.primPyramid': 'Pyramid'`
- feature entry `builder.tool.primCone` — src/ui/i18n/en.ts:1477: `'builder.tool.primCone': 'Cone'`
- feature entry `builder.tool.primCross` — src/ui/i18n/en.ts:1478: `'builder.tool.primCross': 'Cross'`
- feature entry `hint.builder.primBox` — src/ui/i18n/en.ts:1479: `'hint.builder.primBox': 'Click a roof, a wall or the ground to put the shape there · Esc lets go'`
- feature entry `hint.builder.primCylinder` — src/ui/i18n/en.ts:1480: `'hint.builder.primCylinder': 'Click a roof, a wall or the ground to put the shape there · Esc lets go'`
- feature entry `hint.builder.primOctagonal` — src/ui/i18n/en.ts:1481: `'hint.builder.primOctagonal': 'Click a roof, a wall or the ground to put the shape there · Esc lets go'`
- feature entry `hint.builder.primPrism` — src/ui/i18n/en.ts:1482: `'hint.builder.primPrism': 'Click a roof, a wall or the ground to put the shape there · Esc lets go'`
- feature entry `hint.builder.primWedge` — src/ui/i18n/en.ts:1483: `'hint.builder.primWedge': 'Click a roof, a wall or the ground to put the shape there · Esc lets go'`
- feature entry `hint.builder.primPyramid` — src/ui/i18n/en.ts:1484: `'hint.builder.primPyramid': 'Click a roof, a wall or the ground to put the shape there · Esc lets go'`
- feature entry `hint.builder.primCone` — src/ui/i18n/en.ts:1485: `'hint.builder.primCone': 'Click a roof, a wall or the ground to put the shape there · Esc lets go'`
- feature entry `hint.builder.primCross` — src/ui/i18n/en.ts:1486: `'hint.builder.primCross': 'Click a roof, a wall or the ground to put the shape there · Esc lets go'`
- feature entry `builder.section.modelling` — src/ui/i18n/en.ts:1487: `'builder.section.modelling': 'Modelling'`
- feature entry `builder.tool.extrudeOut` — src/ui/i18n/en.ts:1488: `'builder.tool.extrudeOut': 'Extrude +1 m'`
- feature entry `builder.tool.extrudeIn` — src/ui/i18n/en.ts:1489: `'builder.tool.extrudeIn': 'Extrude −1 m'`
- feature entry `builder.tool.extrudeBlock` — src/ui/i18n/en.ts:1490: `'builder.tool.extrudeBlock': 'Extrude block'`
- feature entry `builder.tool.insetFace` — src/ui/i18n/en.ts:1491: `'builder.tool.insetFace': 'Inset'`
- feature entry `builder.tool.offsetOut` — src/ui/i18n/en.ts:1492: `'builder.tool.offsetOut': 'Offset +1 m'`
- feature entry `builder.tool.offsetIn` — src/ui/i18n/en.ts:1493: `'builder.tool.offsetIn': 'Offset −1 m'`
- feature entry `builder.tool.bevelAll` — src/ui/i18n/en.ts:1494: `'builder.tool.bevelAll': 'Bevel corners'`
- feature entry `builder.tool.pointMode` — src/ui/i18n/en.ts:1495: `'builder.tool.pointMode': 'Points'`
- feature entry `builder.tool.bevelCorner` — src/ui/i18n/en.ts:1496: `'builder.tool.bevelCorner': 'Bevel point'`
- feature entry `hint.builder.extrudeOut` — src/ui/i18n/en.ts:1497: `'hint.builder.extrudeOut': 'Pushes the picked face 1 m out'`
- feature entry `hint.builder.extrudeIn` — src/ui/i18n/en.ts:1498: `'hint.builder.extrudeIn': 'Pulls the picked face 1 m in'`
- feature entry `hint.builder.extrudeBlock` — src/ui/i18n/en.ts:1499: `'hint.builder.extrudeBlock': 'The picked face makes a new block against it'`
- feature entry `hint.builder.insetFace` — src/ui/i18n/en.ts:1500: `'hint.builder.insetFace': 'Sets the picked face back inside a frame'`
- feature entry `hint.builder.offsetOut` — src/ui/i18n/en.ts:1501: `'hint.builder.offsetOut': "The block's plan grows 1 m on every side"`
- feature entry `hint.builder.offsetIn` — src/ui/i18n/en.ts:1502: `'hint.builder.offsetIn': "The block's plan shrinks 1 m on every side"`
- feature entry `hint.builder.bevelAll` — src/ui/i18n/en.ts:1503: `'hint.builder.bevelAll': 'Bevels every corner of the block'`
- feature entry `hint.builder.pointMode` — src/ui/i18n/en.ts:1504: `'hint.builder.pointMode': "Shows the block's points: drag them to shape it"`
- feature entry `hint.builder.bevelCorner` — src/ui/i18n/en.ts:1505: `'hint.builder.bevelCorner': 'Bevels the picked point'`
- feature entry `builder.pickFace` — src/ui/i18n/en.ts:1506: `'builder.pickFace': 'Click the block again to pick a face'`
- feature entry `builder.pickCorner` — src/ui/i18n/en.ts:1507: `'builder.pickCorner': 'Turn on Points and click a point of the block'`
- feature entry `builder.section.boolean` — src/ui/i18n/en.ts:1508: `'builder.section.boolean': 'Boolean operation'`
- feature entry `builder.section.block` — src/ui/i18n/en.ts:1509: `'builder.section.block': 'Block'`
- feature entry `builder.section.drawWhere` — src/ui/i18n/en.ts:1510: `'builder.section.drawWhere': 'The drawn shape makes'`
- feature entry `builder.section.drawWhere.short` — src/ui/i18n/en.ts:1511: `'builder.section.drawWhere.short': 'Result'`
- feature entry `builder.section.drawShape` — src/ui/i18n/en.ts:1512: `'builder.section.drawShape': 'Shape'`
- feature entry `builder.drawAction.new` — src/ui/i18n/en.ts:1513: `'builder.drawAction.new': 'New building'`
- feature entry `builder.drawAction.ground` — src/ui/i18n/en.ts:1514: `'builder.drawAction.ground': 'Block beside'`
- feature entry `builder.drawAction.top` — src/ui/i18n/en.ts:1515: `'builder.drawAction.top': 'Block on top'`
- feature entry `builder.drawAction.cut` — src/ui/i18n/en.ts:1516: `'builder.drawAction.cut': 'Cut'`
- feature entry `builder.tool.blockSolid` — src/ui/i18n/en.ts:1517: `'builder.tool.blockSolid': 'Union'`
- feature entry `builder.tool.blockVoid` — src/ui/i18n/en.ts:1518: `'builder.tool.blockVoid': 'Subtract'`
- feature entry `builder.tool.blockIntersect` — src/ui/i18n/en.ts:1519: `'builder.tool.blockIntersect': 'Intersect'`
- feature entry `builder.tool.blockXor` — src/ui/i18n/en.ts:1520: `'builder.tool.blockXor': 'Exclude'`
- feature entry `builder.tool.copyBlock` — src/ui/i18n/en.ts:1521: `'builder.tool.copyBlock': 'Copy block'`
- feature entry `builder.tool.detachBlock` — src/ui/i18n/en.ts:1522: `'builder.tool.detachBlock': 'Detach'`
- feature entry `builder.tool.centerBlock` — src/ui/i18n/en.ts:1523: `'builder.tool.centerBlock': 'Centre'`
- feature entry `builder.tool.turnBlockLeft` — src/ui/i18n/en.ts:1524: `'builder.tool.turnBlockLeft': 'Turn ⟲ 15°'`
- feature entry `builder.tool.turnBlockRight` — src/ui/i18n/en.ts:1525: `'builder.tool.turnBlockRight': 'Turn ⟳ 15°'`
- feature entry `hint.builder.blockSolid` — src/ui/i18n/en.ts:1526: `'hint.builder.blockSolid': 'The block adds its space to the building'`
- feature entry `hint.builder.blockVoid` — src/ui/i18n/en.ts:1527: `'hint.builder.blockVoid': 'The block takes its space out of the others, deleting nothing'`
- feature entry `hint.builder.blockIntersect` — src/ui/i18n/en.ts:1528: `'hint.builder.blockIntersect': 'Of the other blocks, only what is inside this one stays'`
- feature entry `hint.builder.blockXor` — src/ui/i18n/en.ts:1529: `'hint.builder.blockXor': 'The block adds where it is alone and removes where it meets another'`
- feature entry `hint.builder.copyBlock` — src/ui/i18n/en.ts:1530: `'hint.builder.copyBlock': 'A copy of the block beside it'`
- feature entry `hint.builder.detachBlock` — src/ui/i18n/en.ts:1531: `'hint.builder.detachBlock': 'The block becomes a separate building'`
- feature entry `hint.builder.centerBlock` — src/ui/i18n/en.ts:1532: `'hint.builder.centerBlock': 'The block is centred on the one below'`
- feature entry `hint.builder.turnBlockLeft` — src/ui/i18n/en.ts:1533: `'hint.builder.turnBlockLeft': 'Turns the block about its centre'`
- feature entry `hint.builder.turnBlockRight` — src/ui/i18n/en.ts:1534: `'hint.builder.turnBlockRight': 'Turns the block about its centre'`
- feature entry `builder.field.offsetX` — src/ui/i18n/en.ts:1535: `'builder.field.offsetX': 'Position X'`
- feature entry `builder.field.baseLevel` — src/ui/i18n/en.ts:1537: `'builder.field.baseLevel': 'Starts on floor'`
- feature entry `builder.field.lift` — src/ui/i18n/en.ts:1538: `'builder.field.lift': 'Block floor offset'`
- feature entry `builder.field.blockMode` — src/ui/i18n/en.ts:1539: `'builder.field.blockMode': 'Operation'`
- feature entry `builder.blockMode.solid` — src/ui/i18n/en.ts:1540: `'builder.blockMode.solid': 'Union'`
- feature entry `builder.blockMode.void` — src/ui/i18n/en.ts:1541: `'builder.blockMode.void': 'Subtract'`
- feature entry `builder.blockMode.intersect` — src/ui/i18n/en.ts:1542: `'builder.blockMode.intersect': 'Intersect'`
- feature entry `builder.blockMode.xor` — src/ui/i18n/en.ts:1543: `'builder.blockMode.xor': 'Exclude'`
- feature entry `builder.tool.shapeCross` — src/ui/i18n/en.ts:1544: `'builder.tool.shapeCross': 'Cross'`
- feature entry `builder.tool.shapeStepped` — src/ui/i18n/en.ts:1545: `'builder.tool.shapeStepped': 'Stepped cross'`
- feature entry `builder.tool.tierCross` — src/ui/i18n/en.ts:1546: `'builder.tool.tierCross': 'Cross'`
- feature entry `builder.tool.tierStepped` — src/ui/i18n/en.ts:1547: `'builder.tool.tierStepped': 'Stepped cross'`
- feature entry `hint.builder.shapeCross` — src/ui/i18n/en.ts:1548: `'hint.builder.shapeCross': 'Drag on the ground: a cross-shaped plan'`
- feature entry `hint.builder.shapeStepped` — src/ui/i18n/en.ts:1549: `'hint.builder.shapeStepped': 'Drag on the ground: a cross with stepped corners'`
- feature entry `hint.builder.tierCross` — src/ui/i18n/en.ts:1550: `'hint.builder.tierCross': 'The selected block becomes a cross'`
- feature entry `hint.builder.tierStepped` — src/ui/i18n/en.ts:1551: `'hint.builder.tierStepped': 'The selected block becomes a cross with stepped corners'`
- feature entry `builder.section.blockShape` — src/ui/i18n/en.ts:1552: `'builder.section.blockShape': 'Block shape'`
- feature entry `builder.tool.tierRect` — src/ui/i18n/en.ts:1553: `'builder.tool.tierRect': 'Rectangle'`
- feature entry `builder.tool.tierChamfered` — src/ui/i18n/en.ts:1554: `'builder.tool.tierChamfered': 'Chamfered'`
- feature entry `builder.tool.tierOctagon` — src/ui/i18n/en.ts:1555: `'builder.tool.tierOctagon': 'Octagon'`
- feature entry `builder.tool.tierHexagon` — src/ui/i18n/en.ts:1556: `'builder.tool.tierHexagon': 'Hexagon'`
- feature entry `builder.tool.tierCircle` — src/ui/i18n/en.ts:1557: `'builder.tool.tierCircle': 'Cylinder'`
- feature entry `builder.tool.tierL` — src/ui/i18n/en.ts:1558: `'builder.tool.tierL': 'L-shaped'`
- feature entry `builder.tool.tierU` — src/ui/i18n/en.ts:1559: `'builder.tool.tierU': 'U-shaped'`
- feature entry `hint.builder.tierRect` — src/ui/i18n/en.ts:1560: `'hint.builder.tierRect': 'The selected block becomes a rectangle'`
- feature entry `hint.builder.tierChamfered` — src/ui/i18n/en.ts:1561: `'hint.builder.tierChamfered': 'The selected block gets chamfered corners'`
- feature entry `hint.builder.tierOctagon` — src/ui/i18n/en.ts:1562: `'hint.builder.tierOctagon': 'The selected block becomes an octagon'`
- feature entry `hint.builder.tierHexagon` — src/ui/i18n/en.ts:1563: `'hint.builder.tierHexagon': 'The selected block becomes a hexagon'`
- feature entry `hint.builder.tierCircle` — src/ui/i18n/en.ts:1564: `'hint.builder.tierCircle': 'The selected block becomes a cylinder'`
- feature entry `hint.builder.tierL` — src/ui/i18n/en.ts:1565: `'hint.builder.tierL': 'The selected block becomes an L'`
- feature entry `hint.builder.tierU` — src/ui/i18n/en.ts:1566: `'hint.builder.tierU': 'The selected block becomes a U'`
- shared entry `builder.category.models` — src/ui/i18n/en.ts:1567: `'builder.category.models': 'Models'`
- feature entry `builder.category.facade` — src/ui/i18n/en.ts:1568: `'builder.category.facade': 'Facade'`
- feature entry `builder.category.parts` — src/ui/i18n/en.ts:1569: `'builder.category.parts': 'Parts'`
- feature entry `builder.category.paint` — src/ui/i18n/en.ts:1570: `'builder.category.paint': 'Colours'`
- feature entry `builder.section.floors` — src/ui/i18n/en.ts:1571: `'builder.section.floors': 'Floors'`
- feature entry `builder.section.reference` — src/ui/i18n/en.ts:1572: `'builder.section.reference': '3D reference'`
- feature entry `builder.tool.refLoad` — src/ui/i18n/en.ts:1573: `'builder.tool.refLoad': 'Load model'`
- feature entry `hint.builder.refLoad` — src/ui/i18n/en.ts:1574: `'hint.builder.refLoad': 'Pick a 3D model (.glb or .gltf) of a real building: it appears translucent over the selected building, to build over it'`
- feature entry `builder.tool.refAlign` — src/ui/i18n/en.ts:1575: `'builder.tool.refAlign': 'Align to building'`
- feature entry `hint.builder.refAlign` — src/ui/i18n/en.ts:1576: `'hint.builder.refAlign': 'Puts the reference model on the selected building, its foot on the floor of that building'`
- feature entry `builder.tool.refTurn` — src/ui/i18n/en.ts:1577: `'builder.tool.refTurn': 'Turn 90°'`
- feature entry `hint.builder.refTurn` — src/ui/i18n/en.ts:1578: `'hint.builder.refTurn': 'Turns the reference model a quarter turn'`
- feature entry `builder.tool.refFlip` — src/ui/i18n/en.ts:1579: `'builder.tool.refFlip': 'Mirror'`
- feature entry `hint.builder.refFlip` — src/ui/i18n/en.ts:1580: `'hint.builder.refFlip': 'Mirrors the reference model front to back'`
- feature entry `builder.tool.refFainter` — src/ui/i18n/en.ts:1581: `'builder.tool.refFainter': 'More transparent'`
- feature entry `hint.builder.refFainter` — src/ui/i18n/en.ts:1582: `'hint.builder.refFainter': 'Makes the reference model more transparent'`
- feature entry `builder.tool.refStronger` — src/ui/i18n/en.ts:1583: `'builder.tool.refStronger': 'Less transparent'`
- feature entry `hint.builder.refStronger` — src/ui/i18n/en.ts:1584: `'hint.builder.refStronger': 'Makes the reference model more solid'`
- feature entry `builder.tool.refToggle` — src/ui/i18n/en.ts:1585: `'builder.tool.refToggle': 'Show / hide'`
- feature entry `hint.builder.refToggle` — src/ui/i18n/en.ts:1586: `'hint.builder.refToggle': 'Shows or hides the reference model'`
- feature entry `builder.tool.refRemove` — src/ui/i18n/en.ts:1587: `'builder.tool.refRemove': 'Remove reference'`
- feature entry `hint.builder.refRemove` — src/ui/i18n/en.ts:1588: `'hint.builder.refRemove': 'Takes the reference model out of the scene'`
- feature entry `hint.reference.loaded` — src/ui/i18n/en.ts:1589: `'hint.reference.loaded': 'Reference loaded: {name} — {w} × {d} m, {h} m tall'`
- feature entry `hint.reference.failed` — src/ui/i18n/en.ts:1590: `'hint.reference.failed': 'That model could not be opened (use .glb or .gltf)'`
- feature entry `builder.tool.refBuild` — src/ui/i18n/en.ts:1591: `'builder.tool.refBuild': 'Build from reference'`
- feature entry `hint.builder.refBuild` — src/ui/i18n/en.ts:1592: `'hint.builder.refBuild': 'Reads the reference model - blocks, plan, colours, windows, stone base and lantern - and builds it of the game\'s own parts, all edita`
- feature entry `hint.reference.built` — src/ui/i18n/en.ts:1593: `'hint.reference.built': 'Building built from the reference: {volumes} blocks'`
- feature entry `hint.reference.empty` — src/ui/i18n/en.ts:1594: `'hint.reference.empty': 'No blocks to build were found in that model'`
- feature entry `hint.reference.none` — src/ui/i18n/en.ts:1595: `'hint.reference.none': 'Load a reference model first'`
- feature entry `builder.section.volumes` — src/ui/i18n/en.ts:1596: `'builder.section.volumes': 'Blocks'`
- feature entry `builder.section.plan` — src/ui/i18n/en.ts:1597: `'builder.section.plan': 'Plan corners'`
- feature entry `builder.section.pattern` — src/ui/i18n/en.ts:1598: `'builder.section.pattern': 'Facade pattern'`
- feature entry `builder.section.windows` — src/ui/i18n/en.ts:1599: `'builder.section.windows': 'Windows'`
- feature entry `builder.section.doors` — src/ui/i18n/en.ts:1600: `'builder.section.doors': 'Doors'`
- feature entry `builder.section.bays` — src/ui/i18n/en.ts:1601: `'builder.section.bays': 'Bays'`
- feature entry `builder.section.scope` — src/ui/i18n/en.ts:1602: `'builder.section.scope': 'Apply to'`
- feature entry `builder.section.relief` — src/ui/i18n/en.ts:1603: `'builder.section.relief': 'Bay relief'`
- feature entry `builder.section.structure` — src/ui/i18n/en.ts:1604: `'builder.section.structure': 'Structure'`
- feature entry `builder.section.runs` — src/ui/i18n/en.ts:1605: `'builder.section.runs': 'Runs'`
- feature entry `builder.section.greenery` — src/ui/i18n/en.ts:1606: `'builder.section.greenery': 'Greenery'`
- feature entry `builder.section.furniture` — src/ui/i18n/en.ts:1607: `'builder.section.furniture': 'Furniture'`
- feature entry `builder.section.roofGear` — src/ui/i18n/en.ts:1608: `'builder.section.roofGear': 'On the roof'`
- feature entry `builder.section.roofShape` — src/ui/i18n/en.ts:1609: `'builder.section.roofShape': 'Slope'`
- feature entry `builder.section.styles` — src/ui/i18n/en.ts:1610: `'builder.section.styles': 'Styles'`
- feature entry `builder.needsSelection` — src/ui/i18n/en.ts:1611: `'builder.needsSelection': 'Select a building to use this'`
- feature entry `builder.quick.save` — src/ui/i18n/en.ts:1612: `'builder.quick.save': 'Save as a model'`
- feature entry `builder.category.roof` — src/ui/i18n/en.ts:1617: `'builder.category.roof': 'Roof'`
- feature entry `builder.tool.rect` — src/ui/i18n/en.ts:1621: `'builder.tool.rect': 'Rectangle'`
- feature entry `builder.tool.shapeL` — src/ui/i18n/en.ts:1622: `'builder.tool.shapeL': 'L'`
- feature entry `builder.tool.shapeU` — src/ui/i18n/en.ts:1623: `'builder.tool.shapeU': 'U'`
- feature entry `builder.tool.circle` — src/ui/i18n/en.ts:1624: `'builder.tool.circle': 'Circle'`
- feature entry `builder.tool.hexagon` — src/ui/i18n/en.ts:1625: `'builder.tool.hexagon': 'Hexagon'`
- feature entry `builder.tool.octagon` — src/ui/i18n/en.ts:1626: `'builder.tool.octagon': 'Octagon'`
- feature entry `builder.tool.chamfered` — src/ui/i18n/en.ts:1627: `'builder.tool.chamfered': 'Chamfered'`
- feature entry `builder.tool.sketch` — src/ui/i18n/en.ts:1628: `'builder.tool.sketch': 'Free plan'`
- feature entry `builder.tool.storey` — src/ui/i18n/en.ts:1630: `'builder.tool.storey': '+ Floor'`
- feature entry `builder.tool.storeyDown` — src/ui/i18n/en.ts:1631: `'builder.tool.storeyDown': '− Floor'`
- feature entry `builder.tool.wing` — src/ui/i18n/en.ts:1632: `'builder.tool.wing': 'Wing'`
- feature entry `builder.tool.stack` — src/ui/i18n/en.ts:1633: `'builder.tool.stack': 'Stack'`
- feature entry `builder.tool.cut` — src/ui/i18n/en.ts:1634: `'builder.tool.cut': 'Cut out'`
- feature entry `builder.tool.split` — src/ui/i18n/en.ts:1635: `'builder.tool.split': 'Split'`
- feature entry `builder.tool.setback` — src/ui/i18n/en.ts:1636: `'builder.tool.setback': 'Setback'`
- feature entry `builder.tool.vertexAdd` — src/ui/i18n/en.ts:1637: `'builder.tool.vertexAdd': 'Add point'`
- feature entry `builder.tool.vertexRemove` — src/ui/i18n/en.ts:1638: `'builder.tool.vertexRemove': 'Remove point'`
- feature entry `builder.tool.inset` — src/ui/i18n/en.ts:1640: `'builder.tool.inset': 'Inset'`
- feature entry `builder.tool.outset` — src/ui/i18n/en.ts:1641: `'builder.tool.outset': 'Outset'`
- feature entry `builder.tool.flush` — src/ui/i18n/en.ts:1642: `'builder.tool.flush': 'Flush'`
- feature entry `builder.tool.window` — src/ui/i18n/en.ts:1645: `'builder.tool.window': 'Window'`
- feature entry `builder.tool.sashWindow` — src/ui/i18n/en.ts:1646: `'builder.tool.sashWindow': 'Sash window'`
- feature entry `builder.tool.wideWindow` — src/ui/i18n/en.ts:1647: `'builder.tool.wideWindow': 'Wide window'`
- feature entry `builder.tool.frenchWindow` — src/ui/i18n/en.ts:1648: `'builder.tool.frenchWindow': 'French window'`
- feature entry `builder.tool.bayWindow` — src/ui/i18n/en.ts:1649: `'builder.tool.bayWindow': 'Bay window'`
- feature entry `builder.tool.ribbon` — src/ui/i18n/en.ts:1650: `'builder.tool.ribbon': 'Ribbon window'`
- feature entry `builder.tool.balcony` — src/ui/i18n/en.ts:1651: `'builder.tool.balcony': 'Balcony'`
- feature entry `builder.tool.door` — src/ui/i18n/en.ts:1652: `'builder.tool.door': 'Door'`
- feature entry `builder.tool.shopfront` — src/ui/i18n/en.ts:1653: `'builder.tool.shopfront': 'Shopfront'`
- feature entry `builder.tool.loadingDoor` — src/ui/i18n/en.ts:1654: `'builder.tool.loadingDoor': 'Loading door'`
- feature entry `builder.tool.pillarBay` — src/ui/i18n/en.ts:1655: `'builder.tool.pillarBay': 'Arcade'`
- feature entry `builder.tool.wallBay` — src/ui/i18n/en.ts:1656: `'builder.tool.wallBay': 'Wall'`
- feature entry `builder.tool.stair` — src/ui/i18n/en.ts:1657: `'builder.tool.stair': 'Stair'`
- feature entry `builder.tool.ramp` — src/ui/i18n/en.ts:1658: `'builder.tool.ramp': 'Ramp'`
- feature entry `builder.tool.pillar` — src/ui/i18n/en.ts:1659: `'builder.tool.pillar': 'Pillar'`
- feature entry `builder.tool.canopy` — src/ui/i18n/en.ts:1660: `'builder.tool.canopy': 'Canopy'`
- feature entry `builder.tool.wall` — src/ui/i18n/en.ts:1661: `'builder.tool.wall': 'Wall'`
- feature entry `builder.tool.slab` — src/ui/i18n/en.ts:1662: `'builder.tool.slab': 'Slab'`
- feature entry `builder.tool.moveMass` — src/ui/i18n/en.ts:1664: `'builder.tool.moveMass': 'Move block'`
- feature entry `builder.tool.wallRun` — src/ui/i18n/en.ts:1665: `'builder.tool.wallRun': 'Wall run'`
- feature entry `builder.tool.fenceRun` — src/ui/i18n/en.ts:1666: `'builder.tool.fenceRun': 'Fence'`
- feature entry `builder.tool.pavementRun` — src/ui/i18n/en.ts:1667: `'builder.tool.pavementRun': 'Footpath'`
- feature entry `builder.tool.tree` — src/ui/i18n/en.ts:1668: `'builder.tool.tree': 'Tree'`
- feature entry `builder.tool.bench` — src/ui/i18n/en.ts:1669: `'builder.tool.bench': 'Bench'`
- feature entry `builder.tool.planter` — src/ui/i18n/en.ts:1670: `'builder.tool.planter': 'Planter'`
- feature entry `builder.tool.ac` — src/ui/i18n/en.ts:1671: `'builder.tool.ac': 'Air conditioner'`
- feature entry `builder.tool.roofFlat` — src/ui/i18n/en.ts:1672: `'builder.tool.roofFlat': 'Flat'`
- feature entry `builder.tool.roofTerrace` — src/ui/i18n/en.ts:1673: `'builder.tool.roofTerrace': 'Terrace'`
- feature entry `builder.tool.roofGable` — src/ui/i18n/en.ts:1674: `'builder.tool.roofGable': '2 slopes'`
- feature entry `builder.tool.roofHip` — src/ui/i18n/en.ts:1675: `'builder.tool.roofHip': '4 slopes'`
- feature entry `builder.tool.roofShed` — src/ui/i18n/en.ts:1676: `'builder.tool.roofShed': '1 slope'`
- feature entry `builder.tool.roofSawtooth` — src/ui/i18n/en.ts:1677: `'builder.tool.roofSawtooth': 'Sawtooth'`
- feature entry `builder.tool.solar` — src/ui/i18n/en.ts:1679: `'builder.tool.solar': 'Solar panels'`
- feature entry `builder.tool.skylight` — src/ui/i18n/en.ts:1680: `'builder.tool.skylight': 'Skylight'`
- feature entry `builder.tool.vent` — src/ui/i18n/en.ts:1681: `'builder.tool.vent': 'Vent'`
- feature entry `builder.tool.chimney` — src/ui/i18n/en.ts:1682: `'builder.tool.chimney': 'Chimney'`
- feature entry `builder.tool.waterTank` — src/ui/i18n/en.ts:1683: `'builder.tool.waterTank': 'Water tank'`
- feature entry `builder.tool.spire` — src/ui/i18n/en.ts:1684: `'builder.tool.spire': 'Spire'`
- feature entry `builder.tool.lantern` — src/ui/i18n/en.ts:1685: `'builder.tool.lantern': 'Lantern'`
- feature entry `builder.tool.paint` — src/ui/i18n/en.ts:1687: `'builder.tool.paint': 'Paint'`
- feature entry `builder.tool.copyStyle` — src/ui/i18n/en.ts:1690: `'builder.tool.copyStyle': 'Copy style'`
- shared entry `hint.builder.select` — src/ui/i18n/en.ts:1691: `'hint.builder.select': 'Click a building: select · again: its face · drag: move'`
- feature entry `hint.builder.place` — src/ui/i18n/en.ts:1692: `'hint.builder.place': 'Click on the map to place the building · R turns it · Esc puts it down'`
- shared entry `hint.builder.draw.new` — src/ui/i18n/en.ts:1694: `'hint.builder.draw.new': 'Click each corner · Enter closes the plan'`
- shared entry `hint.builder.draw.ground` — src/ui/i18n/en.ts:1695: `'hint.builder.draw.ground': 'Click on the ground: a wing grows against the wall it meets'`
- shared entry `hint.builder.draw.top` — src/ui/i18n/en.ts:1696: `'hint.builder.draw.top': 'Click on the building: a mass stacks on what is under it'`
- shared entry `hint.builder.draw.cut` — src/ui/i18n/en.ts:1697: `'hint.builder.draw.cut': 'Draw a rectangle over a mass to cut it away'`
- shared entry `hint.builder.sketch` — src/ui/i18n/en.ts:1698: `'hint.builder.sketch': 'Click each corner · Enter closes the plan'`
- shared entry `hint.builder.paint` — src/ui/i18n/en.ts:1705: `'hint.builder.paint': 'Click faces to paint them with the brush'`
- shared entry `hint.builder.stair` — src/ui/i18n/en.ts:1707: `'hint.builder.stair': 'Point at a facade and click: the stair runs down from that floor'`
- shared entry `hint.builder.ramp` — src/ui/i18n/en.ts:1708: `'hint.builder.ramp': 'Point at a facade and click: the ramp runs down from that floor'`
- shared entry `hint.builder.pillar` — src/ui/i18n/en.ts:1709: `'hint.builder.pillar': 'Point at a facade and click: the pillar rises between the floors'`
- shared entry `hint.builder.canopy` — src/ui/i18n/en.ts:1710: `'hint.builder.canopy': 'Point at a facade and click'`
- shared entry `hint.builder.wall` — src/ui/i18n/en.ts:1711: `'hint.builder.wall': 'Point at a facade or the ground and click'`
- shared entry `hint.builder.slab` — src/ui/i18n/en.ts:1712: `'hint.builder.slab': 'Point at a facade or the ground and click'`
- feature entry `hint.builder.moveMass` — src/ui/i18n/en.ts:1714: `'hint.builder.moveMass': 'Drag the selected block: it slides in its own plan, and welds where it lands'`
- feature entry `builder.runPlaced` — src/ui/i18n/en.ts:1715: `'builder.runPlaced': 'Run laid along the path'`
- feature entry `builder.welded` — src/ui/i18n/en.ts:1716: `'builder.welded': 'Welded into one building'`
- feature entry `hint.builder.wallRun` — src/ui/i18n/en.ts:1717: `'hint.builder.wallRun': 'Click each corner of the run · Enter lays the wall along it'`
- feature entry `hint.builder.fenceRun` — src/ui/i18n/en.ts:1718: `'hint.builder.fenceRun': 'Click each corner of the run · Enter lays the fence along it'`
- feature entry `hint.builder.pavementRun` — src/ui/i18n/en.ts:1719: `'hint.builder.pavementRun': 'Click each corner · Enter lays the footpath along it'`
- shared entry `hint.builder.tree` — src/ui/i18n/en.ts:1724: `'hint.builder.tree': 'Click the ground to plant a tree'`
- shared entry `hint.builder.bench` — src/ui/i18n/en.ts:1725: `'hint.builder.bench': 'Point at a facade and click: the bench sits beside it'`
- shared entry `hint.builder.planter` — src/ui/i18n/en.ts:1726: `'hint.builder.planter': 'Point at a facade and click'`
- shared entry `hint.builder.ac` — src/ui/i18n/en.ts:1727: `'hint.builder.ac': 'Point at a facade and click: the unit hangs on the wall'`
- shared entry `hint.builder.solar` — src/ui/i18n/en.ts:1728: `'hint.builder.solar': 'Click a roof to place the panels'`
- shared entry `hint.builder.skylight` — src/ui/i18n/en.ts:1729: `'hint.builder.skylight': 'Click a roof to place the skylight'`
- shared entry `hint.builder.vent` — src/ui/i18n/en.ts:1730: `'hint.builder.vent': 'Click a roof to place the vent'`
- shared entry `hint.builder.chimney` — src/ui/i18n/en.ts:1731: `'hint.builder.chimney': 'Click a roof to place the chimney'`
- shared entry `hint.builder.waterTank` — src/ui/i18n/en.ts:1732: `'hint.builder.waterTank': 'Click a roof to place the tank'`
- shared entry `hint.builder.spire` — src/ui/i18n/en.ts:1733: `'hint.builder.spire': 'Click a roof to raise a spire'`
- shared entry `hint.builder.lantern` — src/ui/i18n/en.ts:1734: `'hint.builder.lantern': 'Click a tower top to raise a lantern with its mast and flag'`
- feature entry `hint.builder.storey` — src/ui/i18n/en.ts:1735: `'hint.builder.storey': 'Adds one floor to the selected mass'`
- feature entry `hint.builder.storeyDown` — src/ui/i18n/en.ts:1736: `'hint.builder.storeyDown': 'Removes one floor from the selected mass'`
- feature entry `hint.builder.wing` — src/ui/i18n/en.ts:1737: `'hint.builder.wing': 'Drag a rectangle on the ground: a volume grows against the wall it meets'`
- feature entry `hint.builder.stack` — src/ui/i18n/en.ts:1738: `'hint.builder.stack': 'Drag over the building: a mass stacks on what is under it'`
- feature entry `hint.builder.cut` — src/ui/i18n/en.ts:1739: `'hint.builder.cut': 'Drag a rectangle over a mass to cut it away'`
- feature entry `hint.builder.split` — src/ui/i18n/en.ts:1740: `'hint.builder.split': 'Splits the selected mass in two at the floor above'`
- feature entry `hint.builder.setback` — src/ui/i18n/en.ts:1741: `'hint.builder.setback': 'Stacks an inset mass on the selected one'`
- feature entry `hint.builder.vertexAdd` — src/ui/i18n/en.ts:1742: `'hint.builder.vertexAdd': 'Click a plan edge to insert a point'`
- feature entry `hint.builder.vertexRemove` — src/ui/i18n/en.ts:1743: `'hint.builder.vertexRemove': 'Click a plan corner to remove it'`
- feature entry `hint.builder.inset` — src/ui/i18n/en.ts:1744: `'hint.builder.inset': 'Click a face: its border steps in, framing the middle'`
- feature entry `hint.builder.outset` — src/ui/i18n/en.ts:1745: `'hint.builder.outset': 'Click a face: the region steps out'`
- feature entry `hint.builder.flush` — src/ui/i18n/en.ts:1746: `'hint.builder.flush': 'Click a face: the region returns to the wall'`
- feature entry `hint.builder.copyStyle` — src/ui/i18n/en.ts:1753: `'hint.builder.copyStyle': 'Remembers the material of what is selected, for the brush'`
- feature entry `hint.builder.window` — src/ui/i18n/en.ts:1755: `'hint.builder.window': 'Click facade bays to punch windows'`
- feature entry `hint.builder.sashWindow` — src/ui/i18n/en.ts:1756: `'hint.builder.sashWindow': 'Click facade bays for sash windows'`
- feature entry `hint.builder.wideWindow` — src/ui/i18n/en.ts:1757: `'hint.builder.wideWindow': 'Click facade bays for wide glazing'`
- feature entry `hint.builder.frenchWindow` — src/ui/i18n/en.ts:1758: `'hint.builder.frenchWindow': 'Click facade bays for floor-height French windows'`
- feature entry `hint.builder.bayWindow` — src/ui/i18n/en.ts:1759: `'hint.builder.bayWindow': 'Click facade bays for projecting bay windows'`
- feature entry `hint.builder.ribbon` — src/ui/i18n/en.ts:1760: `'hint.builder.ribbon': 'Click facade bays for a ribbon of glazing'`
- feature entry `hint.builder.balcony` — src/ui/i18n/en.ts:1761: `'hint.builder.balcony': 'Click facade bays for balconies'`
- feature entry `hint.builder.door` — src/ui/i18n/en.ts:1762: `'hint.builder.door': 'Click facade bays for doors'`
- feature entry `hint.builder.shopfront` — src/ui/i18n/en.ts:1763: `'hint.builder.shopfront': 'Click facade bays for shopfronts'`
- feature entry `hint.builder.loadingDoor` — src/ui/i18n/en.ts:1764: `'hint.builder.loadingDoor': 'Click facade bays for loading doors'`
- feature entry `hint.builder.pillarBay` — src/ui/i18n/en.ts:1765: `'hint.builder.pillarBay': 'Click facade bays for an open arcade'`
- feature entry `hint.builder.wallBay` — src/ui/i18n/en.ts:1766: `'hint.builder.wallBay': 'Click facade bays to close them into plain wall'`
- feature entry `hint.builder.roofFlat` — src/ui/i18n/en.ts:1767: `'hint.builder.roofFlat': 'A flat roof over the selected mass'`
- feature entry `hint.builder.roofTerrace` — src/ui/i18n/en.ts:1768: `'hint.builder.roofTerrace': 'A terrace roof: flat, with a parapet'`
- feature entry `hint.builder.roofGable` — src/ui/i18n/en.ts:1769: `'hint.builder.roofGable': 'Two slopes over a ridge'`
- feature entry `hint.builder.roofHip` — src/ui/i18n/en.ts:1770: `'hint.builder.roofHip': 'Four slopes over a ridge'`
- feature entry `hint.builder.roofShed` — src/ui/i18n/en.ts:1771: `'hint.builder.roofShed': 'One slope, falling towards the front'`
- feature entry `hint.builder.roofSawtooth` — src/ui/i18n/en.ts:1772: `'hint.builder.roofSawtooth': 'Sawtooth: north-light workshops'`
- feature entry `hint.builder.rect` — src/ui/i18n/en.ts:1773: `'hint.builder.rect': 'Drag a rectangle on the ground: the plan takes it'`
- feature entry `hint.builder.shapeL` — src/ui/i18n/en.ts:1774: `'hint.builder.shapeL': 'Drag a rectangle: the plan takes an L'`
- feature entry `hint.builder.shapeU` — src/ui/i18n/en.ts:1775: `'hint.builder.shapeU': 'Drag a rectangle: the plan takes a U'`
- feature entry `hint.builder.circle` — src/ui/i18n/en.ts:1776: `'hint.builder.circle': 'Drag a rectangle: the plan takes a circle'`
- feature entry `hint.builder.hexagon` — src/ui/i18n/en.ts:1777: `'hint.builder.hexagon': 'Drag a rectangle: the plan takes a hexagon'`
- feature entry `hint.builder.octagon` — src/ui/i18n/en.ts:1778: `'hint.builder.octagon': 'Drag a rectangle: the plan takes an octagon'`
- feature entry `hint.builder.chamfered` — src/ui/i18n/en.ts:1779: `'hint.builder.chamfered': 'Drag a rectangle: the plan takes a chamfered box'`
- feature entry `builder.tool.railing` — src/ui/i18n/en.ts:1780: `'builder.tool.railing': 'Railing'`
- feature entry `builder.tool.awning` — src/ui/i18n/en.ts:1781: `'builder.tool.awning': 'Awning'`
- feature entry `builder.tool.flowers` — src/ui/i18n/en.ts:1782: `'builder.tool.flowers': 'Flowers'`
- feature entry `builder.tool.rocks` — src/ui/i18n/en.ts:1783: `'builder.tool.rocks': 'Rocks'`
- feature entry `builder.tool.shrub` — src/ui/i18n/en.ts:1784: `'builder.tool.shrub': 'Shrub'`
- feature entry `builder.tool.hedge` — src/ui/i18n/en.ts:1785: `'builder.tool.hedge': 'Hedge'`
- feature entry `builder.tool.parking` — src/ui/i18n/en.ts:1786: `'builder.tool.parking': 'Parking'`
- shared entry `hint.builder.railing` — src/ui/i18n/en.ts:1787: `'hint.builder.railing': 'Point at a facade and click: a railing runs along it'`
- shared entry `hint.builder.awning` — src/ui/i18n/en.ts:1788: `'hint.builder.awning': 'Point at a facade and click: a canvas spreads over the bay'`
- shared entry `hint.builder.flowers` — src/ui/i18n/en.ts:1789: `'hint.builder.flowers': 'Click the ground to plant a flower bed'`
- shared entry `hint.builder.shrub` — src/ui/i18n/en.ts:1790: `'hint.builder.shrub': 'Click the ground to plant a shrub'`
- shared entry `hint.builder.hedge` — src/ui/i18n/en.ts:1791: `'hint.builder.hedge': 'Point at a facade and click: a hedge runs in front of it'`
- shared entry `hint.builder.rocks` — src/ui/i18n/en.ts:1792: `'hint.builder.rocks': 'Click the ground to drop a group of boulders'`
- shared entry `hint.builder.parking` — src/ui/i18n/en.ts:1793: `'hint.builder.parking': 'Point at a facade and click: a parking apron with its stalls'`
- feature entry `building.finish.stucco` — src/ui/i18n/en.ts:1794: `'building.finish.stucco': 'Stucco'`
- feature entry `building.finish.panel` — src/ui/i18n/en.ts:1795: `'building.finish.panel': 'Panel cladding'`
- feature entry `building.finish.slate` — src/ui/i18n/en.ts:1796: `'building.finish.slate': 'Slate'`
- feature entry `builder.tool.doubleDoor` — src/ui/i18n/en.ts:1797: `'builder.tool.doubleDoor': 'Double door'`
- feature entry `builder.tool.garageDoor` — src/ui/i18n/en.ts:1798: `'builder.tool.garageDoor': 'Garage door'`
- feature entry `hint.builder.doubleDoor` — src/ui/i18n/en.ts:1799: `'hint.builder.doubleDoor': 'Click facade bays for a two-leaf entrance'`
- feature entry `hint.builder.garageDoor` — src/ui/i18n/en.ts:1800: `'hint.builder.garageDoor': 'Click facade bays for a sectional garage door'`
- feature entry `builder.tool.stairRun` — src/ui/i18n/en.ts:1825: `'builder.tool.stairRun': 'Stair (traced)'`
- feature entry `hint.builder.stairRun` — src/ui/i18n/en.ts:1826: `'hint.builder.stairRun': 'Click the corners of the climb · Enter lays a flight that turns with the trace and lands on the floor above'`
- feature entry `hint.lot.front` — src/ui/i18n/pt-BR.ts:28: `'hint.lot.front': 'Frente definida: a construção vai olhar para esse lado.'`
- feature entry `hint.lot.frontPick` — src/ui/i18n/pt-BR.ts:29: `'hint.lot.frontPick': 'Clique perto de um lado de um lote.'`
- feature entry `hint.lot.split` — src/ui/i18n/pt-BR.ts:34: `'hint.lot.split': 'Lote dividido em dois (Shift: frente e fundos).'`
- feature entry `hint.lot.join` — src/ui/i18n/pt-BR.ts:35: `'hint.lot.join': 'Lotes juntados.'`
- feature entry `hint.lot.joinPick` — src/ui/i18n/pt-BR.ts:36: `'hint.lot.joinPick': 'Agora clique no lote ao lado.'`
- feature entry `hint.lot.joinFail` — src/ui/i18n/pt-BR.ts:37: `'hint.lot.joinFail': 'Esses lotes não têm um lado em comum.'`
- feature entry `hint.lot.added` — src/ui/i18n/pt-BR.ts:38: `'hint.lot.added': 'Lote novo criado.'`
- feature entry `hint.lot.addFail` — src/ui/i18n/pt-BR.ts:39: `'hint.lot.addFail': 'Não cabe um lote aí.'`
- shared entry `hint.lot.deleted` — src/ui/i18n/pt-BR.ts:40: `'hint.lot.deleted': 'Apagado.'`
- feature entry `hint.lot.splitFail` — src/ui/i18n/pt-BR.ts:46: `'hint.lot.splitFail': 'Não sobra um lote que valha a pena aí.'`
- feature entry `zone.lot.size` — src/ui/i18n/pt-BR.ts:47: `'zone.lot.size': '{w} x {d} m'`
- feature entry `hint.zone.removed` — src/ui/i18n/pt-BR.ts:56: `'hint.zone.removed': 'Zona removida'`
- feature entry `hint.zone.painted` — src/ui/i18n/pt-BR.ts:57: `'hint.zone.painted': 'Zona pintada: os prédios vão crescendo nela'`
- feature entry `hint.zone.empty` — src/ui/i18n/pt-BR.ts:58: `'hint.zone.empty': 'Nenhum lote aqui: desenhe um lote para zonear'`
- feature entry `hint.zone.pending` — src/ui/i18n/pt-BR.ts:59: `'hint.zone.pending': 'Zoneando assim que os lotes da rua estiverem prontos…'`
- feature entry `build.stamp` — src/ui/i18n/pt-BR.ts:173: `'build.stamp': '{branch} · {hash} · {date}'`
- feature entry `hint.blocks.built` — src/ui/i18n/pt-BR.ts:397: `'hint.blocks.built': 'Quarteirões construídos'`
- shared entry `hint.road.invalid` — src/ui/i18n/pt-BR.ts:714: `'hint.road.invalid': 'Esta conexão precisa de mais distância ou outro ponto final.'`
- feature entry `hint.strike.hit` — src/ui/i18n/pt-BR.ts:901: `'hint.strike.hit': 'Atingido: continue batendo para derrubar'`
- feature entry `hint.strike.down` — src/ui/i18n/pt-BR.ts:902: `'hint.strike.down': 'Construção desabou'`
- feature entry `hint.strike.deaths` — src/ui/i18n/pt-BR.ts:903: `'hint.strike.deaths': 'Atingido: houve vítimas'`
- feature entry `hint.strike.ground` — src/ui/i18n/pt-BR.ts:904: `'hint.strike.ground': 'Impacto no chão'`
- feature entry `hint.building.renovated` — src/ui/i18n/pt-BR.ts:905: `'hint.building.renovated': 'Prédio reformado'`
- feature entry `building.preset.block` — src/ui/i18n/pt-BR.ts:1025: `'building.preset.block': 'Bloco'`
- feature entry `building.preset.house` — src/ui/i18n/pt-BR.ts:1026: `'building.preset.house': 'Casa'`
- feature entry `building.preset.rowhouse` — src/ui/i18n/pt-BR.ts:1027: `'building.preset.rowhouse': 'Sobrado geminado'`
- feature entry `building.preset.apartments` — src/ui/i18n/pt-BR.ts:1028: `'building.preset.apartments': 'Prédio residencial'`
- feature entry `building.preset.tower` — src/ui/i18n/pt-BR.ts:1029: `'building.preset.tower': 'Torre com embasamento'`
- feature entry `building.preset.shop` — src/ui/i18n/pt-BR.ts:1030: `'building.preset.shop': 'Loja'`
- feature entry `building.preset.office` — src/ui/i18n/pt-BR.ts:1031: `'building.preset.office': 'Edifício de escritórios'`
- feature entry `building.preset.mixed` — src/ui/i18n/pt-BR.ts:1032: `'building.preset.mixed': 'Prédio de uso misto'`
- feature entry `building.preset.courtyard` — src/ui/i18n/pt-BR.ts:1033: `'building.preset.courtyard': 'Prédio com pátio'`
- feature entry `building.preset.warehouse` — src/ui/i18n/pt-BR.ts:1034: `'building.preset.warehouse': 'Galpão'`
- feature entry `building.preset.factory` — src/ui/i18n/pt-BR.ts:1035: `'building.preset.factory': 'Fábrica'`
- feature entry `building.element.noRoom` — src/ui/i18n/pt-BR.ts:1036: `'building.element.noRoom': 'Sem espaço para repeti-lo ao longo do edifício'`
- feature entry `building.relief.pickFace` — src/ui/i18n/pt-BR.ts:1037: `'building.relief.pickFace': 'Clique numa fachada (Shift+clique ou toque longo: mais vãos) e puxe a seta amarela'`
- feature entry `building.style.brick` — src/ui/i18n/pt-BR.ts:1038: `'building.style.brick': 'Tijolo'`
- feature entry `building.style.modern` — src/ui/i18n/pt-BR.ts:1039: `'building.style.modern': 'Moderno'`
- feature entry `building.style.mediterranean` — src/ui/i18n/pt-BR.ts:1040: `'building.style.mediterranean': 'Mediterrâneo'`
- feature entry `building.style.industrial` — src/ui/i18n/pt-BR.ts:1041: `'building.style.industrial': 'Industrial'`
- feature entry `building.style.timber` — src/ui/i18n/pt-BR.ts:1042: `'building.style.timber': 'Madeira'`
- feature entry `building.material.pickFace` — src/ui/i18n/pt-BR.ts:1043: `'building.material.pickFace': 'Clique numa fachada para pintar essa face'`
- feature entry `building.finish.plaster` — src/ui/i18n/pt-BR.ts:1044: `'building.finish.plaster': 'Reboco'`
- feature entry `building.finish.ceramic` — src/ui/i18n/pt-BR.ts:1045: `'building.finish.ceramic': 'Pastilha de porcelana'`
- feature entry `building.finish.brick` — src/ui/i18n/pt-BR.ts:1046: `'building.finish.brick': 'Tijolo'`
- feature entry `building.finish.stone` — src/ui/i18n/pt-BR.ts:1047: `'building.finish.stone': 'Pedra'`
- feature entry `building.finish.concrete` — src/ui/i18n/pt-BR.ts:1048: `'building.finish.concrete': 'Concreto'`
- feature entry `building.finish.wood` — src/ui/i18n/pt-BR.ts:1049: `'building.finish.wood': 'Madeira'`
- feature entry `building.finish.metal` — src/ui/i18n/pt-BR.ts:1050: `'building.finish.metal': 'Metal'`
- feature entry `building.finish.glass` — src/ui/i18n/pt-BR.ts:1051: `'building.finish.glass': 'Vidro'`
- feature entry `building.finish.tile` — src/ui/i18n/pt-BR.ts:1052: `'building.finish.tile': 'Telha'`
- feature entry `building.finish.roofing` — src/ui/i18n/pt-BR.ts:1053: `'building.finish.roofing': 'Manta'`
- feature entry `building.blueprintName` — src/ui/i18n/pt-BR.ts:1054: `'building.blueprintName': 'Nome deste modelo'`
- feature entry `building.blueprintSaved` — src/ui/i18n/pt-BR.ts:1055: `'building.blueprintSaved': 'Modelo salvo'`
- feature entry `building.blueprintNotSaved` — src/ui/i18n/pt-BR.ts:1056: `'building.blueprintNotSaved': 'Modelo não salvo: o armazenamento do navegador está cheio ou bloqueado'`
- feature entry `building.blueprintNotRemoved` — src/ui/i18n/pt-BR.ts:1057: `'building.blueprintNotRemoved': 'Modelo não removido: o armazenamento do navegador está cheio ou bloqueado'`
- feature entry `building.placed` — src/ui/i18n/pt-BR.ts:1058: `'building.placed': 'Edifício posicionado: arraste as setas para moldá-lo'`
- feature entry `building.copied` — src/ui/i18n/pt-BR.ts:1059: `'building.copied': 'Edifício copiado: Ctrl+V para posicionar uma cópia'`
- feature entry `building.problem.size` — src/ui/i18n/pt-BR.ts:1060: `'building.problem.size': 'Dimensões fora dos limites'`
- feature entry `building.problem.overlap` — src/ui/i18n/pt-BR.ts:1061: `'building.problem.overlap': 'Dois volumes ficariam sobrepostos'`
- feature entry `building.problem.support` — src/ui/i18n/pt-BR.ts:1062: `'building.problem.support': 'Um volume ficaria sem apoio'`
- feature entry `building.problem.footprint` — src/ui/i18n/pt-BR.ts:1063: `'building.problem.footprint': 'Nada ficaria apoiado no chão'`
- feature entry `building.problem.bounds` — src/ui/i18n/pt-BR.ts:1064: `'building.problem.bounds': 'Fora do mapa'`
- feature entry `building.problem.road` — src/ui/i18n/pt-BR.ts:1065: `'building.problem.road': 'Sobrepõe uma via'`
- feature entry `building.problem.building` — src/ui/i18n/pt-BR.ts:1066: `'building.problem.building': 'Sobrepõe outro edifício'`
- feature entry `building.problem.slope` — src/ui/i18n/pt-BR.ts:1067: `'building.problem.slope': 'O terreno é íngreme demais aqui'`
- feature entry `building.demolished.one` — src/ui/i18n/pt-BR.ts:1068: `'building.demolished.one': 'A via demoliu 1 edifício'`
- feature entry `building.demolished.other` — src/ui/i18n/pt-BR.ts:1069: `'building.demolished.other': 'A via demoliu {count} edifícios'`
- feature entry `building.floors.one` — src/ui/i18n/pt-BR.ts:1070: `'building.floors.one': '{count} pavimento'`
- feature entry `building.floors.other` — src/ui/i18n/pt-BR.ts:1071: `'building.floors.other': '{count} pavimentos'`
- feature entry `building.problem.outline` — src/ui/i18n/pt-BR.ts:1072: `'building.problem.outline': 'O contorno se cruza ou é pequeno demais'`
- feature entry `building.problem.roofSpace` — src/ui/i18n/pt-BR.ts:1073: `'building.problem.roofSpace': 'Escolha uma área livre inteiramente dentro de um telhado.'`
- feature entry `building.problem.cut` — src/ui/i18n/pt-BR.ts:1074: `'building.problem.cut': 'O recorte deve alcançar uma lateral. Faça pátio fechado com massas encostadas.'`
- feature entry `building.problem.setback` — src/ui/i18n/pt-BR.ts:1075: `'building.problem.setback': 'Sem espaço para recuo automático. Desenhe uma massa menor sobre o teto.'`
- feature entry `creator.dock.scope.bay` — src/ui/i18n/pt-BR.ts:1076: `'creator.dock.scope.bay': 'Vão'`
- feature entry `creator.dock.scope.column` — src/ui/i18n/pt-BR.ts:1077: `'creator.dock.scope.column': 'Coluna vertical'`
- feature entry `creator.dock.scope.row` — src/ui/i18n/pt-BR.ts:1078: `'creator.dock.scope.row': 'Fileira do andar'`
- feature entry `creator.dock.scope.storey` — src/ui/i18n/pt-BR.ts:1079: `'creator.dock.scope.storey': 'Andar inteiro'`
- feature entry `creator.dock.scope.side` — src/ui/i18n/pt-BR.ts:1080: `'creator.dock.scope.side': 'Face inteira'`
- feature entry `creator.dock.scope.volume` — src/ui/i18n/pt-BR.ts:1081: `'creator.dock.scope.volume': 'Bloco inteiro'`
- feature entry `creator.dock.scope.zone` — src/ui/i18n/pt-BR.ts:1082: `'creator.dock.scope.zone': 'Zona (arrastar)'`
- feature entry `building.reshapeRelief` — src/ui/i18n/pt-BR.ts:1084: `'building.reshapeRelief': 'Alinhe os relevos da fachada antes de trocar o contorno desta faixa.'`
- feature entry `creator.pattern.residential` — src/ui/i18n/pt-BR.ts:1085: `'creator.pattern.residential': 'Residencial'`
- feature entry `creator.pattern.storefront` — src/ui/i18n/pt-BR.ts:1086: `'creator.pattern.storefront': 'Lojas'`
- feature entry `creator.pattern.office` — src/ui/i18n/pt-BR.ts:1087: `'creator.pattern.office': 'Escritórios'`
- feature entry `creator.pattern.industrial` — src/ui/i18n/pt-BR.ts:1088: `'creator.pattern.industrial': 'Industrial'`
- feature entry `creator.pattern.arcade` — src/ui/i18n/pt-BR.ts:1089: `'creator.pattern.arcade': 'Pilotis'`
- feature entry `creator.pattern.gallery` — src/ui/i18n/pt-BR.ts:1090: `'creator.pattern.gallery': 'Galeria aberta'`
- feature entry `creator.pattern.artDeco` — src/ui/i18n/pt-BR.ts:1091: `'creator.pattern.artDeco': 'Art Déco'`
- feature entry `creator.pattern.artDecoCrown` — src/ui/i18n/pt-BR.ts:1092: `'creator.pattern.artDecoCrown': 'Coroamento Déco'`
- feature entry `creator.pattern.observation` — src/ui/i18n/pt-BR.ts:1093: `'creator.pattern.observation': 'Mirante envidraçado'`
- feature entry `builder.grid` — src/ui/i18n/pt-BR.ts:1097: `'builder.grid': 'Grade'`
- feature entry `builder.view.frame` — src/ui/i18n/pt-BR.ts:1098: `'builder.view.frame': 'Enquadrar a seleção'`
- feature entry `builder.hideOthers` — src/ui/i18n/pt-BR.ts:1099: `'builder.hideOthers': 'Ocultar outros'`
- feature entry `builder.floor.title` — src/ui/i18n/pt-BR.ts:1111: `'builder.floor.title': 'Em qual pavimento as ferramentas de face trabalham'`
- feature entry `builder.snap.label` — src/ui/i18n/pt-BR.ts:1112: `'builder.snap.label': 'Encaixe'`
- feature entry `builder.snap.auto` — src/ui/i18n/pt-BR.ts:1113: `'builder.snap.auto': 'Auto'`
- feature entry `builder.snap.grid` — src/ui/i18n/pt-BR.ts:1114: `'builder.snap.grid': 'Grade'`
- feature entry `builder.snap.edge` — src/ui/i18n/pt-BR.ts:1115: `'builder.snap.edge': 'Borda'`
- feature entry `builder.snap.face` — src/ui/i18n/pt-BR.ts:1116: `'builder.snap.face': 'Face'`
- feature entry `builder.snap.centre` — src/ui/i18n/pt-BR.ts:1117: `'builder.snap.centre': 'Centro'`
- feature entry `builder.snap.building` — src/ui/i18n/pt-BR.ts:1118: `'builder.snap.building': 'Edifício'`
- feature entry `builder.snap.road` — src/ui/i18n/pt-BR.ts:1119: `'builder.snap.road': 'Via'`
- feature entry `builder.snap.off` — src/ui/i18n/pt-BR.ts:1120: `'builder.snap.off': 'Desligado'`
- feature entry `builder.sel.title` — src/ui/i18n/pt-BR.ts:1123: `'builder.sel.title': 'Seleção'`
- feature entry `builder.sel.volume` — src/ui/i18n/pt-BR.ts:1124: `'builder.sel.volume': 'Volume {n}'`
- feature entry `builder.sel.building` — src/ui/i18n/pt-BR.ts:1125: `'builder.sel.building': 'Edifício'`
- feature entry `builder.sel.face` — src/ui/i18n/pt-BR.ts:1126: `'builder.sel.face': 'Face'`
- feature entry `builder.sel.floor` — src/ui/i18n/pt-BR.ts:1127: `'builder.sel.floor': 'pavimento {n}'`
- feature entry `builder.sel.element` — src/ui/i18n/pt-BR.ts:1128: `'builder.sel.element': 'Elemento'`
- feature entry `builder.sel.detail` — src/ui/i18n/pt-BR.ts:1129: `'builder.sel.detail': 'Detalhe de cobertura'`
- feature entry `builder.field.storeys` — src/ui/i18n/pt-BR.ts:1130: `'builder.field.storeys': 'Pavimentos'`
- feature entry `builder.field.storeyHeight` — src/ui/i18n/pt-BR.ts:1131: `'builder.field.storeyHeight': 'Pé-direito'`
- feature entry `builder.field.area` — src/ui/i18n/pt-BR.ts:1133: `'builder.field.area': 'Projeção'`
- feature entry `builder.field.pitch` — src/ui/i18n/pt-BR.ts:1134: `'builder.field.pitch': 'Inclinação'`
- feature entry `builder.field.depth` — src/ui/i18n/pt-BR.ts:1135: `'builder.field.depth': 'Profundidade'`
- feature entry `builder.field.width` — src/ui/i18n/pt-BR.ts:1143: `'builder.field.width': 'Largura'`
- feature entry `builder.field.flag` — src/ui/i18n/pt-BR.ts:1145: `'builder.field.flag': 'Bandeira'`
- feature entry `builder.flag.none` — src/ui/i18n/pt-BR.ts:1146: `'builder.flag.none': 'Nenhuma'`
- feature entry `builder.field.flagPattern` — src/ui/i18n/pt-BR.ts:1147: `'builder.field.flagPattern': 'Desenho da bandeira'`
- feature entry `builder.flagPattern.solid` — src/ui/i18n/pt-BR.ts:1148: `'builder.flagPattern.solid': 'Lisa'`
- feature entry `builder.flagPattern.bicolourH` — src/ui/i18n/pt-BR.ts:1149: `'builder.flagPattern.bicolourH': 'Duas faixas horizontais'`
- feature entry `builder.flagPattern.bicolourV` — src/ui/i18n/pt-BR.ts:1150: `'builder.flagPattern.bicolourV': 'Duas faixas verticais'`
- feature entry `builder.flagPattern.tricolourH` — src/ui/i18n/pt-BR.ts:1151: `'builder.flagPattern.tricolourH': 'Três faixas horizontais'`
- feature entry `builder.flagPattern.tricolourV` — src/ui/i18n/pt-BR.ts:1152: `'builder.flagPattern.tricolourV': 'Três faixas verticais'`
- feature entry `builder.flagPattern.cross` — src/ui/i18n/pt-BR.ts:1153: `'builder.flagPattern.cross': 'Cruz'`
- feature entry `builder.flagPattern.nordic` — src/ui/i18n/pt-BR.ts:1154: `'builder.flagPattern.nordic': 'Cruz nórdica'`
- feature entry `builder.flagPattern.diagonal` — src/ui/i18n/pt-BR.ts:1155: `'builder.flagPattern.diagonal': 'Diagonal'`
- feature entry `builder.flagPattern.saltire` — src/ui/i18n/pt-BR.ts:1156: `'builder.flagPattern.saltire': 'Cruz em X'`
- feature entry `builder.flagPattern.canton` — src/ui/i18n/pt-BR.ts:1157: `'builder.flagPattern.canton': 'Cantão'`
- feature entry `builder.flagPattern.stripes` — src/ui/i18n/pt-BR.ts:1158: `'builder.flagPattern.stripes': 'Listras com cantão'`
- feature entry `builder.flagPattern.border` — src/ui/i18n/pt-BR.ts:1159: `'builder.flagPattern.border': 'Borda'`
- feature entry `builder.flagPattern.disc` — src/ui/i18n/pt-BR.ts:1160: `'builder.flagPattern.disc': 'Disco'`
- feature entry `builder.flagPattern.chevron` — src/ui/i18n/pt-BR.ts:1161: `'builder.flagPattern.chevron': 'Triângulo'`
- feature entry `builder.flagColour.0` — src/ui/i18n/pt-BR.ts:1162: `'builder.flagColour.0': 'Branco'`
- feature entry `builder.flagColour.1` — src/ui/i18n/pt-BR.ts:1163: `'builder.flagColour.1': 'Preto'`
- feature entry `builder.flagColour.2` — src/ui/i18n/pt-BR.ts:1164: `'builder.flagColour.2': 'Vermelho'`
- feature entry `builder.flagColour.3` — src/ui/i18n/pt-BR.ts:1165: `'builder.flagColour.3': 'Laranja-avermelhado'`
- feature entry `builder.flagColour.4` — src/ui/i18n/pt-BR.ts:1166: `'builder.flagColour.4': 'Âmbar'`
- feature entry `builder.flagColour.5` — src/ui/i18n/pt-BR.ts:1167: `'builder.flagColour.5': 'Amarelo'`
- feature entry `builder.flagColour.6` — src/ui/i18n/pt-BR.ts:1168: `'builder.flagColour.6': 'Verde'`
- feature entry `builder.flagColour.7` — src/ui/i18n/pt-BR.ts:1169: `'builder.flagColour.7': 'Verde-escuro'`
- feature entry `builder.flagColour.8` — src/ui/i18n/pt-BR.ts:1170: `'builder.flagColour.8': 'Azul'`
- feature entry `builder.flagColour.9` — src/ui/i18n/pt-BR.ts:1171: `'builder.flagColour.9': 'Azul-marinho'`
- feature entry `builder.flagColour.10` — src/ui/i18n/pt-BR.ts:1172: `'builder.flagColour.10': 'Azul-claro'`
- feature entry `builder.flagColour.11` — src/ui/i18n/pt-BR.ts:1173: `'builder.flagColour.11': 'Roxo'`
- feature entry `builder.flagColour.12` — src/ui/i18n/pt-BR.ts:1174: `'builder.flagColour.12': 'Vinho'`
- feature entry `builder.flagColour.13` — src/ui/i18n/pt-BR.ts:1175: `'builder.flagColour.13': 'Marrom'`
- feature entry `builder.flagColour.14` — src/ui/i18n/pt-BR.ts:1176: `'builder.flagColour.14': 'Cinza'`
- feature entry `builder.flagColour.15` — src/ui/i18n/pt-BR.ts:1177: `'builder.flagColour.15': 'Rosa'`
- feature entry `builder.flag.plain` — src/ui/i18n/pt-BR.ts:1178: `'builder.flag.plain': 'Lisa'`
- feature entry `builder.flag.saoPaulo` — src/ui/i18n/pt-BR.ts:1179: `'builder.flag.saoPaulo': 'Cidade de São Paulo'`
- feature entry `builder.flag.saoPauloState` — src/ui/i18n/pt-BR.ts:1180: `'builder.flag.saoPauloState': 'Estado de São Paulo'`
- feature entry `builder.field.height` — src/ui/i18n/pt-BR.ts:1182: `'builder.field.height': 'Altura'`
- feature entry `builder.field.material` — src/ui/i18n/pt-BR.ts:1183: `'builder.field.material': 'Material'`
- feature entry `builder.quick.duplicate` — src/ui/i18n/pt-BR.ts:1184: `'builder.quick.duplicate': 'Duplicar'`
- feature entry `builder.quick.mirror` — src/ui/i18n/pt-BR.ts:1185: `'builder.quick.mirror': 'Espelhar'`
- feature entry `builder.quick.group` — src/ui/i18n/pt-BR.ts:1186: `'builder.quick.group': 'Agrupar com o edifício ao lado'`
- feature entry `builder.quick.delete` — src/ui/i18n/pt-BR.ts:1187: `'builder.quick.delete': 'Demolir'`
- feature entry `builder.styleCopied` — src/ui/i18n/pt-BR.ts:1188: `'builder.styleCopied': 'Estilo copiado: o pincel o carrega'`
- feature entry `builder.roof.ridge.label` — src/ui/i18n/pt-BR.ts:1189: `'builder.roof.ridge.label': 'Cumeeira'`
- feature entry `builder.roof.side.label` — src/ui/i18n/pt-BR.ts:1190: `'builder.roof.side.label': 'Água para'`
- feature entry `builder.roof.ridge.x` — src/ui/i18n/pt-BR.ts:1191: `'builder.roof.ridge.x': 'Cumeeira esquerda–direita'`
- feature entry `builder.roof.ridge.y` — src/ui/i18n/pt-BR.ts:1192: `'builder.roof.ridge.y': 'Cumeeira frente–fundos'`
- feature entry `builder.roof.side.front` — src/ui/i18n/pt-BR.ts:1193: `'builder.roof.side.front': 'Água para a frente'`
- feature entry `builder.roof.side.right` — src/ui/i18n/pt-BR.ts:1194: `'builder.roof.side.right': 'Água para a direita'`
- feature entry `builder.roof.side.back` — src/ui/i18n/pt-BR.ts:1195: `'builder.roof.side.back': 'Água para os fundos'`
- feature entry `builder.roof.side.left` — src/ui/i18n/pt-BR.ts:1196: `'builder.roof.side.left': 'Água para a esquerda'`
- feature entry `builder.plan.finish` — src/ui/i18n/pt-BR.ts:1197: `'builder.plan.finish': 'Concluir'`
- feature entry `builder.plan.back` — src/ui/i18n/pt-BR.ts:1198: `'builder.plan.back': 'Voltar ponto'`
- feature entry `builder.plan.cancel` — src/ui/i18n/pt-BR.ts:1199: `'builder.plan.cancel': 'Cancelar'`
- feature entry `builder.noGroup` — src/ui/i18n/pt-BR.ts:1200: `'builder.noGroup': 'Nenhum edifício perto o bastante para agrupar'`
- feature entry `builder.grouped` — src/ui/i18n/pt-BR.ts:1201: `'builder.grouped': 'Agrupado num só edifício'`
- feature entry `building.selectFirst` — src/ui/i18n/pt-BR.ts:1202: `'building.selectFirst': 'Selecione um edifício primeiro'`
- feature entry `builder.category.draw` — src/ui/i18n/pt-BR.ts:1204: `'builder.category.draw': 'Desenhar'`
- shared entry `inside.toggle` — src/ui/i18n/pt-BR.ts:1205: `'inside.toggle': 'Ver por dentro: o prédio no centro da vista (ou dê dois cliques num prédio)'`
- feature entry `inside.down` — src/ui/i18n/pt-BR.ts:1206: `'inside.down': 'Andar abaixo'`
- feature entry `inside.up` — src/ui/i18n/pt-BR.ts:1207: `'inside.up': 'Andar acima'`
- feature entry `inside.ground` — src/ui/i18n/pt-BR.ts:1208: `'inside.ground': 'Térreo'`
- feature entry `hint.inside.none` — src/ui/i18n/pt-BR.ts:1209: `'hint.inside.none': 'Nenhum prédio no centro da vista: aponte a vista para um, ou dê dois cliques nele'`
- feature entry `inside.floor` — src/ui/i18n/pt-BR.ts:1210: `'inside.floor': '{n}º andar'`
- feature entry `building.fn.house` — src/ui/i18n/pt-BR.ts:1211: `'building.fn.house': 'Casa'`
- feature entry `building.fn.townhouse` — src/ui/i18n/pt-BR.ts:1212: `'building.fn.townhouse': 'Sobrado'`
- feature entry `building.fn.apartments` — src/ui/i18n/pt-BR.ts:1213: `'building.fn.apartments': 'Prédio residencial'`
- feature entry `building.fn.residentialTower` — src/ui/i18n/pt-BR.ts:1214: `'building.fn.residentialTower': 'Torre residencial'`
- feature entry `building.fn.cityHall` — src/ui/i18n/pt-BR.ts:1215: `'building.fn.cityHall': 'Prefeitura'`
- feature entry `building.fn.council` — src/ui/i18n/pt-BR.ts:1216: `'building.fn.council': 'Câmara de Vereadores'`
- feature entry `building.fn.courthouse` — src/ui/i18n/pt-BR.ts:1217: `'building.fn.courthouse': 'Fórum'`
- feature entry `building.fn.postOffice` — src/ui/i18n/pt-BR.ts:1218: `'building.fn.postOffice': 'Correios'`
- feature entry `building.fn.police` — src/ui/i18n/pt-BR.ts:1219: `'building.fn.police': 'Delegacia'`
- feature entry `building.fn.fireStation` — src/ui/i18n/pt-BR.ts:1220: `'building.fn.fireStation': 'Bombeiros'`
- feature entry `building.fn.hospital` — src/ui/i18n/pt-BR.ts:1221: `'building.fn.hospital': 'Hospital'`
- feature entry `building.fn.clinic` — src/ui/i18n/pt-BR.ts:1222: `'building.fn.clinic': 'Posto de saúde'`
- feature entry `building.fn.school` — src/ui/i18n/pt-BR.ts:1223: `'building.fn.school': 'Escola'`
- feature entry `building.fn.university` — src/ui/i18n/pt-BR.ts:1224: `'building.fn.university': 'Faculdade'`
- feature entry `building.fn.library` — src/ui/i18n/pt-BR.ts:1225: `'building.fn.library': 'Biblioteca'`
- feature entry `building.fn.museum` — src/ui/i18n/pt-BR.ts:1226: `'building.fn.museum': 'Museu'`
- feature entry `building.fn.prison` — src/ui/i18n/pt-BR.ts:1227: `'building.fn.prison': 'Presídio'`
- feature entry `building.fn.church` — src/ui/i18n/pt-BR.ts:1228: `'building.fn.church': 'Igreja'`
- feature entry `building.fn.cemetery` — src/ui/i18n/pt-BR.ts:1229: `'building.fn.cemetery': 'Cemitério'`
- feature entry `building.fn.busStation` — src/ui/i18n/pt-BR.ts:1230: `'building.fn.busStation': 'Rodoviária'`
- feature entry `building.fn.shop` — src/ui/i18n/pt-BR.ts:1231: `'building.fn.shop': 'Loja'`
- feature entry `building.fn.supermarket` — src/ui/i18n/pt-BR.ts:1232: `'building.fn.supermarket': 'Supermercado'`
- feature entry `building.fn.mall` — src/ui/i18n/pt-BR.ts:1233: `'building.fn.mall': 'Shopping'`
- feature entry `building.fn.bank` — src/ui/i18n/pt-BR.ts:1234: `'building.fn.bank': 'Banco'`
- feature entry `building.fn.pharmacy` — src/ui/i18n/pt-BR.ts:1235: `'building.fn.pharmacy': 'Farmácia'`
- feature entry `building.fn.bakery` — src/ui/i18n/pt-BR.ts:1236: `'building.fn.bakery': 'Padaria'`
- feature entry `building.fn.restaurant` — src/ui/i18n/pt-BR.ts:1237: `'building.fn.restaurant': 'Restaurante'`
- feature entry `building.fn.snackBar` — src/ui/i18n/pt-BR.ts:1238: `'building.fn.snackBar': 'Lanchonete'`
- feature entry `building.fn.bar` — src/ui/i18n/pt-BR.ts:1239: `'building.fn.bar': 'Bar'`
- feature entry `building.fn.nightclub` — src/ui/i18n/pt-BR.ts:1240: `'building.fn.nightclub': 'Discoteca'`
- feature entry `building.fn.cinema` — src/ui/i18n/pt-BR.ts:1241: `'building.fn.cinema': 'Cinema'`
- feature entry `building.fn.hotel` — src/ui/i18n/pt-BR.ts:1242: `'building.fn.hotel': 'Hotel'`
- feature entry `building.fn.gym` — src/ui/i18n/pt-BR.ts:1243: `'building.fn.gym': 'Academia'`
- feature entry `building.fn.club` — src/ui/i18n/pt-BR.ts:1244: `'building.fn.club': 'Clube'`
- feature entry `building.fn.gasStation` — src/ui/i18n/pt-BR.ts:1245: `'building.fn.gasStation': 'Posto de gasolina'`
- feature entry `building.fn.office` — src/ui/i18n/pt-BR.ts:1246: `'building.fn.office': 'Escritórios'`
- feature entry `building.fn.factory` — src/ui/i18n/pt-BR.ts:1247: `'building.fn.factory': 'Fábrica'`
- feature entry `building.fn.warehouse` — src/ui/i18n/pt-BR.ts:1248: `'building.fn.warehouse': 'Galpão'`
- feature entry `building.fn.park` — src/ui/i18n/pt-BR.ts:1249: `'building.fn.park': 'Parque'`
- feature entry `building.fn.square` — src/ui/i18n/pt-BR.ts:1250: `'building.fn.square': 'Praça'`
- feature entry `building.fn.playground` — src/ui/i18n/pt-BR.ts:1251: `'building.fn.playground': 'Parquinho'`
- feature entry `building.fn.sportsCourt` — src/ui/i18n/pt-BR.ts:1252: `'building.fn.sportsCourt': 'Quadra'`
- feature entry `builder.city.homes` — src/ui/i18n/pt-BR.ts:1253: `'builder.city.homes': 'Moradia'`
- feature entry `builder.city.public` — src/ui/i18n/pt-BR.ts:1254: `'builder.city.public': 'Serviços públicos'`
- feature entry `builder.city.commerce` — src/ui/i18n/pt-BR.ts:1255: `'builder.city.commerce': 'Comércio e serviços'`
- feature entry `builder.city.work` — src/ui/i18n/pt-BR.ts:1256: `'builder.city.work': 'Trabalho'`
- feature entry `builder.city.leisure` — src/ui/i18n/pt-BR.ts:1257: `'builder.city.leisure': 'Lazer'`
- feature entry `builder.city.generic` — src/ui/i18n/pt-BR.ts:1258: `'builder.city.generic': 'Blocos genéricos'`
- feature entry `builder.city.lots` — src/ui/i18n/pt-BR.ts:1259: `'builder.city.lots': 'Lotes'`
- feature entry `builder.lot.balconyMid` — src/ui/i18n/pt-BR.ts:1260: `'builder.lot.balconyMid': 'Varandas e madeira'`
- feature entry `builder.lot.glassOffice` — src/ui/i18n/pt-BR.ts:1261: `'builder.lot.glassOffice': 'Escritório de vidro'`
- feature entry `builder.lot.glassBalcony` — src/ui/i18n/pt-BR.ts:1262: `'builder.lot.glassBalcony': 'Vidro com varandas'`
- feature entry `builder.lot.beigeClassic` — src/ui/i18n/pt-BR.ts:1263: `'builder.lot.beigeClassic': 'Bege clássica'`
- feature entry `builder.lot.darkGlass` — src/ui/i18n/pt-BR.ts:1264: `'builder.lot.darkGlass': 'Vidro escuro'`
- feature entry `builder.lot.whiteBalcony` — src/ui/i18n/pt-BR.ts:1265: `'builder.lot.whiteBalcony': 'Branca com varandas'`
- feature entry `builder.lot.brickFrame` — src/ui/i18n/pt-BR.ts:1266: `'builder.lot.brickFrame': 'Tijolo e vidro'`
- feature entry `builder.lot.roundGlass` — src/ui/i18n/pt-BR.ts:1267: `'builder.lot.roundGlass': 'Redonda de vidro'`
- feature entry `builder.lot.darkGrid` — src/ui/i18n/pt-BR.ts:1268: `'builder.lot.darkGrid': 'Grade escura'`
- feature entry `builder.lot.artDeco` — src/ui/i18n/pt-BR.ts:1269: `'builder.lot.artDeco': 'Art déco'`
- feature entry `builder.lot.twistGreen` — src/ui/i18n/pt-BR.ts:1270: `'builder.lot.twistGreen': 'Torcida verde-água'`
- feature entry `builder.lot.hexTerracotta` — src/ui/i18n/pt-BR.ts:1271: `'builder.lot.hexTerracotta': 'Hexagonal de terracota'`
- feature entry `builder.lot.stepGarden` — src/ui/i18n/pt-BR.ts:1272: `'builder.lot.stepGarden': 'Terraços-jardim'`
- feature entry `builder.lot.waveWhite` — src/ui/i18n/pt-BR.ts:1273: `'builder.lot.waveWhite': 'Varandas em onda'`
- feature entry `builder.lot.cubeBalcony` — src/ui/i18n/pt-BR.ts:1274: `'builder.lot.cubeBalcony': 'Varandas em cubo'`
- feature entry `builder.lot.twinNavy` — src/ui/i18n/pt-BR.ts:1275: `'builder.lot.twinNavy': 'Gêmeas azul-marinho'`
- feature entry `builder.lot.octCopper` — src/ui/i18n/pt-BR.ts:1276: `'builder.lot.octCopper': 'Octogonal de cobre'`
- feature entry `builder.lot.stackedBlocks` — src/ui/i18n/pt-BR.ts:1277: `'builder.lot.stackedBlocks': 'Blocos empilhados'`
- feature entry `builder.lot.pinkCorner` — src/ui/i18n/pt-BR.ts:1278: `'builder.lot.pinkCorner': 'Esquina rosa'`
- feature entry `builder.lot.triangleDark` — src/ui/i18n/pt-BR.ts:1279: `'builder.lot.triangleDark': 'Triangular escura'`
- feature entry `builder.lot.cityHallBrick` — src/ui/i18n/pt-BR.ts:1280: `'builder.lot.cityHallBrick': 'Prefeitura de tijolo'`
- feature entry `builder.lot.decoSpire` — src/ui/i18n/pt-BR.ts:1281: `'builder.lot.decoSpire': 'Art déco com pináculo'`
- feature entry `builder.lot.hospitalA` — src/ui/i18n/pt-BR.ts:1282: `'builder.lot.hospitalA': 'Hospital'`
- feature entry `builder.lot.loftBrick` — src/ui/i18n/pt-BR.ts:1283: `'builder.lot.loftBrick': 'Lofts de tijolo'`
- feature entry `builder.lot.cityHallWhite` — src/ui/i18n/pt-BR.ts:1284: `'builder.lot.cityHallWhite': 'Prefeitura branca'`
- feature entry `builder.lot.forum` — src/ui/i18n/pt-BR.ts:1285: `'builder.lot.forum': 'Fórum'`
- feature entry `builder.lot.church` — src/ui/i18n/pt-BR.ts:1286: `'builder.lot.church': 'Igreja'`
- feature entry `builder.lot.bank` — src/ui/i18n/pt-BR.ts:1287: `'builder.lot.bank': 'Banco'`
- feature entry `builder.lot.school` — src/ui/i18n/pt-BR.ts:1288: `'builder.lot.school': 'Escola'`
- feature entry `builder.lot.fireStation` — src/ui/i18n/pt-BR.ts:1289: `'builder.lot.fireStation': 'Bombeiros'`
- feature entry `builder.lot.police` — src/ui/i18n/pt-BR.ts:1290: `'builder.lot.police': 'Delegacia'`
- feature entry `builder.lot.hospitalB` — src/ui/i18n/pt-BR.ts:1291: `'builder.lot.hospitalB': 'Hospital com ala'`
- feature entry `builder.lot.cemetery` — src/ui/i18n/pt-BR.ts:1292: `'builder.lot.cemetery': 'Cemitério'`
- feature entry `builder.lot.beauxArts` — src/ui/i18n/pt-BR.ts:1293: `'builder.lot.beauxArts': 'Hotel Beaux-Arts'`
- feature entry `builder.lot.decoTower` — src/ui/i18n/pt-BR.ts:1294: `'builder.lot.decoTower': 'Torre art déco'`
- feature entry `builder.lot.modernOffice` — src/ui/i18n/pt-BR.ts:1295: `'builder.lot.modernOffice': 'Escritório modernista'`
- feature entry `builder.lot.residentialTerrace` — src/ui/i18n/pt-BR.ts:1296: `'builder.lot.residentialTerrace': 'Residencial com terraços'`
- feature entry `builder.lot.glassSpire` — src/ui/i18n/pt-BR.ts:1297: `'builder.lot.glassSpire': 'Torre de vidro azul'`
- feature entry `builder.city.all` — src/ui/i18n/pt-BR.ts:1298: `'builder.city.all': 'Todos'`
- feature entry `v2.build.shape` — src/ui/i18n/pt-BR.ts:1377: `'v2.build.shape': 'Forma'`
- feature entry `v2.build.selectToEdit` — src/ui/i18n/pt-BR.ts:1378: `'v2.build.selectToEdit': 'Clique num prédio para editá-lo'`
- feature entry `v2.build.floor` — src/ui/i18n/pt-BR.ts:1379: `'v2.build.floor': 'Andar'`
- feature entry `v2.build.plan` — src/ui/i18n/pt-BR.ts:1380: `'v2.build.plan': 'Planta'`
- feature entry `v2.build.material` — src/ui/i18n/pt-BR.ts:1381: `'v2.build.material': 'Material'`
- feature entry `v2.build.colour` — src/ui/i18n/pt-BR.ts:1382: `'v2.build.colour': 'Cor'`
- feature entry `builder.city.mine` — src/ui/i18n/pt-BR.ts:1383: `'builder.city.mine': 'Meus modelos'`
- feature entry `builder.advanced.off` — src/ui/i18n/pt-BR.ts:1386: `'builder.advanced.off': 'Mostrar modelagem avançada'`
- feature entry `builder.advanced.on` — src/ui/i18n/pt-BR.ts:1387: `'builder.advanced.on': 'Ocultar modelagem avançada'`
- feature entry `builder.field.function` — src/ui/i18n/pt-BR.ts:1388: `'builder.field.function': 'Função'`
- feature entry `building.fn.none` — src/ui/i18n/pt-BR.ts:1389: `'building.fn.none': 'Nenhuma'`
- feature entry `builder.tool.clock` — src/ui/i18n/pt-BR.ts:1390: `'builder.tool.clock': 'Relógio'`
- shared entry `hint.builder.clock` — src/ui/i18n/pt-BR.ts:1391: `'hint.builder.clock': 'Clique num vão da fachada para pendurar um relógio'`
- feature entry `builder.category.interior` — src/ui/i18n/pt-BR.ts:1392: `'builder.category.interior': 'Interior'`
- feature entry `builder.section.interiorView` — src/ui/i18n/pt-BR.ts:1393: `'builder.section.interiorView': 'Ver por dentro'`
- feature entry `builder.section.cores` — src/ui/i18n/pt-BR.ts:1394: `'builder.section.cores': 'Circulação: escadas e elevadores'`
- feature entry `builder.tool.interiorView` — src/ui/i18n/pt-BR.ts:1395: `'builder.tool.interiorView': 'Cortar e ver'`
- feature entry `builder.tool.floorDown` — src/ui/i18n/pt-BR.ts:1396: `'builder.tool.floorDown': 'Andar abaixo'`
- feature entry `builder.tool.floorUp` — src/ui/i18n/pt-BR.ts:1397: `'builder.tool.floorUp': 'Andar acima'`
- feature entry `builder.tool.coreLift` — src/ui/i18n/pt-BR.ts:1398: `'builder.tool.coreLift': 'Elevador'`
- feature entry `builder.tool.coreStair` — src/ui/i18n/pt-BR.ts:1399: `'builder.tool.coreStair': 'Escada'`
- feature entry `builder.tool.coreBoth` — src/ui/i18n/pt-BR.ts:1400: `'builder.tool.coreBoth': 'Escada + elevador'`
- feature entry `builder.tool.furn_sofa` — src/ui/i18n/pt-BR.ts:1401: `'builder.tool.furn_sofa': 'Sofá'`
- feature entry `hint.builder.furn_sofa` — src/ui/i18n/pt-BR.ts:1402: `'hint.builder.furn_sofa': 'Clique no piso para pôr: sofá (R gira)'`
- feature entry `builder.tool.furn_armchair` — src/ui/i18n/pt-BR.ts:1403: `'builder.tool.furn_armchair': 'Poltrona'`
- feature entry `hint.builder.furn_armchair` — src/ui/i18n/pt-BR.ts:1404: `'hint.builder.furn_armchair': 'Clique no piso para pôr: poltrona (R gira)'`
- feature entry `builder.tool.furn_table` — src/ui/i18n/pt-BR.ts:1405: `'builder.tool.furn_table': 'Mesa'`
- feature entry `hint.builder.furn_table` — src/ui/i18n/pt-BR.ts:1406: `'hint.builder.furn_table': 'Clique no piso para pôr: mesa (R gira)'`
- feature entry `builder.tool.furn_chair` — src/ui/i18n/pt-BR.ts:1407: `'builder.tool.furn_chair': 'Cadeira'`
- feature entry `hint.builder.furn_chair` — src/ui/i18n/pt-BR.ts:1408: `'hint.builder.furn_chair': 'Clique no piso para pôr: cadeira (R gira)'`
- feature entry `builder.tool.furn_bed` — src/ui/i18n/pt-BR.ts:1409: `'builder.tool.furn_bed': 'Cama de casal'`
- feature entry `hint.builder.furn_bed` — src/ui/i18n/pt-BR.ts:1410: `'hint.builder.furn_bed': 'Clique no piso para pôr: cama de casal (R gira)'`
- feature entry `builder.tool.furn_singleBed` — src/ui/i18n/pt-BR.ts:1411: `'builder.tool.furn_singleBed': 'Cama de solteiro'`
- feature entry `hint.builder.furn_singleBed` — src/ui/i18n/pt-BR.ts:1412: `'hint.builder.furn_singleBed': 'Clique no piso para pôr: cama de solteiro (R gira)'`
- feature entry `builder.tool.furn_wardrobe` — src/ui/i18n/pt-BR.ts:1413: `'builder.tool.furn_wardrobe': 'Guarda-roupa'`
- feature entry `hint.builder.furn_wardrobe` — src/ui/i18n/pt-BR.ts:1414: `'hint.builder.furn_wardrobe': 'Clique no piso para pôr: guarda-roupa (R gira)'`
- feature entry `builder.tool.furn_tv` — src/ui/i18n/pt-BR.ts:1415: `'builder.tool.furn_tv': 'TV'`
- feature entry `hint.builder.furn_tv` — src/ui/i18n/pt-BR.ts:1416: `'hint.builder.furn_tv': 'Clique no piso para pôr: tv (R gira)'`
- feature entry `builder.tool.furn_desk` — src/ui/i18n/pt-BR.ts:1417: `'builder.tool.furn_desk': 'Escrivaninha'`
- feature entry `hint.builder.furn_desk` — src/ui/i18n/pt-BR.ts:1418: `'hint.builder.furn_desk': 'Clique no piso para pôr: escrivaninha (R gira)'`
- feature entry `builder.tool.furn_officeChair` — src/ui/i18n/pt-BR.ts:1419: `'builder.tool.furn_officeChair': 'Cadeira de escritório'`
- feature entry `hint.builder.furn_officeChair` — src/ui/i18n/pt-BR.ts:1420: `'hint.builder.furn_officeChair': 'Clique no piso para pôr: cadeira de escritório (R gira)'`
- feature entry `builder.tool.furn_bookshelf` — src/ui/i18n/pt-BR.ts:1421: `'builder.tool.furn_bookshelf': 'Estante de livros'`
- feature entry `hint.builder.furn_bookshelf` — src/ui/i18n/pt-BR.ts:1422: `'hint.builder.furn_bookshelf': 'Clique no piso para pôr: estante de livros (R gira)'`
- feature entry `builder.tool.furn_shelf` — src/ui/i18n/pt-BR.ts:1423: `'builder.tool.furn_shelf': 'Prateleira'`
- feature entry `hint.builder.furn_shelf` — src/ui/i18n/pt-BR.ts:1424: `'hint.builder.furn_shelf': 'Clique no piso para pôr: prateleira (R gira)'`
- feature entry `builder.tool.furn_counter` — src/ui/i18n/pt-BR.ts:1425: `'builder.tool.furn_counter': 'Balcão'`
- feature entry `hint.builder.furn_counter` — src/ui/i18n/pt-BR.ts:1426: `'hint.builder.furn_counter': 'Clique no piso para pôr: balcão (R gira)'`
- feature entry `builder.tool.furn_fridge` — src/ui/i18n/pt-BR.ts:1427: `'builder.tool.furn_fridge': 'Geladeira'`
- feature entry `hint.builder.furn_fridge` — src/ui/i18n/pt-BR.ts:1428: `'hint.builder.furn_fridge': 'Clique no piso para pôr: geladeira (R gira)'`
- feature entry `builder.tool.furn_stove` — src/ui/i18n/pt-BR.ts:1429: `'builder.tool.furn_stove': 'Fogão'`
- feature entry `hint.builder.furn_stove` — src/ui/i18n/pt-BR.ts:1430: `'hint.builder.furn_stove': 'Clique no piso para pôr: fogão (R gira)'`
- feature entry `builder.tool.furn_sink` — src/ui/i18n/pt-BR.ts:1431: `'builder.tool.furn_sink': 'Pia'`
- feature entry `hint.builder.furn_sink` — src/ui/i18n/pt-BR.ts:1432: `'hint.builder.furn_sink': 'Clique no piso para pôr: pia (R gira)'`
- feature entry `builder.tool.furn_bath` — src/ui/i18n/pt-BR.ts:1433: `'builder.tool.furn_bath': 'Banheira'`
- feature entry `hint.builder.furn_bath` — src/ui/i18n/pt-BR.ts:1434: `'hint.builder.furn_bath': 'Clique no piso para pôr: banheira (R gira)'`
- feature entry `builder.tool.furn_toilet` — src/ui/i18n/pt-BR.ts:1435: `'builder.tool.furn_toilet': 'Vaso sanitário'`
- feature entry `hint.builder.furn_toilet` — src/ui/i18n/pt-BR.ts:1436: `'hint.builder.furn_toilet': 'Clique no piso para pôr: vaso sanitário (R gira)'`
- feature entry `builder.tool.furn_plant` — src/ui/i18n/pt-BR.ts:1437: `'builder.tool.furn_plant': 'Planta'`
- feature entry `hint.builder.furn_plant` — src/ui/i18n/pt-BR.ts:1438: `'hint.builder.furn_plant': 'Clique no piso para pôr: planta (R gira)'`
- feature entry `builder.tool.furn_ceilingLamp` — src/ui/i18n/pt-BR.ts:1439: `'builder.tool.furn_ceilingLamp': 'Luminária de teto'`
- feature entry `hint.builder.furn_ceilingLamp` — src/ui/i18n/pt-BR.ts:1440: `'hint.builder.furn_ceilingLamp': 'Clique no piso para pôr: luminária de teto (R gira)'`
- feature entry `builder.tool.furn_floorLamp` — src/ui/i18n/pt-BR.ts:1441: `'builder.tool.furn_floorLamp': 'Luminária de pé'`
- feature entry `hint.builder.furn_floorLamp` — src/ui/i18n/pt-BR.ts:1442: `'hint.builder.furn_floorLamp': 'Clique no piso para pôr: luminária de pé (R gira)'`
- feature entry `builder.tool.furn_tableLamp` — src/ui/i18n/pt-BR.ts:1443: `'builder.tool.furn_tableLamp': 'Abajur'`
- feature entry `hint.builder.furn_tableLamp` — src/ui/i18n/pt-BR.ts:1444: `'hint.builder.furn_tableLamp': 'Clique no piso para pôr: abajur (R gira)'`
- feature entry `builder.tool.furnMove` — src/ui/i18n/pt-BR.ts:1445: `'builder.tool.furnMove': 'Mover'`
- feature entry `hint.builder.furnMove` — src/ui/i18n/pt-BR.ts:1446: `'hint.builder.furnMove': 'Clique num móvel para pegá-lo, e de novo onde ele vai (R gira)'`
- feature entry `builder.tool.furnTurn` — src/ui/i18n/pt-BR.ts:1447: `'builder.tool.furnTurn': 'Girar'`
- feature entry `hint.builder.furnTurn` — src/ui/i18n/pt-BR.ts:1448: `'hint.builder.furnTurn': 'Gira o móvel selecionado'`
- feature entry `builder.tool.furnRemove` — src/ui/i18n/pt-BR.ts:1449: `'builder.tool.furnRemove': 'Tirar'`
- feature entry `hint.builder.furnRemove` — src/ui/i18n/pt-BR.ts:1450: `'hint.builder.furnRemove': 'Clique num móvel ou luminária para tirá-lo'`
- feature entry `builder.section.furnishing` — src/ui/i18n/pt-BR.ts:1451: `'builder.section.furnishing': 'Arrumar'`
- feature entry `builder.tool.coreRemove` — src/ui/i18n/pt-BR.ts:1452: `'builder.tool.coreRemove': 'Remover'`
- feature entry `hint.builder.interiorView` — src/ui/i18n/pt-BR.ts:1453: `'hint.builder.interiorView': 'O prédio aparece cortado no andar escolhido: veja o saguão, as escadas, os elevadores'`
- feature entry `hint.builder.floorDown` — src/ui/i18n/pt-BR.ts:1454: `'hint.builder.floorDown': 'Mostra o andar de baixo'`
- feature entry `hint.builder.floorUp` — src/ui/i18n/pt-BR.ts:1455: `'hint.builder.floorUp': 'Mostra o andar de cima'`
- feature entry `hint.builder.coreLift` — src/ui/i18n/pt-BR.ts:1456: `'hint.builder.coreLift': 'Clique no piso: um elevador por todos os andares'`
- feature entry `hint.builder.coreStair` — src/ui/i18n/pt-BR.ts:1457: `'hint.builder.coreStair': 'Clique no piso: uma escada por todos os andares'`
- feature entry `hint.builder.coreBoth` — src/ui/i18n/pt-BR.ts:1458: `'hint.builder.coreBoth': 'Clique no piso: escada e elevador juntos'`
- feature entry `hint.builder.coreRemove` — src/ui/i18n/pt-BR.ts:1459: `'hint.builder.coreRemove': 'Clique numa escada ou elevador para tirá-lo'`
- feature entry `builder.section.primitives` — src/ui/i18n/pt-BR.ts:1460: `'builder.section.primitives': 'Formas básicas: clique e jogue no prédio'`
- feature entry `builder.tool.primBox` — src/ui/i18n/pt-BR.ts:1461: `'builder.tool.primBox': 'Caixa'`
- feature entry `builder.tool.primCylinder` — src/ui/i18n/pt-BR.ts:1462: `'builder.tool.primCylinder': 'Cilindro'`
- feature entry `builder.tool.primOctagonal` — src/ui/i18n/pt-BR.ts:1463: `'builder.tool.primOctagonal': 'Octógono'`
- feature entry `builder.tool.primPrism` — src/ui/i18n/pt-BR.ts:1464: `'builder.tool.primPrism': 'Prisma'`
- feature entry `builder.tool.primWedge` — src/ui/i18n/pt-BR.ts:1465: `'builder.tool.primWedge': 'Cunha'`
- feature entry `builder.tool.primPyramid` — src/ui/i18n/pt-BR.ts:1466: `'builder.tool.primPyramid': 'Pirâmide'`
- feature entry `builder.tool.primCone` — src/ui/i18n/pt-BR.ts:1467: `'builder.tool.primCone': 'Cone'`
- feature entry `builder.tool.primCross` — src/ui/i18n/pt-BR.ts:1468: `'builder.tool.primCross': 'Cruz'`
- feature entry `hint.builder.primBox` — src/ui/i18n/pt-BR.ts:1469: `'hint.builder.primBox': 'Clique num telhado, numa parede ou no chão para pôr a forma · Esc solta'`
- feature entry `hint.builder.primCylinder` — src/ui/i18n/pt-BR.ts:1470: `'hint.builder.primCylinder': 'Clique num telhado, numa parede ou no chão para pôr a forma · Esc solta'`
- feature entry `hint.builder.primOctagonal` — src/ui/i18n/pt-BR.ts:1471: `'hint.builder.primOctagonal': 'Clique num telhado, numa parede ou no chão para pôr a forma · Esc solta'`
- feature entry `hint.builder.primPrism` — src/ui/i18n/pt-BR.ts:1472: `'hint.builder.primPrism': 'Clique num telhado, numa parede ou no chão para pôr a forma · Esc solta'`
- feature entry `hint.builder.primWedge` — src/ui/i18n/pt-BR.ts:1473: `'hint.builder.primWedge': 'Clique num telhado, numa parede ou no chão para pôr a forma · Esc solta'`
- feature entry `hint.builder.primPyramid` — src/ui/i18n/pt-BR.ts:1474: `'hint.builder.primPyramid': 'Clique num telhado, numa parede ou no chão para pôr a forma · Esc solta'`
- feature entry `hint.builder.primCone` — src/ui/i18n/pt-BR.ts:1475: `'hint.builder.primCone': 'Clique num telhado, numa parede ou no chão para pôr a forma · Esc solta'`
- feature entry `hint.builder.primCross` — src/ui/i18n/pt-BR.ts:1476: `'hint.builder.primCross': 'Clique num telhado, numa parede ou no chão para pôr a forma · Esc solta'`
- feature entry `builder.section.modelling` — src/ui/i18n/pt-BR.ts:1477: `'builder.section.modelling': 'Modelagem'`
- feature entry `builder.tool.extrudeOut` — src/ui/i18n/pt-BR.ts:1478: `'builder.tool.extrudeOut': 'Extrudar +1 m'`
- feature entry `builder.tool.extrudeIn` — src/ui/i18n/pt-BR.ts:1479: `'builder.tool.extrudeIn': 'Extrudar −1 m'`
- feature entry `builder.tool.extrudeBlock` — src/ui/i18n/pt-BR.ts:1480: `'builder.tool.extrudeBlock': 'Extrudar bloco'`
- feature entry `builder.tool.insetFace` — src/ui/i18n/pt-BR.ts:1481: `'builder.tool.insetFace': 'Recuar face'`
- feature entry `builder.tool.offsetOut` — src/ui/i18n/pt-BR.ts:1482: `'builder.tool.offsetOut': 'Expandir +1 m'`
- feature entry `builder.tool.offsetIn` — src/ui/i18n/pt-BR.ts:1483: `'builder.tool.offsetIn': 'Contrair −1 m'`
- feature entry `builder.tool.bevelAll` — src/ui/i18n/pt-BR.ts:1484: `'builder.tool.bevelAll': 'Chanfrar cantos'`
- feature entry `builder.tool.pointMode` — src/ui/i18n/pt-BR.ts:1485: `'builder.tool.pointMode': 'Pontos'`
- feature entry `builder.tool.bevelCorner` — src/ui/i18n/pt-BR.ts:1486: `'builder.tool.bevelCorner': 'Chanfrar ponto'`
- feature entry `hint.builder.extrudeOut` — src/ui/i18n/pt-BR.ts:1487: `'hint.builder.extrudeOut': 'Empurra a face escolhida 1 m para fora'`
- feature entry `hint.builder.extrudeIn` — src/ui/i18n/pt-BR.ts:1488: `'hint.builder.extrudeIn': 'Puxa a face escolhida 1 m para dentro'`
- feature entry `hint.builder.extrudeBlock` — src/ui/i18n/pt-BR.ts:1489: `'hint.builder.extrudeBlock': 'A face escolhida cria um bloco novo encostado nela'`
- feature entry `hint.builder.insetFace` — src/ui/i18n/pt-BR.ts:1490: `'hint.builder.insetFace': 'Recua a face escolhida dentro de uma moldura'`
- feature entry `hint.builder.offsetOut` — src/ui/i18n/pt-BR.ts:1491: `'hint.builder.offsetOut': 'A planta do bloco cresce 1 m por todos os lados'`
- feature entry `hint.builder.offsetIn` — src/ui/i18n/pt-BR.ts:1492: `'hint.builder.offsetIn': 'A planta do bloco encolhe 1 m por todos os lados'`
- feature entry `hint.builder.bevelAll` — src/ui/i18n/pt-BR.ts:1493: `'hint.builder.bevelAll': 'Chanfra todos os cantos do bloco'`
- feature entry `hint.builder.pointMode` — src/ui/i18n/pt-BR.ts:1494: `'hint.builder.pointMode': 'Mostra os pontos do bloco: arraste para modelar'`
- feature entry `hint.builder.bevelCorner` — src/ui/i18n/pt-BR.ts:1495: `'hint.builder.bevelCorner': 'Chanfra o ponto escolhido'`
- feature entry `builder.pickFace` — src/ui/i18n/pt-BR.ts:1496: `'builder.pickFace': 'Clique de novo no bloco para escolher a face'`
- feature entry `builder.pickCorner` — src/ui/i18n/pt-BR.ts:1497: `'builder.pickCorner': 'Ative Pontos e clique num ponto do bloco'`
- feature entry `builder.section.boolean` — src/ui/i18n/pt-BR.ts:1498: `'builder.section.boolean': 'Operação booleana'`
- feature entry `builder.section.block` — src/ui/i18n/pt-BR.ts:1499: `'builder.section.block': 'Bloco'`
- feature entry `builder.section.drawWhere` — src/ui/i18n/pt-BR.ts:1500: `'builder.section.drawWhere': 'A forma desenhada vira'`
- feature entry `builder.section.drawWhere.short` — src/ui/i18n/pt-BR.ts:1501: `'builder.section.drawWhere.short': 'Resultado'`
- feature entry `builder.section.drawShape` — src/ui/i18n/pt-BR.ts:1502: `'builder.section.drawShape': 'Forma'`
- feature entry `builder.drawAction.new` — src/ui/i18n/pt-BR.ts:1503: `'builder.drawAction.new': 'Prédio novo'`
- feature entry `builder.drawAction.ground` — src/ui/i18n/pt-BR.ts:1504: `'builder.drawAction.ground': 'Bloco ao lado'`
- feature entry `builder.drawAction.top` — src/ui/i18n/pt-BR.ts:1505: `'builder.drawAction.top': 'Bloco em cima'`
- feature entry `builder.drawAction.cut` — src/ui/i18n/pt-BR.ts:1506: `'builder.drawAction.cut': 'Recorte'`
- feature entry `builder.tool.blockSolid` — src/ui/i18n/pt-BR.ts:1507: `'builder.tool.blockSolid': 'União'`
- feature entry `builder.tool.blockVoid` — src/ui/i18n/pt-BR.ts:1508: `'builder.tool.blockVoid': 'Subtração'`
- feature entry `builder.tool.blockIntersect` — src/ui/i18n/pt-BR.ts:1509: `'builder.tool.blockIntersect': 'Interseção'`
- feature entry `builder.tool.blockXor` — src/ui/i18n/pt-BR.ts:1510: `'builder.tool.blockXor': 'Exclusão'`
- feature entry `builder.tool.copyBlock` — src/ui/i18n/pt-BR.ts:1511: `'builder.tool.copyBlock': 'Copiar bloco'`
- feature entry `builder.tool.detachBlock` — src/ui/i18n/pt-BR.ts:1512: `'builder.tool.detachBlock': 'Desligar'`
- feature entry `builder.tool.centerBlock` — src/ui/i18n/pt-BR.ts:1513: `'builder.tool.centerBlock': 'Centralizar'`
- feature entry `builder.tool.turnBlockLeft` — src/ui/i18n/pt-BR.ts:1514: `'builder.tool.turnBlockLeft': 'Girar ⟲ 15°'`
- feature entry `builder.tool.turnBlockRight` — src/ui/i18n/pt-BR.ts:1515: `'builder.tool.turnBlockRight': 'Girar ⟳ 15°'`
- feature entry `hint.builder.blockSolid` — src/ui/i18n/pt-BR.ts:1516: `'hint.builder.blockSolid': 'O bloco soma o seu espaço ao prédio'`
- feature entry `hint.builder.blockVoid` — src/ui/i18n/pt-BR.ts:1517: `'hint.builder.blockVoid': 'O bloco tira o seu espaço dos outros, sem apagar nada'`
- feature entry `hint.builder.blockIntersect` — src/ui/i18n/pt-BR.ts:1518: `'hint.builder.blockIntersect': 'Dos outros blocos fica só o que está dentro deste'`
- feature entry `hint.builder.blockXor` — src/ui/i18n/pt-BR.ts:1519: `'hint.builder.blockXor': 'O bloco soma onde está sozinho e tira onde encontra outro'`
- feature entry `hint.builder.copyBlock` — src/ui/i18n/pt-BR.ts:1520: `'hint.builder.copyBlock': 'Uma cópia do bloco ao lado dele'`
- feature entry `hint.builder.detachBlock` — src/ui/i18n/pt-BR.ts:1521: `'hint.builder.detachBlock': 'O bloco vira um prédio separado'`
- feature entry `hint.builder.centerBlock` — src/ui/i18n/pt-BR.ts:1522: `'hint.builder.centerBlock': 'O bloco fica centrado sobre o de baixo'`
- feature entry `hint.builder.turnBlockLeft` — src/ui/i18n/pt-BR.ts:1523: `'hint.builder.turnBlockLeft': 'Gira o bloco sobre o seu centro'`
- feature entry `hint.builder.turnBlockRight` — src/ui/i18n/pt-BR.ts:1524: `'hint.builder.turnBlockRight': 'Gira o bloco sobre o seu centro'`
- feature entry `builder.field.offsetX` — src/ui/i18n/pt-BR.ts:1525: `'builder.field.offsetX': 'Posição X'`
- feature entry `builder.field.baseLevel` — src/ui/i18n/pt-BR.ts:1527: `'builder.field.baseLevel': 'Começa no andar'`
- feature entry `builder.field.lift` — src/ui/i18n/pt-BR.ts:1528: `'builder.field.lift': 'Cota do bloco'`
- feature entry `builder.field.blockMode` — src/ui/i18n/pt-BR.ts:1529: `'builder.field.blockMode': 'Operação'`
- feature entry `builder.blockMode.solid` — src/ui/i18n/pt-BR.ts:1530: `'builder.blockMode.solid': 'União'`
- feature entry `builder.blockMode.void` — src/ui/i18n/pt-BR.ts:1531: `'builder.blockMode.void': 'Subtração'`
- feature entry `builder.blockMode.intersect` — src/ui/i18n/pt-BR.ts:1532: `'builder.blockMode.intersect': 'Interseção'`
- feature entry `builder.blockMode.xor` — src/ui/i18n/pt-BR.ts:1533: `'builder.blockMode.xor': 'Exclusão'`
- feature entry `builder.tool.shapeCross` — src/ui/i18n/pt-BR.ts:1534: `'builder.tool.shapeCross': 'Cruz'`
- feature entry `builder.tool.shapeStepped` — src/ui/i18n/pt-BR.ts:1535: `'builder.tool.shapeStepped': 'Cruz escalonada'`
- feature entry `builder.tool.tierCross` — src/ui/i18n/pt-BR.ts:1536: `'builder.tool.tierCross': 'Cruz'`
- feature entry `builder.tool.tierStepped` — src/ui/i18n/pt-BR.ts:1537: `'builder.tool.tierStepped': 'Cruz escalonada'`
- feature entry `hint.builder.shapeCross` — src/ui/i18n/pt-BR.ts:1538: `'hint.builder.shapeCross': 'Arraste no terreno: uma planta em cruz'`
- feature entry `hint.builder.shapeStepped` — src/ui/i18n/pt-BR.ts:1539: `'hint.builder.shapeStepped': 'Arraste no terreno: uma planta em cruz com cantos escalonados'`
- feature entry `hint.builder.tierCross` — src/ui/i18n/pt-BR.ts:1540: `'hint.builder.tierCross': 'O bloco selecionado vira uma cruz'`
- feature entry `hint.builder.tierStepped` — src/ui/i18n/pt-BR.ts:1541: `'hint.builder.tierStepped': 'O bloco selecionado vira uma cruz de cantos escalonados'`
- feature entry `builder.section.blockShape` — src/ui/i18n/pt-BR.ts:1542: `'builder.section.blockShape': 'Forma do bloco'`
- feature entry `builder.tool.tierRect` — src/ui/i18n/pt-BR.ts:1543: `'builder.tool.tierRect': 'Retângulo'`
- feature entry `builder.tool.tierChamfered` — src/ui/i18n/pt-BR.ts:1544: `'builder.tool.tierChamfered': 'Chanfrado'`
- feature entry `builder.tool.tierOctagon` — src/ui/i18n/pt-BR.ts:1545: `'builder.tool.tierOctagon': 'Octógono'`
- feature entry `builder.tool.tierHexagon` — src/ui/i18n/pt-BR.ts:1546: `'builder.tool.tierHexagon': 'Hexágono'`
- feature entry `builder.tool.tierCircle` — src/ui/i18n/pt-BR.ts:1547: `'builder.tool.tierCircle': 'Cilindro'`
- feature entry `builder.tool.tierL` — src/ui/i18n/pt-BR.ts:1548: `'builder.tool.tierL': 'Em L'`
- feature entry `builder.tool.tierU` — src/ui/i18n/pt-BR.ts:1549: `'builder.tool.tierU': 'Em U'`
- feature entry `hint.builder.tierRect` — src/ui/i18n/pt-BR.ts:1550: `'hint.builder.tierRect': 'O bloco selecionado vira um retângulo'`
- feature entry `hint.builder.tierChamfered` — src/ui/i18n/pt-BR.ts:1551: `'hint.builder.tierChamfered': 'O bloco selecionado ganha cantos chanfrados'`
- feature entry `hint.builder.tierOctagon` — src/ui/i18n/pt-BR.ts:1552: `'hint.builder.tierOctagon': 'O bloco selecionado vira um octógono'`
- feature entry `hint.builder.tierHexagon` — src/ui/i18n/pt-BR.ts:1553: `'hint.builder.tierHexagon': 'O bloco selecionado vira um hexágono'`
- feature entry `hint.builder.tierCircle` — src/ui/i18n/pt-BR.ts:1554: `'hint.builder.tierCircle': 'O bloco selecionado vira um cilindro'`
- feature entry `hint.builder.tierL` — src/ui/i18n/pt-BR.ts:1555: `'hint.builder.tierL': 'O bloco selecionado vira um L'`
- feature entry `hint.builder.tierU` — src/ui/i18n/pt-BR.ts:1556: `'hint.builder.tierU': 'O bloco selecionado vira um U'`
- shared entry `builder.category.models` — src/ui/i18n/pt-BR.ts:1557: `'builder.category.models': 'Modelos'`
- feature entry `builder.category.facade` — src/ui/i18n/pt-BR.ts:1558: `'builder.category.facade': 'Fachada'`
- feature entry `builder.category.parts` — src/ui/i18n/pt-BR.ts:1559: `'builder.category.parts': 'Peças'`
- feature entry `builder.category.paint` — src/ui/i18n/pt-BR.ts:1560: `'builder.category.paint': 'Cores'`
- feature entry `builder.section.floors` — src/ui/i18n/pt-BR.ts:1561: `'builder.section.floors': 'Pavimentos'`
- feature entry `builder.section.reference` — src/ui/i18n/pt-BR.ts:1562: `'builder.section.reference': 'Referência 3D'`
- feature entry `builder.tool.refLoad` — src/ui/i18n/pt-BR.ts:1563: `'builder.tool.refLoad': 'Carregar modelo'`
- feature entry `hint.builder.refLoad` — src/ui/i18n/pt-BR.ts:1564: `'hint.builder.refLoad': 'Escolha um modelo 3D (.glb ou .gltf) de um prédio real: ele aparece transparente sobre o prédio selecionado, para construir por cima'`
- feature entry `builder.tool.refAlign` — src/ui/i18n/pt-BR.ts:1565: `'builder.tool.refAlign': 'Alinhar ao prédio'`
- feature entry `hint.builder.refAlign` — src/ui/i18n/pt-BR.ts:1566: `'hint.builder.refAlign': 'Põe o modelo de referência sobre o prédio selecionado, com o pé no chão dele'`
- feature entry `builder.tool.refTurn` — src/ui/i18n/pt-BR.ts:1567: `'builder.tool.refTurn': 'Girar 90°'`
- feature entry `hint.builder.refTurn` — src/ui/i18n/pt-BR.ts:1568: `'hint.builder.refTurn': 'Gira o modelo de referência um quarto de volta'`
- feature entry `builder.tool.refFlip` — src/ui/i18n/pt-BR.ts:1569: `'builder.tool.refFlip': 'Espelhar'`
- feature entry `hint.builder.refFlip` — src/ui/i18n/pt-BR.ts:1570: `'hint.builder.refFlip': 'Espelha o modelo de referência de frente para fundos'`
- feature entry `builder.tool.refFainter` — src/ui/i18n/pt-BR.ts:1571: `'builder.tool.refFainter': 'Mais transparente'`
- feature entry `hint.builder.refFainter` — src/ui/i18n/pt-BR.ts:1572: `'hint.builder.refFainter': 'Deixa o modelo de referência mais transparente'`
- feature entry `builder.tool.refStronger` — src/ui/i18n/pt-BR.ts:1573: `'builder.tool.refStronger': 'Menos transparente'`
- feature entry `hint.builder.refStronger` — src/ui/i18n/pt-BR.ts:1574: `'hint.builder.refStronger': 'Deixa o modelo de referência mais sólido'`
- feature entry `builder.tool.refToggle` — src/ui/i18n/pt-BR.ts:1575: `'builder.tool.refToggle': 'Mostrar / ocultar'`
- feature entry `hint.builder.refToggle` — src/ui/i18n/pt-BR.ts:1576: `'hint.builder.refToggle': 'Mostra ou esconde o modelo de referência'`
- feature entry `builder.tool.refRemove` — src/ui/i18n/pt-BR.ts:1577: `'builder.tool.refRemove': 'Remover referência'`
- feature entry `hint.builder.refRemove` — src/ui/i18n/pt-BR.ts:1578: `'hint.builder.refRemove': 'Tira o modelo de referência da cena'`
- feature entry `hint.reference.loaded` — src/ui/i18n/pt-BR.ts:1579: `'hint.reference.loaded': 'Referência carregada: {name} — {w} × {d} m, {h} m de altura'`
- feature entry `hint.reference.failed` — src/ui/i18n/pt-BR.ts:1580: `'hint.reference.failed': 'Não foi possível abrir esse modelo (use .glb ou .gltf)'`
- feature entry `builder.tool.refBuild` — src/ui/i18n/pt-BR.ts:1581: `'builder.tool.refBuild': 'Construir a partir da referência'`
- feature entry `hint.builder.refBuild` — src/ui/i18n/pt-BR.ts:1582: `'hint.builder.refBuild': 'Lê o modelo de referência - blocos, planta, cores, janelas, base de pedra e farol - e constrói o prédio com as peças do jogo, todas ed`
- feature entry `hint.reference.built` — src/ui/i18n/pt-BR.ts:1583: `'hint.reference.built': 'Prédio construído a partir da referência: {volumes} blocos'`
- feature entry `hint.reference.empty` — src/ui/i18n/pt-BR.ts:1584: `'hint.reference.empty': 'Não achei blocos para construir nesse modelo'`
- feature entry `hint.reference.none` — src/ui/i18n/pt-BR.ts:1585: `'hint.reference.none': 'Carregue um modelo de referência primeiro'`
- feature entry `builder.section.volumes` — src/ui/i18n/pt-BR.ts:1586: `'builder.section.volumes': 'Blocos'`
- feature entry `builder.section.plan` — src/ui/i18n/pt-BR.ts:1587: `'builder.section.plan': 'Cantos da planta'`
- feature entry `builder.section.pattern` — src/ui/i18n/pt-BR.ts:1588: `'builder.section.pattern': 'Padrão de fachada'`
- feature entry `builder.section.windows` — src/ui/i18n/pt-BR.ts:1589: `'builder.section.windows': 'Janelas'`
- feature entry `builder.section.doors` — src/ui/i18n/pt-BR.ts:1590: `'builder.section.doors': 'Portas'`
- feature entry `builder.section.bays` — src/ui/i18n/pt-BR.ts:1591: `'builder.section.bays': 'Vãos'`
- feature entry `builder.section.scope` — src/ui/i18n/pt-BR.ts:1592: `'builder.section.scope': 'Aplicar em'`
- feature entry `builder.section.relief` — src/ui/i18n/pt-BR.ts:1593: `'builder.section.relief': 'Relevo do vão'`
- feature entry `builder.section.structure` — src/ui/i18n/pt-BR.ts:1594: `'builder.section.structure': 'Estrutura'`
- feature entry `builder.section.runs` — src/ui/i18n/pt-BR.ts:1595: `'builder.section.runs': 'Traçados'`
- feature entry `builder.section.greenery` — src/ui/i18n/pt-BR.ts:1596: `'builder.section.greenery': 'Verde'`
- feature entry `builder.section.furniture` — src/ui/i18n/pt-BR.ts:1597: `'builder.section.furniture': 'Mobiliário'`
- feature entry `builder.section.roofGear` — src/ui/i18n/pt-BR.ts:1598: `'builder.section.roofGear': 'No telhado'`
- feature entry `builder.section.roofShape` — src/ui/i18n/pt-BR.ts:1599: `'builder.section.roofShape': 'Inclinação'`
- feature entry `builder.section.styles` — src/ui/i18n/pt-BR.ts:1600: `'builder.section.styles': 'Estilos'`
- feature entry `builder.needsSelection` — src/ui/i18n/pt-BR.ts:1601: `'builder.needsSelection': 'Selecione um edifício para usar'`
- feature entry `builder.quick.save` — src/ui/i18n/pt-BR.ts:1602: `'builder.quick.save': 'Salvar como modelo'`
- feature entry `builder.category.roof` — src/ui/i18n/pt-BR.ts:1607: `'builder.category.roof': 'Telhado'`
- feature entry `builder.tool.rect` — src/ui/i18n/pt-BR.ts:1611: `'builder.tool.rect': 'Retângulo'`
- feature entry `builder.tool.shapeL` — src/ui/i18n/pt-BR.ts:1612: `'builder.tool.shapeL': 'L'`
- feature entry `builder.tool.shapeU` — src/ui/i18n/pt-BR.ts:1613: `'builder.tool.shapeU': 'U'`
- feature entry `builder.tool.circle` — src/ui/i18n/pt-BR.ts:1614: `'builder.tool.circle': 'Círculo'`
- feature entry `builder.tool.hexagon` — src/ui/i18n/pt-BR.ts:1615: `'builder.tool.hexagon': 'Hexágono'`
- feature entry `builder.tool.octagon` — src/ui/i18n/pt-BR.ts:1616: `'builder.tool.octagon': 'Octógono'`
- feature entry `builder.tool.chamfered` — src/ui/i18n/pt-BR.ts:1617: `'builder.tool.chamfered': 'Chanfrado'`
- feature entry `builder.tool.sketch` — src/ui/i18n/pt-BR.ts:1618: `'builder.tool.sketch': 'Planta livre'`
- feature entry `builder.tool.storey` — src/ui/i18n/pt-BR.ts:1620: `'builder.tool.storey': '+ Pavimento'`
- feature entry `builder.tool.storeyDown` — src/ui/i18n/pt-BR.ts:1621: `'builder.tool.storeyDown': '− Pavimento'`
- feature entry `builder.tool.wing` — src/ui/i18n/pt-BR.ts:1622: `'builder.tool.wing': 'Ala'`
- feature entry `builder.tool.stack` — src/ui/i18n/pt-BR.ts:1623: `'builder.tool.stack': 'Empilhar'`
- feature entry `builder.tool.cut` — src/ui/i18n/pt-BR.ts:1624: `'builder.tool.cut': 'Recortar'`
- feature entry `builder.tool.split` — src/ui/i18n/pt-BR.ts:1625: `'builder.tool.split': 'Dividir'`
- feature entry `builder.tool.setback` — src/ui/i18n/pt-BR.ts:1626: `'builder.tool.setback': 'Andar recuado'`
- feature entry `builder.tool.vertexAdd` — src/ui/i18n/pt-BR.ts:1627: `'builder.tool.vertexAdd': 'Inserir ponto'`
- feature entry `builder.tool.vertexRemove` — src/ui/i18n/pt-BR.ts:1628: `'builder.tool.vertexRemove': 'Remover ponto'`
- feature entry `builder.tool.inset` — src/ui/i18n/pt-BR.ts:1630: `'builder.tool.inset': 'Recuar'`
- feature entry `builder.tool.outset` — src/ui/i18n/pt-BR.ts:1631: `'builder.tool.outset': 'Avançar'`
- feature entry `builder.tool.flush` — src/ui/i18n/pt-BR.ts:1632: `'builder.tool.flush': 'Alinhar'`
- feature entry `builder.tool.window` — src/ui/i18n/pt-BR.ts:1635: `'builder.tool.window': 'Janela'`
- feature entry `builder.tool.sashWindow` — src/ui/i18n/pt-BR.ts:1636: `'builder.tool.sashWindow': 'Janela de guilhotina'`
- feature entry `builder.tool.wideWindow` — src/ui/i18n/pt-BR.ts:1637: `'builder.tool.wideWindow': 'Janela ampla'`
- feature entry `builder.tool.frenchWindow` — src/ui/i18n/pt-BR.ts:1638: `'builder.tool.frenchWindow': 'Porta-janela'`
- feature entry `builder.tool.bayWindow` — src/ui/i18n/pt-BR.ts:1639: `'builder.tool.bayWindow': 'Janela saliente'`
- feature entry `builder.tool.ribbon` — src/ui/i18n/pt-BR.ts:1640: `'builder.tool.ribbon': 'Janela fita'`
- feature entry `builder.tool.balcony` — src/ui/i18n/pt-BR.ts:1641: `'builder.tool.balcony': 'Varanda'`
- feature entry `builder.tool.door` — src/ui/i18n/pt-BR.ts:1642: `'builder.tool.door': 'Porta'`
- feature entry `builder.tool.shopfront` — src/ui/i18n/pt-BR.ts:1643: `'builder.tool.shopfront': 'Vitrine'`
- feature entry `builder.tool.loadingDoor` — src/ui/i18n/pt-BR.ts:1644: `'builder.tool.loadingDoor': 'Porta de carga'`
- feature entry `builder.tool.pillarBay` — src/ui/i18n/pt-BR.ts:1645: `'builder.tool.pillarBay': 'Arcada'`
- feature entry `builder.tool.wallBay` — src/ui/i18n/pt-BR.ts:1646: `'builder.tool.wallBay': 'Parede'`
- feature entry `builder.tool.stair` — src/ui/i18n/pt-BR.ts:1647: `'builder.tool.stair': 'Escada'`
- feature entry `builder.tool.ramp` — src/ui/i18n/pt-BR.ts:1648: `'builder.tool.ramp': 'Rampa'`
- feature entry `builder.tool.pillar` — src/ui/i18n/pt-BR.ts:1649: `'builder.tool.pillar': 'Pilar'`
- feature entry `builder.tool.canopy` — src/ui/i18n/pt-BR.ts:1650: `'builder.tool.canopy': 'Marquise'`
- feature entry `builder.tool.wall` — src/ui/i18n/pt-BR.ts:1651: `'builder.tool.wall': 'Muro'`
- feature entry `builder.tool.slab` — src/ui/i18n/pt-BR.ts:1652: `'builder.tool.slab': 'Laje'`
- feature entry `builder.tool.moveMass` — src/ui/i18n/pt-BR.ts:1654: `'builder.tool.moveMass': 'Mover volume'`
- feature entry `builder.tool.wallRun` — src/ui/i18n/pt-BR.ts:1655: `'builder.tool.wallRun': 'Muro (traçado)'`
- feature entry `builder.tool.fenceRun` — src/ui/i18n/pt-BR.ts:1656: `'builder.tool.fenceRun': 'Cerca'`
- feature entry `builder.tool.pavementRun` — src/ui/i18n/pt-BR.ts:1657: `'builder.tool.pavementRun': 'Calçada'`
- feature entry `builder.tool.tree` — src/ui/i18n/pt-BR.ts:1658: `'builder.tool.tree': 'Árvore'`
- feature entry `builder.tool.bench` — src/ui/i18n/pt-BR.ts:1659: `'builder.tool.bench': 'Banco'`
- feature entry `builder.tool.planter` — src/ui/i18n/pt-BR.ts:1660: `'builder.tool.planter': 'Jardineira'`
- feature entry `builder.tool.ac` — src/ui/i18n/pt-BR.ts:1661: `'builder.tool.ac': 'Ar-condicionado'`
- feature entry `builder.tool.roofFlat` — src/ui/i18n/pt-BR.ts:1662: `'builder.tool.roofFlat': 'Plana'`
- feature entry `builder.tool.roofTerrace` — src/ui/i18n/pt-BR.ts:1663: `'builder.tool.roofTerrace': 'Terraço'`
- feature entry `builder.tool.roofGable` — src/ui/i18n/pt-BR.ts:1664: `'builder.tool.roofGable': '2 águas'`
- feature entry `builder.tool.roofHip` — src/ui/i18n/pt-BR.ts:1665: `'builder.tool.roofHip': '4 águas'`
- feature entry `builder.tool.roofShed` — src/ui/i18n/pt-BR.ts:1666: `'builder.tool.roofShed': '1 água'`
- feature entry `builder.tool.roofSawtooth` — src/ui/i18n/pt-BR.ts:1667: `'builder.tool.roofSawtooth': 'Dente de serra'`
- feature entry `builder.tool.solar` — src/ui/i18n/pt-BR.ts:1669: `'builder.tool.solar': 'Painéis solares'`
- feature entry `builder.tool.skylight` — src/ui/i18n/pt-BR.ts:1670: `'builder.tool.skylight': 'Clarabóia'`
- feature entry `builder.tool.vent` — src/ui/i18n/pt-BR.ts:1671: `'builder.tool.vent': 'Ventilação'`
- feature entry `builder.tool.chimney` — src/ui/i18n/pt-BR.ts:1672: `'builder.tool.chimney': 'Chaminé'`
- feature entry `builder.tool.waterTank` — src/ui/i18n/pt-BR.ts:1673: `'builder.tool.waterTank': 'Caixa d’água'`
- feature entry `builder.tool.spire` — src/ui/i18n/pt-BR.ts:1674: `'builder.tool.spire': 'Agulha'`
- feature entry `builder.tool.lantern` — src/ui/i18n/pt-BR.ts:1675: `'builder.tool.lantern': 'Farol'`
- feature entry `builder.tool.paint` — src/ui/i18n/pt-BR.ts:1677: `'builder.tool.paint': 'Pintar'`
- feature entry `builder.tool.copyStyle` — src/ui/i18n/pt-BR.ts:1680: `'builder.tool.copyStyle': 'Copiar estilo'`
- shared entry `hint.builder.select` — src/ui/i18n/pt-BR.ts:1681: `'hint.builder.select': 'Clique num prédio: seleciona · de novo: a face · arraste: move'`
- feature entry `hint.builder.place` — src/ui/i18n/pt-BR.ts:1682: `'hint.builder.place': 'Clique no mapa para colocar o prédio · R gira · Esc solta'`
- shared entry `hint.builder.draw.new` — src/ui/i18n/pt-BR.ts:1684: `'hint.builder.draw.new': 'Clique em cada canto · Enter fecha a planta'`
- shared entry `hint.builder.draw.ground` — src/ui/i18n/pt-BR.ts:1685: `'hint.builder.draw.ground': 'Clique no terreno: uma ala cresce contra a parede que ela encontra'`
- shared entry `hint.builder.draw.top` — src/ui/i18n/pt-BR.ts:1686: `'hint.builder.draw.top': 'Clique no edifício: um volume se apoia no que está embaixo'`
- shared entry `hint.builder.draw.cut` — src/ui/i18n/pt-BR.ts:1687: `'hint.builder.draw.cut': 'Desenhe um retângulo sobre um volume para recortá-lo'`
- shared entry `hint.builder.sketch` — src/ui/i18n/pt-BR.ts:1688: `'hint.builder.sketch': 'Clique em cada canto · Enter fecha a planta'`
- shared entry `hint.builder.paint` — src/ui/i18n/pt-BR.ts:1695: `'hint.builder.paint': 'Clique nas faces para pintá-las com o pincel'`
- shared entry `hint.builder.stair` — src/ui/i18n/pt-BR.ts:1697: `'hint.builder.stair': 'Aponte para uma fachada e clique: a escada desce daquele pavimento'`
- shared entry `hint.builder.ramp` — src/ui/i18n/pt-BR.ts:1698: `'hint.builder.ramp': 'Aponte para uma fachada e clique: a rampa desce daquele pavimento'`
- shared entry `hint.builder.pillar` — src/ui/i18n/pt-BR.ts:1699: `'hint.builder.pillar': 'Aponte para uma fachada e clique: o pilar sobe entre os pavimentos'`
- shared entry `hint.builder.canopy` — src/ui/i18n/pt-BR.ts:1700: `'hint.builder.canopy': 'Aponte para uma fachada e clique'`
- shared entry `hint.builder.wall` — src/ui/i18n/pt-BR.ts:1701: `'hint.builder.wall': 'Aponte para uma fachada ou o terreno e clique'`
- shared entry `hint.builder.slab` — src/ui/i18n/pt-BR.ts:1702: `'hint.builder.slab': 'Aponte para uma fachada ou o terreno e clique'`
- feature entry `hint.builder.moveMass` — src/ui/i18n/pt-BR.ts:1704: `'hint.builder.moveMass': 'Arraste o volume selecionado: ele desliza na própria planta e solda onde parar'`
- feature entry `builder.runPlaced` — src/ui/i18n/pt-BR.ts:1705: `'builder.runPlaced': 'Trecho lançado ao longo do traçado'`
- feature entry `builder.welded` — src/ui/i18n/pt-BR.ts:1706: `'builder.welded': 'Soldado num só edifício'`
- feature entry `hint.builder.wallRun` — src/ui/i18n/pt-BR.ts:1707: `'hint.builder.wallRun': 'Clique em cada canto do traçado · Enter lança o muro ao longo dele'`
- feature entry `hint.builder.fenceRun` — src/ui/i18n/pt-BR.ts:1708: `'hint.builder.fenceRun': 'Clique em cada canto do traçado · Enter lança a cerca ao longo dele'`
- feature entry `hint.builder.pavementRun` — src/ui/i18n/pt-BR.ts:1709: `'hint.builder.pavementRun': 'Clique em cada canto · Enter lança a calçada ao longo do traçado'`
- shared entry `hint.builder.tree` — src/ui/i18n/pt-BR.ts:1714: `'hint.builder.tree': 'Clique no terreno para plantar uma árvore'`
- shared entry `hint.builder.bench` — src/ui/i18n/pt-BR.ts:1715: `'hint.builder.bench': 'Aponte para uma fachada e clique: o banco fica ao lado'`
- shared entry `hint.builder.planter` — src/ui/i18n/pt-BR.ts:1716: `'hint.builder.planter': 'Aponte para uma fachada e clique'`
- shared entry `hint.builder.ac` — src/ui/i18n/pt-BR.ts:1717: `'hint.builder.ac': 'Aponte para uma fachada e clique: o aparelho fica na parede'`
- shared entry `hint.builder.solar` — src/ui/i18n/pt-BR.ts:1718: `'hint.builder.solar': 'Clique num telhado para colocar os painéis'`
- shared entry `hint.builder.skylight` — src/ui/i18n/pt-BR.ts:1719: `'hint.builder.skylight': 'Clique num telhado para colocar a clarabóia'`
- shared entry `hint.builder.vent` — src/ui/i18n/pt-BR.ts:1720: `'hint.builder.vent': 'Clique num telhado para colocar a ventilação'`
- shared entry `hint.builder.chimney` — src/ui/i18n/pt-BR.ts:1721: `'hint.builder.chimney': 'Clique num telhado para colocar a chaminé'`
- shared entry `hint.builder.waterTank` — src/ui/i18n/pt-BR.ts:1722: `'hint.builder.waterTank': 'Clique num telhado para colocar a caixa d’água'`
- shared entry `hint.builder.spire` — src/ui/i18n/pt-BR.ts:1723: `'hint.builder.spire': 'Clique num telhado para erguer uma agulha'`
- shared entry `hint.builder.lantern` — src/ui/i18n/pt-BR.ts:1724: `'hint.builder.lantern': 'Clique no topo de uma torre para erguer um farol com mastro e bandeira'`
- feature entry `hint.builder.storey` — src/ui/i18n/pt-BR.ts:1725: `'hint.builder.storey': 'Soma um pavimento ao volume selecionado'`
- feature entry `hint.builder.storeyDown` — src/ui/i18n/pt-BR.ts:1726: `'hint.builder.storeyDown': 'Tira um pavimento do volume selecionado'`
- feature entry `hint.builder.wing` — src/ui/i18n/pt-BR.ts:1727: `'hint.builder.wing': 'Arraste um retângulo no terreno: um volume cresce contra a parede que ele encontra'`
- feature entry `hint.builder.stack` — src/ui/i18n/pt-BR.ts:1728: `'hint.builder.stack': 'Arraste sobre o edifício: um volume se apoia no que está embaixo'`
- feature entry `hint.builder.cut` — src/ui/i18n/pt-BR.ts:1729: `'hint.builder.cut': 'Arraste um retângulo sobre um volume para recortá-lo'`
- feature entry `hint.builder.split` — src/ui/i18n/pt-BR.ts:1730: `'hint.builder.split': 'Divide o volume selecionado em dois, no pavimento acima'`
- feature entry `hint.builder.setback` — src/ui/i18n/pt-BR.ts:1731: `'hint.builder.setback': 'Empilha um volume recuado sobre o selecionado'`
- feature entry `hint.builder.vertexAdd` — src/ui/i18n/pt-BR.ts:1732: `'hint.builder.vertexAdd': 'Clique numa aresta da planta para inserir um ponto'`
- feature entry `hint.builder.vertexRemove` — src/ui/i18n/pt-BR.ts:1733: `'hint.builder.vertexRemove': 'Clique num canto da planta para removê-lo'`
- feature entry `hint.builder.inset` — src/ui/i18n/pt-BR.ts:1734: `'hint.builder.inset': 'Clique numa face: a borda recua e emoldura o meio'`
- feature entry `hint.builder.outset` — src/ui/i18n/pt-BR.ts:1735: `'hint.builder.outset': 'Clique numa face: a região sai para fora'`
- feature entry `hint.builder.flush` — src/ui/i18n/pt-BR.ts:1736: `'hint.builder.flush': 'Clique numa face: a região volta para a parede'`
- feature entry `hint.builder.copyStyle` — src/ui/i18n/pt-BR.ts:1743: `'hint.builder.copyStyle': 'Guarda o material do que está selecionado, para o pincel'`
- feature entry `hint.builder.window` — src/ui/i18n/pt-BR.ts:1745: `'hint.builder.window': 'Clique nos vãos da fachada para abrir janelas'`
- feature entry `hint.builder.sashWindow` — src/ui/i18n/pt-BR.ts:1746: `'hint.builder.sashWindow': 'Clique nos vãos para janelas de guilhotina'`
- feature entry `hint.builder.wideWindow` — src/ui/i18n/pt-BR.ts:1747: `'hint.builder.wideWindow': 'Clique nos vãos para janelas amplas'`
- feature entry `hint.builder.frenchWindow` — src/ui/i18n/pt-BR.ts:1748: `'hint.builder.frenchWindow': 'Clique nos vãos para portas-janela do piso ao teto'`
- feature entry `hint.builder.bayWindow` — src/ui/i18n/pt-BR.ts:1749: `'hint.builder.bayWindow': 'Clique nos vãos para janelas salientes'`
- feature entry `hint.builder.ribbon` — src/ui/i18n/pt-BR.ts:1750: `'hint.builder.ribbon': 'Clique nos vãos para uma fita de vidro contínua'`
- feature entry `hint.builder.balcony` — src/ui/i18n/pt-BR.ts:1751: `'hint.builder.balcony': 'Clique nos vãos para varandas'`
- feature entry `hint.builder.door` — src/ui/i18n/pt-BR.ts:1752: `'hint.builder.door': 'Clique nos vãos para portas'`
- feature entry `hint.builder.shopfront` — src/ui/i18n/pt-BR.ts:1753: `'hint.builder.shopfront': 'Clique nos vãos para vitrines'`
- feature entry `hint.builder.loadingDoor` — src/ui/i18n/pt-BR.ts:1754: `'hint.builder.loadingDoor': 'Clique nos vãos para portas de carga'`
- feature entry `hint.builder.pillarBay` — src/ui/i18n/pt-BR.ts:1755: `'hint.builder.pillarBay': 'Clique nos vãos para uma arcada aberta'`
- feature entry `hint.builder.wallBay` — src/ui/i18n/pt-BR.ts:1756: `'hint.builder.wallBay': 'Clique nos vãos para fechá-los em parede lisa'`
- feature entry `hint.builder.roofFlat` — src/ui/i18n/pt-BR.ts:1757: `'hint.builder.roofFlat': 'Cobertura plana sobre o volume selecionado'`
- feature entry `hint.builder.roofTerrace` — src/ui/i18n/pt-BR.ts:1758: `'hint.builder.roofTerrace': 'Terraço: plana, com platibanda'`
- feature entry `hint.builder.roofGable` — src/ui/i18n/pt-BR.ts:1759: `'hint.builder.roofGable': 'Duas águas sobre uma cumeeira'`
- feature entry `hint.builder.roofHip` — src/ui/i18n/pt-BR.ts:1760: `'hint.builder.roofHip': 'Quatro águas sobre uma cumeeira'`
- feature entry `hint.builder.roofShed` — src/ui/i18n/pt-BR.ts:1761: `'hint.builder.roofShed': 'Uma água, caindo para a frente'`
- feature entry `hint.builder.roofSawtooth` — src/ui/i18n/pt-BR.ts:1762: `'hint.builder.roofSawtooth': 'Dente de serra: galpões com luz norte'`
- feature entry `hint.builder.rect` — src/ui/i18n/pt-BR.ts:1763: `'hint.builder.rect': 'Arraste um retângulo no terreno: a planta o segue'`
- feature entry `hint.builder.shapeL` — src/ui/i18n/pt-BR.ts:1764: `'hint.builder.shapeL': 'Arraste um retângulo: a planta vira um L'`
- feature entry `hint.builder.shapeU` — src/ui/i18n/pt-BR.ts:1765: `'hint.builder.shapeU': 'Arraste um retângulo: a planta vira um U'`
- feature entry `hint.builder.circle` — src/ui/i18n/pt-BR.ts:1766: `'hint.builder.circle': 'Arraste um retângulo: a planta vira um círculo'`
- feature entry `hint.builder.hexagon` — src/ui/i18n/pt-BR.ts:1767: `'hint.builder.hexagon': 'Arraste um retângulo: a planta vira um hexágono'`
- feature entry `hint.builder.octagon` — src/ui/i18n/pt-BR.ts:1768: `'hint.builder.octagon': 'Arraste um retângulo: a planta vira um octógono'`
- feature entry `hint.builder.chamfered` — src/ui/i18n/pt-BR.ts:1769: `'hint.builder.chamfered': 'Arraste um retângulo: a planta sai chanfrada'`
- feature entry `builder.tool.railing` — src/ui/i18n/pt-BR.ts:1770: `'builder.tool.railing': 'Corrimão'`
- feature entry `builder.tool.awning` — src/ui/i18n/pt-BR.ts:1771: `'builder.tool.awning': 'Toldo'`
- feature entry `builder.tool.flowers` — src/ui/i18n/pt-BR.ts:1772: `'builder.tool.flowers': 'Flores'`
- feature entry `builder.tool.rocks` — src/ui/i18n/pt-BR.ts:1773: `'builder.tool.rocks': 'Pedras'`
- feature entry `builder.tool.shrub` — src/ui/i18n/pt-BR.ts:1774: `'builder.tool.shrub': 'Arbusto'`
- feature entry `builder.tool.hedge` — src/ui/i18n/pt-BR.ts:1775: `'builder.tool.hedge': 'Sebe'`
- feature entry `builder.tool.parking` — src/ui/i18n/pt-BR.ts:1776: `'builder.tool.parking': 'Estacionamento'`
- shared entry `hint.builder.railing` — src/ui/i18n/pt-BR.ts:1777: `'hint.builder.railing': 'Aponte para uma fachada e clique: o corrimão corre ao longo dela'`
- shared entry `hint.builder.awning` — src/ui/i18n/pt-BR.ts:1778: `'hint.builder.awning': 'Aponte para uma fachada e clique: um toldo cobre o vão'`
- shared entry `hint.builder.flowers` — src/ui/i18n/pt-BR.ts:1779: `'hint.builder.flowers': 'Clique no terreno para plantar um canteiro'`
- shared entry `hint.builder.shrub` — src/ui/i18n/pt-BR.ts:1780: `'hint.builder.shrub': 'Clique no terreno para plantar um arbusto'`
- shared entry `hint.builder.hedge` — src/ui/i18n/pt-BR.ts:1781: `'hint.builder.hedge': 'Aponte para uma fachada e clique: uma sebe corre diante dela'`
- shared entry `hint.builder.rocks` — src/ui/i18n/pt-BR.ts:1782: `'hint.builder.rocks': 'Clique no terreno para formar um grupo de pedras'`
- shared entry `hint.builder.parking` — src/ui/i18n/pt-BR.ts:1783: `'hint.builder.parking': 'Aponte para uma fachada e clique: um estacionamento com vagas'`
- feature entry `building.finish.stucco` — src/ui/i18n/pt-BR.ts:1784: `'building.finish.stucco': 'Estuque'`
- feature entry `building.finish.panel` — src/ui/i18n/pt-BR.ts:1785: `'building.finish.panel': 'Painel'`
- feature entry `building.finish.slate` — src/ui/i18n/pt-BR.ts:1786: `'building.finish.slate': 'Ardósia'`
- feature entry `builder.tool.doubleDoor` — src/ui/i18n/pt-BR.ts:1787: `'builder.tool.doubleDoor': 'Porta dupla'`
- feature entry `builder.tool.garageDoor` — src/ui/i18n/pt-BR.ts:1788: `'builder.tool.garageDoor': 'Porta de garagem'`
- feature entry `hint.builder.doubleDoor` — src/ui/i18n/pt-BR.ts:1789: `'hint.builder.doubleDoor': 'Clique nos vãos para uma entrada de duas folhas'`
- feature entry `hint.builder.garageDoor` — src/ui/i18n/pt-BR.ts:1790: `'hint.builder.garageDoor': 'Clique nos vãos para uma porta de garagem seccionada'`
- feature entry `builder.tool.stairRun` — src/ui/i18n/pt-BR.ts:1815: `'builder.tool.stairRun': 'Escada (traçada)'`
- feature entry `hint.builder.stairRun` — src/ui/i18n/pt-BR.ts:1816: `'hint.builder.stairRun': 'Clique nos cantos da subida · Enter assenta um lance que vira com o traçado e chega ao pavimento de cima'`
- shared entry `undo` — src/ui/v2/shell.ts:85: `undo: '<path d="M9 7 4 12l5 5"/><path d="M5 12h8a6 6 0 0 1 6 6"/>'`
- shared entry `close` — src/ui/v2/shell.ts:87: `close: '<path d="M6 6l12 12M18 6 6 18"/>'`
- feature entry `frame` — src/ui/v2/shell.ts:106: `frame: '<path d="M4 9V4h5M20 9V4h-5M4 15v5h5M20 15v5h-5"/>'`
- shared entry `grid` — src/ui/v2/shell.ts:107: `grid: '<path d="M3 3h18v18H3zM3 9h18M3 15h18M9 3v18M15 3v18"/>'`
- feature entry `hide` — src/ui/v2/shell.ts:108: `hide: '<path d="M3 3l18 18"/><path d="M10.6 6.2A9 9 0 0 1 21 12a14 14 0 0 1-2.4 3.2M6.3 6.6A14 14 0 0 0 3 12s3 6 9 6a8.8 8.8 0 0 0 4.3-1.1"/>'`
- feature entry `check` — src/ui/v2/shell.ts:109: `check: '<path d="m5 12 5 5 9-10"/>'`
- feature entry `scope_bay` — src/ui/v2/shell.ts:157: `scope_bay: '<rect x="4" y="4" width="16" height="16" rx="1"/><path d="M9.3 4v16M14.7 4v16M4 9.3h16M4 14.7h16"/><rect x="9.3" y="9.3" width="5.4" height="5.4" fi`
- feature entry `scope_zone` — src/ui/v2/shell.ts:158: `scope_zone: '<rect x="4" y="4" width="16" height="16" rx="1"/><path d="M9.3 4v16M14.7 4v16M4 9.3h16M4 14.7h16"/><rect x="9.3" y="4" width="10.7" height="10.7" f`
- feature entry `scope_row` — src/ui/v2/shell.ts:159: `scope_row: '<rect x="4" y="4" width="16" height="16" rx="1"/><path d="M9.3 4v16M14.7 4v16M4 9.3h16M4 14.7h16"/><rect x="4.8" y="10.1" width="3.7" height="3.8" f`
- feature entry `scope_column` — src/ui/v2/shell.ts:160: `scope_column: '<rect x="4" y="4" width="16" height="16" rx="1"/><path d="M9.3 4v16M14.7 4v16M4 9.3h16M4 14.7h16"/><rect x="9.3" y="4" width="5.4" height="16" fi`
- feature entry `scope_storey` — src/ui/v2/shell.ts:161: `scope_storey: '<rect x="4" y="4" width="16" height="16" rx="1"/><path d="M9.3 4v16M14.7 4v16M4 9.3h16M4 14.7h16"/><rect x="2" y="9.3" width="20" height="5.4" fi`
- feature entry `scope_side` — src/ui/v2/shell.ts:162: `scope_side: '<rect x="4" y="4" width="16" height="16" rx="1"/><path d="M9.3 4v16M14.7 4v16M4 9.3h16M4 14.7h16"/><rect x="4" y="4" width="16" height="16" fill="c`
- feature entry `scope_volume` — src/ui/v2/shell.ts:163: `scope_volume: '<path d="M12 3 20 7.5v9L12 21l-8-4.5v-9Z"/><path d="M4 7.5 12 12l8-4.5M12 12v9"/><path d="M12 3 20 7.5 12 12 4 7.5Z" fill="currentColor" fill-opa`
- feature entry `ridge_x` — src/ui/v2/shell.ts:165: `ridge_x: '<path d="M3 12h18"/><path d="m7 8-4 4 4 4M17 8l4 4-4 4"/>'`
- feature entry `ridge_y` — src/ui/v2/shell.ts:166: `ridge_y: '<path d="M12 3v18"/><path d="m8 7 4-4 4 4M8 17l4 4 4-4"/>'`
- feature entry `fall_front` — src/ui/v2/shell.ts:167: `fall_front: '<path d="M12 4v15"/><path d="m6 13 6 6 6-6"/>'`
- feature entry `fall_right` — src/ui/v2/shell.ts:168: `fall_right: '<path d="M4 12h15"/><path d="m13 6 6 6-6 6"/>'`
- feature entry `fall_back` — src/ui/v2/shell.ts:169: `fall_back: '<path d="M12 20V5"/><path d="m6 11 6-6 6 6"/>'`
- feature entry `fall_left` — src/ui/v2/shell.ts:170: `fall_left: '<path d="M20 12H5"/><path d="m11 6-6 6 6 6"/>'`
- shared entry `advanced` — src/ui/v2/shell.ts:171: `advanced: '<path d="M4 7h10M18 7h2M4 17h4M12 17h8"/><circle cx="16" cy="7" r="2"/><circle cx="10" cy="17" r="2"/>'`
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
- shared style `lab-doc-verdict` — src/lab/doctor/doctor.css:10: `.lab-doc-verdict.bound-gpu { border-left: 4px solid var(--lab-bad); }`
- shared style `lab-doc-verdict` — src/lab/doctor/doctor.css:11: `.lab-doc-verdict.bound-gpu h2 { color: var(--lab-bad); }`
- shared style `lab-doc-verdict` — src/lab/doctor/doctor.css:12: `.lab-doc-verdict.bound-cpu { border-left: 4px solid var(--lab-warn); }`
- shared style `lab-doc-verdict` — src/lab/doctor/doctor.css:13: `.lab-doc-verdict.bound-cpu h2 { color: var(--lab-warn); }`
- shared style `lab-doc-verdict` — src/lab/doctor/doctor.css:14: `.lab-doc-verdict.bound-ok { border-left: 4px solid var(--lab-good); }`
- shared style `lab-doc-verdict` — src/lab/doctor/doctor.css:15: `.lab-doc-verdict.bound-ok h2 { color: var(--lab-good); }`
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
- shared style `lab-doc-card` — src/lab/doctor/doctor.css:33: `.lab-doc-card.sev-high { border-left-color: var(--lab-bad); }`
- shared style `lab-doc-card` — src/lab/doctor/doctor.css:34: `.lab-doc-card.sev-medium { border-left-color: var(--lab-warn); }`
- shared style `lab-doc-card` — src/lab/doctor/doctor.css:35: `.lab-doc-card.sev-low { border-left-color: #6aa9ff; }`
- shared style `lab-doc-card` — src/lab/doctor/doctor.css:36: `.lab-doc-card.sev-info { border-left-color: var(--lab-line2); }`
- shared style `lab-doc-card` — src/lab/doctor/doctor.css:37: `.lab-doc-card header { display: flex; align-items: baseline; gap: 8px; flex-wrap: wrap; }`
- shared style `lab-doc-card` — src/lab/doctor/doctor.css:38: `.lab-doc-card header strong { font-size: 1.06em; }`
- shared style `lab-doc-sev` — src/lab/doctor/doctor.css:39: `.lab-doc-sev, .lab-doc-area { font-size: 0.82em; text-transform: uppercase; letter-spacing: 0.05em; padding: 1px 6px; border-radius: 4px; background: var(--lab-`
- shared style `sev-high` — src/lab/doctor/doctor.css:40: `.sev-high .lab-doc-sev { background: rgb(255 111 97 / 0.2); color: var(--lab-bad); }`
- shared style `sev-medium` — src/lab/doctor/doctor.css:41: `.sev-medium .lab-doc-sev { background: rgb(255 195 90 / 0.18); color: var(--lab-warn); }`
- shared style `lab-doc-card` — src/lab/doctor/doctor.css:42: `.lab-doc-card ul { margin: 6px 0; padding-left: 18px; }`
- shared style `lab-doc-card` — src/lab/doctor/doctor.css:43: `.lab-doc-card li { margin: 2px 0; font-variant-numeric: tabular-nums; }`
- shared style `lab-doc-why` — src/lab/doctor/doctor.css:44: `.lab-doc-why { margin: 4px 0 6px; color: var(--lab-muted); }`
- shared style `lab-doc-ab` — src/lab/doctor/doctor.css:45: `.lab-doc-ab { margin-top: 8px; padding: 6px 8px; border-radius: 6px; background: var(--lab-bg3); }`
- shared style `lab-doc-ab-row` — src/lab/doctor/doctor.css:46: `.lab-doc-ab-row { display: grid; grid-template-columns: 90px 160px 1fr; gap: 8px; font-variant-numeric: tabular-nums; }`
- shared style `lab-panel` — src/lab/lab.css:2: `.lab-panel { --lab-bg: #0f161a; --lab-bg2: #151f25; --lab-bg3: #1c2830; --lab-line: rgb(255 255 255 / 0.09); --lab-line2: rgb(255 255 255 / 0.16); --lab-text: #`
- shared style `lab-panel` — src/lab/lab.css:10: `.lab-panel[hidden] { display: none; }`
- shared style `lab-panel` — src/lab/lab.css:12: `.lab-panel.see-through { background: rgb(15 22 26 / 0.72); }`
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
- shared style `lab-table` — src/lab/lab.css:122: `.lab-table td.bar { background: linear-gradient(to left, rgb(47 194 171 / 0.22) var(--bar), transparent var(--bar)); }`
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
- shared style `panel` — src/peopleLab/peopleLab.css:40: `.panel { background: var(--panel); border-right: 1px solid var(--line); overflow-y: auto; padding: 14px 14px 14px; display: flex; flex-direction: column; scroll`
- shared style `panel` — src/peopleLab/peopleLab.css:41: `.panel h2 { margin: 0 0 10px; font-size: 15px; font-weight: 600; letter-spacing: -.01em; }`
- shared style `panel` — src/peopleLab/peopleLab.css:42: `.panel h3 { margin: 18px 0 8px; font-size: 11px; font-weight: 600; letter-spacing: .06em; text-transform: uppercase; color: var(--ink-3); }`
- shared style `panel` — src/peopleLab/peopleLab.css:43: `.panel h3:first-child { margin-top: 2px; }`
- shared style `row` — src/peopleLab/peopleLab.css:45: `.row { display: grid; grid-template-columns: 96px 1fr 38px; align-items: center; gap: 8px; min-height: 28px; color: var(--ink-2); }`
- shared style `row` — src/peopleLab/peopleLab.css:46: `.row output { text-align: right; color: var(--ink-3); font-variant-numeric: tabular-nums; font-size: 11px; }`
- shared style `seg` — src/peopleLab/peopleLab.css:54: `.seg { display: flex; background: var(--panel-2); border-radius: 7px; padding: 2px; gap: 2px; margin-bottom: 4px; }`
- shared style `seg` — src/peopleLab/peopleLab.css:55: `.seg button { flex: 1; border: 0; padding: 6px; border-radius: 5px; background: transparent; cursor: pointer; font-weight: 500; color: var(--ink-2); }`
- shared style `seg` — src/peopleLab/peopleLab.css:56: `.seg button.on { background: var(--panel-3); color: var(--ink); box-shadow: inset 0 0 0 1px #3a4048; }`
- shared style `swatches` — src/peopleLab/peopleLab.css:57: `.swatches { display: flex; flex-wrap: wrap; gap: 5px; align-items: center; }`
- shared style `swatches` — src/peopleLab/peopleLab.css:58: `.swatches button { width: 22px; height: 22px; border-radius: 50%; border: 0; box-shadow: inset 0 0 0 1px rgba(255,255,255,.12); cursor: pointer; padding: 0; }`
- shared style `swatches` — src/peopleLab/peopleLab.css:59: `.swatches button.on { box-shadow: 0 0 0 2px var(--panel), 0 0 0 3.5px var(--accent); }`
- shared style `swatches` — src/peopleLab/peopleLab.css:60: `.swatches input[type=color] { width: 24px; height: 24px; border: 0; padding: 0; background: none; cursor: pointer; }`
- shared style `grid` — src/peopleLab/peopleLab.css:61: `.grid { display: grid; grid-template-columns: repeat(3, 1fr); gap: 6px; }`
- shared style `tile` — src/peopleLab/peopleLab.css:62: `.tile { border: 1px solid var(--line); border-radius: var(--radius); background: var(--panel-2); padding: 9px 6px; cursor: pointer; display: flex; flex-directio`
- shared style `tile` — src/peopleLab/peopleLab.css:64: `.tile .glyph { display: none; }`
- shared style `tile` — src/peopleLab/peopleLab.css:65: `.tile img { width: 56px; height: 56px; object-fit: contain; border-radius: 6px; }`
- shared style `tile` — src/peopleLab/peopleLab.css:66: `.tile:hover { border-color: #3d434b; color: var(--ink); }`
- shared style `tile` — src/peopleLab/peopleLab.css:67: `.tile.on { border-color: var(--accent); color: var(--ink); background: var(--accent-soft); }`
- shared style `tool` — src/peopleLab/peopleLab.css:69: `.tool { border: 1px solid transparent; border-radius: 7px; background: var(--panel-2); height: 52px; cursor: pointer; display: flex; flex-direction: column; ali`
- shared style `tool` — src/peopleLab/peopleLab.css:70: `.tool:hover { color: var(--ink); background: var(--panel-3); }`
- shared style `tool` — src/peopleLab/peopleLab.css:71: `.tool.on { color: var(--ink); border-color: var(--accent); background: var(--accent-soft); }`
- shared style `chips` — src/peopleLab/peopleLab.css:72: `.chips { display: flex; flex-wrap: wrap; gap: 4px; }`
- shared style `chips` — src/peopleLab/peopleLab.css:73: `.chips button { border: 1px solid var(--line); background: var(--panel-2); border-radius: 6px; padding: 5px 9px; cursor: pointer; font-size: 12px; color: var(--`
- shared style `chips` — src/peopleLab/peopleLab.css:74: `.chips button:hover { color: var(--ink); }`
- shared style `chips` — src/peopleLab/peopleLab.css:75: `.chips button.on { background: var(--accent-soft); border-color: var(--accent); color: var(--ink); }`
- shared style `check` — src/peopleLab/peopleLab.css:76: `.check { display: flex; gap: 8px; align-items: center; color: var(--ink-2); margin: 6px 0; cursor: pointer; }`
- shared style `check` — src/peopleLab/peopleLab.css:77: `.check input { accent-color: var(--accent); }`
- shared style `big` — src/peopleLab/peopleLab.css:78: `.big { display: flex; align-items: center; justify-content: center; gap: 7px; width: 100%; border: 1px solid var(--line); border-radius: 7px; padding: 8px 10px;`
- shared style `big` — src/peopleLab/peopleLab.css:79: `.big:hover { background: var(--panel-3); }`
- shared style `big` — src/peopleLab/peopleLab.css:80: `.big.save { background: var(--accent); border-color: var(--accent); color: #fff; }`
- shared style `big` — src/peopleLab/peopleLab.css:81: `.big.save:hover { background: var(--accent-2); }`
- shared style `big` — src/peopleLab/peopleLab.css:82: `.big:disabled { opacity: .4; cursor: default; }`
- shared style `pair` — src/peopleLab/peopleLab.css:83: `.pair { display: grid; grid-template-columns: 1fr 1fr; gap: 6px; }`
- shared style `fine` — src/peopleLab/peopleLab.css:84: `details.fine { margin: 2px 0 6px; }`
- shared style `fine` — src/peopleLab/peopleLab.css:85: `details.fine summary { cursor: pointer; font-size: 12px; color: var(--ink-3); padding: 6px 0; list-style: none; }`
- shared style `fine` — src/peopleLab/peopleLab.css:86: `details.fine summary::-webkit-details-marker { display: none; }`
- shared style `fine` — src/peopleLab/peopleLab.css:87: `details.fine summary::before { content: '+ '; }`
- shared style `fine` — src/peopleLab/peopleLab.css:88: `details.fine[open] summary::before { content: '\2013 '; }`
- shared style `fine` — src/peopleLab/peopleLab.css:89: `details.fine summary:hover { color: var(--ink); }`
- shared style `pics` — src/peopleLab/peopleLab.css:92: `.pics { display: grid; grid-template-columns: repeat(3, 1fr); gap: 6px; margin-bottom: 4px; align-content: start; }`
- shared style `panel` — src/peopleLab/peopleLab.css:94: `.panel > * { flex-shrink: 0; }`
- shared style `panel` — src/peopleLab/peopleLab.css:95: `.panel > .picker { flex: 1 1 0; min-height: 150px; display: flex; flex-direction: column; overflow: hidden; margin-bottom: 8px; }`
- shared style `picker` — src/peopleLab/peopleLab.css:96: `.picker .pics { margin: 0; }`
- shared style `picker-search` — src/peopleLab/peopleLab.css:97: `.picker-search { display: none; align-items: center; gap: 6px; margin-bottom: 6px; background: var(--panel-2); color: var(--ink-3); border: 1px solid var(--line`
- shared style `picker-search` — src/peopleLab/peopleLab.css:98: `.picker-search.on { display: flex; }`
- shared style `picker-search` — src/peopleLab/peopleLab.css:99: `.picker-search input { flex: 1; min-width: 0; background: transparent; border: 0; outline: 0; color: var(--ink); font: inherit; font-size: 12px; padding: 5px 0;`
- shared style `picker-arrow` — src/peopleLab/peopleLab.css:100: `.picker-arrow { display: grid; place-items: center; }`
- shared style `panel` — src/peopleLab/peopleLab.css:101: `.panel .bar { display: flex; gap: 6px; align-items: center; margin-bottom: 6px; }`
- shared style `panel` — src/peopleLab/peopleLab.css:102: `.panel .bar .seg { flex: 1; margin: 0; }`
- shared style `toggle` — src/peopleLab/peopleLab.css:104: `.toggle { width: 32px; height: 30px; display: grid; place-items: center; border: 1px solid var(--line); border-radius: 7px; background: var(--panel-2); color: v`
- shared style `toggle` — src/peopleLab/peopleLab.css:105: `.toggle.on { color: var(--ink); border-color: var(--accent); box-shadow: inset 0 0 0 1px var(--accent); }`
- shared style `picker-bar` — src/peopleLab/peopleLab.css:106: `.picker-bar { height: 30px; margin-top: auto; display: flex; align-items: center; justify-content: center; gap: 10px; color: var(--ink-3); font-size: 12px; font`
- shared style `picker-bar` — src/peopleLab/peopleLab.css:107: `.picker-bar.one { visibility: hidden; }`
- shared style `picker-arrow` — src/peopleLab/peopleLab.css:108: `.picker-arrow { width: 28px; height: 24px; border-radius: 6px; border: 1px solid var(--line); background: var(--panel-2); color: var(--ink); cursor: pointer; fo`
- shared style `picker-arrow` — src/peopleLab/peopleLab.css:109: `.picker-arrow:disabled { opacity: .35; cursor: default; }`
- shared style `pic` — src/peopleLab/peopleLab.css:110: `.pic { border: 1px solid var(--line); border-radius: var(--radius); background: linear-gradient(180deg, #2b3038, #1f2228); padding: 0; cursor: pointer; overflow`
- shared style `pic` — src/peopleLab/peopleLab.css:112: `.pic img { width: 100%; aspect-ratio: 1; object-fit: cover; display: block; }`
- shared style `pic` — src/peopleLab/peopleLab.css:113: `.pic img.missing { background: var(--panel-2); }`
- shared style `pic` — src/peopleLab/peopleLab.css:114: `.pic span { padding: 4px 5px 5px; white-space: nowrap; overflow: hidden; text-overflow: ellipsis; text-align: center; }`
- shared style `pic` — src/peopleLab/peopleLab.css:115: `.pic:hover { border-color: #444b54; color: var(--ink); }`
- shared style `pic` — src/peopleLab/peopleLab.css:116: `.pic.on { border-color: var(--accent); color: var(--ink); box-shadow: inset 0 0 0 1px var(--accent); }`
- shared style `stage` — src/peopleLab/peopleLab.css:119: `.stage { position: relative; overflow: hidden; background: radial-gradient(ellipse at 50% 35%, var(--stage-top), var(--stage-bottom) 75%); }`
- shared style `stage` — src/peopleLab/peopleLab.css:120: `.stage canvas { display: block; width: 100%; height: 100%; }`
- shared style `top` — src/peopleLab/peopleLab.css:121: `.top { position: absolute; left: 14px; right: 14px; top: 12px; display: flex; align-items: center; gap: 10px; pointer-events: none; }`
- shared style `top` — src/peopleLab/peopleLab.css:122: `.top > * { pointer-events: auto; }`
- shared style `busy` — src/peopleLab/peopleLab.css:129: `.busy { position: absolute; top: 52px; left: 50%; transform: translateX(-50%); background: rgba(22, 24, 28, .85); border: 1px solid var(--line); border-radius: `
- shared style `busy` — src/peopleLab/peopleLab.css:130: `.busy.on { display: block; }`
- shared style `stats` — src/peopleLab/peopleLab.css:131: `.stats { position: absolute; left: 14px; bottom: 16px; font-size: 11px; color: var(--ink-3); line-height: 1.5; font-variant-numeric: tabular-nums; }`
- shared style `side` — src/peopleLab/peopleLab.css:142: `.side { background: var(--panel); border-left: 1px solid var(--line); display: flex; flex-direction: column; padding: 14px; gap: 8px; }`
- shared style `side` — src/peopleLab/peopleLab.css:143: `.side h2 { margin: 0 0 2px; font-size: 11px; font-weight: 600; letter-spacing: .06em; text-transform: uppercase; color: var(--ink-3); }`
- shared style `side` — src/peopleLab/peopleLab.css:144: `.side .big { margin-top: 0; }`
- shared style `gallery` — src/peopleLab/peopleLab.css:145: `.gallery { flex: 1; overflow-y: auto; display: grid; grid-template-columns: 1fr 1fr; gap: 6px; align-content: start; margin-top: 4px; }`
- shared style `card` — src/peopleLab/peopleLab.css:146: `.card { position: relative; border-radius: var(--radius); background: var(--panel-2); border: 1px solid var(--line); cursor: pointer; padding: 3px; text-align: `
- shared style `card` — src/peopleLab/peopleLab.css:147: `.card img { width: 100%; aspect-ratio: 1; object-fit: cover; border-radius: 6px; background: #2a2f37; display: block; margin-bottom: 3px; }`
- shared style `card` — src/peopleLab/peopleLab.css:148: `.card.on { border-color: var(--accent); color: var(--ink); }`
- shared style `card` — src/peopleLab/peopleLab.css:149: `.card .x { position: absolute; top: 5px; right: 5px; width: 18px; height: 18px; border-radius: 4px; border: 0; background: rgba(0,0,0,.55); color: #fff; cursor:`
- shared style `card` — src/peopleLab/peopleLab.css:150: `.card:hover .x { opacity: 1; }`
- shared style `subtabs` — src/peopleLab/peopleLab.css:159: `.subtabs { display: flex; flex-wrap: wrap; gap: 2px; margin: -2px -2px 12px; padding-bottom: 10px; border-bottom: 1px solid var(--line); }`
- shared style `subtabs` — src/peopleLab/peopleLab.css:160: `.subtabs button { border: 0; background: transparent; padding: 5px 9px; border-radius: 6px; cursor: pointer; font-size: 12px; font-weight: 500; color: var(--ink`
- shared style `subtabs` — src/peopleLab/peopleLab.css:161: `.subtabs button:hover { color: var(--ink); background: var(--panel-2); }`
- shared style `subtabs` — src/peopleLab/peopleLab.css:162: `.subtabs button.on { color: var(--ink); background: var(--panel-3); }`
- shared style `gap` — src/peopleLab/peopleLab.css:163: `.gap { height: 12px; }`
- shared style `panel` — src/peopleLab/peopleLab.css:172: `.panel .bar .bar-gap { flex: 1; }`
- shared style `panel` — src/peopleLab/peopleLab.css:175: `.panel .loading { height: 3px; border-radius: 2px; background: linear-gradient(90deg, transparent, var(--accent), transparent); background-size: 50% 100%; backg`
- shared style `panel` — src/peopleLab/peopleLab.css:177: `.panel .chips + .bar { margin-top: 8px; }`
- shared style `glass` — src/ui/app.css:57: `.glass { background: linear-gradient(145deg, var(--ui-hi), transparent 42%), var(--ui-bg); border: 1px solid var(--ui-border); border-radius: 14px; box-shadow: `
- shared style `panel-title` — src/ui/app.css:77: `.panel-title { display: flex; align-items: center; justify-content: space-between; font-size: 10px; letter-spacing: 0.12em; color: var(--muted); margin-bottom: `
- shared style `panel-title` — src/ui/app.css:86: `.panel-title kbd { font: inherit; border: 1px solid var(--ui-border); border-radius: 5px; padding: 1px 5px; }`
- feature style `collapsed` — src/ui/app.css:94: `.collapsed > .panel-title { margin-bottom: 0; }`
- feature style `collapsed` — src/ui/app.css:95: `.collapsed.glass { width: auto; }`
- shared style `signal-chips` — src/ui/app.css:140: `.signal-chips { display: flex; flex-wrap: wrap; gap: 5px; margin-top: 6px; }`
- shared style `signal-chip` — src/ui/app.css:141: `.signal-chip { display: inline-flex; align-items: center; gap: 5px; padding: 4px 8px 4px 6px; border-radius: 999px; border: 1px solid var(--ui-border); backgrou`
- shared style `signal-chip` — src/ui/app.css:152: `.signal-chip i { width: 8px; height: 8px; border-radius: 50%; background: #ff5b50; box-shadow: 0 0 6px currentColor; color: #ff5b50; }`
- shared style `signal-chip` — src/ui/app.css:153: `.signal-chip[data-state="green"] i { background: #4ee08a; color: #4ee08a; }`
- shared style `signal-chip` — src/ui/app.css:154: `.signal-chip[data-state="amber"] i { background: #ffc03d; color: #ffc03d; }`
- feature style `collapsed` — src/ui/app.css:192: `@media (max-width: 780px) { .collapsed.glass { padding-top: 8px; padding-bottom: 8px; } }`
- shared style `bw-icon-button` — src/ui/app.css:234: `.bw-icon-button { display: inline-flex; align-items: center; gap: 6px; height: 30px; padding: 0 9px; border: 1px solid var(--bw-border); border-radius: var(--bw`
- shared style `bw-icon-button` — src/ui/app.css:246: `.bw-icon-button:hover:not(:disabled) { color: var(--bw-text); border-color: var(--bw-active); }`
- shared style `bw-icon-button` — src/ui/app.css:247: `.bw-icon-button { width: 30px; padding: 0; justify-content: center; }`
- shared style `bw-icon-button` — src/ui/builder/workspace.css:4: `.bw-icon-button { display: inline-flex; align-items: center; gap: 6px; height: 28px; padding: 0 9px; border: 1px solid var(--bw-border); border-radius: var(--bw`
- shared style `bw-icon-button` — src/ui/builder/workspace.css:17: `.bw-icon-button:hover:not(:disabled) { color: var(--bw-text); border-color: var(--bw-active); }`
- shared style `bw-icon-button` — src/ui/builder/workspace.css:18: `.bw-icon-button { width: 30px; padding: 0; justify-content: center; }`
- shared style `bw-icon-button` — src/ui/builder/workspace.css:19: `.bw-icon-button:disabled { opacity: 0.3; }`
- shared style `bw-inspector` — src/ui/builder/workspace.css:22: `.bw-inspector { position: absolute; top: 138px; right: 10px; width: 268px; max-height: calc(100dvh - 138px - 60px); overflow-y: auto; scrollbar-width: thin; poi`
- feature style `collapsed` — src/ui/builder/workspace.css:35: `.bw-inspector.collapsed { width: 34px; max-height: 34px; overflow: hidden; }`
- feature style `collapsed` — src/ui/builder/workspace.css:36: `.bw-inspector.collapsed .bw-inspector-title { display: none; }`
- shared style `bw-inspector-head` — src/ui/builder/workspace.css:37: `.bw-inspector-head { display: flex; align-items: center; justify-content: space-between; gap: 6px; padding: 6px 6px 6px 10px; border-bottom: 1px solid var(--bw-`
- feature style `collapsed` — src/ui/builder/workspace.css:45: `.bw-inspector.collapsed .bw-inspector-head { border-bottom: 0; padding: 2px; }`
- shared style `bw-inspector-title` — src/ui/builder/workspace.css:46: `.bw-inspector-title { color: var(--bw-muted); font-size: 9.5px; letter-spacing: 0.12em; text-transform: uppercase; }`
- shared style `bw-inspector-body` — src/ui/builder/workspace.css:47: `.bw-inspector-body { padding: 8px 10px 12px; display: grid; gap: 6px; }`
- shared style `bw-inspector-name` — src/ui/builder/workspace.css:48: `.bw-inspector-name { color: var(--bw-text); font-size: 12.5px; font-weight: 600; }`
- shared style `bw-field` — src/ui/builder/workspace.css:49: `.bw-field { display: grid; grid-template-columns: 1fr 76px 14px; align-items: center; gap: 6px; color: var(--bw-muted); font-size: 11px; }`
- shared style `bw-field-input` — src/ui/builder/workspace.css:50: `.bw-field-input { width: 100%; height: 26px; padding: 0 6px; border: 1px solid var(--bw-border); border-radius: 5px; background: color-mix(in srgb, var(--bw-bg)`
- shared style `bw-field-input` — src/ui/builder/workspace.css:62: `.bw-field-input:focus { outline: none; border-color: var(--bw-active); }`
- shared style `bw-field-unit` — src/ui/builder/workspace.css:63: `.bw-field-unit { color: var(--bw-muted); font-size: 10px; }`
- shared style `bw-field-text` — src/ui/builder/workspace.css:64: `.bw-field-text { grid-column: 2 / -1; color: var(--bw-text); font-size: 11px; text-align: right; }`
- shared style `bw-field-material` — src/ui/builder/workspace.css:65: `.bw-field-material { grid-template-columns: 1fr auto; }`
- shared style `bw-inspector` — src/ui/builder/workspace.css:68: `@media (max-width: 780px) { .bw-inspector { width: min(230px, calc(60vw - 12px)); top: 136px; max-height: calc(100dvh - 330px); } }`
- shared style `bw-inspector` — src/ui/builder/workspace.css:72: `.bw-inspector { top: 64px; max-height: calc(100dvh - 64px - 60px); }`
- shared style `bw-inspector-actions` — src/ui/builder/workspace.css:73: `.bw-inspector-actions { display: flex; gap: 3px; padding: 6px 8px; border-bottom: 1px solid var(--bw-border); }`
- feature style `collapsed` — src/ui/builder/workspace.css:79: `.bw-inspector.collapsed .bw-inspector-actions { display: none; }`
- shared style `bw-inspector-actions` — src/ui/builder/workspace.css:80: `.bw-inspector-actions .bw-action { flex: 1; height: 30px; }`
- shared style `bw-inspector-actions` — src/ui/builder/workspace.css:81: `.bw-inspector-actions .bw-action.danger:hover { color: var(--bw-danger); border-color: var(--bw-danger); }`
- shared style `cn-art` — src/ui/roads/roads.css:166: `.cn-art { background: #1d2a2c; }`
- shared style `cn-art` — src/ui/roads/roads.css:167: `.cn-art svg { cursor: default; }`
- feature style `cn-stub` — src/ui/roads/roads.css:168: `.cn-stub { stroke: #5b6b6e; stroke-width: 5; stroke-linecap: round; }`
- feature style `cn-link` — src/ui/roads/roads.css:169: `.cn-link { fill: none; stroke: #cfd8d6; stroke-width: 2; opacity: 0.75; }`
- feature style `cn-link` — src/ui/roads/roads.css:170: `.cn-link.dim { opacity: 0.18; }`
- feature style `cn-link` — src/ui/roads/roads.css:171: `.cn-link.on { stroke: var(--v-accent, #4fe0bf); stroke-width: 3; opacity: 1; }`
- feature style `cn-end` — src/ui/roads/roads.css:172: `.cn-end { stroke: #0d1617; stroke-width: 2; cursor: pointer; }`
- feature style `cn-end` — src/ui/roads/roads.css:173: `.cn-end.in { fill: #5aa7ff; }`
- feature style `cn-end` — src/ui/roads/roads.css:174: `.cn-end.out { fill: #6fd38a; }`
- feature style `cn-end` — src/ui/roads/roads.css:175: `.cn-end.on { stroke: #ffffff; stroke-width: 3; }`
- feature style `cn-end` — src/ui/roads/roads.css:176: `.cn-end:hover { stroke: #ffffff; }`
- shared style `v2-icon` — src/ui/v2/shell.css:41: `.v2-icon > span, .v2-pill > span, .v2-seg-b > span, .v2-tabs.compact .v2-tab.icon:not(.on) > span, .v2-tabs.tight .v2-tab.icon > span, .v2-choice.icon > span, .`
- shared style `v2-icon` — src/ui/v2/shell.css:69: `.v2-pill, .v2-icon { position: relative; display: inline-flex; align-items: center; justify-content: center; gap: 6px; width: 32px; height: 32px; padding: 0; fl`
- shared style `v2-icon` — src/ui/v2/shell.css:74: `.v2-pill:hover, .v2-icon:hover:not(:disabled) { background: var(--v-raise-2); border-color: var(--v-accent-line); }`
- shared style `v2-icon` — src/ui/v2/shell.css:75: `.v2-icon.on, .v2-pill.on { color: var(--v-accent); border-color: var(--v-accent); background: var(--v-accent-soft); }`
- shared style `v2-icon` — src/ui/v2/shell.css:76: `.v2-icon.danger { color: var(--v-danger-ink); }`
- feature style `v2-swatches` — src/ui/v2/shell.css:265: `.v2-swatches { display: flex; flex-wrap: wrap; gap: 6px; }`
- feature style `v2-swatch` — src/ui/v2/shell.css:266: `.v2-swatch { width: 24px; height: 24px; padding: 0; border: 1px solid var(--v-line); border-radius: 7px; background: var(--c); }`
- feature style `v2-swatch` — src/ui/v2/shell.css:267: `.v2-swatch.on { outline: 2px solid var(--v-accent); outline-offset: 2px; }`
- feature style `v2-swatch` — src/ui/v2/shell.css:268: `.v2-swatch.custom { background: conic-gradient(#e66, #ec6, #6c6, #6ce, #66e, #c6e, #e66); -webkit-appearance: none; appearance: none; }`
- feature style `v2-swatch` — src/ui/v2/shell.css:269: `.v2-swatch.custom::-webkit-color-swatch { opacity: 0; }`

## Where the program plugs the feature in

### src/buildingsWiring.ts

- `src/buildingsWiring.ts#BuildingWiring` (interface, line 49) ← call from `src/main.ts#<ExpressionStatement>@3923`, `src/main.ts#afterRestore`, `src/main.ts#bulldozer`, `src/main.ts#cancelGestures`, …
- `src/buildingsWiring.ts#BuildingWiring` (interface, line 49) ← read from `src/main.ts#<IfStatement>@4036`, `src/main.ts#actions`, `src/main.ts#mountInterface`, `src/main.ts#pointerDown`, …
- `src/buildingsWiring.ts#createBuildingWiring` (function, line 81) ← call from `src/main.ts#buildings`
- `src/buildingsWiring.ts#createBuildingWiring` (function, line 81) ← implementation from `src/ui/v2/shell.ts#mountShell.renderBuild`, `src/ui/v2/shell.ts#mountShell.renderModels`, `src/ui/v2/shell.ts#mountShell.renderSection`, `src/ui/v2/shell.ts#mountShell.seeInside`, …
- `src/buildingsWiring.ts#createBuildingWiring.activate` (method, line 1093) ← implementation from `src/main.ts#takeTool`
- `src/buildingsWiring.ts#createBuildingWiring.afterRoadEdit` (method, line 1191) ← implementation from `src/main.ts#mutateBuilt`
- `src/buildingsWiring.ts#createBuildingWiring.beforeDraw` (method, line 1120) ← implementation from `src/main.ts#frame`
- `src/buildingsWiring.ts#createBuildingWiring.bulldozeAt` (method, line 1198) ← implementation from `src/main.ts#bulldozer`
- `src/buildingsWiring.ts#createBuildingWiring.cancelOperation` (method, line 1065) ← implementation from `src/main.ts#pointerDown`
- `src/buildingsWiring.ts#createBuildingWiring.deactivate` (method, line 1101) ← implementation from `src/main.ts#takeTool`
- `src/buildingsWiring.ts#createBuildingWiring.drawOverlay` (method, line 1147) ← implementation from `src/main.ts#drawOverlayScreen`
- `src/buildingsWiring.ts#createBuildingWiring.hintKey@1206` (method, line 1206) ← implementation from `src/main.ts#hintKey`
- `src/buildingsWiring.ts#createBuildingWiring.insideClick` (method, line 985) ← implementation from `src/main.ts#doubleClick`, `src/main.ts#singleClick`
- `src/buildingsWiring.ts#createBuildingWiring.key` (method, line 1076) ← implementation from `src/main.ts#gameKeys`
- `src/buildingsWiring.ts#createBuildingWiring.languageChanged` (method, line 1209) ← implementation from `src/main.ts#<ExpressionStatement>@3923`
- `src/buildingsWiring.ts#createBuildingWiring.pointerDown` (method, line 1000) ← implementation from `src/main.ts#pointerDown`
- `src/buildingsWiring.ts#createBuildingWiring.pointerMove` (method, line 1036) ← implementation from `src/main.ts#pointerMove`
- `src/buildingsWiring.ts#createBuildingWiring.pointerUp` (method, line 1052) ← implementation from `src/main.ts#cancelGestures`, `src/main.ts#endPointer`
- `src/buildingsWiring.ts#createBuildingWiring.requestThumbnails` (function, line 483) ← implementation from `src/ui/v2/shell.ts#mountShell.ask`
- `src/buildingsWiring.ts#createBuildingWiring.restored` (method, line 1187) ← implementation from `src/main.ts#afterRestore`, `src/main.ts#snapshotApplied`

### src/editor/blocks.ts

- `src/editor/blocks.ts#blockGridLines` (function, line 20) ← call from `src/main.ts#drawOverlayScreen`
- `src/editor/blocks.ts#commitBlockGrid` (function, line 32) ← call from `src/main.ts#pointerDown`

### src/editor/blueprintLibrary.ts

- `src/editor/blueprintLibrary.ts#readEntry` (function, line 52) ← indirectCall from `src/core/keptList.ts#KeptList.read`

### src/editor/buildingTool.ts

- `src/editor/buildingTool.ts#BuildingTool.buildingAt` (method, line 1963) ← call from `src/main.ts#actions`, `src/main.ts#pointerDown`

### src/editor/growthFeed.ts

- `src/editor/growthFeed.ts#GrowthOutcome` (type, line 42) ← read from `src/main.ts#growthAnswered`
- `src/editor/growthFeed.ts#GrowthSession` (class, line 72) ← construct from `src/main.ts#growth`
- `src/editor/growthFeed.ts#GrowthSession.ask` (method, line 104) ← call from `src/main.ts#<IfStatement>@4036`, `src/main.ts#growZones`
- `src/editor/growthFeed.ts#GrowthSession.busy` (getter, line 99) ← call from `src/main.ts#<IfStatement>@4036`, `src/main.ts#growZones`
- `src/editor/growthFeed.ts#GrowthSession.receive` (method, line 129) ← call from `src/main.ts#growthAnswered`
- `src/editor/growthFeed.ts#GrowthSession.restart` (method, line 118) ← call from `src/main.ts#growthPool`

### src/editor/growthReplica.ts

- `src/editor/growthReplica.ts#GrowAnswer` (type, line 100) ← typeRef from `src/main.ts#growthAnswered`

### src/editor/lotTool.ts

- `src/editor/lotTool.ts#LotOverlay` (interface, line 49) ← read from `src/main.ts#drawOverlayScreen`
- `src/editor/lotTool.ts#LotOverlayBook.labels` (field, line 91) ← read from `src/main.ts#drawOverlayScreen`
- `src/editor/lotTool.ts#LotSplitKind` (type, line 29) ← typeRef from `src/main.ts#gameState`
- `src/editor/lotTool.ts#LotTool` (class, line 116) ← construct from `src/main.ts#lotTool`
- `src/editor/lotTool.ts#LotTool.anyZoned` (method, line 151) ← call from `src/main.ts#drawOverlayScreen`
- `src/editor/lotTool.ts#LotTool.cancel` (method, line 200) ← call from `src/main.ts#cancelGestures`
- `src/editor/lotTool.ts#LotTool.down` (method, line 429) ← call from `src/main.ts#pointerDown`
- `src/editor/lotTool.ts#LotTool.gesture` (method, line 187) ← call from `src/main.ts#currentGesture`, `src/main.ts#gestureInProgress`
- `src/editor/lotTool.ts#LotTool.hover` (field, line 118) ← write from `src/main.ts#pointerMove`
- `src/editor/lotTool.ts#LotTool.modeChanged` (method, line 214) ← call from `src/main.ts#setZone`
- `src/editor/lotTool.ts#LotTool.move` (method, line 502) ← call from `src/main.ts#pointerMove`
- `src/editor/lotTool.ts#LotTool.overlay` (method, line 621) ← call from `src/main.ts#drawOverlayScreen`
- `src/editor/lotTool.ts#LotTool.refusedNow` (method, line 174) ← call from `src/main.ts#<IfStatement>@4036`, `src/main.ts#growZones`
- `src/editor/lotTool.ts#LotTool.up` (method, line 541) ← call from `src/main.ts#endPointer`
- `src/editor/lotTool.ts#ZoneMode` (type, line 27) ← typeRef from `src/main.ts#gameState`

### src/growth.worker.ts

- `src/growth.worker.ts#serve` (function, line 5) ← callbackAssign from `src/bays.worker.ts#serve`, `src/cityGen.worker.ts#serve`, `src/conflictTable.worker.ts#serve`, `src/editor/save.worker.ts#serve`, …
- `src/growth.worker.ts#serve` (function, line 5) ← moduleLoad from `src/job.worker.ts#LOADERS`

### src/growthPool.ts

- `src/growthPool.ts#GrowthPool` (class, line 10) ← construct from `src/main.ts#growthPool`
- `src/growthPool.ts#GrowthPool.pump` (method, line 35) ← call from `src/main.ts#frame`

### src/render/buildings/buildingMesh.ts

- `src/render/buildings/buildingMesh.ts#buildBuildingMeshes` (function, line 4158) ← call from `src/render/renderer.ts#createSceneRenderer.warmPreviewShaders`
- `src/render/buildings/buildingMesh.ts#BuildingMeshes` (interface, line 128) ← call from `src/render/renderer.ts#createSceneRenderer.warmPreviewShaders`
- `src/render/buildings/buildingMesh.ts#BuildingMeshes` (interface, line 128) ← read from `src/render/renderer.ts#createSceneRenderer.warmPreviewShaders`
- `src/render/buildings/buildingMesh.ts#interiorFurniture` (function, line 4175) ← call from `src/render/destruction.ts#createDestruction.make`
- `src/render/buildings/buildingMesh.ts#meshesOf.dispose` (method, line 4132) ← implementation from `src/render/renderer.ts#createSceneRenderer.warmPreviewShaders`
- `src/render/buildings/buildingMesh.ts#withShellParts` (function, line 3874) ← call from `src/render/destruction.ts#createDestruction.make`

### src/render/buildings/cell.worker.ts

- `src/render/buildings/cell.worker.ts#serve` (function, line 5) ← callbackAssign from `src/bays.worker.ts#serve`, `src/cityGen.worker.ts#serve`, `src/conflictTable.worker.ts#serve`, `src/editor/save.worker.ts#serve`, …
- `src/render/buildings/cell.worker.ts#serve` (function, line 5) ← moduleLoad from `src/job.worker.ts#LOADERS`

### src/render/buildings/cellArrays.ts

- `src/render/buildings/cellArrays.ts#FINISH_ATTRIBUTE` (variable, line 80) ← read from `src/render/destruction.ts#createDestruction.place`

### src/render/buildings/cellPool.ts

- `src/render/buildings/cellPool.ts#CellPool.onPage` (method, line 345) ← callbackAssign from `src/render/groundShape.ts#padsOfAsk`
- `src/render/buildings/cellPool.ts#CellPool.onPage` (method, line 345) ← implementation from `src/render/groundShape.ts#padsOfAsk`
- `src/render/buildings/cellPool.ts#CellPool.submit` (method, line 265) ← callbackAssign from `src/jobs/jobPool.ts#dispatch`, `src/jobs/jobPool.ts#JobQueue.answer`, `src/jobs/jobPool.ts#JobQueue.fail`
- `src/render/buildings/cellPool.ts#CellPool.submit` (method, line 265) ← implementation from `src/jobs/jobPool.ts#dispatch`, `src/jobs/jobPool.ts#JobQueue.answer`, `src/jobs/jobPool.ts#JobQueue.fail`
- `src/render/buildings/cellPool.ts#FloorColumns.constructor` (constructor, line 120) ← callbackAssign from `src/core/idSlots.ts#IdSlots.take`, `src/sim/agents/buildingOrder.ts#StoreOrder.constructor`, `src/sim/ambient/ambientIndex.ts#BuildingMix.constructor`, `src/sim/ambient/ambientIndex.ts#SightWalls.constructor`, …

### src/render/buildings/cellWork.ts

- `src/render/buildings/cellWork.ts#runCellJob` (function, line 119) ← callbackAssign from `src/render/groundShape.ts#padsOfAsk`
- `src/render/buildings/cellWork.ts#workerReads` (function, line 100) ← callbackAssign from `src/render/groundShape.ts#padsOfAsk`
- `src/render/buildings/cellWork.ts#workerReads` (function, line 100) ← implementation from `src/render/groundShape.ts#padsOfAsk`

### src/render/buildings/chunk.ts

- `src/render/buildings/chunk.ts#BuildingChunk` (interface, line 108) ← typeRef from `src/render/destruction.ts#createDestruction`

### src/render/buildings/fade.ts

- `src/render/buildings/fade.ts#fadeMeshes` (function, line 17) ← call from `src/render/renderer.ts#createSceneRenderer.warmPreviewShaders`

### src/render/buildings/flagLayer.ts

- `src/render/buildings/flagLayer.ts#createFlagLayer.set` (method, line 117) ← implementation from `src/render/shadowCasters.ts#castCastersInto`

### src/render/buildings/fracture.ts

- `src/render/buildings/fracture.ts#Fragment` (interface, line 31) ← read from `src/render/destruction.ts#createDestruction.land`, `src/render/destruction.ts#createDestruction.loosen`, `src/render/destruction.ts#createDestruction.place`, `src/render/destruction.ts#createDestruction.strike`, …
- `src/render/buildings/fracture.ts#Fragment` (interface, line 31) ← typeRef from `src/render/destruction.ts#Piece`
- `src/render/buildings/fracture.ts#fragmentGeometry` (function, line 173) ← call from `src/render/destruction.ts#createDestruction.loosen`
- `src/render/buildings/fracture.ts#prepareFracture` (function, line 118) ← call from `src/render/destruction.ts#createDestruction.make`
- `src/render/buildings/fracture.ts#StillData` (interface, line 164) ← read from `src/render/destruction.ts#createDestruction.place`
- `src/render/buildings/fracture.ts#StillData` (interface, line 164) ← typeRef from `src/render/destruction.ts#createDestruction.place`, `src/render/destruction.ts#createDestruction.workerOf`, `src/render/destruction.ts#Ruin`

### src/render/buildings/fracture.worker.ts

- `src/render/buildings/fracture.worker.ts#serve` (function, line 5) ← callbackAssign from `src/bays.worker.ts#serve`, `src/cityGen.worker.ts#serve`, `src/conflictTable.worker.ts#serve`, `src/editor/save.worker.ts#serve`, …
- `src/render/buildings/fracture.worker.ts#serve` (function, line 5) ← moduleLoad from `src/job.worker.ts#LOADERS`

### src/render/buildings/frameBudget.ts

- `src/render/buildings/frameBudget.ts#FrameScale.budget` (field, line 58) ← write from `src/render/renderer.ts#createSceneRenderer`
- `src/render/buildings/frameBudget.ts#FrameScale.scale` (field, line 59) ← read from `src/render/renderer.ts#createSceneRenderer`

### src/render/buildings/gpuBatchedMesh.ts

- `src/render/buildings/gpuBatchedMesh.ts#gpuBatchContext` (function, line 39) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/buildings/gpuBatchedMesh.ts#GpuBatchedMesh` (class, line 69) ← construct from `src/render/networkFarRoads.ts#NetworkFarRoads.place`
- `src/render/buildings/gpuBatchedMesh.ts#GpuBatchedMesh` (class, line 69) ← typeRef from `src/render/networkFarRoads.ts#NetworkFarRoads.clearMeshes`, `src/render/networkFarRoads.ts#NetworkFarRoads.meshesOf`
- `src/render/buildings/gpuBatchedMesh.ts#GpuBatchedMesh.dispose` (method, line 396) ← call from `src/render/networkFarRoads.ts#NetworkFarRoads.clearMeshes`

### src/render/buildings/kit.ts

- `src/render/buildings/kit.ts#BuildingKit` (interface, line 146) ← call from `src/render/renderer.ts#createSceneRenderer.warmBlastShaders`
- `src/render/buildings/kit.ts#BuildingKit` (interface, line 146) ← read from `src/render/renderer.ts#createSceneRenderer.warmBlastShaders`
- `src/render/buildings/kit.ts#BuildingKit` (interface, line 146) ← typeRef from `src/render/destruction.ts#createDestruction`
- `src/render/buildings/kit.ts#createBuildingKit.furniture` (method, line 689) ← implementation from `src/render/renderer.ts#createSceneRenderer.warmBlastShaders`

### src/render/buildings/layer.ts

- `src/render/buildings/layer.ts#createBuildingLayer` (function, line 82) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/buildings/layer.ts#createBuildingLayer.anyLotHeightAt` (method, line 1175) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/buildings/layer.ts#createBuildingLayer.castInto` (method, line 1101) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/buildings/layer.ts#createBuildingLayer.census` (method, line 1062) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/buildings/layer.ts#createBuildingLayer.chunkOf` (method, line 724) ← implementation from `src/render/renderer.ts#createSceneRenderer.loadEffects`
- `src/render/buildings/layer.ts#createBuildingLayer.covers` (method, line 1167) ← implementation from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.coverFree`, `src/render/renderer.ts#createSceneRenderer.natureOpen`
- `src/render/buildings/layer.ts#createBuildingLayer.cullDetails` (method, line 1081) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/buildings/layer.ts#createBuildingLayer.dispose` (method, line 1211) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/buildings/layer.ts#createBuildingLayer.hideOccluded` (method, line 1139) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/buildings/layer.ts#createBuildingLayer.levelOfDetail` (method, line 981) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/buildings/layer.ts#createBuildingLayer.levels` (method, line 1066) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/buildings/layer.ts#createBuildingLayer.lotHeightAt` (method, line 1170) ← implementation from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.loadEffects`
- `src/render/buildings/layer.ts#createBuildingLayer.occluderCell` (method, line 1132) ← implementation from `src/render/renderer.ts#createSceneRenderer.occluderDrawn`
- `src/render/buildings/layer.ts#createBuildingLayer.refill` (method, line 1185) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/buildings/layer.ts#createBuildingLayer.restoreOccluded` (method, line 1156) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/buildings/layer.ts#createBuildingLayer.revealOccluded` (method, line 1153) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/buildings/layer.ts#createBuildingLayer.setCutaway` (method, line 1161) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/buildings/layer.ts#createBuildingLayer.setDetailBudget` (method, line 1036) ← implementation from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.applyQuality`, `src/render/renderer.ts#createSceneRenderer.governGeometry`
- `src/render/buildings/layer.ts#createBuildingLayer.setDimmed` (method, line 972) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/buildings/layer.ts#createBuildingLayer.setMassBudget` (method, line 1040) ← implementation from `src/render/renderer.ts#createSceneRenderer.applyGeometryBudget`
- `src/render/buildings/layer.ts#createBuildingLayer.setNight` (method, line 978) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/buildings/layer.ts#createBuildingLayer.setPreview` (method, line 975) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/buildings/layer.ts#createBuildingLayer.setRuined` (method, line 967) ← implementation from `src/render/renderer.ts#createSceneRenderer.loadEffects`
- `src/render/buildings/layer.ts#createBuildingLayer.update` (method, line 730) ← implementation from `src/render/groundShape.ts#padsOfAsk`, `src/render/renderer.ts#createSceneRenderer`

### src/render/buildings/layerApi.ts

- `src/render/buildings/layerApi.ts#BuildingLayer` (interface, line 23) ← call from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.applyGeometryBudget`, `src/render/renderer.ts#createSceneRenderer.applyQuality`, `src/render/renderer.ts#createSceneRenderer.coverFree`, …
- `src/render/buildings/layerApi.ts#BuildingLayer` (interface, line 23) ← read from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.governGeometry`, `src/render/renderer.ts#createSceneRenderer.switchedOff`, `src/render/renderer.ts#createSceneRenderer.warmBlastShaders`, …
- `src/render/buildings/layerApi.ts#BuildingLayer` (interface, line 23) ← write from `src/render/renderer.ts#createSceneRenderer`
- `src/render/buildings/layerApi.ts#BuildingPreviewInput` (interface, line 11) ← typeRef from `src/render/renderer.ts#SceneHandle`
- `src/render/buildings/layerApi.ts#CutawaySpec` (interface, line 102) ← read from `src/render/renderer.ts#createSceneRenderer`
- `src/render/buildings/layerApi.ts#CutawaySpec` (interface, line 102) ← typeRef from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#SceneHandle`

### src/render/buildings/layerCells.ts

- `src/render/buildings/layerCells.ts#BATCH_CELL` (variable, line 43) ← read from `src/render/renderer.ts#createSceneRenderer.occluderDrawn`
- `src/render/buildings/layerCells.ts#cellGroup.dispose` (method, line 97) ← implementation from `src/render/renderer.ts#createSceneRenderer.warmPreviewShaders`
- `src/render/buildings/layerCells.ts#createCoverIndex` (function, line 206) ← implementation from `src/world/viewCells.ts#ViewCells.make`

### src/render/buildings/partPose.ts

- `src/render/buildings/partPose.ts#poseMatrixInto` (function, line 13) ← call from `src/lab/catalog/capture.ts#instanceMatrices`

### src/render/buildings/partWarm.ts

- `src/render/buildings/partWarm.ts#partWarmMeshes` (function, line 8) ← call from `src/render/renderer.ts#createSceneRenderer.warmBlastShaders`

### src/render/buildings/townColumns.ts

- `src/render/buildings/townColumns.ts#TownColumns.constructor` (constructor, line 85) ← callbackAssign from `src/core/idSlots.ts#IdSlots.take`, `src/sim/agents/buildingOrder.ts#StoreOrder.constructor`, `src/sim/ambient/ambientIndex.ts#BuildingMix.constructor`, `src/sim/ambient/ambientIndex.ts#SightWalls.constructor`, …

### src/render/buildingSwitches.ts

- `src/render/buildingSwitches.ts#BuildingSwitcher` (interface, line 33) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/buildingSwitches.ts#createBuildingSwitcher` (function, line 43) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/buildingSwitches.ts#createBuildingSwitcher.afterRender` (method, line 85) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/buildingSwitches.ts#createBuildingSwitcher.beforeRender` (method, line 57) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/buildingSwitches.ts#createBuildingSwitcher.casters` (method, line 91) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/buildingSwitches.ts#createBuildingSwitcher.dispose` (method, line 97) ← implementation from `src/render/renderer.ts#createSceneRenderer`

### src/render/gardens.ts

- `src/render/gardens.ts#createGardenCells` (function, line 246) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/gardens.ts#createGardenCells` (function, line 246) ← implementation from `src/render/propCells.ts#createPropLayer`, `src/render/propCells.ts#createPropLayer.batches.make`
- `src/render/gardens.ts#createGardenCells.cull` (method, line 268) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/gardens.ts#createGardenCells.dispose` (method, line 269) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/gardens.ts#createGardenCells.follow` (method, line 266) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/gardens.ts#createGardenCells.setMap` (method, line 267) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/gardens.ts#createGardenCells.source` (method, line 260) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/gardens.ts#GardenCells` (interface, line 233) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/gardens.ts#GardenCells` (interface, line 233) ← read from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.switchedOff`
- `src/render/gardens.ts#GardenCells` (interface, line 233) ← typeRef from `src/render/renderer.ts#createSceneRenderer`
- `src/render/gardens.ts#gardenKinds.kind` (function, line 171) ← callbackAssign from `src/render/cellInstances.ts#batchMesh`, `src/render/cellInstances.ts#createCellInstances.cull`, `src/render/cellInstances.ts#createCellInstances.makeMesh`
- `src/render/gardens.ts#gardenKinds.kind` (function, line 171) ← implementation from `src/render/cellInstances.ts#batchMesh`, `src/render/cellInstances.ts#createCellInstances.cull`, `src/render/cellInstances.ts#createCellInstances.makeMesh`

### src/render/interiorSolids.ts

- `src/render/interiorSolids.ts#CabinParts` (class, line 44) ← construct from `src/render/carBody.ts#buildCarModel`, `src/render/vehicleModels.ts#buildBusModel`, `src/render/vehicleModels.ts#buildTruckModel`
- `src/render/interiorSolids.ts#CabinParts.add` (method, line 49) ← call from `src/render/carBody.ts#buildCarModel`, `src/render/vehicleModels.ts#buildBusModel`, `src/render/vehicleModels.ts#buildTruckModel`
- `src/render/interiorSolids.ts#CabinParts.geometries` (field, line 45) ← read from `src/render/carBody.ts#buildCarModel`, `src/render/vehicleModels.ts#buildBusModel`, `src/render/vehicleModels.ts#buildTruckModel`
- `src/render/interiorSolids.ts#CabinParts.solids` (method, line 59) ← call from `src/render/carBody.ts#buildCarModel`, `src/render/vehicleModels.ts#buildBusModel`, `src/render/vehicleModels.ts#buildTruckModel`
- `src/render/interiorSolids.ts#InteriorSolid` (interface, line 34) ← typeRef from `src/render/vehicleModels.ts#VehicleModel`

### src/render/lotOverlay.ts

- `src/render/lotOverlay.ts#applyLotOverlay` (function, line 208) ← call from `src/render/materials.ts#createMaterials`, `src/render/terrain.ts#terrainMaterial`
- `src/render/lotOverlay.ts#createLotOverlay` (function, line 315) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/lotOverlay.ts#createLotOverlay` (function, line 315) ← typeRef from `src/render/renderer.ts#createSceneRenderer`
- `src/render/lotOverlay.ts#createLotOverlay.dispose` (method, line 510) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/lotOverlay.ts#createLotOverlay.set` (method, line 471) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/lotOverlay.ts#LotOverlayInput` (interface, line 13) ← typeRef from `src/render/renderer.ts#SceneHandle`

### src/render/mesh/farBlockBatches.ts

- `src/render/mesh/farBlockBatches.ts#batchedFar` (function, line 30) ← call from `src/render/mesh/roadResidency.ts#restoreCell`
- `src/render/mesh/farBlockBatches.ts#FarBlockBatches` (class, line 51) ← construct from `src/render/renderer.ts#createSceneRenderer`
- `src/render/mesh/farBlockBatches.ts#FarBlockBatches` (class, line 51) ← typeRef from `src/render/roadSurfaces.ts#SurfaceReuse`
- `src/render/mesh/farBlockBatches.ts#FarBlockBatches.adopt` (method, line 62) ← call from `src/render/roadSurfaces.ts#roadSurfaceSteps`
- `src/render/mesh/farBlockBatches.ts#FarBlockBatches.group` (field, line 52) ← read from `src/render/renderer.ts#createSceneRenderer`
- `src/render/mesh/farBlockBatches.ts#FarBlockBatches.refill` (method, line 95) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/mesh/farBlockBatches.ts#FarBlockBatches.sync` (method, line 75) ← call from `src/render/renderer.ts#createSceneRenderer`

### src/render/roomLamps.ts

- `src/render/roomLamps.ts#RoomLamps` (class, line 10) ← construct from `src/render/renderer.ts#createSceneRenderer`
- `src/render/roomLamps.ts#RoomLamps.lamps` (method, line 14) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/roomLamps.ts#RoomLamps.lamps` (method, line 14) ← read from `src/render/renderer.ts#createSceneRenderer`

### src/render/torusWindow.ts

- `src/render/torusWindow.ts#TexelBand` (type, line 4) ← typeRef from `src/render/fogLayer.ts#createFogField.raster`
- `src/render/torusWindow.ts#TorusWindow` (class, line 6) ← construct from `src/render/fogLayer.ts#createFogField`
- `src/render/torusWindow.ts#TorusWindow.bandOf` (method, line 34) ← call from `src/render/fogLayer.ts#createFogField.update`
- `src/render/torusWindow.ts#TorusWindow.follow` (method, line 46) ← call from `src/render/fogLayer.ts#createFogField.follow`
- `src/render/torusWindow.ts#TorusWindow.ox` (field, line 8) ← read from `src/render/fogLayer.ts#createFogField.layer`
- `src/render/torusWindow.ts#TorusWindow.oy` (field, line 9) ← read from `src/render/fogLayer.ts#createFogField.layer`
- `src/render/torusWindow.ts#TorusWindow.span` (getter, line 17) ← call from `src/render/fogLayer.ts#createFogField.layer`
- `src/render/torusWindow.ts#TorusWindow.texel` (method, line 22) ← call from `src/render/fogLayer.ts#createFogField.raster`
- `src/render/torusWindow.ts#TorusWindow.whole` (getter, line 41) ← call from `src/render/fogLayer.ts#createFogField.whole`

### src/render/wear.ts

- `src/render/wear.ts#applyWear` (function, line 139) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/wear.ts#createWearField` (function, line 44) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/wear.ts#createWearField.dispose` (method, line 132) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/wear.ts#createWearField.feet` (method, line 97) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/wear.ts#createWearField.follow` (method, line 100) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/wear.ts#createWearField.tick` (method, line 117) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/wear.ts#createWearField.wheels` (method, line 90) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/wear.ts#WearField` (interface, line 28) ← call from `src/render/renderer.ts#createSceneRenderer`

### src/ui/builder/catalog.ts

- `src/ui/builder/catalog.ts#BUILDER_TAB_SPECS` (variable, line 36) ← read from `src/ui/v2/shell.ts#mountShell.renderBuild`
- `src/ui/builder/catalog.ts#BuilderCategoryId` (type, line 3) ← typeRef from `src/ui/v2/shell.ts#mountShell.renderBuild`
- `src/ui/builder/catalog.ts#BuilderSection` (interface, line 17) ← read from `src/ui/v2/shell.ts#mountShell.renderBuild`, `src/ui/v2/shell.ts#mountShell.renderSection`
- `src/ui/builder/catalog.ts#BuilderSection` (interface, line 17) ← typeRef from `src/ui/v2/shell.ts#mountShell.renderSection`
- `src/ui/builder/catalog.ts#BuilderTabSpec` (interface, line 26) ← read from `src/ui/v2/shell.ts#mountShell.renderBuild`
- `src/ui/builder/catalog.ts#BuilderToolSpec` (interface, line 7) ← read from `src/ui/v2/shell.ts#mountShell.renderBuild`, `src/ui/v2/shell.ts#mountShell.renderSection`, `src/ui/v2/shell.ts#mountShell.toolButton`, `src/ui/v2/shell.ts#mountShell.toolButton.run`
- `src/ui/builder/catalog.ts#BuilderToolSpec` (interface, line 7) ← typeRef from `src/ui/v2/shell.ts#mountShell.toolButton`
- `src/ui/builder/catalog.ts#DRAW_ACTIONS` (variable, line 169) ← read from `src/ui/v2/shell.ts#mountShell.renderSection`
- `src/ui/builder/catalog.ts#DRAW_SHAPES` (variable, line 125) ← read from `src/ui/v2/shell.ts#mountShell.picture`, `src/ui/v2/shell.ts#mountShell.renderSection`
- `src/ui/builder/catalog.ts#FACADE_SCOPES` (variable, line 172) ← read from `src/ui/v2/shell.ts#mountShell.renderSection`
- `src/ui/builder/catalog.ts#PlanShapeId` (type, line 122) ← typeRef from `src/ui/planSwatch.ts#OUTLINES`, `src/ui/planSwatch.ts#planSwatch`
- `src/ui/builder/catalog.ts#TIER_SHAPES` (variable, line 138) ← read from `src/ui/v2/shell.ts#mountShell.picture`, `src/ui/v2/shell.ts#mountShell.renderSection`

### src/ui/builder/icons.ts

- `src/ui/builder/icons.ts#builderIconSvg` (function, line 313) ← call from `src/ui/v2/shell.ts#mountShell`, `src/ui/v2/shell.ts#mountShell.BARRIER_ICON`, `src/ui/v2/shell.ts#mountShell.renderBuild`, `src/ui/v2/shell.ts#mountShell.renderModels.fill`, …

### src/ui/builder/workspace.ts

- `src/ui/builder/workspace.ts#BuilderActions` (interface, line 52) ← call from `src/ui/v2/shell.ts#mountShell.ask`, `src/ui/v2/shell.ts#mountShell.renderBuild`, `src/ui/v2/shell.ts#mountShell.renderModels`, `src/ui/v2/shell.ts#mountShell.renderSection`, …
- `src/ui/builder/workspace.ts#BuilderState` (interface, line 17) ← read from `src/ui/v2/shell.ts#mountShell.render`, `src/ui/v2/shell.ts#mountShell.renderBuild`, `src/ui/v2/shell.ts#mountShell.renderModels`, `src/ui/v2/shell.ts#mountShell.renderSection`, …
- `src/ui/builder/workspace.ts#BuilderState` (interface, line 17) ← typeRef from `src/ui/v2/shell.ts#mountShell`, `src/ui/v2/shell.ts#mountShell.renderModels`, `src/ui/v2/shell.ts#mountShell.renderSection`, `src/ui/v2/shell.ts#mountShell.sigOf`, …
- `src/ui/builder/workspace.ts#BuilderWorkspace` (interface, line 88) ← call from `src/main.ts#showTool`, `src/ui/v2/shell.ts#mountShell`, `src/ui/v2/shell.ts#mountShell.ask`, `src/ui/v2/shell.ts#mountShell.picture`, …
- `src/ui/builder/workspace.ts#BuilderWorkspace` (interface, line 88) ← read from `src/ui/v2/shell.ts#mountShell`, `src/ui/v2/shell.ts#mountShell.syncInside`
- `src/ui/builder/workspace.ts#BuilderWorkspace` (interface, line 88) ← typeRef from `src/ui/v2/shell.ts#ShellDeps`
- `src/ui/builder/workspace.ts#initBuilderWorkspace.inside` (method, line 341) ← implementation from `src/ui/v2/shell.ts#mountShell`
- `src/ui/builder/workspace.ts#initBuilderWorkspace.onInside` (method, line 342) ← implementation from `src/ui/v2/shell.ts#mountShell`
- `src/ui/builder/workspace.ts#initBuilderWorkspace.setMode` (method, line 345) ← implementation from `src/main.ts#showTool`
- `src/ui/builder/workspace.ts#initBuilderWorkspace.subscribe` (method, line 365) ← implementation from `src/ui/v2/shell.ts#mountShell`
- `src/ui/builder/workspace.ts#initBuilderWorkspace.thumbnail` (method, line 369) ← implementation from `src/ui/v2/shell.ts#mountShell.ask`, `src/ui/v2/shell.ts#mountShell.picture`, `src/ui/v2/shell.ts#mountShell.renderModels.fill`, `src/ui/v2/shell.ts#mountShell.renderSection`
- `src/ui/builder/workspace.ts#SNAP_MODES` (variable, line 108) ← read from `src/ui/v2/shell.ts#mountShell.renderBuild`

### src/ui/buildStamp.ts

- `src/ui/buildStamp.ts#formatBuildStamp` (function, line 20) ← call from `src/lab/recorder/report.ts#buildReport`, `src/ui/about.ts#mountAbout.paint`, `src/ui/about.ts#openAbout`
- `src/ui/buildStamp.ts#onBuildStamp` (function, line 28) ← call from `src/ui/about.ts#mountAbout`
- `src/ui/buildStamp.ts#refreshBuildStamp` (function, line 34) ← call from `src/ui/about.ts#openAbout`

### src/world/buildingGates.ts

- `src/world/buildingGates.ts#entrancesOf` (variable, line 13) ← call from `src/sim/agents/trafficBuildings.ts#project`, `src/world/parkingLayout.ts#entrancesNear`
- `src/world/buildingGates.ts#workOutEntrances` (function, line 16) ← indirectCall from `src/core/identityMemo.ts#identityMemo`

### src/world/buildings/blocks.ts

- `src/world/buildings/blocks.ts#resolve` (function, line 13) ← indirectCall from `src/core/identityMemo.ts#identityMemo`
- `src/world/buildings/blocks.ts#resolveBlocks` (variable, line 11) ← call from `src/actionsWiring.ts#createActions.rayOnBuilding`, `src/render/destruction.ts#createDestruction.make`

### src/world/buildings/blueprints.ts

- `src/world/buildings/blueprints.ts#Blueprint` (interface, line 23) ← read from `src/render/renderer.ts#createSceneRenderer.warmPreviewShaders`, `src/ui/v2/shell.ts#mountShell.renderBuild`, `src/ui/v2/shell.ts#mountShell.renderModels`, `src/ui/v2/shell.ts#mountShell.sigOf`
- `src/world/buildings/blueprints.ts#BLUEPRINTS` (variable, line 306) ← read from `src/render/renderer.ts#createSceneRenderer.warmPreviewShaders`, `src/ui/v2/shell.ts#mountShell.renderBuild`, `src/ui/v2/shell.ts#mountShell.renderModels`
- `src/world/buildings/blueprints.ts#instantiate` (function, line 32) ← call from `src/render/renderer.ts#createSceneRenderer.warmPreviewShaders`

### src/world/buildings/cameraSolids.ts

- `src/world/buildings/cameraSolids.ts#CameraSolids` (interface, line 80) ← call from `src/render/canyonLight.ts#createCanyonLight.follow`, `src/render/occlusion.ts#OcclusionBuffer.build`, `src/render/renderer.ts#createSceneRenderer`
- `src/world/buildings/cameraSolids.ts#CameraSolids` (interface, line 80) ← typeRef from `src/render/canyonLight.ts#CanyonLight`, `src/render/occlusion.ts#OcclusionBuffer.build`, `src/render/postprocess.ts#PostChain`, `src/render/renderer.ts#createSceneRenderer`
- `src/world/buildings/cameraSolids.ts#cameraSolidsKeeper` (function, line 123) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/world/buildings/cameraSolids.ts#CameraSolidsKeeper` (interface, line 113) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/world/buildings/cameraSolids.ts#cameraSolidsKeeper.follow` (method, line 134) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/world/buildings/cameraSolids.ts#cameraSolidsKeeper.solids` (method, line 156) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/world/buildings/cameraSolids.ts#GroundSince` (interface, line 105) ← call from `src/render/renderer.ts#createSceneRenderer.buildingsOnGroundSince`
- `src/world/buildings/cameraSolids.ts#GroundSince` (interface, line 105) ← typeRef from `src/render/renderer.ts#createSceneRenderer`
- `src/world/buildings/cameraSolids.ts#Rect` (type, line 102) ← typeRef from `src/render/canyonLight.ts#createCanyonLight.clearCells`, `src/render/canyonLight.ts#createCanyonLight.follow`
- `src/world/buildings/cameraSolids.ts#SolidFiling.constructor` (constructor, line 205) ← callbackAssign from `src/core/idSlots.ts#IdSlots.take`, `src/sim/agents/buildingOrder.ts#StoreOrder.constructor`, `src/sim/ambient/ambientIndex.ts#BuildingMix.constructor`, `src/sim/ambient/ambientIndex.ts#SightWalls.constructor`, …
- `src/world/buildings/cameraSolids.ts#SolidFiling.floorAt` (method, line 315) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/world/buildings/cameraSolids.ts#SolidFiling.forEachIn` (method, line 273) ← implementation from `src/render/canyonLight.ts#createCanyonLight.follow`
- `src/world/buildings/cameraSolids.ts#SolidFiling.forEachInAny` (method, line 278) ← implementation from `src/render/canyonLight.ts#createCanyonLight.follow`
- `src/world/buildings/cameraSolids.ts#SolidFiling.forEachOccluder` (method, line 298) ← implementation from `src/render/occlusion.ts#OcclusionBuffer.build`
- `src/world/buildings/cameraSolids.ts#SolidFiling.roofAt` (method, line 319) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/world/buildings/cameraSolids.ts#SolidVisit` (type, line 97) ← typeRef from `src/render/canyonLight.ts#createCanyonLight.layVolume`

### src/world/buildings/cityBuildings.ts

- `src/world/buildings/cityBuildings.ts#CITY_BUILDINGS` (variable, line 197) ← read from `src/ui/v2/shell.ts#mountShell.renderBuild`, `src/ui/v2/shell.ts#mountShell.renderModels`
- `src/world/buildings/cityBuildings.ts#CityBuilding` (interface, line 26) ← read from `src/ui/v2/shell.ts#mountShell.renderBuild`, `src/ui/v2/shell.ts#mountShell.renderModels`

### src/world/buildings/decayWatch.ts

- `src/world/buildings/decayWatch.ts#createDecayWatch` (function, line 8) ← call from `src/main.ts#decayLook`
- `src/world/buildings/decayWatch.ts#createDecayWatch` (function, line 8) ← callbackAssign from `src/core/idSlots.ts#IdSlots.take`, `src/sim/agents/buildingOrder.ts#StoreOrder.constructor`, `src/sim/ambient/ambientIndex.ts#BuildingMix.constructor`, `src/sim/ambient/ambientIndex.ts#SightWalls.constructor`, …

### src/world/buildings/docks.ts

- `src/world/buildings/docks.ts#isDock` (function, line 22) ← call from `src/sim/agents/lorryDocks.ts#buildYard`
- `src/world/buildings/docks.ts#LorryDock` (interface, line 7) ← read from `src/sim/agents/lorryDocks.ts#Yard.constructor`
- `src/world/buildings/docks.ts#LorryDock` (interface, line 7) ← typeRef from `src/sim/agents/lorryDocks.ts#YardDock`, `src/sim/agents/trafficBuildings.ts#TrafficBuilding`
- `src/world/buildings/docks.ts#lorryDocks` (function, line 26) ← call from `src/sim/agents/trafficBuildings.ts#project`

### src/world/buildings/elements.ts

- `src/world/buildings/elements.ts#elementRing` (function, line 119) ← call from `src/render/playEffects.ts#createPlayEffects`, `src/world/solids.ts#solidsOfItem`
- `src/world/buildings/elements.ts#onGround` (function, line 85) ← call from `src/world/solids.ts#solidsOfItem`

### src/world/buildings/floors.ts

- `src/world/buildings/floors.ts#buildingFloors` (function, line 19) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/world/buildings/floors.ts#BuildingFloors` (interface, line 7) ← call from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.followFloors`, `src/render/renderer.ts#createSceneRenderer.padsAskOf`
- `src/world/buildings/floors.ts#buildingFloors.floorOf` (method, line 23) ← implementation from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.padsAskOf`
- `src/world/buildings/floors.ts#buildingFloors.follow` (method, line 34) ← implementation from `src/render/renderer.ts#createSceneRenderer.followFloors`

### src/world/buildings/foundation.ts

- `src/world/buildings/foundation.ts#FLOOR_READ_REACH` (variable, line 34) ← read from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.floorBox`
- `src/world/buildings/foundation.ts#floorHeight` (function, line 289) ← call from `src/render/playEffects.ts#createPlayEffects`, `src/render/playEffects.ts#createPlayEffects.strikeBuilding`

### src/world/buildings/geometry.ts

- `src/world/buildings/geometry.ts#Bounds` (type, line 269) ← read from `src/render/renderer.ts#createSceneRenderer.bankBox`, `src/render/renderer.ts#createSceneRenderer.floorBox`
- `src/world/buildings/geometry.ts#levelElevation` (function, line 97) ← call from `src/actionsWiring.ts#createActions.explodeAt`, `src/main.ts#buildings`
- `src/world/buildings/geometry.ts#localDirToWorld` (function, line 73) ← call from `src/sim/agents/lotBays.ts#bayRows`
- `src/world/buildings/geometry.ts#localToWorld` (function, line 32) ← call from `src/render/destruction.ts#createDestruction.make`, `src/sim/agents/lorryDocks.ts#buildYard`, `src/sim/agents/lorryDocks.ts#Yard.constructor`, `src/sim/agents/lorryDocks.ts#Yard.toWorld`, …
- `src/world/buildings/geometry.ts#roofHeightAt` (function, line 775) ← call from `src/render/playEffects.ts#createPlayEffects`
- `src/world/buildings/geometry.ts#roofRise` (function, line 757) ← call from `src/main.ts#buildings`, `src/render/playEffects.ts#createPlayEffects`
- `src/world/buildings/geometry.ts#solidFootprints` (function, line 171) ← call from `src/actionsWiring.ts#createActions.explodeAt`, `src/actionsWiring.ts#createActions.tick`, `src/render/renderer.ts#createSceneRenderer`, `src/sim/agents/trafficBuildings.ts#project`
- `src/world/buildings/geometry.ts#storedBounds` (function, line 277) ← call from `src/render/renderer.ts#createSceneRenderer.bankBox`, `src/render/renderer.ts#createSceneRenderer.floorBox`, `src/sim/agents/trafficBuildings.ts#project`
- `src/world/buildings/geometry.ts#volumeCorners` (function, line 153) ← call from `src/editor/transitTools.ts#TransitTool.built`, `src/render/playEffects.ts#createPlayEffects`
- `src/world/buildings/geometry.ts#volumeElevation` (function, line 117) ← call from `src/actionsWiring.ts#createActions.rayOnBuilding`, `src/render/destruction.ts#createDestruction.make`, `src/render/playEffects.ts#createPlayEffects`
- `src/world/buildings/geometry.ts#worldToLocal` (function, line 59) ← call from `src/actionsWiring.ts#createActions.rayOnBuilding`, `src/render/playEffects.ts#createPlayEffects`, `src/sim/agents/lorryDocks.ts#buildYard`, `src/sim/agents/lorryDocks.ts#Yard.constructor`, …

### src/world/buildings/intern.ts

- `src/world/buildings/intern.ts#SharedMembers` (class, line 117) ← construct from `src/editor/preparedCity.ts#prepareCity`, `src/editor/smallCityMap.ts#buildCityTable`
- `src/world/buildings/intern.ts#SharedMembers` (class, line 117) ← typeRef from `src/editor/preparedCity.ts#PreparedCityWriter.constructor`, `src/editor/smallCityMap.ts#growCity`
- `src/world/buildings/intern.ts#SharedMembers.end` (method, line 137) ← call from `src/editor/preparedCity.ts#prepareCity`, `src/editor/smallCityMap.ts#buildCityTable`
- `src/world/buildings/intern.ts#SharedMembers.keeps` (method, line 165) ← call from `src/editor/preparedCity.ts#PreparedCityWriter.endBatch`
- `src/world/buildings/intern.ts#SharedMembers.share` (method, line 127) ← call from `src/editor/preparedCity.ts#PreparedCityWriter.add`

### src/world/buildings/lotLibrary.ts

- `src/world/buildings/lotLibrary.ts#LOT_NAMES` (variable, line 9) ← read from `src/ui/v2/shell.ts#mountShell.renderBuild`, `src/ui/v2/shell.ts#mountShell.renderModels`
- `src/world/buildings/lotLibrary.ts#lotKey` (function, line 12) ← call from `src/ui/v2/shell.ts#mountShell.renderModels`
- `src/world/buildings/lotLibrary.ts#lotKey` (function, line 12) ← read from `src/ui/v2/shell.ts#mountShell.renderBuild`

### src/world/buildings/materials.ts

- `src/world/buildings/materials.ts#Finish` (type, line 7) ← typeRef from `src/ui/materialSwatch.ts#BASE`, `src/ui/materialSwatch.ts#draw`, `src/ui/materialSwatch.ts#materialSwatch`
- `src/world/buildings/materials.ts#FINISH_COLOUR` (variable, line 30) ← read from `src/ui/materialSwatch.ts#BASE`
- `src/world/buildings/materials.ts#FINISHES` (variable, line 6) ← read from `src/ui/materialSwatch.ts#BASE`, `src/ui/v2/shell.ts#mountShell.renderSection`
- `src/world/buildings/materials.ts#MaterialSpec` (interface, line 9) ← read from `src/ui/v2/shell.ts#mountShell.renderSection`
- `src/world/buildings/materials.ts#STYLES` (variable, line 260) ← read from `src/ui/v2/shell.ts#mountShell.renderSection`

### src/world/buildings/pads.ts

- `src/world/buildings/pads.ts#BuildingPads` (interface, line 47) ← call from `src/render/renderer.ts#createSceneRenderer.shapeGround`
- `src/world/buildings/pads.ts#Pad` (interface, line 32) ← typeRef from `src/render/groundShape.ts#GroundJobAnswer`, `src/render/groundShape.ts#padsOfAsk`, `src/render/groundShapePool.ts#GroundShapePool.pads`, `src/render/groundShapePool.ts#GroundWorkers`, …
- `src/world/buildings/pads.ts#padOf` (function, line 116) ← call from `src/render/groundShape.ts#padsOfAsk`
- `src/world/buildings/pads.ts#PadShapeCut` (interface, line 165) ← read from `src/render/renderer.ts#createSceneRenderer.levelGroundDigest`
- `src/world/buildings/pads.ts#PadShapeCut` (interface, line 165) ← typeRef from `src/render/groundShape.ts#GroundShapeAsk`
- `src/world/buildings/pads.ts#padShaper` (function, line 316) ← call from `src/render/groundShape.ts#shapeGroundAsk`
- `src/world/buildings/pads.ts#townPads` (function, line 184) ← callbackAssign from `src/core/idSlots.ts#IdSlots.take`, `src/sim/agents/buildingOrder.ts#StoreOrder.constructor`, `src/sim/ambient/ambientIndex.ts#BuildingMix.constructor`, `src/sim/ambient/ambientIndex.ts#SightWalls.constructor`, …
- `src/world/buildings/pads.ts#TownPads` (interface, line 157) ← call from `src/render/renderer.ts#createSceneRenderer.levelGroundDigest`, `src/render/renderer.ts#createSceneRenderer.shapeGround`
- `src/world/buildings/pads.ts#townPads.cut` (method, line 284) ← implementation from `src/render/renderer.ts#createSceneRenderer.levelGroundDigest`, `src/render/renderer.ts#createSceneRenderer.shapeGround`
- `src/world/buildings/pads.ts#townPads.shapeBounds` (method, line 253) ← implementation from `src/render/renderer.ts#createSceneRenderer.shapeGround`

### src/world/buildings/recordTable.ts

- `src/world/buildings/recordTable.ts#allocate` (function, line 17) ← call from `src/world/doc.ts#RoadDoc.fromBinary`
- `src/world/buildings/recordTable.ts#eachRecord` (function, line 508) ← call from `src/render/groundShape.ts#padsOfAsk`, `src/sim/agents/trafficBuildings.ts#trafficChangesOf`
- `src/world/buildings/recordTable.ts#encodeRecords` (function, line 108) ← call from `src/editor/regionFrames.ts#squareFrame`
- `src/world/buildings/recordTable.ts#isRecordTable` (function, line 295) ← indirectCall from `src/jobs/processWorker.ts#ProcessWorker.kept`
- `src/world/buildings/recordTable.ts#isRecordTable` (function, line 295) ← reference from `src/desktopJobs.ts#desktopJobWorkers`
- `src/world/buildings/recordTable.ts#RecordRef` (interface, line 468) ← read from `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.keepRef`, `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.load`
- `src/world/buildings/recordTable.ts#RecordRef` (interface, line 468) ← typeRef from `src/render/groundShape.ts#PadsAsk`, `src/sim/agents/trafficBuildings.ts#BuildingChange`, `src/sim/agents/trafficBuildings.ts#BuildingSent`, `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.keepRef`, …
- `src/world/buildings/recordTable.ts#RecordTable` (class, line 302) ← construct from `src/editor/history.ts#openedGenerated`, `src/editor/preparedCity.ts#openedBuildings`, `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.load`, `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.record`
- `src/world/buildings/recordTable.ts#RecordTable.constructor` (constructor, line 312) ← read from `src/editor/preparedCity.ts#PreparedCityWriter.finish`, `src/editor/regionFrames.ts#squareFrame`
- `src/world/buildings/recordTable.ts#RecordTable.reader` (method, line 330) ← call from `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.load`, `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.record`
- `src/world/buildings/recordTable.ts#RecordTable.rows` (field, line 303) ← read from `src/editor/history.ts#openedGenerated`, `src/editor/preparedCity.ts#openedBuildings`
- `src/world/buildings/recordTable.ts#refOf` (function, line 493) ← call from `src/render/renderer.ts#createSceneRenderer.padsAskOf`, `src/sim/agents/trafficBuildings.ts#buildingChangesOf`
- `src/world/buildings/recordTable.ts#tableBytes` (function, line 30) ← call from `src/main.ts#cityJob`
- `src/world/buildings/recordTable.ts#tableIn` (function, line 23) ← call from `src/main.ts#cityData`, `src/main.ts#openedOf`
- `src/world/buildings/recordTable.ts#TableReader` (class, line 336) ← typeRef from `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.load`, `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.readers`
- `src/world/buildings/recordTable.ts#TableReader.record` (method, line 377) ← call from `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.load`, `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.record`
- `src/world/buildings/recordTable.ts#tableRecords` (function, line 484) ← call from `src/editor/regionFrames.ts#readSquareFrame`
- `src/world/buildings/recordTable.ts#TableWriter` (class, line 121) ← construct from `src/editor/preparedCity.ts#PreparedCityWriter.table`
- `src/world/buildings/recordTable.ts#TableWriter.add` (method, line 175) ← call from `src/editor/preparedCity.ts#PreparedCityWriter.add`
- `src/world/buildings/recordTable.ts#TableWriter.finish` (method, line 268) ← call from `src/editor/preparedCity.ts#PreparedCityWriter.finish`
- `src/world/buildings/recordTable.ts#TableWriter.keepOnly` (method, line 170) ← call from `src/editor/preparedCity.ts#PreparedCityWriter.endBatch`

### src/world/buildings/sections.ts

- `src/world/buildings/sections.ts#BuildingSections` (interface, line 7) ← read from `src/world/docBinary.ts#packImage`
- `src/world/buildings/sections.ts#BuildingSections` (interface, line 7) ← typeRef from `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.load`, `src/sim/agents/trafficBuildings.ts#TrafficMap`, `src/world/doc.ts#RoadDoc.fromJSON`, `src/world/docBinary.ts#DocImage`, …
- `src/world/buildings/sections.ts#eachSectionRef` (function, line 74) ← call from `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.load`
- `src/world/buildings/sections.ts#sectionsBytes` (function, line 48) ← call from `src/world/docBinary.ts#imageBytes`
- `src/world/buildings/sections.ts#sectionsSize` (function, line 42) ← call from `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.load`

### src/world/buildings/serialize.ts

- `src/world/buildings/serialize.ts#isSerializedBuildings` (function, line 398) ← call from `src/editor/persistence.ts#isSerializedDoc`
- `src/world/buildings/serialize.ts#migrateBuilding` (function, line 305) ← call from `src/editor/preparedCity.ts#PreparedCityWriter.add`
- `src/world/buildings/serialize.ts#SerializedBuilding` (type, line 59) ← typeRef from `src/world/doc.ts#SerializedDoc`

### src/world/buildings/siteGrading.ts

- `src/world/buildings/siteGrading.ts#siteGrading` (function, line 33) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/world/buildings/siteGrading.ts#siteGrading` (function, line 33) ← callbackAssign from `src/core/idSlots.ts#IdSlots.take`, `src/sim/agents/buildingOrder.ts#StoreOrder.constructor`, `src/sim/ambient/ambientIndex.ts#BuildingMix.constructor`, `src/sim/ambient/ambientIndex.ts#SightWalls.constructor`, …
- `src/world/buildings/siteGrading.ts#SiteGrading` (interface, line 15) ← call from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.shapeBlocksSteps`, `src/render/renderer.ts#createSceneRenderer.shapeGround`
- `src/world/buildings/siteGrading.ts#SiteGrading` (interface, line 15) ← read from `src/render/renderer.ts#createSceneRenderer.levelGroundDigest`, `src/render/renderer.ts#createSceneRenderer.shapeGround`
- `src/world/buildings/siteGrading.ts#siteGrading.changedSites` (function, line 91) ← implementation from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.shapeBlocksSteps`
- `src/world/buildings/siteGrading.ts#siteGrading.follow` (method, line 138) ← implementation from `src/render/renderer.ts#createSceneRenderer.shapeGround`
- `src/world/buildings/siteGrading.ts#siteGrading.forget` (method, line 172) ← implementation from `src/render/renderer.ts#createSceneRenderer.shapeGround`
- `src/world/buildings/siteGrading.ts#siteGrading.rebuild` (method, line 155) ← implementation from `src/render/renderer.ts#createSceneRenderer.shapeBlocksSteps`, `src/render/renderer.ts#createSceneRenderer.shapeGround`
- `src/world/buildings/siteGrading.ts#siteGrading.refresh` (method, line 129) ← implementation from `src/render/renderer.ts#createSceneRenderer.shapeBlocksSteps`
- `src/world/buildings/siteGrading.ts#siteGrading.take` (method, line 134) ← implementation from `src/render/renderer.ts#createSceneRenderer.shapeBlocksSteps`

### src/world/buildings/spilledItems.ts

- `src/world/buildings/spilledItems.ts#SpilledItems` (class, line 16) ← typeRef from `src/editor/smallCityMap.ts#growCity`
- `src/world/buildings/spilledItems.ts#SpilledItems.endBatch` (method, line 51) ← call from `src/editor/smallCityMap.ts#buildCityTable`
- `src/world/buildings/spilledItems.ts#SpilledItems.heldCount` (getter, line 37) ← call from `src/editor/smallCityMap.ts#buildCityTable`
- `src/world/buildings/spilledItems.ts#SpilledItems.spill` (method, line 56) ← call from `src/editor/smallCityMap.ts#buildCityTable`

### src/world/buildings/store.ts

- `src/world/buildings/store.ts#BuildingStore` (class, line 32) ← construct from `src/world/doc.ts#RoadDoc.buildings`
- `src/world/buildings/store.ts#BuildingStore` (class, line 32) ← typeRef from `src/world/doc.ts#RoadDoc.fromJSON`
- `src/world/buildings/store.ts#BuildingStore.all` (method, line 123) ← call from `src/editor/chunkedSave.ts#KEYED`, `src/render/flatAtlas.ts#createFlatAtlas.readWholeContent`, `src/sim/agents/trafficBuildings.ts#docBuildings.eachEntrances`, `src/sim/agents/trafficBuildings.ts#docBuildings.eachFacts`, …
- `src/world/buildings/store.ts#BuildingStore.changedSince` (method, line 82) ← call from `src/world/doc.ts#RoadDoc.buildingsMovedIn`
- `src/world/buildings/store.ts#BuildingStore.forkFrom` (method, line 363) ← call from `src/world/doc.ts#RoadDoc.layOver`
- `src/world/buildings/store.ts#BuildingStore.get` (method, line 115) ← call from `src/actionsWiring.ts#createActions.breakOne`, `src/actionsWiring.ts#createActions.strikeAt`, `src/actionsWiring.ts#createActions.tick`, `src/editor/chunkedSave.ts#KEYED`, …
- `src/world/buildings/store.ts#BuildingStore.has` (method, line 119) ← call from `src/actionsWiring.ts#createActions`, `src/actionsWiring.ts#createActions.explodeAt`, `src/world/doc.ts#RoadDoc.fromJSON`, `src/world/doc.ts#RoadDoc.openLots`
- `src/world/buildings/store.ts#BuildingStore.has` (method, line 119) ← typeRef from `src/world/doc.ts#RoadDoc.fromJSON`
- `src/world/buildings/store.ts#BuildingStore.idList` (method, line 128) ← call from `src/sim/agents/trafficBuildings.ts#docBuildings.ids`, `src/world/solids.ts#createSolids`
- `src/world/buildings/store.ts#BuildingStore.internShared` (field, line 44) ← write from `src/editor/smallCityMap.ts#growCity`
- `src/world/buildings/store.ts#BuildingStore.load` (method, line 330) ← call from `src/world/doc.ts#RoadDoc.fromJSON`
- `src/world/buildings/store.ts#BuildingStore.loadPrepared` (method, line 275) ← call from `src/world/doc.ts#RoadDoc.fromJSON`
- `src/world/buildings/store.ts#BuildingStore.loadSections` (method, line 290) ← call from `src/world/doc.ts#RoadDoc.fromJSON`
- `src/world/buildings/store.ts#BuildingStore.near` (method, line 133) ← call from `src/actionsWiring.ts#createActions.buildingsUnderRay`, `src/actionsWiring.ts#createActions.explodeAt`, `src/editor/bulldozer.ts#Bulldozer.boxed`, `src/editor/transitTools.ts#TransitTool.built`, …
- `src/world/buildings/store.ts#BuildingStore.nearAny` (method, line 140) ← call from `src/render/renderer.ts#createSceneRenderer.buildingsOnGroundSince`, `src/render/renderer.ts#createSceneRenderer.shapeBlocksSteps`, `src/sim/agents/trafficBuildings.ts#docBuildings.nearAny`
- `src/world/buildings/store.ts#BuildingStore.nextId` (getter, line 148) ← call from `src/world/doc.ts#RoadDoc.allocatorMarks`, `src/world/doc.ts#RoadDoc.reserveAllocators`
- `src/world/buildings/store.ts#BuildingStore.put` (method, line 167) ← call from `src/editor/smallCityMap.ts#growLots`, `src/main.ts#growthAnswered`, `src/main.ts#pointerDown`
- `src/world/buildings/store.ts#BuildingStore.remove` (method, line 246) ← call from `src/actionsWiring.ts#createActions`, `src/actionsWiring.ts#createActions.breakOne`, `src/actionsWiring.ts#createActions.explodeAt`, `src/editor/bulldozer.ts#Bulldozer.boxed`
- `src/world/buildings/store.ts#BuildingStore.replaceWith` (method, line 397) ← call from `src/world/doc.ts#RoadDoc.replaceContents`
- `src/world/buildings/store.ts#BuildingStore.restore` (method, line 191) ← call from `src/world/doc.ts#RoadDoc.restore`
- `src/world/buildings/store.ts#BuildingStore.restoreNextId` (method, line 241) ← call from `src/world/doc.ts#RoadDoc.reserveAllocators`, `src/world/doc.ts#RoadDoc.restoreValue`
- `src/world/buildings/store.ts#BuildingStore.revision` (getter, line 64) ← call from `src/main.ts#serializedDoc`, `src/render/flatAtlas.ts#createFlatAtlas.readContent`, `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.shapeBlocksSteps`, …
- `src/world/buildings/store.ts#BuildingStore.sections` (method, line 303) ← call from `src/sim/agents/trafficBuildings.ts#trafficMapOf`, `src/world/doc.ts#RoadDoc.image`
- `src/world/buildings/store.ts#BuildingStore.shareFrom` (method, line 345) ← call from `src/world/doc.ts#RoadDoc.fromJSON`
- `src/world/buildings/store.ts#BuildingStore.size` (getter, line 99) ← call from `src/main.ts#<IfStatement>@4036`, `src/main.ts#serializedDoc`, `src/render/playEffects.ts#createPlayEffects.groundChanged`, `src/render/renderer.ts#createSceneRenderer.shapeGround`, …
- `src/world/buildings/store.ts#BuildingStore.spillToTable` (method, line 47) ← call from `src/editor/smallCityMap.ts#growCity`
- `src/world/buildings/store.ts#BuildingStore.takeFork` (method, line 372) ← call from `src/world/doc.ts#RoadDoc.takeFork`
- `src/world/buildings/store.ts#BuildingStore.takeWhole` (method, line 314) ← call from `src/world/doc.ts#RoadDoc.replaceContents`
- `src/world/buildings/store.ts#BuildingStore.toJSON` (method, line 265) ← call from `src/world/doc.ts#RoadDoc.serialized`
- `src/world/buildings/store.ts#BuildingStore.toText` (method, line 270) ← call from `src/world/doc.ts#RoadDoc.toText`
- `src/world/buildings/store.ts#BuildingStore.watch` (method, line 59) ← call from `src/world/doc.ts#RoadDoc.listen`
- `src/world/buildings/store.ts#BuildingStore.wear` (method, line 179) ← call from `src/actionsWiring.ts#createActions.explodeAt`, `src/main.ts#decayLook`

### src/world/buildings/streetDoors.ts

- `src/world/buildings/streetDoors.ts#StreetDoor` (interface, line 9) ← read from `src/sim/agents/lotDoors.ts#streetWay`
- `src/world/buildings/streetDoors.ts#StreetDoor` (interface, line 9) ← typeRef from `src/sim/agents/trafficBuildings.ts#TrafficBuilding`
- `src/world/buildings/streetDoors.ts#streetDoors` (function, line 27) ← call from `src/sim/agents/trafficBuildings.ts#project`

### src/world/buildings/types.ts

- `src/world/buildings/types.ts#Building` (interface, line 284) ← read from `src/actionsWiring.ts#createActions.breakOne`, `src/actionsWiring.ts#createActions.explodeAt`, `src/actionsWiring.ts#createActions.rayOnBuilding`, `src/actionsWiring.ts#createActions.strikeAt`, …
- `src/world/buildings/types.ts#Building` (interface, line 284) ← typeRef from `src/actionsWiring.ts#Actions`, `src/actionsWiring.ts#createActions.buildingsUnderRay`, `src/actionsWiring.ts#createActions.explodeAt`, `src/actionsWiring.ts#createActions.rayOnBuilding`, …
- `src/world/buildings/types.ts#BuildingElement` (interface, line 259) ← read from `src/render/playEffects.ts#createPlayEffects`, `src/sim/agents/lorryDocks.ts#buildYard`, `src/sim/agents/lorryDocks.ts#elementRing`, `src/sim/agents/lorryDocks.ts#nearestGate`, …
- `src/world/buildings/types.ts#BuildingElement` (interface, line 259) ← typeRef from `src/sim/agents/lotBays.ts#carGates`, `src/sim/agents/lotBays.ts#personGates`, `src/sim/agents/lotNav.ts#LotPart`, `src/sim/agents/trafficBuildings.ts#partOf`, …
- `src/world/buildings/types.ts#BuildingFunction` (type, line 214) ← typeRef from `src/sim/agents/trafficBuildings.ts#TrafficBuilding`
- `src/world/buildings/types.ts#BuildingId` (type, line 9) ← typeRef from `src/actionsWiring.ts#createActions`, `src/actionsWiring.ts#createActions.breakOne`, `src/actionsWiring.ts#createActions.strikeAt`, `src/actionsWiring.ts#createActions.tick`, …
- `src/world/buildings/types.ts#BuildingUse` (type, line 16) ← typeRef from `src/sim/agents/trafficBuildings.ts#TrafficBuilding`
- `src/world/buildings/types.ts#ElementKind` (type, line 256) ← typeRef from `src/sim/agents/lorryDocks.ts#SOLID`, `src/sim/agents/lotNav.ts#LotGridOptions`, `src/sim/agents/lotNav.ts#PEOPLE_SOLID`, `src/sim/agents/lotNav.ts#SOLID`, …
- `src/world/buildings/types.ts#FACADE_PATTERNS` (variable, line 42) ← read from `src/ui/v2/shell.ts#mountShell`, `src/ui/v2/shell.ts#mountShell.renderBuild`, `src/ui/v2/shell.ts#mountShell.renderSection`
- `src/world/buildings/types.ts#MAX_STOREY_HEIGHT` (variable, line 336) ← read from `src/actionsWiring.ts#RAY_ABOVE`
- `src/world/buildings/types.ts#MAX_STOREYS` (variable, line 338) ← read from `src/actionsWiring.ts#RAY_ABOVE`
- `src/world/buildings/types.ts#RoofDetail` (interface, line 56) ← read from `src/main.ts#buildings`
- `src/world/buildings/types.ts#settleBuilding` (function, line 399) ← call from `src/editor/preparedCity.ts#PreparedCityWriter.add`
- `src/world/buildings/types.ts#Volume` (interface, line 150) ← read from `src/actionsWiring.ts#createActions.explodeAt`, `src/actionsWiring.ts#createActions.rayOnBuilding`, `src/editor/transitTools.ts#TransitTool.built`, `src/main.ts#buildings`, …
- `src/world/buildings/types.ts#Volume` (interface, line 150) ← typeRef from `src/sim/agents/lotBays.ts#LotBlock`, `src/sim/agents/trafficBuildings.ts#TrafficBlock`
- `src/world/buildings/types.ts#volumeTop` (function, line 447) ← call from `src/main.ts#buildings`

### src/world/buildings/validate.ts

- `src/world/buildings/validate.ts#ROAD_CLEARANCE` (variable, line 117) ← read from `src/editor/barriers.ts#snapBarrierPoint`
- `src/world/buildings/validate.ts#touchesRoad` (function, line 256) ← call from `src/editor/barriers.ts#barrierProblem`

### src/world/growth/rows.ts

- `src/world/growth/rows.ts#GrownRows` (interface, line 2) ← typeRef from `src/world/doc.ts#PreparedGrown`, `src/world/doc.ts#TileRecords`

### src/world/growth/version.ts

- `src/world/growth/version.ts#LOT_PLAN_VERSION` (variable, line 2) ← read from `src/editor/smallCityMap.ts#growLots`, `src/main.ts#growthAnswered`

### src/world/growth/zoning.ts

- `src/world/growth/zoning.ts#growOnLot` (function, line 483) ← call from `src/editor/smallCityMap.ts#growLots`

### src/world/lotRecord.ts

- `src/world/lotRecord.ts#insideLot` (function, line 16) ← call from `src/world/cityGen/squares.ts#planSquares`
- `src/world/lotRecord.ts#Lot` (interface, line 6) ← read from `src/editor/bulldozer.ts#Bulldozer.boxed`, `src/editor/chunkedSave.ts#KEYED`, `src/editor/cityGenerator.ts#zoneCity`, `src/render/flatAtlas.ts#createFlatAtlas.readWholeContent`, …
- `src/world/lotRecord.ts#Lot` (interface, line 6) ← typeRef from `src/editor/chunkedSave.ts#KEYED`, `src/editor/cityGenerator.ts#zoneCity`, `src/sim/agents/lorryDocks.ts#lotsNear`, `src/sim/agents/lorryDocks.ts#ownLot`, …

### src/world/lotRings.ts

- `src/world/lotRings.ts#lotCentre` (function, line 56) ← call from `src/editor/bulldozer.ts#Bulldozer.boxed`, `src/world/cityGen/squares.ts#blocks`, `src/world/cityGen/squares.ts#planSquares`

### src/world/lots.ts

- `src/world/lots.ts#applyLots` (function, line 895) ← call from `src/editor/smallCityMap.ts#growCity`
- `src/world/lots.ts#cutLotsByRoads` (function, line 1069) ← call from `src/main.ts#mutateBuilt`
- `src/world/lots.ts#deleteLot` (function, line 1027) ← call from `src/editor/bulldozer.ts#Bulldozer.boxed`, `src/editor/cityGenerator.ts#greenCity`
- `src/world/lots.ts#forgetLotPlans` (function, line 158) ← call from `src/editor/smallCityMap.ts#growCity`
- `src/world/lots.ts#isLot` (function, line 1247) ← call from `src/world/doc.ts#RoadDoc.fromJSON`
- `src/world/lots.ts#lotArea` (function, line 39) ← call from `src/world/cityGen/squares.ts#planSquares`
- `src/world/lots.ts#lotFrame` (function, line 47) ← call from `src/editor/cityGenerator.ts#zoneCity`
- `src/world/lots.ts#lotFrame` (function, line 47) ← read from `src/editor/cityGenerator.ts#zoneCity`
- `src/world/lots.ts#LotPlan` (interface, line 138) ← typeRef from `src/sim/host/simClient.ts#SimClient.lotPlan`, `src/sim/host/simHost.ts#SimHost.lotPlan`
- `src/world/lots.ts#planLots` (function, line 151) ← call from `src/editor/smallCityMap.ts#growCity`, `src/sim/host/simHost.ts#SimHost.lotPlan`
- `src/world/lots.ts#zoneLots` (function, line 1227) ← call from `src/editor/cityGenerator.ts#zoneCity`

### src/world/lotStore.ts

- `src/world/lotStore.ts#LotStore` (class, line 33) ← construct from `src/world/doc.ts#RoadDoc.lotStore`
- `src/world/lotStore.ts#LotStore.all` (method, line 91) ← call from `src/editor/chunkedSave.ts#KEYED`
- `src/world/lotStore.ts#LotStore.changedSince` (method, line 142) ← call from `src/world/doc.ts#RoadDoc.lotsMovedIn`
- `src/world/lotStore.ts#LotStore.forkFrom` (method, line 361) ← call from `src/world/doc.ts#RoadDoc.layOver`
- `src/world/lotStore.ts#LotStore.get` (method, line 82) ← call from `src/editor/chunkedSave.ts#KEYED`, `src/sim/host/simClient.ts#recordOf`, `src/world/doc.ts#RoadDoc.recordOf`
- `src/world/lotStore.ts#LotStore.hasKey` (method, line 121) ← call from `src/sim/host/simClient.ts#SimClient.keyChanges`
- `src/world/lotStore.ts#LotStore.in` (method, line 126) ← call from `src/world/doc.ts#RoadDoc.lotsIn`
- `src/world/lotStore.ts#LotStore.keys` (getter, line 117) ← call from `src/sim/agents/trafficBuildings.ts#trafficMapOf`, `src/sim/host/simClient.ts#SimClient.keyChanges`, `src/sim/host/simClient.ts#SimClient.markSent`, `src/sim/host/simClient.ts#SimClient.sendChanges`, …
- `src/world/lotStore.ts#LotStore.keysRewritten` (field, line 54) ← read from `src/sim/host/simClient.ts#SimClient.markSent`, `src/sim/host/simClient.ts#SimClient.sendChanges`
- `src/world/lotStore.ts#LotStore.list` (method, line 101) ← call from `src/world/doc.ts#RoadDoc.lots`
- `src/world/lotStore.ts#LotStore.load` (method, line 270) ← call from `src/sim/agents/trafficBuildings.ts#takeTrafficMap`, `src/world/doc.ts#RoadDoc.fromJSON`
- `src/world/lotStore.ts#LotStore.nextId` (getter, line 107) ← call from `src/world/doc.ts#RoadDoc.valueOf`
- `src/world/lotStore.ts#LotStore.orderOf` (method, line 96) ← call from `src/editor/chunkedSave.ts#KEYED`, `src/sim/agents/lorryDocks.ts#ownLot`
- `src/world/lotStore.ts#LotStore.recordingMap` (method, line 67) ← callbackAssign from `src/world/doc.ts#RoadDoc.listen.tell`
- `src/world/lotStore.ts#LotStore.replaceWith` (method, line 304) ← call from `src/world/doc.ts#RoadDoc.replaceContents`
- `src/world/lotStore.ts#LotStore.restore` (method, line 244) ← call from `src/world/doc.ts#RoadDoc.restore`
- `src/world/lotStore.ts#LotStore.restoreNextId` (method, line 112) ← call from `src/world/doc.ts#RoadDoc.restoreValue`
- `src/world/lotStore.ts#LotStore.size` (getter, line 78) ← call from `src/world/doc.ts#RoadDoc.serialized`
- `src/world/lotStore.ts#LotStore.spliceKeys` (method, line 231) ← call from `src/world/doc.ts#RoadDoc.restore`
- `src/world/lotStore.ts#LotStore.takeFork` (method, line 375) ← call from `src/world/doc.ts#RoadDoc.takeFork`
- `src/world/lotStore.ts#LotStore.takeWhole` (method, line 296) ← call from `src/world/doc.ts#RoadDoc.replaceContents`
- `src/world/lotStore.ts#LotStore.toJSON` (method, line 409) ← call from `src/sim/agents/trafficBuildings.ts#trafficMapOf`, `src/world/doc.ts#RoadDoc.serialized`
- `src/world/lotStore.ts#LotStore.watch` (method, line 60) ← call from `src/world/doc.ts#RoadDoc.listen`
- `src/world/lotStore.ts#LotStore.watch` (method, line 60) ← callbackAssign from `src/world/doc.ts#RoadDoc.listen.tell`

### src/world/openLots.ts

- `src/world/openLots.ts#OpenLots` (class, line 28) ← construct from `src/world/doc.ts#RoadDoc.openLots`
- `src/world/openLots.ts#OpenLots` (class, line 28) ← typeRef from `src/world/doc.ts#RoadDoc.openLots`, `src/world/doc.ts#RoadDoc.openLotsIndex`
- `src/world/openLots.ts#OpenLots.count` (method, line 59) ← call from `src/editor/smallCityMap.ts#growLots`

### src/world/zones.ts

- `src/world/zones.ts#isZoneDensity` (function, line 20) ← call from `src/world/doc.ts#RoadDoc.fromJSON`
- `src/world/zones.ts#isZoneMark` (function, line 35) ← call from `src/world/doc.ts#RoadDoc.fromJSON`
- `src/world/zones.ts#isZoneUse` (function, line 19) ← call from `src/world/doc.ts#RoadDoc.fromJSON`
- `src/world/zones.ts#Zone` (interface, line 7) ← read from `src/world/doc.ts#RoadDoc.fromJSON`, `src/world/doc.ts#RoadDoc.replaceContents`, `src/world/doc.ts#RoadDoc.worldPart`, `src/world/rescale.ts#rescaleSerializedDoc`
- `src/world/zones.ts#Zone` (interface, line 7) ← typeRef from `src/world/doc.ts#RoadDoc.zones`, `src/world/doc.ts#SerializedDoc`
- `src/world/zones.ts#ZoneDensity` (type, line 5) ← typeRef from `src/main.ts#gameState`, `src/world/cityGen/plan.ts#CityPlan`, `src/world/cityGen/plan.ts#planCity.zoneAt`
- `src/world/zones.ts#zoneDensityName` (function, line 23) ← call from `src/world/doc.ts#lotRecord`
- `src/world/zones.ts#ZoneMark` (interface, line 26) ← read from `src/world/doc.ts#RoadDoc.fromJSON`
- `src/world/zones.ts#ZoneMark` (interface, line 26) ← typeRef from `src/world/doc.ts#RoadDoc.zoneMarks`, `src/world/doc.ts#SerializedDoc`
- `src/world/zones.ts#ZoneUse` (type, line 4) ← typeRef from `src/main.ts#gameState`, `src/world/cityGen/plan.ts#CityPlan`, `src/world/cityGen/plan.ts#planCity.zoneAt`
- `src/world/zones.ts#zoneUseName` (function, line 22) ← call from `src/world/doc.ts#lotRecord`

## Shared declarations the feature uses

### src/bays.worker.ts

- `src/bays.worker.ts#serve` (function, line 5) — callbackAssign by 3: `src/growth.worker.ts#serve`, `src/render/buildings/cell.worker.ts#serve`, `src/render/buildings/fracture.worker.ts#serve`

### src/cityGen.worker.ts

- `src/cityGen.worker.ts#serve` (function, line 19) — callbackAssign by 3: `src/growth.worker.ts#serve`, `src/render/buildings/cell.worker.ts#serve`, `src/render/buildings/fracture.worker.ts#serve`

### src/conflictTable.worker.ts

- `src/conflictTable.worker.ts#serve` (function, line 13) — callbackAssign by 3: `src/growth.worker.ts#serve`, `src/render/buildings/cell.worker.ts#serve`, `src/render/buildings/fracture.worker.ts#serve`

### src/core/aabb.ts

- `src/core/aabb.ts#Aabb` (interface, line 3) — read by 15: `src/editor/buildings.ts#boxesMeet`, `src/editor/growthFeed.ts#junctionPiece`, `src/world/buildings/grid.ts#BuildingGrid.placeOf`, `src/world/buildings/pads.ts#shapeOverPads`, …
- `src/core/aabb.ts#Aabb` (interface, line 3) — typeRef by 14: `src/editor/buildings.ts#boxesMeet`, `src/editor/buildings.ts#weldInto`, `src/editor/growthReplica.ts#JunctionPiece`, `src/world/buildings/grid.ts#BuildingGrid.constructor`, …

### src/core/clipper.ts

- `src/core/clipper.ts#CLIPPER_SCALE` (variable, line 14) — read by 1: `src/world/poleLines.ts#markPlates`
- `src/core/clipper.ts#difference` (function, line 148) — call by 3: `src/world/lots.ts#cutLotsByRoads`, `src/world/lots.ts#LotPlanner.fitFree`, `src/world/lots.ts#onLand`
- `src/core/clipper.ts#intersection` (function, line 152) — call by 1: `src/world/growth/zoning.ts#fitToLot.clip`
- `src/core/clipper.ts#MultiPoly` (type, line 9) — typeRef by 1: `src/world/lots.ts#pavingNear`
- `src/core/clipper.ts#union` (function, line 143) — call by 1: `src/world/lots.ts#pavingNear`

### src/core/derivedCache.ts

- `src/core/derivedCache.ts#database` (function, line 11) — call by 2: `src/core/derivedCache.ts#readDerivedAll`, `src/core/derivedCache.ts#keepDerivedNow`
- `src/core/derivedCache.ts#forgetOtherDerived` (function, line 327) — call by 2: `src/render/buildings/cellPool.ts#CellPool.storedKey`, `src/render/buildings/parts.ts#createThumbnailStudio.keptPictures`
- `src/core/derivedCache.ts#opened` (variable, line 9) — read by 1: `src/core/derivedCache.ts#keepDerivedNow`
- `src/core/derivedCache.ts#prefixRange` (function, line 50) — call by 1: `src/core/derivedCache.ts#readDerivedAll`
- `src/core/derivedCache.ts#readDerived` (function, line 53) — call by 1: `src/render/buildings/cellPool.ts#CellPool.run`
- `src/core/derivedCache.ts#readDerivedKeys` (function, line 68) — call by 1: `src/render/buildings/cellPool.ts#keptCellKeys`
- `src/core/derivedCache.ts#RELAXED` (variable, line 5) — read by 1: `src/core/derivedCache.ts#keepDerivedNow`
- `src/core/derivedCache.ts#STORE` (variable, line 3) — read by 2: `src/core/derivedCache.ts#readDerivedAll`, `src/core/derivedCache.ts#keepDerivedNow`
- `src/core/derivedCache.ts#writeDerived` (function, line 250) — call by 1: `src/render/buildings/parts.ts#createThumbnailStudio.shoot`

### src/core/digest.ts

- `src/core/digest.ts#Digest` (class, line 6) — construct by 9: `src/render/buildings/cellPool.ts#CellPool.kitDigest`, `src/render/buildings/cellPool.ts#CellPool.storedKey`, `src/render/buildings/layer.ts#createBuildingLayer.digestOfCell`, `src/render/buildings/layer.ts#createBuildingLayer.entryFor`, …
- `src/core/digest.ts#Digest.add` (method, line 11) — call by 8: `src/render/buildings/cellPool.ts#CellPool.kitDigest`, `src/render/buildings/cellPool.ts#CellPool.storedKey`, `src/render/buildings/layer.ts#createBuildingLayer.digestOfCell`, `src/render/buildings/layer.ts#createBuildingLayer.entryFor`, …
- `src/core/digest.ts#Digest.addPlain` (method, line 31) — call by 4: `src/render/buildings/cellPool.ts#CellPool.kitDigest`, `src/render/buildings/townColumns.ts#memberDigest`, `src/render/buildings/townColumns.ts#recordDigests`, `src/render/gardenLists.ts#gardenLists.plantKeyOf`
- `src/core/digest.ts#Digest.addText` (method, line 25) — call by 2: `src/render/buildings/cellPool.ts#CellPool.storedKey`, `src/render/buildings/townColumns.ts#recordDigests`
- `src/core/digest.ts#Digest.value` (method, line 59) — call by 9: `src/render/buildings/cellPool.ts#CellPool.kitDigest`, `src/render/buildings/cellPool.ts#CellPool.storedKey`, `src/render/buildings/layer.ts#createBuildingLayer.digestOfCell`, `src/render/buildings/layer.ts#createBuildingLayer.entryFor`, …

### src/core/forkMap.ts

- `src/core/forkMap.ts#ForkMap` (class, line 2) — construct by 2: `src/world/buildings/store.ts#BuildingStore.forkFrom`, `src/world/lotStore.ts#LotStore.forkFrom`
- `src/core/forkMap.ts#ForkMap` (class, line 2) — typeRef by 2: `src/world/buildings/store.ts#BuildingStore.takeFork`, `src/world/lotStore.ts#LotStore.takeFork`
- `src/core/forkMap.ts#ForkMap.applyTo` (method, line 100) — call by 1: `src/world/lotStore.ts#LotStore.takeFork`
- `src/core/forkMap.ts#ForkMap.changes` (method, line 95) — call by 2: `src/world/buildings/store.ts#BuildingStore.takeFork`, `src/world/lotStore.ts#LotStore.takeFork`
- `src/core/forkMap.ts#forkWork` (variable, line 175) — read by 1: `src/world/lotStore.ts#LotStore.ownKeys`
- `src/core/forkMap.ts#forkWork` (variable, line 175) — write by 1: `src/world/lotStore.ts#LotStore.ownKeys`

### src/core/heap.ts

- `src/core/heap.ts#Heap` (class, line 3) — construct by 1: `src/world/buildings/decayWatch.ts#createDecayWatch`
- `src/core/heap.ts#Heap.clear` (method, line 13) — call by 1: `src/world/buildings/decayWatch.ts#createDecayWatch`
- `src/core/heap.ts#Heap.peekKey` (method, line 12) — call by 1: `src/world/buildings/decayWatch.ts#createDecayWatch`
- `src/core/heap.ts#Heap.pop` (method, line 24) — call by 1: `src/world/buildings/decayWatch.ts#createDecayWatch`
- `src/core/heap.ts#Heap.push` (method, line 14) — call by 1: `src/world/buildings/decayWatch.ts#createDecayWatch.file`

### src/core/hypot.ts

- `src/core/hypot.ts#hypot2` (function, line 7) — call by 5: `src/render/buildings/massing.ts#emitPolygonRoof`, `src/world/buildings/footprints.ts#offsetRing`, `src/world/buildings/footprints.ts#sideFrame`, `src/world/buildings/interior.ts#roomLights`, …

### src/core/identityMemo.ts

- `src/core/identityMemo.ts#identityMemo` (function, line 2) — call by 14: `src/render/buildings/buildingMesh.ts#looseParts`, `src/world/buildingGates.ts#entrancesOf`, `src/world/buildings/blocks.ts#resolveBlocks`, `src/world/buildings/footprints.ts#framesOf`, …

### src/core/idSlots.ts

- `src/core/idSlots.ts#EMPTY,FULL,GONE` (variable, line 3) — read by 1: `src/core/idSlots.ts#IdSlots.forEach`
- `src/core/idSlots.ts#grown` (function, line 127) — call by 6: `src/render/buildings/cellPool.ts#FloorColumns.constructor`, `src/render/buildings/townColumns.ts#TownColumns.constructor`, `src/world/buildings/cameraSolids.ts#SolidFiling.constructor`, `src/world/buildings/decayWatch.ts#createDecayWatch`, …
- `src/core/idSlots.ts#IdSlots` (class, line 15) — construct by 6: `src/render/buildings/cellPool.ts#FloorColumns.slots`, `src/render/buildings/townColumns.ts#TownColumns.slots`, `src/world/buildings/cameraSolids.ts#SolidFiling.filedIds`, `src/world/buildings/decayWatch.ts#createDecayWatch`, …
- `src/core/idSlots.ts#IdSlots.capacityNow` (field, line 27) — read by 1: `src/core/idSlots.ts#IdSlots.capacity`
- `src/core/idSlots.ts#IdSlots.clear` (method, line 97) — call by 4: `src/render/buildings/townColumns.ts#TownColumns.clear`, `src/world/buildings/cameraSolids.ts#SolidFiling.clear`, `src/world/buildings/decayWatch.ts#createDecayWatch`, `src/world/buildings/siteGrading.ts#siteGrading.forget`
- `src/core/idSlots.ts#IdSlots.delete` (method, line 75) — call by 7: `src/render/buildings/cellPool.ts#FloorColumns.retain`, `src/render/buildings/townColumns.ts#TownColumns.drop`, `src/world/buildings/cameraSolids.ts#SolidFiling.remove`, `src/world/buildings/decayWatch.ts#createDecayWatch`, …
- `src/core/idSlots.ts#IdSlots.get` (method, line 48) — call by 11: `src/render/buildings/cellPool.ts#FloorColumns.get`, `src/render/buildings/townColumns.ts#TownColumns.drop`, `src/render/buildings/townColumns.ts#TownColumns.known`, `src/render/buildings/townColumns.ts#TownColumns.slotOf`, …
- `src/core/idSlots.ts#IdSlots.keys` (field, line 16) — read by 1: `src/core/idSlots.ts#IdSlots.forEach`
- `src/core/idSlots.ts#IdSlots.live` (field, line 20) — read by 1: `src/core/idSlots.ts#IdSlots.size`
- `src/core/idSlots.ts#IdSlots.onGrow` (field, line 26) — callbackAssign by 6: `src/render/buildings/cellPool.ts#FloorColumns.constructor`, `src/render/buildings/townColumns.ts#TownColumns.constructor`, `src/world/buildings/cameraSolids.ts#SolidFiling.constructor`, `src/world/buildings/decayWatch.ts#createDecayWatch`, …
- `src/core/idSlots.ts#IdSlots.onGrow` (field, line 26) — implementation by 6: `src/render/buildings/cellPool.ts#FloorColumns.constructor`, `src/render/buildings/townColumns.ts#TownColumns.constructor`, `src/world/buildings/cameraSolids.ts#SolidFiling.constructor`, `src/world/buildings/decayWatch.ts#createDecayWatch`, …
- `src/core/idSlots.ts#IdSlots.onGrow` (field, line 26) — write by 6: `src/render/buildings/cellPool.ts#FloorColumns.constructor`, `src/render/buildings/townColumns.ts#TownColumns.constructor`, `src/world/buildings/cameraSolids.ts#SolidFiling.constructor`, `src/world/buildings/decayWatch.ts#createDecayWatch`, …
- `src/core/idSlots.ts#IdSlots.states` (field, line 18) — read by 1: `src/core/idSlots.ts#IdSlots.forEach`
- `src/core/idSlots.ts#IdSlots.take` (method, line 58) — call by 6: `src/render/buildings/cellPool.ts#FloorColumns.set`, `src/render/buildings/townColumns.ts#TownColumns.file`, `src/world/buildings/cameraSolids.ts#SolidFiling.add`, `src/world/buildings/decayWatch.ts#createDecayWatch.file`, …
- `src/core/idSlots.ts#IdSlots.values` (field, line 17) — read by 1: `src/core/idSlots.ts#IdSlots.forEach`

### src/core/intersect.ts

- `src/core/intersect.ts#segSeg` (function, line 21) — call by 1: `src/editor/lotTool.ts#segmentCrossesLot`
- `src/core/intersect.ts#segSeg` (function, line 21) — read by 1: `src/editor/lotTool.ts#segmentCrossesLot`

### src/core/keptList.ts

- `src/core/keptList.ts#KeptList` (class, line 2) — construct by 1: `src/editor/blueprintLibrary.ts#BlueprintLibrary.constructor`
- `src/core/keptList.ts#KeptList` (class, line 2) — typeRef by 1: `src/editor/blueprintLibrary.ts#BlueprintLibrary.kept`
- `src/core/keptList.ts#KeptList.read` (method, line 16) — call by 1: `src/editor/blueprintLibrary.ts#BlueprintLibrary.list`
- `src/core/keptList.ts#KeptList.update` (method, line 45) — call by 2: `src/editor/blueprintLibrary.ts#BlueprintLibrary.remove`, `src/editor/blueprintLibrary.ts#BlueprintLibrary.save`

### src/core/plain.ts

- `src/core/plain.ts#samePlain` (function, line 2) — call by 7: `src/editor/buildingTool.ts#BuildingTool.hoverElement`, `src/editor/buildingTool.ts#BuildingTool.hoverPrimitive`, `src/editor/buildingTool.ts#BuildingTool.pointerUp`, `src/editor/buildingTool.ts#BuildingTool.showDraft`, …

### src/core/polygon.ts

- `src/core/polygon.ts#convexHull` (function, line 54) — call by 1: `src/world/buildings/elements.ts#unsupportedElements`
- `src/core/polygon.ts#isSimple` (function, line 8) — call by 1: `src/world/buildings/footprints.ts#validOutline`
- `src/core/polygon.ts#pointInPolygon` (function, line 40) — call by 12: `src/render/buildings/partyWalls.ts#hiddenByCovers`, `src/world/buildings/elements.ts#elementClash`, `src/world/buildings/elements.ts#unsupportedElements`, `src/world/buildings/footprints.ts#containsPoint`, …
- `src/core/polygon.ts#signedArea` (function, line 29) — call by 15: `src/buildingsWiring.ts#createBuildingWiring.inspectorFields`, `src/editor/buildingPlans.ts#movePlanEdge`, `src/editor/buildingPlans.ts#movePlanVertex`, `src/editor/buildingPlans.ts#offsetPlan`, …

### src/core/polyline.ts

- `src/core/polyline.ts#Frame` (interface, line 7) — read by 5: `src/editor/buildingSnap.ts#cornerFlush`, `src/editor/buildingSnap.ts#snapToRoad`, `src/editor/lotTool.ts#LotTool.streetAngleNear`, `src/world/lots.ts#LotPlanner.streetPlan`, …
- `src/core/polyline.ts#Polyline` (class, line 36) — reference by 1: `src/editor/growthReplica.ts#RoadReplica.apply`
- `src/core/polyline.ts#Polyline` (class, line 36) — typeRef by 1: `src/world/buildings/validate.ts#RoadReachRibbon`
- `src/core/polyline.ts#Polyline.bbox` (getter, line 116) — call by 7: `src/world/buildings/validate.ts#touchesRoad`, `src/world/growth/zoning.ts#cornerOf.onStreet`, `src/world/lots.ts#LandPaving.tile`, `src/world/lots.ts#LotPlanner.planAll`, …
- `src/core/polyline.ts#Polyline.closestPoint` (method, line 330) — call by 4: `src/editor/buildingSnap.ts#cornerFlush`, `src/editor/buildingSnap.ts#snapToRoad`, `src/editor/lotTool.ts#LotTool.streetAngleNear`, `src/world/lots.ts#LotPlanner.streetPlan`
- `src/core/polyline.ts#Polyline.closestPoint` (method, line 330) — read by 4: `src/editor/buildingSnap.ts#cornerFlush`, `src/editor/buildingSnap.ts#snapToRoad`, `src/editor/lotTool.ts#LotTool.streetAngleNear`, `src/world/lots.ts#LotPlanner.streetPlan`
- `src/core/polyline.ts#Polyline.distanceTo` (method, line 359) — call by 3: `src/world/growth/zoning.ts#cornerOf.onStreet`, `src/world/zoneGrid.ts#pavedTester.onRoad`, `src/world/nearestRibbon.ts#nearestRibbon`
- `src/core/polyline.ts#Polyline.fromFlat` (method, line 65) — call by 1: `src/editor/growthReplica.ts#RoadReplica.apply`
- `src/core/polyline.ts#Polyline.length` (getter, line 123) — call by 3: `src/editor/buildingSnap.ts#snapToRoad`, `src/world/lots.ts#LotPlanner.streetPlan`, `src/world/zoneGrid.ts#zoneGridSteps`
- `src/core/polyline.ts#Polyline.n` (field, line 40) — read by 1: `src/world/zoneGrid.ts#markRoads`
- `src/core/polyline.ts#Polyline.sampleAt` (method, line 152) — call by 5: `src/editor/buildingSnap.ts#cornerFlush`, `src/editor/buildingSnap.ts#snapToRoad`, `src/editor/lotTool.ts#LotTool.streetAngleNear`, `src/world/lots.ts#LotPlanner.streetPlan`, …
- `src/core/polyline.ts#Polyline.toPoints` (method, line 74) — call by 1: `src/world/buildings/validate.ts#lineOf`
- `src/core/polyline.ts#Polyline.xy` (field, line 38) — read by 2: `src/editor/growthFeed.ts#roadPiece`, `src/world/zoneGrid.ts#markRoads`

### src/core/recordMap.ts

- `src/core/recordMap.ts#RecordMap` (class, line 2) — construct by 2: `src/world/lotStore.ts#LotStore.items`, `src/world/lotStore.ts#LotStore.recordingMap`
- `src/core/recordMap.ts#RecordMap` (class, line 2) — reference by 2: `src/world/lotStore.ts#LotStore.recordingMap`, `src/world/lotStore.ts#LotStore.watch`
- `src/core/recordMap.ts#RecordMap` (class, line 2) — typeRef by 1: `src/world/lotStore.ts#LotStore.recordingMap`
- `src/core/recordMap.ts#RecordMap.before` (field, line 4) — callbackAssign by 2: `src/world/lotStore.ts#LotStore.recordingMap`, `src/world/lotStore.ts#LotStore.watch`
- `src/core/recordMap.ts#RecordMap.before` (field, line 4) — implementation by 2: `src/world/lotStore.ts#LotStore.recordingMap`, `src/world/lotStore.ts#LotStore.watch`
- `src/core/recordMap.ts#RecordMap.before` (field, line 4) — read by 1: `src/world/lotStore.ts#LotStore.recordingMap`
- `src/core/recordMap.ts#RecordMap.before` (field, line 4) — write by 2: `src/world/lotStore.ts#LotStore.recordingMap`, `src/world/lotStore.ts#LotStore.watch`

### src/core/ring.ts

- `src/core/ring.ts#Ring.bbox` (getter, line 123) — call by 1: `src/editor/growthFeed.ts#junctionPiece`
- `src/core/ring.ts#Ring.flatten` (method, line 89) — call by 1: `src/editor/growthFeed.ts#junctionPiece`
- `src/core/ring.ts#Ring.isEmpty` (getter, line 84) — call by 1: `src/editor/growthFeed.ts#junctionPiece`

### src/core/rng.ts

- `src/core/rng.ts#Rng` (class, line 6) — construct by 5: `src/render/buildings/signature.ts#facade`, `src/render/buildings/signature.ts#makeMaterials`, `src/render/gardens.ts#gardenPack`, `src/world/growth/zoning.ts#lotStream`, …
- `src/core/rng.ts#Rng` (class, line 6) — reference by 3: `src/editor/growthReplica.ts#GrowthReplica.handle`, `src/world/growth/zoning.ts#replayOnLot`, `src/world/growth/zoning.ts#tryOnLot`
- `src/core/rng.ts#Rng` (class, line 6) — typeRef by 31: `src/world/buildings/architecture.ts#between`, `src/world/buildings/architecture.ts#composeFacades`, `src/world/buildings/architecture.ts#paintsOf`, `src/world/buildings/architecture.ts#pick`, …
- `src/core/rng.ts#Rng.float` (method, line 57) — call by 25: `src/render/buildings/signature.ts#facade`, `src/render/buildings/signature.ts#facade.brickWall`, `src/render/buildings/signature.ts#makeMaterials`, `src/render/buildings/signature.ts#makeMaterials.ashlar`, …
- `src/core/rng.ts#Rng.int` (method, line 67) — call by 9: `src/world/buildings/architecture.ts#pick`, `src/world/buildings/procedural.ts#flats`, `src/world/buildings/procedural.ts#house`, `src/world/buildings/procedural.ts#office`, …
- `src/core/rng.ts#Rng.s0` (field, line 7) — read by 2: `src/core/rng.ts#Rng.fromState`, `src/core/rng.ts#Rng.state`
- `src/core/rng.ts#Rng.s1` (field, line 8) — read by 2: `src/core/rng.ts#Rng.fromState`, `src/core/rng.ts#Rng.state`
- `src/core/rng.ts#Rng.s2` (field, line 9) — read by 2: `src/core/rng.ts#Rng.fromState`, `src/core/rng.ts#Rng.state`
- `src/core/rng.ts#Rng.s3` (field, line 10) — read by 2: `src/core/rng.ts#Rng.fromState`, `src/core/rng.ts#Rng.state`

### src/core/scalar.ts

- `src/core/scalar.ts#clamp` (function, line 58) — call by 15: `src/editor/buildings.ts#opAddSetback`, `src/editor/buildings.ts#opAddWing`, `src/editor/buildings.ts#opResize`, `src/editor/buildings.ts#opSetLevelHeight`, …
- `src/core/scalar.ts#EPS` (variable, line 6) — read by 1: `src/core/polyline.ts#segmentDistSq`

### src/core/units.ts

- `src/core/units.ts#m` (function, line 6) — call by 365: `src/buildingsWiring.ts#createBuildingWiring.drawGizmos`, `src/buildingsWiring.ts#createBuildingWiring.inspectorFields`, `src/buildingsWiring.ts#createBuildingWiring.runTool`, `src/editor/blocks.ts#blockGridLines`, …
- `src/core/units.ts#METERS_PER_UNIT` (variable, line 2) — read by 10: `src/buildingsWiring.ts#createBuildingWiring.drawOverlay`, `src/buildingsWiring.ts#createBuildingWiring.inspectorFields`, `src/buildingsWiring.ts#createBuildingWiring.setField`, `src/editor/blueprintLibrary.ts#BlueprintLibrary.save`, …
- `src/core/units.ts#perM` (function, line 9) — call by 1: `src/editor/buildingPlans.ts#offsetPlan`
- `src/core/units.ts#stepsIn` (function, line 15) — call by 9: `src/world/buildings/architecture.ts#columnsFor`, `src/world/buildings/interior.ts#kitchen`, `src/world/buildings/procedural.ts#baysOf`, `src/world/buildings/procedural.ts#house`, …
- `src/core/units.ts#TUNED_UNIT` (variable, line 12) — read by 2: `src/world/buildings/footprints.ts#AREA_EPS`, `src/world/buildings/geometry.ts#EPS`
- `src/core/units.ts#UNITS_PER_METER` (variable, line 3) — read by 1: `src/render/buildings/referenceModel.ts#createReferenceModel.apply`

### src/core/vec2.ts

- `src/core/vec2.ts#Vec2` (interface, line 4) — read by 198: `src/buildingsWiring.ts#createBuildingWiring`, `src/buildingsWiring.ts#createBuildingWiring.beforeDraw`, `src/editor/blocks.ts#blockGridLines.at`, `src/editor/buildingPlans.ts#addShapedUpperMass.atScale`, …
- `src/core/vec2.ts#Vec2` (interface, line 4) — typeRef by 275: `src/buildingsWiring.ts#BuildingWiring`, `src/editor/blocks.ts#blockGridLines`, `src/editor/blocks.ts#blockGridLines.at`, `src/editor/blocks.ts#commitBlockGrid`, …

### src/editor/commit.ts

- `src/editor/commit.ts#commitRoadPath` (function, line 63) — call by 1: `src/editor/blocks.ts#commitBlockGrid`
- `src/editor/commit.ts#DraftResult` (interface, line 44) — read by 1: `src/editor/blocks.ts#commitBlockGrid`

### src/editor/history.ts

- `src/editor/history.ts#History` (class, line 10) — typeRef by 1: `src/buildingsWiring.ts#BuildingWiringDeps`
- `src/editor/history.ts#History.begin` (method, line 39) — call by 1: `src/buildingsWiring.ts#createBuildingWiring`
- `src/editor/history.ts#History.canRedo` (getter, line 34) — call by 1: `src/buildingsWiring.ts#createBuildingWiring.state`
- `src/editor/history.ts#History.canUndo` (getter, line 30) — call by 1: `src/buildingsWiring.ts#createBuildingWiring.state`
- `src/editor/history.ts#History.end` (method, line 46) — call by 1: `src/buildingsWiring.ts#createBuildingWiring`

### src/editor/save.worker.ts

- `src/editor/save.worker.ts#serve` (function, line 12) — callbackAssign by 3: `src/growth.worker.ts#serve`, `src/render/buildings/cell.worker.ts#serve`, `src/render/buildings/fracture.worker.ts#serve`

### src/editor/smallCityMap.ts

- `src/editor/smallCityMap.ts#growLots` (function, line 97) — callbackAssign by 1: `src/world/buildings/validate.ts#siteOf`
- `src/editor/smallCityMap.ts#growLots` (function, line 97) — implementation by 1: `src/world/buildings/validate.ts#siteOf`
- `src/editor/smallCityMap.ts#growLots.pavedAt` (function, line 101) — implementation by 1: `src/world/buildings/validate.ts#siteOf`

### src/editor/snap.ts

- `src/editor/snap.ts#Anchor` (interface, line 16) — read by 1: `src/editor/blocks.ts#commitBlockGrid`
- `src/editor/snap.ts#Anchor` (interface, line 16) — typeRef by 1: `src/editor/blocks.ts#commitBlockGrid`
- `src/editor/snap.ts#findAnchor` (function, line 84) — call by 1: `src/editor/blocks.ts#commitBlockGrid`
- `src/editor/snap.ts#ribbonsNear` (function, line 74) — call by 2: `src/editor/buildingSnap.ts#cornerFlush`, `src/editor/buildingSnap.ts#snapToRoad`

### src/job.worker.ts

- `src/job.worker.ts#<ExpressionStatement>` (statement, line 64) — callbackAssign by 3: `src/growth.worker.ts#serve`, `src/render/buildings/cell.worker.ts#serve`, `src/render/buildings/fracture.worker.ts#serve`
- `src/job.worker.ts#<ExpressionStatement>` (statement, line 64) — implementation by 3: `src/growth.worker.ts#serve`, `src/render/buildings/cell.worker.ts#serve`, `src/render/buildings/fracture.worker.ts#serve`
- `src/job.worker.ts#adopt` (function, line 40) — callbackAssign by 3: `src/growth.worker.ts#serve`, `src/render/buildings/cell.worker.ts#serve`, `src/render/buildings/fracture.worker.ts#serve`
- `src/job.worker.ts#adopt` (function, line 40) — implementation by 3: `src/growth.worker.ts#serve`, `src/render/buildings/cell.worker.ts#serve`, `src/render/buildings/fracture.worker.ts#serve`

### src/jobs/jobPool.ts

- `src/jobs/jobPool.ts#canQueueJobs` (function, line 534) — call by 3: `src/render/buildings/cellPool.ts#CellPool.constructor`, `src/render/buildings/cellPool.ts#CellPool.offPage`, `src/render/buildings/cellPool.ts#CellPool.warm`
- `src/jobs/jobPool.ts#grow` (function, line 293) — call by 1: `src/jobs/jobPool.ts#warmJobWorkers`
- `src/jobs/jobPool.ts#jobPort` (function, line 669) — call by 1: `src/growthPool.ts#GrowthPool.start`
- `src/jobs/jobPool.ts#JobSystem.workers` (field, line 231) — read by 1: `src/jobs/jobPool.ts#warmJobWorkers`
- `src/jobs/jobPool.ts#JobTaker` (interface, line 58) — read by 1: `src/render/buildings/cellPool.ts#CellPool.submit`
- `src/jobs/jobPool.ts#JobTicket` (interface, line 61) — call by 2: `src/render/buildings/cellPool.ts#CellPool.cancel`, `src/render/buildings/cellPool.ts#CellPool.coarseFirst@171`
- `src/jobs/jobPool.ts#JobTicket` (interface, line 61) — read by 1: `src/render/buildings/cellPool.ts#CellPool.coarseFirst@171`
- `src/jobs/jobPool.ts#JobTicket` (interface, line 61) — typeRef by 1: `src/render/buildings/cellPool.ts#Asked`
- `src/jobs/jobPool.ts#submitJob` (function, line 484) — call by 1: `src/render/buildings/cellPool.ts#CellPool.submit`
- `src/jobs/jobPool.ts#submitJob.cancel` (method, line 488) — implementation by 2: `src/render/buildings/cellPool.ts#CellPool.cancel`, `src/render/buildings/cellPool.ts#CellPool.coarseFirst@171`
- `src/jobs/jobPool.ts#system` (variable, line 249) — read by 1: `src/jobs/jobPool.ts#warmJobWorkers`

### src/jobs/jobScope.ts

- `src/jobs/jobScope.ts#JobScope` (interface, line 2) — call by 2: `src/growth.worker.ts#serve`, `src/render/buildings/cell.worker.ts#serve`
- `src/jobs/jobScope.ts#JobScope` (interface, line 2) — typeRef by 3: `src/growth.worker.ts#serve`, `src/render/buildings/cell.worker.ts#serve`, `src/render/buildings/fracture.worker.ts#serve`
- `src/jobs/jobScope.ts#JobScope` (interface, line 2) — write by 3: `src/growth.worker.ts#serve`, `src/render/buildings/cell.worker.ts#serve`, `src/render/buildings/fracture.worker.ts#serve`

### src/main.ts

- `src/main.ts#buildings` (variable, line 760) — implementation by 8: `src/buildingsWiring.ts#createBuildingWiring`, `src/buildingsWiring.ts#createBuildingWiring.beforeDraw`, `src/buildingsWiring.ts#createBuildingWiring.buildReference`, `src/buildingsWiring.ts#createBuildingWiring.insideClick`, …
- `src/main.ts#growth` (variable, line 1994) — callbackAssign by 1: `src/editor/growthFeed.ts#GrowthSession.send`
- `src/main.ts#growth` (variable, line 1994) — implementation by 2: `src/editor/growthFeed.ts#GrowthSession.send`, `src/world/buildings/validate.ts#siteOf`
- `src/main.ts#lotTool` (variable, line 530) — callbackAssign by 4: `src/editor/lotTool.ts#LotTool.down`, `src/editor/lotTool.ts#LotTool.overlay`, `src/editor/lotTool.ts#LotTool.refreshGesture`, `src/editor/lotTool.ts#LotTool.up`
- `src/main.ts#lotTool` (variable, line 530) — implementation by 11: `src/editor/lotTool.ts#LotTool.adoptProposal`, `src/editor/lotTool.ts#LotTool.advanceProposal`, `src/editor/lotTool.ts#LotTool.commitStroke`, `src/editor/lotTool.ts#LotTool.down`, …

### src/networkBake.worker.ts

- `src/networkBake.worker.ts#serve` (function, line 32) — callbackAssign by 3: `src/growth.worker.ts#serve`, `src/render/buildings/cell.worker.ts#serve`, `src/render/buildings/fracture.worker.ts#serve`

### src/render/cellInstances.ts

- `src/render/cellInstances.ts#CellInstances` (interface, line 124) — call by 4: `src/render/gardens.ts#createGardens.cull`, `src/render/gardens.ts#createGardens.dispose`, `src/render/gardens.ts#createGardens.setMap`, `src/render/gardens.ts#createGardens.update`
- `src/render/cellInstances.ts#CellInstances` (interface, line 124) — read by 1: `src/render/gardens.ts#createGardens`
- `src/render/cellInstances.ts#createCellInstances` (function, line 447) — call by 1: `src/render/gardens.ts#createGardens`
- `src/render/cellInstances.ts#createCellInstances.cull` (method, line 1067) — implementation by 1: `src/render/gardens.ts#createGardens.cull`
- `src/render/cellInstances.ts#createCellInstances.dispose` (method, line 1123) — implementation by 1: `src/render/gardens.ts#createGardens.dispose`
- `src/render/cellInstances.ts#createCellInstances.setMap` (method, line 1052) — implementation by 1: `src/render/gardens.ts#createGardens.setMap`
- `src/render/cellInstances.ts#createCellInstances.update` (method, line 1010) — implementation by 1: `src/render/gardens.ts#createGardens.update`
- `src/render/cellInstances.ts#KindSpec` (interface, line 88) — typeRef by 2: `src/render/gardens.ts#gardenKinds`, `src/render/gardens.ts#gardenKinds.kind`
- `src/render/cellInstances.ts#Pack` (interface, line 111) — typeRef by 2: `src/render/gardens.ts#gardenPack`, `src/render/gardens.ts#gardenPacksAbout`
- `src/render/cellInstances.ts#SlotFormat` (interface, line 96) — read by 1: `src/render/gardens.ts#POSE`

### src/render/citizenBake.worker.ts

- `src/render/citizenBake.worker.ts#serve` (function, line 89) — callbackAssign by 3: `src/growth.worker.ts#serve`, `src/render/buildings/cell.worker.ts#serve`, `src/render/buildings/fracture.worker.ts#serve`

### src/render/depthConvention.ts

- `src/render/depthConvention.ts#pullTowardEye` (function, line 28) — call by 1: `src/render/buildings/kit.ts#createBuildingKit`

### src/render/derivedWriter.worker.ts

- `src/render/derivedWriter.worker.ts#serve` (function, line 23) — callbackAssign by 3: `src/growth.worker.ts#serve`, `src/render/buildings/cell.worker.ts#serve`, `src/render/buildings/fracture.worker.ts#serve`

### src/render/ecology.worker.ts

- `src/render/ecology.worker.ts#serve` (function, line 8) — callbackAssign by 3: `src/growth.worker.ts#serve`, `src/render/buildings/cell.worker.ts#serve`, `src/render/buildings/fracture.worker.ts#serve`

### src/render/effectSwitches.ts

- `src/render/effectSwitches.ts#BuildingChoices` (interface, line 29) — read by 1: `src/render/buildingSwitches.ts#createBuildingSwitcher.beforeRender`
- `src/render/effectSwitches.ts#BuildingChoices` (interface, line 29) — typeRef by 1: `src/render/buildingSwitches.ts#BuildingSwitcher`
- `src/render/effectSwitches.ts#BuildingSwitch` (type, line 19) — typeRef by 4: `src/render/buildingSwitches.ts#KIND_SWITCH`, `src/render/buildingSwitches.ts#KIND_SWITCHES`, `src/render/buildingSwitches.ts#kindOf`, `src/render/buildingSwitches.ts#ROOT_SWITCH`
- `src/render/effectSwitches.ts#createFrameHider.hide` (method, line 52) — implementation by 1: `src/render/buildingSwitches.ts#createBuildingSwitcher.beforeRender`
- `src/render/effectSwitches.ts#EffectSwitches` (type, line 21) — typeRef by 1: `src/render/buildingSwitches.ts#BuildingSwitcher`
- `src/render/effectSwitches.ts#FrameHider` (interface, line 44) — call by 1: `src/render/buildingSwitches.ts#createBuildingSwitcher.beforeRender`
- `src/render/effectSwitches.ts#FrameHider` (interface, line 44) — typeRef by 1: `src/render/buildingSwitches.ts#BuildingSwitcher`

### src/render/flatGround.worker.ts

- `src/render/flatGround.worker.ts#serve` (function, line 7) — callbackAssign by 3: `src/growth.worker.ts#serve`, `src/render/buildings/cell.worker.ts#serve`, `src/render/buildings/fracture.worker.ts#serve`

### src/render/gpuLedger.ts

- `src/render/gpuLedger.ts#geometryCost` (function, line 73) — call by 5: `src/render/buildings/massCells.ts#createMassCells.evictLevels`, `src/render/buildings/massCells.ts#createMassCells.landBase`, `src/render/buildings/massCells.ts#createMassCells.massTriangles`, `src/render/buildings/massCells.ts#createMassCells.stand`, …
- `src/render/gpuLedger.ts#geometryCost` (function, line 73) — read by 5: `src/render/buildings/massCells.ts#createMassCells.evictLevels`, `src/render/buildings/massCells.ts#createMassCells.landBase`, `src/render/buildings/massCells.ts#createMassCells.massTriangles`, `src/render/buildings/massCells.ts#createMassCells.stand`, …
- `src/render/gpuLedger.ts#GpuClass` (type, line 7) — typeRef by 5: `src/render/buildings/layer.ts#DETAIL_CLASSES`, `src/render/buildings/layer.ts#detailClass`, `src/render/buildings/massCells.ts#levelClass`, `src/render/buildings/massCells.ts#sendUp`, …
- `src/render/gpuLedger.ts#gpuLedger` (variable, line 70) — read by 8: `src/render/buildings/layer.ts#createBuildingLayer.counted`, `src/render/buildings/layer.ts#createBuildingLayer.room`, `src/render/buildings/massCells.ts#createMassCells.drop`, `src/render/buildings/massCells.ts#createMassCells.dropBase`, …
- `src/render/gpuLedger.ts#gpuLedger` (variable, line 70) — write by 3: `src/render/buildings/layer.ts#createBuildingLayer.counted`, `src/render/buildings/massCells.ts#createMassCells.landBase`, `src/render/buildings/massCells.ts#sendUp`
- `src/render/gpuLedger.ts#GpuLedger.bytes` (method, line 44) — call by 2: `src/render/buildings/massCells.ts#createMassCells.evictLevels`, `src/render/gpuLedger.ts#GpuLedger.roomFor`
- `src/render/gpuLedger.ts#GpuLedger.cap` (field, line 17) — read by 1: `src/render/gpuLedger.ts#GpuLedger.roomFor`
- `src/render/gpuLedger.ts#GpuLedger.remove` (method, line 32) — call by 4: `src/render/buildings/layer.ts#createBuildingLayer.counted`, `src/render/buildings/massCells.ts#createMassCells.drop`, `src/render/buildings/massCells.ts#createMassCells.dropBase`, `src/render/buildings/massCells.ts#createMassCells.dropMesh`
- `src/render/gpuLedger.ts#GpuLedger.set` (method, line 20) — call by 3: `src/render/buildings/layer.ts#createBuildingLayer.counted`, `src/render/buildings/massCells.ts#createMassCells.landBase`, `src/render/buildings/massCells.ts#sendUp`
- `src/render/gpuLedger.ts#GpuLedger.sum` (field, line 15) — read by 1: `src/render/gpuLedger.ts#GpuLedger.roomFor`

### src/render/groundChanges.ts

- `src/render/groundChanges.ts#Area` (type, line 70) — typeRef by 1: `src/render/groundChanges.ts#GroundChanges.touches`
- `src/render/groundChanges.ts#GroundChanges.forgotten` (field, line 21) — read by 1: `src/render/groundChanges.ts#GroundChanges.touches`
- `src/render/groundChanges.ts#GroundChanges.log` (field, line 20) — read by 1: `src/render/groundChanges.ts#GroundChanges.touches`
- `src/render/groundChanges.ts#GroundChanges.rectsSince` (method, line 56) — implementation by 1: `src/render/gardenLists.ts#gardenLists.groundMoved`
- `src/render/groundChanges.ts#GroundChanges.serial` (field, line 18) — read by 1: `src/render/groundChanges.ts#GroundChanges.touches`
- `src/render/groundChanges.ts#GroundRecord` (interface, line 10) — call by 2: `src/render/gardenLists.ts#gardenLists.check`, `src/render/gardenLists.ts#gardenLists.groundMoved`
- `src/render/groundChanges.ts#GroundRecord` (interface, line 10) — read by 2: `src/render/gardenLists.ts#gardenLists.groundMoved`, `src/render/gardenLists.ts#gardenLists.update`
- `src/render/groundChanges.ts#GroundRecord` (interface, line 10) — reference by 1: `src/render/gardenLists.ts#gardenLists.groundMoved`
- `src/render/groundChanges.ts#GroundRecord` (interface, line 10) — typeRef by 1: `src/render/gardenLists.ts#gardenLists`
- `src/render/groundChanges.ts#Rect` (type, line 4) — typeRef by 5: `src/render/gardenLists.ts#gardenLists`, `src/render/gardenLists.ts#Site`, `src/render/groundChanges.ts#GroundChanges.touches`, `src/render/groundChanges.ts#isRect`, …
- `src/render/groundChanges.ts#unionRect` (function, line 77) — call by 1: `src/render/groundChanges.ts#GroundChanges.touches`

### src/render/groundShape.worker.ts

- `src/render/groundShape.worker.ts#serve` (function, line 5) — callbackAssign by 3: `src/growth.worker.ts#serve`, `src/render/buildings/cell.worker.ts#serve`, `src/render/buildings/fracture.worker.ts#serve`

### src/render/instanceRuns.ts

- `src/render/instanceRuns.ts#NEVER_NEAR` (variable, line 39) — read by 1: `src/render/gardens.ts#gardenPack.pack`
- `src/render/instanceRuns.ts#YAW_POSE` (variable, line 42) — read by 2: `src/render/gardens.ts#createGardens`, `src/render/gardens.ts#POSE`

### src/render/isoViewport.ts

- `src/render/isoViewport.ts#createIsoRig` (function, line 78) — implementation by 2: `src/buildingsWiring.ts#createBuildingWiring`, `src/buildingsWiring.ts#createBuildingWiring.beforeDraw`

### src/render/landStroke.worker.ts

- `src/render/landStroke.worker.ts#serve` (function, line 5) — callbackAssign by 3: `src/growth.worker.ts#serve`, `src/render/buildings/cell.worker.ts#serve`, `src/render/buildings/fracture.worker.ts#serve`

### src/render/lightLevels.ts

- `src/render/lightLevels.ts#lightPoolTexture` (function, line 39) — call by 1: `src/render/buildings/kit.ts#createBuildingKit`
- `src/render/lightLevels.ts#luminanceOf` (function, line 28) — call by 1: `src/render/buildings/kit.ts#createBuildingKit.setNight`
- `src/render/lightLevels.ts#NIGHT_LUMINANCE` (variable, line 9) — read by 1: `src/render/buildings/kit.ts#createBuildingKit.setNight`

### src/render/mesh/gpuOnly.ts

- `src/render/mesh/gpuOnly.ts#gpuOnly` (function, line 45) — call by 1: `src/render/buildings/massCells.ts#cellMesh`
- `src/render/mesh/gpuOnly.ts#gpuOnlyAttributes` (function, line 22) — call by 3: `src/render/buildings/buildingMesh.ts#meshesOf`, `src/render/buildings/massCells.ts#baseMesh`, `src/render/buildings/partCells.ts#PartCell.constructor`

### src/render/mesh/hiddenFaces.ts

- `src/render/mesh/hiddenFaces.ts#dropHiddenFaces` (function, line 22) — call by 5: `src/render/buildings/facadePropModels.ts#merged`, `src/render/buildings/furnitureKit.ts#createFurnitureGeometries`, `src/render/buildings/furnitureKit.ts#furnitureParts`, `src/render/buildings/kit.ts#mergedParts`, …

### src/render/mesh/textureBaker.ts

- `src/render/mesh/textureBaker.ts#fbm` (function, line 32) — call by 1: `src/render/bake/finishRecipes.ts#LOOKS`
- `src/render/mesh/textureBaker.ts#makeNoise` (function, line 4) — call by 1: `src/render/bake/finishRecipes.ts#LOOKS`
- `src/render/mesh/textureBaker.ts#SurfaceRecipe` (interface, line 81) — typeRef by 1: `src/render/bake/finishRecipes.ts#Shade`
- `src/render/mesh/textureBaker.ts#SurfaceRecipe` (interface, line 81) — write by 1: `src/render/bake/finishRecipes.ts#LOOKS`

### src/render/mesh/weld.ts

- `src/render/mesh/weld.ts#weldGeometry` (function, line 5) — call by 3: `src/render/buildings/facadePropModels.ts#tone`, `src/render/buildings/furnitureKit.ts#paint`, `src/render/buildings/kit.ts#simplerLevels`

### src/render/networkFarRoads.worker.ts

- `src/render/networkFarRoads.worker.ts#serve` (function, line 5) — callbackAssign by 3: `src/growth.worker.ts#serve`, `src/render/buildings/cell.worker.ts#serve`, `src/render/buildings/fracture.worker.ts#serve`

### src/render/people/lodWorker.ts

- `src/render/people/lodWorker.ts#serve` (function, line 82) — callbackAssign by 3: `src/growth.worker.ts#serve`, `src/render/buildings/cell.worker.ts#serve`, `src/render/buildings/fracture.worker.ts#serve`

### src/render/plantImpostors.ts

- `src/render/plantImpostors.ts#createPlantImpostors.attach` (method, line 444) — implementation by 1: `src/render/gardens.ts#gardenKinds.kind`
- `src/render/plantImpostors.ts#createPlantImpostors.ready` (method, line 443) — implementation by 1: `src/render/gardens.ts#gardenKinds.kind`
- `src/render/plantImpostors.ts#createPlantImpostors.setView` (method, line 439) — implementation by 1: `src/render/gardens.ts#gardenKinds.kind`
- `src/render/plantImpostors.ts#PICTURE_PX` (variable, line 467) — read by 1: `src/render/gardens.ts#gardenKinds.kind`
- `src/render/plantImpostors.ts#PlantImpostors` (interface, line 255) — call by 1: `src/render/gardens.ts#gardenKinds.kind`
- `src/render/plantImpostors.ts#PlantImpostors` (interface, line 255) — read by 1: `src/render/gardens.ts#gardenKinds.kind`
- `src/render/plantImpostors.ts#PlantImpostors` (interface, line 255) — reference by 1: `src/render/gardens.ts#gardenKinds.kind`
- `src/render/plantImpostors.ts#sceneryPictures` (function, line 487) — call by 2: `src/render/gardens.ts#createGardenCells`, `src/render/gardens.ts#createGardens`
- `src/render/plantImpostors.ts#SceneryPictures` (interface, line 470) — call by 4: `src/render/gardens.ts#createGardenCells.dispose`, `src/render/gardens.ts#createGardens.dispose`, `src/render/gardens.ts#gardenKinds`, `src/render/gardens.ts#gardenPictures`
- `src/render/plantImpostors.ts#SceneryPictures` (interface, line 470) — read by 2: `src/render/gardens.ts#createGardenCells`, `src/render/gardens.ts#gardenKinds`
- `src/render/plantImpostors.ts#SceneryPictures` (interface, line 470) — typeRef by 2: `src/render/gardens.ts#gardenKinds`, `src/render/gardens.ts#gardenPictures`
- `src/render/plantImpostors.ts#sceneryPictures.bushAt` (method, line 502) — implementation by 2: `src/render/gardens.ts#gardenKinds`, `src/render/gardens.ts#gardenPictures`
- `src/render/plantImpostors.ts#sceneryPictures.release` (method, line 503) — implementation by 2: `src/render/gardens.ts#createGardenCells.dispose`, `src/render/gardens.ts#createGardens.dispose`
- `src/render/plantImpostors.ts#sceneryPictures.treeAt` (method, line 501) — implementation by 2: `src/render/gardens.ts#gardenKinds`, `src/render/gardens.ts#gardenPictures`

### src/render/propCells.ts

- `src/render/propCells.ts#createPropLayer` (function, line 254) — call by 1: `src/render/gardens.ts#createGardenCells`
- `src/render/propCells.ts#createPropLayer.cull` (method, line 591) — implementation by 1: `src/render/gardens.ts#createGardenCells.cull`
- `src/render/propCells.ts#createPropLayer.dispose` (method, line 596) — implementation by 1: `src/render/gardens.ts#createGardenCells.dispose`
- `src/render/propCells.ts#createPropLayer.follow` (method, line 511) — implementation by 1: `src/render/gardens.ts#createGardenCells.follow`
- `src/render/propCells.ts#createPropLayer.setMap` (method, line 590) — implementation by 1: `src/render/gardens.ts#createGardenCells.setMap`
- `src/render/propCells.ts#PropLayer` (interface, line 187) — call by 4: `src/render/gardens.ts#createGardenCells.cull`, `src/render/gardens.ts#createGardenCells.dispose`, `src/render/gardens.ts#createGardenCells.follow`, `src/render/gardens.ts#createGardenCells.setMap`
- `src/render/propCells.ts#PropLayer` (interface, line 187) — read by 2: `src/render/gardens.ts#createGardenCells`, `src/render/gardens.ts#createGardenCells.source`
- `src/render/propCells.ts#PropLayer` (interface, line 187) — typeRef by 1: `src/render/gardens.ts#GardenCells`
- `src/render/propCells.ts#PropLayer` (interface, line 187) — write by 1: `src/render/gardens.ts#createGardenCells.source`

### src/render/propGeometry.ts

- `src/render/propGeometry.ts#BUSH_KINDS` (variable, line 158) — read by 4: `src/render/gardens.ts#gardenKinds`, `src/render/gardens.ts#gardenPack`, `src/render/gardens.ts#gardenPictures`, `src/render/gardens.ts#KIND_KEYS`
- `src/render/propGeometry.ts#TREE_SPECIES` (variable, line 145) — read by 4: `src/render/gardens.ts#gardenKinds`, `src/render/gardens.ts#gardenPack`, `src/render/gardens.ts#gardenPictures`, `src/render/gardens.ts#KIND_KEYS`
- `src/render/propGeometry.ts#TreeSpecies` (type, line 144) — typeRef by 3: `src/render/gardens.ts#gardenPack`, `src/render/gardens.ts#GardenPlant`, `src/render/gardens.ts#gardenSpecies`

### src/render/renderer.ts

- `src/render/renderer.ts#createSceneRenderer` (function, line 432) — callbackAssign by 1: `src/buildingsWiring.ts#createBuildingWiring`
- `src/render/renderer.ts#createSceneRenderer` (function, line 432) — implementation by 9: `src/buildingsWiring.ts#createBuildingWiring`, `src/buildingsWiring.ts#createBuildingWiring.beforeDraw`, `src/buildingsWiring.ts#createBuildingWiring.deactivate`, `src/buildingsWiring.ts#createBuildingWiring.referenceTarget`, …
- `src/render/renderer.ts#createSceneRenderer.floorBox` (function, line 1690) — indirectCall by 2: `src/world/buildings/cameraSolids.ts#cameraSolidsKeeper.follow`, `src/world/buildings/floors.ts#buildingFloors.follow`
- `src/render/renderer.ts#SceneHandle` (interface, line 227) — call by 4: `src/buildingsWiring.ts#createBuildingWiring`, `src/buildingsWiring.ts#createBuildingWiring.beforeDraw`, `src/buildingsWiring.ts#createBuildingWiring.deactivate`, `src/buildingsWiring.ts#createBuildingWiring.referenceTarget`
- `src/render/renderer.ts#SceneHandle` (interface, line 227) — read by 2: `src/buildingsWiring.ts#createBuildingWiring`, `src/buildingsWiring.ts#createBuildingWiring.requestThumbnails`
- `src/render/renderer.ts#SceneHandle` (interface, line 227) — typeRef by 1: `src/buildingsWiring.ts#BuildingWiringDeps`

### src/render/roadStations.worker.ts

- `src/render/roadStations.worker.ts#serve` (function, line 15) — callbackAssign by 4: `src/growth.worker.ts#serve`, `src/render/buildings/cell.worker.ts#serve`, `src/render/buildings/cellWork.ts#workerReads`, `src/render/buildings/fracture.worker.ts#serve`

### src/render/roadTile.worker.ts

- `src/render/roadTile.worker.ts#serve` (function, line 102) — callbackAssign by 3: `src/growth.worker.ts#serve`, `src/render/buildings/cell.worker.ts#serve`, `src/render/buildings/fracture.worker.ts#serve`

### src/render/scenery.ts

- `src/render/scenery.ts#foliageTint` (function, line 373) — call by 1: `src/render/gardens.ts#gardenPack`
- `src/render/scenery.ts#SceneryKit` (interface, line 131) — read by 1: `src/render/gardens.ts#gardenKinds`
- `src/render/scenery.ts#SceneryKit` (interface, line 131) — typeRef by 3: `src/render/gardens.ts#createGardenCells`, `src/render/gardens.ts#createGardens`, `src/render/gardens.ts#gardenKinds`
- `src/render/scenery.ts#SHADOW_REACH` (variable, line 363) — read by 2: `src/render/gardens.ts#createGardenCells`, `src/render/gardens.ts#createGardens`

### src/render/shadowCasters.ts

- `src/render/shadowCasters.ts#castBySize` (function, line 26) — call by 1: `src/render/buildings/flagLayer.ts#createFlagLayer.set`

### src/render/sharedGround.ts

- `src/render/sharedGround.ts#GroundReaders` (interface, line 30) — inherit by 1: `src/render/buildings/cellWork.ts#CellReads`
- `src/render/sharedGround.ts#GroundReaders` (interface, line 30) — read by 1: `src/render/buildings/cellWork.ts#workerReads`
- `src/render/sharedGround.ts#GroundShare.current` (getter, line 94) — call by 2: `src/render/buildings/cellPool.ts#CellPool.offPage`, `src/render/buildings/cellPool.ts#CellPool.write`
- `src/render/sharedGround.ts#SharedGroundAt` (interface, line 36) — read by 2: `src/render/buildings/cellPool.ts#CellPool.offPage`, `src/render/buildings/cellPool.ts#CellPool.write`
- `src/render/sharedGround.ts#SharedGroundAt` (interface, line 36) — typeRef by 2: `src/render/buildings/cellPool.ts#CellPool.offPage`, `src/render/buildings/cellPool.ts#CellPool.write`
- `src/render/sharedGround.ts#SharedGroundData` (interface, line 16) — read by 1: `src/render/sharedGround.ts#groundAround`
- `src/render/sharedGround.ts#SharedGroundData` (interface, line 16) — typeRef by 2: `src/render/buildings/cellWork.ts#CellJob`, `src/render/sharedGround.ts#groundAround`
- `src/render/sharedGround.ts#sharedGroundReaders` (function, line 112) — call by 1: `src/render/buildings/cellWork.ts#workerReads`
- `src/render/sharedGround.ts#sharedGroundReaders` (function, line 112) — callbackAssign by 1: `src/render/buildings/cellWork.ts#workerReads`
- `src/render/sharedGround.ts#sharedGroundReaders` (function, line 112) — implementation by 1: `src/render/buildings/cellWork.ts#workerReads`

### src/render/surfaceTextures.ts

- `src/render/surfaceTextures.ts#SurfaceBake` (interface, line 5) — read by 1: `src/render/buildings/finishes.ts#shellShader`
- `src/render/surfaceTextures.ts#SurfaceBake` (interface, line 5) — typeRef by 2: `src/render/buildings/finishes.ts#shellShader`, `src/render/surfaceTextures.ts#surfaceMaps`
- `src/render/surfaceTextures.ts#surfaceTextureOrNone` (function, line 66) — call by 1: `src/render/surfaceTextures.ts#surfaceMaps`

### src/render/terrainGrid.ts

- `src/render/terrainGrid.ts#TERRAIN_CELL` (variable, line 10) — read by 1: `src/render/buildings/buildingMesh.ts#LAWN_LATTICE`
- `src/render/terrainGrid.ts#TERRAIN_HALF` (variable, line 12) — read by 1: `src/render/buildings/buildingMesh.ts#LAWN_LATTICE`

### src/render/terrainLight.worker.ts

- `src/render/terrainLight.worker.ts#serve` (function, line 5) — callbackAssign by 3: `src/growth.worker.ts#serve`, `src/render/buildings/cell.worker.ts#serve`, `src/render/buildings/fracture.worker.ts#serve`

### src/render/uploads.ts

- `src/render/uploads.ts#cancelWarm` (function, line 455) — call by 4: `src/render/buildings/cellRequests.ts#disposeLanded`, `src/render/buildings/massCells.ts#createMassCells.drop`, `src/render/buildings/massCells.ts#createMassCells.dropBase`, `src/render/buildings/massCells.ts#createMassCells.dropMesh`
- `src/render/uploads.ts#gateUntilBuilt` (function, line 564) — call by 1: `src/render/buildings/partCells.ts#PartCell.makeCaster`
- `src/render/uploads.ts#meshBytes` (function, line 468) — call by 2: `src/render/buildings/massCells.ts#createMassCells.landBase`, `src/render/buildings/massCells.ts#createMassCells.massBytes`
- `src/render/uploads.ts#queuedBytes` (variable, line 410) — read by 1: `src/render/uploads.ts#warmFull`
- `src/render/uploads.ts#reservedBytes` (variable, line 422) — read by 2: `src/render/uploads.ts#warmFull`, `src/render/uploads.ts#reserveWarm`
- `src/render/uploads.ts#reservedBytes` (variable, line 422) — write by 1: `src/render/uploads.ts#reserveWarm`
- `src/render/uploads.ts#WARM_CEILING` (variable, line 412) — read by 1: `src/render/uploads.ts#warmFull`
- `src/render/uploads.ts#warmer` (variable, line 439) — read by 1: `src/render/uploads.ts#warmFull`
- `src/render/uploads.ts#warmLooseAhead` (function, line 445) — call by 3: `src/render/buildings/cellRequests.ts#CellRequests.land`, `src/render/buildings/massCells.ts#createMassCells.landBase`, `src/render/buildings/massCells.ts#sendUp`

### src/render/wind.ts

- `src/render/wind.ts#windUniforms` (variable, line 13) — read by 2: `src/render/buildings/flagLayer.ts#createFlagLayer.set`, `src/render/buildings/flagLayer.ts#material`

### src/sim/agents/buildingOrder.ts

- `src/sim/agents/buildingOrder.ts#StoreOrder.constructor` (constructor, line 21) — callbackAssign by 6: `src/render/buildings/cellPool.ts#FloorColumns.constructor`, `src/render/buildings/townColumns.ts#TownColumns.constructor`, `src/world/buildings/cameraSolids.ts#SolidFiling.constructor`, `src/world/buildings/decayWatch.ts#createDecayWatch`, …

### src/sim/ambient/ambientIndex.ts

- `src/sim/ambient/ambientIndex.ts#BuildingMix.constructor` (constructor, line 230) — callbackAssign by 6: `src/render/buildings/cellPool.ts#FloorColumns.constructor`, `src/render/buildings/townColumns.ts#TownColumns.constructor`, `src/world/buildings/cameraSolids.ts#SolidFiling.constructor`, `src/world/buildings/decayWatch.ts#createDecayWatch`, …
- `src/sim/ambient/ambientIndex.ts#SightWalls.constructor` (constructor, line 66) — callbackAssign by 6: `src/render/buildings/cellPool.ts#FloorColumns.constructor`, `src/render/buildings/townColumns.ts#TownColumns.constructor`, `src/world/buildings/cameraSolids.ts#SolidFiling.constructor`, `src/world/buildings/decayWatch.ts#createDecayWatch`, …

### src/sim/meso/demand.ts

- `src/sim/meso/demand.ts#DemandCells.constructor` (constructor, line 36) — callbackAssign by 6: `src/render/buildings/cellPool.ts#FloorColumns.constructor`, `src/render/buildings/townColumns.ts#TownColumns.constructor`, `src/world/buildings/cameraSolids.ts#SolidFiling.constructor`, `src/world/buildings/decayWatch.ts#createDecayWatch`, …

### src/ui/i18n/index.ts

- `src/ui/i18n/index.ts#applyTranslations` (function, line 114) — call by 1: `src/ui/builder/workspace.ts#initBuilderWorkspace`
- `src/ui/i18n/index.ts#formatDecimal` (function, line 79) — call by 5: `src/buildingsWiring.ts#createBuildingWiring.drawOverlay`, `src/buildingsWiring.ts#createBuildingWiring.inspectorFields`, `src/buildingsWiring.ts#createBuildingWiring.loadReference`, `src/ui/builder/workspace.ts#initBuilderWorkspace.renderInspector`, …
- `src/ui/i18n/index.ts#language` (function, line 50) — call by 1: `src/ui/builder/workspace.ts#initBuilderWorkspace.renderInspector`
- `src/ui/i18n/index.ts#onLanguageChange` (function, line 108) — call by 1: `src/ui/builder/workspace.ts#initBuilderWorkspace`
- `src/ui/i18n/index.ts#plural` (function, line 73) — call by 1: `src/buildingsWiring.ts#createBuildingWiring.drawOverlay`
- `src/ui/i18n/index.ts#t` (function, line 55) — call by 8: `src/buildingsWiring.ts#createBuildingWiring`, `src/buildingsWiring.ts#createBuildingWiring.drawOverlay`, `src/buildingsWiring.ts#createBuildingWiring.inspectorFields`, `src/buildingsWiring.ts#createBuildingWiring.materialLine`, …

### src/ui/overlay/palette.ts

- `src/ui/overlay/palette.ts#HOVER` (variable, line 5) — read by 1: `src/ui/overlay/buildingOverlay.ts#drawBuildingOverlay`
- `src/ui/overlay/palette.ts#INVALID` (variable, line 6) — read by 1: `src/ui/overlay/buildingOverlay.ts#drawBuildingOverlay`
- `src/ui/overlay/palette.ts#SELECTION` (variable, line 4) — read by 2: `src/ui/overlay/buildingOverlay.ts#drawBuildingOverlay`, `src/ui/overlay/buildingOverlay.ts#drawHandle`

### src/view/viewport.ts

- `src/view/viewport.ts#flatViewport.setOrbit` (method, line 80) — implementation by 1: `src/buildingsWiring.ts#createBuildingWiring`
- `src/view/viewport.ts#flatViewport.toScreen` (method, line 58) — implementation by 1: `src/buildingsWiring.ts#createBuildingWiring`
- `src/view/viewport.ts#flatViewport.toWorld` (method, line 55) — implementation by 1: `src/buildingsWiring.ts#createBuildingWiring.beforeDraw`
- `src/view/viewport.ts#flatViewport.toWorldAt` (method, line 57) — implementation by 1: `src/buildingsWiring.ts#createBuildingWiring`
- `src/view/viewport.ts#Viewport` (interface, line 8) — call by 2: `src/buildingsWiring.ts#createBuildingWiring`, `src/buildingsWiring.ts#createBuildingWiring.beforeDraw`
- `src/view/viewport.ts#Viewport` (interface, line 8) — read by 1: `src/buildingsWiring.ts#createBuildingWiring`
- `src/view/viewport.ts#Viewport` (interface, line 8) — typeRef by 1: `src/buildingsWiring.ts#BuildingWiringDeps`

### src/world/bounds.ts

- `src/world/bounds.ts#clampToWorld` (function, line 88) — call by 1: `src/world/buildings/serialize.ts#migrateBuilding`
- `src/world/bounds.ts#PLATE_SIZE` (variable, line 9) — read by 3: `src/render/lotOverlay.ts#FRAME_HALF`, `src/render/lotOverlay.ts#frameHalfFor`, `src/render/sharedGround.ts#groundAround`
- `src/world/bounds.ts#plateOf` (function, line 51) — call by 2: `src/render/lotOverlay.ts#createLotOverlay.standFrame`, `src/render/sharedGround.ts#groundAround`
- `src/world/bounds.ts#plateRegion` (function, line 54) — call by 1: `src/render/lotOverlay.ts#createLotOverlay.standFrame`
- `src/world/bounds.ts#PlateRegion` (interface, line 23) — read by 1: `src/render/lotOverlay.ts#createLotOverlay.standFrame`
- `src/world/bounds.ts#WORLD_HALF` (variable, line 81) — read by 1: `src/world/bounds.ts#insideWorld`

### src/world/cells.ts

- `src/world/cells.ts#CELL` (variable, line 9) — read by 8: `src/render/buildings/cellArrays.ts#cellShellGrid`, `src/render/buildings/cellArrays.ts#massGrid`, `src/render/buildings/cellArrays.ts#SHELL_GRID_STEP`, `src/render/buildings/farParts.ts#FarPartBatch.view`, …
- `src/world/cells.ts#CellFiling` (class, line 60) — construct by 9: `src/editor/growthReplica.ts#RoadReplica.kerbCells`, `src/editor/growthReplica.ts#RoadReplica.roadCells`, `src/editor/lotTool.ts#LotTool.proposedAt`, `src/render/buildings/layer.ts#createBuildingLayer`, …
- `src/world/cells.ts#CellFiling` (class, line 60) — typeRef by 3: `src/editor/lotTool.ts#LotTool.proposalFiling`, `src/world/lotStore.ts#LotStore.cells`, `src/world/lotStore.ts#LotStore.filing`
- `src/world/cells.ts#CellFiling.file` (method, line 76) — call by 11: `src/editor/growthReplica.ts#RoadReplica.apply`, `src/editor/lotTool.ts#LotTool.proposedAt`, `src/render/buildings/layer.ts#createBuildingLayer.putKept`, `src/render/gardenLists.ts#gardenLists.take`, …
- `src/world/cells.ts#CellFiling.idsMeeting` (method, line 131) — call by 13: `src/editor/growthFeed.ts#GrowthSession.syncRoads`, `src/editor/growthReplica.ts#RoadReplica.junctionCells`, `src/editor/growthReplica.ts#RoadReplica.segmentCells`, `src/editor/lotTool.ts#LotTool.proposedAt`, …
- `src/world/cells.ts#CellFiling.idsMeetingAny` (method, line 169) — call by 3: `src/render/gardenLists.ts#gardenLists.groundMoved`, `src/world/lots.ts#LotPlanner.planChanged`, `src/world/lots.ts#LotPlanner.run`
- `src/world/cells.ts#CellFiling.rectOf` (method, line 120) — call by 5: `src/editor/growthFeed.ts#junctionPiece`, `src/editor/growthFeed.ts#roadPiece`, `src/world/lots.ts#LotPlanner.planChanged`, `src/world/zoneGrid.ts#markRoads`, …
- `src/world/cells.ts#CellFiling.remove` (method, line 107) — call by 8: `src/editor/growthReplica.ts#RoadReplica.dropKerb`, `src/editor/growthReplica.ts#RoadReplica.dropRoad`, `src/render/buildings/layer.ts#createBuildingLayer.dropKept`, `src/render/buildings/layer.ts#createBuildingLayer.putKept`, …
- `src/world/cells.ts#grow` (function, line 33) — call by 6: `src/world/lots.ts#addPolygonLot`, `src/world/lots.ts#LandPaving.tile`, `src/world/lots.ts#LotPlanner.blockPlan`, `src/world/lots.ts#LotPlanner.planChanged`, …

### src/world/changes.ts

- `src/world/changes.ts#Change` (interface, line 35) — read by 10: `src/editor/buildingFloors.ts#BuildingFloors.follow`, `src/editor/lotTool.ts#LotTool.refreshLots`, `src/render/buildings/layer.ts#createBuildingLayer.update`, `src/world/buildings/decayWatch.ts#createDecayWatch`, …
- `src/world/changes.ts#Change` (interface, line 35) — typeRef by 1: `src/world/lots.ts#LotPlanner.planChanged`
- `src/world/changes.ts#ChangeJournal` (class, line 156) — construct by 2: `src/world/buildings/store.ts#BuildingStore.constructor`, `src/world/lotStore.ts#LotStore.constructor`
- `src/world/changes.ts#ChangeJournal` (class, line 156) — typeRef by 5: `src/render/buildings/layerCells.ts#CoverSource`, `src/render/gardens.ts#GardenCells`, `src/render/lotOverlay.ts#createLotOverlay.set`, `src/world/buildings/store.ts#BuildingStore.constructor`, …
- `src/world/changes.ts#ChangeJournal.record` (method, line 263) — call by 10: `src/world/buildings/store.ts#BuildingStore.changed`, `src/world/buildings/store.ts#BuildingStore.replaceWith`, `src/world/buildings/store.ts#BuildingStore.wear`, `src/world/lotStore.ts#LotStore.addKeys`, …
- `src/world/changes.ts#ChangeJournal.serialOf` (method, line 203) — call by 3: `src/world/buildings/store.ts#BuildingStore.revision`, `src/world/buildings/store.ts#BuildingStore.wearRevision`, `src/world/lotStore.ts#LotStore.revision`
- `src/world/changes.ts#ChangeJournal.since` (method, line 309) — call by 5: `src/render/buildings/layer.ts#createBuildingLayer.update`, `src/render/lotOverlay.ts#createLotOverlay.set`, `src/world/buildings/store.ts#BuildingStore.changedSince`, `src/world/lotStore.ts#LotStore.changedSince`, …
- `src/world/changes.ts#ChangeJournal.version` (getter, line 198) — call by 3: `src/render/buildings/layer.ts#createBuildingLayer.update`, `src/render/lotOverlay.ts#createLotOverlay.set`, `src/world/lots.ts#LotPlanner.update`
- `src/world/changes.ts#ChangeKind` (type, line 27) — typeRef by 3: `src/editor/buildingFloors.ts#KINDS`, `src/editor/growthFeed.ts#MOVES`, `src/render/lotOverlay.ts#GROUND_KINDS`
- `src/world/changes.ts#ChangeRect` (type, line 4) — typeRef by 57: `src/editor/buildings.ts#buildingsIn`, `src/editor/buildings.ts#clearBuildingsOnRoads`, `src/editor/growthFeed.ts#grow`, `src/editor/growthFeed.ts#GrowthSession.send`, …
- `src/world/changes.ts#isStreaming` (function, line 33) — call by 1: `src/world/lotStore.ts#LotStore.restore`
- `src/world/changes.ts#rectAround` (function, line 147) — call by 8: `src/world/growth/zoning.ts#commitGrown`, `src/world/growth/zoning.ts#growthReachOf`, `src/world/lotStore.ts#lotBox`, `src/world/lots.ts#addPolygonLot`, …

### src/world/derived.ts

- `src/world/derived.ts#JournalCursor` (class, line 6) — construct by 8: `src/editor/buildingFloors.ts#BuildingFloors.cursor`, `src/editor/buildingTool.ts#BuildingTool.judgedCursor`, `src/editor/growthFeed.ts#GrowthSession.moves`, `src/editor/lotTool.ts#LotTool.builtCursor`, …
- `src/world/derived.ts#JournalCursor` (class, line 6) — typeRef by 5: `src/world/buildings/store.ts#BuildingStore.changedFor`, `src/world/buildings/store.ts#BuildingStore.cursor`, `src/world/lotStore.ts#LotStore.changedFor`, `src/world/lotStore.ts#LotStore.cursor`, …
- `src/world/derived.ts#JournalCursor.read` (method, line 19) — call by 9: `src/editor/buildingFloors.ts#BuildingFloors.follow`, `src/editor/buildingTool.ts#BuildingTool.verdict`, `src/editor/growthFeed.ts#GrowthSession.receive`, `src/editor/growthFeed.ts#GrowthSession.send`, …
- `src/world/derived.ts#JournalCursor.reset` (method, line 33) — call by 3: `src/editor/growthFeed.ts#GrowthSession.receive`, `src/world/buildings/decayWatch.ts#createDecayWatch`, `src/world/buildings/siteGrading.ts#siteGrading.forget`

### src/world/doc.ts

- `src/world/doc.ts#RoadDoc` (class, line 153) — typeRef by 43: `src/buildingsWiring.ts#BuildingWiringDeps`, `src/editor/blocks.ts#commitBlockGrid`, `src/editor/buildingFloors.ts#BuildingFloors.floorOf`, `src/editor/buildingFloors.ts#BuildingFloors.follow`, …
- `src/world/doc.ts#RoadDoc.buildings` (field, line 289) — read by 55: `src/buildingsWiring.ts#createBuildingWiring`, `src/buildingsWiring.ts#createBuildingWiring.afterGesture`, `src/buildingsWiring.ts#createBuildingWiring.beforeDraw`, `src/buildingsWiring.ts#createBuildingWiring.drawOverlay`, …
- `src/world/doc.ts#RoadDoc.buildings` (field, line 289) — typeRef by 1: `src/world/lots.ts#setLotFront`
- `src/world/doc.ts#RoadDoc.buildings` (field, line 289) — write by 3: `src/editor/buildings.ts#addBuildingRecord`, `src/editor/buildings.ts#placeBuilding`, `src/world/growth/zoning.ts#commitGrown`
- `src/world/doc.ts#RoadDoc.changes` (field, line 262) — read by 9: `src/editor/buildingFloors.ts#BuildingFloors.follow`, `src/editor/buildingTool.ts#BuildingTool.verdict`, `src/editor/growthFeed.ts#GrowthSession.receive`, `src/editor/growthFeed.ts#GrowthSession.send`, …
- `src/world/doc.ts#RoadDoc.listen.tell` (function, line 2201) — callbackAssign by 2: `src/world/lotStore.ts#LotStore.recordingMap`, `src/world/lotStore.ts#LotStore.watch`
- `src/world/doc.ts#RoadDoc.lotsIn` (method, line 321) — call by 12: `src/editor/growthFeed.ts#GrowthSession.send`, `src/editor/lotTool.ts#LotTool.lotsNear`, `src/editor/lotTool.ts#LotTool.refreshLots`, `src/editor/lotTool.ts#LotTool.refusedNow`, …
- `src/world/doc.ts#RoadDoc.lotStore` (field, line 297) — read by 26: `src/editor/growthFeed.ts#GrowthSession.send`, `src/editor/lotTool.ts#LotTool.anyZoned`, `src/editor/lotTool.ts#LotTool.drawAllLots`, `src/editor/lotTool.ts#LotTool.drawLot`, …
- `src/world/doc.ts#RoadDoc.node` (method, line 416) — call by 1: `src/world/lots.ts#LotPlanner.planChanged`
- `src/world/doc.ts#RoadDoc.openLots` (getter, line 330) — call by 5: `src/editor/growthFeed.ts#GrowthSession.receive`, `src/editor/growthFeed.ts#GrowthSession.send`, `src/world/growth/zoning.ts#commitGrown`, `src/world/growth/zoning.ts#growOnLot`, …
- `src/world/doc.ts#RoadDoc.openLots` (getter, line 330) — implementation by 5: `src/world/openLots.ts#OpenLots.findLast`, `src/world/openLots.ts#OpenLots.flag`, `src/world/openLots.ts#OpenLots.lastGrown`, `src/world/openLots.ts#OpenLots.rebuild`, …
- `src/world/doc.ts#RoadDoc.requireSegment` (method, line 430) — call by 1: `src/world/zoneGrid.ts#zoneGridSteps`
- `src/world/doc.ts#RoadDoc.segment` (method, line 420) — call by 6: `src/editor/buildingSnap.ts#cornerFlush`, `src/editor/buildingSnap.ts#snapToRoad`, `src/editor/growthFeed.ts#roadPiece`, `src/world/lots.ts#plannedStreet`, …
- `src/world/doc.ts#RoadDoc.segments` (field, line 155) — read by 2: `src/world/lots.ts#LotPlanner.planAll`, `src/world/zoneGrid.ts#zoneGridSteps`
- `src/world/doc.ts#RoadDoc.tileContents` (field, line 1446) — read by 1: `src/world/doc.ts#RoadDoc.tileIdAt`
- `src/world/doc.ts#RoadDoc.tileNext` (field, line 1462) — read by 1: `src/world/doc.ts#RoadDoc.tileIdAt`
- `src/world/doc.ts#RoadNode` (interface, line 89) — read by 1: `src/world/lots.ts#LotPlanner.planChanged`
- `src/world/doc.ts#RoadSegment` (interface, line 124) — read by 7: `src/editor/buildingSnap.ts#cornerFlush`, `src/editor/buildingSnap.ts#snapToRoad`, `src/editor/growthFeed.ts#roadPiece`, `src/world/lots.ts#plannedStreet`, …
- `src/world/doc.ts#TileRecords` (interface, line 3116) — read by 2: `src/world/growth/grownRows.ts#tileBuildingsOf`, `src/world/growth/grownRows.ts#withGrownBuildings`
- `src/world/doc.ts#TileRecords` (interface, line 3116) — typeRef by 2: `src/world/growth/grownRows.ts#tileBuildingsOf`, `src/world/growth/grownRows.ts#withGrownBuildings`

### src/world/grid.ts

- `src/world/grid.ts#GRID_CELL` (variable, line 6) — read by 1: `src/world/zoneGrid.ts#ZONE_CELL`

### src/world/groundPatch.ts

- `src/world/groundPatch.ts#cellDiagonal` (function, line 4) — call by 1: `src/world/groundPatch.ts#latticeHeight`
- `src/world/groundPatch.ts#cellHeight` (function, line 9) — call by 1: `src/world/groundPatch.ts#latticeHeight`
- `src/world/groundPatch.ts#ON_LINE` (variable, line 15) — read by 1: `src/world/groundPatch.ts#latticeHeight`

### src/world/ids.ts

- `src/world/ids.ts#IdAllocator` (class, line 19) — construct by 14: `src/world/buildings/store.ts#BuildingStore.copyAllocator`, `src/world/buildings/store.ts#BuildingStore.forkFrom`, `src/world/buildings/store.ts#BuildingStore.ids`, `src/world/buildings/store.ts#BuildingStore.replaceWith`, …
- `src/world/ids.ts#IdAllocator.peek` (getter, line 35) — call by 12: `src/world/buildings/store.ts#BuildingStore.copyAllocator`, `src/world/buildings/store.ts#BuildingStore.forkFrom`, `src/world/buildings/store.ts#BuildingStore.nextId`, `src/world/buildings/store.ts#BuildingStore.replaceWith`, …
- `src/world/ids.ts#IdAllocator.reserve` (method, line 31) — call by 6: `src/world/buildings/store.ts#BuildingStore.load`, `src/world/buildings/store.ts#BuildingStore.loadPrepared`, `src/world/buildings/store.ts#BuildingStore.loadSections`, `src/world/buildings/store.ts#BuildingStore.restore`, …
- `src/world/ids.ts#IdAllocator.take` (method, line 26) — call by 2: `src/world/buildings/store.ts#BuildingStore.add`, `src/world/lotStore.ts#LotStore.apply`
- `src/world/ids.ts#NodeId` (type, line 7) — typeRef by 10: `src/editor/growthFeed.ts#GrowthSession.syncRoads`, `src/editor/growthFeed.ts#junctionPiece`, `src/editor/growthReplica.ts#RoadReplica.apply`, `src/editor/growthReplica.ts#RoadReplica.dropKerb`, …
- `src/world/ids.ts#SegmentId` (type, line 8) — typeRef by 21: `src/editor/growthFeed.ts#GrowthSession.syncRoads`, `src/editor/growthFeed.ts#roadPiece`, `src/editor/growthReplica.ts#RoadReplica.apply`, `src/editor/growthReplica.ts#RoadReplica.doc`, …

### src/world/junction/levels.ts

- `src/world/junction/levels.ts#JunctionLevels.get` (method, line 256) — call by 1: `src/editor/growthFeed.ts#junctionPiece`

### src/world/junction/types.ts

- `src/world/junction/types.ts#Junction` (interface, line 60) — read by 1: `src/editor/growthFeed.ts#junctionPiece`
- `src/world/junction/types.ts#Junction` (interface, line 60) — typeRef by 1: `src/world/lots.ts#pavingNear`

### src/world/network.ts

- `src/world/network.ts#Network` (class, line 46) — typeRef by 33: `src/buildingsWiring.ts#BuildingWiringDeps`, `src/editor/blocks.ts#commitBlockGrid`, `src/editor/buildingSnap.ts#cornerFlush`, `src/editor/buildingSnap.ts#snapPlacement`, …
- `src/world/network.ts#Network.changesSince` (method, line 143) — call by 4: `src/editor/buildings.ts#clearBuildingsAfterEdit`, `src/editor/growthFeed.ts#GrowthSession.readChanges`, `src/editor/lotTool.ts#LotTool.refusedNow`, `src/world/lots.ts#LotPlanner.update`
- `src/world/network.ts#Network.constructor` (constructor, line 100) — read by 3: `src/editor/buildingSnap.ts#cornerFlush`, `src/editor/buildingSnap.ts#snapToRoad`, `src/editor/growthFeed.ts#roadPiece`
- `src/world/network.ts#Network.junctionCells` (field, line 58) — read by 5: `src/editor/growthFeed.ts#GrowthSession.syncRoads`, `src/editor/growthFeed.ts#junctionPiece`, `src/world/lots.ts#LotPlanner.planChanged`, `src/world/lots.ts#pavingNear`, …
- `src/world/network.ts#Network.junctions` (field, line 54) — read by 3: `src/editor/growthFeed.ts#junctionPiece`, `src/world/lots.ts#pavingNear`, `src/world/poleLines.ts#markPlates`
- `src/world/network.ts#Network.ribbons` (field, line 55) — read by 11: `src/editor/growthFeed.ts#roadPiece`, `src/world/lots.ts#cutLotsByRoads`, `src/world/lots.ts#LandPaving.tile`, `src/world/lots.ts#LotPlanner.planAll`, …
- `src/world/network.ts#Network.ribbonsMeeting` (method, line 62) — call by 2: `src/world/lots.ts#pavingNear`, `src/world/nearestRibbon.ts#nearestRibbon`
- `src/world/network.ts#Network.segmentCells` (field, line 57) — read by 8: `src/editor/growthFeed.ts#GrowthSession.syncRoads`, `src/editor/growthFeed.ts#roadPiece`, `src/world/lots.ts#LandPaving.tile`, `src/world/lots.ts#LotPlanner.planChanged`, …
- `src/world/network.ts#Network.state` (getter, line 148) — call by 6: `src/editor/growthFeed.ts#GrowthSession.readChanges`, `src/editor/growthFeed.ts#GrowthSession.receive`, `src/editor/growthFeed.ts#GrowthSession.send`, `src/editor/lotTool.ts#LotTool.proposalKey`, …

### src/world/networkDelta.ts

- `src/world/networkDelta.ts#NetworkDelta` (interface, line 5) — read by 5: `src/editor/buildings.ts#clearBuildingsAfterEdit`, `src/editor/growthFeed.ts#GrowthSession.readChanges`, `src/editor/lotTool.ts#LotTool.refusedNow`, `src/world/lots.ts#LotPlanner.planChanged`, …
- `src/world/networkDelta.ts#NetworkDelta` (interface, line 5) — typeRef by 1: `src/world/lots.ts#LotPlanner.planChanged`

### src/world/parking.ts

- `src/world/parking.ts#SegmentParking` (interface, line 10) — typeRef by 1: `src/editor/blocks.ts#commitBlockGrid`

### src/world/paving.ts

- `src/world/paving.ts#PavingData` (interface, line 27) — read by 1: `src/render/buildings/cellWork.ts#wider`
- `src/world/paving.ts#PavingData` (interface, line 27) — typeRef by 4: `src/editor/growthFeed.ts#GrowthSession.send`, `src/editor/growthReplica.ts#GrowAsk`, `src/editor/growthReplica.ts#pavedLookup`, `src/render/buildings/cellWork.ts#CellJob`
- `src/world/paving.ts#pavingReader` (function, line 41) — call by 2: `src/editor/growthReplica.ts#pavedLookup`, `src/render/buildings/cellWork.ts#workerReads`
- `src/world/paving.ts#PavingSource` (interface, line 34) — call by 2: `src/editor/growthFeed.ts#GrowthSession.send`, `src/render/buildings/cellPool.ts#CellPool.write`
- `src/world/paving.ts#PavingSource` (interface, line 34) — read by 3: `src/editor/growthFeed.ts#GrowthSession.send`, `src/render/buildings/cellPool.ts#CellPool.offPage`, `src/render/buildings/cellPool.ts#CellPool.write`
- `src/world/paving.ts#PavingSource` (interface, line 34) — typeRef by 2: `src/editor/growthFeed.ts#GrowthHost`, `src/render/buildings/cellPool.ts#CellEnv`

### src/world/pedestrianAccess.ts

- `src/world/pedestrianAccess.ts#carriesPedestrians` (function, line 4) — call by 2: `src/world/lots.ts#plannedStreet`, `src/world/zoneGrid.ts#zoneGridSteps`

### src/world/poleLines.ts

- `src/world/poleLines.ts#FootRings` (interface, line 315) — read by 1: `src/world/poleLines.ts#markPlates.place`
- `src/world/poleLines.ts#FootRings` (interface, line 315) — typeRef by 1: `src/world/poleLines.ts#markPlates.place`
- `src/world/poleLines.ts#insideLevels` (function, line 318) — call by 1: `src/world/poleLines.ts#onPaving`
- `src/world/poleLines.ts#insideLevels` (function, line 318) — read by 1: `src/world/poleLines.ts#onPaving`
- `src/world/poleLines.ts#junctionRings` (function, line 355) — call by 1: `src/world/poleLines.ts#markPlates`
- `src/world/poleLines.ts#polesInclude` (function, line 150) — call by 1: `src/world/poleLines.ts#markPlates`
- `src/world/poleLines.ts#ribbonRings` (function, line 341) — call by 1: `src/world/poleLines.ts#markPlates`
- `src/world/poleLines.ts#WindingRing` (interface, line 265) — read by 2: `src/world/poleLines.ts#markPlates`, `src/world/poleLines.ts#markPlates.place`
- `src/world/poleLines.ts#WindingRing` (interface, line 265) — typeRef by 2: `src/world/poleLines.ts#markPlates`, `src/world/poleLines.ts#Placed`

### src/world/rescale.ts

- `src/world/rescale.ts#rescaleBuilding` (function, line 38) — call by 1: `src/editor/blueprintLibrary.ts#readEntry`
- `src/world/rescale.ts#unitFactor` (function, line 10) — call by 1: `src/editor/blueprintLibrary.ts#readEntry`

### src/world/ribbons.ts

- `src/world/ribbons.ts#SegmentRibbon` (interface, line 25) — read by 12: `src/editor/buildingSnap.ts#cornerFlush`, `src/editor/buildingSnap.ts#snapToRoad`, `src/editor/growthFeed.ts#roadPiece`, `src/editor/lotTool.ts#LotTool.streetAngleNear`, …
- `src/world/ribbons.ts#SegmentRibbon` (interface, line 25) — typeRef by 2: `src/world/lots.ts#plannedStreet`, `src/world/nearestRibbon.ts#nearestRibbon`

### src/world/roadTypes.ts

- `src/world/roadTypes.ts#halfWidth` (function, line 339) — call by 8: `src/editor/buildingSnap.ts#cornerFlush`, `src/editor/buildingSnap.ts#snapToRoad`, `src/world/buildings/validate.ts#touchesRoad`, `src/world/growth/zoning.ts#cornerOf.onStreet`, …
- `src/world/roadTypes.ts#Level` (enum, line 10) — reference by 11: `src/editor/buildingSnap.ts#cornerFlush`, `src/editor/buildingSnap.ts#snapToRoad`, `src/editor/growthFeed.ts#junctionPiece`, `src/editor/growthReplica.ts#RoadReplica.apply`, …
- `src/world/roadTypes.ts#RoadType` (interface, line 29) — read by 1: `src/editor/growthFeed.ts#roadPiece`
- `src/world/roadTypes.ts#RoadType` (interface, line 29) — typeRef by 1: `src/world/buildings/validate.ts#RoadReachRibbon`
- `src/world/roadTypes.ts#SurfaceLevel` (type, line 25) — typeRef by 5: `src/editor/growthReplica.ts#RoadReplica.apply`, `src/editor/growthReplica.ts#RoadReplica.junctions`, `src/editor/growthReplica.ts#RoadReplica.kerbs`, `src/world/buildings/validate.ts#RoadReach`, …

### src/world/structures.ts

- `src/world/structures.ts#RoadStructure` (type, line 5) — typeRef by 3: `src/editor/growthReplica.ts#RoadPiece`, `src/editor/growthReplica.ts#Structured`, `src/world/buildings/validate.ts#RoadReach`

### src/world/surfaces.ts

- `src/world/surfaces.ts#levelRingsOf` (function, line 81) — call by 1: `src/world/lots.ts#pavingNear`

### src/world/tiles.ts

- `src/world/tiles.ts#OWNER` (variable, line 82) — call by 1: `src/world/doc.ts#RoadDoc.tileIdAt`
- `src/world/tiles.ts#OWNER` (variable, line 82) — read by 1: `src/world/doc.ts#RoadDoc.tileIdAt`
- `src/world/tiles.ts#TILE_COUNTED` (variable, line 51) — read by 1: `src/world/doc.ts#RoadDoc.tileIdAt`
- `src/world/tiles.ts#tileIdBase` (function, line 48) — call by 1: `src/world/doc.ts#RoadDoc.tileIdAt`

### src/world/viewCells.ts

- `src/world/viewCells.ts#ViewCells` (class, line 95) — construct by 1: `src/render/buildings/layerCells.ts#createCoverIndex`
- `src/world/viewCells.ts#ViewCells` (class, line 95) — typeRef by 1: `src/render/buildings/layerCells.ts#CoverIndex`
- `src/world/viewCells.ts#ViewCells.cellAt` (method, line 117) — call by 2: `src/render/buildings/layerCells.ts#createCoverIndex.covers`, `src/render/buildings/layerCells.ts#createCoverIndex.look`
- `src/world/viewCells.ts#ViewCells.clear` (method, line 271) — call by 2: `src/render/buildings/layerCells.ts#createCoverIndex.follow`, `src/render/gardens.ts#createGardenCells.source`
- `src/world/viewCells.ts#ViewCells.ensure` (method, line 161) — call by 1: `src/render/buildings/layerCells.ts#createCoverIndex.covers`
- `src/world/viewCells.ts#ViewCells.follow` (method, line 279) — call by 1: `src/render/buildings/layerCells.ts#createCoverIndex.follow`
- `src/world/viewCells.ts#ViewCells.has` (method, line 148) — call by 1: `src/render/buildings/layerCells.ts#createCoverIndex.look`
- `src/world/viewCells.ts#ViewCells.look` (method, line 186) — call by 1: `src/render/buildings/layerCells.ts#createCoverIndex.look`
- `src/world/viewCells.ts#ViewCells.pump` (method, line 247) — call by 1: `src/render/buildings/layerCells.ts#createCoverIndex.look`
- `src/world/viewCells.ts#ViewCells.wantedKeys` (getter, line 242) — call by 1: `src/render/buildings/layerCells.ts#createCoverIndex.look`

## External APIs

- `es:Math` × 2499
- `es:Math.max` × 674
- `es:Array.push` × 602
- `es:Array.length` × 587
- `es:Math.min` × 538
- `es:Map.get` × 367
- `es:Infinity` × 262
- `es:Array.map` × 254
- `es:Math.floor` × 247
- `es:Map` × 235
- `es:Math.abs` × 233
- `es:Map.set` × 202
- `es:Set` × 200
- `es:Float32Array` × 196
- `es:Number` × 187
- `es:ReadonlyArray.length` × 186
- `es:Record` × 180
- `es:Math.PI` × 178
- `es:Object` × 174
- `es:Math.round` × 173
- `es:Set.add` × 149
- `three:BufferGeometry` × 142
- `dom:CanvasFillStrokeStyles.fillStyle` × 133
- `es:Array.filter` × 118
- `es:Math.hypot` × 118
- `es:Map.delete` × 114
- `es:Set.has` × 114
- `dom:CanvasRect.fillRect` × 113
- `es:Float64Array` × 102
- `es:ReadonlyArray.map` × 88
- `es:NumberConstructor.isFinite` × 87
- `es:String.replace` × 86
- `es:Math.sin` × 85
- `es:ReadonlySet.has` × 82
- `es:Readonly` × 76
- `es:Uint32Array` × 75
- `es:Map.values` × 73
- `es:Math.cos` × 73
- `es:Number.toFixed` × 73
- `three:BufferGeometry.setAttribute` × 72
- `three:MeshStandardMaterial` × 71
- `es:Map.clear` × 69
- `es:Array.sort` × 68
- `three:BufferAttribute` × 63
- `dom:CanvasPath.lineTo` × 61
- `es:Math.ceil` × 60
- `es:Uint16Array` × 59
- `es:ObjectConstructor.keys` × 58
- `es:Partial` × 57
- `three:Object3D.visible` × 57
- `dom:CanvasDrawPath.beginPath` × 56
- `es:Array` × 56
- `es:Array.some` × 56
- `es:Map.has` × 55
- `es:Map.size` × 55
- `es:JSON` × 53
- `three:BufferGeometry.translate` × 52
- `es:Uint8Array` × 50
- `es:Math.imul` × 49
- `three:BufferAttribute.count` × 49
- `three:Mesh.geometry` × 49
- `es:Array.find` × 48
- `es:ReadonlySet` × 47
- `three:Vector3` × 47
- `es:Int32Array` × 46
- `es:Set.clear` × 46
- `three:BufferGeometry.getAttribute` × 44
- `es:Map.keys` × 42
- `es:Set.delete` × 42
- `three:Object3D.add` × 42
- `three:Object3D.userData` × 41
- `three:Box3` × 38
- `es:JSON.stringify` × 37
- `three:Mesh` × 37
- `three:BufferAttribute.getX` × 36
- `es:Omit` × 35
- `three:Group` × 35
- `three:WebGLProgramParametersWithUniforms.uniforms` × 35
- `es:NaN` × 34
- `es:String` × 34
- `dom:CanvasDrawPath.fill` × 33
- `dom:CanvasPath.moveTo` × 33
- `es:Error` × 33
- `es:Math.atan2` × 33
- `es:String.slice` × 33
- `three:Material` × 33
- `es:Array.slice` × 32
- `three:InterleavedBufferAttribute` × 32
- `three:Object3D.name` × 32
- `three:Vector3.y` × 32
- `dom:HTMLOrSVGElement.dataset` × 31
- `es:Set.size` × 31
- `three:BufferGeometry.attributes` × 31
- `three:Vector3.z` × 31
- `es:Float64Array.length` × 30
- `three:WebGLProgramParameters.vertexShader` × 30
- `es:ObjectConstructor.entries` × 29
- `three:Vector3.x` × 29
- `es:ArrayConstructor.isArray` × 28
- `three:BufferGeometry.dispose` × 28
- `dom:CanvasDrawPath.stroke` × 27
- `es:Float32Array.set` × 27
- `three:Color` × 27
- `three:InstancedMesh` × 27
- `es:Array.splice` × 26
- `es:WeakMap` × 26
- `es:WeakMap.set` × 26
- `three:Vector3.set` × 26
- `three:WebGLProgramParameters.fragmentShader` × 26
- `dom:CanvasPath.arc` × 24
- `es:Float32Array.length` × 24
- `es:ReadonlyArray.includes` × 24
- `es:WeakMap.get` × 24
- `three:Box3.max` × 24
- `three:BufferGeometry.index` × 24
- `es:ReadonlyArray.forEach` × 23
- `es:ReadonlyMap` × 23
- `es:Uint32Array.length` × 23
- `polygon-clipping:polygon-clipping` × 23
- `three:Box3.min` × 23
- `three:Object3D.castShadow` × 23
- `es:Array.forEach` × 22
- `es:NonNullable` × 22
- `es:Pick` × 22
- `es:String.startsWith` × 22
- `three:BufferAttribute.getY` × 22
- `three:Object3D.position` × 22
- `dom:CanvasPathDrawingStyles.lineWidth` × 21
- `es:Array.flatMap` × 21
- `three:DoubleSide` × 21
- `three:Material.customProgramCacheKey` × 21
- `dom:CanvasFillStrokeStyles.strokeStyle` × 20
- `es:ArrayBuffer` × 20
- `es:Int16Array` × 20
- `es:ObjectConstructor.freeze` × 20
- `three:BufferAttribute.array` × 20
- `three:Material.onBeforeCompile` × 20
- `es:Iterable` × 19
- `es:ObjectConstructor.fromEntries` × 19
- `es:ReadonlyArray.some` × 19
- `three:BufferAttribute.getZ` × 19
- `three:BufferGeometry.boundingBox` × 19
- `three:Object3D.matrixAutoUpdate` × 19
- `es:ArrayBufferLike` × 18
- `es:ArrayConstructor.from` × 18
- `es:ObjectConstructor.values` × 18
- `es:Promise` × 18
- `three:InstancedInterleavedBuffer` × 18
- `es:Int32Array.length` × 17
- `es:NumberConstructor.isInteger` × 17
- `es:NumberConstructor.NaN` × 17
- `es:ObjectConstructor.assign` × 17
- `dom:console` × 16
- `es:Array.every` × 16
- `es:JSON.parse` × 16
- `es:ReadonlyArray.filter` × 16
- `es:ReadonlyArray.slice` × 16
- `three:BufferGeometry.getIndex` × 16
- `three:Object3D` × 16
- `three:Object3D.remove` × 16
- `dom:WebGLRenderingContextBase.bindBuffer` × 15
- `es:Array.reduce` × 15
- `three:BufferGeometry.setIndex` × 15
- `three:CylinderGeometry` × 15
- `three:GLBufferAttribute` × 15
- `three:Mesh.material` × 15
- `three:Object3D.receiveShadow` × 15
- `three:Color.b` × 14
- `three:Color.g` × 14
- `three:Color.r` × 14
- `three:DataUtils` × 14
- `three:DataUtils.toHalfFloat` × 14
- `three:Object3D.updateMatrix` × 14
- `dom:structuredClone` × 13
- `es:Array.pop` × 13
- `es:Array.reverse` × 13
- `es:String.split` × 13
- `three:BoxGeometry` × 13
- `three:BufferGeometry.boundingSphere` × 13
- `three:Object3D.clear` × 13
- `dom:Node.appendChild` × 12
- `dom:performance` × 12
- `es:MapIterator` × 12
- `three:BufferGeometry.rotateX` × 12
- `three:Color.setHex` × 12
- `three:Frustum` × 12
- `three:InstancedBufferAttribute` × 12
- `three:InstancedMesh.instanceColor` × 12
- `dom:Element.textContent` × 11
- `dom:Performance.now` × 11
- `es:DataView.getUint32` × 11
- `es:Float32Array.subarray` × 11
- `es:Math.fround` × 11
- `es:Promise.then` × 11
- `es:ReadonlyArray.find` × 11
- `es:String.length` × 11
- `three:Object3D.renderOrder` × 11
- `dom:CanvasRenderingContext2D` × 10
- `dom:Console.warn` × 10
- `dom:HTMLInputElement.value` × 10
- `es:Int32Array.fill` × 10
- `es:Math.sign` × 10
- `es:NumberConstructor.isNaN` × 10
- `es:String.indexOf` × 10
- `es:WeakRef` × 10
- `polygon-clipping:intersection` × 10
- `three:BufferGeometry.computeBoundingBox` × 10
- `three:CanvasTexture` × 10
- `three:Object3D.scale` × 10
- `three:Sphere.center` × 10
- `dom:CanvasText.fillText` × 9
- `dom:CanvasTextDrawingStyles.font` × 9
- `dom:WebGL2RenderingContextBase.COPY_WRITE_BUFFER` × 9
- `es:Array.includes` × 9
- `es:Array.indexOf` × 9
- `es:Array.join` × 9
- `es:Float32Array.buffer` × 9
- `es:Generator` × 9
- `es:Math.sqrt` × 9
- `es:ReturnType` × 9
- `three:BufferGeometry.rotateY` × 9
- `three:ConeGeometry` × 9
- `three:GLBufferAttribute.buffer` × 9
- `three:InstancedMesh.count` × 9
- `three:Object3D.customDepthMaterial` × 9
- `dom:CanvasTextDrawingStyles.textAlign` × 8
- `dom:document` × 8
- `dom:WebGL2RenderingContextBase.COPY_READ_BUFFER` × 8
- `es:ArrayLike` × 8
- `es:CallableFunction.bind` × 8
- `es:Float64Array.set` × 8
- `es:Int8Array` × 8
- `es:ObjectConstructor.is` × 8
- `es:ReadonlyArray.indexOf` × 8
- `es:ReadonlySet.size` × 8
- `es:Symbol` × 8
- `three:BufferAttribute.setXY` × 8
- `three:BufferGeometry.scale` × 8
- `three:InterleavedBuffer.array` × 8
- `three:Mesh.isMesh` × 8
- `three:Object3D.children` × 8
- `dom:KeyboardEvent.key` × 7
- `earcut:earcut` × 7
- `es:Float32ArrayConstructor.from` × 7
- `es:ReadonlyArray.every` × 7
- `es:ReadonlyMap.get` × 7
- `es:Uint16Array.set` × 7
- `es:Uint32Array.set` × 7
- `polygon-clipping:Polygon` × 7
- `three:Box3.union` × 7
- `three:BufferGeometry.computeBoundingSphere` × 7
- `three:BufferGeometry.computeVertexNormals` × 7
- `three:InstancedMesh.dispose` × 7
- `three:MaterialProperties.name` × 7
- `three:mergeGeometries` × 7
- `three:PlaneGeometry` × 7
- `three:Sphere` × 7
- `dom:CanvasTextDrawingStyles.textBaseline` × 6
- `dom:Console.error` × 6
- `dom:MessageEvent.data` × 6
- `es:CallableFunction.call` × 6
- `es:Float32Array.byteLength` × 6
- `es:Float64ArrayConstructor.of` × 6
- `es:IteratorReturnResult.value` × 6
- `es:ReadonlyArray.flatMap` × 6
- `es:WeakMap.has` × 6
- `es:WeakRef.deref` × 6
- `es:WeakSet.add` × 6
- `three:FrontSide` × 6
- `three:MeshStandardMaterialProperties.color` × 6
- `three:Object3D.traverse` × 6
- `three:Scene` × 6
- `three:Texture.needsUpdate` × 6
- `three:uniforms.__index` × 6
- `dom:CanvasGradient.addColorStop` × 5
- `dom:CanvasPath.closePath` × 5
- `dom:Document.createElement` × 5
- `dom:HTMLElement` × 5
- `es:ArrayBuffer.byteLength` × 5
- `es:DataView` × 5
- `es:DataView.getUint8` × 5
- `es:Iterator.next` × 5
- `es:ReadonlyMap.values` × 5
- `es:String.includes` × 5
- `es:Uint32Array.fill` × 5
- `es:Uint8Array.fill` × 5
- `es:Uint8Array.set` × 5
- `es:WeakMap.delete` × 5
- `three:Box3.setFromObject` × 5
- `three:Camera` × 5
- `three:IcosahedronGeometry` × 5
- `three:InstancedMesh.boundingBox` × 5
- `three:Matrix4` × 5
- `three:MeshBasicMaterial` × 5
- `three:MeshDepthMaterial` × 5
- `three:Object3D.frustumCulled` × 5
- `three:Object3D.parent` × 5
- `three:WebGLRenderer` × 5
- `dom:CanvasPathDrawingStyles.setLineDash` × 4
- `dom:Element.innerHTML` × 4
- `dom:HTMLCanvasElement.height` × 4
- `dom:HTMLCanvasElement.width` × 4
- `dom:HTMLInputElement` × 4
- `dom:MessageEvent` × 4
- `dom:ParentNode.append` × 4
- `dom:WebGL2RenderingContext` × 4
- `dom:window` × 4
- `dom:Worker` × 4
- `es:Array.entries` × 4
- `es:Array.fill` × 4
- `es:Array.findIndex` × 4
- `es:FinalizationRegistry` × 4
- `es:Float64Array.buffer` × 4
- `es:IterableIterator` × 4
- `es:Parameters` × 4
- `es:ReadonlyArray.join` × 4
- `es:RegExp.test` × 4
- `es:Uint16Array.byteLength` × 4
- `es:Uint32Array.byteLength` × 4
- `es:WeakSet.has` × 4
- `three:BackSide` × 4
- `three:BatchedMesh.setVisibleAt` × 4
- `three:BatchedMesh.unusedIndexCount` × 4
- `three:BatchedMesh.unusedVertexCount` × 4
- `three:Box3.expandByPoint` × 4
- `three:Box3.getCenter` × 4
- `three:BufferGeometry.clone` × 4
- `three:DataTexture` × 4
- `three:InstancedMesh.boundingSphere` × 4
- `three:InterleavedBufferAttribute.data` × 4
- `three:Material.dispose` × 4
- `three:MaterialProperties.opacity` × 4
- `three:MaterialProperties.transparent` × 4
- `three:Path.lineTo` × 4
- `three:Sphere.radius` × 4
- `three:Texture.repeat` × 4
- `three:Vector2.set` × 4
- `three:Vector3.fromBufferAttribute` × 4
- `three:Vector3.normalize` × 4
- `dom:Blob` × 3
- `dom:CanvasPath.roundRect` × 3
- `dom:CanvasRect.clearRect` × 3
- `dom:CanvasText.measureText` × 3
- `dom:clearTimeout` × 3
- `dom:Event.preventDefault` × 3
- `dom:HTMLElement.hidden` × 3
- `dom:setTimeout` × 3
- `dom:TextMetrics.width` × 3
- `dom:WebGL2RenderingContextOverloads.bufferSubData` × 3
- `dom:WebGLBuffer` × 3
- `dom:Window.addEventListener` × 3
- `dom:Worker.postMessage` × 3
- `es:Array.flat` × 3
- `es:FinalizationRegistry.register` × 3
- `es:Float32Array.fill` × 3
- `es:Generator.next` × 3
- `es:Int32ArrayConstructor.from` × 3
- `es:Math.SQRT2` × 3
- `es:Number.toString` × 3
- `es:Object.constructor` × 3
- `es:PromiseConstructor.resolve` × 3
- `es:ReadonlyArray.reduce` × 3
- `es:SharedArrayBuffer` × 3
- `es:String.charCodeAt` × 3
- `es:String.trim` × 3
- `es:SymbolConstructor.iterator` × 3
- `es:SymbolConstructor.toStringTag` × 3
- `es:Uint32ArrayConstructor.from` × 3
- `es:Uint8Array.buffer` × 3
- `es:WeakSet` × 3
- `polygon-clipping:MultiPolygon` × 3
- `three:BatchedMesh.sortObjects` × 3
- `three:Box3.clone` × 3
- `three:Box3.getSize` × 3
- `three:BufferAttribute.getComponent` × 3
- `three:BufferAttribute.itemSize` × 3
- `three:BufferAttribute.setXYZ` × 3
- `three:BufferGeometry.addGroup` × 3
- `three:BufferGeometry.clearGroups` × 3
- `three:BufferGeometry.deleteAttribute` × 3
- `three:BufferGeometry.groups` × 3
- `three:BufferGeometry.toNonIndexed` × 3
- `three:Color.setRGB` × 3
- `three:Euler.y` × 3
- `three:Frustum.copy` × 3
- `three:Frustum.intersectsBox` × 3
- `three:GeometryGroup.start` × 3
- `three:InterleavedBuffer` × 3
- `three:InterleavedBufferAttribute.isInterleavedBufferAttribute` × 3
- `three:Material.clone` × 3
- `three:MaterialProperties.userData` × 3
- `three:Object3D.rotation` × 3
- `three:Object3D.updateMatrixWorld` × 3
- `three:OrthographicCamera` × 3
- `three:Path.absarc` × 3
- `three:RepeatWrapping` × 3
- `three:Scene.overrideMaterial` × 3
- `three:Shape` × 3
- `three:SphereGeometry` × 3
- `three:Texture.dispose` × 3
- `three:Vector3.applyMatrix4` × 3
- `three:Vector3.setScalar` × 3
- `three:Vector4.w` × 3
- `three:WebGLProgramParametersWithUniforms` × 3
- `three:WebGLRenderer.render` × 3
- `dom:CanvasFillStrokeStyles.createLinearGradient` × 2
- `dom:CanvasPath.quadraticCurveTo` × 2
- `dom:CanvasPathDrawingStyles.lineCap` × 2
- `dom:CanvasState.restore` × 2
- `dom:CanvasState.save` × 2
- `dom:DocumentOrShadowRoot.activeElement` × 2
- `dom:GlobalEventHandlers.onchange` × 2
- `dom:GlobalEventHandlers.onclick` × 2
- `dom:HTMLButtonElement.type` × 2
- `dom:HTMLCanvasElement.getContext` × 2
- `dom:HTMLElementTagNameMap` × 2
- `dom:HTMLInputElement.type` × 2
- `dom:HTMLOptionElement.selected` × 2
- `dom:HTMLOptionElement.value` × 2
- `dom:IDBDatabase.transaction` × 2
- `dom:IDBRequest.onerror` × 2
- `dom:IDBRequest.result` × 2
- `dom:IDBTransaction.objectStore` × 2
- `dom:KeyboardEvent.altKey` × 2
- `dom:requestAnimationFrame` × 2
- `dom:URL` × 2
- `dom:URL.createObjectURL` × 2
- `dom:WebGL2RenderingContextBase.copyBufferSubData` × 2
- `dom:WebGLRenderingContextBase.deleteBuffer` × 2
- `dom:WebGLRenderingContextBase.ELEMENT_ARRAY_BUFFER` × 2
- `dom:WebGLRenderingContextBase.UNSIGNED_INT` × 2
- `es:Array.concat` × 2
- `es:Array.keys` × 2
- `es:Array.shift` × 2
- `es:Array.unshift` × 2
- `es:DataView.getFloat64` × 2
- `es:DataView.getInt32` × 2
- `es:Date` × 2
- `es:DateConstructor.now` × 2
- `es:Float32Array.slice` × 2
- `es:Float64Array.byteLength` × 2
- `es:Float64Array.slice` × 2
- `es:Float64ArrayConstructor.from` × 2
- `es:Int16Array.buffer` × 2
- `es:Int16Array.byteLength` × 2
- `es:Int16Array.set` × 2
- `es:Math.pow` × 2
- `es:ObjectConstructor.defineProperty` × 2
- `es:Promise.catch` × 2
- `es:PromiseConstructor.all` × 2
- `es:ReadonlyMap.has` × 2
- `es:ReadonlyMap.size` × 2
- `es:Required` × 2
- `es:String.padStart` × 2
- `es:String.toLocaleLowerCase` × 2
- `es:String.toLowerCase` × 2
- `es:Uint16Array.buffer` × 2
- `es:Uint16Array.length` × 2
- `es:Uint16Array.subarray` × 2
- `es:Uint32Array.buffer` × 2
- `es:Uint8Array.length` × 2
- `es:Uint8Array.subarray` × 2
- `meshoptimizer:MeshoptSimplifier` × 2
- `polygon-clipping:difference` × 2
- `three:AdditiveBlending` × 2
- `three:BatchedMesh` × 2
- `three:BatchedMesh.addGeometry` × 2
- `three:BatchedMesh.addInstance` × 2
- `three:BatchedMesh.instanceCount` × 2
- `three:BatchedMesh.setGeometrySize` × 2
- `three:BatchedMesh.setInstanceCount` × 2
- `three:Box3.getBoundingSphere` × 2
- `three:Box3.isEmpty` × 2
- `three:BufferAttribute.needsUpdate` × 2
- `three:BufferAttribute.normalized` × 2
- `three:BufferGeometry.drawRange` × 2
- `three:BufferGeometry.hasAttribute` × 2
- `three:BufferGeometry.name` × 2
- `three:BufferGeometry.rotateZ` × 2
- `three:CircleGeometry` × 2
- `three:Color.multiplyScalar` × 2
- `three:DataTexture.generateMipmaps` × 2
- `three:DataTexture.magFilter` × 2
- `three:DataTexture.minFilter` × 2
- `three:Frustum.planes` × 2
- `three:GeometryGroup.count` × 2
- `three:InstancedMesh.setColorAt` × 2
- `three:LinearFilter` × 2
- `three:Material.needsUpdate` × 2
- `three:MaterialProperties.depthWrite` × 2
- `three:MeshBasicMaterialProperties.map` × 2
- `three:MeshStandardMaterialProperties.emissive` × 2
- `three:MeshStandardMaterialProperties.emissiveIntensity` × 2
- `three:NearestFilter` × 2
- `three:Object3D.copy` × 2
- `three:Object3D.lookAt` × 2
- `three:Object3D.matrixWorld` × 2
- `three:Object3D.up` × 2
- `three:OrthographicCamera.near` × 2
- `three:Path.moveTo` × 2
- `three:PerspectiveCamera` × 2
- `three:Plane.constant` × 2
- `three:Plane.normal` × 2
- `three:RGBAIntegerFormat` × 2
- `three:RingGeometry` × 2
- `three:SRGBColorSpace` × 2
- `three:Texture.addUpdateRange` × 2
- `three:Texture.anisotropy` × 2
- `three:Texture.colorSpace` × 2
- `three:Texture.image` × 2
- `three:Texture.internalFormat` × 2
- `three:Texture.offset` × 2
- `three:Texture.wrapS` × 2
- `three:Texture.wrapT` × 2
- `three:UnsignedIntType` × 2
- `three:Vector3.subVectors` × 2
- `three:WebGLRenderer.dispose` × 2
- `three:WebGLRenderer.readRenderTargetPixels` × 2
- `three:WebGLRenderer.setClearColor` × 2
- `three:WebGLRenderer.setRenderTarget` × 2
- `vite:ImportMeta.env` × 2
- `vite:ImportMetaEnv.DEV` × 2
- `clipper2-ts:EndType` × 1
- `clipper2-ts:EndType.Polygon` × 1
- `clipper2-ts:inflatePaths` × 1
- `clipper2-ts:JoinType` × 1
- `clipper2-ts:JoinType.Miter` × 1
- `clipper2-ts:Point64.x` × 1
- `clipper2-ts:Point64.y` × 1
- `dom:AbstractWorker.onerror` × 1
- `dom:Blob.arrayBuffer` × 1
- `dom:Body.json` × 1
- `dom:cancelAnimationFrame` × 1
- `dom:CanvasPathDrawingStyles.lineJoin` × 1
- `dom:CanvasRect.strokeRect` × 1
- `dom:ChildNode.remove` × 1
- `dom:Document.getElementById` × 1
- `dom:DOMTokenList.toggle` × 1
- `dom:Element.classList` × 1
- `dom:Element.className` × 1
- `dom:Element.setAttribute` × 1
- `dom:ElementContentEditable.inputMode` × 1
- `dom:ErrorEvent.message` × 1
- `dom:Event.stopPropagation` × 1
- `dom:fetch` × 1
- `dom:File` × 1
- `dom:File.name` × 1
- `dom:GlobalEventHandlers.onblur` × 1
- `dom:GlobalEventHandlers.oninput` × 1
- `dom:GlobalEventHandlers.onkeydown` × 1
- `dom:GlobalEventHandlers.onwheel` × 1
- `dom:HTMLButtonElement` × 1
- `dom:HTMLCanvasElement` × 1
- `dom:HTMLCanvasElement.toBlob` × 1
- `dom:HTMLElement.click` × 1
- `dom:HTMLElement.spellcheck` × 1
- `dom:HTMLElement.title` × 1
- `dom:HTMLInputElement.accept` × 1
- `dom:HTMLInputElement.autocomplete` × 1
- `dom:HTMLInputElement.files` × 1
- `dom:HTMLInputElement.setSelectionRange` × 1
- `dom:HTMLOrSVGElement.blur` × 1
- `dom:HTMLOrSVGElement.focus` × 1
- `dom:HTMLSelectElement` × 1
- `dom:HTMLSelectElement.options` × 1
- `dom:HTMLSelectElement.value` × 1
- `dom:IDBObjectStore.getAll` × 1
- `dom:IDBObjectStore.getAllKeys` × 1
- `dom:IDBObjectStore.put` × 1
- `dom:IDBRequest.onsuccess` × 1
- `dom:KeyboardEvent` × 1
- `dom:KeyboardEvent.ctrlKey` × 1
- `dom:KeyboardEvent.metaKey` × 1
- `dom:KeyboardEvent.shiftKey` × 1
- `dom:MessageEventTarget.onmessage` × 1
- `dom:MessageEventTarget.onmessageerror` × 1
- `dom:ParentNode.querySelector` × 1
- `dom:queueMicrotask` × 1
- `dom:Response.ok` × 1
- `dom:WebGL2RenderingContextBase.getBufferSubData` × 1
- `dom:WebGL2RenderingContextOverloads.bufferData` × 1
- `dom:WebGLRenderingContextBase.ARRAY_BUFFER` × 1
- `dom:WebGLRenderingContextBase.ARRAY_BUFFER_BINDING` × 1
- `dom:WebGLRenderingContextBase.BYTE` × 1
- `dom:WebGLRenderingContextBase.createBuffer` × 1
- `dom:WebGLRenderingContextBase.ELEMENT_ARRAY_BUFFER_BINDING` × 1
- `dom:WebGLRenderingContextBase.FLOAT` × 1
- `dom:WebGLRenderingContextBase.getParameter` × 1
- `dom:WebGLRenderingContextBase.INT` × 1
- `dom:WebGLRenderingContextBase.SHORT` × 1
- `dom:WebGLRenderingContextBase.STATIC_DRAW` × 1
- `dom:WebGLRenderingContextBase.UNSIGNED_BYTE` × 1
- `dom:WebGLRenderingContextBase.UNSIGNED_SHORT` × 1
- `dom:WheelEvent.deltaY` × 1
- `dom:Window.prompt` × 1
- `dom:Worker.terminate` × 1
- `es:Array.at` × 1
- `es:ArrayLike.length` × 1
- `es:ConstructorParameters` × 1
- `es:DataView.setFloat64` × 1
- `es:DataView.setInt32` × 1
- `es:DataView.setUint32` × 1
- `es:Error.message` × 1
- `es:Error.stack` × 1
- `es:Exclude` × 1
- `es:Float32Array.byteOffset` × 1
- `es:Float32Array.copyWithin` × 1
- `es:Float64Array.every` × 1
- `es:Float64Array.fill` × 1
- `es:Float64Array.sort` × 1
- `es:Function.toString` × 1
- `es:Int32Array.buffer` × 1
- `es:Int32Array.set` × 1
- `es:Int32Array.slice` × 1
- `es:Int8Array.fill` × 1
- `es:Int8Array.length` × 1
- `es:Int8Array.subarray` × 1
- `es:IteratorReturnResult.done` × 1
- `es:IteratorYieldResult.done` × 1
- `es:Map.entries` × 1
- `es:Math.atan` × 1
- `es:Math.log2` × 1
- `es:Math.tan` × 1
- `es:NumberConstructor.MAX_SAFE_INTEGER` × 1
- `es:ObjectConstructor.getPrototypeOf` × 1
- `es:ObjectConstructor.isFrozen` × 1
- `es:ObjectConstructor.prototype` × 1
- `es:RangeError` × 1
- `es:ReadonlyArray.at` × 1
- `es:ReadonlyArray.entries` × 1
- `es:ReadonlyArray.findIndex` × 1
- `es:SharedArrayBuffer.byteLength` × 1
- `es:String.lastIndexOf` × 1
- `es:String.toUpperCase` × 1
- `es:StringConstructor.fromCharCode` × 1
- `es:Uint16Array.copyWithin` × 1
- `es:Uint16Array.fill` × 1
- `es:Uint32Array.sort` × 1
- `es:Uint32Array.subarray` × 1
- `es:Uint8Array.byteLength` × 1
- `meshoptimizer:MeshoptSimplifier.ready` × 1
- `meshoptimizer:MeshoptSimplifier.simplifyWithAttributes` × 1
- `polygon-clipping:union` × 1
- `three:AmbientLight` × 1
- `three:BatchedMesh.deleteGeometry` × 1
- `three:BatchedMesh.deleteInstance` × 1
- `three:BatchedMesh.dispose` × 1
- `three:BatchedMesh.optimize` × 1
- `three:BatchedMesh.setGeometryAt` × 1
- `three:BatchedMesh.setMatrixAt` × 1
- `three:Box3.applyMatrix4` × 1
- `three:Box3.copy` × 1
- `three:Box3.expandByObject` × 1
- `three:BufferAttribute.addUpdateRange` × 1
- `three:BufferAttribute.getW` × 1
- `three:BufferAttribute.gpuType` × 1
- `three:BufferGeometry.count` × 1
- `three:BufferGeometry.setDrawRange` × 1
- `three:BufferGeometry.start` × 1
- `three:BufferGeometry.type` × 1
- `three:Camera.matrixWorldInverse` × 1
- `three:Color.convertSRGBToLinear` × 1
- `three:Color.toArray` × 1
- `three:DataTextureImageData.data` × 1
- `three:DataTextureImageData.height` × 1
- `three:DirectionalLight` × 1
- `three:EventDispatcher.addEventListener` × 1
- `three:EventDispatcher.removeEventListener` × 1
- `three:ExtrudeGeometry` × 1
- `three:Float32BufferAttribute` × 1
- `three:GeometryGroup.materialIndex` × 1
- `three:GLTF.scene` × 1
- `three:GLTFLoader` × 1
- `three:GLTFLoader.parseAsync` × 1
- `three:InstancedMesh.instanceMatrix` × 1
- `three:InstancedMesh.setMatrixAt` × 1
- `three:InterleavedBuffer.addUpdateRange` × 1
- `three:InterleavedBuffer.needsUpdate` × 1
- `three:InterleavedBuffer.stride` × 1
- `three:InterleavedBufferAttribute.getComponent` × 1
- `three:InterleavedBufferAttribute.itemSize` × 1
- `three:InterleavedBufferAttribute.normalized` × 1
- `three:InterleavedBufferAttribute.offset` × 1
- `three:Intersection` × 1
- `three:Layers.disableAll` × 1
- `three:Material.uuid` × 1
- `three:MaterialProperties.alphaTest` × 1
- `three:MaterialProperties.side` × 1
- `three:MaterialProperties.visible` × 1
- `three:Matrix3` × 1
- `three:Matrix3.getNormalMatrix` × 1
- `three:Matrix4.compose` × 1
- `three:Matrix4.fromArray` × 1
- `three:MeshBasicMaterialProperties.color` × 1
- `three:MeshLambertMaterial` × 1
- `three:MeshStandardMaterialProperties.envMapIntensity` × 1
- `three:MeshStandardMaterialProperties.metalness` × 1
- `three:MeshStandardMaterialProperties.roughness` × 1
- `three:Object3D.clone` × 1
- `three:Object3D.layers` × 1
- `three:Object3D.onBeforeRender` × 1
- `three:Object3D.raycast` × 1
- `three:PerspectiveCamera.fov` × 1
- `three:PerspectiveCamera.updateProjectionMatrix` × 1
- `three:Quaternion` × 1
- `three:Quaternion.setFromAxisAngle` × 1
- `three:Raycaster` × 1
- `three:RenderTarget.dispose` × 1
- `three:RGFormat` × 1
- `three:ShaderChunk` × 1
- `three:ShaderChunk.envmap_physical_pars_fragment` × 1
- `three:ShaderMaterial` × 1
- `three:Sphere.clone` × 1
- `three:Texture` × 1
- `three:Texture.clearUpdateRanges` × 1
- `three:UnsignedByteType` × 1
- `three:Vector2` × 1
- `three:Vector2.x` × 1
- `three:Vector2.y` × 1
- `three:Vector3.addScaledVector` × 1
- `three:Vector3.applyMatrix3` × 1
- `three:Vector3.clone` × 1
- `three:Vector3.copy` × 1
- `three:Vector3.cross` × 1
- `three:Vector3.dot` × 1
- `three:Vector3.equals` × 1
- `three:Vector3.length` × 1
- `three:Vector3.project` × 1
- `three:Vector4` × 1
- `three:Vector4.z` × 1
- `three:WebGLRenderer.domElement` × 1
- `three:WebGLRenderer.forceContextLoss` × 1
- `three:WebGLRenderer.setPixelRatio` × 1
- `three:WebGLRenderer.setSize` × 1
- `three:WebGLRenderTarget` × 1
- `vite:ImportMeta.glob` × 1
