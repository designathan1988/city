# world:doc

Grupo: world. Entrada: subsistema. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/world/doc.ts`, `src/world/docBinary.ts`, `src/world/docImage.ts`
- Unidades próprias (só esta funcionalidade alcança): 63 em 2 arquivo(s), 651 linhas
- Alcance total: 1184 unidades; usa 791 compartilhadas de outras; é dona principal de 330 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#showInspector@3573` (6), `src/main.ts#MAP_ACTIONS@1798` (6), `src/main.ts#barrierTool@515` (2), `src/main.ts#SLIDERS@1715` (2), `src/main.ts#transitEditor@854` (1), `src/main.ts#setMarkingStyle@3933` (1), `src/main.ts#setNodeHeightMetres@3546` (1), `src/main.ts#setMapBiome@1777` (1), `src/main.ts#weatherSlider@1675` (1), `src/main.ts#IfStatement@1703` (1)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `world:changes` (59), `world:ids` (46), `world:barriers` (26), `world:tiles` (26), `world:buildings` (22), `world:lot` (21), `world:utilities` (19), `world:roads` (10), `people:spec` (7), `world:tile` (6), `world:terrain` (5), `world:landscape` (5)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `?.a`, `?.after`, `?.approachRules`, `?.at`, `?.auto`, `?.b`, `?.balance`, `?.barriers`, `?.before`, `?.blockedMovements`, `?.borrowedNodes`, `?.buildingSections`, `?.buildingTable`, `?.buildings`, `?.control`, `?.counts`, `?.crossing`, `?.curve`, `?.cutWalls`, `?.dabs`, `?.dashOrigin`, `?.detail`, `?.direction`, `?.doc`, `?.end`, `?.extra`, `?.from`, `?.fromLane`, `?.grown`, `?.heightOffset`, `?.id`, `?.ids`, `?.items`, `?.kind`, `?.lamp`, `?.land`, `?.landscape`, `?.laneLinks`, `?.lanes`, `?.lots`, `?.nodes`, `?.others`, `?.parking`, `?.part`, `?.planted`, `?.points`, `?.preparedBuildings`, `?.radius`, `?.rects`, `?.region`, `?.remove`, `?.repair`, `?.roads`, `?.rule`, `?.section`, `?.seed`, `?.segment`, `?.segments`, `?.settings`, `?.shareBuildings` … (+198)
- Escreve: `?.nodes`, `?.segments`, `EditRecorder.whole`, `LooseNodes.own`, `RoadDoc.barrierCells`, `RoadDoc.barrierIds`, `RoadDoc.barrierMap`, `RoadDoc.below`, `RoadDoc.economy`, `RoadDoc.elementGrid`, `RoadDoc.fogSettings`, `RoadDoc.forkedFrom`, `RoadDoc.gullyAuto`, `RoadDoc.landscapeCells`, `RoadDoc.landscapeIds`, `RoadDoc.landscapeMap`, `RoadDoc.loose`, `RoadDoc.markingStyle`, `RoadDoc.nature`, `RoadDoc.nextTerrainId`, `RoadDoc.nextZoneId`, `RoadDoc.nodeIds`, `RoadDoc.openLotsIndex`, `RoadDoc.poleIds`, `RoadDoc.poleIndex`, `RoadDoc.poleMap`, `RoadDoc.recorder`, `RoadDoc.segIds`, `RoadDoc.sharedIndices`, `RoadDoc.sharedLists`, `RoadDoc.spanIds`, `RoadDoc.spanMap`, `RoadDoc.streetNameCount`, `RoadDoc.terrainRelief`, `RoadDoc.tileContents`, `RoadDoc.tileGrown`, `RoadDoc.tileNext`, `RoadDoc.transit`, `RoadDoc.treeGrid`, `RoadDoc.weather`, `RoadNode.approachRules`, `RoadNode.control`, `RoadNode.crossing`, `RoadNode.end`, `RoadNode.heightOffset`, `RoadNode.laneLinks`, `RoadNode.signal`, `RoadNode.uturn`, `RoadNode.x`, `RoadNode.y`, `RoadSegment.a`, `RoadSegment.b`, `RoadSegment.curve`, `RoadSegment.cutWalls`, `RoadSegment.direction`, `RoadSegment.lanes`, `RoadSegment.parking`, `RoadSegment.section`, `RoadSegment.structure`, `RoadSegment.type`

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/census/worldCensus.spec.ts`, `tests/editor/binarySave.spec.ts`, `tests/editor/blockLift.spec.ts`, `tests/editor/buildPreview.spec.ts`, `tests/editor/builderHover.spec.ts`, `tests/editor/builderMove.spec.ts`, `tests/editor/buildingCorner.spec.ts`, `tests/editor/buildingFloors.spec.ts`, `tests/editor/buildingPlans.spec.ts`, `tests/editor/buildings.spec.ts`, `tests/editor/bulldozer.spec.ts`, `tests/editor/chunkedSave.spec.ts`, `tests/editor/crossStraight.spec.ts`, `tests/editor/crossingPairs.spec.ts`, `tests/editor/dryRunReuse.spec.ts`, `tests/editor/economy.spec.ts`, `tests/editor/editRules.spec.ts`, `tests/editor/editing.spec.ts`, `tests/editor/elementHover.spec.ts`, `tests/editor/exportMap.spec.ts`, `tests/editor/freeRoad.spec.ts`, `tests/editor/fromReference.spec.ts`, `tests/editor/growthWorker.spec.ts`, `tests/editor/history.spec.ts`, `tests/editor/longStroke.spec.ts`, `tests/editor/lotOverlayKey.spec.ts`, `tests/editor/lotProposalPlace.spec.ts`, `tests/editor/lotSnapOnce.spec.ts`, `tests/editor/lotsCutByRoad.spec.ts`, `tests/editor/moveNode.spec.ts`, `tests/editor/persistence.spec.ts`, `tests/editor/pickAlongRay.spec.ts`, `tests/editor/poleCrossing.spec.ts`, `tests/editor/poleNoPath.spec.ts`, `tests/editor/poleTool.spec.ts`, `tests/editor/poles.spec.ts`, `tests/editor/powerLineBuried.spec.ts`, `tests/editor/powerLinePlanner.spec.ts`, `tests/editor/preparedCity.spec.ts`, `tests/editor/primitiveHover.spec.ts`, `tests/editor/roadPriority.spec.ts`, `tests/editor/roadTop.spec.ts`, `tests/editor/roadVerdict.spec.ts`, `tests/editor/roundabout.spec.ts`, `tests/editor/shapeDrag.spec.ts`, `tests/editor/signRule.spec.ts`, `tests/editor/snap.spec.ts`, `tests/editor/snapNear.spec.ts`, `tests/editor/splitLevelTool.spec.ts`, `tests/editor/stairRun.spec.ts`, `tests/editor/streetKit.spec.ts`, `tests/editor/streetObjects.spec.ts`, `tests/editor/streetscapeTool.spec.ts`, `tests/editor/terrainBrush.spec.ts`, `tests/editor/turnBans.spec.ts`, `tests/editor/uturn.spec.ts`, `tests/editor/weld.spec.ts`, `tests/editor/zoneStroke.spec.ts`, `tests/editor/zoning.spec.ts`, `tests/people/spec.spec.ts`, `tests/perf/budgets.spec.ts`, `tests/perf/simScale.spec.ts`, `tests/perf/trafficBuildingsBench.spec.ts`, `tests/render/buildingMesh.spec.ts`, `tests/render/cellPaving.spec.ts`, `tests/render/cellWork.spec.ts`, `tests/render/clearingCells.spec.ts`, `tests/render/editedCells.spec.ts`, `tests/render/facadeLevel.spec.ts`, `tests/render/farSlab.spec.ts`, `tests/render/gardenLists.spec.ts`, `tests/render/grassPlacement.spec.ts`, `tests/render/groundDigest.spec.ts`, `tests/render/layerIncremental.spec.ts`, `tests/render/paintCatalogue.spec.ts`, `tests/render/paintGrow.spec.ts`, `tests/render/partCells.spec.ts`, `tests/render/pierOnRoad.spec.ts`, `tests/render/poleLayer.spec.ts`, `tests/render/poleLevels.spec.ts`, `tests/render/roadPasses.spec.ts`, `tests/render/roadStandIn.spec.ts`, `tests/render/roadTileWorkers.spec.ts`, `tests/render/roadTiles.spec.ts`, `tests/render/roadsRead.spec.ts`, `tests/render/signLayer.spec.ts`, `tests/render/signalBatches.spec.ts`, `tests/render/streetFurnitureLayer.spec.ts`, `tests/render/surfaceInputsResidency.spec.ts`, `tests/render/surfaceReads.spec.ts`, `tests/render/terrainShaping.spec.ts`, `tests/render/tessellation.spec.ts`, `tests/render/utilityWires.spec.ts`, `tests/scale/buildingEdit.spec.ts`, `tests/scale/census/derivedBound.spec.ts`, `tests/scale/edit.spec.ts`, `tests/scale/lotEdit.spec.ts`, `tests/scale/lotPlan.spec.ts`, `tests/scale/render/cityMemory.spec.ts`, `tests/scale/render/layerMemory.spec.ts`, `tests/scale/roadJudge.spec.ts`, `tests/scale/roadKeys.spec.ts`, `tests/sim/admissionGroup.spec.ts`, `tests/sim/advisorReach.spec.ts`, `tests/sim/agents/baysIncremental.spec.ts`, `tests/sim/agents/baysWorkers.spec.ts`, `tests/sim/agents/lotDoors.spec.ts`, `tests/sim/agents/lotGate.spec.ts`, `tests/sim/agents/lotParked.spec.ts`, `tests/sim/agents/parkedInLot.spec.ts`, `tests/sim/agents/population.spec.ts`, `tests/sim/ambientIndex.spec.ts`, `tests/sim/ambientMeso.spec.ts`, `tests/sim/boxPedestrians.spec.ts`, `tests/sim/bulb.spec.ts`, `tests/sim/busesAway.spec.ts`, `tests/sim/cityClock.spec.ts`, `tests/sim/classJoin.spec.ts`, `tests/sim/curvature.spec.ts`, `tests/sim/doLess.spec.ts`, `tests/sim/drive/convoy.spec.ts`, `tests/sim/drive/cycles.spec.ts`, `tests/sim/drive/shortTail.spec.ts`, `tests/sim/drive/tactical.spec.ts`, `tests/sim/drivers.spec.ts`, `tests/sim/heightTopology.spec.ts`, `tests/sim/indexes/baysByDiary.spec.ts`, `tests/sim/indexes/lorries.spec.ts`, `tests/sim/issuesSince.spec.ts`, `tests/sim/junctionRules.spec.ts`, `tests/sim/kerb.spec.ts`, `tests/sim/laneChangeIntent.spec.ts`, `tests/sim/laneChangeRouteFiling.spec.ts`, `tests/sim/laneletGraphCache.spec.ts`, `tests/sim/lorryTraffic.spec.ts`, `tests/sim/lotPlaces.spec.ts`, `tests/sim/lotPlanWorker.spec.ts`, `tests/sim/meso/boundedTick.spec.ts`, `tests/sim/meso/stepGarbage.spec.ts`, `tests/sim/meso/window.spec.ts`, `tests/sim/microWindowCells.spec.ts`, `tests/sim/passKerbStop.spec.ts`, `tests/sim/pedHold.spec.ts`, `tests/sim/populationFollowsCity.spec.ts`, `tests/sim/pose.spec.ts`, `tests/sim/priorityBox.spec.ts`, `tests/sim/queuedSwing.spec.ts`, `tests/sim/rebindReach.spec.ts`, `tests/sim/reservationsChecked.spec.ts`, `tests/sim/restingRears.spec.ts`, `tests/sim/roundabout.spec.ts`, `tests/sim/shortLinkBox.spec.ts`, `tests/sim/sidewalkUpdate.spec.ts`, `tests/sim/signalMovementMatrix.spec.ts`, `tests/sim/signalPlanWindow.spec.ts`, `tests/sim/signalPosts.spec.ts`, `tests/sim/signalWake.spec.ts`, `tests/sim/signals.spec.ts`, `tests/sim/simClientAnswers.spec.ts`, `tests/sim/simClientDecode.spec.ts`, `tests/sim/simClientSignalsReset.spec.ts`, `tests/sim/simClientStart.spec.ts`, `tests/sim/simInitBake.spec.ts`, `tests/sim/simResync.spec.ts`, `tests/sim/stageSeven.spec.ts`, `tests/sim/topologyOffPage.spec.ts`, `tests/sim/traffic.spec.ts`, `tests/sim/trafficBuildings.spec.ts`, `tests/sim/transitLoop.spec.ts`, `tests/sim/transitStops.spec.ts`, `tests/sim/trapLanes.spec.ts`, `tests/sim/uturn.spec.ts`, `tests/sim/vehiclesNear.spec.ts`, `tests/sim/walkArrivals.spec.ts`, `tests/sim/walkHeight.spec.ts`, `tests/sim/walkSpine.spec.ts`, `tests/sim/yardSlices.spec.ts`, `tests/sim/zipperMerge.spec.ts`, `tests/ui/congestion.spec.ts`, `tests/ui/roadProfileEditor.spec.ts`, `tests/world/bendSeam.spec.ts`, `tests/world/bigLists.spec.ts`, `tests/world/buildingNearAny.spec.ts`, `tests/world/buildingPlans.spec.ts`, `tests/world/buildingStoreOwnership.spec.ts`, `tests/world/buildingWear.spec.ts`, `tests/world/buildings.spec.ts`, `tests/world/bulb.spec.ts`, `tests/world/carriedRoad.spec.ts`, `tests/world/changeGuards.spec.ts`, `tests/world/clouds.spec.ts`, `tests/world/conflictCache.spec.ts`, `tests/world/connectorFootprints.spec.ts`, `tests/world/crossingLandings.spec.ts`, `tests/world/curveFit.spec.ts`, `tests/world/cutWalls.spec.ts`, `tests/world/decayWatch.spec.ts`, `tests/world/defaultTown.spec.ts`, `tests/world/diaryPlaces.spec.ts`, `tests/world/docBinary.spec.ts`, `tests/world/docFork.spec.ts`, `tests/world/docImage.spec.ts`, `tests/world/docMemory.spec.ts`, `tests/world/docText.spec.ts`, `tests/world/docTiles.spec.ts`, `tests/world/economy.spec.ts`, `tests/world/elevation.spec.ts`, `tests/world/elevationDifferences.spec.ts`, `tests/world/elevationGround.spec.ts`, `tests/world/elevationLocal.spec.ts`, `tests/world/elevationScope.spec.ts`, `tests/world/facadeOverlap.spec.ts`, `tests/world/fogPaint.spec.ts`, `tests/world/forgetCrossings.spec.ts`, `tests/world/forkApproach.spec.ts`, `tests/world/garageUnder.spec.ts`, `tests/world/gullies.spec.ts`, `tests/world/incrementalRebuild.spec.ts`, `tests/world/junction.spec.ts`, `tests/world/junctionConnectors.spec.ts`, `tests/world/landscapeBatch.spec.ts`, `tests/world/laneLines.spec.ts`, `tests/world/laneTurnRules.spec.ts`, `tests/world/laneletUpdate.spec.ts`, `tests/world/levelFlights.spec.ts`, `tests/world/linksOfSegment.spec.ts`, `tests/world/loadTileDirect.spec.ts`, `tests/world/lorryDocks.spec.ts`, `tests/world/lotCorners.spec.ts`, `tests/world/lotPhysics.spec.ts`, `tests/world/lotRaster.spec.ts`, `tests/world/lotStore.spec.ts`, `tests/world/lots.spec.ts`, `tests/world/medianNose.spec.ts`, `tests/world/migrateOnce.spec.ts`, `tests/world/nearSearch.spec.ts`, `tests/world/nodeSettingsKept.spec.ts`, `tests/world/oldMapsIdentical.spec.ts`, `tests/world/openLots.spec.ts`, `tests/world/openingOffPage.spec.ts`, `tests/world/pageWorldMemory.spec.ts`, `tests/world/poleLinesCells.spec.ts`, `tests/world/poleLinesKept.spec.ts`, `tests/world/rescale.spec.ts`, `tests/world/roadEntries.spec.ts`, `tests/world/roadFurniture.spec.ts`, `tests/world/roadPaint.spec.ts`, `tests/world/roadProfile.spec.ts`, `tests/world/roadSection.spec.ts`, `tests/world/roadStations.spec.ts`, `tests/world/roadValueLedger.spec.ts`, `tests/world/shopFloor.spec.ts`, `tests/world/siteGrading.spec.ts`, `tests/world/solids.spec.ts`, `tests/world/splitLevel.spec.ts`, `tests/world/steppedYards.spec.ts`, `tests/world/streetChain.spec.ts`, `tests/world/streetDoors.spec.ts`, `tests/world/streetFurniture.spec.ts`, `tests/world/streetNames.spec.ts`, `tests/world/surfaces.spec.ts`, `tests/world/terrace.spec.ts`, `tests/world/tileCaps.spec.ts`, `tests/world/tileGates.spec.ts`, `tests/world/tileIndex.spec.ts`, `tests/world/transition.spec.ts`, `tests/world/trees.spec.ts`, `tests/world/turnPlan.spec.ts`, `tests/world/turns.spec.ts`, `tests/world/unboundedPlates.spec.ts`, `tests/world/union.spec.ts`, `tests/world/unzone.spec.ts`, `tests/world/utilities.spec.ts`, `tests/world/variety.spec.ts`, `tests/world/walkways.spec.ts`, `tests/world/weather.spec.ts`, `tests/world/zoneGridOverlap.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `build`: a portar 8
- porte `economy`: já existia 2
- porte `interface`: já existia 37
- porte `landscape`: portado 134
- porte `people`: já existia 2
- porte `roads`: não portado 34
- porte `street-furniture`: portado 2, não se aplica 10
- porte `traffic`: portado 2
- porte `transit`: portado 4

