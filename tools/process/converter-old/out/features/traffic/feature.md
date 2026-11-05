# Feature: src/render/carBody.ts, src/render/densityCars.ts, src/render/exhaust.ts, src/render/occupants.ts, src/render/vehicleFrame.ts, src/render/vehicleLevels.ts, src/render/vehicleModels.ts, src/render/vehiclePanels.ts, src/render/vehicleWindows.ts, src/sim/agents/bay.ts, src/sim/agents/buildingOrder.ts, src/sim/agents/hybridAStar.ts, src/sim/agents/kerbBays.ts, src/sim/agents/laneGrid.ts, src/sim/agents/lorryDocks.ts, src/sim/agents/lorryTraffic.ts, src/sim/agents/lotBays.ts, src/sim/agents/lotDoors.ts, src/sim/agents/lotNav.ts, src/sim/agents/lotTraffic.ts, src/sim/agents/manoeuvre.ts, src/sim/agents/parking.ts, src/sim/agents/reedsShepp.ts, src/sim/agents/trafficBuildings.ts, src/sim/agents/walk.ts, src/sim/intersections/admission.ts, src/sim/intersections/claims.ts, src/sim/intersections/crossingSpans.ts, src/sim/intersections/cycles.ts, src/sim/intersections/spillback.ts, src/sim/meso/demand.ts, src/sim/meso/memory.ts, src/sim/meso/meso.ts, src/sim/meso/model.ts, src/sim/meso/slots.ts, src/sim/meso/tileLinks.ts, src/sim/populationGovernor.ts, src/sim/routing/destination.ts, src/sim/routing/laneReach.ts, src/sim/routing/lowerBound.ts, src/sim/routing/routeIndex.ts, src/sim/routing/router.ts, src/sim/routing/searchScratch.ts, src/sim/signals/fsm.ts, src/sim/signals/permission.ts, src/sim/signals/plan.ts, src/sim/signals/query.ts, src/sim/vehicleSignals.ts, src/sim/vehicles/archetypes.ts, src/sim/vehicles/curvature.ts, src/sim/vehicles/cycleLane.ts, src/sim/vehicles/driver.ts, src/sim/vehicles/idm.ts, src/sim/vehicles/integrate.ts, src/sim/vehicles/kerbStops.ts, src/sim/vehicles/laneChange.ts, src/sim/vehicles/laneClosures.ts, src/sim/vehicles/leaderIndex.ts, src/sim/vehicles/obstacles.ts, src/sim/vehicles/spawn.ts, src/sim/vehicles/state.ts, src/sim/vehicles/vehicleId.ts, src/world/conflictPoints.ts, src/world/conflictTable.ts, src/world/lanelets.ts, src/world/parking.ts, src/world/parkingLayout.ts, src/world/turnPaths.ts, src/sim/ambient/demand.ts, src/sim/ambient/ambientIndex.ts, src/sim/ambient/lotPlaces.ts, src/sim/params.ts, src/render/agents.ts, src/sim/roads/greenWave.ts, src/sim/roads/flowStats.ts, src/sim/roads/controlAdvisor.ts, src/ui/roads/junctionPanel.ts, src/world/roads/rules.ts

The program starts from `src/main.ts`, `src/peopleLab/main.ts`. A declaration is the feature's own when the program reaches it only through the feature; in a declaration of the program that names the feature, its statements are a slice (what names it, what depends on that, what that controls).

- Own: 3322 declarations
- Slices of shared code: 362
- Code: 37631 lines in 184 files (source/)
- Resources: 330 (240 entry, 90 style, 0 import; 217 only the feature uses)
- Uses: 916 shared declarations (interface/: 1511 declarations in 236 files, the types they name included); hooks: 583; external APIs: 531

## Code

- src/actionsWiring.ts — part, 37 lines
- src/bayPool.ts — part, 94 lines
- src/bays.worker.ts — part, 17 lines
- src/conflictPool.ts — part, 300 lines
- src/conflictTable.worker.ts — part, 57 lines
- src/core/polygonBands.ts — whole, 59 lines
- src/core/runPool.ts — whole, 75 lines
- src/core/scalar.ts — part, 1 lines
- src/core/simplifyLine.ts — whole, 25 lines
- src/core/weightedSlots.ts — part, 115 lines
- src/editor/blocks.ts — part, 9 lines
- src/editor/commit.ts — part, 49 lines
- src/editor/roadParking.ts — part, 2 lines
- src/editor/roads/profile.ts — part, 8 lines
- src/editor/roadTool.ts — part, 12 lines
- src/editor/streetObjects.ts — part, 4 lines
- src/editor/streetscapeTool.ts — part, 8 lines
- src/lab/inspector/entity.ts — part, 20 lines
- src/main.ts — part, 7 lines
- src/people/body/morph.ts — part, 22 lines
- src/people/hair/procedural.ts — part, 3 lines
- src/render/agents.ts — whole, 2597 lines
- src/render/bakePool.ts — part, 24 lines
- src/render/blast.ts — part, 33 lines
- src/render/blobShadows.ts — part, 92 lines
- src/render/carBody.ts — whole, 1019 lines
- src/render/citizenBake.ts — part, 1 lines
- src/render/citizenCasting.ts — part, 199 lines
- src/render/citizenGait.ts — part, 364 lines
- src/render/citizenPalette.ts — part, 29 lines
- src/render/citizenPlays.ts — part, 92 lines
- src/render/densityCars.ts — whole, 510 lines
- src/render/destruction.ts — part, 33 lines
- src/render/elements.ts — part, 22 lines
- src/render/exhaust.ts — whole, 240 lines
- src/render/groundShear.ts — whole, 36 lines
- src/render/interiorSolids.ts — whole, 71 lines
- src/render/lightLevels.ts — part, 3 lines
- src/render/mesh/simplify.ts — whole, 29 lines
- src/render/occupants.ts — whole, 241 lines
- src/render/pagePorts.ts — part, 3 lines
- src/render/paintQuads.ts — part, 1 lines
- src/render/people/classRigMade.ts — part, 21 lines
- src/render/people/cookPack.ts — part, 14 lines
- src/render/people/crowdAnimation.ts — part, 90 lines
- src/render/people/crowdBodies.ts — part, 81 lines
- src/render/people/crowdLod.ts — part, 201 lines
- src/render/people/faceExpression.ts — part, 18 lines
- src/render/people/gpuRows.ts — part, 134 lines
- src/render/people/hairTexture.ts — whole, 67 lines
- src/render/people/heldProps.ts — part, 38 lines
- src/render/people/paletteTargets.ts — part, 19 lines
- src/render/people/proceduralBake.ts — part, 27 lines
- src/render/people/proceduralCook.ts — part, 46 lines
- src/render/people/proceduralCrowd.ts — part, 1895 lines
- src/render/people/skinAppearance.ts — part, 7 lines
- src/render/playEffects.ts — part, 78 lines
- src/render/ragdoll.ts — part, 50 lines
- src/render/renderer.ts — part, 107 lines
- src/render/riggedCitizens.ts — part, 554 lines
- src/render/roadSurfaceInputs.ts — part, 15 lines
- src/render/signals.ts — part, 8 lines
- src/render/vehicleFrame.ts — whole, 76 lines
- src/render/vehicleLevels.ts — whole, 83 lines
- src/render/vehicleModels.ts — whole, 1246 lines
- src/render/vehiclePanels.ts — whole, 140 lines
- src/render/vehicleWindows.ts — whole, 54 lines
- src/sim/agents/bay.ts — whole, 45 lines
- src/sim/agents/buildingOrder.ts — whole, 253 lines
- src/sim/agents/hybridAStar.ts — whole, 475 lines
- src/sim/agents/kerbBays.ts — whole, 388 lines
- src/sim/agents/laneGrid.ts — whole, 193 lines
- src/sim/agents/lorryDocks.ts — whole, 559 lines
- src/sim/agents/lorryTraffic.ts — whole, 484 lines
- src/sim/agents/lotBays.ts — whole, 520 lines
- src/sim/agents/lotDoors.ts — whole, 127 lines
- src/sim/agents/lotNav.ts — whole, 319 lines
- src/sim/agents/lotTraffic.ts — whole, 740 lines
- src/sim/agents/manoeuvre.ts — whole, 253 lines
- src/sim/agents/parking.ts — whole, 485 lines
- src/sim/agents/reedsShepp.ts — whole, 272 lines
- src/sim/agents/trafficBuildings.ts — whole, 747 lines
- src/sim/agents/walk.ts — whole, 2276 lines
- src/sim/ambient/ambient.ts — part, 380 lines
- src/sim/ambient/ambientIndex.ts — whole, 402 lines
- src/sim/ambient/demand.ts — whole, 63 lines
- src/sim/ambient/lotPlaces.ts — whole, 392 lines
- src/sim/ambient/playerId.ts — whole, 3 lines
- src/sim/city/city.ts — part, 4 lines
- src/sim/clock.ts — part, 12 lines
- src/sim/crossings/permission.ts — whole, 172 lines
- src/sim/crossings/state.ts — part, 4 lines
- src/sim/drive/operational.ts — whole, 69 lines
- src/sim/drive/physicalMotion.ts — whole, 21 lines
- src/sim/drive/tactical.ts — whole, 148 lines
- src/sim/host/fleetGrid.ts — part, 14 lines
- src/sim/host/protocol.ts — part, 48 lines
- src/sim/host/simAnswers.ts — part, 51 lines
- src/sim/host/simClient.ts — part, 75 lines
- src/sim/host/simHost.ts — part, 147 lines
- src/sim/intersections/admission.ts — whole, 1140 lines
- src/sim/intersections/claims.ts — whole, 209 lines
- src/sim/intersections/crossingSpans.ts — whole, 241 lines
- src/sim/intersections/cycles.ts — whole, 261 lines
- src/sim/intersections/spillback.ts — whole, 94 lines
- src/sim/invariants.ts — part, 216 lines
- src/sim/meso/demand.ts — whole, 118 lines
- src/sim/meso/memory.ts — whole, 171 lines
- src/sim/meso/meso.ts — whole, 799 lines
- src/sim/meso/model.ts — whole, 631 lines
- src/sim/meso/slots.ts — whole, 227 lines
- src/sim/meso/tileLinks.ts — whole, 9 lines
- src/sim/params.ts — whole, 138 lines
- src/sim/peds/sidewalk.ts — part, 43 lines
- src/sim/peds/signalPostIndex.ts — part, 71 lines
- src/sim/people/casualties.ts — part, 10 lines
- src/sim/people/engine.ts — part, 4 lines
- src/sim/people/view.ts — part, 3 lines
- src/sim/pipeline.ts — part, 258 lines
- src/sim/populationGovernor.ts — whole, 78 lines
- src/sim/pose.ts — part, 96 lines
- src/sim/roads/controlAdvisor.ts — whole, 66 lines
- src/sim/roads/flowStats.ts — whole, 98 lines
- src/sim/roads/greenWave.ts — whole, 88 lines
- src/sim/routing/destination.ts — whole, 77 lines
- src/sim/routing/laneReach.ts — whole, 375 lines
- src/sim/routing/lowerBound.ts — whole, 42 lines
- src/sim/routing/routeIndex.ts — whole, 74 lines
- src/sim/routing/router.ts — whole, 403 lines
- src/sim/routing/searchScratch.ts — whole, 232 lines
- src/sim/signals/fsm.ts — whole, 431 lines
- src/sim/signals/permission.ts — whole, 44 lines
- src/sim/signals/plan.ts — whole, 434 lines
- src/sim/signals/query.ts — whole, 51 lines
- src/sim/topologyCatchUp.ts — part, 1 lines
- src/sim/transit/transit.ts — part, 166 lines
- src/sim/vehicles/archetypes.ts — whole, 237 lines
- src/sim/vehicles/curvature.ts — whole, 127 lines
- src/sim/vehicles/cycleLane.ts — whole, 33 lines
- src/sim/vehicles/driver.ts — whole, 77 lines
- src/sim/vehicles/idm.ts — whole, 110 lines
- src/sim/vehicles/integrate.ts — whole, 327 lines
- src/sim/vehicles/kerbStops.ts — whole, 596 lines
- src/sim/vehicles/laneChange.ts — whole, 578 lines
- src/sim/vehicles/laneClosures.ts — whole, 90 lines
- src/sim/vehicles/leaderIndex.ts — whole, 202 lines
- src/sim/vehicles/obstacles.ts — whole, 213 lines
- src/sim/vehicles/spawn.ts — whole, 328 lines
- src/sim/vehicles/state.ts — whole, 250 lines
- src/sim/vehicles/vehicleId.ts — whole, 3 lines
- src/sim/vehicleSignals.ts — whole, 132 lines
- src/sim/world.ts — part, 588 lines
- src/ui/inspector.ts — part, 65 lines
- src/ui/roads/junctionPanel.ts — whole, 326 lines
- src/ui/roads/profileEditor.ts — part, 6 lines
- src/world/buildings/docks.ts — whole, 57 lines
- src/world/buildings/sections.ts — part, 25 lines
- src/world/buildings/streetDoors.ts — whole, 42 lines
- src/world/conflictPoints.ts — whole, 688 lines
- src/world/conflictTable.ts — whole, 644 lines
- src/world/derived.ts — part, 60 lines
- src/world/doc.ts — part, 91 lines
- src/world/heading.ts — part, 6 lines
- src/world/junction/mouthOverlap.ts — whole, 26 lines
- src/world/junctionBuild.ts — part, 250 lines
- src/world/junctionTurns.ts — part, 41 lines
- src/world/lanelets.ts — whole, 846 lines
- src/world/markings.ts — part, 14 lines
- src/world/networkLength.ts — whole, 45 lines
- src/world/parking.ts — whole, 70 lines
- src/world/parkingLayout.ts — whole, 258 lines
- src/world/roads/catalog.ts — part, 4 lines
- src/world/roads/derivedSigns.ts — part, 18 lines
- src/world/roads/laneEnds.ts — part, 30 lines
- src/world/roads/profile.ts — part, 11 lines
- src/world/roads/rules.ts — whole, 143 lines
- src/world/roadSection.ts — part, 4 lines
- src/world/roadTypes.ts — part, 29 lines
- src/world/surfaceParts.ts — part, 37 lines
- src/world/sweptBodies.ts — whole, 55 lines
- src/world/turnChoice.ts — part, 7 lines
- src/world/turnDigest.ts — whole, 28 lines
- src/world/turnPaths.ts — whole, 633 lines
- src/world/walkways.ts — part, 9 lines

## Slices of shared code

- `src/actionsWiring.ts#VEHICLE_HALF_SPAN` — 1 of 1 lines
- `src/actionsWiring.ts#createActions.explodeAt` — 7 of 158 lines
- `src/actionsWiring.ts#createActions.shootVehicle` — 29 of 62 lines
- `src/bayPool.ts#BayPool` — 48 of 103 lines
- `src/bayPool.ts#BayPool.start` — 19 of 35 lines
- `src/bayPool.ts#BayPool.work` — 16 of 19 lines
- `src/bayPool.ts#Lane` — 3 of 8 lines
- `src/bays.worker.ts#serve` — 17 of 20 lines
- `src/conflictPool.ts#ConflictPool` — 155 of 260 lines
- `src/conflictPool.ts#ConflictPool.buildJunctions` — 15 of 17 lines
- `src/conflictPool.ts#ConflictPool.chooseTurns` — 27 of 29 lines
- `src/conflictPool.ts#ConflictPool.finishPairs` — 25 of 27 lines
- `src/conflictPool.ts#ConflictPool.sampleSweeps` — 29 of 31 lines
- `src/conflictPool.ts#ConflictPool.start` — 23 of 40 lines
- `src/conflictTable.worker.ts#JunctionsAsk` — 3 of 5 lines
- `src/conflictTable.worker.ts#serve` — 54 of 152 lines
- `src/editor/blocks.ts#commitBlockGrid` — 9 of 18 lines
- `src/editor/commit.ts#commitDraftInPlace` — 7 of 147 lines
- `src/editor/commit.ts#commitRoadPath` — 26 of 97 lines
- `src/editor/commit.ts#dryRunKey` — 4 of 4 lines
- `src/editor/commit.ts#joinSegments` — 12 of 47 lines
- `src/editor/roadParking.ts#roadParking` — 2 of 11 lines
- `src/editor/roadTool.ts#RoadToolHost` — 12 of 36 lines
- `src/editor/roads/profile.ts#applyProfileTo` — 8 of 22 lines
- `src/editor/streetObjects.ts#commitUturn` — 4 of 34 lines
- `src/editor/streetscapeTool.ts#StreetscapeTool.down` — 8 of 43 lines
- `src/lab/inspector/entity.ts#agentsNear` — 6 of 19 lines
- `src/lab/inspector/entity.ts#entityInfo` — 14 of 93 lines
- `src/main.ts#frame` — 3 of 102 lines
- `src/main.ts#startPopulationShare` — 1 of 1 lines
- `src/main.ts#syncPopulationShare` — 3 of 3 lines
- `src/render/blast.ts#createBlast` — 2 of 543 lines
- `src/render/blast.ts#createBlast.explode` — 9 of 37 lines
- `src/render/blast.ts#createBlast.stepRope` — 5 of 33 lines
- `src/render/blast.ts#createBlast.update` — 17 of 136 lines
- `src/render/destruction.ts#createDestruction` — 3 of 343 lines
- `src/render/destruction.ts#createDestruction.hit` — 5 of 17 lines
- `src/render/destruction.ts#createDestruction.loosen` — 5 of 18 lines
- `src/render/destruction.ts#createDestruction.strike` — 8 of 55 lines
- `src/render/destruction.ts#createDestruction.update` — 12 of 40 lines
- `src/render/elements.ts#EFFECTS` — 9 of 9 lines
- `src/render/elements.ts#ElementLayer` — 3 of 11 lines
- `src/render/elements.ts#createElementLayer.give` — 10 of 10 lines
- `src/render/paintQuads.ts#PAINT_COLOURS` — 1 of 1 lines
- `src/render/playEffects.ts#PlayEffects` — 3 of 35 lines
- `src/render/playEffects.ts#PlayEffectsContext` — 4 of 17 lines
- `src/render/playEffects.ts#createPlayEffects` — 20 of 417 lines
- `src/render/playEffects.ts#createPlayEffects.dispose` — 3 of 8 lines
- `src/render/playEffects.ts#createPlayEffects.dropVehicle` — 9 of 14 lines
- `src/render/playEffects.ts#createPlayEffects.explode` — 29 of 143 lines
- `src/render/playEffects.ts#createPlayEffects.vehicleHit` — 7 of 13 lines
- `src/render/playEffects.ts#createPlayEffects.wound` — 3 of 12 lines
- `src/render/ragdoll.ts#createRagdolls` — 2 of 1556 lines
- `src/render/ragdoll.ts#createRagdolls.collide` — 14 of 70 lines
- `src/render/ragdoll.ts#createRagdolls.detach` — 3 of 43 lines
- `src/render/ragdoll.ts#createRagdolls.mush` — 3 of 21 lines
- `src/render/ragdoll.ts#createRagdolls.openBelly` — 3 of 8 lines
- `src/render/ragdoll.ts#createRagdolls.shootBody` — 3 of 51 lines
- `src/render/ragdoll.ts#createRagdolls.spawn` — 8 of 89 lines
- `src/render/ragdoll.ts#createRagdolls.step` — 7 of 68 lines
- `src/render/ragdoll.ts#createRagdolls.sync` — 7 of 35 lines
- `src/render/renderer.ts#BlastHit` — 3 of 11 lines
- `src/render/renderer.ts#SceneHandle` — 6 of 170 lines
- `src/render/renderer.ts#createSceneRenderer` — 81 of 3774 lines
- `src/render/renderer.ts#createSceneRenderer.movingCasters` — 1 of 1 lines
- `src/render/renderer.ts#createSceneRenderer.switchedOff` — 16 of 16 lines
- `src/render/roadSurfaceInputs.ts#SurfaceInputs.placeSegment` — 8 of 49 lines
- `src/render/roadSurfaceInputs.ts#SurfaceInputs.refresh` — 7 of 47 lines
- `src/render/signals.ts#Head` — 3 of 9 lines
- `src/render/signals.ts#createSignalHeads` — 5 of 626 lines
- `src/sim/ambient/ambient.ts#AmbientWorld.behindWall` — 4 of 6 lines
- `src/sim/ambient/ambient.ts#AmbientWorld.buildingMix` — 1 of 1 lines
- `src/sim/ambient/ambient.ts#AmbientWorld.company` — 7 of 17 lines
- `src/sim/ambient/ambient.ts#AmbientWorld.edgePeople` — 33 of 53 lines
- `src/sim/ambient/ambient.ts#AmbientWorld.isParked` — 3 of 3 lines
- `src/sim/ambient/ambient.ts#AmbientWorld.keepWalking` — 15 of 20 lines
- `src/sim/ambient/ambient.ts#AmbientWorld.lanesAround` — 7 of 9 lines
- `src/sim/ambient/ambient.ts#AmbientWorld.listLanes` — 8 of 12 lines
- `src/sim/ambient/ambient.ts#AmbientWorld.logParked` — 3 of 5 lines
- `src/sim/ambient/ambient.ts#AmbientWorld.mixAround` — 4 of 4 lines
- `src/sim/ambient/ambient.ts#AmbientWorld.moverCells` — 10 of 10 lines
- `src/sim/ambient/ambient.ts#AmbientWorld.movers` — 1 of 1 lines
- `src/sim/ambient/ambient.ts#AmbientWorld.open` — 5 of 9 lines
- `src/sim/ambient/ambient.ts#AmbientWorld.openCars` — 9 of 14 lines
- `src/sim/ambient/ambient.ts#AmbientWorld.openingLanes` — 3 of 4 lines
- `src/sim/ambient/ambient.ts#AmbientWorld.parkInWindow` — 17 of 22 lines
- `src/sim/ambient/ambient.ts#AmbientWorld.parked` — 1 of 1 lines
- `src/sim/ambient/ambient.ts#AmbientWorld.parkedAt` — 1 of 1 lines
- `src/sim/ambient/ambient.ts#AmbientWorld.parkedLog` — 1 of 1 lines
- `src/sim/ambient/ambient.ts#AmbientWorld.parkedSince` — 2 of 5 lines
- `src/sim/ambient/ambient.ts#AmbientWorld.parking` — 17 of 20 lines
- `src/sim/ambient/ambient.ts#AmbientWorld.people` — 16 of 40 lines
- `src/sim/ambient/ambient.ts#AmbientWorld.peopleTarget` — 6 of 11 lines
- `src/sim/ambient/ambient.ts#AmbientWorld.pickPlace` — 7 of 8 lines
- `src/sim/ambient/ambient.ts#AmbientWorld.placesReady` — 3 of 4 lines
- `src/sim/ambient/ambient.ts#AmbientWorld.refreshPlaces` — 3 of 3 lines
- `src/sim/ambient/ambient.ts#AmbientWorld.removeParked` — 9 of 10 lines
- `src/sim/ambient/ambient.ts#AmbientWorld.reset` — 4 of 34 lines
- `src/sim/ambient/ambient.ts#AmbientWorld.roadAround` — 3 of 3 lines
- `src/sim/ambient/ambient.ts#AmbientWorld.stand` — 33 of 38 lines
- `src/sim/ambient/ambient.ts#AmbientWorld.standSource` — 8 of 16 lines
- `src/sim/ambient/ambient.ts#AmbientWorld.standing` — 1 of 1 lines
- `src/sim/ambient/ambient.ts#AmbientWorld.step` — 10 of 41 lines
- `src/sim/ambient/ambient.ts#AmbientWorld.step.density` — 3 of 3 lines
- `src/sim/ambient/ambient.ts#AmbientWorld.street` — 1 of 1 lines
- `src/sim/ambient/ambient.ts#AmbientWorld.takeScenery` — 5 of 8 lines
- `src/sim/ambient/ambient.ts#AmbientWorld.traffic` — 29 of 45 lines
- `src/sim/ambient/ambient.ts#AmbientWorld.trimToCeilings` — 16 of 25 lines
- `src/sim/ambient/ambient.ts#AmbientWorld.walkFrom` — 28 of 36 lines
- `src/sim/ambient/ambient.ts#AmbientWorld.walls` — 1 of 1 lines
- `src/sim/ambient/ambient.ts#PARKED_KINDS` — 1 of 1 lines
- `src/sim/ambient/ambient.ts#REVERSED` — 1 of 1 lines
- `src/sim/ambient/ambient.ts#SceneryCar` — 4 of 9 lines
- `src/sim/ambient/ambient.ts#TRAFFIC` — 3 of 3 lines
- `src/sim/ambient/ambient.ts#newPlaces` — 1 of 1 lines
- `src/sim/ambient/ambient.ts#reversed` — 5 of 5 lines
- `src/sim/city/city.ts#City.lots` — 1 of 1 lines
- `src/sim/city/city.ts#City.step` — 3 of 6 lines
- `src/sim/clock.ts#SimClock.advance` — 12 of 27 lines
- `src/sim/crossings/state.ts#CrossingStates` — 1 of 1 lines
- `src/sim/host/protocol.ts#FromSim` — 12 of 12 lines
- `src/sim/host/protocol.ts#MAX_PEDS` — 1 of 1 lines
- `src/sim/host/protocol.ts#MAX_VEHICLES` — 1 of 1 lines
- `src/sim/host/protocol.ts#PEDS` — 1 of 1 lines
- `src/sim/host/protocol.ts#SLOT` — 1 of 1 lines
- `src/sim/host/protocol.ts#SignalLights` — 3 of 5 lines
- `src/sim/host/protocol.ts#ToSim` — 19 of 19 lines
- `src/sim/host/protocol.ts#VEHICLES` — 1 of 1 lines
- `src/sim/host/protocol.ts#VehicleRich` — 3 of 10 lines
- `src/sim/host/protocol.ts#headBase` — 1 of 1 lines
- `src/sim/host/protocol.ts#makeSharedFrames` — 3 of 3 lines
- `src/sim/host/protocol.ts#pedBase` — 1 of 1 lines
- `src/sim/host/protocol.ts#vehicleBase` — 1 of 1 lines
- `src/sim/host/simAnswers.ts#answer` — 6 of 9 lines
- `src/sim/host/simAnswers.ts#count` — 18 of 19 lines
- `src/sim/host/simAnswers.ts#junctionAnswer` — 18 of 24 lines
- `src/sim/host/simAnswers.ts#roadLanes` — 9 of 11 lines
- `src/sim/host/simClient.ts#SignalFeed` — 3 of 9 lines
- `src/sim/host/simClient.ts#SimClient.applyParked` — 5 of 24 lines
- `src/sim/host/simClient.ts#SimClient.clearParked` — 4 of 10 lines
- `src/sim/host/simClient.ts#SimClient.clearViews` — 4 of 20 lines
- `src/sim/host/simClient.ts#SimClient.dropParked` — 3 of 12 lines
- `src/sim/host/simClient.ts#SimClient.keepMeso` — 4 of 7 lines
- `src/sim/host/simClient.ts#SimClient.lights` — 1 of 1 lines
- `src/sim/host/simClient.ts#SimClient.load` — 3 of 5 lines
- `src/sim/host/simClient.ts#SimClient.mesoDeltas` — 1 of 1 lines
- `src/sim/host/simClient.ts#SimClient.readTick` — 10 of 160 lines
- `src/sim/host/simClient.ts#SimClient.resync` — 3 of 4 lines
- `src/sim/host/simClient.ts#SimClient.sendChanges` — 5 of 65 lines
- `src/sim/host/simClient.ts#SimClient.start` — 4 of 24 lines
- `src/sim/host/simClient.ts#VehicleView` — 4 of 25 lines
- `src/sim/host/simClient.ts#newView` — 5 of 7 lines
- `src/sim/host/simHost.ts#SimHost.act` — 15 of 32 lines
- `src/sim/host/simHost.ts#SimHost.buildings` — 1 of 1 lines
- `src/sim/host/simHost.ts#SimHost.frame` — 8 of 39 lines
- `src/sim/host/simHost.ts#SimHost.governor` — 1 of 1 lines
- `src/sim/host/simHost.ts#SimHost.handle` — 12 of 25 lines
- `src/sim/host/simHost.ts#SimHost.init` — 18 of 32 lines
- `src/sim/host/simHost.ts#SimHost.load` — 3 of 12 lines
- `src/sim/host/simHost.ts#SimHost.meso` — 1 of 1 lines
- `src/sim/host/simHost.ts#SimHost.odometer` — 1 of 1 lines
- `src/sim/host/simHost.ts#SimHost.parked` — 11 of 21 lines
- `src/sim/host/simHost.ts#SimHost.planId` — 5 of 5 lines
- `src/sim/host/simHost.ts#SimHost.publish` — 14 of 80 lines
- `src/sim/host/simHost.ts#SimHost.publish.writeVehicle` — 32 of 32 lines
- `src/sim/host/simHost.ts#SimHost.signals.layoutOf` — 5 of 5 lines
- `src/sim/host/simHost.ts#SimHost.signals.tell` — 7 of 7 lines
- `src/sim/host/simHost.ts#SimHost.stats` — 7 of 14 lines
- `src/sim/host/simHost.ts#SimHost.tellEvery` — 3 of 3 lines
- `src/sim/host/simHost.ts#archetypeMap` — 3 of 3 lines
- `src/sim/invariants.ts#AuditMemory` — 3 of 13 lines
- `src/sim/invariants.ts#auditedSignals` — 10 of 11 lines
- `src/sim/invariants.ts#everySignal` — 2 of 3 lines
- `src/sim/invariants.ts#followSignals` — 5 of 14 lines
- `src/sim/invariants.ts#maxSignalCycle` — 6 of 6 lines
- `src/sim/invariants.ts#onsetsOf` — 7 of 7 lines
- `src/sim/invariants.ts#refileCycles` — 3 of 7 lines
- `src/sim/invariants.ts#runAudit` — 180 of 225 lines
- `src/sim/peds/sidewalk.ts#SidewalkEdge` — 4 of 15 lines
- `src/sim/peds/sidewalk.ts#SidewalkGraph.build` — 3 of 6 lines
- `src/sim/peds/sidewalk.ts#SidewalkGraph.crossings` — 1 of 1 lines
- `src/sim/peds/sidewalk.ts#SidewalkGraph.crossingsAt` — 13 of 33 lines
- `src/sim/peds/sidewalk.ts#SidewalkGraph.kerbsAt` — 3 of 39 lines
- `src/sim/peds/sidewalk.ts#SidewalkGraph.ofNode` — 1 of 1 lines
- `src/sim/peds/sidewalk.ts#SidewalkGraph.update` — 8 of 33 lines
- `src/sim/peds/signalPostIndex.ts#SignalPostIndex.reset` — 3 of 5 lines
- `src/sim/people/engine.ts#PeopleBridge` — 4 of 12 lines
- `src/sim/pipeline.ts#collectGhosts` — 16 of 25 lines
- `src/sim/pipeline.ts#ensureVehicleRoutes` — 49 of 61 lines
- `src/sim/pipeline.ts#pinReservedRoute` — 33 of 39 lines
- `src/sim/pipeline.ts#rebindVehicles` — 28 of 34 lines
- `src/sim/pipeline.ts#reconcileReservedChain` — 13 of 14 lines
- `src/sim/pipeline.ts#reservedChainValid` — 48 of 58 lines
- `src/sim/pipeline.ts#runWatchdog` — 21 of 21 lines
- `src/sim/pipeline.ts#stallCounters` — 26 of 29 lines
- `src/sim/pipeline.ts#step` — 19 of 100 lines
- `src/sim/pipeline.ts#updateStallCounters` — 5 of 10 lines
- `src/sim/pose.ts#axleFrame` — 4 of 12 lines
- `src/sim/pose.ts#bodyFrame` — 21 of 23 lines
- `src/sim/pose.ts#bodyOffset` — 5 of 5 lines
- `src/sim/pose.ts#vehicleFramesInto` — 21 of 24 lines
- `src/sim/pose.ts#vehiclePose` — 3 of 4 lines
- `src/sim/pose.ts#vehiclePoseInto` — 24 of 33 lines
- `src/sim/topologyCatchUp.ts#TopologyCatchUp.constructor` — 1 of 1 lines
- `src/sim/transit/transit.ts#Bus` — 3 of 10 lines
- `src/sim/transit/transit.ts#TransitSim.followLanes` — 15 of 48 lines
- `src/sim/transit/transit.ts#TransitSim.placeBuses` — 7 of 20 lines
- `src/sim/transit/transit.ts#TransitSim.rebuild` — 6 of 37 lines
- `src/sim/transit/transit.ts#TransitSim.sendBus` — 11 of 17 lines
- `src/sim/transit/transit.ts#TransitSim.spawnBus` — 20 of 23 lines
- `src/sim/transit/transit.ts#TransitSim.step` — 5 of 10 lines
- `src/sim/transit/transit.ts#TransitSim.stepBus` — 19 of 44 lines
- `src/sim/transit/transit.ts#TransitSim.stopLanes` — 1 of 1 lines
- `src/sim/transit/transit.ts#TransitSim.stopPoint` — 6 of 7 lines
- `src/sim/transit/transit.ts#levelCrossings` — 15 of 21 lines
- `src/sim/world.ts#CAR_CLASS` — 1 of 1 lines
- `src/sim/world.ts#LaneletRuntime` — 5 of 11 lines
- `src/sim/world.ts#NO_RUNTIME` — 5 of 7 lines
- `src/sim/world.ts#PlanInputs` — 4 of 9 lines
- `src/sim/world.ts#SimWorld.advisor` — 1 of 1 lines
- `src/sim/world.ts#SimWorld.applyAutoControl` — 16 of 18 lines
- `src/sim/world.ts#SimWorld.asleep` — 1 of 1 lines
- `src/sim/world.ts#SimWorld.awake` — 1 of 1 lines
- `src/sim/world.ts#SimWorld.bodiesIn` — 24 of 24 lines
- `src/sim/world.ts#SimWorld.busNear` — 15 of 16 lines
- `src/sim/world.ts#SimWorld.claims` — 1 of 1 lines
- `src/sim/world.ts#SimWorld.clearShadow` — 9 of 9 lines
- `src/sim/world.ts#SimWorld.conflicts` — 1 of 1 lines
- `src/sim/world.ts#SimWorld.connector` — 3 of 3 lines
- `src/sim/world.ts#SimWorld.constructor` — 3 of 9 lines
- `src/sim/world.ts#SimWorld.controlAdvisor` — 1 of 1 lines
- `src/sim/world.ts#SimWorld.controller` — 2 of 3 lines
- `src/sim/world.ts#SimWorld.controllers` — 1 of 1 lines
- `src/sim/world.ts#SimWorld.countsAt` — 4 of 5 lines
- `src/sim/world.ts#SimWorld.crossingSpans` — 1 of 1 lines
- `src/sim/world.ts#SimWorld.crossingsAt` — 7 of 10 lines
- `src/sim/world.ts#SimWorld.enterLanelet` — 17 of 19 lines
- `src/sim/world.ts#SimWorld.entryDemand` — 1 of 1 lines
- `src/sim/world.ts#SimWorld.exitLanelet` — 7 of 7 lines
- `src/sim/world.ts#SimWorld.fileJunctions` — 8 of 9 lines
- `src/sim/world.ts#SimWorld.flow` — 1 of 1 lines
- `src/sim/world.ts#SimWorld.flowStats` — 1 of 1 lines
- `src/sim/world.ts#SimWorld.forgetSignals` — 3 of 6 lines
- `src/sim/world.ts#SimWorld.ghostCandidates` — 1 of 1 lines
- `src/sim/world.ts#SimWorld.graph` — 1 of 1 lines
- `src/sim/world.ts#SimWorld.inNodeOrder` — 9 of 10 lines
- `src/sim/world.ts#SimWorld.junctionHolders` — 9 of 9 lines
- `src/sim/world.ts#SimWorld.junctionOfLane` — 5 of 5 lines
- `src/sim/world.ts#SimWorld.junctionQuiet` — 12 of 12 lines
- `src/sim/world.ts#SimWorld.keepMicroWindow` — 5 of 24 lines
- `src/sim/world.ts#SimWorld.laneReaches` — 1 of 1 lines
- `src/sim/world.ts#SimWorld.lanelet` — 3 of 3 lines
- `src/sim/world.ts#SimWorld.makeSignalDeps` — 10 of 72 lines
- `src/sim/world.ts#SimWorld.makeSignalDeps.reservedGroups` — 20 of 24 lines
- `src/sim/world.ts#SimWorld.maybeHolders` — 1 of 1 lines
- `src/sim/world.ts#SimWorld.mergeTurn` — 1 of 1 lines
- `src/sim/world.ts#SimWorld.moveWindowSteps` — 5 of 9 lines
- `src/sim/world.ts#SimWorld.noteHolder` — 3 of 3 lines
- `src/sim/world.ts#SimWorld.noteJunctionsReached` — 3 of 20 lines
- `src/sim/world.ts#SimWorld.noteJunctionsReached.note` — 3 of 5 lines
- `src/sim/world.ts#SimWorld.noteRoute` — 3 of 3 lines
- `src/sim/world.ts#SimWorld.noteSignal` — 9 of 12 lines
- `src/sim/world.ts#SimWorld.noteSignalStepped` — 3 of 3 lines
- `src/sim/world.ts#SimWorld.occupied` — 1 of 1 lines
- `src/sim/world.ts#SimWorld.ordered` — 1 of 1 lines
- `src/sim/world.ts#SimWorld.planInputsOf` — 10 of 10 lines
- `src/sim/world.ts#SimWorld.prepGraph` — 1 of 1 lines
- `src/sim/world.ts#SimWorld.prepReach` — 4 of 5 lines
- `src/sim/world.ts#SimWorld.prepareVehicleTopology` — 21 of 29 lines
- `src/sim/world.ts#SimWorld.rebindLanes` — 1 of 1 lines
- `src/sim/world.ts#SimWorld.rebuildVehicleTopology` — 37 of 45 lines
- `src/sim/world.ts#SimWorld.rebuildWalkTopology` — 11 of 15 lines
- `src/sim/world.ts#SimWorld.removeVehicle` — 7 of 7 lines
- `src/sim/world.ts#SimWorld.reservationsChecked` — 4 of 4 lines
- `src/sim/world.ts#SimWorld.reset` — 11 of 45 lines
- `src/sim/world.ts#SimWorld.restSignal` — 5 of 5 lines
- `src/sim/world.ts#SimWorld.routes` — 1 of 1 lines
- `src/sim/world.ts#SimWorld.rt` — 3 of 3 lines
- `src/sim/world.ts#SimWorld.runtime` — 1 of 1 lines
- `src/sim/world.ts#SimWorld.runtimeFor` — 9 of 9 lines
- `src/sim/world.ts#SimWorld.signalClock` — 1 of 1 lines
- `src/sim/world.ts#SimWorld.signalDemand` — 29 of 31 lines
- `src/sim/world.ts#SimWorld.signalisedControllers` — 2 of 3 lines
- `src/sim/world.ts#SimWorld.signalsToStep` — 9 of 36 lines
- `src/sim/world.ts#SimWorld.sortLane` — 8 of 15 lines
- `src/sim/world.ts#SimWorld.stepControlAdvisor` — 20 of 26 lines
- `src/sim/world.ts#SimWorld.stepping` — 1 of 1 lines
- `src/sim/world.ts#SimWorld.syncControllers` — 34 of 38 lines
- `src/sim/world.ts#SimWorld.topologyLog` — 1 of 1 lines
- `src/sim/world.ts#SimWorld.useMicroWindow` — 3 of 5 lines
- `src/sim/world.ts#SimWorld.veh` — 3 of 3 lines
- `src/sim/world.ts#SimWorld.vehicles` — 5 of 5 lines
- `src/sim/world.ts#SimWorld.vehiclesInIdOrder` — 2 of 3 lines
- `src/sim/world.ts#SimWorld.vehiclesReached` — 14 of 22 lines
- `src/sim/world.ts#SimWorld.vehiclesReached.take` — 1 of 1 lines
- `src/sim/world.ts#SimWorld.wakeEverySignal` — 3 of 5 lines
- `src/sim/world.ts#SimWorld.windowMoved` — 3 of 6 lines
- `src/sim/world.ts#WorldSignalDeps` — 1 of 1 lines
- `src/sim/world.ts#settingsKey` — 4 of 4 lines
- `src/ui/inspector.ts#InspectorActions` — 6 of 43 lines
- `src/ui/inspector.ts#junctionHost` — 2 of 12 lines
- `src/ui/inspector.ts#parkingSelect` — 5 of 5 lines
- `src/ui/inspector.ts#renderCurrent` — 5 of 43 lines
- `src/ui/inspector.ts#renderNode` — 7 of 61 lines
- `src/ui/inspector.ts#renderSegment` — 31 of 134 lines
- `src/ui/inspector.ts#renderSegment.setParking` — 3 of 4 lines
- `src/ui/roads/profileEditor.ts#newElement` — 6 of 11 lines
- `src/world/doc.ts#RoadDoc.addSegment` — 8 of 34 lines
- `src/world/doc.ts#RoadDoc.setNodeApproachRules` — 6 of 10 lines
- `src/world/doc.ts#RoadDoc.setNodeSignal` — 6 of 10 lines
- `src/world/doc.ts#RoadDoc.setSegmentParking` — 6 of 11 lines
- `src/world/doc.ts#RoadNode` — 4 of 25 lines
- `src/world/doc.ts#RoadSegment` — 3 of 21 lines
- `src/world/doc.ts#SerializedDoc` — 18 of 75 lines
- `src/world/doc.ts#copyNode` — 5 of 13 lines
- `src/world/doc.ts#finishNode` — 6 of 15 lines
- `src/world/doc.ts#fitRoadCurve` — 5 of 7 lines
- `src/world/doc.ts#sameNode` — 7 of 7 lines
- `src/world/doc.ts#sameSegment` — 6 of 6 lines
- `src/world/doc.ts#savedNode` — 5 of 12 lines
- `src/world/doc.ts#segmentRecord` — 6 of 19 lines
- `src/world/junctionBuild.ts#JunctionInput` — 4 of 8 lines
- `src/world/junctionBuild.ts#JunctionLane` — 2 of 3 lines
- `src/world/junctionBuild.ts#MadeConnector` — 5 of 13 lines
- `src/world/junctionBuild.ts#buildJunctionOf` — 20 of 32 lines
- `src/world/junctionBuild.ts#buildJunctionOf.lane` — 1 of 1 lines
- `src/world/junctionBuild.ts#buildJunctionOf.ring` — 4 of 5 lines
- `src/world/junctionBuild.ts#buildJunctionOf.surfaceOf` — 1 of 1 lines
- `src/world/junctionBuild.ts#carriedPair` — 20 of 27 lines
- `src/world/junctionBuild.ts#classifyTurn` — 2 of 8 lines
- `src/world/junctionBuild.ts#fallbackTurnRank` — 5 of 6 lines
- `src/world/junctionBuild.ts#junctionKey` — 8 of 9 lines
- `src/world/junctionBuild.ts#junctionMovements` — 75 of 139 lines
- `src/world/junctionBuild.ts#junctionMovements.addConnector` — 21 of 21 lines
- `src/world/junctionBuild.ts#junctionMovements.addConnector.shape` — 5 of 5 lines
- `src/world/junctionBuild.ts#junctionMovements.leg` — 5 of 5 lines
- `src/world/junctionBuild.ts#laneIsPlausible` — 9 of 13 lines
- `src/world/junctionBuild.ts#legLine` — 4 of 4 lines
- `src/world/junctionBuild.ts#shouldSignalise` — 9 of 13 lines
- `src/world/junctionBuild.ts#turnSpeedFactor` — 2 of 2 lines
- `src/world/junctionTurns.ts#JunctionTurns.entries` — 2 of 3 lines
- `src/world/junctionTurns.ts#JunctionTurns.made` — 1 of 1 lines
- `src/world/junctionTurns.ts#JunctionTurns.of` — 3 of 5 lines
- `src/world/junctionTurns.ts#TurnPlan.asks` — 1 of 1 lines
- `src/world/junctionTurns.ts#TurnPlan.chosen` — 1 of 1 lines
- `src/world/junctionTurns.ts#TurnPlan.surfaces` — 1 of 1 lines
- `src/world/markings.ts#approachLegendsAt` — 14 of 30 lines
- `src/world/roadTypes.ts#RoadType` — 4 of 52 lines
- `src/world/roadTypes.ts#resolveProfile` — 16 of 24 lines
- `src/world/roadTypes.ts#resolveProfile.fit` — 1 of 1 lines
- `src/world/roadTypes.ts#roadProfile` — 8 of 13 lines
- `src/world/roads/catalog.ts#shapeProfile.kerb` — 4 of 4 lines
- `src/world/roads/derivedSigns.ts#signApproach` — 5 of 17 lines
- `src/world/roads/derivedSigns.ts#signsAtNode` — 13 of 35 lines
- `src/world/roads/laneEnds.ts#laneEnds` — 14 of 15 lines
- `src/world/roads/laneEnds.ts#nodeLanes` — 12 of 17 lines
- `src/world/roads/laneEnds.ts#nodeLanes.lane` — 4 of 4 lines
- `src/world/roads/profile.ts#AppliedProfile` — 3 of 8 lines
- `src/world/roads/profile.ts#ProfileSource` — 3 of 7 lines
- `src/world/roads/profile.ts#authoredFields.kindAt` — 1 of 1 lines
- `src/world/roads/profile.ts#profileOf.band` — 4 of 4 lines
- `src/world/turnChoice.ts#chooseTurnOf` — 7 of 7 lines

## Resources

- shared entry `walk` — src/render/citizenBake.ts:35: `walk: WALK`
- feature entry `walkElder` — src/render/citizenBake.ts:35: `walkElder: WALK_ELDER`
- shared entry `walkShuffle` — src/render/citizenBake.ts:35: `walkShuffle: WALK_SHUFFLE`
- feature entry `walkHandL` — src/render/citizenBake.ts:36: `walkHandL: WALK_SHUFFLE + 1 + LIBRARY.length + DIRECTIONAL.length + RIDER_CLIPS.length`
- feature entry `walkHandR` — src/render/citizenBake.ts:37: `walkHandR: WALK_SHUFFLE + 2 + LIBRARY.length + DIRECTIONAL.length + RIDER_CLIPS.length`
- feature entry `read` — src/render/citizenGait.ts:389: `read: 'phone'`
- feature entry `bag` — src/render/citizenGait.ts:389: `bag: 'bag'`
- feature entry `trolley` — src/render/citizenGait.ts:389: `trolley: 'trolley'`
- feature entry `umbrella` — src/render/citizenGait.ts:389: `umbrella: 'umbrella'`
- feature entry `cheer` — src/render/citizenGait.ts:389: `cheer: 'cheer'`
- feature entry `dance` — src/render/citizenGait.ts:389: `dance: 'dance'`
- feature entry `wave` — src/render/citizenGait.ts:390: `wave: 'wave'`
- feature entry `drink` — src/render/citizenGait.ts:390: `drink: 'drink'`
- feature entry `photo` — src/render/citizenGait.ts:390: `photo: 'photo'`
- feature entry `laugh` — src/render/citizenGait.ts:390: `laugh: 'laugh'`
- feature entry `argue` — src/render/citizenGait.ts:390: `argue: 'argue'`
- feature entry `knock` — src/render/citizenGait.ts:390: `knock: 'knock'`
- feature entry `headphones` — src/render/citizenGait.ts:391: `headphones: 'headphones'`
- feature entry `eat` — src/render/citizenGait.ts:391: `eat: 'eatIdle'`
- feature entry `work` — src/render/citizenGait.ts:391: `work: 'workTable'`
- shared entry `male` — src/render/citizenWalk.ts:76: `male: sourceOf(WALKS.male)`
- shared entry `female` — src/render/citizenWalk.ts:76: `female: sourceOf(WALKS.female)`
- shared entry `male` — src/render/citizenWalk.ts:81: `male: 1.495864`
- shared entry `female` — src/render/citizenWalk.ts:81: `female: 1.596625`
- feature entry `Float32Array` — src/render/people/cookPack.ts:7: `Float32Array`
- feature entry `Float64Array` — src/render/people/cookPack.ts:7: `Float64Array`
- feature entry `Int32Array` — src/render/people/cookPack.ts:7: `Int32Array`
- feature entry `Uint32Array` — src/render/people/cookPack.ts:7: `Uint32Array`
- feature entry `Int16Array` — src/render/people/cookPack.ts:7: `Int16Array`
- feature entry `Uint16Array` — src/render/people/cookPack.ts:7: `Uint16Array`
- feature entry `Uint8Array` — src/render/people/cookPack.ts:7: `Uint8Array`
- shared entry `skin` — src/render/people/crowdLod.ts:35: `skin: [Infinity, 2000, 600, 0]`
- shared entry `outfit` — src/render/people/crowdLod.ts:36: `outfit: [Infinity, 1200, 400, 0]`
- shared entry `garment` — src/render/people/crowdLod.ts:38: `garment: [Infinity, 900, 300, 0]`
- shared entry `shoes` — src/render/people/crowdLod.ts:39: `shoes: [Infinity, 300, 80, 0]`
- shared entry `hair` — src/render/people/crowdLod.ts:41: `hair: [SKIN_TRIANGLES_LEVEL0, 1000, 300, 0]`
- shared entry `brows` — src/render/people/crowdLod.ts:42: `brows: [Infinity, 100, 0, 0]`
- shared entry `lashes` — src/render/people/crowdLod.ts:43: `lashes: [Infinity, 0, 0, 0]`
- shared entry `mouth` — src/render/people/crowdLod.ts:44: `mouth: [Infinity, 0, 0, 0]`
- shared entry `accessory` — src/render/people/crowdLod.ts:45: `accessory: [Infinity, 200, 60, 0]`
- feature entry `talk` — src/render/people/faceExpression.ts:45: `talk: { channel: 'faceJoy', amount: 0.25, talk: true }`
- feature entry `laugh` — src/render/people/faceExpression.ts:47: `laugh: { channel: 'faceJoy', amount: 1, laugh: true }`
- feature entry `cheer` — src/render/people/faceExpression.ts:48: `cheer: { channel: 'faceJoy', amount: 0.9, laugh: true }`
- feature entry `wave` — src/render/people/faceExpression.ts:49: `wave: { channel: 'faceJoy', amount: 0.6 }`
- feature entry `argue` — src/render/people/faceExpression.ts:50: `argue: { channel: 'faceAnger', amount: 0.8, talk: true }`
- feature entry `phone` — src/render/people/faceExpression.ts:52: `phone: { amount: 0, talk: true }`
- feature entry `dance` — src/render/people/faceExpression.ts:53: `dance: { channel: 'faceJoy', amount: 0.7 }`
- feature entry `read` — src/render/people/heldProps.ts:14: `read: { kind: 'newspaper' }`
- feature entry `drink` — src/render/people/heldProps.ts:15: `drink: { kind: 'cup' }`
- feature entry `phone` — src/render/people/heldProps.ts:16: `phone: { kind: 'phone' }`
- feature entry `photo` — src/render/people/heldProps.ts:17: `photo: { kind: 'camera' }`
- feature entry `bag` — src/render/people/heldProps.ts:18: `bag: { kind: 'bag' }`
- feature entry `umbrella` — src/render/people/heldProps.ts:19: `umbrella: { kind: 'umbrella' }`
- feature entry `cup` — src/render/people/heldProps.ts:82: `cup: [0.07, 0.0, -0.05]`
- feature entry `bag` — src/render/people/heldProps.ts:83: `bag: [0.08, 0.0, -0.02]`
- feature entry `umbrella` — src/render/people/heldProps.ts:84: `umbrella: [0.06, 0.0, -0.03]`
- shared entry `child` — src/render/people/proceduralBake.ts:29: `child: 9`
- shared entry `young` — src/render/people/proceduralBake.ts:29: `young: 22`
- shared entry `adult` — src/render/people/proceduralBake.ts:29: `adult: 42`
- shared entry `senior` — src/render/people/proceduralBake.ts:29: `senior: 72`
- feature entry `head` — src/render/people/proceduralCrowd.ts:66: `head: ['Bip01_Head']`
- feature entry `torso` — src/render/people/proceduralCrowd.ts:66: `torso: ['Bip01_Spine', 'Bip01_Spine1', 'Bip01_Spine2', 'Bip01_Pelvis']`
- feature entry `armL` — src/render/people/proceduralCrowd.ts:67: `armL: ['Bip01_L_UpperArm', 'Bip01_L_Forearm']`
- feature entry `armR` — src/render/people/proceduralCrowd.ts:67: `armR: ['Bip01_R_UpperArm', 'Bip01_R_Forearm']`
- feature entry `legL` — src/render/people/proceduralCrowd.ts:68: `legL: ['Bip01_L_Thigh', 'Bip01_L_Calf']`
- feature entry `legR` — src/render/people/proceduralCrowd.ts:68: `legR: ['Bip01_R_Thigh', 'Bip01_R_Calf']`
- feature entry `m` — src/sim/people/view.ts:45: `m: 2.9`
- feature entry `f` — src/sim/people/view.ts:45: `f: 4.9`
- feature entry `m` — src/sim/people/view.ts:46: `m: 2.5`
- feature entry `f` — src/sim/people/view.ts:46: `f: 3.33`
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
- feature entry `inspector.numbers` — src/ui/i18n/en.ts:307: `'inspector.numbers': 'Numbers {from} to {to} · even on the right going up'`
- feature entry `inspector.direction` — src/ui/i18n/en.ts:592: `'inspector.direction': 'Traffic direction'`
- feature entry `inspector.laneCount` — src/ui/i18n/en.ts:593: `'inspector.laneCount': 'Travel lanes'`
- feature entry `inspector.curvePosition` — src/ui/i18n/en.ts:594: `'inspector.curvePosition': 'Curve position'`
- feature entry `inspector.heightStart` — src/ui/i18n/en.ts:595: `'inspector.heightStart': 'Start height (m)'`
- feature entry `inspector.heightEnd` — src/ui/i18n/en.ts:596: `'inspector.heightEnd': 'End height (m)'`
- feature entry `inspector.heightNode` — src/ui/i18n/en.ts:601: `'inspector.heightNode': 'Road height (m)'`
- feature entry `inspector.upgrade` — src/ui/i18n/en.ts:613: `'inspector.upgrade': 'Upgrade class'`
- feature entry `inspector.duplicate` — src/ui/i18n/en.ts:614: `'inspector.duplicate': 'Duplicate road'`
- feature entry `inspector.roadClass` — src/ui/i18n/en.ts:620: `'inspector.roadClass': 'Road class'`
- feature entry `inspector.curvature` — src/ui/i18n/en.ts:621: `'inspector.curvature': 'Curvature'`
- feature entry `inspector.reverse` — src/ui/i18n/en.ts:630: `'inspector.reverse': 'Reverse one-way'`
- feature entry `inspector.duplicateHint` — src/ui/i18n/en.ts:631: `'inspector.duplicateHint': 'Duplicate road (Ctrl/Cmd+D)'`
- feature entry `inspector.splitMiddle` — src/ui/i18n/en.ts:632: `'inspector.splitMiddle': 'Split at the middle'`
- feature entry `inspector.addZebra` — src/ui/i18n/en.ts:633: `'inspector.addZebra': 'Zebra crossing'`
- feature entry `inspector.addSignalCrossing` — src/ui/i18n/en.ts:634: `'inspector.addSignalCrossing': 'Signal crossing'`
- feature entry `inspector.addUturnToEnd` — src/ui/i18n/en.ts:635: `'inspector.addUturnToEnd': 'U-turn for traffic heading to the end'`
- feature entry `inspector.addUturnToStart` — src/ui/i18n/en.ts:636: `'inspector.addUturnToStart': 'U-turn for traffic heading to the start'`
- feature entry `inspector.addHeightPoint` — src/ui/i18n/en.ts:648: `'inspector.addHeightPoint': 'Add height point here'`
- feature entry `inspector.demolish` — src/ui/i18n/en.ts:649: `'inspector.demolish': 'Demolish'`
- feature entry `turn.left` — src/ui/i18n/en.ts:660: `'turn.left': 'left'`
- feature entry `turn.right` — src/ui/i18n/en.ts:661: `'turn.right': 'right'`
- feature entry `turn.through` — src/ui/i18n/en.ts:662: `'turn.through': 'through'`
- feature entry `turn.uturn` — src/ui/i18n/en.ts:663: `'turn.uturn': 'u-turn'`
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
- feature entry `inspector.parkingLeft` — src/ui/i18n/en.ts:1319: `'inspector.parkingLeft': 'Parking on the left'`
- feature entry `inspector.parkingRight` — src/ui/i18n/en.ts:1320: `'inspector.parkingRight': 'Parking on the right'`
- feature entry `parking.none` — src/ui/i18n/en.ts:1321: `'parking.none': 'No parking'`
- feature entry `parking.parallel` — src/ui/i18n/en.ts:1322: `'parking.parallel': 'Parallel to the kerb'`
- feature entry `parking.cycle` — src/ui/i18n/en.ts:1323: `'parking.cycle': 'Cycle lane'`
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
- feature entry `inspector.numbers` — src/ui/i18n/pt-BR.ts:306: `'inspector.numbers': 'Nº {from} a {to} · pares à direita no sentido da numeração'`
- feature entry `inspector.direction` — src/ui/i18n/pt-BR.ts:586: `'inspector.direction': 'Sentido do tráfego'`
- feature entry `inspector.laneCount` — src/ui/i18n/pt-BR.ts:587: `'inspector.laneCount': 'Faixas de circulação'`
- feature entry `inspector.curvePosition` — src/ui/i18n/pt-BR.ts:588: `'inspector.curvePosition': 'Posição da curva'`
- feature entry `inspector.heightStart` — src/ui/i18n/pt-BR.ts:589: `'inspector.heightStart': 'Altura inicial (m)'`
- feature entry `inspector.heightEnd` — src/ui/i18n/pt-BR.ts:590: `'inspector.heightEnd': 'Altura final (m)'`
- feature entry `inspector.heightNode` — src/ui/i18n/pt-BR.ts:595: `'inspector.heightNode': 'Altura da via (m)'`
- feature entry `inspector.upgrade` — src/ui/i18n/pt-BR.ts:607: `'inspector.upgrade': 'Melhorar classe'`
- feature entry `inspector.duplicate` — src/ui/i18n/pt-BR.ts:608: `'inspector.duplicate': 'Duplicar via'`
- feature entry `inspector.roadClass` — src/ui/i18n/pt-BR.ts:614: `'inspector.roadClass': 'Classe da via'`
- feature entry `inspector.curvature` — src/ui/i18n/pt-BR.ts:615: `'inspector.curvature': 'Curvatura'`
- feature entry `inspector.reverse` — src/ui/i18n/pt-BR.ts:624: `'inspector.reverse': 'Inverter mão única'`
- feature entry `inspector.duplicateHint` — src/ui/i18n/pt-BR.ts:625: `'inspector.duplicateHint': 'Duplicar via (Ctrl/Cmd+D)'`
- feature entry `inspector.splitMiddle` — src/ui/i18n/pt-BR.ts:626: `'inspector.splitMiddle': 'Dividir no meio'`
- feature entry `inspector.addZebra` — src/ui/i18n/pt-BR.ts:627: `'inspector.addZebra': 'Faixa zebra'`
- feature entry `inspector.addSignalCrossing` — src/ui/i18n/pt-BR.ts:628: `'inspector.addSignalCrossing': 'Faixa com semáforo'`
- feature entry `inspector.addUturnToEnd` — src/ui/i18n/pt-BR.ts:629: `'inspector.addUturnToEnd': 'Retorno para quem vai à ponta final'`
- feature entry `inspector.addUturnToStart` — src/ui/i18n/pt-BR.ts:630: `'inspector.addUturnToStart': 'Retorno para quem vai à ponta inicial'`
- feature entry `inspector.addHeightPoint` — src/ui/i18n/pt-BR.ts:642: `'inspector.addHeightPoint': 'Adicionar ponto de altura aqui'`
- feature entry `inspector.demolish` — src/ui/i18n/pt-BR.ts:643: `'inspector.demolish': 'Demolir'`
- feature entry `turn.left` — src/ui/i18n/pt-BR.ts:654: `'turn.left': 'esquerda'`
- feature entry `turn.right` — src/ui/i18n/pt-BR.ts:655: `'turn.right': 'direita'`
- feature entry `turn.through` — src/ui/i18n/pt-BR.ts:656: `'turn.through': 'em frente'`
- feature entry `turn.uturn` — src/ui/i18n/pt-BR.ts:657: `'turn.uturn': 'retorno'`
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
- feature entry `inspector.parkingLeft` — src/ui/i18n/pt-BR.ts:1309: `'inspector.parkingLeft': 'Estacionamento à esquerda'`
- feature entry `inspector.parkingRight` — src/ui/i18n/pt-BR.ts:1310: `'inspector.parkingRight': 'Estacionamento à direita'`
- feature entry `parking.none` — src/ui/i18n/pt-BR.ts:1311: `'parking.none': 'Sem vagas'`
- feature entry `parking.parallel` — src/ui/i18n/pt-BR.ts:1312: `'parking.parallel': 'Paralelo à guia'`
- feature entry `parking.cycle` — src/ui/i18n/pt-BR.ts:1313: `'parking.cycle': 'Ciclofaixa'`
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
- shared style `panel` — src/peopleLab/peopleLab.css:94: `.panel > * { flex-shrink: 0; }`
- shared style `stage` — src/peopleLab/peopleLab.css:119: `.stage { position: relative; overflow: hidden; background: radial-gradient(ellipse at 50% 35%, var(--stage-top), var(--stage-bottom) 75%); }`
- shared style `stage` — src/peopleLab/peopleLab.css:120: `.stage canvas { display: block; width: 100%; height: 100%; }`
- shared style `stats` — src/peopleLab/peopleLab.css:131: `.stats { position: absolute; left: 14px; bottom: 16px; font-size: 11px; color: var(--ink-3); line-height: 1.5; font-variant-numeric: tabular-nums; }`
- shared style `side` — src/peopleLab/peopleLab.css:142: `.side { background: var(--panel); border-left: 1px solid var(--line); display: flex; flex-direction: column; padding: 14px; gap: 8px; }`
- shared style `side` — src/peopleLab/peopleLab.css:143: `.side h2 { margin: 0 0 2px; font-size: 11px; font-weight: 600; letter-spacing: .06em; text-transform: uppercase; color: var(--ink-3); }`
- shared style `card` — src/peopleLab/peopleLab.css:146: `.card { position: relative; border-radius: var(--radius); background: var(--panel-2); border: 1px solid var(--line); cursor: pointer; padding: 3px; text-align: `
- shared style `card` — src/peopleLab/peopleLab.css:147: `.card img { width: 100%; aspect-ratio: 1; object-fit: cover; border-radius: 6px; background: #2a2f37; display: block; margin-bottom: 3px; }`
- shared style `card` — src/peopleLab/peopleLab.css:148: `.card.on { border-color: var(--accent); color: var(--ink); }`
- shared style `card` — src/peopleLab/peopleLab.css:149: `.card .x { position: absolute; top: 5px; right: 5px; width: 18px; height: 18px; border-radius: 4px; border: 0; background: rgba(0,0,0,.55); color: #fff; cursor:`
- shared style `card` — src/peopleLab/peopleLab.css:150: `.card:hover .x { opacity: 1; }`
- shared style `subtabs` — src/peopleLab/peopleLab.css:159: `.subtabs { display: flex; flex-wrap: wrap; gap: 2px; margin: -2px -2px 12px; padding-bottom: 10px; border-bottom: 1px solid var(--line); }`
- shared style `subtabs` — src/peopleLab/peopleLab.css:160: `.subtabs button { border: 0; background: transparent; padding: 5px 9px; border-radius: 6px; cursor: pointer; font-size: 12px; font-weight: 500; color: var(--ink`
- shared style `subtabs` — src/peopleLab/peopleLab.css:161: `.subtabs button:hover { color: var(--ink); background: var(--panel-2); }`
- shared style `subtabs` — src/peopleLab/peopleLab.css:162: `.subtabs button.on { color: var(--ink); background: var(--panel-3); }`
- shared style `glass` — src/ui/app.css:57: `.glass { background: linear-gradient(145deg, var(--ui-hi), transparent 42%), var(--ui-bg); border: 1px solid var(--ui-border); border-radius: 14px; box-shadow: `
- shared style `signal-chips` — src/ui/app.css:140: `.signal-chips { display: flex; flex-wrap: wrap; gap: 5px; margin-top: 6px; }`
- shared style `signal-chip` — src/ui/app.css:141: `.signal-chip { display: inline-flex; align-items: center; gap: 5px; padding: 4px 8px 4px 6px; border-radius: 999px; border: 1px solid var(--ui-border); backgrou`
- shared style `signal-chip` — src/ui/app.css:152: `.signal-chip i { width: 8px; height: 8px; border-radius: 50%; background: #ff5b50; box-shadow: 0 0 6px currentColor; color: #ff5b50; }`
- shared style `signal-chip` — src/ui/app.css:153: `.signal-chip[data-state="green"] i { background: #4ee08a; color: #4ee08a; }`
- shared style `signal-chip` — src/ui/app.css:154: `.signal-chip[data-state="amber"] i { background: #ffc03d; color: #ffc03d; }`
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
- shared style `rp-btn` — src/ui/roads/roads.css:31: `.rp-icon svg, .rp-tool svg, .rp-btn svg, .rp-issue svg, .rp-ok svg { width: 18px; height: 18px; fill: none; stroke: currentColor; stroke-width: 1.8; stroke-line`
- shared style `rp-label` — src/ui/roads/roads.css:33: `.rp-label { color: var(--v-muted); font-size: 11.5px; font-weight: 600; text-transform: none; letter-spacing: 0.02em; }`
- shared style `xs-label` — src/ui/roads/roads.css:61: `.xs-label { fill: #e8f0ee; font: 600 12.5px Inter, ui-sans-serif, sans-serif; font-variant-numeric: tabular-nums; }`
- shared style `xs-glyph` — src/ui/roads/roads.css:63: `.xs-glyph { fill: #f2efe6; font: 800 13px Inter, ui-sans-serif, sans-serif; }`
- shared style `rp-block` — src/ui/roads/roads.css:84: `.rp-block { display: grid; gap: 8px; align-content: start; padding: 10px 12px; border: 1px solid var(--v-line); border-radius: 10px; background: var(--v-raise);`
- shared style `rp-block-head` — src/ui/roads/roads.css:85: `.rp-block-head { display: flex; align-items: center; gap: 8px; font-weight: 650; }`
- shared style `rp-row` — src/ui/roads/roads.css:88: `.rp-row { display: grid; grid-template-columns: 76px minmax(0, 1fr); align-items: center; gap: 8px; min-height: 30px; }`
- shared style `rp-note` — src/ui/roads/roads.css:89: `.rp-note { color: var(--v-muted); font-size: 11.5px; }`
- shared style `rp-ctrls` — src/ui/roads/roads.css:90: `.rp-ctrls { display: flex; flex-wrap: wrap; align-items: center; gap: 6px; min-width: 0; }`
- shared style `rp-seg` — src/ui/roads/roads.css:92: `.rp-seg { display: inline-flex; flex-wrap: wrap; gap: 2px; padding: 3px; border-radius: 10px; background: rgba(0,0,0,.18); border: 1px solid var(--v-line); }`
- shared style `rp-seg` — src/ui/roads/roads.css:94: `.rp-seg button { min-height: 26px; padding: 0 11px; border: 0; border-radius: 7px; background: none; color: var(--v-muted); font-size: 12px; font-weight: 600; w`
- shared style `rp-seg` — src/ui/roads/roads.css:95: `.rp-seg button:hover { color: var(--v-text); }`
- shared style `rp-seg` — src/ui/roads/roads.css:97: `.rp-seg.dense { display: grid; flex: 1 1 100%; grid-template-columns: repeat(auto-fill, minmax(96px, 1fr)); }`
- shared style `rp-seg` — src/ui/roads/roads.css:98: `.rp-seg.dense button { padding: 0 8px; font-size: 11.5px; }`
- shared style `rp-seg` — src/ui/roads/roads.css:99: `.rp-seg button.on { background: var(--v-accent); color: var(--v-accent-ink, #062520); }`
- shared style `rp-step` — src/ui/roads/roads.css:101: `.rp-step { display: inline-grid; grid-template-columns: 28px minmax(52px, auto) 28px; align-items: center; gap: 2px; padding: 2px; border-radius: 999px; backgro`
- shared style `rp-step` — src/ui/roads/roads.css:102: `.rp-step button { width: 28px; height: 26px; border: 0; border-radius: 999px; background: none; color: var(--v-text); font-size: 16px; font-weight: 700; line-he`
- shared style `rp-step` — src/ui/roads/roads.css:103: `.rp-step button:hover:not(:disabled) { background: var(--v-raise-2); }`
- shared style `rp-step` — src/ui/roads/roads.css:104: `.rp-step output { text-align: center; font-weight: 650; font-variant-numeric: tabular-nums; }`
- shared style `rp-switch` — src/ui/roads/roads.css:106: `.rp-switch { display: inline-flex; align-items: center; gap: 8px; border: 0; background: none; padding: 2px 0; color: var(--v-text); font-size: 12.5px; }`
- shared style `rp-btn` — src/ui/roads/roads.css:119: `.rp-btn { display: inline-flex; align-items: center; gap: 6px; min-height: 34px; padding: 0 14px; border-radius: 999px; border: 1px solid var(--v-line); backgro`
- shared style `rp-btn` — src/ui/roads/roads.css:120: `.rp-btn:hover:not(:disabled) { border-color: var(--v-accent-line); background: var(--v-raise-2); }`
- shared style `rp-junction` — src/ui/roads/roads.css:180: `.rp-junction { display: grid; gap: 8px; margin: 10px 0; color: var(--v-text, #eef4f2); font: 13px/1.3 Inter, ui-sans-serif, "Segoe UI", Roboto, sans-serif; }`
- shared style `rp-junction` — src/ui/roads/roads.css:181: `.rp-junction * { box-sizing: border-box; }`
- shared style `rp-junction` — src/ui/roads/roads.css:182: `.rp-junction button { font: inherit; color: inherit; cursor: pointer; }`
- shared style `rp-junction` — src/ui/roads/roads.css:183: `.rp-junction button:disabled { opacity: 0.35; cursor: default; }`
- shared style `rp-junction` — src/ui/roads/roads.css:184: `.rp-junction :focus-visible { outline: 2px solid var(--v-accent, #4fe0bf); outline-offset: 1px; }`
- shared style `rp-junction` — src/ui/roads/roads.css:185: `.rp-junction .rp-block { display: grid; gap: 8px; padding: 10px; border: 1px solid rgba(255,255,255,.12); border-radius: 10px; background: rgba(255,255,255,.04)`
- shared style `rp-junction` — src/ui/roads/roads.css:186: `.rp-junction .rp-block-head { font-weight: 650; }`
- shared style `rp-junction` — src/ui/roads/roads.css:187: `.rp-junction .rp-row { display: grid; grid-template-columns: 84px minmax(0, 1fr); align-items: center; gap: 8px; min-height: 30px; }`
- shared style `rp-junction` — src/ui/roads/roads.css:188: `.rp-junction .rp-seg.dense { display: grid; grid-template-columns: repeat(auto-fill, minmax(88px, 1fr)); }`
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

## Where the program plugs the feature in

### src/render/agents.ts

- `src/render/agents.ts#AgentMeshes` (interface, line 168) ← call from `src/render/playEffects.ts#createPlayEffects`, `src/render/playEffects.ts#createPlayEffects.dispose`, `src/render/playEffects.ts#createPlayEffects.dropVehicle`, `src/render/playEffects.ts#createPlayEffects.explode`, …
- `src/render/agents.ts#AgentMeshes` (interface, line 168) ← read from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.movingCasters`
- `src/render/agents.ts#AgentMeshes` (interface, line 168) ← typeRef from `src/render/playEffects.ts#PlayEffectsContext`, `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#SceneHandle`
- `src/render/agents.ts#createAgentMeshes` (function, line 552) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/agents.ts#createAgentMeshes` (function, line 552) ← implementation from `src/render/playEffects.ts#createPlayEffects.frame`, `src/render/ragdoll.ts#createRagdolls.build`, `src/render/ragdoll.ts#createRagdolls.captured`, `src/render/ragdoll.ts#createRagdolls.detach`, …
- `src/render/agents.ts#createAgentMeshes` (function, line 552) ← indirectCall from `src/render/riggedCitizens.ts#createRiggedCitizens.load`
- `src/render/agents.ts#createAgentMeshes.animProbe` (method, line 2073) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/agents.ts#createAgentMeshes.carcass` (method, line 2089) ← implementation from `src/render/playEffects.ts#createPlayEffects.dropVehicle`, `src/render/playEffects.ts#createPlayEffects.explode`
- `src/render/agents.ts#createAgentMeshes.census` (method, line 2023) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/agents.ts#createAgentMeshes.dispose` (method, line 2574) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/agents.ts#createAgentMeshes.driverBody` (method, line 2086) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/agents.ts#createAgentMeshes.forceClip` (method, line 2083) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/agents.ts#createAgentMeshes.forgetCast` (function, line 1633) ← callbackAssign from `src/sim/host/simClient.ts#SimClient.clearParked`, `src/sim/host/simClient.ts#SimClient.clearViews`, `src/sim/host/simClient.ts#SimClient.dropParked`, `src/sim/host/simClient.ts#SimClient.readTick`
- `src/render/agents.ts#createAgentMeshes.forgetGone` (function, line 1628) ← indirectCall from `src/sim/host/simClient.ts#SimClient.dropParked`, `src/sim/host/simClient.ts#SimClient.readTick`
- `src/render/agents.ts#createAgentMeshes.keepPeople` (method, line 2152) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/agents.ts#createAgentMeshes.meshProbe` (method, line 2024) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/agents.ts#createAgentMeshes.renderPalettes` (method, line 2151) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/agents.ts#createAgentMeshes.setBleed` (method, line 2147) ← implementation from `src/render/playEffects.ts#createPlayEffects`, `src/render/playEffects.ts#createPlayEffects.dispose`
- `src/render/agents.ts#createAgentMeshes.setNight` (method, line 2148) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/agents.ts#createAgentMeshes.sync` (method, line 2153) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/agents.ts#VEHICLE_REACH` (variable, line 440) ← read from `src/render/renderer.ts#createSceneRenderer`

### src/render/densityCars.ts

- `src/render/densityCars.ts#createDensityCars` (function, line 124) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/densityCars.ts#createDensityCars.dispose` (method, line 485) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/densityCars.ts#createDensityCars.update` (method, line 464) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/densityCars.ts#DensityCars` (interface, line 112) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/densityCars.ts#DensityCars` (interface, line 112) ← read from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.switchedOff`

### src/render/exhaust.ts

- `src/render/exhaust.ts#createExhaust` (function, line 33) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/exhaust.ts#createExhaust.burst` (method, line 219) ← implementation from `src/render/blast.ts#createBlast.explode`, `src/render/blast.ts#createBlast.stepRope`, `src/render/blast.ts#createBlast.update`, `src/render/destruction.ts#createDestruction.hit`, …
- `src/render/exhaust.ts#createExhaust.dispose` (method, line 234) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/exhaust.ts#createExhaust.emit` (method, line 204) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/exhaust.ts#createExhaust.tick` (method, line 226) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/exhaust.ts#Exhaust` (interface, line 22) ← call from `src/render/blast.ts#createBlast.explode`, `src/render/blast.ts#createBlast.stepRope`, `src/render/blast.ts#createBlast.update`, `src/render/destruction.ts#createDestruction.hit`, …
- `src/render/exhaust.ts#Exhaust` (interface, line 22) ← read from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.switchedOff`
- `src/render/exhaust.ts#Exhaust` (interface, line 22) ← typeRef from `src/render/blast.ts#createBlast`, `src/render/destruction.ts#createDestruction`, `src/render/elements.ts#createElementLayer.give`, `src/render/elements.ts#ElementLayer`, …
- `src/render/exhaust.ts#PuffKind` (type, line 20) ← typeRef from `src/render/elements.ts#EFFECTS`

### src/sim/agents/bay.ts

- `src/sim/agents/bay.ts#Bay` (interface, line 20) ← read from `src/sim/ambient/ambient.ts#AmbientWorld.stand`, `src/sim/ambient/ambient.ts#AmbientWorld.standSource`
- `src/sim/agents/bay.ts#Bay` (interface, line 20) ← typeRef from `src/bayPool.ts#BayPool.start`, `src/bayPool.ts#BayPool.work`, `src/bayPool.ts#Lane`, `src/sim/ambient/ambient.ts#AmbientWorld.stand`
- `src/sim/agents/bay.ts#BayLane` (interface, line 7) ← read from `src/sim/ambient/ambient.ts#AmbientWorld.stand`, `src/sim/transit/transit.ts#TransitSim.sendBus`, `src/sim/transit/transit.ts#TransitSim.spawnBus`, `src/sim/transit/transit.ts#TransitSim.stopPoint`
- `src/sim/agents/bay.ts#BayLane` (interface, line 7) ← typeRef from `src/sim/transit/transit.ts#TransitSim.stopLanes`

### src/sim/agents/buildingOrder.ts

- `src/sim/agents/buildingOrder.ts#StoreOrder.constructor` (constructor, line 21) ← callbackAssign from `src/core/idSlots.ts#IdSlots.take`, `src/render/buildings/cellPool.ts#FloorColumns.constructor`, `src/render/buildings/townColumns.ts#TownColumns.constructor`, `src/world/buildings/cameraSolids.ts#SolidFiling.constructor`, …

### src/sim/agents/laneGrid.ts

- `src/sim/agents/laneGrid.ts#laneChangesSince` (function, line 188) ← call from `src/sim/transit/transit.ts#TransitSim.followLanes`, `src/sim/transit/transit.ts#TransitSim.step`
- `src/sim/agents/laneGrid.ts#laneChangesSince` (function, line 188) ← read from `src/sim/transit/transit.ts#TransitSim.step`
- `src/sim/agents/laneGrid.ts#laneGrid` (function, line 167) ← call from `src/sim/host/simHost.ts#SimHost.frame`
- `src/sim/agents/laneGrid.ts#lanesInBox` (function, line 183) ← call from `src/sim/ambient/ambient.ts#AmbientWorld.lanesAround`, `src/sim/transit/transit.ts#levelCrossings`

### src/sim/agents/lotBays.ts

- `src/sim/agents/lotBays.ts#BayShare` (interface, line 406) ← typeRef from `src/bays.worker.ts#serve`
- `src/sim/agents/lotBays.ts#baysOfShare` (function, line 489) ← call from `src/bays.worker.ts#serve`
- `src/sim/agents/lotBays.ts#LotAsk` (interface, line 394) ← typeRef from `src/bayPool.ts#BayPool.work`
- `src/sim/agents/lotBays.ts#shareOf` (function, line 425) ← call from `src/bayPool.ts#BayPool.work`
- `src/sim/agents/lotBays.ts#stallFits` (function, line 60) ← call from `src/sim/ambient/ambient.ts#AmbientWorld.stand`

### src/sim/agents/lotDoors.ts

- `src/sim/agents/lotDoors.ts#doorWay` (function, line 40) ← reference from `src/sim/ambient/ambient.ts#newPlaces`
- `src/sim/agents/lotDoors.ts#DoorWay` (interface, line 12) ← read from `src/sim/ambient/ambient.ts#AmbientWorld.edgePeople`, `src/sim/ambient/ambient.ts#AmbientWorld.keepWalking`, `src/sim/ambient/ambient.ts#AmbientWorld.walkFrom`, `src/sim/ambient/ambient.ts#reversed`
- `src/sim/agents/lotDoors.ts#DoorWay` (interface, line 12) ← typeRef from `src/sim/ambient/ambient.ts#AmbientWorld.edgePeople`, `src/sim/ambient/ambient.ts#AmbientWorld.pickPlace`, `src/sim/ambient/ambient.ts#AmbientWorld.walkFrom`, `src/sim/ambient/ambient.ts#newPlaces`, …

### src/sim/agents/lotTraffic.ts

- `src/sim/agents/lotTraffic.ts#LotTraffic` (class, line 91) ← construct from `src/sim/city/city.ts#City.lots`
- `src/sim/agents/lotTraffic.ts#LotTraffic.moving` (method, line 140) ← call from `src/sim/host/simHost.ts#SimHost.stats`
- `src/sim/agents/lotTraffic.ts#LotTraffic.reset` (method, line 192) ← call from `src/sim/ambient/ambient.ts#AmbientWorld.reset`
- `src/sim/agents/lotTraffic.ts#LotTraffic.sceneryMoved` (field, line 122) ← indirectCall from `src/sim/ambient/ambient.ts#AmbientWorld.stand`, `src/sim/ambient/ambient.ts#AmbientWorld.takeScenery`
- `src/sim/agents/lotTraffic.ts#LotTraffic.step` (method, line 220) ← call from `src/sim/city/city.ts#City.step`
- `src/sim/agents/lotTraffic.ts#LotTraffic.step` (method, line 220) ← callbackAssign from `src/sim/ambient/ambient.ts#AmbientWorld.stand`, `src/sim/ambient/ambient.ts#AmbientWorld.takeScenery`

### src/sim/agents/parking.ts

- `src/sim/agents/parking.ts#bayChangesSince` (function, line 327) ← call from `src/sim/ambient/ambient.ts#AmbientWorld.parking`
- `src/sim/agents/parking.ts#baysOfSource` (function, line 354) ← call from `src/sim/ambient/ambient.ts#AmbientWorld.standSource`
- `src/sim/agents/parking.ts#baySources` (function, line 335) ← call from `src/sim/ambient/ambient.ts#AmbientWorld.parking`
- `src/sim/agents/parking.ts#baySourcesIn` (function, line 344) ← call from `src/sim/ambient/ambient.ts#AmbientWorld.parkInWindow`
- `src/sim/agents/parking.ts#BayWorkers` (interface, line 62) ← implements from `src/bayPool.ts#BayPool`
- `src/sim/agents/parking.ts#LANE_BESIDE_REACH` (variable, line 21) ← read from `src/sim/transit/transit.ts#TransitSim.followLanes`
- `src/sim/agents/parking.ts#laneBeside` (function, line 482) ← call from `src/sim/transit/transit.ts#TransitSim.followLanes`, `src/sim/transit/transit.ts#TransitSim.rebuild`
- `src/sim/agents/parking.ts#notOfTheLot` (function, line 46) ← indirectCall from `src/core/digest.ts#Digest.addPlain`
- `src/sim/agents/parking.ts#useBayWorkers` (function, line 68) ← call from `src/sim/host/simHost.ts#SimHost.init`

### src/sim/agents/trafficBuildings.ts

- `src/sim/agents/trafficBuildings.ts#buildingChangesOf` (function, line 722) ← call from `src/sim/host/simClient.ts#SimClient.sendChanges`
- `src/sim/agents/trafficBuildings.ts#BuildingSent` (interface, line 692) ← typeRef from `src/sim/host/protocol.ts#ToSim`
- `src/sim/agents/trafficBuildings.ts#docBuildings` (function, line 174) ← call from `src/sim/world.ts#SimWorld.constructor`
- `src/sim/agents/trafficBuildings.ts#takeTrafficMap` (function, line 741) ← call from `src/sim/host/simHost.ts#SimHost.handle`, `src/sim/host/simHost.ts#SimHost.init`, `src/sim/host/simHost.ts#SimHost.load`
- `src/sim/agents/trafficBuildings.ts#TrafficBuildings` (interface, line 141) ← typeRef from `src/sim/world.ts#SimWorld.constructor`
- `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable` (class, line 253) ← construct from `src/sim/host/simHost.ts#SimHost.buildings`
- `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.apply` (method, line 391) ← call from `src/sim/host/simHost.ts#SimHost.handle`
- `src/sim/agents/trafficBuildings.ts#trafficChangesOf` (function, line 732) ← call from `src/sim/host/simHost.ts#SimHost.handle`
- `src/sim/agents/trafficBuildings.ts#TrafficMap` (interface, line 698) ← typeRef from `src/sim/host/protocol.ts#ToSim`, `src/sim/host/simHost.ts#SimHost.init`, `src/sim/host/simHost.ts#SimHost.load`
- `src/sim/agents/trafficBuildings.ts#trafficMapOf` (function, line 706) ← call from `src/sim/host/simClient.ts#SimClient.load`, `src/sim/host/simClient.ts#SimClient.resync`, `src/sim/host/simClient.ts#SimClient.start`

### src/sim/agents/walk.ts

- `src/sim/agents/walk.ts#ACCEL` (variable, line 39) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#ACROSS_PROJECTION` (variable, line 723) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#actName` (function, line 409) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#addOther` (function, line 1451) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#addPlayerWalker` (function, line 2024) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#AHEAD` (variable, line 61) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#AIM_FRAME` (variable, line 685) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#AIM_HOLD` (variable, line 44) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#AIM_PROJECTION` (variable, line 734) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#ALIGHT_SNAP` (variable, line 913) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#ALIGHT_TRIES` (variable, line 915) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#ASK_EVERY` (variable, line 85) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#ASK_FROM` (variable, line 77) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#BEND_WINDOW` (variable, line 796) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#bendRadii` (function, line 799) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#BENDS` (variable, line 797) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#BODY` (variable, line 26) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#BODY_SHOCK` (variable, line 450) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#BRAKE` (variable, line 40) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#CarZone` (interface, line 102) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#cell` (function, line 473) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#CELL` (variable, line 96) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#cellKey` (function, line 474) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#clamp` (function, line 825) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#CRAWL_PACE` (variable, line 434) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#createAgentWalkEngine` (function, line 917) ← call from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#createAgentWalkEngine` (function, line 917) ← implementation from `src/sim/ambient/ambient.ts#AmbientWorld.across`, `src/sim/ambient/ambient.ts#AmbientWorld.company`, `src/sim/ambient/ambient.ts#AmbientWorld.edgePeople`, `src/sim/ambient/ambient.ts#AmbientWorld.openPeople`, …
- `src/sim/agents/walk.ts#createAgentWalkEngine` (function, line 917) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#CROSSING_COST` (variable, line 73) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#crossingOf` (function, line 836) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#DOOR_CLEAR` (variable, line 91) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#ensureGraph` (function, line 479) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#fileWay` (function, line 498) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#fileZones` (function, line 1354) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#finish` (function, line 1299) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#FLINCH` (variable, line 432) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#FOLD_SHARE` (variable, line 780) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#frame` (function, line 688) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#frameInto` (function, line 693) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#freeAhead` (function, line 1476) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#frighten` (function, line 2148) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#GAP_MARGIN` (variable, line 853) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#GAP_NEAR` (variable, line 856) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#gapOpen` (function, line 859) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#GATE_JOIN` (variable, line 1225) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#GRANT_LIFE` (variable, line 87) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#HAIL_REACH` (variable, line 911) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#HEADWAY` (variable, line 37) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#HELD_SWITCH` (variable, line 46) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#hypot` (function, line 471) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#IMPATIENT` (variable, line 854) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#INAPPROPRIATE` (variable, line 785) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#insideDiscs` (function, line 1504) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#inspectAgentWalkers` (function, line 1962) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#JAM_AFTER` (variable, line 50) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#JAM_AFTER_CROSSING` (variable, line 51) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#JAM_AFTER_NARROW` (variable, line 53) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#JAM_NO_PROGRESS` (variable, line 55) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#JAM_SHARE` (variable, line 59) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#joinFootway` (function, line 1228) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#joinParty` (function, line 1294) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#joinsFootway` (function, line 2090) ← call from `src/sim/ambient/ambient.ts#AmbientWorld.pickPlace`
- `src/sim/agents/walk.ts#joinsFootway` (function, line 2090) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#KEEP` (variable, line 36) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#KERB_BACK` (variable, line 75) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#kerbInset` (function, line 828) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#KNOCKED_MOST` (variable, line 436) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#lastOf` (function, line 2216) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#LATERAL_PENALTY` (variable, line 48) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#LATERAL_SHARE` (variable, line 65) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#legsLostOf` (function, line 1528) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#LOOK` (variable, line 32) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#LOT_AHEAD` (variable, line 63) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#lotSteps` (function, line 1213) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#mourn` (function, line 2135) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#moved` (function, line 256) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#movePlayerWalker` (function, line 2049) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#MOVING` (variable, line 34) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#NEAREST` (variable, line 731) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#nearestWay` (function, line 517) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#nearMoving` (variable, line 1515) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#nearNow` (function, line 1437) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#nearParked` (variable, line 1514) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#newStepFrame` (function, line 680) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#NO_POINTS` (variable, line 109) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#ONCOMING_CONFLICT` (variable, line 787) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#onLastStretch` (function, line 2083) ← call from `src/sim/ambient/ambient.ts#AmbientWorld.keepWalking`
- `src/sim/agents/walk.ts#onLastStretch` (function, line 2083) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#onZebra` (function, line 900) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#Other` (interface, line 1448) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#OTHERS` (variable, line 1449) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#othersN` (variable, line 1450) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#PANIC_RUN` (variable, line 2118) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#ParkedEntry` (interface, line 124) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#ParkedSource` (interface, line 127) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#parkedZone` (function, line 112) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#ParkedZoneIndex` (class, line 139) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#ParkedZoneIndex.byCar` (field, line 144) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#ParkedZoneIndex.cells` (field, line 141) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#ParkedZoneIndex.follow` (method, line 150) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#ParkedZoneIndex.keep` (method, line 179) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#ParkedZoneIndex.list` (field, line 143) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#ParkedZoneIndex.pass` (field, line 145) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#ParkedZoneIndex.place` (method, line 222) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#ParkedZoneIndex.serial` (field, line 147) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#ParkedZoneIndex.sync` (method, line 189) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#ParkedZoneIndex.unfile` (method, line 238) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#plan` (function, line 1195) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#PROGRESS` (variable, line 57) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#project` (function, line 726) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#PROJECT_FRAME` (variable, line 684) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#projectAcross` (function, line 719) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#projectInto` (function, line 737) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#prune` (function, line 1323) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#publish` (function, line 1884) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#QUEUE_GAP` (variable, line 83) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#QUEUE_REACH` (variable, line 79) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#queuedForZebra` (function, line 1488) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#REACH` (variable, line 89) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#releaseWalker` (function, line 2251) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#removeWalker` (function, line 2096) ← call from `src/sim/ambient/ambient.ts#AmbientWorld.people`, `src/sim/ambient/ambient.ts#AmbientWorld.reset`, `src/sim/ambient/ambient.ts#AmbientWorld.trimToCeilings`
- `src/sim/agents/walk.ts#removeWalker` (function, line 2096) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#replan` (function, line 2223) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#reroute` (function, line 2228) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#RESERVE_MAX` (variable, line 783) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#RESERVE_ONCOMING` (variable, line 782) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#reserveCost` (function, line 1499) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#reservedFor` (function, line 789) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#room` (function, line 754) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#roomOf` (function, line 762) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#ROOMS` (variable, line 760) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#route` (function, line 544) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#ROUTE_SCRATCH` (variable, line 654) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#routeScratch` (function, line 656) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#RouteScratch` (interface, line 653) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#RUN` (variable, line 2116) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#rushName` (function, line 410) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#seen` (function, line 1995) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#seer` (variable, line 1459) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#seerFrame` (variable, line 1460) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#seeWalker` (function, line 1462) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#sendRunning` (function, line 2207) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#shock` (function, line 452) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#SHOCK_LIFE` (variable, line 449) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#SHOULDERS` (variable, line 28) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#SHUFFLE` (variable, line 67) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#SHUFFLE_COS` (variable, line 68) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#SIDESTEP` (variable, line 30) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#SPAN` (variable, line 1536) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#STAGGER_DISTANCE` (variable, line 441) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#STAGGER_FROM` (variable, line 440) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#STAGGER_TIME` (variable, line 438) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#staggerSpeed` (function, line 443) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#startle` (function, line 2121) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#startWalk` (function, line 1237) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#State` (interface, line 355) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#stateOf` (function, line 459) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#STATES` (variable, line 458) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#Step` (interface, line 259) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#STEP_PROJECTION` (variable, line 733) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#StepFrame` (interface, line 679) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#stepLength` (function, line 675) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#stepWalkers` (function, line 1538) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#STRIPE` (variable, line 24) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#SWITCH_GAIN` (variable, line 42) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#takeWalker` (function, line 2010) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#target` (function, line 1328) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#TARGET` (variable, line 1349) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#THERE` (variable, line 93) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#trace` (function, line 411) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#traced` (variable, line 400) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#traceSink` (variable, line 401) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#traceTick` (variable, line 402) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#traceWalkers` (function, line 404) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#TRAFFIC_POSE` (variable, line 850) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#TRAIN_GROUND` (variable, line 2113) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#TURN_RATE` (variable, line 70) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#TURN_STANDING` (variable, line 71) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#WAIT_HALF` (variable, line 81) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#WALK_COMPONENTS` (variable, line 624) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#WALK_FRAME` (variable, line 686) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#walkComponents` (function, line 626) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#Walker` (interface, line 274) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#WALKER_SHAPE` (variable, line 349) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#walkerAct` (function, line 2103) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#walkerAlive` (function, line 2077) ← call from `src/sim/ambient/ambient.ts#AmbientWorld.step`
- `src/sim/agents/walk.ts#walkerAlive` (function, line 2077) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#WalkerGrid` (class, line 1369) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#WalkerGrid.build` (method, line 1382) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#WalkerGrid.cx` (field, line 1371) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#WalkerGrid.cy` (field, line 1372) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#WalkerGrid.entries` (field, line 1374) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#WalkerGrid.list` (field, line 1370) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#WalkerGrid.size` (field, line 1375) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#WalkerGrid.slot` (method, line 1377) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#WalkerGrid.someIn` (method, line 1421) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#WalkerGrid.someNear` (method, line 1415) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#WalkerGrid.start` (field, line 1373) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#walkerOf` (function, line 2004) ← call from `src/sim/ambient/ambient.ts#AmbientWorld.keepWalking`, `src/sim/ambient/ambient.ts#AmbientWorld.people`, `src/sim/ambient/ambient.ts#AmbientWorld.trimToCeilings`
- `src/sim/agents/walk.ts#walkerOf` (function, line 2004) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#WalkerSeen` (interface, line 1983) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#WalkerSeen` (interface, line 1983) ← read from `src/sim/ambient/ambient.ts#AmbientWorld.people`, `src/sim/ambient/ambient.ts#AmbientWorld.trimToCeilings`
- `src/sim/agents/walk.ts#walkersNear` (function, line 1999) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#walkerState` (function, line 2261) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#walkersWithin` (function, line 1311) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#walkOn` (function, line 2058) ← call from `src/sim/ambient/ambient.ts#AmbientWorld.keepWalking`, `src/sim/ambient/ambient.ts#AmbientWorld.people`
- `src/sim/agents/walk.ts#walkOn` (function, line 2058) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#walkThrough` (function, line 1220) ← call from `src/sim/ambient/ambient.ts#AmbientWorld.company`, `src/sim/ambient/ambient.ts#AmbientWorld.walkFrom`
- `src/sim/agents/walk.ts#walkThrough` (function, line 1220) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#WalkTrace` (interface, line 389) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#walkWork` (variable, line 122) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#WAY_CELL` (variable, line 97) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#WAY_SAMPLE` (variable, line 682) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#wayCost` (function, line 541) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#WOUNDED_PACE` (variable, line 423) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#WOUNDED_RUSH` (variable, line 425) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#woundOf` (function, line 418) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#woundPace` (function, line 427) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#Zebra` (interface, line 834) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#zebraAhead` (function, line 885) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#ZEBRAS` (variable, line 848) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#ZONE_CELL` (variable, line 99) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#ZONES` (variable, line 1516) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#zonesNearInto` (function, line 1518) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`

### src/sim/ambient/ambientIndex.ts

- `src/sim/ambient/ambientIndex.ts#atRoll` (function, line 394) ← call from `src/sim/ambient/ambient.ts#AmbientWorld.openCars`
- `src/sim/ambient/ambientIndex.ts#BuildingMix` (class, line 215) ← construct from `src/sim/ambient/ambient.ts#AmbientWorld.buildingMix`
- `src/sim/ambient/ambientIndex.ts#BuildingMix.around` (method, line 291) ← call from `src/sim/ambient/ambient.ts#AmbientWorld.mixAround`
- `src/sim/ambient/ambientIndex.ts#BuildingMix.constructor` (constructor, line 230) ← callbackAssign from `src/core/idSlots.ts#IdSlots.take`, `src/render/buildings/cellPool.ts#FloorColumns.constructor`, `src/render/buildings/townColumns.ts#TownColumns.constructor`, `src/world/buildings/cameraSolids.ts#SolidFiling.constructor`, …
- `src/sim/ambient/ambientIndex.ts#BuildingMix.sync` (method, line 238) ← call from `src/sim/ambient/ambient.ts#AmbientWorld.mixAround`
- `src/sim/ambient/ambientIndex.ts#SightWalls` (class, line 50) ← construct from `src/sim/ambient/ambient.ts#AmbientWorld.walls`
- `src/sim/ambient/ambientIndex.ts#SightWalls.blocked` (method, line 159) ← call from `src/sim/ambient/ambient.ts#AmbientWorld.behindWall`
- `src/sim/ambient/ambientIndex.ts#SightWalls.constructor` (constructor, line 66) ← callbackAssign from `src/core/idSlots.ts#IdSlots.take`, `src/render/buildings/cellPool.ts#FloorColumns.constructor`, `src/render/buildings/townColumns.ts#TownColumns.constructor`, `src/world/buildings/cameraSolids.ts#SolidFiling.constructor`, …
- `src/sim/ambient/ambientIndex.ts#SightWalls.sync` (method, line 71) ← call from `src/sim/ambient/ambient.ts#AmbientWorld.behindWall`
- `src/sim/ambient/ambientIndex.ts#spotKey` (function, line 35) ← call from `src/sim/ambient/ambient.ts#AmbientWorld.stand`, `src/sim/ambient/ambient.ts#AmbientWorld.standSource`
- `src/sim/ambient/ambientIndex.ts#StreetLength` (class, line 348) ← construct from `src/sim/ambient/ambient.ts#AmbientWorld.street`
- `src/sim/ambient/ambientIndex.ts#StreetLength.around` (method, line 354) ← call from `src/sim/ambient/ambient.ts#AmbientWorld.roadAround`

### src/sim/ambient/demand.ts

- `src/sim/ambient/demand.ts#busy` (function, line 9) ← call from `src/sim/ambient/ambient.ts#AmbientWorld.edgePeople`, `src/sim/ambient/ambient.ts#AmbientWorld.peopleTarget`, `src/sim/ambient/ambient.ts#AmbientWorld.pickPlace`, `src/sim/ambient/ambient.ts#AmbientWorld.step.density`
- `src/sim/ambient/demand.ts#CARS` (variable, line 52) ← read from `src/sim/ambient/ambient.ts#AmbientWorld.step`
- `src/sim/ambient/demand.ts#Mix` (interface, line 6) ← read from `src/sim/ambient/ambient.ts#AmbientWorld.peopleTarget`, `src/sim/ambient/ambient.ts#AmbientWorld.step.density`
- `src/sim/ambient/demand.ts#Mix` (interface, line 6) ← typeRef from `src/sim/ambient/ambient.ts#AmbientWorld.mixAround`, `src/sim/ambient/ambient.ts#AmbientWorld.step.density`
- `src/sim/ambient/demand.ts#outShare` (function, line 46) ← call from `src/sim/ambient/ambient.ts#AmbientWorld.edgePeople`, `src/sim/ambient/ambient.ts#AmbientWorld.pickPlace`
- `src/sim/ambient/demand.ts#PEOPLE` (variable, line 51) ← read from `src/sim/ambient/ambient.ts#AmbientWorld.edgePeople`, `src/sim/ambient/ambient.ts#AmbientWorld.peopleTarget`, `src/sim/ambient/ambient.ts#AmbientWorld.pickPlace`, `src/sim/ambient/ambient.ts#AmbientWorld.step`

### src/sim/ambient/lotPlaces.ts

- `src/sim/ambient/lotPlaces.ts#LotPlaces` (class, line 98) ← construct from `src/sim/ambient/ambient.ts#newPlaces`
- `src/sim/ambient/lotPlaces.ts#LotPlaces` (class, line 98) ← typeRef from `src/sim/ambient/ambient.ts#newPlaces`
- `src/sim/ambient/lotPlaces.ts#LotPlaces.frontages` (method, line 188) ← call from `src/sim/ambient/ambient.ts#AmbientWorld.edgePeople`
- `src/sim/ambient/lotPlaces.ts#LotPlaces.pick` (method, line 201) ← call from `src/sim/ambient/ambient.ts#AmbientWorld.pickPlace`
- `src/sim/ambient/lotPlaces.ts#LotPlaces.ready` (method, line 182) ← call from `src/sim/ambient/ambient.ts#AmbientWorld.placesReady`
- `src/sim/ambient/lotPlaces.ts#LotPlaces.sync` (method, line 133) ← call from `src/sim/ambient/ambient.ts#AmbientWorld.refreshPlaces`
- `src/sim/ambient/lotPlaces.ts#PLACE_KINDS` (variable, line 57) ← read from `src/sim/ambient/ambient.ts#AmbientWorld.edgePeople`, `src/sim/ambient/ambient.ts#AmbientWorld.pickPlace`

### src/sim/intersections/admission.ts

- `src/sim/intersections/admission.ts#compactReservationChain` (function, line 411) ← call from `src/sim/pipeline.ts#reservedChainValid`
- `src/sim/intersections/admission.ts#nextConnector` (function, line 1103) ← call from `src/sim/pipeline.ts#stallCounters`
- `src/sim/intersections/admission.ts#stepAdmission` (function, line 58) ← call from `src/sim/pipeline.ts#step`

### src/sim/intersections/claims.ts

- `src/sim/intersections/claims.ts#ClaimTable` (class, line 35) ← construct from `src/sim/world.ts#SimWorld.claims`
- `src/sim/intersections/claims.ts#ClaimTable.clear` (method, line 171) ← call from `src/sim/world.ts#SimWorld.reset`
- `src/sim/intersections/claims.ts#ClaimTable.dropMissing` (method, line 159) ← call from `src/sim/world.ts#SimWorld.rebuildVehicleTopology`, `src/sim/world.ts#SimWorld.windowMoved`
- `src/sim/intersections/claims.ts#ClaimTable.holds` (method, line 50) ← call from `src/sim/invariants.ts#runAudit`
- `src/sim/intersections/claims.ts#ClaimTable.points` (method, line 54) ← call from `src/sim/pipeline.ts#ensureVehicleRoutes`, `src/sim/pipeline.ts#rebindVehicles`, `src/sim/pipeline.ts#reservedChainValid`
- `src/sim/intersections/claims.ts#ClaimTable.releaseAll` (method, line 149) ← call from `src/sim/world.ts#SimWorld.removeVehicle`
- `src/sim/intersections/claims.ts#ClaimTable.releaseConnector` (method, line 141) ← call from `src/sim/pipeline.ts#ensureVehicleRoutes`, `src/sim/pipeline.ts#rebindVehicles`, `src/sim/pipeline.ts#reservedChainValid`

### src/sim/intersections/crossingSpans.ts

- `src/sim/intersections/crossingSpans.ts#CrossingSpans` (class, line 32) ← construct from `src/sim/world.ts#SimWorld.crossingSpans`
- `src/sim/intersections/crossingSpans.ts#CrossingSpans.span` (method, line 138) ← call from `src/sim/invariants.ts#runAudit`
- `src/sim/intersections/crossingSpans.ts#CrossingSpans.update` (method, line 54) ← call from `src/sim/world.ts#SimWorld.rebuildWalkTopology`
- `src/sim/intersections/crossingSpans.ts#pedestrianAffectsSpan` (function, line 222) ← call from `src/sim/invariants.ts#runAudit`
- `src/sim/intersections/crossingSpans.ts#reservationCoversCrossing` (function, line 203) ← call from `src/sim/invariants.ts#runAudit`

### src/sim/intersections/spillback.ts

- `src/sim/intersections/spillback.ts#hasDownstreamStorage` (function, line 9) ← call from `src/sim/world.ts#SimWorld.signalDemand`

### src/sim/meso/demand.ts

- `src/sim/meso/demand.ts#DemandCells.constructor` (constructor, line 36) ← callbackAssign from `src/core/idSlots.ts#IdSlots.take`, `src/render/buildings/cellPool.ts#FloorColumns.constructor`, `src/render/buildings/townColumns.ts#TownColumns.constructor`, `src/world/buildings/cameraSolids.ts#SolidFiling.constructor`, …

### src/sim/meso/memory.ts

- `src/sim/meso/memory.ts#deltaTransfer` (function, line 51) ← call from `src/sim/host/simHost.ts#SimHost.publish`
- `src/sim/meso/memory.ts#MesoDelta` (interface, line 34) ← read from `src/sim/host/simClient.ts#SimClient.keepMeso`
- `src/sim/meso/memory.ts#MesoDelta` (interface, line 34) ← typeRef from `src/sim/host/protocol.ts#FromSim`, `src/sim/host/simClient.ts#SimClient.keepMeso`, `src/sim/host/simClient.ts#SimClient.mesoDeltas`

### src/sim/meso/meso.ts

- `src/sim/meso/meso.ts#Meso` (class, line 49) ← construct from `src/sim/host/simHost.ts#SimHost.meso`
- `src/sim/meso/meso.ts#Meso.requestWhole` (method, line 157) ← call from `src/sim/host/simHost.ts#SimHost.handle`
- `src/sim/meso/meso.ts#Meso.takeDelta` (method, line 162) ← call from `src/sim/host/simHost.ts#SimHost.publish`
- `src/sim/meso/meso.ts#Meso.want` (method, line 167) ← call from `src/sim/host/simHost.ts#SimHost.handle`

### src/sim/params.ts

- `src/sim/params.ts#DRIVER_NOISE` (variable, line 131) ← read from `src/sim/transit/transit.ts#TransitSim.spawnBus`
- `src/sim/params.ts#FLEET_CEILING` (variable, line 119) ← read from `src/sim/ambient/ambient.ts#AmbientWorld.trimToCeilings`, `src/sim/host/protocol.ts#VEHICLES`
- `src/sim/params.ts#JAM_GAP` (variable, line 47) ← read from `src/sim/pipeline.ts#ensureVehicleRoutes`
- `src/sim/params.ts#L_MIN` (variable, line 51) ← read from `src/sim/invariants.ts#runAudit`
- `src/sim/params.ts#MAX_FRAME` (variable, line 10) ← read from `src/sim/clock.ts#SimClock.advance`
- `src/sim/params.ts#MAX_SUBSTEPS` (variable, line 8) ← read from `src/sim/clock.ts#SimClock.advance`
- `src/sim/params.ts#NARROW_SCREEN_SHARE` (variable, line 123) ← read from `src/main.ts#syncPopulationShare`
- `src/sim/params.ts#NARROW_SCREEN_WIDTH` (variable, line 125) ← read from `src/main.ts#syncPopulationShare`
- `src/sim/params.ts#PED_CEILING` (variable, line 120) ← read from `src/sim/ambient/ambient.ts#AmbientWorld.peopleTarget`, `src/sim/ambient/ambient.ts#AmbientWorld.trimToCeilings`, `src/sim/host/protocol.ts#PEDS`
- `src/sim/params.ts#SIGNAL` (variable, line 77) ← read from `src/sim/invariants.ts#runAudit`
- `src/sim/params.ts#STUCK_SECONDS` (variable, line 128) ← read from `src/sim/invariants.ts#runAudit`, `src/sim/pipeline.ts#runWatchdog`

### src/sim/populationGovernor.ts

- `src/sim/populationGovernor.ts#cpuPopulationShare` (function, line 18) ← call from `src/main.ts#startPopulationShare`
- `src/sim/populationGovernor.ts#PopulationGovernor` (class, line 22) ← construct from `src/sim/host/simHost.ts#SimHost.governor`
- `src/sim/populationGovernor.ts#PopulationGovernor.feed` (method, line 58) ← call from `src/sim/host/simHost.ts#SimHost.frame`
- `src/sim/populationGovernor.ts#PopulationGovernor.setMax` (method, line 52) ← call from `src/sim/host/simHost.ts#SimHost.frame`
- `src/sim/populationGovernor.ts#PopulationGovernor.share` (getter, line 34) ← call from `src/sim/host/simHost.ts#SimHost.stats`
- `src/sim/populationGovernor.ts#PopulationGovernor.start` (method, line 44) ← call from `src/sim/host/simHost.ts#SimHost.frame`
- `src/sim/populationGovernor.ts#PopulationGovernor.stepMs` (getter, line 39) ← call from `src/sim/host/simHost.ts#SimHost.stats`

### src/sim/roads/controlAdvisor.ts

- `src/sim/roads/controlAdvisor.ts#AutoControl` (type, line 5) ← typeRef from `src/sim/world.ts#SimWorld.stepControlAdvisor`
- `src/sim/roads/controlAdvisor.ts#ControlAdvisor` (class, line 24) ← construct from `src/sim/world.ts#SimWorld.controlAdvisor`, `src/sim/world.ts#SimWorld.reset`
- `src/sim/roads/controlAdvisor.ts#ControlAdvisor` (class, line 24) ← typeRef from `src/sim/world.ts#SimWorld.advisor`
- `src/sim/roads/controlAdvisor.ts#ControlAdvisor.choice` (method, line 32) ← call from `src/sim/host/simAnswers.ts#junctionAnswer`, `src/sim/world.ts#SimWorld.applyAutoControl`, `src/sim/world.ts#SimWorld.stepControlAdvisor`
- `src/sim/roads/controlAdvisor.ts#ControlAdvisor.drop` (method, line 62) ← call from `src/sim/world.ts#SimWorld.rebuildVehicleTopology`
- `src/sim/roads/controlAdvisor.ts#ControlAdvisor.evaluate` (method, line 45) ← call from `src/sim/world.ts#SimWorld.stepControlAdvisor`
- `src/sim/roads/controlAdvisor.ts#ControlAdvisor.forget` (method, line 57) ← call from `src/sim/world.ts#SimWorld.rebuildVehicleTopology`
- `src/sim/roads/controlAdvisor.ts#ControlAdvisor.tick` (method, line 37) ← call from `src/sim/world.ts#SimWorld.stepControlAdvisor`
- `src/sim/roads/controlAdvisor.ts#QUIET_FLOW` (variable, line 22) ← read from `src/sim/world.ts#SimWorld.stepControlAdvisor`

### src/sim/roads/flowStats.ts

- `src/sim/roads/flowStats.ts#FlowStats` (class, line 12) ← construct from `src/sim/world.ts#SimWorld.flowStats`, `src/sim/world.ts#SimWorld.reset`
- `src/sim/roads/flowStats.ts#FlowStats` (class, line 12) ← typeRef from `src/sim/world.ts#SimWorld.flow`
- `src/sim/roads/flowStats.ts#FlowStats.drop` (method, line 88) ← call from `src/sim/world.ts#SimWorld.rebuildVehicleTopology`
- `src/sim/roads/flowStats.ts#FlowStats.hold` (method, line 67) ← call from `src/sim/world.ts#SimWorld.stepControlAdvisor`
- `src/sim/roads/flowStats.ts#FlowStats.keepOnly` (method, line 82) ← call from `src/sim/world.ts#SimWorld.rebuildVehicleTopology`
- `src/sim/roads/flowStats.ts#FlowStats.measured` (field, line 19) ← read from `src/sim/world.ts#SimWorld.stepControlAdvisor`
- `src/sim/roads/flowStats.ts#FlowStats.rate` (method, line 58) ← call from `src/sim/host/simAnswers.ts#junctionAnswer`, `src/sim/world.ts#SimWorld.stepControlAdvisor`
- `src/sim/roads/flowStats.ts#FlowStats.record` (method, line 21) ← call from `src/sim/world.ts#SimWorld.enterLanelet`
- `src/sim/roads/flowStats.ts#FlowStats.resume` (method, line 72) ← call from `src/sim/world.ts#SimWorld.stepControlAdvisor`
- `src/sim/roads/flowStats.ts#FlowStats.step` (method, line 35) ← call from `src/sim/world.ts#SimWorld.stepControlAdvisor`
- `src/sim/roads/flowStats.ts#FlowStats.takeCounted` (method, line 29) ← call from `src/sim/world.ts#SimWorld.stepControlAdvisor`

### src/sim/roads/greenWave.ts

- `src/sim/roads/greenWave.ts#greenWave` (function, line 8) ← call from `src/sim/host/simAnswers.ts#answer`

### src/sim/routing/destination.ts

- `src/sim/routing/destination.ts#chooseVehicleDestination` (function, line 15) ← call from `src/sim/ambient/ambient.ts#AmbientWorld.traffic`

### src/sim/routing/laneReach.ts

- `src/sim/routing/laneReach.ts#LaneReaches` (class, line 340) ← construct from `src/sim/world.ts#SimWorld.laneReaches`
- `src/sim/routing/laneReach.ts#LaneReaches.follow` (method, line 361) ← call from `src/sim/world.ts#SimWorld.rebuildVehicleTopology`

### src/sim/routing/routeIndex.ts

- `src/sim/routing/routeIndex.ts#RouteIndex` (class, line 4) ← construct from `src/sim/world.ts#SimWorld.routes`
- `src/sim/routing/routeIndex.ts#RouteIndex.clear` (method, line 63) ← call from `src/sim/world.ts#SimWorld.vehicles`
- `src/sim/routing/routeIndex.ts#RouteIndex.file` (method, line 12) ← call from `src/sim/world.ts#SimWorld.noteRoute`, `src/sim/world.ts#SimWorld.vehicles`
- `src/sim/routing/routeIndex.ts#RouteIndex.routing` (method, line 59) ← call from `src/sim/world.ts#SimWorld.vehiclesReached`
- `src/sim/routing/routeIndex.ts#RouteIndex.unfile` (method, line 46) ← call from `src/sim/world.ts#SimWorld.vehicles`

### src/sim/routing/router.ts

- `src/sim/routing/router.ts#cutAtBreak` (function, line 377) ← call from `src/sim/pipeline.ts#pinReservedRoute`
- `src/sim/routing/router.ts#extend` (function, line 189) ← call from `src/sim/pipeline.ts#ensureVehicleRoutes`, `src/sim/pipeline.ts#pinReservedRoute`
- `src/sim/routing/router.ts#planFrom` (function, line 107) ← call from `src/sim/ambient/ambient.ts#AmbientWorld.traffic`, `src/sim/pipeline.ts#ensureVehicleRoutes`, `src/sim/transit/transit.ts#TransitSim.sendBus`, `src/sim/transit/transit.ts#TransitSim.spawnBus`
- `src/sim/routing/router.ts#reconsiderRoute` (function, line 312) ← call from `src/sim/pipeline.ts#ensureVehicleRoutes`
- `src/sim/routing/router.ts#repairRoute` (function, line 330) ← call from `src/sim/pipeline.ts#ensureVehicleRoutes`, `src/sim/pipeline.ts#pinReservedRoute`, `src/sim/pipeline.ts#rebindVehicles`

### src/sim/signals/fsm.ts

- `src/sim/signals/fsm.ts#createController` (function, line 76) ← call from `src/sim/world.ts#SimWorld.syncControllers`
- `src/sim/signals/fsm.ts#rebuildController` (function, line 363) ← call from `src/sim/world.ts#SimWorld.syncControllers`
- `src/sim/signals/fsm.ts#seekFixed` (function, line 415) ← call from `src/sim/world.ts#SimWorld.syncControllers`
- `src/sim/signals/fsm.ts#SignalClock` (interface, line 18) ← read from `src/sim/pipeline.ts#step`
- `src/sim/signals/fsm.ts#SignalClock` (interface, line 18) ← typeRef from `src/sim/world.ts#SimWorld.signalClock`
- `src/sim/signals/fsm.ts#SignalClock` (interface, line 18) ← write from `src/sim/pipeline.ts#step`, `src/sim/world.ts#SimWorld.reset`
- `src/sim/signals/fsm.ts#SignalController` (interface, line 21) ← read from `src/sim/host/simAnswers.ts#junctionAnswer`, `src/sim/host/simHost.ts#SimHost.planId`, `src/sim/host/simHost.ts#SimHost.signals.tell`, `src/sim/invariants.ts#followSignals`, …
- `src/sim/signals/fsm.ts#SignalController` (interface, line 21) ← typeRef from `src/sim/host/simHost.ts#SimHost.planId`, `src/sim/host/simHost.ts#SimHost.signals.tell`, `src/sim/host/simHost.ts#SimHost.tellEvery`, `src/sim/invariants.ts#auditedSignals`, …
- `src/sim/signals/fsm.ts#SignalDeps` (interface, line 41) ← call from `src/sim/invariants.ts#runAudit`
- `src/sim/signals/fsm.ts#SignalDeps` (interface, line 41) ← typeRef from `src/sim/world.ts#WorldSignalDeps`
- `src/sim/signals/fsm.ts#stepController` (function, line 171) ← call from `src/sim/pipeline.ts#step`

### src/sim/signals/plan.ts

- `src/sim/signals/plan.ts#CrossingId` (type, line 8) ← typeRef from `src/sim/crossings/state.ts#CrossingStates`, `src/sim/peds/sidewalk.ts#SidewalkEdge`, `src/sim/peds/sidewalk.ts#SidewalkGraph.crossings`, `src/sim/peds/sidewalk.ts#SidewalkGraph.kerbsAt`, …
- `src/sim/signals/plan.ts#makeCrossingId` (function, line 370) ← call from `src/sim/peds/sidewalk.ts#SidewalkGraph.crossingsAt`, `src/sim/world.ts#SimWorld.crossingsAt`
- `src/sim/signals/plan.ts#SignalPlan` (interface, line 26) ← read from `src/sim/host/simAnswers.ts#junctionAnswer`, `src/sim/invariants.ts#followSignals`, `src/sim/invariants.ts#maxSignalCycle`, `src/sim/invariants.ts#refileCycles`, …
- `src/sim/signals/plan.ts#Stage` (interface, line 10) ← read from `src/sim/host/simAnswers.ts#junctionAnswer`, `src/sim/invariants.ts#maxSignalCycle`, `src/sim/invariants.ts#runAudit`, `src/sim/world.ts#SimWorld.junctionQuiet`

### src/sim/signals/query.ts

- `src/sim/signals/query.ts#anyGreen` (function, line 19) ← call from `src/sim/invariants.ts#runAudit`
- `src/sim/signals/query.ts#SignalState` (type, line 5) ← typeRef from `src/render/signals.ts#createSignalHeads`, `src/render/signals.ts#Head`, `src/sim/host/protocol.ts#SignalLights`, `src/sim/host/simClient.ts#SignalFeed`, …
- `src/sim/signals/query.ts#signalStateFor` (function, line 9) ← call from `src/sim/host/simHost.ts#SimHost.signals.tell`, `src/sim/invariants.ts#runAudit`, `src/sim/pipeline.ts#stallCounters`

### src/sim/vehicles/archetypes.ts

- `src/sim/vehicles/archetypes.ts#Archetype` (interface, line 12) ← read from `src/actionsWiring.ts#createActions.explodeAt`, `src/actionsWiring.ts#createActions.shootVehicle`, `src/actionsWiring.ts#VEHICLE_HALF_SPAN`, `src/lab/inspector/entity.ts#agentsNear`, …
- `src/sim/vehicles/archetypes.ts#Archetype` (interface, line 12) ← typeRef from `src/render/playEffects.ts#PlayEffects`, `src/render/renderer.ts#BlastHit`, `src/render/renderer.ts#SceneHandle`, `src/sim/ambient/ambient.ts#TRAFFIC`, …
- `src/sim/vehicles/archetypes.ts#archetypeById` (function, line 232) ← call from `src/sim/host/simClient.ts#newView`, `src/sim/host/simClient.ts#SimClient.applyParked`, `src/sim/host/simClient.ts#SimClient.readTick`
- `src/sim/vehicles/archetypes.ts#ARCHETYPES` (variable, line 50) ← read from `src/actionsWiring.ts#VEHICLE_HALF_SPAN`, `src/sim/ambient/ambient.ts#PARKED_KINDS`, `src/sim/ambient/ambient.ts#TRAFFIC`, `src/sim/host/simClient.ts#SimClient.applyParked`, …
- `src/sim/vehicles/archetypes.ts#bodyClassOfArchetype` (function, line 236) ← call from `src/sim/ambient/ambient.ts#AmbientWorld.traffic`, `src/sim/pipeline.ts#ensureVehicleRoutes`

### src/sim/vehicles/cycleLane.ts

- `src/sim/vehicles/cycleLane.ts#cycleShift` (function, line 22) ← call from `src/sim/pose.ts#vehicleFramesInto`, `src/sim/pose.ts#vehiclePoseInto`

### src/sim/vehicles/driver.ts

- `src/sim/vehicles/driver.ts#DriverParams` (interface, line 7) ← read from `src/sim/pipeline.ts#ensureVehicleRoutes`, `src/sim/pipeline.ts#stallCounters`
- `src/sim/vehicles/driver.ts#makeDriver` (function, line 48) ← call from `src/sim/transit/transit.ts#TransitSim.spawnBus`

### src/sim/vehicles/idm.ts

- `src/sim/vehicles/idm.ts#ConstraintSet` (interface, line 38) ← read from `src/sim/invariants.ts#runAudit`, `src/sim/pipeline.ts#stallCounters`
- `src/sim/vehicles/idm.ts#Obstacle` (interface, line 6) ← read from `src/sim/invariants.ts#runAudit`, `src/sim/pipeline.ts#stallCounters`

### src/sim/vehicles/integrate.ts

- `src/sim/vehicles/integrate.ts#integrateAll` (function, line 18) ← call from `src/sim/pipeline.ts#step`

### src/sim/vehicles/kerbStops.ts

- `src/sim/vehicles/kerbStops.ts#KerbStop` (interface, line 22) ← typeRef from `src/sim/host/protocol.ts#VehicleRich`, `src/sim/host/simClient.ts#VehicleView`
- `src/sim/vehicles/kerbStops.ts#stepKerbStops` (function, line 192) ← call from `src/sim/pipeline.ts#step`

### src/sim/vehicles/laneChange.ts

- `src/sim/vehicles/laneChange.ts#stepLaneChange` (function, line 41) ← call from `src/sim/pipeline.ts#step`

### src/sim/vehicles/obstacles.ts

- `src/sim/vehicles/obstacles.ts#longitudinalConstraints` (function, line 109) ← call from `src/sim/pipeline.ts#step`

### src/sim/vehicles/spawn.ts

- `src/sim/vehicles/spawn.ts#spawnVehicleAt` (function, line 232) ← call from `src/sim/ambient/ambient.ts#AmbientWorld.openCars`, `src/sim/ambient/ambient.ts#AmbientWorld.traffic`
- `src/sim/vehicles/spawn.ts#stepDespawn` (function, line 295) ← call from `src/sim/pipeline.ts#step`
- `src/sim/vehicles/spawn.ts#stepDispatch` (function, line 98) ← call from `src/sim/pipeline.ts#step`
- `src/sim/vehicles/spawn.ts#trafficTarget` (function, line 32) ← call from `src/sim/ambient/ambient.ts#AmbientWorld.open`

### src/sim/vehicles/state.ts

- `src/sim/vehicles/state.ts#ConnectorClearance` (interface, line 23) ← read from `src/sim/invariants.ts#runAudit`, `src/sim/pipeline.ts#rebindVehicles`, `src/sim/world.ts#SimWorld.vehiclesReached`
- `src/sim/vehicles/state.ts#createVehicle` (function, line 181) ← call from `src/sim/transit/transit.ts#TransitSim.spawnBus`
- `src/sim/vehicles/state.ts#FreePose` (interface, line 155) ← read from `src/sim/ambient/ambient.ts#AmbientWorld.moverCells`, `src/sim/ambient/ambient.ts#AmbientWorld.stand`, `src/sim/host/simHost.ts#SimHost.act`, `src/sim/host/simHost.ts#SimHost.parked`, …
- `src/sim/vehicles/state.ts#FreePose` (interface, line 155) ← typeRef from `src/sim/ambient/ambient.ts#AmbientWorld.stand`, `src/sim/ambient/ambient.ts#SceneryCar`
- `src/sim/vehicles/state.ts#Kinematics` (interface, line 11) ← read from `src/sim/host/simHost.ts#SimHost.publish.writeVehicle`, `src/sim/invariants.ts#runAudit`, `src/sim/pose.ts#bodyFrame`, `src/sim/pose.ts#bodyOffset`, …
- `src/sim/vehicles/state.ts#Kinematics` (interface, line 11) ← typeRef from `src/sim/pose.ts#axleFrame`, `src/sim/pose.ts#bodyFrame`, `src/sim/pose.ts#bodyOffset`
- `src/sim/vehicles/state.ts#snapshot` (function, line 166) ← call from `src/sim/transit/transit.ts#TransitSim.spawnBus`
- `src/sim/vehicles/state.ts#snapshotInto` (function, line 171) ← call from `src/sim/pipeline.ts#step`
- `src/sim/vehicles/state.ts#StandingCar` (interface, line 147) ← inherit from `src/sim/ambient/ambient.ts#SceneryCar`
- `src/sim/vehicles/state.ts#StandingCar` (interface, line 147) ← read from `src/sim/host/simHost.ts#SimHost.parked`
- `src/sim/vehicles/state.ts#StandingCar` (interface, line 147) ← typeRef from `src/sim/ambient/ambient.ts#AmbientWorld.isParked`, `src/sim/ambient/ambient.ts#AmbientWorld.logParked`, `src/sim/ambient/ambient.ts#AmbientWorld.parked`, `src/sim/ambient/ambient.ts#AmbientWorld.parkedAt`, …
- `src/sim/vehicles/state.ts#strandVehicle` (function, line 246) ← call from `src/sim/host/simHost.ts#SimHost.act`
- `src/sim/vehicles/state.ts#Vehicle` (interface, line 30) ← read from `src/sim/ambient/ambient.ts#AmbientWorld.moverCells`, `src/sim/ambient/ambient.ts#AmbientWorld.stand`, `src/sim/ambient/ambient.ts#AmbientWorld.traffic`, `src/sim/ambient/ambient.ts#AmbientWorld.trimToCeilings`, …
- `src/sim/vehicles/state.ts#Vehicle` (interface, line 30) ← typeRef from `src/sim/ambient/ambient.ts#AmbientWorld.moverCells`, `src/sim/ambient/ambient.ts#AmbientWorld.movers`, `src/sim/ambient/ambient.ts#AmbientWorld.parkInWindow`, `src/sim/ambient/ambient.ts#AmbientWorld.removeParked`, …
- `src/sim/vehicles/state.ts#Vehicle` (interface, line 30) ← write from `src/sim/ambient/ambient.ts#AmbientWorld.traffic`, `src/sim/pipeline.ts#ensureVehicleRoutes`, `src/sim/pipeline.ts#pinReservedRoute`, `src/sim/pipeline.ts#rebindVehicles`, …

### src/sim/vehicles/vehicleId.ts

- `src/sim/vehicles/vehicleId.ts#VehicleId` (type, line 2) ← typeRef from `src/sim/transit/transit.ts#Bus`, `src/sim/transit/transit.ts#TransitSim.spawnBus`, `src/sim/world.ts#LaneletRuntime`, `src/sim/world.ts#NO_RUNTIME`, …

### src/sim/vehicleSignals.ts

- `src/sim/vehicleSignals.ts#indicatorSide` (function, line 31) ← call from `src/sim/host/simHost.ts#SimHost.publish.writeVehicle`
- `src/sim/vehicleSignals.ts#pathCurvature` (function, line 95) ← call from `src/sim/host/simHost.ts#SimHost.publish.writeVehicle`
- `src/sim/vehicleSignals.ts#WheelOdometer` (class, line 115) ← construct from `src/sim/host/simHost.ts#SimHost.odometer`
- `src/sim/vehicleSignals.ts#WheelOdometer.advance` (method, line 119) ← call from `src/sim/host/simHost.ts#SimHost.publish.writeVehicle`

### src/ui/roads/junctionPanel.ts

- `src/ui/roads/junctionPanel.ts#mountJunctionPanel` (function, line 49) ← call from `src/ui/inspector.ts#renderNode`
- `src/ui/roads/junctionPanel.ts#mountJunctionPanel` (function, line 49) ← typeRef from `src/ui/inspector.ts#junctionHost`
- `src/ui/roads/junctionPanel.ts#refreshJunctionFlows` (function, line 208) ← call from `src/ui/inspector.ts#renderCurrent`

### src/world/conflictPoints.ts

- `src/world/conflictPoints.ts#ConflictIndex` (class, line 43) ← construct from `src/sim/world.ts#SimWorld.conflicts`
- `src/world/conflictPoints.ts#ConflictIndex.add` (method, line 434) ← implementation from `src/sim/world.ts#SimWorld.makeSignalDeps`
- `src/world/conflictPoints.ts#ConflictIndex.build` (method, line 304) ← call from `src/sim/world.ts#SimWorld.rebuildVehicleTopology`
- `src/world/conflictPoints.ts#ConflictIndex.entering` (method, line 100) ← call from `src/sim/world.ts#SimWorld.moveWindowSteps`
- `src/world/conflictPoints.ts#ConflictIndex.holds` (method, line 95) ← call from `src/sim/world.ts#SimWorld.makeSignalDeps`, `src/sim/world.ts#SimWorld.planInputsOf`
- `src/world/conflictPoints.ts#ConflictIndex.holdsWindow` (method, line 112) ← call from `src/sim/world.ts#SimWorld.keepMicroWindow`
- `src/world/conflictPoints.ts#ConflictIndex.inWindowOf` (method, line 107) ← call from `src/sim/world.ts#SimWorld.countsAt`
- `src/world/conflictPoints.ts#ConflictIndex.lookup` (field, line 123) ← callbackAssign from `src/sim/host/simHost.ts#SimHost.init`
- `src/world/conflictPoints.ts#ConflictIndex.lookup` (field, line 123) ← implementation from `src/sim/host/simHost.ts#SimHost.init`
- `src/world/conflictPoints.ts#ConflictIndex.lookup` (field, line 123) ← write from `src/sim/host/simHost.ts#SimHost.init`
- `src/world/conflictPoints.ts#ConflictIndex.points` (field, line 44) ← read from `src/sim/world.ts#SimWorld.makeSignalDeps`
- `src/world/conflictPoints.ts#ConflictIndex.prepare` (method, line 141) ← call from `src/sim/world.ts#SimWorld.moveWindowSteps`, `src/sim/world.ts#SimWorld.prepareVehicleTopology`
- `src/world/conflictPoints.ts#ConflictIndex.refs` (method, line 475) ← call from `src/sim/world.ts#SimWorld.makeSignalDeps`
- `src/world/conflictPoints.ts#ConflictIndex.seed` (method, line 126) ← call from `src/sim/host/simHost.ts#SimHost.init`
- `src/world/conflictPoints.ts#ConflictIndex.setWindow` (method, line 85) ← call from `src/sim/world.ts#SimWorld.moveWindowSteps`, `src/sim/world.ts#SimWorld.useMicroWindow`
- `src/world/conflictPoints.ts#ConflictIndex.takeMeasured` (method, line 131) ← call from `src/sim/host/simHost.ts#SimHost.stats`
- `src/world/conflictPoints.ts#ConflictIndex.update` (method, line 329) ← call from `src/sim/world.ts#SimWorld.rebuildVehicleTopology`
- `src/world/conflictPoints.ts#ConflictPoint` (interface, line 27) ← call from `src/sim/world.ts#SimWorld.makeSignalDeps`

### src/world/conflictTable.ts

- `src/world/conflictTable.ts#BodyClass` (type, line 15) ← typeRef from `src/sim/world.ts#CAR_CLASS`, `src/world/junctionBuild.ts#MadeConnector`
- `src/world/conflictTable.ts#ChosenTurn` (interface, line 21) ← typeRef from `src/conflictPool.ts#ConflictPool.chooseTurns`, `src/conflictPool.ts#ConflictPool.start`, `src/conflictTable.worker.ts#serve`, `src/sim/world.ts#SimWorld.prepareVehicleTopology`, …
- `src/world/conflictTable.ts#ConflictRef` (interface, line 530) ← read from `src/sim/world.ts#SimWorld.makeSignalDeps`
- `src/world/conflictTable.ts#FinishAsk` (interface, line 198) ← read from `src/conflictPool.ts#ConflictPool.finishPairs`
- `src/world/conflictTable.ts#FinishAsk` (interface, line 198) ← typeRef from `src/conflictPool.ts#ConflictPool.finishPairs`
- `src/world/conflictTable.ts#FinishedPair` (interface, line 205) ← typeRef from `src/conflictPool.ts#ConflictPool.finishPairs`, `src/conflictPool.ts#ConflictPool.start`, `src/conflictTable.worker.ts#serve`
- `src/world/conflictTable.ts#finishPair` (function, line 214) ← call from `src/conflictTable.worker.ts#serve`
- `src/world/conflictTable.ts#JunctionAnswer` (interface, line 486) ← read from `src/conflictTable.worker.ts#serve`
- `src/world/conflictTable.ts#JunctionAnswer` (interface, line 486) ← typeRef from `src/conflictPool.ts#ConflictPool.buildJunctions`, `src/conflictPool.ts#ConflictPool.start`, `src/conflictTable.worker.ts#serve`, `src/world/junctionBuild.ts#buildJunctionOf`
- `src/world/conflictTable.ts#JunctionAsk` (interface, line 476) ← read from `src/world/junctionBuild.ts#buildJunctionOf`
- `src/world/conflictTable.ts#JunctionAsk` (interface, line 476) ← typeRef from `src/conflictPool.ts#ConflictPool.buildJunctions`, `src/conflictTable.worker.ts#JunctionsAsk`, `src/conflictTable.worker.ts#serve`, `src/world/junctionBuild.ts#buildJunctionOf`
- `src/world/conflictTable.ts#JunctionLeg` (interface, line 453) ← read from `src/world/junctionBuild.ts#carriedPair`, `src/world/junctionBuild.ts#junctionMovements`, `src/world/junctionBuild.ts#legLine`
- `src/world/conflictTable.ts#JunctionLeg` (interface, line 453) ← typeRef from `src/world/junctionBuild.ts#carriedPair`, `src/world/junctionBuild.ts#JunctionInput`, `src/world/junctionBuild.ts#junctionMovements`, `src/world/junctionBuild.ts#junctionMovements.leg`, …
- `src/world/conflictTable.ts#JunctionNode` (type, line 450) ← typeRef from `src/world/junctionBuild.ts#carriedPair`, `src/world/junctionBuild.ts#JunctionInput`
- `src/world/conflictTable.ts#LaneAsk` (interface, line 460) ← inherit from `src/world/junctionBuild.ts#JunctionLane`
- `src/world/conflictTable.ts#LaneAsk` (interface, line 460) ← read from `src/world/junctionBuild.ts#buildJunctionOf.lane`, `src/world/junctionBuild.ts#junctionKey`, `src/world/junctionBuild.ts#junctionMovements`, `src/world/junctionBuild.ts#junctionMovements.addConnector`, …
- `src/world/conflictTable.ts#LaneAsk` (interface, line 460) ← typeRef from `src/world/junctionBuild.ts#buildJunctionOf.lane`
- `src/world/conflictTable.ts#PairMeasurer` (interface, line 503) ← call from `src/sim/world.ts#SimWorld.prepareVehicleTopology`
- `src/world/conflictTable.ts#PairMeasurer` (interface, line 503) ← implements from `src/conflictPool.ts#ConflictPool`
- `src/world/conflictTable.ts#PairMeasurer` (interface, line 503) ← reference from `src/sim/world.ts#SimWorld.prepareVehicleTopology`
- `src/world/conflictTable.ts#PairMeasurer` (interface, line 503) ← typeRef from `src/sim/topologyCatchUp.ts#TopologyCatchUp.constructor`, `src/sim/world.ts#SimWorld.moveWindowSteps`, `src/sim/world.ts#SimWorld.prepareVehicleTopology`
- `src/world/conflictTable.ts#pairTable` (function, line 347) ← call from `src/conflictTable.worker.ts#serve`
- `src/world/conflictTable.ts#PairWork` (class, line 339) ← construct from `src/conflictTable.worker.ts#serve`
- `src/world/conflictTable.ts#SampledSweep` (interface, line 80) ← read from `src/conflictTable.worker.ts#serve`
- `src/world/conflictTable.ts#SampledSweep` (interface, line 80) ← typeRef from `src/conflictPool.ts#ConflictPool.sampleSweeps`, `src/conflictPool.ts#ConflictPool.start`, `src/conflictTable.worker.ts#serve`
- `src/world/conflictTable.ts#sampleSweep` (function, line 147) ← call from `src/conflictTable.worker.ts#serve`
- `src/world/conflictTable.ts#SurfaceRings` (interface, line 39) ← read from `src/conflictPool.ts#ConflictPool.chooseTurns`
- `src/world/conflictTable.ts#SurfaceRings` (interface, line 39) ← typeRef from `src/conflictPool.ts#ConflictPool.chooseTurns`, `src/world/junctionTurns.ts#TurnPlan.surfaces`, `src/world/turnChoice.ts#chooseTurnOf`
- `src/world/conflictTable.ts#SweepAsk` (interface, line 71) ← read from `src/conflictPool.ts#ConflictPool.sampleSweeps`
- `src/world/conflictTable.ts#SweepAsk` (interface, line 71) ← typeRef from `src/conflictPool.ts#ConflictPool.sampleSweeps`
- `src/world/conflictTable.ts#SweepSamples` (interface, line 219) ← typeRef from `src/conflictTable.worker.ts#serve`
- `src/world/conflictTable.ts#TABLE_SIZE` (variable, line 211) ← read from `src/conflictPool.ts#ConflictPool.finishPairs`, `src/conflictTable.worker.ts#serve`
- `src/world/conflictTable.ts#TurnAsk` (interface, line 27) ← read from `src/conflictPool.ts#ConflictPool.chooseTurns`, `src/world/turnChoice.ts#chooseTurnOf`
- `src/world/conflictTable.ts#TurnAsk` (interface, line 27) ← typeRef from `src/conflictPool.ts#ConflictPool.chooseTurns`, `src/world/junctionTurns.ts#TurnPlan.asks`, `src/world/turnChoice.ts#chooseTurnOf`
- `src/world/conflictTable.ts#TurnKind` (type, line 447) ← typeRef from `src/world/junctionBuild.ts#classifyTurn`, `src/world/junctionBuild.ts#fallbackTurnRank`, `src/world/junctionBuild.ts#junctionMovements`, `src/world/junctionBuild.ts#junctionMovements.addConnector`, …

### src/world/lanelets.ts

- `src/world/lanelets.ts#Connector` (interface, line 62) ← read from `src/editor/streetObjects.ts#commitUturn`, `src/sim/host/simAnswers.ts#junctionAnswer`, `src/sim/host/simHost.ts#SimHost.publish.writeVehicle`, `src/sim/invariants.ts#runAudit`, …
- `src/world/lanelets.ts#Connector` (interface, line 62) ← typeRef from `src/sim/world.ts#PlanInputs`, `src/sim/world.ts#SimWorld.connector`
- `src/world/lanelets.ts#GraphChange` (interface, line 131) ← read from `src/sim/peds/sidewalk.ts#SidewalkGraph.update`, `src/sim/world.ts#SimWorld.fileJunctions`, `src/sim/world.ts#SimWorld.prepReach`, `src/sim/world.ts#SimWorld.rebuildVehicleTopology`, …
- `src/world/lanelets.ts#GraphChange` (interface, line 131) ← typeRef from `src/sim/peds/sidewalk.ts#SidewalkGraph.update`, `src/sim/world.ts#SimWorld.fileJunctions`, `src/sim/world.ts#SimWorld.prepReach`, `src/sim/world.ts#SimWorld.topologyLog`
- `src/world/lanelets.ts#JunctionTopology` (interface, line 78) ← read from `src/sim/host/simAnswers.ts#junctionAnswer`, `src/sim/host/simHost.ts#SimHost.signals.layoutOf`, `src/sim/host/simHost.ts#SimHost.signals.tell`, `src/sim/pipeline.ts#stallCounters`, …
- `src/world/lanelets.ts#JunctionTopology` (interface, line 78) ← typeRef from `src/sim/world.ts#PlanInputs`, `src/sim/world.ts#SimWorld.applyAutoControl`, `src/sim/world.ts#SimWorld.planInputsOf`, `src/sim/world.ts#SimWorld.syncControllers`
- `src/world/lanelets.ts#Lanelet` (interface, line 31) ← read from `src/sim/ambient/ambient.ts#AmbientWorld.lanesAround`, `src/sim/ambient/ambient.ts#AmbientWorld.listLanes`, `src/sim/ambient/ambient.ts#AmbientWorld.traffic`, `src/sim/host/simAnswers.ts#count`, …
- `src/world/lanelets.ts#Lanelet` (interface, line 31) ← typeRef from `src/sim/world.ts#SimWorld.lanelet`
- `src/world/lanelets.ts#LaneletGraph` (class, line 137) ← construct from `src/sim/world.ts#SimWorld.graph`, `src/sim/world.ts#SimWorld.prepareVehicleTopology`
- `src/world/lanelets.ts#LaneletGraph` (class, line 137) ← reference from `src/editor/streetObjects.ts#commitUturn`
- `src/world/lanelets.ts#LaneletGraph` (class, line 137) ← typeRef from `src/sim/peds/sidewalk.ts#SidewalkGraph.build`, `src/sim/peds/sidewalk.ts#SidewalkGraph.crossingsAt`, `src/sim/peds/sidewalk.ts#SidewalkGraph.update`, `src/sim/world.ts#SimWorld.prepGraph`, …
- `src/world/lanelets.ts#LaneletGraph.adopt` (method, line 216) ← call from `src/sim/world.ts#SimWorld.rebuildVehicleTopology`
- `src/world/lanelets.ts#LaneletGraph.buildJunction.surfaceOf` (function, line 707) ← indirectCall from `src/world/junctionBuild.ts#junctionMovements.addConnector.shape`
- `src/world/lanelets.ts#LaneletGraph.connectors` (field, line 139) ← read from `src/sim/world.ts#SimWorld.connector`, `src/sim/world.ts#SimWorld.makeSignalDeps`, `src/sim/world.ts#SimWorld.planInputsOf`, `src/sim/world.ts#SimWorld.signalDemand`, …
- `src/world/lanelets.ts#LaneletGraph.current` (method, line 256) ← call from `src/sim/world.ts#SimWorld.rebuildVehicleTopology`
- `src/world/lanelets.ts#LaneletGraph.exitsOf` (method, line 742) ← call from `src/sim/pipeline.ts#ensureVehicleRoutes`, `src/sim/world.ts#SimWorld.bodiesIn`
- `src/world/lanelets.ts#LaneletGraph.forget` (method, line 175) ← call from `src/sim/world.ts#SimWorld.reset`
- `src/world/lanelets.ts#LaneletGraph.inbound` (field, line 144) ← read from `src/world/roads/laneEnds.ts#laneEnds`, `src/world/roads/laneEnds.ts#nodeLanes`
- `src/world/lanelets.ts#LaneletGraph.junctionConnectors` (method, line 726) ← call from `src/editor/streetObjects.ts#commitUturn`
- `src/world/lanelets.ts#LaneletGraph.junctionConnectors` (method, line 726) ← read from `src/editor/streetObjects.ts#commitUturn`
- `src/world/lanelets.ts#LaneletGraph.junctions` (field, line 140) ← read from `src/sim/host/simAnswers.ts#junctionAnswer`, `src/sim/host/simHost.ts#SimHost.signals.layoutOf`, `src/sim/host/simHost.ts#SimHost.signals.tell`, `src/sim/pipeline.ts#stallCounters`, …
- `src/world/lanelets.ts#LaneletGraph.junctions` (field, line 140) ← write from `src/sim/world.ts#SimWorld.applyAutoControl`
- `src/world/lanelets.ts#LaneletGraph.lanelet` (method, line 746) ← call from `src/sim/host/simAnswers.ts#count`, `src/sim/host/simAnswers.ts#roadLanes`, `src/world/roads/laneEnds.ts#laneEnds`, `src/world/roads/laneEnds.ts#nodeLanes.lane`
- `src/world/lanelets.ts#LaneletGraph.lanelets` (field, line 138) ← read from `src/sim/ambient/ambient.ts#AmbientWorld.listLanes`, `src/sim/invariants.ts#runAudit`, `src/sim/pipeline.ts#collectGhosts`, `src/sim/world.ts#SimWorld.junctionOfLane`, …
- `src/world/lanelets.ts#LaneletGraph.linksOfSegment` (method, line 738) ← call from `src/sim/peds/sidewalk.ts#SidewalkGraph.crossingsAt`
- `src/world/lanelets.ts#LaneletGraph.outbound` (field, line 146) ← read from `src/world/roads/laneEnds.ts#laneEnds`, `src/world/roads/laneEnds.ts#nodeLanes`
- `src/world/lanelets.ts#LaneletGraph.redoProvisional` (method, line 357) ← call from `src/sim/world.ts#SimWorld.prepareVehicleTopology`
- `src/world/lanelets.ts#LaneletGraph.revision` (field, line 150) ← read from `src/world/roads/laneEnds.ts#laneEnds`
- `src/world/lanelets.ts#LaneletGraph.siblingLanes` (method, line 751) ← call from `src/sim/pipeline.ts#ensureVehicleRoutes`
- `src/world/lanelets.ts#LaneletGraph.turnPlan` (field, line 167) ← read from `src/sim/world.ts#SimWorld.prepareVehicleTopology`
- `src/world/lanelets.ts#LaneletGraph.turnPlan` (field, line 167) ← write from `src/sim/world.ts#SimWorld.prepareVehicleTopology`
- `src/world/lanelets.ts#LaneletGraph.update` (method, line 180) ← call from `src/sim/world.ts#SimWorld.rebuildVehicleTopology`
- `src/world/lanelets.ts#LaneletGraph.updateSteps` (method, line 188) ← call from `src/sim/world.ts#SimWorld.prepareVehicleTopology`
- `src/world/lanelets.ts#laneletId` (function, line 119) ← call from `src/sim/host/simAnswers.ts#roadLanes`
- `src/world/lanelets.ts#LaneletId` (type, line 27) ← typeRef from `src/sim/ambient/ambient.ts#SceneryCar`, `src/sim/peds/sidewalk.ts#SidewalkEdge`, `src/sim/people/engine.ts#PeopleBridge`, `src/sim/world.ts#LaneletRuntime`, …

### src/world/parking.ts

- `src/world/parking.ts#flipParking` (function, line 61) ← call from `src/editor/commit.ts#joinSegments`
- `src/world/parking.ts#normalizeParking` (function, line 48) ← call from `src/world/doc.ts#RoadDoc.addSegment`, `src/world/doc.ts#RoadDoc.setSegmentParking`, `src/world/doc.ts#segmentRecord`
- `src/world/parking.ts#PARKING_DEPTH` (variable, line 16) ← read from `src/ui/roads/profileEditor.ts#newElement`, `src/world/roads/catalog.ts#shapeProfile.kerb`, `src/world/roads/profile.ts#profileOf.band`, `src/world/roadTypes.ts#resolveProfile`
- `src/world/parking.ts#PARKING_KINDS` (variable, line 6) ← read from `src/ui/inspector.ts#parkingSelect`
- `src/world/parking.ts#parkingAllowed` (function, line 66) ← call from `src/ui/inspector.ts#parkingSelect`, `src/world/roadTypes.ts#resolveProfile.fit`
- `src/world/parking.ts#ParkingKind` (type, line 7) ← typeRef from `src/ui/inspector.ts#parkingSelect`, `src/ui/inspector.ts#renderSegment.setParking`, `src/world/roads/profile.ts#authoredFields.kindAt`, `src/world/roads/profile.ts#profileOf.band`, …
- `src/world/parking.ts#sameParking` (function, line 56) ← call from `src/editor/commit.ts#joinSegments`, `src/editor/roads/profile.ts#applyProfileTo`, `src/world/doc.ts#RoadDoc.setSegmentParking`, `src/world/doc.ts#sameSegment`
- `src/world/parking.ts#SegmentParking` (interface, line 10) ← read from `src/ui/inspector.ts#renderSegment`, `src/world/roadTypes.ts#resolveProfile`
- `src/world/parking.ts#SegmentParking` (interface, line 10) ← typeRef from `src/editor/blocks.ts#commitBlockGrid`, `src/editor/commit.ts#commitDraftInPlace`, `src/editor/commit.ts#commitRoadPath`, `src/editor/commit.ts#dryRunKey`, …

### src/world/parkingLayout.ts

- `src/world/parkingLayout.ts#PARKING_PAINT` (variable, line 53) ← read from `src/render/paintQuads.ts#PAINT_COLOURS`
- `src/world/parkingLayout.ts#ParkingLine` (interface, line 30) ← read from `src/render/roadSurfaceInputs.ts#SurfaceInputs.placeSegment`
- `src/world/parkingLayout.ts#parkingLinesOf` (function, line 70) ← call from `src/render/roadSurfaceInputs.ts#SurfaceInputs.placeSegment`
- `src/world/parkingLayout.ts#parksAlong` (function, line 90) ← call from `src/render/roadSurfaceInputs.ts#SurfaceInputs.placeSegment`, `src/render/roadSurfaceInputs.ts#SurfaceInputs.refresh`

### src/world/roads/rules.ts

- `src/world/roads/rules.ts#ApproachRuleEntry` (interface, line 11) ← read from `src/editor/streetscapeTool.ts#StreetscapeTool.down`
- `src/world/roads/rules.ts#ApproachRuleEntry` (interface, line 11) ← typeRef from `src/ui/inspector.ts#InspectorActions`, `src/world/doc.ts#RoadDoc.setNodeApproachRules`, `src/world/doc.ts#RoadNode`
- `src/world/roads/rules.ts#approachSign` (function, line 122) ← call from `src/world/markings.ts#approachLegendsAt`, `src/world/roads/derivedSigns.ts#signsAtNode`
- `src/world/roads/rules.ts#arrivesAt` (function, line 137) ← call from `src/world/markings.ts#approachLegendsAt`, `src/world/roads/derivedSigns.ts#signApproach`, `src/world/roads/derivedSigns.ts#signsAtNode`
- `src/world/roads/rules.ts#mainRoadLegs` (function, line 73) ← call from `src/sim/world.ts#SimWorld.stepControlAdvisor`
- `src/world/roads/rules.ts#normalizeApproachRules` (function, line 31) ← call from `src/world/doc.ts#finishNode`, `src/world/doc.ts#RoadDoc.setNodeApproachRules`
- `src/world/roads/rules.ts#normalizeSignalSettings` (function, line 45) ← call from `src/world/doc.ts#finishNode`, `src/world/doc.ts#RoadDoc.setNodeSignal`
- `src/world/roads/rules.ts#rulesDigest` (function, line 63) ← call from `src/world/doc.ts#RoadDoc.setNodeApproachRules`, `src/world/doc.ts#sameNode`
- `src/world/roads/rules.ts#signalDigest` (function, line 62) ← call from `src/world/doc.ts#RoadDoc.setNodeSignal`, `src/world/doc.ts#sameNode`
- `src/world/roads/rules.ts#SignalSettings` (interface, line 17) ← read from `src/sim/world.ts#settingsKey`, `src/sim/world.ts#SimWorld.syncControllers`, `src/world/doc.ts#copyNode`, `src/world/doc.ts#savedNode`
- `src/world/roads/rules.ts#SignalSettings` (interface, line 17) ← typeRef from `src/sim/host/simAnswers.ts#answer`, `src/sim/world.ts#settingsKey`, `src/sim/world.ts#SimWorld.planInputsOf`, `src/ui/inspector.ts#InspectorActions`, …

### src/world/turnPaths.ts

- `src/world/turnPaths.ts#bulbTurnPath` (function, line 406) ← call from `src/world/junctionBuild.ts#junctionMovements.addConnector.shape`
- `src/world/turnPaths.ts#chooseTurn` (function, line 539) ← call from `src/world/turnChoice.ts#chooseTurnOf`
- `src/world/turnPaths.ts#JunctionSurface` (class, line 76) ← construct from `src/world/junctionBuild.ts#buildJunctionOf.surfaceOf`, `src/world/turnChoice.ts#chooseTurnOf`
- `src/world/turnPaths.ts#JunctionSurface` (class, line 76) ← typeRef from `src/conflictTable.worker.ts#serve`, `src/world/junctionBuild.ts#buildJunctionOf`, `src/world/junctionBuild.ts#buildJunctionOf.surfaceOf`, `src/world/junctionBuild.ts#junctionMovements`, …
- `src/world/turnPaths.ts#JunctionSurface.key` (field, line 85) ← read from `src/world/junctionBuild.ts#buildJunctionOf`
- `src/world/turnPaths.ts#medianUturnPath` (function, line 448) ← call from `src/world/junctionBuild.ts#junctionMovements.addConnector.shape`
- `src/world/turnPaths.ts#turnPath` (function, line 380) ← call from `src/world/junctionBuild.ts#junctionMovements.addConnector.shape`
- `src/world/turnPaths.ts#TurnPathResult` (interface, line 367) ← typeRef from `src/world/junctionBuild.ts#junctionMovements.addConnector.shape`, `src/world/junctionBuild.ts#MadeConnector`
- `src/world/turnPaths.ts#unflat` (function, line 73) ← call from `src/world/junctionBuild.ts#buildJunctionOf.ring`

## Shared declarations the feature uses

### src/bayPool.ts

- `src/bayPool.ts#BayPool.lanes` (field, line 26) — read by 1: `src/bayPool.ts#BayPool.check`
- `src/bayPool.ts#BayPool.work` (method, line 32) — implementation by 1: `src/sim/agents/parking.ts#baysNow`
- `src/bayPool.ts#Lane` (interface, line 7) — read by 1: `src/bayPool.ts#BayPool.check`
- `src/bayPool.ts#Lane` (interface, line 7) — write by 1: `src/bayPool.ts#BayPool.check`
- `src/bayPool.ts#SILENCE_MS` (variable, line 17) — read by 1: `src/bayPool.ts#BayPool.check`

### src/bays.worker.ts

- `src/bays.worker.ts#serve.beat` (function, line 12) — indirectCall by 1: `src/sim/agents/lotBays.ts#baysOfShare`

### src/conflictPool.ts

- `src/conflictPool.ts#ConflictPool.ask` (method, line 61) — call by 1: `src/conflictPool.ts#ConflictPool.measure`
- `src/conflictPool.ts#ConflictPool.buildJunctions` (method, line 165) — implementation by 2: `src/world/lanelets.ts#LaneletGraph.buildJunctions`, `src/world/lanelets.ts#LaneletGraph.junctionsOffPage`
- `src/conflictPool.ts#ConflictPool.constructor` (constructor, line 24) — read by 1: `src/conflictPool.ts#ConflictPool.measure`
- `src/conflictPool.ts#ConflictPool.finishPairs` (method, line 136) — implementation by 2: `src/world/conflictPoints.ts#ConflictIndex.finishOffPage`, `src/world/conflictPoints.ts#ConflictIndex.prepare`
- `src/conflictPool.ts#ConflictPool.sampleSweeps` (method, line 103) — implementation by 2: `src/world/conflictPoints.ts#ConflictIndex.prepare`, `src/world/conflictPoints.ts#ConflictIndex.sampleOffPage`
- `src/conflictPool.ts#ConflictPool.start` (method, line 210) — call by 1: `src/conflictPool.ts#ConflictPool.measure`

### src/core/aabb.ts

- `src/core/aabb.ts#Aabb` (interface, line 3) — read by 12: `src/sim/agents/kerbBays.ts#fileStreet`, `src/sim/agents/laneGrid.ts#boxOf`, `src/sim/agents/laneGrid.ts#LaneGrid.file`, `src/sim/agents/laneGrid.ts#LaneGrid.inBox`, …

### src/core/changeLog.ts

- `src/core/changeLog.ts#ChangeLog` (class, line 12) — construct by 1: `src/sim/agents/laneGrid.ts#LaneGrid.changes`
- `src/core/changeLog.ts#ChangeLog` (class, line 12) — typeRef by 5: `src/sim/agents/kerbBays.ts#KerbWorld`, `src/sim/agents/laneGrid.ts#LaneGrid.follow`, `src/sim/agents/laneGrid.ts#LaneGridWorld`, `src/sim/intersections/cycles.ts#CycleIndex.follow`, …
- `src/core/changeLog.ts#ChangeLog.push` (method, line 26) — call by 2: `src/sim/agents/laneGrid.ts#LaneGrid.fileAll`, `src/sim/agents/laneGrid.ts#LaneGrid.follow`
- `src/core/changeLog.ts#ChangeLog.since` (method, line 59) — call by 10: `src/sim/agents/kerbBays.ts#followKerbs`, `src/sim/agents/laneGrid.ts#laneChangesSince`, `src/sim/agents/laneGrid.ts#LaneGrid.follow`, `src/sim/agents/parking.ts#baysNow`, …
- `src/core/changeLog.ts#ChangeLog.state` (getter, line 21) — call by 10: `src/sim/agents/kerbBays.ts#followKerbs`, `src/sim/agents/laneGrid.ts#laneChangesSince`, `src/sim/agents/laneGrid.ts#LaneGrid.follow`, `src/sim/agents/parking.ts#baysNow`, …

### src/core/clipper.ts

- `src/core/clipper.ts#MultiPoly` (type, line 9) — typeRef by 1: `src/sim/agents/lorryDocks.ts#AsphaltPiece`

### src/core/derivedCache.ts

- `src/core/derivedCache.ts#readDerived` (function, line 53) — call by 1: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.lodPlan`
- `src/core/derivedCache.ts#writeDerived` (function, line 250) — call by 1: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.lodPlan`

### src/core/digest.ts

- `src/core/digest.ts#Digest` (class, line 6) — construct by 11: `src/sim/agents/kerbBays.ts#kerbKey`, `src/sim/agents/laneGrid.ts#digestOf`, `src/sim/agents/lorryDocks.ts#yardInputs`, `src/sim/agents/parking.ts#buildingWalls`, …
- `src/core/digest.ts#Digest` (class, line 6) — typeRef by 1: `src/world/parkingLayout.ts#parkingDigest`
- `src/core/digest.ts#Digest.add` (method, line 11) — call by 8: `src/sim/agents/kerbBays.ts#kerbKey`, `src/sim/agents/laneGrid.ts#digestOf`, `src/sim/agents/lorryDocks.ts#yardInputs`, `src/sim/agents/parking.ts#buildingWalls`, …
- `src/core/digest.ts#Digest.addAll` (method, line 19) — call by 6: `src/sim/agents/laneGrid.ts#digestOf`, `src/sim/agents/lorryDocks.ts#yardInputs`, `src/world/conflictTable.ts#lineDigest`, `src/world/conflictTable.ts#shapeDigest`, …
- `src/core/digest.ts#Digest.addPlain` (method, line 31) — call by 1: `src/sim/agents/parking.ts#lotKey`
- `src/core/digest.ts#Digest.addText` (method, line 25) — call by 3: `src/sim/agents/laneGrid.ts#digestOf`, `src/sim/agents/lorryDocks.ts#yardInputs`, `src/world/parkingLayout.ts#parkingDigest`
- `src/core/digest.ts#Digest.value` (method, line 59) — call by 11: `src/sim/agents/kerbBays.ts#kerbKey`, `src/sim/agents/laneGrid.ts#digestOf`, `src/sim/agents/lorryDocks.ts#yardInputs`, `src/sim/agents/parking.ts#buildingWalls`, …

### src/core/heap.ts

- `src/core/heap.ts#Heap` (class, line 3) — construct by 5: `src/sim/agents/hybridAStar.ts#holonomic`, `src/sim/agents/hybridAStar.ts#WaySearch.open`, `src/sim/agents/lotNav.ts#buildLotGrid`, `src/sim/agents/lotTraffic.ts#LotTraffic.stays`, …
- `src/core/heap.ts#Heap` (class, line 3) — typeRef by 1: `src/sim/agents/walk.ts#RouteScratch`
- `src/core/heap.ts#Heap.clear` (method, line 13) — call by 2: `src/sim/agents/lotTraffic.ts#LotTraffic.reset`, `src/sim/agents/walk.ts#routeScratch`
- `src/core/heap.ts#Heap.peekKey` (method, line 12) — call by 2: `src/sim/agents/lotTraffic.ts#LotTraffic.ripen`, `src/sim/agents/walk.ts#route`
- `src/core/heap.ts#Heap.pop` (method, line 24) — call by 5: `src/sim/agents/hybridAStar.ts#holonomic`, `src/sim/agents/hybridAStar.ts#WaySearch.run`, `src/sim/agents/lotNav.ts#buildLotGrid`, `src/sim/agents/lotTraffic.ts#LotTraffic.ripen`, …
- `src/core/heap.ts#Heap.push` (method, line 14) — call by 5: `src/sim/agents/hybridAStar.ts#holonomic`, `src/sim/agents/hybridAStar.ts#WaySearch.push`, `src/sim/agents/lotNav.ts#buildLotGrid`, `src/sim/agents/lotTraffic.ts#LotTraffic.parking`, …
- `src/core/heap.ts#Heap.size` (getter, line 10) — call by 5: `src/sim/agents/hybridAStar.ts#holonomic`, `src/sim/agents/hybridAStar.ts#WaySearch.run`, `src/sim/agents/lotNav.ts#buildLotGrid`, `src/sim/agents/lotTraffic.ts#LotTraffic.ripen`, …

### src/core/hitch.ts

- `src/core/hitch.ts#measureHitch` (function, line 2) — call by 2: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.wornItem`, `src/render/people/proceduralBake.ts#hairCards`

### src/core/idSlots.ts

- `src/core/idSlots.ts#grown` (function, line 127) — call by 9: `src/sim/agents/buildingOrder.ts#StoreOrder.constructor`, `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.append`, `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.file`, `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.grow`, …
- `src/core/idSlots.ts#IdSlots` (class, line 15) — construct by 6: `src/sim/agents/buildingOrder.ts#StoreOrder.slots`, `src/sim/agents/kerbBays.ts#EntranceBook.slots`, `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.slots`, `src/sim/meso/demand.ts#DemandCells.byBuilding`, …
- `src/core/idSlots.ts#IdSlots.clear` (method, line 97) — call by 6: `src/sim/agents/buildingOrder.ts#StoreOrder.reread`, `src/sim/agents/kerbBays.ts#EntranceBook.clear`, `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.load`, `src/sim/meso/demand.ts#DemandCells.sync`, …
- `src/core/idSlots.ts#IdSlots.delete` (method, line 75) — call by 6: `src/sim/agents/buildingOrder.ts#StoreOrder.apply`, `src/sim/agents/kerbBays.ts#EntranceBook.put`, `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.remove`, `src/sim/meso/demand.ts#DemandCells.unfile`, …
- `src/core/idSlots.ts#IdSlots.get` (method, line 48) — call by 17: `src/sim/agents/buildingOrder.ts#StoreOrder.has`, `src/sim/agents/buildingOrder.ts#StoreOrder.rank`, `src/sim/agents/kerbBays.ts#EntranceBook.box`, `src/sim/agents/kerbBays.ts#EntranceBook.each`, …
- `src/core/idSlots.ts#IdSlots.onGrow` (field, line 26) — callbackAssign by 4: `src/sim/agents/buildingOrder.ts#StoreOrder.constructor`, `src/sim/meso/demand.ts#DemandCells.constructor`, `src/sim/ambient/ambientIndex.ts#BuildingMix.constructor`, `src/sim/ambient/ambientIndex.ts#SightWalls.constructor`
- `src/core/idSlots.ts#IdSlots.onGrow` (field, line 26) — implementation by 4: `src/sim/agents/buildingOrder.ts#StoreOrder.constructor`, `src/sim/meso/demand.ts#DemandCells.constructor`, `src/sim/ambient/ambientIndex.ts#BuildingMix.constructor`, `src/sim/ambient/ambientIndex.ts#SightWalls.constructor`
- `src/core/idSlots.ts#IdSlots.onGrow` (field, line 26) — write by 4: `src/sim/agents/buildingOrder.ts#StoreOrder.constructor`, `src/sim/meso/demand.ts#DemandCells.constructor`, `src/sim/ambient/ambientIndex.ts#BuildingMix.constructor`, `src/sim/ambient/ambientIndex.ts#SightWalls.constructor`
- `src/core/idSlots.ts#IdSlots.size` (getter, line 38) — call by 1: `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.size`
- `src/core/idSlots.ts#IdSlots.take` (method, line 58) — call by 6: `src/sim/agents/buildingOrder.ts#StoreOrder.place`, `src/sim/agents/kerbBays.ts#EntranceBook.put`, `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.place`, `src/sim/meso/demand.ts#DemandCells.file`, …

### src/core/intersect.ts

- `src/core/intersect.ts#segSeg` (function, line 21) — call by 1: `src/world/conflictTable.ts#firstCrossing`
- `src/core/intersect.ts#segSeg` (function, line 21) — read by 1: `src/world/conflictTable.ts#firstCrossing`

### src/core/offset.ts

- `src/core/offset.ts#offsetPolyline` (function, line 46) — call by 1: `src/world/lanelets.ts#LaneletGraph.buildSegmentLinks`

### src/core/polygon.ts

- `src/core/polygon.ts#pointInPolygon` (function, line 40) — call by 3: `src/sim/agents/lorryDocks.ts#Yard.bodyInLot`, `src/sim/agents/lotBays.ts#clearOfWalls`, `src/world/buildings/streetDoors.ts#streetDoors`

### src/core/polyline.ts

- `src/core/polyline.ts#Frame` (interface, line 7) — read by 32: `src/render/densityCars.ts#createDensityCars.writeLine`, `src/sim/agents/lorryDocks.ts#Yard.constructor`, `src/sim/agents/lorryTraffic.ts#LorryTraffic.call`, `src/sim/agents/lotBays.ts#gateLane`, …
- `src/core/polyline.ts#FrameOut` (interface, line 23) — read by 8: `src/sim/agents/walk.ts#frameInto`, `src/sim/vehicleSignals.ts#indicatorSide`, `src/sim/vehicleSignals.ts#pathCurvature`, `src/sim/vehicles/laneChange.ts#pullOutRoom`, …
- `src/core/polyline.ts#FrameOut` (interface, line 23) — typeRef by 4: `src/world/turnPaths.ts#approachPosesOf.frameAt`, `src/world/turnPaths.ts#poseAt`, `src/world/turnPaths.ts#poseInto`, `src/world/turnPaths.ts#sweep.frameAt`
- `src/core/polyline.ts#newFrame` (function, line 33) — call by 8: `src/sim/agents/walk.ts#WAY_SAMPLE`, `src/sim/vehicleSignals.ts#AFTER`, `src/sim/vehicleSignals.ts#BEFORE`, `src/sim/vehicleSignals.ts#HERE`, …
- `src/core/polyline.ts#Polyline` (class, line 36) — reference by 8: `src/sim/agents/lotBays.ts#baysOfShare`, `src/world/conflictTable.ts#sampleSweep.lineOf`, `src/world/lanelets.ts#LaneletGraph.buildSegmentLinks`, `src/world/lanelets.ts#LaneletGraph.junctionsOffPage`, …
- `src/core/polyline.ts#Polyline` (class, line 36) — typeRef by 37: `src/sim/intersections/crossingSpans.ts#CrossingSpans.measureJunction`, `src/sim/intersections/crossingSpans.ts#CrossingSpans.measureJunction.samplesOf`, `src/sim/intersections/crossingSpans.ts#measure`, `src/sim/intersections/crossingSpans.ts#sampleZebra`, …
- `src/core/polyline.ts#Polyline.bbox` (getter, line 116) — call by 12: `src/sim/agents/kerbBays.ts#fileStreet`, `src/sim/agents/laneGrid.ts#boxOf`, `src/sim/agents/laneGrid.ts#LaneGrid.file`, `src/sim/agents/laneGrid.ts#LaneGrid.inBox`, …
- `src/core/polyline.ts#Polyline.closestInto` (method, line 347) — call by 3: `src/sim/agents/walk.ts#projectInto`, `src/sim/vehicleSignals.ts#indicatorSide`, `src/world/parkingLayout.ts#frontagesOn`
- `src/core/polyline.ts#Polyline.closestPoint` (method, line 330) — call by 12: `src/sim/agents/lorryDocks.ts#Yard.constructor`, `src/sim/agents/lorryTraffic.ts#closures`, `src/sim/agents/lotBays.ts#exitsOf`, `src/sim/agents/lotBays.ts#gateLane`, …
- `src/core/polyline.ts#Polyline.closestPoint` (method, line 330) — read by 12: `src/sim/agents/lorryDocks.ts#Yard.constructor`, `src/sim/agents/lorryTraffic.ts#closures`, `src/sim/agents/lotBays.ts#exitsOf`, `src/sim/agents/lotBays.ts#gateLane`, …
- `src/core/polyline.ts#Polyline.cum` (getter, line 81) — call by 2: `src/sim/agents/walk.ts#bendRadii`, `src/sim/agents/walk.ts#createAgentWalkEngine`
- `src/core/polyline.ts#Polyline.distanceTo` (method, line 359) — call by 2: `src/sim/vehicles/laneChange.ts#pullOutRoom`, `src/sim/vehicles/laneChange.ts#sideways`
- `src/core/polyline.ts#Polyline.fromFlat` (method, line 65) — call by 3: `src/sim/agents/lotBays.ts#baysOfShare`, `src/world/conflictTable.ts#sampleSweep.lineOf`, `src/world/lanelets.ts#LaneletGraph.junctionsOffPage`
- `src/core/polyline.ts#Polyline.fromPoints` (method, line 55) — call by 5: `src/world/lanelets.ts#LaneletGraph.buildSegmentLinks`, `src/world/turnPaths.ts#bezierTurn`, `src/world/turnPaths.ts#bulbTurnPath`, `src/world/turnPaths.ts#medianUturnPath`, …
- `src/core/polyline.ts#Polyline.length` (getter, line 123) — call by 26: `src/render/densityCars.ts#createDensityCars.writeLine`, `src/sim/agents/walk.ts#createAgentWalkEngine`, `src/sim/agents/walk.ts#gapOpen`, `src/sim/agents/walk.ts#route`, …
- `src/core/polyline.ts#Polyline.n` (field, line 40) — read by 7: `src/sim/agents/walk.ts#bendRadii`, `src/sim/agents/walk.ts#createAgentWalkEngine`, `src/sim/agents/walk.ts#frameInto`, `src/sim/routing/lowerBound.ts#goalEnd`, …
- `src/core/polyline.ts#Polyline.point` (method, line 69) — call by 4: `src/sim/routing/lowerBound.ts#goalEnd`, `src/sim/routing/lowerBound.ts#laneLowerBound`, `src/sim/routing/lowerBound.ts#tripLowerBound`, `src/sim/crossings/permission.ts#vehicleBodyIntersectsCrossing`
- `src/core/polyline.ts#Polyline.sampleAt` (method, line 152) — call by 32: `src/render/densityCars.ts#createDensityCars.writeLine`, `src/sim/agents/lorryDocks.ts#Yard.constructor`, `src/sim/agents/lorryTraffic.ts#LorryTraffic.call`, `src/sim/agents/lotBays.ts#gateLane`, …
- `src/core/polyline.ts#Polyline.sampleAtInto` (method, line 157) — call by 7: `src/sim/agents/walk.ts#frameInto`, `src/sim/vehicleSignals.ts#indicatorSide`, `src/sim/vehicleSignals.ts#pathCurvature`, `src/sim/vehicles/laneChange.ts#pullOutRoom`, …
- `src/core/polyline.ts#Polyline.sub` (method, line 198) — call by 1: `src/world/lanelets.ts#LaneletGraph.buildSegmentLinks`
- `src/core/polyline.ts#Polyline.tan` (getter, line 96) — call by 1: `src/sim/agents/walk.ts#frameInto`
- `src/core/polyline.ts#Polyline.toPoints` (method, line 74) — call by 3: `src/world/conflictPoints.ts#ConflictIndex.buildJunction`, `src/world/lanelets.ts#LaneletGraph.buildSegmentLinks`, `src/sim/ambient/ambientIndex.ts#StreetLength.add`
- `src/core/polyline.ts#Polyline.xy` (field, line 38) — read by 16: `src/sim/agents/laneGrid.ts#digestOf`, `src/sim/agents/lorryDocks.ts#yardInputs`, `src/sim/agents/lotBays.ts#shareOf`, `src/sim/agents/walk.ts#bendRadii`, …

### src/core/ring.ts

- `src/core/ring.ts#Ring` (class, line 27) — typeRef by 1: `src/world/surfaceParts.ts#surfaceParts`
- `src/core/ring.ts#Ring.flatten` (method, line 89) — call by 1: `src/world/turnPaths.ts#JunctionSurface.constructor`
- `src/core/ring.ts#Ring.flatXY` (method, line 98) — call by 1: `src/world/junctionBuild.ts#junctionAsk`
- `src/core/ring.ts#Ring.isEmpty` (getter, line 84) — call by 2: `src/world/surfaceParts.ts#surfaceParts`, `src/world/surfaceParts.ts#surfaceParts.road`

### src/core/rng.ts

- `src/core/rng.ts#Rng` (class, line 6) — construct by 1: `src/sim/ambient/ambient.ts#AmbientWorld.release`
- `src/core/rng.ts#Rng.float` (method, line 57) — call by 14: `src/sim/agents/lorryTraffic.ts#LorryTraffic.call`, `src/sim/agents/lorryTraffic.ts#LorryTraffic.drive`, `src/sim/agents/lorryTraffic.ts#LorryTraffic.sendNow`, `src/sim/agents/lotTraffic.ts#LotTraffic.call`, …
- `src/core/rng.ts#Rng.int` (method, line 67) — call by 2: `src/sim/meso/meso.ts#Meso.admit`, `src/sim/meso/meso.ts#Meso.handoffDestination`
- `src/core/rng.ts#Rng.range` (method, line 62) — call by 4: `src/sim/agents/lorryTraffic.ts#LorryTraffic.join`, `src/sim/agents/lotTraffic.ts#LotTraffic.leaving`, `src/sim/vehicles/spawn.ts#spawnAt`, `src/sim/vehicles/spawn.ts#spawnVehicleAt`
- `src/core/rng.ts#Rng.weighted` (method, line 87) — call by 2: `src/sim/meso/meso.ts#Meso.admit`, `src/sim/vehicles/spawn.ts#spawnAt`

### src/core/scalar.ts

- `src/core/scalar.ts#clamp` (function, line 58) — call by 7: `src/sim/signals/plan.ts#buildStages`, `src/sim/signals/plan.ts#withAmbers`, `src/sim/vehicles/driver.ts#makeDriver`, `src/sim/vehicles/idm.ts#smooth`, …
- `src/core/scalar.ts#COARSE_EPS` (variable, line 12) — read by 2: `src/sim/intersections/admission.ts#compactReservationChain`, `src/sim/vehicles/integrate.ts#CLEARANCE_EPSILON`
- `src/core/scalar.ts#EPS` (variable, line 6) — read by 1: `src/sim/signals/fsm.ts#betterCandidate`
- `src/core/scalar.ts#GEO_EPS` (variable, line 9) — read by 3: `src/world/turnPaths.ts#bezierTurn`, `src/world/turnPaths.ts#medianUturnPath`, `src/world/turnPaths.ts#twoPieceTurn`
- `src/core/scalar.ts#MIN_RIBBON` (variable, line 45) — read by 1: `src/world/lanelets.ts#LaneletGraph.buildSegmentLinks`

### src/core/units.ts

- `src/core/units.ts#kmh` (function, line 19) — call by 3: `src/sim/meso/model.ts#WAVE_SPEED`, `src/sim/routing/lowerBound.ts#FASTEST_LIMIT`, `src/sim/params.ts#LANE_CHANGE_FAST_SPEED`
- `src/core/units.ts#m` (function, line 6) — call by 295: `src/render/carBody.ts#M`, `src/render/densityCars.ts#carGeometry`, `src/render/densityCars.ts#FAR_LANE`, `src/render/densityCars.ts#TILE_RADIUS`, …
- `src/core/units.ts#m` (function, line 6) — read by 1: `src/world/turnPaths.ts#WAYPOINTS`
- `src/core/units.ts#METERS_PER_UNIT` (variable, line 2) — read by 2: `src/sim/agents/trafficBuildings.ts#trafficMapOf`, `src/sim/signals/plan.ts#withAmbers`
- `src/core/units.ts#perM` (function, line 9) — call by 4: `src/render/agents.ts#FAR_BODY_ZOOM`, `src/render/agents.ts#NEAR_DETAIL_ZOOM`, `src/render/agents.ts#OCCUPANT_ZOOM`, `src/render/riggedCitizens.ts#citizenBandAt`
- `src/core/units.ts#stepsIn` (function, line 15) — call by 6: `src/sim/agents/lotBays.ts#clearOfWalls`, `src/sim/vehicles/spawn.ts#trafficTarget`, `src/world/parkingLayout.ts#parkingOf`, `src/world/turnPaths.ts#bulbTurnPath`, …
- `src/core/units.ts#TUNED_UNIT` (variable, line 12) — read by 2: `src/render/exhaust.ts#createExhaust`, `src/render/exhaust.ts#createExhaust.emit`
- `src/core/units.ts#UNITS_PER_METER` (variable, line 3) — read by 3: `src/sim/meso/memory.ts#MICRO_MIN_RADIUS`, `src/sim/meso/memory.ts#MICRO_RADIUS`, `src/ui/roads/junctionPanel.ts#mountJunctionPanel`

### src/core/vec2.ts

- `src/core/vec2.ts#addScaled` (function, line 43) — call by 5: `src/sim/vehicles/integrate.ts#applyLaneChange`, `src/world/turnPaths.ts#bezierTurn`, `src/world/turnPaths.ts#bulbTurnPath.cubic`, `src/world/turnPaths.ts#medianUturnPath`, …
- `src/core/vec2.ts#dot` (function, line 14) — call by 2: `src/sim/vehicles/integrate.ts#applyLaneChange`, `src/world/turnPaths.ts#bezierTurn`
- `src/core/vec2.ts#len` (function, line 19) — call by 3: `src/world/turnPaths.ts#bezierTurn`, `src/world/turnPaths.ts#twoPieceTurn`, `src/world/turnPaths.ts#twoPieceTurn.cubic`
- `src/core/vec2.ts#normalize` (function, line 25) — call by 2: `src/world/turnPaths.ts#bezierTurn`, `src/world/turnPaths.ts#twoPieceTurn`
- `src/core/vec2.ts#perp` (function, line 23) — call by 1: `src/sim/vehicles/integrate.ts#applyLaneChange`
- `src/core/vec2.ts#sub` (function, line 10) — call by 4: `src/sim/vehicles/integrate.ts#applyLaneChange`, `src/world/turnPaths.ts#bezierTurn`, `src/world/turnPaths.ts#twoPieceTurn`, `src/world/turnPaths.ts#twoPieceTurn.cubic`
- `src/core/vec2.ts#Vec2` (interface, line 4) — read by 84: `src/render/densityCars.ts#createDensityCars.writeLine`, `src/sim/agents/kerbBays.ts#EntranceBook.put`, `src/sim/agents/kerbBays.ts#EntranceBook.same`, `src/sim/agents/kerbBays.ts#entranceBox`, …
- `src/core/vec2.ts#Vec2` (interface, line 4) — typeRef by 80: `src/sim/agents/kerbBays.ts#Entrance`, `src/sim/agents/lotDoors.ts#DoorWay`, `src/sim/agents/lotDoors.ts#hitRings`, `src/sim/agents/trafficBuildings.ts#TrafficBuilding`, …

### src/jobs/jobPool.ts

- `src/jobs/jobPool.ts#jobPort` (function, line 669) — call by 2: `src/render/agents.ts#<ExpressionStatement>`, `src/render/agents.ts#<ExpressionStatement>@75`

### src/jobs/jobScope.ts

- `src/jobs/jobScope.ts#JobService` (type, line 8) — typeRef by 1: `src/render/pagePorts.ts#provideLanes`

### src/main.ts

- `src/main.ts#showInspector` (function, line 3573) — callbackAssign by 6: `src/ui/inspector.ts#junctionHost.setMovementBlocked`, `src/ui/inspector.ts#junctionHost.setSignal`, `src/ui/inspector.ts#junctionHost.setControl`, `src/ui/inspector.ts#junctionHost.setHeight`, …
- `src/main.ts#showInspector` (function, line 3573) — implementation by 6: `src/ui/inspector.ts#junctionHost.setMovementBlocked`, `src/ui/inspector.ts#junctionHost.setSignal`, `src/ui/inspector.ts#junctionHost.setControl`, `src/ui/inspector.ts#junctionHost.setHeight`, …

### src/people/body/assets.ts

- `src/people/body/assets.ts#PeopleAssets` (interface, line 15) — read by 6: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.strandPiece`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.buildClass`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.classRigFor`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.woundSpots`, …

### src/people/body/macro.ts

- `src/people/body/macro.ts#ageFromYears` (function, line 60) — call by 1: `src/render/agents.ts#createAgentMeshes.procBuildNext`
- `src/people/body/macro.ts#MacroParams` (interface, line 4) — read by 2: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.add`, `src/render/people/proceduralCrowd.ts#proceduralLook`
- `src/people/body/macro.ts#MacroParams` (interface, line 4) — typeRef by 2: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.classRigFor`, `src/people/body/morph.ts#Morpher.coefficients`
- `src/people/body/macro.ts#macroTargetWeights` (function, line 79) — call by 1: `src/people/body/morph.ts#Morpher.coefficients`
- `src/people/body/macro.ts#yearsFromAge` (function, line 69) — call by 2: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.add`, `src/render/people/proceduralCrowd.ts#proceduralLook`

### src/people/body/morph.ts

- `src/people/body/morph.ts#MacroPcaMeta` (interface, line 14) — read by 1: `src/people/body/morph.ts#Morpher.coefficients`
- `src/people/body/morph.ts#Morpher.addRegional` (method, line 171) — call by 1: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.setFace`
- `src/people/body/morph.ts#Morpher.base` (field, line 61) — read by 1: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.strandPiece`
- `src/people/body/morph.ts#Morpher.coeff` (field, line 63) — read by 1: `src/people/body/morph.ts#Morpher.coefficients`
- `src/people/body/morph.ts#Morpher.constructor` (constructor, line 74) — read by 1: `src/people/body/morph.ts#Morpher.coefficients`
- `src/people/body/morph.ts#Morpher.height` (method, line 191) — call by 1: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.add`
- `src/people/body/morph.ts#Morpher.macroNames` (field, line 68) — read by 1: `src/people/body/morph.ts#Morpher.coefficients`
- `src/people/body/morph.ts#Morpher.macroRow` (field, line 69) — read by 1: `src/people/body/morph.ts#Morpher.coefficients`
- `src/people/body/morph.ts#Morpher.vertexCount` (field, line 59) — read by 1: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.setFace`
- `src/people/body/morph.ts#PeoplePacks` (interface, line 42) — read by 1: `src/people/body/morph.ts#Morpher.coefficients`

### src/people/body/proxy.ts

- `src/people/body/proxy.ts#loadProxyItem` (function, line 159) — call by 1: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.item`
- `src/people/body/proxy.ts#ProxyItem` (interface, line 128) — read by 10: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.buildClass`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.classRigFor`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.add`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.coverRow`, …
- `src/people/body/proxy.ts#ProxyItem` (interface, line 128) — typeRef by 12: `src/render/people/proceduralCrowd.ts#createProceduralCrowd`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.strandPiece`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.classRigFor`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.add`, …
- `src/people/body/proxy.ts#ProxyPack` (interface, line 6) — read by 10: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.strandPiece`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.buildClass`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.classRigFor`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.add`, …
- `src/people/body/proxy.ts#ProxyPack` (interface, line 6) — typeRef by 4: `src/people/hair/procedural.ts#generateHairStrands`, `src/render/bakePool.ts#wearInWorker`, `src/render/people/proceduralBake.ts#hairCards`, `src/render/people/proceduralBake.ts#packFromRecord`

### src/people/hair/procedural.ts

- `src/people/hair/procedural.ts#FEMALE_HAIR` (variable, line 74) — read by 1: `src/render/people/proceduralCrowd.ts#proceduralLook`
- `src/people/hair/procedural.ts#generateHair` (function, line 246) — call by 2: `src/people/hair/procedural.ts#generateHairStrands`, `src/render/people/proceduralBake.ts#hairCards`
- `src/people/hair/procedural.ts#generateHeadband` (function, line 615) — call by 1: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.item`
- `src/people/hair/procedural.ts#HAIR_STYLES` (variable, line 42) — read by 4: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.strandPiece`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.item`, `src/render/people/proceduralCrowd.ts#proceduralLook`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.textureOf`
- `src/people/hair/procedural.ts#HairBase` (interface, line 79) — typeRef by 1: `src/people/hair/procedural.ts#generateHairStrands`
- `src/people/hair/procedural.ts#HairStyle` (interface, line 6) — read by 3: `src/render/people/proceduralCrowd.ts#proceduralLook`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.textureOf`, `src/render/people/proceduralBake.ts#hairCards`
- `src/people/hair/procedural.ts#HairStyle` (interface, line 6) — typeRef by 2: `src/people/hair/procedural.ts#generateHairStrands`, `src/render/people/proceduralBake.ts#hairCards`
- `src/people/hair/procedural.ts#MALE_HAIR` (variable, line 77) — read by 1: `src/render/people/proceduralCrowd.ts#proceduralLook`

### src/people/spec.ts

- `src/people/spec.ts#PersonLook` (interface, line 22) — read by 4: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.add`, `src/render/people/proceduralCrowd.ts#proceduralLook`, `src/render/people/proceduralCrowd.ts#roleOf`, `src/render/people/proceduralCrowd.ts#proceduralLook.mouth`
- `src/people/spec.ts#PersonLook` (interface, line 22) — typeRef by 3: `src/render/people/proceduralCrowd.ts#proceduralLook`, `src/render/people/proceduralCrowd.ts#roleOf`, `src/render/people/proceduralCrowd.ts#proceduralLook.mouth`
- `src/people/spec.ts#PersonSpec` (interface, line 7) — read by 6: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.update`, `src/render/riggedCitizens.ts#createRiggedCitizens.draw`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.add`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.bareSkin`, …
- `src/people/spec.ts#PersonSpec` (interface, line 7) — typeRef by 4: `src/render/people/proceduralCrowd.ts#ProceduralCrowd`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.bareSkin`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.skinItem`, `src/render/people/proceduralCrowd.ts#proceduralLook`
- `src/people/spec.ts#randomPerson` (function, line 129) — call by 2: `src/render/agents.ts#createAgentMeshes`, `src/render/agents.ts#createAgentMeshes.procBuildNext`
- `src/people/spec.ts#wornItems` (function, line 88) — call by 1: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.add`

### src/render/bakePool.ts

- `src/render/bakePool.ts#clipsOf` (function, line 137) — call by 1: `src/render/bakePool.ts#bakeDeferredInWorker`
- `src/render/bakePool.ts#releaseIdleBakeWorkers` (function, line 198) — call by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.dispose`
- `src/render/bakePool.ts#run` (function, line 127) — call by 5: `src/render/bakePool.ts#bakeDeferredInWorker`, `src/render/bakePool.ts#classRigInWorker`, `src/render/bakePool.ts#wearInWorker`, `src/render/bakePool.ts#personFaceInWorker`, …

### src/render/buildings/cellPool.ts

- `src/render/buildings/cellPool.ts#FloorColumns.constructor` (constructor, line 120) — callbackAssign by 4: `src/sim/agents/buildingOrder.ts#StoreOrder.constructor`, `src/sim/meso/demand.ts#DemandCells.constructor`, `src/sim/ambient/ambientIndex.ts#BuildingMix.constructor`, `src/sim/ambient/ambientIndex.ts#SightWalls.constructor`

### src/render/buildings/townColumns.ts

- `src/render/buildings/townColumns.ts#TownColumns.constructor` (constructor, line 85) — callbackAssign by 4: `src/sim/agents/buildingOrder.ts#StoreOrder.constructor`, `src/sim/meso/demand.ts#DemandCells.constructor`, `src/sim/ambient/ambientIndex.ts#BuildingMix.constructor`, `src/sim/ambient/ambientIndex.ts#SightWalls.constructor`

### src/render/citizenBake.ts

- `src/render/citizenBake.ts#breathe` (function, line 87) — call by 5: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.buildClass`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.buildFar`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.add`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.wornItem`, …
- `src/render/citizenBake.ts#CARRY_AT` (variable, line 47) — read by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.draw`
- `src/render/citizenBake.ts#ClipFrames` (interface, line 55) — read by 12: `src/render/people/proceduralCrowd.ts#createProceduralCrowd`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.update`, `src/render/riggedCitizens.ts#createRiggedCitizens.draw`, `src/render/riggedCitizens.ts#createRiggedCitizens.want`, …
- `src/render/citizenBake.ts#ClipFrames` (interface, line 55) — typeRef by 10: `src/render/riggedCitizens.ts#createRiggedCitizens.emit`, `src/render/bakePool.ts#bakeDeferredInWorker`, `src/render/riggedCitizens.ts#createRiggedCitizens.addToAtlas`, `src/render/citizenPlays.ts#appendClip`, …
- `src/render/citizenBake.ts#ClipFrames` (interface, line 55) — write by 1: `src/render/people/proceduralCook.ts#clipOf`
- `src/render/citizenBake.ts#Deferred` (type, line 347) — call by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.want`
- `src/render/citizenBake.ts#deferredClips` (function, line 367) — callbackAssign by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.want`
- `src/render/citizenBake.ts#deferredClips` (function, line 367) — implementation by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.want`
- `src/render/citizenBake.ts#deferredClips.carried` (function, line 381) — callbackAssign by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.want`
- `src/render/citizenBake.ts#deferredClips.carried` (function, line 381) — implementation by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.want`
- `src/render/citizenBake.ts#GAIT_AT` (variable, line 34) — read by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.draw`
- `src/render/citizenBake.ts#gaitClips` (function, line 447) — call by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.want`
- `src/render/citizenBake.ts#LIBRARY_AT` (variable, line 28) — read by 2: `src/render/riggedCitizens.ts#createRiggedCitizens.drawClip`, `src/render/riggedCitizens.ts#createRiggedCitizens.clipPose`
- `src/render/citizenBake.ts#Played` (type, line 27) — typeRef by 3: `src/render/citizenBake.ts#CitizenClipKey`, `src/render/riggedCitizens.ts#createRiggedCitizens.drawClip`, `src/render/riggedCitizens.ts#createRiggedCitizens.clipPose`
- `src/render/citizenBake.ts#RIDER_AT` (variable, line 40) — read by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.drawClip`
- `src/render/citizenBake.ts#rigData` (function, line 475) — call by 1: `src/render/bakePool.ts#bakeRig`
- `src/render/citizenBake.ts#rigData` (function, line 475) — read by 1: `src/render/bakePool.ts#bakeRig`
- `src/render/citizenBake.ts#RigNode` (interface, line 455) — typeRef by 2: `src/render/bakePool.ts#bakeDeferredInWorker`, `src/render/bakePool.ts#bakeRig`
- `src/render/citizenBake.ts#ShuffleFacts` (interface, line 353) — read by 1: `src/render/bakePool.ts#bakeDeferredInWorker`
- `src/render/citizenBake.ts#ShuffleFacts` (interface, line 353) — typeRef by 1: `src/render/bakePool.ts#bakeDeferredInWorker`
- `src/render/citizenBake.ts#WALK` (variable, line 15) — read by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.drawClip`
- `src/render/citizenBake.ts#WALK_ELDER` (variable, line 17) — read by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.drawClip`
- `src/render/citizenBake.ts#WALK_SHUFFLE` (variable, line 19) — read by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.want`

### src/render/citizenBake.worker.ts

- `src/render/citizenBake.worker.ts#BakeReply` (interface, line 56) — read by 5: `src/render/bakePool.ts#bakeDeferredInWorker`, `src/render/bakePool.ts#classRigInWorker`, `src/render/bakePool.ts#wearInWorker`, `src/render/bakePool.ts#personFaceInWorker`, …
- `src/render/citizenBake.worker.ts#PersonFaceJob` (interface, line 44) — typeRef by 3: `src/render/people/proceduralCrowd.ts#createProceduralCrowd`, `src/render/bakePool.ts#personFaceInWorker`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.faceShare`

### src/render/citizenCasting.ts

- `src/render/citizenCasting.ts#AgeBand` (type, line 10) — typeRef by 1: `src/render/citizenCasting.ts#CastingRegistry.candidates.of`
- `src/render/citizenCasting.ts#CAST_NEAR` (variable, line 145) — read by 1: `src/render/citizenCasting.ts#CastingRegistry.pickCitizenModel`
- `src/render/citizenCasting.ts#CastingContext` (interface, line 121) — read by 4: `src/render/citizenCasting.ts#CastingRegistry.pickCitizenModel`, `src/render/citizenCasting.ts#CastingRegistry.candidates`, `src/render/citizenCasting.ts#CastingRegistry.note`, `src/render/citizenCasting.ts#CastingRegistry.candidates.of`
- `src/render/citizenCasting.ts#CastingContext` (interface, line 121) — typeRef by 6: `src/render/riggedCitizens.ts#createRiggedCitizens.draw`, `src/render/riggedCitizens.ts#createRiggedCitizens.bodyFor`, `src/render/citizenCasting.ts#CastingRegistry.pickCitizenModel`, `src/render/citizenCasting.ts#CastingRegistry.candidates`, …
- `src/render/citizenCasting.ts#CastingContext` (interface, line 121) — write by 2: `src/render/riggedCitizens.ts#createRiggedCitizens.draw`, `src/render/riggedCitizens.ts#createRiggedCitizens.contextOf`
- `src/render/citizenCasting.ts#CastingRegistry.cast` (field, line 158) — read by 4: `src/render/citizenCasting.ts#CastingRegistry.pickCitizenModel`, `src/render/citizenCasting.ts#CastingRegistry.dressesMembers`, `src/render/citizenCasting.ts#CastingRegistry.drop`, `src/render/citizenCasting.ts#CastingRegistry.recode`
- `src/render/citizenCasting.ts#CastingRegistry.cast` (field, line 158) — write by 2: `src/render/citizenCasting.ts#CastingRegistry.pickCitizenModel`, `src/render/citizenCasting.ts#CastingRegistry.drop`
- `src/render/citizenCasting.ts#CitizenModel` (interface, line 13) — read by 6: `src/render/riggedCitizens.ts#createRiggedCitizens.draw`, `src/render/citizenCasting.ts#CastingRegistry.pickCitizenModel`, `src/render/citizenCasting.ts#CastingRegistry.dressesMembers`, `src/render/citizenCasting.ts#CastingRegistry.candidates.of`, …
- `src/render/citizenCasting.ts#CitizenModel` (interface, line 13) — typeRef by 1: `src/render/citizenCasting.ts#CastingRegistry.constructor`
- `src/render/citizenCasting.ts#CODES` (variable, line 71) — read by 2: `src/render/citizenCasting.ts#codesFor`, `src/render/citizenCasting.ts#codesFor.withTradition`
- `src/render/citizenCasting.ts#Company` (type, line 83) — typeRef by 8: `src/render/agents.ts#createAgentMeshes.vehicleCompany`, `src/render/riggedCitizens.ts#companyOf`, `src/render/citizenCasting.ts#codesFor`, `src/render/citizenCasting.ts#CastingRegistry.drawn`, …
- `src/render/citizenCasting.ts#CROWD` (variable, line 29) — read by 3: `src/render/riggedCitizens.ts#createRiggedCitizens.draw`, `src/render/citizenCasting.ts#CastingRegistry.constructor`, `src/render/riggedCitizens.ts#createRiggedCitizens.drawClip`
- `src/render/citizenCasting.ts#CROWD_IDS` (variable, line 59) — read by 2: `src/render/agents.ts#createAgentMeshes`, `src/render/riggedCitizens.ts#createRiggedCitizens.request`
- `src/render/citizenCasting.ts#DressCode` (type, line 69) — typeRef by 11: `src/render/citizenCasting.ts#CastingRegistry.pickCitizenModel`, `src/render/citizenCasting.ts#Casting`, `src/render/citizenCasting.ts#CastingRegistry.candidates`, `src/render/citizenCasting.ts#CastingRegistry.codes`, …
- `src/render/citizenCasting.ts#Entry` (interface, line 154) — read by 4: `src/render/citizenCasting.ts#CastingRegistry.pickCitizenModel`, `src/render/citizenCasting.ts#CastingRegistry.dressesMembers`, `src/render/citizenCasting.ts#CastingRegistry.drop`, `src/render/citizenCasting.ts#CastingRegistry.note`
- `src/render/citizenCasting.ts#Entry` (interface, line 154) — typeRef by 2: `src/render/citizenCasting.ts#CastingRegistry.pickCitizenModel`, `src/render/citizenCasting.ts#CastingRegistry.note`
- `src/render/citizenCasting.ts#Entry` (interface, line 154) — write by 2: `src/render/citizenCasting.ts#CastingRegistry.pickCitizenModel`, `src/render/citizenCasting.ts#CastingRegistry.recode`
- `src/render/citizenCasting.ts#Wardrobe` (type, line 9) — typeRef by 1: `src/render/citizenCasting.ts#CastingRegistry.census`

### src/render/citizenGait.ts

- `src/render/citizenGait.ts#ACCEL_TIME` (variable, line 302) — read by 1: `src/render/citizenGait.ts#stepGait`
- `src/render/citizenGait.ts#ACTIVITY` (variable, line 387) — read by 1: `src/render/citizenGait.ts#standingKey`
- `src/render/citizenGait.ts#ADULT_WALKS` (variable, line 201) — read by 1: `src/render/citizenGait.ts#pace`
- `src/render/citizenGait.ts#CARRY_WALKS` (variable, line 209) — read by 1: `src/render/citizenGait.ts#pace`
- `src/render/citizenGait.ts#CLIP_FLOOR` (variable, line 290) — read by 1: `src/render/citizenGait.ts#moving`
- `src/render/citizenGait.ts#Cycle` (type, line 28) — typeRef by 1: `src/render/citizenGait.ts#walksOf`
- `src/render/citizenGait.ts#DRUNK_WALKS` (variable, line 204) — read by 1: `src/render/citizenGait.ts#pace`
- `src/render/citizenGait.ts#ELDER_WALKS` (variable, line 202) — read by 1: `src/render/citizenGait.ts#pace`
- `src/render/citizenGait.ts#FADE` (variable, line 298) — read by 4: `src/render/citizenGait.ts#createGait`, `src/render/citizenGait.ts#stepGait`, `src/render/citizenGait.ts#play`, `src/render/citizenGait.ts#standing`
- `src/render/citizenGait.ts#FEET_LEAD_RATE` (variable, line 273) — read by 1: `src/render/citizenGait.ts#standing`
- `src/render/citizenGait.ts#Gait` (interface, line 41) — read by 9: `src/render/citizenGait.ts#gaitHeading`, `src/render/citizenGait.ts#gaitPlays`, `src/render/citizenGait.ts#stepGait`, `src/render/citizenGait.ts#moving`, …
- `src/render/citizenGait.ts#Gait` (interface, line 41) — typeRef by 10: `src/render/citizenGait.ts#createGait`, `src/render/citizenGait.ts#gaitHeading`, `src/render/citizenGait.ts#gaitPlays`, `src/render/citizenGait.ts#stepGait`, …
- `src/render/citizenGait.ts#Gait` (interface, line 41) — write by 6: `src/render/citizenGait.ts#stepGait`, `src/render/citizenGait.ts#moving`, `src/render/citizenGait.ts#play`, `src/render/citizenGait.ts#standing`, …
- `src/render/citizenGait.ts#GAIT_FADE` (variable, line 296) — read by 1: `src/render/citizenGait.ts#moving`
- `src/render/citizenGait.ts#GaitClip` (interface, line 8) — read by 13: `src/render/citizenGait.ts#stepGait`, `src/render/citizenGait.ts#moving`, `src/render/citizenGait.ts#standing`, `src/render/citizenGait.ts#fractionAtYaw`, …
- `src/render/citizenGait.ts#GaitClip` (interface, line 8) — typeRef by 8: `src/render/citizenGait.ts#fractionAtYaw`, `src/render/citizenGait.ts#fractionAtTravel`, `src/render/citizenGait.ts#moving.share`, `src/render/citizenGait.ts#naturalPace`, …
- `src/render/citizenGait.ts#GaitClipName` (type, line 21) — typeRef by 1: `src/render/citizenGait.ts#put`
- `src/render/citizenGait.ts#GaitClips` (type, line 26) — typeRef by 8: `src/render/citizenGait.ts#gaitPlays`, `src/render/citizenGait.ts#stepGait`, `src/render/citizenGait.ts#moving`, `src/render/citizenGait.ts#standing`, …
- `src/render/citizenGait.ts#GaitPlay` (interface, line 88) — read by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.draw`
- `src/render/citizenGait.ts#GaitPlay` (interface, line 88) — typeRef by 3: `src/render/citizenGait.ts#gaitPlays`, `src/render/citizenGait.ts#add`, `src/render/citizenGait.ts#put`
- `src/render/citizenGait.ts#GaitPlay` (interface, line 88) — write by 1: `src/render/citizenGait.ts#put`
- `src/render/citizenGait.ts#HAND_L_WALKS` (variable, line 206) — read by 1: `src/render/citizenGait.ts#pace`
- `src/render/citizenGait.ts#HAND_R_WALKS` (variable, line 207) — read by 1: `src/render/citizenGait.ts#pace`
- `src/render/citizenGait.ts#JUMP` (variable, line 304) — read by 1: `src/render/citizenGait.ts#drawnVelocity`
- `src/render/citizenGait.ts#LAG_STEPS` (variable, line 275) — read by 1: `src/render/citizenGait.ts#stepGait`
- `src/render/citizenGait.ts#Play` (interface, line 32) — read by 6: `src/render/citizenGait.ts#stepGait`, `src/render/citizenGait.ts#moving`, `src/render/citizenGait.ts#play`, `src/render/citizenGait.ts#standing`, …
- `src/render/citizenGait.ts#Play` (interface, line 32) — typeRef by 1: `src/render/citizenGait.ts#add`
- `src/render/citizenGait.ts#Play` (interface, line 32) — write by 3: `src/render/citizenGait.ts#stepGait`, `src/render/citizenGait.ts#moving`, `src/render/citizenGait.ts#standing`
- `src/render/citizenGait.ts#PlayKey` (type, line 31) — typeRef by 1: `src/render/citizenGait.ts#play`
- `src/render/citizenGait.ts#RUN_TIME` (variable, line 299) — read by 1: `src/render/citizenGait.ts#moving`
- `src/render/citizenGait.ts#Single` (type, line 30) — typeRef by 2: `src/render/citizenGait.ts#standing`, `src/render/citizenGait.ts#standingKey`
- `src/render/citizenGait.ts#START_BELOW` (variable, line 288) — read by 1: `src/render/citizenGait.ts#moving`
- `src/render/citizenGait.ts#STEPPED_TURN_RATE` (variable, line 271) — read by 1: `src/render/citizenGait.ts#stepGait`
- `src/render/citizenGait.ts#STILL_BELOW` (variable, line 466) — read by 1: `src/render/citizenGait.ts#stepGait`
- `src/render/citizenGait.ts#STOP_BELOW` (variable, line 285) — read by 1: `src/render/citizenGait.ts#moving`
- `src/render/citizenGait.ts#STOP_DECEL` (variable, line 286) — read by 1: `src/render/citizenGait.ts#moving`
- `src/render/citizenGait.ts#STOP_SETTLE` (variable, line 292) — read by 2: `src/render/citizenGait.ts#stepGait`, `src/render/citizenGait.ts#standing`
- `src/render/citizenGait.ts#STRIDE_EXPONENT` (variable, line 294) — read by 2: `src/render/citizenGait.ts#moving.share`, `src/render/citizenGait.ts#pace`
- `src/render/citizenGait.ts#styleWalks` (variable, line 217) — read by 1: `src/render/citizenGait.ts#walksOf`
- `src/render/citizenGait.ts#styleWalks` (variable, line 217) — write by 1: `src/render/citizenGait.ts#walksOf`
- `src/render/citizenGait.ts#TURN_FADE` (variable, line 297) — read by 1: `src/render/citizenGait.ts#standing`
- `src/render/citizenGait.ts#TURN_FOOT` (variable, line 283) — read by 2: `src/render/citizenGait.ts#stepGait`, `src/render/citizenGait.ts#moving`
- `src/render/citizenGait.ts#TURN_IN_PLACE` (variable, line 277) — read by 1: `src/render/citizenGait.ts#stepGait`
- `src/render/citizenGait.ts#TURN_SETTLE` (variable, line 279) — read by 1: `src/render/citizenGait.ts#standing`
- `src/render/citizenGait.ts#TURN_STEPS` (variable, line 267) — read by 1: `src/render/citizenGait.ts#stepGait`
- `src/render/citizenGait.ts#TURN_STEPS_STANDING` (variable, line 269) — read by 1: `src/render/citizenGait.ts#stepGait`
- `src/render/citizenGait.ts#TURN_WINDUP` (variable, line 281) — read by 1: `src/render/citizenGait.ts#standing`
- `src/render/citizenGait.ts#VELOCITY_TIME` (variable, line 301) — read by 1: `src/render/citizenGait.ts#stepGait`
- `src/render/citizenGait.ts#WALK_STYLES` (variable, line 212) — read by 1: `src/render/citizenGait.ts#walkStyle`
- `src/render/citizenGait.ts#WalkStyle` (type, line 213) — typeRef by 3: `src/render/citizenGait.ts#pace`, `src/render/citizenGait.ts#walkStyle`, `src/render/citizenGait.ts#walksOf`

### src/render/citizenPalette.ts

- `src/render/citizenPalette.ts#PACKED_BONE_FLOATS` (variable, line 2) — read by 7: `src/render/people/proceduralCrowd.ts#createProceduralCrowd`, `src/render/citizenPlays.ts#appendClip`, `src/render/citizenPlays.ts#cpuPose`, `src/render/citizenPalette.ts#blendPackedFrames`, …
- `src/render/citizenPalette.ts#SKIN_BONE_FLOATS` (variable, line 3) — read by 17: `src/render/people/proceduralCrowd.ts#createProceduralCrowd`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.update`, `src/render/citizenPlays.ts#cpuPose`, `src/render/riggedCitizens.ts#createRiggedCitizens.maim`, …

### src/render/citizenPlays.ts

- `src/render/citizenPlays.ts#animationOf` (function, line 39) — call by 1: `src/render/citizenPlays.ts#appendClip`
- `src/render/citizenPlays.ts#ATLAS_TEXELS` (variable, line 24) — read by 1: `src/render/citizenPlays.ts#appendClip`
- `src/render/citizenPlays.ts#atlasTexture` (function, line 29) — call by 1: `src/render/citizenPlays.ts#appendClip`
- `src/render/citizenPlays.ts#CitizenAtlas` (interface, line 15) — read by 3: `src/render/riggedCitizens.ts#createRiggedCitizens.addToAtlas`, `src/render/citizenPlays.ts#appendClip`, `src/render/citizenPlays.ts#gpuPlays`
- `src/render/citizenPlays.ts#CitizenAtlas` (interface, line 15) — typeRef by 2: `src/render/citizenPlays.ts#appendClip`, `src/render/citizenPlays.ts#gpuPlays`
- `src/render/citizenPlays.ts#CitizenAtlas` (interface, line 15) — write by 1: `src/render/citizenPlays.ts#appendClip`
- `src/render/citizenPlays.ts#GPU_PLAYS` (variable, line 9) — read by 1: `src/render/citizenPlays.ts#gpuPlays`
- `src/render/citizenPlays.ts#Play` (interface, line 12) — typeRef by 1: `src/render/citizenPlays.ts#gpuPlays`
- `src/render/citizenPlays.ts#Play` (interface, line 12) — write by 1: `src/render/citizenPlays.ts#gpuPlays`

### src/render/citizenWalk.ts

- `src/render/citizenWalk.ts#WalkSex` (type, line 77) — typeRef by 7: `src/render/bakePool.ts#bakeDeferredInWorker`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.buildClass`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.classRigFor`, `src/render/bakePool.ts#classRigInWorker`, …

### src/render/depthConvention.ts

- `src/render/depthConvention.ts#pullTowardEye` (function, line 28) — call by 1: `src/render/blobShadows.ts#blobMesh`

### src/render/instanceBounds.ts

- `src/render/instanceBounds.ts#fitInstances` (function, line 5) — call by 3: `src/render/agents.ts#createAgentMeshes.sync`, `src/render/riggedCitizens.ts#createRiggedCitizens.finish`, `src/render/people/heldProps.ts#createHeldProps.put`

### src/render/lightLevels.ts

- `src/render/lightLevels.ts#luminanceOf` (function, line 28) — call by 1: `src/render/lightLevels.ts#nightGain`
- `src/render/lightLevels.ts#NIGHT_LUMINANCE` (variable, line 9) — read by 1: `src/render/agents.ts#LAMP_GAIN`

### src/render/mesh/weld.ts

- `src/render/mesh/weld.ts#weldGeometry` (function, line 5) — call by 3: `src/render/carBody.ts#merge`, `src/render/vehicleModels.ts#weldPart`, `src/render/mesh/simplify.ts#simplified`

### src/render/pagePorts.ts

- `src/render/pagePorts.ts#providers` (variable, line 6) — read by 1: `src/render/pagePorts.ts#provideLanes`
- `src/render/pagePorts.ts#providers` (variable, line 6) — write by 1: `src/render/pagePorts.ts#provideLanes`

### src/render/partLevelsWork.ts

- `src/render/partLevelsWork.ts#ArrayOf` (interface, line 30) — read by 1: `src/render/vehicleLevels.ts#attributeOf`
- `src/render/partLevelsWork.ts#ArrayOf` (interface, line 30) — typeRef by 2: `src/render/vehicleLevels.ts#attributeOf`, `src/render/vehicleLevels.ts#copyOf`
- `src/render/partLevelsWork.ts#LEVEL_ERRORS` (variable, line 20) — read by 3: `src/render/vehicleLevels.ts#levelFor`, `src/render/vehicleLevels.ts#LEVELS`, `src/render/agents.ts#standingRange`
- `src/render/partLevelsWork.ts#LevelArrays` (type, line 41) — read by 1: `src/render/vehicleLevels.ts#partLevels`
- `src/render/partLevelsWork.ts#PartArrays` (interface, line 33) — typeRef by 1: `src/render/vehicleLevels.ts#partLevels`
- `src/render/partLevelsWork.ts#partLevelArrays` (function, line 54) — call by 1: `src/render/vehicleLevels.ts#partLevels`
- `src/render/partLevelsWork.ts#PartLevels` (interface, line 47) — read by 1: `src/render/vehicleLevels.ts#partLevels`
- `src/render/partLevelsWork.ts#PartLevels` (interface, line 47) — typeRef by 1: `src/render/vehicleLevels.ts#partLevels`
- `src/render/partLevelsWork.ts#Weld` (type, line 27) — typeRef by 3: `src/render/vehicleLevels.ts#partLevels`, `src/render/agents.ts#createAgentMeshes.ladder`, `src/render/agents.ts#createAgentMeshes.levelsFor`

### src/render/people/bodyPixels.ts

- `src/render/people/bodyPixels.ts#bodyPixels` (function, line 4) — call by 1: `src/render/people/crowdBodies.ts#createClassBodies.bake`

### src/render/people/classRigMade.ts

- `src/render/people/classRigMade.ts#ClassRigMade` (interface, line 9) — read by 2: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.classRigFor`, `src/render/people/classRigMade.ts#rigFromMade`
- `src/render/people/classRigMade.ts#ClassRigMade` (interface, line 9) — typeRef by 2: `src/render/bakePool.ts#classRigInWorker`, `src/render/people/classRigMade.ts#rigFromMade`

### src/render/people/cookedPerson.ts

- `src/render/people/cookedPerson.ts#assemblePerson` (function, line 205) — call by 1: `src/render/people/classRigMade.ts#rigFromMade`

### src/render/people/cookPack.ts

- `src/render/people/cookPack.ts#KindName` (type, line 8) — typeRef by 1: `src/render/people/cookPack.ts#unpackRecord`
- `src/render/people/cookPack.ts#KINDS` (variable, line 7) — read by 1: `src/render/people/cookPack.ts#unpackRecord`
- `src/render/people/cookPack.ts#PackValue` (type, line 4) — typeRef by 5: `src/render/people/proceduralBake.ts#classFromRecord`, `src/render/people/proceduralCook.ts#loadProcedural`, `src/render/people/cookPack.ts#unpackRecord`, `src/render/people/proceduralCook.ts#clipOf`, …

### src/render/people/crowdAnimation.ts

- `src/render/people/crowdAnimation.ts#buildClassAnimation` (function, line 49) — call by 1: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.buildClass`
- `src/render/people/crowdAnimation.ts#buildClassAnimation` (function, line 49) — read by 2: `src/render/people/crowdAnimation.ts#buildClassAnimation.rowOf`, `src/render/people/crowdAnimation.ts#buildClassAnimation.refill`
- `src/render/people/crowdAnimation.ts#ClassAnimation` (interface, line 36) — call by 2: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.update`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.restore`
- `src/render/people/crowdAnimation.ts#ClassAnimation` (interface, line 36) — read by 2: `src/render/riggedCitizens.ts#createRiggedCitizens.addToAtlas`, `src/render/citizenPlays.ts#appendClip`
- `src/render/people/crowdAnimation.ts#createPalettePass` (function, line 117) — call by 2: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.buildClass`, `src/render/people/crowdAnimation.ts#createPalettePass.render`
- `src/render/people/crowdAnimation.ts#createPalettePass` (function, line 117) — read by 6: `src/render/people/crowdAnimation.ts#createPalettePass.setAtlas`, `src/render/people/crowdAnimation.ts#createPalettePass.grow`, `src/render/people/crowdAnimation.ts#createPalettePass.setOverride`, `src/render/people/crowdAnimation.ts#createPalettePass.setPlays`, …
- `src/render/people/crowdAnimation.ts#createPalettePass` (function, line 117) — write by 6: `src/render/people/crowdAnimation.ts#createPalettePass.setAtlas`, `src/render/people/crowdAnimation.ts#createPalettePass.grow`, `src/render/people/crowdAnimation.ts#createPalettePass.setOverride`, `src/render/people/crowdAnimation.ts#createPalettePass.setPlays`, …
- `src/render/people/crowdAnimation.ts#makeTarget` (function, line 357) — call by 1: `src/render/people/crowdAnimation.ts#createPalettePass.grow`
- `src/render/people/crowdAnimation.ts#MOST_ROW_RANGES` (variable, line 28) — read by 3: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.update`, `src/render/riggedCitizens.ts#createRiggedCitizens.finish`, `src/render/people/crowdAnimation.ts#createPalettePass.render`
- `src/render/people/crowdAnimation.ts#PalettePass` (interface, line 93) — call by 9: `src/render/people/proceduralCrowd.ts#createProceduralCrowd`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.update`, `src/render/riggedCitizens.ts#createRiggedCitizens.grow`, `src/render/riggedCitizens.ts#createRiggedCitizens.addToAtlas`, …
- `src/render/people/crowdAnimation.ts#PalettePass` (interface, line 93) — read by 5: `src/render/riggedCitizens.ts#createRiggedCitizens.grow`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.buildClass`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.growRows`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.renderPalettes`, …
- `src/render/people/crowdAnimation.ts#PalettePlay` (interface, line 87) — read by 1: `src/render/people/crowdAnimation.ts#createPalettePass.setPlays`
- `src/render/people/crowdAnimation.ts#PLAY_TEXELS` (variable, line 34) — read by 4: `src/render/people/crowdAnimation.ts#createPalettePass.grow`, `src/render/people/crowdAnimation.ts#createPalettePass.setPlays`, `src/render/people/crowdAnimation.ts#createPalettePass.rest`, `src/render/people/crowdAnimation.ts#createPalettePass.render`
- `src/render/people/crowdAnimation.ts#PLAYS` (variable, line 32) — read by 2: `src/render/people/crowdAnimation.ts#createPalettePass.setPlays`, `src/render/people/crowdAnimation.ts#createPalettePass.rest`
- `src/render/people/crowdAnimation.ts#rowsTexture` (function, line 350) — call by 1: `src/render/people/crowdAnimation.ts#createPalettePass.grow`

### src/render/people/crowdBodies.ts

- `src/render/people/crowdBodies.ts#BODIES` (variable, line 7) — read by 2: `src/render/people/crowdBodies.ts#createClassBodies`, `src/render/people/crowdBodies.ts#createClassBodies.pick`
- `src/render/people/crowdBodies.ts#ClassBodies` (interface, line 12) — call by 3: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.renderPalettes`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.restore`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.add`
- `src/render/people/crowdBodies.ts#ClassBodies` (interface, line 12) — read by 2: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.buildClass`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.add`
- `src/render/people/crowdBodies.ts#ClassBodies` (interface, line 12) — typeRef by 1: `src/render/people/crowdBodies.ts#createClassBodies`
- `src/render/people/crowdBodies.ts#WIDTH` (variable, line 10) — read by 1: `src/render/people/crowdBodies.ts#createClassBodies`

### src/render/people/crowdLod.ts

- `src/render/people/crowdLod.ts#EYE_TRIANGLES` (variable, line 57) — read by 1: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.makeLodPlan`
- `src/render/people/crowdLod.ts#FAR_TRIANGLES` (variable, line 59) — read by 1: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.buildFar`
- `src/render/people/crowdLod.ts#FAR_ZONES` (variable, line 64) — read by 3: `src/render/people/proceduralCrowd.ts#zoneAreas`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.zoneColours`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.zoneCover`
- `src/render/people/crowdLod.ts#FarZone` (variable, line 63) — read by 4: `src/render/people/crowdLod.ts#boneZone`, `src/render/people/proceduralCrowd.ts#vertexZones`, `src/render/people/crowdLod.ts#headZone`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.zoneColours`
- `src/render/people/crowdLod.ts#HeadFrame` (interface, line 82) — read by 1: `src/render/people/crowdLod.ts#headZone`
- `src/render/people/crowdLod.ts#HeadFrame` (interface, line 82) — typeRef by 3: `src/render/people/proceduralCrowd.ts#headFrame`, `src/render/people/proceduralCrowd.ts#vertexZones`, `src/render/people/crowdLod.ts#headZone`
- `src/render/people/crowdLod.ts#HYSTERESIS` (variable, line 12) — read by 1: `src/render/people/crowdLod.ts#levelFor`
- `src/render/people/crowdLod.ts#ITEM_LEAST_PIXELS` (variable, line 51) — read by 1: `src/render/people/crowdLod.ts#itemShownAt`
- `src/render/people/crowdLod.ts#LEVEL_CAPS` (variable, line 14) — read by 1: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.update`
- `src/render/people/crowdLod.ts#LEVEL_PIXELS` (variable, line 8) — read by 1: `src/render/people/crowdLod.ts#levelFor`
- `src/render/people/crowdLod.ts#LEVEL_TOP_PIXELS` (variable, line 49) — read by 1: `src/render/people/crowdLod.ts#itemShownAt`
- `src/render/people/crowdLod.ts#LEVEL_TRIANGLES` (variable, line 34) — read by 1: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.makeLodPlan`
- `src/render/people/crowdLod.ts#LEVELS` (variable, line 10) — read by 3: `src/render/people/crowdLod.ts#levelFor`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.makeItem`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.lodPlan`
- `src/render/people/crowdLod.ts#lodReady` (variable, line 87) — read by 2: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.buildFar`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.makeItem`
- `src/render/people/crowdLod.ts#PieceRole` (type, line 28) — typeRef by 6: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.wornItem`, `src/render/people/proceduralCrowd.ts#roleOf`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.zoneColours.layer`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.makeItem`, …
- `src/render/people/crowdLod.ts#REGION` (variable, line 213) — read by 2: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.buildFar`, `src/render/people/crowdLod.ts#farMesh`
- `src/render/people/crowdLod.ts#sortedWeights` (variable, line 272) — read by 1: `src/render/people/crowdLod.ts#sortSkinWeights`
- `src/render/people/crowdLod.ts#sortedWeights` (variable, line 272) — write by 1: `src/render/people/crowdLod.ts#sortSkinWeights`

### src/render/people/faceExpression.ts

- `src/render/people/faceExpression.ts#CHANNELS` (variable, line 7) — read by 1: `src/render/people/faceExpression.ts#applyFace`
- `src/render/people/faceExpression.ts#faceAt` (function, line 57) — call by 3: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.update`, `src/render/riggedCitizens.ts#createRiggedCitizens.draw`, `src/render/riggedCitizens.ts#createRiggedCitizens.drawClip`
- `src/render/people/faceExpression.ts#FaceWeights` (type, line 4) — typeRef by 4: `src/render/riggedCitizens.ts#createRiggedCitizens.emit`, `src/render/riggedCitizens.ts#createRiggedCitizens.place`, `src/render/riggedCitizens.ts#setFacialExpression`, `src/render/people/faceExpression.ts#applyFace`
- `src/render/people/faceExpression.ts#GAZE` (variable, line 22) — read by 1: `src/render/people/faceExpression.ts#applyFace`

### src/render/people/gpuRows.ts

- `src/render/people/gpuRows.ts#copier` (variable, line 33) — read by 3: `src/render/people/gpuRows.ts#createGpuRows.flush`, `src/render/people/gpuRows.ts#copyPass`, `src/render/people/gpuRows.ts#drawRows`
- `src/render/people/gpuRows.ts#copier` (variable, line 33) — typeRef by 1: `src/render/people/gpuRows.ts#copyPass`
- `src/render/people/gpuRows.ts#copier` (variable, line 33) — write by 1: `src/render/people/gpuRows.ts#copyPass`
- `src/render/people/gpuRows.ts#GpuRows` (interface, line 13) — call by 11: `src/render/people/proceduralCrowd.ts#createProceduralCrowd`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.buildClass`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.growRows`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.renderPalettes`, …
- `src/render/people/gpuRows.ts#GpuRows` (interface, line 13) — read by 3: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.buildClass`, `src/render/people/crowdBodies.ts#createClassBodies`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.growRows`
- `src/render/people/gpuRows.ts#GpuRows` (interface, line 13) — typeRef by 1: `src/render/people/gpuRows.ts#createGpuRows`
- `src/render/people/gpuRows.ts#warmed` (variable, line 34) — read by 1: `src/render/people/gpuRows.ts#createGpuRows.flush`
- `src/render/people/gpuRows.ts#warmed` (variable, line 34) — write by 1: `src/render/people/gpuRows.ts#createGpuRows.flush`

### src/render/people/heldProps.ts

- `src/render/people/heldProps.ts#CAPACITY` (variable, line 89) — read by 2: `src/render/people/heldProps.ts#createHeldProps.place`, `src/render/people/heldProps.ts#createHeldProps.placeMatrix`
- `src/render/people/heldProps.ts#createHeldProps` (function, line 101) — read by 3: `src/render/people/heldProps.ts#createHeldProps.place`, `src/render/people/heldProps.ts#createHeldProps.placeMatrix`, `src/render/people/heldProps.ts#createHeldProps.begin`
- `src/render/people/heldProps.ts#createHeldProps` (function, line 101) — write by 3: `src/render/people/heldProps.ts#createHeldProps.place`, `src/render/people/heldProps.ts#createHeldProps.placeMatrix`, `src/render/people/heldProps.ts#createHeldProps.begin`
- `src/render/people/heldProps.ts#HELD` (variable, line 13) — read by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.draw`
- `src/render/people/heldProps.ts#HeldProps` (interface, line 91) — call by 2: `src/render/riggedCitizens.ts#createRiggedCitizens.begin`, `src/render/riggedCitizens.ts#createRiggedCitizens.draw`
- `src/render/people/heldProps.ts#UP` (variable, line 87) — read by 1: `src/render/people/heldProps.ts#createHeldProps.place`
- `src/render/people/heldProps.ts#UPRIGHT` (variable, line 81) — read by 1: `src/render/people/heldProps.ts#createHeldProps.place`

### src/render/people/lodPool.ts

- `src/render/people/lodPool.ts#lodPool` (function, line 9) — call by 2: `src/render/vehicleLevels.ts#partLevels`, `src/render/people/crowdLod.ts#simplifiedIndexAway`
- `src/render/people/lodPool.ts#lodPool` (function, line 9) — implementation by 2: `src/render/vehicleLevels.ts#partLevels`, `src/render/people/crowdLod.ts#simplifiedIndexAway`
- `src/render/people/lodPool.ts#lodPool` (function, line 9) — reference by 1: `src/render/agents.ts#<ExpressionStatement>@76`

### src/render/people/lodPort.ts

- `src/render/people/lodPort.ts#LodPool` (interface, line 7) — call by 2: `src/render/vehicleLevels.ts#partLevels`, `src/render/people/crowdLod.ts#simplifiedIndexAway`
- `src/render/people/lodPort.ts#providePeopleLod` (function, line 18) — call by 1: `src/render/agents.ts#<ExpressionStatement>@76`

### src/render/people/lodWorker.ts

- `src/render/people/lodWorker.ts#simplifyIndex` (function, line 68) — call by 1: `src/render/people/crowdLod.ts#simplifiedIndex`

### src/render/people/paletteTargets.ts

- `src/render/people/paletteTargets.ts#fit` (function, line 44) — call by 1: `src/render/people/paletteTargets.ts#growSlab`
- `src/render/people/paletteTargets.ts#freeSlab` (function, line 75) — call by 1: `src/render/people/paletteTargets.ts#growSlab`
- `src/render/people/paletteTargets.ts#Page` (interface, line 9) — read by 2: `src/render/people/paletteTargets.ts#growSlab`, `src/render/people/crowdAnimation.ts#createPalettePass.render`
- `src/render/people/paletteTargets.ts#Page` (interface, line 9) — write by 1: `src/render/people/paletteTargets.ts#growSlab`
- `src/render/people/paletteTargets.ts#PAGE_ROWS` (variable, line 7) — read by 1: `src/render/people/paletteTargets.ts#growSlab`
- `src/render/people/paletteTargets.ts#Slab` (interface, line 18) — read by 2: `src/render/people/paletteTargets.ts#growSlab`, `src/render/people/crowdAnimation.ts#createPalettePass.render`
- `src/render/people/paletteTargets.ts#Slab` (interface, line 18) — typeRef by 1: `src/render/people/paletteTargets.ts#growSlab`
- `src/render/people/paletteTargets.ts#Slab` (interface, line 18) — write by 1: `src/render/people/paletteTargets.ts#growSlab`
- `src/render/people/paletteTargets.ts#takeSlab` (function, line 56) — call by 1: `src/render/people/paletteTargets.ts#growSlab`

### src/render/people/personRig.ts

- `src/render/people/personRig.ts#CAPTURE_NAME` (variable, line 41) — read by 5: `src/render/people/proceduralCrowd.ts#createProceduralCrowd`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.buildClass`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.applyJolt`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.postureRig.name`, …
- `src/render/people/personRig.ts#createPersonRig` (function, line 181) — callbackAssign by 2: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.classRigFor.fit`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.setFace`
- `src/render/people/personRig.ts#createPersonRig.deltasAt` (function, line 401) — implementation by 1: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.setFace`
- `src/render/people/personRig.ts#createPersonRig.wear` (function, line 374) — implementation by 1: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.classRigFor.fit`
- `src/render/people/personRig.ts#PersonRig` (interface, line 118) — call by 2: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.classRigFor.fit`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.setFace`
- `src/render/people/personRig.ts#PersonRig` (interface, line 118) — read by 9: `src/render/people/proceduralCrowd.ts#createProceduralCrowd`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.buildClass`, `src/render/people/classRigMade.ts#rigFromMade`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.applyJolt`, …
- `src/render/people/personRig.ts#PersonRig` (interface, line 118) — typeRef by 2: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.classRigFor`, `src/render/people/classRigMade.ts#rigFromMade`
- `src/render/people/personRig.ts#RigDeltas` (interface, line 145) — read by 2: `src/render/people/classRigMade.ts#rigFromMade`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.faceShare`
- `src/render/people/personRig.ts#rigDeltasAt` (function, line 153) — call by 1: `src/render/people/classRigMade.ts#rigFromMade`
- `src/render/people/personRig.ts#WornPiece` (interface, line 110) — read by 2: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.strandPiece`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.wornItem`
- `src/render/people/personRig.ts#WornPiece` (interface, line 110) — typeRef by 2: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.classRigFor`, `src/render/bakePool.ts#wearInWorker`

### src/render/people/personVariants.ts

- `src/render/people/personVariants.ts#castsAt` (function, line 34) — call by 1: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.makeItem`
- `src/render/people/personVariants.ts#Kind` (type, line 4) — typeRef by 4: `src/render/people/proceduralCrowd.ts#kindOf`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.makeItem`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.lodPlan`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.makeLodPlan`
- `src/render/people/personVariants.ts#Variant` (interface, line 7) — read by 1: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.makePiece`
- `src/render/people/personVariants.ts#Variant` (interface, line 7) — typeRef by 4: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.strandPiece`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.makePiece`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.buildFar`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.makeItem`

### src/render/people/proceduralBake.ts

- `src/render/people/proceduralBake.ts#AgeBand` (type, line 28) — typeRef by 6: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.buildClass`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.classRigFor`, `src/render/bakePool.ts#classRigInWorker`, `src/render/bakePool.ts#wearInWorker`, …
- `src/render/people/proceduralBake.ts#BakeEnv` (interface, line 38) — call by 6: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.strandPiece`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.buildClass`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.classRigFor`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.add`, …
- `src/render/people/proceduralBake.ts#BakeEnv` (interface, line 38) — read by 8: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.strandPiece`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.buildClass`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.classRigFor`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.woundSpots`, …
- `src/render/people/proceduralBake.ts#BakeEnv` (interface, line 38) — typeRef by 1: `src/render/people/proceduralBake.ts#hairCards`
- `src/render/people/proceduralBake.ts#classData` (function, line 167) — call by 2: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.buildClass`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.buildClass.reloadHead`
- `src/render/people/proceduralBake.ts#ClassData` (interface, line 101) — read by 2: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.buildClass`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.buildClass.headOf`
- `src/render/people/proceduralBake.ts#ClassData` (interface, line 101) — typeRef by 1: `src/render/people/proceduralBake.ts#classFromRecord`
- `src/render/people/proceduralBake.ts#classRig` (function, line 150) — call by 1: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.classRigFor`
- `src/render/people/proceduralBake.ts#classRig` (function, line 150) — read by 1: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.classRigFor.fit`
- `src/render/people/proceduralBake.ts#createBakeEnv` (function, line 48) — call by 1: `src/render/people/proceduralCrowd.ts#createProceduralCrowd`
- `src/render/people/proceduralBake.ts#createBakeEnv.eyes` (method, line 61) — implementation by 2: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.classRigFor`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.item`
- `src/render/people/proceduralBake.ts#createBakeEnv.setup` (method, line 55) — implementation by 6: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.strandPiece`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.buildClass`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.classRigFor`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.add`, …
- `src/render/people/proceduralBake.ts#EXPR` (variable, line 25) — read by 1: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.update`
- `src/render/people/proceduralBake.ts#EXPR_SLOTS` (variable, line 26) — read by 1: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.buildClass`
- `src/render/people/proceduralBake.ts#FACE_WIDTH` (variable, line 22) — read by 5: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.buildClass`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.buildClass.headOf`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.setFace`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.faceShare`, …
- `src/render/people/proceduralBake.ts#hairBase` (function, line 86) — call by 2: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.item`, `src/render/people/proceduralBake.ts#hairCards`
- `src/render/people/proceduralBake.ts#SHAPES` (variable, line 19) — read by 2: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.buildClass`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.add`

### src/render/people/proceduralCook.ts

- `src/render/people/proceduralCook.ts#COOK_HASH` (variable, line 8) — read by 1: `src/render/people/proceduralCook.ts#cookedNames`
- `src/render/people/proceduralCook.ts#manifest` (variable, line 10) — read by 1: `src/render/people/proceduralCook.ts#cookedNames`
- `src/render/people/proceduralCook.ts#manifest` (variable, line 10) — write by 1: `src/render/people/proceduralCook.ts#cookedNames`

### src/render/people/proceduralCrowd.ts

- `src/render/people/proceduralCrowd.ts#ACCESSORY_COLOURS` (variable, line 285) — read by 1: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.add`
- `src/render/people/proceduralCrowd.ts#BLOB_RADIUS` (variable, line 282) — read by 1: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.update`
- `src/render/people/proceduralCrowd.ts#BodyClass` (interface, line 150) — call by 3: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.strandPiece`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.restore`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.wornItem`
- `src/render/people/proceduralCrowd.ts#BodyClass` (interface, line 150) — read by 35: `src/render/people/proceduralCrowd.ts#createProceduralCrowd`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.update`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.moveTo`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.poseRow`, …
- `src/render/people/proceduralCrowd.ts#BodyClass` (interface, line 150) — typeRef by 30: `src/render/people/proceduralCrowd.ts#createProceduralCrowd`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.poseRow`, `src/render/people/proceduralCrowd.ts#PersonState`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.strandPiece`, …
- `src/render/people/proceduralCrowd.ts#BodyClass` (interface, line 150) — write by 16: `src/render/people/proceduralCrowd.ts#createProceduralCrowd`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.update`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.poseRow`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.strandPiece`, …
- `src/render/people/proceduralCrowd.ts#COVER_WIDTH` (variable, line 104) — read by 2: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.buildClass`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.coverRow`
- `src/render/people/proceduralCrowd.ts#COVERING` (variable, line 113) — read by 2: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.add`, `src/render/people/proceduralCrowd.ts#roleOf`
- `src/render/people/proceduralCrowd.ts#depthMaterial` (function, line 627) — call by 1: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.makeItem`
- `src/render/people/proceduralCrowd.ts#EXPR_AT` (variable, line 59) — read by 3: `src/render/people/proceduralCrowd.ts#createProceduralCrowd`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.update`, `src/render/people/proceduralCrowd.ts#personRows`
- `src/render/people/proceduralCrowd.ts#farMaterial` (function, line 581) — call by 1: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.buildFar`
- `src/render/people/proceduralCrowd.ts#INSTANCED` (variable, line 116) — read by 2: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.skinItem`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.makeItem`
- `src/render/people/proceduralCrowd.ts#Item` (interface, line 134) — read by 6: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.moveTo`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.makePiece`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.add`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.zoneColours`, …
- `src/render/people/proceduralCrowd.ts#Item` (interface, line 134) — typeRef by 7: `src/render/people/proceduralCrowd.ts#createProceduralCrowd`, `src/render/people/proceduralCrowd.ts#Wear`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.makePiece`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.skinItem`, …
- `src/render/people/proceduralCrowd.ts#Item` (interface, line 134) — write by 1: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.makeItem`
- `src/render/people/proceduralCrowd.ts#LOD_PLAN_HASH` (variable, line 858) — read by 1: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.lodPlan`
- `src/render/people/proceduralCrowd.ts#PERSON_FLOATS` (variable, line 57) — read by 7: `src/render/people/proceduralCrowd.ts#createProceduralCrowd`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.update`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.woundedRow`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.buildClass`, …
- `src/render/people/proceduralCrowd.ts#Piece` (interface, line 119) — read by 11: `src/render/people/proceduralCrowd.ts#createProceduralCrowd`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.update`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.strandsUpdate`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.upload`, …
- `src/render/people/proceduralCrowd.ts#Piece` (interface, line 119) — typeRef by 9: `src/render/people/proceduralCrowd.ts#createProceduralCrowd`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.upload`, `src/render/people/proceduralCrowd.ts#PersonState`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.growPiece`, …
- `src/render/people/proceduralCrowd.ts#Piece` (interface, line 119) — write by 7: `src/render/people/proceduralCrowd.ts#createProceduralCrowd`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.update`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.strandsUpdate`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.growPiece`, …
- `src/render/people/proceduralCrowd.ts#PIECE_START` (variable, line 280) — read by 1: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.makePiece`
- `src/render/people/proceduralCrowd.ts#pieceMaterial` (function, line 475) — call by 2: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.strandPiece`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.makeItem`
- `src/render/people/proceduralCrowd.ts#PLACE_AT` (variable, line 61) — read by 1: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.update`
- `src/render/people/proceduralCrowd.ts#plain` (variable, line 287) — read by 1: `src/render/people/proceduralCrowd.ts#plainTexture`
- `src/render/people/proceduralCrowd.ts#plain` (variable, line 287) — write by 1: `src/render/people/proceduralCrowd.ts#plainTexture`
- `src/render/people/proceduralCrowd.ts#ProcClip` (type, line 231) — typeRef by 6: `src/render/agents.ts#AgentMeshes`, `src/render/agents.ts#createAgentMeshes`, `src/render/agents.ts#createAgentMeshes.procPose`, `src/render/agents.ts#gaitClip`, …
- `src/render/people/proceduralCrowd.ts#ProceduralPerson` (interface, line 233) — read by 17: `src/render/agents.ts#createAgentMeshes`, `src/render/agents.ts#createAgentMeshes.animProbe`, `src/render/agents.ts#createAgentMeshes.meshProbe`, `src/render/agents.ts#createAgentMeshes.procBuildNext`, …
- `src/render/people/proceduralCrowd.ts#ProceduralPerson` (interface, line 233) — typeRef by 17: `src/render/agents.ts#createAgentMeshes`, `src/render/agents.ts#createAgentMeshes.procOf`, `src/render/agents.ts#createAgentMeshes.procSpareFor`, `src/render/agents.ts#createAgentMeshes.procSuits`, …
- `src/render/people/proceduralCrowd.ts#ProceduralPerson` (interface, line 233) — write by 3: `src/render/agents.ts#createAgentMeshes`, `src/render/agents.ts#createAgentMeshes.procDraw`, `src/render/agents.ts#createAgentMeshes.procPose`
- `src/render/people/proceduralCrowd.ts#ROW_START` (variable, line 278) — read by 1: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.buildClass`
- `src/render/people/proceduralCrowd.ts#samplers` (variable, line 738) — read by 1: `src/render/people/proceduralCrowd.ts#textureSampler`
- `src/render/people/proceduralCrowd.ts#samplers` (variable, line 738) — write by 1: `src/render/people/proceduralCrowd.ts#textureSampler`
- `src/render/people/proceduralCrowd.ts#spotsOf` (variable, line 70) — read by 1: `src/render/people/proceduralCrowd.ts#woundSpotsOf`
- `src/render/people/proceduralCrowd.ts#spotsOf` (variable, line 70) — write by 1: `src/render/people/proceduralCrowd.ts#woundSpotsOf`
- `src/render/people/proceduralCrowd.ts#WOUND_BONES` (variable, line 65) — read by 1: `src/render/people/proceduralCrowd.ts#woundSpotsOf`
- `src/render/people/proceduralCrowd.ts#WOUND_SLOTS` (variable, line 52) — read by 2: `src/render/people/proceduralCrowd.ts#createProceduralCrowd`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.woundedRow`
- `src/render/people/proceduralCrowd.ts#ZONE_AT` (variable, line 63) — read by 2: `src/render/people/proceduralCrowd.ts#createProceduralCrowd`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.zoneColours`

### src/render/people/skinAppearance.ts

- `src/render/people/skinAppearance.ts#cardLeaseOf` (function, line 272) — call by 2: `src/render/people/skinAppearance.ts#itemTexture`, `src/render/people/skinAppearance.ts#skinTextures`
- `src/render/people/skinAppearance.ts#paddedGarment` (function, line 648) — call by 1: `src/render/people/skinAppearance.ts#itemTexture`
- `src/render/people/skinAppearance.ts#skinChoice` (function, line 217) — call by 1: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.bareSkin`
- `src/render/people/skinAppearance.ts#skinChoice` (function, line 217) — read by 2: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.add`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.skinItem`
- `src/render/people/skinAppearance.ts#skinChoice` (function, line 217) — typeRef by 1: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.bareSkin`
- `src/render/people/skinAppearance.ts#skinLeaseOf` (function, line 263) — call by 1: `src/render/people/skinAppearance.ts#skinTextures`
- `src/render/people/skinAppearance.ts#TextureLease` (interface, line 604) — read by 2: `src/render/people/skinAppearance.ts#itemTexture`, `src/render/people/skinAppearance.ts#skinTextures`

### src/render/pinned.ts

- `src/render/pinned.ts#pinned` (function, line 5) — call by 2: `src/render/densityCars.ts#createDensityCars`, `src/render/densityCars.ts#createDensityCars.tileAt`

### src/render/quality.ts

- `src/render/quality.ts#HAIR_STRAND_PEOPLE` (variable, line 10) — read by 2: `src/render/people/proceduralCrowd.ts#createProceduralCrowd`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.update`
- `src/render/quality.ts#QualitySettings` (interface, line 12) — read by 1: `src/render/agents.ts#createAgentMeshes.sync`
- `src/render/quality.ts#QualitySettings` (interface, line 12) — typeRef by 1: `src/render/agents.ts#AgentRenderOptions`
- `src/render/quality.ts#VEHICLE_ERROR_PIXELS` (variable, line 8) — read by 1: `src/render/vehicleLevels.ts#ERROR_PIXELS`
- `src/render/quality.ts#VEHICLE_WHOLE_CAP` (variable, line 9) — read by 1: `src/render/agents.ts#VEHICLE_NEAR_CAP`

### src/render/ragdoll.ts

- `src/render/ragdoll.ts#RagdollCitizens` (interface, line 14) — typeRef by 2: `src/render/agents.ts#AgentRenderOptions`, `src/render/agents.ts#createAgentMeshes`

### src/render/renderer.ts

- `src/render/renderer.ts#createSceneRenderer` (function, line 432) — callbackAssign by 2: `src/render/agents.ts#createAgentMeshes.sync`, `src/render/agents.ts#createAgentMeshes.sync.drawVehicle`
- `src/render/renderer.ts#createSceneRenderer` (function, line 432) — implementation by 2: `src/render/agents.ts#createAgentMeshes.sync`, `src/render/agents.ts#createAgentMeshes.sync.drawVehicle`
- `src/render/renderer.ts#createSceneRenderer.deckHeight` (function, line 1257) — indirectCall by 4: `src/render/densityCars.ts#createDensityCars.writeLine`, `src/render/agents.ts#createAgentMeshes.elevationOnCurrent`, `src/render/agents.ts#createAgentMeshes.standingDeck`, `src/render/agents.ts#createAgentMeshes.sync`
- `src/render/renderer.ts#createSceneRenderer.onAssetsReady` (function, line 447) — indirectCall by 2: `src/render/agents.ts#createAgentMeshes.ladder`, `src/render/agents.ts#createAgentMeshes.panelLadder`
- `src/render/renderer.ts#createSceneRenderer.pedestrianVisible` (function, line 1331) — implementation by 1: `src/render/agents.ts#createAgentMeshes.sync`
- `src/render/renderer.ts#createSceneRenderer.personPixels` (function, line 1337) — implementation by 1: `src/render/agents.ts#createAgentMeshes.sync`
- `src/render/renderer.ts#createSceneRenderer.screenScale` (function, line 1349) — implementation by 1: `src/render/agents.ts#createAgentMeshes.sync`
- `src/render/renderer.ts#createSceneRenderer.vehicleInside` (function, line 1365) — implementation by 1: `src/render/agents.ts#createAgentMeshes.sync`
- `src/render/renderer.ts#createSceneRenderer.vehicleVisible` (function, line 1359) — implementation by 2: `src/render/agents.ts#createAgentMeshes.sync`, `src/render/agents.ts#createAgentMeshes.sync.drawVehicle`

### src/render/riderPoses.ts

- `src/render/riderPoses.ts#BIKE_FIT` (variable, line 108) — read by 1: `src/render/vehicleModels.ts#bicycleModel`
- `src/render/riderPoses.ts#CAB_WHEEL` (variable, line 63) — read by 3: `src/render/carBody.ts#buildCarModel`, `src/render/vehicleModels.ts#buildBusModel`, `src/render/vehicleModels.ts#buildTruckModel`
- `src/render/riderPoses.ts#DRIVER_WHEEL` (variable, line 60) — read by 1: `src/render/carBody.ts#buildCarModel`
- `src/render/riderPoses.ts#gripPoint` (function, line 129) — call by 1: `src/render/vehicleModels.ts#gripInHead`
- `src/render/riderPoses.ts#HELMET_SEGMENTS` (variable, line 328) — read by 1: `src/render/agents.ts#createAgentMeshes`
- `src/render/riderPoses.ts#MOTO_FIT` (variable, line 98) — read by 1: `src/render/vehicleModels.ts#motorcycleModel`
- `src/render/riderPoses.ts#NO_HELMET` (variable, line 319) — read by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.helmetFits`
- `src/render/riderPoses.ts#RiderClipKey` (type, line 5) — typeRef by 7: `src/render/occupants.ts#clipsFor`, `src/render/occupants.ts#OccupantClip`, `src/render/occupants.ts#occupantPlays`, `src/render/occupants.ts#SEAT_CLIPS`, …
- `src/render/riderPoses.ts#SEATED_IDLE` (variable, line 47) — read by 1: `src/render/occupants.ts#occupantPlays`
- `src/render/riderPoses.ts#STEER_FULL` (variable, line 121) — read by 1: `src/render/agents.ts#createAgentMeshes.placeRiderFigure`
- `src/render/riderPoses.ts#TwoWheelerFit` (interface, line 76) — read by 4: `src/render/vehicleModels.ts#bicycleModel`, `src/render/vehicleModels.ts#gripInHead`, `src/render/vehicleModels.ts#motorcycleModel`, `src/render/agents.ts#createAgentMeshes.sync.drawVehicle`
- `src/render/riderPoses.ts#TwoWheelerFit` (interface, line 76) — typeRef by 3: `src/render/vehicleModels.ts#gripInHead`, `src/render/vehicleModels.ts#handlebar`, `src/render/vehicleModels.ts#TwoWheelerModel`
- `src/render/riderPoses.ts#WheelSpec` (interface, line 52) — read by 4: `src/render/carBody.ts#buildCarModel`, `src/render/vehicleModels.ts#buildBusModel`, `src/render/vehicleModels.ts#buildTruckModel`, `src/render/agents.ts#createAgentMeshes.placeCarOccupants`
- `src/render/riderPoses.ts#WheelSpec` (interface, line 52) — typeRef by 1: `src/render/vehicleModels.ts#SteeringModel`

### src/render/riggedCitizens.ts

- `src/render/riggedCitizens.ts#BODY_EXTENT` (variable, line 115) — read by 3: `src/render/riggedCitizens.ts#createRiggedCitizens.levelOf`, `src/render/riggedCitizens.ts#citizenLevelOfFigure`, `src/render/riggedCitizens.ts#citizenLevelAt`
- `src/render/riggedCitizens.ts#BURNT` (variable, line 178) — read by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.drawPalette`
- `src/render/riggedCitizens.ts#CAPACITY` (variable, line 40) — read by 6: `src/render/riggedCitizens.ts#createRiggedCitizens.draw`, `src/render/riggedCitizens.ts#createRiggedCitizens.grow`, `src/render/riggedCitizens.ts#growLevel`, `src/render/riggedCitizens.ts#setFacialExpression`, …
- `src/render/riggedCitizens.ts#CITIZEN_LEVEL_ERROR` (variable, line 113) — read by 1: `src/render/riggedCitizens.ts#citizenLevelAt`
- `src/render/riggedCitizens.ts#CitizenBatch` (interface, line 59) — read by 18: `src/render/riggedCitizens.ts#createRiggedCitizens.begin`, `src/render/riggedCitizens.ts#createRiggedCitizens.draw`, `src/render/riggedCitizens.ts#createRiggedCitizens.finish`, `src/render/riggedCitizens.ts#createRiggedCitizens.emit`, …
- `src/render/riggedCitizens.ts#CitizenBatch` (interface, line 59) — typeRef by 7: `src/render/riggedCitizens.ts#createRiggedCitizens.emit`, `src/render/riggedCitizens.ts#createRiggedCitizens.grow`, `src/render/riggedCitizens.ts#createRiggedCitizens.want`, `src/render/riggedCitizens.ts#createRiggedCitizens.addToAtlas`, …
- `src/render/riggedCitizens.ts#CitizenBatch` (interface, line 59) — write by 7: `src/render/riggedCitizens.ts#createRiggedCitizens.begin`, `src/render/riggedCitizens.ts#createRiggedCitizens.emit`, `src/render/riggedCitizens.ts#createRiggedCitizens.grow`, `src/render/riggedCitizens.ts#createRiggedCitizens.want`, …
- `src/render/riggedCitizens.ts#CitizenPart` (interface, line 55) — read by 3: `src/render/riggedCitizens.ts#createRiggedCitizens.begin`, `src/render/riggedCitizens.ts#createRiggedCitizens.finish`, `src/render/riggedCitizens.ts#createRiggedCitizens.place`
- `src/render/riggedCitizens.ts#createRiggedCitizens` (function, line 339) — call by 6: `src/render/agents.ts#createAgentMeshes`, `src/render/agents.ts#createAgentMeshes.setBleed`, `src/render/agents.ts#createAgentMeshes.sync`, `src/render/riggedCitizens.ts#createRiggedCitizens.draw`, …
- `src/render/riggedCitizens.ts#createRiggedCitizens` (function, line 339) — read by 27: `src/render/agents.ts#createAgentMeshes`, `src/render/riggedCitizens.ts#createRiggedCitizens.begin`, `src/render/riggedCitizens.ts#createRiggedCitizens.draw`, `src/render/riggedCitizens.ts#createRiggedCitizens.finish`, …
- `src/render/riggedCitizens.ts#createRiggedCitizens` (function, line 339) — typeRef by 1: `src/render/agents.ts#AgentMeshes`
- `src/render/riggedCitizens.ts#createRiggedCitizens` (function, line 339) — write by 10: `src/render/riggedCitizens.ts#createRiggedCitizens.begin`, `src/render/riggedCitizens.ts#createRiggedCitizens.draw`, `src/render/riggedCitizens.ts#createRiggedCitizens.finish`, `src/render/riggedCitizens.ts#createRiggedCitizens.request`, …
- `src/render/riggedCitizens.ts#createRiggedCitizens.applyBand` (function, line 443) — call by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.begin`
- `src/render/riggedCitizens.ts#createRiggedCitizens.disposeOwned` (function, line 360) — call by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.dispose`
- `src/render/riggedCitizens.ts#createRiggedCitizens.load` (function, line 514) — call by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.pump`
- `src/render/riggedCitizens.ts#KeptPose` (interface, line 90) — read by 3: `src/render/riggedCitizens.ts#createRiggedCitizens.draw`, `src/render/riggedCitizens.ts#createRiggedCitizens.finish`, `src/render/riggedCitizens.ts#createRiggedCitizens.capturedPose`
- `src/render/riggedCitizens.ts#KeptPose` (interface, line 90) — write by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.draw`
- `src/render/riggedCitizens.ts#LevelMesh` (interface, line 46) — read by 3: `src/render/riggedCitizens.ts#createRiggedCitizens.finish`, `src/render/riggedCitizens.ts#createRiggedCitizens.place`, `src/render/riggedCitizens.ts#growLevel`
- `src/render/riggedCitizens.ts#LevelMesh` (interface, line 46) — typeRef by 1: `src/render/riggedCitizens.ts#growLevel`
- `src/render/riggedCitizens.ts#LevelMesh` (interface, line 46) — write by 4: `src/render/riggedCitizens.ts#createRiggedCitizens.begin`, `src/render/riggedCitizens.ts#createRiggedCitizens.finish`, `src/render/riggedCitizens.ts#createRiggedCitizens.place`, `src/render/riggedCitizens.ts#growLevel`
- `src/render/riggedCitizens.ts#LIMB_REACH` (variable, line 42) — read by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.finish`
- `src/render/riggedCitizens.ts#NEUTRAL_FACE` (variable, line 181) — read by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.drawPalette`
- `src/render/riggedCitizens.ts#paletteBytes` (function, line 103) — call by 2: `src/render/riggedCitizens.ts#createRiggedCitizens.grow`, `src/render/riggedCitizens.ts#createRiggedCitizens.addToAtlas`
- `src/render/riggedCitizens.ts#UNBURNT` (variable, line 177) — read by 2: `src/render/riggedCitizens.ts#createRiggedCitizens.emit`, `src/render/riggedCitizens.ts#createRiggedCitizens.drawPalette`

### src/render/stagedTexels.ts

- `src/render/stagedTexels.ts#stageTexels` (function, line 68) — call by 1: `src/render/people/hairTexture.ts#hairStrandTexture`

### src/render/uploads.ts

- `src/render/uploads.ts#compileAhead` (function, line 242) — call by 1: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.makePiece`
- `src/render/uploads.ts#shadowStandIn` (function, line 75) — call by 1: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.makePiece`

### src/render/wind.ts

- `src/render/wind.ts#windUniforms` (variable, line 13) — read by 1: `src/render/exhaust.ts#createExhaust`

### src/sim/ambient/ambient.ts

- `src/sim/ambient/ambient.ts#AmbientWorld` (class, line 62) — typeRef by 1: `src/sim/agents/lotTraffic.ts#LotTraffic.ambient`
- `src/sim/ambient/ambient.ts#AmbientWorld.enabled` (field, line 64) — read by 1: `src/sim/agents/lotTraffic.ts#LotTraffic.step`
- `src/sim/ambient/ambient.ts#AmbientWorld.isParked` (method, line 752) — call by 1: `src/sim/agents/lorryTraffic.ts#LorryTraffic.step`
- `src/sim/ambient/ambient.ts#AmbientWorld.leaveSource` (method, line 956) — call by 1: `src/sim/ambient/ambient.ts#AmbientWorld.release`
- `src/sim/ambient/ambient.ts#AmbientWorld.logParked` (method, line 790) — call by 1: `src/sim/ambient/ambient.ts#AmbientWorld.addParked`
- `src/sim/ambient/ambient.ts#AmbientWorld.movers` (field, line 74) — read by 1: `src/sim/ambient/ambient.ts#AmbientWorld.addParked`
- `src/sim/ambient/ambient.ts#AmbientWorld.movers` (field, line 74) — write by 1: `src/sim/ambient/ambient.ts#AmbientWorld.addParked`
- `src/sim/ambient/ambient.ts#AmbientWorld.parked` (field, line 68) — read by 1: `src/sim/ambient/ambient.ts#AmbientWorld.addParked`
- `src/sim/ambient/ambient.ts#AmbientWorld.parked` (field, line 68) — write by 1: `src/sim/ambient/ambient.ts#AmbientWorld.addParked`
- `src/sim/ambient/ambient.ts#AmbientWorld.parkedAt` (field, line 72) — read by 1: `src/sim/ambient/ambient.ts#AmbientWorld.addParked`
- `src/sim/ambient/ambient.ts#AmbientWorld.parkedAt` (field, line 72) — write by 1: `src/sim/ambient/ambient.ts#AmbientWorld.addParked`
- `src/sim/ambient/ambient.ts#AmbientWorld.removeParked` (method, line 767) — call by 3: `src/sim/agents/lorryTraffic.ts#LorryTraffic.drop`, `src/sim/agents/lotTraffic.ts#LotTraffic.drop`, `src/sim/ambient/ambient.ts#AmbientWorld.release`
- `src/sim/ambient/ambient.ts#AmbientWorld.sceneryAt` (field, line 84) — read by 2: `src/sim/ambient/ambient.ts#AmbientWorld.sceneryCarAt`, `src/sim/ambient/ambient.ts#AmbientWorld.release`
- `src/sim/ambient/ambient.ts#AmbientWorld.sceneryAt` (field, line 84) — write by 1: `src/sim/ambient/ambient.ts#AmbientWorld.release`
- `src/sim/ambient/ambient.ts#AmbientWorld.sceneryWatch` (field, line 86) — call by 1: `src/sim/ambient/ambient.ts#AmbientWorld.release`
- `src/sim/ambient/ambient.ts#AmbientWorld.sceneryWatch` (field, line 86) — callbackAssign by 2: `src/sim/agents/lotTraffic.ts#LotTraffic.step`, `src/sim/ambient/ambient.ts#AmbientWorld.release`
- `src/sim/ambient/ambient.ts#AmbientWorld.sceneryWatch` (field, line 86) — implementation by 2: `src/sim/agents/lotTraffic.ts#LotTraffic.step`, `src/sim/ambient/ambient.ts#AmbientWorld.release`
- `src/sim/ambient/ambient.ts#AmbientWorld.sceneryWatch` (field, line 86) — write by 1: `src/sim/agents/lotTraffic.ts#LotTraffic.step`
- `src/sim/ambient/ambient.ts#AmbientWorld.sighted` (field, line 96) — read by 1: `src/sim/vehicles/spawn.ts#stepDespawn`
- `src/sim/ambient/ambient.ts#AmbientWorld.standing` (field, line 70) — read by 1: `src/sim/ambient/ambient.ts#AmbientWorld.addParked`
- `src/sim/ambient/ambient.ts#AmbientWorld.standing` (field, line 70) — write by 1: `src/sim/ambient/ambient.ts#AmbientWorld.addParked`
- `src/sim/ambient/ambient.ts#AmbientWorld.vacated` (field, line 731) — read by 1: `src/sim/ambient/ambient.ts#AmbientWorld.release`
- `src/sim/ambient/ambient.ts#AmbientWorld.vacated` (field, line 731) — write by 1: `src/sim/ambient/ambient.ts#AmbientWorld.release`
- `src/sim/ambient/ambient.ts#newPlaces` (function, line 987) — callbackAssign by 1: `src/sim/ambient/lotPlaces.ts#LotPlaces.workOut`
- `src/sim/ambient/ambient.ts#newPlaces` (function, line 987) — implementation by 1: `src/sim/ambient/lotPlaces.ts#LotPlaces.workOut`
- `src/sim/ambient/ambient.ts#SceneryCar` (interface, line 52) — read by 1: `src/sim/ambient/ambient.ts#AmbientWorld.release`
- `src/sim/ambient/ambient.ts#SceneryCar` (interface, line 52) — typeRef by 2: `src/sim/ambient/ambient.ts#AmbientWorld.sceneryCarAt`, `src/sim/ambient/ambient.ts#AmbientWorld.release`
- `src/sim/ambient/ambient.ts#TRAFFIC` (variable, line 46) — read by 1: `src/sim/meso/meso.ts#Meso.admit`

### src/sim/audit.ts

- `src/sim/audit.ts#AuditIssue` (interface, line 29) — typeRef by 1: `src/sim/agents/walk.ts#createAgentWalkEngine`

### src/sim/city/city.ts

- `src/sim/city/city.ts#City.minutes` (method, line 21) — call by 3: `src/sim/agents/lotTraffic.ts#LotTraffic.hour`, `src/sim/meso/meso.ts#Meso.begin`, `src/sim/meso/meso.ts#Meso.sync`
- `src/sim/city/city.ts#City.transit` (field, line 14) — read by 3: `src/sim/agents/walk.ts#stepWalkers`, `src/sim/intersections/admission.ts#admit`, `src/sim/vehicles/obstacles.ts#longitudinalConstraints`

### src/sim/clock.ts

- `src/sim/clock.ts#SimClock.tick` (field, line 7) — read by 12: `src/sim/agents/lorryTraffic.ts#LorryTraffic.join`, `src/sim/agents/lorryTraffic.ts#LorryTraffic.offRoad`, `src/sim/agents/lorryTraffic.ts#LorryTraffic.sendNow`, `src/sim/agents/lotTraffic.ts#LotTraffic.leaving`, …
- `src/sim/clock.ts#SimClock.time` (getter, line 16) — call by 1: `src/sim/meso/meso.ts#Meso.work`

### src/sim/crossings/state.ts

- `src/sim/crossings/state.ts#CrossingOccupant` (interface, line 4) — read by 2: `src/sim/intersections/admission.ts#crossingReachedFirst`, `src/sim/intersections/crossingSpans.ts#pedestrianAffectsSpan`
- `src/sim/crossings/state.ts#CrossingOccupant` (interface, line 4) — typeRef by 1: `src/sim/intersections/crossingSpans.ts#pedestrianAffectsSpan`
- `src/sim/crossings/state.ts#CrossingState` (interface, line 16) — read by 4: `src/sim/agents/walk.ts#publish`, `src/sim/intersections/admission.ts#crossingReachedFirst`, `src/sim/intersections/admission.ts#pedestrianHasPriority`, `src/sim/intersections/crossingSpans.ts#pedestrianInSpan`
- `src/sim/crossings/state.ts#CrossingState` (interface, line 16) — typeRef by 2: `src/sim/agents/walk.ts#State`, `src/sim/crossings/state.ts#emptyCrossingState`
- `src/sim/crossings/state.ts#CrossingState` (interface, line 16) — write by 1: `src/sim/agents/walk.ts#publish`

### src/sim/dt.ts

- `src/sim/dt.ts#DT` (variable, line 2) — read by 23: `src/sim/agents/lorryTraffic.ts#LorryTraffic.drive`, `src/sim/agents/lorryTraffic.ts#LorryTraffic.mayGo`, `src/sim/agents/lorryTraffic.ts#LorryTraffic.step`, `src/sim/agents/lorryTraffic.ts#LorryTraffic.unloading`, …

### src/sim/host/fleetGrid.ts

- `src/sim/host/fleetGrid.ts#FLEET_CELL` (variable, line 10) — read by 1: `src/sim/host/fleetGrid.ts#FleetGrid.cellsNear`
- `src/sim/host/fleetGrid.ts#FleetCell` (interface, line 17) — read by 2: `src/render/agents.ts#createAgentMeshes.sync`, `src/sim/host/fleetGrid.ts#FleetGrid.cellsNear`
- `src/sim/host/fleetGrid.ts#FleetCell` (interface, line 17) — typeRef by 3: `src/render/agents.ts#createAgentMeshes`, `src/sim/host/simClient.ts#SimClient.parkedCellsIn`, `src/sim/host/fleetGrid.ts#FleetGrid.cellsNear`
- `src/sim/host/fleetGrid.ts#FleetGrid` (class, line 22) — typeRef by 1: `src/sim/host/fleetGrid.ts#FleetGrid.cellsNear`
- `src/sim/host/fleetGrid.ts#FleetGrid.cells` (field, line 23) — read by 1: `src/sim/host/fleetGrid.ts#FleetGrid.cellsNear`
- `src/sim/host/fleetGrid.ts#GroundBox` (interface, line 7) — read by 3: `src/render/agents.ts#createAgentMeshes.depthField`, `src/render/agents.ts#createAgentMeshes.sync`, `src/sim/host/simClient.ts#SimClient.parkedCellsIn`
- `src/sim/host/fleetGrid.ts#GroundBox` (interface, line 7) — typeRef by 3: `src/render/agents.ts#AgentRenderOptions`, `src/render/agents.ts#createAgentMeshes.depthField`, `src/sim/host/simClient.ts#SimClient.parkedCellsIn`
- `src/sim/host/fleetGrid.ts#keyOf` (function, line 14) — call by 1: `src/sim/host/fleetGrid.ts#FleetGrid.cellsNear`

### src/sim/host/protocol.ts

- `src/sim/host/protocol.ts#SimAsk` (type, line 144) — typeRef by 1: `src/sim/host/simClient.ts#SimClient.ask`

### src/sim/host/simAnswers.ts

- `src/sim/host/simAnswers.ts#JunctionAnswer` (interface, line 24) — read by 3: `src/ui/roads/junctionPanel.ts#mountJunctionPanel`, `src/ui/roads/junctionPanel.ts#mountJunctionPanel.set`, `src/ui/roads/junctionPanel.ts#refreshJunctionFlows`

### src/sim/host/simClient.ts

- `src/sim/host/simClient.ts#SimClient` (class, line 84) — typeRef by 6: `src/render/densityCars.ts#createDensityCars.tellWindow`, `src/render/densityCars.ts#DensityCars`, `src/render/agents.ts#AgentMeshes`, `src/render/agents.ts#createAgentMeshes.forgetCast`, …
- `src/sim/host/simClient.ts#SimClient.clock` (field, line 192) — call by 1: `src/render/densityCars.ts#createDensityCars.update`
- `src/sim/host/simClient.ts#SimClient.clock` (field, line 192) — read by 1: `src/render/densityCars.ts#createDensityCars.update`
- `src/sim/host/simClient.ts#SimClient.junction` (method, line 404) — call by 2: `src/ui/roads/junctionPanel.ts#mountJunctionPanel`, `src/ui/roads/junctionPanel.ts#refreshJunctionFlows`
- `src/sim/host/simClient.ts#SimClient.mesoDeltas` (field, line 323) — read by 1: `src/sim/host/simClient.ts#SimClient.takeMeso`
- `src/sim/host/simClient.ts#SimClient.mesoDeltas` (field, line 323) — write by 1: `src/sim/host/simClient.ts#SimClient.takeMeso`
- `src/sim/host/simClient.ts#SimClient.nextAsk` (field, line 104) — read by 1: `src/sim/host/simClient.ts#SimClient.ask`
- `src/sim/host/simClient.ts#SimClient.nextAsk` (field, line 104) — write by 1: `src/sim/host/simClient.ts#SimClient.ask`
- `src/sim/host/simClient.ts#SimClient.onVehicleGone` (field, line 131) — callbackAssign by 1: `src/render/agents.ts#createAgentMeshes.forgetCast`
- `src/sim/host/simClient.ts#SimClient.onVehicleGone` (field, line 131) — implementation by 1: `src/render/agents.ts#createAgentMeshes.forgetCast`
- `src/sim/host/simClient.ts#SimClient.onVehicleGone` (field, line 131) — read by 1: `src/render/agents.ts#createAgentMeshes.forgetCast`
- `src/sim/host/simClient.ts#SimClient.onVehicleGone` (field, line 131) — write by 1: `src/render/agents.ts#createAgentMeshes.forgetCast`
- `src/sim/host/simClient.ts#SimClient.parked` (field, line 114) — read by 1: `src/render/agents.ts#createAgentMeshes.sync`
- `src/sim/host/simClient.ts#SimClient.parkedGrid` (field, line 120) — read by 1: `src/sim/host/simClient.ts#SimClient.parkedCellsIn`
- `src/sim/host/simClient.ts#SimClient.parkedSlack` (field, line 123) — read by 1: `src/sim/host/simClient.ts#SimClient.parkedCellsIn`
- `src/sim/host/simClient.ts#SimClient.pedViews` (field, line 129) — read by 1: `src/render/agents.ts#createAgentMeshes.sync`
- `src/sim/host/simClient.ts#SimClient.pending` (field, line 103) — read by 1: `src/sim/host/simClient.ts#SimClient.ask`
- `src/sim/host/simClient.ts#SimClient.pending` (field, line 103) — write by 1: `src/sim/host/simClient.ts#SimClient.ask`
- `src/sim/host/simClient.ts#SimClient.send` (method, line 423) — call by 1: `src/sim/host/simClient.ts#SimClient.ask`
- `src/sim/host/simClient.ts#SimClient.tick` (field, line 226) — read by 1: `src/render/agents.ts#createAgentMeshes.sync`
- `src/sim/host/simClient.ts#SimClient.time` (field, line 185) — read by 1: `src/render/agents.ts#createAgentMeshes.sync`
- `src/sim/host/simClient.ts#SimClient.traffic` (field, line 113) — read by 1: `src/render/agents.ts#createAgentMeshes.sync`
- `src/sim/host/simClient.ts#SimClient.vehiclesIn` (method, line 138) — call by 1: `src/render/agents.ts#createAgentMeshes.sync`
- `src/sim/host/simClient.ts#SimClient.window` (method, line 308) — call by 1: `src/render/densityCars.ts#createDensityCars.tellWindow`
- `src/sim/host/simClient.ts#VehicleView` (interface, line 28) — read by 16: `src/render/agents.ts#createAgentMeshes.bodyKey`, `src/render/agents.ts#createAgentMeshes.carriesChild`, `src/render/agents.ts#createAgentMeshes.deckHolds`, `src/render/agents.ts#createAgentMeshes.drawCar`, …
- `src/sim/host/simClient.ts#VehicleView` (interface, line 28) — typeRef by 22: `src/render/agents.ts#AgentMeshes`, `src/render/agents.ts#createAgentMeshes`, `src/render/agents.ts#createAgentMeshes.bodyKey`, `src/render/agents.ts#createAgentMeshes.carcass`, …

### src/sim/host/simHost.ts

- `src/sim/host/simHost.ts#SimHost.init` (method, line 113) — callbackAssign by 1: `src/world/conflictPoints.ts#ConflictIndex.prepare`

### src/sim/peds/sidewalk.ts

- `src/sim/peds/sidewalk.ts#SidewalkEdge` (interface, line 30) — read by 7: `src/sim/agents/walk.ts#publish`, `src/sim/intersections/crossingSpans.ts#CrossingSpans.measureJunction`, `src/sim/vehicles/kerbStops.ts#footwayBeside`, `src/sim/crossings/permission.ts#vehicleBodyIntersectsCrossing`, …
- `src/sim/peds/sidewalk.ts#SidewalkEdge` (interface, line 30) — typeRef by 8: `src/sim/agents/walk.ts#Zebra`, `src/sim/peds/sidewalk.ts#SidewalkGraph.walksBeside`, `src/sim/crossings/permission.ts#vehicleBodyIntersectsCrossing`, `src/sim/crossings/permission.ts#mayEnterCrossing`, …
- `src/sim/peds/sidewalk.ts#SidewalkGraph.crossings` (field, line 50) — read by 3: `src/sim/intersections/admission.ts#pedestrianHasPriority`, `src/sim/intersections/crossingSpans.ts#CrossingSpans.measureJunction`, `src/sim/peds/sidewalk.ts#SidewalkGraph.crossingEdge`
- `src/sim/peds/sidewalk.ts#SidewalkGraph.edges` (field, line 49) — read by 3: `src/sim/intersections/admission.ts#pedestrianHasPriority`, `src/sim/intersections/crossingSpans.ts#CrossingSpans.measureJunction`, `src/sim/peds/sidewalk.ts#SidewalkGraph.crossingEdge`
- `src/sim/peds/sidewalk.ts#SidewalkGraph.ofNode` (field, line 55) — read by 1: `src/sim/peds/sidewalk.ts#SidewalkGraph.madeAt`
- `src/sim/peds/sidewalk.ts#SidewalkGraph.walksBySegment` (field, line 52) — read by 1: `src/sim/peds/sidewalk.ts#SidewalkGraph.walksBeside`

### src/sim/peds/signalPostIndex.ts

- `src/sim/peds/signalPostIndex.ts#POST_CELL` (variable, line 17) — read by 3: `src/sim/peds/signalPostIndex.ts#SignalPostIndex.near`, `src/sim/peds/signalPostIndex.ts#SignalPostIndex.lift`, `src/sim/peds/signalPostIndex.ts#SignalPostIndex.file`
- `src/sim/peds/signalPostIndex.ts#SignalPostIndex.byNode` (field, line 23) — read by 3: `src/sim/peds/signalPostIndex.ts#SignalPostIndex.follow`, `src/sim/peds/signalPostIndex.ts#SignalPostIndex.lift`, `src/sim/peds/signalPostIndex.ts#SignalPostIndex.file`
- `src/sim/peds/signalPostIndex.ts#SignalPostIndex.byNode` (field, line 23) — write by 3: `src/sim/peds/signalPostIndex.ts#SignalPostIndex.follow`, `src/sim/peds/signalPostIndex.ts#SignalPostIndex.lift`, `src/sim/peds/signalPostIndex.ts#SignalPostIndex.file`
- `src/sim/peds/signalPostIndex.ts#SignalPostIndex.cells` (field, line 24) — read by 4: `src/sim/peds/signalPostIndex.ts#SignalPostIndex.follow`, `src/sim/peds/signalPostIndex.ts#SignalPostIndex.near`, `src/sim/peds/signalPostIndex.ts#SignalPostIndex.lift`, `src/sim/peds/signalPostIndex.ts#SignalPostIndex.file`
- `src/sim/peds/signalPostIndex.ts#SignalPostIndex.cells` (field, line 24) — write by 3: `src/sim/peds/signalPostIndex.ts#SignalPostIndex.follow`, `src/sim/peds/signalPostIndex.ts#SignalPostIndex.lift`, `src/sim/peds/signalPostIndex.ts#SignalPostIndex.file`
- `src/sim/peds/signalPostIndex.ts#SignalPostIndex.serial` (field, line 22) — read by 1: `src/sim/peds/signalPostIndex.ts#SignalPostIndex.follow`
- `src/sim/peds/signalPostIndex.ts#SignalPostIndex.serial` (field, line 22) — write by 1: `src/sim/peds/signalPostIndex.ts#SignalPostIndex.follow`

### src/sim/people/casualties.ts

- `src/sim/people/casualties.ts#Casualty` (interface, line 8) — typeRef by 1: `src/sim/people/casualties.ts#recordCasualty`
- `src/sim/people/casualties.ts#RECORDS` (variable, line 42) — read by 1: `src/sim/people/casualties.ts#recordCasualty`
- `src/sim/people/casualties.ts#RECORDS` (variable, line 42) — write by 1: `src/sim/people/casualties.ts#recordCasualty`
- `src/sim/people/casualties.ts#WOUNDS` (variable, line 43) — read by 1: `src/sim/people/casualties.ts#recordWound`
- `src/sim/people/casualties.ts#WOUNDS` (variable, line 43) — write by 1: `src/sim/people/casualties.ts#recordWound`

### src/sim/people/engine.ts

- `src/sim/people/engine.ts#Boarder` (interface, line 56) — read by 6: `src/sim/agents/walk.ts#createAgentWalkEngine`, `src/sim/agents/walk.ts#createAgentWalkEngine.walk`, `src/sim/vehicles/kerbStops.ts#completeTransfer`, `src/sim/vehicles/kerbStops.ts#hail`, …
- `src/sim/people/engine.ts#Boarder` (interface, line 56) — typeRef by 1: `src/sim/vehicles/kerbStops.ts#KerbPerson`
- `src/sim/people/engine.ts#PedestrianEngine` (interface, line 8) — read by 9: `src/sim/agents/lorryTraffic.ts#LorryTraffic.mayGo`, `src/sim/agents/lotTraffic.ts#LotTraffic.mayGo`, `src/sim/vehicles/kerbStops.ts#completeTransfer`, `src/sim/vehicles/kerbStops.ts#hail`, …
- `src/sim/people/engine.ts#PedestrianEngine` (interface, line 8) — typeRef by 1: `src/sim/agents/walk.ts#createAgentWalkEngine`
- `src/sim/people/engine.ts#PeopleBridge` (interface, line 68) — call by 9: `src/sim/agents/lorryTraffic.ts#LorryTraffic.mayGo`, `src/sim/agents/lotTraffic.ts#LotTraffic.mayGo`, `src/sim/vehicles/kerbStops.ts#completeTransfer`, `src/sim/vehicles/kerbStops.ts#hail`, …
- `src/sim/people/engine.ts#PeopleBridge` (interface, line 68) — read by 3: `src/sim/agents/walk.ts#createAgentWalkEngine`, `src/sim/vehicles/kerbStops.ts#plan`, `src/sim/vehicles/kerbStops.ts#planBusStop`
- `src/sim/people/engine.ts#PeopleBridge` (interface, line 68) — typeRef by 1: `src/sim/agents/walk.ts#createAgentWalkEngine`
- `src/sim/people/engine.ts#ResidentWalk` (interface, line 38) — read by 1: `src/sim/agents/walk.ts#startWalk`
- `src/sim/people/engine.ts#ResidentWalk` (interface, line 38) — typeRef by 2: `src/sim/agents/walk.ts#startWalk`, `src/sim/agents/walk.ts#walkThrough`

### src/sim/people/view.ts

- `src/sim/people/view.ts#BodyPart` (type, line 53) — typeRef by 13: `src/sim/agents/walk.ts#createAgentWalkEngine`, `src/sim/agents/walk.ts#Walker`, `src/sim/agents/walk.ts#walkerState`, `src/sim/agents/walk.ts#woundOf`, …
- `src/sim/people/view.ts#GestureKind` (type, line 19) — typeRef by 2: `src/sim/agents/walk.ts#Walker`, `src/sim/agents/walk.ts#walkerAct`
- `src/sim/people/view.ts#GestureView` (interface, line 32) — read by 4: `src/render/agents.ts#createAgentMeshes.sync`, `src/render/riggedCitizens.ts#createRiggedCitizens.draw`, `src/render/citizenGait.ts#stepGait`, `src/render/citizenGait.ts#standingKey`
- `src/sim/people/view.ts#GestureView` (interface, line 32) — write by 1: `src/sim/agents/walk.ts#publish`
- `src/sim/people/view.ts#PartyView` (interface, line 11) — read by 9: `src/sim/agents/walk.ts#createAgentWalkEngine`, `src/sim/agents/walk.ts#finish`, `src/sim/agents/walk.ts#joinParty`, `src/sim/agents/walk.ts#mourn`, …
- `src/sim/people/view.ts#PartyView` (interface, line 11) — typeRef by 1: `src/render/riggedCitizens.ts#companyOf`
- `src/sim/people/view.ts#PedView` (interface, line 55) — read by 15: `src/sim/agents/walk.ts#createAgentWalkEngine`, `src/sim/agents/walk.ts#finish`, `src/sim/agents/walk.ts#joinParty`, `src/sim/agents/walk.ts#mourn`, …
- `src/sim/people/view.ts#PedView` (interface, line 55) — typeRef by 12: `src/sim/agents/walk.ts#addPlayerWalker`, `src/sim/agents/walk.ts#startWalk`, `src/sim/agents/walk.ts#Walker`, `src/render/riggedCitizens.ts#createRiggedCitizens.draw`, …
- `src/sim/people/view.ts#PedView` (interface, line 55) — write by 2: `src/sim/agents/walk.ts#publish`, `src/sim/agents/walk.ts#startWalk`
- `src/sim/people/view.ts#PersonAgeClass` (type, line 6) — typeRef by 11: `src/sim/agents/walk.ts#addPlayerWalker`, `src/sim/agents/walk.ts#startWalk`, `src/sim/vehicles/kerbStops.ts#seatAgeClass`, `src/sim/vehicles/kerbStops.ts#Seated`, …
- `src/sim/people/view.ts#PersonGender` (type, line 7) — typeRef by 3: `src/sim/agents/walk.ts#addPlayerWalker`, `src/sim/agents/walk.ts#startWalk`, `src/sim/vehicles/kerbStops.ts#SeatPerson`
- `src/sim/people/view.ts#personHash` (function, line 105) — call by 10: `src/sim/agents/walk.ts#createAgentWalkEngine`, `src/sim/agents/walk.ts#frighten`, `src/sim/agents/walk.ts#mourn`, `src/sim/agents/walk.ts#startWalk`, …
- `src/sim/people/view.ts#Severable` (type, line 51) — typeRef by 10: `src/sim/agents/walk.ts#createAgentWalkEngine`, `src/sim/agents/walk.ts#Walker`, `src/sim/agents/walk.ts#walkerState`, `src/render/agents.ts#createAgentMeshes.procDraw`, …
- `src/sim/people/view.ts#SIT_DOWN_SECONDS` (variable, line 45) — read by 1: `src/render/citizenGait.ts#stepGait`
- `src/sim/people/view.ts#STAND_UP_SECONDS` (variable, line 46) — read by 1: `src/render/citizenGait.ts#stepGait`

### src/sim/pose.ts

- `src/sim/pose.ts#AXLE_NOW` (variable, line 146) — read by 1: `src/sim/pose.ts#vehiclePoseNowInto`
- `src/sim/pose.ts#axleFrame` (function, line 151) — call by 1: `src/sim/pose.ts#vehiclePoseNowInto`
- `src/sim/pose.ts#bodyOffset` (function, line 39) — call by 1: `src/sim/pose.ts#vehiclePoseNowInto`
- `src/sim/pose.ts#framesPoseInto` (function, line 122) — call by 1: `src/render/agents.ts#createAgentMeshes.sync.drawVehicle`
- `src/sim/pose.ts#laneChangeYaw` (function, line 31) — call by 1: `src/sim/pose.ts#vehiclePoseNowInto`
- `src/sim/pose.ts#lerpAngle` (function, line 190) — call by 1: `src/sim/pose.ts#pedPoseInto`
- `src/sim/pose.ts#newPose` (function, line 23) — call by 2: `src/sim/agents/walk.ts#TRAFFIC_POSE`, `src/render/agents.ts#POSE`
- `src/sim/pose.ts#Pose` (interface, line 11) — read by 5: `src/sim/agents/lorryTraffic.ts#LorryTraffic.call`, `src/sim/agents/lorryTraffic.ts#LorryTraffic.coming`, `src/sim/agents/lotTraffic.ts#LotTraffic.call`, `src/sim/agents/lotTraffic.ts#LotTraffic.coming`, …
- `src/sim/pose.ts#POSE_JUMP_LIMIT` (variable, line 28) — read by 1: `src/sim/pose.ts#pedPoseInto`
- `src/sim/pose.ts#PoseOut` (interface, line 17) — read by 5: `src/sim/agents/walk.ts#gapOpen`, `src/render/agents.ts#createAgentMeshes.sync`, `src/render/agents.ts#createAgentMeshes.sync.drawVehicle`, `src/sim/pose.ts#pedPoseInto`, …
- `src/sim/pose.ts#PoseOut` (interface, line 17) — typeRef by 2: `src/sim/pose.ts#pedPoseInto`, `src/sim/pose.ts#vehiclePoseNowInto`
- `src/sim/pose.ts#PoseOut` (interface, line 17) — write by 3: `src/render/agents.ts#createAgentMeshes.sync.drawVehicle`, `src/sim/pose.ts#pedPoseInto`, `src/sim/pose.ts#vehiclePoseNowInto`
- `src/sim/pose.ts#vehiclePose` (function, line 46) — call by 5: `src/sim/agents/lorryTraffic.ts#LorryTraffic.call`, `src/sim/agents/lorryTraffic.ts#LorryTraffic.coming`, `src/sim/agents/lotTraffic.ts#LotTraffic.call`, `src/sim/agents/lotTraffic.ts#LotTraffic.coming`, …

### src/sim/transit/transit.ts

- `src/sim/transit/transit.ts#AHEAD_TIME` (variable, line 33) — read by 2: `src/sim/transit/transit.ts#TransitSim.trainZones`, `src/sim/transit/transit.ts#TransitSim.trainZones.far`
- `src/sim/transit/transit.ts#CROSSING_STOP` (variable, line 35) — read by 2: `src/sim/intersections/admission.ts#admit`, `src/sim/vehicles/obstacles.ts#longitudinalConstraints`
- `src/sim/transit/transit.ts#CROSSING_WARN` (variable, line 39) — read by 1: `src/sim/transit/transit.ts#TransitSim.closedOn`
- `src/sim/transit/transit.ts#KEEP_CLEAR` (variable, line 37) — read by 1: `src/sim/transit/transit.ts#TransitSim.crossingNearEnd`
- `src/sim/transit/transit.ts#LevelCrossing` (interface, line 77) — read by 2: `src/sim/transit/transit.ts#TransitSim.closedOn`, `src/sim/transit/transit.ts#TransitSim.crossingNearEnd`
- `src/sim/transit/transit.ts#NO_CROSSINGS` (variable, line 582) — read by 2: `src/sim/transit/transit.ts#TransitSim.closedOn`, `src/sim/transit/transit.ts#TransitSim.crossingNearEnd`
- `src/sim/transit/transit.ts#NONE` (variable, line 40) — read by 1: `src/sim/transit/transit.ts#TransitSim.closedOn`
- `src/sim/transit/transit.ts#RailLine` (interface, line 79) — read by 2: `src/sim/transit/transit.ts#TransitSim.closedOn`, `src/sim/transit/transit.ts#TransitSim.trainZones`
- `src/sim/transit/transit.ts#RailLine` (interface, line 79) — typeRef by 1: `src/sim/transit/transit.ts#TransitSim.trainZones.far`
- `src/sim/transit/transit.ts#Train` (interface, line 66) — read by 3: `src/sim/transit/transit.ts#TransitSim.closedOn`, `src/sim/transit/transit.ts#TransitSim.trainZones`, `src/sim/transit/transit.ts#TransitSim.trainZones.far`
- `src/sim/transit/transit.ts#Train` (interface, line 66) — typeRef by 1: `src/sim/transit/transit.ts#TransitSim.trainZones.far`
- `src/sim/transit/transit.ts#TRAIN_AHEAD` (variable, line 31) — read by 2: `src/sim/transit/transit.ts#TransitSim.trainZones`, `src/sim/transit/transit.ts#TransitSim.trainZones.far`
- `src/sim/transit/transit.ts#TRAIN_CARS,METRO_CARS,CAR_LENGTH` (variable, line 42) — read by 3: `src/sim/transit/transit.ts#TransitSim.closedOn`, `src/sim/transit/transit.ts#TransitSim.trainZones`, `src/sim/transit/transit.ts#TransitSim.trainZones.far`
- `src/sim/transit/transit.ts#TransitSim.crossingsByLane` (field, line 236) — read by 2: `src/sim/transit/transit.ts#TransitSim.closedOn`, `src/sim/transit/transit.ts#TransitSim.crossingNearEnd`
- `src/sim/transit/transit.ts#TransitSim.pointAt` (method, line 446) — call by 2: `src/sim/transit/transit.ts#TransitSim.trainZones`, `src/sim/transit/transit.ts#TransitSim.trainZones.far`
- `src/sim/transit/transit.ts#TransitSim.pointAt` (method, line 446) — read by 1: `src/sim/transit/transit.ts#TransitSim.trainZones`
- `src/sim/transit/transit.ts#TransitSim.rails` (field, line 109) — read by 1: `src/sim/transit/transit.ts#TransitSim.trainZones`

### src/sim/world.ts

- `src/sim/world.ts#BODY_OFF_LANE` (variable, line 39) — read by 2: `src/sim/world.ts#SimWorld.someVehicleNear`, `src/sim/world.ts#SimWorld.vehiclesNear`
- `src/sim/world.ts#LaneletRuntime` (interface, line 78) — read by 34: `src/sim/intersections/admission.ts#aheadOnMovement`, `src/sim/intersections/admission.ts#movementReservedByOther`, `src/sim/intersections/cycles.ts#cycleFull`, `src/sim/intersections/spillback.ts#hasDownstreamStorage`, …
- `src/sim/world.ts#LaneletRuntime` (interface, line 78) — write by 1: `src/sim/world.ts#SimWorld.addShadow`
- `src/sim/world.ts#NO_PEDESTRIANS` (variable, line 61) — implementation by 9: `src/sim/agents/lorryTraffic.ts#LorryTraffic.mayGo`, `src/sim/agents/lotTraffic.ts#LotTraffic.mayGo`, `src/sim/vehicles/kerbStops.ts#completeTransfer`, `src/sim/vehicles/kerbStops.ts#hail`, …
- `src/sim/world.ts#SimStreams` (interface, line 91) — read by 16: `src/sim/agents/lorryTraffic.ts#LorryTraffic.call`, `src/sim/agents/lorryTraffic.ts#LorryTraffic.drive`, `src/sim/agents/lorryTraffic.ts#LorryTraffic.join`, `src/sim/agents/lorryTraffic.ts#LorryTraffic.sendNow`, …
- `src/sim/world.ts#SimWorld` (class, line 153) — typeRef by 259: `src/sim/agents/lorryDocks.ts#buildYard`, `src/sim/agents/lorryDocks.ts#forgetYard`, `src/sim/agents/lorryDocks.ts#forgetYardsBut`, `src/sim/agents/lorryDocks.ts#keepYard`, …
- `src/sim/world.ts#SimWorld.advisor` (getter, line 222) — call by 1: `src/sim/intersections/admission.ts#rightOfWay`
- `src/sim/world.ts#SimWorld.ambient` (field, line 267) — read by 8: `src/sim/agents/lorryTraffic.ts#LorryTraffic.drop`, `src/sim/agents/lorryTraffic.ts#LorryTraffic.step`, `src/sim/agents/lotTraffic.ts#LotTraffic.coming`, `src/sim/agents/lotTraffic.ts#LotTraffic.depart`, …
- `src/sim/world.ts#SimWorld.asleep` (field, line 199) — read by 1: `src/sim/world.ts#SimWorld.signalAtCrossing`
- `src/sim/world.ts#SimWorld.bodiesIn` (method, line 1261) — call by 9: `src/sim/agents/lotTraffic.ts#LotTraffic.gap`, `src/sim/intersections/admission.ts#exitSwingBlocked`, `src/sim/vehicles/laneChange.ts#canLeaveLane`, `src/sim/vehicles/laneChange.ts#gapIsSafe`, …
- `src/sim/world.ts#SimWorld.bodiesIn` (method, line 1261) — read by 9: `src/sim/agents/lotTraffic.ts#LotTraffic.gap`, `src/sim/intersections/admission.ts#exitSwingBlocked`, `src/sim/vehicles/laneChange.ts#canLeaveLane`, `src/sim/vehicles/laneChange.ts#gapIsSafe`, …
- `src/sim/world.ts#SimWorld.city` (field, line 260) — read by 6: `src/sim/agents/lotTraffic.ts#LotTraffic.hour`, `src/sim/agents/walk.ts#stepWalkers`, `src/sim/intersections/admission.ts#admit`, `src/sim/meso/meso.ts#Meso.begin`, …
- `src/sim/world.ts#SimWorld.claims` (field, line 160) — read by 10: `src/sim/intersections/admission.ts#actualAllocation`, `src/sim/intersections/admission.ts#admit`, `src/sim/intersections/admission.ts#convoyCanEnter`, `src/sim/intersections/admission.ts#evaluate`, …
- `src/sim/world.ts#SimWorld.clearShadow` (method, line 1250) — call by 3: `src/sim/vehicles/integrate.ts#advanceTo`, `src/sim/vehicles/integrate.ts#integrateAll`, `src/sim/world.ts#SimWorld.addShadow`
- `src/sim/world.ts#SimWorld.clock` (field, line 154) — read by 13: `src/sim/agents/lorryTraffic.ts#LorryTraffic.join`, `src/sim/agents/lorryTraffic.ts#LorryTraffic.offRoad`, `src/sim/agents/lorryTraffic.ts#LorryTraffic.sendNow`, `src/sim/agents/lotTraffic.ts#LotTraffic.leaving`, …
- `src/sim/world.ts#SimWorld.completedTrips` (field, line 246) — read by 1: `src/sim/vehicles/spawn.ts#stepDespawn`
- `src/sim/world.ts#SimWorld.completedTrips` (field, line 246) — write by 1: `src/sim/vehicles/spawn.ts#stepDespawn`
- `src/sim/world.ts#SimWorld.conflicts` (field, line 156) — read by 15: `src/sim/intersections/admission.ts#actualAllocation`, `src/sim/intersections/admission.ts#clearTime`, `src/sim/intersections/admission.ts#connectorResources`, `src/sim/intersections/admission.ts#convoyCanEnter`, …
- `src/sim/world.ts#SimWorld.connector` (method, line 371) — call by 35: `src/sim/intersections/admission.ts#BankerPass.holdingOf`, `src/sim/intersections/admission.ts#compactReservationChain`, `src/sim/intersections/admission.ts#evaluate`, `src/sim/intersections/admission.ts#hasAcceptableGap`, …
- `src/sim/world.ts#SimWorld.constructor` (constructor, line 293) — read by 35: `src/sim/agents/lorryDocks.ts#buildYard`, `src/sim/agents/lorryDocks.ts#knownYard`, `src/sim/agents/lorryDocks.ts#lotsNear`, `src/sim/agents/lorryDocks.ts#ownLot`, …
- `src/sim/world.ts#SimWorld.controller` (method, line 395) — call by 7: `src/sim/intersections/admission.ts#movementIsActive`, `src/sim/intersections/admission.ts#pedestrianHasPriority`, `src/sim/intersections/admission.ts#protectedNow`, `src/sim/intersections/admission.ts#revokeStaleGrants`, …
- `src/sim/world.ts#SimWorld.controllers` (field, line 187) — read by 1: `src/sim/roads/greenWave.ts#greenWave`
- `src/sim/world.ts#SimWorld.crossingSpans` (field, line 210) — read by 6: `src/sim/intersections/admission.ts#crossingReachedFirst`, `src/sim/intersections/admission.ts#pedestrianHasPriority`, `src/sim/intersections/crossingSpans.ts#pedestrianInSpan`, `src/sim/vehicles/obstacles.ts#longitudinalConstraints`, …
- `src/sim/world.ts#SimWorld.crossingStates` (field, line 204) — read by 5: `src/sim/agents/walk.ts#publish`, `src/sim/intersections/admission.ts#crossingReachedFirst`, `src/sim/intersections/admission.ts#pedestrianHasPriority`, `src/sim/intersections/crossingSpans.ts#pedestrianInSpan`, …
- `src/sim/world.ts#SimWorld.crossingStates` (field, line 204) — write by 1: `src/sim/agents/walk.ts#publish`
- `src/sim/world.ts#SimWorld.demandMultiplier` (field, line 256) — read by 4: `src/sim/meso/meso.ts#Meso.step`, `src/sim/vehicles/spawn.ts#chosenTraffic`, `src/sim/vehicles/spawn.ts#stepDispatch`, `src/sim/vehicles/spawn.ts#trafficTarget`
- `src/sim/world.ts#SimWorld.doLess` (field, line 189) — read by 1: `src/sim/intersections/admission.ts#admit`
- `src/sim/world.ts#SimWorld.edgeTraffic` (field, line 258) — read by 1: `src/sim/vehicles/spawn.ts#stepDispatch`
- `src/sim/world.ts#SimWorld.enterLanelet` (method, line 1184) — call by 6: `src/sim/agents/lorryTraffic.ts#LorryTraffic.join`, `src/sim/agents/lotTraffic.ts#LotTraffic.leaving`, `src/sim/vehicles/integrate.ts#advanceTo`, `src/sim/vehicles/integrate.ts#applyLaneChange`, …
- `src/sim/world.ts#SimWorld.entryDemand` (field, line 242) — read by 1: `src/sim/vehicles/spawn.ts#stepDispatch`
- `src/sim/world.ts#SimWorld.entryDemand` (field, line 242) — write by 1: `src/sim/vehicles/spawn.ts#stepDispatch`
- `src/sim/world.ts#SimWorld.entryDemandLost` (field, line 244) — read by 1: `src/sim/vehicles/spawn.ts#stepDispatch`
- `src/sim/world.ts#SimWorld.entryDemandLost` (field, line 244) — write by 1: `src/sim/vehicles/spawn.ts#stepDispatch`
- `src/sim/world.ts#SimWorld.exitLanelet` (method, line 1204) — call by 2: `src/sim/vehicles/integrate.ts#advanceTo`, `src/sim/vehicles/integrate.ts#applyLaneChange`
- `src/sim/world.ts#SimWorld.focus` (field, line 273) — read by 1: `src/sim/meso/meso.ts#Meso.step`
- `src/sim/world.ts#SimWorld.followCity` (field, line 254) — read by 1: `src/sim/vehicles/spawn.ts#chosenTraffic`
- `src/sim/world.ts#SimWorld.graph` (field, line 155) — read by 46: `src/sim/agents/lorryDocks.ts#yardKey`, `src/sim/intersections/admission.ts#admit`, `src/sim/intersections/admission.ts#compactReservationChain`, `src/sim/intersections/admission.ts#exitSwingBlocked`, …
- `src/sim/world.ts#SimWorld.junctionHolders` (method, line 434) — call by 3: `src/sim/intersections/admission.ts#revokeStaleGrants`, `src/sim/intersections/admission.ts#stepAdmission`, `src/sim/crossings/permission.ts#indexReservations`
- `src/sim/world.ts#SimWorld.junctionNodesInOrder` (method, line 486) — call by 1: `src/sim/intersections/admission.ts#admit`
- `src/sim/world.ts#SimWorld.junctionOfLane` (method, line 519) — call by 1: `src/sim/world.ts#SimWorld.junctionsApproached`
- `src/sim/world.ts#SimWorld.lanelet` (method, line 361) — call by 72: `src/sim/agents/lorryDocks.ts#Yard.constructor`, `src/sim/agents/lorryTraffic.ts#LorryTraffic.call`, `src/sim/agents/lorryTraffic.ts#LorryTraffic.join`, `src/sim/agents/lorryTraffic.ts#LorryTraffic.sendNow`, …
- `src/sim/world.ts#SimWorld.laneReaches` (field, line 237) — read by 4: `src/sim/meso/meso.ts#Meso.handoffDestination`, `src/sim/routing/destination.ts#reachableExits`, `src/sim/routing/destination.ts#routeToDestination`, `src/sim/drive/tactical.ts#planTrip`
- `src/sim/world.ts#SimWorld.lastAdmission` (field, line 213) — read by 1: `src/sim/intersections/admission.ts#admit`
- `src/sim/world.ts#SimWorld.lastAdmission` (field, line 213) — write by 1: `src/sim/intersections/admission.ts#admit`
- `src/sim/world.ts#SimWorld.makeSignalDeps` (method, line 983) — callbackAssign by 7: `src/sim/signals/fsm.ts#createController`, `src/sim/signals/fsm.ts#pickNextStage`, `src/sim/signals/fsm.ts#rebuildController`, `src/sim/signals/fsm.ts#safePlan`, …
- `src/sim/world.ts#SimWorld.makeSignalDeps` (method, line 983) — implementation by 7: `src/sim/signals/fsm.ts#createController`, `src/sim/signals/fsm.ts#pickNextStage`, `src/sim/signals/fsm.ts#rebuildController`, `src/sim/signals/fsm.ts#safePlan`, …
- `src/sim/world.ts#SimWorld.mergeTurn` (field, line 215) — read by 2: `src/sim/intersections/admission.ts#admit`, `src/sim/intersections/admission.ts#zipperHolds`
- `src/sim/world.ts#SimWorld.mergeTurn` (field, line 215) — write by 1: `src/sim/intersections/admission.ts#admit`
- `src/sim/world.ts#SimWorld.meso` (field, line 269) — read by 1: `src/sim/vehicles/spawn.ts#stepDispatch`
- `src/sim/world.ts#SimWorld.nextVehicleId` (field, line 229) — read by 5: `src/sim/agents/lorryTraffic.ts#LorryTraffic.join`, `src/sim/agents/lorryTraffic.ts#LorryTraffic.sendNow`, `src/sim/agents/lotTraffic.ts#LotTraffic.leaving`, `src/sim/vehicles/spawn.ts#spawnAt`, …
- `src/sim/world.ts#SimWorld.nextVehicleId` (field, line 229) — write by 5: `src/sim/agents/lorryTraffic.ts#LorryTraffic.join`, `src/sim/agents/lorryTraffic.ts#LorryTraffic.sendNow`, `src/sim/agents/lotTraffic.ts#LotTraffic.leaving`, `src/sim/vehicles/spawn.ts#spawnAt`, …
- `src/sim/world.ts#SimWorld.noteHolder` (method, line 429) — call by 2: `src/sim/intersections/admission.ts#admit`, `src/sim/vehicles/integrate.ts#advanceTo`
- `src/sim/world.ts#SimWorld.noteRoute` (method, line 400) — call by 7: `src/sim/routing/router.ts#cutAtBreak`, `src/sim/routing/router.ts#extend`, `src/sim/routing/router.ts#planFrom`, `src/sim/routing/router.ts#repairRoute`, …
- `src/sim/world.ts#SimWorld.occupied` (field, line 191) — read by 4: `src/sim/meso/meso.ts#Meso.countMicro`, `src/sim/vehicles/integrate.ts#integrateAll`, `src/sim/vehicles/spawn.ts#stepDespawn`, `src/sim/world.ts#SimWorld.junctionsApproached`
- `src/sim/world.ts#SimWorld.pedEngine` (field, line 182) — read by 9: `src/sim/agents/lorryTraffic.ts#LorryTraffic.mayGo`, `src/sim/agents/lotTraffic.ts#LotTraffic.mayGo`, `src/sim/vehicles/kerbStops.ts#completeTransfer`, `src/sim/vehicles/kerbStops.ts#hail`, …
- `src/sim/world.ts#SimWorld.pedViewById` (field, line 208) — read by 1: `src/sim/agents/walk.ts#publish`
- `src/sim/world.ts#SimWorld.pedViews` (field, line 206) — read by 1: `src/sim/agents/walk.ts#publish`
- `src/sim/world.ts#SimWorld.populationShare` (field, line 271) — read by 2: `src/sim/meso/meso.ts#Meso.admit`, `src/sim/vehicles/spawn.ts#trafficTarget`
- `src/sim/world.ts#SimWorld.removeVehicle` (method, line 1286) — call by 8: `src/sim/agents/lorryTraffic.ts#LorryTraffic.coming`, `src/sim/agents/lotTraffic.ts#LotTraffic.coming`, `src/sim/meso/meso.ts#Meso.leave`, `src/sim/meso/meso.ts#Meso.takeBack`, …
- `src/sim/world.ts#SimWorld.rng` (getter, line 226) — call by 16: `src/sim/agents/lorryTraffic.ts#LorryTraffic.call`, `src/sim/agents/lorryTraffic.ts#LorryTraffic.drive`, `src/sim/agents/lorryTraffic.ts#LorryTraffic.join`, `src/sim/agents/lorryTraffic.ts#LorryTraffic.sendNow`, …
- `src/sim/world.ts#SimWorld.rt` (method, line 376) — call by 30: `src/sim/intersections/admission.ts#aheadOnMovement`, `src/sim/intersections/cycles.ts#cycleFull`, `src/sim/intersections/spillback.ts#hasDownstreamStorage`, `src/sim/meso/meso.ts#Meso.countMicro`, …
- `src/sim/world.ts#SimWorld.runtime` (field, line 186) — read by 4: `src/sim/intersections/admission.ts#movementReservedByOther`, `src/sim/intersections/spillback.ts#hasDownstreamStorage`, `src/sim/vehicles/integrate.ts#integrateAll`, `src/sim/vehicles/spawn.ts#entryLanes`
- `src/sim/world.ts#SimWorld.runtimeFor` (method, line 381) — call by 1: `src/sim/world.ts#SimWorld.addShadow`
- `src/sim/world.ts#SimWorld.sidewalks` (field, line 157) — read by 4: `src/sim/agents/walk.ts#crossingOf`, `src/sim/intersections/admission.ts#pedestrianHasPriority`, `src/sim/intersections/crossingSpans.ts#CrossingSpans.measureJunction`, `src/sim/vehicles/kerbStops.ts#footwayBeside`
- `src/sim/world.ts#SimWorld.signalPosts` (field, line 159) — read by 1: `src/sim/agents/walk.ts#stepWalkers`
- `src/sim/world.ts#SimWorld.sortLane` (method, line 1213) — call by 1: `src/sim/vehicles/integrate.ts#integrateAll`
- `src/sim/world.ts#SimWorld.topologyLog` (field, line 700) — read by 5: `src/sim/intersections/cycles.ts#indexOf`, `src/sim/meso/meso.ts#Meso.sync`, `src/sim/routing/router.ts#TrapIndex.follow`, `src/sim/vehicles/obstacles.ts#crossingIdsAt`, …
- `src/sim/world.ts#SimWorld.topologyRevision` (field, line 286) — read by 1: `src/sim/agents/lorryDocks.ts#yardKey`
- `src/sim/world.ts#SimWorld.trafficCount` (field, line 251) — read by 2: `src/sim/vehicles/spawn.ts#chosenTraffic`, `src/sim/vehicles/spawn.ts#trafficTarget`
- `src/sim/world.ts#SimWorld.trafficIntensity` (field, line 248) — read by 4: `src/sim/meso/meso.ts#Meso.step`, `src/sim/vehicles/spawn.ts#chosenTraffic`, `src/sim/vehicles/spawn.ts#stepDispatch`, `src/sim/vehicles/spawn.ts#trafficTarget`
- `src/sim/world.ts#SimWorld.veh` (method, line 391) — call by 20: `src/sim/intersections/admission.ts#aheadOnMovement`, `src/sim/intersections/admission.ts#convoyCanEnter`, `src/sim/intersections/admission.ts#holderState`, `src/sim/intersections/admission.ts#mergeFollows`, …
- `src/sim/world.ts#SimWorld.vehicles` (field, line 163) — read by 18: `src/sim/agents/lorryTraffic.ts#LorryTraffic.coming`, `src/sim/agents/lorryTraffic.ts#LorryTraffic.join`, `src/sim/agents/lotTraffic.ts#LotTraffic.coming`, `src/sim/agents/lotTraffic.ts#LotTraffic.depart`, …
- `src/sim/world.ts#SimWorld.vehicles` (field, line 163) — write by 4: `src/sim/agents/lorryTraffic.ts#LorryTraffic.join`, `src/sim/agents/lotTraffic.ts#LotTraffic.leaving`, `src/sim/vehicles/spawn.ts#spawnAt`, `src/sim/vehicles/spawn.ts#spawnVehicleAt`
- `src/sim/world.ts#SimWorld.vehiclesInIdOrder` (method, line 481) — call by 3: `src/sim/vehicles/integrate.ts#integrateAll`, `src/sim/vehicles/kerbStops.ts#stepKerbStops`, `src/sim/vehicles/laneChange.ts#decideLaneChanges`
- `src/sim/world.ts#SimWorld.vehicleSpawnClock` (field, line 239) — read by 1: `src/sim/vehicles/spawn.ts#stepDispatch`
- `src/sim/world.ts#SimWorld.vehicleSpawnClock` (field, line 239) — write by 1: `src/sim/vehicles/spawn.ts#stepDispatch`
- `src/sim/world.ts#SimWorld.wakeSignal` (method, line 601) — call by 1: `src/sim/world.ts#SimWorld.signalAtCrossing`
- `src/sim/world.ts#SimWorld.walkwaysNow` (method, line 685) — call by 1: `src/sim/agents/walk.ts#ensureGraph`
- `src/sim/world.ts#VersionedMap.set` (method, line 1349) — call by 4: `src/sim/agents/lorryTraffic.ts#LorryTraffic.join`, `src/sim/agents/lotTraffic.ts#LotTraffic.leaving`, `src/sim/vehicles/spawn.ts#spawnAt`, `src/sim/vehicles/spawn.ts#spawnVehicleAt`

### src/ui/i18n/index.ts

- `src/ui/i18n/index.ts#formatDecimal` (function, line 79) — call by 1: `src/ui/roads/junctionPanel.ts#fmt`
- `src/ui/i18n/index.ts#t` (function, line 55) — call by 4: `src/ui/roads/junctionPanel.ts#compassOf`, `src/ui/roads/junctionPanel.ts#mountJunctionPanel`, `src/ui/roads/junctionPanel.ts#moveChip`, `src/ui/roads/junctionPanel.ts#refreshJunctionFlows`

### src/ui/inspector.ts

- `src/ui/inspector.ts#actionsForNode` (function, line 463) — call by 6: `src/ui/inspector.ts#junctionHost.setMovementBlocked`, `src/ui/inspector.ts#junctionHost.setSignal`, `src/ui/inspector.ts#junctionHost.setControl`, `src/ui/inspector.ts#junctionHost.setHeight`, …
- `src/ui/inspector.ts#InspectorActions` (interface, line 28) — call by 6: `src/ui/inspector.ts#junctionHost.setMovementBlocked`, `src/ui/inspector.ts#junctionHost.setSignal`, `src/ui/inspector.ts#junctionHost.setControl`, `src/ui/inspector.ts#junctionHost.setHeight`, …
- `src/ui/inspector.ts#junctionHost` (function, line 447) — callbackAssign by 1: `src/ui/roads/junctionPanel.ts#mountJunctionPanel`
- `src/ui/inspector.ts#junctionHost` (function, line 447) — implementation by 1: `src/ui/roads/junctionPanel.ts#mountJunctionPanel`
- `src/ui/inspector.ts#junctionHost` (function, line 447) — read by 5: `src/ui/inspector.ts#junctionHost.setMovementBlocked`, `src/ui/inspector.ts#junctionHost.setSignal`, `src/ui/inspector.ts#junctionHost.setControl`, `src/ui/inspector.ts#junctionHost.setHeight`, …

### src/world/approach.ts

- `src/world/approach.ts#CROSSWALK_DEPTH` (variable, line 7) — read by 3: `src/sim/intersections/crossingSpans.ts#measure`, `src/sim/intersections/crossingSpans.ts#PED_CROSSING_STOP_BUFFER`, `src/world/parkingLayout.ts#endClear`

### src/world/approachGroups.ts

- `src/world/approachGroups.ts#ApproachGroup` (interface, line 5) — read by 6: `src/sim/signals/fsm.ts#safePlan`, `src/sim/signals/plan.ts#annotate`, `src/sim/signals/plan.ts#buildStages`, `src/sim/signals/plan.ts#conflictFreeStages`, …
- `src/world/approachGroups.ts#ApproachGroup` (interface, line 5) — typeRef by 4: `src/world/conflictTable.ts#JunctionAnswer`, `src/world/lanelets.ts#JunctionCache`, `src/world/lanelets.ts#JunctionTopology`, `src/world/lanelets.ts#PendingJunction`

### src/world/buildingGates.ts

- `src/world/buildingGates.ts#entrancesOf` (variable, line 13) — call by 2: `src/sim/agents/trafficBuildings.ts#project`, `src/world/parkingLayout.ts#entrancesNear`

### src/world/buildings/cameraSolids.ts

- `src/world/buildings/cameraSolids.ts#SolidFiling.constructor` (constructor, line 205) — callbackAssign by 4: `src/sim/agents/buildingOrder.ts#StoreOrder.constructor`, `src/sim/meso/demand.ts#DemandCells.constructor`, `src/sim/ambient/ambientIndex.ts#BuildingMix.constructor`, `src/sim/ambient/ambientIndex.ts#SightWalls.constructor`

### src/world/buildings/decayWatch.ts

- `src/world/buildings/decayWatch.ts#createDecayWatch` (function, line 8) — callbackAssign by 4: `src/sim/agents/buildingOrder.ts#StoreOrder.constructor`, `src/sim/meso/demand.ts#DemandCells.constructor`, `src/sim/ambient/ambientIndex.ts#BuildingMix.constructor`, `src/sim/ambient/ambientIndex.ts#SightWalls.constructor`

### src/world/buildings/footprints.ts

- `src/world/buildings/footprints.ts#localFootprint` (function, line 17) — call by 1: `src/world/buildings/streetDoors.ts#streetDoors`

### src/world/buildings/geometry.ts

- `src/world/buildings/geometry.ts#GB_X,GB_Y,GB_NX,GB_NY,GB_Z,GB_SIDE,GB_WIDTH` (variable, line 615) — read by 1: `src/world/buildings/streetDoors.ts#streetDoors`
- `src/world/buildings/geometry.ts#GROUND_BAY_STRIDE` (variable, line 614) — read by 1: `src/world/buildings/streetDoors.ts#streetDoors`
- `src/world/buildings/geometry.ts#groundBayComponent` (function, line 647) — call by 1: `src/world/buildings/streetDoors.ts#streetDoors`
- `src/world/buildings/geometry.ts#groundBays` (function, line 623) — call by 1: `src/world/buildings/streetDoors.ts#streetDoors`
- `src/world/buildings/geometry.ts#localDirToWorld` (function, line 73) — call by 1: `src/sim/agents/lotBays.ts#bayRows`
- `src/world/buildings/geometry.ts#localToWorld` (function, line 32) — call by 10: `src/sim/agents/lorryDocks.ts#buildYard`, `src/sim/agents/lorryDocks.ts#Yard.constructor`, `src/sim/agents/lorryDocks.ts#Yard.toWorld`, `src/sim/agents/lorryDocks.ts#yardBox`, …
- `src/world/buildings/geometry.ts#solidFootprints` (function, line 171) — call by 2: `src/sim/agents/trafficBuildings.ts#project`, `src/world/buildings/docks.ts#docksOf`
- `src/world/buildings/geometry.ts#storedBounds` (function, line 277) — call by 1: `src/sim/agents/trafficBuildings.ts#project`
- `src/world/buildings/geometry.ts#worldToLocal` (function, line 59) — call by 6: `src/sim/agents/lorryDocks.ts#buildYard`, `src/sim/agents/lorryDocks.ts#Yard.constructor`, `src/sim/agents/lotBays.ts#bayRows`, `src/sim/agents/lotDoors.ts#workOut`, …

### src/world/buildings/pads.ts

- `src/world/buildings/pads.ts#townPads` (function, line 184) — callbackAssign by 4: `src/sim/agents/buildingOrder.ts#StoreOrder.constructor`, `src/sim/meso/demand.ts#DemandCells.constructor`, `src/sim/ambient/ambientIndex.ts#BuildingMix.constructor`, `src/sim/ambient/ambientIndex.ts#SightWalls.constructor`

### src/world/buildings/recordTable.ts

- `src/world/buildings/recordTable.ts#eachRecord` (function, line 508) — call by 1: `src/sim/agents/trafficBuildings.ts#trafficChangesOf`
- `src/world/buildings/recordTable.ts#RecordRef` (interface, line 468) — read by 2: `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.keepRef`, `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.load`
- `src/world/buildings/recordTable.ts#RecordRef` (interface, line 468) — typeRef by 5: `src/sim/agents/trafficBuildings.ts#BuildingChange`, `src/sim/agents/trafficBuildings.ts#BuildingSent`, `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.keepRef`, `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.place`, …
- `src/world/buildings/recordTable.ts#RecordTable` (class, line 302) — construct by 4: `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.load`, `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.record`, `src/world/buildings/sections.ts#eachSectionRef`, `src/world/buildings/sections.ts#sectionsSize.rows`
- `src/world/buildings/recordTable.ts#RecordTable.ids` (field, line 304) — read by 1: `src/world/buildings/sections.ts#eachSectionRef`
- `src/world/buildings/recordTable.ts#RecordTable.reader` (method, line 330) — call by 2: `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.load`, `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.record`
- `src/world/buildings/recordTable.ts#RecordTable.rows` (field, line 303) — read by 2: `src/world/buildings/sections.ts#eachSectionRef`, `src/world/buildings/sections.ts#sectionsSize.rows`
- `src/world/buildings/recordTable.ts#refOf` (function, line 493) — call by 1: `src/sim/agents/trafficBuildings.ts#buildingChangesOf`
- `src/world/buildings/recordTable.ts#TableReader` (class, line 336) — typeRef by 2: `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.load`, `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.readers`
- `src/world/buildings/recordTable.ts#TableReader.record` (method, line 377) — call by 2: `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.load`, `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.record`

### src/world/buildings/sections.ts

- `src/world/buildings/sections.ts#BuildingSections` (interface, line 7) — read by 2: `src/world/buildings/sections.ts#eachSectionRef`, `src/world/buildings/sections.ts#sectionsSize`
- `src/world/buildings/sections.ts#BuildingSections` (interface, line 7) — typeRef by 4: `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.load`, `src/sim/agents/trafficBuildings.ts#TrafficMap`, `src/world/buildings/sections.ts#eachSectionRef`, `src/world/buildings/sections.ts#sectionsSize`

### src/world/buildings/siteGrading.ts

- `src/world/buildings/siteGrading.ts#siteGrading` (function, line 33) — callbackAssign by 4: `src/sim/agents/buildingOrder.ts#StoreOrder.constructor`, `src/sim/meso/demand.ts#DemandCells.constructor`, `src/sim/ambient/ambientIndex.ts#BuildingMix.constructor`, `src/sim/ambient/ambientIndex.ts#SightWalls.constructor`

### src/world/buildings/store.ts

- `src/world/buildings/store.ts#BuildingStore.all` (method, line 123) — call by 4: `src/sim/agents/trafficBuildings.ts#docBuildings.all`, `src/sim/agents/trafficBuildings.ts#docBuildings.eachEntrances`, `src/sim/agents/trafficBuildings.ts#docBuildings.eachFacts`, `src/sim/agents/trafficBuildings.ts#docBuildings.eachFootprints`
- `src/world/buildings/store.ts#BuildingStore.get` (method, line 115) — call by 5: `src/sim/agents/trafficBuildings.ts#buildingChangesOf`, `src/sim/agents/trafficBuildings.ts#docBuildings.entrances`, `src/sim/agents/trafficBuildings.ts#docBuildings.facts`, `src/sim/agents/trafficBuildings.ts#docBuildings.footprints`, …
- `src/world/buildings/store.ts#BuildingStore.idList` (method, line 128) — call by 1: `src/sim/agents/trafficBuildings.ts#docBuildings.ids`
- `src/world/buildings/store.ts#BuildingStore.near` (method, line 133) — call by 3: `src/sim/agents/trafficBuildings.ts#docBuildings.near`, `src/sim/agents/trafficBuildings.ts#docBuildings.nearIds`, `src/world/parkingLayout.ts#entrancesNear`
- `src/world/buildings/store.ts#BuildingStore.nearAny` (method, line 140) — call by 1: `src/sim/agents/trafficBuildings.ts#docBuildings.nearAny`
- `src/world/buildings/store.ts#BuildingStore.sections` (method, line 303) — call by 1: `src/sim/agents/trafficBuildings.ts#trafficMapOf`

### src/world/buildings/types.ts

- `src/world/buildings/types.ts#BayComponent` (type, line 40) — typeRef by 1: `src/world/buildings/streetDoors.ts#PEOPLE_DOORS`
- `src/world/buildings/types.ts#Building` (interface, line 284) — read by 8: `src/sim/agents/trafficBuildings.ts#docBuildings.eachEntrances`, `src/sim/agents/trafficBuildings.ts#docBuildings.eachFootprints`, `src/sim/agents/trafficBuildings.ts#docBuildings.nearAny`, `src/sim/agents/trafficBuildings.ts#docBuildings.nearIds`, …
- `src/world/buildings/types.ts#Building` (interface, line 284) — typeRef by 10: `src/sim/agents/trafficBuildings.ts#docBuildings`, `src/sim/agents/trafficBuildings.ts#docBuildings.factsOfRecord`, `src/sim/agents/trafficBuildings.ts#docBuildings.trafficBuilding`, `src/sim/agents/trafficBuildings.ts#project`, …
- `src/world/buildings/types.ts#BuildingElement` (interface, line 259) — read by 17: `src/sim/agents/lorryDocks.ts#buildYard`, `src/sim/agents/lorryDocks.ts#elementRing`, `src/sim/agents/lorryDocks.ts#nearestGate`, `src/sim/agents/lotBays.ts#bayRows`, …
- `src/world/buildings/types.ts#BuildingElement` (interface, line 259) — typeRef by 6: `src/sim/agents/lotBays.ts#carGates`, `src/sim/agents/lotBays.ts#personGates`, `src/sim/agents/lotNav.ts#LotPart`, `src/sim/agents/trafficBuildings.ts#partOf`, …
- `src/world/buildings/types.ts#BuildingFunction` (type, line 214) — typeRef by 1: `src/sim/agents/trafficBuildings.ts#TrafficBuilding`
- `src/world/buildings/types.ts#BuildingId` (type, line 9) — typeRef by 67: `src/sim/agents/bay.ts#Bay`, `src/sim/agents/buildingOrder.ts#BuildingDelta`, `src/sim/agents/buildingOrder.ts#BuildingsInOrder.file`, `src/sim/agents/buildingOrder.ts#BuildingsInOrder.filedAt`, …
- `src/world/buildings/types.ts#BuildingUse` (type, line 16) — typeRef by 1: `src/sim/agents/trafficBuildings.ts#TrafficBuilding`
- `src/world/buildings/types.ts#ElementKind` (type, line 256) — typeRef by 4: `src/sim/agents/lorryDocks.ts#SOLID`, `src/sim/agents/lotNav.ts#LotGridOptions`, `src/sim/agents/lotNav.ts#PEOPLE_SOLID`, `src/sim/agents/lotNav.ts#SOLID`
- `src/world/buildings/types.ts#Volume` (interface, line 150) — read by 11: `src/sim/agents/lorryDocks.ts#buildYard`, `src/sim/agents/lorryDocks.ts#yardInputs`, `src/sim/agents/lotBays.ts#bayRows`, `src/sim/agents/lotBays.ts#gatedProperty`, …
- `src/world/buildings/types.ts#Volume` (interface, line 150) — typeRef by 2: `src/sim/agents/lotBays.ts#LotBlock`, `src/sim/agents/trafficBuildings.ts#TrafficBlock`

### src/world/cells.ts

- `src/world/cells.ts#CELL` (variable, line 9) — read by 3: `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.eachCell`, `src/sim/meso/meso.ts#cellOfPoint`, `src/sim/meso/meso.ts#Meso.slotsIn`
- `src/world/cells.ts#CellFiling.idsMeeting` (method, line 131) — call by 1: `src/sim/agents/lorryDocks.ts#asphaltNear`

### src/world/changes.ts

- `src/world/changes.ts#Change` (interface, line 35) — read by 5: `src/sim/agents/buildingOrder.ts#BuildingsInOrder.update`, `src/sim/agents/buildingOrder.ts#StoreOrder.follow`, `src/sim/ambient/ambientIndex.ts#buildingsChangedSince`, `src/sim/ambient/lotPlaces.ts#LotPlaces.sync`, …
- `src/world/changes.ts#ChangeJournal` (class, line 156) — typeRef by 7: `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.constructor`, `src/sim/meso/demand.ts#DemandCells.sync`, `src/sim/ambient/ambientIndex.ts#BuildingMix.sync`, `src/sim/ambient/ambientIndex.ts#buildingsChangedSince`, …
- `src/world/changes.ts#ChangeJournal.follow` (method, line 162) — call by 2: `src/sim/agents/buildingOrder.ts#BuildingsInOrder.update`, `src/world/derived.ts#ChangeSweep.constructor`
- `src/world/changes.ts#ChangeJournal.record` (method, line 263) — call by 2: `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.apply`, `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.load`
- `src/world/changes.ts#ChangeJournal.serialOf` (method, line 203) — call by 3: `src/sim/agents/buildingOrder.ts#BuildingsInOrder.update`, `src/sim/agents/buildingOrder.ts#StoreOrder.follow`, `src/world/derived.ts#ChangeSweep.advance`
- `src/world/changes.ts#ChangeJournal.since` (method, line 309) — call by 5: `src/sim/agents/buildingOrder.ts#BuildingsInOrder.update`, `src/sim/agents/buildingOrder.ts#StoreOrder.follow`, `src/sim/ambient/ambientIndex.ts#buildingsChangedSince`, `src/sim/ambient/lotPlaces.ts#LotPlaces.sync`, …
- `src/world/changes.ts#ChangeJournal.touches` (method, line 331) — call by 2: `src/sim/agents/lorryDocks.ts#knownYard`, `src/world/derived.ts#ChangeSweep.touches`
- `src/world/changes.ts#ChangeJournal.version` (getter, line 198) — call by 11: `src/sim/agents/buildingOrder.ts#BuildingsInOrder.update`, `src/sim/agents/buildingOrder.ts#StoreOrder.follow`, `src/sim/agents/lorryDocks.ts#buildYard`, `src/sim/agents/lorryDocks.ts#knownYard`, …
- `src/world/changes.ts#ChangeKind` (type, line 27) — typeRef by 4: `src/sim/agents/buildingOrder.ts#BuildingsInOrder.kinds`, `src/sim/agents/lorryDocks.ts#NEAR_KINDS`, `src/render/agents.ts#OFF_ROAD_GROUND`, `src/world/derived.ts#ChangeSweep.constructor`
- `src/world/changes.ts#ChangeRect` (type, line 4) — typeRef by 20: `src/sim/agents/buildingOrder.ts#BuildingDelta`, `src/sim/agents/buildingOrder.ts#BuildingsInOrder.update`, `src/sim/agents/laneGrid.ts#boxOf`, `src/sim/agents/laneGrid.ts#laneChangesSince`, …
- `src/world/changes.ts#JournalReader` (interface, line 68) — implements by 2: `src/sim/agents/buildingOrder.ts#BuildingsInOrder`, `src/world/derived.ts#ChangeSweep`

### src/world/curbRamps.ts

- `src/world/curbRamps.ts#walkingRise` (function, line 174) — call by 1: `src/render/agents.ts#createAgentMeshes.sync`

### src/world/derived.ts

- `src/world/derived.ts#SWEEP_CELL` (variable, line 123) — read by 2: `src/world/derived.ts#ChangeSweep.advance`, `src/world/derived.ts#ChangeSweep.touches`
- `src/world/derived.ts#SWEEP_MAX_CELLS` (variable, line 125) — read by 1: `src/world/derived.ts#ChangeSweep.advance`

### src/world/doc.ts

- `src/world/doc.ts#JunctionControl` (type, line 77) — typeRef by 4: `src/world/lanelets.ts#JunctionTopology`, `src/ui/roads/junctionPanel.ts#CONTROLS`, `src/ui/roads/junctionPanel.ts#JunctionPanelHost`, `src/ui/inspector.ts#junctionHost.setControl`
- `src/world/doc.ts#NodeCrossing` (interface, line 119) — read by 1: `src/world/lanelets.ts#LaneletGraph.buildSegmentLinks`
- `src/world/doc.ts#RoadDoc` (class, line 153) — typeRef by 35: `src/sim/agents/buildingOrder.ts#BuildingsInOrder.doc`, `src/sim/agents/buildingOrder.ts#StoreOrder.doc`, `src/sim/agents/kerbBays.ts#KerbWorld`, `src/sim/agents/trafficBuildings.ts#buildingChangesOf`, …
- `src/world/doc.ts#RoadDoc.buildings` (field, line 289) — read by 15: `src/sim/agents/trafficBuildings.ts#buildingChangesOf`, `src/sim/agents/trafficBuildings.ts#docBuildings.all`, `src/sim/agents/trafficBuildings.ts#docBuildings.eachEntrances`, `src/sim/agents/trafficBuildings.ts#docBuildings.eachFacts`, …
- `src/world/doc.ts#RoadDoc.changes` (field, line 262) — read by 9: `src/sim/agents/buildingOrder.ts#BuildingsInOrder.update`, `src/sim/agents/buildingOrder.ts#StoreOrder.follow`, `src/sim/agents/lorryDocks.ts#buildYard`, `src/sim/agents/lorryDocks.ts#knownYard`, …
- `src/world/doc.ts#RoadDoc.degree` (method, line 686) — call by 4: `src/sim/vehicles/spawn.ts#spawnAt`, `src/world/lanelets.ts#LaneletGraph.buildSegmentLinks`, `src/world/parkingLayout.ts#endClear`, `src/world/surfaceParts.ts#surfaceParts.road`
- `src/world/doc.ts#RoadDoc.lotsIn` (method, line 321) — call by 1: `src/sim/agents/lorryDocks.ts#lotsNear`
- `src/world/doc.ts#RoadDoc.lotStore` (field, line 297) — read by 3: `src/sim/agents/lorryDocks.ts#ownLot`, `src/sim/agents/trafficBuildings.ts#takeTrafficMap`, `src/sim/agents/trafficBuildings.ts#trafficMapOf`
- `src/world/doc.ts#RoadDoc.mapEdge` (method, line 703) — call by 5: `src/sim/routing/laneReach.ts#boundaryExit`, `src/sim/vehicles/spawn.ts#isEntry`, `src/sim/vehicles/spawn.ts#stepDespawn`, `src/world/lanelets.ts#LaneletGraph.buildSegmentLinks`, …
- `src/world/doc.ts#RoadDoc.node` (method, line 416) — call by 26: `src/sim/agents/parking.ts#streetTile`, `src/sim/intersections/admission.ts#continues`, `src/sim/intersections/admission.ts#crossingLegs`, `src/sim/intersections/admission.ts#crossingReachedFirst`, …
- `src/world/doc.ts#RoadDoc.nodes` (field, line 154) — read by 3: `src/sim/agents/trafficBuildings.ts#trafficMapOf`, `src/world/lanelets.ts#LaneletGraph.buildJunctions`, `src/world/lanelets.ts#LaneletGraph.junctionsOffPage`
- `src/world/doc.ts#RoadDoc.replaceFromJSON` (method, line 2053) — call by 1: `src/sim/agents/trafficBuildings.ts#takeTrafficMap`
- `src/world/doc.ts#RoadDoc.requireNode` (method, line 424) — call by 1: `src/world/lanelets.ts#LaneletGraph.junctionInput`
- `src/world/doc.ts#RoadDoc.requireSegment` (method, line 430) — call by 2: `src/world/lanelets.ts#LaneletGraph.buildSegmentLinks`, `src/world/lanelets.ts#LaneletGraph.junctionInput`
- `src/world/doc.ts#RoadDoc.segment` (method, line 420) — call by 20: `src/sim/agents/parking.ts#bothWays`, `src/sim/agents/parking.ts#streetTile`, `src/sim/intersections/admission.ts#throughRank`, `src/sim/meso/meso.ts#Meso.relink`, …
- `src/world/doc.ts#RoadDoc.segments` (field, line 155) — read by 5: `src/sim/agents/trafficBuildings.ts#trafficMapOf`, `src/world/lanelets.ts#LaneletGraph.buildLinks`, `src/world/lanelets.ts#LaneletGraph.linksOnly`, `src/world/surfaceParts.ts#surfaceParts`, …
- `src/world/doc.ts#RoadDoc.transit` (field, line 187) — read by 1: `src/sim/agents/trafficBuildings.ts#trafficMapOf`
- `src/world/doc.ts#RoadNode` (interface, line 89) — read by 29: `src/sim/intersections/admission.ts#continues`, `src/sim/intersections/admission.ts#crossingLegs`, `src/sim/intersections/admission.ts#crossingReachedFirst`, `src/sim/intersections/admission.ts#mergeFollows`, …
- `src/world/doc.ts#RoadNode` (interface, line 89) — typeRef by 9: `src/world/conflictTable.ts#JunctionNode`, `src/world/lanelets.ts#LaneletGraph.buildJunction`, `src/world/lanelets.ts#LaneletGraph.junctionInput`, `src/world/lanelets.ts#LaneletGraph.junctionsOffPage`, …
- `src/world/doc.ts#RoadSegment` (interface, line 124) — read by 20: `src/sim/agents/parking.ts#bothWays`, `src/sim/agents/parking.ts#streetTile`, `src/sim/intersections/admission.ts#throughRank`, `src/sim/meso/meso.ts#Meso.relink`, …
- `src/world/doc.ts#RoadSegment` (interface, line 124) — typeRef by 1: `src/world/conflictTable.ts#JunctionLeg`
- `src/world/doc.ts#savedNode` (function, line 3650) — reference by 1: `src/sim/agents/trafficBuildings.ts#trafficMapOf`
- `src/world/doc.ts#savedSegment` (function, line 3672) — reference by 1: `src/sim/agents/trafficBuildings.ts#trafficMapOf`
- `src/world/doc.ts#SerializedDoc` (interface, line 3181) — typeRef by 1: `src/sim/agents/trafficBuildings.ts#TrafficMap`

### src/world/geometry.ts

- `src/world/geometry.ts#orientedPolyline` (function, line 33) — call by 2: `src/world/lanelets.ts#LaneletGraph.buildSegmentLinks`, `src/world/roads/rules.ts#mainRoadLegs`
- `src/world/geometry.ts#segmentLine` (function, line 28) — call by 1: `src/sim/meso/meso.ts#Meso.relink`

### src/world/heading.ts

- `src/world/heading.ts#HEADING_CHORD` (variable, line 5) — read by 4: `src/world/conflictTable.ts#sweepFrame`, `src/world/turnPaths.ts#approachPosesOf`, `src/world/turnPaths.ts#LANE_REACH`, `src/world/turnPaths.ts#poseInto`

### src/world/ids.ts

- `src/world/ids.ts#NodeId` (type, line 7) — typeRef by 101: `src/sim/intersections/admission.ts#continues`, `src/sim/intersections/admission.ts#crossingLegs`, `src/sim/intersections/admission.ts#pending`, `src/sim/intersections/admission.ts#rightOfWay`, …
- `src/world/ids.ts#SegmentId` (type, line 8) — typeRef by 69: `src/render/densityCars.ts#createDensityCars`, `src/render/densityCars.ts#createDensityCars.writeLine`, `src/render/densityCars.ts#createDensityCars.writeTile`, `src/sim/agents/kerbBays.ts#fileStreet`, …

### src/world/junction/levels.ts

- `src/world/junction/levels.ts#JunctionLevels.get` (method, line 256) — call by 1: `src/world/surfaceParts.ts#surfaceParts`

### src/world/junction/types.ts

- `src/world/junction/types.ts#Junction` (interface, line 60) — read by 1: `src/world/surfaceParts.ts#surfaceParts`

### src/world/junctionBuild.ts

- `src/world/junctionBuild.ts#connectorId` (function, line 23) — call by 1: `src/world/lanelets.ts#LaneletGraph.place`
- `src/world/junctionBuild.ts#JunctionInput` (interface, line 31) — read by 3: `src/world/lanelets.ts#LaneletGraph.place`, `src/world/lanelets.ts#LaneletGraph.takeKnown`, `src/world/junctionBuild.ts#junctionAsk`
- `src/world/junctionBuild.ts#JunctionInput` (interface, line 31) — typeRef by 5: `src/world/lanelets.ts#LaneletGraph.junctionInput`, `src/world/lanelets.ts#LaneletGraph.junctionsOffPage`, `src/world/lanelets.ts#LaneletGraph.place`, `src/world/lanelets.ts#LaneletGraph.takeKnown`, …
- `src/world/junctionBuild.ts#junctionKey` (function, line 70) — call by 1: `src/world/lanelets.ts#LaneletGraph.buildJunction`
- `src/world/junctionBuild.ts#JunctionLane` (interface, line 26) — read by 1: `src/world/junctionBuild.ts#junctionAsk.lane`
- `src/world/junctionBuild.ts#JunctionLane` (interface, line 26) — typeRef by 2: `src/world/lanelets.ts#LaneletGraph.junctionInput.lane`, `src/world/junctionBuild.ts#junctionAsk.lane`
- `src/world/junctionBuild.ts#JunctionMade` (interface, line 56) — read by 1: `src/world/lanelets.ts#LaneletGraph.place`
- `src/world/junctionBuild.ts#JunctionMade` (interface, line 56) — typeRef by 2: `src/world/lanelets.ts#LaneletGraph.junctionsOffPage`, `src/world/lanelets.ts#LaneletGraph.place`
- `src/world/junctionBuild.ts#junctionMovements` (function, line 87) — call by 1: `src/world/lanelets.ts#LaneletGraph.buildJunction`
- `src/world/junctionBuild.ts#junctionMovements.addConnector` (function, line 116) — callbackAssign by 1: `src/world/lanelets.ts#LaneletGraph.place`
- `src/world/junctionBuild.ts#junctionMovements.addConnector` (function, line 116) — implementation by 1: `src/world/lanelets.ts#LaneletGraph.place`
- `src/world/junctionBuild.ts#MadeConnector` (interface, line 41) — read by 1: `src/world/lanelets.ts#LaneletGraph.place`

### src/world/junctionTurns.ts

- `src/world/junctionTurns.ts#JunctionTurns` (class, line 19) — construct by 1: `src/world/lanelets.ts#LaneletGraph.buildJunction`
- `src/world/junctionTurns.ts#JunctionTurns` (class, line 19) — reference by 1: `src/world/lanelets.ts#LaneletGraph.junctionsOffPage`
- `src/world/junctionTurns.ts#JunctionTurns` (class, line 19) — typeRef by 8: `src/world/lanelets.ts#JunctionCache`, `src/world/lanelets.ts#LaneletGraph.buildJunction`, `src/world/lanelets.ts#LaneletGraph.place`, `src/world/lanelets.ts#PendingJunction`, …
- `src/world/junctionTurns.ts#JunctionTurns.deferred` (field, line 24) — read by 1: `src/world/junctionTurns.ts#JunctionTurns.defer`
- `src/world/junctionTurns.ts#JunctionTurns.deferred` (field, line 24) — write by 1: `src/world/junctionTurns.ts#JunctionTurns.defer`
- `src/world/junctionTurns.ts#JunctionTurns.entries` (method, line 73) — call by 1: `src/world/junctionBuild.ts#junctionAsk`
- `src/world/junctionTurns.ts#JunctionTurns.made` (field, line 20) — read by 2: `src/world/junctionTurns.ts#JunctionTurns.find`, `src/world/junctionTurns.ts#JunctionTurns.keep`
- `src/world/junctionTurns.ts#JunctionTurns.made` (field, line 20) — write by 2: `src/world/junctionTurns.ts#JunctionTurns.find`, `src/world/junctionTurns.ts#JunctionTurns.keep`
- `src/world/junctionTurns.ts#JunctionTurns.of` (method, line 78) — call by 1: `src/world/lanelets.ts#LaneletGraph.junctionsOffPage`
- `src/world/junctionTurns.ts#JunctionTurns.settle` (method, line 66) — call by 2: `src/world/lanelets.ts#LaneletGraph.place`, `src/world/lanelets.ts#LaneletGraph.reshape`
- `src/world/junctionTurns.ts#TurnPlan` (class, line 8) — typeRef by 2: `src/world/lanelets.ts#LaneletGraph.turnPlan`, `src/world/junctionTurns.ts#JunctionTurns.constructor`
- `src/world/junctionTurns.ts#TurnPlan.asks` (field, line 10) — read by 1: `src/world/junctionTurns.ts#JunctionTurns.defer`
- `src/world/junctionTurns.ts#TurnPlan.asks` (field, line 10) — write by 1: `src/world/junctionTurns.ts#JunctionTurns.defer`
- `src/world/junctionTurns.ts#TurnPlan.chosen` (field, line 13) — read by 1: `src/world/junctionTurns.ts#JunctionTurns.find`
- `src/world/junctionTurns.ts#TurnPlan.recording` (field, line 9) — read by 1: `src/world/junctionTurns.ts#JunctionTurns.deferring`
- `src/world/junctionTurns.ts#TurnPlan.recording` (field, line 9) — write by 1: `src/world/lanelets.ts#LaneletGraph.redoProvisional`
- `src/world/junctionTurns.ts#TurnPlan.surfaces` (field, line 12) — read by 1: `src/world/junctionTurns.ts#JunctionTurns.defer`
- `src/world/junctionTurns.ts#TurnPlan.surfaces` (field, line 12) — write by 1: `src/world/junctionTurns.ts#JunctionTurns.defer`

### src/world/lotRecord.ts

- `src/world/lotRecord.ts#Lot` (interface, line 6) — read by 3: `src/sim/agents/lorryDocks.ts#buildYard`, `src/sim/agents/lorryDocks.ts#ownLot`, `src/sim/agents/lorryDocks.ts#yardInputs`
- `src/world/lotRecord.ts#Lot` (interface, line 6) — typeRef by 4: `src/sim/agents/lorryDocks.ts#lotsNear`, `src/sim/agents/lorryDocks.ts#ownLot`, `src/sim/agents/lorryDocks.ts#yardInputs`, `src/sim/agents/trafficBuildings.ts#TrafficMap`

### src/world/lotStore.ts

- `src/world/lotStore.ts#LotStore.keys` (getter, line 117) — call by 1: `src/sim/agents/trafficBuildings.ts#trafficMapOf`
- `src/world/lotStore.ts#LotStore.load` (method, line 270) — call by 1: `src/sim/agents/trafficBuildings.ts#takeTrafficMap`
- `src/world/lotStore.ts#LotStore.orderOf` (method, line 96) — call by 1: `src/sim/agents/lorryDocks.ts#ownLot`
- `src/world/lotStore.ts#LotStore.toJSON` (method, line 409) — call by 1: `src/sim/agents/trafficBuildings.ts#trafficMapOf`

### src/world/network.ts

- `src/world/network.ts#Network` (class, line 46) — typeRef by 41: `src/render/densityCars.ts#createDensityCars.readLines`, `src/render/densityCars.ts#createDensityCars.syncLines`, `src/render/densityCars.ts#createDensityCars.writeLine`, `src/render/densityCars.ts#createDensityCars.writeTile`, …
- `src/world/network.ts#Network.changesSince` (method, line 143) — call by 5: `src/render/densityCars.ts#createDensityCars.syncLines`, `src/sim/agents/kerbBays.ts#followKerbs`, `src/world/lanelets.ts#LaneletGraph.adopt`, `src/world/lanelets.ts#LaneletGraph.updateSteps`, …
- `src/world/network.ts#Network.constructor` (constructor, line 100) — read by 7: `src/world/parkingLayout.ts#endClear`, `src/world/parkingLayout.ts#parkingDigest`, `src/world/parkingLayout.ts#parkingOf`, `src/world/parkingLayout.ts#parksAlong`, …
- `src/world/network.ts#Network.crosswalkDistanceAt` (method, line 324) — call by 2: `src/sim/intersections/admission.ts#pedestrianHasPriority`, `src/world/parkingLayout.ts#endClear`
- `src/world/network.ts#Network.junctionCells` (field, line 58) — read by 1: `src/sim/agents/lorryDocks.ts#asphaltNear`
- `src/world/network.ts#Network.junctions` (field, line 54) — read by 2: `src/sim/agents/lorryDocks.ts#asphaltNear`, `src/world/surfaceParts.ts#surfaceParts`
- `src/world/network.ts#Network.mouthDistance` (method, line 288) — call by 4: `src/world/lanelets.ts#LaneletGraph.buildSegmentLinks`, `src/world/parkingLayout.ts#endClear`, `src/world/parkingLayout.ts#parkingDigest`, `src/world/parkingLayout.ts#parkingOf`
- `src/world/network.ts#Network.revision` (field, line 87) — read by 3: `src/sim/agents/lorryDocks.ts#yardKey`, `src/world/lanelets.ts#LaneletGraph.brought`, `src/world/lanelets.ts#LaneletGraph.buildSteps`
- `src/world/network.ts#Network.ribbons` (field, line 55) — read by 11: `src/render/densityCars.ts#createDensityCars.writeLine`, `src/render/densityCars.ts#createDensityCars.writeTile`, `src/sim/agents/kerbBays.ts#fileStreet`, `src/sim/agents/kerbBays.ts#kerbBaysOf`, …
- `src/world/network.ts#Network.ribbonsMeeting` (method, line 62) — call by 2: `src/sim/agents/lorryDocks.ts#asphaltNear`, `src/sim/ambient/ambientIndex.ts#StreetLength.around`
- `src/world/network.ts#Network.state` (getter, line 148) — call by 7: `src/render/densityCars.ts#createDensityCars.syncLines`, `src/sim/agents/kerbBays.ts#followKerbs`, `src/world/lanelets.ts#LaneletGraph.brought`, `src/world/lanelets.ts#LaneletGraph.build`, …
- `src/world/network.ts#Network.stopLineDistance` (method, line 298) — call by 2: `src/world/lanelets.ts#LaneletGraph.buildSegmentLinks`, `src/world/parkingLayout.ts#endClear`
- `src/world/network.ts#Network.trims` (field, line 75) — read by 1: `src/world/surfaceParts.ts#surfaceParts.road`

### src/world/networkDelta.ts

- `src/world/networkDelta.ts#NetworkDelta` (interface, line 5) — read by 3: `src/render/densityCars.ts#createDensityCars.syncLines`, `src/sim/agents/kerbBays.ts#followKerbs`, `src/world/networkLength.ts#NetworkLength.of`

### src/world/rebuildWork.ts

- `src/world/rebuildWork.ts#rebuildWork` (variable, line 5) — read by 6: `src/sim/intersections/crossingSpans.ts#CrossingSpans.measureJunction`, `src/sim/routing/router.ts#peel`, `src/sim/routing/router.ts#seedsOf`, `src/sim/routing/router.ts#TrapIndex.follow`, …
- `src/world/rebuildWork.ts#rebuildWork` (variable, line 5) — write by 6: `src/sim/intersections/crossingSpans.ts#CrossingSpans.measureJunction`, `src/sim/routing/router.ts#peel`, `src/sim/routing/router.ts#seedsOf`, `src/sim/routing/router.ts#TrapIndex.follow`, …

### src/world/ribbons.ts

- `src/world/ribbons.ts#SegmentRibbon` (interface, line 25) — read by 11: `src/render/densityCars.ts#createDensityCars.writeLine`, `src/render/densityCars.ts#createDensityCars.writeTile`, `src/sim/agents/kerbBays.ts#fileStreet`, `src/world/parkingLayout.ts#parkingDigest`, …
- `src/world/ribbons.ts#SegmentRibbon` (interface, line 25) — typeRef by 6: `src/world/parkingLayout.ts#parkingDigest`, `src/world/parkingLayout.ts#parkingLinesOf`, `src/world/parkingLayout.ts#parkingOf`, `src/world/parkingLayout.ts#parksAlong`, …
- `src/world/ribbons.ts#SegmentTrims` (interface, line 18) — read by 1: `src/world/surfaceParts.ts#surfaceParts.road`

### src/world/roads/connectors.ts

- `src/world/roads/connectors.ts#movementKey` (function, line 36) — call by 2: `src/ui/roads/junctionPanel.ts#mountJunctionPanel`, `src/ui/roads/junctionPanel.ts#moveChip`

### src/world/roads/footwayRise.ts

- `src/world/roads/footwayRise.ts#footwayRiseAt` (function, line 33) — call by 1: `src/render/agents.ts#createAgentMeshes.sync`

### src/world/roads/tuning.ts

- `src/world/roads/tuning.ts#ROAD_TUNING` (variable, line 4) — read by 6: `src/sim/roads/flowStats.ts#TAU`, `src/sim/roads/controlAdvisor.ts#ControlAdvisor.evaluate`, `src/sim/roads/controlAdvisor.ts#F`, `src/sim/roads/controlAdvisor.ts#QUIET_FLOW`, …

### src/world/roadSection.ts

- `src/world/roadSection.ts#laneUse` (function, line 262) — call by 2: `src/world/lanelets.ts#LaneletGraph.buildSegmentLinks`, `src/world/roadSection.ts#laneLineCrossable`
- `src/world/roadSection.ts#ROAD_SECTION_LIMITS` (variable, line 64) — read by 1: `src/sim/routing/lowerBound.ts#FASTEST_LIMIT`
- `src/world/roadSection.ts#RoadSection` (interface, line 23) — read by 4: `src/sim/intersections/admission.ts#throughRank`, `src/world/lanelets.ts#LaneletGraph.buildSegmentLinks`, `src/world/roads/rules.ts#roadRank`, `src/world/roadSection.ts#laneLineCrossable`
- `src/world/roadSection.ts#RoadSection` (interface, line 23) — typeRef by 1: `src/world/roadSection.ts#laneLineCrossable`

### src/world/roadTypes.ts

- `src/world/roadTypes.ts#halfWidth` (function, line 339) — call by 1: `src/world/surfaceParts.ts#surfaceParts.road`
- `src/world/roadTypes.ts#laneOffset` (function, line 420) — call by 2: `src/render/densityCars.ts#createDensityCars.writeTile`, `src/world/lanelets.ts#LaneletGraph.buildSegmentLinks`
- `src/world/roadTypes.ts#laneWidth` (function, line 395) — call by 1: `src/world/lanelets.ts#LaneletGraph.buildSegmentLinks`
- `src/world/roadTypes.ts#Level` (enum, line 10) — reference by 2: `src/sim/agents/lorryDocks.ts#asphaltNear`, `src/world/surfaceParts.ts#surfaceParts`
- `src/world/roadTypes.ts#Level` (enum, line 10) — typeRef by 1: `src/world/surfaceParts.ts#surfaceParts.road`
- `src/world/roadTypes.ts#roadProfile` (function, line 248) — call by 1: `src/world/lanelets.ts#LaneletGraph.buildSegmentLinks`
- `src/world/roadTypes.ts#roadType` (function, line 235) — call by 1: `src/world/roads/rules.ts#isHighwayClass`
- `src/world/roadTypes.ts#RoadType` (interface, line 29) — read by 7: `src/sim/agents/kerbBays.ts#fileStreet`, `src/world/lanelets.ts#LaneletGraph.buildSegmentLinks`, `src/world/parkingLayout.ts#parkingDigest`, `src/world/parkingLayout.ts#parkingOf`, …
- `src/world/roadTypes.ts#travelLanes` (function, line 391) — call by 1: `src/world/lanelets.ts#LaneletGraph.buildSegmentLinks`

### src/world/signalPosts.ts

- `src/world/signalPosts.ts#SIGNAL_POST_RADIUS` (variable, line 13) — read by 1: `src/sim/agents/walk.ts#stepWalkers`
- `src/world/signalPosts.ts#SignalPost` (interface, line 15) — read by 3: `src/sim/agents/walk.ts#stepWalkers`, `src/sim/peds/signalPostIndex.ts#SignalPostIndex.lift`, `src/sim/peds/signalPostIndex.ts#SignalPostIndex.file`
- `src/world/signalPosts.ts#SignalPost` (interface, line 15) — typeRef by 3: `src/sim/peds/signalPostIndex.ts#SignalPostIndex.near`, `src/sim/peds/signalPostIndex.ts#SignalPostIndex.found`, `src/sim/peds/signalPostIndex.ts#SignalPostIndex.file`
- `src/world/signalPosts.ts#signalPostPlace` (function, line 25) — call by 1: `src/sim/peds/signalPostIndex.ts#SignalPostIndex.placeJunction`
- `src/world/signalPosts.ts#signalPosts` (function, line 53) — call by 1: `src/sim/peds/signalPostIndex.ts#SignalPostIndex.placeAll`

### src/world/structures.ts

- `src/world/structures.ts#TUNNELS_DRAWN` (variable, line 75) — read by 1: `src/world/lanelets.ts#LaneletGraph.buildSegmentLinks`

### src/world/surfaceParts.ts

- `src/world/surfaceParts.ts#MouthStrip` (interface, line 9) — read by 2: `src/world/turnPaths.ts#JunctionSurface.constructor`, `src/world/junctionBuild.ts#junctionAsk`
- `src/world/surfaceParts.ts#MouthStrip` (interface, line 9) — typeRef by 1: `src/world/surfaceParts.ts#surfaceParts`
- `src/world/surfaceParts.ts#SurfaceSource` (interface, line 16) — read by 2: `src/world/turnPaths.ts#JunctionSurface.constructor`, `src/world/junctionBuild.ts#junctionAsk`
- `src/world/surfaceParts.ts#SurfaceSource` (interface, line 16) — typeRef by 3: `src/world/turnPaths.ts#JunctionSurface.constructor`, `src/world/surfaceParts.ts#surfaceParts`, `src/world/junctionBuild.ts#junctionAsk`

### src/world/surfaces.ts

- `src/world/surfaces.ts#levelRingsOf` (function, line 81) — call by 1: `src/sim/agents/lorryDocks.ts#asphaltNear`

### src/world/terrain.ts

- `src/world/terrain.ts#TERRAIN_MAX_HEIGHT` (variable, line 39) — read by 2: `src/render/densityCars.ts#createDensityCars.tileAt`, `src/render/densityCars.ts#TILE_RADIUS`
- `src/world/terrain.ts#TERRAIN_MIN_HEIGHT` (variable, line 38) — read by 2: `src/render/densityCars.ts#createDensityCars.tileAt`, `src/render/densityCars.ts#TILE_RADIUS`

### src/world/tiles.ts

- `src/world/tiles.ts#OWNER` (variable, line 82) — call by 1: `src/sim/agents/parking.ts#streetTile`
- `src/world/tiles.ts#OWNER` (variable, line 82) — read by 1: `src/sim/agents/parking.ts#streetTile`
- `src/world/tiles.ts#TILE` (variable, line 7) — read by 3: `src/render/densityCars.ts#createDensityCars.fileSlot`, `src/render/densityCars.ts#createDensityCars.tileAt`, `src/render/densityCars.ts#TILE_RADIUS`

### src/world/transit.ts

- `src/world/transit.ts#hasTransit` (function, line 103) — call by 1: `src/sim/agents/trafficBuildings.ts#trafficMapOf`
- `src/world/transit.ts#TransitLine` (interface, line 35) — read by 2: `src/sim/transit/transit.ts#TransitSim.closedOn`, `src/sim/transit/transit.ts#TransitSim.trainZones`

### src/world/waiting.ts

- `src/world/waiting.ts#answered` (function, line 5) — call by 5: `src/world/conflictPoints.ts#ConflictIndex.finishOffPage`, `src/world/conflictPoints.ts#ConflictIndex.measureOffPage`, `src/world/conflictPoints.ts#ConflictIndex.prepare`, `src/world/conflictPoints.ts#ConflictIndex.sampleOffPage`, …
- `src/world/waiting.ts#WAITING` (variable, line 2) — typeRef by 8: `src/world/conflictPoints.ts#ConflictIndex.finishOffPage`, `src/world/conflictPoints.ts#ConflictIndex.measureOffPage`, `src/world/conflictPoints.ts#ConflictIndex.prepare`, `src/world/conflictPoints.ts#ConflictIndex.sampleOffPage`, …

### src/world/walkingLines.ts

- `src/world/walkingLines.ts#CornerMeasurer` (interface, line 381) — inherit by 1: `src/world/conflictTable.ts#PairMeasurer`

### src/world/walkways.ts

- `src/world/walkways.ts#WalkChange` (interface, line 72) — read by 1: `src/sim/agents/walk.ts#ensureGraph`
- `src/world/walkways.ts#WalkChange` (interface, line 72) — typeRef by 2: `src/world/walkways.ts#WalkGraph.changesSince`, `src/world/walkways.ts#WalkGraph.none`
- `src/world/walkways.ts#WalkGraph` (class, line 81) — typeRef by 7: `src/sim/agents/walk.ts#ensureGraph`, `src/sim/agents/walk.ts#route`, `src/sim/agents/walk.ts#ROUTE_SCRATCH`, `src/sim/agents/walk.ts#routeScratch`, …
- `src/world/walkways.ts#WalkGraph.at` (field, line 86) — read by 2: `src/sim/agents/walk.ts#route`, `src/sim/agents/walk.ts#walkComponents`
- `src/world/walkways.ts#WalkGraph.log` (field, line 93) — read by 1: `src/world/walkways.ts#WalkGraph.changesSince`
- `src/world/walkways.ts#WalkGraph.nodes` (field, line 83) — read by 4: `src/sim/agents/walk.ts#route`, `src/sim/agents/walk.ts#route.estimate`, `src/sim/agents/walk.ts#routeScratch`, `src/sim/agents/walk.ts#walkComponents`
- `src/world/walkways.ts#WalkGraph.state` (getter, line 100) — call by 2: `src/sim/agents/walk.ts#ensureGraph`, `src/sim/agents/walk.ts#walkComponents`
- `src/world/walkways.ts#WalkGraph.token` (field, line 96) — read by 1: `src/world/walkways.ts#WalkGraph.changesSince`
- `src/world/walkways.ts#WalkGraph.ways` (field, line 84) — read by 5: `src/sim/agents/walk.ts#ensureGraph`, `src/sim/agents/walk.ts#nearestWay`, `src/sim/agents/walk.ts#route`, `src/sim/agents/walk.ts#walkComponents`, …
- `src/world/walkways.ts#WalkNode` (interface, line 26) — read by 1: `src/sim/agents/walk.ts#route.estimate`
- `src/world/walkways.ts#Walkway` (interface, line 32) — read by 20: `src/sim/agents/walk.ts#bendRadii`, `src/sim/agents/walk.ts#createAgentWalkEngine`, `src/sim/agents/walk.ts#crossingOf`, `src/sim/agents/walk.ts#fileWay`, …
- `src/world/walkways.ts#Walkway` (interface, line 32) — typeRef by 12: `src/sim/agents/walk.ts#bendRadii`, `src/sim/agents/walk.ts#BENDS`, `src/sim/agents/walk.ts#crossingOf`, `src/sim/agents/walk.ts#fileWay`, …

## External APIs

- `es:Math` × 1912
- `es:Map.get` × 605
- `es:Array.push` × 586
- `es:Math.max` × 557
- `es:Array.length` × 458
- `es:Math.min` × 373
- `es:Map` × 324
- `es:Map.set` × 304
- `es:Math.floor` × 237
- `es:Infinity` × 221
- `es:Float64Array` × 217
- `es:ReadonlyArray.length` × 206
- `es:Set` × 183
- `es:Float32Array` × 180
- `es:Set.add` × 179
- `es:Map.delete` × 163
- `three:BufferGeometry` × 155
- `es:Array.map` × 139
- `three:Vector3` × 138
- `es:Math.abs` × 137
- `es:Math.sin` × 103
- `es:Number` × 103
- `es:Math.cos` × 102
- `es:Map.clear` × 96
- `es:Math.hypot` × 92
- `es:Int32Array` × 88
- `es:Math.PI` × 82
- `es:Promise` × 80
- `es:Set.has` × 80
- `es:ReadonlyArray.map` × 77
- `three:BufferGeometry.setAttribute` × 67
- `es:Array.sort` × 66
- `three:BufferAttribute.array` × 65
- `three:BufferAttribute` × 64
- `es:Array.filter` × 63
- `three:BufferGeometry.getAttribute` × 63
- `es:Float64Array.length` × 57
- `es:Map.size` × 55
- `es:WeakMap.get` × 55
- `es:Map.has` × 54
- `es:Map.values` × 53
- `es:Uint8Array` × 53
- `three:BufferAttribute.count` × 53
- `es:NumberConstructor.isFinite` × 51
- `es:NaN` × 50
- `es:WeakMap.set` × 50
- `es:Math.ceil` × 47
- `three:Matrix4` × 46
- `es:ReadonlySet` × 44
- `es:Uint32Array` × 44
- `es:Math.round` × 43
- `three:BufferGeometry.translate` × 40
- `es:WeakMap` × 38
- `three:BufferAttribute.getX` × 38
- `three:Vector3.y` × 38
- `es:Set.delete` × 37
- `es:Map.keys` × 36
- `es:Math.atan2` × 36
- `es:Record` × 36
- `three:InstancedMesh.instanceMatrix` × 34
- `dom:ParentNode.append` × 32
- `es:ReadonlyArray.includes` × 31
- `es:Object` × 30
- `es:Pick` × 30
- `es:Array.slice` × 29
- `es:Array.indexOf` × 28
- `es:Float32Array.fill` × 28
- `es:Float32Array.set` × 27
- `es:Set.clear` × 27
- `three:Vector3.x` × 27
- `es:Array.pop` × 26
- `es:Int32Array.length` × 26
- `es:Math.sqrt` × 26
- `es:Set.size` × 26
- `three:InstancedBufferAttribute` × 26
- `three:Vector3.z` × 26
- `es:Array.some` × 25
- `es:ReadonlyArray.filter` × 24
- `es:ReadonlySet.has` × 24
- `es:Array.splice` × 23
- `es:Promise.then` × 22
- `dom:performance` × 21
- `three:Color` × 21
- `three:Mesh.geometry` × 21
- `three:Object3D.name` × 21
- `es:NumberConstructor.isNaN` × 20
- `three:BufferGeometry.clone` × 20
- `three:DataTexture` × 20
- `three:Vector3.sub` × 20
- `three:BufferAttribute.setUsage` × 19
- `three:BufferGeometry.dispose` × 19
- `three:DynamicDrawUsage` × 19
- `three:InstancedMesh.count` × 19
- `dom:Performance.now` × 18
- `es:Math.imul` × 18
- `three:BufferAttribute.getY` × 18
- `three:Object3D.castShadow` × 18
- `three:Texture.needsUpdate` × 18
- `dom:HTMLOrSVGElement.dataset` × 17
- `es:Array` × 17
- `es:NumberConstructor.NaN` × 17
- `three:BufferGeometry.index` × 17
- `three:InstancedMesh.instanceColor` × 17
- `es:Array.find` × 16
- `es:ArrayLike` × 16
- `es:ReadonlyMap` × 16
- `es:Uint32Array.length` × 16
- `three:Vector3.copy` × 16
- `three:Vector3.normalize` × 16
- `dom:Element.textContent` × 15
- `es:Array.every` × 15
- `es:ReadonlyArray.slice` × 15
- `es:String.replace` × 15
- `three:BufferAttribute.getZ` × 15
- `dom:document` × 14
- `es:Float32Array.length` × 14
- `es:Generator` × 14
- `es:Number.toFixed` × 14
- `es:RegExp.test` × 14
- `three:Box3.min` × 14
- `three:BufferGeometry.getIndex` × 14
- `three:Float32BufferAttribute` × 14
- `three:SkinnedMesh.skeleton` × 14
- `dom:Document.createElement` × 13
- `es:Array.includes` × 13
- `es:Math.random` × 13
- `es:PromiseConstructor.all` × 13
- `es:ReadonlyArray.find` × 13
- `es:ReadonlyArray.some` × 13
- `es:String.startsWith` × 13
- `three:BoxGeometry` × 13
- `three:CylinderGeometry` × 13
- `three:Object3D.visible` × 13
- `es:Int32Array.fill` × 12
- `three:BufferAttribute.itemSize` × 12
- `three:BufferGeometry.boundingBox` × 12
- `three:BufferGeometry.computeVertexNormals` × 12
- `three:BufferGeometry.setIndex` × 12
- `three:Vector3.clone` × 12
- `dom:console` × 11
- `es:Array.shift` × 11
- `es:Error` × 11
- `es:Readonly` × 11
- `three:Box3.max` × 11
- `three:BufferAttribute.needsUpdate` × 11
- `three:BufferGeometry.rotateX` × 11
- `three:Color.b` × 11
- `three:Color.g` × 11
- `three:Color.r` × 11
- `three:Texture` × 11
- `three:Vector3.set` × 11
- `es:Array.forEach` × 10
- `es:Float64Array.set` × 10
- `es:Int8Array.set` × 10
- `es:ReadonlyArray.forEach` × 10
- `three:Matrix4.copy` × 10
- `three:Matrix4.fromArray` × 10
- `three:Matrix4.makeScale` × 10
- `three:Matrix4.multiply` × 10
- `dom:Console.error` × 9
- `es:PromiseConstructor.resolve` × 9
- `es:ReturnType` × 9
- `es:SharedArrayBuffer` × 9
- `es:String` × 9
- `three:BufferAttribute.addUpdateRange` × 9
- `three:BufferGeometry.rotateZ` × 9
- `three:InstancedMesh` × 9
- `three:Matrix4.elements` × 9
- `three:NearestFilter` × 9
- `three:Object3D.rotation` × 9
- `three:RGBAFormat` × 9
- `three:Texture.image` × 9
- `three:Vector3.distanceTo` × 9
- `dom:Element.setAttribute` × 8
- `es:Array.findIndex` × 8
- `es:Float64ArrayConstructor.from` × 8
- `es:Generator.next` × 8
- `es:Math.exp` × 8
- `es:Math.pow` × 8
- `es:ObjectConstructor.assign` × 8
- `es:ObjectConstructor.freeze` × 8
- `es:String.length` × 8
- `es:Uint32Array.fill` × 8
- `three:BufferAttribute.clearUpdateRanges` × 8
- `three:BufferGeometry.computeBoundingBox` × 8
- `three:Object3D.userData` × 8
- `three:Path.lineTo` × 8
- `three:Path.quadraticCurveTo` × 8
- `three:Skeleton.bones` × 8
- `three:Vector2` × 8
- `three:Vector3.addScaledVector` × 8
- `three:WebGLProgramParameters.vertexShader` × 8
- `three:WebGLProgramParametersWithUniforms.uniforms` × 8
- `es:Array.join` × 7
- `es:Array.reduce` × 7
- `es:ArrayConstructor.from` × 7
- `es:Float32Array.slice` × 7
- `es:Float64Array.fill` × 7
- `es:Iterable` × 7
- `es:IteratorReturnResult.value` × 7
- `es:Math.atan` × 7
- `es:Math.sign` × 7
- `es:NumberConstructor.EPSILON` × 7
- `es:Promise.catch` × 7
- `es:ReadonlyMap.get` × 7
- `es:String.slice` × 7
- `es:Uint32ArrayConstructor.from` × 7
- `three:BufferAttribute.getComponent` × 7
- `three:MeshStandardMaterial` × 7
- `three:Object3D.matrix` × 7
- `three:Object3D.parent` × 7
- `three:Quaternion` × 7
- `three:Texture.addUpdateRange` × 7
- `three:Texture.clearUpdateRanges` × 7
- `three:Texture.dispose` × 7
- `three:Vector3.setY` × 7
- `dom:GlobalEventHandlers.onclick` × 6
- `dom:HTMLButtonElement.type` × 6
- `dom:HTMLDivElement` × 6
- `es:ArrayBuffer` × 6
- `es:Float32Array.subarray` × 6
- `es:Object.constructor` × 6
- `es:ObjectConstructor.keys` × 6
- `es:ReadonlyArray.every` × 6
- `es:ReadonlyArray.indexOf` × 6
- `es:ReadonlyArray.join` × 6
- `three:FloatType` × 6
- `three:Matrix4.invert` × 6
- `three:Matrix4.makeTranslation` × 6
- `three:RenderTarget.texture` × 6
- `three:RenderTarget.viewport` × 6
- `three:Skeleton.boneInverses` × 6
- `three:WebGLRenderer.autoClear` × 6
- `three:WebGLRenderer.setRenderTarget` × 6
- `dom:Element.className` × 5
- `dom:HTMLElement` × 5
- `es:Array.fill` × 5
- `es:Array.flatMap` × 5
- `es:Array.reverse` × 5
- `es:ArrayConstructor.isArray` × 5
- `es:NonNullable` × 5
- `es:ObjectConstructor.entries` × 5
- `meshoptimizer:MeshoptSimplifier` × 5
- `three:BufferAttribute.setXYZW` × 5
- `three:BufferGeometry.boundingSphere` × 5
- `three:BufferGeometry.deleteAttribute` × 5
- `three:DataTexture.magFilter` × 5
- `three:DataTexture.minFilter` × 5
- `three:DataTextureImageData.data` × 5
- `three:ExtrudeGeometry` × 5
- `three:Matrix4.multiplyMatrices` × 5
- `three:Object3D.frustumCulled` × 5
- `three:RenderTarget.dispose` × 5
- `three:UnsignedByteType` × 5
- `three:Vector3.applyMatrix4` × 5
- `three:Vector3.cross` × 5
- `three:Vector3.fromArray` × 5
- `three:Vector3.setFromMatrixPosition` × 5
- `three:WebGLRenderTarget` × 5
- `dom:location` × 4
- `es:ArrayBufferLike` × 4
- `es:Float32Array.byteLength` × 4
- `es:Int32Array.set` × 4
- `es:IteratorYieldResult.done` × 4
- `es:Math.SQRT1_2` × 4
- `es:Uint32Array.set` × 4
- `es:Uint8Array.length` × 4
- `es:WeakRef` × 4
- `es:WeakRef.deref` × 4
- `three:BufferAttribute.normalized` × 4
- `three:BufferGeometry.attributes` × 4
- `three:Color.clone` × 4
- `three:Color.setHex` × 4
- `three:Euler.order` × 4
- `three:Group` × 4
- `three:InstancedBufferGeometry` × 4
- `three:IUniform.value` × 4
- `three:Material` × 4
- `three:Material.customProgramCacheKey` × 4
- `three:Material.dispose` × 4
- `three:Material.onBeforeCompile` × 4
- `three:Matrix4.premultiply` × 4
- `three:mergeGeometries` × 4
- `three:Mesh` × 4
- `three:Object3D` × 4
- `three:Object3D.add` × 4
- `three:Object3D.position` × 4
- `three:Object3D.receiveShadow` × 4
- `three:Object3D.scale` × 4
- `three:Quaternion.setFromAxisAngle` × 4
- `three:ShaderMaterialProperties.uniforms` × 4
- `three:Shape` × 4
- `three:Sphere` × 4
- `three:Texture.userData` × 4
- `three:Vector3.add` × 4
- `three:WebGLProgramParameters.fragmentShader` × 4
- `three:WebGLRenderer` × 4
- `dom:fetch` × 3
- `dom:HTMLButtonElement` × 3
- `es:FinalizationRegistry.register` × 3
- `es:Float32Array.buffer` × 3
- `es:Float32Array.copyWithin` × 3
- `es:Float64Array.subarray` × 3
- `es:IterableIterator` × 3
- `es:Math.acos` × 3
- `es:Math.SQRT2` × 3
- `es:Number.toString` × 3
- `es:ObjectConstructor.create` × 3
- `es:ReadonlyArray.flatMap` × 3
- `es:ReadonlyArray.reduce` × 3
- `es:Uint32Array.slice` × 3
- `es:WeakMap.has` × 3
- `three:BufferAttribute.setXYZ` × 3
- `three:BufferGeometry.computeBoundingSphere` × 3
- `three:BufferGeometry.toNonIndexed` × 3
- `three:BufferGeometry.userData` × 3
- `three:InstancedMesh.dispose` × 3
- `three:InstancedMesh.morphTexture` × 3
- `three:InstancedMesh.setMatrixAt` × 3
- `three:Matrix4.compose` × 3
- `three:Matrix4.makeRotationAxis` × 3
- `three:MeshPhysicalMaterial` × 3
- `three:Quaternion.multiply` × 3
- `three:Quaternion.setFromUnitVectors` × 3
- `three:SkinnedMesh` × 3
- `three:TorusGeometry` × 3
- `three:Vector3.crossVectors` × 3
- `three:Vector3.fromBufferAttribute` × 3
- `three:Vector3.lerp` × 3
- `three:Vector3.multiplyScalar` × 3
- `three:Vector4.set` × 3
- `three:WebGLRenderer.render` × 3
- `dom:Console.warn` × 2
- `dom:HTMLCanvasElement.width` × 2
- `dom:Location.search` × 2
- `dom:OffscreenCanvas` × 2
- `dom:Response.ok` × 2
- `dom:setTimeout` × 2
- `dom:URLSearchParams` × 2
- `es:Array.flat` × 2
- `es:ArrayLike.length` × 2
- `es:Float32ArrayConstructor.from` × 2
- `es:Float64Array.buffer` × 2
- `es:Float64Array.byteLength` × 2
- `es:Int16Array` × 2
- `es:Int32Array.buffer` × 2
- `es:Int32Array.byteLength` × 2
- `es:Int32Array.slice` × 2
- `es:Int32ArrayConstructor.from` × 2
- `es:Int8Array` × 2
- `es:Int8Array.subarray` × 2
- `es:JSON` × 2
- `es:Map.entries` × 2
- `es:NumberConstructor.POSITIVE_INFINITY` × 2
- `es:Partial` × 2
- `es:Promise.finally` × 2
- `es:PromiseConstructor.reject` × 2
- `es:ReadonlyArray.findIndex` × 2
- `es:ReadonlyArray.flat` × 2
- `es:ReadonlySet.size` × 2
- `es:String.charCodeAt` × 2
- `es:String.indexOf` × 2
- `es:String.split` × 2
- `es:Uint16Array` × 2
- `meshoptimizer:MeshoptSimplifier.ready` × 2
- `node:process` × 2
- `three:BufferAttribute.setComponent` × 2
- `three:BufferGeometry.applyQuaternion` × 2
- `three:BufferGeometry.clearGroups` × 2
- `three:BufferGeometry.rotateY` × 2
- `three:Color.multiplyScalar` × 2
- `three:DataTexture.generateMipmaps` × 2
- `three:DataTextureImageData.width` × 2
- `three:DoubleSide` × 2
- `three:Euler.set` × 2
- `three:GLSL3` × 2
- `three:InstancedBufferGeometry.instanceCount` × 2
- `three:Material.clone` × 2
- `three:Matrix4.clone` × 2
- `three:Matrix4.makeRotationFromQuaternion` × 2
- `three:Matrix4.toArray` × 2
- `three:Mesh.material` × 2
- `three:MeshBasicMaterial` × 2
- `three:NoBlending` × 2
- `three:NormalBlending` × 2
- `three:Object3D.updateMatrix` × 2
- `three:Path.moveTo` × 2
- `three:Points` × 2
- `three:RedFormat` × 2
- `three:ShaderMaterial` × 2
- `three:SkinnedMesh.bindMatrix` × 2
- `three:Texture.colorSpace` × 2
- `three:Texture.flipY` × 2
- `three:Texture.onUpdate` × 2
- `three:Vector3.applyQuaternion` × 2
- `three:Vector3.dot` × 2
- `three:Vector3.length` × 2
- `three:Vector3.lengthSq` × 2
- `three:Vector3.negate` × 2
- `three:Vector3.subVectors` × 2
- `three:Vector3.transformDirection` × 2
- `three:WebGLRenderer.getRenderTarget` × 2
- `three:WebGLRenderer.initRenderTarget` × 2
- `vite:ImportMeta.env` × 2
- `vite:ImportMetaEnv.BASE_URL` × 2
- `dom:Body.arrayBuffer` × 1
- `dom:Body.json` × 1
- `dom:CanvasDrawImage.drawImage` × 1
- `dom:CanvasImageData.getImageData` × 1
- `dom:CanvasImageSource` × 1
- `dom:CanvasRenderingContext2D` × 1
- `dom:clearInterval` × 1
- `dom:DOMTokenList.add` × 1
- `dom:Element.classList` × 1
- `dom:Element.innerHTML` × 1
- `dom:HTMLCanvasElement.addEventListener` × 1
- `dom:HTMLCanvasElement.getContext` × 1
- `dom:HTMLCanvasElement.height` × 1
- `dom:HTMLElement.title` × 1
- `dom:ImageData.data` × 1
- `dom:Node.isConnected` × 1
- `dom:OffscreenCanvas.getContext` × 1
- `dom:OffscreenCanvasRenderingContext2D` × 1
- `dom:ParentNode.querySelector` × 1
- `dom:ParentNode.querySelectorAll` × 1
- `dom:TextDecoder` × 1
- `dom:TextDecoder.decode` × 1
- `dom:URLSearchParams.get` × 1
- `dom:URLSearchParams.has` × 1
- `dom:Worker` × 1
- `es:Array.unshift` × 1
- `es:ArrayBuffer.slice` × 1
- `es:DataView` × 1
- `es:DataView.getUint32` × 1
- `es:Error.stack` × 1
- `es:FinalizationRegistry` × 1
- `es:Float32Array.map` × 1
- `es:Float64Array.slice` × 1
- `es:Float64Array.sort` × 1
- `es:Int16Array.fill` × 1
- `es:Int32Array.map` × 1
- `es:Int32ArrayConstructor.BYTES_PER_ELEMENT` × 1
- `es:Int8Array.length` × 1
- `es:Int8Array.slice` × 1
- `es:Iterator.next` × 1
- `es:IteratorReturnResult.done` × 1
- `es:IteratorYieldResult.value` × 1
- `es:JSON.parse` × 1
- `es:JSON.stringify` × 1
- `es:Map.forEach` × 1
- `es:Math.asin` × 1
- `es:Math.fround` × 1
- `es:Math.log` × 1
- `es:Math.log2` × 1
- `es:Math.tan` × 1
- `es:Math.tanh` × 1
- `es:NumberConstructor.isInteger` × 1
- `es:Omit` × 1
- `es:ReadonlyMap.has` × 1
- `es:ReadonlyMap.size` × 1
- `es:ReadonlyMap.values` × 1
- `es:String.toLowerCase` × 1
- `es:String.trim` × 1
- `es:StringConstructor.fromCharCode` × 1
- `es:ThisType` × 1
- `es:Uint32Array.every` × 1
- `es:Uint8Array.fill` × 1
- `es:Uint8Array.set` × 1
- `es:Uint8Array.slice` × 1
- `es:Uint8Array.subarray` × 1
- `es:Uint8ArrayConstructor.from` × 1
- `es:Uint8ClampedArray` × 1
- `es:Uint8ClampedArray.length` × 1
- `es:WeakMap.delete` × 1
- `es:WeakSet.add` × 1
- `es:WeakSet.has` × 1
- `meshoptimizer:MeshoptSimplifier.simplify` × 1
- `meshoptimizer:MeshoptSimplifier.simplifyWithAttributes` × 1
- `meshoptimizer:MeshoptSimplifier.supported` × 1
- `node:Process.env` × 1
- `three:Box3.getCenter` × 1
- `three:BufferAttribute.onUpload` × 1
- `three:BufferGeometry.morphAttributes` × 1
- `three:BufferGeometry.position` × 1
- `three:BufferGeometry.scale` × 1
- `three:Color.getHex` × 1
- `three:Color.multiply` × 1
- `three:Color.set` × 1
- `three:DataTexture.flipY` × 1
- `three:DataTextureImageData.height` × 1
- `three:InstancedMesh.boundingSphere` × 1
- `three:InstancedMesh.setMorphAt` × 1
- `three:LatheGeometry` × 1
- `three:LinearFilter` × 1
- `three:LinearMipmapLinearFilter` × 1
- `three:MaterialProperties.name` × 1
- `three:Matrix4.decompose` × 1
- `three:Matrix4.extractRotation` × 1
- `three:Matrix4.getMaxScaleOnAxis` × 1
- `three:Matrix4.makeRotationY` × 1
- `three:Mesh.morphTargetDictionary` × 1
- `three:Mesh.morphTargetInfluences` × 1
- `three:MeshDepthMaterial` × 1
- `three:NoColorSpace` × 1
- `three:Object3D.clear` × 1
- `three:Object3D.customDepthMaterial` × 1
- `three:Object3D.matrixAutoUpdate` × 1
- `three:Object3D.onAfterShadow` × 1
- `three:Object3D.onBeforeShadow` × 1
- `three:Object3D.quaternion` × 1
- `three:Object3D.remove` × 1
- `three:Object3D.renderOrder` × 1
- `three:Object3D.traverse` × 1
- `three:OrthographicCamera` × 1
- `three:Quaternion.copy` × 1
- `three:Quaternion.setFromEuler` × 1
- `three:RenderTarget.height` × 1
- `three:RenderTarget.width` × 1
- `three:Scene` × 1
- `three:ShapeUtils` × 1
- `three:ShapeUtils.triangulateShape` × 1
- `three:SphereGeometry` × 1
- `three:SRGBColorSpace` × 1
- `three:Texture.anisotropy` × 1
- `three:TypedArray` × 1
- `three:Vector2.set` × 1
- `three:Vector3.addVectors` × 1
- `three:Vector3.angleTo` × 1
- `three:WebGLRenderer.compile` × 1
- `three:WebGLRenderer.copyTextureToTexture` × 1
- `three:WebGLRenderer.domElement` × 1
