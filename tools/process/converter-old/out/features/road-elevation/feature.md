# Feature: src/world/elevation.ts, src/world/structures.ts, src/render/structures.ts, src/render/structureCells.ts, src/world/junction/levels.ts, src/world/roads/tuning.ts, src/world/roads/buildMode.ts, src/world/roadField.ts, src/ui/roads/cutWalls.ts

The program starts from `src/main.ts`, `src/peopleLab/main.ts`. A declaration is the feature's own when the program reaches it only through the feature; in a declaration of the program that names the feature, its statements are a slice (what names it, what depends on that, what that controls).

- Own: 373 declarations
- Slices of shared code: 209
- Code: 6038 lines in 66 files (source/)
- Resources: 29 (23 entry, 6 style, 0 import; 21 only the feature uses)
- Uses: 190 shared declarations (interface/: 927 declarations in 229 files, the types they name included); hooks: 96; external APIs: 146

## Code

- src/core/aabb.ts — part, 6 lines
- src/core/rerootMap.ts — part, 73 lines
- src/core/ring.ts — part, 9 lines
- src/editor/commit.ts — part, 137 lines
- src/editor/editRules.ts — part, 35 lines
- src/editor/growthFeed.ts — part, 9 lines
- src/editor/growthReplica.ts — part, 4 lines
- src/editor/persistence.ts — part, 5 lines
- src/editor/poleRoutes.ts — part, 7 lines
- src/editor/poles.ts — part, 38 lines
- src/editor/repair.ts — part, 11 lines
- src/editor/roads/economy.ts — part, 3 lines
- src/editor/roadTool.ts — part, 22 lines
- src/editor/smallCityMap.ts — part, 8 lines
- src/editor/snap.ts — part, 14 lines
- src/lab/catalog/layers.ts — part, 3 lines
- src/lab/catalog/layout.ts — part, 14 lines
- src/main.ts — part, 187 lines
- src/render/grass.ts — part, 12 lines
- src/render/groundShape.ts — part, 71 lines
- src/render/networkFarRoads.ts — part, 12 lines
- src/render/poleLayer.ts — part, 5 lines
- src/render/renderer.ts — part, 556 lines
- src/render/roadStandIn.ts — part, 7 lines
- src/render/roadStations.ts — part, 7 lines
- src/render/roadStations.worker.ts — part, 8 lines
- src/render/roadSurfaceEntries.ts — part, 3 lines
- src/render/roadSurfaceInputs.ts — part, 39 lines
- src/render/roadSurfacePass.ts — part, 19 lines
- src/render/roadSurfaceReads.ts — part, 115 lines
- src/render/roadSurfaces.ts — part, 222 lines
- src/render/roadTileMesh.ts — part, 49 lines
- src/render/signLayer.ts — part, 24 lines
- src/render/streetFurnitureLayer.ts — part, 30 lines
- src/render/structureCells.ts — whole, 116 lines
- src/render/structures.ts — whole, 716 lines
- src/render/utilities.ts — part, 10 lines
- src/sim/roads/controlAdvisor.ts — part, 14 lines
- src/sim/roads/flowStats.ts — part, 15 lines
- src/ui/roads/cutWalls.ts — whole, 18 lines
- src/ui/roads/money.ts — part, 3 lines
- src/ui/v2/shell.ts — part, 11 lines
- src/world/buildings/validate.ts — part, 3 lines
- src/world/carriageway.ts — part, 10 lines
- src/world/doc.ts — part, 93 lines
- src/world/economy.ts — part, 28 lines
- src/world/elevation.ts — whole, 1541 lines
- src/world/junction/build.ts — part, 3 lines
- src/world/junction/bulb.ts — part, 7 lines
- src/world/junction/levels.ts — whole, 345 lines
- src/world/junction/trimSummary.ts — part, 21 lines
- src/world/lanelets.ts — part, 3 lines
- src/world/markings.ts — part, 58 lines
- src/world/network.ts — part, 11 lines
- src/world/networkBuild.ts — part, 60 lines
- src/world/paving.ts — part, 27 lines
- src/world/ribbons.ts — part, 3 lines
- src/world/roadField.ts — whole, 751 lines
- src/world/roads/buildMode.ts — whole, 125 lines
- src/world/roads/tuning.ts — whole, 86 lines
- src/world/structures.ts — whole, 76 lines
- src/world/surfaceParts.ts — part, 9 lines
- src/world/surfaces.ts — part, 17 lines
- src/world/utilities.ts — part, 3 lines
- src/world/walkingLines.ts — part, 55 lines
- src/world/walkways.ts — part, 36 lines

## Slices of shared code

