# Feature: src/editor/roadTool.ts, src/editor/roadPath.ts, src/editor/roadTop.ts, src/editor/roadParking.ts, src/editor/nodeMover.ts, src/editor/roundabout.ts, src/editor/snap.ts, src/editor/streetscapeTool.ts, src/editor/streetObjects.ts, src/editor/roads/economy.ts, src/editor/roads/powerLine.ts, src/editor/roads/profile.ts, src/editor/roads/streetKit.ts, src/world/roads/buildMode.ts, src/world/roads/catalog.ts, src/world/roads/connectors.ts, src/world/roads/derivedSigns.ts, src/world/roads/footwayRise.ts, src/world/roads/furnitureCatalog.ts, src/world/roads/furnitureSets.ts, src/world/roads/laneEnds.ts, src/world/roads/markingStyle.ts, src/world/roads/profile.ts, src/world/roads/rules.ts, src/world/roads/streetChain.ts, src/world/roads/streetNames.ts, src/world/roads/templates.ts, src/world/roads/tuning.ts, src/world/junction/build.ts, src/world/junction/bulb.ts, src/world/junction/corners.ts, src/world/junction/derive.ts, src/world/junction/legs.ts, src/world/junction/levels.ts, src/world/junction/mouthOverlap.ts, src/world/junction/polygon.ts, src/world/junction/transition.ts, src/world/junction/trim.ts, src/world/junction/trimSummary.ts, src/world/junction/types.ts, src/ui/roads/catalogPanel.ts, src/ui/roads/connectorPanel.ts, src/ui/roads/crossSection.ts, src/ui/roads/cutWalls.ts, src/ui/roads/drawProfile.ts, src/ui/roads/furnitureChoice.ts, src/ui/roads/junctionPanel.ts, src/ui/roads/keys.ts, src/ui/roads/keysPanel.ts, src/ui/roads/money.ts, src/ui/roads/profileEditor.ts, src/ui/roads/profilePanel.ts, src/ui/roads/profileSwatch.ts, src/ui/roads/templates.ts, src/world/junctionBuild.ts, src/world/junctionTurns.ts, src/world/network.ts, src/world/networkBuild.ts, src/world/networkDelta.ts, src/world/networkLength.ts, src/world/rampRise.ts, src/world/roadField.ts, src/world/roadSection.ts, src/world/roadTypes.ts, src/world/elevation.ts, src/world/lanelets.ts, src/world/markings.ts, src/world/curbRamps.ts, src/world/surfaces.ts, src/world/surfaceParts.ts, src/world/carriageway.ts, src/world/paving.ts, src/world/ribbons.ts, src/world/gullies.ts, src/world/legAngles.ts, src/world/streetFurniture.ts, src/render/roadSurfaces.ts, src/render/roadSurfaceEntries.ts, src/render/roadSurfaceInputs.ts, src/render/roadSurfacePass.ts, src/render/roadSurfaceReads.ts, src/render/roadSurfaceRings.ts, src/render/roadTileMesh.ts, src/render/roadTilePool.ts, src/render/roadTileShape.ts, src/render/roadTones.ts, src/render/roadsRead.ts, src/render/roadStandIn.ts, src/render/roadProgramStandIns.ts, src/render/roadStations.ts, src/render/markings.ts, src/render/paintQuads.ts, src/render/platePaint.ts, src/render/networkFarRoads.ts, src/render/networkFarRoadsBuild.ts, src/render/mesh/roadEdges.ts, src/render/mesh/roadLod.ts, src/render/mesh/roadLodPure.ts, src/render/mesh/roadResidency.ts, src/render/mesh/surfaceMesh.ts, src/render/mesh/surfaceTiles.ts, src/render/surfaceBakeClient.ts, src/render/surfaceTextures.ts, src/render/bake/roadRecipes.ts, src/render/streetFurnitureLayer.ts, src/ui/roadSectionEditor.ts, src/ui/roadSwatch.ts, src/world/cityGen/streets.ts, src/world/cityGen/plan.ts

The program starts from `src/main.ts`, `src/peopleLab/main.ts`. A declaration is the feature's own when the program reaches it only through the feature; in a declaration of the program that names the feature, its statements are a slice (what names it, what depends on that, what that controls).

- Own: 2280 declarations
- Slices of shared code: 872
- Code: 35518 lines in 271 files (source/)
- Resources: 1111 (727 entry, 383 style, 1 import; 694 only the feature uses)
- Uses: 558 shared declarations (interface/: 2502 declarations in 332 files, the types they name included); hooks: 436; external APIs: 475

## Code

- src/actionsWiring.ts — part, 3 lines
- src/buildingsWiring.ts — part, 3 lines
- src/conflictTable.worker.ts — part, 14 lines
- src/core/aabb.ts — part, 6 lines
- src/core/axisCut.ts — whole, 232 lines
- src/core/clipper.ts — part, 12 lines
- src/core/fillet.ts — part, 7 lines
- src/core/forkMap.ts — part, 63 lines
- src/core/intersect.ts — part, 12 lines
- src/core/recentCache.ts — part, 29 lines
- src/core/regionIndex.ts — part, 520 lines
- src/core/rerootMap.ts — whole, 105 lines
- src/core/ring.ts — part, 199 lines
- src/core/vec2.ts — part, 5 lines
- src/editor/barriers.ts — part, 34 lines
- src/editor/blocks.ts — part, 14 lines
- src/editor/buildings.ts — part, 4 lines
- src/editor/buildingSnap.ts — part, 65 lines
- src/editor/bulldozer.ts — part, 45 lines
- src/editor/cityGenerator.ts — part, 44 lines
- src/editor/commit.ts — part, 499 lines
- src/editor/editRules.ts — part, 125 lines
- src/editor/growthFeed.ts — part, 131 lines
- src/editor/growthReplica.ts — part, 19 lines
- src/editor/history.ts — part, 10 lines
- src/editor/lotTool.ts — part, 47 lines
- src/editor/nodeMover.ts — whole, 121 lines
- src/editor/persistence.ts — part, 15 lines
- src/editor/poleRoutes.ts — part, 25 lines
- src/editor/poles.ts — part, 116 lines
- src/editor/repair.ts — part, 38 lines
- src/editor/roadParking.ts — whole, 41 lines
- src/editor/roadPath.ts — whole, 109 lines
- src/editor/roads/economy.ts — whole, 23 lines
- src/editor/roads/powerLine.ts — whole, 95 lines
- src/editor/roads/profile.ts — whole, 35 lines
- src/editor/roads/streetKit.ts — whole, 91 lines
- src/editor/roadTool.ts — whole, 600 lines
- src/editor/roadTop.ts — whole, 94 lines
- src/editor/roundabout.ts — whole, 86 lines
- src/editor/smallCityMap.ts — part, 39 lines
- src/editor/snap.ts — whole, 425 lines
- src/editor/streetObjects.ts — whole, 106 lines
- src/editor/streetscapeTool.ts — whole, 155 lines
- src/editor/transitTools.ts — part, 4 lines
- src/jobs/jobPool.ts — part, 4 lines
- src/lab/catalog/city.ts — part, 3 lines
- src/lab/catalog/layers.ts — part, 47 lines
- src/lab/catalog/layout.ts — part, 39 lines
- src/main.ts — part, 1086 lines
- src/networkBake.worker.ts — part, 6 lines
- src/render/agents.ts — part, 23 lines
- src/render/bake/roadRecipes.ts — whole, 218 lines
- src/render/buildings/cellPool.ts — part, 14 lines
- src/render/buildings/cellWork.ts — part, 12 lines
- src/render/buildings/finishes.ts — part, 44 lines
- src/render/densityCars.ts — part, 57 lines
- src/render/flatAtlas.ts — part, 9 lines
- src/render/grass.ts — whole, 98 lines
- src/render/groundShape.ts — part, 77 lines
- src/render/ktxParse.d.ts — part, 57 lines
- src/render/markings.ts — whole, 62 lines
- src/render/materials.ts — part, 82 lines
- src/render/mesh/detailLayer.ts — part, 6 lines
- src/render/mesh/farBlockBatches.ts — part, 13 lines
- src/render/mesh/roadEdges.ts — whole, 53 lines
- src/render/mesh/roadLod.ts — whole, 79 lines
- src/render/mesh/roadLodPure.ts — whole, 82 lines
- src/render/mesh/roadResidency.ts — whole, 353 lines
- src/render/mesh/stream.ts — whole, 55 lines
- src/render/mesh/surfaceMesh.ts — whole, 48 lines
- src/render/mesh/surfaceTiles.ts — whole, 242 lines
- src/render/mesh/tileMesh.ts — part, 265 lines
- src/render/mesh/tileShape.ts — whole, 578 lines
- src/render/networkFarRoads.ts — whole, 281 lines
- src/render/networkFarRoads.worker.ts — part, 14 lines
- src/render/networkFarRoadsBuild.ts — whole, 188 lines
- src/render/paintQuads.ts — whole, 119 lines
- src/render/platePaint.ts — whole, 37 lines
- src/render/poleBake.ts — part, 6 lines
- src/render/poleCells.ts — part, 25 lines
- src/render/poleLayer.ts — part, 23 lines
- src/render/propCells.ts — part, 4 lines
- src/render/propGeometry.ts — part, 17 lines
- src/render/renderer.ts — part, 1214 lines
- src/render/roadProgramStandIns.ts — whole, 59 lines
- src/render/roadsRead.ts — whole, 13 lines
- src/render/roadStandIn.ts — whole, 135 lines
- src/render/roadStations.ts — whole, 84 lines
- src/render/roadStations.worker.ts — part, 8 lines
- src/render/roadSurfaceEntries.ts — whole, 200 lines
- src/render/roadSurfaceInputs.ts — whole, 776 lines
- src/render/roadSurfacePass.ts — whole, 143 lines
- src/render/roadSurfaceReads.ts — whole, 360 lines
- src/render/roadSurfaceRings.ts — whole, 78 lines
- src/render/roadSurfaces.ts — whole, 824 lines
- src/render/roadTile.worker.ts — part, 59 lines
- src/render/roadTileMesh.ts — whole, 173 lines
- src/render/roadTilePool.ts — whole, 208 lines
- src/render/roadTileShape.ts — whole, 84 lines
- src/render/roadTones.ts — whole, 20 lines
- src/render/scenery.ts — part, 3 lines
- src/render/signals.ts — part, 92 lines
- src/render/signLayer.ts — part, 90 lines
- src/render/signs.ts — part, 20 lines
- src/render/streetFurnitureLayer.ts — whole, 471 lines
- src/render/structureCells.ts — part, 28 lines
- src/render/structures.ts — part, 291 lines
- src/render/surfaceBakeClient.ts — whole, 122 lines
- src/render/surfaceTextures.ts — whole, 105 lines
- src/render/terrain.ts — part, 12 lines
- src/render/utilities.ts — part, 11 lines
- src/sim/agents/bay.ts — part, 3 lines
- src/sim/agents/kerbBays.ts — part, 88 lines
- src/sim/agents/laneGrid.ts — part, 95 lines
- src/sim/agents/lorryDocks.ts — part, 31 lines
- src/sim/agents/lorryTraffic.ts — part, 47 lines
- src/sim/agents/lotBays.ts — part, 100 lines
- src/sim/agents/lotTraffic.ts — part, 12 lines
- src/sim/agents/parking.ts — part, 16 lines
- src/sim/agents/walk.ts — part, 32 lines
- src/sim/ambient/ambient.ts — part, 71 lines
- src/sim/ambient/ambientIndex.ts — part, 30 lines
- src/sim/crossings/permission.ts — part, 54 lines
- src/sim/drive/tactical.ts — part, 84 lines
- src/sim/host/protocol.ts — part, 19 lines
- src/sim/host/simAnswers.ts — part, 49 lines
- src/sim/host/simClient.ts — part, 10 lines
- src/sim/host/simHost.ts — part, 50 lines
- src/sim/intersections/admission.ts — part, 521 lines
- src/sim/intersections/claims.ts — part, 63 lines
- src/sim/intersections/crossingSpans.ts — part, 78 lines
- src/sim/intersections/cycles.ts — part, 163 lines
- src/sim/intersections/spillback.ts — part, 41 lines
- src/sim/invariants.ts — part, 103 lines
- src/sim/meso/meso.ts — part, 19 lines
- src/sim/peds/sidewalk.ts — part, 89 lines
- src/sim/peds/signalPostIndex.ts — part, 11 lines
- src/sim/people/engine.ts — part, 4 lines
- src/sim/pipeline.ts — part, 161 lines
- src/sim/pose.ts — part, 16 lines
- src/sim/roads/controlAdvisor.ts — part, 14 lines
- src/sim/roads/flowStats.ts — part, 15 lines
- src/sim/roads/greenWave.ts — part, 60 lines
- src/sim/routing/destination.ts — part, 46 lines
- src/sim/routing/laneReach.ts — part, 219 lines
- src/sim/routing/lowerBound.ts — part, 14 lines
- src/sim/routing/routeIndex.ts — part, 20 lines
- src/sim/routing/router.ts — part, 224 lines
- src/sim/routing/searchScratch.ts — part, 12 lines
- src/sim/signals/fsm.ts — part, 119 lines
- src/sim/signals/plan.ts — part, 217 lines
- src/sim/topologyCatchUp.ts — part, 18 lines
- src/sim/transit/transit.ts — part, 34 lines
- src/sim/vehicles/curvature.ts — part, 56 lines
- src/sim/vehicles/cycleLane.ts — part, 15 lines
- src/sim/vehicles/integrate.ts — part, 150 lines
- src/sim/vehicles/kerbStops.ts — part, 100 lines
- src/sim/vehicles/laneChange.ts — part, 316 lines
- src/sim/vehicles/laneClosures.ts — part, 42 lines
- src/sim/vehicles/leaderIndex.ts — part, 86 lines
- src/sim/vehicles/obstacles.ts — part, 124 lines
- src/sim/vehicles/spawn.ts — part, 104 lines
- src/sim/vehicles/state.ts — part, 30 lines
- src/sim/vehicleSignals.ts — part, 35 lines
- src/sim/world.ts — part, 418 lines
- src/ui/inspector.ts — part, 151 lines
- src/ui/labels.ts — part, 8 lines
- src/ui/overlay/congestion.ts — part, 44 lines
- src/ui/roads/catalogPanel.ts — whole, 187 lines
- src/ui/roads/connectorPanel.ts — whole, 177 lines
- src/ui/roads/crossSection.ts — whole, 318 lines
- src/ui/roads/cutWalls.ts — whole, 18 lines
- src/ui/roads/drawProfile.ts — whole, 19 lines
- src/ui/roads/furnitureChoice.ts — whole, 23 lines
- src/ui/roads/junctionPanel.ts — whole, 326 lines
- src/ui/roads/keys.ts — whole, 111 lines
- src/ui/roads/keysPanel.ts — whole, 100 lines
- src/ui/roads/money.ts — whole, 19 lines
- src/ui/roads/profileEditor.ts — whole, 755 lines
- src/ui/roads/profilePanel.ts — whole, 84 lines
- src/ui/roads/profileSwatch.ts — whole, 137 lines
- src/ui/roads/templates.ts — whole, 54 lines
- src/ui/roadSectionEditor.ts — whole, 129 lines
- src/ui/roadSwatch.ts — whole, 98 lines
- src/ui/v2/shell.ts — part, 52 lines
- src/view/cameraMotion.ts — part, 3 lines
- src/world/approach.ts — part, 2 lines
- src/world/approachGroups.ts — part, 58 lines
- src/world/buildings/validate.ts — part, 24 lines
- src/world/carriageway.ts — whole, 44 lines
- src/world/cells.ts — part, 7 lines
- src/world/cityGen/field.ts — part, 54 lines
- src/world/cityGen/largeCity.ts — part, 3 lines
- src/world/cityGen/plan.ts — whole, 157 lines
- src/world/cityGen/smallCity.ts — part, 5 lines
- src/world/cityGen/squares.ts — part, 6 lines
- src/world/cityGen/streets.ts — whole, 484 lines
- src/world/cityGen/wholeMap.ts — part, 2 lines
- src/world/conflictPoints.ts — part, 236 lines
- src/world/curbRamps.ts — whole, 204 lines
- src/world/doc.ts — part, 267 lines
- src/world/economy.ts — part, 104 lines
- src/world/elevation.ts — whole, 1541 lines
- src/world/footwayPath.ts — part, 13 lines
- src/world/geometry.ts — part, 63 lines
- src/world/grid.ts — part, 10 lines
- src/world/growth/zoning.ts — part, 5 lines
- src/world/gullies.ts — whole, 24 lines
- src/world/junction/build.ts — whole, 299 lines
- src/world/junction/bulb.ts — whole, 136 lines
- src/world/junction/corners.ts — whole, 205 lines
- src/world/junction/derive.ts — whole, 203 lines
- src/world/junction/legs.ts — whole, 165 lines
- src/world/junction/levels.ts — whole, 345 lines
- src/world/junction/mouthOverlap.ts — whole, 26 lines
- src/world/junction/polygon.ts — whole, 239 lines
- src/world/junction/transition.ts — whole, 120 lines
- src/world/junction/trim.ts — whole, 55 lines
- src/world/junction/trimSummary.ts — whole, 42 lines
- src/world/junction/types.ts — whole, 95 lines
- src/world/junctionBuild.ts — whole, 416 lines
- src/world/junctionTurns.ts — whole, 84 lines
- src/world/landscape.ts — part, 234 lines
- src/world/lanelets.ts — whole, 846 lines
- src/world/legAngles.ts — whole, 45 lines
- src/world/lots.ts — part, 191 lines
- src/world/markings.ts — whole, 469 lines
- src/world/nearestRibbon.ts — part, 11 lines
- src/world/network.ts — whole, 381 lines
- src/world/networkBuild.ts — whole, 798 lines
- src/world/networkDelta.ts — whole, 123 lines
- src/world/networkLength.ts — whole, 45 lines
- src/world/parkingLayout.ts — part, 105 lines
- src/world/paving.ts — whole, 64 lines
- src/world/pedestrianAccess.ts — part, 3 lines
- src/world/poleLines.ts — part, 178 lines
- src/world/rampRise.ts — whole, 95 lines
- src/world/ribbons.ts — whole, 212 lines
- src/world/roadField.ts — whole, 751 lines
- src/world/roads/buildMode.ts — whole, 125 lines
- src/world/roads/catalog.ts — whole, 110 lines
- src/world/roads/connectors.ts — whole, 84 lines
- src/world/roads/derivedSigns.ts — whole, 84 lines
- src/world/roads/footwayRise.ts — whole, 92 lines
- src/world/roads/furnitureCatalog.ts — whole, 29 lines
- src/world/roads/furnitureSets.ts — whole, 165 lines
- src/world/roads/laneEnds.ts — whole, 42 lines
- src/world/roads/markingStyle.ts — whole, 87 lines
- src/world/roads/profile.ts — whole, 268 lines
- src/world/roads/rules.ts — whole, 143 lines
- src/world/roads/streetChain.ts — whole, 69 lines
- src/world/roads/streetNames.ts — whole, 142 lines
- src/world/roads/templates.ts — whole, 90 lines
- src/world/roads/tuning.ts — whole, 86 lines
- src/world/roadSection.ts — whole, 271 lines
- src/world/roadTypes.ts — whole, 428 lines
- src/world/section.ts — part, 24 lines
- src/world/signalPosts.ts — part, 34 lines
- src/world/streetFurniture.ts — whole, 124 lines
- src/world/structures.ts — part, 11 lines
- src/world/surfaceParts.ts — whole, 59 lines
- src/world/surfaces.ts — whole, 151 lines
- src/world/turnDigest.ts — part, 24 lines
- src/world/turnPaths.ts — part, 136 lines
- src/world/utilities.ts — part, 3 lines
- src/world/viewCells.ts — part, 18 lines
- src/world/walkable.ts — part, 121 lines
- src/world/walkingLines.ts — part, 19 lines
- src/world/walkways.ts — part, 177 lines
- src/world/zoneGrid.ts — part, 73 lines

## Slices of shared code

- `src/actionsWiring.ts#ActionsDeps` — 3 of 23 lines
- `src/buildingsWiring.ts#BuildingWiringDeps` — 3 of 17 lines
- `src/conflictTable.worker.ts#serve` — 14 of 152 lines
- `src/core/ring.ts#Ring.isEmpty` — 3 of 3 lines
- `src/editor/barriers.ts#BarrierToolHost` — 3 of 10 lines
- `src/editor/barriers.ts#barrierProblem` — 6 of 22 lines
- `src/editor/barriers.ts#onCarriageway` — 6 of 10 lines
- `src/editor/barriers.ts#snapBarrierPoint` — 19 of 22 lines
- `src/editor/blocks.ts#commitBlockGrid` — 14 of 18 lines
- `src/editor/buildingSnap.ts#cornerFlush` — 30 of 37 lines
- `src/editor/buildingSnap.ts#snapPlacement` — 6 of 25 lines
- `src/editor/buildingSnap.ts#snapToRoad` — 29 of 32 lines
- `src/editor/buildings.ts#clearBuildingsAfterEdit` — 4 of 5 lines
- `src/editor/bulldozer.ts#Bulldozer.boxed` — 7 of 25 lines
- `src/editor/bulldozer.ts#Bulldozer.click` — 22 of 29 lines
- `src/editor/bulldozer.ts#Bulldozer.down` — 3 of 3 lines
- `src/editor/bulldozer.ts#Bulldozer.drag` — 1 of 1 lines
- `src/editor/bulldozer.ts#BulldozerHost` — 3 of 13 lines
- `src/editor/bulldozer.ts#removeRoads` — 9 of 9 lines
- `src/editor/cityGenerator.ts#greenCity` — 8 of 17 lines
- `src/editor/cityGenerator.ts#layCity` — 8 of 10 lines
- `src/editor/cityGenerator.ts#widenCityFootways` — 9 of 10 lines
- `src/editor/cityGenerator.ts#zoneCity` — 11 of 31 lines
- `src/editor/cityGenerator.ts#zoneCity.mainsNear` — 8 of 10 lines
- `src/editor/commit.ts#AUTO_TUNNEL_COVER` — 1 of 1 lines
- `src/editor/commit.ts#CROSSING_CLEARANCE` — 1 of 1 lines
- `src/editor/commit.ts#DraftResult` — 4 of 14 lines
- `src/editor/commit.ts#boreDeepCuts` — 28 of 38 lines
- `src/editor/commit.ts#commitDraftInPlace` — 102 of 147 lines
- `src/editor/commit.ts#commitRoadPath` — 73 of 97 lines
- `src/editor/commit.ts#draftStations` — 19 of 24 lines
- `src/editor/commit.ts#dryRunKey` — 4 of 4 lines
- `src/editor/commit.ts#duplicateSegment` — 8 of 37 lines
- `src/editor/commit.ts#joinSegments` — 20 of 47 lines
- `src/editor/commit.ts#joinShortPieces` — 19 of 19 lines
- `src/editor/commit.ts#lastDryRun` — 6 of 6 lines
- `src/editor/commit.ts#layCopy` — 8 of 11 lines
- `src/editor/commit.ts#materializeEndpoints` — 14 of 51 lines
- `src/editor/commit.ts#materializeEndpoints.queue` — 26 of 26 lines
- `src/editor/commit.ts#materializeFree` — 7 of 16 lines
- `src/editor/commit.ts#onTerrain` — 1 of 1 lines
- `src/editor/commit.ts#solveOver` — 8 of 9 lines
- `src/editor/commit.ts#splitSegment` — 5 of 10 lines
- `src/editor/commit.ts#splitSegmentAtCuts` — 49 of 111 lines
- `src/editor/commit.ts#terrainIndexOf` — 6 of 6 lines
- `src/editor/editRules.ts#MAX_BUILT_GRADE` — 1 of 1 lines
- `src/editor/editRules.ts#PASS_CLEARANCE` — 1 of 1 lines
- `src/editor/editRules.ts#RoadState` — 3 of 5 lines
- `src/editor/editRules.ts#flatSolves` — 4 of 7 lines
- `src/editor/editRules.ts#flatSolves.after` — 1 of 1 lines
- `src/editor/editRules.ts#flatSolves.beforeSolve` — 1 of 1 lines
- `src/editor/editRules.ts#grade` — 11 of 23 lines
- `src/editor/editRules.ts#guardRoadEdit` — 11 of 15 lines
- `src/editor/editRules.ts#overlapReach` — 7 of 7 lines
- `src/editor/editRules.ts#overlapsAnother` — 41 of 49 lines
- `src/editor/editRules.ts#refuseRoadEdit` — 10 of 18 lines
- `src/editor/editRules.ts#refuseRoadEdit.deck` — 1 of 1 lines
- `src/editor/editRules.ts#segmentsThrough` — 13 of 15 lines
- `src/editor/editRules.ts#tooSteep` — 9 of 19 lines
- `src/editor/editRules.ts#withinSharedPlate` — 5 of 7 lines
- `src/editor/editRules.ts#written` — 6 of 7 lines
- `src/editor/growthFeed.ts#GrowthHost` — 4 of 9 lines
- `src/editor/growthFeed.ts#GrowthSession.ask` — 3 of 12 lines
- `src/editor/growthFeed.ts#GrowthSession.busy` — 3 of 3 lines
- `src/editor/growthFeed.ts#GrowthSession.readChanges` — 11 of 17 lines
- `src/editor/growthFeed.ts#GrowthSession.receive` — 46 of 63 lines
- `src/editor/growthFeed.ts#GrowthSession.restart` — 5 of 9 lines
- `src/editor/growthFeed.ts#GrowthSession.roadsOf` — 1 of 1 lines
- `src/editor/growthFeed.ts#GrowthSession.send` — 16 of 48 lines
- `src/editor/growthFeed.ts#GrowthSession.syncRoads` — 26 of 33 lines
- `src/editor/growthFeed.ts#junctionPiece` — 10 of 10 lines
- `src/editor/growthFeed.ts#roadPiece` — 6 of 6 lines
- `src/editor/growthReplica.ts#GrowAsk` — 3 of 30 lines
- `src/editor/growthReplica.ts#RoadReplica.apply` — 7 of 25 lines
- `src/editor/growthReplica.ts#RoadReplica.kerbs` — 1 of 1 lines
- `src/editor/growthReplica.ts#pavedLookup` — 8 of 10 lines
- `src/editor/history.ts#followNetwork` — 3 of 3 lines
- `src/editor/history.ts#restoreInto` — 4 of 10 lines
- `src/editor/history.ts#restoreSnapshot` — 3 of 5 lines
- `src/editor/lotTool.ts#LotTool.frontSideOf` — 6 of 10 lines
- `src/editor/lotTool.ts#LotTool.landLot` — 4 of 4 lines
- `src/editor/lotTool.ts#LotTool.net` — 1 of 1 lines
- `src/editor/lotTool.ts#LotTool.proposalKey` — 6 of 8 lines
- `src/editor/lotTool.ts#LotTool.refreshGesture` — 6 of 65 lines
- `src/editor/lotTool.ts#LotTool.refusedNow` — 10 of 11 lines
- `src/editor/lotTool.ts#LotTool.snap` — 1 of 1 lines
- `src/editor/lotTool.ts#LotTool.snapExcept` — 3 of 4 lines
- `src/editor/lotTool.ts#LotTool.streetAngleNear` — 7 of 7 lines
- `src/editor/lotTool.ts#LotToolHost` — 3 of 15 lines
- `src/editor/persistence.ts#isSegmentDirection` — 2 of 3 lines
- `src/editor/persistence.ts#isSerializedDoc` — 13 of 98 lines
- `src/editor/poleRoutes.ts#buildRouteGraph` — 25 of 73 lines
- `src/editor/poles.ts#PoleToolHost` — 3 of 14 lines
- `src/editor/poles.ts#computePoleRun` — 45 of 91 lines
- `src/editor/poles.ts#lastPlan` — 5 of 5 lines
- `src/editor/poles.ts#planPoleRun` — 12 of 19 lines
- `src/editor/poles.ts#poleRunPlanner` — 2 of 40 lines
- `src/editor/poles.ts#poleRunPlanner.plan` — 20 of 21 lines
- `src/editor/poles.ts#regional` — 4 of 4 lines
- `src/editor/poles.ts#routeGraph` — 5 of 7 lines
- `src/editor/poles.ts#routeRun` — 12 of 25 lines
- `src/editor/poles.ts#snapOn` — 5 of 9 lines
- `src/editor/poles.ts#snapPole` — 3 of 3 lines
- `src/editor/repair.ts#nearestCandidate` — 25 of 33 lines
- `src/editor/repair.ts#repairNearConnections` — 13 of 19 lines
- `src/editor/smallCityMap.ts#buildCityMap` — 3 of 3 lines
- `src/editor/smallCityMap.ts#buildCityTable` — 12 of 17 lines
- `src/editor/smallCityMap.ts#growCity` — 16 of 24 lines
- `src/editor/smallCityMap.ts#growLots` — 7 of 17 lines
- `src/editor/smallCityMap.ts#growLots.pavedAt` — 1 of 1 lines
- `src/editor/transitTools.ts#TransitTool.onRoad` — 1 of 1 lines
- `src/editor/transitTools.ts#TransitToolDeps` — 3 of 8 lines
- `src/lab/catalog/city.ts#GameProbe` — 3 of 18 lines
- `src/lab/catalog/layers.ts#footwayOut` — 5 of 5 lines
- `src/lab/catalog/layers.ts#oneRoad` — 8 of 13 lines
- `src/lab/catalog/layers.ts#signItems.one` — 9 of 9 lines
- `src/lab/catalog/layers.ts#signalItems` — 10 of 17 lines
- `src/lab/catalog/layers.ts#transitItems` — 6 of 25 lines
- `src/lab/catalog/layers.ts#transitItems.lay` — 8 of 8 lines
- `src/lab/catalog/layout.ts#piecesOf` — 7 of 14 lines
- `src/lab/catalog/layout.ts#road` — 3 of 3 lines
- `src/lab/catalog/layout.ts#shown` — 3 of 29 lines
- `src/main.ts#<ExpressionStatement>` — 1 of 1 lines
- `src/main.ts#<ExpressionStatement>@1611` — 1 of 1 lines
- `src/main.ts#<ExpressionStatement>@657` — 1 of 1 lines
- `src/main.ts#<IfStatement>@4036` — 24 of 196 lines
- `src/main.ts#BUILD_FILL` — 7 of 7 lines
- `src/main.ts#MEDIAN_CLASS` — 1 of 1 lines
- `src/main.ts#SLIDERS` — 3 of 57 lines
- `src/main.ts#actions` — 3 of 16 lines
- `src/main.ts#afterRestore` — 4 of 11 lines
- `src/main.ts#applySnapshot` — 8 of 14 lines
- `src/main.ts#barrierTool` — 3 of 14 lines
- `src/main.ts#buildSummary` — 9 of 9 lines
- `src/main.ts#buildings` — 3 of 47 lines
- `src/main.ts#bulldozer` — 4 of 10 lines
- `src/main.ts#cancelGestures` — 3 of 13 lines
- `src/main.ts#cancelMove` — 3 of 3 lines
- `src/main.ts#clearBulbs` — 1 of 1 lines
- `src/main.ts#currentGesture` — 6 of 18 lines
- `src/main.ts#currentStatus` — 6 of 20 lines
- `src/main.ts#drawBuildModes` — 38 of 39 lines
- `src/main.ts#drawOverlayScreen` — 316 of 469 lines
- `src/main.ts#drawOverlayScreen.roadBand` — 8 of 8 lines
- `src/main.ts#drawStreetscapeHover` — 33 of 40 lines
- `src/main.ts#drawTunnelStretches` — 8 of 15 lines
- `src/main.ts#endPointer` — 8 of 30 lines
- `src/main.ts#frame` — 23 of 102 lines
- `src/main.ts#gameKeys` — 14 of 77 lines
- `src/main.ts#gestureInProgress` — 3 of 3 lines
- `src/main.ts#growZones` — 4 of 10 lines
- `src/main.ts#growth` — 3 of 3 lines
- `src/main.ts#growthAnswered` — 3 of 15 lines
- `src/main.ts#hoverAnchor` — 1 of 1 lines
- `src/main.ts#laneChoices` — 7 of 9 lines
- `src/main.ts#lotTool` — 3 of 14 lines
- `src/main.ts#mountInterface` — 19 of 88 lines
- `src/main.ts#mover` — 9 of 9 lines
- `src/main.ts#mutateBuilt` — 12 of 36 lines
- `src/main.ts#mutateRoads` — 8 of 10 lines
- `src/main.ts#net` — 1 of 1 lines
- `src/main.ts#networkBakeOf` — 2 of 17 lines
- `src/main.ts#newMap` — 5 of 13 lines
- `src/main.ts#openCity` — 4 of 15 lines
- `src/main.ts#openSession` — 7 of 21 lines
- `src/main.ts#pointerDown` — 78 of 180 lines
- `src/main.ts#pointerLeft` — 6 of 6 lines
- `src/main.ts#pointerMove` — 20 of 72 lines
- `src/main.ts#pointerWorld` — 6 of 7 lines
- `src/main.ts#poleTool` — 3 of 11 lines
- `src/main.ts#restoreBootSave` — 7 of 29 lines
- `src/main.ts#roadClasses` — 9 of 9 lines
- `src/main.ts#roadTool` — 29 of 29 lines
- `src/main.ts#roadTop` — 1 of 1 lines
- `src/main.ts#selectRoadType` — 4 of 7 lines
- `src/main.ts#setAlignment` — 3 of 6 lines
- `src/main.ts#setLanes` — 4 of 9 lines
- `src/main.ts#setMarkingStyle` — 3 of 9 lines
- `src/main.ts#setNodeHeightMetres` — 16 of 26 lines
- `src/main.ts#showInspector` — 246 of 246 lines
- `src/main.ts#sim` — 1 of 1 lines
- `src/main.ts#streetscapeTool` — 12 of 12 lines
- `src/main.ts#surfaceBake` — 1 of 1 lines
- `src/main.ts#surfaceTop` — 4 of 5 lines
- `src/main.ts#transitEditor` — 3 of 7 lines
- `src/networkBake.worker.ts#NetworkBakeReply` — 1 of 1 lines
- `src/networkBake.worker.ts#bakeNetwork` — 5 of 7 lines
- `src/render/agents.ts#AgentMeshes` — 3 of 31 lines
- `src/render/agents.ts#createAgentMeshes.sync` — 20 of 421 lines
- `src/render/buildings/cellPool.ts#CellEnv` — 3 of 5 lines
- `src/render/buildings/cellPool.ts#CellPool.offPage` — 3 of 5 lines
- `src/render/buildings/cellPool.ts#CellPool.write` — 8 of 14 lines
- `src/render/buildings/cellWork.ts#CellJob` — 3 of 20 lines
- `src/render/buildings/cellWork.ts#wider` — 5 of 7 lines
- `src/render/buildings/cellWork.ts#workerReads` — 4 of 14 lines
- `src/render/buildings/finishes.ts#createShellMaterial` — 8 of 18 lines
- `src/render/buildings/finishes.ts#shellShader` — 36 of 85 lines
- `src/render/densityCars.ts#DensityCars` — 3 of 6 lines
- `src/render/densityCars.ts#createDensityCars.readLines` — 5 of 28 lines
- `src/render/densityCars.ts#createDensityCars.syncLines` — 11 of 11 lines
- `src/render/densityCars.ts#createDensityCars.writeLine` — 30 of 40 lines
- `src/render/densityCars.ts#createDensityCars.writeTile` — 8 of 61 lines
- `src/render/flatAtlas.ts#createFlatAtlas.takePaint` — 6 of 8 lines
- `src/render/flatAtlas.ts#createFlatAtlas.takeStamps` — 3 of 8 lines
- `src/render/groundShape.ts#BlockDigestAsk` — 3 of 7 lines
- `src/render/groundShape.ts#BlockRoads` — 1 of 1 lines
- `src/render/groundShape.ts#GroundShapeAsk` — 3 of 5 lines
- `src/render/groundShape.ts#PadsAsk` — 3 of 7 lines
- `src/render/groundShape.ts#blockDigest` — 12 of 15 lines
- `src/render/groundShape.ts#digestBlocks` — 11 of 24 lines
- `src/render/groundShape.ts#padsOfAsk` — 3 of 18 lines
- `src/render/groundShape.ts#shapeGroundAsk` — 41 of 45 lines
- `src/render/materials.ts#createMaterials` — 54 of 114 lines
- `src/render/materials.ts#createMaterials.dispose` — 3 of 5 lines
- `src/render/materials.ts#createMaterials.surface` — 25 of 25 lines
- `src/render/mesh/detailLayer.ts#detailTextures` — 6 of 7 lines
- `src/render/networkFarRoads.worker.ts#serve` — 14 of 14 lines
- `src/render/poleBake.ts#framePole` — 6 of 9 lines
- `src/render/poleCells.ts#PoleCells.build` — 20 of 38 lines
- `src/render/poleCells.ts#PoleCells.from` — 1 of 1 lines
- `src/render/poleCells.ts#PoleCells.source` — 4 of 4 lines
- `src/render/poleLayer.ts#PoleLayer.fill` — 3 of 5 lines
- `src/render/poleLayer.ts#PoleLayer.place` — 8 of 16 lines
- `src/render/poleLayer.ts#buildPolePreview` — 6 of 22 lines
- `src/render/poleLayer.ts#buildUtilities` — 6 of 11 lines
- `src/render/propGeometry.ts#drainGeometry` — 17 of 20 lines
- `src/render/renderer.ts#FAR_ROAD_CELL` — 1 of 1 lines
- `src/render/renderer.ts#SURFACE_SETS` — 1 of 1 lines
- `src/render/renderer.ts#SceneHandle` — 7 of 170 lines
- `src/render/renderer.ts#createSceneRenderer` — 637 of 3774 lines
- `src/render/renderer.ts#createSceneRenderer.applyGeometryBudget` — 4 of 8 lines
- `src/render/renderer.ts#createSceneRenderer.blockRead` — 9 of 9 lines
- `src/render/renderer.ts#createSceneRenderer.buildWorld` — 78 of 78 lines
- `src/render/renderer.ts#createSceneRenderer.compareSolves` — 74 of 74 lines
- `src/render/renderer.ts#createSceneRenderer.completeSolve` — 24 of 24 lines
- `src/render/renderer.ts#createSceneRenderer.coverFree` — 1 of 1 lines
- `src/render/renderer.ts#createSceneRenderer.deckHeight` — 4 of 8 lines
- `src/render/renderer.ts#createSceneRenderer.derived` — 3 of 3 lines
- `src/render/renderer.ts#createSceneRenderer.farCellOf` — 1 of 1 lines
- `src/render/renderer.ts#createSceneRenderer.farCellRect` — 4 of 5 lines
- `src/render/renderer.ts#createSceneRenderer.farShown` — 1 of 1 lines
- `src/render/renderer.ts#createSceneRenderer.fetchTiles` — 19 of 19 lines
- `src/render/renderer.ts#createSceneRenderer.fillStep` — 25 of 25 lines
- `src/render/renderer.ts#createSceneRenderer.followFloors` — 1 of 1 lines
- `src/render/renderer.ts#createSceneRenderer.footwayRiseHere` — 1 of 1 lines
- `src/render/renderer.ts#createSceneRenderer.landGroundDigest` — 6 of 6 lines
- `src/render/renderer.ts#createSceneRenderer.levelGroundDigest` — 7 of 8 lines
- `src/render/renderer.ts#createSceneRenderer.natureOpen` — 11 of 11 lines
- `src/render/renderer.ts#createSceneRenderer.natureStands` — 3 of 5 lines
- `src/render/renderer.ts#createSceneRenderer.networkExtent` — 11 of 11 lines
- `src/render/renderer.ts#createSceneRenderer.nextWatched` — 7 of 7 lines
- `src/render/renderer.ts#createSceneRenderer.padsAskOf` — 6 of 12 lines
- `src/render/renderer.ts#createSceneRenderer.readOf` — 5 of 5 lines
- `src/render/renderer.ts#createSceneRenderer.rebuildUtilities` — 7 of 7 lines
- `src/render/renderer.ts#createSceneRenderer.rebuildWorld` — 15 of 15 lines
- `src/render/renderer.ts#createSceneRenderer.requestNetworkFar` — 4 of 4 lines
- `src/render/renderer.ts#createSceneRenderer.shapeBlocksSteps` — 42 of 54 lines
- `src/render/renderer.ts#createSceneRenderer.shapeGround` — 47 of 47 lines
- `src/render/renderer.ts#createSceneRenderer.stationsAsked` — 2 of 2 lines
- `src/render/renderer.ts#createSceneRenderer.switchedOff` — 16 of 16 lines
- `src/render/renderer.ts#createSceneRenderer.syncUtilities` — 12 of 12 lines
- `src/render/renderer.ts#createSceneRenderer.takeEditedCells` — 26 of 26 lines
- `src/render/renderer.ts#createSceneRenderer.warmRoadShaders` — 22 of 39 lines
- `src/render/renderer.ts#createSceneRenderer.watchRoads` — 27 of 27 lines
- `src/render/renderer.ts#createSceneRenderer.worldSteps` — 42 of 105 lines
- `src/render/renderer.ts#isLevelLand` — 1 of 1 lines
- `src/render/roadStations.worker.ts#serve` — 8 of 13 lines
- `src/render/roadTile.worker.ts#Message` — 4 of 4 lines
- `src/render/roadTile.worker.ts#handler` — 42 of 58 lines
- `src/render/roadTile.worker.ts#keepTiles` — 13 of 17 lines
- `src/render/scenery.ts#SceneryKit` — 3 of 42 lines
- `src/render/signLayer.ts#SignLayerKept` — 3 of 14 lines
- `src/render/signLayer.ts#SignProduct.allKeys` — 4 of 7 lines
- `src/render/signLayer.ts#SignProduct.dirty` — 16 of 30 lines
- `src/render/signLayer.ts#SignSource` — 4 of 5 lines
- `src/render/signLayer.ts#createSignLayer.sync` — 3 of 15 lines
- `src/render/signLayer.ts#itemSigns` — 40 of 42 lines
- `src/render/signLayer.ts#junctionSigns` — 14 of 16 lines
- `src/render/signLayer.ts#ribbonBox` — 6 of 6 lines
- `src/render/signals.ts#SignalHeads` — 3 of 6 lines
- `src/render/signals.ts#createSignalHeads` — 4 of 626 lines
- `src/render/signals.ts#createSignalHeads.junctionOf` — 27 of 27 lines
- `src/render/signals.ts#createSignalHeads.placeAll` — 15 of 15 lines
- `src/render/signals.ts#createSignalHeads.placeSome` — 25 of 25 lines
- `src/render/signals.ts#createSignalHeads.sync` — 18 of 46 lines
- `src/render/signs.ts#streetChain` — 13 of 44 lines
- `src/render/signs.ts#streetChain.dirAt` — 7 of 9 lines
- `src/render/structureCells.ts#StructureCells` — 4 of 8 lines
- `src/render/structureCells.ts#cellOfRibbon` — 4 of 4 lines
- `src/render/structureCells.ts#createStructureCells.prepare` — 12 of 46 lines
- `src/render/structureCells.ts#createStructureCells.ribbonsOf` — 8 of 8 lines
- `src/render/structures.ts#buildStructureDetails` — 7 of 14 lines
- `src/render/structures.ts#onAnotherCasing` — 9 of 10 lines
- `src/render/structures.ts#retainingWalls` — 57 of 58 lines
- `src/render/structures.ts#structureParts` — 187 of 205 lines
- `src/render/structures.ts#structureRibbons` — 17 of 35 lines
- `src/render/structures.ts#structureRibbons.near` — 5 of 5 lines
- `src/render/structures.ts#structureRibbons.samples` — 9 of 9 lines
- `src/render/terrain.ts#TerrainSurface` — 3 of 84 lines
- `src/render/terrain.ts#terrainBakes` — 9 of 9 lines
- `src/render/utilities.ts#poleGroundAt` — 11 of 14 lines
- `src/sim/agents/bay.ts#BayLane` — 3 of 12 lines
- `src/sim/agents/kerbBays.ts#KerbBook` — 3 of 17 lines
- `src/sim/agents/kerbBays.ts#KerbWorld` — 4 of 7 lines
- `src/sim/agents/kerbBays.ts#fileStreet` — 9 of 17 lines
- `src/sim/agents/kerbBays.ts#followKerbs` — 46 of 79 lines
- `src/sim/agents/kerbBays.ts#kerbBaysOf` — 18 of 22 lines
- `src/sim/agents/kerbBays.ts#kerbKey` — 5 of 5 lines
- `src/sim/agents/kerbBays.ts#readStreets` — 3 of 8 lines
- `src/sim/agents/laneGrid.ts#Filed` — 3 of 8 lines
- `src/sim/agents/laneGrid.ts#LaneGrid.cells` — 1 of 1 lines
- `src/sim/agents/laneGrid.ts#LaneGrid.digest` — 6 of 6 lines
- `src/sim/agents/laneGrid.ts#LaneGrid.file` — 12 of 14 lines
- `src/sim/agents/laneGrid.ts#LaneGrid.fileAll` — 3 of 6 lines
- `src/sim/agents/laneGrid.ts#LaneGrid.filed` — 1 of 1 lines
- `src/sim/agents/laneGrid.ts#LaneGrid.follow` — 15 of 17 lines
- `src/sim/agents/laneGrid.ts#LaneGrid.gather` — 9 of 15 lines
- `src/sim/agents/laneGrid.ts#LaneGrid.inBox` — 9 of 9 lines
- `src/sim/agents/laneGrid.ts#LaneGrid.lanesIn` — 5 of 5 lines
- `src/sim/agents/laneGrid.ts#LaneGrid.near` — 2 of 4 lines
- `src/sim/agents/laneGrid.ts#LaneGrid.nearInto` — 5 of 5 lines
- `src/sim/agents/laneGrid.ts#LaneGrid.unfile` — 7 of 11 lines
- `src/sim/agents/laneGrid.ts#LaneGridWorld` — 4 of 5 lines
- `src/sim/agents/laneGrid.ts#boxOf` — 4 of 4 lines
- `src/sim/agents/laneGrid.ts#byId` — 1 of 1 lines
- `src/sim/agents/laneGrid.ts#digestOf` — 1 of 1 lines
- `src/sim/agents/laneGrid.ts#lanesInBox` — 2 of 3 lines
- `src/sim/agents/laneGrid.ts#lanesNear` — 2 of 3 lines
- `src/sim/agents/laneGrid.ts#lanesNearInto` — 3 of 3 lines
- `src/sim/agents/lorryDocks.ts#Yard.constructor` — 13 of 48 lines
- `src/sim/agents/lorryDocks.ts#asphaltNear` — 12 of 15 lines
- `src/sim/agents/lorryDocks.ts#yardInputs` — 5 of 18 lines
- `src/sim/agents/lorryDocks.ts#yardKey` — 1 of 1 lines
- `src/sim/agents/lorryTraffic.ts#Lorry` — 3 of 24 lines
- `src/sim/agents/lorryTraffic.ts#LorryTraffic.call` — 18 of 45 lines
- `src/sim/agents/lorryTraffic.ts#LorryTraffic.join` — 11 of 19 lines
- `src/sim/agents/lorryTraffic.ts#closures` — 15 of 24 lines
- `src/sim/agents/lotBays.ts#BayLaneLine` — 1 of 1 lines
- `src/sim/agents/lotBays.ts#BayReads` — 3 of 10 lines
- `src/sim/agents/lotBays.ts#BayShare` — 6 of 17 lines
- `src/sim/agents/lotBays.ts#LotAsk` — 3 of 7 lines
- `src/sim/agents/lotBays.ts#baysOfShare` — 11 of 31 lines
- `src/sim/agents/lotBays.ts#exitsOf` — 13 of 27 lines
- `src/sim/agents/lotBays.ts#gateLane` — 17 of 22 lines
- `src/sim/agents/lotBays.ts#laneFor` — 14 of 21 lines
- `src/sim/agents/lotBays.ts#shareOf` — 32 of 46 lines
- `src/sim/agents/lotTraffic.ts#LotTraffic.leaving` — 12 of 30 lines
- `src/sim/agents/parking.ts#BayWorld` — 3 of 4 lines
- `src/sim/agents/parking.ts#kerbReads.linksKey` — 6 of 10 lines
- `src/sim/agents/parking.ts#lookAtLot` — 7 of 43 lines
- `src/sim/agents/walk.ts#createAgentWalkEngine` — 32 of 276 lines
- `src/sim/ambient/ambient.ts#AmbientWorld.lanesAround` — 7 of 9 lines
- `src/sim/ambient/ambient.ts#AmbientWorld.listLanes` — 8 of 12 lines
- `src/sim/ambient/ambient.ts#AmbientWorld.openingLanes` — 3 of 4 lines
- `src/sim/ambient/ambient.ts#AmbientWorld.roadEnds` — 39 of 42 lines
- `src/sim/ambient/ambient.ts#AmbientWorld.traffic` — 11 of 45 lines
- `src/sim/ambient/ambient.ts#SceneryCar` — 3 of 9 lines
- `src/sim/ambient/ambientIndex.ts#StreetLength.add` — 16 of 16 lines
- `src/sim/ambient/ambientIndex.ts#StreetLength.around` — 9 of 12 lines
- `src/sim/ambient/ambientIndex.ts#StreetLength.pieces` — 1 of 1 lines
- `src/sim/ambient/ambientIndex.ts#StreetLength.whole` — 1 of 1 lines
- `src/sim/ambient/ambientIndex.ts#StreetLength.wholeMap` — 3 of 3 lines
- `src/sim/crossings/permission.ts#crossingReservedByVehicle` — 7 of 11 lines
- `src/sim/crossings/permission.ts#indexReservations` — 11 of 22 lines
- `src/sim/crossings/permission.ts#mayEnterCrossing` — 10 of 26 lines
- `src/sim/crossings/permission.ts#pedGapAccepted` — 8 of 21 lines
- `src/sim/crossings/permission.ts#turnOwedToVehicle` — 13 of 14 lines
- `src/sim/crossings/permission.ts#vehicleBodyOnCrossing` — 5 of 10 lines
- `src/sim/drive/tactical.ts#TripPlan` — 5 of 9 lines
- `src/sim/drive/tactical.ts#connectorCost` — 10 of 11 lines
- `src/sim/drive/tactical.ts#planTrip` — 69 of 102 lines
- `src/sim/host/protocol.ts#ToSim` — 19 of 19 lines
- `src/sim/host/simAnswers.ts#answer` — 6 of 9 lines
- `src/sim/host/simAnswers.ts#count` — 18 of 19 lines
- `src/sim/host/simAnswers.ts#junctionAnswer` — 16 of 24 lines
- `src/sim/host/simAnswers.ts#roadLanes` — 9 of 11 lines
- `src/sim/host/simClient.ts#SimClient.bake` — 5 of 7 lines
- `src/sim/host/simClient.ts#SimClient.constructor` — 1 of 1 lines
- `src/sim/host/simHost.ts#SimHost.ask` — 3 of 5 lines
- `src/sim/host/simHost.ts#SimHost.frame` — 6 of 39 lines
- `src/sim/host/simHost.ts#SimHost.init` — 19 of 32 lines
- `src/sim/host/simHost.ts#SimHost.load` — 4 of 12 lines
- `src/sim/host/simHost.ts#SimHost.lotPlan` — 4 of 6 lines
- `src/sim/host/simHost.ts#SimHost.net` — 1 of 1 lines
- `src/sim/host/simHost.ts#SimHost.publish.writeVehicle` — 4 of 32 lines
- `src/sim/host/simHost.ts#SimHost.signals.layoutOf` — 5 of 5 lines
- `src/sim/host/simHost.ts#SimHost.signals.tell` — 4 of 7 lines
- `src/sim/intersections/admission.ts#ConnectorReservation` — 3 of 4 lines
- `src/sim/intersections/admission.ts#Request` — 3 of 8 lines
- `src/sim/intersections/admission.ts#actualAllocation` — 3 of 14 lines
- `src/sim/intersections/admission.ts#admissionProbe` — 3 of 3 lines
- `src/sim/intersections/admission.ts#admit` — 76 of 117 lines
- `src/sim/intersections/admission.ts#aheadOnMovement` — 14 of 20 lines
- `src/sim/intersections/admission.ts#arrivalDirection` — 7 of 8 lines
- `src/sim/intersections/admission.ts#clearTime` — 10 of 12 lines
- `src/sim/intersections/admission.ts#comesFrom` — 7 of 8 lines
- `src/sim/intersections/admission.ts#compactReservationChain` — 26 of 33 lines
- `src/sim/intersections/admission.ts#connectorResources` — 12 of 13 lines
- `src/sim/intersections/admission.ts#continues` — 3 of 3 lines
- `src/sim/intersections/admission.ts#convoyCanEnter` — 16 of 23 lines
- `src/sim/intersections/admission.ts#crossingBusy` — 5 of 6 lines
- `src/sim/intersections/admission.ts#crossingReachedFirst` — 25 of 28 lines
- `src/sim/intersections/admission.ts#evaluate` — 52 of 82 lines
- `src/sim/intersections/admission.ts#exitSwingBlocked` — 9 of 12 lines
- `src/sim/intersections/admission.ts#hasAcceptableGap` — 34 of 41 lines
- `src/sim/intersections/admission.ts#holderState` — 9 of 15 lines
- `src/sim/intersections/admission.ts#holdsAnything` — 3 of 7 lines
- `src/sim/intersections/admission.ts#mainRoadRank` — 11 of 12 lines
- `src/sim/intersections/admission.ts#mainRoads` — 1 of 1 lines
- `src/sim/intersections/admission.ts#mergeFollows` — 7 of 18 lines
- `src/sim/intersections/admission.ts#mergeRemaining` — 8 of 9 lines
- `src/sim/intersections/admission.ts#movementIsActive` — 6 of 6 lines
- `src/sim/intersections/admission.ts#movementReservedByOther` — 10 of 16 lines
- `src/sim/intersections/admission.ts#nextConnector` — 5 of 8 lines
- `src/sim/intersections/admission.ts#outranksForExit` — 7 of 9 lines
- `src/sim/intersections/admission.ts#pedestrianHasPriority` — 30 of 32 lines
- `src/sim/intersections/admission.ts#protectedNow` — 6 of 6 lines
- `src/sim/intersections/admission.ts#queuedBodyInZone` — 18 of 20 lines
- `src/sim/intersections/admission.ts#revokeStaleGrants` — 23 of 41 lines
- `src/sim/intersections/admission.ts#rightOfWay` — 26 of 50 lines
- `src/sim/intersections/admission.ts#stopShortOfIntrusion` — 21 of 23 lines
- `src/sim/intersections/admission.ts#throughRank` — 6 of 6 lines
- `src/sim/intersections/admission.ts#yieldSide` — 7 of 8 lines
- `src/sim/intersections/admission.ts#zipperHolds` — 9 of 12 lines
- `src/sim/intersections/claims.ts#Claim` — 3 of 5 lines
- `src/sim/intersections/claims.ts#ClaimTable.available` — 11 of 26 lines
- `src/sim/intersections/claims.ts#ClaimTable.grantAll` — 10 of 17 lines
- `src/sim/intersections/claims.ts#ClaimTable.releaseConnector` — 5 of 7 lines
- `src/sim/intersections/claims.ts#ClaimTable.releasePassed` — 16 of 33 lines
- `src/sim/intersections/claims.ts#clearedAt` — 8 of 9 lines
- `src/sim/intersections/claims.ts#zoneShareable` — 10 of 11 lines
- `src/sim/intersections/crossingSpans.ts#CrossingSpans.build` — 3 of 8 lines
- `src/sim/intersections/crossingSpans.ts#CrossingSpans.measureJunction` — 48 of 59 lines
- `src/sim/intersections/crossingSpans.ts#CrossingSpans.ofJunction` — 1 of 1 lines
- `src/sim/intersections/crossingSpans.ts#CrossingSpans.span` — 3 of 3 lines
- `src/sim/intersections/crossingSpans.ts#CrossingSpans.spans` — 1 of 1 lines
- `src/sim/intersections/crossingSpans.ts#key` — 1 of 1 lines
- `src/sim/intersections/crossingSpans.ts#pedestrianInSpan` — 8 of 11 lines
- `src/sim/intersections/crossingSpans.ts#reservationCoversCrossing` — 13 of 17 lines
- `src/sim/intersections/cycles.ts#CycleIndex.byLanelet` — 1 of 1 lines
- `src/sim/intersections/cycles.ts#CycleIndex.follow` — 17 of 17 lines
- `src/sim/intersections/cycles.ts#CycleIndex.forget` — 18 of 18 lines
- `src/sim/intersections/cycles.ts#CycleIndex.laneReaders` — 1 of 1 lines
- `src/sim/intersections/cycles.ts#CycleIndex.nodeReaders` — 1 of 1 lines
- `src/sim/intersections/cycles.ts#CycleIndex.ofStart` — 1 of 1 lines
- `src/sim/intersections/cycles.ts#CycleIndex.restart` — 5 of 10 lines
- `src/sim/intersections/cycles.ts#CycleIndex.search` — 56 of 84 lines
- `src/sim/intersections/cycles.ts#CycleIndex.search.length` — 5 of 5 lines
- `src/sim/intersections/cycles.ts#CycleIndex.search.next` — 13 of 13 lines
- `src/sim/intersections/cycles.ts#OpenSet.items` — 1 of 1 lines
- `src/sim/intersections/cycles.ts#OpenSet.pop` — 2 of 19 lines
- `src/sim/intersections/cycles.ts#OpenSet.push` — 3 of 11 lines
- `src/sim/intersections/cycles.ts#RoadCycle` — 3 of 6 lines
- `src/sim/intersections/cycles.ts#before` — 2 of 2 lines
- `src/sim/intersections/cycles.ts#cycleFull` — 14 of 15 lines
- `src/sim/intersections/cycles.ts#cyclesOf` — 3 of 3 lines
- `src/sim/intersections/cycles.ts#file` — 4 of 4 lines
- `src/sim/intersections/cycles.ts#indexOf` — 6 of 6 lines
- `src/sim/intersections/cycles.ts#indexes` — 1 of 1 lines
- `src/sim/intersections/cycles.ts#unfile` — 6 of 6 lines
- `src/sim/intersections/spillback.ts#endsAtSignal` — 3 of 4 lines
- `src/sim/intersections/spillback.ts#hasDownstreamStorage` — 38 of 54 lines
- `src/sim/invariants.ts#runAudit` — 103 of 225 lines
- `src/sim/meso/meso.ts#Meso.handoff` — 1 of 1 lines
- `src/sim/meso/meso.ts#Meso.handoffDestination` — 11 of 11 lines
- `src/sim/meso/meso.ts#Meso.handoffLanes` — 6 of 17 lines
- `src/sim/meso/meso.ts#Meso.laneLists` — 1 of 1 lines
- `src/sim/peds/sidewalk.ts#SidewalkEdge` — 3 of 15 lines
- `src/sim/peds/sidewalk.ts#SidewalkGraph.build` — 5 of 6 lines
- `src/sim/peds/sidewalk.ts#SidewalkGraph.crossingsAt` — 10 of 33 lines
- `src/sim/peds/sidewalk.ts#SidewalkGraph.footwaysOf` — 28 of 51 lines
- `src/sim/peds/sidewalk.ts#SidewalkGraph.kerbsAt` — 20 of 39 lines
- `src/sim/peds/sidewalk.ts#SidewalkGraph.update` — 10 of 33 lines
- `src/sim/peds/sidewalk.ts#clearOfJunction` — 13 of 26 lines
- `src/sim/peds/signalPostIndex.ts#SignalPostIndex.placeJunction` — 7 of 10 lines
- `src/sim/peds/signalPostIndex.ts#SignalPostWorld` — 4 of 6 lines
- `src/sim/people/engine.ts#PeopleBridge` — 4 of 12 lines
- `src/sim/pipeline.ts#collectGhosts` — 16 of 25 lines
- `src/sim/pipeline.ts#ensureVehicleRoutes` — 44 of 61 lines
- `src/sim/pipeline.ts#pinReservedRoute` — 28 of 39 lines
- `src/sim/pipeline.ts#rebindVehicles` — 13 of 34 lines
- `src/sim/pipeline.ts#reservedChainValid` — 42 of 58 lines
- `src/sim/pipeline.ts#stallCounters` — 7 of 29 lines
- `src/sim/pipeline.ts#step` — 6 of 100 lines
- `src/sim/pipeline.ts#updateStallCounters` — 5 of 10 lines
- `src/sim/pose.ts#bodyFrame` — 16 of 23 lines
- `src/sim/roads/controlAdvisor.ts#ControlAdvisor.evaluate` — 4 of 11 lines
- `src/sim/roads/controlAdvisor.ts#F` — 1 of 1 lines
- `src/sim/roads/controlAdvisor.ts#QUIET_FLOW` — 1 of 1 lines
- `src/sim/roads/controlAdvisor.ts#warrantedControl` — 7 of 8 lines
- `src/sim/roads/controlAdvisor.ts#warrantedControl.k` — 1 of 1 lines
- `src/sim/roads/flowStats.ts#FlowStats.step` — 10 of 21 lines
- `src/sim/roads/flowStats.ts#KEEP` — 1 of 1 lines
- `src/sim/roads/flowStats.ts#TAU` — 1 of 1 lines
- `src/sim/roads/flowStats.ts#agedTo` — 3 of 4 lines
- `src/sim/roads/greenWave.ts#corridorFrom` — 19 of 40 lines
- `src/sim/roads/greenWave.ts#corridorFrom.otherLeg` — 1 of 1 lines
- `src/sim/roads/greenWave.ts#corridorFrom.walk` — 13 of 16 lines
- `src/sim/roads/greenWave.ts#greenWave` — 27 of 29 lines
- `src/sim/routing/destination.ts#chooseVehicleDestination` — 5 of 5 lines
- `src/sim/routing/destination.ts#reachableExits` — 4 of 4 lines
- `src/sim/routing/destination.ts#routeToDestination` — 37 of 55 lines
- `src/sim/routing/laneReach.ts#BodyReach.buildAll` — 8 of 11 lines
- `src/sim/routing/laneReach.ts#BodyReach.comp` — 1 of 1 lines
- `src/sim/routing/laneReach.ts#BodyReach.countIn` — 6 of 6 lines
- `src/sim/routing/laneReach.ts#BodyReach.edge` — 5 of 5 lines
- `src/sim/routing/laneReach.ts#BodyReach.exitsFrom` — 4 of 4 lines
- `src/sim/routing/laneReach.ts#BodyReach.exitsMemo` — 1 of 1 lines
- `src/sim/routing/laneReach.ts#BodyReach.exitsOf` — 10 of 22 lines
- `src/sim/routing/laneReach.ts#BodyReach.follow` — 72 of 93 lines
- `src/sim/routing/laneReach.ts#BodyReach.follow.isLink` — 1 of 1 lines
- `src/sim/routing/laneReach.ts#BodyReach.into` — 1 of 1 lines
- `src/sim/routing/laneReach.ts#BodyReach.label` — 4 of 6 lines
- `src/sim/routing/laneReach.ts#BodyReach.members` — 1 of 1 lines
- `src/sim/routing/laneReach.ts#BodyReach.next` — 9 of 9 lines
- `src/sim/routing/laneReach.ts#BodyReach.out` — 1 of 1 lines
- `src/sim/routing/laneReach.ts#BodyReach.reaches` — 22 of 23 lines
- `src/sim/routing/laneReach.ts#BodyReach.reachesFrom` — 8 of 8 lines
- `src/sim/routing/laneReach.ts#BodyReach.retag` — 10 of 11 lines
- `src/sim/routing/laneReach.ts#BodyReach.tarjan` — 38 of 39 lines
- `src/sim/routing/laneReach.ts#LaneReaches.exitsFrom` — 3 of 6 lines
- `src/sim/routing/laneReach.ts#LaneReaches.mayReach` — 3 of 6 lines
- `src/sim/routing/laneReach.ts#ReachWorld` — 6 of 7 lines
- `src/sim/routing/laneReach.ts#boundaryExit` — 5 of 5 lines
- `src/sim/routing/lowerBound.ts#FASTEST_LIMIT` — 1 of 1 lines
- `src/sim/routing/lowerBound.ts#LaneLookup` — 3 of 3 lines
- `src/sim/routing/lowerBound.ts#goalEnd` — 4 of 4 lines
- `src/sim/routing/lowerBound.ts#laneLowerBound` — 6 of 6 lines
- `src/sim/routing/routeIndex.ts#RouteIndex.add` — 4 of 4 lines
- `src/sim/routing/routeIndex.ts#RouteIndex.after` — 1 of 1 lines
- `src/sim/routing/routeIndex.ts#RouteIndex.before` — 1 of 1 lines
- `src/sim/routing/routeIndex.ts#RouteIndex.byLane` — 1 of 1 lines
- `src/sim/routing/routeIndex.ts#RouteIndex.drop` — 6 of 6 lines
- `src/sim/routing/routeIndex.ts#RouteIndex.filed` — 1 of 1 lines
- `src/sim/routing/routeIndex.ts#RouteIndex.routing` — 3 of 3 lines
- `src/sim/routing/routeIndex.ts#sameTail` — 3 of 4 lines
- `src/sim/routing/router.ts#TrapIndex.bodies` — 1 of 1 lines
- `src/sim/routing/router.ts#TrapIndex.follow` — 18 of 20 lines
- `src/sim/routing/router.ts#TrapIndex.traps` — 2 of 6 lines
- `src/sim/routing/router.ts#chooseExit` — 11 of 31 lines
- `src/sim/routing/router.ts#extend` — 24 of 47 lines
- `src/sim/routing/router.ts#fittingExits` — 6 of 8 lines
- `src/sim/routing/router.ts#isLegalTransition` — 12 of 12 lines
- `src/sim/routing/router.ts#isSeed` — 2 of 2 lines
- `src/sim/routing/router.ts#peel` — 21 of 21 lines
- `src/sim/routing/router.ts#planFrom` — 61 of 80 lines
- `src/sim/routing/router.ts#reconsiderRoute` — 4 of 16 lines
- `src/sim/routing/router.ts#repairRoute` — 23 of 45 lines
- `src/sim/routing/router.ts#routeCost` — 29 of 40 lines
- `src/sim/routing/router.ts#seedsOf` — 8 of 8 lines
- `src/sim/routing/router.ts#trapLanes` — 2 of 5 lines
- `src/sim/routing/searchScratch.ts#SearchScratch.ids` — 1 of 1 lines
- `src/sim/routing/searchScratch.ts#SearchScratch.slot` — 10 of 10 lines
- `src/sim/routing/searchScratch.ts#SearchScratch.slots` — 1 of 1 lines
- `src/sim/signals/fsm.ts#SignalDeps` — 4 of 28 lines
- `src/sim/signals/fsm.ts#createController` — 16 of 25 lines
- `src/sim/signals/fsm.ts#fixedCycle` — 3 of 3 lines
- `src/sim/signals/fsm.ts#rebuildController` — 29 of 40 lines
- `src/sim/signals/fsm.ts#safePlan` — 15 of 21 lines
- `src/sim/signals/fsm.ts#seekFixed` — 15 of 16 lines
- `src/sim/signals/fsm.ts#stepController` — 37 of 95 lines
- `src/sim/signals/plan.ts#TURN_RANK` — 1 of 1 lines
- `src/sim/signals/plan.ts#annotate` — 30 of 49 lines
- `src/sim/signals/plan.ts#annotate.protectedIn` — 11 of 11 lines
- `src/sim/signals/plan.ts#buildSignalPlan` — 8 of 11 lines
- `src/sim/signals/plan.ts#buildStages` — 68 of 90 lines
- `src/sim/signals/plan.ts#conflictFreeStages` — 37 of 54 lines
- `src/sim/signals/plan.ts#conflictFreeStages.compatible` — 12 of 13 lines
- `src/sim/signals/plan.ts#crossingsCompatibleWith` — 12 of 18 lines
- `src/sim/signals/plan.ts#mustYieldToPeds` — 1 of 1 lines
- `src/sim/signals/plan.ts#pairOpposingGroups` — 29 of 33 lines
- `src/sim/signals/plan.ts#withAmbers` — 8 of 18 lines
- `src/sim/topologyCatchUp.ts#TopologyCatchUp.step` — 15 of 19 lines
- `src/sim/topologyCatchUp.ts#TopologyCatchUp.stepWindow` — 3 of 8 lines
- `src/sim/transit/transit.ts#TransitSim.spawnBus` — 15 of 23 lines
- `src/sim/transit/transit.ts#TransitSim.stopPoint` — 4 of 7 lines
- `src/sim/transit/transit.ts#levelCrossings` — 15 of 21 lines
- `src/sim/vehicleSignals.ts#indicatorSide` — 21 of 35 lines
- `src/sim/vehicleSignals.ts#pathCurvature` — 14 of 18 lines
- `src/sim/vehicles/curvature.ts#curveSpeedCap` — 24 of 43 lines
- `src/sim/vehicles/curvature.ts#peaks` — 1 of 1 lines
- `src/sim/vehicles/curvature.ts#profileOf` — 19 of 20 lines
- `src/sim/vehicles/curvature.ts#profiles` — 1 of 1 lines
- `src/sim/vehicles/curvature.ts#slowestBend` — 7 of 9 lines
- `src/sim/vehicles/curvature.ts#straight` — 4 of 4 lines
- `src/sim/vehicles/cycleLane.ts#cycleShift` — 4 of 4 lines
- `src/sim/vehicles/cycleLane.ts#inside` — 7 of 7 lines
- `src/sim/vehicles/cycleLane.ts#outOfTheWay` — 4 of 5 lines
- `src/sim/vehicles/integrate.ts#advance` — 29 of 37 lines
- `src/sim/vehicles/integrate.ts#advanceClearanceTokens` — 12 of 27 lines
- `src/sim/vehicles/integrate.ts#advanceTo` — 30 of 48 lines
- `src/sim/vehicles/integrate.ts#applyLaneChange` — 18 of 52 lines
- `src/sim/vehicles/integrate.ts#integrateAll` — 61 of 118 lines
- `src/sim/vehicles/kerbStops.ts#KerbStop` — 3 of 27 lines
- `src/sim/vehicles/kerbStops.ts#doorPlaces` — 6 of 10 lines
- `src/sim/vehicles/kerbStops.ts#footwayBeside` — 17 of 20 lines
- `src/sim/vehicles/kerbStops.ts#kerbSide` — 6 of 6 lines
- `src/sim/vehicles/kerbStops.ts#newStop` — 4 of 4 lines
- `src/sim/vehicles/kerbStops.ts#plan` — 35 of 60 lines
- `src/sim/vehicles/kerbStops.ts#planBusStop` — 17 of 33 lines
- `src/sim/vehicles/kerbStops.ts#roomToOpen` — 12 of 20 lines
- `src/sim/vehicles/laneChange.ts#RestingRears.ahead` — 17 of 20 lines
- `src/sim/vehicles/laneChange.ts#RestingRears.build` — 18 of 34 lines
- `src/sim/vehicles/laneChange.ts#RestingRears.lanes` — 1 of 1 lines
- `src/sim/vehicles/laneChange.ts#canLeaveLane` — 12 of 18 lines
- `src/sim/vehicles/laneChange.ts#changeLength` — 4 of 4 lines
- `src/sim/vehicles/laneChange.ts#decideLaneChanges` — 32 of 58 lines
- `src/sim/vehicles/laneChange.ts#discretionary` — 75 of 91 lines
- `src/sim/vehicles/laneChange.ts#followerIn` — 20 of 22 lines
- `src/sim/vehicles/laneChange.ts#gapIsSafe` — 27 of 34 lines
- `src/sim/vehicles/laneChange.ts#leaderIn` — 20 of 22 lines
- `src/sim/vehicles/laneChange.ts#mandatory` — 24 of 28 lines
- `src/sim/vehicles/laneChange.ts#nextOutSegment` — 3 of 4 lines
- `src/sim/vehicles/laneChange.ts#positioningSpeedCap` — 18 of 27 lines
- `src/sim/vehicles/laneChange.ts#pullOutRoom` — 13 of 16 lines
- `src/sim/vehicles/laneChange.ts#restingRearAhead` — 13 of 15 lines
- `src/sim/vehicles/laneChange.ts#roomNeeded` — 3 of 3 lines
- `src/sim/vehicles/laneChange.ts#sideways` — 13 of 15 lines
- `src/sim/vehicles/laneChange.ts#standingIntent` — 3 of 5 lines
- `src/sim/vehicles/laneClosures.ts#CLOSED` — 1 of 1 lines
- `src/sim/vehicles/laneClosures.ts#Closure` — 3 of 8 lines
- `src/sim/vehicles/laneClosures.ts#closedOn` — 3 of 3 lines
- `src/sim/vehicles/laneClosures.ts#laneClear` — 23 of 26 lines
- `src/sim/vehicles/laneClosures.ts#lanesInto` — 12 of 12 lines
- `src/sim/vehicles/leaderIndex.ts#divergeObstacle` — 41 of 48 lines
- `src/sim/vehicles/leaderIndex.ts#findLeader` — 25 of 68 lines
- `src/sim/vehicles/leaderIndex.ts#shadowsAhead` — 12 of 17 lines
- `src/sim/vehicles/leaderIndex.ts#upcomingLanelets` — 8 of 8 lines
- `src/sim/vehicles/obstacles.ts#crossingIdsAt` — 13 of 13 lines
- `src/sim/vehicles/obstacles.ts#longitudinalConstraints` — 54 of 104 lines
- `src/sim/vehicles/obstacles.ts#mergeObstacle` — 27 of 29 lines
- `src/sim/vehicles/obstacles.ts#pedestrianAhead` — 30 of 37 lines
- `src/sim/vehicles/spawn.ts#DestinationPicker` — 1 of 1 lines
- `src/sim/vehicles/spawn.ts#entryLanes` — 23 of 24 lines
- `src/sim/vehicles/spawn.ts#isEntry` — 4 of 4 lines
- `src/sim/vehicles/spawn.ts#roadLength` — 3 of 3 lines
- `src/sim/vehicles/spawn.ts#spawnAt` — 30 of 60 lines
- `src/sim/vehicles/spawn.ts#spawnVehicleAt` — 28 of 46 lines
- `src/sim/vehicles/spawn.ts#stepDespawn` — 15 of 31 lines
- `src/sim/vehicles/state.ts#ConnectorClearance` — 3 of 6 lines
- `src/sim/vehicles/state.ts#Kinematics` — 4 of 11 lines
- `src/sim/vehicles/state.ts#Vehicle` — 14 of 115 lines
- `src/sim/vehicles/state.ts#createVehicle` — 9 of 63 lines
- `src/sim/world.ts#LaneletRuntime` — 3 of 11 lines
- `src/sim/world.ts#NO_RUNTIME` — 3 of 7 lines
- `src/sim/world.ts#PlanInputs` — 4 of 9 lines
- `src/sim/world.ts#SimWorld.addShadow` — 5 of 6 lines
- `src/sim/world.ts#SimWorld.applyAutoControl` — 16 of 18 lines
- `src/sim/world.ts#SimWorld.bodiesIn` — 22 of 24 lines
- `src/sim/world.ts#SimWorld.busNear` — 15 of 16 lines
- `src/sim/world.ts#SimWorld.connector` — 3 of 3 lines
- `src/sim/world.ts#SimWorld.constructor` — 3 of 9 lines
- `src/sim/world.ts#SimWorld.enterLanelet` — 17 of 19 lines
- `src/sim/world.ts#SimWorld.entryDemand` — 1 of 1 lines
- `src/sim/world.ts#SimWorld.exitLanelet` — 7 of 7 lines
- `src/sim/world.ts#SimWorld.fileJunctions` — 8 of 9 lines
- `src/sim/world.ts#SimWorld.forgetSignals` — 3 of 6 lines
- `src/sim/world.ts#SimWorld.ghostCandidates` — 1 of 1 lines
- `src/sim/world.ts#SimWorld.graph` — 1 of 1 lines
- `src/sim/world.ts#SimWorld.inNodeOrder` — 8 of 10 lines
- `src/sim/world.ts#SimWorld.junctionHolders` — 6 of 9 lines
- `src/sim/world.ts#SimWorld.junctionOfLane` — 5 of 5 lines
- `src/sim/world.ts#SimWorld.junctionQuiet` — 5 of 12 lines
- `src/sim/world.ts#SimWorld.laneHead` — 5 of 5 lines
- `src/sim/world.ts#SimWorld.lanelet` — 3 of 3 lines
- `src/sim/world.ts#SimWorld.makeSignalDeps` — 6 of 72 lines
- `src/sim/world.ts#SimWorld.makeSignalDeps.reservedGroups` — 15 of 24 lines
- `src/sim/world.ts#SimWorld.mergeTurn` — 1 of 1 lines
- `src/sim/world.ts#SimWorld.moveWindowSteps` — 5 of 9 lines
- `src/sim/world.ts#SimWorld.nearLanes` — 1 of 1 lines
- `src/sim/world.ts#SimWorld.noteJunctionsReached` — 11 of 20 lines
- `src/sim/world.ts#SimWorld.noteJunctionsReached.note` — 3 of 5 lines
- `src/sim/world.ts#SimWorld.noteSignal` — 9 of 12 lines
- `src/sim/world.ts#SimWorld.occupied` — 1 of 1 lines
- `src/sim/world.ts#SimWorld.planInputsOf` — 9 of 10 lines
- `src/sim/world.ts#SimWorld.prepGraph` — 1 of 1 lines
- `src/sim/world.ts#SimWorld.prepReach` — 4 of 5 lines
- `src/sim/world.ts#SimWorld.prepareVehicleTopology` — 20 of 29 lines
- `src/sim/world.ts#SimWorld.rebindLanes` — 1 of 1 lines
- `src/sim/world.ts#SimWorld.rebuildVehicleTopology` — 36 of 45 lines
- `src/sim/world.ts#SimWorld.rebuildWalkTopology` — 13 of 15 lines
- `src/sim/world.ts#SimWorld.reservationsChecked` — 4 of 4 lines
- `src/sim/world.ts#SimWorld.reset` — 7 of 45 lines
- `src/sim/world.ts#SimWorld.roadLength` — 1 of 1 lines
- `src/sim/world.ts#SimWorld.rt` — 3 of 3 lines
- `src/sim/world.ts#SimWorld.runtime` — 1 of 1 lines
- `src/sim/world.ts#SimWorld.runtimeFor` — 9 of 9 lines
- `src/sim/world.ts#SimWorld.signalDemand` — 29 of 31 lines
- `src/sim/world.ts#SimWorld.signalsToStep` — 8 of 36 lines
- `src/sim/world.ts#SimWorld.someVehicleNear` — 11 of 15 lines
- `src/sim/world.ts#SimWorld.stepControlAdvisor` — 14 of 26 lines
- `src/sim/world.ts#SimWorld.syncControllers` — 34 of 38 lines
- `src/sim/world.ts#SimWorld.topologyLog` — 1 of 1 lines
- `src/sim/world.ts#SimWorld.topologyToken` — 1 of 1 lines
- `src/sim/world.ts#SimWorld.useMicroWindow` — 3 of 5 lines
- `src/sim/world.ts#SimWorld.vehiclesNear` — 8 of 14 lines
- `src/sim/world.ts#settingsKey` — 4 of 4 lines
- `src/ui/inspector.ts#InspectorActions` — 10 of 43 lines
- `src/ui/inspector.ts#InspectorContext` — 3 of 7 lines
- `src/ui/inspector.ts#directionOptions` — 6 of 6 lines
- `src/ui/inspector.ts#junctionHost` — 2 of 12 lines
- `src/ui/inspector.ts#laneOptions` — 8 of 12 lines
- `src/ui/inspector.ts#openInspector` — 5 of 11 lines
- `src/ui/inspector.ts#parkingSelect` — 5 of 5 lines
- `src/ui/inspector.ts#renderCurrent` — 5 of 43 lines
- `src/ui/inspector.ts#renderNode` — 30 of 61 lines
- `src/ui/inspector.ts#renderSegment` — 55 of 134 lines
- `src/ui/inspector.ts#segmentStats` — 16 of 19 lines
- `src/ui/labels.ts#roadTypeDescription` — 7 of 7 lines
- `src/ui/labels.ts#roadTypeName` — 1 of 1 lines
- `src/ui/overlay/congestion.ts#bandOf` — 17 of 17 lines
- `src/ui/overlay/congestion.ts#drawCongestion` — 18 of 24 lines
- `src/ui/overlay/congestion.ts#roadOccupancy` — 8 of 12 lines
- `src/ui/overlay/congestion.ts#sampled` — 1 of 1 lines
- `src/ui/v2/shell.ts#mountShell` — 3 of 1639 lines
- `src/ui/v2/shell.ts#mountShell.renderRoads` — 45 of 116 lines
- `src/ui/v2/shell.ts#mountShell.syncMoney` — 4 of 6 lines
- `src/world/buildings/validate.ts#RoadReach` — 3 of 9 lines
- `src/world/buildings/validate.ts#RoadReachRibbon` — 3 of 5 lines
- `src/world/buildings/validate.ts#SiteContext` — 3 of 9 lines
- `src/world/buildings/validate.ts#touchesRoad` — 15 of 22 lines
- `src/world/cityGen/largeCity.ts#largeCityPlan` — 3 of 3 lines
- `src/world/cityGen/smallCity.ts#plans` — 1 of 1 lines
- `src/world/cityGen/smallCity.ts#smallCityPlan` — 4 of 5 lines
- `src/world/cityGen/squares.ts#mainRoadsNear` — 6 of 27 lines
- `src/world/cityGen/wholeMap.ts#EDGE_INSET` — 1 of 1 lines
- `src/world/cityGen/wholeMap.ts#WholeMapOptions` — 1 of 1 lines
- `src/world/conflictPoints.ts#ConflictIndex.add` — 26 of 40 lines
- `src/world/conflictPoints.ts#ConflictIndex.build` — 3 of 23 lines
- `src/world/conflictPoints.ts#ConflictIndex.buildJunction` — 52 of 64 lines
- `src/world/conflictPoints.ts#ConflictIndex.byConnector` — 1 of 1 lines
- `src/world/conflictPoints.ts#ConflictIndex.diverges` — 1 of 1 lines
- `src/world/conflictPoints.ts#ConflictIndex.divergesOf` — 3 of 3 lines
- `src/world/conflictPoints.ts#ConflictIndex.entering` — 3 of 5 lines
- `src/world/conflictPoints.ts#ConflictIndex.ofJunction` — 1 of 1 lines
- `src/world/conflictPoints.ts#ConflictIndex.prepare` — 37 of 61 lines
- `src/world/conflictPoints.ts#ConflictIndex.queueIntrusions` — 1 of 1 lines
- `src/world/conflictPoints.ts#ConflictIndex.recordIntrusions` — 3 of 3 lines
- `src/world/conflictPoints.ts#ConflictIndex.refs` — 3 of 3 lines
- `src/world/conflictPoints.ts#ConflictIndex.sampleOffPage` — 24 of 38 lines
- `src/world/conflictPoints.ts#ConflictIndex.setWindow` — 4 of 8 lines
- `src/world/conflictPoints.ts#ConflictIndex.takePrepared` — 12 of 12 lines
- `src/world/conflictPoints.ts#ConflictIndex.update` — 11 of 34 lines
- `src/world/conflictPoints.ts#ConflictPoint` — 5 of 14 lines
- `src/world/conflictPoints.ts#Movement` — 5 of 9 lines
- `src/world/conflictPoints.ts#PreparedJunction` — 4 of 6 lines
- `src/world/conflictPoints.ts#Sweep` — 3 of 5 lines
- `src/world/conflictPoints.ts#measureShape` — 3 of 4 lines
- `src/world/conflictPoints.ts#shapeFrom` — 7 of 10 lines
- `src/world/conflictPoints.ts#shapeOf` — 5 of 9 lines
- `src/world/conflictPoints.ts#sweepOf` — 19 of 23 lines
- `src/world/doc.ts#RoadDoc.addGullyDab` — 7 of 9 lines
- `src/world/doc.ts#RoadDoc.addSegment` — 11 of 34 lines
- `src/world/doc.ts#RoadDoc.economy` — 1 of 1 lines
- `src/world/doc.ts#RoadDoc.fromJSON` — 11 of 187 lines
- `src/world/doc.ts#RoadDoc.gullyAuto` — 1 of 1 lines
- `src/world/doc.ts#RoadDoc.gullyDabs` — 1 of 1 lines
- `src/world/doc.ts#RoadDoc.layOver` — 4 of 47 lines
- `src/world/doc.ts#RoadDoc.markingStyle` — 1 of 1 lines
- `src/world/doc.ts#RoadDoc.nodeRect` — 6 of 10 lines
- `src/world/doc.ts#RoadDoc.replaceContents` — 14 of 149 lines
- `src/world/doc.ts#RoadDoc.restoreValue` — 7 of 27 lines
- `src/world/doc.ts#RoadDoc.savedGlobals` — 8 of 18 lines
- `src/world/doc.ts#RoadDoc.segmentRect` — 3 of 7 lines
- `src/world/doc.ts#RoadDoc.setGullyAuto` — 5 of 5 lines
- `src/world/doc.ts#RoadDoc.setMarkingStyle` — 4 of 6 lines
- `src/world/doc.ts#RoadDoc.setMovementBlocked` — 7 of 10 lines
- `src/world/doc.ts#RoadDoc.setNodeApproachRules` — 6 of 10 lines
- `src/world/doc.ts#RoadDoc.setNodeLaneLinks` — 6 of 10 lines
- `src/world/doc.ts#RoadDoc.setNodeSignal` — 6 of 10 lines
- `src/world/doc.ts#RoadDoc.setSegmentDirection` — 5 of 10 lines
- `src/world/doc.ts#RoadDoc.setSegmentSection` — 6 of 11 lines
- `src/world/doc.ts#RoadDoc.takeFork` — 4 of 49 lines
- `src/world/doc.ts#RoadDoc.valueOf` — 6 of 15 lines
- `src/world/doc.ts#RoadDoc.withRoom` — 6 of 12 lines
- `src/world/doc.ts#RoadDoc.worldPart` — 7 of 26 lines
- `src/world/doc.ts#RoadNode` — 5 of 25 lines
- `src/world/doc.ts#RoadSegment` — 4 of 21 lines
- `src/world/doc.ts#SerializedDoc` — 18 of 75 lines
- `src/world/doc.ts#TILE_CAPS` — 4 of 4 lines
- `src/world/doc.ts#copyNode` — 5 of 13 lines
- `src/world/doc.ts#copySegment` — 5 of 8 lines
- `src/world/doc.ts#finishNode` — 8 of 15 lines
- `src/world/doc.ts#fitRoadCurve` — 5 of 7 lines
- `src/world/doc.ts#normaliseLaneCount` — 3 of 6 lines
- `src/world/doc.ts#sameNode` — 7 of 7 lines
- `src/world/doc.ts#sameSegment` — 6 of 6 lines
- `src/world/doc.ts#savedNode` — 5 of 12 lines
- `src/world/doc.ts#savedSegment` — 5 of 16 lines
- `src/world/doc.ts#segmentRecord` — 6 of 19 lines
- `src/world/economy.ts#DEFAULT_ECONOMY` — 1 of 1 lines
- `src/world/economy.ts#E` — 1 of 1 lines
- `src/world/economy.ts#PER_SQUARE_UNIT` — 5 of 5 lines
- `src/world/footwayPath.ts#FootwayNet` — 3 of 4 lines
- `src/world/footwayPath.ts#FootwayRibbon` — 3 of 5 lines
- `src/world/footwayPath.ts#footwayPath` — 7 of 8 lines
- `src/world/growth/zoning.ts#cornerOf.onStreet` — 5 of 6 lines
- `src/world/landscape.ts#FootwayHit` — 3 of 11 lines
- `src/world/landscape.ts#accessMemo` — 1 of 1 lines
- `src/world/landscape.ts#crossingAccessesAt` — 10 of 12 lines
- `src/world/landscape.ts#footwayAt` — 28 of 31 lines
- `src/world/landscape.ts#ribbonsNear` — 3 of 3 lines
- `src/world/lots.ts#LandPaving.tile` — 20 of 28 lines
- `src/world/lots.ts#LotPlanner.constructor` — 2 of 3 lines
- `src/world/lots.ts#LotPlanner.planAll` — 8 of 29 lines
- `src/world/lots.ts#LotPlanner.planChanged` — 14 of 61 lines
- `src/world/lots.ts#LotPlanner.run` — 9 of 42 lines
- `src/world/lots.ts#LotPlanner.streetPlan` — 49 of 76 lines
- `src/world/lots.ts#LotPlanner.update` — 6 of 9 lines
- `src/world/lots.ts#cutLotsByRoads` — 15 of 30 lines
- `src/world/lots.ts#footwayEdgesNear` — 8 of 10 lines
- `src/world/lots.ts#forgetLotPlans` — 3 of 3 lines
- `src/world/lots.ts#landLot` — 7 of 8 lines
- `src/world/lots.ts#lotSnapper` — 14 of 47 lines
- `src/world/lots.ts#onLand` — 11 of 17 lines
- `src/world/lots.ts#pavingNear` — 17 of 17 lines
- `src/world/lots.ts#planLots` — 5 of 5 lines
- `src/world/lots.ts#plannedStreet` — 2 of 2 lines
- `src/world/lots.ts#planners` — 1 of 1 lines
- `src/world/nearestRibbon.ts#nearestRibbon` — 11 of 11 lines
- `src/world/parkingLayout.ts#endClear` — 10 of 10 lines
- `src/world/parkingLayout.ts#parkingDigest` — 9 of 9 lines
- `src/world/parkingLayout.ts#parkingLayout` — 3 of 6 lines
- `src/world/parkingLayout.ts#parkingOf` — 74 of 83 lines
- `src/world/parkingLayout.ts#parksAlong` — 4 of 4 lines
- `src/world/pedestrianAccess.ts#carriesPedestrians` — 3 of 3 lines
- `src/world/poleLines.ts#PoleLineState.letGoStale` — 6 of 6 lines
- `src/world/poleLines.ts#PoleLineState.linesIn` — 7 of 9 lines
- `src/world/poleLines.ts#insideLevels` — 13 of 21 lines
- `src/world/poleLines.ts#junctionRings` — 12 of 12 lines
- `src/world/poleLines.ts#kerbward` — 29 of 36 lines
- `src/world/poleLines.ts#kerbwardOfUnion` — 27 of 31 lines
- `src/world/poleLines.ts#localRings` — 11 of 13 lines
- `src/world/poleLines.ts#markPlates` — 11 of 89 lines
- `src/world/poleLines.ts#onFootway` — 4 of 4 lines
- `src/world/poleLines.ts#onPaving` — 4 of 4 lines
- `src/world/poleLines.ts#poleLinesAt` — 3 of 3 lines
- `src/world/poleLines.ts#poleLinesIn` — 3 of 3 lines
- `src/world/poleLines.ts#poleLinesOf` — 25 of 28 lines
- `src/world/poleLines.ts#polesInclude` — 7 of 7 lines
- `src/world/poleLines.ts#regionStamp` — 3 of 4 lines
- `src/world/poleLines.ts#ribbonRings` — 12 of 12 lines
- `src/world/poleLines.ts#stateOf` — 1 of 1 lines
- `src/world/section.ts#sectionOf` — 9 of 11 lines
- `src/world/section.ts#sideZones` — 14 of 20 lines
- `src/world/section.ts#zonesOn` — 1 of 1 lines
- `src/world/signalPosts.ts#signalPostPlace` — 17 of 21 lines
- `src/world/signalPosts.ts#signalPosts` — 17 of 24 lines
- `src/world/structures.ts#ROAD_STRUCTURES` — 7 of 7 lines
- `src/world/structures.ts#TUNNEL_GRADE` — 1 of 1 lines
- `src/world/structures.ts#roadStructure` — 2 of 2 lines
- `src/world/turnPaths.ts#chooseTurnFor` — 8 of 50 lines
- `src/world/utilities.ts#DEFAULT_POLE_SPACING` — 1 of 1 lines
- `src/world/utilities.ts#MAX_POLE_SPACING` — 1 of 1 lines
- `src/world/utilities.ts#MIN_POLE_SPACING` — 1 of 1 lines
- `src/world/walkingLines.ts#CornerAsk` — 3 of 4 lines
- `src/world/walkingLines.ts#LocalLines.at` — 4 of 12 lines
- `src/world/walkingLines.ts#LocalLines.paving` — 8 of 9 lines
- `src/world/walkingLines.ts#LocalLines.polygons` — 3 of 3 lines
- `src/world/walkingLines.ts#RingsOf` — 1 of 1 lines
- `src/world/walkways.ts#Walkways.apply` — 9 of 34 lines
- `src/world/walkways.ts#Walkways.buildNode` — 58 of 114 lines
- `src/world/walkways.ts#Walkways.plan` — 37 of 41 lines
- `src/world/walkways.ts#Walkways.update` — 4 of 4 lines
- `src/world/walkways.ts#Walkways.updateSteps` — 10 of 11 lines
- `src/world/walkways.ts#cornersOf` — 30 of 33 lines
- `src/world/walkways.ts#footwayShapes` — 20 of 24 lines
- `src/world/walkways.ts#ringsIn` — 9 of 9 lines
- `src/world/zoneGrid.ts#markPaved` — 4 of 5 lines
- `src/world/zoneGrid.ts#markRoads` — 57 of 58 lines
- `src/world/zoneGrid.ts#pavedTester` — 2 of 17 lines
- `src/world/zoneGrid.ts#pavedTester.onRoad` — 10 of 11 lines

## Resources

- feature entry `paint` — src/main.ts:3490: `paint: '#f2d27a'`
- feature entry `fog` — src/main.ts:3491: `fog: '#e8eef4'`
- feature entry `cloud` — src/main.ts:3492: `cloud: '#ffffff'`
- feature entry `elements` — src/main.ts:3493: `elements: '#b8e07a'`
- feature entry `gully` — src/main.ts:3494: `gully: '#c98a5a'`
- feature entry `trees` — src/main.ts:3495: `trees: '#5fbf5a'`
- feature entry `weather` — src/main.ts:3496: `weather: '#cfe3ff'`
- feature entry `raise` — src/main.ts:3497: `raise: SELECTION`
- feature entry `lower` — src/main.ts:3498: `lower: '#ffc864'`
- feature entry `flatten` — src/main.ts:3499: `flatten: '#cfd8d4'`
- feature entry `river` — src/main.ts:3500: `river: '#73cfe7'`
- feature entry `mesa` — src/main.ts:3501: `mesa: '#e29a5c'`
- feature entry `canyon` — src/main.ts:3502: `canyon: '#e29a5c'`
- feature entry `escarpment` — src/main.ts:3503: `escarpment: '#9aa0a8'`
- feature entry `sugarloaf` — src/main.ts:3504: `sugarloaf: '#c9c4bb'`
- feature entry `paint` — src/main.ts:3508: `paint: 'rgba(242,210,122,0.10)'`
- feature entry `fog` — src/main.ts:3509: `fog: 'rgba(232,238,244,0.12)'`
- feature entry `cloud` — src/main.ts:3510: `cloud: 'rgba(255,255,255,0)'`
- feature entry `elements` — src/main.ts:3511: `elements: 'rgba(184,224,122,0.10)'`
- feature entry `gully` — src/main.ts:3512: `gully: 'rgba(201,138,90,0.10)'`
- feature entry `trees` — src/main.ts:3513: `trees: 'rgba(95,191,90,0.10)'`
- feature entry `weather` — src/main.ts:3514: `weather: 'rgba(207,227,255,0)'`
- feature entry `raise` — src/main.ts:3515: `raise: 'rgba(101,229,195,0.08)'`
- feature entry `lower` — src/main.ts:3516: `lower: 'rgba(255,200,100,0.08)'`
- feature entry `flatten` — src/main.ts:3517: `flatten: 'rgba(207,216,212,0.08)'`
- feature entry `river` — src/main.ts:3518: `river: 'rgba(70,160,190,0.12)'`
- feature entry `mesa` — src/main.ts:3519: `mesa: 'rgba(226,154,92,0.10)'`
- feature entry `canyon` — src/main.ts:3520: `canyon: 'rgba(226,154,92,0.10)'`
- feature entry `escarpment` — src/main.ts:3521: `escarpment: 'rgba(154,160,168,0.10)'`
- feature entry `sugarloaf` — src/main.ts:3522: `sugarloaf: 'rgba(201,196,187,0.10)'`
- feature entry `auto` — src/main.ts:3527: `auto: 'A'`
- feature entry `signal` — src/main.ts:3528: `signal: 'S'`
- feature entry `priority` — src/main.ts:3529: `priority: 'P'`
- feature entry `stop` — src/main.ts:3530: `stop: '\u25A0'`
- feature entry `yield` — src/main.ts:3531: `yield: '\u25BC'`
- feature entry `none` — src/main.ts:3532: `none: '\u2013'`
- feature entry `mini` — src/main.ts:3533: `mini: '○'`
- feature entry `auto` — src/main.ts:3537: `auto: '#8fb3a6'`
- feature entry `signal` — src/main.ts:3538: `signal: '#ffd24a'`
- feature entry `priority` — src/main.ts:3539: `priority: '#7ec8ff'`
- feature entry `stop` — src/main.ts:3540: `stop: '#ff7a6a'`
- feature entry `yield` — src/main.ts:3541: `yield: '#ffb057'`
- feature entry `none` — src/main.ts:3542: `none: '#9aa3a0'`
- feature entry `mini` — src/main.ts:3543: `mini: '#c9b6ff'`
- shared entry `signalGreen` — src/sim/intersections/admission.ts:50: `signalGreen: 3`
- shared entry `priority` — src/sim/intersections/admission.ts:51: `priority: 2`
- shared entry `stop` — src/sim/intersections/admission.ts:52: `stop: 1`
- shared entry `yield` — src/sim/intersections/admission.ts:53: `yield: 0`
- shared entry `none` — src/sim/intersections/admission.ts:54: `none: -1`
- feature entry `through` — src/sim/params.ts:62: `through: 4.5`
- feature entry `right` — src/sim/params.ts:63: `right: 4.0`
- feature entry `left` — src/sim/params.ts:64: `left: 5.5`
- feature entry `uturn` — src/sim/params.ts:65: `uturn: 6.5`
- feature entry `hint.roundabout.size` — src/ui/i18n/en.ts:62: `'hint.roundabout.size': 'Choose a roundabout radius between 32 and 128 m.'`
- feature entry `hint.roundabout.bounds` — src/ui/i18n/en.ts:63: `'hint.roundabout.bounds': 'Keep the whole roundabout inside the map.'`
- feature entry `hint.roundabout.occupied` — src/ui/i18n/en.ts:64: `'hint.roundabout.occupied': 'Choose open ground with room for the four entrances.'`
- feature entry `hint.roundabout.geometry` — src/ui/i18n/en.ts:65: `'hint.roundabout.geometry': 'The roundabout does not fit here.'`
- feature entry `road.section.title` — src/ui/i18n/en.ts:66: `'road.section.title': 'Cross-section · experimental'`
- feature entry `road.section.preview` — src/ui/i18n/en.ts:67: `'road.section.preview': 'Road cross-section preview'`
- feature entry `road.section.walk` — src/ui/i18n/en.ts:68: `'road.section.walk': 'Walk'`
- feature entry `road.section.laneWidth` — src/ui/i18n/en.ts:69: `'road.section.laneWidth': 'Lane width'`
- feature entry `road.section.sidewalk` — src/ui/i18n/en.ts:70: `'road.section.sidewalk': 'Sidewalk on each side'`
- feature entry `road.section.median` — src/ui/i18n/en.ts:71: `'road.section.median': 'Central reservation'`
- feature entry `road.section.speedKmh` — src/ui/i18n/en.ts:72: `'road.section.speedKmh': 'Speed limit'`
- feature entry `road.section.priority` — src/ui/i18n/en.ts:73: `'road.section.priority': 'Priority (higher goes first)'`
- feature entry `road.section.reset` — src/ui/i18n/en.ts:74: `'road.section.reset': 'Restore class dimensions'`
- feature entry `road.section.turns` — src/ui/i18n/en.ts:75: `'road.section.turns': 'Lane turn arrows'`
- feature entry `road.section.turnsForward` — src/ui/i18n/en.ts:76: `'road.section.turnsForward': 'A → B · lane {count} from centre'`
- feature entry `road.section.turnsBackward` — src/ui/i18n/en.ts:77: `'road.section.turnsBackward': 'B → A · lane {count} from centre'`
- feature entry `road.turn.all` — src/ui/i18n/en.ts:78: `'road.turn.all': 'All directions'`
- feature entry `road.turn.left` — src/ui/i18n/en.ts:79: `'road.turn.left': '← Left only'`
- feature entry `road.turn.through` — src/ui/i18n/en.ts:80: `'road.turn.through': '↑ Straight only'`
- feature entry `road.turn.right` — src/ui/i18n/en.ts:81: `'road.turn.right': '→ Right only'`
- feature entry `road.turn.leftThrough` — src/ui/i18n/en.ts:82: `'road.turn.leftThrough': '← ↑ Left or straight'`
- feature entry `road.turn.throughRight` — src/ui/i18n/en.ts:83: `'road.turn.throughRight': '↑ → Straight or right'`
- feature entry `screenDir.up` — src/ui/i18n/en.ts:108: `'screenDir.up': 'Road above'`
- feature entry `screenDir.upRight` — src/ui/i18n/en.ts:109: `'screenDir.upRight': 'Road above right'`
- feature entry `screenDir.right` — src/ui/i18n/en.ts:110: `'screenDir.right': 'Road on the right'`
- feature entry `screenDir.downRight` — src/ui/i18n/en.ts:111: `'screenDir.downRight': 'Road below right'`
- feature entry `screenDir.down` — src/ui/i18n/en.ts:112: `'screenDir.down': 'Road below'`
- feature entry `screenDir.downLeft` — src/ui/i18n/en.ts:113: `'screenDir.downLeft': 'Road below left'`
- feature entry `screenDir.left` — src/ui/i18n/en.ts:114: `'screenDir.left': 'Road on the left'`
- feature entry `screenDir.upLeft` — src/ui/i18n/en.ts:115: `'screenDir.upLeft': 'Road above left'`
- feature entry `compass.n` — src/ui/i18n/en.ts:116: `'compass.n': 'north'`
- feature entry `compass.ne` — src/ui/i18n/en.ts:117: `'compass.ne': 'north-east'`
- feature entry `compass.e` — src/ui/i18n/en.ts:118: `'compass.e': 'east'`
- feature entry `compass.se` — src/ui/i18n/en.ts:119: `'compass.se': 'south-east'`
- feature entry `compass.s` — src/ui/i18n/en.ts:120: `'compass.s': 'south'`
- feature entry `compass.sw` — src/ui/i18n/en.ts:121: `'compass.sw': 'south-west'`
- feature entry `compass.w` — src/ui/i18n/en.ts:122: `'compass.w': 'west'`
- feature entry `compass.nw` — src/ui/i18n/en.ts:123: `'compass.nw': 'north-west'`
- feature entry `roadFurniture.complete` — src/ui/i18n/en.ts:295: `'roadFurniture.complete': 'Complete'`
- feature entry `roadFurniture.basic` — src/ui/i18n/en.ts:296: `'roadFurniture.basic': 'Basic'`
- feature entry `roadFurniture.none` — src/ui/i18n/en.ts:297: `'roadFurniture.none': 'None'`
- feature entry `hint.streetscape.rule.stop` — src/ui/i18n/en.ts:298: `'hint.streetscape.rule.stop': 'Stop sign: this road now stops at the junction'`
- feature entry `hint.streetscape.rule.yield` — src/ui/i18n/en.ts:299: `'hint.streetscape.rule.yield': 'Give-way sign: this road now gives way at the junction'`
- feature entry `profile.drawWith` — src/ui/i18n/en.ts:300: `'profile.drawWith': 'Draw with this profile'`
- feature entry `profile.drawWithTip` — src/ui/i18n/en.ts:301: `'profile.drawWithTip': 'Eyedropper: the road tool takes this road as it is, to draw new roads like it'`
- feature entry `profile.copied` — src/ui/i18n/en.ts:302: `'profile.copied': 'Copied road'`
- feature entry `profile.toStreet` — src/ui/i18n/en.ts:303: `'profile.toStreet': 'Whole street ({count})'`
- feature entry `profile.toStreetTip` — src/ui/i18n/en.ts:304: `'profile.toStreetTip': 'This profile on every stretch of the street it runs in, through its junctions'`
- feature entry `hint.profile.toStreet` — src/ui/i18n/en.ts:305: `'hint.profile.toStreet': 'Profile copied to the whole street'`
- feature entry `hint.profile.picked` — src/ui/i18n/en.ts:306: `'hint.profile.picked': 'Drawing with the picked profile'`
- feature entry `inspector.numbers` — src/ui/i18n/en.ts:307: `'inspector.numbers': 'Numbers {from} to {to} · even on the right going up'`
- feature entry `hint.streetscape.row` — src/ui/i18n/en.ts:334: `'hint.streetscape.row': 'Row laid along the footway'`
- feature entry `hint.streetscape.removed` — src/ui/i18n/en.ts:335: `'hint.streetscape.removed': 'Item removed'`
- feature entry `hint.streetscape.offFootway` — src/ui/i18n/en.ts:336: `'hint.streetscape.offFootway': 'Footways only'`
- feature entry `hint.streetscape.narrow` — src/ui/i18n/en.ts:337: `'hint.streetscape.narrow': 'The footway is too narrow for that'`
- feature entry `hint.streetscape.occupied` — src/ui/i18n/en.ts:338: `'hint.streetscape.occupied': 'Something is already here'`
- feature entry `hint.streetscape.crossing` — src/ui/i18n/en.ts:339: `'hint.streetscape.crossing': 'Not in a crossing landing'`
- feature entry `hint.blocks.built` — src/ui/i18n/en.ts:401: `'hint.blocks.built': 'Blocks built'`
- feature entry `palette.width` — src/ui/i18n/en.ts:402: `'palette.width': 'Total width'`
- feature entry `palette.width.auto` — src/ui/i18n/en.ts:403: `'palette.width.auto': 'Default'`
- shared entry `palette.lanes.count.one` — src/ui/i18n/en.ts:404: `'palette.lanes.count.one': '{count} lane'`
- shared entry `palette.lanes.count.other` — src/ui/i18n/en.ts:405: `'palette.lanes.count.other': '{count} lanes'`
- feature entry `road.way.two` — src/ui/i18n/en.ts:431: `'road.way.two': ' / two-way'`
- feature entry `road.way.one` — src/ui/i18n/en.ts:432: `'road.way.one': ' / one-way'`
- feature entry `terrain.level` — src/ui/i18n/en.ts:580: `'terrain.level': 'Level to'`
- feature entry `inspector.road` — src/ui/i18n/en.ts:586: `'inspector.road': 'ROAD'`
- feature entry `inspector.length` — src/ui/i18n/en.ts:589: `'inspector.length': 'LENGTH'`
- feature entry `inspector.speed` — src/ui/i18n/en.ts:590: `'inspector.speed': 'SPEED'`
- feature entry `inspector.lanes` — src/ui/i18n/en.ts:591: `'inspector.lanes': 'LANES'`
- feature entry `inspector.direction` — src/ui/i18n/en.ts:592: `'inspector.direction': 'Traffic direction'`
- feature entry `inspector.laneCount` — src/ui/i18n/en.ts:593: `'inspector.laneCount': 'Travel lanes'`
- feature entry `inspector.curvePosition` — src/ui/i18n/en.ts:594: `'inspector.curvePosition': 'Curve position'`
- feature entry `inspector.heightStart` — src/ui/i18n/en.ts:595: `'inspector.heightStart': 'Start height (m)'`
- feature entry `inspector.heightEnd` — src/ui/i18n/en.ts:596: `'inspector.heightEnd': 'End height (m)'`
- feature entry `inspector.heightNode` — src/ui/i18n/en.ts:601: `'inspector.heightNode': 'Road height (m)'`
- shared entry `inspector.vehicles` — src/ui/i18n/en.ts:604: `'inspector.vehicles': 'VEHICLES'`
- shared entry `inspector.capacity` — src/ui/i18n/en.ts:605: `'inspector.capacity': 'CAPACITY'`
- shared entry `inspector.capacityValue.one` — src/ui/i18n/en.ts:606: `'inspector.capacityValue.one': '{count} vehicle'`
- shared entry `inspector.capacityValue.other` — src/ui/i18n/en.ts:607: `'inspector.capacityValue.other': '{count} vehicles'`
- feature entry `inspector.lanesOneWay` — src/ui/i18n/en.ts:610: `'inspector.lanesOneWay': '{count} one-way'`
- feature entry `inspector.lanesTwoWay` — src/ui/i18n/en.ts:611: `'inspector.lanesTwoWay': '{count} ({perSide} each way)'`
- feature entry `inspector.upgrade` — src/ui/i18n/en.ts:613: `'inspector.upgrade': 'Upgrade class'`
- feature entry `inspector.duplicate` — src/ui/i18n/en.ts:614: `'inspector.duplicate': 'Duplicate road'`
- feature entry `inspector.impossible` — src/ui/i18n/en.ts:615: `'inspector.impossible': 'CANNOT BE BUILT'`
- feature entry `inspector.impossibleBody` — src/ui/i18n/en.ts:616: `'inspector.impossibleBody': 'Below 25° the junction cannot close. It is drawn in a degraded form so that ' + 'no pavement is painted over the grass.'`
- feature entry `inspector.roadClass` — src/ui/i18n/en.ts:620: `'inspector.roadClass': 'Road class'`
- feature entry `inspector.curvature` — src/ui/i18n/en.ts:621: `'inspector.curvature': 'Curvature'`
- shared entry `inspector.meanSpeed` — src/ui/i18n/en.ts:622: `'inspector.meanSpeed': 'MEAN SPEED'`
- shared entry `inspector.queue` — src/ui/i18n/en.ts:623: `'inspector.queue': 'QUEUE'`
- shared entry `inspector.volume` — src/ui/i18n/en.ts:624: `'inspector.volume': 'VOLUME'`
- feature entry `inspector.mouths` — src/ui/i18n/en.ts:629: `'inspector.mouths': 'Setback per leg'`
- feature entry `inspector.reverse` — src/ui/i18n/en.ts:630: `'inspector.reverse': 'Reverse one-way'`
- feature entry `inspector.duplicateHint` — src/ui/i18n/en.ts:631: `'inspector.duplicateHint': 'Duplicate road (Ctrl/Cmd+D)'`
- feature entry `inspector.splitMiddle` — src/ui/i18n/en.ts:632: `'inspector.splitMiddle': 'Split at the middle'`
- feature entry `inspector.addZebra` — src/ui/i18n/en.ts:633: `'inspector.addZebra': 'Zebra crossing'`
- feature entry `inspector.addSignalCrossing` — src/ui/i18n/en.ts:634: `'inspector.addSignalCrossing': 'Signal crossing'`
- feature entry `inspector.addUturnToEnd` — src/ui/i18n/en.ts:635: `'inspector.addUturnToEnd': 'U-turn for traffic heading to the end'`
- feature entry `inspector.addUturnToStart` — src/ui/i18n/en.ts:636: `'inspector.addUturnToStart': 'U-turn for traffic heading to the start'`
- feature entry `hint.crossing.tooShort` — src/ui/i18n/en.ts:640: `'hint.crossing.tooShort': 'A crossing needs room for a queue of cars on both sides.'`
- feature entry `hint.crossing.unsupported` — src/ui/i18n/en.ts:641: `'hint.crossing.unsupported': 'This road carries no pedestrians.'`
- feature entry `hint.crossing.geometry` — src/ui/i18n/en.ts:642: `'hint.crossing.geometry': 'A crossing cannot be placed here.'`
- feature entry `hint.uturn.placed` — src/ui/i18n/en.ts:643: `'hint.uturn.placed': 'U-turn opened in the median.'`
- feature entry `hint.uturn.noMedian` — src/ui/i18n/en.ts:644: `'hint.uturn.noMedian': 'A U-turn needs a two-way road with a central median.'`
- feature entry `hint.uturn.tooShort` — src/ui/i18n/en.ts:645: `'hint.uturn.tooShort': 'A U-turn needs room for a queue of cars on both sides.'`
- feature entry `hint.uturn.narrow` — src/ui/i18n/en.ts:646: `'hint.uturn.narrow': 'Median too narrow: a car turns round in 12.8 m from lane to lane. Widen the median in the road profile.'`
- feature entry `hint.uturn.geometry` — src/ui/i18n/en.ts:647: `'hint.uturn.geometry': 'A U-turn cannot be opened here.'`
- feature entry `inspector.addHeightPoint` — src/ui/i18n/en.ts:648: `'inspector.addHeightPoint': 'Add height point here'`
- feature entry `inspector.demolish` — src/ui/i18n/en.ts:649: `'inspector.demolish': 'Demolish'`
- feature entry `inspector.impossibleGap` — src/ui/i18n/en.ts:650: `'inspector.impossibleGap': '{angle}° between two roads'`
- feature entry `inspector.joinAligned` — src/ui/i18n/en.ts:651: `'inspector.joinAligned': 'Join the aligned roads'`
- feature entry `inspector.laneOption.one` — src/ui/i18n/en.ts:652: `'inspector.laneOption.one': '{count} lane'`
- feature entry `inspector.laneOption.other` — src/ui/i18n/en.ts:653: `'inspector.laneOption.other': '{count} lanes'`
- feature entry `turn.left` — src/ui/i18n/en.ts:660: `'turn.left': 'left'`
- feature entry `turn.right` — src/ui/i18n/en.ts:661: `'turn.right': 'right'`
- feature entry `turn.through` — src/ui/i18n/en.ts:662: `'turn.through': 'through'`
- feature entry `turn.uturn` — src/ui/i18n/en.ts:663: `'turn.uturn': 'u-turn'`
- feature entry `direction.both` — src/ui/i18n/en.ts:665: `'direction.both': 'Two-way'`
- feature entry `direction.aToB` — src/ui/i18n/en.ts:666: `'direction.aToB': 'One-way: A → B'`
- feature entry `direction.bToA` — src/ui/i18n/en.ts:667: `'direction.bToA': 'One-way: B → A'`
- shared entry `junction.lower` — src/ui/i18n/en.ts:676: `'junction.lower': 'Lower 0.5 m'`
- shared entry `junction.raise` — src/ui/i18n/en.ts:677: `'junction.raise': 'Raise 0.5 m'`
- feature entry `junction.control` — src/ui/i18n/en.ts:678: `'junction.control': 'Junction control'`
- feature entry `junction.control.auto` — src/ui/i18n/en.ts:679: `'junction.control.auto': 'Automatic'`
- feature entry `junction.control.signal` — src/ui/i18n/en.ts:680: `'junction.control.signal': 'Signal'`
- feature entry `junction.control.priority` — src/ui/i18n/en.ts:681: `'junction.control.priority': 'Signs'`
- feature entry `junction.control.stop` — src/ui/i18n/en.ts:682: `'junction.control.stop': 'All-way stop'`
- feature entry `junction.control.mini` — src/ui/i18n/en.ts:683: `'junction.control.mini': 'Mini-roundabout'`
- feature entry `junction.control.none` — src/ui/i18n/en.ts:684: `'junction.control.none': 'Unsigned'`
- feature entry `junction.control.yield` — src/ui/i18n/en.ts:685: `'junction.control.yield': 'All give way'`
- feature entry `junction.autoChoice` — src/ui/i18n/en.ts:686: `'junction.autoChoice': 'Chosen by the flow: {choice}. It changes by itself with the trend, never for a peak.'`
- feature entry `junction.auto.signal` — src/ui/i18n/en.ts:687: `'junction.auto.signal': 'signal'`
- feature entry `junction.auto.stop` — src/ui/i18n/en.ts:688: `'junction.auto.stop': 'all-way stop'`
- feature entry `junction.auto.priority` — src/ui/i18n/en.ts:689: `'junction.auto.priority': 'main road has priority'`
- feature entry `junction.locked` — src/ui/i18n/en.ts:690: `'junction.locked': 'Locked on your choice.'`
- feature entry `junction.unlock` — src/ui/i18n/en.ts:691: `'junction.unlock': 'Unlock (automatic)'`
- feature entry `junction.mainRoad` — src/ui/i18n/en.ts:692: `'junction.mainRoad': 'main road'`
- feature entry `junction.perHour` — src/ui/i18n/en.ts:693: `'junction.perHour': '{n} veh/h'`
- feature entry `junction.legs` — src/ui/i18n/en.ts:694: `'junction.legs': 'Each road’s rule'`
- feature entry `junction.rule.priority` — src/ui/i18n/en.ts:695: `'junction.rule.priority': 'Priority'`
- feature entry `junction.rule.yield` — src/ui/i18n/en.ts:696: `'junction.rule.yield': 'Give way'`
- feature entry `junction.rule.stop` — src/ui/i18n/en.ts:697: `'junction.rule.stop': 'Stop'`
- feature entry `junction.rulesReset` — src/ui/i18n/en.ts:698: `'junction.rulesReset': 'Back to the automatic rules'`
- feature entry `junction.signal` — src/ui/i18n/en.ts:699: `'junction.signal': 'Signal'`
- feature entry `junction.signalMode` — src/ui/i18n/en.ts:700: `'junction.signalMode': 'Timing'`
- feature entry `junction.mode.adaptive` — src/ui/i18n/en.ts:701: `'junction.mode.adaptive': 'Adaptive'`
- feature entry `junction.mode.fixed` — src/ui/i18n/en.ts:702: `'junction.mode.fixed': 'Fixed time'`
- feature entry `junction.stage` — src/ui/i18n/en.ts:703: `'junction.stage': 'Stage {n}'`
- feature entry `junction.shorter` — src/ui/i18n/en.ts:704: `'junction.shorter': 'Shorter green'`
- feature entry `junction.longer` — src/ui/i18n/en.ts:705: `'junction.longer': 'Longer green'`
- feature entry `junction.offset` — src/ui/i18n/en.ts:706: `'junction.offset': 'Offset'`
- feature entry `junction.earlier` — src/ui/i18n/en.ts:707: `'junction.earlier': 'Earlier'`
- feature entry `junction.later` — src/ui/i18n/en.ts:708: `'junction.later': 'Later'`
- feature entry `junction.greenWave` — src/ui/i18n/en.ts:709: `'junction.greenWave': 'Green wave along the main road'`
- feature entry `junction.greenWaveNone` — src/ui/i18n/en.ts:710: `'junction.greenWaveNone': 'The main road has no other signal to coordinate.'`
- feature entry `junction.busPriority` — src/ui/i18n/en.ts:711: `'junction.busPriority': 'Bus priority'`
- feature entry `junction.movements` — src/ui/i18n/en.ts:712: `'junction.movements': 'Movements allowed'`
- feature entry `junction.from` — src/ui/i18n/en.ts:713: `'junction.from': '{leg}'`
- feature entry `junction.moveBlocked` — src/ui/i18n/en.ts:714: `'junction.moveBlocked': 'Banned: click to allow'`
- feature entry `junction.moveAllowed` — src/ui/i18n/en.ts:715: `'junction.moveAllowed': 'Allowed: click to ban'`
- feature entry `hint.road.gradeLimited` — src/ui/i18n/en.ts:720: `'hint.road.gradeLimited': 'The road reached its safe grade; continue drawing to reach the selected height.'`
- feature entry `hint.road.invalid` — src/ui/i18n/en.ts:721: `'hint.road.invalid': 'This connection needs more distance or a different endpoint.'`
- feature entry `hint.rule.sharp` — src/ui/i18n/en.ts:722: `'hint.rule.sharp': 'Refused: the roads would meet at too sharp an angle (under 25°).'`
- feature entry `hint.rule.squeezed` — src/ui/i18n/en.ts:723: `'hint.rule.squeezed': 'Refused: stretch too short for the lanes to separate at the junction (junctions too close or angle too narrow).'`
- feature entry `hint.rule.overlap` — src/ui/i18n/en.ts:724: `'hint.rule.overlap': 'Refused: the road would lie on another one. Move it away, cross it or pass over it ({heightUp}/{heightDown}).'`
- feature entry `hint.rule.steep` — src/ui/i18n/en.ts:725: `'hint.rule.steep': 'Refused: ramp too steep between the junctions. Make the road longer or change its height.'`
- feature entry `rule.short.sharp` — src/ui/i18n/en.ts:726: `'rule.short.sharp': 'angle too sharp'`
- feature entry `rule.short.squeezed` — src/ui/i18n/en.ts:727: `'rule.short.squeezed': 'stretch too short'`
- feature entry `rule.short.overlap` — src/ui/i18n/en.ts:728: `'rule.short.overlap': 'on another road'`
- feature entry `rule.short.steep` — src/ui/i18n/en.ts:729: `'rule.short.steep': 'too steep'`
- feature entry `rule.short.clearance` — src/ui/i18n/en.ts:730: `'rule.short.clearance': 'no clearance'`
- feature entry `rule.short.funds` — src/ui/i18n/en.ts:731: `'rule.short.funds': 'not enough money'`
- feature entry `buildMode.ground` — src/ui/i18n/en.ts:732: `'buildMode.ground': 'at grade'`
- feature entry `buildMode.embankment` — src/ui/i18n/en.ts:733: `'buildMode.embankment': 'embankment'`
- feature entry `buildMode.cutting` — src/ui/i18n/en.ts:734: `'buildMode.cutting': 'cutting'`
- feature entry `buildMode.bridge` — src/ui/i18n/en.ts:735: `'buildMode.bridge': 'bridge'`
- feature entry `buildMode.tunnel` — src/ui/i18n/en.ts:736: `'buildMode.tunnel': 'tunnel'`
- feature entry `buildMode.cuttingWalls` — src/ui/i18n/en.ts:737: `'buildMode.cuttingWalls': 'walled cutting'`
- feature entry `cutSides.batter` — src/ui/i18n/en.ts:739: `'cutSides.batter': 'Slope'`
- feature entry `cutSides.walls` — src/ui/i18n/en.ts:740: `'cutSides.walls': 'Walls'`
- feature entry `roadKeys.row` — src/ui/i18n/en.ts:741: `'roadKeys.row': 'Keys'`
- feature entry `roadKeys.title` — src/ui/i18n/en.ts:742: `'roadKeys.title': 'Road tool keys'`
- feature entry `roadKeys.heightUp` — src/ui/i18n/en.ts:743: `'roadKeys.heightUp': 'Raise the road 1 m'`
- feature entry `roadKeys.heightDown` — src/ui/i18n/en.ts:744: `'roadKeys.heightDown': 'Lower the road 1 m'`
- feature entry `roadKeys.alignment` — src/ui/i18n/en.ts:745: `'roadKeys.alignment': 'Change the plan'`
- feature entry `roadKeys.press` — src/ui/i18n/en.ts:746: `'roadKeys.press': 'Press a key…'`
- feature entry `roadKeys.reserved` — src/ui/i18n/en.ts:747: `'roadKeys.reserved': '{key} is already used by the game. Pick another.'`
- feature entry `roadKeys.reset` — src/ui/i18n/en.ts:748: `'roadKeys.reset': 'Restore defaults'`
- feature entry `profile.title` — src/ui/i18n/en.ts:749: `'profile.title': 'Road profile'`
- feature entry `catalog.cat.streets` — src/ui/i18n/en.ts:750: `'catalog.cat.streets': "Streets"`
- feature entry `catalog.cat.avenues` — src/ui/i18n/en.ts:751: `'catalog.cat.avenues': "Avenues"`
- feature entry `catalog.cat.highways` — src/ui/i18n/en.ts:752: `'catalog.cat.highways': "Highways"`
- feature entry `catalog.cat.special` — src/ui/i18n/en.ts:753: `'catalog.cat.special': "Special"`
- feature entry `catalog.cat.mine` — src/ui/i18n/en.ts:754: `'catalog.cat.mine': "My roads"`
- feature entry `catalog.empty` — src/ui/i18n/en.ts:755: `'catalog.empty': "Nothing in this category."`
- feature entry `catalog.mineEmpty` — src/ui/i18n/en.ts:756: `'catalog.mineEmpty': "Your roads appear here: pick a road, click \"Customise…\" and save it as a template."`
- feature entry `catalog.meta` — src/ui/i18n/en.ts:757: `'catalog.meta': "{width} m · {price}/m"`
- feature entry `catalog.tip` — src/ui/i18n/en.ts:758: `'catalog.tip': "{name}: {lanes}, {way}, {speed} km/h. Click to draw with it."`
- feature entry `catalog.oneWay` — src/ui/i18n/en.ts:759: `'catalog.oneWay': "one-way"`
- feature entry `catalog.twoWay` — src/ui/i18n/en.ts:760: `'catalog.twoWay': "two-way"`
- feature entry `catalog.customise` — src/ui/i18n/en.ts:761: `'catalog.customise': "Customise…"`
- feature entry `profileEditor.fold` — src/ui/i18n/en.ts:762: `'profileEditor.fold': "Fold or open"`
- feature entry `profileEditor.title` — src/ui/i18n/en.ts:788: `'profileEditor.title': 'Road profile'`
- feature entry `profileEditor.sub` — src/ui/i18n/en.ts:789: `'profileEditor.sub': '{target} · seen from A towards B'`
- feature entry `profileEditor.open` — src/ui/i18n/en.ts:790: `'profileEditor.open': 'Edit profile'`
- feature entry `profileEditor.thisRoad` — src/ui/i18n/en.ts:791: `'profileEditor.thisRoad': 'This road'`
- feature entry `profileEditor.newRoads` — src/ui/i18n/en.ts:792: `'profileEditor.newRoads': 'New roads'`
- feature entry `profileEditor.close` — src/ui/i18n/en.ts:793: `'profileEditor.close': 'Close'`
- feature entry `profileEditor.templates` — src/ui/i18n/en.ts:794: `'profileEditor.templates': 'Templates'`
- feature entry `profileEditor.blank` — src/ui/i18n/en.ts:795: `'profileEditor.blank': 'Blank'`
- feature entry `profileEditor.deleteTemplate` — src/ui/i18n/en.ts:796: `'profileEditor.deleteTemplate': 'Delete template'`
- feature entry `profileEditor.bands` — src/ui/i18n/en.ts:797: `'profileEditor.bands': 'The road across. Click an element to pick it, drag it to change the order, drag the line between two elements to change the width.'`
- feature entry `profileEditor.totalWidth` — src/ui/i18n/en.ts:798: `'profileEditor.totalWidth': 'Total width {width} m'`
- feature entry `profileEditor.leftEnd` — src/ui/i18n/en.ts:799: `'profileEditor.leftEnd': 'Left edge'`
- feature entry `profileEditor.rightEnd` — src/ui/i18n/en.ts:800: `'profileEditor.rightEnd': 'Right edge'`
- feature entry `profileEditor.empty` — src/ui/i18n/en.ts:801: `'profileEditor.empty': 'Nothing on the road yet. Start from a template or add elements below.'`
- feature entry `profileEditor.pick` — src/ui/i18n/en.ts:802: `'profileEditor.pick': 'Click an element on the road to edit it. Drag the line between two elements to change the width; drag an element to change the order.'`
- feature entry `profileEditor.add` — src/ui/i18n/en.ts:803: `'profileEditor.add': 'Add'`
- feature entry `profileEditor.palette.footway` — src/ui/i18n/en.ts:804: `'profileEditor.palette.footway': 'Footway'`
- feature entry `profileEditor.palette.laneBackward` — src/ui/i18n/en.ts:805: `'profileEditor.palette.laneBackward': 'Lane to A'`
- feature entry `profileEditor.palette.laneForward` — src/ui/i18n/en.ts:806: `'profileEditor.palette.laneForward': 'Lane to B'`
- feature entry `profileEditor.palette.median` — src/ui/i18n/en.ts:807: `'profileEditor.palette.median': 'Median'`
- feature entry `profileEditor.palette.parking` — src/ui/i18n/en.ts:808: `'profileEditor.palette.parking': 'Parking'`
- feature entry `profileEditor.palette.cycle` — src/ui/i18n/en.ts:809: `'profileEditor.palette.cycle': 'Cycle lane'`
- feature entry `profileEditor.width` — src/ui/i18n/en.ts:810: `'profileEditor.width': 'Width'`
- feature entry `profileEditor.allLanes` — src/ui/i18n/en.ts:811: `'profileEditor.allLanes': 'all lanes together'`
- feature entry `profileEditor.saveShort` — src/ui/i18n/en.ts:812: `'profileEditor.saveShort': 'Save template'`
- feature entry `profileEditor.narrower` — src/ui/i18n/en.ts:813: `'profileEditor.narrower': 'Narrower'`
- feature entry `profileEditor.wider` — src/ui/i18n/en.ts:814: `'profileEditor.wider': 'Wider'`
- feature entry `profileEditor.fixedWidth` — src/ui/i18n/en.ts:815: `'profileEditor.fixedWidth': 'fixed width'`
- feature entry `profileEditor.direction` — src/ui/i18n/en.ts:816: `'profileEditor.direction': 'Direction'`
- feature entry `profileEditor.dirForward` — src/ui/i18n/en.ts:817: `'profileEditor.dirForward': 'To B'`
- feature entry `profileEditor.dirBackward` — src/ui/i18n/en.ts:818: `'profileEditor.dirBackward': 'To A'`
- feature entry `profileEditor.use` — src/ui/i18n/en.ts:819: `'profileEditor.use': 'Use'`
- feature entry `profileEditor.useAll` — src/ui/i18n/en.ts:820: `'profileEditor.useAll': 'All'`
- feature entry `profileEditor.useBus` — src/ui/i18n/en.ts:821: `'profileEditor.useBus': 'Bus'`
- feature entry `profileEditor.lineRight` — src/ui/i18n/en.ts:822: `'profileEditor.lineRight': 'Line on the right'`
- feature entry `profileEditor.lineDashed` — src/ui/i18n/en.ts:823: `'profileEditor.lineDashed': 'Dashed'`
- feature entry `profileEditor.lineSolid` — src/ui/i18n/en.ts:824: `'profileEditor.lineSolid': 'Solid'`
- feature entry `profileEditor.lineBus` — src/ui/i18n/en.ts:825: `'profileEditor.lineBus': 'solid beside a bus lane'`
- feature entry `profileEditor.paving` — src/ui/i18n/en.ts:826: `'profileEditor.paving': 'Paving'`
- feature entry `profileEditor.flush` — src/ui/i18n/en.ts:827: `'profileEditor.flush': 'No kerb (level with the road)'`
- feature entry `profileEditor.medianFlush` — src/ui/i18n/en.ts:828: `'profileEditor.medianFlush': 'Painted on the road (no kerb)'`
- feature entry `profileEditor.moveLeft` — src/ui/i18n/en.ts:829: `'profileEditor.moveLeft': 'Move left'`
- feature entry `profileEditor.moveRight` — src/ui/i18n/en.ts:830: `'profileEditor.moveRight': 'Move right'`
- feature entry `profileEditor.remove` — src/ui/i18n/en.ts:831: `'profileEditor.remove': 'Remove'`
- feature entry `profileEditor.road` — src/ui/i18n/en.ts:832: `'profileEditor.road': 'The whole road'`
- feature entry `profileEditor.class` — src/ui/i18n/en.ts:833: `'profileEditor.class': 'Class'`
- feature entry `profileEditor.centreLine` — src/ui/i18n/en.ts:834: `'profileEditor.centreLine': 'Centre line'`
- feature entry `profileEditor.centreLineName.auto` — src/ui/i18n/en.ts:835: `'profileEditor.centreLineName.auto': 'Automatic'`
- feature entry `profileEditor.centreLineName.dashed` — src/ui/i18n/en.ts:836: `'profileEditor.centreLineName.dashed': 'Dashed'`
- feature entry `profileEditor.centreLineName.solid` — src/ui/i18n/en.ts:837: `'profileEditor.centreLineName.solid': 'Solid'`
- feature entry `profileEditor.centreLineName.double` — src/ui/i18n/en.ts:838: `'profileEditor.centreLineName.double': 'Double'`
- feature entry `profileEditor.speed` — src/ui/i18n/en.ts:839: `'profileEditor.speed': 'Speed'`
- feature entry `profileEditor.speedValue` — src/ui/i18n/en.ts:840: `'profileEditor.speedValue': '{speed} km/h'`
- feature entry `profileEditor.slower` — src/ui/i18n/en.ts:841: `'profileEditor.slower': 'Slower'`
- feature entry `profileEditor.faster` — src/ui/i18n/en.ts:842: `'profileEditor.faster': 'Faster'`
- feature entry `profileEditor.ok` — src/ui/i18n/en.ts:843: `'profileEditor.ok': 'This road can be built.'`
- feature entry `profileEditor.seeIssue` — src/ui/i18n/en.ts:844: `'profileEditor.seeIssue': 'Show'`
- feature entry `profileEditor.apply` — src/ui/i18n/en.ts:845: `'profileEditor.apply': 'Apply to the road'`
- feature entry `profileEditor.drawWith` — src/ui/i18n/en.ts:846: `'profileEditor.drawWith': 'Draw new roads with it'`
- feature entry `profileEditor.custom` — src/ui/i18n/en.ts:847: `'profileEditor.custom': 'Custom profile'`
- feature entry `profileEditor.save` — src/ui/i18n/en.ts:848: `'profileEditor.save': 'Save'`
- feature entry `profileEditor.cancel` — src/ui/i18n/en.ts:849: `'profileEditor.cancel': 'Cancel'`
- feature entry `profile.diagram` — src/ui/i18n/en.ts:850: `'profile.diagram': 'The road across, left edge to right edge, seen from A towards B'`
- feature entry `profile.summary.lanes.one` — src/ui/i18n/en.ts:851: `'profile.summary.lanes.one': '1 lane'`
- feature entry `profile.summary.lanes.other` — src/ui/i18n/en.ts:852: `'profile.summary.lanes.other': '{count} lanes'`
- feature entry `profile.material.carriageway` — src/ui/i18n/en.ts:853: `'profile.material.carriageway': 'Carriageway'`
- feature entry `profile.materialName.asphalt` — src/ui/i18n/en.ts:854: `'profile.materialName.asphalt': 'Asphalt'`
- feature entry `profile.materialName.concrete` — src/ui/i18n/en.ts:855: `'profile.materialName.concrete': 'Concrete'`
- feature entry `profile.materialName.cobble` — src/ui/i18n/en.ts:856: `'profile.materialName.cobble': 'Cobblestones'`
- feature entry `profile.materialName.pavers` — src/ui/i18n/en.ts:857: `'profile.materialName.pavers': 'Pavers'`
- feature entry `profile.materialName.stone` — src/ui/i18n/en.ts:858: `'profile.materialName.stone': 'Stone'`
- feature entry `profile.materialName.grass` — src/ui/i18n/en.ts:859: `'profile.materialName.grass': 'Grass'`
- feature entry `profile.element.footway` — src/ui/i18n/en.ts:860: `'profile.element.footway': 'Footway'`
- feature entry `profile.element.parking` — src/ui/i18n/en.ts:861: `'profile.element.parking': 'Parking'`
- feature entry `profile.element.cycle` — src/ui/i18n/en.ts:862: `'profile.element.cycle': 'Cycle lane'`
- feature entry `profile.element.lane` — src/ui/i18n/en.ts:863: `'profile.element.lane': 'Lane'`
- feature entry `profile.element.median` — src/ui/i18n/en.ts:864: `'profile.element.median': 'Median'`
- feature entry `profile.templateName` — src/ui/i18n/en.ts:865: `'profile.templateName': 'Template name'`
- feature entry `profile.notSaved` — src/ui/i18n/en.ts:866: `'profile.notSaved': 'Not saved: give it a name.'`
- feature entry `profile.notStored` — src/ui/i18n/en.ts:867: `'profile.notStored': 'Not saved: browser storage is full or blocked.'`
- feature entry `profile.notRemoved` — src/ui/i18n/en.ts:868: `'profile.notRemoved': 'Not deleted: browser storage is full or blocked.'`
- feature entry `profile.problem.footways` — src/ui/i18n/en.ts:869: `'profile.problem.footways': 'A road needs a footway at each edge, and only there.'`
- feature entry `profile.problem.noLanes` — src/ui/i18n/en.ts:870: `'profile.problem.noLanes': 'A road needs at least one lane.'`
- feature entry `profile.problem.laneWidths` — src/ui/i18n/en.ts:871: `'profile.problem.laneWidths': 'All lanes must be the same width.'`
- feature entry `profile.problem.laneBalance` — src/ui/i18n/en.ts:872: `'profile.problem.laneBalance': 'Both directions must have the same number of lanes.'`
- feature entry `profile.problem.order` — src/ui/i18n/en.ts:873: `'profile.problem.order': 'Lanes to A go on the left, lanes to B on the right.'`
- feature entry `profile.problem.median` — src/ui/i18n/en.ts:874: `'profile.problem.median': 'A median goes between the two directions.'`
- feature entry `profile.problem.parkingSide` — src/ui/i18n/en.ts:875: `'profile.problem.parkingSide': 'Parking and cycle lanes go next to the footway.'`
- feature entry `profile.problem.width` — src/ui/i18n/en.ts:876: `'profile.problem.width': 'This element is narrower or wider than can be built.'`
- feature entry `profile.problem.busOnly` — src/ui/i18n/en.ts:877: `'profile.problem.busOnly': 'Each direction needs at least one lane for every vehicle besides its bus lanes.'`
- feature entry `connectors.title` — src/ui/i18n/en.ts:878: `'connectors.title': 'Lane connections'`
- feature entry `connectors.auto` — src/ui/i18n/en.ts:879: `'connectors.auto': 'automatic'`
- feature entry `connectors.custom` — src/ui/i18n/en.ts:880: `'connectors.custom': 'set by hand'`
- feature entry `connectors.none` — src/ui/i18n/en.ts:881: `'connectors.none': 'This node has no lanes to connect.'`
- feature entry `connectors.pickIn` — src/ui/i18n/en.ts:882: `'connectors.pickIn': 'Click an arriving lane (blue) to see where it may go.'`
- feature entry `connectors.pickOut` — src/ui/i18n/en.ts:883: `'connectors.pickOut': 'Click a leaving lane (green) to add or remove that connection.'`
- feature entry `connectors.lastExit` — src/ui/i18n/en.ts:884: `'connectors.lastExit': 'That is the lane’s last way on: it cannot be left without one.'`
- feature entry `connectors.reset` — src/ui/i18n/en.ts:885: `'connectors.reset': 'Restore automatic'`
- feature entry `profile.problem.class` — src/ui/i18n/en.ts:886: `'profile.problem.class': 'This road class has no parking or cycle lane.'`
- feature entry `hint.rule.funds` — src/ui/i18n/en.ts:887: `'hint.rule.funds': 'Refused: not enough money to build this. Demolishing roads gives back part of their price.'`
- feature entry `economy.money` — src/ui/i18n/en.ts:888: `'economy.money': '$ {value}'`
- feature entry `economy.balance` — src/ui/i18n/en.ts:889: `'economy.balance': 'Money in hand. Roads cost by width and length; bridges and tunnels more. Demolishing a road gives back {share}% of its price.'`
- feature entry `economy.refund` — src/ui/i18n/en.ts:890: `'economy.refund': 'refund {value}'`
- feature entry `hint.road.clearance` — src/ui/i18n/en.ts:891: `'hint.road.clearance': 'Not built: it would cross a road too close in height to join it or to pass over it. Raise or lower it ({heightUp}/{heightDown}).'`
- feature entry `hint.control.miss` — src/ui/i18n/en.ts:903: `'hint.control.miss': 'No junction there — click where two or more roads meet'`
- shared entry `hint.pole.removed` — src/ui/i18n/en.ts:906: `'hint.pole.removed': 'Pole removed'`
- feature entry `hint.move.tooShort` — src/ui/i18n/en.ts:933: `'hint.move.tooShort': 'Not moved: a road there would be too short to build'`
- feature entry `hint.move.clearance` — src/ui/i18n/en.ts:934: `'hint.move.clearance': 'Not moved: the roads would cross too close in height to join or to pass over each other'`
- feature entry `inspector.parkingLeft` — src/ui/i18n/en.ts:1319: `'inspector.parkingLeft': 'Parking on the left'`
- feature entry `inspector.parkingRight` — src/ui/i18n/en.ts:1320: `'inspector.parkingRight': 'Parking on the right'`
- feature entry `parking.none` — src/ui/i18n/en.ts:1321: `'parking.none': 'No parking'`
- feature entry `parking.parallel` — src/ui/i18n/en.ts:1322: `'parking.parallel': 'Parallel to the kerb'`
- feature entry `parking.cycle` — src/ui/i18n/en.ts:1323: `'parking.cycle': 'Cycle lane'`
- feature entry `parking.preset.cycle` — src/ui/i18n/en.ts:1324: `'parking.preset.cycle': 'Cycle lanes on both sides'`
- feature entry `parking.preset.cycleParking` — src/ui/i18n/en.ts:1325: `'parking.preset.cycleParking': 'Cycle lane on the right, parking on the left'`
- feature entry `parking.preset.none` — src/ui/i18n/en.ts:1326: `'parking.preset.none': 'No parking'`
- feature entry `parking.preset.parallel` — src/ui/i18n/en.ts:1327: `'parking.preset.parallel': 'Parallel bays on both sides'`
- feature entry `parking.preset.parallelLeft` — src/ui/i18n/en.ts:1328: `'parking.preset.parallelLeft': 'Parallel bays on the left'`
- feature entry `parking.preset.parallelRight` — src/ui/i18n/en.ts:1329: `'parking.preset.parallelRight': 'Parallel bays on the right'`
- feature entry `v2.row.lanes` — src/ui/i18n/en.ts:1334: `'v2.row.lanes': 'Lanes'`
- feature entry `v2.row.width` — src/ui/i18n/en.ts:1335: `'v2.row.width': 'Width'`
- feature entry `v2.row.parking` — src/ui/i18n/en.ts:1337: `'v2.row.parking': 'Parking'`
- feature entry `hint.roundabout.size` — src/ui/i18n/pt-BR.ts:62: `'hint.roundabout.size': 'Escolha um raio entre 32 e 128 m.'`
- feature entry `hint.roundabout.bounds` — src/ui/i18n/pt-BR.ts:63: `'hint.roundabout.bounds': 'Mantenha toda a rotatória dentro do mapa.'`
- feature entry `hint.roundabout.occupied` — src/ui/i18n/pt-BR.ts:64: `'hint.roundabout.occupied': 'Escolha terreno livre com espaço para os quatro acessos.'`
- feature entry `hint.roundabout.geometry` — src/ui/i18n/pt-BR.ts:65: `'hint.roundabout.geometry': 'A rotatória não cabe aqui.'`
- feature entry `road.section.title` — src/ui/i18n/pt-BR.ts:66: `'road.section.title': 'Seção transversal · experimental'`
- feature entry `road.section.preview` — src/ui/i18n/pt-BR.ts:67: `'road.section.preview': 'Prévia da seção transversal da via'`
- feature entry `road.section.walk` — src/ui/i18n/pt-BR.ts:68: `'road.section.walk': 'Passeio'`
- feature entry `road.section.laneWidth` — src/ui/i18n/pt-BR.ts:69: `'road.section.laneWidth': 'Largura da faixa'`
- feature entry `road.section.sidewalk` — src/ui/i18n/pt-BR.ts:70: `'road.section.sidewalk': 'Calçada de cada lado'`
- feature entry `road.section.median` — src/ui/i18n/pt-BR.ts:71: `'road.section.median': 'Canteiro central'`
- feature entry `road.section.speedKmh` — src/ui/i18n/pt-BR.ts:72: `'road.section.speedKmh': 'Limite de velocidade'`
- feature entry `road.section.priority` — src/ui/i18n/pt-BR.ts:73: `'road.section.priority': 'Preferência (maior passa primeiro)'`
- feature entry `road.section.reset` — src/ui/i18n/pt-BR.ts:74: `'road.section.reset': 'Restaurar dimensões da classe'`
- feature entry `road.section.turns` — src/ui/i18n/pt-BR.ts:75: `'road.section.turns': 'Setas de conversão por faixa'`
- feature entry `road.section.turnsForward` — src/ui/i18n/pt-BR.ts:76: `'road.section.turnsForward': 'A → B · faixa {count} a partir do centro'`
- feature entry `road.section.turnsBackward` — src/ui/i18n/pt-BR.ts:77: `'road.section.turnsBackward': 'B → A · faixa {count} a partir do centro'`
- feature entry `road.turn.all` — src/ui/i18n/pt-BR.ts:78: `'road.turn.all': 'Todas as direções'`
- feature entry `road.turn.left` — src/ui/i18n/pt-BR.ts:79: `'road.turn.left': '← Só esquerda'`
- feature entry `road.turn.through` — src/ui/i18n/pt-BR.ts:80: `'road.turn.through': '↑ Só em frente'`
- feature entry `road.turn.right` — src/ui/i18n/pt-BR.ts:81: `'road.turn.right': '→ Só direita'`
- feature entry `road.turn.leftThrough` — src/ui/i18n/pt-BR.ts:82: `'road.turn.leftThrough': '← ↑ Esquerda ou em frente'`
- feature entry `road.turn.throughRight` — src/ui/i18n/pt-BR.ts:83: `'road.turn.throughRight': '↑ → Em frente ou direita'`
- feature entry `screenDir.up` — src/ui/i18n/pt-BR.ts:107: `'screenDir.up': 'Via de cima'`
- feature entry `screenDir.upRight` — src/ui/i18n/pt-BR.ts:108: `'screenDir.upRight': 'Via de cima à direita'`
- feature entry `screenDir.right` — src/ui/i18n/pt-BR.ts:109: `'screenDir.right': 'Via da direita'`
- feature entry `screenDir.downRight` — src/ui/i18n/pt-BR.ts:110: `'screenDir.downRight': 'Via de baixo à direita'`
- feature entry `screenDir.down` — src/ui/i18n/pt-BR.ts:111: `'screenDir.down': 'Via de baixo'`
- feature entry `screenDir.downLeft` — src/ui/i18n/pt-BR.ts:112: `'screenDir.downLeft': 'Via de baixo à esquerda'`
- feature entry `screenDir.left` — src/ui/i18n/pt-BR.ts:113: `'screenDir.left': 'Via da esquerda'`
- feature entry `screenDir.upLeft` — src/ui/i18n/pt-BR.ts:114: `'screenDir.upLeft': 'Via de cima à esquerda'`
- feature entry `compass.n` — src/ui/i18n/pt-BR.ts:115: `'compass.n': 'norte'`
- feature entry `compass.ne` — src/ui/i18n/pt-BR.ts:116: `'compass.ne': 'nordeste'`
- feature entry `compass.e` — src/ui/i18n/pt-BR.ts:117: `'compass.e': 'leste'`
- feature entry `compass.se` — src/ui/i18n/pt-BR.ts:118: `'compass.se': 'sudeste'`
- feature entry `compass.s` — src/ui/i18n/pt-BR.ts:119: `'compass.s': 'sul'`
- feature entry `compass.sw` — src/ui/i18n/pt-BR.ts:120: `'compass.sw': 'sudoeste'`
- feature entry `compass.w` — src/ui/i18n/pt-BR.ts:121: `'compass.w': 'oeste'`
- feature entry `compass.nw` — src/ui/i18n/pt-BR.ts:122: `'compass.nw': 'noroeste'`
- feature entry `roadFurniture.complete` — src/ui/i18n/pt-BR.ts:294: `'roadFurniture.complete': 'Completo'`
- feature entry `roadFurniture.basic` — src/ui/i18n/pt-BR.ts:295: `'roadFurniture.basic': 'Básico'`
- feature entry `roadFurniture.none` — src/ui/i18n/pt-BR.ts:296: `'roadFurniture.none': 'Nenhum'`
- feature entry `hint.streetscape.rule.stop` — src/ui/i18n/pt-BR.ts:297: `'hint.streetscape.rule.stop': 'Placa de Pare: esta via agora para no cruzamento'`
- feature entry `hint.streetscape.rule.yield` — src/ui/i18n/pt-BR.ts:298: `'hint.streetscape.rule.yield': 'Placa de Dê a preferência: esta via agora dá a preferência no cruzamento'`
- feature entry `profile.drawWith` — src/ui/i18n/pt-BR.ts:299: `'profile.drawWith': 'Desenhar com este perfil'`
- feature entry `profile.drawWithTip` — src/ui/i18n/pt-BR.ts:300: `'profile.drawWithTip': 'Conta-gotas: a ferramenta de vias pega esta via como ela é, para desenhar outras iguais'`
- feature entry `profile.copied` — src/ui/i18n/pt-BR.ts:301: `'profile.copied': 'Via copiada'`
- feature entry `profile.toStreet` — src/ui/i18n/pt-BR.ts:302: `'profile.toStreet': 'Rua inteira ({count})'`
- feature entry `profile.toStreetTip` — src/ui/i18n/pt-BR.ts:303: `'profile.toStreetTip': 'Este perfil em todos os trechos da rua, atravessando os cruzamentos'`
- feature entry `hint.profile.toStreet` — src/ui/i18n/pt-BR.ts:304: `'hint.profile.toStreet': 'Perfil copiado para a rua inteira'`
- feature entry `hint.profile.picked` — src/ui/i18n/pt-BR.ts:305: `'hint.profile.picked': 'Desenhando com o perfil escolhido'`
- feature entry `inspector.numbers` — src/ui/i18n/pt-BR.ts:306: `'inspector.numbers': 'Nº {from} a {to} · pares à direita no sentido da numeração'`
- feature entry `hint.streetscape.row` — src/ui/i18n/pt-BR.ts:332: `'hint.streetscape.row': 'Fileira posta ao longo da calçada'`
- feature entry `hint.streetscape.removed` — src/ui/i18n/pt-BR.ts:333: `'hint.streetscape.removed': 'Item removido'`
- feature entry `hint.streetscape.offFootway` — src/ui/i18n/pt-BR.ts:334: `'hint.streetscape.offFootway': 'Só nas calçadas'`
- feature entry `hint.streetscape.narrow` — src/ui/i18n/pt-BR.ts:335: `'hint.streetscape.narrow': 'A calçada é estreita demais para isso'`
- feature entry `hint.streetscape.occupied` — src/ui/i18n/pt-BR.ts:336: `'hint.streetscape.occupied': 'Já há algo aqui'`
- feature entry `hint.streetscape.crossing` — src/ui/i18n/pt-BR.ts:337: `'hint.streetscape.crossing': 'Não na saída de uma faixa de pedestres'`
- feature entry `hint.blocks.built` — src/ui/i18n/pt-BR.ts:397: `'hint.blocks.built': 'Quarteirões construídos'`
- feature entry `palette.width` — src/ui/i18n/pt-BR.ts:398: `'palette.width': 'Largura total'`
- feature entry `palette.width.auto` — src/ui/i18n/pt-BR.ts:399: `'palette.width.auto': 'Padrão'`
- shared entry `palette.lanes.count.one` — src/ui/i18n/pt-BR.ts:400: `'palette.lanes.count.one': '{count} faixa'`
- shared entry `palette.lanes.count.other` — src/ui/i18n/pt-BR.ts:401: `'palette.lanes.count.other': '{count} faixas'`
- feature entry `road.way.two` — src/ui/i18n/pt-BR.ts:427: `'road.way.two': ' / mão dupla'`
- feature entry `road.way.one` — src/ui/i18n/pt-BR.ts:428: `'road.way.one': ' / mão única'`
- feature entry `terrain.level` — src/ui/i18n/pt-BR.ts:575: `'terrain.level': 'Nivelar em'`
- feature entry `inspector.road` — src/ui/i18n/pt-BR.ts:580: `'inspector.road': 'VIA'`
- feature entry `inspector.length` — src/ui/i18n/pt-BR.ts:583: `'inspector.length': 'EXTENSÃO'`
- feature entry `inspector.speed` — src/ui/i18n/pt-BR.ts:584: `'inspector.speed': 'VELOCIDADE'`
- feature entry `inspector.lanes` — src/ui/i18n/pt-BR.ts:585: `'inspector.lanes': 'FAIXAS'`
- feature entry `inspector.direction` — src/ui/i18n/pt-BR.ts:586: `'inspector.direction': 'Sentido do tráfego'`
- feature entry `inspector.laneCount` — src/ui/i18n/pt-BR.ts:587: `'inspector.laneCount': 'Faixas de circulação'`
- feature entry `inspector.curvePosition` — src/ui/i18n/pt-BR.ts:588: `'inspector.curvePosition': 'Posição da curva'`
- feature entry `inspector.heightStart` — src/ui/i18n/pt-BR.ts:589: `'inspector.heightStart': 'Altura inicial (m)'`
- feature entry `inspector.heightEnd` — src/ui/i18n/pt-BR.ts:590: `'inspector.heightEnd': 'Altura final (m)'`
- feature entry `inspector.heightNode` — src/ui/i18n/pt-BR.ts:595: `'inspector.heightNode': 'Altura da via (m)'`
- shared entry `inspector.vehicles` — src/ui/i18n/pt-BR.ts:598: `'inspector.vehicles': 'VEÍCULOS'`
- shared entry `inspector.capacity` — src/ui/i18n/pt-BR.ts:599: `'inspector.capacity': 'CAPACIDADE'`
- shared entry `inspector.capacityValue.one` — src/ui/i18n/pt-BR.ts:600: `'inspector.capacityValue.one': '{count} veículo'`
- shared entry `inspector.capacityValue.other` — src/ui/i18n/pt-BR.ts:601: `'inspector.capacityValue.other': '{count} veículos'`
- feature entry `inspector.lanesOneWay` — src/ui/i18n/pt-BR.ts:604: `'inspector.lanesOneWay': '{count} em mão única'`
- feature entry `inspector.lanesTwoWay` — src/ui/i18n/pt-BR.ts:605: `'inspector.lanesTwoWay': '{count} ({perSide} por sentido)'`
- feature entry `inspector.upgrade` — src/ui/i18n/pt-BR.ts:607: `'inspector.upgrade': 'Melhorar classe'`
- feature entry `inspector.duplicate` — src/ui/i18n/pt-BR.ts:608: `'inspector.duplicate': 'Duplicar via'`
- feature entry `inspector.impossible` — src/ui/i18n/pt-BR.ts:609: `'inspector.impossible': 'IMPOSSÍVEL DE CONSTRUIR'`
- feature entry `inspector.impossibleBody` — src/ui/i18n/pt-BR.ts:610: `'inspector.impossibleBody': 'Abaixo de 25° a junção não fecha. Ela está sendo desenhada em forma ' + 'degradada para não pintar calçada sobre a grama.'`
- feature entry `inspector.roadClass` — src/ui/i18n/pt-BR.ts:614: `'inspector.roadClass': 'Classe da via'`
- feature entry `inspector.curvature` — src/ui/i18n/pt-BR.ts:615: `'inspector.curvature': 'Curvatura'`
- shared entry `inspector.meanSpeed` — src/ui/i18n/pt-BR.ts:616: `'inspector.meanSpeed': 'VEL. MÉDIA'`
- shared entry `inspector.queue` — src/ui/i18n/pt-BR.ts:617: `'inspector.queue': 'FILA'`
- shared entry `inspector.volume` — src/ui/i18n/pt-BR.ts:618: `'inspector.volume': 'VOLUME'`
- feature entry `inspector.mouths` — src/ui/i18n/pt-BR.ts:623: `'inspector.mouths': 'Recuo por via'`
- feature entry `inspector.reverse` — src/ui/i18n/pt-BR.ts:624: `'inspector.reverse': 'Inverter mão única'`
- feature entry `inspector.duplicateHint` — src/ui/i18n/pt-BR.ts:625: `'inspector.duplicateHint': 'Duplicar via (Ctrl/Cmd+D)'`
- feature entry `inspector.splitMiddle` — src/ui/i18n/pt-BR.ts:626: `'inspector.splitMiddle': 'Dividir no meio'`
- feature entry `inspector.addZebra` — src/ui/i18n/pt-BR.ts:627: `'inspector.addZebra': 'Faixa zebra'`
- feature entry `inspector.addSignalCrossing` — src/ui/i18n/pt-BR.ts:628: `'inspector.addSignalCrossing': 'Faixa com semáforo'`
- feature entry `inspector.addUturnToEnd` — src/ui/i18n/pt-BR.ts:629: `'inspector.addUturnToEnd': 'Retorno para quem vai à ponta final'`
- feature entry `inspector.addUturnToStart` — src/ui/i18n/pt-BR.ts:630: `'inspector.addUturnToStart': 'Retorno para quem vai à ponta inicial'`
- feature entry `hint.crossing.tooShort` — src/ui/i18n/pt-BR.ts:634: `'hint.crossing.tooShort': 'A faixa precisa de espaço para uma fila de carros dos dois lados.'`
- feature entry `hint.crossing.unsupported` — src/ui/i18n/pt-BR.ts:635: `'hint.crossing.unsupported': 'Esta via não tem pedestres.'`
- feature entry `hint.crossing.geometry` — src/ui/i18n/pt-BR.ts:636: `'hint.crossing.geometry': 'Não é possível colocar uma faixa aqui.'`
- feature entry `hint.uturn.placed` — src/ui/i18n/pt-BR.ts:637: `'hint.uturn.placed': 'Retorno aberto no canteiro.'`
- feature entry `hint.uturn.noMedian` — src/ui/i18n/pt-BR.ts:638: `'hint.uturn.noMedian': 'O retorno precisa de uma via de mão dupla com canteiro central.'`
- feature entry `hint.uturn.tooShort` — src/ui/i18n/pt-BR.ts:639: `'hint.uturn.tooShort': 'O retorno precisa de espaço para uma fila de carros dos dois lados.'`
- feature entry `hint.uturn.narrow` — src/ui/i18n/pt-BR.ts:640: `'hint.uturn.narrow': 'Canteiro estreito demais: um carro dá a volta em 12,8 m de uma faixa à outra. Alargue o canteiro no perfil da via.'`
- feature entry `hint.uturn.geometry` — src/ui/i18n/pt-BR.ts:641: `'hint.uturn.geometry': 'Não é possível abrir um retorno aqui.'`
- feature entry `inspector.addHeightPoint` — src/ui/i18n/pt-BR.ts:642: `'inspector.addHeightPoint': 'Adicionar ponto de altura aqui'`
- feature entry `inspector.demolish` — src/ui/i18n/pt-BR.ts:643: `'inspector.demolish': 'Demolir'`
- feature entry `inspector.impossibleGap` — src/ui/i18n/pt-BR.ts:644: `'inspector.impossibleGap': '{angle}° entre duas vias'`
- feature entry `inspector.joinAligned` — src/ui/i18n/pt-BR.ts:645: `'inspector.joinAligned': 'Unir vias alinhadas'`
- feature entry `inspector.laneOption.one` — src/ui/i18n/pt-BR.ts:646: `'inspector.laneOption.one': '{count} faixa'`
- feature entry `inspector.laneOption.other` — src/ui/i18n/pt-BR.ts:647: `'inspector.laneOption.other': '{count} faixas'`
- feature entry `turn.left` — src/ui/i18n/pt-BR.ts:654: `'turn.left': 'esquerda'`
- feature entry `turn.right` — src/ui/i18n/pt-BR.ts:655: `'turn.right': 'direita'`
- feature entry `turn.through` — src/ui/i18n/pt-BR.ts:656: `'turn.through': 'em frente'`
- feature entry `turn.uturn` — src/ui/i18n/pt-BR.ts:657: `'turn.uturn': 'retorno'`
- feature entry `direction.both` — src/ui/i18n/pt-BR.ts:659: `'direction.both': 'Mão dupla'`
- feature entry `direction.aToB` — src/ui/i18n/pt-BR.ts:660: `'direction.aToB': 'Mão única: A → B'`
- feature entry `direction.bToA` — src/ui/i18n/pt-BR.ts:661: `'direction.bToA': 'Mão única: B → A'`
- shared entry `junction.lower` — src/ui/i18n/pt-BR.ts:670: `'junction.lower': 'Descer 0,5 m'`
- shared entry `junction.raise` — src/ui/i18n/pt-BR.ts:671: `'junction.raise': 'Subir 0,5 m'`
- feature entry `junction.control` — src/ui/i18n/pt-BR.ts:672: `'junction.control': 'Controle do cruzamento'`
- feature entry `junction.control.auto` — src/ui/i18n/pt-BR.ts:673: `'junction.control.auto': 'Automático'`
- feature entry `junction.control.signal` — src/ui/i18n/pt-BR.ts:674: `'junction.control.signal': 'Semáforo'`
- feature entry `junction.control.priority` — src/ui/i18n/pt-BR.ts:675: `'junction.control.priority': 'Placas'`
- feature entry `junction.control.stop` — src/ui/i18n/pt-BR.ts:676: `'junction.control.stop': 'Pare em todas'`
- feature entry `junction.control.mini` — src/ui/i18n/pt-BR.ts:677: `'junction.control.mini': 'Minirrotatória'`
- feature entry `junction.control.none` — src/ui/i18n/pt-BR.ts:678: `'junction.control.none': 'Sem sinalização'`
- feature entry `junction.control.yield` — src/ui/i18n/pt-BR.ts:679: `'junction.control.yield': 'Preferência em todas'`
- feature entry `junction.autoChoice` — src/ui/i18n/pt-BR.ts:680: `'junction.autoChoice': 'Escolhido pelo fluxo: {choice}. Muda sozinho pela tendência, nunca por um pico.'`
- feature entry `junction.auto.signal` — src/ui/i18n/pt-BR.ts:681: `'junction.auto.signal': 'semáforo'`
- feature entry `junction.auto.stop` — src/ui/i18n/pt-BR.ts:682: `'junction.auto.stop': 'pare em todas'`
- feature entry `junction.auto.priority` — src/ui/i18n/pt-BR.ts:683: `'junction.auto.priority': 'via principal com preferência'`
- feature entry `junction.locked` — src/ui/i18n/pt-BR.ts:684: `'junction.locked': 'Travado na sua escolha.'`
- feature entry `junction.unlock` — src/ui/i18n/pt-BR.ts:685: `'junction.unlock': 'Destravar (automático)'`
- feature entry `junction.mainRoad` — src/ui/i18n/pt-BR.ts:686: `'junction.mainRoad': 'via principal'`
- feature entry `junction.perHour` — src/ui/i18n/pt-BR.ts:687: `'junction.perHour': '{n} veíc./h'`
- feature entry `junction.legs` — src/ui/i18n/pt-BR.ts:688: `'junction.legs': 'Regra de cada via'`
- feature entry `junction.rule.priority` — src/ui/i18n/pt-BR.ts:689: `'junction.rule.priority': 'Preferencial'`
- feature entry `junction.rule.yield` — src/ui/i18n/pt-BR.ts:690: `'junction.rule.yield': 'Dê a preferência'`
- feature entry `junction.rule.stop` — src/ui/i18n/pt-BR.ts:691: `'junction.rule.stop': 'Pare'`
- feature entry `junction.rulesReset` — src/ui/i18n/pt-BR.ts:692: `'junction.rulesReset': 'Voltar às regras automáticas'`
- feature entry `junction.signal` — src/ui/i18n/pt-BR.ts:693: `'junction.signal': 'Semáforo'`
- feature entry `junction.signalMode` — src/ui/i18n/pt-BR.ts:694: `'junction.signalMode': 'Tempos'`
- feature entry `junction.mode.adaptive` — src/ui/i18n/pt-BR.ts:695: `'junction.mode.adaptive': 'Adaptativo'`
- feature entry `junction.mode.fixed` — src/ui/i18n/pt-BR.ts:696: `'junction.mode.fixed': 'Tempo fixo'`
- feature entry `junction.stage` — src/ui/i18n/pt-BR.ts:697: `'junction.stage': 'Fase {n}'`
- feature entry `junction.shorter` — src/ui/i18n/pt-BR.ts:698: `'junction.shorter': 'Verde mais curto'`
- feature entry `junction.longer` — src/ui/i18n/pt-BR.ts:699: `'junction.longer': 'Verde mais longo'`
- feature entry `junction.offset` — src/ui/i18n/pt-BR.ts:700: `'junction.offset': 'Defasagem'`
- feature entry `junction.earlier` — src/ui/i18n/pt-BR.ts:701: `'junction.earlier': 'Mais cedo'`
- feature entry `junction.later` — src/ui/i18n/pt-BR.ts:702: `'junction.later': 'Mais tarde'`
- feature entry `junction.greenWave` — src/ui/i18n/pt-BR.ts:703: `'junction.greenWave': 'Onda verde na via principal'`
- feature entry `junction.greenWaveNone` — src/ui/i18n/pt-BR.ts:704: `'junction.greenWaveNone': 'A via principal não tem outro semáforo para coordenar.'`
- feature entry `junction.busPriority` — src/ui/i18n/pt-BR.ts:705: `'junction.busPriority': 'Prioridade ao ônibus'`
- feature entry `junction.movements` — src/ui/i18n/pt-BR.ts:706: `'junction.movements': 'Movimentos permitidos'`
- feature entry `junction.from` — src/ui/i18n/pt-BR.ts:707: `'junction.from': '{leg}'`
- feature entry `junction.moveBlocked` — src/ui/i18n/pt-BR.ts:708: `'junction.moveBlocked': 'Proibido: clique para permitir'`
- feature entry `junction.moveAllowed` — src/ui/i18n/pt-BR.ts:709: `'junction.moveAllowed': 'Permitido: clique para proibir'`
- feature entry `hint.road.gradeLimited` — src/ui/i18n/pt-BR.ts:713: `'hint.road.gradeLimited': 'A via atingiu a inclinação segura; continue desenhando para chegar à altura escolhida.'`
- feature entry `hint.road.invalid` — src/ui/i18n/pt-BR.ts:714: `'hint.road.invalid': 'Esta conexão precisa de mais distância ou outro ponto final.'`
- feature entry `hint.rule.sharp` — src/ui/i18n/pt-BR.ts:715: `'hint.rule.sharp': 'Recusado: as vias se encontrariam num ângulo fechado demais (menos de 25°).'`
- feature entry `hint.rule.squeezed` — src/ui/i18n/pt-BR.ts:716: `'hint.rule.squeezed': 'Recusado: trecho curto demais para as faixas se separarem na junção (junções perto demais ou ângulo fechado).'`
- feature entry `hint.rule.overlap` — src/ui/i18n/pt-BR.ts:717: `'hint.rule.overlap': 'Recusado: a via ficaria em cima de outra. Afaste, cruze ou passe por cima ({heightUp}/{heightDown}).'`
- feature entry `hint.rule.steep` — src/ui/i18n/pt-BR.ts:718: `'hint.rule.steep': 'Recusado: rampa íngreme demais entre as junções. Alongue a via ou mude a altura.'`
- feature entry `rule.short.sharp` — src/ui/i18n/pt-BR.ts:719: `'rule.short.sharp': 'ângulo fechado'`
- feature entry `rule.short.squeezed` — src/ui/i18n/pt-BR.ts:720: `'rule.short.squeezed': 'trecho curto demais'`
- feature entry `rule.short.overlap` — src/ui/i18n/pt-BR.ts:721: `'rule.short.overlap': 'sobre outra via'`
- feature entry `rule.short.steep` — src/ui/i18n/pt-BR.ts:722: `'rule.short.steep': 'íngreme demais'`
- feature entry `rule.short.clearance` — src/ui/i18n/pt-BR.ts:723: `'rule.short.clearance': 'altura sem vão'`
- feature entry `rule.short.funds` — src/ui/i18n/pt-BR.ts:724: `'rule.short.funds': 'dinheiro insuficiente'`
- feature entry `buildMode.ground` — src/ui/i18n/pt-BR.ts:725: `'buildMode.ground': 'no chão'`
- feature entry `buildMode.embankment` — src/ui/i18n/pt-BR.ts:726: `'buildMode.embankment': 'aterro'`
- feature entry `buildMode.cutting` — src/ui/i18n/pt-BR.ts:727: `'buildMode.cutting': 'trincheira'`
- feature entry `buildMode.bridge` — src/ui/i18n/pt-BR.ts:728: `'buildMode.bridge': 'ponte'`
- feature entry `buildMode.tunnel` — src/ui/i18n/pt-BR.ts:729: `'buildMode.tunnel': 'túnel'`
- feature entry `buildMode.cuttingWalls` — src/ui/i18n/pt-BR.ts:730: `'buildMode.cuttingWalls': 'trincheira com muro'`
- feature entry `cutSides.batter` — src/ui/i18n/pt-BR.ts:732: `'cutSides.batter': 'Talude'`
- feature entry `cutSides.walls` — src/ui/i18n/pt-BR.ts:733: `'cutSides.walls': 'Muro'`
- feature entry `roadKeys.row` — src/ui/i18n/pt-BR.ts:734: `'roadKeys.row': 'Teclas'`
- feature entry `roadKeys.title` — src/ui/i18n/pt-BR.ts:735: `'roadKeys.title': 'Teclas da ferramenta de vias'`
- feature entry `roadKeys.heightUp` — src/ui/i18n/pt-BR.ts:736: `'roadKeys.heightUp': 'Subir a via 1 m'`
- feature entry `roadKeys.heightDown` — src/ui/i18n/pt-BR.ts:737: `'roadKeys.heightDown': 'Descer a via 1 m'`
- feature entry `roadKeys.alignment` — src/ui/i18n/pt-BR.ts:738: `'roadKeys.alignment': 'Trocar o traçado'`
- feature entry `roadKeys.press` — src/ui/i18n/pt-BR.ts:739: `'roadKeys.press': 'Aperte uma tecla…'`
- feature entry `roadKeys.reserved` — src/ui/i18n/pt-BR.ts:740: `'roadKeys.reserved': '{key} já é usada pelo jogo. Escolha outra.'`
- feature entry `roadKeys.reset` — src/ui/i18n/pt-BR.ts:741: `'roadKeys.reset': 'Restaurar padrão'`
- feature entry `profile.title` — src/ui/i18n/pt-BR.ts:742: `'profile.title': 'Perfil da via'`
- feature entry `catalog.cat.streets` — src/ui/i18n/pt-BR.ts:743: `'catalog.cat.streets': "Ruas"`
- feature entry `catalog.cat.avenues` — src/ui/i18n/pt-BR.ts:744: `'catalog.cat.avenues': "Avenidas"`
- feature entry `catalog.cat.highways` — src/ui/i18n/pt-BR.ts:745: `'catalog.cat.highways': "Rodovias"`
- feature entry `catalog.cat.special` — src/ui/i18n/pt-BR.ts:746: `'catalog.cat.special': "Especiais"`
- feature entry `catalog.cat.mine` — src/ui/i18n/pt-BR.ts:747: `'catalog.cat.mine': "Meus modelos"`
- feature entry `catalog.empty` — src/ui/i18n/pt-BR.ts:748: `'catalog.empty': "Nada nesta categoria."`
- feature entry `catalog.mineEmpty` — src/ui/i18n/pt-BR.ts:749: `'catalog.mineEmpty': "Seus modelos aparecem aqui: escolha uma via, clique em \"Personalizar…\" e salve como modelo."`
- feature entry `catalog.meta` — src/ui/i18n/pt-BR.ts:750: `'catalog.meta': "{width} m · {price}/m"`
- feature entry `catalog.tip` — src/ui/i18n/pt-BR.ts:751: `'catalog.tip': "{name}: {lanes}, {way}, {speed} km/h. Clique para desenhar com ela."`
- feature entry `catalog.oneWay` — src/ui/i18n/pt-BR.ts:752: `'catalog.oneWay': "mão única"`
- feature entry `catalog.twoWay` — src/ui/i18n/pt-BR.ts:753: `'catalog.twoWay': "mão dupla"`
- feature entry `catalog.customise` — src/ui/i18n/pt-BR.ts:754: `'catalog.customise': "Personalizar…"`
- feature entry `profileEditor.fold` — src/ui/i18n/pt-BR.ts:755: `'profileEditor.fold': "Recolher ou abrir"`
- feature entry `profileEditor.title` — src/ui/i18n/pt-BR.ts:781: `'profileEditor.title': 'Perfil da via'`
- feature entry `profileEditor.sub` — src/ui/i18n/pt-BR.ts:782: `'profileEditor.sub': '{target} · vista de A para B'`
- feature entry `profileEditor.open` — src/ui/i18n/pt-BR.ts:783: `'profileEditor.open': 'Editar perfil'`
- feature entry `profileEditor.thisRoad` — src/ui/i18n/pt-BR.ts:784: `'profileEditor.thisRoad': 'Esta via'`
- feature entry `profileEditor.newRoads` — src/ui/i18n/pt-BR.ts:785: `'profileEditor.newRoads': 'Vias novas'`
- feature entry `profileEditor.close` — src/ui/i18n/pt-BR.ts:786: `'profileEditor.close': 'Fechar'`
- feature entry `profileEditor.templates` — src/ui/i18n/pt-BR.ts:787: `'profileEditor.templates': 'Modelos'`
- feature entry `profileEditor.blank` — src/ui/i18n/pt-BR.ts:788: `'profileEditor.blank': 'Em branco'`
- feature entry `profileEditor.deleteTemplate` — src/ui/i18n/pt-BR.ts:789: `'profileEditor.deleteTemplate': 'Apagar modelo'`
- feature entry `profileEditor.bands` — src/ui/i18n/pt-BR.ts:790: `'profileEditor.bands': 'A via de lado a lado. Clique num elemento para escolhê-lo, arraste-o para mudar a ordem, arraste a linha entre dois elementos para mudar`
- feature entry `profileEditor.totalWidth` — src/ui/i18n/pt-BR.ts:791: `'profileEditor.totalWidth': 'Largura total {width} m'`
- feature entry `profileEditor.leftEnd` — src/ui/i18n/pt-BR.ts:792: `'profileEditor.leftEnd': 'Borda esquerda'`
- feature entry `profileEditor.rightEnd` — src/ui/i18n/pt-BR.ts:793: `'profileEditor.rightEnd': 'Borda direita'`
- feature entry `profileEditor.empty` — src/ui/i18n/pt-BR.ts:794: `'profileEditor.empty': 'Nada na via ainda. Comece por um modelo ou adicione elementos abaixo.'`
- feature entry `profileEditor.pick` — src/ui/i18n/pt-BR.ts:795: `'profileEditor.pick': 'Clique num elemento da via para editá-lo. Arraste a linha entre dois elementos para mudar a largura; arraste um elemento para mudar a ord`
- feature entry `profileEditor.add` — src/ui/i18n/pt-BR.ts:796: `'profileEditor.add': 'Adicionar'`
- feature entry `profileEditor.palette.footway` — src/ui/i18n/pt-BR.ts:797: `'profileEditor.palette.footway': 'Calçada'`
- feature entry `profileEditor.palette.laneBackward` — src/ui/i18n/pt-BR.ts:798: `'profileEditor.palette.laneBackward': 'Faixa para A'`
- feature entry `profileEditor.palette.laneForward` — src/ui/i18n/pt-BR.ts:799: `'profileEditor.palette.laneForward': 'Faixa para B'`
- feature entry `profileEditor.palette.median` — src/ui/i18n/pt-BR.ts:800: `'profileEditor.palette.median': 'Canteiro'`
- feature entry `profileEditor.palette.parking` — src/ui/i18n/pt-BR.ts:801: `'profileEditor.palette.parking': 'Estacionamento'`
- feature entry `profileEditor.palette.cycle` — src/ui/i18n/pt-BR.ts:802: `'profileEditor.palette.cycle': 'Ciclofaixa'`
- feature entry `profileEditor.width` — src/ui/i18n/pt-BR.ts:803: `'profileEditor.width': 'Largura'`
- feature entry `profileEditor.allLanes` — src/ui/i18n/pt-BR.ts:804: `'profileEditor.allLanes': 'todas as faixas juntas'`
- feature entry `profileEditor.saveShort` — src/ui/i18n/pt-BR.ts:805: `'profileEditor.saveShort': 'Salvar modelo'`
- feature entry `profileEditor.narrower` — src/ui/i18n/pt-BR.ts:806: `'profileEditor.narrower': 'Mais estreita'`
- feature entry `profileEditor.wider` — src/ui/i18n/pt-BR.ts:807: `'profileEditor.wider': 'Mais larga'`
- feature entry `profileEditor.fixedWidth` — src/ui/i18n/pt-BR.ts:808: `'profileEditor.fixedWidth': 'largura fixa'`
- feature entry `profileEditor.direction` — src/ui/i18n/pt-BR.ts:809: `'profileEditor.direction': 'Sentido'`
- feature entry `profileEditor.dirForward` — src/ui/i18n/pt-BR.ts:810: `'profileEditor.dirForward': 'Para B'`
- feature entry `profileEditor.dirBackward` — src/ui/i18n/pt-BR.ts:811: `'profileEditor.dirBackward': 'Para A'`
- feature entry `profileEditor.use` — src/ui/i18n/pt-BR.ts:812: `'profileEditor.use': 'Uso'`
- feature entry `profileEditor.useAll` — src/ui/i18n/pt-BR.ts:813: `'profileEditor.useAll': 'Todos'`
- feature entry `profileEditor.useBus` — src/ui/i18n/pt-BR.ts:814: `'profileEditor.useBus': 'Ônibus'`
- feature entry `profileEditor.lineRight` — src/ui/i18n/pt-BR.ts:815: `'profileEditor.lineRight': 'Linha à direita'`
- feature entry `profileEditor.lineDashed` — src/ui/i18n/pt-BR.ts:816: `'profileEditor.lineDashed': 'Tracejada'`
- feature entry `profileEditor.lineSolid` — src/ui/i18n/pt-BR.ts:817: `'profileEditor.lineSolid': 'Contínua'`
- feature entry `profileEditor.lineBus` — src/ui/i18n/pt-BR.ts:818: `'profileEditor.lineBus': 'contínua por ser faixa exclusiva'`
- feature entry `profileEditor.paving` — src/ui/i18n/pt-BR.ts:819: `'profileEditor.paving': 'Piso'`
- feature entry `profileEditor.flush` — src/ui/i18n/pt-BR.ts:820: `'profileEditor.flush': 'Sem meio-fio (no nível da rua)'`
- feature entry `profileEditor.medianFlush` — src/ui/i18n/pt-BR.ts:821: `'profileEditor.medianFlush': 'Pintado na pista (sem meio-fio)'`
- feature entry `profileEditor.moveLeft` — src/ui/i18n/pt-BR.ts:822: `'profileEditor.moveLeft': 'Mover para a esquerda'`
- feature entry `profileEditor.moveRight` — src/ui/i18n/pt-BR.ts:823: `'profileEditor.moveRight': 'Mover para a direita'`
- feature entry `profileEditor.remove` — src/ui/i18n/pt-BR.ts:824: `'profileEditor.remove': 'Remover'`
- feature entry `profileEditor.road` — src/ui/i18n/pt-BR.ts:825: `'profileEditor.road': 'A via inteira'`
- feature entry `profileEditor.class` — src/ui/i18n/pt-BR.ts:826: `'profileEditor.class': 'Classe'`
- feature entry `profileEditor.centreLine` — src/ui/i18n/pt-BR.ts:827: `'profileEditor.centreLine': 'Linha central'`
- feature entry `profileEditor.centreLineName.auto` — src/ui/i18n/pt-BR.ts:828: `'profileEditor.centreLineName.auto': 'Automática'`
- feature entry `profileEditor.centreLineName.dashed` — src/ui/i18n/pt-BR.ts:829: `'profileEditor.centreLineName.dashed': 'Tracejada'`
- feature entry `profileEditor.centreLineName.solid` — src/ui/i18n/pt-BR.ts:830: `'profileEditor.centreLineName.solid': 'Contínua'`
- feature entry `profileEditor.centreLineName.double` — src/ui/i18n/pt-BR.ts:831: `'profileEditor.centreLineName.double': 'Dupla'`
- feature entry `profileEditor.speed` — src/ui/i18n/pt-BR.ts:832: `'profileEditor.speed': 'Velocidade'`
- feature entry `profileEditor.speedValue` — src/ui/i18n/pt-BR.ts:833: `'profileEditor.speedValue': '{speed} km/h'`
- feature entry `profileEditor.slower` — src/ui/i18n/pt-BR.ts:834: `'profileEditor.slower': 'Mais devagar'`
- feature entry `profileEditor.faster` — src/ui/i18n/pt-BR.ts:835: `'profileEditor.faster': 'Mais rápido'`
- feature entry `profileEditor.ok` — src/ui/i18n/pt-BR.ts:836: `'profileEditor.ok': 'Esta via pode ser construída.'`
- feature entry `profileEditor.seeIssue` — src/ui/i18n/pt-BR.ts:837: `'profileEditor.seeIssue': 'Mostrar'`
- feature entry `profileEditor.apply` — src/ui/i18n/pt-BR.ts:838: `'profileEditor.apply': 'Aplicar na via'`
- feature entry `profileEditor.drawWith` — src/ui/i18n/pt-BR.ts:839: `'profileEditor.drawWith': 'Desenhar vias novas com ele'`
- feature entry `profileEditor.custom` — src/ui/i18n/pt-BR.ts:840: `'profileEditor.custom': 'Perfil próprio'`
- feature entry `profileEditor.save` — src/ui/i18n/pt-BR.ts:841: `'profileEditor.save': 'Salvar'`
- feature entry `profileEditor.cancel` — src/ui/i18n/pt-BR.ts:842: `'profileEditor.cancel': 'Cancelar'`
- feature entry `profile.diagram` — src/ui/i18n/pt-BR.ts:843: `'profile.diagram': 'A via de lado a lado, da borda esquerda à direita, vista de A para B'`
- feature entry `profile.summary.lanes.one` — src/ui/i18n/pt-BR.ts:844: `'profile.summary.lanes.one': '1 faixa'`
- feature entry `profile.summary.lanes.other` — src/ui/i18n/pt-BR.ts:845: `'profile.summary.lanes.other': '{count} faixas'`
- feature entry `profile.material.carriageway` — src/ui/i18n/pt-BR.ts:846: `'profile.material.carriageway': 'Pista'`
- feature entry `profile.materialName.asphalt` — src/ui/i18n/pt-BR.ts:847: `'profile.materialName.asphalt': 'Asfalto'`
- feature entry `profile.materialName.concrete` — src/ui/i18n/pt-BR.ts:848: `'profile.materialName.concrete': 'Concreto'`
- feature entry `profile.materialName.cobble` — src/ui/i18n/pt-BR.ts:849: `'profile.materialName.cobble': 'Paralelepípedo'`
- feature entry `profile.materialName.pavers` — src/ui/i18n/pt-BR.ts:850: `'profile.materialName.pavers': 'Bloquete'`
- feature entry `profile.materialName.stone` — src/ui/i18n/pt-BR.ts:851: `'profile.materialName.stone': 'Pedra'`
- feature entry `profile.materialName.grass` — src/ui/i18n/pt-BR.ts:852: `'profile.materialName.grass': 'Grama'`
- feature entry `profile.element.footway` — src/ui/i18n/pt-BR.ts:853: `'profile.element.footway': 'Calçada'`
- feature entry `profile.element.parking` — src/ui/i18n/pt-BR.ts:854: `'profile.element.parking': 'Estacionamento'`
- feature entry `profile.element.cycle` — src/ui/i18n/pt-BR.ts:855: `'profile.element.cycle': 'Ciclofaixa'`
- feature entry `profile.element.lane` — src/ui/i18n/pt-BR.ts:856: `'profile.element.lane': 'Faixa'`
- feature entry `profile.element.median` — src/ui/i18n/pt-BR.ts:857: `'profile.element.median': 'Canteiro'`
- feature entry `profile.templateName` — src/ui/i18n/pt-BR.ts:858: `'profile.templateName': 'Nome do modelo'`
- feature entry `profile.notSaved` — src/ui/i18n/pt-BR.ts:859: `'profile.notSaved': 'Não salvo: dê um nome.'`
- feature entry `profile.notStored` — src/ui/i18n/pt-BR.ts:860: `'profile.notStored': 'Não salvo: o armazenamento do navegador está cheio ou bloqueado.'`
- feature entry `profile.notRemoved` — src/ui/i18n/pt-BR.ts:861: `'profile.notRemoved': 'Não apagado: o armazenamento do navegador está cheio ou bloqueado.'`
- feature entry `profile.problem.footways` — src/ui/i18n/pt-BR.ts:862: `'profile.problem.footways': 'A via precisa de uma calçada em cada borda, e só nelas.'`
- feature entry `profile.problem.noLanes` — src/ui/i18n/pt-BR.ts:863: `'profile.problem.noLanes': 'A via precisa de pelo menos uma faixa.'`
- feature entry `profile.problem.laneWidths` — src/ui/i18n/pt-BR.ts:864: `'profile.problem.laneWidths': 'Todas as faixas precisam ter a mesma largura.'`
- feature entry `profile.problem.laneBalance` — src/ui/i18n/pt-BR.ts:865: `'profile.problem.laneBalance': 'Os dois sentidos precisam ter o mesmo número de faixas.'`
- feature entry `profile.problem.order` — src/ui/i18n/pt-BR.ts:866: `'profile.problem.order': 'As faixas para A ficam à esquerda; as faixas para B, à direita.'`
- feature entry `profile.problem.median` — src/ui/i18n/pt-BR.ts:867: `'profile.problem.median': 'O canteiro fica entre os dois sentidos.'`
- feature entry `profile.problem.parkingSide` — src/ui/i18n/pt-BR.ts:868: `'profile.problem.parkingSide': 'Estacionamento e ciclofaixa ficam junto à calçada.'`
- feature entry `profile.problem.width` — src/ui/i18n/pt-BR.ts:869: `'profile.problem.width': 'Este elemento está mais estreito ou mais largo do que se pode construir.'`
- feature entry `profile.problem.busOnly` — src/ui/i18n/pt-BR.ts:870: `'profile.problem.busOnly': 'Cada sentido precisa de pelo menos uma faixa para todos os veículos, além das de ônibus.'`
- feature entry `connectors.title` — src/ui/i18n/pt-BR.ts:871: `'connectors.title': 'Conexões de faixa'`
- feature entry `connectors.auto` — src/ui/i18n/pt-BR.ts:872: `'connectors.auto': 'automáticas'`
- feature entry `connectors.custom` — src/ui/i18n/pt-BR.ts:873: `'connectors.custom': 'ajustadas à mão'`
- feature entry `connectors.none` — src/ui/i18n/pt-BR.ts:874: `'connectors.none': 'Este nó não tem faixas para conectar.'`
- feature entry `connectors.pickIn` — src/ui/i18n/pt-BR.ts:875: `'connectors.pickIn': 'Clique numa faixa que chega (azul) para ver aonde ela pode ir.'`
- feature entry `connectors.pickOut` — src/ui/i18n/pt-BR.ts:876: `'connectors.pickOut': 'Clique numa faixa que sai (verde) para ligar ou desligar a conexão.'`
- feature entry `connectors.lastExit` — src/ui/i18n/pt-BR.ts:877: `'connectors.lastExit': 'Essa é a última saída da faixa: ela não pode ficar sem caminho.'`
- feature entry `connectors.reset` — src/ui/i18n/pt-BR.ts:878: `'connectors.reset': 'Restaurar automáticas'`
- feature entry `profile.problem.class` — src/ui/i18n/pt-BR.ts:879: `'profile.problem.class': 'Esta classe de via não tem estacionamento nem ciclofaixa.'`
- feature entry `hint.rule.funds` — src/ui/i18n/pt-BR.ts:880: `'hint.rule.funds': 'Recusado: não há dinheiro para construir isto. Demolir vias devolve parte do preço delas.'`
- feature entry `economy.money` — src/ui/i18n/pt-BR.ts:881: `'economy.money': 'R$ {value}'`
- feature entry `economy.balance` — src/ui/i18n/pt-BR.ts:882: `'economy.balance': 'Dinheiro em caixa. Vias custam pela largura e pelo comprimento; pontes e túneis custam mais. Demolir uma via devolve {share}% do preço dela.`
- feature entry `economy.refund` — src/ui/i18n/pt-BR.ts:883: `'economy.refund': 'devolve {value}'`
- feature entry `hint.road.clearance` — src/ui/i18n/pt-BR.ts:884: `'hint.road.clearance': 'Não construída: cruzaria uma via com diferença de altura pequena demais para se unir a ela ou passar por cima. Suba ou desça a via ({hei`
- feature entry `hint.control.miss` — src/ui/i18n/pt-BR.ts:896: `'hint.control.miss': 'Nenhum cruzamento aí — clique onde duas ou mais vias se encontram'`
- shared entry `hint.pole.removed` — src/ui/i18n/pt-BR.ts:899: `'hint.pole.removed': 'Poste removido'`
- feature entry `hint.move.tooShort` — src/ui/i18n/pt-BR.ts:926: `'hint.move.tooShort': 'Não movido: uma via ali ficaria curta demais para ser construída'`
- feature entry `hint.move.clearance` — src/ui/i18n/pt-BR.ts:927: `'hint.move.clearance': 'Não movido: as vias se cruzariam com diferença de altura pequena demais para se unir ou passar uma sobre a outra'`
- feature entry `inspector.parkingLeft` — src/ui/i18n/pt-BR.ts:1309: `'inspector.parkingLeft': 'Estacionamento à esquerda'`
- feature entry `inspector.parkingRight` — src/ui/i18n/pt-BR.ts:1310: `'inspector.parkingRight': 'Estacionamento à direita'`
- feature entry `parking.none` — src/ui/i18n/pt-BR.ts:1311: `'parking.none': 'Sem vagas'`
- feature entry `parking.parallel` — src/ui/i18n/pt-BR.ts:1312: `'parking.parallel': 'Paralelo à guia'`
- feature entry `parking.cycle` — src/ui/i18n/pt-BR.ts:1313: `'parking.cycle': 'Ciclofaixa'`
- feature entry `parking.preset.cycle` — src/ui/i18n/pt-BR.ts:1314: `'parking.preset.cycle': 'Ciclofaixa dos dois lados'`
- feature entry `parking.preset.cycleParking` — src/ui/i18n/pt-BR.ts:1315: `'parking.preset.cycleParking': 'Ciclofaixa à direita, vagas à esquerda'`
- feature entry `parking.preset.none` — src/ui/i18n/pt-BR.ts:1316: `'parking.preset.none': 'Sem estacionamento'`
- feature entry `parking.preset.parallel` — src/ui/i18n/pt-BR.ts:1317: `'parking.preset.parallel': 'Vagas paralelas nos dois lados'`
- feature entry `parking.preset.parallelLeft` — src/ui/i18n/pt-BR.ts:1318: `'parking.preset.parallelLeft': 'Vagas paralelas à esquerda'`
- feature entry `parking.preset.parallelRight` — src/ui/i18n/pt-BR.ts:1319: `'parking.preset.parallelRight': 'Vagas paralelas à direita'`
- feature entry `v2.row.lanes` — src/ui/i18n/pt-BR.ts:1324: `'v2.row.lanes': 'Faixas'`
- feature entry `v2.row.width` — src/ui/i18n/pt-BR.ts:1325: `'v2.row.width': 'Largura'`
- feature entry `v2.row.parking` — src/ui/i18n/pt-BR.ts:1327: `'v2.row.parking': 'Vagas'`
- shared entry `undo` — src/ui/v2/shell.ts:85: `undo: '<path d="M9 7 4 12l5 5"/><path d="M5 12h8a6 6 0 0 1 6 6"/>'`
- feature entry `park-none` — src/ui/v2/shell.ts:136: `'park-none': '<path d="M5 3v18M19 3v18"/><path d="M12 4v3m0 3v4m0 3v3"/>'`
- feature entry `park-parallel` — src/ui/v2/shell.ts:137: `'park-parallel': '<path d="M3 3v18M21 3v18"/><path d="M7 3v18M17 3v18"/><path d="M3 9h4M3 15h4M17 9h4M17 15h4"/>'`
- feature entry `park-parallelRight` — src/ui/v2/shell.ts:138: `'park-parallelRight': '<path d="M4 3v18M21 3v18"/><path d="M17 3v18"/><path d="M17 9h4M17 15h4"/><path d="M10.5 4v3m0 3v4m0 3v3"/>'`
- feature entry `park-parallelLeft` — src/ui/v2/shell.ts:139: `'park-parallelLeft': '<path d="M3 3v18M20 3v18"/><path d="M7 3v18"/><path d="M3 9h4M3 15h4"/><path d="M13.5 4v3m0 3v4m0 3v3"/>'`
- feature entry `park-cycle` — src/ui/v2/shell.ts:140: `'park-cycle': '<path d="M3 3v18M21 3v18M7 3v18M17 3v18"/><circle cx="5" cy="12" r="1.2"/><circle cx="19" cy="12" r="1.2"/>'`
- feature entry `park-cycleParking` — src/ui/v2/shell.ts:141: `'park-cycleParking': '<path d="M3 3v18M21 3v18M7 3v18M17 3v18"/><path d="M3 9h4M3 15h4"/><circle cx="19" cy="12" r="1.2"/>'`
- feature entry `lanes2` — src/ui/v2/shell.ts:143: `lanes2: '<path d="M7 3v18M17 3v18"/><path d="M12 4v3m0 3v4m0 3v3" />'`
- feature entry `lanes4` — src/ui/v2/shell.ts:144: `lanes4: '<path d="M4 3v18M20 3v18"/><path d="M12 3v18"/><path d="M8 5v2m0 4v2m0 4v2M16 5v2m0 4v2m0 4v2"/>'`
- feature entry `lanes6` — src/ui/v2/shell.ts:145: `lanes6: '<path d="M3 3v18M21 3v18M12 3v18"/><path d="M6 5v2m0 4v2m0 4v2M9 5v2m0 4v2m0 4v2M15 5v2m0 4v2m0 4v2M18 5v2m0 4v2m0 4v2"/>'`
- feature entry `median` — src/ui/v2/shell.ts:146: `median: '<path d="M3 3v18M21 3v18"/><path d="M10.5 3v18M13.5 3v18"/><path d="M7 5v2m0 4v2m0 4v2M17 5v2m0 4v2m0 4v2"/>'`
- feature entry `keyboard` — src/ui/v2/shell.ts:245: `keyboard: '<rect x="2.5" y="6" width="19" height="12" rx="2"/><path d="M6 10h1M9.5 10h1M13 10h1M16.5 10h1M7 14h10"/>'`
- shared entry `tree` — src/world/landscape.ts:58: `tree: TREE_PIT / 2`
- shared entry `shrub` — src/world/landscape.ts:59: `shrub: m(0.35)`
- shared entry `bench` — src/world/landscape.ts:60: `bench: Math.hypot(m(0.9), m(0.26))`
- shared entry `bin` — src/world/landscape.ts:61: `bin: m(0.33)`
- shared entry `lamp` — src/world/landscape.ts:62: `lamp: m(0.13)`
- shared entry `hydrant` — src/world/landscape.ts:63: `hydrant: m(0.16)`
- shared entry `postbox` — src/world/landscape.ts:64: `postbox: m(0.33)`
- shared entry `phone` — src/world/landscape.ts:65: `phone: m(0.45)`
- shared entry `drain` — src/world/landscape.ts:66: `drain: m(0.5)`
- shared entry `meadow` — src/world/landscape.ts:67: `meadow: m(2)`
- shared entry `sign` — src/world/landscape.ts:68: `sign: m(0.15)`
- shared entry `streetname` — src/world/landscape.ts:69: `streetname: m(1)`
- feature entry `none` — src/world/parking.ts:17: `none: 0`
- feature entry `parallel` — src/world/parking.ts:18: `parallel: m(2)`
- feature entry `cycle` — src/world/parking.ts:20: `cycle: m(2)`
- feature entry `parallel` — src/world/parking.ts:26: `parallel: m(6)`
- feature import `three/examples/jsm/libs/ktx-parse.module.js` — src/render/surfaceBakeClient.ts:6: `import { KHR_SUPERCOMPRESSION_ZSTD, read, type KTX2Container } from 'three/examples/jsm/libs/ktx-parse.module.js';`
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
- shared style `lab-table` — src/lab/lab.css:123: `.lab-table tbody tr:nth-child(even) { background: rgb(255 255 255 / 0.018); }`
- shared style `lab-table` — src/lab/lab.css:124: `.lab-table tbody tr:hover { background: rgb(47 194 171 / 0.12); }`
- shared style `lab-table` — src/lab/lab.css:125: `.lab-table tbody tr[data-selected="true"] { background: rgb(47 194 171 / 0.22); }`
- shared style `lab-body` — src/lab/lab.css:128: `.lab-body .effects-panel.embedded { --text: var(--lab-text); --muted: var(--lab-muted); --ui-border: var(--lab-line2); --v-accent-ink: var(--lab-accent); color:`
- shared style `lab-body` — src/lab/lab.css:129: `.lab-body .effects-panel.embedded .effects-group { background: var(--lab-bg2); }`
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
- shared style `seg` — src/peopleLab/peopleLab.css:54: `.seg { display: flex; background: var(--panel-2); border-radius: 7px; padding: 2px; gap: 2px; margin-bottom: 4px; }`
- shared style `seg` — src/peopleLab/peopleLab.css:55: `.seg button { flex: 1; border: 0; padding: 6px; border-radius: 5px; background: transparent; cursor: pointer; font-weight: 500; color: var(--ink-2); }`
- shared style `seg` — src/peopleLab/peopleLab.css:56: `.seg button.on { background: var(--panel-3); color: var(--ink); box-shadow: inset 0 0 0 1px #3a4048; }`
- shared style `swatches` — src/peopleLab/peopleLab.css:57: `.swatches { display: flex; flex-wrap: wrap; gap: 5px; align-items: center; }`
- shared style `swatches` — src/peopleLab/peopleLab.css:58: `.swatches button { width: 22px; height: 22px; border-radius: 50%; border: 0; box-shadow: inset 0 0 0 1px rgba(255,255,255,.12); cursor: pointer; padding: 0; }`
- shared style `swatches` — src/peopleLab/peopleLab.css:59: `.swatches button.on { box-shadow: 0 0 0 2px var(--panel), 0 0 0 3.5px var(--accent); }`
- shared style `swatches` — src/peopleLab/peopleLab.css:60: `.swatches input[type=color] { width: 24px; height: 24px; border: 0; padding: 0; background: none; cursor: pointer; }`
- shared style `grid` — src/peopleLab/peopleLab.css:61: `.grid { display: grid; grid-template-columns: repeat(3, 1fr); gap: 6px; }`
- shared style `tile` — src/peopleLab/peopleLab.css:62: `.tile { border: 1px solid var(--line); border-radius: var(--radius); background: var(--panel-2); padding: 9px 6px; cursor: pointer; display: flex; flex-directio`
- shared style `tile` — src/peopleLab/peopleLab.css:65: `.tile img { width: 56px; height: 56px; object-fit: contain; border-radius: 6px; }`
- shared style `tile` — src/peopleLab/peopleLab.css:66: `.tile:hover { border-color: #3d434b; color: var(--ink); }`
- shared style `tile` — src/peopleLab/peopleLab.css:67: `.tile.on { border-color: var(--accent); color: var(--ink); background: var(--accent-soft); }`
- shared style `fine` — src/peopleLab/peopleLab.css:84: `details.fine { margin: 2px 0 6px; }`
- shared style `fine` — src/peopleLab/peopleLab.css:85: `details.fine summary { cursor: pointer; font-size: 12px; color: var(--ink-3); padding: 6px 0; list-style: none; }`
- shared style `fine` — src/peopleLab/peopleLab.css:86: `details.fine summary::-webkit-details-marker { display: none; }`
- shared style `fine` — src/peopleLab/peopleLab.css:87: `details.fine summary::before { content: '+ '; }`
- shared style `fine` — src/peopleLab/peopleLab.css:88: `details.fine[open] summary::before { content: '\2013 '; }`
- shared style `fine` — src/peopleLab/peopleLab.css:89: `details.fine summary:hover { color: var(--ink); }`
- shared style `stage` — src/peopleLab/peopleLab.css:119: `.stage { position: relative; overflow: hidden; background: radial-gradient(ellipse at 50% 35%, var(--stage-top), var(--stage-bottom) 75%); }`
- shared style `stage` — src/peopleLab/peopleLab.css:120: `.stage canvas { display: block; width: 100%; height: 100%; }`
- shared style `stats` — src/peopleLab/peopleLab.css:131: `.stats { position: absolute; left: 14px; bottom: 16px; font-size: 11px; color: var(--ink-3); line-height: 1.5; font-variant-numeric: tabular-nums; }`
- shared style `side` — src/peopleLab/peopleLab.css:142: `.side { background: var(--panel); border-left: 1px solid var(--line); display: flex; flex-direction: column; padding: 14px; gap: 8px; }`
- shared style `side` — src/peopleLab/peopleLab.css:143: `.side h2 { margin: 0 0 2px; font-size: 11px; font-weight: 600; letter-spacing: .06em; text-transform: uppercase; color: var(--ink-3); }`
- shared style `subtabs` — src/peopleLab/peopleLab.css:159: `.subtabs { display: flex; flex-wrap: wrap; gap: 2px; margin: -2px -2px 12px; padding-bottom: 10px; border-bottom: 1px solid var(--line); }`
- shared style `subtabs` — src/peopleLab/peopleLab.css:160: `.subtabs button { border: 0; background: transparent; padding: 5px 9px; border-radius: 6px; cursor: pointer; font-size: 12px; font-weight: 500; color: var(--ink`
- shared style `subtabs` — src/peopleLab/peopleLab.css:161: `.subtabs button:hover { color: var(--ink); background: var(--panel-2); }`
- shared style `subtabs` — src/peopleLab/peopleLab.css:162: `.subtabs button.on { color: var(--ink); background: var(--panel-3); }`
- shared style `inspect-select` — src/ui/app.css:96: `.inspect-select select option { color: #0d1512; background: #e8efe9; }`
- shared style `inspect-actions` — src/ui/app.css:110: `.inspect-actions { display: grid; grid-template-columns: 1fr 1fr; gap: 6px; margin-top: 8px; }`
- shared style `inspect-actions` — src/ui/app.css:111: `.inspect-actions button { padding: 8px; border-radius: 9px; border: 1px solid var(--ui-border); background: transparent; color: var(--text); font-size: 11px; cu`
- shared style `inspect-actions` — src/ui/app.css:121: `.inspect-actions button:hover:not(:disabled) { border-color: rgba(101, 229, 195, 0.45); background: rgba(101, 229, 195, 0.08); }`
- shared style `inspect-actions` — src/ui/app.css:123: `.inspect-actions button:disabled { opacity: 0.4; cursor: default; }`
- shared style `inspect-actions` — src/ui/app.css:124: `.inspect-actions button:last-child:nth-child(odd) { grid-column: 1 / -1; }`
- shared style `inspect-select` — src/ui/app.css:125: `.inspect-select { display: grid; gap: 4px; margin-top: 8px; color: var(--muted); font-size: 10px; }`
- shared style `inspect-select` — src/ui/app.css:126: `.inspect-select select { width: 100%; border: 1px solid var(--ui-border); border-radius: 8px; background: rgba(20, 28, 23, 0.7); color: var(--text); padding: 7p`
- shared style `inspect-range` — src/ui/app.css:127: `.inspect-range { display: grid; grid-template-columns: 1fr auto; align-items: center; gap: 2px 8px; margin-top: 9px; color: var(--muted); font-size: 11px; }`
- shared style `inspect-range` — src/ui/app.css:136: `.inspect-range input { grid-column: 1 / -1; width: 100%; margin: 2px 0 0; accent-color: var(--accent); }`
- shared style `inspect-range` — src/ui/app.css:137: `.inspect-range output { color: var(--text); font-variant-numeric: tabular-nums; }`
- shared style `inspect-range` — src/ui/app.css:138: `.inspect-range[data-disabled] { opacity: 0.45; }`
- shared style `signal-chips` — src/ui/app.css:140: `.signal-chips { display: flex; flex-wrap: wrap; gap: 5px; margin-top: 6px; }`
- shared style `signal-chip` — src/ui/app.css:141: `.signal-chip { display: inline-flex; align-items: center; gap: 5px; padding: 4px 8px 4px 6px; border-radius: 999px; border: 1px solid var(--ui-border); backgrou`
- shared style `signal-chip` — src/ui/app.css:152: `.signal-chip i { width: 8px; height: 8px; border-radius: 50%; background: #ff5b50; box-shadow: 0 0 6px currentColor; color: #ff5b50; }`
- shared style `signal-chip` — src/ui/app.css:153: `.signal-chip[data-state="green"] i { background: #4ee08a; color: #4ee08a; }`
- shared style `signal-chip` — src/ui/app.css:154: `.signal-chip[data-state="amber"] i { background: #ffc03d; color: #ffc03d; }`
- feature style `about-dialog` — src/ui/app.css:156: `.about-dialog { max-width: min(560px, calc(100vw - 32px)); color: var(--text); padding: 18px 20px; border: 1px solid var(--ui-border); border-radius: 12px; }`
- feature style `about-dialog` — src/ui/app.css:157: `.about-dialog::backdrop { background: rgb(0 0 0 / 0.35); }`
- feature style `about-dialog` — src/ui/app.css:158: `.about-dialog h2 { margin: 0 0 10px; font-size: 16px; }`
- feature style `about-dialog` — src/ui/app.css:159: `.about-dialog .about-close { margin-top: 12px; padding: 6px 14px; border-radius: 8px; border: 1px solid var(--ui-border); background: transparent; color: var(--`
- shared style `effects-panel` — src/ui/app.css:173: `.effects-panel { position: fixed; top: 64px; right: 12px; z-index: 70; width: min(380px, calc(100vw - 24px)); max-height: calc(100vh - 76px); overflow: auto; co`
- shared style `effects-panel` — src/ui/app.css:174: `.effects-panel[hidden] { display: none; }`
- shared style `effects-panel` — src/ui/app.css:175: `.effects-panel.embedded { position: static; width: auto; max-height: none; overflow: visible; background: none; border: 0; box-shadow: none; padding: 0; }`
- shared style `effects-panel` — src/ui/app.css:176: `.effects-panel.embedded .effects-close { display: none; }`
- shared style `effects-panel` — src/ui/app.css:177: `.effects-panel header { display: flex; align-items: center; gap: 6px; }`
- shared style `effects-panel` — src/ui/app.css:178: `.effects-panel h2 { margin: 0; font-size: 15px; flex: 1; }`
- shared style `effects-close` — src/ui/app.css:181: `.effects-close { border: 0; background: transparent; color: var(--text); font-size: 18px; cursor: pointer; padding: 0 4px; }`
- shared style `effects-stats` — src/ui/app.css:182: `.effects-stats { display: grid; grid-template-columns: 1fr auto; gap: 2px 10px; margin: 8px 0; padding: 8px; border: 1px solid var(--ui-border); border-radius: `
- shared style `effects-stats` — src/ui/app.css:183: `.effects-stats dt { color: var(--muted); }`
- shared style `effects-stats` — src/ui/app.css:184: `.effects-stats dd { margin: 0; text-align: right; }`
- shared style `effects-group` — src/ui/app.css:185: `.effects-group { margin: 8px 0 0; padding: 6px 8px; border: 1px solid var(--ui-border); border-radius: 8px; }`
- shared style `effects-group` — src/ui/app.css:186: `.effects-group legend { display: flex; align-items: center; gap: 4px; padding: 0 4px; color: var(--muted); font-weight: 600; }`
- shared style `effects-choice` — src/ui/app.css:187: `.effects-choice { display: grid; gap: 4px; padding: 4px 0; }`
- shared style `effects-item` — src/ui/app.css:188: `.effects-item { display: flex; align-items: center; gap: 6px; padding: 2px 0; cursor: pointer; }`
- feature style `about-dialog` — src/ui/app.css:189: `.about-dialog #aboutCredits { font-size: 12px; max-height: 50vh; overflow: auto; white-space: pre-wrap; }`
- shared style `rp-panel` — src/ui/roads/roads.css:3: `.rp-panel { position: fixed; left: 16px; right: 16px; bottom: calc(var(--v-dock-h, 54px) + 14px); z-index: 40; max-width: 1000px; margin: 0 auto; max-height: ca`
- shared style `rp-panel` — src/ui/roads/roads.css:18: `.rp-panel *, .rp-summary * { box-sizing: border-box; }`
- shared style `rp-panel` — src/ui/roads/roads.css:19: `.rp-panel button { font: inherit; color: inherit; cursor: pointer; }`
- shared style `rp-panel` — src/ui/roads/roads.css:20: `.rp-panel button:disabled { opacity: 0.35; cursor: default; }`
- shared style `rp-panel` — src/ui/roads/roads.css:21: `.rp-panel :focus-visible { outline: 2px solid var(--v-accent, #4fe0bf); outline-offset: 1px; }`
- shared style `rp-head` — src/ui/roads/roads.css:23: `.rp-head { position: relative; display: flex; flex-wrap: wrap; align-items: center; gap: 8px 10px; padding-right: 38px; }`
- shared style `rp-head` — src/ui/roads/roads.css:24: `.rp-head > .rp-icon { position: absolute; top: 0; right: 0; }`
- shared style `rp-head` — src/ui/roads/roads.css:25: `.rp-head .rp-foot { margin-left: auto; }`
- shared style `rp-title` — src/ui/roads/roads.css:26: `.rp-title { font-size: 15px; font-weight: 650; letter-spacing: 0.01em; }`
- shared style `rp-sub` — src/ui/roads/roads.css:27: `.rp-sub { color: var(--v-muted, #a3b4b0); font-size: 12.5px; }`
- shared style `rp-chip` — src/ui/roads/roads.css:28: `.rp-chip { padding: 4px 10px; border-radius: 999px; background: var(--v-raise, rgba(255,255,255,.05)); border: 1px solid var(--v-line); font-variant-numeric: ta`
- shared style `rp-panel` — src/ui/roads/roads.css:29: `.rp-panel .rp-icon { display: grid; place-items: center; width: 30px; height: 30px; border: 0; border-radius: var(--v-r-s, 8px); background: none; color: var(--`
- shared style `rp-icon` — src/ui/roads/roads.css:30: `.rp-icon:hover { background: var(--v-raise-2, rgba(255,255,255,.09)); color: var(--v-text); }`
- shared style `rp-icon` — src/ui/roads/roads.css:31: `.rp-icon svg, .rp-tool svg, .rp-btn svg, .rp-issue svg, .rp-ok svg { width: 18px; height: 18px; fill: none; stroke: currentColor; stroke-width: 1.8; stroke-line`
- shared style `rp-label` — src/ui/roads/roads.css:33: `.rp-label { color: var(--v-muted); font-size: 11.5px; font-weight: 600; text-transform: none; letter-spacing: 0.02em; }`
- shared style `rp-cards` — src/ui/roads/roads.css:36: `.rp-cards { display: flex; gap: 8px; overflow-x: auto; padding-bottom: 4px; scrollbar-width: thin; scrollbar-color: rgba(255,255,255,.18) transparent; }`
- shared style `rp-cards` — src/ui/roads/roads.css:37: `.rp-cards::-webkit-scrollbar { height: 6px; }`
- shared style `rp-cards` — src/ui/roads/roads.css:38: `.rp-cards::-webkit-scrollbar-thumb { background: rgba(255,255,255,.18); border-radius: 3px; }`
- shared style `rp-cards` — src/ui/roads/roads.css:39: `.rp-cards::-webkit-scrollbar-track { background: transparent; }`
- shared style `rp-card` — src/ui/roads/roads.css:40: `.rp-card { position: relative; flex: none; width: 96px; border: 1px solid var(--v-line); border-radius: 10px; background: var(--v-raise); }`
- shared style `rp-panel` — src/ui/roads/roads.css:41: `.rp-panel .rp-card-main { display: grid; gap: 5px; width: 100%; padding: 6px 6px 7px; border: 0; background: none; text-align: left; border-radius: 10px; }`
- shared style `rp-card` — src/ui/roads/roads.css:42: `.rp-card:hover { border-color: var(--v-accent-line, rgba(79,224,191,.5)); background: var(--v-raise-2); }`
- shared style `rp-card` — src/ui/roads/roads.css:43: `.rp-card.on { border-color: var(--v-accent); box-shadow: 0 0 0 2px var(--v-accent) inset; }`
- shared style `rp-panel` — src/ui/roads/roads.css:47: `.rp-panel .rp-card-x { position: absolute; top: 3px; right: 3px; width: 20px; height: 20px; display: grid; place-items: center; border: 0; border-radius: 6px; b`
- shared style `rp-card-x` — src/ui/roads/roads.css:48: `.rp-card-x svg { width: 12px; height: 12px; fill: none; stroke: currentColor; stroke-width: 2.4; stroke-linecap: round; }`
- shared style `rp-panel` — src/ui/roads/roads.css:49: `.rp-panel .rp-card-x:hover { color: var(--v-danger-ink, #ffb3aa); }`
- shared style `rp-stage` — src/ui/roads/roads.css:54: `.rp-stage { position: relative; border-radius: 10px; overflow: hidden; background: linear-gradient(180deg, #2b4448 0%, #1d2f32 70%); border: 1px solid var(--v-l`
- shared style `rp-stage` — src/ui/roads/roads.css:60: `.rp-stage .xs-dragging { opacity: 0.55; }`
- shared style `rp-end` — src/ui/roads/roads.css:64: `.rp-end { position: absolute; top: 6px; padding: 2px 7px; border-radius: 999px; background: rgba(0,0,0,.32); color: #c9d6d3; font-size: 11px; pointer-events: no`
- shared style `rp-end-l` — src/ui/roads/roads.css:65: `.rp-end-l { left: 8px; }`
- shared style `rp-end-r` — src/ui/roads/roads.css:65: `.rp-end-r { right: 8px; }`
- shared style `rp-picture` — src/ui/roads/roads.css:66: `.rp-picture { min-height: 120px; }`
- shared style `rp-palette` — src/ui/roads/roads.css:72: `.rp-palette { display: flex; flex-wrap: wrap; gap: 6px; align-items: center; }`
- shared style `rp-palette` — src/ui/roads/roads.css:73: `.rp-palette > .rp-label { margin-right: 4px; }`
- shared style `rp-tool` — src/ui/roads/roads.css:74: `.rp-tool { display: inline-flex; align-items: center; gap: 7px; min-height: 34px; padding: 0 12px 0 9px; border: 1px solid var(--v-line); border-radius: var(--v`
- shared style `rp-tool` — src/ui/roads/roads.css:75: `.rp-tool:hover { border-color: var(--v-accent-line); color: var(--v-text); background: var(--v-raise-2); }`
- shared style `rp-tool` — src/ui/roads/roads.css:76: `.rp-tool svg { width: 22px; height: 22px; color: #cfe3de; }`
- shared style `rp-props` — src/ui/roads/roads.css:79: `.rp-props { display: grid; grid-template-columns: minmax(0, 1fr) minmax(0, 1.12fr); gap: 8px; }`
- shared style `rp-panel` — src/ui/roads/roads.css:80: `@media (max-width: 760px) { .rp-panel { left: 8px; right: 8px; } }`
- shared style `rp-panel` — src/ui/roads/roads.css:82: `.rp-panel { container-type: inline-size; }`
- shared style `rp-props` — src/ui/roads/roads.css:83: `@container (max-width: 640px) { .rp-props { grid-template-columns: 1fr; } }`
- shared style `rp-block` — src/ui/roads/roads.css:84: `.rp-block { display: grid; gap: 8px; align-content: start; padding: 10px 12px; border: 1px solid var(--v-line); border-radius: 10px; background: var(--v-raise);`
- shared style `rp-block-head` — src/ui/roads/roads.css:85: `.rp-block-head { display: flex; align-items: center; gap: 8px; font-weight: 650; }`
- shared style `rp-block-head` — src/ui/roads/roads.css:86: `.rp-block-head .rp-swatch { width: 14px; height: 14px; border-radius: 4px; flex: none; }`
- shared style `rp-block-head` — src/ui/roads/roads.css:87: `.rp-block-head .rp-actions { margin-left: auto; display: flex; gap: 2px; }`
- shared style `rp-row` — src/ui/roads/roads.css:88: `.rp-row { display: grid; grid-template-columns: 76px minmax(0, 1fr); align-items: center; gap: 8px; min-height: 30px; }`
- shared style `rp-note` — src/ui/roads/roads.css:89: `.rp-note { color: var(--v-muted); font-size: 11.5px; }`
- shared style `rp-ctrls` — src/ui/roads/roads.css:90: `.rp-ctrls { display: flex; flex-wrap: wrap; align-items: center; gap: 6px; min-width: 0; }`
- shared style `rp-seg` — src/ui/roads/roads.css:92: `.rp-seg { display: inline-flex; flex-wrap: wrap; gap: 2px; padding: 3px; border-radius: 10px; background: rgba(0,0,0,.18); border: 1px solid var(--v-line); }`
- shared style `rp-seg` — src/ui/roads/roads.css:94: `.rp-seg button { min-height: 26px; padding: 0 11px; border: 0; border-radius: 7px; background: none; color: var(--v-muted); font-size: 12px; font-weight: 600; w`
- shared style `rp-seg` — src/ui/roads/roads.css:95: `.rp-seg button:hover { color: var(--v-text); }`
- feature style `dense` — src/ui/roads/roads.css:97: `.rp-seg.dense { display: grid; flex: 1 1 100%; grid-template-columns: repeat(auto-fill, minmax(96px, 1fr)); }`
- feature style `dense` — src/ui/roads/roads.css:98: `.rp-seg.dense button { padding: 0 8px; font-size: 11.5px; }`
- shared style `rp-seg` — src/ui/roads/roads.css:99: `.rp-seg button.on { background: var(--v-accent); color: var(--v-accent-ink, #062520); }`
- shared style `rp-step` — src/ui/roads/roads.css:101: `.rp-step { display: inline-grid; grid-template-columns: 28px minmax(52px, auto) 28px; align-items: center; gap: 2px; padding: 2px; border-radius: 999px; backgro`
- shared style `rp-step` — src/ui/roads/roads.css:102: `.rp-step button { width: 28px; height: 26px; border: 0; border-radius: 999px; background: none; color: var(--v-text); font-size: 16px; font-weight: 700; line-he`
- shared style `rp-step` — src/ui/roads/roads.css:103: `.rp-step button:hover:not(:disabled) { background: var(--v-raise-2); }`
- shared style `rp-step` — src/ui/roads/roads.css:104: `.rp-step output { text-align: center; font-weight: 650; font-variant-numeric: tabular-nums; }`
- shared style `rp-switch` — src/ui/roads/roads.css:106: `.rp-switch { display: inline-flex; align-items: center; gap: 8px; border: 0; background: none; padding: 2px 0; color: var(--v-text); font-size: 12.5px; }`
- shared style `rp-panel` — src/ui/roads/roads.css:112: `.rp-panel .rp-issue { width: 100%; text-align: left; font: inherit; display: flex; gap: 8px; align-items: flex-start; padding: 8px 10px; border-radius: 8px; bac`
- shared style `rp-issue` — src/ui/roads/roads.css:113: `button.rp-issue:hover { background: rgba(255, 111, 97, 0.2); }`
- shared style `rp-issue` — src/ui/roads/roads.css:114: `.rp-issue svg { flex: none; margin-top: 1px; color: var(--v-danger, #ff6f61); }`
- shared style `rp-ok` — src/ui/roads/roads.css:115: `.rp-ok { display: flex; gap: 8px; align-items: center; color: #9be3c9; font-size: 12.5px; }`
- shared style `rp-foot` — src/ui/roads/roads.css:118: `.rp-foot { display: flex; flex-wrap: wrap; align-items: center; gap: 8px; }`
- shared style `rp-btn` — src/ui/roads/roads.css:119: `.rp-btn { display: inline-flex; align-items: center; gap: 6px; min-height: 34px; padding: 0 14px; border-radius: 999px; border: 1px solid var(--v-line); backgro`
- shared style `rp-btn` — src/ui/roads/roads.css:120: `.rp-btn:hover:not(:disabled) { border-color: var(--v-accent-line); background: var(--v-raise-2); }`
- shared style `rp-btn` — src/ui/roads/roads.css:121: `.rp-btn.primary { background: var(--v-accent); color: var(--v-accent-ink); border-color: var(--v-accent); }`
- shared style `rp-btn` — src/ui/roads/roads.css:122: `.rp-btn.primary:hover:not(:disabled) { filter: brightness(1.08); background: var(--v-accent); }`
- shared style `rp-field` — src/ui/roads/roads.css:124: `.rp-field { min-height: 32px; width: 180px; padding: 0 12px; border-radius: 999px; border: 1px solid var(--v-line); background: rgba(0,0,0,.25); color: var(--v-`
- shared style `rp-field` — src/ui/roads/roads.css:125: `.rp-field::placeholder { color: var(--v-muted); }`
- shared style `rp-field` — src/ui/roads/roads.css:126: `.rp-field:focus { border-color: var(--v-accent-line); }`
- shared style `rp-summary` — src/ui/roads/roads.css:129: `.rp-summary { display: grid; gap: 8px; margin: 10px 0; padding: 10px; border: 1px solid rgba(255,255,255,.12); border-radius: 10px; background: rgba(255,255,255`
- shared style `rp-summary-head` — src/ui/roads/roads.css:130: `.rp-summary-head { display: flex; align-items: baseline; gap: 8px; font-weight: 650; }`
- shared style `rp-summary-head` — src/ui/roads/roads.css:131: `.rp-summary-head .rp-sub { margin-left: auto; font-weight: 500; }`
- shared style `rp-summary-art` — src/ui/roads/roads.css:132: `.rp-summary-art { border-radius: 8px; overflow: hidden; background: linear-gradient(180deg, #2b4448 0%, #1d2f32 70%); }`
- shared style `rp-summary-art` — src/ui/roads/roads.css:133: `.rp-summary-art svg { display: block; width: 100%; height: auto; }`
- shared style `rp-summary` — src/ui/roads/roads.css:134: `.rp-summary .rp-btn { justify-self: start; }`
- shared style `v2-pill` — src/ui/roads/roads.css:137: `.v2-pill.rp-open { width: auto; max-width: 170px; padding: 0 10px 0 8px; font-size: 12.5px; font-weight: 600; }`
- shared style `v2-pill` — src/ui/roads/roads.css:138: `.v2-pill.rp-open > span { position: static; width: auto; height: auto; overflow: hidden; clip: auto; text-overflow: ellipsis; white-space: nowrap; }`
- shared style `rp-keys` — src/ui/roads/roads.css:142: `.rp-keys { position: fixed; left: 330px; bottom: 80px; z-index: 41; width: 264px; display: grid; gap: 8px; padding: 12px; background: rgba(14, 22, 24, 0.97); co`
- shared style `rp-keys` — src/ui/roads/roads.css:148: `.rp-keys * { box-sizing: border-box; }`
- shared style `rp-keys` — src/ui/roads/roads.css:149: `.rp-keys button { font: inherit; color: inherit; cursor: pointer; }`
- shared style `rp-keys-head` — src/ui/roads/roads.css:150: `.rp-keys-head { display: flex; align-items: center; justify-content: space-between; }`
- shared style `rp-keys` — src/ui/roads/roads.css:151: `.rp-keys .rp-icon { display: grid; place-items: center; width: 28px; height: 28px; border: 0; border-radius: 8px; background: none; color: var(--v-muted); }`
- shared style `rp-keys` — src/ui/roads/roads.css:152: `.rp-keys .rp-icon svg { width: 16px; height: 16px; fill: none; stroke: currentColor; stroke-width: 2; stroke-linecap: round; }`
- shared style `rp-keys-row` — src/ui/roads/roads.css:153: `.rp-keys-row { display: flex; align-items: center; justify-content: space-between; gap: 8px; min-height: 32px; }`
- shared style `rp-key` — src/ui/roads/roads.css:154: `.rp-key { min-width: 86px; min-height: 30px; padding: 0 10px; border-radius: 8px; border: 1px solid var(--v-line); border-bottom-width: 3px; background: var(--v`
- shared style `rp-key` — src/ui/roads/roads.css:155: `.rp-key:hover { border-color: var(--v-accent-line, rgba(79,224,191,.5)); }`
- shared style `rp-key` — src/ui/roads/roads.css:156: `.rp-key.waiting { border-color: var(--v-accent, #4fe0bf); color: var(--v-accent, #4fe0bf); }`
- shared style `rp-keys` — src/ui/roads/roads.css:157: `.rp-keys .rp-btn { justify-self: start; display: inline-flex; align-items: center; min-height: 30px; padding: 0 12px; border-radius: 999px; border: 1px solid va`
- shared style `rp-keys` — src/ui/roads/roads.css:158: `.rp-keys .rp-note { color: var(--v-danger-ink, #ffb3aa); font-size: 12px; }`
- shared style `rp-keys` — src/ui/roads/roads.css:159: `.rp-keys :focus-visible { outline: 2px solid var(--v-accent, #4fe0bf); outline-offset: 1px; }`
- shared style `v2-seg-b` — src/ui/roads/roads.css:162: `.v2-seg-b.rp-seg-text { width: auto; padding: 0 11px; font-size: 12.5px; font-weight: 600; }`
- shared style `v2-seg-b` — src/ui/roads/roads.css:163: `.v2-seg-b.rp-seg-text > span { position: static; width: auto; height: auto; overflow: visible; clip: auto; }`
- shared style `cn-art` — src/ui/roads/roads.css:166: `.cn-art { background: #1d2a2c; }`
- shared style `cn-art` — src/ui/roads/roads.css:167: `.cn-art svg { cursor: default; }`
- shared style `rp-connectors` — src/ui/roads/roads.css:177: `.rp-connectors .rp-btn { justify-self: start; display: inline-flex; align-items: center; min-height: 30px; padding: 0 12px; border-radius: 999px; border: 1px so`
- shared style `rp-junction` — src/ui/roads/roads.css:180: `.rp-junction { display: grid; gap: 8px; margin: 10px 0; color: var(--v-text, #eef4f2); font: 13px/1.3 Inter, ui-sans-serif, "Segoe UI", Roboto, sans-serif; }`
- shared style `rp-junction` — src/ui/roads/roads.css:181: `.rp-junction * { box-sizing: border-box; }`
- shared style `rp-junction` — src/ui/roads/roads.css:182: `.rp-junction button { font: inherit; color: inherit; cursor: pointer; }`
- shared style `rp-junction` — src/ui/roads/roads.css:183: `.rp-junction button:disabled { opacity: 0.35; cursor: default; }`
- shared style `rp-junction` — src/ui/roads/roads.css:184: `.rp-junction :focus-visible { outline: 2px solid var(--v-accent, #4fe0bf); outline-offset: 1px; }`
- shared style `rp-junction` — src/ui/roads/roads.css:185: `.rp-junction .rp-block { display: grid; gap: 8px; padding: 10px; border: 1px solid rgba(255,255,255,.12); border-radius: 10px; background: rgba(255,255,255,.04)`
- shared style `rp-junction` — src/ui/roads/roads.css:186: `.rp-junction .rp-block-head { font-weight: 650; }`
- shared style `rp-junction` — src/ui/roads/roads.css:187: `.rp-junction .rp-row { display: grid; grid-template-columns: 84px minmax(0, 1fr); align-items: center; gap: 8px; min-height: 30px; }`
- feature style `dense` — src/ui/roads/roads.css:188: `.rp-junction .rp-seg.dense { display: grid; grid-template-columns: repeat(auto-fill, minmax(88px, 1fr)); }`
- shared style `rp-junction` — src/ui/roads/roads.css:189: `.rp-junction .rp-btn { justify-self: start; display: inline-flex; align-items: center; min-height: 30px; padding: 0 12px; border-radius: 999px; border: 1px soli`
- shared style `rp-junction` — src/ui/roads/roads.css:190: `.rp-junction .rp-btn:hover { border-color: var(--v-accent-line, rgba(79,224,191,.5)); }`
- shared style `rp-lock` — src/ui/roads/roads.css:191: `.rp-lock { display: flex; align-items: center; justify-content: space-between; gap: 8px; color: var(--v-muted, #a3b4b0); font-size: 12px; }`
- shared style `rp-flows` — src/ui/roads/roads.css:192: `.rp-flows { display: grid; gap: 2px; }`
- shared style `rp-flow` — src/ui/roads/roads.css:193: `.rp-flow { display: flex; justify-content: space-between; font-size: 12px; color: var(--v-muted, #a3b4b0); font-variant-numeric: tabular-nums; }`
- shared style `rp-flow` — src/ui/roads/roads.css:194: `.rp-flow strong { color: var(--v-text, #eef4f2); font-weight: 650; }`
- shared style `rp-moves` — src/ui/roads/roads.css:195: `.rp-moves { display: grid; gap: 4px; }`
- shared style `rp-moves` — src/ui/roads/roads.css:196: `.rp-moves .rp-ctrls { display: flex; flex-wrap: wrap; gap: 4px; }`
- shared style `rp-move` — src/ui/roads/roads.css:197: `.rp-move { min-height: 28px; padding: 0 10px; border-radius: 8px; border: 1px solid rgba(255,255,255,.12); background: rgba(0,0,0,.18); color: var(--v-muted, #a`
- shared style `rp-move` — src/ui/roads/roads.css:198: `.rp-move.on { background: rgba(79, 224, 191, 0.14); border-color: rgba(79, 224, 191, 0.45); color: var(--v-text, #eef4f2); text-decoration: none; }`
- shared style `rp-panel` — src/ui/roads/roads.css:202: `.rp-panel.docked { top: calc(var(--v-hud-h, 46px) + 22px); bottom: 160px; max-width: 920px; gap: 6px; padding: 10px 12px 12px; }`
- shared style `rp-panel` — src/ui/roads/roads.css:203: `.rp-panel.docked .rp-cards { display: none; }`
- shared style `rp-panel` — src/ui/roads/roads.css:204: `.rp-panel.docked.folded > :not(.rp-head) { display: none; }`
- shared style `rp-panel` — src/ui/roads/roads.css:205: `.rp-panel.docked.folded { bottom: auto !important; }`
- shared style `rp-panel` — src/ui/roads/roads.css:206: `.rp-panel.docked.folded [data-action="fold"] svg { transform: rotate(180deg); }`
- shared style `rc-catalog` — src/ui/roads/roads.css:210: `.rc-catalog { display: grid; gap: 6px; min-width: 0; font: 12px/1.25 Inter, ui-sans-serif, "Segoe UI", Roboto, sans-serif; color: var(--v-text, #eef4f2); }`
- shared style `rc-catalog` — src/ui/roads/roads.css:211: `.rc-catalog * { box-sizing: border-box; }`
- shared style `rc-catalog` — src/ui/roads/roads.css:212: `.rc-catalog button { font: inherit; color: inherit; cursor: pointer; }`
- shared style `rc-tabs` — src/ui/roads/roads.css:213: `.rc-tabs { display: flex; gap: 2px; padding: 2px; border-radius: 9px; background: rgba(0,0,0,.22); justify-self: start; }`
- shared style `rc-tab` — src/ui/roads/roads.css:214: `.rc-tab { display: inline-flex; align-items: center; gap: 5px; min-height: 26px; padding: 0 10px; border-radius: 7px; border: 0; background: none; font-weight: `
- shared style `rc-tab` — src/ui/roads/roads.css:215: `.rc-tab em { font-style: normal; font-size: 10.5px; opacity: .7; }`
- shared style `rc-tab` — src/ui/roads/roads.css:216: `.rc-tab:hover { color: var(--v-text, #eef4f2); background: rgba(255,255,255,.06); }`
- shared style `rc-tab` — src/ui/roads/roads.css:217: `.rc-tab.on { background: var(--v-raise-2, rgba(255,255,255,.12)); color: var(--v-text, #eef4f2); }`
- shared style `rc-cards` — src/ui/roads/roads.css:218: `.rc-cards { display: flex; gap: 6px; overflow-x: auto; padding: 2px 2px 6px; scrollbar-width: thin; scrollbar-color: rgba(255,255,255,.18) transparent; }`
- shared style `rc-card` — src/ui/roads/roads.css:219: `.rc-card { position: relative; flex: 0 0 124px; border-radius: 9px; }`
- shared style `rc-card-main` — src/ui/roads/roads.css:220: `.rc-card-main { display: grid; gap: 3px; width: 124px; padding: 0; border: 0; background: none; text-align: left; }`
- shared style `rc-custom` — src/ui/roads/roads.css:227: `.rc-custom { position: absolute; top: 4px; right: 4px; display: grid; place-items: center; width: 24px; height: 24px; padding: 0; border-radius: 7px; border: 0;`
- shared style `rc-custom` — src/ui/roads/roads.css:228: `.rc-custom:hover { background: var(--v-accent, #4fe0bf); color: var(--v-accent-ink, #062520); }`
- shared style `rc-empty` — src/ui/roads/roads.css:229: `.rc-empty { padding: 10px 4px; color: var(--v-muted, #a3b4b0); max-width: 360px; }`
- shared style `rp-summary-actions` — src/ui/roads/roads.css:238: `.rp-summary-actions { display: flex; flex-wrap: wrap; gap: 6px; }`
- shared style `v2-icon` — src/ui/v2/shell.css:41: `.v2-icon > span, .v2-pill > span, .v2-seg-b > span, .v2-tabs.compact .v2-tab.icon:not(.on) > span, .v2-tabs.tight .v2-tab.icon > span, .v2-choice.icon > span, .`
- shared style `v2-seg-b` — src/ui/v2/shell.css:58: `.v2-seg-b, .v2-speed-b { position: relative; display: grid; place-items: center; min-width: 34px; height: 28px; padding: 0 6px; border: 0; border-radius: 999px;`
- shared style `v2-seg-b` — src/ui/v2/shell.css:62: `.v2-seg-b:hover, .v2-speed-b:hover { color: var(--v-text); }`
- shared style `v2-seg-b` — src/ui/v2/shell.css:63: `.v2-seg-b.on, .v2-speed-b.on { background: var(--v-accent); color: var(--v-accent-ink); }`
- shared style `v2-pill` — src/ui/v2/shell.css:69: `.v2-pill, .v2-icon { position: relative; display: inline-flex; align-items: center; justify-content: center; gap: 6px; width: 32px; height: 32px; padding: 0; fl`
- shared style `v2-pill` — src/ui/v2/shell.css:74: `.v2-pill:hover, .v2-icon:hover:not(:disabled) { background: var(--v-raise-2); border-color: var(--v-accent-line); }`
- shared style `v2-icon` — src/ui/v2/shell.css:75: `.v2-icon.on, .v2-pill.on { color: var(--v-accent); border-color: var(--v-accent); background: var(--v-accent-soft); }`

## Where the program plugs the feature in

### src/editor/nodeMover.ts

- `src/editor/nodeMover.ts#NodeMover` (class, line 26) ← construct from `src/main.ts#mover`
- `src/editor/nodeMover.ts#NodeMover.cancel` (method, line 71) ← call from `src/main.ts#cancelMove`
- `src/editor/nodeMover.ts#NodeMover.dragging` (getter, line 32) ← call from `src/main.ts#frame`, `src/main.ts#gestureInProgress`, `src/main.ts#growthAnswered`, `src/main.ts#growZones`
- `src/editor/nodeMover.ts#NodeMover.drop` (method, line 80) ← call from `src/main.ts#endPointer`
- `src/editor/nodeMover.ts#NodeMover.gesture` (method, line 37) ← call from `src/main.ts#currentGesture`
- `src/editor/nodeMover.ts#NodeMover.grab` (method, line 42) ← call from `src/main.ts#pointerDown`
- `src/editor/nodeMover.ts#NodeMover.move` (method, line 56) ← call from `src/main.ts#pointerMove`
- `src/editor/nodeMover.ts#NodeMover.previewDue` (method, line 64) ← call from `src/main.ts#frame`

### src/editor/roadParking.ts

- `src/editor/roadParking.ts#ROAD_PARKING_PRESETS` (variable, line 4) ← read from `src/main.ts#mountInterface`
- `src/editor/roadParking.ts#roadParking` (function, line 30) ← call from `src/main.ts#drawOverlayScreen`, `src/main.ts#pointerDown`, `src/main.ts#roadTool`
- `src/editor/roadParking.ts#roadParkingPreset` (function, line 16) ← reference from `src/main.ts#mountInterface`
- `src/editor/roadParking.ts#RoadParkingPreset` (type, line 5) ← typeRef from `src/main.ts#mountInterface`
- `src/editor/roadParking.ts#setRoadParkingPreset` (function, line 20) ← call from `src/main.ts#mountInterface`

### src/editor/roadPath.ts

- `src/editor/roadPath.ts#RoadPathPiece` (interface, line 12) ← read from `src/editor/commit.ts#commitRoadPath`, `src/editor/commit.ts#draftStations`, `src/editor/commit.ts#joinShortPieces`, `src/main.ts#drawOverlayScreen`
- `src/editor/roadPath.ts#RoadPathPiece` (interface, line 12) ← typeRef from `src/editor/commit.ts#commitRoadPath`, `src/editor/commit.ts#draftStations`, `src/editor/commit.ts#dryRunKey`, `src/editor/commit.ts#joinShortPieces`
- `src/editor/roadPath.ts#RoadPathPoint` (interface, line 6) ← read from `src/editor/commit.ts#commitRoadPath`, `src/editor/commit.ts#draftStations`, `src/editor/commit.ts#joinShortPieces`, `src/main.ts#drawOverlayScreen`

### src/editor/roads/economy.ts

- `src/editor/roads/economy.ts#roadsBefore` (function, line 11) ← call from `src/editor/bulldozer.ts#removeRoads`, `src/editor/commit.ts#commitRoadPath`, `src/editor/commit.ts#layCopy`, `src/editor/editRules.ts#guardRoadEdit`
- `src/editor/roads/economy.ts#settleRoadEdit` (function, line 16) ← call from `src/editor/bulldozer.ts#removeRoads`, `src/editor/commit.ts#commitRoadPath`, `src/editor/commit.ts#layCopy`, `src/editor/editRules.ts#guardRoadEdit`

### src/editor/roads/powerLine.ts

- `src/editor/roads/powerLine.ts#layPowerLines` (function, line 16) ← call from `src/editor/cityGenerator.ts#greenCity`

### src/editor/roads/profile.ts

- `src/editor/roads/profile.ts#applyProfileTo` (function, line 13) ← call from `src/main.ts#showInspector`
- `src/editor/roads/profile.ts#ApplyResult` (interface, line 8) ← read from `src/main.ts#showInspector`

### src/editor/roads/streetKit.ts

- `src/editor/roads/streetKit.ts#carryStreetKit` (function, line 73) ← call from `src/main.ts#pointerDown`, `src/main.ts#showInspector`
- `src/editor/roads/streetKit.ts#removeWithStreetKit` (function, line 62) ← call from `src/editor/bulldozer.ts#removeRoads`

### src/editor/roadTool.ts

- `src/editor/roadTool.ts#RoadDraft` (interface, line 44) ← read from `src/main.ts#drawOverlayScreen`
- `src/editor/roadTool.ts#RoadDraft` (interface, line 44) ← typeRef from `src/main.ts#drawOverlayScreen`
- `src/editor/roadTool.ts#RoadTool` (class, line 103) ← construct from `src/main.ts#roadTool`
- `src/editor/roadTool.ts#RoadTool.bend` (method, line 179) ← call from `src/main.ts#pointerDown`
- `src/editor/roadTool.ts#RoadTool.bending` (method, line 131) ← call from `src/main.ts#pointerMove`
- `src/editor/roadTool.ts#RoadTool.cancel` (method, line 139) ← call from `src/main.ts#afterRestore`, `src/main.ts#applySnapshot`, `src/main.ts#cancelGestures`, `src/main.ts#openSession`
- `src/editor/roadTool.ts#RoadTool.cost` (method, line 381) ← call from `src/main.ts#drawOverlayScreen`
- `src/editor/roadTool.ts#RoadTool.down` (method, line 198) ← call from `src/main.ts#pointerDown`
- `src/editor/roadTool.ts#RoadTool.draft` (field, line 105) ← read from `src/main.ts#drawOverlayScreen`, `src/main.ts#frame`
- `src/editor/roadTool.ts#RoadTool.dropCurve` (method, line 147) ← call from `src/main.ts#setAlignment`
- `src/editor/roadTool.ts#RoadTool.gesture` (method, line 122) ← call from `src/main.ts#currentGesture`
- `src/editor/roadTool.ts#RoadTool.heightChosen` (method, line 158) ← call from `src/main.ts#<IfStatement>@4036`
- `src/editor/roadTool.ts#RoadTool.hover` (method, line 258) ← call from `src/main.ts#pointerMove`
- `src/editor/roadTool.ts#RoadTool.inProgress` (method, line 135) ← call from `src/main.ts#gestureInProgress`, `src/main.ts#pointerWorld`
- `src/editor/roadTool.ts#RoadTool.move` (method, line 240) ← call from `src/main.ts#pointerMove`
- `src/editor/roadTool.ts#RoadTool.pieces` (method, line 534) ← call from `src/main.ts#drawOverlayScreen`
- `src/editor/roadTool.ts#RoadTool.pointerAt` (method, line 164) ← call from `src/main.ts#pointerDown`, `src/main.ts#pointerMove`
- `src/editor/roadTool.ts#RoadTool.preview` (method, line 517) ← call from `src/main.ts#drawOverlayScreen`
- `src/editor/roadTool.ts#RoadTool.preview` (method, line 517) ← read from `src/main.ts#drawOverlayScreen`
- `src/editor/roadTool.ts#RoadTool.reset` (method, line 152) ← call from `src/main.ts#newMap`, `src/main.ts#openCity`
- `src/editor/roadTool.ts#RoadTool.stations` (method, line 375) ← call from `src/main.ts#drawOverlayScreen`
- `src/editor/roadTool.ts#RoadTool.stepHeight` (method, line 307) ← call from `src/main.ts#gameKeys`, `src/main.ts#mountInterface`
- `src/editor/roadTool.ts#RoadTool.up` (method, line 274) ← call from `src/main.ts#endPointer`
- `src/editor/roadTool.ts#RoadTool.verdict` (method, line 356) ← call from `src/main.ts#drawOverlayScreen`

### src/editor/roadTop.ts

- `src/editor/roadTop.ts#RoadTop` (class, line 16) ← construct from `src/main.ts#roadTop`
- `src/editor/roadTop.ts#RoadTop.constructor` (constructor, line 24) ← implementation from `src/world/derived.ts#Derived.refresh`
- `src/editor/roadTop.ts#RoadTop.height` (method, line 46) ← call from `src/main.ts#surfaceTop`

### src/editor/roundabout.ts

- `src/editor/roundabout.ts#commitRoundabout` (function, line 39) ← call from `src/main.ts#pointerDown`
- `src/editor/roundabout.ts#roundaboutRefusal` (function, line 19) ← call from `src/main.ts#drawOverlayScreen`
- `src/editor/roundabout.ts#RoundaboutResult` (type, line 14) ← read from `src/main.ts#pointerDown`

### src/editor/snap.ts

- `src/editor/snap.ts#Anchor` (interface, line 16) ← read from `src/editor/blocks.ts#commitBlockGrid`, `src/editor/bulldozer.ts#Bulldozer.click`, `src/editor/commit.ts#commitDraftInPlace`, `src/editor/commit.ts#commitRoadPath`, …
- `src/editor/snap.ts#Anchor` (interface, line 16) ← typeRef from `src/editor/blocks.ts#commitBlockGrid`, `src/editor/bulldozer.ts#Bulldozer.click`, `src/editor/bulldozer.ts#Bulldozer.down`, `src/editor/bulldozer.ts#Bulldozer.drag`, …
- `src/editor/snap.ts#anchorForHeight` (function, line 420) ← call from `src/main.ts#pointerMove`
- `src/editor/snap.ts#findAnchor` (function, line 84) ← call from `src/editor/blocks.ts#commitBlockGrid`, `src/main.ts#<IfStatement>@4036`, `src/main.ts#pointerDown`, `src/main.ts#pointerMove`
- `src/editor/snap.ts#ribbonsNear` (function, line 74) ← call from `src/editor/barriers.ts#onCarriageway`, `src/editor/barriers.ts#snapBarrierPoint`, `src/editor/buildingSnap.ts#cornerFlush`, `src/editor/buildingSnap.ts#snapToRoad`, …
- `src/editor/snap.ts#roadSnap` (function, line 61) ← reference from `src/main.ts#mountInterface`
- `src/editor/snap.ts#setRoadSnap` (function, line 64) ← reference from `src/main.ts#mountInterface`
- `src/editor/snap.ts#SnapResult` (interface, line 25) ← read from `src/main.ts#drawOverlayScreen`

### src/editor/streetObjects.ts

- `src/editor/streetObjects.ts#commitPedestrianCrossing` (function, line 17) ← call from `src/main.ts#showInspector`
- `src/editor/streetObjects.ts#commitUturn` (function, line 72) ← call from `src/main.ts#showInspector`
- `src/editor/streetObjects.ts#PedestrianCrossingResult` (type, line 12) ← read from `src/main.ts#showInspector`
- `src/editor/streetObjects.ts#UturnResult` (type, line 67) ← read from `src/main.ts#showInspector`

### src/editor/streetscapeTool.ts

- `src/editor/streetscapeTool.ts#StreetscapeTool` (class, line 28) ← construct from `src/main.ts#streetscapeTool`
- `src/editor/streetscapeTool.ts#StreetscapeTool.down` (method, line 42) ← call from `src/main.ts#pointerDown`
- `src/editor/streetscapeTool.ts#StreetscapeTool.hover` (field, line 30) ← read from `src/main.ts#drawStreetscapeHover`
- `src/editor/streetscapeTool.ts#StreetscapeTool.move` (method, line 87) ← call from `src/main.ts#pointerMove`
- `src/editor/streetscapeTool.ts#StreetscapeTool.reach` (method, line 37) ← call from `src/main.ts#bulldozer`
- `src/editor/streetscapeTool.ts#StreetscapeTool.row` (field, line 32) ← read from `src/main.ts#drawStreetscapeHover`
- `src/editor/streetscapeTool.ts#StreetscapeTool.row` (field, line 32) ← write from `src/main.ts#endPointer`
- `src/editor/streetscapeTool.ts#StreetscapeTool.up` (method, line 100) ← call from `src/main.ts#endPointer`

### src/render/bake/roadRecipes.ts

- `src/render/bake/roadRecipes.ts#ASPHALT_TILE` (variable, line 6) ← read from `src/render/materials.ts#createMaterials`
- `src/render/bake/roadRecipes.ts#asphaltRecipe` (function, line 24) ← indirectCall from `src/render/mesh/textureBaker.ts#fbm`
- `src/render/bake/roadRecipes.ts#DECK_TILE` (variable, line 13) ← read from `src/render/materials.ts#createMaterials`
- `src/render/bake/roadRecipes.ts#deckRecipe` (function, line 182) ← indirectCall from `src/render/mesh/textureBaker.ts#fbm`
- `src/render/bake/roadRecipes.ts#FOOTWAY_TILE` (variable, line 10) ← read from `src/render/materials.ts#createMaterials`
- `src/render/bake/roadRecipes.ts#footwayRecipe` (function, line 76) ← indirectCall from `src/render/mesh/textureBaker.ts#fbm`
- `src/render/bake/roadRecipes.ts#KERB_TILE` (variable, line 11) ← read from `src/render/materials.ts#createMaterials`
- `src/render/bake/roadRecipes.ts#kerbRecipe` (function, line 126) ← indirectCall from `src/render/mesh/textureBaker.ts#fbm`
- `src/render/bake/roadRecipes.ts#VERGE_TILE` (variable, line 12) ← read from `src/render/materials.ts#createMaterials`
- `src/render/bake/roadRecipes.ts#vergeRecipe` (function, line 158) ← indirectCall from `src/render/mesh/textureBaker.ts#fbm`

### src/render/markings.ts

- `src/render/markings.ts#paintCatalogue` (function, line 10) ← call from `src/render/renderer.ts#createSceneRenderer`

### src/render/mesh/roadEdges.ts

- `src/render/mesh/roadEdges.ts#applyRoadEdges` (function, line 13) ← call from `src/render/materials.ts#createMaterials`

### src/render/mesh/roadLod.ts

- `src/render/mesh/roadLod.ts#BlockLevels` (interface, line 22) ← read from `src/lab/catalog/layout.ts#piecesOf`, `src/lab/catalog/layout.ts#shown`
- `src/render/mesh/roadLod.ts#levelsOf` (function, line 53) ← call from `src/lab/catalog/layout.ts#piecesOf`, `src/lab/catalog/layout.ts#shown`

### src/render/mesh/roadLodPure.ts

- `src/render/mesh/roadLodPure.ts#BlockArrays` (type, line 6) ← typeRef from `src/render/roadTile.worker.ts#Message`
- `src/render/mesh/roadLodPure.ts#blockBuffers` (function, line 16) ← call from `src/render/roadTile.worker.ts#handler`
- `src/render/mesh/roadLodPure.ts#coarseArrays` (function, line 29) ← call from `src/render/roadTile.worker.ts#handler`

### src/render/mesh/roadResidency.ts

- `src/render/mesh/roadResidency.ts#forgetRoadCells` (function, line 342) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/mesh/roadResidency.ts#isNear` (function, line 351) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/mesh/roadResidency.ts#RoadResidency` (interface, line 76) ← read from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.farShown`
- `src/render/mesh/roadResidency.ts#RoadResidency` (interface, line 76) ← typeRef from `src/render/renderer.ts#createSceneRenderer`
- `src/render/mesh/roadResidency.ts#RoadResidency` (interface, line 76) ← write from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.applyGeometryBudget`
- `src/render/mesh/roadResidency.ts#updateRoadLevels` (function, line 273) ← call from `src/render/renderer.ts#createSceneRenderer`

### src/render/mesh/surfaceMesh.ts

- `src/render/mesh/surfaceMesh.ts#disposeMesh` (function, line 40) ← call from `src/render/renderer.ts#createSceneRenderer.worldSteps`

### src/render/networkFarRoads.ts

- `src/render/networkFarRoads.ts#NetworkFarRoads` (class, line 97) ← construct from `src/render/renderer.ts#createSceneRenderer`
- `src/render/networkFarRoads.ts#NetworkFarRoads.cellKeys` (method, line 208) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/networkFarRoads.ts#NetworkFarRoads.clear` (method, line 264) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/networkFarRoads.ts#NetworkFarRoads.dispose` (method, line 274) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/networkFarRoads.ts#NetworkFarRoads.group` (field, line 98) ← read from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.switchedOff`
- `src/render/networkFarRoads.ts#NetworkFarRoads.hide` (method, line 234) ← call from `src/render/renderer.ts#createSceneRenderer.worldSteps`
- `src/render/networkFarRoads.ts#NetworkFarRoads.outdate` (method, line 229) ← call from `src/render/renderer.ts#createSceneRenderer.worldSteps`
- `src/render/networkFarRoads.ts#NetworkFarRoads.outdated` (method, line 224) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/networkFarRoads.ts#NetworkFarRoads.pending` (getter, line 123) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/networkFarRoads.ts#NetworkFarRoads.request` (method, line 137) ← call from `src/render/renderer.ts#createSceneRenderer.buildWorld`, `src/render/renderer.ts#createSceneRenderer.requestNetworkFar`
- `src/render/networkFarRoads.ts#NetworkFarRoads.settle` (method, line 246) ← call from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.worldSteps`
- `src/render/networkFarRoads.ts#NetworkFarRoads.settled` (getter, line 132) ← call from `src/render/renderer.ts#createSceneRenderer.worldSteps`

### src/render/networkFarRoadsBuild.ts

- `src/render/networkFarRoadsBuild.ts#buildFarRoadCells` (function, line 119) ← call from `src/render/networkFarRoads.worker.ts#serve`
- `src/render/networkFarRoadsBuild.ts#FarRoadCell` (interface, line 31) ← read from `src/render/networkFarRoads.worker.ts#serve`
- `src/render/networkFarRoadsBuild.ts#FarRoadInput` (interface, line 18) ← typeRef from `src/render/networkFarRoads.worker.ts#serve`

### src/render/platePaint.ts

- `src/render/platePaint.ts#plateDabs` (function, line 14) ← call from `src/render/flatAtlas.ts#createFlatAtlas.takePaint`
- `src/render/platePaint.ts#sameList` (function, line 6) ← call from `src/render/flatAtlas.ts#createFlatAtlas.takeStamps`

### src/render/roadProgramStandIns.ts

- `src/render/roadProgramStandIns.ts#roadProgramStandIns` (function, line 24) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/roadProgramStandIns.ts#roadProgramStandIns` (function, line 24) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/roadProgramStandIns.ts#roadProgramStandIns` (function, line 24) ← read from `src/render/renderer.ts#createSceneRenderer`
- `src/render/roadProgramStandIns.ts#roadProgramStandIns.dispose` (method, line 53) ← implementation from `src/render/renderer.ts#createSceneRenderer`

### src/render/roadsRead.ts

- `src/render/roadsRead.ts#RoadsRead` (class, line 2) ← construct from `src/render/renderer.ts#createSceneRenderer`
- `src/render/roadsRead.ts#RoadsRead.of` (method, line 5) ← call from `src/render/renderer.ts#createSceneRenderer.buildWorld`

### src/render/roadStandIn.ts

- `src/render/roadStandIn.ts#RoadStandIn` (class, line 64) ← construct from `src/render/renderer.ts#createSceneRenderer`
- `src/render/roadStandIn.ts#RoadStandIn.group` (field, line 65) ← read from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.switchedOff`
- `src/render/roadStandIn.ts#RoadStandIn.landed` (method, line 116) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/roadStandIn.ts#RoadStandIn.show` (method, line 77) ← call from `src/render/renderer.ts#createSceneRenderer`

### src/render/roadStations.ts

- `src/render/roadStations.ts#RoadStations` (class, line 33) ← construct from `src/render/renderer.ts#createSceneRenderer`
- `src/render/roadStations.ts#RoadStations.current` (getter, line 75) ← call from `src/render/renderer.ts#createSceneRenderer.buildWorld`, `src/render/renderer.ts#createSceneRenderer.completeSolve`
- `src/render/roadStations.ts#RoadStations.used` (method, line 80) ← call from `src/render/renderer.ts#createSceneRenderer.buildWorld`, `src/render/renderer.ts#createSceneRenderer.completeSolve`
- `src/render/roadStations.ts#RoadStations.waiting` (method, line 43) ← call from `src/render/renderer.ts#createSceneRenderer.stationsAsked`
- `src/render/roadStations.ts#RoadStations.waiting` (method, line 43) ← callbackAssign from `src/jobs/jobPool.ts#dispatch`, `src/jobs/jobPool.ts#JobQueue.answer`, `src/jobs/jobPool.ts#JobQueue.fail`
- `src/render/roadStations.ts#RoadStations.waiting` (method, line 43) ← implementation from `src/jobs/jobPool.ts#dispatch`, `src/jobs/jobPool.ts#JobQueue.answer`, `src/jobs/jobPool.ts#JobQueue.fail`

### src/render/roadSurfaceEntries.ts

- `src/render/roadSurfaceEntries.ts#ROAD_INPUT_RESIDENCY` (variable, line 140) ← read from `src/render/renderer.ts#createSceneRenderer`
- `src/render/roadSurfaceEntries.ts#TILE` (variable, line 12) ← read from `src/render/renderer.ts#createSceneRenderer.buildWorld`, `src/render/renderer.ts#FAR_ROAD_CELL`
- `src/render/roadSurfaceEntries.ts#TILE_ORIGIN` (variable, line 14) ← read from `src/render/renderer.ts#createSceneRenderer.buildWorld`, `src/render/renderer.ts#createSceneRenderer.farCellOf`, `src/render/renderer.ts#createSceneRenderer.farCellRect`, `src/render/renderer.ts#createSceneRenderer.requestNetworkFar`

### src/render/roadSurfaceInputs.ts

- `src/render/roadSurfaceInputs.ts#SurfaceInputs` (class, line 42) ← construct from `src/render/renderer.ts#createSceneRenderer`
- `src/render/roadSurfaceInputs.ts#SurfaceInputs.constructor` (constructor, line 45) ← implementation from `src/world/viewCells.ts#ViewCells.drop`, `src/world/viewCells.ts#ViewCells.make`

### src/render/roadSurfaceReads.ts

- `src/render/roadSurfaceReads.ts#TERRAIN_UV` (variable, line 24) ← read from `src/render/structures.ts#structureParts`

### src/render/roadSurfaces.ts

- `src/render/roadSurfaces.ts#buildRoadSurfaces` (function, line 267) ← call from `src/render/renderer.ts#createSceneRenderer.warmRoadShaders`
- `src/render/roadSurfaces.ts#disposeSurfaceReuse` (function, line 100) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/roadSurfaces.ts#RoadSurfaces` (interface, line 202) ← call from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.warmRoadShaders`, `src/render/renderer.ts#createSceneRenderer.worldSteps`
- `src/render/roadSurfaces.ts#RoadSurfaces` (interface, line 202) ← read from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.switchedOff`, `src/render/renderer.ts#createSceneRenderer.warmRoadShaders`, `src/render/renderer.ts#createSceneRenderer.watchRoads`, …
- `src/render/roadSurfaces.ts#RoadSurfaces` (interface, line 202) ← typeRef from `src/render/renderer.ts#createSceneRenderer`
- `src/render/roadSurfaces.ts#roadSurfaceSteps` (function, line 283) ← call from `src/render/renderer.ts#createSceneRenderer.worldSteps`
- `src/render/roadSurfaces.ts#roadSurfaceSteps.adopt` (method, line 784) ← implementation from `src/render/renderer.ts#createSceneRenderer.worldSteps`
- `src/render/roadSurfaces.ts#roadSurfaceSteps.dispose` (method, line 788) ← implementation from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.warmRoadShaders`, `src/render/renderer.ts#createSceneRenderer.worldSteps`
- `src/render/roadSurfaces.ts#SurfaceReuse` (interface, line 45) ← read from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.fillStep`, `src/render/renderer.ts#createSceneRenderer.takeEditedCells`, `src/render/renderer.ts#createSceneRenderer.worldSteps`
- `src/render/roadSurfaces.ts#SurfaceReuse` (interface, line 45) ← typeRef from `src/render/renderer.ts#createSceneRenderer`
- `src/render/roadSurfaces.ts#SurfaceReuse` (interface, line 45) ← write from `src/render/renderer.ts#createSceneRenderer.fillStep`, `src/render/renderer.ts#createSceneRenderer.takeEditedCells`, `src/render/renderer.ts#createSceneRenderer.worldSteps`
- `src/render/roadSurfaces.ts#TileBundle` (type, line 118) ← typeRef from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.fetchTiles`

### src/render/roadTileMesh.ts

- `src/render/roadTileMesh.ts#MeshAsk` (interface, line 28) ← typeRef from `src/render/roadTile.worker.ts#Message`
- `src/render/roadTileMesh.ts#tileBuffers` (function, line 162) ← call from `src/render/roadTile.worker.ts#handler`
- `src/render/roadTileMesh.ts#TileMesher` (class, line 77) ← construct from `src/render/roadTile.worker.ts#handler`
- `src/render/roadTileMesh.ts#TileMesher` (class, line 77) ← typeRef from `src/render/roadTile.worker.ts#handler`
- `src/render/roadTileMesh.ts#TileMesher.give` (method, line 115) ← call from `src/render/roadTile.worker.ts#handler`
- `src/render/roadTileMesh.ts#TileMesher.need` (method, line 96) ← call from `src/render/roadTile.worker.ts#handler`
- `src/render/roadTileMesh.ts#TileMesher.run` (method, line 130) ← call from `src/render/roadTile.worker.ts#handler`
- `src/render/roadTileMesh.ts#TileMesher.run` (method, line 130) ← read from `src/render/roadTile.worker.ts#handler`
- `src/render/roadTileMesh.ts#TileMeshes` (type, line 35) ← typeRef from `src/render/roadTile.worker.ts#keepTiles`

### src/render/roadTilePool.ts

- `src/render/roadTilePool.ts#RoadTilePool` (class, line 51) ← construct from `src/render/renderer.ts#createSceneRenderer`
- `src/render/roadTilePool.ts#RoadTilePool.dispose` (method, line 191) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/roadTilePool.ts#RoadTilePool.unseen` (getter, line 56) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/roadTilePool.ts#RoadTilePool.unseen@60` (setter, line 60) ← call from `src/render/renderer.ts#createSceneRenderer`

### src/render/streetFurnitureLayer.ts

- `src/render/streetFurnitureLayer.ts#createStreetFurniture` (function, line 400) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/streetFurnitureLayer.ts#createStreetFurniture` (function, line 400) ← implementation from `src/render/propCells.ts#createPropLayer`, `src/render/propCells.ts#createPropLayer.batches.make`
- `src/render/streetFurnitureLayer.ts#createStreetFurniture.cull` (method, line 460) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/streetFurnitureLayer.ts#createStreetFurniture.dispose` (method, line 464) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/streetFurnitureLayer.ts#createStreetFurniture.setMap` (method, line 454) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/streetFurnitureLayer.ts#createStreetFurniture.sync` (method, line 430) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/streetFurnitureLayer.ts#createStreetFurniture.warm` (method, line 457) ← implementation from `src/render/renderer.ts#createSceneRenderer.warmRoadShaders`
- `src/render/streetFurnitureLayer.ts#furnitureStores.plant` (function, line 126) ← callbackAssign from `src/render/cellInstances.ts#batchMesh`, `src/render/cellInstances.ts#createCellInstances.cull`, `src/render/cellInstances.ts#createCellInstances.makeMesh`
- `src/render/streetFurnitureLayer.ts#furnitureStores.plant` (function, line 126) ← implementation from `src/render/cellInstances.ts#batchMesh`, `src/render/cellInstances.ts#createCellInstances.cull`, `src/render/cellInstances.ts#createCellInstances.makeMesh`
- `src/render/streetFurnitureLayer.ts#PlacedProduct.allKeys` (method, line 263) ← implementation from `src/world/derived.ts#Derived.refresh`
- `src/render/streetFurnitureLayer.ts#PlacedProduct.changed` (method, line 295) ← implementation from `src/world/derived.ts#Derived.refresh`
- `src/render/streetFurnitureLayer.ts#PlacedProduct.compute` (method, line 290) ← implementation from `src/world/derived.ts#Derived.refresh`
- `src/render/streetFurnitureLayer.ts#PlacedProduct.dirty` (method, line 270) ← implementation from `src/world/derived.ts#Derived.refresh`
- `src/render/streetFurnitureLayer.ts#StreetFurnitureLayer` (interface, line 358) ← call from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.warmRoadShaders`
- `src/render/streetFurnitureLayer.ts#StreetFurnitureLayer` (interface, line 358) ← read from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.switchedOff`

### src/render/surfaceBakeClient.ts

- `src/render/surfaceBakeClient.ts#primeSurfaceBake` (function, line 114) ← call from `src/main.ts#<ExpressionStatement>@657`
- `src/render/surfaceBakeClient.ts#startSurfaceBake` (function, line 103) ← call from `src/main.ts#surfaceBake`

### src/render/surfaceTextures.ts

- `src/render/surfaceTextures.ts#disposeSurfaceTextures` (function, line 100) ← call from `src/render/materials.ts#createMaterials.dispose`
- `src/render/surfaceTextures.ts#requireSurfaceMaps` (function, line 92) ← call from `src/render/materials.ts#createMaterials`
- `src/render/surfaceTextures.ts#SurfaceBake` (interface, line 5) ← read from `src/render/buildings/finishes.ts#shellShader`, `src/render/materials.ts#createMaterials`, `src/render/materials.ts#createMaterials.surface`
- `src/render/surfaceTextures.ts#SurfaceBake` (interface, line 5) ← typeRef from `src/render/buildings/finishes.ts#shellShader`, `src/render/materials.ts#createMaterials`, `src/render/materials.ts#createMaterials.surface`
- `src/render/surfaceTextures.ts#surfaceMaps` (function, line 84) ← call from `src/render/buildings/finishes.ts#createShellMaterial`
- `src/render/surfaceTextures.ts#surfaceTexture` (function, line 77) ← call from `src/render/materials.ts#createMaterials`, `src/render/mesh/detailLayer.ts#detailTextures`, `src/render/terrain.ts#terrainBakes`
- `src/render/surfaceTextures.ts#wantAllTexels` (function, line 61) ← call from `src/render/renderer.ts#createSceneRenderer`

### src/ui/roads/catalogPanel.ts

- `src/ui/roads/catalogPanel.ts#catalogStrip` (function, line 65) ← call from `src/ui/v2/shell.ts#mountShell.renderRoads`

### src/ui/roads/connectorPanel.ts

- `src/ui/roads/connectorPanel.ts#ConnectorPanelHost` (interface, line 7) ← typeRef from `src/ui/inspector.ts#renderNode`
- `src/ui/roads/connectorPanel.ts#mountConnectorPanel` (function, line 25) ← call from `src/ui/inspector.ts#renderNode`
- `src/ui/roads/connectorPanel.ts#refreshConnectorPanel` (function, line 35) ← call from `src/ui/inspector.ts#renderCurrent`

### src/ui/roads/cutWalls.ts

- `src/ui/roads/cutWalls.ts#cutWallsChosen` (function, line 9) ← call from `src/main.ts#buildSummary`, `src/main.ts#roadTool`, `src/ui/v2/shell.ts#mountShell.renderRoads`
- `src/ui/roads/cutWalls.ts#setCutWalls` (function, line 13) ← call from `src/ui/v2/shell.ts#mountShell.renderRoads`

### src/ui/roads/drawProfile.ts

- `src/ui/roads/drawProfile.ts#drawProfile` (function, line 12) ← call from `src/main.ts#drawOverlayScreen`, `src/main.ts#roadTool`, `src/ui/v2/shell.ts#mountShell.renderRoads`
- `src/ui/roads/drawProfile.ts#DrawProfile` (interface, line 4) ← read from `src/main.ts#drawOverlayScreen`, `src/main.ts#showInspector`
- `src/ui/roads/drawProfile.ts#DrawProfile` (interface, line 4) ← typeRef from `src/ui/inspector.ts#InspectorActions`, `src/ui/inspector.ts#renderSegment`
- `src/ui/roads/drawProfile.ts#setDrawProfile` (function, line 16) ← call from `src/main.ts#selectRoadType`, `src/main.ts#setLanes`, `src/main.ts#showInspector`

### src/ui/roads/furnitureChoice.ts

- `src/ui/roads/furnitureChoice.ts#furnitureChosen` (function, line 14) ← call from `src/main.ts#roadTool`, `src/ui/v2/shell.ts#mountShell.renderRoads`
- `src/ui/roads/furnitureChoice.ts#setFurnitureChosen` (function, line 18) ← call from `src/ui/v2/shell.ts#mountShell.renderRoads`

### src/ui/roads/junctionPanel.ts

- `src/ui/roads/junctionPanel.ts#mountJunctionPanel` (function, line 49) ← call from `src/ui/inspector.ts#renderNode`
- `src/ui/roads/junctionPanel.ts#mountJunctionPanel` (function, line 49) ← typeRef from `src/ui/inspector.ts#junctionHost`
- `src/ui/roads/junctionPanel.ts#refreshJunctionFlows` (function, line 208) ← call from `src/ui/inspector.ts#renderCurrent`

### src/ui/roads/keys.ts

- `src/ui/roads/keys.ts#keyLabel` (function, line 97) ← call from `src/ui/v2/shell.ts#mountShell.renderRoads`
- `src/ui/roads/keys.ts#onRoadKeysChange` (function, line 91) ← call from `src/main.ts#<ExpressionStatement>@1611`, `src/ui/v2/shell.ts#mountShell`
- `src/ui/roads/keys.ts#roadKey` (function, line 58) ← call from `src/ui/v2/shell.ts#mountShell.renderRoads`
- `src/ui/roads/keys.ts#roadKeyAction` (function, line 86) ← call from `src/main.ts#gameKeys`
- `src/ui/roads/keys.ts#roadKeyParams` (function, line 105) ← reference from `src/main.ts#<ExpressionStatement>`

### src/ui/roads/keysPanel.ts

- `src/ui/roads/keysPanel.ts#closeRoadKeys` (function, line 9) ← call from `src/ui/v2/shell.ts#mountShell.renderRoads`
- `src/ui/roads/keysPanel.ts#openRoadKeys` (function, line 17) ← call from `src/ui/v2/shell.ts#mountShell.renderRoads`
- `src/ui/roads/keysPanel.ts#roadKeysOpen` (function, line 15) ← call from `src/ui/v2/shell.ts#mountShell.renderRoads`

### src/ui/roads/money.ts

- `src/ui/roads/money.ts#balanceTip` (function, line 16) ← call from `src/ui/v2/shell.ts#mountShell.syncMoney`
- `src/ui/roads/money.ts#formatCost` (function, line 11) ← call from `src/main.ts#drawOverlayScreen`
- `src/ui/roads/money.ts#formatMoney` (function, line 5) ← call from `src/ui/v2/shell.ts#mountShell.syncMoney`

### src/ui/roads/profilePanel.ts

- `src/ui/roads/profilePanel.ts#mountProfilePanel` (function, line 10) ← call from `src/ui/inspector.ts#renderSegment`

### src/ui/roadSectionEditor.ts

- `src/ui/roadSectionEditor.ts#freeRoadsEnabled` (function, line 10) ← call from `src/main.ts#showInspector`, `src/ui/inspector.ts#renderSegment`
- `src/ui/roadSectionEditor.ts#mountRoadSectionEditor` (function, line 15) ← call from `src/ui/inspector.ts#renderSegment`

### src/ui/roadSwatch.ts

- `src/ui/roadSwatch.ts#roadSwatch` (function, line 28) ← call from `src/main.ts#roadClasses`

### src/world/carriageway.ts

- `src/world/carriageway.ts#onCarriageway` (function, line 9) ← call from `src/editor/bulldozer.ts#Bulldozer.click`, `src/editor/transitTools.ts#TransitTool.onRoad`, `src/render/renderer.ts#createSceneRenderer`
- `src/world/carriageway.ts#onRoadGround` (function, line 27) ← call from `src/render/renderer.ts#createSceneRenderer.coverFree`, `src/render/renderer.ts#createSceneRenderer.natureOpen`

### src/world/cityGen/plan.ts

- `src/world/cityGen/plan.ts#CityOptions` (interface, line 41) ← read from `src/editor/smallCityMap.ts#growCity`
- `src/world/cityGen/plan.ts#CityOptions` (interface, line 41) ← typeRef from `src/world/cityGen/wholeMap.ts#WholeMapOptions`
- `src/world/cityGen/plan.ts#CityPlan` (interface, line 61) ← call from `src/editor/cityGenerator.ts#zoneCity`
- `src/world/cityGen/plan.ts#CityPlan` (interface, line 61) ← read from `src/editor/cityGenerator.ts#layCity`, `src/editor/cityGenerator.ts#zoneCity`, `src/editor/smallCityMap.ts#growCity`
- `src/world/cityGen/plan.ts#CityPlan` (interface, line 61) ← typeRef from `src/editor/cityGenerator.ts#layCity`, `src/editor/cityGenerator.ts#zoneCity`, `src/editor/smallCityMap.ts#buildCityMap`, `src/editor/smallCityMap.ts#buildCityTable`, …
- `src/world/cityGen/plan.ts#DEFAULT_CITY` (variable, line 56) ← read from `src/world/cityGen/largeCity.ts#largeCityPlan`, `src/world/cityGen/smallCity.ts#smallCityPlan`
- `src/world/cityGen/plan.ts#LEVEL_ROAD` (variable, line 59) ← read from `src/editor/cityGenerator.ts#layCity`
- `src/world/cityGen/plan.ts#planCity` (function, line 75) ← call from `src/world/cityGen/largeCity.ts#largeCityPlan`, `src/world/cityGen/smallCity.ts#smallCityPlan`
- `src/world/cityGen/plan.ts#planCity.zoneAt` (function, line 137) ← implementation from `src/editor/cityGenerator.ts#zoneCity`

### src/world/cityGen/streets.ts

- `src/world/cityGen/streets.ts#StreetEdge` (interface, line 262) ← read from `src/editor/cityGenerator.ts#layCity`
- `src/world/cityGen/streets.ts#StreetGraph` (interface, line 270) ← read from `src/editor/cityGenerator.ts#layCity`

### src/world/curbRamps.ts

- `src/world/curbRamps.ts#walkingRise` (function, line 174) ← call from `src/render/agents.ts#createAgentMeshes.sync`

### src/world/elevation.ts

- `src/world/elevation.ts#buildRoadElevation` (function, line 279) ← call from `src/editor/commit.ts#solveOver`, `src/editor/editRules.ts#flatSolves.after`, `src/editor/editRules.ts#flatSolves.beforeSolve`, `src/editor/smallCityMap.ts#growLots`, …
- `src/world/elevation.ts#buildRoadElevation` (function, line 279) ← implementation from `src/editor/commit.ts#boreDeepCuts`, `src/editor/editRules.ts#grade`, `src/editor/editRules.ts#refuseRoadEdit.deck`, `src/editor/smallCityMap.ts#growLots.pavedAt`, …
- `src/world/elevation.ts#CUT_WALL_REACH` (variable, line 222) ← read from `src/render/structures.ts#retainingWalls`
- `src/world/elevation.ts#FLAT_GROUND` (function, line 951) ← read from `src/editor/editRules.ts#flatSolves.after`, `src/editor/editRules.ts#flatSolves.beforeSolve`, `src/editor/smallCityMap.ts#growLots`, `src/render/renderer.ts#createSceneRenderer.buildWorld`, …
- `src/world/elevation.ts#GROUND_ONLY` (variable, line 208) ← read from `src/editor/smallCityMap.ts#growLots.pavedAt`, `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#SURFACE_SETS`, `src/render/utilities.ts#poleGroundAt`
- `src/world/elevation.ts#MAX_AUTHORED_GRADE` (variable, line 68) ← read from `src/editor/commit.ts#commitRoadPath`, `src/main.ts#drawOverlayScreen`, `src/main.ts#setNodeHeightMetres`
- `src/world/elevation.ts#pendingOrder` (function, line 196) ← call from `src/render/renderer.ts#createSceneRenderer.completeSolve`
- `src/world/elevation.ts#RoadElevation` (interface, line 135) ← call from `src/editor/smallCityMap.ts#growLots.pavedAt`, `src/render/groundShape.ts#blockDigest`, `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.compareSolves`, …
- `src/world/elevation.ts#RoadElevation` (interface, line 135) ← read from `src/render/groundShape.ts#blockDigest`, `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.buildWorld`, `src/render/renderer.ts#createSceneRenderer.compareSolves`, …
- `src/world/elevation.ts#RoadElevation` (interface, line 135) ← reference from `src/render/groundShape.ts#shapeGroundAsk`, `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.compareSolves`
- `src/world/elevation.ts#RoadElevation` (interface, line 135) ← typeRef from `src/editor/commit.ts#boreDeepCuts`, `src/editor/commit.ts#commitRoadPath`, `src/editor/commit.ts#DraftResult`, `src/editor/commit.ts#draftStations`, …
- `src/world/elevation.ts#RoadShapeCut` (interface, line 840) ← typeRef from `src/render/groundShape.ts#BlockDigestAsk`, `src/render/groundShape.ts#GroundShapeAsk`
- `src/world/elevation.ts#roadShaper` (function, line 888) ← call from `src/render/groundShape.ts#digestBlocks`, `src/render/groundShape.ts#shapeGroundAsk`
- `src/world/elevation.ts#roadShaper` (function, line 888) ← implementation from `src/render/groundShape.ts#blockDigest`, `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.deckHeight`, `src/render/utilities.ts#poleGroundAt`
- `src/world/elevation.ts#roadShaper.shapeAt` (method, line 894) ← implementation from `src/render/groundShape.ts#blockDigest`, `src/render/groundShape.ts#shapeGroundAsk`
- `src/world/elevation.ts#roadStations` (function, line 240) ← call from `src/render/roadStations.worker.ts#serve`
- `src/world/elevation.ts#roadsToRead` (function, line 248) ← call from `src/render/renderer.ts#createSceneRenderer.buildWorld`, `src/render/renderer.ts#createSceneRenderer.completeSolve`
- `src/world/elevation.ts#roadsToRead` (function, line 248) ← typeRef from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.stationsAsked`
- `src/world/elevation.ts#SolveScope` (interface, line 169) ← read from `src/render/renderer.ts#createSceneRenderer.buildWorld`
- `src/world/elevation.ts#SolveScope` (interface, line 169) ← typeRef from `src/render/renderer.ts#createSceneRenderer.buildWorld`, `src/render/renderer.ts#createSceneRenderer.completeSolve`
- `src/world/elevation.ts#SURFACE_READ_REACH` (variable, line 88) ← read from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.takeEditedCells`

### src/world/gullies.ts

- `src/world/gullies.ts#DEFAULT_GULLY_AUTO` (variable, line 16) ← read from `src/world/doc.ts#RoadDoc.gullyAuto`, `src/world/doc.ts#RoadDoc.setGullyAuto`, `src/world/doc.ts#RoadDoc.worldPart`
- `src/world/gullies.ts#GullyDab` (interface, line 3) ← typeRef from `src/render/terrain.ts#TerrainSurface`, `src/world/doc.ts#RoadDoc.addGullyDab`, `src/world/doc.ts#RoadDoc.gullyDabs`
- `src/world/gullies.ts#MAX_GULLY_DABS` (variable, line 14) ← read from `src/world/doc.ts#TILE_CAPS`
- `src/world/gullies.ts#readGullyDab` (function, line 18) ← call from `src/world/doc.ts#RoadDoc.fromJSON`

### src/world/junction/build.ts

- `src/world/junction/build.ts#surfaceMode` (function, line 48) ← call from `src/ui/inspector.ts#renderNode`

### src/world/junction/bulb.ts

- `src/world/junction/bulb.ts#bulbClearing` (function, line 94) ← call from `src/main.ts#clearBulbs`
- `src/world/junction/bulb.ts#bulbLine` (function, line 81) ← call from `src/world/walkways.ts#Walkways.buildNode`

### src/world/junction/levels.ts

- `src/world/junction/levels.ts#JunctionLevels.get` (method, line 256) ← call from `src/editor/growthFeed.ts#junctionPiece`, `src/world/walkways.ts#cornersOf`

### src/world/junction/types.ts

- `src/world/junction/types.ts#Corner` (interface, line 43) ← read from `src/world/walkways.ts#cornersOf`
- `src/world/junction/types.ts#Junction` (interface, line 60) ← read from `src/editor/growthFeed.ts#junctionPiece`, `src/sim/peds/sidewalk.ts#clearOfJunction`, `src/world/walkways.ts#cornersOf`
- `src/world/junction/types.ts#Junction` (interface, line 60) ← typeRef from `src/world/lots.ts#pavingNear`, `src/world/poleLines.ts#junctionRings`, `src/world/poleLines.ts#poleLinesOf`
- `src/world/junction/types.ts#Leg` (interface, line 14) ← read from `src/world/walkways.ts#cornersOf`

### src/world/junctionBuild.ts

- `src/world/junctionBuild.ts#buildJunctionOf` (function, line 275) ← call from `src/conflictTable.worker.ts#serve`

### src/world/junctionTurns.ts

- `src/world/junctionTurns.ts#TurnPlan` (class, line 8) ← construct from `src/sim/world.ts#SimWorld.prepareVehicleTopology`
- `src/world/junctionTurns.ts#TurnPlan.asks` (field, line 10) ← read from `src/sim/world.ts#SimWorld.prepareVehicleTopology`
- `src/world/junctionTurns.ts#TurnPlan.chosen` (field, line 13) ← read from `src/sim/world.ts#SimWorld.prepareVehicleTopology`
- `src/world/junctionTurns.ts#TurnPlan.chosen` (field, line 13) ← write from `src/sim/world.ts#SimWorld.prepareVehicleTopology`
- `src/world/junctionTurns.ts#TurnPlan.recording` (field, line 9) ← write from `src/sim/world.ts#SimWorld.prepareVehicleTopology`
- `src/world/junctionTurns.ts#TurnPlan.surfaces` (field, line 12) ← read from `src/sim/world.ts#SimWorld.prepareVehicleTopology`

### src/world/lanelets.ts

- `src/world/lanelets.ts#Connector` (interface, line 62) ← read from `src/sim/crossings/permission.ts#crossingReservedByVehicle`, `src/sim/crossings/permission.ts#indexReservations`, `src/sim/crossings/permission.ts#turnOwedToVehicle`, `src/sim/drive/tactical.ts#connectorCost`, …
- `src/world/lanelets.ts#Connector` (interface, line 62) ← typeRef from `src/sim/intersections/admission.ts#admissionProbe`, `src/sim/intersections/admission.ts#comesFrom`, `src/sim/intersections/admission.ts#compactReservationChain`, `src/sim/intersections/admission.ts#ConnectorReservation`, …
- `src/world/lanelets.ts#ConnectorId` (type, line 28) ← typeRef from `src/sim/intersections/claims.ts#Claim`, `src/sim/intersections/claims.ts#ClaimTable.available`, `src/sim/intersections/claims.ts#ClaimTable.grantAll`, `src/sim/intersections/claims.ts#ClaimTable.releaseConnector`, …
- `src/world/lanelets.ts#GraphChange` (interface, line 131) ← read from `src/sim/agents/kerbBays.ts#followKerbs`, `src/sim/agents/laneGrid.ts#LaneGrid.follow`, `src/sim/intersections/cycles.ts#CycleIndex.follow`, `src/sim/peds/sidewalk.ts#SidewalkGraph.update`, …
- `src/world/lanelets.ts#GraphChange` (interface, line 131) ← typeRef from `src/sim/agents/kerbBays.ts#KerbWorld`, `src/sim/agents/laneGrid.ts#LaneGrid.follow`, `src/sim/agents/laneGrid.ts#LaneGridWorld`, `src/sim/intersections/cycles.ts#CycleIndex.follow`, …
- `src/world/lanelets.ts#JunctionTopology` (interface, line 78) ← read from `src/sim/crossings/permission.ts#mayEnterCrossing`, `src/sim/crossings/permission.ts#turnOwedToVehicle`, `src/sim/host/simAnswers.ts#junctionAnswer`, `src/sim/host/simHost.ts#SimHost.signals.layoutOf`, …
- `src/world/lanelets.ts#JunctionTopology` (interface, line 78) ← typeRef from `src/sim/intersections/admission.ts#mainRoadRank`, `src/sim/intersections/admission.ts#mainRoads`, `src/sim/signals/fsm.ts#createController`, `src/sim/signals/fsm.ts#rebuildController`, …
- `src/world/lanelets.ts#Lanelet` (interface, line 31) ← read from `src/sim/agents/laneGrid.ts#boxOf`, `src/sim/agents/laneGrid.ts#byId`, `src/sim/agents/laneGrid.ts#digestOf`, `src/sim/agents/laneGrid.ts#LaneGrid.digest`, …
- `src/world/lanelets.ts#Lanelet` (interface, line 31) ← typeRef from `src/sim/agents/laneGrid.ts#boxOf`, `src/sim/agents/laneGrid.ts#byId`, `src/sim/agents/laneGrid.ts#digestOf`, `src/sim/agents/laneGrid.ts#Filed`, …
- `src/world/lanelets.ts#LaneletGraph` (class, line 137) ← construct from `src/sim/world.ts#SimWorld.graph`, `src/sim/world.ts#SimWorld.prepareVehicleTopology`
- `src/world/lanelets.ts#LaneletGraph` (class, line 137) ← typeRef from `src/sim/agents/laneGrid.ts#LaneGrid.fileAll`, `src/sim/agents/laneGrid.ts#LaneGrid.follow`, `src/sim/agents/laneGrid.ts#LaneGridWorld`, `src/sim/intersections/cycles.ts#CycleIndex.follow`, …
- `src/world/lanelets.ts#LaneletGraph.adopt` (method, line 216) ← call from `src/sim/world.ts#SimWorld.rebuildVehicleTopology`
- `src/world/lanelets.ts#LaneletGraph.changeTargets` (method, line 766) ← call from `src/sim/drive/tactical.ts#planTrip`, `src/sim/routing/router.ts#planFrom`, `src/sim/vehicles/laneChange.ts#discretionary`, `src/sim/vehicles/laneChange.ts#mandatory`, …
- `src/world/lanelets.ts#LaneletGraph.connectors` (field, line 139) ← read from `src/sim/drive/tactical.ts#planTrip`, `src/sim/intersections/crossingSpans.ts#CrossingSpans.measureJunction`, `src/sim/roads/greenWave.ts#greenWave`, `src/sim/routing/destination.ts#routeToDestination`, …
- `src/world/lanelets.ts#LaneletGraph.current` (method, line 256) ← call from `src/sim/world.ts#SimWorld.rebuildVehicleTopology`
- `src/world/lanelets.ts#LaneletGraph.exitsOf` (method, line 742) ← call from `src/sim/drive/tactical.ts#planTrip`, `src/sim/intersections/admission.ts#compactReservationChain`, `src/sim/intersections/cycles.ts#CycleIndex.search`, `src/sim/intersections/cycles.ts#CycleIndex.search.next`, …
- `src/world/lanelets.ts#LaneletGraph.forget` (method, line 175) ← call from `src/sim/world.ts#SimWorld.reset`
- `src/world/lanelets.ts#LaneletGraph.inbound` (field, line 144) ← read from `src/sim/routing/laneReach.ts#BodyReach.follow`, `src/sim/routing/router.ts#TrapIndex.follow`
- `src/world/lanelets.ts#LaneletGraph.junctions` (field, line 140) ← read from `src/sim/crossings/permission.ts#mayEnterCrossing`, `src/sim/crossings/permission.ts#turnOwedToVehicle`, `src/sim/host/simAnswers.ts#junctionAnswer`, `src/sim/host/simHost.ts#SimHost.signals.layoutOf`, …
- `src/world/lanelets.ts#LaneletGraph.junctions` (field, line 140) ← write from `src/sim/world.ts#SimWorld.applyAutoControl`
- `src/world/lanelets.ts#LaneletGraph.lanelet` (method, line 746) ← call from `src/sim/agents/parking.ts#kerbReads.linksKey`, `src/sim/host/simAnswers.ts#count`, `src/sim/host/simAnswers.ts#roadLanes`, `src/world/conflictPoints.ts#ConflictIndex.prepare`, …
- `src/world/lanelets.ts#LaneletGraph.lanelets` (field, line 138) ← read from `src/sim/agents/laneGrid.ts#LaneGrid.fileAll`, `src/sim/agents/laneGrid.ts#LaneGrid.follow`, `src/sim/ambient/ambient.ts#AmbientWorld.listLanes`, `src/sim/drive/tactical.ts#planTrip`, …
- `src/world/lanelets.ts#LaneletGraph.laneUsable` (method, line 760) ← call from `src/sim/drive/tactical.ts#connectorCost`, `src/sim/routing/router.ts#chooseExit`, `src/sim/routing/router.ts#planFrom`, `src/sim/vehicles/spawn.ts#spawnAt`, …
- `src/world/lanelets.ts#LaneletGraph.linksOfSegment` (method, line 738) ← call from `src/sim/agents/parking.ts#kerbReads.linksKey`, `src/sim/peds/sidewalk.ts#SidewalkGraph.crossingsAt`
- `src/world/lanelets.ts#LaneletGraph.outbound` (field, line 146) ← read from `src/sim/vehicles/spawn.ts#entryLanes`
- `src/world/lanelets.ts#LaneletGraph.redoProvisional` (method, line 357) ← call from `src/sim/world.ts#SimWorld.prepareVehicleTopology`
- `src/world/lanelets.ts#LaneletGraph.revision` (field, line 150) ← read from `src/sim/agents/lorryDocks.ts#yardKey`
- `src/world/lanelets.ts#LaneletGraph.siblingLanes` (method, line 751) ← call from `src/sim/intersections/admission.ts#exitSwingBlocked`, `src/sim/pipeline.ts#ensureVehicleRoutes`, `src/sim/routing/laneReach.ts#BodyReach.next`, `src/sim/vehicles/kerbStops.ts#plan`, …
- `src/world/lanelets.ts#LaneletGraph.turnPlan` (field, line 167) ← read from `src/sim/world.ts#SimWorld.prepareVehicleTopology`
- `src/world/lanelets.ts#LaneletGraph.turnPlan` (field, line 167) ← write from `src/sim/world.ts#SimWorld.prepareVehicleTopology`
- `src/world/lanelets.ts#LaneletGraph.update` (method, line 180) ← call from `src/sim/world.ts#SimWorld.rebuildVehicleTopology`
- `src/world/lanelets.ts#LaneletGraph.updateSteps` (method, line 188) ← call from `src/sim/world.ts#SimWorld.prepareVehicleTopology`
- `src/world/lanelets.ts#laneletId` (function, line 119) ← call from `src/sim/host/simAnswers.ts#roadLanes`, `src/sim/vehicles/laneChange.ts#mandatory`, `src/sim/vehicles/laneChange.ts#positioningSpeedCap`
- `src/world/lanelets.ts#LaneletId` (type, line 27) ← typeRef from `src/sim/agents/bay.ts#BayLane`, `src/sim/agents/laneGrid.ts#LaneGrid.cells`, `src/sim/agents/laneGrid.ts#LaneGrid.filed`, `src/sim/agents/lorryTraffic.ts#closures`, …

### src/world/network.ts

- `src/world/network.ts#Network` (class, line 46) ← construct from `src/editor/smallCityMap.ts#growCity`, `src/lab/catalog/layers.ts#oneRoad`, `src/main.ts#net`, `src/networkBake.worker.ts#bakeNetwork`, …
- `src/world/network.ts#Network` (class, line 46) ← typeRef from `src/actionsWiring.ts#ActionsDeps`, `src/buildingsWiring.ts#BuildingWiringDeps`, `src/editor/barriers.ts#barrierProblem`, `src/editor/barriers.ts#BarrierToolHost`, …
- `src/world/network.ts#Network.changesSince` (method, line 143) ← call from `src/editor/buildings.ts#clearBuildingsAfterEdit`, `src/editor/growthFeed.ts#GrowthSession.readChanges`, `src/editor/lotTool.ts#LotTool.refusedNow`, `src/main.ts#mutateBuilt`, …
- `src/world/network.ts#Network.constructor` (constructor, line 100) ← read from `src/editor/barriers.ts#snapBarrierPoint`, `src/editor/buildingSnap.ts#cornerFlush`, `src/editor/buildingSnap.ts#snapToRoad`, `src/editor/growthFeed.ts#roadPiece`, …
- `src/world/network.ts#Network.crossingLandingAt` (method, line 233) ← call from `src/world/landscape.ts#crossingAccessesAt`
- `src/world/network.ts#Network.crosswalkDistanceAt` (method, line 324) ← call from `src/sim/intersections/admission.ts#pedestrianHasPriority`, `src/sim/peds/sidewalk.ts#SidewalkGraph.crossingsAt`, `src/sim/peds/sidewalk.ts#SidewalkGraph.footwaysOf`, `src/sim/peds/sidewalk.ts#SidewalkGraph.kerbsAt`, …
- `src/world/network.ts#Network.exportBake` (method, line 133) ← call from `src/networkBake.worker.ts#bakeNetwork`, `src/sim/host/simClient.ts#SimClient.bake`
- `src/world/network.ts#Network.fork` (method, line 174) ← call from `src/editor/commit.ts#commitRoadPath`, `src/editor/editRules.ts#guardRoadEdit`
- `src/world/network.ts#Network.forkedFrom` (field, line 84) ← read from `src/editor/editRules.ts#written`
- `src/world/network.ts#Network.impossible` (field, line 72) ← read from `src/editor/editRules.ts#refuseRoadEdit`, `src/main.ts#currentStatus`, `src/main.ts#mountInterface`, `src/ui/inspector.ts#renderNode`
- `src/world/network.ts#Network.isBuilt` (method, line 170) ← call from `src/sim/host/simClient.ts#SimClient.bake`, `src/sim/host/simHost.ts#SimHost.frame`
- `src/world/network.ts#Network.junctionAt` (method, line 109) ← call from `src/sim/peds/sidewalk.ts#clearOfJunction`
- `src/world/network.ts#Network.junctionCells` (field, line 58) ← read from `src/editor/growthFeed.ts#GrowthSession.syncRoads`, `src/editor/growthFeed.ts#junctionPiece`, `src/main.ts#drawOverlayScreen`, `src/sim/agents/lorryDocks.ts#asphaltNear`, …
- `src/world/network.ts#Network.junctions` (field, line 54) ← read from `src/editor/growthFeed.ts#junctionPiece`, `src/sim/agents/lorryDocks.ts#asphaltNear`, `src/world/lots.ts#pavingNear`, `src/world/poleLines.ts#insideLevels`, …
- `src/world/network.ts#Network.mouthDistance` (method, line 288) ← call from `src/render/signLayer.ts#itemSigns`, `src/sim/peds/sidewalk.ts#SidewalkGraph.footwaysOf`, `src/sim/peds/sidewalk.ts#SidewalkGraph.kerbsAt`, `src/ui/inspector.ts#renderNode`, …
- `src/world/network.ts#Network.offerBake` (method, line 138) ← call from `src/main.ts#openCity`, `src/sim/host/simHost.ts#SimHost.init`, `src/sim/host/simHost.ts#SimHost.load`
- `src/world/network.ts#Network.poleLines` (field, line 82) ← read from `src/world/poleLines.ts#stateOf`
- `src/world/network.ts#Network.poleLines` (field, line 82) ← write from `src/world/poleLines.ts#stateOf`
- `src/world/network.ts#Network.polylines` (field, line 80) ← read from `src/editor/commit.ts#commitDraftInPlace`, `src/editor/commit.ts#splitSegmentAtCuts`, `src/editor/editRules.ts#grade`, `src/editor/editRules.ts#segmentsThrough`, …
- `src/world/network.ts#Network.rebuild` (method, line 119) ← call from `src/editor/cityGenerator.ts#greenCity`, `src/editor/commit.ts#commitDraftInPlace`, `src/editor/commit.ts#commitRoadPath`, `src/editor/editRules.ts#guardRoadEdit`, …
- `src/world/network.ts#Network.revision` (field, line 87) ← read from `src/editor/cityGenerator.ts#greenCity`, `src/editor/commit.ts#commitRoadPath`, `src/editor/editRules.ts#guardRoadEdit`, `src/editor/history.ts#followNetwork`, …
- `src/world/network.ts#Network.ribbons` (field, line 55) ← read from `src/editor/commit.ts#boreDeepCuts`, `src/editor/commit.ts#draftStations`, `src/editor/commit.ts#duplicateSegment`, `src/editor/editRules.ts#grade`, …
- `src/world/network.ts#Network.ribbonsMeeting` (method, line 62) ← call from `src/render/signs.ts#streetChain`, `src/sim/agents/lorryDocks.ts#asphaltNear`, `src/sim/ambient/ambientIndex.ts#StreetLength.around`, `src/ui/overlay/congestion.ts#drawCongestion`, …
- `src/world/network.ts#Network.segmentCells` (field, line 57) ← read from `src/editor/growthFeed.ts#GrowthSession.syncRoads`, `src/editor/growthFeed.ts#roadPiece`, `src/editor/poleRoutes.ts#buildRouteGraph`, `src/render/signals.ts#createSignalHeads.sync`, …
- `src/world/network.ts#Network.squeezed` (field, line 74) ← read from `src/editor/editRules.ts#refuseRoadEdit`
- `src/world/network.ts#Network.state` (getter, line 148) ← call from `src/editor/growthFeed.ts#GrowthSession.readChanges`, `src/editor/growthFeed.ts#GrowthSession.receive`, `src/editor/growthFeed.ts#GrowthSession.send`, `src/editor/lotTool.ts#LotTool.proposalKey`, …
- `src/world/network.ts#Network.stopLineDistance` (method, line 298) ← call from `src/world/parkingLayout.ts#endClear`, `src/world/signalPosts.ts#signalPostPlace`
- `src/world/network.ts#Network.takeBack` (method, line 198) ← call from `src/editor/commit.ts#layCopy`, `src/editor/editRules.ts#guardRoadEdit`
- `src/world/network.ts#Network.trafficRevision` (field, line 89) ← read from `src/main.ts#mutateBuilt`, `src/sim/pipeline.ts#step`, `src/sim/world.ts#SimWorld.rebuildVehicleTopology`, `src/sim/world.ts#SimWorld.rebuildWalkTopology`
- `src/world/network.ts#Network.trafficToken` (method, line 91) ← call from `src/sim/host/simHost.ts#SimHost.frame`
- `src/world/network.ts#Network.trims` (field, line 75) ← read from `src/editor/editRules.ts#overlapsAnother`, `src/editor/editRules.ts#withinSharedPlate`, `src/render/renderer.ts#createSceneRenderer`

### src/world/networkBuild.ts

- `src/world/networkBuild.ts#NetworkBake` (interface, line 69) ← typeRef from `src/main.ts#networkBakeOf`, `src/main.ts#openCity`, `src/networkBake.worker.ts#bakeNetwork`, `src/networkBake.worker.ts#NetworkBakeReply`, …

### src/world/networkDelta.ts

- `src/world/networkDelta.ts#NetworkDelta` (interface, line 5) ← read from `src/editor/buildings.ts#clearBuildingsAfterEdit`, `src/editor/growthFeed.ts#GrowthSession.readChanges`, `src/editor/lotTool.ts#LotTool.refusedNow`, `src/main.ts#mutateBuilt`, …
- `src/world/networkDelta.ts#NetworkDelta` (interface, line 5) ← typeRef from `src/world/lots.ts#LotPlanner.planChanged`

### src/world/networkLength.ts

- `src/world/networkLength.ts#NetworkLength` (class, line 8) ← construct from `src/sim/ambient/ambientIndex.ts#StreetLength.whole`, `src/sim/world.ts#SimWorld.roadLength`
- `src/world/networkLength.ts#NetworkLength.of` (method, line 14) ← call from `src/sim/ambient/ambientIndex.ts#StreetLength.wholeMap`, `src/sim/vehicles/spawn.ts#roadLength`

### src/world/paving.ts

- `src/world/paving.ts#pavedHeightFrom` (function, line 12) ← call from `src/editor/smallCityMap.ts#growLots.pavedAt`, `src/render/renderer.ts#createSceneRenderer`
- `src/world/paving.ts#PavingData` (interface, line 27) ← read from `src/render/buildings/cellWork.ts#wider`
- `src/world/paving.ts#PavingData` (interface, line 27) ← typeRef from `src/editor/growthFeed.ts#GrowthSession.send`, `src/editor/growthReplica.ts#GrowAsk`, `src/editor/growthReplica.ts#pavedLookup`, `src/render/buildings/cellWork.ts#CellJob`, …
- `src/world/paving.ts#pavingReader` (function, line 41) ← call from `src/editor/growthReplica.ts#pavedLookup`, `src/render/buildings/cellWork.ts#workerReads`, `src/render/groundShape.ts#padsOfAsk`
- `src/world/paving.ts#PavingSource` (interface, line 34) ← call from `src/editor/growthFeed.ts#GrowthSession.send`, `src/render/buildings/cellPool.ts#CellPool.write`, `src/render/renderer.ts#createSceneRenderer.padsAskOf`
- `src/world/paving.ts#PavingSource` (interface, line 34) ← read from `src/editor/growthFeed.ts#GrowthSession.send`, `src/render/buildings/cellPool.ts#CellPool.offPage`, `src/render/buildings/cellPool.ts#CellPool.write`, `src/render/renderer.ts#createSceneRenderer.padsAskOf`
- `src/world/paving.ts#PavingSource` (interface, line 34) ← typeRef from `src/editor/growthFeed.ts#GrowthHost`, `src/render/buildings/cellPool.ts#CellEnv`, `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#SceneHandle`

### src/world/ribbons.ts

- `src/world/ribbons.ts#SegmentRibbon` (interface, line 25) ← read from `src/editor/barriers.ts#onCarriageway`, `src/editor/barriers.ts#snapBarrierPoint`, `src/editor/buildingSnap.ts#cornerFlush`, `src/editor/buildingSnap.ts#snapToRoad`, …
- `src/world/ribbons.ts#SegmentRibbon` (interface, line 25) ← typeRef from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.nextWatched`, `src/render/structureCells.ts#cellOfRibbon`, `src/render/structureCells.ts#createStructureCells.ribbonsOf`, …
- `src/world/ribbons.ts#SegmentTrims` (interface, line 18) ← read from `src/editor/editRules.ts#overlapsAnother`, `src/editor/editRules.ts#withinSharedPlate`, `src/render/renderer.ts#createSceneRenderer`

### src/world/roadField.ts

- `src/world/roadField.ts#roadField` (function, line 417) ← implementation from `src/render/groundShape.ts#blockDigest`, `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.deckHeight`, `src/render/utilities.ts#poleGroundAt`
- `src/world/roadField.ts#roadField.onSegment` (method, line 601) ← implementation from `src/editor/commit.ts#boreDeepCuts`, `src/editor/editRules.ts#grade`, `src/editor/editRules.ts#refuseRoadEdit.deck`, `src/render/renderer.ts#createSceneRenderer`, …
- `src/world/roadField.ts#roadField.roadAt` (method, line 624) ← implementation from `src/render/groundShape.ts#blockDigest`, `src/render/renderer.ts#createSceneRenderer`, `src/render/utilities.ts#poleGroundAt`
- `src/world/roadField.ts#RoadReads` (interface, line 96) ← call from `src/editor/commit.ts#boreDeepCuts`, `src/editor/editRules.ts#grade`, `src/editor/editRules.ts#refuseRoadEdit.deck`, `src/render/groundShape.ts#blockDigest`, …
- `src/world/roadField.ts#RoadSample` (interface, line 57) ← read from `src/render/groundShape.ts#blockDigest`, `src/render/renderer.ts#createSceneRenderer`, `src/render/utilities.ts#poleGroundAt`

### src/world/roads/buildMode.ts

- `src/world/roads/buildMode.ts#BuildMode` (type, line 10) ← typeRef from `src/main.ts#BUILD_FILL`, `src/main.ts#buildSummary`, `src/main.ts#drawBuildModes`, `src/main.ts#drawOverlayScreen`, …
- `src/world/roads/buildMode.ts#buildModeAt` (function, line 13) ← call from `src/render/structures.ts#retainingWalls`
- `src/world/roads/buildMode.ts#BuildRun` (interface, line 37) ← read from `src/main.ts#buildSummary`
- `src/world/roads/buildMode.ts#buildRuns` (function, line 110) ← call from `src/main.ts#buildSummary`
- `src/world/roads/buildMode.ts#buildRuns` (function, line 110) ← typeRef from `src/main.ts#buildSummary`
- `src/world/roads/buildMode.ts#orderAlong` (function, line 44) ← call from `src/editor/commit.ts#draftStations`
- `src/world/roads/buildMode.ts#RoadStation` (interface, line 24) ← read from `src/main.ts#drawOverlayScreen`
- `src/world/roads/buildMode.ts#RoadStation` (interface, line 24) ← typeRef from `src/editor/commit.ts#DraftResult`, `src/editor/commit.ts#draftStations`
- `src/world/roads/buildMode.ts#stationsAlong` (function, line 82) ← call from `src/editor/commit.ts#draftStations`

### src/world/roads/connectors.ts

- `src/world/roads/connectors.ts#LaneLink` (interface, line 5) ← typeRef from `src/ui/inspector.ts#InspectorActions`, `src/world/doc.ts#RoadDoc.setNodeLaneLinks`, `src/world/doc.ts#RoadNode`
- `src/world/roads/connectors.ts#linksDigest` (function, line 50) ← call from `src/world/doc.ts#RoadDoc.setNodeLaneLinks`, `src/world/doc.ts#sameNode`
- `src/world/roads/connectors.ts#movementKey` (function, line 36) ← call from `src/world/doc.ts#RoadDoc.setMovementBlocked`
- `src/world/roads/connectors.ts#normalizeLaneLinks` (function, line 17) ← call from `src/world/doc.ts#finishNode`, `src/world/doc.ts#RoadDoc.setNodeLaneLinks`

### src/world/roads/derivedSigns.ts

- `src/world/roads/derivedSigns.ts#DerivedSign` (interface, line 10) ← read from `src/render/signLayer.ts#junctionSigns`
- `src/world/roads/derivedSigns.ts#signsAtNode` (function, line 27) ← call from `src/render/signLayer.ts#junctionSigns`

### src/world/roads/footwayRise.ts

- `src/world/roads/footwayRise.ts#FlushData` (interface, line 36) ← read from `src/render/renderer.ts#createSceneRenderer.landGroundDigest`, `src/render/renderer.ts#createSceneRenderer.levelGroundDigest`
- `src/world/roads/footwayRise.ts#footwayRiseAt` (function, line 33) ← call from `src/editor/smallCityMap.ts#growLots.pavedAt`, `src/render/agents.ts#createAgentMeshes.sync`, `src/render/renderer.ts#createSceneRenderer.footwayRiseHere`, `src/render/signLayer.ts#itemSigns`, …
- `src/world/roads/footwayRise.ts#packFlush` (function, line 44) ← call from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.landGroundDigest`, `src/render/renderer.ts#createSceneRenderer.levelGroundDigest`
- `src/world/roads/footwayRise.ts#packFlush` (function, line 44) ← read from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.landGroundDigest`, `src/render/renderer.ts#createSceneRenderer.levelGroundDigest`

### src/world/roads/furnitureSets.ts

- `src/world/roads/furnitureSets.ts#FURNITURE_SETS` (variable, line 14) ← read from `src/ui/v2/shell.ts#mountShell.renderRoads`
- `src/world/roads/furnitureSets.ts#furnitureFor` (function, line 71) ← call from `src/editor/cityGenerator.ts#greenCity`
- `src/world/roads/furnitureSets.ts#FurniturePlacement` (interface, line 65) ← read from `src/editor/cityGenerator.ts#greenCity`

### src/world/roads/laneEnds.ts

- `src/world/roads/laneEnds.ts#laneEnds` (function, line 27) ← call from `src/sim/host/simAnswers.ts#answer`

### src/world/roads/markingStyle.ts

- `src/world/roads/markingStyle.ts#isMarkingStyle` (function, line 44) ← call from `src/main.ts#setMarkingStyle`, `src/world/doc.ts#RoadDoc.fromJSON`
- `src/world/roads/markingStyle.ts#MarkingStyleId` (type, line 4) ← typeRef from `src/world/doc.ts#RoadDoc.markingStyle`, `src/world/doc.ts#RoadDoc.restoreValue`, `src/world/doc.ts#RoadDoc.setMarkingStyle`

### src/world/roads/profile.ts

- `src/world/roads/profile.ts#profileOf` (function, line 43) ← call from `src/main.ts#showInspector`
- `src/world/roads/profile.ts#profileRoad` (function, line 260) ← call from `src/main.ts#drawOverlayScreen`
- `src/world/roads/profile.ts#RoadProfileSpec` (interface, line 23) ← typeRef from `src/ui/inspector.ts#InspectorActions`

### src/world/roads/rules.ts

- `src/world/roads/rules.ts#ApproachRuleEntry` (interface, line 11) ← typeRef from `src/ui/inspector.ts#InspectorActions`, `src/world/doc.ts#RoadDoc.setNodeApproachRules`, `src/world/doc.ts#RoadNode`
- `src/world/roads/rules.ts#isHighwayClass` (function, line 114) ← call from `src/sim/intersections/admission.ts#rightOfWay`, `src/sim/intersections/admission.ts#yieldSide`
- `src/world/roads/rules.ts#legRule` (function, line 106) ← call from `src/sim/intersections/admission.ts#rightOfWay`
- `src/world/roads/rules.ts#mainRoadLegs` (function, line 73) ← call from `src/sim/roads/greenWave.ts#corridorFrom`, `src/sim/roads/greenWave.ts#corridorFrom.otherLeg`, `src/sim/roads/greenWave.ts#corridorFrom.walk`, `src/sim/world.ts#SimWorld.stepControlAdvisor`
- `src/world/roads/rules.ts#normalizeApproachRules` (function, line 31) ← call from `src/world/doc.ts#finishNode`, `src/world/doc.ts#RoadDoc.setNodeApproachRules`
- `src/world/roads/rules.ts#normalizeSignalSettings` (function, line 45) ← call from `src/world/doc.ts#finishNode`, `src/world/doc.ts#RoadDoc.setNodeSignal`
- `src/world/roads/rules.ts#rulesDigest` (function, line 63) ← call from `src/world/doc.ts#RoadDoc.setNodeApproachRules`, `src/world/doc.ts#sameNode`
- `src/world/roads/rules.ts#signalDigest` (function, line 62) ← call from `src/world/doc.ts#RoadDoc.setNodeSignal`, `src/world/doc.ts#sameNode`
- `src/world/roads/rules.ts#SignalSettings` (interface, line 17) ← read from `src/sim/roads/greenWave.ts#greenWave`, `src/sim/signals/fsm.ts#fixedCycle`, `src/sim/signals/fsm.ts#seekFixed`, `src/sim/signals/fsm.ts#stepController`, …
- `src/world/roads/rules.ts#SignalSettings` (interface, line 17) ← typeRef from `src/sim/host/simAnswers.ts#answer`, `src/sim/roads/greenWave.ts#greenWave`, `src/sim/signals/fsm.ts#fixedCycle`, `src/sim/signals/fsm.ts#seekFixed`, …

### src/world/roads/streetChain.ts

- `src/world/roads/streetChain.ts#ChainPiece` (interface, line 11) ← read from `src/main.ts#showInspector`
- `src/world/roads/streetChain.ts#flipProfile` (function, line 63) ← call from `src/main.ts#showInspector`
- `src/world/roads/streetChain.ts#streetChain` (function, line 17) ← call from `src/main.ts#showInspector`

### src/world/roads/streetNames.ts

- `src/world/roads/streetNames.ts#nameFor` (function, line 69) ← call from `src/editor/cityGenerator.ts#greenCity`
- `src/world/roads/streetNames.ts#nameFor` (function, line 69) ← read from `src/editor/cityGenerator.ts#greenCity`
- `src/world/roads/streetNames.ts#streetNameOf` (function, line 57) ← call from `src/ui/inspector.ts#renderSegment`
- `src/world/roads/streetNames.ts#streetNumbers` (function, line 107) ← call from `src/ui/inspector.ts#renderSegment`
- `src/world/roads/streetNames.ts#StreetNumbers` (interface, line 99) ← read from `src/ui/inspector.ts#renderSegment`

### src/world/roads/tuning.ts

- `src/world/roads/tuning.ts#ROAD_TUNING` (variable, line 4) ← read from `src/editor/commit.ts#AUTO_TUNNEL_COVER`, `src/editor/commit.ts#CROSSING_CLEARANCE`, `src/editor/editRules.ts#MAX_BUILT_GRADE`, `src/editor/editRules.ts#PASS_CLEARANCE`, …

### src/world/roadSection.ts

- `src/world/roadSection.ts#cloneRoadSection` (function, line 226) ← call from `src/world/doc.ts#copySegment`, `src/world/doc.ts#savedSegment`
- `src/world/roadSection.ts#flipSection` (function, line 163) ← call from `src/editor/commit.ts#joinSegments`
- `src/world/roadSection.ts#normalizeRoadSection` (function, line 73) ← call from `src/editor/persistence.ts#isSerializedDoc`, `src/world/doc.ts#RoadDoc.addSegment`, `src/world/doc.ts#RoadDoc.setSegmentSection`, `src/world/doc.ts#segmentRecord`
- `src/world/roadSection.ts#ROAD_SECTION_LIMITS` (variable, line 64) ← read from `src/sim/routing/lowerBound.ts#FASTEST_LIMIT`
- `src/world/roadSection.ts#RoadSection` (interface, line 23) ← read from `src/editor/commit.ts#joinSegments`, `src/sim/intersections/admission.ts#throughRank`
- `src/world/roadSection.ts#RoadSection` (interface, line 23) ← typeRef from `src/ui/inspector.ts#InspectorActions`, `src/world/doc.ts#fitRoadCurve`, `src/world/doc.ts#RoadDoc.addSegment`, `src/world/doc.ts#RoadDoc.setSegmentSection`, …
- `src/world/roadSection.ts#sameRoadSection` (function, line 197) ← call from `src/world/doc.ts#RoadDoc.setSegmentSection`, `src/world/doc.ts#sameSegment`
- `src/world/roadSection.ts#sameRoadSectionIgnoringArrows` (function, line 221) ← call from `src/editor/commit.ts#joinSegments`
- `src/world/roadSection.ts#sectionForPiece` (function, line 210) ← call from `src/editor/commit.ts#joinSegments`, `src/editor/commit.ts#splitSegmentAtCuts`
- `src/world/roadSection.ts#sectionForWidth` (function, line 246) ← call from `src/main.ts#drawOverlayScreen`

### src/world/roadTypes.ts

- `src/world/roadTypes.ts#casingHalf` (function, line 384) ← call from `src/editor/repair.ts#nearestCandidate`, `src/render/structures.ts#onAnotherCasing`, `src/render/structures.ts#retainingWalls`, `src/render/structures.ts#structureParts`, …
- `src/world/roadTypes.ts#CURB_BAND` (variable, line 90) ← read from `src/editor/barriers.ts#onCarriageway`, `src/editor/barriers.ts#snapBarrierPoint`, `src/editor/poleRoutes.ts#buildRouteGraph`, `src/render/propGeometry.ts#drainGeometry`, …
- `src/world/roadTypes.ts#flushOn` (function, line 360) ← call from `src/world/section.ts#sectionOf`
- `src/world/roadTypes.ts#FOOTWAY_RISE` (variable, line 93) ← read from `src/render/propGeometry.ts#drainGeometry`, `src/render/renderer.ts#createSceneRenderer.footwayRiseHere`, `src/render/utilities.ts#poleGroundAt`
- `src/world/roadTypes.ts#footwayOn` (function, line 356) ← call from `src/sim/peds/sidewalk.ts#SidewalkGraph.footwaysOf`, `src/sim/peds/sidewalk.ts#SidewalkGraph.kerbsAt`, `src/world/section.ts#sectionOf`
- `src/world/roadTypes.ts#halfWidth` (function, line 339) ← call from `src/editor/barriers.ts#snapBarrierPoint`, `src/editor/buildingSnap.ts#cornerFlush`, `src/editor/buildingSnap.ts#snapToRoad`, `src/editor/editRules.ts#overlapReach`, …
- `src/world/roadTypes.ts#LANE_LINE` (variable, line 86) ← read from `src/render/renderer.ts#createSceneRenderer`
- `src/world/roadTypes.ts#laneOffset` (function, line 420) ← call from `src/render/densityCars.ts#createDensityCars.writeTile`, `src/world/section.ts#sectionOf`
- `src/world/roadTypes.ts#laneWidth` (function, line 395) ← call from `src/world/section.ts#sectionOf`
- `src/world/roadTypes.ts#LAST_UPGRADE_CLASS` (variable, line 233) ← read from `src/main.ts#pointerDown`, `src/main.ts#showInspector`, `src/ui/inspector.ts#renderSegment`
- `src/world/roadTypes.ts#Level` (enum, line 10) ← reference from `src/editor/barriers.ts#snapBarrierPoint`, `src/editor/buildingSnap.ts#cornerFlush`, `src/editor/buildingSnap.ts#snapToRoad`, `src/editor/editRules.ts#overlapReach`, …
- `src/world/roadTypes.ts#ROAD_TYPES` (variable, line 97) ← read from `src/editor/cityGenerator.ts#layCity`, `src/editor/cityGenerator.ts#zoneCity`, `src/editor/commit.ts#commitDraftInPlace`, `src/editor/commit.ts#commitRoadPath`, …
- `src/world/roadTypes.ts#roadProfile` (function, line 248) ← call from `src/editor/cityGenerator.ts#widenCityFootways`, `src/main.ts#drawOverlayScreen`, `src/render/structures.ts#onAnotherCasing`, `src/sim/peds/sidewalk.ts#SidewalkGraph.crossingsAt`, …
- `src/world/roadTypes.ts#RoadSide` (type, line 353) ← typeRef from `src/world/section.ts#zonesOn`
- `src/world/roadTypes.ts#roadType` (function, line 235) ← call from `src/main.ts#drawOverlayScreen`, `src/main.ts#laneChoices`, `src/world/cityGen/wholeMap.ts#EDGE_INSET`
- `src/world/roadTypes.ts#RoadType` (interface, line 29) ← read from `src/editor/barriers.ts#onCarriageway`, `src/editor/barriers.ts#snapBarrierPoint`, `src/editor/cityGenerator.ts#layCity`, `src/editor/cityGenerator.ts#widenCityFootways`, …
- `src/world/roadTypes.ts#RoadType` (interface, line 29) ← typeRef from `src/ui/inspector.ts#parkingSelect`, `src/ui/labels.ts#roadTypeDescription`, `src/ui/labels.ts#roadTypeName`, `src/world/buildings/validate.ts#RoadReachRibbon`, …
- `src/world/roadTypes.ts#sectionFromProfile` (function, line 407) ← call from `src/editor/cityGenerator.ts#widenCityFootways`
- `src/world/roadTypes.ts#SegmentDirection` (type, line 7) ← typeRef from `src/editor/persistence.ts#isSegmentDirection`, `src/lab/catalog/layout.ts#road`, `src/ui/inspector.ts#directionOptions`, `src/ui/inspector.ts#InspectorActions`, …
- `src/world/roadTypes.ts#sidewalkHalf` (function, line 383) ← call from `src/render/renderer.ts#createSceneRenderer`, `src/render/structures.ts#structureParts`
- `src/world/roadTypes.ts#SurfaceLevel` (type, line 25) ← typeRef from `src/editor/growthReplica.ts#RoadReplica.apply`, `src/editor/growthReplica.ts#RoadReplica.kerbs`, `src/world/buildings/validate.ts#RoadReach`, `src/world/lots.ts#pavingNear`, …
- `src/world/roadTypes.ts#symmetric` (function, line 364) ← call from `src/world/section.ts#sectionOf`
- `src/world/roadTypes.ts#travelLanes` (function, line 391) ← call from `src/ui/inspector.ts#segmentStats`, `src/world/section.ts#sectionOf`

### src/world/streetFurniture.ts

- `src/world/streetFurniture.ts#FurnitureKind` (type, line 11) ← typeRef from `src/render/scenery.ts#SceneryKit`

### src/world/surfaces.ts

- `src/world/surfaces.ts#levelRingsOf` (function, line 81) ← call from `src/sim/agents/lorryDocks.ts#asphaltNear`, `src/world/lots.ts#pavingNear`, `src/world/poleLines.ts#junctionRings`, `src/world/poleLines.ts#poleLinesOf`, …

## Shared declarations the feature uses

### src/conflictPool.ts

- `src/conflictPool.ts#ConflictPool.buildJunctions` (method, line 165) — implementation by 2: `src/world/lanelets.ts#LaneletGraph.buildJunctions`, `src/world/lanelets.ts#LaneletGraph.junctionsOffPage`

### src/core/aabb.ts

- `src/core/aabb.ts#Aabb` (interface, line 3) — read by 18: `src/editor/roadTool.ts#landscapeAlong`, `src/editor/snap.ts#findAnchor`, `src/editor/roads/streetKit.ts#kitOf`, `src/world/roads/furnitureSets.ts#furnitureFor`, …
- `src/core/aabb.ts#Aabb` (interface, line 3) — typeRef by 8: `src/world/roadField.ts#boxDistance`, `src/world/roadField.ts#FieldProfile`, `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#Profile`, …

### src/core/bezier.ts

- `src/core/bezier.ts#CurveShape` (interface, line 5) — read by 1: `src/world/networkBuild.ts#segmentSourceKey`
- `src/core/bezier.ts#CurveShape` (interface, line 5) — typeRef by 5: `src/editor/roadTool.ts#RoadTool.curveOf`, `src/editor/roadPath.ts#RoadPathPiece`, `src/editor/nodeMover.ts#NodeMover.drag`, `src/editor/nodeMover.ts#NodeMover.grab`, …
- `src/core/bezier.ts#shapeFromControl` (function, line 21) — call by 3: `src/editor/roadTool.ts#RoadTool.curveOf`, `src/editor/roadPath.ts#roadPathFromGesture`, `src/editor/roundabout.ts#commitRoundabout`

### src/core/clipper.ts

- `src/core/clipper.ts#difference` (function, line 148) — call by 3: `src/world/surfaces.ts#bands`, `src/render/roadTileShape.ts#tileShapes`, `src/render/networkFarRoadsBuild.ts#buildFarRoadCells`
- `src/core/clipper.ts#input` (function, line 26) — call by 1: `src/core/clipper.ts#opened`
- `src/core/clipper.ts#intersection` (function, line 152) — call by 3: `src/render/roadTileShape.ts#tileShapes`, `src/render/networkFarRoadsBuild.ts#buildFarRoadCells`, `src/render/mesh/tileShape.ts#clipHalves`
- `src/core/clipper.ts#MultiPoly` (type, line 9) — typeRef by 20: `src/world/surfaces.ts#Bands`, `src/world/surfaces.ts#levelPolygons`, `src/world/surfaces.ts#levelRings`, `src/world/surfaces.ts#levelRingsOf`, …
- `src/core/clipper.ts#OPEN_MITER` (variable, line 171) — read by 1: `src/core/clipper.ts#opened`
- `src/core/clipper.ts#output` (function, line 58) — call by 1: `src/core/clipper.ts#opened`
- `src/core/clipper.ts#Poly` (type, line 8) — typeRef by 16: `src/render/roadSurfaceInputs.ts#SurfaceInputs.placeSegment`, `src/render/roadSurfacePass.ts#polysOf.polys`, `src/render/roadSurfaceRings.ts#Input`, `src/render/roadSurfaceRings.ts#inputOf`, …
- `src/core/clipper.ts#Ring` (type, line 6) — typeRef by 9: `src/render/networkFarRoadsBuild.ts#buildFarRoadCells`, `src/render/networkFarRoadsBuild.ts#CellOutlines`, `src/core/axisCut.ts#cutAtAxis`, `src/core/axisCut.ts#clearLine`, …
- `src/core/clipper.ts#SCALE` (variable, line 12) — read by 1: `src/core/clipper.ts#opened`
- `src/core/clipper.ts#union` (function, line 143) — call by 4: `src/world/surfaces.ts#levelPolygons`, `src/render/roadSurfaceInputs.ts#SurfaceInputs.footwayEdges`, `src/render/roadTileShape.ts#tileShapes.merged`, `src/render/networkFarRoadsBuild.ts#buildFarRoadCells`

### src/core/derivedCache.ts

- `src/core/derivedCache.ts#forgetOtherDerived` (function, line 327) — call by 1: `src/render/surfaceBakeClient.ts#startSurfaceBake`

### src/core/digest.ts

- `src/core/digest.ts#Digest` (class, line 6) — construct by 14: `src/world/junctionBuild.ts#junctionKey`, `src/world/networkBuild.ts#NetworkBuild.junctionKey`, `src/world/networkBuild.ts#nodeSourceDigest`, `src/world/networkBuild.ts#segmentSourceKey`, …
- `src/core/digest.ts#Digest` (class, line 6) — typeRef by 1: `src/world/networkBuild.ts#nodeSourceDigest`
- `src/core/digest.ts#Digest.add` (method, line 11) — call by 12: `src/world/networkBuild.ts#NetworkBuild.junctionKey`, `src/world/networkBuild.ts#nodeBakeKey`, `src/world/networkBuild.ts#nodeSourceDigest`, `src/world/networkBuild.ts#segmentSourceKey`, …
- `src/core/digest.ts#Digest.addAll` (method, line 19) — call by 4: `src/world/networkBuild.ts#nodeSourceDigest`, `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#buildRoadElevation.keyOf`, `src/world/lanelets.ts#LaneletGraph.buildSegmentLinks`
- `src/core/digest.ts#Digest.addPlain` (method, line 31) — call by 6: `src/world/networkBuild.ts#NetworkBuild.junctionKey`, `src/world/networkBuild.ts#nodeRecordKey`, `src/world/networkBuild.ts#nodeSourceDigest`, `src/world/networkBuild.ts#segmentSourceKey`, …
- `src/core/digest.ts#Digest.addText` (method, line 25) — call by 7: `src/world/junctionBuild.ts#junctionKey`, `src/world/networkBuild.ts#nodeSourceDigest`, `src/world/networkBuild.ts#segmentSourceKey`, `src/world/elevation.ts#buildRoadElevation`, …
- `src/core/digest.ts#Digest.value` (method, line 59) — call by 16: `src/world/junctionBuild.ts#junctionKey`, `src/world/networkBuild.ts#NetworkBuild.junctionKey`, `src/world/networkBuild.ts#nodeBakeKey`, `src/world/networkBuild.ts#nodeRecordKey`, …

### src/core/fillet.ts

- `src/core/fillet.ts#ACUTE_CLEARANCE` (variable, line 48) — read by 1: `src/core/fillet.ts#acuteSetback`
- `src/core/fillet.ts#CURB_R_MAX` (variable, line 60) — read by 2: `src/world/junction/corners.ts#resolveCorner`, `src/core/fillet.ts#curbRadius`
- `src/core/fillet.ts#CURB_R_MIN` (variable, line 57) — read by 1: `src/core/fillet.ts#curbRadius`
- `src/core/fillet.ts#Fillet` (interface, line 5) — read by 5: `src/world/junction/corners.ts#cornerOf`, `src/world/junction/corners.ts#packCorners`, `src/world/junction/corners.ts#resolveCorner`, `src/world/junction/derive.ts#deriveFromLegs`, …
- `src/core/fillet.ts#Fillet` (interface, line 5) — typeRef by 3: `src/world/junction/corners.ts#PackedCorner.fillet`, `src/world/junction/derive.ts#deriveFromLegs`, `src/world/junction/types.ts#Corner`
- `src/core/fillet.ts#filletCorner` (function, line 19) — call by 1: `src/world/junction/corners.ts#resolveCorner`

### src/core/forkMap.ts

- `src/core/forkMap.ts#ForkMap` (class, line 2) — construct by 3: `src/world/network.ts#Network.fork`, `src/world/networkBuild.ts#NetworkBuild.forkFrom`, `src/world/geometry.ts#PolylineCache.forkFrom`
- `src/core/forkMap.ts#ForkMap` (class, line 2) — reference by 3: `src/world/network.ts#Network.takeBack`, `src/world/networkBuild.ts#NetworkBuild.takeBack`, `src/world/cells.ts#CellFiling.takeBack`
- `src/core/forkMap.ts#ForkMap` (class, line 2) — typeRef by 3: `src/world/cells.ts#CellFiling.takeBack`, `src/core/forkMap.ts#ForkSet.map`, `src/world/geometry.ts#PolylineCache.takeBack`
- `src/core/forkMap.ts#ForkMap.applyTo` (method, line 100) — call by 5: `src/world/network.ts#Network.takeBack`, `src/world/networkBuild.ts#NetworkBuild.takeBack`, `src/core/forkMap.ts#ForkSet.applyTo`, `src/world/cells.ts#CellFiling.takeBack`, …

### src/core/hitch.ts

- `src/core/hitch.ts#measureHitch` (function, line 2) — call by 3: `src/world/network.ts#Network.rebuild`, `src/world/elevation.ts#buildRoadElevation`, `src/render/roadSurfaces.ts#roadSurfaceSteps`

### src/core/hypot.ts

- `src/core/hypot.ts#hypot2` (function, line 7) — call by 7: `src/world/roadField.ts#boxDistance`, `src/render/mesh/tileShape.ts#skirtShape`, `src/render/mesh/tileShape.ts#densify`, `src/render/mesh/tileShape.ts#flipToDelaunay`, …

### src/core/identityMemo.ts

- `src/core/identityMemo.ts#identityMemo` (function, line 2) — call by 3: `src/ui/roads/keys.ts#paramsOf`, `src/world/roadTypes.ts#profilesOf`, `src/world/carriageway.ts#ringPoints`

### src/core/intersect.ts

- `src/core/intersect.ts#lineLine` (function, line 5) — call by 2: `src/world/junction/corners.ts#resolveCorner`, `src/world/junction/derive.ts#deriveFromLegs`
- `src/core/intersect.ts#lineLine` (function, line 5) — read by 2: `src/world/junction/corners.ts#resolveCorner`, `src/world/junction/derive.ts#deriveFromLegs`
- `src/core/intersect.ts#segSeg` (function, line 21) — call by 2: `src/editor/snap.ts#rayHitsPolyline`, `src/editor/commit.ts#reconcileMovedNode`
- `src/core/intersect.ts#segSeg` (function, line 21) — read by 2: `src/editor/snap.ts#rayHitsPolyline`, `src/editor/commit.ts#reconcileMovedNode`

### src/core/keptList.ts

- `src/core/keptList.ts#KeptList` (class, line 2) — construct by 1: `src/ui/roads/templates.ts#kept`
- `src/core/keptList.ts#KeptList.readShared` (method, line 30) — call by 1: `src/ui/roads/templates.ts#savedTemplates`
- `src/core/keptList.ts#KeptList.update` (method, line 45) — call by 2: `src/ui/roads/templates.ts#deleteTemplate`, `src/ui/roads/templates.ts#saveTemplate`

### src/core/offset.ts

- `src/core/offset.ts#offsetPolyline` (function, line 46) — call by 5: `src/world/lanelets.ts#LaneletGraph.buildSegmentLinks`, `src/world/markings.ts#segmentMarkings`, `src/world/markings.ts#segmentMarkings.centreLine`, `src/world/ribbons.ts#ribbonRing`, …

### src/core/plain.ts

- `src/core/plain.ts#samePlain` (function, line 2) — call by 2: `src/world/networkBuild.ts#NetworkBuild.buildRibbon`, `src/world/networkBuild.ts#NetworkBuild.rebuild`

### src/core/polygon.ts

- `src/core/polygon.ts#convexHull` (function, line 54) — call by 1: `src/world/junction/polygon.ts#buildJunctionRing`
- `src/core/polygon.ts#isSimple` (function, line 8) — call by 2: `src/world/junction/derive.ts#deriveFromLegs`, `src/world/junction/polygon.ts#buildJunctionRing`
- `src/core/polygon.ts#pointInPolygon` (function, line 40) — call by 2: `src/world/junction/bulb.ts#clearBulbsOf`, `src/world/carriageway.ts#onRoadGround`

### src/core/polyline.ts

- `src/core/polyline.ts#Frame` (interface, line 7) — read by 31: `src/editor/snap.ts#snapEndpoint`, `src/editor/snap.ts#snapHeading`, `src/editor/streetscapeTool.ts#rowPieces`, `src/editor/streetObjects.ts#commitPedestrianCrossing`, …
- `src/core/polyline.ts#FrameOut` (interface, line 23) — read by 4: `src/world/roads/footwayRise.ts#flushHolds`, `src/world/roadField.ts#roadField.frameInto`, `src/world/roadField.ts#roadField.roadAt`, `src/world/roadField.ts#roadField.roadAtInto`
- `src/core/polyline.ts#newFrame` (function, line 33) — call by 2: `src/world/roads/footwayRise.ts#frame`, `src/world/roadField.ts#roadField`
- `src/core/polyline.ts#Polyline` (class, line 36) — reference by 11: `src/world/roads/footwayRise.ts#flushShareOf`, `src/world/junctionBuild.ts#buildJunctionOf.line`, `src/world/networkBuild.ts#ribbonFromBake`, `src/world/roadField.ts#unpackField`, …
- `src/core/polyline.ts#Polyline` (class, line 36) — typeRef by 43: `src/world/roads/footwayRise.ts#flushHolds`, `src/world/roads/footwayRise.ts#flushShareOf`, `src/world/junction/mouthOverlap.ts#mouthOverlapStrip`, `src/world/junctionBuild.ts#buildJunctionOf`, …
- `src/core/polyline.ts#Polyline.bbox` (getter, line 116) — call by 13: `src/editor/roadTool.ts#landscapeAlong`, `src/editor/snap.ts#findAnchor`, `src/editor/roads/streetKit.ts#kitOf`, `src/world/roads/furnitureSets.ts#furnitureFor`, …
- `src/core/polyline.ts#Polyline.closestInto` (method, line 347) — call by 11: `src/editor/snap.ts#snapEndpoint`, `src/world/roads/footwayRise.ts#flushHolds`, `src/world/roadField.ts#roadField.frameInto`, `src/world/roadField.ts#roadField.nearest`, …
- `src/core/polyline.ts#Polyline.closestPoint` (method, line 330) — call by 6: `src/editor/roadTool.ts#RoadTool.down`, `src/editor/snap.ts#findAnchor`, `src/editor/roads/streetKit.ts#nearestStreet`, `src/world/roads/derivedSigns.ts#signApproach`, …
- `src/core/polyline.ts#Polyline.closestPoint` (method, line 330) — read by 6: `src/editor/roadTool.ts#RoadTool.down`, `src/editor/snap.ts#findAnchor`, `src/editor/roads/streetKit.ts#nearestStreet`, `src/world/roads/derivedSigns.ts#signApproach`, …
- `src/core/polyline.ts#Polyline.cum` (getter, line 81) — call by 1: `src/editor/commit.ts#reconcileMovedNode`
- `src/core/polyline.ts#Polyline.distanceTo` (method, line 359) — call by 3: `src/editor/roundabout.ts#roundaboutRefusal`, `src/world/roads/streetNames.ts#nameItemSegment`, `src/world/landscape.ts#onAnyRoad`
- `src/core/polyline.ts#Polyline.fromFlat` (method, line 65) — call by 7: `src/world/roads/footwayRise.ts#flushShareOf`, `src/world/junctionBuild.ts#buildJunctionOf.line`, `src/world/networkBuild.ts#ribbonFromBake`, `src/world/roadField.ts#unpackField`, …
- `src/core/polyline.ts#Polyline.fromPoints` (method, line 55) — call by 4: `src/world/lanelets.ts#LaneletGraph.buildSegmentLinks`, `src/world/ribbons.ts#overlapUntrimmedEnds`, `src/world/turnPaths.ts#bulbTurnPath`, `src/world/turnPaths.ts#medianUturnPath`
- `src/core/polyline.ts#Polyline.length` (getter, line 123) — call by 50: `src/editor/snap.ts#anchorHeightOffset`, `src/editor/snap.ts#findAnchor`, `src/editor/snap.ts#snapHeading`, `src/editor/streetscapeTool.ts#rowPieces`, …
- `src/core/polyline.ts#Polyline.n` (field, line 40) — read by 6: `src/world/junctionBuild.ts#endDirection`, `src/world/junctionBuild.ts#tangentAtStart`, `src/world/markings.ts#segmentMarkings`, `src/world/ribbons.ts#ribbonRing`, …
- `src/core/polyline.ts#Polyline.point` (method, line 69) — call by 4: `src/editor/snap.ts#snapHeading`, `src/world/junctionBuild.ts#smoothedDirectionFromNode`, `src/render/roadSurfaceRings.ts#medianOf`, `src/editor/commit.ts#reconcileMovedNode`
- `src/core/polyline.ts#Polyline.reversed` (method, line 214) — call by 2: `src/world/roads/derivedSigns.ts#signsAtNode`, `src/world/junctionBuild.ts#legLine`
- `src/core/polyline.ts#Polyline.sampleAt` (method, line 152) — call by 31: `src/editor/snap.ts#snapEndpoint`, `src/editor/snap.ts#snapHeading`, `src/editor/streetscapeTool.ts#rowPieces`, `src/editor/streetObjects.ts#commitPedestrianCrossing`, …
- `src/core/polyline.ts#Polyline.sampleAtInto` (method, line 157) — call by 4: `src/world/roads/footwayRise.ts#flushHolds`, `src/world/roadField.ts#roadField.frameInto`, `src/world/roadField.ts#roadField.roadAt`, `src/world/roadField.ts#roadField.roadAtInto`
- `src/core/polyline.ts#Polyline.sub` (method, line 198) — call by 4: `src/world/lanelets.ts#LaneletGraph.buildSegmentLinks`, `src/world/markings.ts#segmentMarkings`, `src/world/ribbons.ts#ribbonParts`, `src/render/roadSurfaceRings.ts#medianOf`
- `src/core/polyline.ts#Polyline.toPoints` (method, line 74) — call by 9: `src/editor/snap.ts#snapEndpoint`, `src/world/roads/laneEnds.ts#laneEnds`, `src/world/lanelets.ts#LaneletGraph.buildSegmentLinks`, `src/world/markings.ts#segmentMarkings`, …
- `src/core/polyline.ts#Polyline.xy` (field, line 38) — read by 12: `src/world/roads/footwayRise.ts#packFlush`, `src/world/junctionBuild.ts#buildJunctionOf`, `src/world/junctionBuild.ts#junctionAsk`, `src/world/junctionBuild.ts#junctionAsk.lane`, …

### src/core/ring.ts

- `src/core/ring.ts#AREA,MIN_X,MIN_Y,MAX_X,MAX_Y,START,EDGES` (variable, line 20) — read by 3: `src/core/ring.ts#Ring.signedArea`, `src/core/ring.ts#Ring.edges`, `src/core/ring.ts#Ring.start`
- `src/core/ring.ts#LINE_SIZE,ARC_SIZE` (variable, line 24) — read by 1: `src/core/ring.ts#Ring.edges`
- `src/core/ring.ts#LINE,ARC_CCW,ARC_CW` (variable, line 22) — read by 1: `src/core/ring.ts#Ring.edges`
- `src/core/ring.ts#Ring.bbox` (getter, line 123) — call by 4: `src/world/junction/levels.ts#boundsOf`, `src/world/carriageway.ts#onRoadGround`, `src/world/ribbons.ts#boundsOf`, `src/world/ribbons.ts#junctionBounds`
- `src/core/ring.ts#Ring.count` (field, line 34) — read by 2: `src/core/ring.ts#Ring.reversed`, `src/core/ring.ts#Ring.run`
- `src/core/ring.ts#Ring.data` (field, line 33) — read by 4: `src/core/ring.ts#Ring.signedArea`, `src/core/ring.ts#Ring.edges`, `src/core/ring.ts#Ring.start`, `src/core/ring.ts#Ring.run`
- `src/core/ring.ts#Ring.flatten` (method, line 89) — call by 10: `src/world/junction/bulb.ts#bulbLine`, `src/world/junction/bulb.ts#clearBulbsOf`, `src/world/junction/derive.ts#deriveFromLegs`, `src/world/junction/polygon.ts#buildJunctionRing`, …
- `src/core/ring.ts#Ring.flatXY` (method, line 98) — call by 2: `src/world/junctionBuild.ts#junctionAsk`, `src/core/ring.ts#Ring.signedArea`
- `src/core/ring.ts#Ring.isEmpty` (getter, line 84) — call by 10: `src/world/junction/bulb.ts#clearBulbsOf`, `src/world/junction/derive.ts#derivable`, `src/world/junction/levels.ts#boundsOf`, `src/world/surfaces.ts#levelRingsOf`, …

### src/core/rng.ts

- `src/core/rng.ts#Rng` (class, line 6) — construct by 3: `src/render/streetFurnitureLayer.ts#furniturePlace`, `src/world/cityGen/plan.ts#planCity`, `src/render/grass.ts#grassClump`
- `src/core/rng.ts#Rng` (class, line 6) — typeRef by 1: `src/world/cityGen/streets.ts#traceStreets`
- `src/core/rng.ts#Rng.float` (method, line 57) — call by 3: `src/render/streetFurnitureLayer.ts#furniturePlace`, `src/world/cityGen/plan.ts#planCity`, `src/render/grass.ts#grassClump`
- `src/core/rng.ts#Rng.range` (method, line 62) — call by 2: `src/world/cityGen/streets.ts#traceStreets`, `src/world/cityGen/plan.ts#planCity`

### src/core/scalar.ts

- `src/core/scalar.ts#ACUTE_EPS` (variable, line 27) — read by 1: `src/world/junction/corners.ts#resolveCorner`
- `src/core/scalar.ts#clamp` (function, line 58) — call by 3: `src/editor/roadTool.ts#RoadTool.curveOf`, `src/world/junction/corners.ts#resolveCorner`, `src/core/fillet.ts#curbRadius`
- `src/core/scalar.ts#COARSE_EPS` (variable, line 12) — read by 9: `src/editor/nodeMover.ts#NodeMover.drop`, `src/world/junction/build.ts#buildJunction`, `src/world/junction/build.ts#outlinesMeet`, `src/world/junction/corners.ts#resolveCorner`, …
- `src/core/scalar.ts#CORNER_MITER` (variable, line 33) — read by 1: `src/world/junction/corners.ts#resolveCorner`
- `src/core/scalar.ts#EPS` (variable, line 6) — read by 1: `src/render/roadSurfaceReads.ts#passSurfaces.fromFootway`
- `src/core/scalar.ts#FINE_EPS` (variable, line 15) — read by 6: `src/world/junction/build.ts#buildJunction`, `src/world/junction/derive.ts#deriveFromLegs`, `src/world/junction/derive.ts#deriveFromLegs.push`, `src/world/junction/polygon.ts#arcReach`, …
- `src/core/scalar.ts#GEO_EPS` (variable, line 9) — read by 4: `src/render/paintQuads.ts#quad`, `src/render/paintQuads.ts#strokeQuads`, `src/world/turnPaths.ts#medianUturnPath`, `src/core/intersect.ts#inForwardSlab`
- `src/core/scalar.ts#MIN_RIBBON` (variable, line 45) — read by 6: `src/world/junction/trim.ts#clampSegmentTrims`, `src/world/network.ts#Network.crosswalkDistanceAt`, `src/world/networkBuild.ts#NetworkBuild.reconcileSegment`, `src/world/lanelets.ts#LaneletGraph.buildSegmentLinks`, …
- `src/core/scalar.ts#normalizeAngle` (function, line 67) — call by 7: `src/editor/snap.ts#onGridAlong`, `src/editor/snap.ts#snapHeading`, `src/world/junction/bulb.ts#outline`, `src/world/junction/derive.ts#deriveFromLegs`, …
- `src/core/scalar.ts#SEAM_OVERLAP` (variable, line 48) — read by 1: `src/world/ribbons.ts#ribbonParts`
- `src/core/scalar.ts#SIN_EPS` (variable, line 24) — read by 2: `src/world/junction/corners.ts#resolveCorner`, `src/world/junction/derive.ts#deriveFromLegs`
- `src/core/scalar.ts#SURFACE_END_STEP` (variable, line 51) — read by 1: `src/world/ribbons.ts#ribbonParts`
- `src/core/scalar.ts#TAPER_ANGLE` (variable, line 42) — read by 1: `src/world/junction/corners.ts#resolveCorner`

### src/core/units.ts

- `src/core/units.ts#kmh` (function, line 19) — call by 7: `src/world/roads/derivedSigns.ts#signsAtNode`, `src/world/roads/profile.ts#profileOf`, `src/world/roadTypes.ts#ROAD_TYPES`, `src/world/roadTypes.ts#sectionFromProfile`, …
- `src/core/units.ts#m` (function, line 6) — call by 161: `src/editor/roadTool.ts#FURNITURE_CELL`, `src/editor/roadTool.ts#landscapeAlong`, `src/editor/roadTool.ts#pushSample`, `src/editor/roadTool.ts#RoadTool.down`, …
- `src/core/units.ts#METERS_PER_UNIT` (variable, line 2) — read by 9: `src/editor/roadTool.ts#RoadTool.layPart`, `src/world/roads/streetNames.ts#streetNumbers`, `src/ui/roads/crossSection.ts#drawSection`, `src/ui/roads/crossSection.ts#metresText`, …
- `src/core/units.ts#stepsIn` (function, line 15) — call by 2: `src/world/turnPaths.ts#bulbTurnPath`, `src/world/turnPaths.ts#medianUturnPath`
- `src/core/units.ts#UNITS_PER_METER` (variable, line 3) — read by 6: `src/editor/roadTool.ts#RoadTool.stepHeight`, `src/ui/roads/catalogPanel.ts#catalogStrip`, `src/ui/roads/junctionPanel.ts#mountJunctionPanel`, `src/ui/roads/profileEditor.ts#newElement`, …

### src/core/vec2.ts

- `src/core/vec2.ts#add` (function, line 9) — call by 3: `src/world/junction/bulb.ts#buildBulb.at`, `src/world/junction/bulb.ts#outline.at`, `src/world/junction/bulb.ts#outline.touch`
- `src/core/vec2.ts#addScaled` (function, line 43) — call by 12: `src/editor/snap.ts#onGridAlong`, `src/editor/snap.ts#rayHitsPolyline`, `src/editor/snap.ts#snapEndpoint`, `src/world/junction/corners.ts#resolveCorner`, …
- `src/core/vec2.ts#angleOf` (function, line 35) — call by 11: `src/editor/snap.ts#snapEndpoint`, `src/editor/snap.ts#snapHeading`, `src/editor/snap.ts#snapRoadEndpoint`, `src/world/junction/bulb.ts#outline`, …
- `src/core/vec2.ts#cross` (function, line 16) — call by 1: `src/core/intersect.ts#inForwardSlab`
- `src/core/vec2.ts#dist` (function, line 20) — call by 16: `src/editor/roadTool.ts#RoadTool.bend`, `src/editor/roadTool.ts#RoadTool.preview`, `src/editor/roadTool.ts#RoadTool.up`, `src/editor/roadPath.ts#distanceToChord`, …
- `src/core/vec2.ts#dot` (function, line 14) — call by 9: `src/world/junction/build.ts#bendAtNode`, `src/world/junction/build.ts#buildJunction`, `src/world/junction/build.ts#surfaceMode`, `src/world/junction/corners.ts#project`, …
- `src/core/vec2.ts#normalize` (function, line 25) — call by 3: `src/editor/snap.ts#snapHeading`, `src/world/junctionBuild.ts#smoothedDirectionFromNode`, `src/world/approachGroups.ts#computeApproachGroups`
- `src/core/vec2.ts#perp` (function, line 23) — call by 1: `src/world/geometry.ts#frameFromNode`
- `src/core/vec2.ts#scale` (function, line 11) — call by 4: `src/world/junction/bulb.ts#buildBulb.at`, `src/world/junction/bulb.ts#outline`, `src/world/junction/bulb.ts#outline.at`, `src/world/junction/bulb.ts#outline.touch`
- `src/core/vec2.ts#sub` (function, line 10) — call by 9: `src/editor/snap.ts#snapEndpoint`, `src/editor/snap.ts#snapHeading`, `src/editor/snap.ts#snapRoadEndpoint`, `src/world/junction/bulb.ts#outline`, …
- `src/core/vec2.ts#Vec2` (interface, line 4) — read by 112: `src/editor/roadTool.ts#pushSample`, `src/editor/roadTool.ts#RoadTool.curveOf`, `src/editor/roadTool.ts#RoadTool.down`, `src/editor/roadTool.ts#RoadTool.draftKey`, …
- `src/core/vec2.ts#Vec2` (interface, line 4) — typeRef by 161: `src/editor/roadTool.ts#CurvePending`, `src/editor/roadTool.ts#pushSample`, `src/editor/roadTool.ts#RoadDraft`, `src/editor/roadTool.ts#RoadTool.bend`, …

### src/editor/commit.ts

- `src/editor/commit.ts#commitRoadPath` (function, line 63) — call by 2: `src/editor/roadTool.ts#RoadTool.judge`, `src/editor/roadTool.ts#RoadTool.layPart`
- `src/editor/commit.ts#commitRoadPath` (function, line 63) — typeRef by 1: `src/editor/roadTool.ts#RoadTool.layPart`
- `src/editor/commit.ts#CROSSING_CLEARANCE` (variable, line 40) — read by 1: `src/editor/commit.ts#reconcileMovedNode`
- `src/editor/commit.ts#DraftResult` (interface, line 44) — read by 4: `src/editor/roadTool.ts#RoadTool.judge`, `src/editor/roadTool.ts#RoadTool.layPart`, `src/editor/nodeMover.ts#NodeMover.drop`, `src/editor/commit.ts#moveNodeChecked`
- `src/editor/commit.ts#DraftResult` (interface, line 44) — typeRef by 3: `src/editor/nodeMover.ts#NodeMover.drop`, `src/editor/commit.ts#moveNodeChecked`, `src/editor/commit.ts#reconcileMovedNode`
- `src/editor/commit.ts#draftStations` (function, line 254) — indirectCall by 1: `src/world/roads/buildMode.ts#stationsAlong`
- `src/editor/commit.ts#HEIGHT_JOIN_EPS` (variable, line 38) — read by 1: `src/editor/commit.ts#reconcileMovedNode`
- `src/editor/commit.ts#MERGE_EPS` (variable, line 34) — read by 1: `src/editor/commit.ts#reconcileMovedNode`
- `src/editor/commit.ts#segmentOffsetAt` (function, line 654) — call by 1: `src/editor/commit.ts#reconcileMovedNode`
- `src/editor/commit.ts#solveOver` (function, line 204) — indirectCall by 1: `src/world/elevation.ts#buildRoadElevation`
- `src/editor/commit.ts#splitSegment` (function, line 662) — call by 2: `src/editor/streetObjects.ts#commitPedestrianCrossing`, `src/editor/streetObjects.ts#commitUturn`
- `src/editor/commit.ts#splitSegmentAtCuts` (function, line 856) — call by 1: `src/editor/commit.ts#reconcileMovedNode`

### src/editor/editRules.ts

- `src/editor/editRules.ts#refuseRoadEdit` (function, line 43) — call by 1: `src/editor/commit.ts#moveNodeChecked`
- `src/editor/editRules.ts#RoadEditRefusal` (type, line 17) — typeRef by 1: `src/editor/roadTool.ts#RefusalReason`

### src/editor/poles.ts

- `src/editor/poles.ts#commitPoleRunPoles` (function, line 373) — call by 1: `src/editor/roads/powerLine.ts#layPowerLinesSteps`
- `src/editor/poles.ts#planPoleRun` (function, line 240) — call by 1: `src/editor/roads/powerLine.ts#runPlanner`
- `src/editor/poles.ts#PoleRunPlan` (interface, line 74) — typeRef by 1: `src/editor/roads/powerLine.ts#PlanRun`
- `src/editor/poles.ts#poleRunPlanner` (function, line 160) — call by 1: `src/editor/roads/powerLine.ts#runPlanner`
- `src/editor/poles.ts#PoleRunPlanner` (interface, line 155) — call by 1: `src/editor/roads/powerLine.ts#runPlanner`
- `src/editor/poles.ts#poleRunPlanner.plan` (method, line 177) — implementation by 1: `src/editor/roads/powerLine.ts#runPlanner`

### src/jobs/jobPool.ts

- `src/jobs/jobPool.ts#canQueueJobs` (function, line 534) — call by 1: `src/render/roadStations.ts#RoadStations.waiting`
- `src/jobs/jobPool.ts#jobLanes` (function, line 286) — call by 2: `src/render/roadTilePool.ts#RoadTilePool.start`, `src/render/roadTilePool.ts#RoadTilePool.startCoarse`
- `src/jobs/jobPool.ts#jobPort` (function, line 669) — call by 2: `src/render/roadTilePool.ts#RoadTilePool.open`, `src/render/networkFarRoads.ts#NetworkFarRoads.request`
- `src/jobs/jobPool.ts#JobTicket` (interface, line 61) — read by 1: `src/render/roadStations.ts#RoadStations.waiting`
- `src/jobs/jobPool.ts#JobTicket` (interface, line 61) — typeRef by 1: `src/render/roadStations.ts#RoadStations.asked`
- `src/jobs/jobPool.ts#submitJob` (function, line 484) — call by 1: `src/render/roadStations.ts#RoadStations.waiting`
- `src/jobs/jobPool.ts#system` (variable, line 249) — read by 1: `src/jobs/jobPool.ts#laneFirst`

### src/jobs/jobScope.ts

- `src/jobs/jobScope.ts#JobService` (type, line 8) — typeRef by 2: `src/jobs/jobPool.ts#laneFirst`, `src/jobs/jobPool.ts#JobSystem.firstLanes`

### src/lab/catalog/layers.ts

- `src/lab/catalog/layers.ts#signItems.flat` (function, line 77) — indirectCall by 1: `src/world/elevation.ts#buildRoadElevation`

### src/lab/catalog/layout.ts

- `src/lab/catalog/layout.ts#layoutItems.flat` (function, line 166) — indirectCall by 1: `src/world/elevation.ts#buildRoadElevation`

### src/main.ts

- `src/main.ts#mover` (variable, line 569) — implementation by 3: `src/editor/nodeMover.ts#NodeMover.cancel`, `src/editor/nodeMover.ts#NodeMover.drop`, `src/editor/nodeMover.ts#NodeMover.move`
- `src/main.ts#roadTool` (variable, line 458) — implementation by 16: `src/editor/roadTool.ts#RoadTool.anchorHeight`, `src/editor/roadTool.ts#RoadTool.bend`, `src/editor/roadTool.ts#RoadTool.curveOf`, `src/editor/roadTool.ts#RoadTool.down`, …
- `src/main.ts#showInspector` (function, line 3573) — callbackAssign by 6: `src/ui/inspector.ts#junctionHost.setMovementBlocked`, `src/ui/inspector.ts#junctionHost.setSignal`, `src/ui/inspector.ts#junctionHost.setControl`, `src/ui/inspector.ts#junctionHost.setHeight`, …
- `src/main.ts#showInspector` (function, line 3573) — implementation by 6: `src/ui/inspector.ts#junctionHost.setMovementBlocked`, `src/ui/inspector.ts#junctionHost.setSignal`, `src/ui/inspector.ts#junctionHost.setControl`, `src/ui/inspector.ts#junctionHost.setHeight`, …
- `src/main.ts#streetscapeTool` (variable, line 502) — implementation by 4: `src/editor/streetscapeTool.ts#StreetscapeTool.down`, `src/editor/streetscapeTool.ts#StreetscapeTool.move`, `src/editor/streetscapeTool.ts#StreetscapeTool.reach`, `src/editor/streetscapeTool.ts#StreetscapeTool.up`

### src/render/buildings/gpuBatchedMesh.ts

- `src/render/buildings/gpuBatchedMesh.ts#GpuBatchedMesh` (class, line 69) — construct by 1: `src/render/networkFarRoads.ts#NetworkFarRoads.place`
- `src/render/buildings/gpuBatchedMesh.ts#GpuBatchedMesh` (class, line 69) — typeRef by 2: `src/render/networkFarRoads.ts#NetworkFarRoads.clearMeshes`, `src/render/networkFarRoads.ts#NetworkFarRoads.meshesOf`
- `src/render/buildings/gpuBatchedMesh.ts#GpuBatchedMesh.dispose` (method, line 396) — call by 1: `src/render/networkFarRoads.ts#NetworkFarRoads.clearMeshes`

### src/render/depthConvention.ts

- `src/render/depthConvention.ts#pullTowardEye` (function, line 28) — call by 3: `src/render/roadStandIn.ts#standInMaterial`, `src/render/markings.ts#paintMaterial`, `src/render/networkFarRoads.ts#layerMaterial`

### src/render/gpuLedger.ts

- `src/render/gpuLedger.ts#geometryCost` (function, line 73) — call by 2: `src/render/mesh/roadResidency.ts#fullBytes`, `src/render/mesh/roadResidency.ts#updateRoadLevels`
- `src/render/gpuLedger.ts#geometryCost` (function, line 73) — read by 2: `src/render/mesh/roadResidency.ts#fullBytes`, `src/render/mesh/roadResidency.ts#updateRoadLevels`

### src/render/instanceRuns.ts

- `src/render/instanceRuns.ts#createInstanceRuns` (function, line 201) — call by 1: `src/render/streetFurnitureLayer.ts#createStreetFurniture`
- `src/render/instanceRuns.ts#createInstanceRuns.cull` (method, line 214) — implementation by 1: `src/render/streetFurnitureLayer.ts#createStreetFurniture.cull`
- `src/render/instanceRuns.ts#createInstanceRuns.dispose` (method, line 215) — implementation by 1: `src/render/streetFurnitureLayer.ts#createStreetFurniture.dispose`
- `src/render/instanceRuns.ts#createInstanceRuns.update` (method, line 209) — implementation by 1: `src/render/streetFurnitureLayer.ts#createStreetFurniture.sync`
- `src/render/instanceRuns.ts#InstanceRuns` (interface, line 170) — call by 3: `src/render/streetFurnitureLayer.ts#createStreetFurniture.cull`, `src/render/streetFurnitureLayer.ts#createStreetFurniture.dispose`, `src/render/streetFurnitureLayer.ts#createStreetFurniture.sync`
- `src/render/instanceRuns.ts#InstanceRuns` (interface, line 170) — read by 1: `src/render/streetFurnitureLayer.ts#createStreetFurniture`
- `src/render/instanceRuns.ts#kindOf` (function, line 185) — read by 1: `src/render/streetFurnitureLayer.ts#createStreetFurniture`
- `src/render/instanceRuns.ts#Run` (type, line 100) — typeRef by 3: `src/render/streetFurnitureLayer.ts#createStreetFurniture`, `src/render/streetFurnitureLayer.ts#createStreetFurniture.sync`, `src/render/streetFurnitureLayer.ts#MeadowPlace`
- `src/render/instanceRuns.ts#RunBuilder` (class, line 111) — construct by 2: `src/render/streetFurnitureLayer.ts#furnitureAbout`, `src/render/streetFurnitureLayer.ts#meadowProduct`
- `src/render/instanceRuns.ts#RunBuilder` (class, line 111) — typeRef by 2: `src/render/streetFurnitureLayer.ts#meadowInto`, `src/render/streetFurnitureLayer.ts#placeInto`
- `src/render/instanceRuns.ts#RunBuilder.build` (method, line 137) — call by 2: `src/render/streetFurnitureLayer.ts#furnitureAbout`, `src/render/streetFurnitureLayer.ts#meadowProduct`
- `src/render/instanceRuns.ts#RunBuilder.turned` (method, line 132) — call by 2: `src/render/streetFurnitureLayer.ts#meadowInto.add`, `src/render/streetFurnitureLayer.ts#placeInto`
- `src/render/instanceRuns.ts#RunStore` (interface, line 69) — typeRef by 6: `src/render/streetFurnitureLayer.ts#furnitureAbout`, `src/render/streetFurnitureLayer.ts#furnitureStores`, `src/render/streetFurnitureLayer.ts#furnitureStores.plant`, `src/render/streetFurnitureLayer.ts#furnitureStores.prop`, …
- `src/render/instanceRuns.ts#sameRun` (function, line 152) — call by 1: `src/render/streetFurnitureLayer.ts#meadowProduct`
- `src/render/instanceRuns.ts#warmMeshes` (function, line 196) — call by 1: `src/render/streetFurnitureLayer.ts#createStreetFurniture.warm`

### src/render/materials.ts

- `src/render/materials.ts#SceneMaterials` (interface, line 11) — read by 3: `src/render/roadSurfaces.ts#passLook`, `src/render/roadSurfaces.ts#roadSurfaceSteps.paintFor`, `src/render/roadSurfaces.ts#surfaceLook`
- `src/render/materials.ts#SceneMaterials` (interface, line 11) — typeRef by 7: `src/render/roadSurfaces.ts#buildRoadSurfaces`, `src/render/roadSurfaces.ts#dress`, `src/render/roadSurfaces.ts#passLook`, `src/render/roadSurfaces.ts#roadSurfaceSteps`, …

### src/render/mesh/farBlockBatches.ts

- `src/render/mesh/farBlockBatches.ts#ADOPTED` (variable, line 27) — read by 2: `src/render/mesh/farBlockBatches.ts#batchedFar`, `src/render/mesh/farBlockBatches.ts#FarBlockBatches.adopt`
- `src/render/mesh/farBlockBatches.ts#FarBlockBatches` (class, line 51) — typeRef by 1: `src/render/roadSurfaces.ts#SurfaceReuse`
- `src/render/mesh/farBlockBatches.ts#FarBlockBatches.entries` (field, line 54) — read by 1: `src/render/mesh/farBlockBatches.ts#FarBlockBatches.adopt`
- `src/render/mesh/farBlockBatches.ts#FarBlockBatches.entries` (field, line 54) — write by 1: `src/render/mesh/farBlockBatches.ts#FarBlockBatches.adopt`

### src/render/mesh/gpuOnly.ts

- `src/render/mesh/gpuOnly.ts#fillArrays` (function, line 37) — call by 1: `src/render/mesh/roadResidency.ts#makeNear`
- `src/render/mesh/gpuOnly.ts#GeometryArrays` (type, line 6) — typeRef by 3: `src/render/mesh/roadLod.ts#BlockLevels`, `src/render/mesh/surfaceTiles.ts#attachFarLevel`, `src/render/mesh/surfaceTiles.ts#restoreFarLevel`
- `src/render/mesh/gpuOnly.ts#gpuOnly` (function, line 45) — call by 3: `src/render/mesh/surfaceTiles.ts#attachFarLevel`, `src/render/mesh/surfaceTiles.ts#restoreFarLevel`, `src/render/mesh/surfaceTiles.ts#tilesOnGpu`

### src/render/mesh/textureBaker.ts

- `src/render/mesh/textureBaker.ts#fbm` (function, line 32) — call by 5: `src/render/bake/roadRecipes.ts#asphaltRecipe.shade`, `src/render/bake/roadRecipes.ts#deckRecipe.shade`, `src/render/bake/roadRecipes.ts#footwayRecipe.shade`, `src/render/bake/roadRecipes.ts#kerbRecipe.shade`, …
- `src/render/mesh/textureBaker.ts#makeNoise` (function, line 4) — call by 5: `src/render/bake/roadRecipes.ts#asphaltRecipe`, `src/render/bake/roadRecipes.ts#deckRecipe`, `src/render/bake/roadRecipes.ts#footwayRecipe`, `src/render/bake/roadRecipes.ts#kerbRecipe`, …
- `src/render/mesh/textureBaker.ts#SurfaceRecipe` (interface, line 81) — typeRef by 6: `src/render/bake/roadRecipes.ts#asphaltRecipe`, `src/render/bake/roadRecipes.ts#deckRecipe`, `src/render/bake/roadRecipes.ts#footwayRecipe`, `src/render/bake/roadRecipes.ts#kerbRecipe`, …
- `src/render/mesh/textureBaker.ts#SurfaceRecipe` (interface, line 81) — write by 5: `src/render/bake/roadRecipes.ts#asphaltRecipe.shade`, `src/render/bake/roadRecipes.ts#deckRecipe.shade`, `src/render/bake/roadRecipes.ts#footwayRecipe.shade`, `src/render/bake/roadRecipes.ts#kerbRecipe.shade`, …

### src/render/mesh/tileMesh.ts

- `src/render/mesh/tileMesh.ts#Tile` (interface, line 158) — read by 4: `src/render/roadTileMesh.ts#tileBuffers`, `src/render/mesh/surfaceTiles.ts#memberSizes`, `src/render/mesh/surfaceTiles.ts#mergeTiles`, `src/render/mesh/surfaceTiles.ts#tileArrays`
- `src/render/mesh/tileMesh.ts#Tile` (interface, line 158) — typeRef by 15: `src/render/roadSurfaces.ts#roadSurfaceSteps`, `src/render/roadSurfaces.ts#roadSurfaceSteps.ask`, `src/render/roadSurfaces.ts#surfaceSource`, `src/render/roadSurfaces.ts#TileBundle`, …

### src/render/plantImpostors.ts

- `src/render/plantImpostors.ts#createPlantImpostors.attach` (method, line 444) — implementation by 1: `src/render/streetFurnitureLayer.ts#furnitureStores.plant`
- `src/render/plantImpostors.ts#createPlantImpostors.ready` (method, line 443) — implementation by 1: `src/render/streetFurnitureLayer.ts#furnitureStores.plant`
- `src/render/plantImpostors.ts#createPlantImpostors.setView` (method, line 439) — implementation by 1: `src/render/streetFurnitureLayer.ts#furnitureStores.plant`
- `src/render/plantImpostors.ts#PICTURE_PX` (variable, line 467) — read by 1: `src/render/streetFurnitureLayer.ts#furnitureStores.plant`
- `src/render/plantImpostors.ts#PlantImpostors` (interface, line 255) — call by 1: `src/render/streetFurnitureLayer.ts#furnitureStores.plant`
- `src/render/plantImpostors.ts#PlantImpostors` (interface, line 255) — read by 1: `src/render/streetFurnitureLayer.ts#furnitureStores.plant`
- `src/render/plantImpostors.ts#PlantImpostors` (interface, line 255) — reference by 1: `src/render/streetFurnitureLayer.ts#furnitureStores.plant`
- `src/render/plantImpostors.ts#sceneryPictures` (function, line 487) — call by 1: `src/render/streetFurnitureLayer.ts#createStreetFurniture`
- `src/render/plantImpostors.ts#SceneryPictures` (interface, line 470) — call by 3: `src/render/streetFurnitureLayer.ts#createStreetFurniture`, `src/render/streetFurnitureLayer.ts#createStreetFurniture.dispose`, `src/render/streetFurnitureLayer.ts#furnitureStores`
- `src/render/plantImpostors.ts#SceneryPictures` (interface, line 470) — read by 2: `src/render/streetFurnitureLayer.ts#createStreetFurniture`, `src/render/streetFurnitureLayer.ts#furnitureStores`
- `src/render/plantImpostors.ts#SceneryPictures` (interface, line 470) — typeRef by 1: `src/render/streetFurnitureLayer.ts#furnitureStores`
- `src/render/plantImpostors.ts#sceneryPictures.bushAt` (method, line 502) — implementation by 2: `src/render/streetFurnitureLayer.ts#createStreetFurniture`, `src/render/streetFurnitureLayer.ts#furnitureStores`
- `src/render/plantImpostors.ts#sceneryPictures.release` (method, line 503) — implementation by 1: `src/render/streetFurnitureLayer.ts#createStreetFurniture.dispose`
- `src/render/plantImpostors.ts#sceneryPictures.treeAt` (method, line 501) — implementation by 2: `src/render/streetFurnitureLayer.ts#createStreetFurniture`, `src/render/streetFurnitureLayer.ts#furnitureStores`

### src/render/propCells.ts

- `src/render/propCells.ts#createPropLayer` (function, line 254) — call by 1: `src/render/streetFurnitureLayer.ts#createStreetFurniture`
- `src/render/propCells.ts#createPropLayer` (function, line 254) — read by 1: `src/render/propCells.ts#createPropLayer.forgetGrowing`
- `src/render/propCells.ts#createPropLayer.cull` (method, line 591) — implementation by 1: `src/render/streetFurnitureLayer.ts#createStreetFurniture.cull`
- `src/render/propCells.ts#createPropLayer.dispose` (method, line 596) — implementation by 1: `src/render/streetFurnitureLayer.ts#createStreetFurniture.dispose`
- `src/render/propCells.ts#createPropLayer.follow` (method, line 511) — implementation by 1: `src/render/streetFurnitureLayer.ts#createStreetFurniture.sync`
- `src/render/propCells.ts#createPropLayer.setMap` (method, line 590) — implementation by 1: `src/render/streetFurnitureLayer.ts#createStreetFurniture.setMap`
- `src/render/propCells.ts#PropLayer` (interface, line 187) — call by 4: `src/render/streetFurnitureLayer.ts#createStreetFurniture.cull`, `src/render/streetFurnitureLayer.ts#createStreetFurniture.dispose`, `src/render/streetFurnitureLayer.ts#createStreetFurniture.setMap`, `src/render/streetFurnitureLayer.ts#createStreetFurniture.sync`
- `src/render/propCells.ts#PropLayer` (interface, line 187) — read by 2: `src/render/streetFurnitureLayer.ts#createStreetFurniture`, `src/render/streetFurnitureLayer.ts#createStreetFurniture.sync`
- `src/render/propCells.ts#PropLayer` (interface, line 187) — typeRef by 1: `src/render/streetFurnitureLayer.ts#StreetFurnitureLayer`
- `src/render/propCells.ts#PropLayer` (interface, line 187) — write by 1: `src/render/streetFurnitureLayer.ts#createStreetFurniture.sync`
- `src/render/propCells.ts#PropPlacement` (interface, line 137) — typeRef by 1: `src/render/streetFurnitureLayer.ts#furnitureAbout`

### src/render/propGeometry.ts

- `src/render/propGeometry.ts#BUSH_KINDS` (variable, line 158) — read by 3: `src/render/streetFurnitureLayer.ts#bushStore`, `src/render/streetFurnitureLayer.ts#createStreetFurniture`, `src/render/streetFurnitureLayer.ts#furnitureStores`
- `src/render/propGeometry.ts#BushKind` (type, line 157) — typeRef by 2: `src/render/streetFurnitureLayer.ts#bushStore`, `src/render/streetFurnitureLayer.ts#furniturePlace`
- `src/render/propGeometry.ts#LAMP_HEIGHT` (variable, line 233) — read by 1: `src/render/streetFurnitureLayer.ts#furniturePlace`
- `src/render/propGeometry.ts#LAMP_OUTREACH` (variable, line 234) — read by 1: `src/render/streetFurnitureLayer.ts#furniturePlace`
- `src/render/propGeometry.ts#TREE_SPECIES` (variable, line 145) — read by 4: `src/render/streetFurnitureLayer.ts#bushStore`, `src/render/streetFurnitureLayer.ts#createStreetFurniture`, `src/render/streetFurnitureLayer.ts#furnitureStores`, `src/render/streetFurnitureLayer.ts#treeStore`
- `src/render/propGeometry.ts#TreeSpecies` (type, line 144) — typeRef by 2: `src/render/streetFurnitureLayer.ts#streetSpecies`, `src/render/streetFurnitureLayer.ts#treeStore`

### src/render/renderer.ts

- `src/render/renderer.ts#createSceneRenderer` (function, line 432) — callbackAssign by 1: `src/render/roadSurfaces.ts#roadSurfaceSteps`
- `src/render/renderer.ts#createSceneRenderer` (function, line 432) — implementation by 2: `src/render/roadSurfaces.ts#roadSurfaceSteps`, `src/render/roadSurfaces.ts#roadSurfaceSteps.ask`
- `src/render/renderer.ts#createSceneRenderer.buildWorld` (function, line 2317) — indirectCall by 1: `src/world/elevation.ts#buildRoadElevation`
- `src/render/renderer.ts#createSceneRenderer.completeSolve` (function, line 2225) — indirectCall by 1: `src/world/elevation.ts#buildRoadElevation`
- `src/render/renderer.ts#createSceneRenderer.footwayRiseHere` (function, line 1267) — indirectCall by 1: `src/world/paving.ts#pavedHeightFrom`
- `src/render/renderer.ts#createSceneRenderer.warmRoadShaders` (function, line 2506) — indirectCall by 1: `src/world/elevation.ts#buildRoadElevation`

### src/render/roadStations.worker.ts

- `src/render/roadStations.worker.ts#RoadStationsJob` (interface, line 7) — typeRef by 1: `src/render/roadStations.ts#RoadStations.waiting`
- `src/render/roadStations.worker.ts#RoadStationsReply` (interface, line 11) — read by 1: `src/render/roadStations.ts#RoadStations.waiting`
- `src/render/roadStations.worker.ts#RoadStationsReply` (interface, line 11) — typeRef by 1: `src/render/roadStations.ts#RoadStations.waiting`

### src/render/roadTile.worker.ts

- `src/render/roadTile.worker.ts#handler.beat` (function, line 49) — indirectCall by 1: `src/render/roadTileMesh.ts#TileMesher.run`

### src/render/scenery.ts

- `src/render/scenery.ts#foliageTint` (function, line 373) — call by 1: `src/render/streetFurnitureLayer.ts#furniturePlace`
- `src/render/scenery.ts#LAMP_BEAM_RADIUS` (variable, line 66) — read by 1: `src/render/streetFurnitureLayer.ts#furniturePlace`
- `src/render/scenery.ts#LAMP_POOL_RADIUS` (variable, line 61) — read by 1: `src/render/streetFurnitureLayer.ts#furniturePlace`
- `src/render/scenery.ts#POOL_ALONG` (variable, line 63) — read by 1: `src/render/streetFurnitureLayer.ts#furniturePlace`
- `src/render/scenery.ts#SceneryKit` (interface, line 131) — read by 4: `src/render/streetFurnitureLayer.ts#createStreetFurniture`, `src/render/streetFurnitureLayer.ts#furnitureProduct`, `src/render/streetFurnitureLayer.ts#furnitureStores`, `src/render/streetFurnitureLayer.ts#MEADOW_STORES`
- `src/render/scenery.ts#SceneryKit` (interface, line 131) — typeRef by 5: `src/render/streetFurnitureLayer.ts#createStreetFurniture`, `src/render/streetFurnitureLayer.ts#furnitureProduct`, `src/render/streetFurnitureLayer.ts#furnitureStores`, `src/render/streetFurnitureLayer.ts#MEADOW_STORES`, …
- `src/render/scenery.ts#SHADOW_REACH` (variable, line 363) — read by 1: `src/render/streetFurnitureLayer.ts#createStreetFurniture`

### src/render/sharedGround.ts

- `src/render/sharedGround.ts#SharedGroundData` (interface, line 16) — read by 2: `src/render/roadStations.ts#platesUnder`, `src/render/roadStations.ts#RoadStations.waiting.stands`
- `src/render/sharedGround.ts#SharedGroundData` (interface, line 16) — typeRef by 2: `src/render/roadStations.ts#platesUnder`, `src/render/roadStations.ts#RoadStations.waiting`

### src/render/structures.ts

- `src/render/structures.ts#buildStructureDetails` (function, line 452) — call by 1: `src/lab/catalog/layout.ts#surfacesOf`
- `src/render/structures.ts#StructureDetails` (interface, line 39) — read by 1: `src/lab/catalog/layout.ts#surfacesOf`

### src/render/terrainGrid.ts

- `src/render/terrainGrid.ts#TERRAIN_CELL` (variable, line 10) — read by 2: `src/render/roadSurfacePass.ts#GROUND_MAX_EDGE`, `src/render/roadSurfacePass.ts#RAISED_MAX_EDGE`

### src/render/uploads.ts

- `src/render/uploads.ts#cancelWarm` (function, line 455) — call by 2: `src/render/roadSurfaces.ts#roadSurfaceSteps.retire`, `src/render/mesh/roadResidency.ts#dropCell`
- `src/render/uploads.ts#warmLooseAhead` (function, line 445) — call by 2: `src/render/roadSurfaces.ts#roadSurfaceSteps`, `src/render/mesh/roadResidency.ts#restoreCell`

### src/sim/host/protocol.ts

- `src/sim/host/protocol.ts#SimAsk` (type, line 144) — typeRef by 1: `src/sim/host/simClient.ts#SimClient.ask`

### src/sim/host/simAnswers.ts

- `src/sim/host/simAnswers.ts#JunctionAnswer` (interface, line 24) — read by 3: `src/ui/roads/junctionPanel.ts#mountJunctionPanel`, `src/ui/roads/junctionPanel.ts#mountJunctionPanel.set`, `src/ui/roads/junctionPanel.ts#refreshJunctionFlows`

### src/sim/host/simClient.ts

- `src/sim/host/simClient.ts#SimClient` (class, line 84) — typeRef by 2: `src/ui/roads/junctionPanel.ts#JunctionPanelHost`, `src/ui/roads/junctionPanel.ts#refreshJunctionFlows`
- `src/sim/host/simClient.ts#SimClient.junction` (method, line 404) — call by 2: `src/ui/roads/junctionPanel.ts#mountJunctionPanel`, `src/ui/roads/junctionPanel.ts#refreshJunctionFlows`
- `src/sim/host/simClient.ts#SimClient.nextAsk` (field, line 104) — read by 1: `src/sim/host/simClient.ts#SimClient.ask`
- `src/sim/host/simClient.ts#SimClient.nextAsk` (field, line 104) — write by 1: `src/sim/host/simClient.ts#SimClient.ask`
- `src/sim/host/simClient.ts#SimClient.pending` (field, line 103) — read by 1: `src/sim/host/simClient.ts#SimClient.ask`
- `src/sim/host/simClient.ts#SimClient.pending` (field, line 103) — write by 1: `src/sim/host/simClient.ts#SimClient.ask`
- `src/sim/host/simClient.ts#SimClient.send` (method, line 423) — call by 1: `src/sim/host/simClient.ts#SimClient.ask`

### src/ui/i18n/index.ts

- `src/ui/i18n/index.ts#formatDecimal` (function, line 79) — call by 4: `src/ui/roads/crossSection.ts#metresText`, `src/ui/roads/junctionPanel.ts#fmt`, `src/ui/roads/money.ts#formatMoney`, `src/ui/roadSectionEditor.ts#mountRoadSectionEditor`
- `src/ui/i18n/index.ts#language` (function, line 50) — call by 2: `src/ui/roads/catalogPanel.ts#catalogStrip`, `src/ui/roads/money.ts#formatMoney`
- `src/ui/i18n/index.ts#plural` (function, line 73) — call by 2: `src/ui/roads/catalogPanel.ts#catalogStrip`, `src/ui/roads/profilePanel.ts#mountProfilePanel`
- `src/ui/i18n/index.ts#t` (function, line 55) — call by 27: `src/ui/roads/catalogPanel.ts#catalogStrip`, `src/ui/roads/catalogPanel.ts#customButton`, `src/ui/roads/catalogPanel.ts#nameOf`, `src/ui/roads/connectorPanel.ts#draw`, …

### src/ui/inspector.ts

- `src/ui/inspector.ts#actionsForNode` (function, line 463) — call by 6: `src/ui/inspector.ts#junctionHost.setMovementBlocked`, `src/ui/inspector.ts#junctionHost.setSignal`, `src/ui/inspector.ts#junctionHost.setControl`, `src/ui/inspector.ts#junctionHost.setHeight`, …
- `src/ui/inspector.ts#InspectorActions` (interface, line 28) — call by 6: `src/ui/inspector.ts#junctionHost.setMovementBlocked`, `src/ui/inspector.ts#junctionHost.setSignal`, `src/ui/inspector.ts#junctionHost.setControl`, `src/ui/inspector.ts#junctionHost.setHeight`, …
- `src/ui/inspector.ts#junctionHost` (function, line 447) — callbackAssign by 1: `src/ui/roads/junctionPanel.ts#mountJunctionPanel`
- `src/ui/inspector.ts#junctionHost` (function, line 447) — implementation by 1: `src/ui/roads/junctionPanel.ts#mountJunctionPanel`
- `src/ui/inspector.ts#junctionHost` (function, line 447) — read by 5: `src/ui/inspector.ts#junctionHost.setMovementBlocked`, `src/ui/inspector.ts#junctionHost.setSignal`, `src/ui/inspector.ts#junctionHost.setControl`, `src/ui/inspector.ts#junctionHost.setHeight`, …
- `src/ui/inspector.ts#renderNode` (function, line 384) — callbackAssign by 3: `src/ui/roads/connectorPanel.ts#draw`, `src/ui/roads/connectorPanel.ts#draw.press`, `src/ui/roads/connectorPanel.ts#draw.view`
- `src/ui/inspector.ts#renderNode` (function, line 384) — implementation by 3: `src/ui/roads/connectorPanel.ts#draw`, `src/ui/roads/connectorPanel.ts#draw.press`, `src/ui/roads/connectorPanel.ts#draw.view`
- `src/ui/inspector.ts#renderSegment` (function, line 210) — callbackAssign by 1: `src/ui/roads/profilePanel.ts#mountProfilePanel.hold`
- `src/ui/inspector.ts#renderSegment` (function, line 210) — implementation by 1: `src/ui/roads/profilePanel.ts#mountProfilePanel.hold`

### src/ui/labels.ts

- `src/ui/labels.ts#roadTypeName` (function, line 6) — call by 1: `src/ui/roads/profileEditor.ts#openProfileEditor.renderProps`

### src/ui/v2/shell.ts

- `src/ui/v2/shell.ts#mountShell.renderRoads` (function, line 1223) — callbackAssign by 4: `src/ui/roads/catalogPanel.ts#catalogStrip`, `src/ui/roads/catalogPanel.ts#customButton`, `src/ui/roads/catalogPanel.ts#pickedRoad`, `src/ui/roads/catalogPanel.ts#pickRoad`
- `src/ui/v2/shell.ts#mountShell.renderRoads` (function, line 1223) — implementation by 4: `src/ui/roads/catalogPanel.ts#catalogStrip`, `src/ui/roads/catalogPanel.ts#customButton`, `src/ui/roads/catalogPanel.ts#pickedRoad`, `src/ui/roads/catalogPanel.ts#pickRoad`

### src/view/cameraMotion.ts

- `src/view/cameraMotion.ts#CameraMotion` (class, line 50) — reference by 1: `src/ui/roads/keys.ts#cameraKey`
- `src/view/cameraMotion.ts#KEYS` (variable, line 7) — read by 1: `src/view/cameraMotion.ts#CameraMotion.owns`

### src/world/approach.ts

- `src/world/approach.ts#CROSSWALK_CAP` (variable, line 43) — read by 1: `src/world/network.ts#Network.crosswalkDistanceAt`
- `src/world/approach.ts#CROSSWALK_DEPTH` (variable, line 7) — read by 11: `src/editor/streetObjects.ts#commitPedestrianCrossing`, `src/world/network.ts#Network.crosswalkDistanceAt`, `src/world/network.ts#Network.stopLineDistance`, `src/world/markings.ts#crosswalkBars`, …
- `src/world/approach.ts#CROSSWALK_SETBACK` (variable, line 5) — read by 1: `src/world/approach.ts#stopLineDistance`
- `src/world/approach.ts#crosswalkDistance` (function, line 14) — call by 2: `src/world/network.ts#Network.crosswalkDistanceAt`, `src/world/markings.ts#crosswalkBars`
- `src/world/approach.ts#LEG_TRIM_CAP` (variable, line 34) — read by 1: `src/world/junction/legs.ts#buildLegs`
- `src/world/approach.ts#MIN_DRIVABLE_RESERVE` (variable, line 29) — read by 1: `src/world/junction/trim.ts#clampSegmentTrims`
- `src/world/approach.ts#MIN_LINK_LENGTH` (variable, line 26) — read by 4: `src/editor/streetObjects.ts#commitPedestrianCrossing`, `src/editor/streetObjects.ts#commitUturn`, `src/world/network.ts#Network.crosswalkDistanceAt`, `src/editor/commit.ts#reconcileMovedNode`
- `src/world/approach.ts#RESERVE_CAP` (variable, line 37) — read by 1: `src/world/junction/trim.ts#clampSegmentTrims`
- `src/world/approach.ts#STOP_BAR_SETBACK` (variable, line 9) — read by 4: `src/editor/streetObjects.ts#commitPedestrianCrossing`, `src/world/network.ts#Network.crosswalkDistanceAt`, `src/world/network.ts#Network.stopLineDistance`, `src/world/approach.ts#stopLineDistance`
- `src/world/approach.ts#STOP_BAR_WIDTH` (variable, line 11) — read by 3: `src/world/markings.ts#midBlockDetailAt`, `src/world/markings.ts#stopBar`, `src/render/paintQuads.ts#roadStrokes.cut`
- `src/world/approach.ts#STOP_LINE_CAP` (variable, line 40) — read by 1: `src/world/network.ts#Network.stopLineDistance`

### src/world/approachGroups.ts

- `src/world/approachGroups.ts#ApproachGroup` (interface, line 5) — read by 1: `src/world/junctionBuild.ts#junctionMovements`
- `src/world/approachGroups.ts#ApproachGroup` (interface, line 5) — typeRef by 5: `src/world/junctionBuild.ts#JunctionMade`, `src/world/lanelets.ts#JunctionCache`, `src/world/lanelets.ts#JunctionTopology`, `src/world/lanelets.ts#PendingJunction`, …
- `src/world/approachGroups.ts#GROUP_MERGE_DEG` (variable, line 14) — read by 1: `src/world/approachGroups.ts#computeApproachGroups`

### src/world/bounds.ts

- `src/world/bounds.ts#platesOver` (function, line 63) — call by 1: `src/render/roadStations.ts#platesUnder`

### src/world/cells.ts

- `src/world/cells.ts#CELL` (variable, line 9) — read by 2: `src/editor/roadTop.ts#dirtyCells`, `src/world/walkable.ts#WalkableSurface.cellOf`
- `src/world/cells.ts#cellAt` (function, line 23) — call by 4: `src/editor/roadTop.ts#allCells`, `src/editor/roadTop.ts#dirtyCells`, `src/editor/roadTop.ts#RoadTop.cellTop`, `src/world/walkable.ts#WalkableSurface.cellOf`
- `src/world/cells.ts#CellFiling` (class, line 60) — construct by 10: `src/world/network.ts#Network.clear`, `src/world/network.ts#Network.flushCells`, `src/world/network.ts#Network.junctionCells`, `src/world/network.ts#Network.segmentCells`, …
- `src/world/cells.ts#CellFiling` (class, line 60) — reference by 2: `src/world/network.ts#Network.fork`, `src/world/networkBuild.ts#NetworkBuild.forkFrom`
- `src/world/cells.ts#CellFiling` (class, line 60) — typeRef by 2: `src/world/networkBuild.ts#Network`, `src/world/cells.ts#CellFiling.takeBack`
- `src/world/cells.ts#CellFiling.byCell` (field, line 66) — read by 1: `src/world/cells.ts#CellFiling.takeBack`
- `src/world/cells.ts#CellFiling.copyFrom` (method, line 200) — call by 2: `src/world/network.ts#Network.clear`, `src/world/networkBuild.ts#NetworkBuild.forget`
- `src/world/cells.ts#CellFiling.entries` (field, line 64) — read by 1: `src/world/cells.ts#CellFiling.takeBack`
- `src/world/cells.ts#CellFiling.epoch` (field, line 69) — read by 2: `src/world/cells.ts#CellFiling.takeBack`, `src/world/walkable.ts#WalkableSurface.cellOf`
- `src/world/cells.ts#CellFiling.epoch` (field, line 69) — write by 1: `src/world/cells.ts#CellFiling.takeBack`
- `src/world/cells.ts#CellFiling.file` (method, line 76) — call by 7: `src/world/networkBuild.ts#NetworkBuild.findChanges`, `src/world/networkBuild.ts#NetworkBuild.rebuild`, `src/render/roadSurfaceInputs.ts#SurfaceInputs.fileNode`, `src/render/roadSurfaceInputs.ts#SurfaceInputs.fileSegment`, …
- `src/world/cells.ts#CellFiling.ids` (method, line 126) — call by 1: `src/render/streetFurnitureLayer.ts#PlacedProduct.allKeys`
- `src/world/cells.ts#CellFiling.idsMeeting` (method, line 131) — call by 9: `src/editor/roadTop.ts#RoadTop.cellTop`, `src/editor/snap.ts#ribbonsNear`, `src/editor/roads/streetKit.ts#nearestStreet`, `src/world/network.ts#Network.ribbonsMeeting`, …
- `src/world/cells.ts#CellFiling.idsMeetingAny` (method, line 169) — call by 7: `src/world/roads/footwayRise.ts#packFlush`, `src/world/network.ts#Network.forgetCrossings`, `src/world/networkBuild.ts#NetworkBuild.findChanges`, `src/world/elevation.ts#buildRoadElevation`, …
- `src/world/cells.ts#CellFiling.over` (method, line 212) — call by 2: `src/world/network.ts#Network.fork`, `src/world/networkBuild.ts#NetworkBuild.forkFrom`
- `src/world/cells.ts#CellFiling.rectOf` (method, line 120) — call by 4: `src/world/roads/footwayRise.ts#packFlush`, `src/world/elevation.ts#pendingOrder`, `src/render/roadSurfaceInputs.ts#SurfaceInputs.nodeBox`, `src/render/roadSurfaceInputs.ts#SurfaceInputs.segmentBox`
- `src/world/cells.ts#CellFiling.remove` (method, line 107) — call by 5: `src/world/networkBuild.ts#NetworkBuild.findChanges`, `src/world/networkBuild.ts#NetworkBuild.rebuild`, `src/render/roadSurfaceInputs.ts#SurfaceInputs.placeNode`, `src/render/roadSurfaceInputs.ts#SurfaceInputs.placeSegment`, …
- `src/world/cells.ts#CellFiling.someAt` (method, line 157) — call by 3: `src/world/roads/footwayRise.ts#footwayRiseShare`, `src/world/carriageway.ts#onCarriageway`, `src/world/carriageway.ts#onRoadGround`
- `src/world/cells.ts#CellFiling.stamp` (method, line 193) — call by 1: `src/world/walkable.ts#WalkableSurface.cellOf`
- `src/world/cells.ts#CellFiling.versions` (field, line 67) — read by 1: `src/world/cells.ts#CellFiling.takeBack`
- `src/world/cells.ts#cellRect` (function, line 17) — call by 3: `src/editor/roadTop.ts#RoadTop.cellTop`, `src/world/walkable.ts#WalkableSurface.cellOf`, `src/world/walkable.ts#WalkableSurface.forgetChanged`
- `src/world/cells.ts#grow` (function, line 33) — call by 2: `src/render/streetFurnitureLayer.ts#furnitureAbout`, `src/render/streetFurnitureLayer.ts#PlacedProduct.dirty`

### src/world/changes.ts

- `src/world/changes.ts#Change` (interface, line 35) — read by 6: `src/editor/roadTool.ts#RoadTool.mapAround`, `src/editor/roadTop.ts#dirtyCells`, `src/world/networkBuild.ts#NetworkBuild.findChanges`, `src/render/roadSurfaceInputs.ts#SurfaceInputs.refresh`, …
- `src/world/changes.ts#Change` (interface, line 35) — typeRef by 2: `src/editor/roadTop.ts#dirtyCells`, `src/render/streetFurnitureLayer.ts#PlacedProduct.dirty`
- `src/world/changes.ts#ChangeJournal` (class, line 156) — typeRef by 2: `src/editor/roadTop.ts#Source`, `src/render/streetFurnitureLayer.ts#FurnitureSource`
- `src/world/changes.ts#ChangeJournal.record` (method, line 263) — call by 2: `src/world/doc.ts#RoadDoc.removeElementItems`, `src/world/doc.ts#RoadDoc.setBalance`
- `src/world/changes.ts#ChangeJournal.serialOf` (method, line 203) — call by 1: `src/world/doc.ts#RoadDoc.trafficRevision`
- `src/world/changes.ts#ChangeJournal.since` (method, line 309) — call by 2: `src/world/networkBuild.ts#NetworkBuild.findChanges`, `src/world/economy.ts#roadValue`
- `src/world/changes.ts#ChangeJournal.version` (getter, line 198) — call by 5: `src/world/networkBuild.ts#NetworkBuild.rebuild`, `src/world/networkBuild.ts#NetworkBuild.takeBack`, `src/world/economy.ts#roadValue`, `src/world/doc.ts#RoadDoc.forkOrigin`, …
- `src/world/changes.ts#ChangeKind` (type, line 27) — typeRef by 3: `src/editor/roadTop.ts#KINDS`, `src/render/streetFurnitureLayer.ts#PlacedProduct.constructor`, `src/render/streetFurnitureLayer.ts#PlacedProduct.kinds`
- `src/world/changes.ts#ChangeRect` (type, line 4) — typeRef by 36: `src/editor/snap.ts#ribbonsNear`, `src/world/junction/levels.ts#boundsOf`, `src/world/junction/levels.ts#JunctionLevels.constructor`, `src/world/junction/levels.ts#JunctionLevelsData`, …
- `src/world/changes.ts#DOC_CHANGE_KINDS` (variable, line 24) — read by 1: `src/editor/roadTool.ts#RoadTool.mapSeen`

### src/world/cityGen/mapSize.ts

- `src/world/cityGen/mapSize.ts#MAP_HALF` (variable, line 9) — read by 2: `src/editor/roundabout.ts#roundaboutRefusal`, `src/world/cityGen/plan.ts#ROUND_REACH`

### src/world/conflictTable.ts

- `src/world/conflictTable.ts#BodyClass` (type, line 15) — typeRef by 4: `src/world/junctionBuild.ts#MadeConnector`, `src/world/lanelets.ts#Connector`, `src/world/turnPaths.ts#TurnPathResult`, `src/world/turnPaths.ts#turnFits`
- `src/world/conflictTable.ts#ChosenTurn` (interface, line 21) — read by 1: `src/world/turnPaths.ts#turnPath`
- `src/world/conflictTable.ts#ChosenTurn` (interface, line 21) — typeRef by 6: `src/world/junctionTurns.ts#JunctionTurns.entries`, `src/world/junctionTurns.ts#JunctionTurns.find`, `src/world/junctionTurns.ts#JunctionTurns.keep`, `src/world/junctionTurns.ts#JunctionTurns.made`, …
- `src/world/conflictTable.ts#HEAVY` (variable, line 50) — read by 3: `src/world/turnPaths.ts#bulbTurnPath`, `src/world/turnPaths.ts#medianUturnPath`, `src/world/turnPaths.ts#turnPath`
- `src/world/conflictTable.ts#JunctionAnswer` (interface, line 486) — read by 1: `src/world/lanelets.ts#LaneletGraph.junctionsOffPage`
- `src/world/conflictTable.ts#JunctionAnswer` (interface, line 486) — typeRef by 1: `src/world/junctionBuild.ts#buildJunctionOf`
- `src/world/conflictTable.ts#JunctionAsk` (interface, line 476) — read by 1: `src/world/junctionBuild.ts#buildJunctionOf`
- `src/world/conflictTable.ts#JunctionAsk` (interface, line 476) — typeRef by 3: `src/world/junctionBuild.ts#buildJunctionOf`, `src/world/junctionBuild.ts#junctionAsk`, `src/world/lanelets.ts#LaneletGraph.junctionsOffPage`
- `src/world/conflictTable.ts#JunctionLeg` (interface, line 453) — read by 4: `src/world/junctionBuild.ts#carriedPair`, `src/world/junctionBuild.ts#junctionAsk.leg`, `src/world/junctionBuild.ts#junctionMovements`, `src/world/junctionBuild.ts#legLine`
- `src/world/conflictTable.ts#JunctionLeg` (interface, line 453) — typeRef by 9: `src/world/junctionBuild.ts#carriedPair`, `src/world/junctionBuild.ts#junctionAsk`, `src/world/junctionBuild.ts#junctionAsk.leg`, `src/world/junctionBuild.ts#JunctionInput`, …
- `src/world/conflictTable.ts#JunctionMeasurer` (interface, line 498) — call by 1: `src/world/lanelets.ts#LaneletGraph.junctionsOffPage`
- `src/world/conflictTable.ts#JunctionMeasurer` (interface, line 498) — reference by 1: `src/world/lanelets.ts#LaneletGraph.buildJunctions`
- `src/world/conflictTable.ts#JunctionMeasurer` (interface, line 498) — typeRef by 4: `src/world/lanelets.ts#LaneletGraph.buildJunctions`, `src/world/lanelets.ts#LaneletGraph.buildSteps`, `src/world/lanelets.ts#LaneletGraph.junctionsOffPage`, `src/world/lanelets.ts#LaneletGraph.updateSteps`
- `src/world/conflictTable.ts#JunctionNode` (type, line 450) — typeRef by 2: `src/world/junctionBuild.ts#carriedPair`, `src/world/junctionBuild.ts#JunctionInput`
- `src/world/conflictTable.ts#LaneAsk` (interface, line 460) — inherit by 1: `src/world/junctionBuild.ts#JunctionLane`
- `src/world/conflictTable.ts#LaneAsk` (interface, line 460) — read by 9: `src/world/junctionBuild.ts#buildJunctionOf.lane`, `src/world/junctionBuild.ts#junctionAsk.lane`, `src/world/junctionBuild.ts#junctionKey`, `src/world/junctionBuild.ts#junctionMovements`, …
- `src/world/conflictTable.ts#LaneAsk` (interface, line 460) — typeRef by 4: `src/world/junctionBuild.ts#buildJunctionOf.lane`, `src/world/junctionBuild.ts#junctionAsk`, `src/world/junctionBuild.ts#junctionAsk.lane`, `src/world/lanelets.ts#LaneletGraph.junctionsOffPage`
- `src/world/conflictTable.ts#SurfaceRings` (interface, line 39) — typeRef by 2: `src/world/junctionTurns.ts#JunctionTurns.defer`, `src/world/junctionTurns.ts#TurnPlan.surfaces`
- `src/world/conflictTable.ts#TurnAsk` (interface, line 27) — read by 1: `src/world/junctionTurns.ts#JunctionTurns.defer`
- `src/world/conflictTable.ts#TurnAsk` (interface, line 27) — typeRef by 2: `src/world/junctionTurns.ts#JunctionTurns.defer`, `src/world/junctionTurns.ts#TurnPlan.asks`
- `src/world/conflictTable.ts#TurnChoice` (type, line 18) — read by 1: `src/world/turnPaths.ts#makeChoice`
- `src/world/conflictTable.ts#TurnChoice` (type, line 18) — typeRef by 1: `src/world/turnPaths.ts#makeChoice`
- `src/world/conflictTable.ts#TurnKind` (type, line 447) — typeRef by 9: `src/world/junctionBuild.ts#classifyTurn`, `src/world/junctionBuild.ts#fallbackTurnRank`, `src/world/junctionBuild.ts#junctionMovements`, `src/world/junctionBuild.ts#junctionMovements.addConnector`, …

### src/world/derived.ts

- `src/world/derived.ts#Derived` (class, line 71) — construct by 3: `src/editor/roadTop.ts#RoadTop.constructor`, `src/render/streetFurnitureLayer.ts#furnitureProduct`, `src/render/streetFurnitureLayer.ts#meadowProduct`
- `src/world/derived.ts#Derived` (class, line 71) — typeRef by 3: `src/editor/roadTop.ts#RoadTop.cells`, `src/render/streetFurnitureLayer.ts#furnitureProduct`, `src/render/streetFurnitureLayer.ts#meadowProduct`
- `src/world/derived.ts#Derived.get` (method, line 80) — call by 1: `src/render/streetFurnitureLayer.ts#createStreetFurniture.sync`
- `src/world/derived.ts#Derived.refresh` (method, line 93) — call by 2: `src/editor/roadTop.ts#RoadTop.height`, `src/render/streetFurnitureLayer.ts#createStreetFurniture.sync`
- `src/world/derived.ts#Derived.work` (field, line 74) — read by 2: `src/editor/roadTop.ts#RoadTop.lastComputes`, `src/render/streetFurnitureLayer.ts#createStreetFurniture.sync`
- `src/world/derived.ts#DerivedSpec` (interface, line 40) — implements by 1: `src/render/streetFurnitureLayer.ts#PlacedProduct`
- `src/world/derived.ts#DerivedWork` (interface, line 60) — read by 2: `src/editor/roadTop.ts#RoadTop.lastComputes`, `src/render/streetFurnitureLayer.ts#createStreetFurniture.sync`
- `src/world/derived.ts#JournalCursor` (class, line 6) — construct by 2: `src/editor/roadTool.ts#RoadTool.mapSeen`, `src/render/roadSurfaceInputs.ts#SurfaceInputs.buildings`
- `src/world/derived.ts#JournalCursor.read` (method, line 19) — call by 2: `src/editor/roadTool.ts#RoadTool.mapAround`, `src/render/roadSurfaceInputs.ts#SurfaceInputs.refresh`

### src/world/doc.ts

- `src/world/doc.ts#elementsBox` (function, line 3338) — call by 1: `src/world/doc.ts#RoadDoc.removeElementItems`
- `src/world/doc.ts#fitRoadCurve` (function, line 64) — call by 2: `src/editor/roadTool.ts#RoadTool.curveOf`, `src/editor/roadTool.ts#RoadTool.pieces`
- `src/world/doc.ts#ItemGrid.remove` (method, line 3496) — call by 1: `src/world/doc.ts#RoadDoc.removeElementItems`
- `src/world/doc.ts#JunctionControl` (type, line 77) — typeRef by 5: `src/ui/roads/junctionPanel.ts#CONTROLS`, `src/ui/roads/junctionPanel.ts#JunctionPanelHost`, `src/world/junctionBuild.ts#shouldSignalise`, `src/world/lanelets.ts#JunctionTopology`, …
- `src/world/doc.ts#keepWhere` (function, line 3516) — call by 1: `src/world/doc.ts#RoadDoc.removeElementItems`
- `src/world/doc.ts#NodeCrossing` (interface, line 119) — read by 4: `src/world/junctionBuild.ts#junctionKey`, `src/world/network.ts#Network.crosswalkDistanceAt`, `src/world/networkBuild.ts#nodeSourceDigest`, `src/world/lanelets.ts#LaneletGraph.buildSegmentLinks`
- `src/world/doc.ts#NodeCrossing` (interface, line 119) — typeRef by 1: `src/world/network.ts#Network.midBlockCrossing`
- `src/world/doc.ts#NodeCrossingKind` (type, line 118) — typeRef by 1: `src/world/doc.ts#RoadDoc.setNodeCrossing`
- `src/world/doc.ts#RoadDoc` (class, line 153) — typeRef by 79: `src/editor/roadTool.ts#landscapeAlong`, `src/editor/roadTool.ts#RoadToolHost`, `src/editor/roadTop.ts#allCells`, `src/editor/roadTop.ts#dirtyCells`, …
- `src/world/doc.ts#RoadDoc.addLandscape` (method, line 576) — call by 4: `src/editor/roadTool.ts#RoadTool.layPart`, `src/editor/streetscapeTool.ts#StreetscapeTool.down`, `src/editor/streetscapeTool.ts#StreetscapeTool.up`, `src/editor/roads/streetKit.ts#carryStreetKit`
- `src/world/doc.ts#RoadDoc.addNode` (method, line 711) — call by 1: `src/editor/roundabout.ts#commitRoundabout`
- `src/world/doc.ts#RoadDoc.addPoleSpan` (method, line 498) — call by 1: `src/editor/roads/powerLine.ts#layPowerLinesSteps.join`
- `src/world/doc.ts#RoadDoc.addSegment` (method, line 732) — call by 1: `src/editor/roundabout.ts#commitRoundabout`
- `src/world/doc.ts#RoadDoc.changes` (field, line 262) — read by 13: `src/editor/roadTool.ts#RoadTool.mapAround`, `src/editor/roadTop.ts#RoadTop.constructor`, `src/world/networkBuild.ts#NetworkBuild.findChanges`, `src/world/networkBuild.ts#NetworkBuild.rebuild`, …
- `src/world/doc.ts#RoadDoc.degree` (method, line 686) — call by 4: `src/world/network.ts#Network.crosswalkDistanceAt`, `src/world/lanelets.ts#LaneletGraph.buildSegmentLinks`, `src/world/surfaceParts.ts#surfaceParts.road`, `src/render/paintQuads.ts#roadStrokes.cut`
- `src/world/doc.ts#RoadDoc.economy` (field, line 220) — read by 2: `src/editor/roads/economy.ts#roadsBefore`, `src/world/doc.ts#RoadDoc.setBalance`
- `src/world/doc.ts#RoadDoc.economy` (field, line 220) — write by 1: `src/world/doc.ts#RoadDoc.setBalance`
- `src/world/doc.ts#RoadDoc.editableNode` (method, line 691) — call by 2: `src/world/doc.ts#RoadDoc.setNodeUturn`, `src/world/doc.ts#RoadDoc.setNodeCrossing`
- `src/world/doc.ts#RoadDoc.elementGrid` (field, line 245) — read by 1: `src/world/doc.ts#RoadDoc.removeElementItems`
- `src/world/doc.ts#RoadDoc.elements` (field, line 243) — read by 1: `src/world/doc.ts#RoadDoc.removeElementItems`
- `src/world/doc.ts#RoadDoc.elementsIn` (method, line 256) — call by 1: `src/world/junction/bulb.ts#clearBulbsOf`
- `src/world/doc.ts#RoadDoc.fork` (method, line 1827) — call by 4: `src/editor/roundabout.ts#commitRoundabout`, `src/editor/streetObjects.ts#commitPedestrianCrossing`, `src/editor/streetObjects.ts#commitUturn`, `src/editor/commit.ts#moveNodeChecked`
- `src/world/doc.ts#RoadDoc.forkedFrom` (field, line 264) — read by 1: `src/world/doc.ts#RoadDoc.forkOrigin`
- `src/world/doc.ts#RoadDoc.landscape` (getter, line 178) — call by 4: `src/world/streetFurniture.ts#streetFurniture`, `src/render/streetFurnitureLayer.ts#PlacedProduct.allKeys`, `src/render/streetFurnitureLayer.ts#PlacedProduct.compute`, `src/render/streetFurnitureLayer.ts#PlacedProduct.dirty`
- `src/world/doc.ts#RoadDoc.landscapeIn` (method, line 605) — call by 7: `src/editor/roadTool.ts#landscapeAlong`, `src/editor/streetscapeTool.ts#itemsAround`, `src/editor/roads/streetKit.ts#kitOf`, `src/world/roads/furnitureSets.ts#furnitureFor`, …
- `src/world/doc.ts#RoadDoc.landscapeInAny` (method, line 610) — call by 1: `src/render/streetFurnitureLayer.ts#PlacedProduct.dirty`
- `src/world/doc.ts#RoadDoc.mapEdge` (method, line 703) — call by 2: `src/world/lanelets.ts#LaneletGraph.buildSegmentLinks`, `src/world/lanelets.ts#LaneletGraph.junctionInput.lane`
- `src/world/doc.ts#RoadDoc.markingStyle` (field, line 204) — read by 4: `src/world/roads/derivedSigns.ts#signsAtNode`, `src/render/roadSurfaceInputs.ts#SurfaceInputs.placeNode`, `src/render/roadSurfaceInputs.ts#SurfaceInputs.refresh`, `src/render/paintQuads.ts#roadStrokes`
- `src/world/doc.ts#RoadDoc.markNode` (method, line 1407) — call by 2: `src/world/doc.ts#RoadDoc.setNodeUturn`, `src/world/doc.ts#RoadDoc.setNodeCrossing`
- `src/world/doc.ts#RoadDoc.mergeNodes` (method, line 820) — call by 1: `src/editor/commit.ts#reconcileMovedNode`
- `src/world/doc.ts#RoadDoc.moveNode` (method, line 862) — call by 3: `src/editor/nodeMover.ts#NodeMover.move`, `src/editor/nodeMover.ts#NodeMover.putBack`, `src/editor/commit.ts#moveNodeChecked`
- `src/world/doc.ts#RoadDoc.node` (method, line 416) — call by 75: `src/editor/roadTop.ts#RoadTop.cellTop`, `src/editor/nodeMover.ts#NodeMover.drop`, `src/editor/nodeMover.ts#NodeMover.grab`, `src/editor/snap.ts#anchorHeightOffset`, …
- `src/world/doc.ts#RoadDoc.nodes` (field, line 154) — read by 8: `src/editor/roadTop.ts#allCells`, `src/world/networkBuild.ts#NetworkBuild.findChanges`, `src/world/lanelets.ts#LaneletGraph.buildJunctions`, `src/world/lanelets.ts#LaneletGraph.junctionsOffPage`, …
- `src/world/doc.ts#RoadDoc.nodeToWrite` (method, line 1953) — call by 2: `src/world/doc.ts#RoadDoc.setNodeUturn`, `src/world/doc.ts#RoadDoc.setNodeCrossing`
- `src/world/doc.ts#RoadDoc.ownList` (method, line 1969) — call by 1: `src/world/doc.ts#RoadDoc.removeElementItems`
- `src/world/doc.ts#RoadDoc.poles` (getter, line 164) — call by 1: `src/editor/roads/powerLine.ts#layPowerLinesSteps`
- `src/world/doc.ts#RoadDoc.polesIn` (method, line 450) — call by 3: `src/editor/roads/streetKit.ts#kitOf`, `src/world/roads/furnitureSets.ts#furnitureFor`, `src/world/junction/bulb.ts#clearBulbsOf`
- `src/world/doc.ts#RoadDoc.removeLandscape` (method, line 596) — call by 4: `src/editor/streetscapeTool.ts#StreetscapeTool.down`, `src/editor/roads/streetKit.ts#carryStreetKit`, `src/editor/roads/streetKit.ts#removeWithStreetKit`, `src/world/junction/bulb.ts#clearBulbsOf`
- `src/world/doc.ts#RoadDoc.removePole` (method, line 654) — call by 3: `src/editor/roads/streetKit.ts#carryStreetKit`, `src/editor/roads/streetKit.ts#removeWithStreetKit`, `src/world/junction/bulb.ts#clearBulbsOf`
- `src/world/doc.ts#RoadDoc.requireNode` (method, line 424) — call by 4: `src/editor/roundabout.ts#commitRoundabout`, `src/editor/streetObjects.ts#commitPedestrianCrossing`, `src/editor/streetObjects.ts#commitUturn`, `src/world/lanelets.ts#LaneletGraph.junctionInput`
- `src/world/doc.ts#RoadDoc.requireSegment` (method, line 430) — call by 12: `src/editor/roadTool.ts#RoadTool.layPart`, `src/editor/streetObjects.ts#commitPedestrianCrossing`, `src/editor/streetObjects.ts#commitUturn`, `src/world/roads/furnitureSets.ts#furnitureFor`, …
- `src/world/doc.ts#RoadDoc.revision` (getter, line 371) — call by 7: `src/editor/roadTool.ts#RoadTool.layPart`, `src/editor/nodeMover.ts#NodeMover.putBack`, `src/world/network.ts#Network.fork`, `src/world/network.ts#Network.isBuilt`, …
- `src/world/doc.ts#RoadDoc.segment` (method, line 420) — call by 69: `src/editor/roadTop.ts#RoadTop.cellTop`, `src/editor/nodeMover.ts#NodeMover.grab`, `src/editor/nodeMover.ts#NodeMover.putBack`, `src/editor/roundabout.ts#roundaboutRefusal`, …
- `src/world/doc.ts#RoadDoc.segments` (field, line 155) — read by 7: `src/world/networkBuild.ts#NetworkBuild.findChanges`, `src/world/lanelets.ts#LaneletGraph.buildLinks`, `src/world/lanelets.ts#LaneletGraph.linksOnly`, `src/world/surfaceParts.ts#surfaceParts`, …
- `src/world/doc.ts#RoadDoc.setNodeApproachRules` (method, line 1027) — call by 1: `src/editor/streetscapeTool.ts#StreetscapeTool.down`
- `src/world/doc.ts#RoadDoc.setNodeControl` (method, line 1347) — call by 2: `src/editor/roundabout.ts#commitRoundabout`, `src/editor/streetscapeTool.ts#StreetscapeTool.down`
- `src/world/doc.ts#RoadDoc.setSegmentCurve` (method, line 919) — call by 1: `src/editor/nodeMover.ts#NodeMover.putBack`
- `src/world/doc.ts#RoadDoc.setSegmentCutWalls` (method, line 977) — call by 1: `src/editor/roadTool.ts#RoadTool.layPart`
- `src/world/doc.ts#RoadDoc.setSegmentDirection` (method, line 928) — call by 2: `src/editor/roadTool.ts#RoadTool.layPart`, `src/editor/roads/profile.ts#applyProfileTo`
- `src/world/doc.ts#RoadDoc.setSegmentLanes` (method, line 939) — call by 2: `src/editor/roadTool.ts#RoadTool.layPart`, `src/editor/roads/profile.ts#applyProfileTo`
- `src/world/doc.ts#RoadDoc.setSegmentParking` (method, line 964) — call by 2: `src/editor/roadTool.ts#RoadTool.layPart`, `src/editor/roads/profile.ts#applyProfileTo`
- `src/world/doc.ts#RoadDoc.setSegmentSection` (method, line 951) — call by 2: `src/editor/roadTool.ts#RoadTool.layPart`, `src/editor/roads/profile.ts#applyProfileTo`
- `src/world/doc.ts#RoadDoc.setSegmentType` (method, line 909) — call by 2: `src/editor/roadTool.ts#RoadTool.layPart`, `src/editor/roads/profile.ts#applyProfileTo`
- `src/world/doc.ts#RoadDoc.streetNameCount` (field, line 182) — read by 1: `src/world/doc.ts#RoadDoc.streetNamesInUse`
- `src/world/doc.ts#RoadDoc.takeBack` (method, line 1847) — call by 4: `src/editor/roundabout.ts#commitRoundabout`, `src/editor/streetObjects.ts#commitPedestrianCrossing`, `src/editor/streetObjects.ts#commitUturn`, `src/editor/commit.ts#moveNodeChecked`
- `src/world/doc.ts#RoadDoc.terrainRelief` (field, line 202) — read by 1: `src/render/roadSurfacePass.ts#followsGround`
- `src/world/doc.ts#RoadDoc.terrainStamps` (field, line 200) — read by 1: `src/render/roadSurfacePass.ts#followsGround`
- `src/world/doc.ts#RoadNode` (interface, line 89) — read by 89: `src/editor/roadTop.ts#allCells`, `src/editor/roadTop.ts#RoadTop.cellTop`, `src/editor/nodeMover.ts#NodeMover.drop`, `src/editor/nodeMover.ts#NodeMover.grab`, …
- `src/world/doc.ts#RoadNode` (interface, line 89) — typeRef by 12: `src/world/roads/rules.ts#approachSign`, `src/world/roads/rules.ts#legRule`, `src/world/junctionBuild.ts#isJunction`, `src/world/networkBuild.ts#nodeBakeKey`, …
- `src/world/doc.ts#RoadNode` (interface, line 89) — write by 2: `src/world/doc.ts#RoadDoc.setNodeUturn`, `src/world/doc.ts#RoadDoc.setNodeCrossing`
- `src/world/doc.ts#RoadSegment` (interface, line 124) — read by 83: `src/editor/roadTool.ts#RoadTool.layPart`, `src/editor/roadTop.ts#RoadTop.cellTop`, `src/editor/nodeMover.ts#NodeMover.grab`, `src/editor/nodeMover.ts#NodeMover.putBack`, …
- `src/world/doc.ts#RoadSegment` (interface, line 124) — typeRef by 13: `src/world/junction/build.ts#outlinesMeet`, `src/world/junction/build.ts#outlinesMeet.minus`, `src/world/junction/build.ts#outlinesMeet.plus`, `src/ui/roads/profilePanel.ts#mountProfilePanel`, …

### src/world/economy.ts

- `src/world/economy.ts#BUILD_KINDS` (variable, line 12) — read by 1: `src/world/economy.ts#segmentCost`
- `src/world/economy.ts#BuildKind` (type, line 11) — typeRef by 1: `src/world/economy.ts#buildLengths`
- `src/world/economy.ts#E` (variable, line 28) — read by 3: `src/world/economy.ts#chargeFor`, `src/world/economy.ts#segmentCost`, `src/world/economy.ts#buildLengths`
- `src/world/economy.ts#Economy` (interface, line 14) — read by 2: `src/editor/roads/economy.ts#roadsBefore`, `src/world/doc.ts#RoadDoc.setBalance`
- `src/world/economy.ts#Ledger` (interface, line 73) — read by 2: `src/world/economy.ts#roadValue`, `src/world/economy.ts#priceOf`
- `src/world/economy.ts#Ledger` (interface, line 73) — typeRef by 2: `src/world/economy.ts#priceAll`, `src/world/economy.ts#priceOf`
- `src/world/economy.ts#Ledger` (interface, line 73) — write by 1: `src/world/economy.ts#roadValue`
- `src/world/economy.ts#ledgers` (variable, line 84) — read by 1: `src/world/economy.ts#roadValue`
- `src/world/economy.ts#ledgers` (variable, line 84) — write by 1: `src/world/economy.ts#roadValue`
- `src/world/economy.ts#PER_SQUARE_UNIT` (variable, line 30) — read by 1: `src/world/economy.ts#pricePerUnit`
- `src/world/economy.ts#RoadCharge` (interface, line 150) — read by 3: `src/editor/roadTool.ts#RoadTool.layPart`, `src/editor/roads/economy.ts#settleRoadEdit`, `src/editor/commit.ts#moveNodeChecked`
- `src/world/economy.ts#RoadCharge` (interface, line 150) — typeRef by 2: `src/editor/roads/economy.ts#settleRoadEdit`, `src/world/economy.ts#chargeFor`

### src/world/elements.ts

- `src/world/elements.ts#ElementItem` (interface, line 12) — read by 1: `src/world/junction/bulb.ts#clearBulbsOf`
- `src/world/elements.ts#ElementItem` (interface, line 12) — typeRef by 2: `src/world/junction/bulb.ts#clearBulbsOf`, `src/world/doc.ts#RoadDoc.removeElementItems`

### src/world/geometry.ts

- `src/world/geometry.ts#orientedPolyline` (function, line 33) — call by 6: `src/editor/snap.ts#snapHeading`, `src/world/roads/rules.ts#mainRoadLegs`, `src/world/roads/streetChain.ts#streetChain.leaving`, `src/world/network.ts#Network.crossingLandingAt`, …
- `src/world/geometry.ts#PolylineCache.get` (method, line 70) — call by 21: `src/editor/roadTool.ts#RoadTool.down`, `src/editor/roundabout.ts#roundaboutRefusal`, `src/editor/snap.ts#anchorHeightOffset`, `src/editor/snap.ts#findAnchor`, …
- `src/world/geometry.ts#PolylineCache.map` (field, line 67) — read by 4: `src/world/geometry.ts#PolylineCache.invalidate`, `src/world/geometry.ts#PolylineCache.takeBack`, `src/world/geometry.ts#PolylineCache.forkFrom`, `src/world/geometry.ts#PolylineCache.clear`
- `src/world/geometry.ts#PolylineCache.map` (field, line 67) — write by 3: `src/world/geometry.ts#PolylineCache.invalidate`, `src/world/geometry.ts#PolylineCache.forkFrom`, `src/world/geometry.ts#PolylineCache.clear`
- `src/world/geometry.ts#segmentLine` (function, line 28) — call by 1: `src/world/junctionBuild.ts#legLine`
- `src/world/geometry.ts#segmentPolyline` (function, line 21) — call by 1: `src/world/economy.ts#segmentPrice`

### src/world/grid.ts

- `src/world/grid.ts#GRID_CELL` (variable, line 6) — read by 1: `src/editor/roadTool.ts#RoadTool.down`
- `src/world/grid.ts#GRID_STEP` (variable, line 8) — read by 6: `src/editor/roadTool.ts#RoadTool.down`, `src/editor/snap.ts#gridSnapStep`, `src/editor/snap.ts#snapHeading`, `src/world/grid.ts#onGridLength`, …

### src/world/ids.ts

- `src/world/ids.ts#NodeId` (type, line 7) — typeRef by 177: `src/editor/nodeMover.ts#NodeMover.drag`, `src/editor/nodeMover.ts#NodeMover.grab`, `src/editor/roundabout.ts#commitRoundabout`, `src/editor/snap.ts#Anchor`, …
- `src/world/ids.ts#PoleId` (type, line 10) — typeRef by 4: `src/editor/roads/powerLine.ts#layPowerLinesSteps`, `src/editor/roads/powerLine.ts#layPowerLinesSteps.join`, `src/editor/roads/streetKit.ts#Kit`, `src/editor/roads/streetKit.ts#kitOf`
- `src/world/ids.ts#SegmentId` (type, line 8) — typeRef by 210: `src/editor/roadTool.ts#landscapeAlong`, `src/editor/roadTool.ts#RoadToolHost`, `src/editor/nodeMover.ts#NodeMover.drag`, `src/editor/nodeMover.ts#NodeMover.grab`, …

### src/world/landscape.ts

- `src/world/landscape.ts#ACCESS_CELL` (variable, line 293) — read by 1: `src/world/landscape.ts#crossingAccessTester`
- `src/world/landscape.ts#ACCESS_REACH` (variable, line 278) — read by 3: `src/world/roads/furnitureSets.ts#furnitureFor`, `src/render/streetFurnitureLayer.ts#ROAD_REACH`, `src/world/landscape.ts#nearCrossingAccess`
- `src/world/landscape.ts#accessMemo` (variable, line 237) — read by 1: `src/world/landscape.ts#crossingAccesses`
- `src/world/landscape.ts#accessMemo` (variable, line 237) — write by 1: `src/world/landscape.ts#crossingAccesses`
- `src/world/landscape.ts#apart` (function, line 412) — call by 1: `src/world/landscape.ts#snapLandscape`
- `src/world/landscape.ts#CrossingAccess` (interface, line 227) — read by 2: `src/world/landscape.ts#crossingAccessTester`, `src/world/landscape.ts#onCrossingAccess`
- `src/world/landscape.ts#CrossingAccess` (interface, line 227) — typeRef by 4: `src/world/landscape.ts#crossingAccesses`, `src/world/landscape.ts#crossingAccessTester`, `src/world/landscape.ts#onCrossingAccess`, `src/world/landscape.ts#crossingAccessesNow`
- `src/world/landscape.ts#crossingAccessesAt` (function, line 251) — call by 3: `src/world/roads/furnitureSets.ts#furnitureFor`, `src/world/landscape.ts#crossingAccessesNow`, `src/world/landscape.ts#nearCrossingAccess`
- `src/world/landscape.ts#footwayAt` (function, line 94) — call by 3: `src/editor/streetscapeTool.ts#rowPieces`, `src/world/streetFurniture.ts#standing`, `src/world/landscape.ts#snapLandscape`
- `src/world/landscape.ts#FootwayHit` (interface, line 76) — read by 5: `src/editor/streetscapeTool.ts#rowPieces`, `src/editor/streetscapeTool.ts#StreetscapeTool.down`, `src/world/roads/furnitureSets.ts#furnitureFor`, `src/world/streetFurniture.ts#standing`, …
- `src/world/landscape.ts#GROW_MINUTES` (variable, line 30) — read by 2: `src/render/streetFurnitureLayer.ts#stillGrowing`, `src/world/landscape.ts#plantGrowth`
- `src/world/landscape.ts#ITEM_GAP` (variable, line 73) — read by 2: `src/world/landscape.ts#snapLandscape`, `src/world/landscape.ts#snapLandscape.others`
- `src/world/landscape.ts#LANDSCAPE_RADIUS` (variable, line 57) — read by 3: `src/world/streetFurniture.ts#standing`, `src/world/landscape.ts#snapLandscape`, `src/world/landscape.ts#depthFor`
- `src/world/landscape.ts#LandscapeItem` (interface, line 16) — read by 17: `src/editor/roadTool.ts#landscapeAlong`, `src/editor/streetscapeTool.ts#StreetscapeTool.down`, `src/editor/roads/streetKit.ts#kitOf`, `src/world/roads/furnitureSets.ts#furnitureFor.file`, …
- `src/world/landscape.ts#LandscapeItem` (interface, line 16) — typeRef by 17: `src/editor/roadTool.ts#landscapeAlong`, `src/editor/streetscapeTool.ts#itemsAround`, `src/editor/streetscapeTool.ts#rowPieces`, `src/world/roads/furnitureSets.ts#furnitureFor`, …
- `src/world/landscape.ts#LandscapeKind` (type, line 14) — typeRef by 12: `src/editor/streetscapeTool.ts#ROW_SPACING`, `src/editor/streetscapeTool.ts#rowPieces`, `src/editor/streetscapeTool.ts#StreetscapeTool.row`, `src/editor/streetscapeTool.ts#StreetscapeToolHost`, …
- `src/world/landscape.ts#landscapeNearIn` (function, line 398) — call by 1: `src/editor/streetscapeTool.ts#StreetscapeTool.down`
- `src/world/landscape.ts#LandscapeSnap` (type, line 315) — read by 4: `src/editor/streetscapeTool.ts#rowPieces`, `src/editor/streetscapeTool.ts#StreetscapeTool.down`, `src/editor/streetscapeTool.ts#StreetscapeTool.up`, `src/world/roads/furnitureSets.ts#furnitureFor`
- `src/world/landscape.ts#LandscapeSnap` (type, line 315) — typeRef by 4: `src/editor/streetscapeTool.ts#rowPieces`, `src/editor/streetscapeTool.ts#StreetscapeTool.hover`, `src/editor/streetscapeTool.ts#StreetscapeTool.row`, `src/world/landscape.ts#snapLandscape`
- `src/world/landscape.ts#MAX_ITEM_RADIUS` (variable, line 395) — read by 4: `src/editor/streetscapeTool.ts#itemsAround`, `src/world/roads/furnitureSets.ts#furnitureFor`, `src/render/streetFurnitureLayer.ts#ROAD_REACH`, `src/world/landscape.ts#snapLandscape.others`
- `src/world/landscape.ts#MEDIAN_NOSE_GAP` (variable, line 145) — read by 1: `src/world/landscape.ts#medianNose`
- `src/world/landscape.ts#MEDIAN_TREE_MIN` (variable, line 136) — read by 1: `src/world/landscape.ts#plantedMedian`
- `src/world/landscape.ts#MEDIAN_UTURN_OPENING` (variable, line 158) — read by 2: `src/editor/streetObjects.ts#commitUturn`, `src/world/landscape.ts#medianNose`
- `src/world/landscape.ts#MedianHit` (interface, line 127) — read by 3: `src/world/roads/furnitureSets.ts#furnitureFor`, `src/world/streetFurniture.ts#standing`, `src/world/landscape.ts#snapLandscape`
- `src/world/landscape.ts#MedianHit` (interface, line 127) — typeRef by 1: `src/world/landscape.ts#medianAt`
- `src/world/landscape.ts#PLANTED_SIZE` (variable, line 32) — read by 1: `src/world/landscape.ts#plantGrowth`
- `src/world/landscape.ts#ribbonsNear` (function, line 89) — call by 2: `src/world/landscape.ts#medianAt`, `src/world/landscape.ts#onAnyRoad`
- `src/world/landscape.ts#SignType` (type, line 43) — typeRef by 1: `src/editor/streetscapeTool.ts#StreetscapeToolHost`

### src/world/parking.ts

- `src/world/parking.ts#PARKING_DEPTH` (variable, line 16) — read by 5: `src/world/roads/catalog.ts#shapeProfile.kerb`, `src/world/roads/profile.ts#profileOf.band`, `src/ui/roads/profileEditor.ts#newElement`, `src/world/roadTypes.ts#resolveProfile`, …
- `src/world/parking.ts#parkingAllowed` (function, line 66) — call by 1: `src/world/roadTypes.ts#resolveProfile.fit`
- `src/world/parking.ts#ParkingKind` (type, line 7) — typeRef by 4: `src/world/roads/profile.ts#authoredFields.kindAt`, `src/world/roads/profile.ts#profileOf.band`, `src/world/roadTypes.ts#resolveProfile.fit`, `src/world/roadTypes.ts#RoadType`
- `src/world/parking.ts#sameParking` (function, line 56) — call by 1: `src/editor/roads/profile.ts#applyProfileTo`
- `src/world/parking.ts#SegmentParking` (interface, line 10) — read by 1: `src/world/roadTypes.ts#resolveProfile`
- `src/world/parking.ts#SegmentParking` (interface, line 10) — typeRef by 6: `src/editor/roadTool.ts#RoadToolHost`, `src/editor/roadParking.ts#roadParking`, `src/world/roads/profile.ts#AppliedProfile`, `src/world/roads/profile.ts#ProfileSource`, …

### src/world/parkingLayout.ts

- `src/world/parkingLayout.ts#PARKING_PAINT` (variable, line 53) — read by 1: `src/render/paintQuads.ts#PAINT_COLOURS`
- `src/world/parkingLayout.ts#ParkingLine` (interface, line 30) — read by 1: `src/render/roadSurfaceInputs.ts#SurfaceInputs.placeSegment`
- `src/world/parkingLayout.ts#ParkingLine` (interface, line 30) — typeRef by 1: `src/world/parkingLayout.ts#parkingLinesOf`
- `src/world/parkingLayout.ts#parkingOf` (function, line 107) — call by 1: `src/world/parkingLayout.ts#parkingLinesOf`
- `src/world/parkingLayout.ts#parksAlong` (function, line 90) — call by 2: `src/render/roadSurfaceInputs.ts#SurfaceInputs.placeSegment`, `src/render/roadSurfaceInputs.ts#SurfaceInputs.refresh`

### src/world/pedestrianAccess.ts

- `src/world/pedestrianAccess.ts#carriesPedestrians` (function, line 4) — call by 3: `src/editor/roads/powerLine.ts#layPowerLinesSteps`, `src/world/roads/furnitureSets.ts#furnitureFor`, `src/world/curbRamps.ts#rampsAt.pair`

### src/world/poleLines.ts

- `src/world/poleLines.ts#junctionRings` (function, line 355) — indirectCall by 1: `src/world/surfaces.ts#levelRingsOf`
- `src/world/poleLines.ts#poleLinesOf` (function, line 230) — indirectCall by 1: `src/world/surfaces.ts#levelRingsOf`
- `src/world/poleLines.ts#PoleLineState` (class, line 193) — typeRef by 1: `src/world/network.ts#Network.poleLines`
- `src/world/poleLines.ts#ribbonRings` (function, line 341) — indirectCall by 1: `src/world/surfaces.ts#levelRingsOf`

### src/world/rebuildWork.ts

- `src/world/rebuildWork.ts#networkWork` (variable, line 3) — read by 4: `src/world/networkBuild.ts#NetworkBuild.findChanges`, `src/world/networkBuild.ts#NetworkBuild.rebuild`, `src/world/networkBuild.ts#NetworkBuild.solveNode`, `src/world/networkBuild.ts#NetworkBuild.takeBakes`
- `src/world/rebuildWork.ts#networkWork` (variable, line 3) — write by 4: `src/world/networkBuild.ts#NetworkBuild.findChanges`, `src/world/networkBuild.ts#NetworkBuild.rebuild`, `src/world/networkBuild.ts#NetworkBuild.solveNode`, `src/world/networkBuild.ts#NetworkBuild.takeBakes`

### src/world/rescale.ts

- `src/world/rescale.ts#unitFactor` (function, line 10) — call by 1: `src/ui/roads/templates.ts#inThisUnit`

### src/world/section.ts

- `src/world/section.ts#Band` (interface, line 8) — read by 2: `src/world/roads/furnitureSets.ts#furnitureFor`, `src/world/landscape.ts#depthFor`
- `src/world/section.ts#BENCH_ZONE` (variable, line 56) — read by 3: `src/world/roads/furnitureCatalog.ts#FURNITURE_CATALOG`, `src/world/roads/furnitureSets.ts#furnitureFor`, `src/world/landscape.ts#depthFor`
- `src/world/section.ts#CrossSection` (interface, line 33) — read by 1: `src/editor/streetObjects.ts#commitPedestrianCrossing`
- `src/world/section.ts#LAMP_ZONE` (variable, line 54) — read by 2: `src/world/roads/furnitureCatalog.ts#FURNITURE_CATALOG`, `src/world/landscape.ts#depthFor`
- `src/world/section.ts#MIN_THROUGH` (variable, line 60) — read by 1: `src/world/landscape.ts#depthFor`
- `src/world/section.ts#sectionOf` (function, line 67) — call by 3: `src/editor/streetObjects.ts#commitPedestrianCrossing`, `src/world/roads/furnitureSets.ts#furnitureFor`, `src/world/landscape.ts#depthFor`
- `src/world/section.ts#SideZones` (interface, line 14) — read by 2: `src/world/roads/furnitureSets.ts#furnitureFor`, `src/world/landscape.ts#depthFor`
- `src/world/section.ts#TREE_KERB_SETBACK` (variable, line 50) — read by 2: `src/world/roads/furnitureCatalog.ts#FURNITURE_CATALOG`, `src/world/landscape.ts#depthFor`
- `src/world/section.ts#TREE_PIT` (variable, line 49) — read by 2: `src/world/roads/furnitureCatalog.ts#FURNITURE_CATALOG`, `src/world/landscape.ts#depthFor`
- `src/world/section.ts#zonesOn` (function, line 80) — call by 2: `src/world/roads/furnitureSets.ts#furnitureFor`, `src/world/landscape.ts#depthFor`

### src/world/signalPosts.ts

- `src/world/signalPosts.ts#SIGNAL_POST_KERB` (variable, line 11) — read by 1: `src/world/roads/derivedSigns.ts#signsAtNode`

### src/world/structures.ts

- `src/world/structures.ts#isRaised` (function, line 68) — call by 5: `src/world/roads/buildMode.ts#buildModeAt`, `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#buildRoadElevation.solve`, `src/world/elevation.ts#roadShapeAt`, …
- `src/world/structures.ts#RAISED_LIFT` (variable, line 65) — read by 2: `src/world/roads/buildMode.ts#buildModeAt`, `src/render/roadSurfacePass.ts#passRaised`
- `src/world/structures.ts#ROAD_GROUND_CLEARANCE` (variable, line 53) — read by 5: `src/world/roadField.ts#roadField.blend`, `src/world/roadField.ts#roadField.onSegment`, `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#readStations`, …
- `src/world/structures.ts#ROAD_STRUCTURES` (variable, line 44) — read by 1: `src/render/roadSurfaceInputs.ts#SurfaceInputs.passList`
- `src/world/structures.ts#roadStructure` (function, line 55) — call by 11: `src/editor/snap.ts#anchorHeightOffset`, `src/editor/snap.ts#findAnchor`, `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#buildRoadElevation.wantedOf`, …
- `src/world/structures.ts#RoadStructure` (type, line 5) — typeRef by 28: `src/world/roads/buildMode.ts#buildModeAt`, `src/world/roadField.ts#FieldData`, `src/world/roadField.ts#FieldProfile`, `src/world/roadField.ts#packField`, …
- `src/world/structures.ts#RoadStructureSpec` (interface, line 17) — read by 12: `src/editor/snap.ts#anchorHeightOffset`, `src/editor/snap.ts#findAnchor`, `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#buildRoadElevation.wantedOf`, …
- `src/world/structures.ts#TUNNEL_BORE` (variable, line 38) — read by 2: `src/world/roads/buildMode.ts#buildModeAt`, `src/world/elevation.ts#roadShapeAt`
- `src/world/structures.ts#TUNNEL_GRADE` (variable, line 42) — read by 1: `src/world/elevation.ts#solveSunken`
- `src/world/structures.ts#TUNNEL_HEADROOM` (variable, line 30) — read by 1: `src/world/junction/build.ts#surfaceMode`
- `src/world/structures.ts#TUNNEL_ROOF` (variable, line 37) — read by 1: `src/world/elevation.ts#roadShapeAt`
- `src/world/structures.ts#TUNNELS_DRAWN` (variable, line 75) — read by 1: `src/world/lanelets.ts#LaneletGraph.buildSegmentLinks`

### src/world/terrain.ts

- `src/world/terrain.ts#RELIEF_FLAT` (variable, line 90) — read by 1: `src/render/roadSurfacePass.ts#followsGround`
- `src/world/terrain.ts#valueNoise` (function, line 72) — call by 3: `src/world/cityGen/plan.ts#planCity.inside`, `src/world/cityGen/plan.ts#planCity.zoneAt`, `src/world/cityGen/field.ts#TensorField.major`

### src/world/terrainPaint.ts

- `src/world/terrainPaint.ts#PaintDab` (interface, line 23) — read by 2: `src/render/platePaint.ts#dabInRects`, `src/render/platePaint.ts#dabKey`
- `src/world/terrainPaint.ts#PaintDab` (interface, line 23) — typeRef by 3: `src/render/platePaint.ts#dabInRects`, `src/render/platePaint.ts#dabKey`, `src/render/platePaint.ts#plateDabs`

### src/world/turnPaths.ts

- `src/world/turnPaths.ts#Area` (interface, line 32) — read by 1: `src/world/turnPaths.ts#JunctionSurface.flatRings`
- `src/world/turnPaths.ts#Area` (interface, line 32) — write by 1: `src/world/turnPaths.ts#JunctionSurface.flatRings`
- `src/world/turnPaths.ts#bezierTurn` (function, line 144) — call by 2: `src/world/turnPaths.ts#turnPath`, `src/world/turnPaths.ts#makeChoice`
- `src/world/turnPaths.ts#CENTRE_TURN_RADIUS` (variable, line 445) — read by 1: `src/world/turnPaths.ts#medianUturnPath`
- `src/world/turnPaths.ts#chooseTurn` (function, line 539) — call by 1: `src/world/turnPaths.ts#turnPath`
- `src/world/turnPaths.ts#chooseTurn` (function, line 539) — read by 1: `src/world/turnPaths.ts#turnPath`
- `src/world/turnPaths.ts#FALLBACK_HANDLE` (variable, line 20) — read by 1: `src/world/turnPaths.ts#turnPath`
- `src/world/turnPaths.ts#JunctionSurface` (class, line 76) — construct by 2: `src/world/junctionBuild.ts#buildJunctionOf.surfaceOf`, `src/world/lanelets.ts#LaneletGraph.buildJunction.surfaceOf`
- `src/world/turnPaths.ts#JunctionSurface` (class, line 76) — typeRef by 9: `src/world/junctionBuild.ts#buildJunctionOf`, `src/world/junctionBuild.ts#buildJunctionOf.surfaceOf`, `src/world/junctionBuild.ts#junctionMovements`, `src/world/lanelets.ts#LaneletGraph.buildJunction`, …
- `src/world/turnPaths.ts#JunctionSurface.areas` (field, line 77) — read by 2: `src/world/turnPaths.ts#JunctionSurface.empty`, `src/world/turnPaths.ts#JunctionSurface.flatRings`
- `src/world/turnPaths.ts#JunctionSurface.centre` (field, line 83) — read by 1: `src/world/turnPaths.ts#turnPath`
- `src/world/turnPaths.ts#KEY_STEP` (variable, line 514) — read by 1: `src/world/turnPaths.ts#laneEndKey`
- `src/world/turnPaths.ts#LANE_REACH` (variable, line 512) — read by 1: `src/world/turnPaths.ts#laneEndKey`
- `src/world/turnPaths.ts#laneKeys` (variable, line 515) — read by 1: `src/world/turnPaths.ts#laneEndKey`
- `src/world/turnPaths.ts#laneKeys` (variable, line 515) — write by 1: `src/world/turnPaths.ts#laneEndKey`
- `src/world/turnPaths.ts#NO_ROAD_ENDS` (variable, line 378) — read by 2: `src/world/turnPaths.ts#turnPath`, `src/world/turnPaths.ts#turnFits`
- `src/world/turnPaths.ts#QUEUE_REACH` (variable, line 179) — read by 1: `src/world/turnPaths.ts#turnPath`
- `src/world/turnPaths.ts#RoadEnds` (interface, line 374) — read by 1: `src/world/turnPaths.ts#turnPath`
- `src/world/turnPaths.ts#RoadEnds` (interface, line 374) — typeRef by 2: `src/world/turnPaths.ts#turnPath`, `src/world/turnPaths.ts#turnFits`
- `src/world/turnPaths.ts#STEPS` (variable, line 24) — read by 1: `src/world/turnPaths.ts#bulbTurnPath.cubic`
- `src/world/turnPaths.ts#sweep` (function, line 284) — call by 1: `src/world/turnPaths.ts#turnFits`
- `src/world/turnPaths.ts#twoPieceTurn` (function, line 489) — call by 1: `src/world/turnPaths.ts#makeChoice`
- `src/world/turnPaths.ts#WaitingLane` (interface, line 174) — read by 1: `src/world/turnPaths.ts#turnPath`
- `src/world/turnPaths.ts#WaitingLane` (interface, line 174) — typeRef by 1: `src/world/turnPaths.ts#turnPath`

### src/world/utilities.ts

- `src/world/utilities.ts#UtilityPole` (interface, line 43) — read by 3: `src/editor/roads/streetKit.ts#kitOf`, `src/world/roads/furnitureSets.ts#furnitureFor`, `src/world/junction/bulb.ts#clearBulbsOf`

### src/world/viewCells.ts

- `src/world/viewCells.ts#CellKey` (type, line 23) — typeRef by 2: `src/world/viewCells.ts#ViewCells.forget`, `src/world/viewCells.ts#ViewCells.ensureMany`
- `src/world/viewCells.ts#Held` (interface, line 68) — read by 1: `src/world/viewCells.ts#ViewCells.ensureMany`
- `src/world/viewCells.ts#ViewCells` (class, line 95) — construct by 1: `src/render/roadSurfaceInputs.ts#SurfaceInputs.constructor`
- `src/world/viewCells.ts#ViewCells` (class, line 95) — typeRef by 1: `src/render/roadSurfaceInputs.ts#SurfaceInputs.cells`
- `src/world/viewCells.ts#ViewCells.clear` (method, line 271) — call by 2: `src/render/roadSurfaceInputs.ts#SurfaceInputs.rebuildAll`, `src/render/streetFurnitureLayer.ts#createStreetFurniture.sync`
- `src/world/viewCells.ts#ViewCells.constructor` (constructor, line 111) — read by 2: `src/render/roadSurfaceInputs.ts#SurfaceInputs.keepNear`, `src/world/viewCells.ts#ViewCells.ensureMany`
- `src/world/viewCells.ts#ViewCells.drop` (method, line 328) — call by 1: `src/world/viewCells.ts#ViewCells.forget`
- `src/world/viewCells.ts#ViewCells.forCellsOver` (method, line 128) — call by 2: `src/render/roadSurfaceInputs.ts#SurfaceInputs.held`, `src/render/roadSurfaceInputs.ts#SurfaceInputs.wakeRect`
- `src/world/viewCells.ts#ViewCells.has` (method, line 148) — call by 1: `src/render/roadSurfaceInputs.ts#SurfaceInputs.held`
- `src/world/viewCells.ts#ViewCells.held` (field, line 98) — read by 2: `src/world/viewCells.ts#ViewCells.forget`, `src/world/viewCells.ts#ViewCells.ensureMany`
- `src/world/viewCells.ts#ViewCells.look` (method, line 186) — call by 1: `src/render/roadSurfaceInputs.ts#SurfaceInputs.keepNear`
- `src/world/viewCells.ts#ViewCells.make` (method, line 316) — call by 1: `src/world/viewCells.ts#ViewCells.ensureMany`
- `src/world/viewCells.ts#ViewCells.pump` (method, line 247) — call by 1: `src/render/roadSurfaceInputs.ts#SurfaceInputs.keepNear`
- `src/world/viewCells.ts#ViewCells.touch` (method, line 338) — call by 1: `src/world/viewCells.ts#ViewCells.ensureMany`
- `src/world/viewCells.ts#ViewCells.trim` (method, line 292) — call by 2: `src/render/roadSurfaceInputs.ts#SurfaceInputs.keepNear`, `src/world/viewCells.ts#ViewCells.ensureMany`
- `src/world/viewCells.ts#ViewCells.version` (field, line 109) — read by 1: `src/world/viewCells.ts#ViewCells.forget`
- `src/world/viewCells.ts#ViewCells.version` (field, line 109) — write by 1: `src/world/viewCells.ts#ViewCells.forget`
- `src/world/viewCells.ts#ViewCells.work` (field, line 107) — read by 2: `src/render/streetFurnitureLayer.ts#createStreetFurniture.sync`, `src/world/viewCells.ts#ViewCells.forget`
- `src/world/viewCells.ts#ViewCellsSpec` (interface, line 37) — read by 2: `src/render/roadSurfaceInputs.ts#SurfaceInputs.keepNear`, `src/world/viewCells.ts#ViewCells.ensureMany`
- `src/world/viewCells.ts#ViewCellsWork` (interface, line 62) — read by 2: `src/render/streetFurnitureLayer.ts#createStreetFurniture.sync`, `src/world/viewCells.ts#ViewCells.forget`
- `src/world/viewCells.ts#ViewCellsWork` (interface, line 62) — write by 1: `src/world/viewCells.ts#ViewCells.forget`

### src/world/waiting.ts

- `src/world/waiting.ts#answered` (function, line 5) — call by 1: `src/world/lanelets.ts#LaneletGraph.junctionsOffPage`
- `src/world/waiting.ts#WAITING` (variable, line 2) — typeRef by 4: `src/world/lanelets.ts#LaneletGraph.buildJunctions`, `src/world/lanelets.ts#LaneletGraph.buildSteps`, `src/world/lanelets.ts#LaneletGraph.junctionsOffPage`, `src/world/lanelets.ts#LaneletGraph.updateSteps`

### src/world/zones.ts

- `src/world/zones.ts#ZoneDensity` (type, line 5) — typeRef by 2: `src/world/cityGen/plan.ts#CityPlan`, `src/world/cityGen/plan.ts#planCity.zoneAt`
- `src/world/zones.ts#ZoneUse` (type, line 4) — typeRef by 2: `src/world/cityGen/plan.ts#CityPlan`, `src/world/cityGen/plan.ts#planCity.zoneAt`

## External APIs

- `es:Math` × 1039
- `es:Map.get` × 387
- `es:Array.length` × 355
- `es:Array.push` × 347
- `es:Map` × 287
- `es:Math.max` × 270
- `es:Map.set` × 229
- `es:Math.min` × 188
- `es:Set.add` × 185
- `es:Set` × 183
- `es:Math.floor` × 174
- `es:ReadonlyArray.length` × 174
- `es:Array.map` × 134
- `dom:ParentNode.append` × 113
- `es:Float64Array` × 110
- `es:Map.delete` × 97
- `es:Math.PI` × 89
- `es:Infinity` × 86
- `es:Set.has` × 86
- `dom:document` × 85
- `es:Math.abs` × 81
- `dom:Document.createElement` × 80
- `es:ReadonlyArray.map` × 79
- `es:Array.sort` × 72
- `es:Record` × 71
- `dom:Element.textContent` × 69
- `dom:HTMLOrSVGElement.dataset` × 60
- `es:Math.round` × 60
- `es:Map.clear` × 58
- `es:Float32Array` × 56
- `dom:Element.className` × 52
- `es:Math.hypot` × 51
- `es:Number` × 49
- `es:ReadonlyMap` × 49
- `es:Uint32Array` × 42
- `es:ReadonlySet` × 41
- `es:Array.filter` × 40
- `three:Mesh` × 40
- `dom:Element.setAttribute` × 37
- `es:Float64Array.length` × 35
- `es:Array.slice` × 34
- `es:NaN` × 34
- `es:Promise` × 34
- `es:Map.keys` × 32
- `es:Array` × 27
- `es:Iterable` × 27
- `es:Object` × 27
- `dom:GlobalEventHandlers.onclick` × 26
- `dom:HTMLButtonElement.type` × 26
- `es:Array.some` × 26
- `es:ArrayBuffer` × 26
- `es:Math.cos` × 26
- `es:ReadonlyArray.filter` × 26
- `es:ReadonlyMap.get` × 26
- `es:Set.delete` × 26
- `es:Map.has` × 24
- `es:Math.imul` × 24
- `es:ReadonlyArray.includes` × 24
- `es:Error` × 23
- `three:Material` × 23
- `dom:CanvasFillStrokeStyles.fillStyle` × 22
- `dom:CanvasRect.fillRect` × 22
- `es:String` × 22
- `three:BufferAttribute` × 21
- `dom:ParentNode.querySelector` × 20
- `es:Float32Array.length` × 20
- `es:NumberConstructor.isFinite` × 20
- `es:Uint32Array.length` × 20
- `three:Color` × 20
- `dom:HTMLElement` × 19
- `es:Int32Array` × 19
- `es:Map.values` × 19
- `es:Math.sin` × 19
- `es:ReadonlyArray.some` × 19
- `dom:performance` × 18
- `dom:Element.innerHTML` × 17
- `dom:Performance.now` × 17
- `es:Array.join` × 17
- `es:Map.size` × 17
- `three:Mesh.geometry` × 17
- `es:ArrayLike` × 16
- `es:Number.toFixed` × 16
- `three:BufferGeometry.setAttribute` × 16
- `es:Math.ceil` × 15
- `es:Readonly` × 15
- `es:Set.size` × 15
- `es:Uint8Array` × 15
- `three:Object3D.visible` × 15
- `dom:KeyboardEvent.key` × 14
- `dom:localStorage` × 14
- `es:Array.find` × 14
- `es:Set.clear` × 14
- `es:Array.every` × 13
- `es:ArrayConstructor.isArray` × 13
- `es:Pick` × 13
- `es:ReadonlyArray.find` × 13
- `three:BufferGeometry` × 13
- `es:Math.atan2` × 12
- `es:ReadonlyArray.forEach` × 12
- `three:Object3D.userData` × 12
- `dom:console` × 11
- `es:Array.reverse` × 11
- `es:ArrayLike.length` × 11
- `es:String.replace` × 11
- `es:WeakMap.get` × 11
- `three:Group` × 11
- `dom:HTMLDivElement` × 10
- `es:Array.forEach` × 10
- `es:Promise.then` × 10
- `es:ReadonlyArray.slice` × 10
- `es:ReadonlySet.has` × 10
- `three:CompressedTexture` × 10
- `dom:Console.warn` × 9
- `dom:HTMLButtonElement` × 9
- `dom:HTMLDivElement.addEventListener` × 9
- `es:Float32Array.buffer` × 9
- `es:Float64ArrayConstructor.from` × 9
- `es:ObjectConstructor.keys` × 9
- `es:ReturnType` × 9
- `es:String.startsWith` × 9
- `es:WeakRef` × 9
- `three:BufferGeometry.boundingBox` × 9
- `three:Object3D.add` × 9
- `three:Object3D.name` × 9
- `dom:HTMLButtonElement.disabled` × 8
- `dom:MessageEvent.data` × 8
- `dom:window` × 8
- `es:Extract` × 8
- `es:Generator` × 8
- `es:Generator.next` × 8
- `es:ReadonlyArray.every` × 8
- `es:ReadonlyArray.reduce` × 8
- `es:Uint16Array` × 8
- `es:WeakMap.set` × 8
- `three:BufferGeometry.dispose` × 8
- `dom:Element.classList` × 7
- `dom:ElementCSSInlineStyle.style` × 7
- `dom:HTMLElement.title` × 7
- `dom:PointerEvent.pointerId` × 7
- `es:Array.splice` × 7
- `es:Math.tan` × 7
- `es:ObjectConstructor.freeze` × 7
- `es:ReadonlyArray.flatMap` × 7
- `es:ReadonlyMap.values` × 7
- `three:BufferAttribute.count` × 7
- `three:BufferGeometry.boundingSphere` × 7
- `three:MeshStandardMaterial` × 7
- `dom:Element.getAttribute` × 6
- `dom:HTMLInputElement.value` × 6
- `dom:ParentNode.replaceChildren` × 6
- `es:Array.flatMap` × 6
- `es:Array.includes` × 6
- `es:ArrayConstructor.from` × 6
- `es:Float32Array.set` × 6
- `es:Float32Array.slice` × 6
- `es:Float64Array.buffer` × 6
- `es:Float64Array.set` × 6
- `es:Float64Array.subarray` × 6
- `es:Int32Array.fill` × 6
- `es:Int32Array.length` × 6
- `es:IteratorReturnResult.value` × 6
- `es:JSON` × 6
- `es:Partial` × 6
- `es:ReadonlyArray.join` × 6
- `three:BufferGeometry.index` × 6
- `three:Color.b` × 6
- `three:Color.g` × 6
- `three:Color.r` × 6
- `three:Object3D.renderOrder` × 6
- `three:Texture` × 6
- `dom:Element` × 5
- `dom:Element.closest` × 5
- `dom:Event.target` × 5
- `dom:Storage.getItem` × 5
- `dom:Storage.setItem` × 5
- `es:Array.fill` × 5
- `es:Array.pop` × 5
- `es:Math.acos` × 5
- `es:Math.sign` × 5
- `es:PromiseConstructor.all` × 5
- `es:ReadonlyArray.findIndex` × 5
- `es:String.slice` × 5
- `es:String.split` × 5
- `three:Box3.max` × 5
- `three:Box3.min` × 5
- `three:Material.uuid` × 5
- `three:Object3D.receiveShadow` × 5
- `three:ZSTDDecoder` × 5
- `dom:CanvasGradient.addColorStop` × 4
- `dom:Event.stopPropagation` × 4
- `dom:MouseEvent.clientX` × 4
- `dom:Window.innerHeight` × 4
- `dom:Worker` × 4
- `dom:Worker.postMessage` × 4
- `es:Exclude` × 4
- `es:IteratorYieldResult.done` × 4
- `es:JSON.stringify` × 4
- `es:MapIterator` × 4
- `es:Math.exp` × 4
- `es:Math.sqrt` × 4
- `es:Number.toString` × 4
- `es:NumberConstructor.isInteger` × 4
- `es:NumberConstructor.isNaN` × 4
- `es:NumberConstructor.NaN` × 4
- `es:ReadonlyArray.indexOf` × 4
- `es:String.length` × 4
- `es:String.trim` × 4
- `three:BatchedMesh.setVisibleAt` × 4
- `three:BufferGeometry.attributes` × 4
- `three:BufferGeometry.computeBoundingSphere` × 4
- `three:BufferGeometry.setIndex` × 4
- `three:CompressedTexture.mipmaps` × 4
- `three:Material.dispose` × 4
- `three:Vector3.x` × 4
- `three:Vector3.y` × 4
- `three:Vector3.z` × 4
- `three:WebGLProgramParameters.fragmentShader` × 4
- `three:WebGLProgramParameters.vertexShader` × 4
- `dom:CanvasDrawPath.beginPath` × 3
- `dom:CanvasDrawPath.fill` × 3
- `dom:ChildNode.remove` × 3
- `dom:CSSStyleDeclaration.cssText` × 3
- `dom:Document.body` × 3
- `dom:DOMTokenList.add` × 3
- `dom:Element.getBoundingClientRect` × 3
- `dom:Event.preventDefault` × 3
- `dom:HTMLOrSVGElement.focus` × 3
- `dom:ParentNode.querySelectorAll` × 3
- `dom:Worker.terminate` × 3
- `earcut:earcut` × 3
- `es:Array.flat` × 3
- `es:Array.indexOf` × 3
- `es:ObjectConstructor.entries` × 3
- `es:ObjectConstructor.fromEntries` × 3
- `es:Omit` × 3
- `es:ReadonlyMap.keys` × 3
- `es:ReadonlyMap.size` × 3
- `es:ReadonlySet.size` × 3
- `es:Uint32Array.buffer` × 3
- `es:WeakMap` × 3
- `meshoptimizer:MeshoptSimplifier` × 3
- `three:Color.multiplyScalar` × 3
- `three:KTX2Loader` × 3
- `three:Material.customProgramCacheKey` × 3
- `three:Material.onBeforeCompile` × 3
- `three:MaterialProperties.userData` × 3
- `three:Object3D.castShadow` × 3
- `three:Object3D.matrixAutoUpdate` × 3
- `three:Object3D.parent` × 3
- `three:Texture.needsUpdate` × 3
- `clipper2-ts:EndType` × 2
- `clipper2-ts:EndType.Polygon` × 2
- `clipper2-ts:inflatePaths` × 2
- `clipper2-ts:JoinType` × 2
- `clipper2-ts:JoinType.Miter` × 2
- `dom:AbortController` × 2
- `dom:AbstractWorker.onerror` × 2
- `dom:CanvasFillStrokeStyles.createLinearGradient` × 2
- `dom:CanvasPath.arc` × 2
- `dom:CanvasPath.lineTo` × 2
- `dom:CanvasTransform.scale` × 2
- `dom:Console.error` × 2
- `dom:CSSStyleDeclaration.bottom` × 2
- `dom:DOMTokenList.toggle` × 2
- `dom:Element.clientWidth` × 2
- `dom:ErrorEvent.message` × 2
- `dom:fetch` × 2
- `dom:FocusEvent.relatedTarget` × 2
- `dom:GlobalEventHandlers.onchange` × 2
- `dom:HTMLCanvasElement.getContext` × 2
- `dom:HTMLCanvasElement.height` × 2
- `dom:HTMLCanvasElement.toDataURL` × 2
- `dom:HTMLCanvasElement.width` × 2
- `dom:HTMLDetailsElement.open` × 2
- `dom:HTMLInputElement.type` × 2
- `dom:HTMLSelectElement.value` × 2
- `dom:MessageEvent` × 2
- `dom:MessageEventTarget.onmessage` × 2
- `dom:Node.contains` × 2
- `dom:Node.isConnected` × 2
- `dom:Response.ok` × 2
- `dom:Response.status` × 2
- `dom:WebGLRenderingContextBase.getExtension` × 2
- `dom:Window.addEventListener` × 2
- `es:Array.reduce` × 2
- `es:Array.shift` × 2
- `es:Array.unshift` × 2
- `es:CallableFunction.call` × 2
- `es:Float32Array.fill` × 2
- `es:JSON.parse` × 2
- `es:Map.entries` × 2
- `es:Math.log` × 2
- `es:Math.SQRT2` × 2
- `es:ObjectConstructor.is` × 2
- `es:ObjectConstructor.values` × 2
- `es:Promise.catch` × 2
- `es:PromiseConstructor.reject` × 2
- `es:PromiseConstructor.resolve` × 2
- `es:ReadonlyMap.has` × 2
- `es:RegExp.test` × 2
- `es:Required` × 2
- `es:SharedArrayBuffer` × 2
- `es:String.toUpperCase` × 2
- `es:Symbol` × 2
- `es:Uint32Array.slice` × 2
- `es:Uint32Array.subarray` × 2
- `es:WeakRef.deref` × 2
- `three:Box3` × 2
- `three:BufferAttribute.itemSize` × 2
- `three:BufferGeometry.computeBoundingBox` × 2
- `three:BufferGeometry.getAttribute` × 2
- `three:Color.clone` × 2
- `three:Frustum` × 2
- `three:InstancedMesh` × 2
- `three:MaterialProperties.name` × 2
- `three:MaterialProperties.vertexColors` × 2
- `three:Mesh.material` × 2
- `three:MeshDepthMaterial` × 2
- `three:Object3D.children` × 2
- `three:Object3D.clear` × 2
- `three:Object3D.remove` × 2
- `three:RepeatWrapping` × 2
- `three:Texture.anisotropy` × 2
- `three:UnsignedByteType` × 2
- `three:Vector3` × 2
- `three:Vector3.set` × 2
- `vite:ImportMeta.env` × 2
- `vite:ImportMetaEnv.BASE_URL` × 2
- `clipper2-ts:Clipper64` × 1
- `clipper2-ts:Clipper64.addPaths` × 1
- `clipper2-ts:Clipper64.execute` × 1
- `clipper2-ts:ClipType` × 1
- `clipper2-ts:ClipType.Union` × 1
- `clipper2-ts:FillRule` × 1
- `clipper2-ts:FillRule.NonZero` × 1
- `clipper2-ts:PathType` × 1
- `clipper2-ts:PathType.Subject` × 1
- `clipper2-ts:PolyTree64` × 1
- `dom:AbortController.abort` × 1
- `dom:AbortController.signal` × 1
- `dom:Body.arrayBuffer` × 1
- `dom:Body.json` × 1
- `dom:CanvasPath.closePath` × 1
- `dom:CanvasPath.moveTo` × 1
- `dom:CanvasRenderingContext2D` × 1
- `dom:CSSStyleDeclaration.background` × 1
- `dom:CSSStyleDeclaration.left` × 1
- `dom:DocumentOrShadowRoot.activeElement` × 1
- `dom:DOMRect.width` × 1
- `dom:DOMRectReadOnly.bottom` × 1
- `dom:DOMRectReadOnly.left` × 1
- `dom:DOMRectReadOnly.right` × 1
- `dom:DOMRectReadOnly.top` × 1
- `dom:DOMTokenList.contains` × 1
- `dom:DOMTokenList.remove` × 1
- `dom:Element.hasPointerCapture` × 1
- `dom:Element.releasePointerCapture` × 1
- `dom:Element.scrollIntoView` × 1
- `dom:Element.setPointerCapture` × 1
- `dom:GlobalEventHandlers.oninput` × 1
- `dom:GlobalEventHandlers.ontoggle` × 1
- `dom:HTMLElement.addEventListener` × 1
- `dom:HTMLImageElement` × 1
- `dom:HTMLImageElement.src` × 1
- `dom:HTMLInputElement` × 1
- `dom:HTMLInputElement.addEventListener` × 1
- `dom:HTMLInputElement.disabled` × 1
- `dom:HTMLInputElement.max` × 1
- `dom:HTMLInputElement.maxLength` × 1
- `dom:HTMLInputElement.min` × 1
- `dom:HTMLInputElement.placeholder` × 1
- `dom:HTMLInputElement.step` × 1
- `dom:HTMLOptionElement.value` × 1
- `dom:HTMLOutputElement.value` × 1
- `dom:KeyboardEvent` × 1
- `dom:location` × 1
- `dom:Location.search` × 1
- `dom:MessageEventTarget.onmessageerror` × 1
- `dom:MouseEvent.button` × 1
- `dom:NodeListOf.forEach` × 1
- `dom:OffscreenCanvas` × 1
- `dom:OffscreenCanvas.getContext` × 1
- `dom:Performance.measure` × 1
- `dom:PointerEvent` × 1
- `dom:ResizeObserver` × 1
- `dom:ResizeObserver.disconnect` × 1
- `dom:ResizeObserver.observe` × 1
- `dom:setTimeout` × 1
- `dom:structuredClone` × 1
- `dom:SVGElement` × 1
- `dom:URLSearchParams` × 1
- `dom:URLSearchParams.get` × 1
- `dom:WEBGL_lose_context.loseContext` × 1
- `dom:WebGLRenderingContextBase.getSupportedExtensions` × 1
- `dom:Window.innerWidth` × 1
- `dom:Window.removeEventListener` × 1
- `es:CallableFunction.bind` × 1
- `es:Date` × 1
- `es:DateConstructor.now` × 1
- `es:Float32Array.byteOffset` × 1
- `es:Int32Array.slice` × 1
- `es:Int8Array` × 1
- `es:Int8Array.length` × 1
- `es:IterableIterator` × 1
- `es:Iterator.next` × 1
- `es:IteratorReturnResult.done` × 1
- `es:IteratorYieldResult.value` × 1
- `es:Math.SQRT1_2` × 1
- `es:NonNullable` × 1
- `es:ObjectConstructor.create` × 1
- `es:Promise.finally` × 1
- `es:String.localeCompare` × 1
- `es:String.toLowerCase` × 1
- `es:SymbolConstructor.iterator` × 1
- `es:Uint16Array.length` × 1
- `es:Uint32Array.fill` × 1
- `es:Uint32Array.set` × 1
- `es:Uint32ArrayConstructor.from` × 1
- `es:Uint8Array.buffer` × 1
- `es:Uint8Array.set` × 1
- `es:Uint8ArrayConstructor.from` × 1
- `meshoptimizer:MeshoptSimplifier.ready` × 1
- `meshoptimizer:MeshoptSimplifier.simplifyWithAttributes` × 1
- `meshoptimizer:MeshoptSimplifier.supported` × 1
- `three:BatchedMesh.addGeometry` × 1
- `three:BatchedMesh.addInstance` × 1
- `three:BatchedMesh.computeBoundingBox` × 1
- `three:BatchedMesh.computeBoundingSphere` × 1
- `three:BatchedMesh.sortObjects` × 1
- `three:Box3.clone` × 1
- `three:Box3.copy` × 1
- `three:Box3.expandByScalar` × 1
- `three:Box3.getBoundingSphere` × 1
- `three:Box3.getCenter` × 1
- `three:Box3.getSize` × 1
- `three:BoxGeometry` × 1
- `three:BufferAttribute.getComponent` × 1
- `three:BufferAttribute.normalized` × 1
- `three:BufferGeometry.translate` × 1
- `three:Color.setRGB` × 1
- `three:CompressedArrayTexture` × 1
- `three:CompressedPixelFormat` × 1
- `three:CompressedTexture.generateMipmaps` × 1
- `three:CompressedTextureMipmap.data` × 1
- `three:Frustum.intersectsBox` × 1
- `three:KTX2Loader.detectSupport` × 1
- `three:KTX2Loader.dispose` × 1
- `three:KTX2Loader.parse` × 1
- `three:KTX2Loader.setTranscoderPath` × 1
- `three:Layers.disableAll` × 1
- `three:LinearFilter` × 1
- `three:LinearMipmapLinearFilter` × 1
- `three:Matrix4` × 1
- `three:NoColorSpace` × 1
- `three:Object3D` × 1
- `three:Object3D.frustumCulled` × 1
- `three:Object3D.layers` × 1
- `three:Object3D.removeFromParent` × 1
- `three:Object3D.updateMatrix` × 1
- `three:RGBAFormat` × 1
- `three:Sphere` × 1
- `three:Sphere.clone` × 1
- `three:Sphere.radius` × 1
- `three:SRGBColorSpace` × 1
- `three:Texture.colorSpace` × 1
- `three:Texture.dispose` × 1
- `three:Texture.magFilter` × 1
- `three:Texture.minFilter` × 1
- `three:Texture.name` × 1
- `three:Texture.onUpdate` × 1
- `three:Texture.wrapS` × 1
- `three:Texture.wrapT` × 1
- `three:WebGLRenderer` × 1
- `three:ZSTDDecoder.decode` × 1
- `three:ZSTDDecoder.init` × 1
