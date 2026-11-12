# Conversion: zoning

Units: 1018. In the game: 0. Translated and accepted: 1. Open: 1017. Constants without a source: 1.

**Not complete.** Each open unit below still has to be written in its owner, by the method given.

## Modules translated

- `src/core/polyline.ts`: accepted, 5 files

## Open units

| Unit | Role | Owner | Why | Method |
|---|---|---|---|---|
| `src/bays.worker.ts#serve` | mixed | `split by responsibility` | a part of shared code: it joins the code of the owner that holds the rest | worker-copy: Transfer the ArrayBuffers (or share memory) instead of cloning them. |
| `src/cityGen.worker.ts#serve` | mixed | `split by responsibility` | a part of shared code: it joins the code of the owner that holds the rest | worker-copy: Transfer the ArrayBuffers (or share memory) instead of cloning them. |
| `src/cityGrowth.worker.ts#serve` | mixed | `split by responsibility` | a part of shared code: it joins the code of the owner that holds the rest | worker-copy: Transfer the ArrayBuffers (or share memory) instead of cloning them. |
| `src/conflictTable.worker.ts#serve` | mixed | `split by responsibility` | a part of shared code: it joins the code of the owner that holds the rest | god-function: Extract each step into a function with one responsibility, owned by the module of its concept.<br>hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous.<br>worker-copy: Transfer the ArrayBuffers (or share memory) instead of cloning them. |
| `src/core/clipper.ts#difference` | compute | `native/src/core` | reaches src/core/clipper.ts#legacyInput (wrong methodology: hot-allocation) |  |
| `src/core/clipper.ts#intersection` | compute | `native/src/core` | reaches src/core/clipper.ts#legacyInput (wrong methodology: hot-allocation) |  |
| `src/core/derivedCache.ts#WriteBudget.tryTake` | compute | `native/src/core` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/core/forkMap.ts#ForkMap.writable` | tick | `native/src/core` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/core/rng.ts#Rng.fromState` | compute | `native/src/core` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/core/rng.ts#Rng.state` | compute | `native/src/core` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/core/rng.ts#RngState` | type | `native/src/core` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/buildingTool.ts#pickAlongRay.fresh` | tick | `native/src/sim/commands` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/editor/bulldozer.ts#Bulldozer.boxed` | compute | `native/src/sim/commands` | a part of shared code: it joins the code of the owner that holds the rest | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/editor/cityGenerator.ts#greenCity` | compute | `native/src/sim/commands` | a part of shared code: it joins the code of the owner that holds the rest (port landscape says já existia, not shown: its owner "" names no file of the game) | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/editor/cityGenerator.ts#zoneCity` | compute | `native/src/sim/commands` | a part of shared code: it joins the code of the owner that holds the rest | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/editor/cityTiles.ts#EDGE_REACH` | data | `native/src/sim/commands` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/editor/cityTiles.ts#TileGrowth` | type | `native/src/sim/commands` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/editor/cityTiles.ts#assembleCity` | compute | `native/src/sim/commands` | a part of shared code: it joins the code of the owner that holds the rest | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/editor/cityTiles.ts#edgeBuildingsNear` | compute | `native/src/sim/commands` | a part of shared code: it joins the code of the owner that holds the rest | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/editor/cityTiles.ts#growTile` | tick | `native/src/sim/commands` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/editor/cityTiles.ts#nearBorder` | compute | `native/src/sim/commands` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/editor/cityTiles.ts#tilePackagesOf` | compute | `native/src/sim/commands` | a part of shared code: it joins the code of the owner that holds the rest | duplicated-code: Extract the shared code into one owner and call it from both places.<br>hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/editor/editRules.ts#flatSolves.after` | tick | `native/src/sim/commands` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/editor/editRules.ts#flatSolves.beforeSolve` | tick | `native/src/sim/commands` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/editor/editRules.ts#overlappedBefore` | compute | `native/src/sim/commands` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/editor/editRules.ts#refuseRoadEdit` | compute | `native/src/sim/commands` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/editor/editRules.ts#segmentsThrough` | compute | `native/src/sim/commands` | a part of shared code: it joins the code of the owner that holds the rest | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/editor/editRules.ts#tooSteep` | compute | `native/src/sim/commands` | a part of shared code: it joins the code of the owner that holds the rest | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/editor/editRules.ts#withinSharedPlate` | compute | `native/src/sim/commands` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/editor/growthFeed.ts#Asked` | type | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/growthFeed.ts#GrowthHost` | type | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/growthFeed.ts#GrowthLane` | type | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/growthFeed.ts#GrowthOutcome` | type | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/growthFeed.ts#GrowthSession` | state | `native/src/sim/commands` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/editor/growthFeed.ts#GrowthSession.ask` | tick | `native/src/sim/commands` | reaches src/editor/growthFeed.ts#GrowthSession.syncBuildings (wrong methodology: hot-allocation) |  |
| `src/editor/growthFeed.ts#GrowthSession.asked` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/growthFeed.ts#GrowthSession.buildingsRead` | state | `native/src/sim/commands` | reaches src/world/buildings/store.ts#BuildingStore (wrong methodology: scattered-writes, hot-allocation) |  |
| `src/editor/growthFeed.ts#GrowthSession.busy` | compute | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/growthFeed.ts#GrowthSession.constructor` | compute | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/growthFeed.ts#GrowthSession.forgetRoads` | tick | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/growthFeed.ts#GrowthSession.lotOpen` | compute | `native/src/sim/commands` | reaches src/world/buildings/store.ts#BuildingStore (wrong methodology: scattered-writes, hot-allocation) |  |
| `src/editor/growthFeed.ts#GrowthSession.most` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/growthFeed.ts#GrowthSession.moves` | state | `native/src/sim/commands` | reaches src/world/changes.ts#ChangeJournal.since (wrong methodology: hot-allocation) |  |
| `src/editor/growthFeed.ts#GrowthSession.nextAsk` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/growthFeed.ts#GrowthSession.readChanges` | tick | `native/src/sim/commands` | reaches src/world/buildings/store.ts#BuildingStore (wrong methodology: scattered-writes, hot-allocation) |  |
| `src/editor/growthFeed.ts#GrowthSession.receive` | tick | `native/src/sim/commands` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/editor/growthFeed.ts#GrowthSession.refusals` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/growthFeed.ts#GrowthSession.resetBuildings` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/growthFeed.ts#GrowthSession.resetRoads` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/growthFeed.ts#GrowthSession.restart` | tick | `native/src/sim/commands` | reaches src/editor/growthFeed.ts#GrowthSession.syncBuildings (wrong methodology: hot-allocation) |  |
| `src/editor/growthFeed.ts#GrowthSession.roadsOf` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/growthFeed.ts#GrowthSession.roadsState` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/growthFeed.ts#GrowthSession.seed` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/growthFeed.ts#GrowthSession.send` | tick | `native/src/sim/commands` | reaches src/editor/growthFeed.ts#GrowthSession.syncBuildings (wrong methodology: hot-allocation) |  |
| `src/editor/growthFeed.ts#GrowthSession.send.union` | compute | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/growthFeed.ts#GrowthSession.sentBuildings` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/growthFeed.ts#GrowthSession.sentKerbs` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/growthFeed.ts#GrowthSession.sentRoads` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/growthFeed.ts#GrowthSession.staleBuildings` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/growthFeed.ts#GrowthSession.staleKerbs` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/growthFeed.ts#GrowthSession.staleRoads` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/growthFeed.ts#GrowthSession.syncBuildings` | tick | `native/src/sim/commands` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/editor/growthFeed.ts#GrowthSession.syncRoads` | tick | `native/src/sim/commands` | reaches src/core/ring.ts#Ring.flatten (wrong methodology: hot-allocation) |  |
| `src/editor/growthFeed.ts#MISS_MARGIN` | data | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/growthFeed.ts#MOST_MISSES` | data | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/growthFeed.ts#MOST_REFUSALS` | data | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/growthFeed.ts#MOVES` | data | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/growthFeed.ts#READ_PAD` | data | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/growthFeed.ts#REGION_HALF` | data | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/growthFeed.ts#grow` | compute | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/growthFeed.ts#junctionPiece` | compute | `native/src/sim/commands` | reaches src/core/ring.ts#Ring.flatten (wrong methodology: hot-allocation) |  |
| `src/editor/growthFeed.ts#roadPiece` | compute | `native/src/sim/commands` | reaches src/world/networkBuild.ts#NetworkBuild (wrong methodology: hot-allocation, god-function) |  |
| `src/editor/growthReplica.ts#BATCH_MOST` | data | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/growthReplica.ts#GrowAnswer` | type | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/growthReplica.ts#GrowAsk` | type | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/growthReplica.ts#Grown` | type | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/growthReplica.ts#GrowthMiss` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/growthReplica.ts#GrowthReplica` | state | `native/src/sim/commands` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/editor/growthReplica.ts#GrowthReplica.handle` | tick | `native/src/sim/commands` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/editor/growthReplica.ts#GrowthReplica.neighbours` | state | `native/src/sim/commands` | reaches src/world/buildings/grid.ts#BuildingGrid (wrong methodology: hot-allocation) |  |
| `src/editor/growthReplica.ts#GrowthReplica.roads` | state | `native/src/sim/commands` | reaches src/editor/growthReplica.ts#RoadReplica (wrong methodology: hot-allocation) |  |
| `src/editor/growthReplica.ts#GrowthSync` | type | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/growthReplica.ts#JunctionPiece` | type | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/growthReplica.ts#NeighbourReplica` | state | `native/src/sim/commands` | reaches src/world/buildings/grid.ts#BuildingGrid (wrong methodology: hot-allocation) |  |
| `src/editor/growthReplica.ts#NeighbourReplica.apply` | tick | `native/src/sim/commands` | reaches src/world/buildings/grid.ts#BuildingGrid (wrong methodology: hot-allocation) |  |
| `src/editor/growthReplica.ts#NeighbourReplica.covered` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/growthReplica.ts#NeighbourReplica.grew` | tick | `native/src/sim/commands` | reaches src/world/buildings/types.ts#Building (wrong methodology: scattered-writes) |  |
| `src/editor/growthReplica.ts#NeighbourReplica.grid` | state | `native/src/sim/commands` | reaches src/world/buildings/grid.ts#BuildingGrid (wrong methodology: hot-allocation) |  |
| `src/editor/growthReplica.ts#NeighbourReplica.items` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/growthReplica.ts#NeighbourReplica.near` | compute | `native/src/sim/commands` | reaches src/world/buildings/grid.ts#BuildingGrid.near (wrong methodology: hot-allocation) |  |
| `src/editor/growthReplica.ts#NeighbourReplica.own` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/growthReplica.ts#NeighbourReplica.put` | tick | `native/src/sim/commands` | reaches src/world/buildings/grid.ts#BuildingGrid (wrong methodology: hot-allocation) |  |
| `src/editor/growthReplica.ts#RoadPiece` | type | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/growthReplica.ts#RoadReplica` | state | `native/src/sim/commands` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/editor/growthReplica.ts#RoadReplica.apply` | tick | `native/src/sim/commands` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/editor/growthReplica.ts#RoadReplica.check` | compute | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/growthReplica.ts#RoadReplica.covered` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/growthReplica.ts#RoadReplica.doc` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/growthReplica.ts#RoadReplica.dropKerb` | tick | `native/src/sim/commands` | reaches src/world/cells.ts#CellFiling (wrong methodology: hot-allocation, duplicated-code) |  |
| `src/editor/growthReplica.ts#RoadReplica.dropRoad` | tick | `native/src/sim/commands` | reaches src/world/cells.ts#CellFiling (wrong methodology: hot-allocation, duplicated-code) |  |
| `src/editor/growthReplica.ts#RoadReplica.junctionCells` | state | `native/src/sim/commands` | reaches src/world/cells.ts#CellFiling (wrong methodology: hot-allocation, duplicated-code) |  |
| `src/editor/growthReplica.ts#RoadReplica.junctions` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/growthReplica.ts#RoadReplica.kerbCells` | state | `native/src/sim/commands` | reaches src/world/cells.ts#CellFiling (wrong methodology: hot-allocation, duplicated-code) |  |
| `src/editor/growthReplica.ts#RoadReplica.kerbs` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/growthReplica.ts#RoadReplica.records` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/growthReplica.ts#RoadReplica.ribbons` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/growthReplica.ts#RoadReplica.ribbonsMeeting` | compute | `native/src/sim/commands` | reaches src/world/cells.ts#CellFiling (wrong methodology: hot-allocation, duplicated-code) |  |
| `src/editor/growthReplica.ts#RoadReplica.roadCells` | state | `native/src/sim/commands` | reaches src/world/cells.ts#CellFiling (wrong methodology: hot-allocation, duplicated-code) |  |
| `src/editor/growthReplica.ts#RoadReplica.roads` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/growthReplica.ts#RoadReplica.segmentCells` | state | `native/src/sim/commands` | reaches src/world/cells.ts#CellFiling (wrong methodology: hot-allocation, duplicated-code) |  |
| `src/editor/growthReplica.ts#Structured` | type | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/growthReplica.ts#pavedLookup` | compute | `native/src/sim/commands` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/editor/growthReplica.ts#within` | compute | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#CellRange` | type | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#DRAG_REACH` | data | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#GESTURE_RANK` | data | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotDecor` | type | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotOverlay` | type | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotOverlayBook` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotOverlayBook.changed` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotOverlayBook.changedFor` | tick | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotOverlayBook.groups` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotOverlayBook.labels` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotOverlayBook.put` | tick | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotOverlayBook.reader` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotOverlayGroup` | type | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotSplitKind` | type | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotTool` | state | `native/src/sim/commands` | wrong methodology: hot-allocation, quadratic-proximity, scattered-writes, duplicated-code | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous.<br>quadratic-proximity: A spatial partition: uniform grid or spatial hash, querying only nearby cells.<br>scattered-writes: One owner mutates the state; everyone else sends it a command or action.<br>duplicated-code: Extract the shared code into one owner and call it from both places. |
| `src/editor/lotTool.ts#LotTool.adoptProposal` | compute | `native/src/sim/commands` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/editor/lotTool.ts#LotTool.advanceProposal` | tick | `native/src/sim/commands` | reaches src/editor/lotTool.ts#LotTool.commitStroke (wrong methodology: hot-allocation) |  |
| `src/editor/lotTool.ts#LotTool.anyZoned` | tick | `native/src/sim/commands` | reaches src/world/lotStore.ts#LotStore (wrong methodology: hot-allocation) |  |
| `src/editor/lotTool.ts#LotTool.asking` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotTool.book` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotTool.brushProposed` | tick | `native/src/sim/commands` | reaches src/editor/lotTool.ts#LotTool.proposedAt (wrong methodology: hot-allocation) |  |
| `src/editor/lotTool.ts#LotTool.builtCursor` | state | `native/src/sim/commands` | reaches src/world/changes.ts#ChangeJournal.since (wrong methodology: hot-allocation) |  |
| `src/editor/lotTool.ts#LotTool.cancel` | tick | `native/src/sim/commands` | reaches src/editor/lotTool.ts#LotTool.commitStroke (wrong methodology: hot-allocation) |  |
| `src/editor/lotTool.ts#LotTool.commitStroke` | tick | `native/src/sim/commands` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/editor/lotTool.ts#LotTool.constructor` | compute | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotTool.corner` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotTool.curve` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotTool.cutLine` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotTool.doc` | compute | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotTool.down` | tick | `native/src/sim/commands` | wrong methodology: quadratic-proximity | quadratic-proximity: A spatial partition: uniform grid or spatial hash, querying only nearby cells. |
| `src/editor/lotTool.ts#LotTool.drawAllLots` | tick | `native/src/sim/commands` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/editor/lotTool.ts#LotTool.drawLot` | tick | `native/src/sim/commands` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/editor/lotTool.ts#LotTool.drawLot.dragged` | compute | `native/src/sim/commands` | reaches src/editor/lotTool.ts#LotTool.drawLot (wrong methodology: hot-allocation) |  |
| `src/editor/lotTool.ts#LotTool.drawn` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotTool.drawnCorner` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotTool.drawnFrom` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotTool.drawnLots` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotTool.drawnRange` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotTool.drawnWith` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotTool.erase` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotTool.eraseUnder` | tick | `native/src/sim/commands` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/editor/lotTool.ts#LotTool.frontSideOf` | compute | `native/src/sim/commands` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/editor/lotTool.ts#LotTool.gesture` | compute | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotTool.gestureKey` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotTool.held` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotTool.holdOverlay` | tick | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotTool.hover` | state | `native/src/sim/commands` | the translator rewrites scattered-writes | scattered-writes: One owner mutates the state; everyone else sends it a command or action. |
| `src/editor/lotTool.ts#LotTool.inWindow` | compute | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotTool.joinFirst` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotTool.keptRange` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotTool.landLot` | compute | `native/src/sim/commands` | reaches src/editor/lotTool.ts#LotTool.frontSideOf (wrong methodology: hot-allocation) |  |
| `src/editor/lotTool.ts#LotTool.lotAt` | compute | `native/src/sim/commands` | reaches src/world/lotStore.ts#LotStore.at (wrong methodology: hot-allocation) |  |
| `src/editor/lotTool.ts#LotTool.lotsCursor` | state | `native/src/sim/commands` | reaches src/world/lotStore.ts#LotStore (wrong methodology: hot-allocation) |  |
| `src/editor/lotTool.ts#LotTool.lotsNear` | compute | `native/src/sim/commands` | reaches src/world/lotStore.ts#LotStore (wrong methodology: hot-allocation) |  |
| `src/editor/lotTool.ts#LotTool.lotsSeen` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotTool.modeChanged` | tick | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotTool.move` | tick | `native/src/sim/commands` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/editor/lotTool.ts#LotTool.moveWindow` | tick | `native/src/sim/commands` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/editor/lotTool.ts#LotTool.net` | compute | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotTool.nextProposalId` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotTool.overlay` | tick | `native/src/sim/commands` | reaches src/editor/lotTool.ts#LotTool.refreshGesture (wrong methodology: hot-allocation) |  |
| `src/editor/lotTool.ts#LotTool.painted` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotTool.paintedRemove` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotTool.polygon` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotTool.proposal` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotTool.proposalCursor` | state | `native/src/sim/commands` | reaches src/world/changes.ts#ChangeJournal.since (wrong methodology: hot-allocation) |  |
| `src/editor/lotTool.ts#LotTool.proposalFiling` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotTool.proposalKey` | tick | `native/src/sim/commands` | reaches src/world/changes.ts#ChangeJournal.since (wrong methodology: hot-allocation) |  |
| `src/editor/lotTool.ts#LotTool.proposalNet` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotTool.proposalReady` | compute | `native/src/sim/commands` | reaches src/world/changes.ts#ChangeJournal.since (wrong methodology: hot-allocation) |  |
| `src/editor/lotTool.ts#LotTool.proposalStamp` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotTool.proposedAt` | tick | `native/src/sim/commands` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/editor/lotTool.ts#LotTool.readPending` | tick | `native/src/sim/commands` | reaches src/editor/lotTool.ts#LotTool.proposedAt (wrong methodology: hot-allocation) |  |
| `src/editor/lotTool.ts#LotTool.refreshGesture` | tick | `native/src/sim/commands` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/editor/lotTool.ts#LotTool.refreshGesture.at` | compute | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotTool.refreshGesture.hoverSnapped` | tick | `native/src/sim/commands` | reaches src/editor/lotTool.ts#LotTool.refreshGesture (wrong methodology: hot-allocation) |  |
| `src/editor/lotTool.ts#LotTool.refreshLots` | tick | `native/src/sim/commands` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/editor/lotTool.ts#LotTool.refreshProposal` | tick | `native/src/sim/commands` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/editor/lotTool.ts#LotTool.refreshProposal.draw` | compute | `native/src/sim/commands` | reaches src/editor/lotTool.ts#LotTool.refreshProposal (wrong methodology: hot-allocation) |  |
| `src/editor/lotTool.ts#LotTool.refreshProposal.stateOf` | compute | `native/src/sim/commands` | reaches src/editor/lotTool.ts#LotTool.refreshProposal (wrong methodology: hot-allocation) |  |
| `src/editor/lotTool.ts#LotTool.refused` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotTool.refusedNet` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotTool.refusedNow` | tick | `native/src/sim/commands` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/editor/lotTool.ts#LotTool.seen` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotTool.shownProposal` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotTool.sideAt` | compute | `native/src/sim/commands` | wrong methodology: duplicated-code, hot-allocation, quadratic-proximity | duplicated-code: Extract the shared code into one owner and call it from both places.<br>hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous.<br>quadratic-proximity: A spatial partition: uniform grid or spatial hash, querying only nearby cells. |
| `src/editor/lotTool.ts#LotTool.sideNear` | compute | `native/src/sim/commands` | wrong methodology: hot-allocation, quadratic-proximity | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous.<br>quadratic-proximity: A spatial partition: uniform grid or spatial hash, querying only nearby cells. |
| `src/editor/lotTool.ts#LotTool.snap` | compute | `native/src/sim/commands` | reaches src/world/lots.ts#lotSnapper (wrong methodology: hot-allocation, quadratic-proximity) |  |
| `src/editor/lotTool.ts#LotTool.snapExcept` | compute | `native/src/sim/commands` | reaches src/world/lots.ts#lotSnapper (wrong methodology: hot-allocation, quadratic-proximity) |  |
| `src/editor/lotTool.ts#LotTool.snapReach` | compute | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotTool.streetAngleNear` | compute | `native/src/sim/commands` | reaches src/world/nearestRibbon.ts#nearestRibbon (wrong methodology: hot-allocation, quadratic-proximity) |  |
| `src/editor/lotTool.ts#LotTool.stroke` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotTool.takeWaiting` | tick | `native/src/sim/commands` | reaches src/editor/lotTool.ts#LotTool.commitStroke (wrong methodology: hot-allocation) |  |
| `src/editor/lotTool.ts#LotTool.up` | tick | `native/src/sim/commands` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/editor/lotTool.ts#LotTool.waiting` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotTool.windowCells` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotTool.zonedCursor` | state | `native/src/sim/commands` | reaches src/world/lotStore.ts#LotStore (wrong methodology: hot-allocation) |  |
| `src/editor/lotTool.ts#LotTool.zonedIds` | state | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#LotToolHost` | type | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#NO_PROPOSAL` | data | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#PROPOSAL_CELL` | data | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#PROPOSAL_RANK` | data | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#REFUSED_REACH` | data | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#STROKE_STEP` | data | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#WINDOW_CELL` | data | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#WINDOW_KEEP` | data | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#WINDOW_REACH` | data | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#ZONE_COLOURS` | data | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#ZoneMode` | type | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#cellRange` | compute | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#forCellsOutside` | compute | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#forCellsOutside.strip` | compute | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#forWindowCells` | compute | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#sameRange` | compute | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#segmentCrossesLot` | compute | `native/src/sim/commands` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/editor/lotTool.ts#windowCellRect` | compute | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/lotTool.ts#windowKey` | compute | `native/src/sim/commands` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/editor/preparedCity.ts#prepareCity` | compute | `native/src/sim/commands` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/editor/roads/powerLine.ts#runPlanner` | compute | `native/src/sim/commands` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/editor/save.worker.ts#serve` | mixed | `split by responsibility` | a part of shared code: it joins the code of the owner that holds the rest | worker-copy: Transfer the ArrayBuffers (or share memory) instead of cloning them. |
| `src/editor/saveLayers.ts#KEYED` | data | `native/src/sim/commands` | a part of shared code: it joins the code of the owner that holds the rest | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/editor/smallCityMap.ts#growLots` | compute | `native/src/sim/commands` | a part of shared code: it joins the code of the owner that holds the rest (port landscape says já existia, not shown: its owner "" names no file of the game) | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/editor/smallCityMap.ts#layOut` | compute | `native/src/sim/commands` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/editor/squaresRead.worker.ts#serve` | mixed | `split by responsibility` | a part of shared code: it joins the code of the owner that holds the rest | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous.<br>worker-copy: Transfer the ArrayBuffers (or share memory) instead of cloning them. |
| `src/editor/workerParts.ts#takeStoreSetup` | tick | `native/src/sim/commands` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/graphicsWiring.ts#createGraphicsWiring` | ui | `project/ui` | a part of shared code: it joins the code of the owner that holds the rest | god-function: Extract each step into a function with one responsibility, owned by the module of its concept.<br>module-state: Keep state in one store (single source of truth), read-only to everyone else, changed only by actions. |
| `src/growth.worker.ts#serve` | mixed | `split by responsibility` | split by responsibility first: what changes state becomes a command in sim/commands, what draws goes to render, what shows goes to project/ui | worker-copy: Transfer the ArrayBuffers (or share memory) instead of cloning them. |
| `src/growthPool.ts#GrowthPool` | state | `no owner yet: decide by what it does` | reaches src/editor/growthReplica.ts#GrowthReplica (wrong methodology: hot-allocation) |  |
| `src/growthPool.ts#GrowthPool.constructor` | compute | `no owner yet: decide by what it does` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/growthPool.ts#GrowthPool.dispose` | tick | `no owner yet: decide by what it does` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/growthPool.ts#GrowthPool.fail` | tick | `no owner yet: decide by what it does` | reaches src/editor/growthReplica.ts#GrowthReplica (wrong methodology: hot-allocation) |  |
| `src/growthPool.ts#GrowthPool.held` | state | `no owner yet: decide by what it does` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/growthPool.ts#GrowthPool.local` | state | `no owner yet: decide by what it does` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/growthPool.ts#GrowthPool.offer` | tick | `no owner yet: decide by what it does` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/growthPool.ts#GrowthPool.pending` | state | `no owner yet: decide by what it does` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/growthPool.ts#GrowthPool.post` | tick | `no owner yet: decide by what it does` | reaches src/growthPool.ts#GrowthPool.start (mixed code) |  |
| `src/growthPool.ts#GrowthPool.postedAt` | state | `no owner yet: decide by what it does` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/growthPool.ts#GrowthPool.pump` | tick | `no owner yet: decide by what it does` | reaches src/editor/growthReplica.ts#GrowthReplica.handle (wrong methodology: hot-allocation) |  |
| `src/growthPool.ts#GrowthPool.start` | mixed | `split by responsibility` | split by responsibility first: what changes state becomes a command in sim/commands, what draws goes to render, what shows goes to project/ui |  |
| `src/growthPool.ts#GrowthPool.told` | state | `no owner yet: decide by what it does` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/growthPool.ts#GrowthPool.worker` | state | `no owner yet: decide by what it does` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/growthPool.ts#SILENCE_MS` | data | `no owner yet: decide by what it does` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/job.worker.ts#<ExpressionStatement>` | ui | `project/ui` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/job.worker.ts#LOADERS` | data | `no owner yet: decide by what it does` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/job.worker.ts#adopt` | ui | `project/ui` | a part of shared code: it joins the code of the owner that holds the rest | worker-copy: Transfer the ArrayBuffers (or share memory) instead of cloning them. |
| `src/lab/doctor/waste.ts#scanWaste.bandOf` | render | `native/src/render` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/lab/inspector/entity.ts#entityInfo` | compute | `native/src/api/lab` | a part of shared code: it joins the code of the owner that holds the rest | god-function: Extract each step into a function with one responsibility, owned by the module of its concept. |
| `src/main.ts#<IfStatement>@4097` | render | `native/src/render` | a part of shared code: it joins the code of the owner that holds the rest | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/main.ts#cancelGestures` | compute | `no owner yet: decide by what it does` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/main.ts#currentGesture` | compute | `no owner yet: decide by what it does` | a part of shared code: it joins the code of the owner that holds the rest (port landscape says portado, not shown: its owner "" names no file of the game) |  |
| `src/main.ts#drawOverlayScreen` | mixed | `split by responsibility` | a part of shared code: it joins the code of the owner that holds the rest (port landscape says portado, not shown: its owner "" names no file of the game) | god-function: Extract each step into a function with one responsibility, owned by the module of its concept.<br>hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/main.ts#endPointer` | mixed | `split by responsibility` | a part of shared code: it joins the code of the owner that holds the rest (port landscape says portado, not shown: its owner "" names no file of the game) |  |
| `src/main.ts#frame` | mixed | `split by responsibility` | a part of shared code: it joins the code of the owner that holds the rest (port landscape says portado, not shown: its owner "" names no file of the game) | god-function: Extract each step into a function with one responsibility, owned by the module of its concept.<br>hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/main.ts#gameState` | data | `no owner yet: decide by what it does` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/main.ts#gestureInProgress` | compute | `no owner yet: decide by what it does` | a part of shared code: it joins the code of the owner that holds the rest (port landscape says portado, not shown: its owner "" names no file of the game) |  |
| `src/main.ts#growZones` | compute | `no owner yet: decide by what it does` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/main.ts#growth` | data | `no owner yet: decide by what it does` | a part of shared code: it joins the code of the owner that holds the rest (port landscape says portado, not shown: its owner "" names no file of the game) |  |
| `src/main.ts#growthAnswered` | tick | `no owner yet: decide by what it does` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/main.ts#growthPool` | data | `no owner yet: decide by what it does` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/main.ts#lotTool` | data | `no owner yet: decide by what it does` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/main.ts#mutateBuilt` | tick | `no owner yet: decide by what it does` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/main.ts#pointerDown` | mixed | `split by responsibility` | a part of shared code: it joins the code of the owner that holds the rest (port landscape says portado, not shown: its owner "" names no file of the game) | god-function: Extract each step into a function with one responsibility, owned by the module of its concept. |
| `src/main.ts#pointerMove` | mixed | `split by responsibility` | a part of shared code: it joins the code of the owner that holds the rest (port landscape says portado, not shown: its owner "" names no file of the game) |  |
| `src/main.ts#setZone` | tick | `no owner yet: decide by what it does` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/networkBake.worker.ts#serve` | mixed | `split by responsibility` | a part of shared code: it joins the code of the owner that holds the rest | worker-copy: Transfer the ArrayBuffers (or share memory) instead of cloning them. |
| `src/render/buildings/buildingMesh.ts#emitProgram` | tick | `native/src/render/buildings` | a part of shared code: it joins the code of the owner that holds the rest | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/render/buildings/cameraSolid.worker.ts#serve` | mixed | `split by responsibility` | a part of shared code: it joins the code of the owner that holds the rest | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous.<br>worker-copy: Transfer the ArrayBuffers (or share memory) instead of cloning them. |
| `src/render/buildings/cell.worker.ts#serve` | mixed | `split by responsibility` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/render/buildings/coverCell.worker.ts#serve` | mixed | `split by responsibility` | a part of shared code: it joins the code of the owner that holds the rest | worker-copy: Transfer the ArrayBuffers (or share memory) instead of cloning them. |
| `src/render/buildings/finishes.ts#createFarShellMaterial` | render | `native/src/render` | a part of shared code: it joins the code of the owner that holds the rest | undisposed-gpu-resource: Dispose every GPU resource when it is no longer used (one owner releases it). |
| `src/render/buildings/finishes.ts#createShellMaterial` | render | `native/src/render` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/render/buildings/fracture.worker.ts#serve` | mixed | `split by responsibility` | a part of shared code: it joins the code of the owner that holds the rest | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous.<br>worker-copy: Transfer the ArrayBuffers (or share memory) instead of cloning them. |
| `src/render/buildings/massingMaterial.ts#createMassingMaterial` | render | `native/src/render` | a part of shared code: it joins the code of the owner that holds the rest | undisposed-gpu-resource: Dispose every GPU resource when it is no longer used (one owner releases it). |
| `src/render/citizenBake.worker.ts#serve` | mixed | `split by responsibility` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/render/derivedWriter.worker.ts#serve` | mixed | `split by responsibility` | a part of shared code: it joins the code of the owner that holds the rest | worker-copy: Transfer the ArrayBuffers (or share memory) instead of cloning them. |
| `src/render/derivedWriterLane.ts#DerivedWriterLane.post` | compute | `native/src/render/derived_writer_lane` | reaches src/render/derivedWriterLane.ts#DerivedWriterLane.constructor (mixed code) |  |
| `src/render/ecology.worker.ts#serve` | mixed | `split by responsibility` | a part of shared code: it joins the code of the owner that holds the rest (port landscape says já existia, not shown: its owner "" names no file of the game) | worker-copy: Transfer the ArrayBuffers (or share memory) instead of cloning them. |
| `src/render/flatAtlas.ts#createFlatAtlas.readWholeContent` | tick | `native/src/render/flat_atlas` | a part of shared code: it joins the code of the owner that holds the rest | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/render/flatGround.worker.ts#serve` | mixed | `split by responsibility` | a part of shared code: it joins the code of the owner that holds the rest | worker-copy: Transfer the ArrayBuffers (or share memory) instead of cloning them. |
| `src/render/furnitureCell.worker.ts#serve` | mixed | `split by responsibility` | a part of shared code: it joins the code of the owner that holds the rest | worker-copy: Transfer the ArrayBuffers (or share memory) instead of cloning them. |
| `src/render/gardenCell.worker.ts#serve` | mixed | `split by responsibility` | a part of shared code: it joins the code of the owner that holds the rest | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous.<br>worker-copy: Transfer the ArrayBuffers (or share memory) instead of cloning them. |
| `src/render/groundShape.worker.ts#serve` | mixed | `split by responsibility` | a part of shared code: it joins the code of the owner that holds the rest | worker-copy: Transfer the ArrayBuffers (or share memory) instead of cloning them. |
| `src/render/landStroke.worker.ts#serve` | mixed | `split by responsibility` | a part of shared code: it joins the code of the owner that holds the rest | worker-copy: Transfer the ArrayBuffers (or share memory) instead of cloning them. |
| `src/render/lotLattice.ts#AA_MARGIN` | data | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#DASH` | data | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#FLAG_BUTT` | data | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#FLAG_DASHED` | data | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#FLAG_FILL` | data | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#FLAG_INSIDE` | data | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#FLAG_LEVEL` | data | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#FLAG_STROKE` | data | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#GAP` | data | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#LOT_CELL` | data | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#LatticeShape` | type | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#LotLattice` | state | `native/src/render/lot_lattice` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/render/lotLattice.ts#LotLattice.add` | tick | `native/src/render/lot_lattice` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/render/lotLattice.ts#LotLattice.alloc` | tick | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#LotLattice.blocks` | state | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#LotLattice.cellShapes` | state | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#LotLattice.cellsPerSide` | state | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#LotLattice.cellsWritten` | state | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#LotLattice.clear` | tick | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#LotLattice.compact` | tick | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#LotLattice.constructor` | tick | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#LotLattice.data` | state | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#LotLattice.dirty` | state | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#LotLattice.flush` | tick | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#LotLattice.grow` | tick | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#LotLattice.live` | state | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#LotLattice.mark` | tick | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#LotLattice.placed` | state | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#LotLattice.records` | compute | `native/src/render/lot_lattice` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/render/lotLattice.ts#LotLattice.remove` | tick | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#LotLattice.reorder` | tick | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#LotLattice.reserve` | compute | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#LotLattice.rows` | compute | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#LotLattice.setEntry` | tick | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#LotLattice.setTable` | tick | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#LotLattice.size` | compute | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#LotLattice.streams` | state | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#LotLattice.tableTexels` | state | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#LotLattice.takeUpload` | tick | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#LotLattice.tilesPerSide` | state | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#LotLattice.top` | state | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#LotLattice.wholeChanged` | state | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#LotLattice.writeCell` | tick | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#LotLattice.written` | state | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#LotLattice.x0` | state | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#LotLattice.y0` | state | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#Placed` | type | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#REFERENCES` | data | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#REFERENCE_CLEAR` | data | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#Stream` | type | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#TEXTURE_WIDTH` | data | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#TILE_CELLS` | data | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#UPLOAD_GAP` | data | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#UPLOAD_RANGES` | data | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#f32` | data | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#floatBits` | tick | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#halfBits` | tick | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#insideEdges` | compute | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#segmentDistance` | compute | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#segmentNearBox` | compute | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#segmentNearBox.clip` | tick | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotLattice.ts#u32` | data | `native/src/render/lot_lattice` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotOverlay.ts#<ExpressionStatement>` | render | `native/src/render` | three.js geometry goes through MeshBuilder, instances through MultiMesh grouped by area, materials through the game's shaders and tables, streaming through the render owners |  |
| `src/render/lotOverlay.ts#<ExpressionStatement>@59` | render | `native/src/render` | three.js geometry goes through MeshBuilder, instances through MultiMesh grouped by area, materials through the game's shaders and tables, streaming through the render owners |  |
| `src/render/lotOverlay.ts#BAND` | data | `native/src/render/lot_overlay` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotOverlay.ts#CELL_SHIFT` | data | `native/src/render/lot_overlay` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotOverlay.ts#CELL_SIDE` | data | `native/src/render/lot_overlay` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotOverlay.ts#EMPTY` | render | `native/src/render` | three.js geometry goes through MeshBuilder, instances through MultiMesh grouped by area, materials through the game's shaders and tables, streaming through the render owners |  |
| `src/render/lotOverlay.ts#FACING_UP` | data | `native/src/render/lot_overlay` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotOverlay.ts#FRAME_HALF` | data | `native/src/render/lot_overlay` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotOverlay.ts#GROUND_KINDS` | data | `native/src/render/lot_overlay` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotOverlay.ts#Keyed` | type | `native/src/render/lot_overlay` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotOverlay.ts#LEVEL_REACH` | data | `native/src/render/lot_overlay` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotOverlay.ts#LOT_PARS` | data | `native/src/render/lot_overlay` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotOverlay.ts#LotOverlayGroup` | type | `native/src/render/lot_overlay` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotOverlay.ts#LotOverlayInput` | type | `native/src/render/lot_overlay` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotOverlay.ts#LotOverlayShapes` | type | `native/src/render/lot_overlay` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotOverlay.ts#MAX_RANK` | data | `native/src/render/lot_overlay` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotOverlay.ts#RANK` | data | `native/src/render/lot_overlay` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotOverlay.ts#UNIFORMS` | render | `native/src/render` | three.js geometry goes through MeshBuilder, instances through MultiMesh grouped by area, materials through the game's shaders and tables, streaming through the render owners |  |
| `src/render/lotOverlay.ts#applyLotOverlay` | render | `native/src/render` | three.js geometry goes through MeshBuilder, instances through MultiMesh grouped by area, materials through the game's shaders and tables, streaming through the render owners |  |
| `src/render/lotOverlay.ts#createLotOverlay` | render | `native/src/render` | three.js geometry goes through MeshBuilder, instances through MultiMesh grouped by area, materials through the game's shaders and tables, streaming through the render owners | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/render/lotOverlay.ts#createLotOverlay.cellKey` | compute | `native/src/render/lot_overlay` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotOverlay.ts#createLotOverlay.cellOf` | compute | `native/src/render/lot_overlay` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotOverlay.ts#createLotOverlay.dispose` | render | `native/src/render` | three.js geometry goes through MeshBuilder, instances through MultiMesh grouped by area, materials through the game's shaders and tables, streaming through the render owners |  |
| `src/render/lotOverlay.ts#createLotOverlay.forgetLevels` | tick | `native/src/render/lot_overlay` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/render/lotOverlay.ts#createLotOverlay.leaves` | compute | `native/src/render/lot_overlay` | reaches src/render/lotOverlay.ts#createLotOverlay (render code) |  |
| `src/render/lotOverlay.ts#createLotOverlay.levelOf` | tick | `native/src/render/lot_overlay` | reaches src/render/lotOverlay.ts#createLotOverlay (render code) |  |
| `src/render/lotOverlay.ts#createLotOverlay.remake` | tick | `native/src/render/lot_overlay` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/render/lotOverlay.ts#createLotOverlay.set` | render | `native/src/render` | three.js geometry goes through MeshBuilder, instances through MultiMesh grouped by area, materials through the game's shaders and tables, streaming through the render owners | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/render/lotOverlay.ts#createLotOverlay.standFrame` | tick | `native/src/render/lot_overlay` | reaches src/render/lotLattice.ts#LotLattice (wrong methodology: hot-allocation) |  |
| `src/render/lotOverlay.ts#createLotOverlay.upload` | render | `native/src/render` | three.js geometry goes through MeshBuilder, instances through MultiMesh grouped by area, materials through the game's shaders and tables, streaming through the render owners |  |
| `src/render/lotOverlay.ts#frameHalfFor` | compute | `native/src/render/lot_overlay` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotOverlay.ts#frameUniforms` | render | `native/src/render` | three.js geometry goes through MeshBuilder, instances through MultiMesh grouped by area, materials through the game's shaders and tables, streaming through the render owners |  |
| `src/render/lotOverlay.ts#groupBox` | compute | `native/src/render/lot_overlay` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotOverlay.ts#groupBox.grow` | tick | `native/src/render/lot_overlay` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotOverlay.ts#rgba` | compute | `native/src/render/lot_overlay` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/render/lotOverlay.ts#shapesOf` | compute | `native/src/render/lot_overlay` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/render/materials.ts#createMaterials` | render | `native/src/render` | a part of shared code: it joins the code of the owner that holds the rest (port landscape says já existia, not shown: its owner "" names no file of the game) |  |
| `src/render/networkFarRoads.worker.ts#serve` | mixed | `split by responsibility` | a part of shared code: it joins the code of the owner that holds the rest | worker-copy: Transfer the ArrayBuffers (or share memory) instead of cloning them. |
| `src/render/networkFarTile.worker.ts#serve` | mixed | `split by responsibility` | a part of shared code: it joins the code of the owner that holds the rest | worker-copy: Transfer the ArrayBuffers (or share memory) instead of cloning them. |
| `src/render/people/lodWorker.ts#serve` | mixed | `split by responsibility` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/render/renderer.ts#SceneHandle` | type | `native/src/render/renderer` | a part of shared code: it joins the code of the owner that holds the rest (port landscape says portado, not shown: its owner "" names no file of the game) | scattered-writes: One owner mutates the state; everyone else sends it a command or action. |
| `src/render/renderer.ts#createSceneRenderer` | mixed | `split by responsibility` | a part of shared code: it joins the code of the owner that holds the rest (port landscape says portado, not shown: its owner "" names no file of the game) | god-function: Extract each step into a function with one responsibility, owned by the module of its concept.<br>hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous.<br>queue-shift: A ring buffer, a head index into the array, or a container with O(1) removal at the front.<br>per-object-draw: Merge static geometry or draw identical items with instancing. |
| `src/render/roadStations.worker.ts#serve` | mixed | `split by responsibility` | a part of shared code: it joins the code of the owner that holds the rest | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/render/roadTile.worker.ts#serve` | mixed | `split by responsibility` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/render/terrain.ts#terrainMaterial` | render | `native/src/render` | a part of shared code: it joins the code of the owner that holds the rest (port landscape says portado em parte, not shown: its owner "" names no file of the game) |  |
| `src/render/terrainLight.worker.ts#serve` | mixed | `split by responsibility` | a part of shared code: it joins the code of the owner that holds the rest | worker-copy: Transfer the ArrayBuffers (or share memory) instead of cloning them. |
| `src/render/uploads.ts#drainCompiles.build` | mixed | `split by responsibility` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/roadBake.worker.ts#serve` | mixed | `split by responsibility` | a part of shared code: it joins the code of the owner that holds the rest | worker-copy: Transfer the ArrayBuffers (or share memory) instead of cloning them. |
| `src/sim/agents/lorryDocks.ts#buildYard` | tick | `native/src/sim/agents` | a part of shared code: it joins the code of the owner that holds the rest | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/sim/agents/lorryDocks.ts#lotsNear` | compute | `native/src/sim/agents` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/sim/agents/lorryDocks.ts#ownLot` | compute | `native/src/sim/agents` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/sim/agents/lorryDocks.ts#yardInputs` | compute | `native/src/sim/agents` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/sim/agents/trafficBuildings.ts#takeTrafficMap` | compute | `native/src/sim/agents` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/sim/agents/trafficBuildings.ts#trafficMapOf` | compute | `native/src/sim/agents` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/sim/host/simClient.ts#SimClient.keyChanges` | tick | `native/src/sim/host` | a part of shared code: it joins the code of the owner that holds the rest | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/sim/host/simClient.ts#SimClient.lotPlan` | tick | `native/src/sim/host` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/sim/host/simClient.ts#SimClient.markSent` | tick | `native/src/sim/host` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/sim/host/simClient.ts#SimClient.sendChanges` | tick | `native/src/sim/host` | a part of shared code: it joins the code of the owner that holds the rest | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/sim/host/simHost.ts#SimHost.lotPlan` | compute | `native/src/sim/host` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/sim/host/simViews.ts#recordOf` | compute | `native/src/sim/host` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/world/buildings/architecture.ts#ArchStyle` | type | `native/src/sim/buildings` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/buildings/architecture.ts#Composition` | type | `native/src/sim/buildings` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/buildings/architecture.ts#ERA_AFFINITY` | data | `native/src/sim/buildings` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/buildings/architecture.ts#Era` | type | `native/src/sim/buildings` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/buildings/architecture.ts#FOR` | data | `native/src/sim/buildings` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/buildings/architecture.ts#GroundRole` | type | `native/src/sim/buildings` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/buildings/architecture.ts#STYLES` | data | `native/src/sim/buildings` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/buildings/architecture.ts#StyleKey` | type | `native/src/sim/buildings` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/buildings/architecture.ts#StylePaints` | type | `native/src/sim/buildings` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/buildings/architecture.ts#between` | compute | `native/src/sim/buildings` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/buildings/architecture.ts#columnComponent` | compute | `native/src/sim/buildings` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/buildings/architecture.ts#columnsFor` | compute | `native/src/sim/buildings` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/buildings/architecture.ts#composeFacades` | tick | `native/src/sim/buildings` | wrong methodology: hot-allocation, object-delete | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous.<br>object-delete: A Map for dynamic keys, or a fixed shape with the slot set to undefined. |
| `src/world/buildings/architecture.ts#paintsOf` | compute | `native/src/sim/buildings` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/buildings/architecture.ts#paintsOf.light` | compute | `native/src/sim/buildings` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/buildings/architecture.ts#pick` | compute | `native/src/sim/buildings` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/buildings/architecture.ts#stoneBaseOf` | compute | `native/src/sim/buildings` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/buildings/architecture.ts#stoneBaseOf.ch` | compute | `native/src/sim/buildings` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/buildings/architecture.ts#styleFor` | compute | `native/src/sim/buildings` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/buildings/architecture.ts#tint` | compute | `native/src/sim/buildings` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/buildings/architecture.ts#tint.ch` | compute | `native/src/sim/buildings` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/buildings/architecture.ts#tones` | compute | `native/src/sim/buildings` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/buildings/elements.ts#elementsMeet` | compute | `native/src/sim/buildings` | reaches src/world/buildings/types.ts#BuildingElement (wrong methodology: scattered-writes) |  |
| `src/world/buildings/elements.ts#foliage` | compute | `native/src/sim/buildings` | reaches src/world/buildings/types.ts#BuildingElement (wrong methodology: scattered-writes) |  |
| `src/world/buildings/elements.ts#footprintOf` | tick | `native/src/sim/buildings` | reaches src/world/buildings/types.ts#BuildingElement (wrong methodology: scattered-writes) |  |
| `src/world/buildings/elements.ts#heightsMeet` | compute | `native/src/sim/buildings` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/buildings/elements.ts#isEarthwork` | compute | `native/src/sim/buildings` | reaches src/world/buildings/types.ts#BuildingElement (wrong methodology: scattered-writes) |  |
| `src/world/buildings/elements.ts#isStructure` | compute | `native/src/sim/buildings` | reaches src/world/buildings/types.ts#BuildingElement (wrong methodology: scattered-writes) |  |
| `src/world/buildings/elements.ts#planDepth` | compute | `native/src/sim/buildings` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/buildings/elements.ts#planReach` | compute | `native/src/sim/buildings` | reaches src/world/buildings/types.ts#BuildingElement (wrong methodology: scattered-writes) |  |
| `src/world/buildings/elements.ts#reachAlong` | compute | `native/src/sim/buildings` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/buildings/elements.ts#thinRun` | compute | `native/src/sim/buildings` | reaches src/world/buildings/types.ts#BuildingElement (wrong methodology: scattered-writes) |  |
| `src/world/buildings/items.ts#TableItems.clear` | tick | `native/src/sim/buildings` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/world/buildings/items.ts#TableItems.delete` | tick | `native/src/sim/buildings` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/world/buildings/items.ts#TableItems.eachBox` | compute | `native/src/sim/buildings` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/world/buildings/items.ts#TableItems.eachBox.take` | compute | `native/src/sim/buildings` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/world/buildings/items.ts#TableItems.eachInOrder` | compute | `native/src/sim/buildings` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/buildings/items.ts#TableItems.rowOrder` | compute | `native/src/sim/buildings` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/buildings/items.ts#TableItems.set` | tick | `native/src/sim/buildings` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/world/buildings/items.ts#TableItems.size` | compute | `native/src/sim/buildings` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/buildings/items.ts#TableItems.writeField` | tick | `native/src/sim/buildings` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/world/buildings/lotTemplate.ts#LotTemplate` | type | `native/src/sim/buildings` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/world/buildings/neighbours.ts#RecordNeighbour.bounds` | compute | `native/src/sim/buildings` | reaches src/world/buildings/geometry.ts#buildingBounds (wrong methodology: hot-allocation) |  |
| `src/world/buildings/neighbours.ts#RecordNeighbour.footprints` | compute | `native/src/sim/buildings` | reaches src/world/buildings/neighbours.ts#groundOf (wrong methodology: hot-allocation) |  |
| `src/world/buildings/neighbours.ts#RecordNeighbour.projections` | compute | `native/src/sim/buildings` | reaches src/world/buildings/neighbours.ts#groundOf (wrong methodology: hot-allocation) |  |
| `src/world/buildings/neighbours.ts#RecordNeighbour.signature` | compute | `native/src/sim/buildings` | reaches src/world/buildings/types.ts#Building (wrong methodology: scattered-writes) |  |
| `src/world/buildings/neighbours.ts#RecordNeighbour.storeys` | tick | `native/src/sim/buildings` | reaches src/world/buildings/types.ts#Building (wrong methodology: scattered-writes) |  |
| `src/world/buildings/neighbours.ts#RecordNeighbour.top` | state | `native/src/sim/buildings` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/buildings/neighbours.ts#settledNeighbour` | compute | `native/src/sim/buildings` | reaches src/world/buildings/neighbours.ts#groundOf (wrong methodology: hot-allocation) |  |
| `src/world/buildings/neighbours.ts#storeNeighbours.near` | compute | `native/src/sim/buildings` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/world/buildings/neighbours.ts#storeysOf` | compute | `native/src/sim/buildings` | reaches src/world/buildings/types.ts#Building (wrong methodology: scattered-writes) |  |
| `src/world/buildings/procedural.ts#Envelope` | type | `native/src/sim/buildings` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/buildings/procedural.ts#HouseForm` | type | `native/src/sim/buildings` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/buildings/procedural.ts#MadeBuilding` | type | `native/src/sim/buildings` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/buildings/procedural.ts#Rect` | type | `native/src/sim/buildings` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/buildings/procedural.ts#backDoorOn` | tick | `native/src/sim/buildings` | reaches src/world/buildings/types.ts#Facade (wrong methodology: scattered-writes) |  |
| `src/world/buildings/procedural.ts#baysOf` | compute | `native/src/sim/buildings` | reaches src/world/buildings/types.ts#FacadeGeometry (wrong methodology: scattered-writes) |  |
| `src/world/buildings/procedural.ts#between` | compute | `native/src/sim/buildings` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/buildings/procedural.ts#doorAt` | tick | `native/src/sim/buildings` | reaches src/world/buildings/types.ts#Facade (wrong methodology: scattered-writes) |  |
| `src/world/buildings/procedural.ts#dressBuilding` | tick | `native/src/sim/buildings` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/buildings/procedural.ts#finish` | tick | `native/src/sim/buildings` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/buildings/procedural.ts#flats` | tick | `native/src/sim/buildings` | wrong methodology: god-function | god-function: Extract each step into a function with one responsibility, owned by the module of its concept. |
| `src/world/buildings/procedural.ts#half` | compute | `native/src/sim/buildings` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/buildings/procedural.ts#hang` | tick | `native/src/sim/buildings` | reaches src/world/buildings/elements.ts#elementClash (wrong methodology: hot-allocation) |  |
| `src/world/buildings/procedural.ts#house` | tick | `native/src/sim/buildings` | wrong methodology: god-function, hot-allocation | god-function: Extract each step into a function with one responsibility, owned by the module of its concept.<br>hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/buildings/procedural.ts#houseRoof` | compute | `native/src/sim/buildings` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/buildings/procedural.ts#lightness` | compute | `native/src/sim/buildings` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/buildings/procedural.ts#madeToMeasure` | compute | `native/src/sim/buildings` | reaches src/world/buildings/procedural.ts#flats (wrong methodology: god-function) |  |
| `src/world/buildings/procedural.ts#nudge` | compute | `native/src/sim/buildings` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/buildings/procedural.ts#nudge.ch` | compute | `native/src/sim/buildings` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/buildings/procedural.ts#office` | compute | `native/src/sim/buildings` | reaches src/world/buildings/architecture.ts#styleFor (wrong methodology: hot-allocation) |  |
| `src/world/buildings/procedural.ts#otherSkin` | compute | `native/src/sim/buildings` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/buildings/procedural.ts#pickOf` | compute | `native/src/sim/buildings` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/buildings/procedural.ts#roofOf` | compute | `native/src/sim/buildings` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/buildings/procedural.ts#roofShape` | tick | `native/src/sim/buildings` | wrong methodology: object-delete | object-delete: A Map for dynamic keys, or a fixed shape with the slot set to undefined. |
| `src/world/buildings/procedural.ts#setbacks` | compute | `native/src/sim/buildings` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/buildings/procedural.ts#shop` | tick | `native/src/sim/buildings` | reaches src/world/buildings/architecture.ts#styleFor (wrong methodology: hot-allocation) |  |
| `src/world/buildings/procedural.ts#signatureDistance` | compute | `native/src/sim/buildings` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/buildings/procedural.ts#towerShape` | compute | `native/src/sim/buildings` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/buildings/procedural.ts#works` | tick | `native/src/sim/buildings` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/buildings/splitLevel.ts#SlopeSite` | type | `native/src/sim/buildings` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/buildings/splitLevel.ts#SplitLevel` | type | `native/src/sim/buildings` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/buildings/splitLevel.ts#clone` | compute | `native/src/sim/buildings` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/buildings/splitLevel.ts#doorOn` | compute | `native/src/sim/buildings` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/buildings/splitLevel.ts#garageUnder` | tick | `native/src/sim/buildings` | wrong methodology: hot-allocation | holey-array: An array literal filled with push, or a typed array of length n for numbers.<br>hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/buildings/splitLevel.ts#isAccess` | compute | `native/src/sim/buildings` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/buildings/splitLevel.ts#snap` | compute | `native/src/sim/buildings` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/buildings/splitLevel.ts#splitBlock` | tick | `native/src/sim/buildings` | wrong methodology: hot-allocation, object-delete | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous.<br>object-delete: A Map for dynamic keys, or a fixed shape with the slot set to undefined. |
| `src/world/buildings/splitLevel.ts#splitBlock.keep` | tick | `native/src/sim/buildings` | wrong methodology: hot-allocation, object-delete | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous.<br>object-delete: A Map for dynamic keys, or a fixed shape with the slot set to undefined. |
| `src/world/buildings/splitLevel.ts#stepToSlope` | tick | `native/src/sim/buildings` | wrong methodology: god-function, hot-allocation, object-delete, queue-shift | god-function: Extract each step into a function with one responsibility, owned by the module of its concept.<br>hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous.<br>object-delete: A Map for dynamic keys, or a fixed shape with the slot set to undefined.<br>queue-shift: A ring buffer, a head index into the array, or a container with O(1) removal at the front. |
| `src/world/buildings/splitLevel.ts#stepToSlope.backDoor` | compute | `native/src/sim/buildings` | reaches src/world/buildings/types.ts#Facade (wrong methodology: scattered-writes) |  |
| `src/world/buildings/splitLevel.ts#stepToSlope.blind` | tick | `native/src/sim/buildings` | wrong methodology: hot-allocation, object-delete | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous.<br>object-delete: A Map for dynamic keys, or a fixed shape with the slot set to undefined. |
| `src/world/buildings/splitLevel.ts#stepToSlope.costAt` | compute | `native/src/sim/buildings` | reaches src/world/buildings/splitLevel.ts#stepToSlope (wrong methodology: god-function, hot-allocation, object-delete, queue-shift) |  |
| `src/world/buildings/splitLevel.ts#stepToSlope.onEdge` | compute | `native/src/sim/buildings` | reaches src/world/buildings/splitLevel.ts#stepToSlope (wrong methodology: god-function, hot-allocation, object-delete, queue-shift) |  |
| `src/world/buildings/splitLevel.ts#withoutBays` | tick | `native/src/sim/buildings` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/buildings/store.ts#BuildingStore.watch` | tick | `native/src/sim/buildings` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/world/buildings/store.ts#BuildingStore.withWatch` | tick | `native/src/sim/buildings` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/world/buildings/tableRows.ts#TableRows.rankOf` | tick | `native/src/sim/buildings` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/buildings/tableRows.ts#TableRows.ranks` | state | `native/src/sim/buildings` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/buildings/validate.ts#ringOf` | data | `native/src/sim/buildings` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/world/buildings/validate.ts#siteOf` | compute | `native/src/sim/buildings` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/world/buildings/validate.ts#touchesRoad` | compute | `native/src/sim/buildings` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/world/buildings/validate.ts#validateOn` | compute | `native/src/sim/buildings` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/world/cityGen/plan.ts#CityPlan` | type | `native/src/sim/city_gen` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/world/cityGen/plan.ts#planCity.zoneAt` | compute | `native/src/sim/city_gen` | a part of shared code: it joins the code of the owner that holds the rest (port landscape says já existia, not shown: its owner "" names no file of the game) |  |
| `src/world/cityGen/squares.ts#blocks` | compute | `native/src/sim/city_gen` | a part of shared code: it joins the code of the owner that holds the rest | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous.<br>duplicated-code: Extract the shared code into one owner and call it from both places. |
| `src/world/cityGen/squares.ts#blocks.near` | compute | `native/src/sim/city_gen` | a part of shared code: it joins the code of the owner that holds the rest | duplicated-code: Extract the shared code into one owner and call it from both places. |
| `src/world/cityGen/squares.ts#planSquares` | compute | `native/src/sim/city_gen` | a part of shared code: it joins the code of the owner that holds the rest (port landscape says já existia, not shown: its owner "" names no file of the game) | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/conflictPoints.ts#ConflictIndex.add` | tick | `native/src/sim/conflict_points` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/world/conflictPoints.ts#zoneCentre` | compute | `native/src/sim/conflict_points` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/world/conflictTable.ts#clearExits` | compute | `native/src/sim/conflict_table` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/world/conflictTable.ts#earliestFront` | compute | `native/src/sim/conflict_table` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/world/conflictTable.ts#pairFacts` | compute | `native/src/sim/conflict_table` | a part of shared code: it joins the code of the owner that holds the rest | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/conflictTable.ts#pushDiverge` | compute | `native/src/sim/conflict_table` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/world/doc.ts#RoadDoc.constructor` | tick | `native/src/sim/doc` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/world/doc.ts#RoadDoc.fromJSON` | tick | `native/src/sim/doc` | a part of shared code: it joins the code of the owner that holds the rest (port landscape says portado, not shown: its owner "" names no file of the game) | god-function: Extract each step into a function with one responsibility, owned by the module of its concept.<br>hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/doc.ts#RoadDoc.layOver` | tick | `native/src/sim/doc` | a part of shared code: it joins the code of the owner that holds the rest (port landscape says portado, not shown: its owner "" names no file of the game) | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/doc.ts#RoadDoc.listOf` | compute | `native/src/sim/doc` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/world/doc.ts#RoadDoc.listen` | compute | `native/src/sim/doc` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/world/doc.ts#RoadDoc.listen.tell` | tick | `native/src/sim/doc` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/world/doc.ts#RoadDoc.loadLandscapeSections` | tick | `native/src/sim/doc` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/world/doc.ts#RoadDoc.loadPoleSections.over` | tick | `native/src/sim/doc` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/world/doc.ts#RoadDoc.lot` | compute | `native/src/sim/doc` | reaches src/world/lotStore.ts#LotStore (wrong methodology: hot-allocation) |  |
| `src/world/doc.ts#RoadDoc.lotAt` | compute | `native/src/sim/doc` | reaches src/world/lotStore.ts#LotStore.at (wrong methodology: hot-allocation) |  |
| `src/world/doc.ts#RoadDoc.lotIdsNear` | compute | `native/src/sim/doc` | reaches src/world/lotStore.ts#LotStore (wrong methodology: hot-allocation) |  |
| `src/world/doc.ts#RoadDoc.lotStore` | state | `native/src/sim/doc` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/world/doc.ts#RoadDoc.lots` | compute | `native/src/sim/doc` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/world/doc.ts#RoadDoc.lotsIn` | compute | `native/src/sim/doc` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/world/doc.ts#RoadDoc.lotsMovedIn` | compute | `native/src/sim/doc` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/world/doc.ts#RoadDoc.nextLotId` | compute | `native/src/sim/doc` | reaches src/world/lotStore.ts#LotStore (wrong methodology: hot-allocation) |  |
| `src/world/doc.ts#RoadDoc.openLots` | tick | `native/src/sim/doc` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/world/doc.ts#RoadDoc.openLotsIndex` | state | `native/src/sim/doc` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/world/doc.ts#RoadDoc.ownList` | tick | `native/src/sim/doc` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/world/doc.ts#RoadDoc.recordOf` | compute | `native/src/sim/doc` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/world/doc.ts#RoadDoc.replaceContents` | tick | `native/src/sim/doc` | a part of shared code: it joins the code of the owner that holds the rest (port landscape says portado, not shown: its owner "" names no file of the game) | god-function: Extract each step into a function with one responsibility, owned by the module of its concept.<br>hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/doc.ts#RoadDoc.restore` | tick | `native/src/sim/doc` | a part of shared code: it joins the code of the owner that holds the rest (port landscape says portado, not shown: its owner "" names no file of the game) | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/doc.ts#RoadDoc.restoreValue` | tick | `native/src/sim/doc` | a part of shared code: it joins the code of the owner that holds the rest (port landscape says portado, not shown: its owner "" names no file of the game) |  |
| `src/world/doc.ts#RoadDoc.savedGlobals` | compute | `native/src/sim/doc` | a part of shared code: it joins the code of the owner that holds the rest (port landscape says portado, not shown: its owner "" names no file of the game) |  |
| `src/world/doc.ts#RoadDoc.serialized` | compute | `native/src/sim/doc` | a part of shared code: it joins the code of the owner that holds the rest (port landscape says portado, not shown: its owner "" names no file of the game) | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/doc.ts#RoadDoc.tableOf` | compute | `native/src/sim/doc` | a part of shared code: it joins the code of the owner that holds the rest | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/doc.ts#RoadDoc.tableSections` | compute | `native/src/sim/doc` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/world/doc.ts#RoadDoc.takeFork` | tick | `native/src/sim/doc` | a part of shared code: it joins the code of the owner that holds the rest (port landscape says portado, not shown: its owner "" names no file of the game) | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/doc.ts#RoadDoc.takeLandscapeFrom` | tick | `native/src/sim/doc` | a part of shared code: it joins the code of the owner that holds the rest | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/doc.ts#RoadDoc.takePolesFrom.taken` | tick | `native/src/sim/doc` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/world/doc.ts#RoadDoc.tileIndex` | state | `native/src/sim/doc` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/world/doc.ts#RoadDoc.valueOf` | compute | `native/src/sim/doc` | a part of shared code: it joins the code of the owner that holds the rest (port landscape says portado, not shown: its owner "" names no file of the game) |  |
| `src/world/doc.ts#RoadDoc.worldPart` | compute | `native/src/sim/doc` | a part of shared code: it joins the code of the owner that holds the rest (port landscape says portado, not shown: its owner "" names no file of the game) | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/doc.ts#RoadDoc.zoneMarks` | state | `native/src/sim/doc` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/world/doc.ts#RoadDoc.zones` | state | `native/src/sim/doc` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/world/doc.ts#SerializedDoc` | type | `native/src/sim/doc` | a part of shared code: it joins the code of the owner that holds the rest (port landscape says portado, not shown: its owner "" names no file of the game) |  |
| `src/world/doc.ts#TileRecords` | type | `native/src/sim/doc` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/world/elevation.ts#buildRoadElevation` | tick | `native/src/sim/elevation` | a part of shared code: it joins the code of the owner that holds the rest | god-function: Extract each step into a function with one responsibility, owned by the module of its concept.<br>nondeterminism-in-simulation: Fixed timestep driven by an accumulator, seeded random streams owned by the simulation.<br>hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/elevation.ts#solveGround` | compute | `native/src/sim/elevation` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/world/elevation.ts#solveRaised` | compute | `native/src/sim/elevation` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/world/elevation.ts#solveSunken` | compute | `native/src/sim/elevation` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/world/elevation.ts#solveVariable` | compute | `native/src/sim/elevation` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/world/footwayPath.ts#nearestFootway` | compute | `native/src/sim/footway_path` | a part of shared code: it joins the code of the owner that holds the rest | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous.<br>quadratic-proximity: A spatial partition: uniform grid or spatial hash, querying only nearby cells. |
| `src/world/groundPatch.ts#GroundPatch` | type | `native/src/sim/ground_patch` | computes or edits; nothing wrong in it or in what it reaches (port landscape says já existia, not shown: its owner "" names no file of the game) |  |
| `src/world/groundPatch.ts#PatchMiss` | state | `native/src/sim/ground_patch` | computes or edits; nothing wrong in it or in what it reaches (port landscape says já existia, not shown: its owner "" names no file of the game) |  |
| `src/world/groundPatch.ts#latticeHeight` | compute | `native/src/sim/ground_patch` | reaches src/world/groundPatch.ts#cellDiagonal, which the game already has in landscape: name it in the conversion library (port landscape says já existia, not shown: its owner "" names no file of the game) |  |
| `src/world/groundPatch.ts#patchGround` | compute | `native/src/sim/ground_patch` | reaches src/world/groundPatch.ts#latticeHeight, which the game already has in landscape: name it in the conversion library (port landscape says já existia, not shown: its owner "" names no file of the game) |  |
| `src/world/groundPatch.ts#sampleGroundPatch` | compute | `native/src/sim/ground_patch` | computes or edits; nothing wrong in it or in what it reaches (port landscape says já existia, not shown: its owner "" names no file of the game) |  |
| `src/world/growth/lotBoundaryRuns.ts#CoverBox` | type | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/lotBoundaryRuns.ts#FIRST,STEP` | data | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/lotBoundaryRuns.ts#firstWhere` | compute | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/lotBoundaryRuns.ts#openRunsAlong` | compute | `native/src/sim/growth` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/growth/lotPlan.ts#ALLEY_COBBLES` | data | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/lotPlan.ts#BOUNDARY_RESERVE` | data | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/lotPlan.ts#BOUNDARY_SET` | data | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/lotPlan.ts#BackUse` | type | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/lotPlan.ts#CONCRETE_PATH` | data | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/lotPlan.ts#DOCK` | data | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/lotPlan.ts#DRIVE,HOUSE_DRIVE,FOOTPATH,TRUCK_LANE` | data | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/lotPlan.ts#FLAT_KINDS` | data | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/lotPlan.ts#FrontUse` | type | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/lotPlan.ts#GARAGE_APRON` | data | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/lotPlan.ts#GARAGE_GRADE` | data | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/lotPlan.ts#Lot` | type | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/lotPlan.ts#LotKind` | type | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/lotPlan.ts#LotPlan` | type | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/lotPlan.ts#PARK_SLOPE` | data | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/lotPlan.ts#PAVERS` | data | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/lotPlan.ts#PAVERS_WARM` | data | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/lotPlan.ts#RAMP_LANDING` | data | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/lotPlan.ts#RETAINING` | data | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/lotPlan.ts#STALL,AISLE` | data | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/lotPlan.ts#STEP_RISE_M` | data | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/lotPlan.ts#STEP_RUN_M` | data | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/lotPlan.ts#STONE_PATH` | data | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/lotPlan.ts#SidePlan` | type | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/lotPlan.ts#SideUse` | type | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/lotPlan.ts#TERRACE_MAX_RISERS` | data | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/lotPlan.ts#TERRACE_MIN` | data | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/lotPlan.ts#YARD_FULL` | data | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/lotPlan.ts#apart` | compute | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/lotPlan.ts#between` | compute | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/lotPlan.ts#driveSide` | compute | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/lotPlan.ts#furnishLot` | tick | `native/src/sim/growth` | wrong methodology: god-function, hot-allocation | god-function: Extract each step into a function with one responsibility, owned by the module of its concept.<br>hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/growth/lotPlan.ts#furnishLot.X,Y` | compute | `native/src/sim/growth` | reaches src/world/growth/lotPlan.ts#furnishLot (wrong methodology: god-function, hot-allocation) |  |
| `src/world/growth/lotPlan.ts#furnishLot.flightBack` | tick | `native/src/sim/growth` | reaches src/world/growth/lotPlan.ts#furnishLot (wrong methodology: god-function, hot-allocation) |  |
| `src/world/growth/lotPlan.ts#furnishLot.frontAt` | compute | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/lotPlan.ts#furnishLot.frontRun` | compute | `native/src/sim/growth` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/growth/lotPlan.ts#furnishLot.furnishFree` | compute | `native/src/sim/growth` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/growth/lotPlan.ts#furnishLot.isCorner` | compute | `native/src/sim/growth` | reaches src/world/growth/lotPlan.ts#furnishLot (wrong methodology: god-function, hot-allocation) |  |
| `src/world/growth/lotPlan.ts#furnishLot.lift` | tick | `native/src/sim/growth` | reaches src/world/buildings/types.ts#Building (wrong methodology: scattered-writes) |  |
| `src/world/growth/lotPlan.ts#furnishLot.near` | compute | `native/src/sim/growth` | reaches src/world/buildings/types.ts#BuildingElement (wrong methodology: scattered-writes) |  |
| `src/world/growth/lotPlan.ts#furnishLot.piece` | tick | `native/src/sim/growth` | reaches src/world/growth/lotPlan.ts#furnishLot (wrong methodology: god-function, hot-allocation) |  |
| `src/world/growth/lotPlan.ts#furnishLot.probe` | compute | `native/src/sim/growth` | reaches src/world/growth/lotPlan.ts#furnishLot (wrong methodology: god-function, hot-allocation) |  |
| `src/world/growth/lotPlan.ts#furnishLot.reachOf` | tick | `native/src/sim/growth` | reaches src/world/growth/lotPlan.ts#furnishLot (wrong methodology: god-function, hot-allocation) |  |
| `src/world/growth/lotPlan.ts#furnishLot.retain` | compute | `native/src/sim/growth` | reaches src/world/growth/lotPlan.ts#furnishLot (wrong methodology: god-function, hot-allocation) |  |
| `src/world/growth/lotPlan.ts#furnishLot.stepYard` | tick | `native/src/sim/growth` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/growth/lotPlan.ts#furnishLot.stepYard.clear` | compute | `native/src/sim/growth` | reaches src/world/buildings/types.ts#BuildingElement (wrong methodology: scattered-writes) |  |
| `src/world/growth/lotPlan.ts#furnishLot.stepYard.flightClear` | compute | `native/src/sim/growth` | reaches src/world/growth/lotPlan.ts#furnishLot (wrong methodology: god-function, hot-allocation) |  |
| `src/world/growth/lotPlan.ts#furnishLot.stepYard.inYard` | compute | `native/src/sim/growth` | reaches src/world/growth/lotPlan.ts#furnishLot.stepYard (wrong methodology: hot-allocation) |  |
| `src/world/growth/lotPlan.ts#furnishLot.streetSide` | compute | `native/src/sim/growth` | reaches src/world/growth/lotPlan.ts#furnishLot (wrong methodology: god-function, hot-allocation) |  |
| `src/world/growth/lotPlan.ts#furnishLot.streetSide.at` | compute | `native/src/sim/growth` | reaches src/world/growth/lotPlan.ts#furnishLot (wrong methodology: god-function, hot-allocation) |  |
| `src/world/growth/lotPlan.ts#furnishLot.strip` | compute | `native/src/sim/growth` | reaches src/world/growth/lotPlan.ts#furnishLot (wrong methodology: god-function, hot-allocation) |  |
| `src/world/growth/lotPlan.ts#garageRamp` | compute | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/lotPlan.ts#half` | compute | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/lotPlan.ts#loadingDoors` | compute | `native/src/sim/growth` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/growth/lotPlan.ts#lotKind` | compute | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/lotPlan.ts#planLot` | compute | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/lotPlan.ts#planLot.parkingRows` | compute | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/lotPlan.ts#planLot.sides` | compute | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/lotPlan.ts#yardRamp` | compute | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/version.ts#LOT_PLAN_VERSION` | data | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/zoning.ts#BOUNDARY_KINDS` | data | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/zoning.ts#BOUNDARY_PARTS` | data | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/zoning.ts#CANDIDATES` | data | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/zoning.ts#GROW_ON_REACH` | data | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/zoning.ts#GrownChoice` | type | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/zoning.ts#GrowthSite` | type | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/zoning.ts#JOINT` | data | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/zoning.ts#LIKE_REACH` | data | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/zoning.ts#LOT_READ_MARGIN` | data | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/zoning.ts#LotPick` | type | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/zoning.ts#STREET_MARGIN` | data | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/zoning.ts#TRIES` | data | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/zoning.ts#Tried` | type | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/zoning.ts#TrySite` | type | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/zoning.ts#apart` | compute | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/zoning.ts#commitGrown` | tick | `native/src/sim/growth` | reaches src/world/buildings/types.ts#Building (wrong methodology: scattered-writes) |  |
| `src/world/growth/zoning.ts#cornerOf` | compute | `native/src/sim/growth` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/growth/zoning.ts#cornerOf.onStreet` | compute | `native/src/sim/growth` | reaches src/world/growth/zoning.ts#cornerOf (wrong methodology: hot-allocation) |  |
| `src/world/growth/zoning.ts#densitiesOf` | compute | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/zoning.ts#designOnLot` | tick | `native/src/sim/growth` | wrong methodology: hot-allocation, object-delete | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous.<br>object-delete: A Map for dynamic keys, or a fixed shape with the slot set to undefined. |
| `src/world/growth/zoning.ts#designOnLot.floorOf` | compute | `native/src/sim/growth` | reaches src/world/growth/zoning.ts#designOnLot (wrong methodology: hot-allocation, object-delete) |  |
| `src/world/growth/zoning.ts#fitToLot` | compute | `native/src/sim/growth` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/growth/zoning.ts#fitToLot.asOutline` | tick | `native/src/sim/growth` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/growth/zoning.ts#fitToLot.clip` | compute | `native/src/sim/growth` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/growth/zoning.ts#fitToLot.inside` | compute | `native/src/sim/growth` | reaches src/world/growth/zoning.ts#fitToLot (wrong methodology: hot-allocation) |  |
| `src/world/growth/zoning.ts#fitToLot.insideAt` | tick | `native/src/sim/growth` | reaches src/world/growth/zoning.ts#fitToLot (wrong methodology: hot-allocation) |  |
| `src/world/growth/zoning.ts#fitToLot.sound` | compute | `native/src/sim/growth` | reaches src/world/buildings/types.ts#Volume (wrong methodology: scattered-writes) |  |
| `src/world/growth/zoning.ts#frameOf` | compute | `native/src/sim/growth` | reaches src/world/lots.ts#lotBuildFrame (wrong methodology: hot-allocation) |  |
| `src/world/growth/zoning.ts#growOnLot` | compute | `native/src/sim/growth` | reaches src/world/growth/zoning.ts#designOnLot (wrong methodology: hot-allocation, object-delete) |  |
| `src/world/growth/zoning.ts#growthReachOf` | compute | `native/src/sim/growth` | reaches src/world/lots.ts#lotBuildFrame (wrong methodology: hot-allocation) |  |
| `src/world/growth/zoning.ts#growthSiteOf` | compute | `native/src/sim/growth` | reaches src/world/buildings/store.ts#BuildingStore (wrong methodology: scattered-writes, hot-allocation) |  |
| `src/world/growth/zoning.ts#lotStream` | compute | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/zoning.ts#meets` | compute | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/zoning.ts#metresOf` | compute | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/zoning.ts#nearSignatures` | compute | `native/src/sim/growth` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/growth/zoning.ts#nearestLot` | compute | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/zoning.ts#neighbourStoreys` | compute | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/zoning.ts#pickLot` | compute | `native/src/sim/growth` | reaches src/world/openLots.ts#OpenLots.near (wrong methodology: hot-allocation) |  |
| `src/world/growth/zoning.ts#placed` | compute | `native/src/sim/growth` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/growth/zoning.ts#quarterOf` | compute | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/zoning.ts#quarterOf.hash` | compute | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/zoning.ts#reachStreet` | tick | `native/src/sim/growth` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/growth/zoning.ts#reachStreet.lay` | tick | `native/src/sim/growth` | reaches src/world/growth/zoning.ts#reachStreet (wrong methodology: hot-allocation) |  |
| `src/world/growth/zoning.ts#rectInside` | compute | `native/src/sim/growth` | reaches src/world/buildings/types.ts#Volume (wrong methodology: scattered-writes) |  |
| `src/world/growth/zoning.ts#streetFloor` | compute | `native/src/sim/growth` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/growth/zoning.ts#streetPointsOf` | compute | `native/src/sim/growth` | reaches src/world/growth/zoning.ts#trySites (wrong methodology: hot-allocation) |  |
| `src/world/growth/zoning.ts#tryOnLot` | tick | `native/src/sim/growth` | wrong methodology: god-function, hot-allocation | god-function: Extract each step into a function with one responsibility, owned by the module of its concept.<br>hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/growth/zoning.ts#trySites` | compute | `native/src/sim/growth` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/growth/zoning.ts#trySites.local` | compute | `native/src/sim/growth` | reaches src/world/growth/zoning.ts#trySites (wrong methodology: hot-allocation) |  |
| `src/world/growth/zoning.ts#unlikeness` | compute | `native/src/sim/growth` | reaches src/world/buildings/types.ts#Building (wrong methodology: scattered-writes) |  |
| `src/world/landscape.ts#snapLandscape.others` | compute | `native/src/sim/landscape` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/world/lotRecord.ts#Lot` | type | `native/src/sim/lot_record` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lotRecord.ts#insideLot` | compute | `native/src/sim/lot_record` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lotRecord.ts#lotRecord` | compute | `native/src/sim/lot_record` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/lotRings.ts#hull` | compute | `native/src/sim/lot_rings` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lotRings.ts#hull.cross` | compute | `native/src/sim/lot_rings` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lotRings.ts#lotBounds` | compute | `native/src/sim/lot_rings` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lotRings.ts#lotCentre` | compute | `native/src/sim/lot_rings` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lotRings.ts#orientedBox` | compute | `native/src/sim/lot_rings` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/lotRings.ts#simplifyRing` | compute | `native/src/sim/lot_rings` | wrong methodology: quadratic-proximity | quadratic-proximity: A spatial partition: uniform grid or spatial hash, querying only nearby cells. |
| `src/world/lotStore.ts#LOT_CELL` | data | `native/src/sim/lot_store` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lotStore.ts#LOT_REACH` | data | `native/src/sim/lot_store` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lotStore.ts#LotEdit` | type | `native/src/sim/lot_store` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lotStore.ts#LotStore` | state | `native/src/sim/lot_store` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/lotStore.ts#LotStore.addKeys` | tick | `native/src/sim/lot_store` | reaches src/world/changes.ts#ChangeJournal.record (wrong methodology: nondeterminism-in-simulation) |  |
| `src/world/lotStore.ts#LotStore.all` | compute | `native/src/sim/lot_store` | reaches src/world/buildings/items.ts#TableItems (wrong methodology: scattered-writes, hot-allocation) |  |
| `src/world/lotStore.ts#LotStore.append` | tick | `native/src/sim/lot_store` | reaches src/world/buildings/items.ts#TableItems (wrong methodology: scattered-writes, hot-allocation) |  |
| `src/world/lotStore.ts#LotStore.apply` | compute | `native/src/sim/lot_store` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/lotStore.ts#LotStore.at` | compute | `native/src/sim/lot_store` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/lotStore.ts#LotStore.beforeKeys` | state | `native/src/sim/lot_store` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lotStore.ts#LotStore.below` | state | `native/src/sim/lot_store` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lotStore.ts#LotStore.cells` | tick | `native/src/sim/lot_store` | reaches src/world/buildings/items.ts#TableItems (wrong methodology: scattered-writes, hot-allocation) |  |
| `src/world/lotStore.ts#LotStore.centreOf` | compute | `native/src/sim/lot_store` | reaches src/world/buildings/items.ts#TableItems (wrong methodology: scattered-writes, hot-allocation) |  |
| `src/world/lotStore.ts#LotStore.changedFor` | compute | `native/src/sim/lot_store` | reaches src/world/changes.ts#ChangeJournal.since (wrong methodology: hot-allocation) |  |
| `src/world/lotStore.ts#LotStore.changedSince` | compute | `native/src/sim/lot_store` | reaches src/world/changes.ts#ChangeJournal.since (wrong methodology: hot-allocation) |  |
| `src/world/lotStore.ts#LotStore.constructor` | compute | `native/src/sim/lot_store` | reaches src/world/changes.ts#ChangeJournal (wrong methodology: nondeterminism-in-simulation, hot-allocation) |  |
| `src/world/lotStore.ts#LotStore.cursor` | compute | `native/src/sim/lot_store` | reaches src/world/changes.ts#ChangeJournal.since (wrong methodology: hot-allocation) |  |
| `src/world/lotStore.ts#LotStore.drop` | tick | `native/src/sim/lot_store` | reaches src/world/buildings/items.ts#TableItems (wrong methodology: scattered-writes, hot-allocation) |  |
| `src/world/lotStore.ts#LotStore.dropKeys` | tick | `native/src/sim/lot_store` | reaches src/world/changes.ts#ChangeJournal.record (wrong methodology: nondeterminism-in-simulation) |  |
| `src/world/lotStore.ts#LotStore.eachBox` | compute | `native/src/sim/lot_store` | reaches src/world/buildings/items.ts#TableItems (wrong methodology: scattered-writes, hot-allocation) |  |
| `src/world/lotStore.ts#LotStore.eachNumbers` | compute | `native/src/sim/lot_store` | reaches src/world/buildings/items.ts#TableItems (wrong methodology: scattered-writes, hot-allocation) |  |
| `src/world/lotStore.ts#LotStore.eachNumbers.ofLot` | compute | `native/src/sim/lot_store` | reaches src/world/buildings/items.ts#TableItems (wrong methodology: scattered-writes, hot-allocation) |  |
| `src/world/lotStore.ts#LotStore.filing` | state | `native/src/sim/lot_store` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lotStore.ts#LotStore.forkFrom` | tick | `native/src/sim/lot_store` | reaches src/core/forkMap.ts#forkWork (wrong methodology: scattered-writes) |  |
| `src/world/lotStore.ts#LotStore.get` | compute | `native/src/sim/lot_store` | reaches src/world/buildings/items.ts#TableItems (wrong methodology: scattered-writes, hot-allocation) |  |
| `src/world/lotStore.ts#LotStore.has` | compute | `native/src/sim/lot_store` | reaches src/world/buildings/items.ts#TableItems (wrong methodology: scattered-writes, hot-allocation) |  |
| `src/world/lotStore.ts#LotStore.hasKey` | compute | `native/src/sim/lot_store` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lotStore.ts#LotStore.idAt` | state | `native/src/sim/lot_store` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lotStore.ts#LotStore.ids` | state | `native/src/sim/lot_store` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lotStore.ts#LotStore.idsNear` | compute | `native/src/sim/lot_store` | reaches src/world/buildings/items.ts#TableItems (wrong methodology: scattered-writes, hot-allocation) |  |
| `src/world/lotStore.ts#LotStore.idsPlacedIn` | compute | `native/src/sim/lot_store` | reaches src/world/buildings/items.ts#TableItems (wrong methodology: scattered-writes, hot-allocation) |  |
| `src/world/lotStore.ts#LotStore.in` | compute | `native/src/sim/lot_store` | reaches src/world/buildings/items.ts#TableItems (wrong methodology: scattered-writes, hot-allocation) |  |
| `src/world/lotStore.ts#LotStore.items` | state | `native/src/sim/lot_store` | reaches src/world/buildings/items.ts#TableItems (wrong methodology: scattered-writes, hot-allocation) |  |
| `src/world/lotStore.ts#LotStore.itemsOf` | compute | `native/src/sim/lot_store` | reaches src/world/buildings/items.ts#TableItems (wrong methodology: scattered-writes, hot-allocation) |  |
| `src/world/lotStore.ts#LotStore.keyList` | state | `native/src/sim/lot_store` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lotStore.ts#LotStore.keySet` | state | `native/src/sim/lot_store` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lotStore.ts#LotStore.keys` | compute | `native/src/sim/lot_store` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lotStore.ts#LotStore.keysRewritten` | state | `native/src/sim/lot_store` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lotStore.ts#LotStore.keysShared` | state | `native/src/sim/lot_store` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lotStore.ts#LotStore.leaveBelow` | tick | `native/src/sim/lot_store` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lotStore.ts#LotStore.list` | tick | `native/src/sim/lot_store` | reaches src/world/buildings/items.ts#TableItems (wrong methodology: scattered-writes, hot-allocation) |  |
| `src/world/lotStore.ts#LotStore.listed` | state | `native/src/sim/lot_store` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lotStore.ts#LotStore.load` | compute | `native/src/sim/lot_store` | reaches src/world/changes.ts#ChangeJournal.record (wrong methodology: nondeterminism-in-simulation) |  |
| `src/world/lotStore.ts#LotStore.loadKeys` | tick | `native/src/sim/lot_store` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lotStore.ts#LotStore.loadSections` | tick | `native/src/sim/lot_store` | reaches src/world/changes.ts#ChangeJournal.record (wrong methodology: nondeterminism-in-simulation) |  |
| `src/world/lotStore.ts#LotStore.nextId` | compute | `native/src/sim/lot_store` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lotStore.ts#LotStore.nextOrder` | state | `native/src/sim/lot_store` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lotStore.ts#LotStore.order` | state | `native/src/sim/lot_store` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lotStore.ts#LotStore.orderOf` | compute | `native/src/sim/lot_store` | reaches src/world/buildings/items.ts#TableItems (wrong methodology: scattered-writes, hot-allocation) |  |
| `src/world/lotStore.ts#LotStore.ownKeys` | tick | `native/src/sim/lot_store` | reaches src/core/forkMap.ts#forkWork (wrong methodology: scattered-writes) |  |
| `src/world/lotStore.ts#LotStore.placeOf` | compute | `native/src/sim/lot_store` | reaches src/world/buildings/items.ts#TableItems (wrong methodology: scattered-writes, hot-allocation) |  |
| `src/world/lotStore.ts#LotStore.replace` | tick | `native/src/sim/lot_store` | reaches src/world/buildings/items.ts#TableItems (wrong methodology: scattered-writes, hot-allocation) |  |
| `src/world/lotStore.ts#LotStore.replaceWith` | tick | `native/src/sim/lot_store` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/lotStore.ts#LotStore.reset` | tick | `native/src/sim/lot_store` | reaches src/world/buildings/items.ts#TableItems (wrong methodology: scattered-writes, hot-allocation) |  |
| `src/world/lotStore.ts#LotStore.restore` | compute | `native/src/sim/lot_store` | reaches src/world/changes.ts#ChangeJournal.record (wrong methodology: nondeterminism-in-simulation) |  |
| `src/world/lotStore.ts#LotStore.restoreNextId` | tick | `native/src/sim/lot_store` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lotStore.ts#LotStore.revision` | compute | `native/src/sim/lot_store` | reaches src/world/changes.ts#ChangeJournal (wrong methodology: nondeterminism-in-simulation, hot-allocation) |  |
| `src/world/lotStore.ts#LotStore.sections` | compute | `native/src/sim/lot_store` | reaches src/world/buildings/items.ts#TableItems (wrong methodology: scattered-writes, hot-allocation) |  |
| `src/world/lotStore.ts#LotStore.shareFrom` | tick | `native/src/sim/lot_store` | reaches src/world/buildings/items.ts#TableItems (wrong methodology: scattered-writes, hot-allocation) |  |
| `src/world/lotStore.ts#LotStore.size` | compute | `native/src/sim/lot_store` | reaches src/world/buildings/items.ts#TableItems (wrong methodology: scattered-writes, hot-allocation) |  |
| `src/world/lotStore.ts#LotStore.spliceKeys` | tick | `native/src/sim/lot_store` | reaches src/core/forkMap.ts#forkWork (wrong methodology: scattered-writes) |  |
| `src/world/lotStore.ts#LotStore.tableItems` | state | `native/src/sim/lot_store` | reaches src/world/buildings/items.ts#TableItems (wrong methodology: scattered-writes, hot-allocation) |  |
| `src/world/lotStore.ts#LotStore.tableOf` | compute | `native/src/sim/lot_store` | reaches src/world/lotRecord.ts#lotRecord (wrong methodology: hot-allocation) |  |
| `src/world/lotStore.ts#LotStore.tableRef` | compute | `native/src/sim/lot_store` | reaches src/world/buildings/items.ts#TableItems (wrong methodology: scattered-writes, hot-allocation) |  |
| `src/world/lotStore.ts#LotStore.takeFork` | tick | `native/src/sim/lot_store` | reaches src/world/buildings/items.ts#TableItems (wrong methodology: scattered-writes, hot-allocation) |  |
| `src/world/lotStore.ts#LotStore.takeWhole` | tick | `native/src/sim/lot_store` | reaches src/world/changes.ts#ChangeJournal.record (wrong methodology: nondeterminism-in-simulation) |  |
| `src/world/lotStore.ts#LotStore.toJSON` | compute | `native/src/sim/lot_store` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/lotStore.ts#LotStore.watch` | tick | `native/src/sim/lot_store` | reaches src/world/buildings/items.ts#TableItems (wrong methodology: scattered-writes, hot-allocation) |  |
| `src/world/lotStore.ts#LotStore.withWatch` | tick | `native/src/sim/lot_store` | reaches src/world/buildings/items.ts#TableItems (wrong methodology: scattered-writes, hot-allocation) |  |
| `src/world/lotStore.ts#lotBox` | compute | `native/src/sim/lot_store` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lotStore.ts#placeOf` | compute | `native/src/sim/lot_store` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lotSummary.ts#lotSummary` | compute | `native/src/sim/lot_summary` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/world/lotSummary.ts#lotSummaryColumns` | compute | `native/src/sim/lot_summary` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/world/lots.ts#CROSSED_AREA` | data | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#Candidate` | type | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#FIT_CELL` | data | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#FiledLot` | type | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#LAND_CLEARANCE` | data | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#LOT_DEPTH` | data | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#LOT_FRONTAGE` | data | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#LandLabels` | state | `native/src/sim/lots` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/lots.ts#LandLabels.constructor` | compute | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#LandLabels.flood` | tick | `native/src/sim/lots` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/lots.ts#LandLabels.label` | compute | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#LandLabels.labelUnder` | compute | `native/src/sim/lots` | reaches src/world/lots.ts#LandPaving.tile (wrong methodology: hot-allocation) |  |
| `src/world/lots.ts#LandLabels.labels` | state | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#LandLabels.lastKey` | state | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#LandLabels.lastTile` | state | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#LandLabels.pieceAt` | compute | `native/src/sim/lots` | reaches src/world/lots.ts#LandLabels.flood (wrong methodology: hot-allocation) |  |
| `src/world/lots.ts#LandLabels.pieceUnder` | compute | `native/src/sim/lots` | reaches src/world/lots.ts#LandLabels.flood (wrong methodology: hot-allocation) |  |
| `src/world/lots.ts#LandLabels.pieces` | state | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#LandLabels.scanTile` | compute | `native/src/sim/lots` | reaches src/world/lots.ts#LandLabels.flood (wrong methodology: hot-allocation) |  |
| `src/world/lots.ts#LandLabels.tileOf` | tick | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#LandPaving` | state | `native/src/sim/lots` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/lots.ts#LandPaving.clear` | tick | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#LandPaving.constructor` | compute | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#LandPaving.forget` | tick | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#LandPaving.lastBits` | state | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#LandPaving.lastKey` | state | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#LandPaving.paved` | tick | `native/src/sim/lots` | reaches src/world/lots.ts#LandPaving.tile (wrong methodology: hot-allocation) |  |
| `src/world/lots.ts#LandPaving.tile` | tick | `native/src/sim/lots` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/lots.ts#LandPaving.tiles` | state | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#LandPiece` | type | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#LotCut` | type | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#LotPlan` | type | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#LotPlanner` | state | `native/src/sim/lots` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/lots.ts#LotPlanner.around` | compute | `native/src/sim/lots` | reaches src/world/lotStore.ts#LotStore (wrong methodology: hot-allocation) |  |
| `src/world/lots.ts#LotPlanner.blockPlan` | compute | `native/src/sim/lots` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/lots.ts#LotPlanner.blockPlan.at` | compute | `native/src/sim/lots` | reaches src/world/lotRings.ts#orientedBox (wrong methodology: hot-allocation) |  |
| `src/world/lots.ts#LotPlanner.blockPlan.frame` | compute | `native/src/sim/lots` | reaches src/world/lotRings.ts#orientedBox (wrong methodology: hot-allocation) |  |
| `src/world/lots.ts#LotPlanner.blockPlan.rowSpans` | compute | `native/src/sim/lots` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/lots.ts#LotPlanner.byId` | state | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#LotPlanner.constructor` | tick | `native/src/sim/lots` | reaches src/world/lots.ts#LandPaving (wrong methodology: hot-allocation) |  |
| `src/world/lots.ts#LotPlanner.dropped` | state | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#LotPlanner.filed` | state | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#LotPlanner.fitFree` | compute | `native/src/sim/lots` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/lots.ts#LotPlanner.install` | tick | `native/src/sim/lots` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/lots.ts#LotPlanner.lotCells` | state | `native/src/sim/lots` | reaches src/world/cells.ts#CellFiling (wrong methodology: hot-allocation, duplicated-code) |  |
| `src/world/lots.ts#LotPlanner.netState` | state | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#LotPlanner.nextId` | state | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#LotPlanner.pavedTest` | compute | `native/src/sim/lots` | reaches src/world/lots.ts#LandPaving (wrong methodology: hot-allocation) |  |
| `src/world/lots.ts#LotPlanner.paving` | state | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#LotPlanner.plan` | compute | `native/src/sim/lots` | reaches src/world/lots.ts#LotPlanner.blockPlan (wrong methodology: hot-allocation) |  |
| `src/world/lots.ts#LotPlanner.planAll` | tick | `native/src/sim/lots` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/lots.ts#LotPlanner.planChanged` | tick | `native/src/sim/lots` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/lots.ts#LotPlanner.planChanged.put` | tick | `native/src/sim/lots` | reaches src/world/lots.ts#LotPlanner.planChanged (wrong methodology: hot-allocation) |  |
| `src/world/lots.ts#LotPlanner.result` | compute | `native/src/sim/lots` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/lots.ts#LotPlanner.run` | tick | `native/src/sim/lots` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/lots.ts#LotPlanner.run.enqueue` | tick | `native/src/sim/lots` | reaches src/world/lots.ts#LotPlanner.run (wrong methodology: hot-allocation) |  |
| `src/world/lots.ts#LotPlanner.seen` | state | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#LotPlanner.slots` | state | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#LotPlanner.sourceCells` | state | `native/src/sim/lots` | reaches src/world/cells.ts#CellFiling (wrong methodology: hot-allocation, duplicated-code) |  |
| `src/world/lots.ts#LotPlanner.streetPlan` | compute | `native/src/sim/lots` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/lots.ts#LotPlanner.streetPlan.open` | compute | `native/src/sim/lots` | reaches src/world/lots.ts#LotPlanner.streetPlan (wrong methodology: hot-allocation) |  |
| `src/world/lots.ts#LotPlanner.uninstall` | tick | `native/src/sim/lots` | reaches src/world/cells.ts#CellFiling (wrong methodology: hot-allocation, duplicated-code) |  |
| `src/world/lots.ts#LotPlanner.unknown` | state | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#LotPlanner.update` | tick | `native/src/sim/lots` | reaches src/world/changes.ts#ChangeJournal.since (wrong methodology: hot-allocation) |  |
| `src/world/lots.ts#MAX_BLOCK` | data | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#MIN_LOT` | data | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#PAD` | data | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#PAVING_MARGIN` | data | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#PlanOrder` | type | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#PlanSource` | type | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#PlanWork` | type | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#Planned` | type | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#PlannedLot` | type | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#RASTER` | data | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#SIDES` | data | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#SMALL_PIECE` | data | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#STRAIGHT_BACK` | data | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#STREET_GROW` | data | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#TILE` | data | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#TILE_SIDE` | data | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#TWO_ROWS` | data | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#addPolygonLot` | compute | `native/src/sim/lots` | reaches src/world/lots.ts#overlapDeep (wrong methodology: hot-allocation) |  |
| `src/world/lots.ts#applyLots` | compute | `native/src/sim/lots` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/lots.ts#blockWork` | compute | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#cellIndex` | compute | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#clipLeft` | compute | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#clipLeft.side` | compute | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#compareOrder` | compute | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#coveredBy` | compute | `native/src/sim/lots` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/lots.ts#curveLotSide` | compute | `native/src/sim/lots` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/lots.ts#curveLotSide.curve` | compute | `native/src/sim/lots` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/lots.ts#cutLines` | compute | `native/src/sim/lots` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/lots.ts#cutLines.P` | compute | `native/src/sim/lots` | reaches src/world/lots.ts#cutLines (wrong methodology: hot-allocation) |  |
| `src/world/lots.ts#cutLotsByRoads` | compute | `native/src/sim/lots` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/lots.ts#cutSpans` | compute | `native/src/sim/lots` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/lots.ts#deleteLot` | compute | `native/src/sim/lots` | reaches src/world/lotStore.ts#LotStore.apply (wrong methodology: hot-allocation) |  |
| `src/world/lots.ts#facingCorners` | compute | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#footwayEdgesNear` | compute | `native/src/sim/lots` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/lots.ts#forTiles` | compute | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#forgetLotPlans` | tick | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#freeSpans` | compute | `native/src/sim/lots` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/lots.ts#isLot` | compute | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#joinLots` | compute | `native/src/sim/lots` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/lots.ts#landLot` | compute | `native/src/sim/lots` | reaches src/world/buildings/footprints.ts#offsetRing (wrong methodology: hot-allocation) |  |
| `src/world/lots.ts#lerp` | compute | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#lotArea` | compute | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#lotBuildFrame` | compute | `native/src/sim/lots` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/lots.ts#lotFrame` | compute | `native/src/sim/lots` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/lots.ts#lotPlanWork` | data | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#lotRect` | compute | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#lotRect.p` | compute | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#lotSnapper` | compute | `native/src/sim/lots` | wrong methodology: hot-allocation, quadratic-proximity | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous.<br>quadratic-proximity: A spatial partition: uniform grid or spatial hash, querying only nearby cells. |
| `src/world/lots.ts#meets` | compute | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#meetsInside` | compute | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#moveLotCorner` | compute | `native/src/sim/lots` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/lots.ts#near` | compute | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#nearerStreet` | compute | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#onLand` | compute | `native/src/sim/lots` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/lots.ts#overlapDeep` | compute | `native/src/sim/lots` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/lots.ts#overlapDeep.inner` | compute | `native/src/sim/lots` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/lots.ts#pavingNear` | compute | `native/src/sim/lots` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/lots.ts#planLots` | tick | `native/src/sim/lots` | reaches src/world/lots.ts#LotPlanner (wrong methodology: hot-allocation) |  |
| `src/world/lots.ts#plannedStreet` | compute | `native/src/sim/lots` | reaches src/core/recordMap.ts#RecordMap (wrong methodology: scattered-writes) |  |
| `src/world/lots.ts#planners` | data | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#rayExit` | compute | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#reachesTiles` | compute | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#roadEditRects` | compute | `native/src/sim/lots` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/lots.ts#sameMade` | compute | `native/src/sim/lots` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/lots.ts#sameRect` | compute | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#setLotFront` | compute | `native/src/sim/lots` | reaches src/world/lotStore.ts#LotStore.apply (wrong methodology: hot-allocation) |  |
| `src/world/lots.ts#spanOf` | compute | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#splitLot` | compute | `native/src/sim/lots` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/lots.ts#squareCorners` | compute | `native/src/sim/lots` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/lots.ts#streetWork` | compute | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#tileKey` | compute | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#tileRect` | compute | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#withFront` | compute | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#withFront.off` | compute | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#workFor` | compute | `native/src/sim/lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/lots.ts#zoneLots` | compute | `native/src/sim/lots` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/nearestRibbon.ts#FIRST_REACH` | data | `native/src/sim/nearest_ribbon` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/nearestRibbon.ts#LAST_REACH` | data | `native/src/sim/nearest_ribbon` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/nearestRibbon.ts#nearestRibbon` | compute | `native/src/sim/nearest_ribbon` | wrong methodology: hot-allocation, quadratic-proximity | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous.<br>quadratic-proximity: A spatial partition: uniform grid or spatial hash, querying only nearby cells. |
| `src/world/openLots.ts#CELL` | data | `native/src/sim/open_lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/openLots.ts#LotsHost` | type | `native/src/sim/open_lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/openLots.ts#MIN_ROOM` | data | `native/src/sim/open_lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/openLots.ts#OpenLots` | state | `native/src/sim/open_lots` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous.<br>holey-array: An array literal filled with push, or a typed array of length n for numbers. |
| `src/world/openLots.ts#OpenLots.at` | compute | `native/src/sim/open_lots` | reaches src/world/buildings/items.ts#TableItems (wrong methodology: scattered-writes, hot-allocation) |  |
| `src/world/openLots.ts#OpenLots.byBuilding` | state | `native/src/sim/open_lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/openLots.ts#OpenLots.cellOfSlot` | state | `native/src/sim/open_lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/openLots.ts#OpenLots.cells` | state | `native/src/sim/open_lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/openLots.ts#OpenLots.clear` | tick | `native/src/sim/open_lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/openLots.ts#OpenLots.constructor` | compute | `native/src/sim/open_lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/openLots.ts#OpenLots.count` | compute | `native/src/sim/open_lots` | reaches src/world/openLots.ts#OpenLots.sync (wrong methodology: hot-allocation) |  |
| `src/world/openLots.ts#OpenLots.cursor` | state | `native/src/sim/open_lots` | reaches src/world/lotStore.ts#LotStore (wrong methodology: hot-allocation) |  |
| `src/world/openLots.ts#OpenLots.file` | tick | `native/src/sim/open_lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/openLots.ts#OpenLots.findLast` | tick | `native/src/sim/open_lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/openLots.ts#OpenLots.flag` | tick | `native/src/sim/open_lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/openLots.ts#OpenLots.grew` | compute | `native/src/sim/open_lots` | reaches src/world/lotStore.ts#LotStore.apply (wrong methodology: hot-allocation) |  |
| `src/world/openLots.ts#OpenLots.holes` | state | `native/src/sim/open_lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/openLots.ts#OpenLots.ids` | state | `native/src/sim/open_lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/openLots.ts#OpenLots.lastGrown` | compute | `native/src/sim/open_lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/openLots.ts#OpenLots.lastId` | state | `native/src/sim/open_lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/openLots.ts#OpenLots.lastStale` | state | `native/src/sim/open_lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/openLots.ts#OpenLots.lotAt` | compute | `native/src/sim/open_lots` | reaches src/world/buildings/items.ts#TableItems (wrong methodology: scattered-writes, hot-allocation) |  |
| `src/world/openLots.ts#OpenLots.name` | tick | `native/src/sim/open_lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/openLots.ts#OpenLots.named` | state | `native/src/sim/open_lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/openLots.ts#OpenLots.near` | compute | `native/src/sim/open_lots` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/openLots.ts#OpenLots.open` | state | `native/src/sim/open_lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/openLots.ts#OpenLots.openCount` | state | `native/src/sim/open_lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/openLots.ts#OpenLots.place` | tick | `native/src/sim/open_lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/openLots.ts#OpenLots.rebuild` | tick | `native/src/sim/open_lots` | reaches src/world/buildings/items.ts#TableItems (wrong methodology: scattered-writes, hot-allocation) | holey-array: An array literal filled with push, or a typed array of length n for numbers. |
| `src/world/openLots.ts#OpenLots.refuse` | tick | `native/src/sim/open_lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/openLots.ts#OpenLots.refused` | state | `native/src/sim/open_lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/openLots.ts#OpenLots.refusedSize` | state | `native/src/sim/open_lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/openLots.ts#OpenLots.room` | state | `native/src/sim/open_lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/openLots.ts#OpenLots.seenBuildings` | state | `native/src/sim/open_lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/openLots.ts#OpenLots.slotOf` | state | `native/src/sim/open_lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/openLots.ts#OpenLots.slotOrder` | state | `native/src/sim/open_lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/openLots.ts#OpenLots.sync` | tick | `native/src/sim/open_lots` | wrong methodology: hot-allocation | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/openLots.ts#OpenLots.top` | state | `native/src/sim/open_lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/openLots.ts#OpenLots.tree` | state | `native/src/sim/open_lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/openLots.ts#OpenLots.unname` | tick | `native/src/sim/open_lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/openLots.ts#OpenLots.zoned` | state | `native/src/sim/open_lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/openLots.ts#cellOf` | compute | `native/src/sim/open_lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/openLots.ts#keyOf` | compute | `native/src/sim/open_lots` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/poleLines.ts#Placed` | type | `native/src/sim/pole_lines` | reaches src/world/poleLines.ts#markPlates, which the game already has in sim/street/pole_routes.cpp: name it in the conversion library (port street-furniture says portado, not shown: sim/street/pole_routes.cpp never names Placed (nor placed)) |  |
| `src/world/poleLines.ts#markPlates` | compute | `native/src/sim/pole_lines` | wrong methodology: hot-allocation (port street-furniture says portado, not shown: sim/street/pole_routes.cpp never cites world/poleLines.ts) | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/poleLines.ts#markPlates.cx` | compute | `native/src/sim/pole_lines` | reaches src/world/poleLines.ts#markPlates, which the game already has in sim/street/pole_routes.cpp: name it in the conversion library (port street-furniture says portado, not shown: sim/street/pole_routes.cpp never cites world/poleLines.ts) |  |
| `src/world/poleLines.ts#markPlates.cy` | compute | `native/src/sim/pole_lines` | reaches src/world/poleLines.ts#markPlates, which the game already has in sim/street/pole_routes.cpp: name it in the conversion library (port street-furniture says portado, not shown: sim/street/pole_routes.cpp never cites world/poleLines.ts) |  |
| `src/world/poleLines.ts#markPlates.place` | tick | `native/src/sim/pole_lines` | wrong methodology: hot-allocation (port street-furniture says portado, not shown: sim/street/pole_routes.cpp never cites world/poleLines.ts) | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous. |
| `src/world/poleLines.ts#onPaving` | compute | `native/src/sim/pole_lines` | reaches src/world/poleLines.ts#insideLevels, which the game already has in sim/street/footway.cpp: name it in the conversion library (port street-furniture says portado, not shown: sim/street/footway.cpp never names onPaving (nor on_paving)) |  |
| `src/world/rasterGrid.ts#LAND` | data | `native/src/sim/raster_grid` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/rasterGrid.ts#NEAR` | data | `native/src/sim/raster_grid` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/rasterGrid.ts#PAVED` | data | `native/src/sim/raster_grid` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/rasterGrid.ts#RasterGrid` | type | `native/src/sim/raster_grid` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/rasterGrid.ts#firstTrue` | compute | `native/src/sim/raster_grid` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/rasterGrid.ts#gridRect` | compute | `native/src/sim/raster_grid` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/rescale.ts#rescaleSerializedDoc` | compute | `native/src/sim/rescale` | a part of shared code: it joins the code of the owner that holds the rest | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous.<br>object-delete: A Map for dynamic keys, or a fixed shape with the slot set to undefined. |
| `src/world/roadField.ts#roadField.frameInto` | compute | `native/src/sim/road_field` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/world/roadField.ts#roadField.nearest` | compute | `native/src/sim/road_field` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/world/roadField.ts#roadField.onSegment` | compute | `native/src/sim/road_field` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/world/roadField.ts#roadField.query` | tick | `native/src/sim/road_field` | a part of shared code: it joins the code of the owner that holds the rest |  |
| `src/world/zoneGrid.ts#BUCKET` | data | `native/src/sim/zone_grid` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/zoneGrid.ts#OVERLAP_SLACK` | data | `native/src/sim/zone_grid` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/zoneGrid.ts#SUB_CELL` | data | `native/src/sim/zone_grid` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/zoneGrid.ts#SWEEP_SLACK` | data | `native/src/sim/zone_grid` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/zoneGrid.ts#Street` | type | `native/src/sim/zone_grid` | reaches src/world/zoneGrid.ts#zoneGridSteps (wrong methodology: god-function, quadratic-proximity) |  |
| `src/world/zoneGrid.ts#ZONE_CELL` | data | `native/src/sim/zone_grid` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/zoneGrid.ts#ZONE_DEPTH` | data | `native/src/sim/zone_grid` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/zoneGrid.ts#ZoneCell` | type | `native/src/sim/zone_grid` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/zoneGrid.ts#ZoneGrid` | type | `native/src/sim/zone_grid` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/zoneGrid.ts#bucketKey` | compute | `native/src/sim/zone_grid` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/zoneGrid.ts#buildZoneGrid` | compute | `native/src/sim/zone_grid` | reaches src/world/zoneGrid.ts#zoneGridSteps (wrong methodology: god-function, quadratic-proximity) |  |
| `src/world/zoneGrid.ts#insideQuad` | compute | `native/src/sim/zone_grid` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/zoneGrid.ts#key` | compute | `native/src/sim/zone_grid` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/zoneGrid.ts#markPaved` | compute | `native/src/sim/zone_grid` | reaches src/world/poleLines.ts#markPlates, which the game already has in sim/street/pole_routes.cpp: name it in the conversion library |  |
| `src/world/zoneGrid.ts#markRoads` | compute | `native/src/sim/zone_grid` | wrong methodology: hot-allocation, quadratic-proximity | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous.<br>quadratic-proximity: A spatial partition: uniform grid or spatial hash, querying only nearby cells. |
| `src/world/zoneGrid.ts#pavedTester` | compute | `native/src/sim/zone_grid` | reaches src/world/poleLines.ts#onPaving, which the game already has in sim/street/footway.cpp: name it in the conversion library |  |
| `src/world/zoneGrid.ts#pavedTester.onPlate` | compute | `native/src/sim/zone_grid` | reaches src/world/poleLines.ts#onPaving, which the game already has in sim/street/footway.cpp: name it in the conversion library |  |
| `src/world/zoneGrid.ts#pavedTester.onRoad` | compute | `native/src/sim/zone_grid` | reaches src/world/roadTypes.ts#Level (a enum) |  |
| `src/world/zoneGrid.ts#quadsOverlap` | compute | `native/src/sim/zone_grid` | wrong methodology: hot-allocation, quadratic-proximity | hot-allocation: Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous.<br>quadratic-proximity: A spatial partition: uniform grid or spatial hash, querying only nearby cells. |
| `src/world/zoneGrid.ts#zoneGridSteps` | compute | `native/src/sim/zone_grid` | wrong methodology: god-function, quadratic-proximity | god-function: Extract each step into a function with one responsibility, owned by the module of its concept.<br>quadratic-proximity: A spatial partition: uniform grid or spatial hash, querying only nearby cells. |
| `src/world/zoneGrid.ts#zoneGridSteps.at` | compute | `native/src/sim/zone_grid` | a method |  |
| `src/world/zoneGrid.ts#zoneGridSteps.cellAt` | compute | `native/src/sim/zone_grid` | a method |  |
| `src/world/zoneGrid.ts#zoneGridSteps.cellsNear` | compute | `native/src/sim/zone_grid` | a method | quadratic-proximity: A spatial partition: uniform grid or spatial hash, querying only nearby cells. |
| `src/world/zoneGrid.ts#zoneGridSteps.inset` | compute | `native/src/sim/zone_grid` | reaches src/world/zoneGrid.ts#zoneGridSteps (wrong methodology: god-function, quadratic-proximity) |  |
| `src/world/zoneGrid.ts#zoneGridSteps.lay` | tick | `native/src/sim/zone_grid` | reaches src/world/zoneGrid.ts#zoneGridSteps (wrong methodology: god-function, quadratic-proximity) |  |
| `src/world/zoneGrid.ts#zoneGridSteps.taken` | compute | `native/src/sim/zone_grid` | wrong methodology: quadratic-proximity | quadratic-proximity: A spatial partition: uniform grid or spatial hash, querying only nearby cells. |
| `src/world/zones.ts#ZONE_DENSITIES` | data | `native/src/sim/zones` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/zones.ts#ZONE_USES` | data | `native/src/sim/zones` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/zones.ts#Zone` | type | `native/src/sim/zones` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/zones.ts#ZoneDensity` | type | `native/src/sim/zones` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/zones.ts#ZoneMark` | type | `native/src/sim/zones` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/zones.ts#ZoneUse` | type | `native/src/sim/zones` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/zones.ts#isZoneDensity` | compute | `native/src/sim/zones` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/zones.ts#isZoneMark` | compute | `native/src/sim/zones` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/zones.ts#isZoneUse` | compute | `native/src/sim/zones` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/zones.ts#zoneDensityName` | compute | `native/src/sim/zones` | computes or edits; nothing wrong in it or in what it reaches |  |
| `src/world/zones.ts#zoneUseName` | compute | `native/src/sim/zones` | computes or edits; nothing wrong in it or in what it reaches |  |

## Constants that need a measurement or a primary source

- `EPS`