- `src/core/rerootMap.ts#RerootMap.get` — 8 of 9 lines
- `src/editor/commit.ts#AUTO_TUNNEL_COVER` — 1 of 1 lines
- `src/editor/commit.ts#CROSSING_CLEARANCE` — 1 of 1 lines
- `src/editor/commit.ts#DraftResult` — 4 of 14 lines
- `src/editor/commit.ts#alreadyJoined` — 7 of 26 lines
- `src/editor/commit.ts#boreDeepCuts` — 14 of 38 lines
- `src/editor/commit.ts#commitDraftInPlace` — 18 of 147 lines
- `src/editor/commit.ts#commitRoadPath` — 44 of 97 lines
- `src/editor/commit.ts#draftStations` — 8 of 24 lines
- `src/editor/commit.ts#lastDryRun` — 6 of 6 lines
- `src/editor/commit.ts#layCopy` — 5 of 11 lines
- `src/editor/commit.ts#onTerrain` — 1 of 1 lines
- `src/editor/commit.ts#reconcileMovedNode` — 11 of 71 lines
- `src/editor/commit.ts#segmentOffsetAt` — 3 of 6 lines
- `src/editor/commit.ts#solveOver` — 8 of 9 lines
- `src/editor/commit.ts#terrainIndexOf` — 6 of 6 lines
- `src/editor/editRules.ts#MAX_BUILT_GRADE` — 1 of 1 lines
- `src/editor/editRules.ts#PASS_CLEARANCE` — 1 of 1 lines
- `src/editor/editRules.ts#deckAt` — 3 of 6 lines
- `src/editor/editRules.ts#flatSolves` — 4 of 7 lines
- `src/editor/editRules.ts#flatSolves.after` — 1 of 1 lines
- `src/editor/editRules.ts#flatSolves.beforeSolve` — 1 of 1 lines
- `src/editor/editRules.ts#grade` — 6 of 23 lines
- `src/editor/editRules.ts#overlapsAnother` — 8 of 49 lines
- `src/editor/editRules.ts#refuseRoadEdit.deck` — 1 of 1 lines
- `src/editor/editRules.ts#tooSteep` — 9 of 19 lines
- `src/editor/growthFeed.ts#junctionPiece` — 9 of 10 lines
- `src/editor/growthReplica.ts#RoadPiece` — 3 of 10 lines
- `src/editor/growthReplica.ts#Structured` — 1 of 1 lines
- `src/editor/persistence.ts#isSerializedDoc` — 5 of 98 lines
- `src/editor/poleRoutes.ts#buildRouteGraph` — 7 of 73 lines
- `src/editor/poles.ts#computePoleRun` — 8 of 91 lines
- `src/editor/poles.ts#planPoleRun` — 10 of 19 lines
- `src/editor/poles.ts#poleRunPlanner.plan` — 20 of 21 lines
- `src/editor/repair.ts#nearestCandidate` — 11 of 33 lines
- `src/editor/roadTool.ts#RoadTool.cost` — 3 of 4 lines
- `src/editor/roadTool.ts#RoadTool.judge` — 3 of 13 lines
- `src/editor/roadTool.ts#RoadTool.judged` — 2 of 2 lines
- `src/editor/roadTool.ts#RoadTool.stations` — 3 of 4 lines
- `src/editor/roadTool.ts#RoadTool.verdict` — 7 of 14 lines
- `src/editor/roadTool.ts#RoadToolHost` — 4 of 36 lines
- `src/editor/roads/economy.ts#roadsBefore` — 3 of 3 lines
- `src/editor/smallCityMap.ts#growLots` — 7 of 17 lines
- `src/editor/smallCityMap.ts#growLots.pavedAt` — 1 of 1 lines
- `src/editor/snap.ts#anchorHeightOffset` — 5 of 15 lines
- `src/editor/snap.ts#findAnchor` — 9 of 79 lines
- `src/lab/catalog/layers.ts#signItems.one` — 3 of 9 lines
- `src/lab/catalog/layout.ts#road` — 3 of 3 lines
- `src/lab/catalog/layout.ts#surfacesOf` — 11 of 26 lines
- `src/main.ts#BUILD_FILL` — 7 of 7 lines
- `src/main.ts#buildSummary` — 9 of 9 lines
- `src/main.ts#currentStatus` — 5 of 20 lines
- `src/main.ts#drawBuildModes` — 38 of 39 lines
- `src/main.ts#drawOverlayScreen` — 99 of 469 lines
- `src/main.ts#drawTunnelStretches` — 8 of 15 lines
- `src/main.ts#roadTool` — 5 of 29 lines
- `src/main.ts#setNodeHeightMetres` — 16 of 26 lines
- `src/render/grass.ts#grassClump` — 12 of 57 lines
- `src/render/groundShape.ts#BlockDigestAsk` — 3 of 7 lines
- `src/render/groundShape.ts#BlockRoads` — 1 of 1 lines
- `src/render/groundShape.ts#GroundShapeAsk` — 3 of 5 lines
- `src/render/groundShape.ts#blockDigest` — 12 of 15 lines
- `src/render/groundShape.ts#digestBlocks` — 11 of 24 lines
- `src/render/groundShape.ts#shapeGroundAsk` — 41 of 45 lines
- `src/render/networkFarRoads.ts#farRoadInput` — 11 of 36 lines
- `src/render/networkFarRoads.ts#farRoadInput.onGround` — 1 of 1 lines
- `src/render/renderer.ts#SURFACE_SETS` — 1 of 1 lines
- `src/render/renderer.ts#SceneHandle` — 5 of 170 lines
- `src/render/renderer.ts#createSceneRenderer` — 345 of 3774 lines
- `src/render/renderer.ts#createSceneRenderer.blockRead` — 9 of 9 lines
- `src/render/renderer.ts#createSceneRenderer.buildWorld` — 12 of 78 lines
- `src/render/renderer.ts#createSceneRenderer.compareSolves` — 74 of 74 lines
- `src/render/renderer.ts#createSceneRenderer.completeSolve` — 12 of 24 lines
- `src/render/renderer.ts#createSceneRenderer.deckHeight` — 4 of 8 lines
- `src/render/renderer.ts#createSceneRenderer.landGroundDigest` — 5 of 6 lines
- `src/render/renderer.ts#createSceneRenderer.levelGroundDigest` — 6 of 8 lines
- `src/render/renderer.ts#createSceneRenderer.readOf` — 5 of 5 lines
- `src/render/renderer.ts#createSceneRenderer.requestNetworkFar` — 4 of 4 lines
- `src/render/renderer.ts#createSceneRenderer.shapeGround` — 9 of 47 lines
- `src/render/renderer.ts#createSceneRenderer.stationsAsked` — 2 of 2 lines
- `src/render/renderer.ts#createSceneRenderer.switchedOff` — 16 of 16 lines
- `src/render/renderer.ts#createSceneRenderer.takeEditedCells` — 18 of 26 lines
- `src/render/renderer.ts#createSceneRenderer.warmRoadShaders` — 15 of 39 lines
- `src/render/renderer.ts#createSceneRenderer.worldSteps` — 14 of 105 lines
- `src/render/roadStandIn.ts#RoadStandIn.show` — 7 of 32 lines
- `src/render/roadStations.ts#Hint` — 2 of 4 lines
- `src/render/roadStations.ts#RoadStations.current` — 2 of 3 lines
- `src/render/roadStations.ts#RoadStations.waiting` — 3 of 30 lines
- `src/render/roadStations.worker.ts#serve` — 8 of 13 lines
- `src/render/roadSurfaceEntries.ts#SurfacePass` — 3 of 6 lines
- `src/render/roadSurfaceInputs.ts#SurfaceInputs.ownerOf` — 10 of 16 lines
- `src/render/roadSurfaceInputs.ts#SurfaceInputs.passList` — 7 of 7 lines
- `src/render/roadSurfaceInputs.ts#SurfaceInputs.placeNode` — 22 of 81 lines
- `src/render/roadSurfacePass.ts#passDesc` — 6 of 19 lines
- `src/render/roadSurfacePass.ts#passRaised` — 8 of 12 lines
- `src/render/roadSurfacePass.ts#passReads` — 5 of 23 lines
- `src/render/roadSurfaceReads.ts#PassDesc` — 3 of 22 lines
- `src/render/roadSurfaceReads.ts#SurfaceReads` — 3 of 15 lines
- `src/render/roadSurfaceReads.ts#passSurfaces` — 66 of 252 lines
- `src/render/roadSurfaceReads.ts#passSurfaces.asphaltSection` — 8 of 8 lines
- `src/render/roadSurfaceReads.ts#passSurfaces.asphaltTint` — 13 of 17 lines
- `src/render/roadSurfaceReads.ts#passSurfaces.footwaySection` — 8 of 9 lines
- `src/render/roadSurfaceReads.ts#passSurfaces.footwayTint` — 7 of 7 lines
- `src/render/roadSurfaceReads.ts#passSurfaces.frameFor` — 7 of 8 lines
- `src/render/roadSurfaces.ts#buildRoadSurfaces` — 9 of 14 lines
- `src/render/roadSurfaces.ts#roadSurfaceSteps` — 212 of 512 lines
- `src/render/roadSurfaces.ts#roadSurfaceSteps.reading` — 1 of 1 lines
- `src/render/roadTileMesh.ts#SurfaceData` — 3 of 12 lines
- `src/render/roadTileMesh.ts#TileMesher.constructor` — 3 of 7 lines
- `src/render/roadTileMesh.ts#TileMesher.give` — 3 of 7 lines
- `src/render/roadTileMesh.ts#TileMesher.heights` — 1 of 1 lines
- `src/render/roadTileMesh.ts#TileMesher.need` — 3 of 17 lines
- `src/render/roadTileMesh.ts#TileMesher.need.take` — 1 of 1 lines
- `src/render/roadTileMesh.ts#TileMesher.renew` — 3 of 4 lines
- `src/render/roadTileMesh.ts#TileMesher.run` — 16 of 29 lines
- `src/render/roadTileMesh.ts#readsOf` — 12 of 37 lines
- `src/render/roadTileMesh.ts#readsOf.terrainAt` — 4 of 6 lines
- `src/render/signLayer.ts#SignLayerKept` — 3 of 14 lines
- `src/render/signLayer.ts#SignSource` — 3 of 5 lines
- `src/render/signLayer.ts#itemSigns` — 12 of 42 lines
- `src/render/signLayer.ts#junctionSigns` — 6 of 16 lines
- `src/render/streetFurnitureLayer.ts#FurnitureSource` — 3 of 6 lines
- `src/render/streetFurnitureLayer.ts#StreetFurnitureLayer` — 3 of 17 lines
- `src/render/streetFurnitureLayer.ts#furniturePlace` — 19 of 65 lines
- `src/render/streetFurnitureLayer.ts#meadowInto` — 5 of 9 lines
- `src/render/utilities.ts#poleGroundAt` — 10 of 14 lines
- `src/sim/roads/controlAdvisor.ts#ControlAdvisor.evaluate` — 4 of 11 lines
- `src/sim/roads/controlAdvisor.ts#F` — 1 of 1 lines
- `src/sim/roads/controlAdvisor.ts#QUIET_FLOW` — 1 of 1 lines
- `src/sim/roads/controlAdvisor.ts#warrantedControl` — 7 of 8 lines
- `src/sim/roads/controlAdvisor.ts#warrantedControl.k` — 1 of 1 lines
- `src/sim/roads/flowStats.ts#FlowStats.step` — 10 of 21 lines
- `src/sim/roads/flowStats.ts#KEEP` — 1 of 1 lines
- `src/sim/roads/flowStats.ts#TAU` — 1 of 1 lines
- `src/sim/roads/flowStats.ts#agedTo` — 3 of 4 lines
- `src/ui/roads/money.ts#balanceTip` — 3 of 3 lines
- `src/ui/v2/shell.ts#mountShell.renderRoads` — 11 of 116 lines
- `src/world/buildings/validate.ts#RoadReach` — 3 of 9 lines
- `src/world/carriageway.ts#onRoadGround` — 10 of 17 lines
- `src/world/doc.ts#RoadDoc.addSegment` — 7 of 34 lines
- `src/world/doc.ts#RoadDoc.connects` — 5 of 8 lines
- `src/world/doc.ts#RoadDoc.economy` — 1 of 1 lines
- `src/world/doc.ts#RoadDoc.fromJSON` — 3 of 187 lines
- `src/world/doc.ts#RoadDoc.layOver` — 3 of 47 lines
- `src/world/doc.ts#RoadDoc.replaceContents` — 7 of 149 lines
- `src/world/doc.ts#RoadDoc.restoreValue` — 5 of 27 lines
- `src/world/doc.ts#RoadDoc.savedGlobals` — 7 of 18 lines
- `src/world/doc.ts#RoadDoc.setBalance` — 4 of 5 lines
- `src/world/doc.ts#RoadDoc.setNodeHeightOffset` — 5 of 8 lines
- `src/world/doc.ts#RoadDoc.setSegmentStructure` — 4 of 6 lines
- `src/world/doc.ts#RoadDoc.takeFork` — 3 of 49 lines
- `src/world/doc.ts#RoadDoc.valueOf` — 5 of 15 lines
- `src/world/doc.ts#RoadDoc.worldPart` — 5 of 26 lines
- `src/world/doc.ts#RoadSegment` — 3 of 21 lines
- `src/world/doc.ts#SerializedDoc` — 18 of 75 lines
- `src/world/doc.ts#nodeMovesTraffic` — 3 of 5 lines
- `src/world/doc.ts#segmentRecord` — 5 of 19 lines
- `src/world/economy.ts#DEFAULT_ECONOMY` — 1 of 1 lines
- `src/world/economy.ts#E` — 1 of 1 lines
- `src/world/economy.ts#PER_SQUARE_UNIT` — 5 of 5 lines
- `src/world/economy.ts#buildLengths` — 9 of 17 lines
- `src/world/economy.ts#chargeFor` — 4 of 10 lines
- `src/world/economy.ts#pricePerUnit` — 4 of 5 lines
- `src/world/economy.ts#segmentCost` — 4 of 8 lines
- `src/world/junction/build.ts#surfaceMode` — 3 of 33 lines
- `src/world/junction/bulb.ts#clearBulbsOf` — 7 of 32 lines
- `src/world/junction/trimSummary.ts#trimSummary` — 21 of 28 lines
- `src/world/lanelets.ts#LaneletGraph.buildSegmentLinks` — 3 of 94 lines
- `src/world/markings.ts#junctionDetailAt` — 19 of 22 lines
- `src/world/markings.ts#transitionMarkings` — 39 of 51 lines
- `src/world/network.ts#Network.continues` — 6 of 10 lines
- `src/world/network.ts#Network.junctionAt` — 3 of 3 lines
- `src/world/network.ts#Network.junctionCells` — 1 of 1 lines
- `src/world/network.ts#Network.junctions` — 1 of 1 lines
- `src/world/networkBuild.ts#BakedMemo` — 1 of 1 lines
- `src/world/networkBuild.ts#MemoEntry` — 3 of 7 lines
- `src/world/networkBuild.ts#Network` — 4 of 14 lines
- `src/world/networkBuild.ts#NetworkBuild.finalTrims` — 9 of 14 lines
- `src/world/networkBuild.ts#NetworkBuild.levels3` — 2 of 13 lines
- `src/world/networkBuild.ts#NetworkBuild.rebuild` — 11 of 201 lines
- `src/world/networkBuild.ts#NetworkBuild.solveNode` — 3 of 29 lines
- `src/world/networkBuild.ts#NetworkBuild.squeeze` — 18 of 20 lines
- `src/world/networkBuild.ts#memoData` — 3 of 4 lines
- `src/world/networkBuild.ts#memoEntry` — 3 of 3 lines
- `src/world/networkBuild.ts#memoOf` — 3 of 3 lines
- `src/world/paving.ts#PavingData` — 3 of 5 lines
- `src/world/paving.ts#SAMPLE` — 1 of 1 lines
- `src/world/paving.ts#pavedHeightFrom` — 12 of 13 lines
- `src/world/paving.ts#pavingReader` — 11 of 23 lines
- `src/world/ribbons.ts#junctionBounds` — 3 of 13 lines
- `src/world/surfaceParts.ts#surfaceParts` — 9 of 37 lines
- `src/world/surfaces.ts#SurfaceNet` — 7 of 10 lines
- `src/world/surfaces.ts#ownsJunction` — 10 of 16 lines
- `src/world/utilities.ts#DEFAULT_POLE_SPACING` — 1 of 1 lines
- `src/world/utilities.ts#MAX_POLE_SPACING` — 1 of 1 lines
- `src/world/utilities.ts#MIN_POLE_SPACING` — 1 of 1 lines
- `src/world/walkingLines.ts#CornerAsk` — 3 of 4 lines
- `src/world/walkingLines.ts#CornerEnd` — 3 of 7 lines
- `src/world/walkingLines.ts#LocalLines.at` — 8 of 12 lines
- `src/world/walkingLines.ts#LocalLines.nearestOuter` — 5 of 5 lines
- `src/world/walkingLines.ts#LocalLines.onPaving` — 15 of 16 lines
- `src/world/walkingLines.ts#LocalLines.outer` — 8 of 10 lines
- `src/world/walkingLines.ts#LocalLines.paving` — 9 of 9 lines
- `src/world/walkingLines.ts#LocalLines.polygons` — 3 of 3 lines
- `src/world/walkingLines.ts#RingsOf` — 1 of 1 lines
- `src/world/walkways.ts#FootwayShape` — 3 of 17 lines
- `src/world/walkways.ts#Walkway` — 3 of 23 lines
- `src/world/walkways.ts#cornersOf` — 28 of 33 lines
- `src/world/walkways.ts#deckOf` — 2 of 7 lines

## Resources