## Código próprio (linhas — declaração — comentário)

### src/world/doc.ts
- 190-192 `RoadDoc.setTransit` (method): The public transport replaced by an edit of it.
- 299-303 `RoadDoc.constructor` (method)
- 309-311 `RoadDoc.lotKeys` (method): The blocks and strips already cut into lots, so a lot deleted on purpose is not made again.
- 341-343 `RoadDoc.zonesChanged` (method): The zoning changed there (`zoneRevision`): the way in for the tools that change it in place
- 349-354 `RoadDoc.savePerson` (method): Adds a person, or replaces the one with the same id.
- 356-361 `RoadDoc.removePerson` (method)
- 364-366 `RoadDoc.nextPersonId` (method): An id no saved person has.
- 378-378 `RoadDoc.barrierRevision` (method): Walls, fences and hedges: a fence drawn must not rebuild the roads.
- 383-383 `RoadDoc.fogRevision` (method)
- 385-385 `RoadDoc.cloudRevision` (method)
- 386-386 `RoadDoc.weatherRevision` (method)
- 389-389 `RoadDoc.elementRevision` (method)
- 390-390 `RoadDoc.zoneRevision` (method)
- 392-392 `RoadDoc.peopleRevision` (method)
- 393-393 `RoadDoc.economyRevision` (method)
- 512-519 `RoadDoc.addBarrier` (method): Draws a barrier along `points` (two or more, on the map).
- 559-573 `RoadDoc.barrierNear` (method): The barrier whose path passes nearest a point, within `radius`, or null: those round it only (`barriersIn`).
- 698-700 `RoadDoc.canEditNode` (method): Whether an edit may change the node (`editableNode`): the tools leave alone a node whose own tile is not loaded.
- 987-996 `RoadDoc.setNodeLaneLinks` (method): The player's lane connections at a node; undefined or empty: all derived (`RoadNode.laneLinks`).
- 999-1004 `RoadDoc.setMarkingStyle` (method): The regional standard of the road paint: every road painted again.
- 1017-1024 `RoadDoc.setNodeEnd` (method): What a road's end is (`RoadNode.end`): a turning circle, or (undefined) the map's edge.
- 1039-1048 `RoadDoc.setNodeSignal` (method): A signal's settings (`RoadNode.signal`); undefined: adaptive with the game's timings.
- 1057-1064 `RoadDoc.setNodeHeightOffset` (method)
- 1067-1070 `RoadDoc.setNature` (method): Gives the map an ecosystem (or takes it away: null).
- 1140-1146 `RoadDoc.clearGullies` (method): A list cleared is written where its items lay (one box round them all), not as the whole map
- 1148-1152 `RoadDoc.setGullyAuto` (method)
- 1161-1165 `RoadDoc.setWeather` (method)
- 1198-1212 `RoadDoc.clearTrees` (method): Every planted tree gone, and every clearing grown over again.
- 1253-1260 `RoadDoc.clearElements` (method)
- 1273-1286 `RoadDoc.addClouds` (method): Several clouds at once, as many as the sky has room for: how many were added.
- 1304-1310 `RoadDoc.clearFog` (method)
- 1312-1316 `RoadDoc.setFogSettings` (method)
- 1318-1324 `RoadDoc.clearPaint` (method)
- 1338-1345 `RoadDoc.clearTerrain` (method)
- 1366-1372 `RoadDoc.clearNodeCrossing` (method): Removes a node's mid-block crossing; the node goes back to `auto`.
- 1374-1383 `RoadDoc.setMovementBlocked` (method)
- 1448-1448 `RoadDoc.tileGates` (field): Each loaded tile's buildings' gates (`TileRecords.gates`) while no building of it changed: written back as they are, not worked out again from every building at each write (`tileRecords`).
- 1450-1450 `RoadDoc.gatesCursor` (field): The buildings' diary as the gates last read it: an entry over a tile lets its gates go.
- 1454-1460 `RoadDoc.tileIndex` (field): Each tile's nodes, roads, land keys and dabs, kept by the diary (`tileIndex.ts`): a tile's members read without walking the region.
- 1467-1469 `RoadDoc.editing` (method): Whether an edit is open (`beginEdit`..`endEdit`): no tile comes in or goes out then.
- 1477-1479 `RoadDoc.hasTile` (method): Whether a tile is in this document.
- 1492-1499 `RoadDoc.tileIdAt` (method): The id a building or a lot made at a place takes (`BuildingStore`, `LotStore`): a loaded tile's, or null for the store's own allocator.
- 1503-1620 `RoadDoc.loadTile` (method): A tile's records put in (`world/tiles.ts` TileRecords): read and checked as a map is (`fromJSON`), moved in, one diary entry per kind with the tile's square and the ids (cause `TILE_LOADED`) - its roads join the nodes already here (a neigh
- 1623-1628 `RoadDoc.tileGatesOf` (method): A tile's gates: those kept while none of its buildings changed, else worked out from them now and kept.
- 1631-1643 `RoadDoc.followGates` (method): The buildings' diary since the gates last read it: each tile an edit reached lets its gates go (a tile put in or let go reaches no other's buildings).
- 1646-1652 `RoadDoc.leaveTiles` (method): The streamed world closed (`main.ts` afterStreamedWorld): no tile is held any more, and no id is taken from a tile's range; the records stay until the document is replaced.
- 1655-1696 `RoadDoc.tileMembers` (method): What a tile holds now, by `OWNER` (what the player added or moved there included): read off the page's document, which holds the active region only, so bounded by it, never by the world
- 1699-1705 `RoadDoc.tileIds` (method): The ids of what a tile holds now (`tileMembers`), the parts another copy of the document follows: what that copy takes in or lets go with the tile (`sim/host/simClient.ts`).
- 1708-1741 `RoadDoc.tileRecords` (method): A tile as the store keeps it (`world/tiles.ts` TileRecords), from what the page's document holds of it now: written when it changed, before it goes out.
- 1744-1822 `RoadDoc.unloadTile` (method): A tile taken out of the page's document (cause `TILE_UNLOADED`): what it holds now (`tileMembers`); a node at its edge stays while a road of a tile still here ends on it
- 2003-2049 `RoadDoc.clone` (method): Makes an independent working copy, including allocator high-water marks.
- 2619-2623 `RoadDoc.worldValues` (method): The map's parts no tile holds (CLAUDE.md, "Streaming world"): its terrain stamps, fog, gullies, clouds, clearings, zones, people, transit, economy, weather and the like, as `toJSON` writes them - the streamed world's values (`editor/tileSto
- 2917-2921 `LooseNodes.copy` (method): An independent copy (`RoadDoc.clone`).
- 2968-2984 `TileMembers` (interface): What a tile holds in the page's document now (`RoadDoc.tileMembers`).
- 2990-2993 `freshTileNext` (function): A tile never written yet counts from the start of its range.
- 2996-3009 `tileNextOf` (function): A tile's counters as written, raised past every id of its range its records hold: a counter written behind (an old save, a tile made by hand) never hands out an id in use, which would overwrite that record.
- 3012-3012 `SPAN_REACH` (variable): How far a span reaches past its pole: the longest a wire runs (`roads/powerLine.ts` spans are tens of metres; a margin).
- 3088-3092 `migratedBuildings` (function): A tile's buildings not prepared by a worker (`preparedTiles`), each through `migrateBuilding` as a map's are (`BuildingStore.load`).
- 3101-3104 `lotKeyPlace` (function)
- 3108-3108 `preparedTiles` (variable): The tiles whose buildings a worker prepared (`editor/tileStore.ts` `prepareTile`: migrated and interned there): `loadTile` files them as they are. A set beside the records, never a field of them: every list field of `TileRecords` is a list.
- 3179-3179 `WORLD_PARTS` (variable)
- 3591-3593 `MemberDiff.toJSON` (method): What the step holds of its own (`patchBytes`): the values that changed; the key orders are one list for every record of that shape (`keyOrder`).

### src/world/docImage.ts
- 7-14 `PARTS_OF_SAVED` (variable): Each saved key of the map and the part that keeps it: a key added to the format and not here does not compile.