- feature entry `pavers` — src/render/roadTones.ts:16: `pavers: [1, 1, 1]`
- feature entry `concrete` — src/render/roadTones.ts:17: `concrete: [1.08, 1.08, 1.1]`
- feature entry `stone` — src/render/roadTones.ts:18: `stone: [1, 0.84, 0.66]`
- shared entry `hint.road.gradeLimited` — src/ui/i18n/en.ts:720: `'hint.road.gradeLimited': 'The road reached its safe grade; continue drawing to reach the selected height.'`
- feature entry `buildMode.ground` — src/ui/i18n/en.ts:732: `'buildMode.ground': 'at grade'`
- feature entry `buildMode.embankment` — src/ui/i18n/en.ts:733: `'buildMode.embankment': 'embankment'`
- feature entry `buildMode.cutting` — src/ui/i18n/en.ts:734: `'buildMode.cutting': 'cutting'`
- feature entry `buildMode.bridge` — src/ui/i18n/en.ts:735: `'buildMode.bridge': 'bridge'`
- feature entry `buildMode.tunnel` — src/ui/i18n/en.ts:736: `'buildMode.tunnel': 'tunnel'`
- feature entry `buildMode.cuttingWalls` — src/ui/i18n/en.ts:737: `'buildMode.cuttingWalls': 'walled cutting'`
- feature entry `cutSides.batter` — src/ui/i18n/en.ts:739: `'cutSides.batter': 'Slope'`
- feature entry `cutSides.walls` — src/ui/i18n/en.ts:740: `'cutSides.walls': 'Walls'`
- feature entry `economy.balance` — src/ui/i18n/en.ts:889: `'economy.balance': 'Money in hand. Roads cost by width and length; bridges and tunnels more. Demolishing a road gives back {share}% of its price.'`
- shared entry `hint.road.gradeLimited` — src/ui/i18n/pt-BR.ts:713: `'hint.road.gradeLimited': 'A via atingiu a inclinação segura; continue desenhando para chegar à altura escolhida.'`
- feature entry `buildMode.ground` — src/ui/i18n/pt-BR.ts:725: `'buildMode.ground': 'no chão'`
- feature entry `buildMode.embankment` — src/ui/i18n/pt-BR.ts:726: `'buildMode.embankment': 'aterro'`
- feature entry `buildMode.cutting` — src/ui/i18n/pt-BR.ts:727: `'buildMode.cutting': 'trincheira'`
- feature entry `buildMode.bridge` — src/ui/i18n/pt-BR.ts:728: `'buildMode.bridge': 'ponte'`
- feature entry `buildMode.tunnel` — src/ui/i18n/pt-BR.ts:729: `'buildMode.tunnel': 'túnel'`
- feature entry `buildMode.cuttingWalls` — src/ui/i18n/pt-BR.ts:730: `'buildMode.cuttingWalls': 'trincheira com muro'`
- feature entry `cutSides.batter` — src/ui/i18n/pt-BR.ts:732: `'cutSides.batter': 'Talude'`
- feature entry `cutSides.walls` — src/ui/i18n/pt-BR.ts:733: `'cutSides.walls': 'Muro'`
- feature entry `economy.balance` — src/ui/i18n/pt-BR.ts:882: `'economy.balance': 'Dinheiro em caixa. Vias custam pela largura e pelo comprimento; pontes e túneis custam mais. Demolir uma via devolve {share}% do preço dela.`
- shared style `v2-seg-b` — src/ui/roads/roads.css:162: `.v2-seg-b.rp-seg-text { width: auto; padding: 0 11px; font-size: 12.5px; font-weight: 600; }`
- shared style `v2-seg-b` — src/ui/roads/roads.css:163: `.v2-seg-b.rp-seg-text > span { position: static; width: auto; height: auto; overflow: visible; clip: auto; }`
- shared style `v2-seg-b` — src/ui/v2/shell.css:41: `.v2-icon > span, .v2-pill > span, .v2-seg-b > span, .v2-tabs.compact .v2-tab.icon:not(.on) > span, .v2-tabs.tight .v2-tab.icon > span, .v2-choice.icon > span, .`
- shared style `v2-seg-b` — src/ui/v2/shell.css:58: `.v2-seg-b, .v2-speed-b { position: relative; display: grid; place-items: center; min-width: 34px; height: 28px; padding: 0 6px; border: 0; border-radius: 999px;`
- shared style `v2-seg-b` — src/ui/v2/shell.css:62: `.v2-seg-b:hover, .v2-speed-b:hover { color: var(--v-text); }`
- shared style `v2-seg-b` — src/ui/v2/shell.css:63: `.v2-seg-b.on, .v2-speed-b.on { background: var(--v-accent); color: var(--v-accent-ink); }`

## Where the program plugs the feature in

### src/render/structureCells.ts

- `src/render/structureCells.ts#createStructureCells` (function, line 37) ← call from `src/render/renderer.ts#createSceneRenderer.worldSteps`
- `src/render/structureCells.ts#createStructureCells.dispose` (method, line 106) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/structureCells.ts#createStructureCells.prepare` (method, line 60) ← implementation from `src/render/renderer.ts#createSceneRenderer.worldSteps`
- `src/render/structureCells.ts#createStructureCells.prepare.apply` (method, line 87) ← implementation from `src/render/renderer.ts#createSceneRenderer.worldSteps`
- `src/render/structureCells.ts#StructureCells` (interface, line 28) ← call from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.worldSteps`
- `src/render/structureCells.ts#StructureCells` (interface, line 28) ← read from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.switchedOff`, `src/render/renderer.ts#createSceneRenderer.worldSteps`
- `src/render/structureCells.ts#StructureCells` (interface, line 28) ← typeRef from `src/render/renderer.ts#createSceneRenderer`

### src/render/structures.ts

- `src/render/structures.ts#buildStructureDetails` (function, line 452) ← call from `src/lab/catalog/layout.ts#surfacesOf`, `src/render/renderer.ts#createSceneRenderer.warmRoadShaders`
- `src/render/structures.ts#StructureDetails` (interface, line 39) ← call from `src/render/renderer.ts#createSceneRenderer.warmRoadShaders`
- `src/render/structures.ts#StructureDetails` (interface, line 39) ← read from `src/lab/catalog/layout.ts#surfacesOf`, `src/render/renderer.ts#createSceneRenderer.warmRoadShaders`
- `src/render/structures.ts#structureMeshes.dispose` (method, line 708) ← implementation from `src/render/renderer.ts#createSceneRenderer.warmRoadShaders`

### src/ui/roads/cutWalls.ts

- `src/ui/roads/cutWalls.ts#cutWallsChosen` (function, line 9) ← call from `src/main.ts#buildSummary`, `src/main.ts#roadTool`, `src/ui/v2/shell.ts#mountShell.renderRoads`
- `src/ui/roads/cutWalls.ts#setCutWalls` (function, line 13) ← call from `src/ui/v2/shell.ts#mountShell.renderRoads`

### src/world/elevation.ts

- `src/world/elevation.ts#buildRoadElevation` (function, line 279) ← call from `src/editor/commit.ts#solveOver`, `src/editor/editRules.ts#flatSolves.after`, `src/editor/editRules.ts#flatSolves.beforeSolve`, `src/editor/smallCityMap.ts#growLots`, …
- `src/world/elevation.ts#buildRoadElevation` (function, line 279) ← implementation from `src/editor/commit.ts#boreDeepCuts`, `src/editor/editRules.ts#grade`, `src/editor/editRules.ts#refuseRoadEdit.deck`, `src/editor/smallCityMap.ts#growLots.pavedAt`, …
- `src/world/elevation.ts#FLAT_GROUND` (function, line 951) ← read from `src/editor/editRules.ts#flatSolves.after`, `src/editor/editRules.ts#flatSolves.beforeSolve`, `src/editor/smallCityMap.ts#growLots`, `src/render/renderer.ts#createSceneRenderer.buildWorld`, …
- `src/world/elevation.ts#GROUND_ONLY` (variable, line 208) ← read from `src/editor/smallCityMap.ts#growLots.pavedAt`, `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#SURFACE_SETS`, `src/render/utilities.ts#poleGroundAt`
- `src/world/elevation.ts#MAX_AUTHORED_GRADE` (variable, line 68) ← read from `src/editor/commit.ts#commitRoadPath`, `src/main.ts#drawOverlayScreen`, `src/main.ts#setNodeHeightMetres`
- `src/world/elevation.ts#pendingOrder` (function, line 196) ← call from `src/render/renderer.ts#createSceneRenderer.completeSolve`
- `src/world/elevation.ts#ProfileIndex.near` (method, line 1052) ← implementation from `src/editor/buildingTool.ts#pickAlongRay.fresh`, `src/world/buildings/neighbours.ts#storeNeighbours.near`, `src/world/buildings/validate.ts#validateOn`, `src/world/growth/zoning.ts#nearSignatures`, …
- `src/world/elevation.ts#RoadElevation` (interface, line 135) ← call from `src/editor/smallCityMap.ts#growLots.pavedAt`, `src/render/groundShape.ts#blockDigest`, `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.compareSolves`, …
- `src/world/elevation.ts#RoadElevation` (interface, line 135) ← read from `src/render/groundShape.ts#blockDigest`, `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.buildWorld`, `src/render/renderer.ts#createSceneRenderer.compareSolves`, …
- `src/world/elevation.ts#RoadElevation` (interface, line 135) ← reference from `src/render/groundShape.ts#shapeGroundAsk`, `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.compareSolves`, `src/render/roadSurfaces.ts#roadSurfaceSteps`
- `src/world/elevation.ts#RoadElevation` (interface, line 135) ← typeRef from `src/editor/commit.ts#boreDeepCuts`, `src/editor/commit.ts#commitRoadPath`, `src/editor/commit.ts#DraftResult`, `src/editor/commit.ts#draftStations`, …
- `src/world/elevation.ts#RoadShapeCut` (interface, line 840) ← typeRef from `src/render/groundShape.ts#BlockDigestAsk`, `src/render/groundShape.ts#GroundShapeAsk`
- `src/world/elevation.ts#roadShaper` (function, line 888) ← call from `src/render/groundShape.ts#digestBlocks`, `src/render/groundShape.ts#shapeGroundAsk`
- `src/world/elevation.ts#roadShaper` (function, line 888) ← implementation from `src/render/grass.ts#grassClump`, `src/render/groundShape.ts#blockDigest`, `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.deckHeight`, …
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

### src/world/junction/levels.ts

- `src/world/junction/levels.ts#JunctionLevels` (class, line 185) ← reference from `src/world/networkBuild.ts#memoOf`, `src/world/networkBuild.ts#NetworkBuild.solveNode`, `src/world/ribbons.ts#junctionBounds`
- `src/world/junction/levels.ts#JunctionLevels` (class, line 185) ← typeRef from `src/world/junction/trimSummary.ts#trimSummary`, `src/world/network.ts#Network.junctionCells`, `src/world/network.ts#Network.junctions`, `src/world/networkBuild.ts#memoEntry`, …
- `src/world/junction/levels.ts#JunctionLevels.constructor` (constructor, line 186) ← read from `src/world/ribbons.ts#junctionBounds`
- `src/world/junction/levels.ts#JunctionLevels.data` (method, line 238) ← call from `src/world/networkBuild.ts#memoData`
- `src/world/junction/levels.ts#JunctionLevels.forEachTrim` (method, line 284) ← call from `src/world/junction/trimSummary.ts#trimSummary`, `src/world/networkBuild.ts#NetworkBuild.finalTrims`
- `src/world/junction/levels.ts#JunctionLevels.fromData` (method, line 232) ← call from `src/world/networkBuild.ts#memoOf`
- `src/world/junction/levels.ts#JunctionLevels.get` (method, line 256) ← call from `src/editor/growthFeed.ts#junctionPiece`, `src/render/roadSurfaceInputs.ts#SurfaceInputs.placeNode`, `src/world/carriageway.ts#onRoadGround`, `src/world/junction/bulb.ts#clearBulbsOf`, …
- `src/world/junction/levels.ts#JunctionLevels.get` (method, line 256) ← implementation from `src/lab/inspector/entity.ts#entityInfo`, `src/render/uploads.ts#drainCompiles.build`, `src/world/buildings/validate.ts#touchesRoad`, `src/world/conflictPoints.ts#ConflictIndex.add`, …
- `src/world/junction/levels.ts#JunctionLevels.of` (method, line 207) ← call from `src/world/networkBuild.ts#NetworkBuild.solveNode`
- `src/world/junction/levels.ts#JunctionLevels.ringAt` (method, line 277) ← call from `src/render/networkFarRoads.ts#farRoadInput`, `src/render/roadStandIn.ts#RoadStandIn.show`
- `src/world/junction/levels.ts#JunctionLevels.size` (getter, line 246) ← call from `src/world/networkBuild.ts#memoEntry`
- `src/world/junction/levels.ts#JunctionLevelsData` (interface, line 157) ← typeRef from `src/world/networkBuild.ts#BakedMemo`
- `src/world/junction/levels.ts#LeanJunction.rings` (getter, line 72) ← implementation from `src/sim/peds/sidewalk.ts#clearOfJunction`, `src/world/ribbons.ts#junctionBounds`, `src/world/surfaceParts.ts#surfaceParts`, `src/world/surfaces.ts#levelRingsOf`, …
- `src/world/junction/levels.ts#made` (function, line 77) ← indirectCall from `src/core/recentCache.ts#RecentCache.get`

### src/world/roadField.ts

- `src/world/roadField.ts#FieldData` (interface, line 657) ← read from `src/render/roadSurfaces.ts#roadSurfaceSteps`, `src/render/roadTileMesh.ts#readsOf`
- `src/world/roadField.ts#FieldData` (interface, line 657) ← typeRef from `src/render/roadTileMesh.ts#SurfaceData`, `src/world/paving.ts#PavingData`
- `src/world/roadField.ts#fieldOf` (function, line 746) ← call from `src/render/roadTileMesh.ts#readsOf`, `src/world/paving.ts#pavingReader`
- `src/world/roadField.ts#PointHeights` (class, line 371) ← construct from `src/render/roadTileMesh.ts#TileMesher.heights`, `src/render/roadTileMesh.ts#TileMesher.need`, `src/render/roadTileMesh.ts#TileMesher.run`, `src/world/paving.ts#pavingReader`
- `src/world/roadField.ts#PointHeights` (class, line 371) ← typeRef from `src/render/roadTileMesh.ts#readsOf`
- `src/world/roadField.ts#PointHeights.add` (method, line 390) ← call from `src/render/roadTileMesh.ts#TileMesher.give`, `src/render/roadTileMesh.ts#TileMesher.need.take`, `src/render/roadTileMesh.ts#TileMesher.run`, `src/world/paving.ts#pavingReader`
- `src/world/roadField.ts#PointHeights.get` (method, line 384) ← call from `src/render/roadTileMesh.ts#readsOf.terrainAt`, `src/world/paving.ts#pavingReader`
- `src/world/roadField.ts#PointHeights.get` (method, line 384) ← implementation from `src/lab/inspector/entity.ts#entityInfo`, `src/render/uploads.ts#drainCompiles.build`, `src/world/buildings/validate.ts#touchesRoad`, `src/world/conflictPoints.ts#ConflictIndex.add`, …
- `src/world/roadField.ts#PointMemo.roads` (field, line 307) ← implementation from `src/main.ts#frame`
- `src/world/roadField.ts#roadField` (function, line 417) ← implementation from `src/render/groundShape.ts#blockDigest`, `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.deckHeight`, `src/render/roadSurfaceReads.ts#passSurfaces`, …
- `src/world/roadField.ts#roadField.onSegment` (method, line 601) ← implementation from `src/editor/commit.ts#boreDeepCuts`, `src/editor/editRules.ts#grade`, `src/editor/editRules.ts#refuseRoadEdit.deck`, `src/render/renderer.ts#createSceneRenderer`, …
- `src/world/roadField.ts#roadField.remember` (function, line 465) ← indirectCall from `src/actionsWiring.ts#createActions.explodeAt`, `src/core/aabb.ts#fromFlat`, `src/core/bezier.ts#quadMinRadius`, `src/core/health.ts#HealthLog.restore`, …
- `src/world/roadField.ts#roadField.remembering` (method, line 591) ← implementation from `src/render/roadSurfaces.ts#roadSurfaceSteps.reading`, `src/render/roadTileMesh.ts#TileMesher.run`
- `src/world/roadField.ts#roadField.remembers` (function, line 449) ← indirectCall from `src/actionsWiring.ts#createActions.explodeAt`, `src/core/aabb.ts#fromFlat`, `src/core/bezier.ts#quadMinRadius`, `src/core/health.ts#HealthLog.restore`, …
- `src/world/roadField.ts#roadField.roadAt` (method, line 624) ← implementation from `src/render/grass.ts#grassClump`, `src/render/groundShape.ts#blockDigest`, `src/render/renderer.ts#createSceneRenderer`, `src/render/roadSurfaceReads.ts#passSurfaces.asphaltSection`, …
- `src/world/roadField.ts#roadField.roadAtInto` (method, line 606) ← implementation from `src/world/paving.ts#pavedHeightFrom`
- `src/world/roadField.ts#roadField.surfaceFrameAt` (method, line 642) ← implementation from `src/render/roadSurfaceReads.ts#passSurfaces.frameFor`
- `src/world/roadField.ts#roadField.surfaceFrameInto` (method, line 645) ← implementation from `src/render/roadSurfaceReads.ts#passSurfaces.frameFor`
- `src/world/roadField.ts#RoadReads` (interface, line 96) ← call from `src/editor/commit.ts#boreDeepCuts`, `src/editor/editRules.ts#grade`, `src/editor/editRules.ts#refuseRoadEdit.deck`, `src/render/grass.ts#grassClump`, …
- `src/world/roadField.ts#RoadReads` (interface, line 96) ← read from `src/render/roadSurfaceReads.ts#passSurfaces.frameFor`
- `src/world/roadField.ts#RoadReads` (interface, line 96) ← reference from `src/render/roadSurfaceReads.ts#passSurfaces.frameFor`, `src/render/roadSurfaces.ts#roadSurfaceSteps.reading`, `src/world/paving.ts#pavedHeightFrom`
- `src/world/roadField.ts#RoadReads` (interface, line 96) ← typeRef from `src/render/roadSurfacePass.ts#passRaised`, `src/render/roadSurfacePass.ts#passReads`, `src/render/roadSurfaceReads.ts#SurfaceReads`, `src/world/paving.ts#pavedHeightFrom`
- `src/world/roadField.ts#RoadSample` (interface, line 57) ← read from `src/render/grass.ts#grassClump`, `src/render/groundShape.ts#blockDigest`, `src/render/renderer.ts#createSceneRenderer`, `src/render/roadSurfaceReads.ts#passSurfaces.asphaltSection`, …
- `src/world/roadField.ts#roadSampleOut` (function, line 91) ← call from `src/world/paving.ts#SAMPLE`
- `src/world/roadField.ts#RoadSampleOut` (interface, line 78) ← read from `src/world/paving.ts#pavedHeightFrom`
- `src/world/roadField.ts#SpatialIndex.near` (method, line 250) ← implementation from `src/editor/buildingTool.ts#pickAlongRay.fresh`, `src/world/buildings/neighbours.ts#storeNeighbours.near`, `src/world/buildings/validate.ts#validateOn`, `src/world/growth/zoning.ts#nearSignatures`, …

### src/world/roads/buildMode.ts

- `src/world/roads/buildMode.ts#BuildMode` (type, line 10) ← typeRef from `src/main.ts#BUILD_FILL`, `src/main.ts#buildSummary`, `src/main.ts#drawBuildModes`, `src/main.ts#drawOverlayScreen`, …
- `src/world/roads/buildMode.ts#BuildRun` (interface, line 37) ← read from `src/main.ts#buildSummary`
- `src/world/roads/buildMode.ts#buildRuns` (function, line 110) ← call from `src/main.ts#buildSummary`
- `src/world/roads/buildMode.ts#buildRuns` (function, line 110) ← typeRef from `src/main.ts#buildSummary`
- `src/world/roads/buildMode.ts#orderAlong` (function, line 44) ← call from `src/editor/commit.ts#draftStations`
- `src/world/roads/buildMode.ts#RoadStation` (interface, line 24) ← read from `src/main.ts#drawOverlayScreen`
- `src/world/roads/buildMode.ts#RoadStation` (interface, line 24) ← typeRef from `src/editor/commit.ts#DraftResult`, `src/editor/commit.ts#draftStations`, `src/editor/roadTool.ts#RoadTool.judged`, `src/editor/roadTool.ts#RoadTool.stations`
- `src/world/roads/buildMode.ts#stationsAlong` (function, line 82) ← call from `src/editor/commit.ts#draftStations`

### src/world/roads/tuning.ts

- `src/world/roads/tuning.ts#ROAD_TUNING` (variable, line 4) ← read from `src/editor/commit.ts#AUTO_TUNNEL_COVER`, `src/editor/commit.ts#CROSSING_CLEARANCE`, `src/editor/editRules.ts#MAX_BUILT_GRADE`, `src/editor/editRules.ts#PASS_CLEARANCE`, …

### src/world/structures.ts

- `src/world/structures.ts#isRaised` (function, line 68) ← call from `src/render/roadSurfacePass.ts#passRaised`
- `src/world/structures.ts#migrateStructure` (function, line 11) ← call from `src/editor/persistence.ts#isSerializedDoc`, `src/world/doc.ts#segmentRecord`
- `src/world/structures.ts#RAISED_LIFT` (variable, line 65) ← read from `src/render/roadSurfacePass.ts#passRaised`
- `src/world/structures.ts#ROAD_GROUND_CLEARANCE` (variable, line 53) ← read from `src/render/networkFarRoads.ts#farRoadInput.onGround`
- `src/world/structures.ts#ROAD_STRUCTURES` (variable, line 44) ← read from `src/render/roadSurfaceInputs.ts#SurfaceInputs.passList`
- `src/world/structures.ts#roadStructure` (function, line 55) ← call from `src/editor/commit.ts#segmentOffsetAt`, `src/editor/editRules.ts#deckAt`, `src/editor/repair.ts#nearestCandidate`, `src/editor/snap.ts#anchorHeightOffset`, …
- `src/world/structures.ts#RoadStructure` (type, line 5) ← typeRef from `src/editor/commit.ts#alreadyJoined`, `src/editor/commit.ts#commitDraftInPlace`, `src/editor/growthReplica.ts#RoadPiece`, `src/editor/growthReplica.ts#Structured`, …
- `src/world/structures.ts#RoadStructureSpec` (interface, line 17) ← read from `src/editor/commit.ts#segmentOffsetAt`, `src/editor/editRules.ts#deckAt`, `src/editor/repair.ts#nearestCandidate`, `src/editor/snap.ts#anchorHeightOffset`, …
- `src/world/structures.ts#TUNNEL_HEADROOM` (variable, line 30) ← read from `src/world/doc.ts#nodeMovesTraffic`, `src/world/doc.ts#RoadDoc.setNodeHeightOffset`, `src/world/junction/build.ts#surfaceMode`
- `src/world/structures.ts#TUNNELS_DRAWN` (variable, line 75) ← read from `src/world/lanelets.ts#LaneletGraph.buildSegmentLinks`

## Where the platform calls the feature

Code of the feature a package or the platform calls (no declaration of the program does): an override of a member its bases declare, or a function stored in or given to one of its declarations.

- `src/world/junction/levels.ts#JunctionLevels.entries` (method, line 310) — override of `es:ReadonlyMap.entries` (src/world/junction/levels.ts:310)
- `src/world/junction/levels.ts#JunctionLevels.forEach` (method, line 318) — override of `es:ReadonlyMap.forEach` (src/world/junction/levels.ts:318)
- `src/world/junction/levels.ts#JunctionLevels.get` (method, line 256) — override of `es:ReadonlyMap.get` (src/world/junction/levels.ts:256)
- `src/world/junction/levels.ts#JunctionLevels.get` (method, line 256) — callback of `program:src/core/recentCache.ts#RecentCache.get` (src/world/junction/levels.ts:259)
- `src/world/junction/levels.ts#JunctionLevels.has` (method, line 252) — override of `es:ReadonlyMap.has` (src/world/junction/levels.ts:252)
- `src/world/junction/levels.ts#JunctionLevels.keys` (method, line 302) — override of `es:ReadonlyMap.keys` (src/world/junction/levels.ts:302)
- `src/world/junction/levels.ts#JunctionLevels.size` (getter, line 246) — override of `es:ReadonlyMap.size` (src/world/junction/levels.ts:246)
- `src/world/junction/levels.ts#JunctionLevels.values` (method, line 306) — override of `es:ReadonlyMap.values` (src/world/junction/levels.ts:306)
- `src/world/junction/levels.ts#made` (function, line 77) — callback of `program:src/core/recentCache.ts#RecentCache.get` (src/world/junction/levels.ts:73)
- `src/world/junction/levels.ts#made` (function, line 77) — callback of `program:src/core/recentCache.ts#RecentCache.get` (src/world/junction/levels.ts:69)

## External APIs reached through a cast

A package's object seen through a type of the program: each member of that type is the package's.


## Shared declarations the feature uses

### src/core/aabb.ts

- `src/core/aabb.ts#Aabb` (interface, line 3) — read by 9: `src/world/elevation.ts#buildRoadElevation.take`, `src/world/elevation.ts#ProfileIndex.cells`, `src/render/structures.ts#structureParts`, `src/render/structureCells.ts#cellOfRibbon`, …
- `src/core/aabb.ts#Aabb` (interface, line 3) — typeRef by 8: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#Profile`, `src/world/elevation.ts#ProfileIndex.cells`, `src/world/elevation.ts#RoadElevation`, …

### src/core/digest.ts

- `src/core/digest.ts#Digest` (class, line 6) — construct by 6: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#buildRoadElevation.keyOf`, `src/world/elevation.ts#buildRoadElevation.localDigest`, `src/world/elevation.ts#lookDigest`, …
- `src/core/digest.ts#Digest.add` (method, line 11) — call by 5: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#buildRoadElevation.localDigest`, `src/world/elevation.ts#lookDigest`, `src/world/elevation.ts#ProfileIndex.digest`, …
- `src/core/digest.ts#Digest.addAll` (method, line 19) — call by 2: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#buildRoadElevation.keyOf`
- `src/core/digest.ts#Digest.addPlain` (method, line 31) — call by 2: `src/world/elevation.ts#buildRoadElevation.keyOf`, `src/world/elevation.ts#lookDigest`
- `src/core/digest.ts#Digest.addText` (method, line 25) — call by 2: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#buildRoadElevation.localDigest`
- `src/core/digest.ts#Digest.value` (method, line 59) — call by 6: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#buildRoadElevation.keyOf`, `src/world/elevation.ts#buildRoadElevation.localDigest`, `src/world/elevation.ts#lookDigest`, …

### src/core/forkMap.ts

- `src/core/forkMap.ts#ForkMap.get` (method, line 20) — implementation by 7: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#solveGround`, `src/world/elevation.ts#solveRaised`, `src/world/elevation.ts#solveSunken`, …

### src/core/gameState.ts

- `src/core/gameState.ts#GameState.get` (method, line 48) — implementation by 7: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#solveGround`, `src/world/elevation.ts#solveRaised`, `src/world/elevation.ts#solveSunken`, …

### src/core/hitch.ts

- `src/core/hitch.ts#measureHitch` (function, line 2) — call by 1: `src/world/elevation.ts#buildRoadElevation`

### src/core/hypot.ts

- `src/core/hypot.ts#hypot2` (function, line 7) — call by 1: `src/world/roadField.ts#boxDistance`

### src/core/idSlots.ts

- `src/core/idSlots.ts#IdSlots.get` (method, line 48) — implementation by 7: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#solveGround`, `src/world/elevation.ts#solveRaised`, `src/world/elevation.ts#solveSunken`, …

### src/core/polyline.ts

- `src/core/polyline.ts#Frame` (interface, line 7) — read by 8: `src/world/elevation.ts#readStations`, `src/world/elevation.ts#stationsAt`, `src/render/structures.ts#boxAlong`, `src/render/structures.ts#parapetRun`, …
- `src/core/polyline.ts#Frame` (interface, line 7) — typeRef by 2: `src/render/structures.ts#boxAlong`, `src/render/structures.ts#structureParts`
- `src/core/polyline.ts#FrameOut` (interface, line 23) — read by 3: `src/world/roadField.ts#roadField.frameInto`, `src/world/roadField.ts#roadField.roadAt`, `src/world/roadField.ts#roadField.roadAtInto`
- `src/core/polyline.ts#newFrame` (function, line 33) — call by 1: `src/world/roadField.ts#roadField`
- `src/core/polyline.ts#Polyline` (class, line 36) — reference by 2: `src/world/elevation.ts#roadStations`, `src/world/roadField.ts#unpackField`
- `src/core/polyline.ts#Polyline` (class, line 36) — typeRef by 6: `src/world/elevation.ts#Profile`, `src/world/elevation.ts#readStations`, `src/world/elevation.ts#sameLine`, `src/world/elevation.ts#ShapingRoad`, …
- `src/core/polyline.ts#Polyline.bbox` (getter, line 116) — call by 5: `src/world/elevation.ts#buildRoadElevation`, `src/render/structures.ts#structureParts`, `src/render/structureCells.ts#cellOfRibbon`, `src/world/roadField.ts#roadField.nearest`, …
- `src/core/polyline.ts#Polyline.bbox` (getter, line 116) — implementation by 1: `src/world/roadField.ts#SpatialIndex.constructor`
- `src/core/polyline.ts#Polyline.closestInto` (method, line 347) — call by 7: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#buildRoadElevation.localDigest`, `src/world/elevation.ts#roadShapeAt`, `src/world/roadField.ts#roadField.frameInto`, …
- `src/core/polyline.ts#Polyline.distanceTo` (method, line 359) — call by 1: `src/render/structures.ts#onAnotherCasing`
- `src/core/polyline.ts#Polyline.fromFlat` (method, line 65) — call by 2: `src/world/elevation.ts#roadStations`, `src/world/roadField.ts#unpackField`
- `src/core/polyline.ts#Polyline.length` (getter, line 123) — call by 8: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#roadStations`, `src/render/structures.ts#parapetRun`, `src/render/structures.ts#retainingWalls`, …
- `src/core/polyline.ts#Polyline.nearestSegment` (method, line 275) — indirectCall by 9: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#buildRoadElevation.wantedOf`, `src/world/elevation.ts#pendingOrder`, `src/world/elevation.ts#readStations`, …
- `src/core/polyline.ts#Polyline.sampleAt` (method, line 152) — call by 7: `src/world/elevation.ts#readStations`, `src/world/elevation.ts#stationsAt`, `src/render/structures.ts#parapetRun`, `src/render/structures.ts#retainingWalls`, …
- `src/core/polyline.ts#Polyline.sampleAtInto` (method, line 157) — call by 3: `src/world/roadField.ts#roadField.frameInto`, `src/world/roadField.ts#roadField.roadAt`, `src/world/roadField.ts#roadField.roadAtInto`
- `src/core/polyline.ts#Polyline.xy` (field, line 38) — read by 5: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#buildRoadElevation.localDigest`, `src/world/elevation.ts#roadsToRead`, `src/world/elevation.ts#sameLine`, …

### src/core/recentCache.ts

- `src/core/recentCache.ts#RecentCache` (class, line 6) — construct by 4: `src/world/junction/levels.ts#madeCasing`, `src/world/junction/levels.ts#madeCurb`, `src/world/junction/levels.ts#madeRings`, `src/world/junction/levels.ts#madeSidewalk`
- `src/core/recentCache.ts#RecentCache` (class, line 6) — typeRef by 1: `src/world/junction/levels.ts#madeAt`
- `src/core/recentCache.ts#RecentCache.get` (method, line 12) — call by 3: `src/world/junction/levels.ts#JunctionLevels.get`, `src/world/junction/levels.ts#LeanJunction.rings`, `src/world/junction/levels.ts#LeanJunction.tongues`
- `src/core/recentCache.ts#RecentCache.get` (method, line 12) — implementation by 7: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#solveGround`, `src/world/elevation.ts#solveRaised`, `src/world/elevation.ts#solveSunken`, …

### src/core/rerootMap.ts

- `src/core/rerootMap.ts#GONE` (variable, line 5) — read by 3: `src/core/rerootMap.ts#RerootMap.note`, `src/core/rerootMap.ts#RerootMap.reroot`, `src/core/rerootMap.ts#RerootMap.entries`
- `src/core/rerootMap.ts#RerootMap` (class, line 9) — construct by 3: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#ProfileIndex.empty`, `src/core/rerootMap.ts#RerootMap.derive`
- `src/core/rerootMap.ts#RerootMap` (class, line 9) — typeRef by 9: `src/world/elevation.ts#ProfileIndex.constructor`, `src/world/elevation.ts#SolveMemo`, `src/core/rerootMap.ts#RerootMap.prev`, `src/core/rerootMap.ts#RerootMap.delete`, …
- `src/core/rerootMap.ts#RerootMap.data` (field, line 11) — read by 6: `src/core/rerootMap.ts#RerootMap.delete`, `src/core/rerootMap.ts#RerootMap.set`, `src/core/rerootMap.ts#RerootMap.note`, `src/core/rerootMap.ts#RerootMap.derive`, …
- `src/core/rerootMap.ts#RerootMap.data` (field, line 11) — write by 5: `src/core/rerootMap.ts#RerootMap.delete`, `src/core/rerootMap.ts#RerootMap.set`, `src/core/rerootMap.ts#RerootMap.note`, `src/core/rerootMap.ts#RerootMap.derive`, …
- `src/core/rerootMap.ts#RerootMap.get` (method, line 19) — call by 10: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#buildRoadElevation.incident`, `src/world/elevation.ts#buildRoadElevation.profileOf`, `src/world/elevation.ts#buildRoadElevation.settledAt`, …
- `src/core/rerootMap.ts#RerootMap.get` (method, line 19) — implementation by 7: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#solveGround`, `src/world/elevation.ts#solveRaised`, `src/world/elevation.ts#solveSunken`, …
- `src/core/rerootMap.ts#RerootMap.up` (field, line 13) — read by 3: `src/core/rerootMap.ts#RerootMap.writable`, `src/core/rerootMap.ts#RerootMap.reroot`, `src/core/rerootMap.ts#RerootMap.entries`
- `src/core/rerootMap.ts#RerootMap.up` (field, line 13) — write by 2: `src/core/rerootMap.ts#RerootMap.derive`, `src/core/rerootMap.ts#RerootMap.reroot`
- `src/core/rerootMap.ts#Slot` (type, line 6) — typeRef by 1: `src/core/rerootMap.ts#RerootMap.reroot`

### src/core/ring.ts

- `src/core/ring.ts#Ring` (class, line 27) — reference by 3: `src/world/junction/levels.ts#JunctionLevels.fromData`, `src/world/junction/levels.ts#junctionOf`, `src/core/ring.ts#Ring.fromRun`
- `src/core/ring.ts#Ring` (class, line 27) — typeRef by 13: `src/world/junction/levels.ts#EMPTY_RINGS`, `src/world/junction/levels.ts#fill`, `src/world/junction/levels.ts#JunctionLevels.constructor`, `src/world/junction/levels.ts#JunctionLevels.of`, …
- `src/core/ring.ts#Ring.bbox` (getter, line 123) — call by 1: `src/world/junction/levels.ts#boundsOf`
- `src/core/ring.ts#Ring.bbox` (getter, line 123) — implementation by 1: `src/world/roadField.ts#SpatialIndex.constructor`
- `src/core/ring.ts#Ring.count` (field, line 34) — read by 1: `src/core/ring.ts#Ring.run`
- `src/core/ring.ts#Ring.data` (field, line 33) — read by 1: `src/core/ring.ts#Ring.run`
- `src/core/ring.ts#Ring.isEmpty` (getter, line 84) — call by 1: `src/world/junction/levels.ts#boundsOf`

### src/core/scalar.ts

- `src/core/scalar.ts#COARSE_EPS` (variable, line 12) — read by 1: `src/world/junction/levels.ts#filled`

### src/core/units.ts

- `src/core/units.ts#m` (function, line 6) — call by 61: `src/world/elevation.ts#BESIDE_DROP`, `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#CUT_SHOULDER`, `src/world/elevation.ts#ORDER_CELL`, …

### src/core/vec2.ts

- `src/core/vec2.ts#angleOf` (function, line 35) — call by 2: `src/render/structures.ts#boxAlong`, `src/render/structures.ts#structureParts`
- `src/core/vec2.ts#Vec2` (interface, line 4) — read by 11: `src/world/elevation.ts#readStations`, `src/world/elevation.ts#stationsAt`, `src/render/structures.ts#boxAlong`, `src/render/structures.ts#parapetRun`, …
- `src/core/vec2.ts#Vec2` (interface, line 4) — typeRef by 1: `src/world/roads/buildMode.ts#orderAlong`

### src/editor/commit.ts

- `src/editor/commit.ts#draftStations` (function, line 254) — indirectCall by 1: `src/world/roads/buildMode.ts#stationsAlong`
- `src/editor/commit.ts#solveOver` (function, line 204) — indirectCall by 1: `src/world/elevation.ts#buildRoadElevation`

### src/editor/growthReplica.ts

- `src/editor/growthReplica.ts#NeighbourReplica.near` (method, line 155) — implementation by 2: `src/world/roadField.ts#roadField.nearest`, `src/world/roadField.ts#roadField.query`

### src/lab/catalog/layers.ts

- `src/lab/catalog/layers.ts#farNetworkItems.flat` (function, line 200) — indirectCall by 1: `src/world/elevation.ts#buildRoadElevation`
- `src/lab/catalog/layers.ts#signItems.flat` (function, line 77) — indirectCall by 1: `src/world/elevation.ts#buildRoadElevation`

### src/lab/catalog/layout.ts

- `src/lab/catalog/layout.ts#layoutItems.flat` (function, line 166) — indirectCall by 1: `src/world/elevation.ts#buildRoadElevation`
- `src/lab/catalog/layout.ts#surfacesOf.flat` (function, line 112) — indirectCall by 1: `src/world/elevation.ts#buildRoadElevation`

### src/lab/resources/meter.ts

- `src/lab/resources/meter.ts#MeterBook.get` (method, line 166) — implementation by 7: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#solveGround`, `src/world/elevation.ts#solveRaised`, `src/world/elevation.ts#solveSunken`, …

### src/render/buildings/cellPool.ts

- `src/render/buildings/cellPool.ts#FloorColumns.get` (method, line 132) — implementation by 7: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#solveGround`, `src/world/elevation.ts#solveRaised`, `src/world/elevation.ts#solveSunken`, …

### src/render/buildings/layer.ts

- `src/render/buildings/layer.ts#createBuildingLayer.levelOfDetail` (method, line 981) — indirectCall by 9: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#buildRoadElevation.wantedOf`, `src/world/elevation.ts#pendingOrder`, `src/world/elevation.ts#readStations`, …

### src/render/fogLayer.ts

- `src/render/fogLayer.ts#createFogField.raster` (function, line 97) — indirectCall by 9: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#buildRoadElevation.wantedOf`, `src/world/elevation.ts#pendingOrder`, `src/world/elevation.ts#readStations`, …
- `src/render/fogLayer.ts#createFogField.readBack` (function, line 67) — indirectCall by 9: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#buildRoadElevation.wantedOf`, `src/world/elevation.ts#pendingOrder`, `src/world/elevation.ts#readStations`, …

### src/render/materials.ts

- `src/render/materials.ts#SceneMaterials` (interface, line 11) — read by 2: `src/render/structures.ts#buildStructureDetails`, `src/render/structures.ts#structureMeshes`
- `src/render/materials.ts#SceneMaterials` (interface, line 11) — typeRef by 3: `src/render/structures.ts#buildStructureDetails`, `src/render/structures.ts#structureMeshes`, `src/render/structureCells.ts#createStructureCells`

### src/render/mesh/cylinder.ts

- `src/render/mesh/cylinder.ts#cylinderWith` (function, line 16) — call by 1: `src/render/structures.ts#pierShape`

### src/render/occlusion.ts

- `src/render/occlusion.ts#OcclusionBuffer.near` (field, line 46) — implementation by 2: `src/world/roadField.ts#roadField.nearest`, `src/world/roadField.ts#roadField.query`

### src/render/poleLayer.ts

- `src/render/poleLayer.ts#Block` (interface, line 506) — read by 1: `src/render/poleLayer.ts#PoleLayer.triangles`
- `src/render/poleLayer.ts#PartStore.count` (field, line 156) — read by 1: `src/render/poleLayer.ts#PoleLayer.triangles`
- `src/render/poleLayer.ts#PartStore.perInstance` (field, line 169) — read by 1: `src/render/poleLayer.ts#PoleLayer.triangles`
- `src/render/poleLayer.ts#PoleLayer.blocks` (field, line 516) — read by 1: `src/render/poleLayer.ts#PoleLayer.triangles`
- `src/render/poleLayer.ts#PoleLayer.dispose` (method, line 697) — implementation by 3: `src/render/structureCells.ts#createStructureCells.dispose`, `src/render/structureCells.ts#createStructureCells.prepare`, `src/render/structureCells.ts#createStructureCells.prepare.apply`

### src/render/renderer.ts

- `src/render/renderer.ts#createSceneRenderer.buildWorld` (function, line 2317) — indirectCall by 1: `src/world/elevation.ts#buildRoadElevation`
- `src/render/renderer.ts#createSceneRenderer.completeSolve` (function, line 2225) — indirectCall by 1: `src/world/elevation.ts#buildRoadElevation`
- `src/render/renderer.ts#createSceneRenderer.warmRoadShaders` (function, line 2506) — indirectCall by 1: `src/world/elevation.ts#buildRoadElevation`

### src/render/roadSurfaceReads.ts

- `src/render/roadSurfaceReads.ts#TERRAIN_UV` (variable, line 24) — read by 1: `src/render/structures.ts#structureParts`

### src/sim/agents/kerbBays.ts

- `src/sim/agents/kerbBays.ts#EntranceBook.box` (method, line 127) — indirectCall by 9: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#buildRoadElevation.wantedOf`, `src/world/elevation.ts#pendingOrder`, `src/world/elevation.ts#readStations`, …

### src/sim/agents/laneGrid.ts

- `src/sim/agents/laneGrid.ts#LaneGrid.near` (method, line 127) — implementation by 2: `src/world/roadField.ts#roadField.nearest`, `src/world/roadField.ts#roadField.query`

### src/sim/agents/trafficBuildings.ts

- `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.get` (method, line 306) — implementation by 7: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#solveGround`, `src/world/elevation.ts#solveRaised`, `src/world/elevation.ts#solveSunken`, …
- `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.near` (method, line 414) — implementation by 2: `src/world/roadField.ts#roadField.nearest`, `src/world/roadField.ts#roadField.query`

### src/sim/ambient/lotPlaces.ts

- `src/sim/ambient/lotPlaces.ts#Fenwick.get` (method, line 16) — implementation by 7: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#solveGround`, `src/world/elevation.ts#solveRaised`, `src/world/elevation.ts#solveSunken`, …

### src/sim/host/fleetGrid.ts

- `src/sim/host/fleetGrid.ts#FleetGrid.near` (method, line 85) — implementation by 2: `src/world/roadField.ts#roadField.nearest`, `src/world/roadField.ts#roadField.query`

### src/sim/meso/slots.ts

- `src/sim/meso/slots.ts#SlotIndex.get` (method, line 60) — implementation by 7: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#solveGround`, `src/world/elevation.ts#solveRaised`, `src/world/elevation.ts#solveSunken`, …

### src/sim/peds/signalPostIndex.ts

- `src/sim/peds/signalPostIndex.ts#SignalPostIndex.near` (method, line 93) — implementation by 2: `src/world/roadField.ts#roadField.nearest`, `src/world/roadField.ts#roadField.query`

### src/world/buildings/grid.ts

- `src/world/buildings/grid.ts#BuildingGrid.near` (method, line 132) — implementation by 2: `src/world/roadField.ts#roadField.nearest`, `src/world/roadField.ts#roadField.query`

### src/world/buildings/items.ts

- `src/world/buildings/items.ts#BuildingItems.get` (method, line 73) — implementation by 7: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#solveGround`, `src/world/elevation.ts#solveRaised`, `src/world/elevation.ts#solveSunken`, …

### src/world/buildings/neighbours.ts

- `src/world/buildings/neighbours.ts#RecordNeighbour.x` (getter, line 75) — implementation by 14: `src/world/elevation.ts#readStations`, `src/world/elevation.ts#stationsAt`, `src/render/structures.ts#boxAlong`, `src/render/structures.ts#parapetRun`, …
- `src/world/buildings/neighbours.ts#RecordNeighbour.y` (getter, line 76) — implementation by 14: `src/world/elevation.ts#readStations`, `src/world/elevation.ts#stationsAt`, `src/render/structures.ts#boxAlong`, `src/render/structures.ts#parapetRun`, …

### src/world/buildings/spilledItems.ts

- `src/world/buildings/spilledItems.ts#SpilledItems.get` (method, line 99) — implementation by 7: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#solveGround`, `src/world/elevation.ts#solveRaised`, `src/world/elevation.ts#solveSunken`, …

### src/world/buildings/store.ts

- `src/world/buildings/store.ts#BuildingStore.get` (method, line 115) — implementation by 7: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#solveGround`, `src/world/elevation.ts#solveRaised`, `src/world/elevation.ts#solveSunken`, …
- `src/world/buildings/store.ts#BuildingStore.near` (method, line 133) — implementation by 2: `src/world/roadField.ts#roadField.nearest`, `src/world/roadField.ts#roadField.query`

### src/world/buildings/tableRows.ts

- `src/world/buildings/tableRows.ts#TableRows.get` (method, line 87) — implementation by 7: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#solveGround`, `src/world/elevation.ts#solveRaised`, `src/world/elevation.ts#solveSunken`, …

### src/world/cells.ts

- `src/world/cells.ts#CellFiling` (class, line 60) — construct by 1: `src/render/structureCells.ts#createStructureCells`
- `src/world/cells.ts#CellFiling.file` (method, line 76) — call by 1: `src/render/structureCells.ts#createStructureCells.prepare.apply`
- `src/world/cells.ts#CellFiling.idsMeeting` (method, line 131) — call by 5: `src/world/elevation.ts#scopeIds`, `src/render/structures.ts#onAnotherCasing`, `src/render/structures.ts#structureRibbons.near`, `src/render/structureCells.ts#createStructureCells.prepare`, …
- `src/world/cells.ts#CellFiling.idsMeetingAny` (method, line 169) — call by 2: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#roadsToRead`
- `src/world/cells.ts#CellFiling.rectOf` (method, line 120) — call by 1: `src/world/elevation.ts#pendingOrder`
- `src/world/cells.ts#CellFiling.remove` (method, line 107) — call by 1: `src/render/structureCells.ts#createStructureCells.prepare.apply`

### src/world/changes.ts

- `src/world/changes.ts#ChangeJournal.get` (method, line 377) — implementation by 7: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#solveGround`, `src/world/elevation.ts#solveRaised`, `src/world/elevation.ts#solveSunken`, …
- `src/world/changes.ts#ChangeRect` (type, line 4) — typeRef by 9: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#roadsToRead`, `src/world/elevation.ts#SolveScope`, `src/render/structureCells.ts#Cell`, …

### src/world/cityGen/streets.ts

- `src/world/cityGen/streets.ts#SampleGrid.near` (method, line 48) — implementation by 2: `src/world/roadField.ts#roadField.nearest`, `src/world/roadField.ts#roadField.query`
- `src/world/cityGen/streets.ts#StretchGrid.near` (method, line 214) — implementation by 2: `src/world/roadField.ts#roadField.nearest`, `src/world/roadField.ts#roadField.query`

### src/world/derived.ts

- `src/world/derived.ts#Derived.get` (method, line 80) — implementation by 7: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#solveGround`, `src/world/elevation.ts#solveRaised`, `src/world/elevation.ts#solveSunken`, …

### src/world/doc.ts

- `src/world/doc.ts#ItemGrid.near` (method, line 3504) — implementation by 2: `src/world/roadField.ts#roadField.nearest`, `src/world/roadField.ts#roadField.query`
- `src/world/doc.ts#RoadDoc` (class, line 153) — typeRef by 1: `src/world/roads/buildMode.ts#orderAlong`
- `src/world/doc.ts#RoadDoc.node` (method, line 416) — call by 6: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#buildRoadElevation.incident`, `src/world/elevation.ts#buildRoadElevation.keyOf`, `src/world/elevation.ts#roadsToRead`, …
- `src/world/doc.ts#RoadDoc.requireSegment` (method, line 430) — call by 2: `src/render/structures.ts#retainingWalls`, `src/render/structures.ts#structureParts`
- `src/world/doc.ts#RoadDoc.segment` (method, line 420) — call by 6: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#roadsToRead`, `src/render/structures.ts#onAnotherCasing`, `src/render/structures.ts#structureRibbons`, …
- `src/world/doc.ts#RoadNode` (interface, line 89) — read by 6: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#buildRoadElevation.incident`, `src/world/elevation.ts#buildRoadElevation.keyOf`, `src/world/elevation.ts#roadsToRead`, …
- `src/world/doc.ts#RoadSegment` (interface, line 124) — read by 7: `src/world/elevation.ts#buildRoadElevation`, `src/render/structures.ts#onAnotherCasing`, `src/render/structures.ts#retainingWalls`, `src/render/structures.ts#structureParts`, …
- `src/world/doc.ts#RoadSegment` (interface, line 124) — typeRef by 1: `src/render/structures.ts#structureParts`

### src/world/geometry.ts

- `src/world/geometry.ts#PolylineCache.get` (method, line 70) — call by 1: `src/render/structures.ts#onAnotherCasing`
- `src/world/geometry.ts#PolylineCache.get` (method, line 70) — implementation by 7: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#solveGround`, `src/world/elevation.ts#solveRaised`, `src/world/elevation.ts#solveSunken`, …

### src/world/ids.ts

- `src/world/ids.ts#NodeId` (type, line 7) — typeRef by 15: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#buildRoadElevation.capOf`, `src/world/elevation.ts#buildRoadElevation.gradeOf`, `src/world/elevation.ts#buildRoadElevation.incident`, …
- `src/world/ids.ts#SegmentId` (type, line 8) — typeRef by 25: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#buildRoadElevation.incident`, `src/world/elevation.ts#buildRoadElevation.mine`, `src/world/elevation.ts#buildRoadElevation.profileOf`, …

### src/world/junction/corners.ts

- `src/world/junction/corners.ts#packCorners` (function, line 45) — implementation by 1: `src/world/junction/levels.ts#junctionData`
- `src/world/junction/corners.ts#unpackCorners` (function, line 56) — implementation by 1: `src/world/junction/levels.ts#junctionOf`

### src/world/junction/derive.ts

- `src/world/junction/derive.ts#<ImportDeclaration>` (statement, line 14) — implementation by 2: `src/world/junction/levels.ts#fill`, `src/world/junction/levels.ts#filled`
- `src/world/junction/derive.ts#<ImportDeclaration>@11` (statement, line 11) — implementation by 3: `src/world/junction/levels.ts#junctionData`, `src/world/junction/levels.ts#junctionOf`, `src/world/junction/levels.ts#legAt`
- `src/world/junction/derive.ts#<ImportDeclaration>@12` (statement, line 12) — implementation by 2: `src/world/junction/levels.ts#junctionData`, `src/world/junction/levels.ts#junctionOf`
- `src/world/junction/derive.ts#baseTrims` (function, line 47) — implementation by 1: `src/world/junction/levels.ts#JunctionLevels.forEachTrim`
- `src/world/junction/derive.ts#deriveFromLegs` (function, line 57) — implementation by 1: `src/world/junction/levels.ts#JunctionLevels.make`

### src/world/junction/legs.ts

- `src/world/junction/legs.ts#legOf` (function, line 48) — implementation by 1: `src/world/junction/levels.ts#legAt`
- `src/world/junction/legs.ts#PackedLeg.ang` (getter, line 38) — implementation by 2: `src/world/junction/levels.ts#framesOf`, `src/world/junction/levels.ts#sameLeg`
- `src/world/junction/legs.ts#PackedLeg.dir` (getter, line 36) — implementation by 2: `src/world/junction/levels.ts#framesOf`, `src/world/junction/levels.ts#sameLeg`
- `src/world/junction/legs.ts#PackedLeg.hw` (getter, line 39) — implementation by 2: `src/world/elevation.ts#buildRoadElevation`, `src/world/junction/levels.ts#sameLeg`
- `src/world/junction/legs.ts#PackedLeg.hwLeft` (getter, line 40) — implementation by 1: `src/world/junction/levels.ts#sameLeg`
- `src/world/junction/legs.ts#PackedLeg.hwRight` (getter, line 41) — implementation by 1: `src/world/junction/levels.ts#sameLeg`
- `src/world/junction/legs.ts#PackedLeg.hwSidewalk` (getter, line 42) — implementation by 2: `src/world/junction/levels.ts#legAt`, `src/world/junction/levels.ts#sameLeg`
- `src/world/junction/legs.ts#PackedLeg.length` (getter, line 43) — implementation by 2: `src/world/junction/levels.ts#framesOf`, `src/world/junction/levels.ts#sameLeg`
- `src/world/junction/legs.ts#PackedLeg.nrm` (getter, line 37) — implementation by 2: `src/world/junction/levels.ts#framesOf`, `src/world/junction/levels.ts#sameLeg`
- `src/world/junction/legs.ts#PackedLeg.origin` (getter, line 35) — implementation by 2: `src/world/junction/levels.ts#framesOf`, `src/world/junction/levels.ts#sameLeg`
- `src/world/junction/legs.ts#PackedLeg.shift` (getter, line 44) — implementation by 2: `src/world/junction/levels.ts#legAt`, `src/world/junction/levels.ts#sameLeg`
- `src/world/junction/legs.ts#packLegs` (function, line 65) — implementation by 1: `src/world/junction/levels.ts#junctionData`
- `src/world/junction/legs.ts#unpackLegs` (function, line 77) — implementation by 1: `src/world/junction/levels.ts#junctionOf`

### src/world/junction/polygon.ts

- `src/world/junction/polygon.ts#legTongue` (function, line 63) — implementation by 1: `src/world/junction/levels.ts#fill`
- `src/world/junction/polygon.ts#nodeCap` (function, line 82) — implementation by 2: `src/world/junction/levels.ts#fill`, `src/world/junction/levels.ts#filled`
- `src/world/junction/polygon.ts#nodeCentre` (function, line 26) — implementation by 2: `src/world/junction/levels.ts#fill`, `src/world/junction/levels.ts#filled`

### src/world/junction/types.ts

- `src/world/junction/types.ts#Corner` (interface, line 43) — typeRef by 1: `src/world/junction/levels.ts#LeanJunction.constructor`
- `src/world/junction/types.ts#Junction` (interface, line 60) — implements by 2: `src/world/junction/levels.ts#JunctionLevels`, `src/world/junction/levels.ts#LeanJunction`
- `src/world/junction/types.ts#Junction` (interface, line 60) — read by 11: `src/world/junction/levels.ts#boundsOf`, `src/world/junction/levels.ts#filled`, `src/world/junction/levels.ts#framesOf`, `src/world/junction/levels.ts#junctionData`, …
- `src/world/junction/types.ts#Junction` (interface, line 60) — typeRef by 19: `src/world/junction/levels.ts#boundsOf`, `src/world/junction/levels.ts#filled`, `src/world/junction/levels.ts#framesOf`, `src/world/junction/levels.ts#junctionData`, …
- `src/world/junction/types.ts#JunctionKit` (interface, line 79) — call by 7: `src/world/junction/levels.ts#fill`, `src/world/junction/levels.ts#filled`, `src/world/junction/levels.ts#junctionData`, `src/world/junction/levels.ts#JunctionLevels.forEachTrim`, …
- `src/world/junction/types.ts#JunctionKit` (interface, line 79) — typeRef by 11: `src/world/junction/levels.ts#fill`, `src/world/junction/levels.ts#filled`, `src/world/junction/levels.ts#junctionData`, `src/world/junction/levels.ts#JunctionLevels.constructor`, …
- `src/world/junction/types.ts#Leg` (interface, line 14) — read by 5: `src/world/elevation.ts#buildRoadElevation`, `src/world/junction/levels.ts#framesOf`, `src/world/junction/levels.ts#JunctionLevels.forEachTrim`, `src/world/junction/levels.ts#legAt`, …
- `src/world/junction/types.ts#Leg` (interface, line 14) — typeRef by 8: `src/world/junction/levels.ts#fill`, `src/world/junction/levels.ts#framesOf`, `src/world/junction/levels.ts#JunctionLevels.legsAt`, `src/world/junction/levels.ts#LeanJunction.constructor`, …

### src/world/lotStore.ts

- `src/world/lotStore.ts#LotStore.get` (method, line 82) — implementation by 7: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#solveGround`, `src/world/elevation.ts#solveRaised`, `src/world/elevation.ts#solveSunken`, …

### src/world/network.ts

- `src/world/network.ts#Network` (class, line 46) — typeRef by 15: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#everyRoad`, `src/world/elevation.ts#flatSolves`, `src/world/elevation.ts#flatSolvesOf`, …
- `src/world/network.ts#Network.changesSince` (method, line 143) — call by 2: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#roadsToRead`
- `src/world/network.ts#Network.constructor` (constructor, line 100) — read by 9: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#buildRoadElevation.incident`, `src/world/elevation.ts#buildRoadElevation.keyOf`, `src/world/elevation.ts#roadsToRead`, …
- `src/world/network.ts#Network.forkedFrom` (field, line 84) — read by 1: `src/world/elevation.ts#flatSolvesOf`
- `src/world/network.ts#Network.isBuilt` (method, line 170) — call by 2: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#roadsToRead`
- `src/world/network.ts#Network.junctions` (field, line 54) — read by 1: `src/world/elevation.ts#buildRoadElevation`
- `src/world/network.ts#Network.polylines` (field, line 80) — read by 1: `src/render/structures.ts#onAnotherCasing`
- `src/world/network.ts#Network.ribbons` (field, line 55) — read by 9: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#everyRoad`, `src/world/elevation.ts#roadsToRead`, `src/world/elevation.ts#scopeIds`, …
- `src/world/network.ts#Network.segmentCells` (field, line 57) — read by 8: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#pendingOrder`, `src/world/elevation.ts#roadsToRead`, `src/world/elevation.ts#scopeIds`, …
- `src/world/network.ts#Network.state` (getter, line 148) — call by 1: `src/world/elevation.ts#buildRoadElevation`
- `src/world/network.ts#Network.transitions` (field, line 79) — read by 1: `src/world/elevation.ts#buildRoadElevation`
- `src/world/network.ts#Network.trims` (field, line 75) — read by 1: `src/world/elevation.ts#buildRoadElevation`

### src/world/networkDelta.ts

- `src/world/networkDelta.ts#NetworkDelta` (interface, line 5) — read by 2: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#roadsToRead`
- `src/world/networkDelta.ts#NetworkDelta` (interface, line 5) — typeRef by 1: `src/world/elevation.ts#buildRoadElevation`

### src/world/openLots.ts

- `src/world/openLots.ts#OpenLots.near` (method, line 76) — implementation by 2: `src/world/roadField.ts#roadField.nearest`, `src/world/roadField.ts#roadField.query`

### src/world/poleIndex.ts

- `src/world/poleIndex.ts#PoleIndex.near` (method, line 100) — implementation by 2: `src/world/roadField.ts#roadField.nearest`, `src/world/roadField.ts#roadField.query`

### src/world/rampRise.ts

- `src/world/rampRise.ts#PackedRamp.x` (getter, line 34) — implementation by 14: `src/world/elevation.ts#readStations`, `src/world/elevation.ts#stationsAt`, `src/render/structures.ts#boxAlong`, `src/render/structures.ts#parapetRun`, …
- `src/world/rampRise.ts#PackedRamp.y` (getter, line 35) — implementation by 14: `src/world/elevation.ts#readStations`, `src/world/elevation.ts#stationsAt`, `src/render/structures.ts#boxAlong`, `src/render/structures.ts#parapetRun`, …

### src/world/ribbons.ts

- `src/world/ribbons.ts#LeanRibbon.centre` (getter, line 92) — implementation by 2: `src/render/structures.ts#retainingWalls`, `src/render/structures.ts#structureParts`
- `src/world/ribbons.ts#SegmentRibbon` (interface, line 25) — read by 9: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#roadsToRead`, `src/render/structures.ts#retainingWalls`, `src/render/structures.ts#structureParts`, …
- `src/world/ribbons.ts#SegmentRibbon` (interface, line 25) — typeRef by 8: `src/render/structures.ts#buildStructureDetails`, `src/render/structures.ts#retainingWalls`, `src/render/structures.ts#structureParts`, `src/render/structures.ts#structureRibbons`, …
- `src/world/ribbons.ts#SegmentTrims` (interface, line 18) — read by 1: `src/world/elevation.ts#buildRoadElevation`

### src/world/roadTypes.ts

- `src/world/roadTypes.ts#CASING_BAND` (variable, line 95) — read by 1: `src/world/elevation.ts#roadShapeAt`
- `src/world/roadTypes.ts#casingHalf` (function, line 384) — call by 5: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#roadsToRead`, `src/render/structures.ts#onAnotherCasing`, `src/render/structures.ts#retainingWalls`, …
- `src/world/roadTypes.ts#FOOTWAY_RISE` (variable, line 93) — read by 1: `src/world/elevation.ts#solveRaised`
- `src/world/roadTypes.ts#footwayOn` (function, line 356) — call by 1: `src/world/elevation.ts#buildRoadElevation`
- `src/world/roadTypes.ts#halfWidth` (function, line 339) — call by 1: `src/world/junction/levels.ts#legAt`
- `src/world/roadTypes.ts#Level` (enum, line 10) — reference by 9: `src/world/elevation.ts#buildRoadElevation`, `src/render/structures.ts#retainingWalls`, `src/render/structures.ts#structureParts`, `src/world/junction/levels.ts#JunctionLevels.forEachTrim`, …
- `src/world/roadTypes.ts#roadProfile` (function, line 248) — call by 1: `src/render/structures.ts#onAnotherCasing`
- `src/world/roadTypes.ts#RoadType` (interface, line 29) — read by 2: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#lookDigest`
- `src/world/roadTypes.ts#RoadType` (interface, line 29) — typeRef by 5: `src/world/elevation.ts#lookDigest`, `src/world/junction/levels.ts#junctionData`, `src/world/junction/levels.ts#JunctionLevels.data`, `src/world/junction/levels.ts#JunctionLevelsData`, …
- `src/world/roadTypes.ts#sideHalfWidth` (function, line 368) — call by 1: `src/world/junction/levels.ts#legAt`
- `src/world/roadTypes.ts#sidewalkHalf` (function, line 383) — call by 1: `src/render/structures.ts#structureParts`
- `src/world/roadTypes.ts#SURFACE_LEVELS` (variable, line 18) — read by 3: `src/world/junction/levels.ts#JunctionLevels.forEachTrim`, `src/world/junction/levels.ts#JunctionLevels.keys`, `src/world/junction/levels.ts#JunctionLevels.size`
- `src/world/roadTypes.ts#SurfaceLevel` (type, line 25) — implements by 1: `src/world/junction/levels.ts#JunctionLevels`
- `src/world/roadTypes.ts#SurfaceLevel` (type, line 25) — typeRef by 18: `src/world/junction/levels.ts#bit`, `src/world/junction/levels.ts#boundsOf`, `src/world/junction/levels.ts#JunctionData`, `src/world/junction/levels.ts#JunctionLevels.[Symbol.iterator]`, …
- `src/world/roadTypes.ts#symmetric` (function, line 364) — call by 1: `src/world/junction/levels.ts#legAt`

### src/world/terrain.ts

- `src/world/terrain.ts#TerrainIndex.near` (method, line 205) — implementation by 2: `src/world/roadField.ts#roadField.nearest`, `src/world/roadField.ts#roadField.query`

### src/world/turnPaths.ts

- `src/world/turnPaths.ts#areaOf` (function, line 44) — indirectCall by 9: `src/world/elevation.ts#buildRoadElevation`, `src/world/elevation.ts#buildRoadElevation.wantedOf`, `src/world/elevation.ts#pendingOrder`, `src/world/elevation.ts#readStations`, …

## External APIs

- `es:Math` × 206
- `es:Math.max` × 79
- `es:Array.length` × 60
- `es:Float64Array` × 45
- `es:Math.min` × 45
- `es:Set.add` × 42
- `es:Math.floor` × 39
- `es:Array.push` × 38
- `es:Map.get` × 35
- `es:Set` × 30
- `es:Map.set` × 26
- `es:ReadonlyArray.length` × 25
- `es:Map` × 23
- `es:ReadonlySet` × 19
- `es:Set.has` × 18
- `es:NaN` × 17
- `es:ArrayBuffer` × 16
- `es:Infinity` × 16
- `es:Float32Array` × 15
- `es:Array.map` × 13
- `es:Float64Array.length` × 11
- `es:Math.abs` × 10
- `es:Uint32Array` × 10
- `es:Array.slice` × 9
- `es:Array.sort` × 9
- `es:ReadonlyArray.map` × 9
- `es:ReadonlyMap` × 8
- `three:BufferGeometry` × 8
- `es:Math.ceil` × 7
- `es:Map.has` × 6
- `es:Map.values` × 6
- `es:Math.round` × 6
- `three:InstancedMesh` × 6
- `es:Array.filter` × 5
- `es:ArrayLike.length` × 5
- `es:Int32Array` × 5
- `es:Int32Array.length` × 5
- `es:Map.keys` × 5
- `es:Math.hypot` × 5
- `es:ReadonlyArray.filter` × 5
- `es:ReadonlyArray.find` × 5
- `es:Uint8Array` × 5
- `three:Group` × 5
- `three:Material` × 5
- `es:Error` × 4
- `es:Iterable` × 4
- `es:Map.delete` × 4
- `es:MapIterator` × 4
- `es:Math.imul` × 4
- `es:Object` × 4
- `es:ReadonlySet.has` × 4
- `es:WeakMap.get` × 4
- `three:BufferAttribute` × 4
- `three:Object3D.name` × 4
- `es:ArrayLike` × 3
- `es:Float32ArrayConstructor.from` × 3
- `es:Float64Array.set` × 3
- `es:Math.exp` × 3
- `es:ObjectConstructor.freeze` × 3
- `es:ReadonlyArray.forEach` × 3
- `es:ReadonlyMap.get` × 3
- `three:BufferGeometry.dispose` × 3
- `three:BufferGeometry.setAttribute` × 3
- `three:Mesh` × 3
- `three:Mesh.geometry` × 3
- `three:Object3D.castShadow` × 3
- `dom:localStorage` × 2
- `dom:performance` × 2
- `es:Array` × 2
- `es:Array.some` × 2
- `es:Float64Array.buffer` × 2
- `es:Float64Array.subarray` × 2
- `es:Math.log` × 2
- `es:Math.sign` × 2
- `es:Number` × 2
- `es:Pick` × 2
- `es:Required` × 2
- `es:Set.size` × 2
- `es:Uint16Array` × 2
- `es:WeakMap` × 2
- `es:WeakMap.set` × 2
- `three:BoxGeometry` × 2
- `three:BufferAttribute.count` × 2
- `three:InstancedMesh.count` × 2
- `three:Object3D.add` × 2
- `three:Object3D.children` × 2
- `three:Object3D.clear` × 2
- `three:Object3D.position` × 2
- `three:Object3D.receiveShadow` × 2
- `three:Object3D.rotation` × 2
- `three:Object3D.scale` × 2
- `three:Vector3.set` × 2
- `dom:Performance.measure` × 1
- `dom:Performance.now` × 1
- `dom:Storage.getItem` × 1
- `dom:Storage.setItem` × 1
- `es:Array.every` × 1
- `es:Array.fill` × 1
- `es:Array.find` × 1
- `es:Array.pop` × 1
- `es:Array.reduce` × 1
- `es:Float32Array.length` × 1
- `es:Float64ArrayConstructor.from` × 1
- `es:Int32Array.fill` × 1
- `es:IterableIterator` × 1
- `es:Map.clear` × 1
- `es:Map.size` × 1
- `es:Math.atan2` × 1
- `es:Math.cos` × 1
- `es:Math.PI` × 1
- `es:Math.sin` × 1
- `es:NumberConstructor.isFinite` × 1
- `es:NumberConstructor.isNaN` × 1
- `es:ObjectConstructor.create` × 1
- `es:Readonly` × 1
- `es:ReadonlyArray.findIndex` × 1
- `es:ReadonlyArray.slice` × 1
- `es:ReadonlyMap.has` × 1
- `es:ReadonlyMap.size` × 1
- `es:ReadonlyMap.values` × 1
- `es:ReadonlySet.size` × 1
- `es:Record` × 1
- `es:ReturnType` × 1
- `es:Symbol` × 1
- `es:SymbolConstructor.iterator` × 1
- `es:Uint16ArrayConstructor.from` × 1
- `es:Uint32Array.fill` × 1
- `es:Uint32ArrayConstructor.from` × 1
- `es:Uint8Array.set` × 1
- `es:Uint8ArrayConstructor.from` × 1
- `three:BufferGeometry.computeBoundingSphere` × 1
- `three:BufferGeometry.getAttribute` × 1
- `three:BufferGeometry.index` × 1
- `three:BufferGeometry.setIndex` × 1
- `three:Euler.set` × 1
- `three:InstancedBufferAttribute` × 1
- `three:InstancedMesh.computeBoundingSphere` × 1
- `three:InstancedMesh.dispose` × 1
- `three:InstancedMesh.instanceMatrix` × 1
- `three:InstancedMesh.isInstancedMesh` × 1
- `three:Matrix4.toArray` × 1
- `three:Object3D` × 1
- `three:Object3D.matrix` × 1
- `three:Object3D.matrixAutoUpdate` × 1
- `three:Object3D.remove` × 1
- `three:Object3D.updateMatrix` × 1
