# Feature: src/people/body/assets.ts, src/people/body/expressions.ts, src/people/body/macro.ts, src/people/body/morph.ts, src/people/body/proxy.ts, src/people/hair/procedural.ts, src/people/phenotype.ts, src/people/roster.ts, src/people/spec.ts, src/people/wardrobe.ts, src/render/citizenAssets.ts, src/render/citizenBake.ts, src/render/citizenBake.worker.ts, src/render/citizenCasting.ts, src/render/citizenGait.ts, src/render/citizenPalette.ts, src/render/citizenPlays.ts, src/render/citizenStride.ts, src/render/citizenWalk.ts, src/render/people/bodyPixels.ts, src/render/people/cartoon.ts, src/render/people/classRigMade.ts, src/render/people/cookPack.ts, src/render/people/cookedLoad.ts, src/render/people/cookedPerson.ts, src/render/people/crowdAnimation.ts, src/render/people/crowdBodies.ts, src/render/people/crowdLod.ts, src/render/people/faceExpression.ts, src/render/people/facialMorphs.ts, src/render/people/garmentPadding.ts, src/render/people/garmentPadding.worker.ts, src/render/people/garmentPaddingPool.ts, src/render/people/garmentSlots.ts, src/render/people/gpuRows.ts, src/render/people/hairTexture.ts, src/render/people/heldProps.ts, src/render/people/lodPool.ts, src/render/people/lodPort.ts, src/render/people/lodWorker.ts, src/render/people/paletteTargets.ts, src/render/people/personBuild.ts, src/render/people/personMesh.ts, src/render/people/personRig.ts, src/render/people/personVariants.ts, src/render/people/proceduralBake.ts, src/render/people/proceduralCook.ts, src/render/people/proceduralCrowd.ts, src/render/people/skinAppearance.ts, src/render/riderPoses.ts, src/render/riggedCitizens.ts, src/sim/agents/walk.ts, src/sim/crossings/permission.ts, src/sim/crossings/state.ts, src/sim/intersections/crossingSpans.ts, src/sim/peds/sidewalk.ts, src/sim/peds/signalPostIndex.ts, src/sim/people/casualties.ts, src/sim/people/engine.ts, src/sim/people/view.ts, src/world/footwayPath.ts, src/world/pedestrianAccess.ts, src/world/roads/footwayRise.ts, src/world/walkable.ts, src/world/walkingLines.ts, src/world/walkways.ts, src/sim/ambient/ambient.ts, src/sim/ambient/ambientIndex.ts, src/sim/ambient/demand.ts, src/sim/ambient/lotPlaces.ts, src/sim/ambient/playerId.ts, src/render/blobShadows.ts

The program starts from `src/main.ts`, `src/peopleLab/main.ts`. A declaration is the feature's own when the program reaches it only through the feature; in a declaration of the program that names the feature, its statements are a slice (what names it, what depends on that, what that controls).

- Own: 1965 declarations
- Slices of shared code: 297
- Code: 33167 lines in 157 files (source/)
- Resources: 84 (59 entry, 19 style, 6 import; 48 only the feature uses)
- Uses: 464 shared declarations (interface/: 1199 declarations in 249 files, the types they name included); hooks: 483; external APIs: 654

## Code

- src/actionsWiring.ts — part, 45 lines
- src/conflictPool.ts — part, 13 lines
- src/conflictTable.worker.ts — part, 22 lines
- src/core/regionIndex.ts — part, 520 lines
- src/core/rng.ts — part, 10 lines
- src/editor/roads/powerLine.ts — part, 5 lines
- src/editor/smallCityMap.ts — part, 1 lines
- src/editor/transitTools.ts — part, 3 lines
- src/lab/inspector/entity.ts — part, 19 lines
- src/people/body/assets.ts — whole, 98 lines
- src/people/body/expressions.ts — whole, 43 lines
- src/people/body/macro.ts — whole, 115 lines
- src/people/body/morph.ts — whole, 412 lines
- src/people/body/proxy.ts — whole, 206 lines
- src/people/hair/procedural.ts — whole, 639 lines
- src/people/phenotype.ts — whole, 35 lines
- src/people/roster.ts — whole, 130 lines
- src/people/spec.ts — whole, 361 lines
- src/people/wardrobe.ts — whole, 117 lines
- src/people/wardrobeSelection.json — whole, 166 lines
- src/peopleLab/bodyFit.ts — part, 21 lines
- src/peopleLab/build.ts — part, 100 lines
- src/peopleLab/choices.ts — part, 25 lines
- src/peopleLab/dress.ts — part, 19 lines
- src/peopleLab/faceRig.ts — part, 25 lines
- src/peopleLab/hairShape.ts — part, 57 lines
- src/peopleLab/main.ts — part, 135 lines
- src/peopleLab/panels.ts — part, 208 lines
- src/peopleLab/polish.ts — part, 16 lines
- src/peopleLab/pose.ts — part, 15 lines
- src/peopleLab/presets.ts — part, 5 lines
- src/peopleLab/proportions.ts — part, 49 lines
- src/peopleLab/record.ts — part, 33 lines
- src/peopleLab/reshape.ts — part, 6 lines
- src/peopleLab/sculpt.ts — part, 69 lines
- src/peopleLab/shapeKeys.ts — part, 19 lines
- src/peopleLab/thumbs.ts — part, 5 lines
- src/peopleLab/widgets.ts — part, 5 lines
- src/render/agents.ts — part, 449 lines
- src/render/bakePool.ts — whole, 202 lines
- src/render/blobShadows.ts — whole, 104 lines
- src/render/carBody.ts — part, 24 lines
- src/render/citizenAssets.ts — whole, 9 lines
- src/render/citizenBake.ts — whole, 532 lines
- src/render/citizenBake.worker.ts — whole, 166 lines
- src/render/citizenCasting.ts — whole, 344 lines
- src/render/citizenGait.ts — whole, 609 lines
- src/render/citizenPalette.ts — whole, 56 lines
- src/render/citizenPlays.ts — whole, 155 lines
- src/render/citizenStride.ts — whole, 114 lines
- src/render/citizenWalk.ts — whole, 389 lines
- src/render/groundShear.ts — part, 12 lines
- src/render/occupants.ts — part, 18 lines
- src/render/pagePorts.ts — part, 5 lines
- src/render/partLevelsWork.ts — part, 10 lines
- src/render/people/bodyPixels.ts — whole, 20 lines
- src/render/people/cartoon.ts — whole, 132 lines
- src/render/people/classRigMade.ts — whole, 63 lines
- src/render/people/cookedLoad.ts — whole, 64 lines
- src/render/people/cookedPerson.ts — whole, 267 lines
- src/render/people/cookPack.ts — whole, 53 lines
- src/render/people/crowdAnimation.ts — whole, 363 lines
- src/render/people/crowdBodies.ts — whole, 115 lines
- src/render/people/crowdLod.ts — whole, 288 lines
- src/render/people/faceExpression.ts — whole, 140 lines
- src/render/people/facialMorphs.ts — whole, 64 lines
- src/render/people/garmentPadding.ts — whole, 101 lines
- src/render/people/garmentPadding.worker.ts — whole, 33 lines
- src/render/people/garmentPaddingPool.ts — whole, 124 lines
- src/render/people/garmentSlots.ts — whole, 21 lines
- src/render/people/gpuRows.ts — whole, 173 lines
- src/render/people/hairTexture.ts — whole, 67 lines
- src/render/people/heldProps.ts — whole, 175 lines
- src/render/people/lodPool.ts — whole, 60 lines
- src/render/people/lodPort.ts — whole, 24 lines
- src/render/people/lodWorker.ts — whole, 112 lines
- src/render/people/paletteTargets.ts — whole, 104 lines
- src/render/people/personBuild.ts — whole, 54 lines
- src/render/people/personMesh.ts — whole, 307 lines
- src/render/people/personRig.ts — whole, 897 lines
- src/render/people/personVariants.ts — whole, 79 lines
- src/render/people/proceduralBake.ts — whole, 307 lines
- src/render/people/proceduralCook.ts — whole, 75 lines
- src/render/people/proceduralCrowd.ts — whole, 2532 lines
- src/render/people/skinAppearance.ts — whole, 693 lines
- src/render/playEffects.ts — part, 17 lines
- src/render/ragdoll.ts — part, 244 lines
- src/render/renderer.ts — part, 78 lines
- src/render/riderPoses.ts — whole, 587 lines
- src/render/riggedCitizens.ts — whole, 1300 lines
- src/render/roadSurfaceInputs.ts — part, 5 lines
- src/render/roadSurfacePass.ts — part, 1 lines
- src/render/roadSurfaces.ts — part, 10 lines
- src/render/roadTileMesh.ts — part, 9 lines
- src/render/signLayer.ts — part, 18 lines
- src/render/streetFurnitureLayer.ts — part, 19 lines
- src/render/utilities.ts — part, 6 lines
- src/render/vehicleLevels.ts — part, 24 lines
- src/render/vehicleModels.ts — part, 101 lines
- src/sim/agents/kerbBays.ts — part, 12 lines
- src/sim/agents/lorryTraffic.ts — part, 25 lines
- src/sim/agents/lotDoors.ts — whole, 127 lines
- src/sim/agents/lotTraffic.ts — part, 113 lines
- src/sim/agents/parking.ts — part, 8 lines
- src/sim/agents/trafficBuildings.ts — part, 14 lines
- src/sim/agents/walk.ts — whole, 2276 lines
- src/sim/ambient/ambient.ts — whole, 1000 lines
- src/sim/ambient/ambientIndex.ts — whole, 402 lines
- src/sim/ambient/demand.ts — whole, 63 lines
- src/sim/ambient/lotPlaces.ts — whole, 392 lines
- src/sim/ambient/playerId.ts — whole, 3 lines
- src/sim/city/city.ts — part, 3 lines
- src/sim/crossings/permission.ts — whole, 172 lines
- src/sim/crossings/state.ts — whole, 36 lines
- src/sim/host/protocol.ts — part, 24 lines
- src/sim/host/simClient.ts — part, 29 lines
- src/sim/host/simHost.ts — part, 79 lines
- src/sim/intersections/admission.ts — part, 55 lines
- src/sim/intersections/crossingSpans.ts — whole, 241 lines
- src/sim/invariants.ts — part, 26 lines
- src/sim/meso/demand.ts — part, 1 lines
- src/sim/meso/meso.ts — part, 37 lines
- src/sim/meso/model.ts — part, 583 lines
- src/sim/meso/slots.ts — part, 108 lines
- src/sim/peds/sidewalk.ts — whole, 309 lines
- src/sim/peds/signalPostIndex.ts — whole, 106 lines
- src/sim/people/casualties.ts — whole, 76 lines
- src/sim/people/engine.ts — whole, 80 lines
- src/sim/people/view.ts — whole, 116 lines
- src/sim/pipeline.ts — part, 10 lines
- src/sim/pose.ts — part, 17 lines
- src/sim/transit/transit.ts — part, 39 lines
- src/sim/vehicles/kerbStops.ts — part, 84 lines
- src/sim/vehicles/obstacles.ts — part, 28 lines
- src/sim/vehicles/spawn.ts — part, 63 lines
- src/sim/vehicles/state.ts — part, 3 lines
- src/sim/world.ts — part, 81 lines
- src/world/conflictTable.ts — part, 2 lines
- src/world/curbRamps.ts — part, 3 lines
- src/world/doc.ts — part, 20 lines
- src/world/footwayPath.ts — whole, 62 lines
- src/world/junction/bulb.ts — part, 11 lines
- src/world/landscape.ts — part, 5 lines
- src/world/lots.ts — part, 2 lines
- src/world/network.ts — part, 11 lines
- src/world/paving.ts — part, 6 lines
- src/world/pedestrianAccess.ts — whole, 7 lines
- src/world/poleLines.ts — part, 6 lines
- src/world/roads/footwayRise.ts — whole, 92 lines
- src/world/roads/furnitureSets.ts — part, 6 lines
- src/world/section.ts — part, 7 lines
- src/world/walkable.ts — whole, 137 lines
- src/world/walkingLines.ts — whole, 404 lines
- src/world/walkways.ts — whole, 674 lines
- public/models/people/proxies/community.json — resource, 7252 lines
- src/people/wardrobeSex.json — resource, 172 lines
- public/models/people/skins/index.json — resource, 551 lines

## Slices of shared code

- `src/actionsWiring.ts#createActions.explodeAt` — 4 of 158 lines
- `src/actionsWiring.ts#createActions.shootAt` — 41 of 58 lines
- `src/conflictPool.ts#ConflictPool.roundCorners` — 13 of 17 lines
- `src/conflictTable.worker.ts#CornersAsk` — 3 of 5 lines
- `src/conflictTable.worker.ts#serve` — 19 of 152 lines
- `src/editor/roads/powerLine.ts#layPowerLinesSteps` — 5 of 44 lines
- `src/editor/smallCityMap.ts#growLots.pavedAt` — 1 of 1 lines
- `src/editor/transitTools.ts#TransitTool.footwayAt` — 3 of 3 lines
- `src/lab/inspector/entity.ts#agentsNear` — 6 of 19 lines
- `src/lab/inspector/entity.ts#entityInfo` — 13 of 93 lines
- `src/peopleLab/bodyFit.ts#fitBody` — 14 of 37 lines
- `src/peopleLab/bodyFit.ts#fitBody.at` — 6 of 6 lines
- `src/peopleLab/bodyFit.ts#fitBody.withF` — 1 of 1 lines
- `src/peopleLab/build.ts#BuiltPerson` — 3 of 12 lines
- `src/peopleLab/build.ts#LabBuilder.assets` — 1 of 1 lines
- `src/peopleLab/build.ts#LabBuilder.build` — 35 of 48 lines
- `src/peopleLab/build.ts#LabBuilder.grown` — 1 of 1 lines
- `src/peopleLab/build.ts#LabBuilder.grownHair` — 11 of 11 lines
- `src/peopleLab/build.ts#LabBuilder.hairItem` — 6 of 8 lines
- `src/peopleLab/build.ts#LabBuilder.item` — 4 of 4 lines
- `src/peopleLab/build.ts#LabBuilder.morpher` — 1 of 1 lines
- `src/peopleLab/build.ts#LabBuilder.proportions` — 3 of 5 lines
- `src/peopleLab/build.ts#LabBuilder.sculptBody` — 3 of 4 lines
- `src/peopleLab/build.ts#LabBuilder.setup` — 5 of 5 lines
- `src/peopleLab/build.ts#LabBuilder.shape` — 3 of 4 lines
- `src/peopleLab/build.ts#LabBuilder.shaped` — 9 of 12 lines
- `src/peopleLab/build.ts#LabBuilder.studioBody` — 10 of 14 lines
- `src/peopleLab/build.ts#LabBuilder.wornPiece` — 2 of 6 lines
- `src/peopleLab/build.ts#hairBase` — 2 of 2 lines
- `src/peopleLab/build.ts#labStyle` — 1 of 1 lines
- `src/peopleLab/choices.ts#CLOTH` — 1 of 1 lines
- `src/peopleLab/choices.ts#MOODS` — 10 of 10 lines
- `src/peopleLab/choices.ts#READY_POSES` — 6 of 6 lines
- `src/peopleLab/choices.ts#ROADCRAFT` — 1 of 1 lines
- `src/peopleLab/choices.ts#STYLE_PRESETS` — 7 of 7 lines
- `src/peopleLab/dress.ts#dressPerson` — 19 of 30 lines
- `src/peopleLab/faceRig.ts#applyLabFace` — 10 of 14 lines
- `src/peopleLab/faceRig.ts#attachFace` — 15 of 26 lines
- `src/peopleLab/hairShape.ts#axisScales` — 2 of 8 lines
- `src/peopleLab/hairShape.ts#axisScales.axis` — 5 of 5 lines
- `src/peopleLab/hairShape.ts#shapedPack` — 50 of 60 lines
- `src/peopleLab/main.ts#startLab` — 46 of 731 lines
- `src/peopleLab/main.ts#startLab.closeSculptor` — 6 of 16 lines
- `src/peopleLab/main.ts#startLab.extraOf` — 1 of 1 lines
- `src/peopleLab/main.ts#startLab.female` — 1 of 1 lines
- `src/peopleLab/main.ts#startLab.fits` — 1 of 1 lines
- `src/peopleLab/main.ts#startLab.freshPerson` — 6 of 7 lines
- `src/peopleLab/main.ts#startLab.kindOf` — 1 of 1 lines
- `src/peopleLab/main.ts#startLab.loop` — 12 of 37 lines
- `src/peopleLab/main.ts#startLab.openCarver` — 3 of 10 lines
- `src/peopleLab/main.ts#startLab.openSculptor` — 12 of 18 lines
- `src/peopleLab/main.ts#startLab.render` — 3 of 24 lines
- `src/peopleLab/main.ts#startLab.renderGallery` — 14 of 17 lines
- `src/peopleLab/main.ts#startLab.runBuild` — 14 of 54 lines
- `src/peopleLab/main.ts#startLab.setBody` — 1 of 1 lines
- `src/peopleLab/main.ts#startLab.setExtra` — 4 of 4 lines
- `src/peopleLab/main.ts#startLab.setFeature` — 1 of 1 lines
- `src/peopleLab/main.ts#startLab.setLook` — 4 of 4 lines
- `src/peopleLab/main.ts#startLab.setSpec` — 3 of 5 lines
- `src/peopleLab/main.ts#startLab.setStyle` — 1 of 1 lines
- `src/peopleLab/main.ts#startLab.sliderOf` — 1 of 1 lines
- `src/peopleLab/panels.ts#PanelContext` — 10 of 38 lines
- `src/peopleLab/panels.ts#makePanels` — 171 of 243 lines
- `src/peopleLab/panels.ts#makePanels.b` — 1 of 1 lines
- `src/peopleLab/panels.ts#makePanels.child` — 1 of 1 lines
- `src/peopleLab/panels.ts#makePanels.extraPics` — 5 of 5 lines
- `src/peopleLab/panels.ts#makePanels.featureSlider` — 1 of 1 lines
- `src/peopleLab/panels.ts#makePanels.mf` — 1 of 1 lines
- `src/peopleLab/panels.ts#makePanels.opts` — 1 of 1 lines
- `src/peopleLab/panels.ts#makePanels.outfitPics` — 2 of 2 lines
- `src/peopleLab/panels.ts#makePanels.partGroup` — 9 of 10 lines
- `src/peopleLab/panels.ts#makePanels.pics` — 4 of 6 lines
- `src/peopleLab/panels.ts#makePanels.set` — 1 of 1 lines
- `src/peopleLab/panels.ts#makePanels.withLook` — 1 of 1 lines
- `src/peopleLab/polish.ts#clearScalpPaint` — 6 of 18 lines
- `src/peopleLab/polish.ts#polishPerson` — 6 of 14 lines
- `src/peopleLab/polish.ts#solidHair` — 4 of 9 lines
- `src/peopleLab/pose.ts#ClipPlayer.apply` — 4 of 5 lines
- `src/peopleLab/pose.ts#ClipPlayer.clip` — 1 of 1 lines
- `src/peopleLab/pose.ts#ClipPlayer.constructor` — 1 of 1 lines
- `src/peopleLab/pose.ts#ClipPlayer.play` — 7 of 9 lines
- `src/peopleLab/pose.ts#ClipPlayer.playing` — 1 of 1 lines
- `src/peopleLab/pose.ts#ClipPlayer.transfer` — 1 of 1 lines
- `src/peopleLab/presets.ts#BodyPreset` — 1 of 1 lines
- `src/peopleLab/presets.ts#LookPreset` — 1 of 1 lines
- `src/peopleLab/presets.ts#withBody` — 3 of 6 lines
- `src/peopleLab/proportions.ts#FaceProportions.constructor` — 10 of 14 lines
- `src/peopleLab/proportions.ts#FaceProportions.influenceOf` — 14 of 16 lines
- `src/peopleLab/proportions.ts#FaceProportions.landmark` — 9 of 10 lines
- `src/peopleLab/proportions.ts#FaceProportions.measure` — 3 of 39 lines
- `src/peopleLab/proportions.ts#measureBody` — 5 of 39 lines
- `src/peopleLab/proportions.ts#measureBody.centre` — 4 of 5 lines
- `src/peopleLab/proportions.ts#measureBody.main` — 4 of 5 lines
- `src/peopleLab/record.ts#LabPerson` — 5 of 21 lines
- `src/peopleLab/record.ts#compactPerson` — 17 of 25 lines
- `src/peopleLab/record.ts#readRoster` — 5 of 8 lines
- `src/peopleLab/record.ts#rosterMarkdown` — 6 of 6 lines
- `src/peopleLab/reshape.ts#Reshaper.constructor` — 2 of 10 lines
- `src/peopleLab/reshape.ts#Reshaper.flat` — 4 of 6 lines
- `src/peopleLab/sculpt.ts#HairSculptor.constructor` — 36 of 91 lines
- `src/peopleLab/sculpt.ts#HairSculptor.delta` — 3 of 5 lines
- `src/peopleLab/sculpt.ts#HairSculptor.refresh` — 15 of 39 lines
- `src/peopleLab/sculpt.ts#HairSculptor.reset` — 3 of 5 lines
- `src/peopleLab/sculpt.ts#axisScales` — 2 of 8 lines
- `src/peopleLab/sculpt.ts#axisScales.axis` — 5 of 5 lines
- `src/peopleLab/sculpt.ts#sculptedPack` — 5 of 5 lines
- `src/peopleLab/shapeKeys.ts#bodySlider` — 2 of 2 lines
- `src/peopleLab/shapeKeys.ts#buildKey` — 3 of 7 lines
- `src/peopleLab/shapeKeys.ts#effective` — 11 of 12 lines
- `src/peopleLab/shapeKeys.ts#featureSlider` — 2 of 2 lines
- `src/peopleLab/shapeKeys.ts#shapeOf` — 1 of 1 lines
- `src/peopleLab/thumbs.ts#Thumbs.layers` — 1 of 1 lines
- `src/peopleLab/thumbs.ts#Thumbs.shoot` — 4 of 38 lines
- `src/peopleLab/widgets.ts#widgets.pretty` — 2 of 2 lines
- `src/peopleLab/widgets.ts#widgets.thumbOf` — 3 of 4 lines
- `src/render/agents.ts#<ExpressionStatement>@76` — 1 of 1 lines
- `src/render/agents.ts#AgentMeshes` — 4 of 31 lines
- `src/render/agents.ts#createAgentMeshes` — 87 of 2045 lines
- `src/render/agents.ts#createAgentMeshes.animProbe` — 7 of 10 lines
- `src/render/agents.ts#createAgentMeshes.census` — 1 of 1 lines
- `src/render/agents.ts#createAgentMeshes.dispose` — 3 of 21 lines
- `src/render/agents.ts#createAgentMeshes.driverBody` — 3 of 3 lines
- `src/render/agents.ts#createAgentMeshes.forgetGone` — 3 of 5 lines
- `src/render/agents.ts#createAgentMeshes.keepPeople` — 1 of 1 lines
- `src/render/agents.ts#createAgentMeshes.meshProbe` — 39 of 49 lines
- `src/render/agents.ts#createAgentMeshes.placeBlob` — 3 of 5 lines
- `src/render/agents.ts#createAgentMeshes.placeCarOccupants` — 13 of 36 lines
- `src/render/agents.ts#createAgentMeshes.placeKerbPerson` — 16 of 46 lines
- `src/render/agents.ts#createAgentMeshes.placeKerbPerson.walkTo` — 4 of 8 lines
- `src/render/agents.ts#createAgentMeshes.placeRiderFigure` — 18 of 31 lines
- `src/render/agents.ts#createAgentMeshes.procBuildNext` — 8 of 24 lines
- `src/render/agents.ts#createAgentMeshes.procDraw` — 23 of 23 lines
- `src/render/agents.ts#createAgentMeshes.procFinish` — 8 of 22 lines
- `src/render/agents.ts#createAgentMeshes.procOf` — 2 of 2 lines
- `src/render/agents.ts#createAgentMeshes.procPose` — 124 of 124 lines
- `src/render/agents.ts#createAgentMeshes.procSpareFor` — 4 of 4 lines
- `src/render/agents.ts#createAgentMeshes.procSuits` — 3 of 3 lines
- `src/render/agents.ts#createAgentMeshes.renderPalettes` — 1 of 1 lines
- `src/render/agents.ts#createAgentMeshes.riderPlay` — 7 of 7 lines
- `src/render/agents.ts#createAgentMeshes.seatIdentity` — 7 of 7 lines
- `src/render/agents.ts#createAgentMeshes.setBleed` — 1 of 1 lines
- `src/render/agents.ts#createAgentMeshes.sync` — 49 of 421 lines
- `src/render/agents.ts#createAgentMeshes.sync.drawVehicle` — 5 of 161 lines
- `src/render/agents.ts#createAgentMeshes.vehicleCompany` — 3 of 3 lines
- `src/render/agents.ts#gaitClip` — 1 of 1 lines
- `src/render/carBody.ts#buildCarModel` — 24 of 577 lines
- `src/render/occupants.ts#OccupantClip` — 1 of 1 lines
- `src/render/occupants.ts#SEAT_CLIPS` — 5 of 5 lines
- `src/render/occupants.ts#clipsFor` — 3 of 3 lines
- `src/render/occupants.ts#occupantPlays` — 7 of 22 lines
- `src/render/occupants.ts#seatBaseClip` — 2 of 3 lines
- `src/render/playEffects.ts#PlayEffects` — 4 of 35 lines
- `src/render/playEffects.ts#createPlayEffects.frame` — 13 of 26 lines
- `src/render/ragdoll.ts#Body` — 3 of 70 lines
- `src/render/ragdoll.ts#J_STUMP` — 1 of 1 lines
- `src/render/ragdoll.ts#LOST_PARTS` — 3 of 3 lines
- `src/render/ragdoll.ts#RagdollCitizens` — 3 of 20 lines
- `src/render/ragdoll.ts#Ragdolls` — 4 of 32 lines
- `src/render/ragdoll.ts#createRagdolls` — 11 of 1556 lines
- `src/render/ragdoll.ts#createRagdolls.blast` — 31 of 31 lines
- `src/render/ragdoll.ts#createRagdolls.detach` — 43 of 43 lines
- `src/render/ragdoll.ts#createRagdolls.fling` — 10 of 18 lines
- `src/render/ragdoll.ts#createRagdolls.sever` — 12 of 12 lines
- `src/render/ragdoll.ts#createRagdolls.shootBody` — 16 of 51 lines
- `src/render/ragdoll.ts#createRagdolls.shove` — 18 of 18 lines
- `src/render/ragdoll.ts#createRagdolls.spawn` — 89 of 89 lines
- `src/render/renderer.ts#SceneHandle` — 4 of 170 lines
- `src/render/renderer.ts#createSceneRenderer` — 65 of 3774 lines
- `src/render/renderer.ts#createSceneRenderer.footwayRiseHere` — 1 of 1 lines
- `src/render/renderer.ts#createSceneRenderer.landGroundDigest` — 4 of 6 lines
- `src/render/renderer.ts#createSceneRenderer.levelGroundDigest` — 4 of 8 lines
- `src/render/roadSurfaceInputs.ts#SurfaceInputs.placeNode` — 5 of 81 lines
- `src/render/roadSurfacePass.ts#passReads.footwayShare` — 1 of 1 lines
- `src/render/roadSurfaces.ts#roadSurfaceSteps` — 10 of 512 lines
- `src/render/roadTileMesh.ts#SurfaceData` — 3 of 12 lines
- `src/render/roadTileMesh.ts#readsOf` — 6 of 37 lines
- `src/render/signLayer.ts#itemSigns` — 12 of 42 lines
- `src/render/signLayer.ts#junctionSigns` — 6 of 16 lines
- `src/render/streetFurnitureLayer.ts#furniturePlace` — 19 of 65 lines
- `src/render/utilities.ts#poleGroundAt` — 6 of 14 lines
- `src/render/vehicleLevels.ts#partLevels` — 24 of 46 lines
- `src/render/vehicleModels.ts#SteeringModel` — 3 of 7 lines
- `src/render/vehicleModels.ts#TwoWheelerModel` — 3 of 21 lines
- `src/render/vehicleModels.ts#bicycleModel` — 37 of 67 lines
- `src/render/vehicleModels.ts#buildBusModel` — 5 of 284 lines
- `src/render/vehicleModels.ts#buildTruckModel` — 9 of 176 lines
- `src/render/vehicleModels.ts#gripInHead` — 4 of 5 lines
- `src/render/vehicleModels.ts#handlebar` — 8 of 14 lines
- `src/render/vehicleModels.ts#motorcycleModel` — 32 of 70 lines
- `src/sim/agents/lorryTraffic.ts#LorryTraffic.drop` — 3 of 4 lines
- `src/sim/agents/lorryTraffic.ts#LorryTraffic.mayGo` — 12 of 17 lines
- `src/sim/agents/lorryTraffic.ts#LorryTraffic.step` — 3 of 21 lines
- `src/sim/agents/lotTraffic.ts#KINDS` — 1 of 1 lines
- `src/sim/agents/lotTraffic.ts#LotTraffic.ambient` — 1 of 1 lines
- `src/sim/agents/lotTraffic.ts#LotTraffic.call` — 3 of 48 lines
- `src/sim/agents/lotTraffic.ts#LotTraffic.coming` — 3 of 19 lines
- `src/sim/agents/lotTraffic.ts#LotTraffic.depart` — 8 of 40 lines
- `src/sim/agents/lotTraffic.ts#LotTraffic.drop` — 3 of 4 lines
- `src/sim/agents/lotTraffic.ts#LotTraffic.fileBay` — 16 of 18 lines
- `src/sim/agents/lotTraffic.ts#LotTraffic.followBuildings` — 5 of 8 lines
- `src/sim/agents/lotTraffic.ts#LotTraffic.holdLorries` — 10 of 15 lines
- `src/sim/agents/lotTraffic.ts#LotTraffic.mayGo` — 5 of 23 lines
- `src/sim/agents/lotTraffic.ts#LotTraffic.placeBody` — 7 of 9 lines
- `src/sim/agents/lotTraffic.ts#LotTraffic.readBays` — 9 of 42 lines
- `src/sim/agents/lotTraffic.ts#LotTraffic.refreshBay` — 6 of 6 lines
- `src/sim/agents/lotTraffic.ts#LotTraffic.step` — 5 of 33 lines
- `src/sim/agents/lotTraffic.ts#LotTraffic.track` — 5 of 14 lines
- `src/sim/agents/trafficBuildings.ts#TRAFFIC_KINDS` — 1 of 1 lines
- `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.place` — 3 of 29 lines
- `src/sim/agents/trafficBuildings.ts#TrafficFacts` — 3 of 12 lines
- `src/sim/agents/trafficBuildings.ts#factsOf` — 3 of 3 lines
- `src/sim/city/city.ts#City.step` — 3 of 6 lines
- `src/sim/host/protocol.ts#FromSim` — 12 of 12 lines
- `src/sim/host/protocol.ts#PedRich` — 1 of 1 lines
- `src/sim/host/protocol.ts#SimAct` — 7 of 7 lines
- `src/sim/host/protocol.ts#VehicleRich` — 3 of 10 lines
- `src/sim/host/protocol.ts#Wound` — 1 of 1 lines
- `src/sim/host/simClient.ts#PedRead` — 1 of 1 lines
- `src/sim/host/simClient.ts#SimClient.casualties` — 5 of 5 lines
- `src/sim/host/simClient.ts#SimClient.casualtyList` — 1 of 1 lines
- `src/sim/host/simClient.ts#SimClient.pedViews` — 1 of 1 lines
- `src/sim/host/simClient.ts#SimClient.readTick` — 18 of 160 lines
- `src/sim/host/simClient.ts#VehicleView` — 3 of 25 lines
- `src/sim/host/simHost.ts#PED_RICH` — 1 of 1 lines
- `src/sim/host/simHost.ts#SimHost.act` — 7 of 32 lines
- `src/sim/host/simHost.ts#SimHost.casualtiesSent` — 1 of 1 lines
- `src/sim/host/simHost.ts#SimHost.frame` — 6 of 39 lines
- `src/sim/host/simHost.ts#SimHost.handle` — 5 of 25 lines
- `src/sim/host/simHost.ts#SimHost.init` — 6 of 32 lines
- `src/sim/host/simHost.ts#SimHost.load` — 4 of 12 lines
- `src/sim/host/simHost.ts#SimHost.parked` — 19 of 21 lines
- `src/sim/host/simHost.ts#SimHost.publish` — 20 of 80 lines
- `src/sim/host/simHost.ts#pedDiffers` — 3 of 4 lines
- `src/sim/host/simHost.ts#pedRich` — 4 of 4 lines
- `src/sim/host/simHost.ts#pedRichKey` — 3 of 3 lines
- `src/sim/intersections/admission.ts#admit` — 10 of 117 lines
- `src/sim/intersections/admission.ts#crossingBusy` — 5 of 6 lines
- `src/sim/intersections/admission.ts#crossingReachedFirst` — 18 of 28 lines
- `src/sim/intersections/admission.ts#pedestrianHasPriority` — 19 of 32 lines
- `src/sim/intersections/admission.ts#stoppedForWalker` — 3 of 5 lines
- `src/sim/invariants.ts#runAudit` — 26 of 225 lines
- `src/sim/meso/demand.ts#KINDS` — 1 of 1 lines
- `src/sim/pipeline.ts#rebindPeds` — 3 of 3 lines
- `src/sim/pipeline.ts#step` — 7 of 100 lines
- `src/sim/pose.ts#pedPoseInto` — 7 of 8 lines
- `src/sim/vehicles/kerbStops.ts#KerbPerson` — 1 of 1 lines
- `src/sim/vehicles/kerbStops.ts#SeatPerson` — 1 of 1 lines
- `src/sim/vehicles/kerbStops.ts#Seated` — 1 of 1 lines
- `src/sim/vehicles/kerbStops.ts#completeTransfer` — 17 of 29 lines
- `src/sim/vehicles/kerbStops.ts#footwayBeside` — 13 of 20 lines
- `src/sim/vehicles/kerbStops.ts#hail` — 9 of 13 lines
- `src/sim/vehicles/kerbStops.ts#plan` — 9 of 60 lines
- `src/sim/vehicles/kerbStops.ts#planBusStop` — 8 of 33 lines
- `src/sim/vehicles/kerbStops.ts#roomToOpen` — 5 of 20 lines
- `src/sim/vehicles/kerbStops.ts#seatAgeClass` — 3 of 7 lines
- `src/sim/vehicles/kerbStops.ts#seatPersonInto` — 5 of 13 lines
- `src/sim/vehicles/kerbStops.ts#stepKerbStops` — 12 of 84 lines
- `src/sim/vehicles/obstacles.ts#longitudinalConstraints` — 15 of 104 lines
- `src/sim/vehicles/obstacles.ts#pedestrianAhead` — 13 of 37 lines
- `src/sim/vehicles/spawn.ts#spawnAt` — 5 of 60 lines
- `src/sim/vehicles/spawn.ts#stepDespawn` — 8 of 31 lines
- `src/sim/vehicles/spawn.ts#stepDispatch` — 3 of 61 lines
- `src/sim/vehicles/state.ts#Vehicle` — 3 of 115 lines
- `src/sim/world.ts#NO_PEDESTRIANS` — 5 of 5 lines
- `src/sim/world.ts#SimWorld.ambient` — 1 of 1 lines
- `src/sim/world.ts#SimWorld.crossingSpans` — 1 of 1 lines
- `src/sim/world.ts#SimWorld.crossingStates` — 1 of 1 lines
- `src/sim/world.ts#SimWorld.crossingsAt` — 7 of 10 lines
- `src/sim/world.ts#SimWorld.junctionQuiet` — 7 of 12 lines
- `src/sim/world.ts#SimWorld.makeSignalDeps` — 11 of 72 lines
- `src/sim/world.ts#SimWorld.pedEngine` — 1 of 1 lines
- `src/sim/world.ts#SimWorld.pedViewById` — 1 of 1 lines
- `src/sim/world.ts#SimWorld.pedViews` — 1 of 1 lines
- `src/sim/world.ts#SimWorld.prepareVehicleTopology` — 3 of 29 lines
- `src/sim/world.ts#SimWorld.rebuildWalkTopology` — 4 of 15 lines
- `src/sim/world.ts#SimWorld.reset` — 5 of 45 lines
- `src/sim/world.ts#SimWorld.sidewalks` — 1 of 1 lines
- `src/sim/world.ts#SimWorld.signalPosts` — 1 of 1 lines
- `src/sim/world.ts#SimWorld.usePedestrianEngine` — 4 of 7 lines
- `src/sim/world.ts#SimWorld.walkways` — 1 of 1 lines
- `src/sim/world.ts#SimWorld.walkwaysNow` — 3 of 3 lines
- `src/world/conflictTable.ts#PairMeasurer` — 2 of 12 lines
- `src/world/curbRamps.ts#rampsAt.pair` — 3 of 18 lines
- `src/world/doc.ts#RoadDoc.fromJSON` — 8 of 187 lines
- `src/world/doc.ts#RoadDoc.people` — 1 of 1 lines
- `src/world/doc.ts#RoadDoc.replaceContents` — 6 of 149 lines
- `src/world/doc.ts#RoadDoc.worldPart` — 5 of 26 lines
- `src/world/landscape.ts#footwayAt` — 5 of 31 lines
- `src/world/lots.ts#plannedStreet` — 2 of 2 lines
- `src/world/network.ts#Network.crossingSurface` — 3 of 3 lines
- `src/world/network.ts#Network.crossingWalkable` — 1 of 1 lines
- `src/world/network.ts#Network.crosswalkDistanceAt.fits` — 4 of 6 lines
- `src/world/paving.ts#PavingData` — 3 of 5 lines
- `src/world/paving.ts#pavingReader` — 3 of 23 lines
- `src/world/poleLines.ts#polesInclude` — 6 of 7 lines
- `src/world/roads/furnitureSets.ts#furnitureFor` — 6 of 94 lines
- `src/world/section.ts#sectionOf` — 6 of 11 lines

## Resources

- feature entry `female` — src/peopleLab/choices.ts:71: `female: ['long01', 'bob02', 'ponytail01', 'braid01', 'bob01']`
- feature entry `male` — src/peopleLab/choices.ts:71: `male: ['short02', 'short01', 'short04', 'short03']`
- feature entry `female` — src/peopleLab/choices.ts:75: `female: ['long01', 'bob02', 'bob01', 'ponytail01', 'braid01', 'culturalibre_hair_01', 'culturalibre_hair_18', 'culturalibre_hair_17', 'cortu_straight_bangs', 'e`
- feature entry `male` — src/peopleLab/choices.ts:80: `male: ['short02', 'short01', 'short03', 'short04', 'culturalibre_hair_02', 'culturalibre_hair_05', 'culturalibre_hair_14', 'elvs_maxwell_hair', 'elvs_short_side`
- feature entry `female` — src/peopleLab/choices.ts:86: `female: ['elvs_flatshoe_pointy1', 'elvs_flatshoe_gemmed1', 'elvs_flatshoe_bow1', 'elvs_flatshoe_plain1', 'toigo_ballet_flats', 'toigo_ballet_flats_with_bows', '`
- feature entry `male` — src/peopleLab/choices.ts:89: `male: ['shoes02', 'shoes01', 'shoes04', 'shoes05', 'shoes06', 'mindfront_shoes_oxford_male', 'mindfront_shoes_monk_strap_male', 'callharvey3d_harvey_saddleshoes`
- feature entry `style` — src/peopleLab/icons.ts:6: `style: 'M12 3l1.7 4.6L18 9.3l-4.3 1.7L12 15.6l-1.7-4.6L6 9.3l4.3-1.7zM18 15l.8 2.2L21 18l-2.2.8L18 21l-.8-2.2L15 18l2.2-.8z'`
- shared entry `sculpt` — src/peopleLab/icons.ts:11: `sculpt: 'M14.5 4.5l5 5-9 9H5.5v-5zM13 6l5 5M4 21h16'`
- shared entry `trash` — src/peopleLab/icons.ts:27: `trash: 'M5 7h14M10 7V4h4v3M7 7l1 13h8l1-13'`
- feature entry `tronco` — src/peopleLab/panels.ts:65: `tronco: [['measure-shoulder-dist-decr-incr', 'Ombros'], ['measure-bust-circ-decr-incr', 'Peito'], ['measure-underbust-circ-decr-incr', 'Sob o busto'], ['measure`
- feature entry `bracos` — src/peopleLab/panels.ts:69: `bracos: [['measure-upperarm-length-decr-incr', 'Comprimento'], ['measure-upperarm-circ-decr-incr', 'Braço'], ['upperarm-muscle-decr-incr', 'Bíceps'], ['upperarm`
- feature entry `pernas` — src/peopleLab/panels.ts:71: `pernas: [['upperlegs-height-decr-incr', 'Coxas (altura)'], ['lowerlegs-height-decr-incr', 'Canelas (altura)'], ['measure-thigh-circ-decr-incr', 'Coxas'], ['meas`
- feature entry `eyes` — src/peopleLab/panels.ts:77: `eyes: 'eyes'`
- feature entry `brows` — src/peopleLab/panels.ts:77: `brows: 'brows'`
- feature entry `nose` — src/peopleLab/panels.ts:77: `nose: 'nose'`
- feature entry `mouth` — src/peopleLab/panels.ts:77: `mouth: 'mouth'`
- feature entry `cheeks` — src/peopleLab/panels.ts:77: `cheeks: 'cheeks'`
- feature entry `jaw` — src/peopleLab/panels.ts:77: `jaw: 'jaw'`
- feature entry `ears` — src/peopleLab/panels.ts:77: `ears: 'ears'`
- shared entry `female` — src/peopleLab/presets.ts:217: `female: { ...SHARED_FACE, 'nose-scale-horiz-decr-incr': -0.25, 'nose-scale-vert-decr-incr': -0.2, 'nose-volume-decr-incr': -0.2, 'chin-width-decr-incr': -0.3, '`
- shared entry `male` — src/peopleLab/presets.ts:219: `male: { ...SHARED_FACE, 'eye-scale-decr-incr': 0.25, 'chin-width-decr-incr': 0.3, 'chin-bones-decr-incr': 0.25, 'head-square': 0.3, 'mouth-angles-down-up': 0.1 `
- feature entry `female` — src/peopleLab/proportions.ts:213: `female: { heads: 6.3, shoulders: 1.15, neck: 0.48 }`
- feature entry `male` — src/peopleLab/proportions.ts:213: `male: { heads: 6.5, shoulders: 1.35, neck: 0.66 }`
- shared entry `torso` — src/render/ragdoll.ts:99: `torso: { origin: PEL, to: CHE, ref: 'side' }`
- shared entry `neck` — src/render/ragdoll.ts:100: `neck: { origin: NEC, to: HEA, ref: 'side' }`
- shared entry `head` — src/render/ragdoll.ts:101: `head: { origin: HEA, to: TOP, ref: 'side' }`
- shared entry `lua` — src/render/ragdoll.ts:102: `lua: { origin: LS, to: LE, ref: 'larm' }`
- shared entry `lfa` — src/render/ragdoll.ts:102: `lfa: { origin: LE, to: LW, ref: 'larm' }`
- shared entry `rua` — src/render/ragdoll.ts:103: `rua: { origin: RS, to: RE, ref: 'rarm' }`
- shared entry `rfa` — src/render/ragdoll.ts:103: `rfa: { origin: RE, to: RW, ref: 'rarm' }`
- shared entry `lth` — src/render/ragdoll.ts:104: `lth: { origin: LH, to: LK, ref: 'lleg' }`
- shared entry `lca` — src/render/ragdoll.ts:104: `lca: { origin: LK, to: LA, ref: 'lleg' }`
- shared entry `lfo` — src/render/ragdoll.ts:104: `lfo: { origin: LA, to: LT, ref: 'lleg' }`
- shared entry `rth` — src/render/ragdoll.ts:105: `rth: { origin: RH, to: RK, ref: 'rleg' }`
- shared entry `rca` — src/render/ragdoll.ts:105: `rca: { origin: RK, to: RA, ref: 'rleg' }`
- shared entry `rfo` — src/render/ragdoll.ts:105: `rfo: { origin: RA, to: RT, ref: 'rleg' }`
- feature entry `torso` — src/render/ragdoll.ts:172: `torso: J_TORSO`
- feature entry `neck` — src/render/ragdoll.ts:172: `neck: J_HEAD`
- feature entry `head` — src/render/ragdoll.ts:172: `head: J_HEAD`
- feature entry `lua` — src/render/ragdoll.ts:172: `lua: J_LUA`
- feature entry `lfa` — src/render/ragdoll.ts:172: `lfa: J_LFA`
- feature entry `rua` — src/render/ragdoll.ts:172: `rua: J_RUA`
- feature entry `rfa` — src/render/ragdoll.ts:172: `rfa: J_RFA`
- feature entry `lth` — src/render/ragdoll.ts:173: `lth: J_LTH`
- feature entry `lca` — src/render/ragdoll.ts:173: `lca: J_LCA`
- feature entry `lfo` — src/render/ragdoll.ts:173: `lfo: J_LFO`
- feature entry `rth` — src/render/ragdoll.ts:173: `rth: J_RTH`
- feature entry `rca` — src/render/ragdoll.ts:173: `rca: J_RCA`
- feature entry `rfo` — src/render/ragdoll.ts:173: `rfo: J_RFO`
- feature entry `hint.shoot.miss` — src/ui/i18n/en.ts:31: `'hint.shoot.miss': 'Nobody there: click a person.'`
- feature entry `hint.strike.hit` — src/ui/i18n/en.ts:908: `'hint.strike.hit': 'Hit: keep striking to bring it down'`
- feature entry `hint.strike.down` — src/ui/i18n/en.ts:909: `'hint.strike.down': 'The building came down'`
- feature entry `hint.strike.deaths` — src/ui/i18n/en.ts:910: `'hint.strike.deaths': 'Hit: there were casualties'`
- feature entry `hint.strike.ground` — src/ui/i18n/en.ts:911: `'hint.strike.ground': 'Struck the ground'`
- feature entry `hint.shoot.miss` — src/ui/i18n/pt-BR.ts:31: `'hint.shoot.miss': 'Ninguém aí: clique numa pessoa.'`
- feature entry `hint.strike.hit` — src/ui/i18n/pt-BR.ts:901: `'hint.strike.hit': 'Atingido: continue batendo para derrubar'`
- feature entry `hint.strike.down` — src/ui/i18n/pt-BR.ts:902: `'hint.strike.down': 'Construção desabou'`
- feature entry `hint.strike.deaths` — src/ui/i18n/pt-BR.ts:903: `'hint.strike.deaths': 'Atingido: houve vítimas'`
- feature entry `hint.strike.ground` — src/ui/i18n/pt-BR.ts:904: `'hint.strike.ground': 'Impacto no chão'`
- feature import `virtual:model-urls/people/proxies?ext=json,bin,webp` — src/people/body/proxy.ts:199: `import URLS from 'virtual:model-urls/people/proxies?ext=json,bin,webp';`
- feature import `../../public/models/people/proxies/community.json` — src/people/wardrobe.ts:2: `import community from '../../public/models/people/proxies/community.json';`
- feature import `./wardrobeSelection.json` — src/people/wardrobe.ts:3: `import selection from './wardrobeSelection.json';`
- feature import `./wardrobeSex.json` — src/people/wardrobe.ts:4: `import sexes from './wardrobeSex.json';`
- feature import `../../../public/models/people/skins/index.json` — src/render/people/skinAppearance.ts:8: `import index from '../../../public/models/people/skins/index.json';`
- feature import `virtual:model-urls/people/skins?ext=webp` — src/render/people/skinAppearance.ts:10: `import urls from 'virtual:model-urls/people/skins?ext=webp';`
- shared style `row` — src/peopleLab/peopleLab.css:45: `.row { display: grid; grid-template-columns: 96px 1fr 38px; align-items: center; gap: 8px; min-height: 28px; color: var(--ink-2); }`
- shared style `row` — src/peopleLab/peopleLab.css:46: `.row output { text-align: right; color: var(--ink-3); font-variant-numeric: tabular-nums; font-size: 11px; }`
- shared style `grid` — src/peopleLab/peopleLab.css:61: `.grid { display: grid; grid-template-columns: repeat(3, 1fr); gap: 6px; }`
- shared style `tile` — src/peopleLab/peopleLab.css:62: `.tile { border: 1px solid var(--line); border-radius: var(--radius); background: var(--panel-2); padding: 9px 6px; cursor: pointer; display: flex; flex-directio`
- shared style `tile` — src/peopleLab/peopleLab.css:65: `.tile img { width: 56px; height: 56px; object-fit: contain; border-radius: 6px; }`
- shared style `tile` — src/peopleLab/peopleLab.css:66: `.tile:hover { border-color: #3d434b; color: var(--ink); }`
- shared style `tile` — src/peopleLab/peopleLab.css:67: `.tile.on { border-color: var(--accent); color: var(--ink); background: var(--accent-soft); }`
- shared style `toggle` — src/peopleLab/peopleLab.css:104: `.toggle { width: 32px; height: 30px; display: grid; place-items: center; border: 1px solid var(--line); border-radius: 7px; background: var(--panel-2); color: v`
- shared style `toggle` — src/peopleLab/peopleLab.css:105: `.toggle.on { color: var(--ink); border-color: var(--accent); box-shadow: inset 0 0 0 1px var(--accent); }`
- shared style `top` — src/peopleLab/peopleLab.css:121: `.top { position: absolute; left: 14px; right: 14px; top: 12px; display: flex; align-items: center; gap: 10px; pointer-events: none; }`
- shared style `top` — src/peopleLab/peopleLab.css:122: `.top > * { pointer-events: auto; }`
- shared style `card` — src/peopleLab/peopleLab.css:146: `.card { position: relative; border-radius: var(--radius); background: var(--panel-2); border: 1px solid var(--line); cursor: pointer; padding: 3px; text-align: `
- shared style `card` — src/peopleLab/peopleLab.css:147: `.card img { width: 100%; aspect-ratio: 1; object-fit: cover; border-radius: 6px; background: #2a2f37; display: block; margin-bottom: 3px; }`
- shared style `card` — src/peopleLab/peopleLab.css:148: `.card.on { border-color: var(--accent); color: var(--ink); }`
- shared style `card` — src/peopleLab/peopleLab.css:149: `.card .x { position: absolute; top: 5px; right: 5px; width: 18px; height: 18px; border-radius: 4px; border: 0; background: rgba(0,0,0,.55); color: #fff; cursor:`
- shared style `card` — src/peopleLab/peopleLab.css:150: `.card:hover .x { opacity: 1; }`
- shared style `gap` — src/peopleLab/peopleLab.css:163: `.gap { height: 12px; }`
- shared style `xs-label` — src/ui/roads/roads.css:61: `.xs-label { fill: #e8f0ee; font: 600 12.5px Inter, ui-sans-serif, sans-serif; font-variant-numeric: tabular-nums; }`
- shared style `xs-glyph` — src/ui/roads/roads.css:63: `.xs-glyph { fill: #f2efe6; font: 800 13px Inter, ui-sans-serif, sans-serif; }`

## Where the program plugs the feature in

### src/people/body/assets.ts

- `src/people/body/assets.ts#loadPeopleAssets` (function, line 45) ← call from `src/peopleLab/build.ts#LabBuilder.setup`
- `src/people/body/assets.ts#PeopleAssets` (interface, line 15) ← read from `src/peopleLab/build.ts#hairBase`, `src/peopleLab/build.ts#LabBuilder.build`, `src/peopleLab/build.ts#LabBuilder.sculptBody`, `src/peopleLab/build.ts#LabBuilder.setup`, …
- `src/people/body/assets.ts#PeopleAssets` (interface, line 15) ← typeRef from `src/peopleLab/build.ts#hairBase`, `src/peopleLab/build.ts#LabBuilder.assets`, `src/peopleLab/build.ts#LabBuilder.grownHair`, `src/peopleLab/build.ts#LabBuilder.setup`, …

### src/people/body/expressions.ts

- `src/people/body/expressions.ts#expressionShapes` (function, line 20) ← call from `src/peopleLab/build.ts#LabBuilder.build`, `src/peopleLab/faceRig.ts#attachFace`

### src/people/body/macro.ts

- `src/people/body/macro.ts#ageFromYears` (function, line 60) ← call from `src/peopleLab/main.ts#startLab`, `src/peopleLab/panels.ts#makePanels`, `src/render/agents.ts#createAgentMeshes.procBuildNext`
- `src/people/body/macro.ts#MacroParams` (interface, line 4) ← read from `src/peopleLab/bodyFit.ts#fitBody`, `src/peopleLab/build.ts#LabBuilder.build`, `src/peopleLab/main.ts#startLab.female`, `src/peopleLab/main.ts#startLab.freshPerson`, …
- `src/people/body/macro.ts#MacroParams` (interface, line 4) ← typeRef from `src/peopleLab/main.ts#startLab.setBody`, `src/peopleLab/main.ts#startLab.sliderOf`, `src/peopleLab/panels.ts#PanelContext`, `src/peopleLab/presets.ts#BodyPreset`, …
- `src/people/body/macro.ts#yearsFromAge` (function, line 69) ← call from `src/peopleLab/panels.ts#makePanels`, `src/peopleLab/panels.ts#makePanels.child`, `src/peopleLab/record.ts#rosterMarkdown`, `src/peopleLab/shapeKeys.ts#effective`

### src/people/body/morph.ts

- `src/people/body/morph.ts#Morpher` (class, line 58) ← construct from `src/peopleLab/build.ts#LabBuilder.setup`
- `src/people/body/morph.ts#Morpher` (class, line 58) ← typeRef from `src/peopleLab/build.ts#hairBase`, `src/peopleLab/build.ts#LabBuilder.grownHair`, `src/peopleLab/build.ts#LabBuilder.morpher`, `src/peopleLab/build.ts#LabBuilder.setup`, …
- `src/people/body/morph.ts#Morpher.base` (field, line 61) ← read from `src/peopleLab/build.ts#hairBase`, `src/peopleLab/proportions.ts#FaceProportions.landmark`
- `src/people/body/morph.ts#Morpher.shape` (method, line 129) ← call from `src/peopleLab/build.ts#LabBuilder.build`, `src/peopleLab/build.ts#LabBuilder.proportions`, `src/peopleLab/build.ts#LabBuilder.shape`, `src/peopleLab/proportions.ts#FaceProportions.influenceOf`
- `src/people/body/morph.ts#Morpher.vertexCount` (field, line 59) ← read from `src/peopleLab/proportions.ts#FaceProportions.influenceOf`

### src/people/body/proxy.ts

- `src/people/body/proxy.ts#fitProxy` (function, line 33) ← call from `src/peopleLab/hairShape.ts#shapedPack`, `src/peopleLab/sculpt.ts#HairSculptor.constructor`
- `src/people/body/proxy.ts#loadProxyItem` (function, line 159) ← call from `src/peopleLab/build.ts#LabBuilder.build`, `src/peopleLab/build.ts#LabBuilder.hairItem`, `src/peopleLab/build.ts#LabBuilder.item`
- `src/people/body/proxy.ts#ProxyItem` (interface, line 128) ← read from `src/peopleLab/build.ts#LabBuilder.build`, `src/peopleLab/build.ts#LabBuilder.shaped`, `src/peopleLab/main.ts#startLab.openSculptor`
- `src/people/body/proxy.ts#ProxyItem` (interface, line 128) ← typeRef from `src/peopleLab/build.ts#LabBuilder.build`, `src/peopleLab/build.ts#LabBuilder.grown`, `src/peopleLab/build.ts#LabBuilder.grownHair`, `src/peopleLab/build.ts#LabBuilder.hairItem`, …
- `src/people/body/proxy.ts#ProxyPack` (interface, line 6) ← read from `src/peopleLab/build.ts#LabBuilder.build`, `src/peopleLab/hairShape.ts#axisScales.axis`, `src/peopleLab/hairShape.ts#shapedPack`, `src/peopleLab/main.ts#startLab.openSculptor`, …
- `src/people/body/proxy.ts#ProxyPack` (interface, line 6) ← typeRef from `src/peopleLab/hairShape.ts#axisScales`, `src/peopleLab/hairShape.ts#shapedPack`, `src/peopleLab/sculpt.ts#axisScales`, `src/peopleLab/sculpt.ts#HairSculptor.constructor`, …
- `src/people/body/proxy.ts#ProxyTexture` (interface, line 120) ← read from `src/peopleLab/sculpt.ts#HairSculptor.constructor`
- `src/people/body/proxy.ts#ProxyTexture` (interface, line 120) ← typeRef from `src/peopleLab/sculpt.ts#HairSculptor.constructor`
- `src/people/body/proxy.ts#proxyUrl` (function, line 201) ← call from `src/peopleLab/widgets.ts#widgets.thumbOf`

### src/people/hair/procedural.ts

- `src/people/hair/procedural.ts#generateHair` (function, line 246) ← call from `src/peopleLab/build.ts#LabBuilder.grownHair`
- `src/people/hair/procedural.ts#HairBase` (interface, line 79) ← typeRef from `src/peopleLab/build.ts#hairBase`
- `src/people/hair/procedural.ts#HairStyle` (interface, line 6) ← typeRef from `src/peopleLab/build.ts#LabBuilder.grownHair`, `src/peopleLab/record.ts#LabPerson`
- `src/people/hair/procedural.ts#nearestThree` (function, line 145) ← call from `src/peopleLab/hairShape.ts#shapedPack`

### src/people/spec.ts

- `src/people/spec.ts#CLOTH_COLOURS` (variable, line 116) ← read from `src/peopleLab/choices.ts#CLOTH`
- `src/people/spec.ts#EYE_COLOURS` (variable, line 115) ← read from `src/peopleLab/panels.ts#makePanels`
- `src/people/spec.ts#normalizePerson` (function, line 307) ← call from `src/world/doc.ts#RoadDoc.fromJSON`
- `src/people/spec.ts#PersonLook` (interface, line 22) ← read from `src/peopleLab/build.ts#LabBuilder.build`, `src/peopleLab/build.ts#LabBuilder.hairItem`, `src/peopleLab/build.ts#LabBuilder.item`, `src/peopleLab/build.ts#LabBuilder.shaped`, …
- `src/people/spec.ts#PersonLook` (interface, line 22) ← typeRef from `src/peopleLab/main.ts#startLab`, `src/peopleLab/main.ts#startLab.setLook`, `src/peopleLab/panels.ts#makePanels.withLook`, `src/peopleLab/panels.ts#PanelContext`, …
- `src/people/spec.ts#PersonSpec` (interface, line 7) ← read from `src/peopleLab/bodyFit.ts#fitBody`, `src/peopleLab/bodyFit.ts#fitBody.at`, `src/peopleLab/bodyFit.ts#fitBody.withF`, `src/peopleLab/build.ts#LabBuilder.build`, …
- `src/people/spec.ts#PersonSpec` (interface, line 7) ← typeRef from `src/peopleLab/dress.ts#dressPerson`, `src/peopleLab/main.ts#startLab`, `src/peopleLab/main.ts#startLab.setSpec`, `src/peopleLab/panels.ts#PanelContext`, …
- `src/people/spec.ts#randomPerson` (function, line 129) ← call from `src/peopleLab/main.ts#startLab`, `src/peopleLab/main.ts#startLab.freshPerson`, `src/render/agents.ts#createAgentMeshes`, `src/render/agents.ts#createAgentMeshes.procBuildNext`
- `src/people/spec.ts#WARDROBE` (variable, line 54) ← read from `src/peopleLab/panels.ts#makePanels`
- `src/people/spec.ts#wornItems` (function, line 88) ← call from `src/peopleLab/build.ts#LabBuilder.build`

### src/people/wardrobe.ts

- `src/people/wardrobe.ts#communityItem` (function, line 102) ← call from `src/peopleLab/main.ts#startLab`, `src/peopleLab/main.ts#startLab.fits`, `src/peopleLab/main.ts#startLab.kindOf`, `src/peopleLab/widgets.ts#widgets.pretty`
- `src/people/wardrobe.ts#itemsOf` (function, line 112) ← call from `src/peopleLab/panels.ts#makePanels`, `src/peopleLab/panels.ts#makePanels.extraPics`, `src/peopleLab/panels.ts#makePanels.opts`
- `src/people/wardrobe.ts#WardrobeItem` (interface, line 15) ← read from `src/peopleLab/main.ts#startLab`, `src/peopleLab/main.ts#startLab.fits`, `src/peopleLab/main.ts#startLab.kindOf`
- `src/people/wardrobe.ts#WardrobeItem` (interface, line 15) ← typeRef from `src/peopleLab/main.ts#startLab.extraOf`, `src/peopleLab/main.ts#startLab.kindOf`, `src/peopleLab/main.ts#startLab.setExtra`, `src/peopleLab/panels.ts#makePanels.extraPics`, …

### src/render/blobShadows.ts

- `src/render/blobShadows.ts#blobInto` (function, line 54) ← call from `src/render/agents.ts#createAgentMeshes.placeBlob`
- `src/render/blobShadows.ts#blobMesh` (function, line 21) ← call from `src/render/agents.ts#createAgentMeshes`

### src/render/citizenBake.ts

- `src/render/citizenBake.ts#CitizenClipKey` (type, line 51) ← typeRef from `src/render/agents.ts#createAgentMeshes`, `src/render/agents.ts#createAgentMeshes.placeCarOccupants`, `src/render/agents.ts#createAgentMeshes.riderPlay`

### src/render/citizenBake.worker.ts

- `src/render/citizenBake.worker.ts#<ExpressionStatement>` (statement, line 20) ← moduleLoad from `src/job.worker.ts#LOADERS`
- `src/render/citizenBake.worker.ts#BakedClip` (type, line 54) ← moduleLoad from `src/job.worker.ts#LOADERS`
- `src/render/citizenBake.worker.ts#bakeEnv` (variable, line 67) ← moduleLoad from `src/job.worker.ts#LOADERS`
- `src/render/citizenBake.worker.ts#bakeJob` (function, line 100) ← moduleLoad from `src/job.worker.ts#LOADERS`
- `src/render/citizenBake.worker.ts#BakeReply` (interface, line 56) ← moduleLoad from `src/job.worker.ts#LOADERS`
- `src/render/citizenBake.worker.ts#classRigOf` (function, line 69) ← moduleLoad from `src/job.worker.ts#LOADERS`
- `src/render/citizenBake.worker.ts#classRigs` (variable, line 68) ← moduleLoad from `src/job.worker.ts#LOADERS`
- `src/render/citizenBake.worker.ts#Job` (interface, line 23) ← moduleLoad from `src/job.worker.ts#LOADERS`
- `src/render/citizenBake.worker.ts#libraries` (variable, line 86) ← moduleLoad from `src/job.worker.ts#LOADERS`
- `src/render/citizenBake.worker.ts#morpher` (variable, line 65) ← moduleLoad from `src/job.worker.ts#LOADERS`
- `src/render/citizenBake.worker.ts#PersonFaceJob` (interface, line 44) ← moduleLoad from `src/job.worker.ts#LOADERS`
- `src/render/citizenBake.worker.ts#sent` (function, line 94) ← moduleLoad from `src/job.worker.ts#LOADERS`
- `src/render/citizenBake.worker.ts#serve` (function, line 89) ← callbackAssign from `src/bays.worker.ts#serve`, `src/cityGen.worker.ts#serve`, `src/conflictTable.worker.ts#serve`, `src/editor/save.worker.ts#serve`, …
- `src/render/citizenBake.worker.ts#serve` (function, line 89) ← moduleLoad from `src/job.worker.ts#LOADERS`

### src/render/citizenCasting.ts

- `src/render/citizenCasting.ts#Company` (type, line 83) ← typeRef from `src/render/agents.ts#createAgentMeshes.vehicleCompany`
- `src/render/citizenCasting.ts#CROWD_IDS` (variable, line 59) ← read from `src/render/agents.ts#createAgentMeshes`

### src/render/citizenStride.ts

- `src/render/citizenStride.ts#directionalWalkFor.pose` (method, line 47) ← implementation from `src/peopleLab/pose.ts#ClipPlayer.apply`

### src/render/citizenWalk.ts

- `src/render/citizenWalk.ts#captureBind` (function, line 119) ← call from `src/peopleLab/build.ts#LabBuilder.build`
- `src/render/citizenWalk.ts#captureBindRotations` (function, line 125) ← call from `src/peopleLab/build.ts#LabBuilder.build`
- `src/render/citizenWalk.ts#clipTransferFor` (function, line 113) ← call from `src/peopleLab/pose.ts#ClipPlayer.play`
- `src/render/citizenWalk.ts#LibraryClip` (interface, line 305) ← read from `src/peopleLab/pose.ts#ClipPlayer.apply`
- `src/render/citizenWalk.ts#LibraryClip` (interface, line 305) ← typeRef from `src/peopleLab/pose.ts#ClipPlayer.clip`
- `src/render/citizenWalk.ts#LibraryClipName` (type, line 287) ← typeRef from `src/peopleLab/choices.ts#MOODS`, `src/peopleLab/choices.ts#READY_POSES`, `src/peopleLab/main.ts#startLab`, `src/peopleLab/panels.ts#PanelContext`, …
- `src/render/citizenWalk.ts#loadRocketboxClips` (function, line 371) ← call from `src/peopleLab/main.ts#startLab.runBuild`
- `src/render/citizenWalk.ts#loadRocketboxClips` (function, line 371) ← typeRef from `src/peopleLab/main.ts#startLab`
- `src/render/citizenWalk.ts#NeutralWalk` (interface, line 97) ← call from `src/peopleLab/pose.ts#ClipPlayer.apply`
- `src/render/citizenWalk.ts#NeutralWalk` (interface, line 97) ← typeRef from `src/peopleLab/pose.ts#ClipPlayer.transfer`
- `src/render/citizenWalk.ts#RocketboxClips` (type, line 319) ← typeRef from `src/peopleLab/pose.ts#ClipPlayer.constructor`
- `src/render/citizenWalk.ts#transferOnto.pose` (method, line 253) ← implementation from `src/peopleLab/pose.ts#ClipPlayer.apply`

### src/render/people/cartoon.ts

- `src/render/people/cartoon.ts#cartoonBody` (function, line 43) ← call from `src/peopleLab/build.ts#LabBuilder.build`, `src/peopleLab/build.ts#LabBuilder.shape`
- `src/render/people/cartoon.ts#CartoonStyle` (interface, line 5) ← read from `src/peopleLab/bodyFit.ts#fitBody`, `src/peopleLab/bodyFit.ts#fitBody.at`, `src/peopleLab/panels.ts#makePanels`, `src/peopleLab/record.ts#readRoster`
- `src/render/people/cartoon.ts#CartoonStyle` (interface, line 5) ← typeRef from `src/peopleLab/build.ts#labStyle`, `src/peopleLab/choices.ts#ROADCRAFT`, `src/peopleLab/choices.ts#STYLE_PRESETS`, `src/peopleLab/main.ts#startLab.setStyle`, …
- `src/render/people/cartoon.ts#PIXAR` (variable, line 20) ← read from `src/peopleLab/choices.ts#STYLE_PRESETS`
- `src/render/people/cartoon.ts#REALISTIC` (variable, line 17) ← read from `src/peopleLab/choices.ts#STYLE_PRESETS`

### src/render/people/crowdAnimation.ts

- `src/render/people/crowdAnimation.ts#buildClassAnimation.texelsInto` (function, line 58) ← indirectCall from `src/render/stagedTexels.ts#stageTexels`

### src/render/people/faceExpression.ts

- `src/render/people/faceExpression.ts#CHANNELS` (variable, line 7) ← read from `src/peopleLab/faceRig.ts#applyLabFace`
- `src/render/people/faceExpression.ts#channelShapes` (function, line 25) ← call from `src/peopleLab/build.ts#LabBuilder.build`
- `src/render/people/faceExpression.ts#faceAt` (function, line 57) ← call from `src/peopleLab/main.ts#startLab.loop`

### src/render/people/facialMorphs.ts

- `src/render/people/facialMorphs.ts#attachFacialMorphs` (function, line 5) ← call from `src/peopleLab/build.ts#LabBuilder.build`

### src/render/people/garmentPadding.worker.ts

- `src/render/people/garmentPadding.worker.ts#serve` (function, line 5) ← moduleLoad from `src/job.worker.ts#LOADERS`

### src/render/people/heldProps.ts

- `src/render/people/heldProps.ts#createHeldProps` (function, line 101) ← implementation from `src/render/shadowCasters.ts#castCastersInto`

### src/render/people/lodPool.ts

- `src/render/people/lodPool.ts#lodPool` (function, line 9) ← call from `src/render/vehicleLevels.ts#partLevels`
- `src/render/people/lodPool.ts#lodPool` (function, line 9) ← implementation from `src/render/vehicleLevels.ts#partLevels`
- `src/render/people/lodPool.ts#lodPool` (function, line 9) ← reference from `src/render/agents.ts#<ExpressionStatement>@76`

### src/render/people/lodPort.ts

- `src/render/people/lodPort.ts#LodPool` (interface, line 7) ← call from `src/render/vehicleLevels.ts#partLevels`
- `src/render/people/lodPort.ts#providePeopleLod` (function, line 18) ← call from `src/render/agents.ts#<ExpressionStatement>@76`

### src/render/people/lodWorker.ts

- `src/render/people/lodWorker.ts#IndexRequest` (interface, line 17) ← moduleLoad from `src/job.worker.ts#LOADERS`
- `src/render/people/lodWorker.ts#LadderRequest` (interface, line 26) ← moduleLoad from `src/job.worker.ts#LOADERS`
- `src/render/people/lodWorker.ts#LodLevel` (interface, line 42) ← moduleLoad from `src/job.worker.ts#LOADERS`
- `src/render/people/lodWorker.ts#LodRequest` (interface, line 6) ← moduleLoad from `src/job.worker.ts#LOADERS`
- `src/render/people/lodWorker.ts#PartRequest` (interface, line 38) ← moduleLoad from `src/job.worker.ts#LOADERS`
- `src/render/people/lodWorker.ts#serve` (function, line 82) ← callbackAssign from `src/bays.worker.ts#serve`, `src/cityGen.worker.ts#serve`, `src/conflictTable.worker.ts#serve`, `src/editor/save.worker.ts#serve`, …
- `src/render/people/lodWorker.ts#serve` (function, line 82) ← moduleLoad from `src/job.worker.ts#LOADERS`
- `src/render/people/lodWorker.ts#simplifyIndex` (function, line 68) ← moduleLoad from `src/job.worker.ts#LOADERS`
- `src/render/people/lodWorker.ts#simplifyLevels` (function, line 47) ← moduleLoad from `src/job.worker.ts#LOADERS`

### src/render/people/personRig.ts

- `src/render/people/personRig.ts#createPersonRig` (function, line 181) ← call from `src/peopleLab/build.ts#LabBuilder.build`
- `src/render/people/personRig.ts#createPersonRig` (function, line 181) ← callbackAssign from `src/peopleLab/build.ts#LabBuilder.build`
- `src/render/people/personRig.ts#createPersonRig` (function, line 181) ← implementation from `src/peopleLab/build.ts#LabBuilder.build`
- `src/render/people/personRig.ts#PersonRig` (interface, line 118) ← read from `src/peopleLab/build.ts#LabBuilder.build`
- `src/render/people/personRig.ts#personSimplifier` (variable, line 785) ← read from `src/peopleLab/build.ts#LabBuilder.build`
- `src/render/people/personRig.ts#Reshaped` (interface, line 139) ← read from `src/peopleLab/reshape.ts#Reshaper.flat`
- `src/render/people/personRig.ts#Reshaped` (interface, line 139) ← typeRef from `src/peopleLab/build.ts#BuiltPerson`, `src/peopleLab/reshape.ts#Reshaper.constructor`

### src/render/people/proceduralCrowd.ts

- `src/render/people/proceduralCrowd.ts#createProceduralCrowd` (function, line 951) ← call from `src/render/agents.ts#createAgentMeshes`
- `src/render/people/proceduralCrowd.ts#createProceduralCrowd` (function, line 951) ← implementation from `src/render/agents.ts#createAgentMeshes`, `src/render/agents.ts#createAgentMeshes.animProbe`, `src/render/agents.ts#createAgentMeshes.meshProbe`, `src/render/agents.ts#createAgentMeshes.procBuildNext`, …
- `src/render/people/proceduralCrowd.ts#createProceduralCrowd.add` (method, line 2080) ← implementation from `src/render/agents.ts#createAgentMeshes`, `src/render/agents.ts#createAgentMeshes.procBuildNext`
- `src/render/people/proceduralCrowd.ts#createProceduralCrowd.clipDuration` (method, line 2449) ← implementation from `src/render/agents.ts#createAgentMeshes.procDraw`, `src/render/agents.ts#createAgentMeshes.procPose`
- `src/render/people/proceduralCrowd.ts#createProceduralCrowd.renderPalettes` (method, line 2414) ← implementation from `src/render/agents.ts#createAgentMeshes.renderPalettes`
- `src/render/people/proceduralCrowd.ts#createProceduralCrowd.stride` (method, line 2453) ← implementation from `src/render/agents.ts#createAgentMeshes.procDraw`, `src/render/agents.ts#createAgentMeshes.procPose`
- `src/render/people/proceduralCrowd.ts#createProceduralCrowd.update` (method, line 2308) ← implementation from `src/render/agents.ts#createAgentMeshes.sync`
- `src/render/people/proceduralCrowd.ts#createProceduralCrowd.warmClasses` (method, line 2463) ← implementation from `src/render/agents.ts#createAgentMeshes`
- `src/render/people/proceduralCrowd.ts#personProgramStandIns` (function, line 667) ← indirectCall from `src/render/shaderCatalogue.ts#createShaderCatalogue.compile`
- `src/render/people/proceduralCrowd.ts#personProgramStandIns` (function, line 667) ← reference from `src/render/renderer.ts#createSceneRenderer`
- `src/render/people/proceduralCrowd.ts#ProcClip` (type, line 231) ← typeRef from `src/render/agents.ts#AgentMeshes`, `src/render/agents.ts#createAgentMeshes`, `src/render/agents.ts#createAgentMeshes.procPose`, `src/render/agents.ts#gaitClip`
- `src/render/people/proceduralCrowd.ts#ProceduralCrowd` (interface, line 878) ← call from `src/render/agents.ts#createAgentMeshes`, `src/render/agents.ts#createAgentMeshes.animProbe`, `src/render/agents.ts#createAgentMeshes.meshProbe`, `src/render/agents.ts#createAgentMeshes.procBuildNext`, …
- `src/render/people/proceduralCrowd.ts#ProceduralCrowd` (interface, line 878) ← read from `src/render/agents.ts#createAgentMeshes`, `src/render/agents.ts#createAgentMeshes.animProbe`, `src/render/agents.ts#createAgentMeshes.meshProbe`, `src/render/agents.ts#createAgentMeshes.procBuildNext`, …
- `src/render/people/proceduralCrowd.ts#ProceduralPerson` (interface, line 233) ← read from `src/render/agents.ts#createAgentMeshes`, `src/render/agents.ts#createAgentMeshes.animProbe`, `src/render/agents.ts#createAgentMeshes.meshProbe`, `src/render/agents.ts#createAgentMeshes.procBuildNext`, …
- `src/render/people/proceduralCrowd.ts#ProceduralPerson` (interface, line 233) ← typeRef from `src/render/agents.ts#createAgentMeshes`, `src/render/agents.ts#createAgentMeshes.procOf`, `src/render/agents.ts#createAgentMeshes.procSpareFor`, `src/render/agents.ts#createAgentMeshes.procSuits`
- `src/render/people/proceduralCrowd.ts#ProceduralPerson` (interface, line 233) ← write from `src/render/agents.ts#createAgentMeshes`, `src/render/agents.ts#createAgentMeshes.procDraw`, `src/render/agents.ts#createAgentMeshes.procPose`

### src/render/people/skinAppearance.ts

- `src/render/people/skinAppearance.ts#applySkinAppearance` (function, line 377) ← call from `src/peopleLab/dress.ts#dressPerson`
- `src/render/people/skinAppearance.ts#createCardAtlas.fill` (function, line 98) ← indirectCall from `src/render/stagedTexels.ts#stageTexels`
- `src/render/people/skinAppearance.ts#createPersonLayers` (function, line 41) ← call from `src/peopleLab/main.ts#startLab`, `src/peopleLab/thumbs.ts#Thumbs.layers`
- `src/render/people/skinAppearance.ts#loadSkinAppearance` (function, line 292) ← call from `src/peopleLab/dress.ts#dressPerson`
- `src/render/people/skinAppearance.ts#PersonLayers` (interface, line 33) ← typeRef from `src/peopleLab/dress.ts#dressPerson`, `src/peopleLab/thumbs.ts#Thumbs.layers`
- `src/render/people/skinAppearance.ts#releaseSkinAppearance` (function, line 369) ← call from `src/peopleLab/main.ts#startLab.runBuild`
- `src/render/people/skinAppearance.ts#SkinAppearance` (interface, line 12) ← read from `src/peopleLab/dress.ts#dressPerson`
- `src/render/people/skinAppearance.ts#SkinAppearance` (interface, line 12) ← typeRef from `src/peopleLab/dress.ts#dressPerson`, `src/peopleLab/main.ts#startLab`
- `src/render/people/skinAppearance.ts#SkinAppearance` (interface, line 12) ← write from `src/peopleLab/dress.ts#dressPerson`

### src/render/riderPoses.ts

- `src/render/riderPoses.ts#BIKE_FIT` (variable, line 108) ← read from `src/render/vehicleModels.ts#bicycleModel`
- `src/render/riderPoses.ts#CAB_WHEEL` (variable, line 63) ← read from `src/render/carBody.ts#buildCarModel`, `src/render/vehicleModels.ts#buildBusModel`, `src/render/vehicleModels.ts#buildTruckModel`
- `src/render/riderPoses.ts#DRIVER_WHEEL` (variable, line 60) ← read from `src/render/carBody.ts#buildCarModel`
- `src/render/riderPoses.ts#gripPoint` (function, line 129) ← call from `src/render/vehicleModels.ts#gripInHead`
- `src/render/riderPoses.ts#HELMET_SEGMENTS` (variable, line 328) ← read from `src/render/agents.ts#createAgentMeshes`
- `src/render/riderPoses.ts#MOTO_FIT` (variable, line 98) ← read from `src/render/vehicleModels.ts#motorcycleModel`
- `src/render/riderPoses.ts#RiderClipKey` (type, line 5) ← typeRef from `src/render/occupants.ts#clipsFor`, `src/render/occupants.ts#OccupantClip`, `src/render/occupants.ts#occupantPlays`, `src/render/occupants.ts#SEAT_CLIPS`, …
- `src/render/riderPoses.ts#SEATED_IDLE` (variable, line 47) ← read from `src/render/occupants.ts#occupantPlays`
- `src/render/riderPoses.ts#STEER_FULL` (variable, line 121) ← read from `src/render/agents.ts#createAgentMeshes.placeRiderFigure`
- `src/render/riderPoses.ts#TwoWheelerFit` (interface, line 76) ← read from `src/render/agents.ts#createAgentMeshes.sync.drawVehicle`, `src/render/vehicleModels.ts#bicycleModel`, `src/render/vehicleModels.ts#gripInHead`, `src/render/vehicleModels.ts#motorcycleModel`
- `src/render/riderPoses.ts#TwoWheelerFit` (interface, line 76) ← typeRef from `src/render/vehicleModels.ts#gripInHead`, `src/render/vehicleModels.ts#handlebar`, `src/render/vehicleModels.ts#TwoWheelerModel`
- `src/render/riderPoses.ts#WheelSpec` (interface, line 52) ← read from `src/render/agents.ts#createAgentMeshes.placeCarOccupants`, `src/render/carBody.ts#buildCarModel`, `src/render/vehicleModels.ts#buildBusModel`, `src/render/vehicleModels.ts#buildTruckModel`
- `src/render/riderPoses.ts#WheelSpec` (interface, line 52) ← typeRef from `src/render/vehicleModels.ts#SteeringModel`

### src/render/riggedCitizens.ts

- `src/render/riggedCitizens.ts#ClipIdentity` (interface, line 141) ← read from `src/render/agents.ts#createAgentMeshes.placeCarOccupants`
- `src/render/riggedCitizens.ts#ClipIdentity` (interface, line 141) ← typeRef from `src/render/agents.ts#createAgentMeshes`, `src/render/agents.ts#createAgentMeshes.seatIdentity`
- `src/render/riggedCitizens.ts#ClipIdentity` (interface, line 141) ← write from `src/render/agents.ts#createAgentMeshes.seatIdentity`
- `src/render/riggedCitizens.ts#createRiggedCitizens` (function, line 339) ← call from `src/render/agents.ts#createAgentMeshes`, `src/render/agents.ts#createAgentMeshes.setBleed`, `src/render/agents.ts#createAgentMeshes.sync`
- `src/render/riggedCitizens.ts#createRiggedCitizens` (function, line 339) ← read from `src/render/agents.ts#createAgentMeshes`
- `src/render/riggedCitizens.ts#createRiggedCitizens` (function, line 339) ← typeRef from `src/render/agents.ts#AgentMeshes`
- `src/render/riggedCitizens.ts#createRiggedCitizens.begin` (method, line 944) ← call from `src/render/agents.ts#createAgentMeshes.sync`
- `src/render/riggedCitizens.ts#createRiggedCitizens.capturedPose` (method, line 1194) ← call from `src/render/agents.ts#createAgentMeshes`
- `src/render/riggedCitizens.ts#createRiggedCitizens.census` (method, line 1137) ← call from `src/render/agents.ts#createAgentMeshes.census`
- `src/render/riggedCitizens.ts#createRiggedCitizens.clipPose` (method, line 1224) ← call from `src/render/agents.ts#createAgentMeshes`
- `src/render/riggedCitizens.ts#createRiggedCitizens.dispose` (method, line 1275) ← call from `src/render/agents.ts#createAgentMeshes.dispose`
- `src/render/riggedCitizens.ts#createRiggedCitizens.draw` (method, line 967) ← call from `src/render/agents.ts#createAgentMeshes.sync`
- `src/render/riggedCitizens.ts#createRiggedCitizens.drawClip` (method, line 1060) ← call from `src/render/agents.ts#createAgentMeshes.placeCarOccupants`, `src/render/agents.ts#createAgentMeshes.placeKerbPerson`, `src/render/agents.ts#createAgentMeshes.placeKerbPerson.walkTo`, `src/render/agents.ts#createAgentMeshes.placeRiderFigure`
- `src/render/riggedCitizens.ts#createRiggedCitizens.drawPalette` (method, line 1261) ← call from `src/render/agents.ts#createAgentMeshes`
- `src/render/riggedCitizens.ts#createRiggedCitizens.finish` (method, line 1143) ← call from `src/render/agents.ts#createAgentMeshes.sync`
- `src/render/riggedCitizens.ts#createRiggedCitizens.forgetCompany` (method, line 1254) ← call from `src/render/agents.ts#createAgentMeshes.forgetGone`
- `src/render/riggedCitizens.ts#createRiggedCitizens.indexFor` (method, line 1242) ← call from `src/render/agents.ts#createAgentMeshes.driverBody`
- `src/render/riggedCitizens.ts#createRiggedCitizens.keepAll` (method, line 1246) ← call from `src/render/agents.ts#createAgentMeshes.keepPeople`
- `src/render/riggedCitizens.ts#createRiggedCitizens.loadedIndices` (method, line 1221) ← call from `src/render/agents.ts#createAgentMeshes`
- `src/render/riggedCitizens.ts#createRiggedCitizens.renderPalettes` (method, line 1185) ← call from `src/render/agents.ts#createAgentMeshes.renderPalettes`
- `src/render/riggedCitizens.ts#createRiggedCitizens.skeletonOf` (method, line 1204) ← call from `src/render/agents.ts#createAgentMeshes`

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
- `src/sim/agents/walk.ts#createAgentWalkEngine` (function, line 917) ← implementation from `src/sim/agents/lorryTraffic.ts#LorryTraffic.mayGo`, `src/sim/agents/lotTraffic.ts#LotTraffic.mayGo`, `src/sim/host/simHost.ts#SimHost.act`, `src/sim/invariants.ts#runAudit`, …
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
- `src/sim/agents/walk.ts#STAGGER_FROM` (variable, line 440) ← read from `src/render/agents.ts#createAgentMeshes.procPose`
- `src/sim/agents/walk.ts#STAGGER_TIME` (variable, line 438) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#STAGGER_TIME` (variable, line 438) ← read from `src/render/agents.ts#createAgentMeshes.procPose`
- `src/sim/agents/walk.ts#staggerSpeed` (function, line 443) ← call from `src/render/agents.ts#createAgentMeshes.procPose`
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
- `src/sim/agents/walk.ts#walkerOf` (function, line 2004) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#WalkerSeen` (interface, line 1983) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#walkersNear` (function, line 1999) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#walkerState` (function, line 2261) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#walkersWithin` (function, line 1311) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/agents/walk.ts#walkOn` (function, line 2058) ← moduleLoad from `src/sim/host/simHost.ts#SimHost.init`
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

### src/sim/ambient/ambient.ts

- `src/sim/ambient/ambient.ts#AmbientWorld` (class, line 62) ← construct from `src/sim/world.ts#SimWorld.ambient`
- `src/sim/ambient/ambient.ts#AmbientWorld` (class, line 62) ← typeRef from `src/sim/agents/lotTraffic.ts#LotTraffic.ambient`
- `src/sim/ambient/ambient.ts#AmbientWorld.addParked` (method, line 757) ← call from `src/sim/agents/lorryTraffic.ts#LorryTraffic.step`, `src/sim/agents/lotTraffic.ts#LotTraffic.coming`, `src/sim/agents/lotTraffic.ts#LotTraffic.depart`
- `src/sim/ambient/ambient.ts#AmbientWorld.enabled` (field, line 64) ← read from `src/sim/agents/lotTraffic.ts#LotTraffic.step`, `src/sim/city/city.ts#City.step`
- `src/sim/ambient/ambient.ts#AmbientWorld.enabled` (field, line 64) ← write from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/ambient/ambient.ts#AmbientWorld.isParked` (method, line 752) ← call from `src/sim/agents/lorryTraffic.ts#LorryTraffic.step`, `src/sim/host/simHost.ts#SimHost.parked`
- `src/sim/ambient/ambient.ts#AmbientWorld.movers` (field, line 74) ← read from `src/sim/host/simHost.ts#SimHost.publish`
- `src/sim/ambient/ambient.ts#AmbientWorld.parked` (field, line 68) ← read from `src/sim/host/simHost.ts#SimHost.parked`
- `src/sim/ambient/ambient.ts#AmbientWorld.parkedSerialNow` (method, line 779) ← call from `src/sim/host/simHost.ts#SimHost.parked`
- `src/sim/ambient/ambient.ts#AmbientWorld.parkedSince` (method, line 784) ← call from `src/sim/host/simHost.ts#SimHost.parked`
- `src/sim/ambient/ambient.ts#AmbientWorld.parkedSince` (method, line 784) ← read from `src/sim/host/simHost.ts#SimHost.parked`
- `src/sim/ambient/ambient.ts#AmbientWorld.release` (method, line 734) ← call from `src/sim/agents/lotTraffic.ts#LotTraffic.depart`
- `src/sim/ambient/ambient.ts#AmbientWorld.removeParked` (method, line 767) ← call from `src/sim/agents/lorryTraffic.ts#LorryTraffic.drop`, `src/sim/agents/lotTraffic.ts#LotTraffic.drop`
- `src/sim/ambient/ambient.ts#AmbientWorld.reset` (method, line 120) ← call from `src/sim/host/simHost.ts#SimHost.handle`, `src/sim/host/simHost.ts#SimHost.load`
- `src/sim/ambient/ambient.ts#AmbientWorld.sceneryCarAt` (method, line 821) ← call from `src/sim/agents/lotTraffic.ts#LotTraffic.depart`, `src/sim/agents/lotTraffic.ts#LotTraffic.refreshBay`
- `src/sim/ambient/ambient.ts#AmbientWorld.sceneryWatch` (field, line 86) ← callbackAssign from `src/sim/agents/lotTraffic.ts#LotTraffic.step`
- `src/sim/ambient/ambient.ts#AmbientWorld.sceneryWatch` (field, line 86) ← implementation from `src/sim/agents/lotTraffic.ts#LotTraffic.step`
- `src/sim/ambient/ambient.ts#AmbientWorld.sceneryWatch` (field, line 86) ← write from `src/sim/agents/lotTraffic.ts#LotTraffic.step`
- `src/sim/ambient/ambient.ts#AmbientWorld.sighted` (field, line 96) ← read from `src/sim/vehicles/spawn.ts#stepDespawn`
- `src/sim/ambient/ambient.ts#AmbientWorld.source` (field, line 66) ← read from `src/sim/city/city.ts#City.step`
- `src/sim/ambient/ambient.ts#AmbientWorld.source` (field, line 66) ← write from `src/sim/host/simHost.ts#SimHost.init`
- `src/sim/ambient/ambient.ts#AmbientWorld.step` (method, line 155) ← call from `src/sim/pipeline.ts#step`

### src/sim/ambient/ambientIndex.ts

- `src/sim/ambient/ambientIndex.ts#BuildingMix.constructor` (constructor, line 230) ← callbackAssign from `src/core/idSlots.ts#IdSlots.take`, `src/render/buildings/cellPool.ts#FloorColumns.constructor`, `src/render/buildings/townColumns.ts#TownColumns.constructor`, `src/sim/agents/buildingOrder.ts#StoreOrder.constructor`, …
- `src/sim/ambient/ambientIndex.ts#buildingsChangedSince` (function, line 17) ← call from `src/sim/agents/lotTraffic.ts#LotTraffic.followBuildings`
- `src/sim/ambient/ambientIndex.ts#SightWalls.constructor` (constructor, line 66) ← callbackAssign from `src/core/idSlots.ts#IdSlots.take`, `src/render/buildings/cellPool.ts#FloorColumns.constructor`, `src/render/buildings/townColumns.ts#TownColumns.constructor`, `src/sim/agents/buildingOrder.ts#StoreOrder.constructor`, …
- `src/sim/ambient/ambientIndex.ts#spotKey` (function, line 35) ← call from `src/sim/agents/lotTraffic.ts#LotTraffic.fileBay`, `src/sim/agents/lotTraffic.ts#LotTraffic.holdLorries`, `src/sim/agents/lotTraffic.ts#LotTraffic.placeBody`, `src/sim/agents/lotTraffic.ts#LotTraffic.readBays`, …

### src/sim/ambient/demand.ts

- `src/sim/ambient/demand.ts#flows` (function, line 35) ← call from `src/sim/agents/lotTraffic.ts#LotTraffic.call`, `src/sim/agents/lotTraffic.ts#LotTraffic.depart`
- `src/sim/ambient/demand.ts#flows` (function, line 35) ← read from `src/sim/agents/lotTraffic.ts#LotTraffic.call`, `src/sim/agents/lotTraffic.ts#LotTraffic.depart`
- `src/sim/ambient/demand.ts#kindOf` (function, line 55) ← call from `src/sim/agents/trafficBuildings.ts#factsOf`, `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.place`
- `src/sim/ambient/demand.ts#Mix` (interface, line 6) ← typeRef from `src/sim/agents/lotTraffic.ts#KINDS`, `src/sim/agents/trafficBuildings.ts#TRAFFIC_KINDS`, `src/sim/agents/trafficBuildings.ts#TrafficFacts`, `src/sim/meso/demand.ts#KINDS`

### src/sim/ambient/playerId.ts

- `src/sim/ambient/playerId.ts#PLAYER_ID` (variable, line 2) ← read from `src/render/agents.ts#createAgentMeshes.sync`

### src/sim/crossings/permission.ts

- `src/sim/crossings/permission.ts#PED_COURTESY` (variable, line 106) ← read from `src/sim/intersections/admission.ts#pedestrianHasPriority`
- `src/sim/crossings/permission.ts#vehicleBodyIntersectsCrossing` (function, line 148) ← call from `src/sim/intersections/admission.ts#pedestrianHasPriority`

### src/sim/crossings/state.ts

- `src/sim/crossings/state.ts#CrossingOccupant` (interface, line 4) ← read from `src/sim/intersections/admission.ts#crossingReachedFirst`, `src/sim/invariants.ts#runAudit`
- `src/sim/crossings/state.ts#CrossingState` (interface, line 16) ← read from `src/sim/intersections/admission.ts#crossingReachedFirst`, `src/sim/intersections/admission.ts#pedestrianHasPriority`, `src/sim/invariants.ts#runAudit`, `src/sim/world.ts#SimWorld.junctionQuiet`, …
- `src/sim/crossings/state.ts#CrossingStates` (type, line 30) ← typeRef from `src/sim/world.ts#SimWorld.crossingStates`

### src/sim/intersections/crossingSpans.ts

- `src/sim/intersections/crossingSpans.ts#CrossingSpan` (interface, line 16) ← read from `src/sim/intersections/admission.ts#crossingReachedFirst`, `src/sim/intersections/admission.ts#pedestrianHasPriority`, `src/sim/vehicles/obstacles.ts#pedestrianAhead`
- `src/sim/intersections/crossingSpans.ts#CrossingSpans` (class, line 32) ← construct from `src/sim/world.ts#SimWorld.crossingSpans`
- `src/sim/intersections/crossingSpans.ts#CrossingSpans.approachStop` (method, line 133) ← call from `src/sim/vehicles/obstacles.ts#longitudinalConstraints`
- `src/sim/intersections/crossingSpans.ts#CrossingSpans.span` (method, line 138) ← call from `src/sim/intersections/admission.ts#crossingReachedFirst`, `src/sim/intersections/admission.ts#pedestrianHasPriority`, `src/sim/invariants.ts#runAudit`
- `src/sim/intersections/crossingSpans.ts#CrossingSpans.update` (method, line 54) ← call from `src/sim/world.ts#SimWorld.rebuildWalkTopology`
- `src/sim/intersections/crossingSpans.ts#PED_BODY` (variable, line 195) ← read from `src/sim/intersections/admission.ts#crossingReachedFirst`, `src/sim/intersections/admission.ts#stoppedForWalker`
- `src/sim/intersections/crossingSpans.ts#PED_CROSSING_STOP_BUFFER` (variable, line 199) ← read from `src/sim/intersections/admission.ts#admit`, `src/sim/vehicles/obstacles.ts#pedestrianAhead`
- `src/sim/intersections/crossingSpans.ts#PED_MIN_PACE` (variable, line 196) ← read from `src/sim/intersections/admission.ts#crossingReachedFirst`, `src/sim/intersections/admission.ts#pedestrianHasPriority`
- `src/sim/intersections/crossingSpans.ts#PED_REACH_TIME` (variable, line 197) ← read from `src/sim/intersections/admission.ts#crossingReachedFirst`, `src/sim/intersections/admission.ts#pedestrianHasPriority`
- `src/sim/intersections/crossingSpans.ts#pedestrianAffectsSpan` (function, line 222) ← call from `src/sim/invariants.ts#runAudit`
- `src/sim/intersections/crossingSpans.ts#pedestrianInSpan` (function, line 230) ← call from `src/sim/intersections/admission.ts#crossingBusy`, `src/sim/vehicles/obstacles.ts#pedestrianAhead`
- `src/sim/intersections/crossingSpans.ts#reservationCoversCrossing` (function, line 203) ← call from `src/sim/invariants.ts#runAudit`

### src/sim/peds/sidewalk.ts

- `src/sim/peds/sidewalk.ts#SidewalkEdge` (interface, line 30) ← read from `src/sim/vehicles/kerbStops.ts#footwayBeside`
- `src/sim/peds/sidewalk.ts#SidewalkGraph` (class, line 47) ← construct from `src/sim/world.ts#SimWorld.sidewalks`
- `src/sim/peds/sidewalk.ts#SidewalkGraph.clear` (method, line 230) ← call from `src/sim/world.ts#SimWorld.reset`
- `src/sim/peds/sidewalk.ts#SidewalkGraph.crossings` (field, line 50) ← read from `src/sim/intersections/admission.ts#pedestrianHasPriority`, `src/sim/world.ts#SimWorld.crossingsAt`
- `src/sim/peds/sidewalk.ts#SidewalkGraph.edges` (field, line 49) ← read from `src/sim/intersections/admission.ts#pedestrianHasPriority`
- `src/sim/peds/sidewalk.ts#SidewalkGraph.update` (method, line 66) ← call from `src/sim/world.ts#SimWorld.rebuildWalkTopology`
- `src/sim/peds/sidewalk.ts#SidewalkGraph.walksBeside` (method, line 249) ← call from `src/sim/vehicles/kerbStops.ts#footwayBeside`

### src/sim/peds/signalPostIndex.ts

- `src/sim/peds/signalPostIndex.ts#SignalPostIndex` (class, line 21) ← construct from `src/sim/world.ts#SimWorld.signalPosts`
- `src/sim/peds/signalPostIndex.ts#SignalPostIndex.reset` (method, line 29) ← call from `src/sim/world.ts#SimWorld.reset`

### src/sim/people/casualties.ts

- `src/sim/people/casualties.ts#Casualty` (interface, line 8) ← read from `src/render/ragdoll.ts#createRagdolls.blast`, `src/render/ragdoll.ts#createRagdolls.sever`, `src/render/ragdoll.ts#createRagdolls.shove`, `src/render/ragdoll.ts#createRagdolls.spawn`, …
- `src/sim/people/casualties.ts#Casualty` (interface, line 8) ← typeRef from `src/render/ragdoll.ts#createRagdolls`, `src/render/ragdoll.ts#createRagdolls.blast`, `src/render/ragdoll.ts#createRagdolls.fling`, `src/render/ragdoll.ts#createRagdolls.sever`, …
- `src/sim/people/casualties.ts#Casualty` (interface, line 8) ← write from `src/sim/host/simClient.ts#SimClient.casualties`
- `src/sim/people/casualties.ts#impactCasualties` (function, line 69) ← call from `src/sim/host/simHost.ts#SimHost.frame`, `src/sim/host/simHost.ts#SimHost.publish`
- `src/sim/people/casualties.ts#takeWounds` (function, line 53) ← call from `src/sim/host/simHost.ts#SimHost.publish`

### src/sim/people/engine.ts

- `src/sim/people/engine.ts#Boarder` (interface, line 56) ← read from `src/render/agents.ts#createAgentMeshes.placeKerbPerson`, `src/sim/vehicles/kerbStops.ts#completeTransfer`, `src/sim/vehicles/kerbStops.ts#hail`, `src/sim/vehicles/kerbStops.ts#stepKerbStops`
- `src/sim/people/engine.ts#Boarder` (interface, line 56) ← typeRef from `src/sim/vehicles/kerbStops.ts#KerbPerson`
- `src/sim/people/engine.ts#PedestrianEngine` (interface, line 8) ← call from `src/sim/host/simHost.ts#SimHost.act`, `src/sim/invariants.ts#runAudit`, `src/sim/pipeline.ts#rebindPeds`, `src/sim/pipeline.ts#step`, …
- `src/sim/people/engine.ts#PedestrianEngine` (interface, line 8) ← read from `src/sim/agents/lorryTraffic.ts#LorryTraffic.mayGo`, `src/sim/agents/lotTraffic.ts#LotTraffic.mayGo`, `src/sim/vehicles/kerbStops.ts#completeTransfer`, `src/sim/vehicles/kerbStops.ts#hail`, …
- `src/sim/people/engine.ts#PedestrianEngine` (interface, line 8) ← typeRef from `src/sim/world.ts#NO_PEDESTRIANS`, `src/sim/world.ts#SimWorld.pedEngine`, `src/sim/world.ts#SimWorld.usePedestrianEngine`
- `src/sim/people/engine.ts#PeopleBridge` (interface, line 68) ← call from `src/sim/agents/lorryTraffic.ts#LorryTraffic.mayGo`, `src/sim/agents/lotTraffic.ts#LotTraffic.mayGo`, `src/sim/vehicles/kerbStops.ts#completeTransfer`, `src/sim/vehicles/kerbStops.ts#hail`, …
- `src/sim/people/engine.ts#PeopleBridge` (interface, line 68) ← read from `src/sim/vehicles/kerbStops.ts#plan`, `src/sim/vehicles/kerbStops.ts#planBusStop`

### src/sim/people/view.ts

- `src/sim/people/view.ts#BodyPart` (type, line 53) ← typeRef from `src/actionsWiring.ts#createActions.shootAt`, `src/render/agents.ts#createAgentMeshes.procDraw`, `src/render/agents.ts#createAgentMeshes.procPose`, `src/render/playEffects.ts#PlayEffects`, …
- `src/sim/people/view.ts#gaitSpeed` (function, line 113) ← call from `src/render/agents.ts#createAgentMeshes.sync`
- `src/sim/people/view.ts#GestureView` (interface, line 32) ← read from `src/render/agents.ts#createAgentMeshes.sync`, `src/render/playEffects.ts#createPlayEffects.frame`, `src/sim/host/simClient.ts#SimClient.readTick`
- `src/sim/people/view.ts#GestureView` (interface, line 32) ← write from `src/sim/host/simClient.ts#SimClient.readTick`
- `src/sim/people/view.ts#PedView` (interface, line 55) ← read from `src/actionsWiring.ts#createActions.explodeAt`, `src/actionsWiring.ts#createActions.shootAt`, `src/lab/inspector/entity.ts#agentsNear`, `src/lab/inspector/entity.ts#entityInfo`, …
- `src/sim/people/view.ts#PedView` (interface, line 55) ← typeRef from `src/sim/host/protocol.ts#PedRich`, `src/sim/host/simClient.ts#PedRead`, `src/sim/host/simClient.ts#SimClient.pedViews`, `src/sim/host/simClient.ts#SimClient.readTick`, …
- `src/sim/people/view.ts#PedView` (interface, line 55) ← write from `src/sim/host/simClient.ts#SimClient.readTick`
- `src/sim/people/view.ts#PersonAgeClass` (type, line 6) ← typeRef from `src/render/agents.ts#createAgentMeshes`, `src/render/agents.ts#createAgentMeshes.procDraw`, `src/render/agents.ts#createAgentMeshes.procPose`, `src/render/agents.ts#createAgentMeshes.procSpareFor`, …
- `src/sim/people/view.ts#PersonGender` (type, line 7) ← typeRef from `src/sim/vehicles/kerbStops.ts#SeatPerson`
- `src/sim/people/view.ts#personHash` (function, line 105) ← call from `src/sim/vehicles/kerbStops.ts#seatAgeClass`, `src/sim/vehicles/kerbStops.ts#seatPersonInto`
- `src/sim/people/view.ts#Severable` (type, line 51) ← typeRef from `src/render/agents.ts#createAgentMeshes.procDraw`, `src/render/agents.ts#createAgentMeshes.procPose`, `src/render/playEffects.ts#PlayEffects`, `src/render/ragdoll.ts#createRagdolls`, …

### src/world/footwayPath.ts

- `src/world/footwayPath.ts#nearestFootway` (function, line 35) ← call from `src/editor/transitTools.ts#TransitTool.footwayAt`

### src/world/pedestrianAccess.ts

- `src/world/pedestrianAccess.ts#carriesPedestrians` (function, line 4) ← call from `src/editor/roads/powerLine.ts#layPowerLinesSteps`, `src/world/curbRamps.ts#rampsAt.pair`, `src/world/landscape.ts#footwayAt`, `src/world/lots.ts#plannedStreet`, …

### src/world/roads/footwayRise.ts

- `src/world/roads/footwayRise.ts#FlushData` (interface, line 36) ← read from `src/render/renderer.ts#createSceneRenderer.landGroundDigest`, `src/render/renderer.ts#createSceneRenderer.levelGroundDigest`
- `src/world/roads/footwayRise.ts#FlushData` (interface, line 36) ← typeRef from `src/render/roadTileMesh.ts#SurfaceData`, `src/world/paving.ts#PavingData`
- `src/world/roads/footwayRise.ts#flushShareOf` (function, line 74) ← call from `src/render/roadTileMesh.ts#readsOf`, `src/world/paving.ts#pavingReader`
- `src/world/roads/footwayRise.ts#footwayRiseAt` (function, line 33) ← call from `src/editor/smallCityMap.ts#growLots.pavedAt`, `src/render/agents.ts#createAgentMeshes.sync`, `src/render/renderer.ts#createSceneRenderer.footwayRiseHere`, `src/render/signLayer.ts#itemSigns`, …
- `src/world/roads/footwayRise.ts#footwayRiseShare` (function, line 27) ← call from `src/render/roadSurfaceInputs.ts#SurfaceInputs.placeNode`, `src/render/roadSurfacePass.ts#passReads.footwayShare`
- `src/world/roads/footwayRise.ts#packFlush` (function, line 44) ← call from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.landGroundDigest`, `src/render/renderer.ts#createSceneRenderer.levelGroundDigest`, `src/render/roadSurfaces.ts#roadSurfaceSteps`
- `src/world/roads/footwayRise.ts#packFlush` (function, line 44) ← read from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.landGroundDigest`, `src/render/renderer.ts#createSceneRenderer.levelGroundDigest`, `src/render/roadSurfaces.ts#roadSurfaceSteps`

### src/world/walkable.ts

- `src/world/walkable.ts#WalkableSurface` (class, line 15) ← construct from `src/world/network.ts#Network.crossingSurface`
- `src/world/walkable.ts#WalkableSurface` (class, line 15) ← typeRef from `src/world/network.ts#Network.crossingSurface`, `src/world/network.ts#Network.crossingWalkable`
- `src/world/walkable.ts#WalkableSurface.footway` (method, line 46) ← call from `src/world/network.ts#Network.crosswalkDistanceAt.fits`

### src/world/walkingLines.ts

- `src/world/walkingLines.ts#CornerAsk` (interface, line 375) ← typeRef from `src/conflictPool.ts#ConflictPool.roundCorners`, `src/conflictTable.worker.ts#CornersAsk`, `src/conflictTable.worker.ts#serve`
- `src/world/walkingLines.ts#CornerMeasurer` (interface, line 381) ← inherit from `src/world/conflictTable.ts#PairMeasurer`
- `src/world/walkingLines.ts#roundCorners` (function, line 395) ← call from `src/conflictTable.worker.ts#serve`

### src/world/walkways.ts

- `src/world/walkways.ts#deckOf` (function, line 60) ← call from `src/world/poleLines.ts#polesInclude`
- `src/world/walkways.ts#WalkGraph` (class, line 81) ← typeRef from `src/sim/world.ts#SimWorld.walkwaysNow`
- `src/world/walkways.ts#Walkways` (class, line 303) ← construct from `src/sim/world.ts#SimWorld.walkways`
- `src/world/walkways.ts#Walkways.update` (method, line 312) ← call from `src/sim/world.ts#SimWorld.walkwaysNow`
- `src/world/walkways.ts#Walkways.updateSteps` (method, line 318) ← call from `src/sim/world.ts#SimWorld.prepareVehicleTopology`

## Shared declarations the feature uses

### src/bays.worker.ts

- `src/bays.worker.ts#serve` (function, line 5) — callbackAssign by 2: `src/render/citizenBake.worker.ts#serve`, `src/render/people/lodWorker.ts#serve`

### src/cityGen.worker.ts

- `src/cityGen.worker.ts#serve` (function, line 19) — callbackAssign by 2: `src/render/citizenBake.worker.ts#serve`, `src/render/people/lodWorker.ts#serve`

### src/conflictPool.ts

- `src/conflictPool.ts#ConflictPool.roundCorners` (method, line 184) — implementation by 1: `src/world/walkways.ts#Walkways.updateSteps`

### src/conflictTable.worker.ts

- `src/conflictTable.worker.ts#serve` (function, line 13) — callbackAssign by 2: `src/render/citizenBake.worker.ts#serve`, `src/render/people/lodWorker.ts#serve`

### src/core/aabb.ts

- `src/core/aabb.ts#Aabb` (interface, line 3) — read by 6: `src/sim/agents/walk.ts#fileWay`, `src/sim/agents/walk.ts#gapOpen`, `src/sim/intersections/crossingSpans.ts#measure`, `src/sim/intersections/crossingSpans.ts#measure.gap`, …

### src/core/changeLog.ts

- `src/core/changeLog.ts#ChangeLog.state` (getter, line 21) — call by 2: `src/sim/ambient/ambient.ts#AmbientWorld.listLanes`, `src/sim/ambient/ambient.ts#AmbientWorld.openingLanes`

### src/core/clipper.ts

- `src/core/clipper.ts#MultiPoly` (type, line 9) — typeRef by 5: `src/world/walkingLines.ts#flatRings`, `src/world/walkingLines.ts#LocalLines.paving`, `src/world/walkingLines.ts#LocalLines.pavings`, `src/world/walkingLines.ts#LocalLines.polygons`, …
- `src/core/clipper.ts#union` (function, line 143) — call by 1: `src/world/walkingLines.ts#LocalLines.polygons`

### src/core/derivedCache.ts

- `src/core/derivedCache.ts#forgetOtherDerived` (function, line 327) — call by 1: `src/render/people/proceduralCrowd.ts#<IfStatement>`
- `src/core/derivedCache.ts#readDerived` (function, line 53) — call by 1: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.lodPlan`
- `src/core/derivedCache.ts#writeDerived` (function, line 250) — call by 1: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.lodPlan`

### src/core/digest.ts

- `src/core/digest.ts#Digest` (class, line 6) — construct by 1: `src/world/walkways.ts#wayKey`
- `src/core/digest.ts#Digest.add` (method, line 11) — call by 1: `src/world/walkways.ts#wayKey`
- `src/core/digest.ts#Digest.addAll` (method, line 19) — call by 1: `src/world/walkways.ts#wayKey`
- `src/core/digest.ts#Digest.addText` (method, line 25) — call by 1: `src/world/walkways.ts#wayKey`
- `src/core/digest.ts#Digest.value` (method, line 59) — call by 1: `src/world/walkways.ts#wayKey`

### src/core/fillet.ts

- `src/core/fillet.ts#Fillet` (interface, line 5) — read by 2: `src/world/walkingLines.ts#cornerPath`, `src/world/walkways.ts#cornersOf`
- `src/core/fillet.ts#filletCorner` (function, line 19) — call by 1: `src/world/walkingLines.ts#cornerPath`

### src/core/heap.ts

- `src/core/heap.ts#Heap` (class, line 3) — construct by 1: `src/sim/agents/walk.ts#routeScratch`
- `src/core/heap.ts#Heap` (class, line 3) — typeRef by 1: `src/sim/agents/walk.ts#RouteScratch`
- `src/core/heap.ts#Heap.clear` (method, line 13) — call by 2: `src/sim/agents/walk.ts#routeScratch`, `src/sim/agents/lotTraffic.ts#LotTraffic.reset`
- `src/core/heap.ts#Heap.peekKey` (method, line 12) — call by 1: `src/sim/agents/walk.ts#route`
- `src/core/heap.ts#Heap.pop` (method, line 24) — call by 1: `src/sim/agents/walk.ts#route`
- `src/core/heap.ts#Heap.push` (method, line 14) — call by 1: `src/sim/agents/walk.ts#route.push`
- `src/core/heap.ts#Heap.size` (getter, line 10) — call by 1: `src/sim/agents/walk.ts#route`

### src/core/hitch.ts

- `src/core/hitch.ts#measureHitch` (function, line 2) — call by 4: `src/render/people/personBuild.ts#buildPersonScene`, `src/render/people/proceduralBake.ts#hairCards`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.wornItem`, `src/render/riggedCitizens.ts#createRiggedCitizens.load`

### src/core/idSlots.ts

- `src/core/idSlots.ts#grown` (function, line 127) — call by 4: `src/sim/ambient/ambientIndex.ts#BuildingMix.constructor`, `src/sim/ambient/ambientIndex.ts#SightWalls.constructor`, `src/sim/ambient/ambientIndex.ts#SightWalls.newWall`, `src/sim/ambient/lotPlaces.ts#LotPlaces.newPlace`
- `src/core/idSlots.ts#IdSlots` (class, line 15) — construct by 2: `src/sim/ambient/ambientIndex.ts#BuildingMix.slots`, `src/sim/ambient/ambientIndex.ts#SightWalls.buildings`
- `src/core/idSlots.ts#IdSlots.clear` (method, line 97) — call by 2: `src/sim/ambient/ambientIndex.ts#BuildingMix.fileAll`, `src/sim/ambient/ambientIndex.ts#SightWalls.fileAll`
- `src/core/idSlots.ts#IdSlots.delete` (method, line 75) — call by 2: `src/sim/ambient/ambientIndex.ts#BuildingMix.unfile`, `src/sim/ambient/ambientIndex.ts#SightWalls.unfile`
- `src/core/idSlots.ts#IdSlots.get` (method, line 48) — call by 2: `src/sim/ambient/ambientIndex.ts#BuildingMix.unfile`, `src/sim/ambient/ambientIndex.ts#SightWalls.unfile`
- `src/core/idSlots.ts#IdSlots.onGrow` (field, line 26) — callbackAssign by 2: `src/sim/ambient/ambientIndex.ts#BuildingMix.constructor`, `src/sim/ambient/ambientIndex.ts#SightWalls.constructor`
- `src/core/idSlots.ts#IdSlots.onGrow` (field, line 26) — implementation by 2: `src/sim/ambient/ambientIndex.ts#BuildingMix.constructor`, `src/sim/ambient/ambientIndex.ts#SightWalls.constructor`
- `src/core/idSlots.ts#IdSlots.onGrow` (field, line 26) — write by 2: `src/sim/ambient/ambientIndex.ts#BuildingMix.constructor`, `src/sim/ambient/ambientIndex.ts#SightWalls.constructor`
- `src/core/idSlots.ts#IdSlots.take` (method, line 58) — call by 2: `src/sim/ambient/ambientIndex.ts#BuildingMix.file`, `src/sim/ambient/ambientIndex.ts#SightWalls.file`

### src/core/intersect.ts

- `src/core/intersect.ts#lineLine` (function, line 5) — call by 1: `src/world/walkingLines.ts#cornerPath`
- `src/core/intersect.ts#lineLine` (function, line 5) — read by 1: `src/world/walkingLines.ts#cornerPath`

### src/core/offset.ts

- `src/core/offset.ts#offsetPolyline` (function, line 46) — call by 1: `src/world/footwayPath.ts#footwayPath`

### src/core/polygon.ts

- `src/core/polygon.ts#pointInPolygon` (function, line 40) — call by 2: `src/sim/peds/sidewalk.ts#clearOfJunction`, `src/world/walkingLines.ts#inRing`

### src/core/polyline.ts

- `src/core/polyline.ts#Frame` (interface, line 7) — read by 19: `src/sim/agents/walk.ts#createAgentWalkEngine`, `src/sim/agents/walk.ts#joinFootway`, `src/sim/agents/walk.ts#plan`, `src/sim/agents/walk.ts#publish`, …
- `src/core/polyline.ts#FrameOut` (interface, line 23) — read by 3: `src/sim/agents/walk.ts#frameInto`, `src/world/roads/footwayRise.ts#flushHolds`, `src/sim/pose.ts#vehiclePoseNowInto`
- `src/core/polyline.ts#newFrame` (function, line 33) — call by 2: `src/sim/agents/walk.ts#WAY_SAMPLE`, `src/world/roads/footwayRise.ts#frame`
- `src/core/polyline.ts#Polyline` (class, line 36) — reference by 12: `src/sim/peds/sidewalk.ts#SidewalkGraph.crossingsAt`, `src/sim/peds/sidewalk.ts#SidewalkGraph.footwaysOf`, `src/world/footwayPath.ts#footwayPath`, `src/world/roads/footwayRise.ts#flushShareOf`, …
- `src/core/polyline.ts#Polyline` (class, line 36) — typeRef by 23: `src/sim/intersections/crossingSpans.ts#CrossingSpans.measureJunction`, `src/sim/intersections/crossingSpans.ts#CrossingSpans.measureJunction.samplesOf`, `src/sim/intersections/crossingSpans.ts#measure`, `src/sim/intersections/crossingSpans.ts#sampleZebra`, …
- `src/core/polyline.ts#Polyline.bbox` (getter, line 116) — call by 5: `src/sim/agents/walk.ts#fileWay`, `src/sim/agents/walk.ts#gapOpen`, `src/sim/intersections/crossingSpans.ts#measure`, `src/world/walkingLines.ts#stretch`, …
- `src/core/polyline.ts#Polyline.closestInto` (method, line 347) — call by 2: `src/sim/agents/walk.ts#projectInto`, `src/world/roads/footwayRise.ts#flushHolds`
- `src/core/polyline.ts#Polyline.closestPoint` (method, line 330) — call by 8: `src/sim/agents/walk.ts#createAgentWalkEngine`, `src/sim/agents/walk.ts#gapOpen`, `src/sim/agents/walk.ts#nearestWay`, `src/sim/agents/walk.ts#publish`, …
- `src/core/polyline.ts#Polyline.closestPoint` (method, line 330) — read by 8: `src/sim/agents/walk.ts#createAgentWalkEngine`, `src/sim/agents/walk.ts#gapOpen`, `src/sim/agents/walk.ts#nearestWay`, `src/sim/agents/walk.ts#publish`, …
- `src/core/polyline.ts#Polyline.cum` (getter, line 81) — call by 2: `src/sim/agents/walk.ts#bendRadii`, `src/sim/agents/walk.ts#createAgentWalkEngine`
- `src/core/polyline.ts#Polyline.fromFlat` (method, line 65) — call by 2: `src/world/roads/footwayRise.ts#flushShareOf`, `src/world/walkways.ts#Walkways.buildNode`
- `src/core/polyline.ts#Polyline.fromPoints` (method, line 55) — call by 11: `src/sim/peds/sidewalk.ts#SidewalkGraph.crossingsAt`, `src/sim/peds/sidewalk.ts#SidewalkGraph.footwaysOf`, `src/world/footwayPath.ts#footwayPath`, `src/world/walkingLines.ts#cornerPath`, …
- `src/core/polyline.ts#Polyline.length` (getter, line 123) — call by 15: `src/sim/agents/walk.ts#createAgentWalkEngine`, `src/sim/agents/walk.ts#gapOpen`, `src/sim/agents/walk.ts#route`, `src/sim/agents/walk.ts#wayCost`, …
- `src/core/polyline.ts#Polyline.n` (field, line 40) — read by 5: `src/sim/agents/walk.ts#bendRadii`, `src/sim/agents/walk.ts#createAgentWalkEngine`, `src/sim/agents/walk.ts#frameInto`, `src/sim/crossings/permission.ts#vehicleBodyIntersectsCrossing`, …
- `src/core/polyline.ts#Polyline.point` (method, line 69) — call by 1: `src/sim/crossings/permission.ts#vehicleBodyIntersectsCrossing`
- `src/core/polyline.ts#Polyline.sampleAt` (method, line 152) — call by 19: `src/sim/agents/walk.ts#createAgentWalkEngine`, `src/sim/agents/walk.ts#joinFootway`, `src/sim/agents/walk.ts#plan`, `src/sim/agents/walk.ts#publish`, …
- `src/core/polyline.ts#Polyline.sampleAtInto` (method, line 157) — call by 2: `src/sim/agents/walk.ts#frameInto`, `src/world/roads/footwayRise.ts#flushHolds`
- `src/core/polyline.ts#Polyline.sub` (method, line 198) — call by 3: `src/sim/peds/sidewalk.ts#SidewalkGraph.footwaysOf`, `src/world/walkways.ts#Walkways.buildNode.landOn`, `src/world/walkways.ts#Walkways.cut`
- `src/core/polyline.ts#Polyline.tan` (getter, line 96) — call by 1: `src/sim/agents/walk.ts#frameInto`
- `src/core/polyline.ts#Polyline.toPoints` (method, line 74) — call by 3: `src/sim/peds/sidewalk.ts#SidewalkGraph.footwaysOf`, `src/world/footwayPath.ts#footwayPath`, `src/sim/ambient/ambientIndex.ts#StreetLength.add`
- `src/core/polyline.ts#Polyline.xy` (field, line 38) — read by 8: `src/sim/agents/walk.ts#bendRadii`, `src/sim/agents/walk.ts#createAgentWalkEngine`, `src/sim/intersections/crossingSpans.ts#CrossingSpans.measureJunction`, `src/world/roads/footwayRise.ts#packFlush`, …

### src/core/ring.ts

- `src/core/ring.ts#Ring.flatten` (method, line 89) — call by 2: `src/sim/peds/sidewalk.ts#clearOfJunction`, `src/world/junction/bulb.ts#bulbLine`

### src/core/rng.ts

- `src/core/rng.ts#Rng` (class, line 6) — construct by 2: `src/sim/ambient/ambient.ts#AmbientWorld.release`, `src/sim/ambient/ambient.ts#AmbientWorld.stand`
- `src/core/rng.ts#Rng.float` (method, line 57) — call by 13: `src/sim/ambient/ambient.ts#AmbientWorld.across`, `src/sim/ambient/ambient.ts#AmbientWorld.company`, `src/sim/ambient/ambient.ts#AmbientWorld.edgePeople`, `src/sim/ambient/ambient.ts#AmbientWorld.keepWalking`, …
- `src/core/rng.ts#Rng.int` (method, line 67) — call by 1: `src/sim/meso/meso.ts#Meso.handoffDestination`
- `src/core/rng.ts#Rng.range` (method, line 62) — call by 1: `src/sim/vehicles/spawn.ts#spawnVehicleAt`
- `src/core/rng.ts#Rng.weighted` (method, line 87) — call by 2: `src/sim/ambient/ambient.ts#AmbientWorld.openCars`, `src/sim/ambient/ambient.ts#AmbientWorld.traffic`

### src/core/runPool.ts

- `src/core/runPool.ts#RunPool` (class, line 2) — construct by 1: `src/sim/ambient/ambientIndex.ts#SightWalls.rings`
- `src/core/runPool.ts#RunPool.clear` (method, line 50) — call by 1: `src/sim/ambient/ambientIndex.ts#SightWalls.fileAll`
- `src/core/runPool.ts#RunPool.data` (field, line 6) — read by 2: `src/sim/ambient/ambientIndex.ts#SightWalls.blocked`, `src/sim/ambient/ambientIndex.ts#SightWalls.file`
- `src/core/runPool.ts#RunPool.drop` (method, line 43) — call by 1: `src/sim/ambient/ambientIndex.ts#SightWalls.unfile`
- `src/core/runPool.ts#RunPool.length` (method, line 16) — call by 1: `src/sim/ambient/ambientIndex.ts#SightWalls.blocked`
- `src/core/runPool.ts#RunPool.put` (method, line 21) — call by 1: `src/sim/ambient/ambientIndex.ts#SightWalls.file`
- `src/core/runPool.ts#RunPool.start` (method, line 11) — call by 1: `src/sim/ambient/ambientIndex.ts#SightWalls.blocked`

### src/core/scalar.ts

- `src/core/scalar.ts#COARSE_EPS` (variable, line 12) — read by 1: `src/sim/peds/sidewalk.ts#SidewalkGraph.addEdge`
- `src/core/scalar.ts#EPS` (variable, line 6) — read by 1: `src/world/walkingLines.ts#EDGE_EPS`

### src/core/units.ts

- `src/core/units.ts#m` (function, line 6) — call by 111: `src/render/citizenCasting.ts#CAST_NEAR`, `src/render/citizenGait.ts#drawnVelocity`, `src/render/riggedCitizens.ts#BODY_EXTENT`, `src/render/riggedCitizens.ts#createRiggedCitizens.draw`, …
- `src/core/units.ts#perM` (function, line 9) — call by 1: `src/render/riggedCitizens.ts#citizenBandAt`
- `src/core/units.ts#stepsIn` (function, line 15) — call by 5: `src/world/walkingLines.ts#stretch`, `src/sim/ambient/ambient.ts#AmbientWorld.peopleTarget`, `src/sim/ambient/ambient.ts#AmbientWorld.stand`, `src/sim/ambient/ambient.ts#AmbientWorld.step`, …

### src/core/vec2.ts

- `src/core/vec2.ts#addScaled` (function, line 43) — call by 3: `src/sim/peds/sidewalk.ts#clearOfJunction`, `src/sim/peds/sidewalk.ts#SidewalkGraph.footwaysOf`, `src/sim/peds/sidewalk.ts#SidewalkGraph.kerbsAt`
- `src/core/vec2.ts#dist` (function, line 20) — call by 1: `src/sim/peds/sidewalk.ts#SidewalkGraph.footwaysOf`
- `src/core/vec2.ts#perp` (function, line 23) — call by 3: `src/sim/peds/sidewalk.ts#clearOfJunction`, `src/sim/peds/sidewalk.ts#SidewalkGraph.footwaysOf`, `src/sim/peds/sidewalk.ts#SidewalkGraph.kerbsAt`
- `src/core/vec2.ts#Vec2` (interface, line 4) — read by 48: `src/sim/agents/walk.ts#createAgentWalkEngine`, `src/sim/agents/walk.ts#createAgentWalkEngine.walk`, `src/sim/agents/walk.ts#frameInto`, `src/sim/agents/walk.ts#gapOpen`, …
- `src/core/vec2.ts#Vec2` (interface, line 4) — typeRef by 49: `src/sim/agents/walk.ts#joinFootway`, `src/sim/agents/walk.ts#joinsFootway`, `src/sim/agents/walk.ts#lastOf`, `src/sim/agents/walk.ts#lotSteps`, …

### src/core/weightedSlots.ts

- `src/core/weightedSlots.ts#WeightedSlots.clear` (method, line 87) — call by 1: `src/sim/agents/lotTraffic.ts#LotTraffic.reset`

### src/editor/save.worker.ts

- `src/editor/save.worker.ts#serve` (function, line 12) — callbackAssign by 2: `src/render/citizenBake.worker.ts#serve`, `src/render/people/lodWorker.ts#serve`

### src/globals.d.ts

- `src/globals.d.ts#urls` (variable, line 5) — read by 2: `src/people/body/proxy.ts#proxyUrl`, `src/render/people/skinAppearance.ts#skinChoice`

### src/growth.worker.ts

- `src/growth.worker.ts#serve` (function, line 5) — callbackAssign by 2: `src/render/citizenBake.worker.ts#serve`, `src/render/people/lodWorker.ts#serve`

### src/job.worker.ts

- `src/job.worker.ts#<ExpressionStatement>` (statement, line 64) — callbackAssign by 2: `src/render/citizenBake.worker.ts#serve`, `src/render/people/lodWorker.ts#serve`
- `src/job.worker.ts#<ExpressionStatement>` (statement, line 64) — implementation by 3: `src/render/citizenBake.worker.ts#bakeJob`, `src/render/citizenBake.worker.ts#serve`, `src/render/people/lodWorker.ts#serve`
- `src/job.worker.ts#adopt` (function, line 40) — callbackAssign by 2: `src/render/citizenBake.worker.ts#serve`, `src/render/people/lodWorker.ts#serve`
- `src/job.worker.ts#adopt` (function, line 40) — implementation by 3: `src/render/citizenBake.worker.ts#bakeJob`, `src/render/citizenBake.worker.ts#serve`, `src/render/people/lodWorker.ts#serve`

### src/jobs/jobPool.ts

- `src/jobs/jobPool.ts#jobPort` (function, line 669) — call by 1: `src/render/people/lodPool.ts#lodPool`

### src/jobs/jobScope.ts

- `src/jobs/jobScope.ts#JobScope` (interface, line 2) — call by 2: `src/render/citizenBake.worker.ts#bakeJob`, `src/render/people/lodWorker.ts#serve`
- `src/jobs/jobScope.ts#JobScope` (interface, line 2) — typeRef by 4: `src/render/citizenBake.worker.ts#bakeJob`, `src/render/citizenBake.worker.ts#serve`, `src/render/people/garmentPadding.worker.ts#serve`, `src/render/people/lodWorker.ts#serve`
- `src/jobs/jobScope.ts#JobScope` (interface, line 2) — write by 2: `src/render/citizenBake.worker.ts#serve`, `src/render/people/lodWorker.ts#serve`
- `src/jobs/jobScope.ts#JobService` (type, line 8) — typeRef by 2: `src/render/pagePorts.ts#hasLanes`, `src/render/pagePorts.ts#laneOf`

### src/networkBake.worker.ts

- `src/networkBake.worker.ts#serve` (function, line 32) — callbackAssign by 2: `src/render/citizenBake.worker.ts#serve`, `src/render/people/lodWorker.ts#serve`

### src/render/agents.ts

- `src/render/agents.ts#createAgentMeshes` (function, line 552) — indirectCall by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.load`

### src/render/buildings/cell.worker.ts

- `src/render/buildings/cell.worker.ts#serve` (function, line 5) — callbackAssign by 2: `src/render/citizenBake.worker.ts#serve`, `src/render/people/lodWorker.ts#serve`

### src/render/buildings/cellPool.ts

- `src/render/buildings/cellPool.ts#FloorColumns.constructor` (constructor, line 120) — callbackAssign by 2: `src/sim/ambient/ambientIndex.ts#BuildingMix.constructor`, `src/sim/ambient/ambientIndex.ts#SightWalls.constructor`

### src/render/buildings/fracture.worker.ts

- `src/render/buildings/fracture.worker.ts#serve` (function, line 5) — callbackAssign by 2: `src/render/citizenBake.worker.ts#serve`, `src/render/people/lodWorker.ts#serve`

### src/render/buildings/townColumns.ts

- `src/render/buildings/townColumns.ts#TownColumns.constructor` (constructor, line 85) — callbackAssign by 2: `src/sim/ambient/ambientIndex.ts#BuildingMix.constructor`, `src/sim/ambient/ambientIndex.ts#SightWalls.constructor`

### src/render/depthConvention.ts

- `src/render/depthConvention.ts#pullTowardEye` (function, line 28) — call by 2: `src/render/people/proceduralCrowd.ts#pieceMaterial`, `src/render/blobShadows.ts#blobMesh`

### src/render/derivedWriter.worker.ts

- `src/render/derivedWriter.worker.ts#serve` (function, line 23) — callbackAssign by 2: `src/render/citizenBake.worker.ts#serve`, `src/render/people/lodWorker.ts#serve`

### src/render/ecology.worker.ts

- `src/render/ecology.worker.ts#serve` (function, line 8) — callbackAssign by 2: `src/render/citizenBake.worker.ts#serve`, `src/render/people/lodWorker.ts#serve`

### src/render/flatGround.worker.ts

- `src/render/flatGround.worker.ts#serve` (function, line 7) — callbackAssign by 2: `src/render/citizenBake.worker.ts#serve`, `src/render/people/lodWorker.ts#serve`

### src/render/groundShape.worker.ts

- `src/render/groundShape.worker.ts#serve` (function, line 5) — callbackAssign by 2: `src/render/citizenBake.worker.ts#serve`, `src/render/people/lodWorker.ts#serve`

### src/render/groundShear.ts

- `src/render/groundShear.ts#Gradient` (interface, line 4) — read by 1: `src/render/groundShear.ts#shearMatrix`
- `src/render/groundShear.ts#Gradient` (interface, line 4) — typeRef by 3: `src/render/riggedCitizens.ts#createRiggedCitizens.draw`, `src/render/riggedCitizens.ts#createRiggedCitizens.emit`, `src/render/groundShear.ts#shearMatrix`

### src/render/instanceBounds.ts

- `src/render/instanceBounds.ts#fitInstances` (function, line 5) — call by 2: `src/render/people/heldProps.ts#createHeldProps.put`, `src/render/riggedCitizens.ts#createRiggedCitizens.finish`

### src/render/landStroke.worker.ts

- `src/render/landStroke.worker.ts#serve` (function, line 5) — callbackAssign by 2: `src/render/citizenBake.worker.ts#serve`, `src/render/people/lodWorker.ts#serve`

### src/render/mesh/gpuOnly.ts

- `src/render/mesh/gpuOnly.ts#fillArrays` (function, line 37) — call by 1: `src/render/riggedCitizens.ts#keepPartOnGpu`
- `src/render/mesh/gpuOnly.ts#GeometryArrays` (type, line 6) — typeRef by 2: `src/render/riggedCitizens.ts#createRiggedCitizens.partArraysAgain`, `src/render/riggedCitizens.ts#keepPartOnGpu`
- `src/render/mesh/gpuOnly.ts#gpuOnlyAttributes` (function, line 22) — call by 1: `src/render/riggedCitizens.ts#keepPartOnGpu`

### src/render/networkFarRoads.worker.ts

- `src/render/networkFarRoads.worker.ts#serve` (function, line 5) — callbackAssign by 2: `src/render/citizenBake.worker.ts#serve`, `src/render/people/lodWorker.ts#serve`

### src/render/pagePorts.ts

- `src/render/pagePorts.ts#providers` (variable, line 6) — read by 2: `src/render/pagePorts.ts#hasLanes`, `src/render/pagePorts.ts#laneOf`

### src/render/partLevelsWork.ts

- `src/render/partLevelsWork.ts#ArrayOf` (interface, line 30) — read by 2: `src/render/people/lodPool.ts#lodPool`, `src/render/partLevelsWork.ts#levelBuffers`
- `src/render/partLevelsWork.ts#ladderIndices` (function, line 10) — call by 1: `src/render/people/lodWorker.ts#serve`
- `src/render/partLevelsWork.ts#LevelArrays` (type, line 41) — read by 1: `src/render/partLevelsWork.ts#levelBuffers`
- `src/render/partLevelsWork.ts#PartArrays` (interface, line 33) — read by 1: `src/render/people/lodPool.ts#lodPool`
- `src/render/partLevelsWork.ts#PartArrays` (interface, line 33) — typeRef by 2: `src/render/people/lodPort.ts#LodPool`, `src/render/people/lodWorker.ts#PartRequest`
- `src/render/partLevelsWork.ts#partLevelArrays` (function, line 54) — call by 1: `src/render/people/lodWorker.ts#serve`
- `src/render/partLevelsWork.ts#PartLevels` (interface, line 47) — read by 1: `src/render/partLevelsWork.ts#levelBuffers`
- `src/render/partLevelsWork.ts#PartLevels` (interface, line 47) — typeRef by 3: `src/render/people/lodPool.ts#lodPool`, `src/render/people/lodPort.ts#LodPool`, `src/render/partLevelsWork.ts#levelBuffers`
- `src/render/partLevelsWork.ts#Weld` (type, line 27) — typeRef by 2: `src/render/people/lodPort.ts#LodPool`, `src/render/people/lodWorker.ts#PartRequest`

### src/render/quality.ts

- `src/render/quality.ts#HAIR_STRAND_PEOPLE` (variable, line 10) — read by 2: `src/render/people/proceduralCrowd.ts#createProceduralCrowd`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.update`

### src/render/roadStations.worker.ts

- `src/render/roadStations.worker.ts#serve` (function, line 15) — callbackAssign by 2: `src/render/citizenBake.worker.ts#serve`, `src/render/people/lodWorker.ts#serve`

### src/render/roadTile.worker.ts

- `src/render/roadTile.worker.ts#serve` (function, line 102) — callbackAssign by 2: `src/render/citizenBake.worker.ts#serve`, `src/render/people/lodWorker.ts#serve`

### src/render/shadowCasters.ts

- `src/render/shadowCasters.ts#castBySize` (function, line 26) — call by 1: `src/render/people/heldProps.ts#createHeldProps`

### src/render/stagedTexels.ts

- `src/render/stagedTexels.ts#stageTexels` (function, line 68) — call by 3: `src/render/people/crowdAnimation.ts#buildClassAnimation`, `src/render/people/hairTexture.ts#hairStrandTexture`, `src/render/people/skinAppearance.ts#createCardAtlas.fresh`
- `src/render/stagedTexels.ts#TexelPart` (interface, line 13) — read by 1: `src/render/people/skinAppearance.ts#createCardAtlas.fill`
- `src/render/stagedTexels.ts#TexelPart` (interface, line 13) — typeRef by 1: `src/render/people/skinAppearance.ts#createCardAtlas.fill`

### src/render/terrainLight.worker.ts

- `src/render/terrainLight.worker.ts#serve` (function, line 5) — callbackAssign by 2: `src/render/citizenBake.worker.ts#serve`, `src/render/people/lodWorker.ts#serve`

### src/render/uploads.ts

- `src/render/uploads.ts#cancelUploads` (function, line 17) — call by 4: `src/render/people/skinAppearance.ts#acquireTexture.release`, `src/render/people/skinAppearance.ts#createCardAtlas.dispose`, `src/render/people/skinAppearance.ts#createCardAtlas.take`, `src/render/riggedCitizens.ts#createRiggedCitizens.disposeOwned`
- `src/render/uploads.ts#compileAhead` (function, line 242) — call by 2: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.makePiece`, `src/render/riggedCitizens.ts#createRiggedCitizens.load`
- `src/render/uploads.ts#compilePassAhead` (function, line 254) — call by 1: `src/render/people/crowdAnimation.ts#createPalettePass`
- `src/render/uploads.ts#queueUpload` (function, line 8) — call by 2: `src/render/people/skinAppearance.ts#applySkinAppearance`, `src/render/people/skinAppearance.ts#loadSkinAppearance`
- `src/render/uploads.ts#shadowStandIn` (function, line 75) — call by 3: `src/render/people/proceduralCrowd.ts#createProceduralCrowd.makePiece`, `src/render/people/proceduralCrowd.ts#personProgramStandIns`, `src/render/riggedCitizens.ts#createRiggedCitizens.load`
- `src/render/uploads.ts#warmAhead` (function, line 440) — call by 1: `src/render/riggedCitizens.ts#createRiggedCitizens.load`

### src/sim/agents/bay.ts

- `src/sim/agents/bay.ts#Bay` (interface, line 20) — read by 2: `src/sim/ambient/ambient.ts#AmbientWorld.stand`, `src/sim/ambient/ambient.ts#AmbientWorld.standSource`
- `src/sim/agents/bay.ts#Bay` (interface, line 20) — typeRef by 1: `src/sim/ambient/ambient.ts#AmbientWorld.stand`
- `src/sim/agents/bay.ts#BayLane` (interface, line 7) — read by 1: `src/sim/ambient/ambient.ts#AmbientWorld.stand`

### src/sim/agents/buildingOrder.ts

- `src/sim/agents/buildingOrder.ts#StoreOrder.constructor` (constructor, line 21) — callbackAssign by 2: `src/sim/ambient/ambientIndex.ts#BuildingMix.constructor`, `src/sim/ambient/ambientIndex.ts#SightWalls.constructor`

### src/sim/agents/kerbBays.ts

- `src/sim/agents/kerbBays.ts#KERB_CELL` (variable, line 54) — read by 1: `src/sim/agents/kerbBays.ts#kerbStreetsIn`
- `src/sim/agents/kerbBays.ts#KerbBook` (interface, line 72) — read by 2: `src/sim/agents/parking.ts#baySourcesIn`, `src/sim/agents/kerbBays.ts#kerbStreetsIn`
- `src/sim/agents/kerbBays.ts#KerbBook` (interface, line 72) — typeRef by 1: `src/sim/agents/kerbBays.ts#kerbStreetsIn`
- `src/sim/agents/kerbBays.ts#kerbCell` (function, line 55) — call by 1: `src/sim/agents/kerbBays.ts#kerbStreetsIn`
- `src/sim/agents/kerbBays.ts#KerbWorld` (interface, line 21) — read by 1: `src/sim/agents/parking.ts#baySourcesIn`

### src/sim/agents/laneGrid.ts

- `src/sim/agents/laneGrid.ts#lanesInBox` (function, line 183) — call by 1: `src/sim/ambient/ambient.ts#AmbientWorld.lanesAround`
- `src/sim/agents/laneGrid.ts#lanesNearInto` (function, line 178) — call by 1: `src/sim/world.ts#SimWorld.someVehicleNear`

### src/sim/agents/lorryTraffic.ts

- `src/sim/agents/lorryTraffic.ts#Lorry` (interface, line 44) — read by 1: `src/sim/agents/lorryTraffic.ts#LorryTraffic.reset`
- `src/sim/agents/lorryTraffic.ts#LorryTraffic.clock` (field, line 71) — write by 1: `src/sim/agents/lorryTraffic.ts#LorryTraffic.reset`
- `src/sim/agents/lorryTraffic.ts#LorryTraffic.lorries` (field, line 70) — read by 1: `src/sim/agents/lorryTraffic.ts#LorryTraffic.reset`
- `src/sim/agents/lorryTraffic.ts#LorryTraffic.lorries` (field, line 70) — write by 1: `src/sim/agents/lorryTraffic.ts#LorryTraffic.reset`
- `src/sim/agents/lorryTraffic.ts#LorryTraffic.making` (field, line 177) — write by 1: `src/sim/agents/lorryTraffic.ts#LorryTraffic.reset`
- `src/sim/agents/lorryTraffic.ts#LorryTraffic.planning` (field, line 73) — write by 1: `src/sim/agents/lorryTraffic.ts#LorryTraffic.reset`
- `src/sim/agents/lorryTraffic.ts#LorryTraffic.world` (field, line 80) — read by 1: `src/sim/agents/lorryTraffic.ts#LorryTraffic.reset`

### src/sim/agents/lotBays.ts

- `src/sim/agents/lotBays.ts#personGates` (function, line 242) — call by 1: `src/sim/agents/lotDoors.ts#personGates`
- `src/sim/agents/lotBays.ts#stallFits` (function, line 60) — call by 1: `src/sim/ambient/ambient.ts#AmbientWorld.stand`

### src/sim/agents/lotNav.ts

- `src/sim/agents/lotNav.ts#buildLotGrid` (function, line 178) — call by 1: `src/sim/agents/lotDoors.ts#workOut`
- `src/sim/agents/lotNav.ts#PEOPLE_GRID` (variable, line 31) — read by 1: `src/sim/agents/lotDoors.ts#workOut`
- `src/sim/agents/lotNav.ts#sees` (function, line 273) — call by 1: `src/sim/agents/lotDoors.ts#workOut`
- `src/sim/agents/lotNav.ts#wayOut` (function, line 285) — call by 1: `src/sim/agents/lotDoors.ts#workOut`
- `src/sim/agents/lotNav.ts#wayOut` (function, line 285) — read by 1: `src/sim/agents/lotDoors.ts#workOut`

### src/sim/agents/lotTraffic.ts

- `src/sim/agents/lotTraffic.ts#LotTraffic.active` (field, line 131) — read by 1: `src/sim/agents/lotTraffic.ts#LotTraffic.reset`
- `src/sim/agents/lotTraffic.ts#LotTraffic.active` (field, line 131) — write by 1: `src/sim/agents/lotTraffic.ts#LotTraffic.reset`
- `src/sim/agents/lotTraffic.ts#LotTraffic.baysRead` (field, line 96) — write by 1: `src/sim/agents/lotTraffic.ts#LotTraffic.reset`
- `src/sim/agents/lotTraffic.ts#LotTraffic.baysSeen` (field, line 97) — write by 1: `src/sim/agents/lotTraffic.ts#LotTraffic.reset`
- `src/sim/agents/lotTraffic.ts#LotTraffic.bodies` (field, line 109) — read by 1: `src/sim/agents/lotTraffic.ts#LotTraffic.reset`
- `src/sim/agents/lotTraffic.ts#LotTraffic.bodies` (field, line 109) — write by 1: `src/sim/agents/lotTraffic.ts#LotTraffic.reset`
- `src/sim/agents/lotTraffic.ts#LotTraffic.buildingsSeen` (field, line 117) — write by 1: `src/sim/agents/lotTraffic.ts#LotTraffic.reset`
- `src/sim/agents/lotTraffic.ts#LotTraffic.called` (field, line 126) — write by 1: `src/sim/agents/lotTraffic.ts#LotTraffic.reset`
- `src/sim/agents/lotTraffic.ts#LotTraffic.carBySlot` (field, line 113) — read by 1: `src/sim/agents/lotTraffic.ts#LotTraffic.reset`
- `src/sim/agents/lotTraffic.ts#LotTraffic.cars` (field, line 92) — read by 1: `src/sim/agents/lotTraffic.ts#LotTraffic.reset`
- `src/sim/agents/lotTraffic.ts#LotTraffic.cars` (field, line 92) — write by 1: `src/sim/agents/lotTraffic.ts#LotTraffic.reset`
- `src/sim/agents/lotTraffic.ts#LotTraffic.carsAt` (field, line 108) — read by 1: `src/sim/agents/lotTraffic.ts#LotTraffic.reset`
- `src/sim/agents/lotTraffic.ts#LotTraffic.carsAt` (field, line 108) — write by 1: `src/sim/agents/lotTraffic.ts#LotTraffic.reset`
- `src/sim/agents/lotTraffic.ts#LotTraffic.clock` (field, line 94) — write by 1: `src/sim/agents/lotTraffic.ts#LotTraffic.reset`
- `src/sim/agents/lotTraffic.ts#LotTraffic.comingCount` (field, line 135) — write by 1: `src/sim/agents/lotTraffic.ts#LotTraffic.reset`
- `src/sim/agents/lotTraffic.ts#LotTraffic.dropEveryBay` (method, line 407) — call by 1: `src/sim/agents/lotTraffic.ts#LotTraffic.reset`
- `src/sim/agents/lotTraffic.ts#LotTraffic.leftBy` (field, line 128) — write by 1: `src/sim/agents/lotTraffic.ts#LotTraffic.reset`
- `src/sim/agents/lotTraffic.ts#LotTraffic.lorries` (field, line 124) — read by 1: `src/sim/agents/lotTraffic.ts#LotTraffic.reset`
- `src/sim/agents/lotTraffic.ts#LotTraffic.lorriesAt` (field, line 110) — read by 1: `src/sim/agents/lotTraffic.ts#LotTraffic.reset`
- `src/sim/agents/lotTraffic.ts#LotTraffic.lorriesAt` (field, line 110) — write by 1: `src/sim/agents/lotTraffic.ts#LotTraffic.reset`
- `src/sim/agents/lotTraffic.ts#LotTraffic.movingCount` (field, line 136) — write by 1: `src/sim/agents/lotTraffic.ts#LotTraffic.reset`
- `src/sim/agents/lotTraffic.ts#LotTraffic.movingIn` (field, line 137) — read by 1: `src/sim/agents/lotTraffic.ts#LotTraffic.reset`
- `src/sim/agents/lotTraffic.ts#LotTraffic.movingIn` (field, line 137) — write by 1: `src/sim/agents/lotTraffic.ts#LotTraffic.reset`
- `src/sim/agents/lotTraffic.ts#LotTraffic.nextOrder` (field, line 93) — write by 1: `src/sim/agents/lotTraffic.ts#LotTraffic.reset`
- `src/sim/agents/lotTraffic.ts#LotTraffic.owed` (field, line 260) — write by 1: `src/sim/agents/lotTraffic.ts#LotTraffic.reset`
- `src/sim/agents/lotTraffic.ts#LotTraffic.parkedIn` (field, line 127) — write by 1: `src/sim/agents/lotTraffic.ts#LotTraffic.reset`
- `src/sim/agents/lotTraffic.ts#LotTraffic.sceneryMoved` (field, line 122) — indirectCall by 3: `src/sim/ambient/ambient.ts#AmbientWorld.release`, `src/sim/ambient/ambient.ts#AmbientWorld.stand`, `src/sim/ambient/ambient.ts#AmbientWorld.takeScenery`
- `src/sim/agents/lotTraffic.ts#LotTraffic.stays` (field, line 115) — read by 1: `src/sim/agents/lotTraffic.ts#LotTraffic.reset`
- `src/sim/agents/lotTraffic.ts#LotTraffic.stays` (field, line 115) — write by 1: `src/sim/agents/lotTraffic.ts#LotTraffic.reset`
- `src/sim/agents/lotTraffic.ts#LotTraffic.step` (method, line 220) — callbackAssign by 3: `src/sim/ambient/ambient.ts#AmbientWorld.release`, `src/sim/ambient/ambient.ts#AmbientWorld.stand`, `src/sim/ambient/ambient.ts#AmbientWorld.takeScenery`
- `src/sim/agents/lotTraffic.ts#LotTraffic.time` (field, line 133) — write by 1: `src/sim/agents/lotTraffic.ts#LotTraffic.reset`
- `src/sim/agents/lotTraffic.ts#LotTraffic.waiting` (field, line 112) — read by 1: `src/sim/agents/lotTraffic.ts#LotTraffic.reset`
- `src/sim/agents/lotTraffic.ts#LotTraffic.waiting` (field, line 112) — write by 1: `src/sim/agents/lotTraffic.ts#LotTraffic.reset`

### src/sim/agents/parking.ts

- `src/sim/agents/parking.ts#BayBook` (interface, line 92) — read by 1: `src/sim/agents/parking.ts#baySourcesIn`
- `src/sim/agents/parking.ts#bayChangesSince` (function, line 327) — call by 1: `src/sim/ambient/ambient.ts#AmbientWorld.parking`
- `src/sim/agents/parking.ts#baysNow` (function, line 233) — call by 1: `src/sim/agents/parking.ts#baySourcesIn`
- `src/sim/agents/parking.ts#baysOfSource` (function, line 354) — call by 1: `src/sim/ambient/ambient.ts#AmbientWorld.standSource`
- `src/sim/agents/parking.ts#baySources` (function, line 335) — call by 1: `src/sim/ambient/ambient.ts#AmbientWorld.parking`
- `src/sim/agents/parking.ts#BayWorld` (interface, line 26) — typeRef by 1: `src/sim/agents/parking.ts#baySourcesIn`

### src/sim/agents/trafficBuildings.ts

- `src/sim/agents/trafficBuildings.ts#docBuildings` (function, line 174) — read by 1: `src/sim/agents/trafficBuildings.ts#docBuildings.eachFootprints`
- `src/sim/agents/trafficBuildings.ts#docBuildings.eachFacts` (method, line 211) — implementation by 2: `src/sim/ambient/ambientIndex.ts#BuildingMix.fileAll`, `src/sim/ambient/lotPlaces.ts#LotPlaces.fileAll`
- `src/sim/agents/trafficBuildings.ts#docBuildings.facts` (method, line 198) — implementation by 2: `src/sim/ambient/ambientIndex.ts#BuildingMix.sync`, `src/sim/ambient/lotPlaces.ts#LotPlaces.sync`
- `src/sim/agents/trafficBuildings.ts#docBuildings.footprints` (method, line 206) — implementation by 1: `src/sim/ambient/ambientIndex.ts#SightWalls.sync`
- `src/sim/agents/trafficBuildings.ts#docBuildings.get` (method, line 190) — implementation by 1: `src/sim/ambient/lotPlaces.ts#LotPlaces.workOut`
- `src/sim/agents/trafficBuildings.ts#docBuildings.nearIds` (method, line 217) — implementation by 1: `src/sim/agents/parking.ts#baySourcesIn`
- `src/sim/agents/trafficBuildings.ts#docBuildings.trafficBuilding` (function, line 177) — call by 1: `src/sim/agents/trafficBuildings.ts#docBuildings.eachFootprints`
- `src/sim/agents/trafficBuildings.ts#TrafficBox` (interface, line 26) — read by 1: `src/sim/ambient/lotPlaces.ts#LotPlaces.file`
- `src/sim/agents/trafficBuildings.ts#TrafficBuilding` (interface, line 33) — read by 3: `src/sim/agents/trafficBuildings.ts#docBuildings.eachFootprints`, `src/sim/agents/lotDoors.ts#workOut`, `src/sim/agents/lotDoors.ts#streetWay`
- `src/sim/agents/trafficBuildings.ts#TrafficBuilding` (interface, line 33) — typeRef by 6: `src/sim/ambient/lotPlaces.ts#PlaceDeps`, `src/sim/agents/lotDoors.ts#DOORS`, `src/sim/agents/lotDoors.ts#doorWay`, `src/sim/agents/lotDoors.ts#workOut`, …
- `src/sim/agents/trafficBuildings.ts#TrafficBuildings` (interface, line 141) — call by 8: `src/sim/ambient/ambientIndex.ts#BuildingMix.fileAll`, `src/sim/ambient/ambientIndex.ts#BuildingMix.sync`, `src/sim/ambient/ambientIndex.ts#SightWalls.fileAll`, `src/sim/ambient/ambientIndex.ts#SightWalls.sync`, …
- `src/sim/agents/trafficBuildings.ts#TrafficBuildings` (interface, line 141) — typeRef by 7: `src/sim/ambient/ambientIndex.ts#BuildingMix.fileAll`, `src/sim/ambient/ambientIndex.ts#BuildingMix.sync`, `src/sim/ambient/ambientIndex.ts#SightWalls.fileAll`, `src/sim/ambient/ambientIndex.ts#SightWalls.sync`, …
- `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.eachFacts` (method, line 335) — implementation by 2: `src/sim/ambient/ambientIndex.ts#BuildingMix.fileAll`, `src/sim/ambient/lotPlaces.ts#LotPlaces.fileAll`
- `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.facts` (method, line 322) — implementation by 2: `src/sim/ambient/ambientIndex.ts#BuildingMix.sync`, `src/sim/ambient/lotPlaces.ts#LotPlaces.sync`
- `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.footprints` (method, line 347) — call by 1: `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.eachFootprints`
- `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.footprints` (method, line 347) — implementation by 1: `src/sim/ambient/ambientIndex.ts#SightWalls.sync`
- `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.get` (method, line 306) — implementation by 1: `src/sim/ambient/lotPlaces.ts#LotPlaces.workOut`
- `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.ids` (method, line 315) — call by 1: `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.eachFootprints`
- `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.nearIds` (method, line 418) — implementation by 1: `src/sim/agents/parking.ts#baySourcesIn`
- `src/sim/agents/trafficBuildings.ts#TrafficFacts` (interface, line 91) — read by 2: `src/sim/ambient/ambientIndex.ts#BuildingMix.file`, `src/sim/ambient/lotPlaces.ts#LotPlaces.file`
- `src/sim/agents/trafficBuildings.ts#TrafficFacts` (interface, line 91) — typeRef by 2: `src/sim/ambient/ambientIndex.ts#BuildingMix.file`, `src/sim/ambient/lotPlaces.ts#LotPlaces.file`
- `src/sim/agents/trafficBuildings.ts#TrafficPart` (type, line 30) — typeRef by 1: `src/sim/agents/lotDoors.ts#personGates`

### src/sim/audit.ts

- `src/sim/audit.ts#AuditIssue` (interface, line 29) — typeRef by 2: `src/sim/agents/walk.ts#createAgentWalkEngine`, `src/sim/people/engine.ts#PedestrianEngine`

### src/sim/city/city.ts

- `src/sim/city/city.ts#City.lots` (field, line 16) — read by 1: `src/sim/ambient/ambient.ts#AmbientWorld.reset`
- `src/sim/city/city.ts#City.minutes` (method, line 21) — call by 5: `src/sim/ambient/ambient.ts#AmbientWorld.edgePeople`, `src/sim/ambient/ambient.ts#AmbientWorld.keepWalking`, `src/sim/ambient/ambient.ts#AmbientWorld.openPeople`, `src/sim/ambient/ambient.ts#AmbientWorld.peopleTarget`, …
- `src/sim/city/city.ts#City.transit` (field, line 14) — read by 1: `src/sim/agents/walk.ts#stepWalkers`

### src/sim/clock.ts

- `src/sim/clock.ts#SimClock.tick` (field, line 7) — read by 3: `src/sim/crossings/permission.ts#heldOnlyByPedestrians`, `src/sim/ambient/ambient.ts#AmbientWorld.release`, `src/sim/vehicles/spawn.ts#spawnVehicleAt`

### src/sim/dt.ts

- `src/sim/dt.ts#DT` (variable, line 2) — read by 6: `src/render/citizenGait.ts#drawnVelocity`, `src/render/riggedCitizens.ts#createRiggedCitizens.draw`, `src/sim/agents/walk.ts#movePlayerWalker`, `src/sim/agents/walk.ts#stepWalkers`, …

### src/sim/meso/demand.ts

- `src/sim/meso/demand.ts#DemandCells.constructor` (constructor, line 36) — callbackAssign by 2: `src/sim/ambient/ambientIndex.ts#BuildingMix.constructor`, `src/sim/ambient/ambientIndex.ts#SightWalls.constructor`

### src/sim/meso/meso.ts

- `src/sim/meso/meso.ts#HANDOFF_TRIES` (variable, line 37) — read by 1: `src/sim/meso/meso.ts#Meso.handoffDestination`
- `src/sim/meso/meso.ts#MESO_DT` (variable, line 23) — read by 1: `src/sim/meso/meso.ts#Meso.model`

### src/sim/params.ts

- `src/sim/params.ts#DRIVER_NOISE` (variable, line 131) — read by 1: `src/sim/vehicles/spawn.ts#spawnVehicleAt`
- `src/sim/params.ts#FLEET_CEILING` (variable, line 119) — read by 1: `src/sim/ambient/ambient.ts#AmbientWorld.trimToCeilings`
- `src/sim/params.ts#JAM_GAP` (variable, line 47) — read by 1: `src/sim/vehicles/spawn.ts#spawnVehicleAt`
- `src/sim/params.ts#PED` (variable, line 93) — read by 3: `src/sim/agents/walk.ts#addPlayerWalker`, `src/sim/agents/walk.ts#startWalk`, `src/sim/crossings/permission.ts#pedGapAccepted`
- `src/sim/params.ts#PED_CEILING` (variable, line 120) — read by 2: `src/sim/ambient/ambient.ts#AmbientWorld.peopleTarget`, `src/sim/ambient/ambient.ts#AmbientWorld.trimToCeilings`

### src/sim/pose.ts

- `src/sim/pose.ts#AXLE_NOW` (variable, line 146) — read by 1: `src/sim/pose.ts#vehiclePoseNowInto`
- `src/sim/pose.ts#axleFrame` (function, line 151) — call by 1: `src/sim/pose.ts#vehiclePoseNowInto`
- `src/sim/pose.ts#bodyOffset` (function, line 39) — call by 1: `src/sim/pose.ts#vehiclePoseNowInto`
- `src/sim/pose.ts#laneChangeYaw` (function, line 31) — call by 1: `src/sim/pose.ts#vehiclePoseNowInto`
- `src/sim/pose.ts#newPose` (function, line 23) — call by 1: `src/sim/agents/walk.ts#TRAFFIC_POSE`
- `src/sim/pose.ts#Pose` (interface, line 11) — read by 3: `src/sim/crossings/permission.ts#vehicleBodyIntersectsCrossing`, `src/sim/ambient/ambient.ts#AmbientWorld.traffic`, `src/sim/ambient/ambient.ts#AmbientWorld.trimToCeilings`
- `src/sim/pose.ts#PoseOut` (interface, line 17) — read by 2: `src/sim/agents/walk.ts#gapOpen`, `src/sim/pose.ts#vehiclePoseNowInto`
- `src/sim/pose.ts#PoseOut` (interface, line 17) — typeRef by 1: `src/sim/pose.ts#vehiclePoseNowInto`
- `src/sim/pose.ts#PoseOut` (interface, line 17) — write by 1: `src/sim/pose.ts#vehiclePoseNowInto`
- `src/sim/pose.ts#vehiclePose` (function, line 46) — call by 3: `src/sim/crossings/permission.ts#vehicleBodyIntersectsCrossing`, `src/sim/ambient/ambient.ts#AmbientWorld.traffic`, `src/sim/ambient/ambient.ts#AmbientWorld.trimToCeilings`

### src/sim/routing/destination.ts

- `src/sim/routing/destination.ts#chooseVehicleDestination` (function, line 15) — call by 1: `src/sim/ambient/ambient.ts#AmbientWorld.traffic`
- `src/sim/routing/destination.ts#chooseVehicleDestination` (function, line 15) — reference by 1: `src/sim/vehicles/spawn.ts#spawnVehicleAt`

### src/sim/routing/laneReach.ts

- `src/sim/routing/laneReach.ts#LaneReaches.mayReach` (method, line 353) — call by 1: `src/sim/meso/meso.ts#Meso.handoffDestination`

### src/sim/routing/router.ts

- `src/sim/routing/router.ts#planFrom` (function, line 107) — call by 2: `src/sim/ambient/ambient.ts#AmbientWorld.traffic`, `src/sim/vehicles/spawn.ts#spawnVehicleAt`

### src/sim/signals/fsm.ts

- `src/sim/signals/fsm.ts#SignalController` (interface, line 21) — read by 1: `src/sim/world.ts#SimWorld.signalAtCrossing`
- `src/sim/signals/fsm.ts#SignalController` (interface, line 21) — typeRef by 1: `src/sim/crossings/permission.ts#turnOwedToVehicle`

### src/sim/signals/plan.ts

- `src/sim/signals/plan.ts#CrossingId` (type, line 8) — typeRef by 16: `src/sim/agents/walk.ts#State`, `src/sim/agents/walk.ts#Walker`, `src/sim/agents/walk.ts#Zebra`, `src/sim/crossings/permission.ts#crossingReservedByVehicle`, …
- `src/sim/signals/plan.ts#makeCrossingId` (function, line 370) — call by 3: `src/sim/agents/walk.ts#crossingOf`, `src/sim/crossings/permission.ts#mayEnterCrossing`, `src/sim/peds/sidewalk.ts#SidewalkGraph.crossingsAt`
- `src/sim/signals/plan.ts#SignalPlan` (interface, line 26) — read by 1: `src/sim/world.ts#SimWorld.signalAtCrossing`
- `src/sim/signals/plan.ts#Stage` (interface, line 10) — read by 1: `src/sim/world.ts#SimWorld.signalAtCrossing`

### src/sim/signals/query.ts

- `src/sim/signals/query.ts#pedestrianSignalState` (function, line 40) — call by 1: `src/sim/crossings/permission.ts#mayEnterCrossing`
- `src/sim/signals/query.ts#signalStateFor` (function, line 9) — call by 1: `src/sim/crossings/permission.ts#turnOwedToVehicle`

### src/sim/transit/transit.ts

- `src/sim/transit/transit.ts#AHEAD_TIME` (variable, line 33) — read by 2: `src/sim/transit/transit.ts#TransitSim.trainZones`, `src/sim/transit/transit.ts#TransitSim.trainZones.far`
- `src/sim/transit/transit.ts#RailLine` (interface, line 79) — read by 1: `src/sim/transit/transit.ts#TransitSim.trainZones`
- `src/sim/transit/transit.ts#RailLine` (interface, line 79) — typeRef by 1: `src/sim/transit/transit.ts#TransitSim.trainZones.far`
- `src/sim/transit/transit.ts#Train` (interface, line 66) — read by 2: `src/sim/transit/transit.ts#TransitSim.trainZones`, `src/sim/transit/transit.ts#TransitSim.trainZones.far`
- `src/sim/transit/transit.ts#Train` (interface, line 66) — typeRef by 1: `src/sim/transit/transit.ts#TransitSim.trainZones.far`
- `src/sim/transit/transit.ts#TRAIN_AHEAD` (variable, line 31) — read by 2: `src/sim/transit/transit.ts#TransitSim.trainZones`, `src/sim/transit/transit.ts#TransitSim.trainZones.far`
- `src/sim/transit/transit.ts#TRAIN_CARS,METRO_CARS,CAR_LENGTH` (variable, line 42) — read by 2: `src/sim/transit/transit.ts#TransitSim.trainZones`, `src/sim/transit/transit.ts#TransitSim.trainZones.far`
- `src/sim/transit/transit.ts#TransitSim.pointAt` (method, line 446) — call by 2: `src/sim/transit/transit.ts#TransitSim.trainZones`, `src/sim/transit/transit.ts#TransitSim.trainZones.far`
- `src/sim/transit/transit.ts#TransitSim.pointAt` (method, line 446) — read by 1: `src/sim/transit/transit.ts#TransitSim.trainZones`
- `src/sim/transit/transit.ts#TransitSim.rails` (field, line 109) — read by 1: `src/sim/transit/transit.ts#TransitSim.trainZones`

### src/sim/vehicles/archetypes.ts

- `src/sim/vehicles/archetypes.ts#Archetype` (interface, line 12) — read by 11: `src/sim/agents/walk.ts#parkedZone`, `src/sim/crossings/permission.ts#vehicleBodyIntersectsCrossing`, `src/sim/intersections/crossingSpans.ts#reservationCoversCrossing`, `src/sim/ambient/ambient.ts#AmbientWorld.openCars`, …
- `src/sim/vehicles/archetypes.ts#Archetype` (interface, line 12) — typeRef by 2: `src/sim/ambient/ambient.ts#TRAFFIC`, `src/sim/vehicles/spawn.ts#spawnVehicleAt`
- `src/sim/vehicles/archetypes.ts#ARCHETYPES` (variable, line 50) — read by 2: `src/sim/ambient/ambient.ts#PARKED_KINDS`, `src/sim/ambient/ambient.ts#TRAFFIC`
- `src/sim/vehicles/archetypes.ts#bodyClassOfArchetype` (function, line 236) — call by 2: `src/sim/ambient/ambient.ts#AmbientWorld.traffic`, `src/sim/vehicles/spawn.ts#spawnVehicleAt`

### src/sim/vehicles/curvature.ts

- `src/sim/vehicles/curvature.ts#curveSpeedCap` (function, line 84) — call by 1: `src/sim/vehicles/spawn.ts#spawnVehicleAt`

### src/sim/vehicles/cycleLane.ts

- `src/sim/vehicles/cycleLane.ts#cycleShift` (function, line 22) — call by 1: `src/sim/pose.ts#vehiclePoseNowInto`

### src/sim/vehicles/driver.ts

- `src/sim/vehicles/driver.ts#Driver` (interface, line 20) — read by 1: `src/sim/vehicles/spawn.ts#spawnVehicleAt`
- `src/sim/vehicles/driver.ts#DriverParams` (interface, line 7) — read by 1: `src/sim/vehicles/spawn.ts#spawnVehicleAt`
- `src/sim/vehicles/driver.ts#makeDriver` (function, line 48) — call by 2: `src/sim/ambient/ambient.ts#AmbientWorld.release`, `src/sim/vehicles/spawn.ts#spawnVehicleAt`

### src/sim/vehicles/idm.ts

- `src/sim/vehicles/idm.ts#canStopComfortably` (function, line 108) — call by 2: `src/sim/crossings/permission.ts#pedGapAccepted`, `src/sim/intersections/crossingSpans.ts#reservationCoversCrossing`
- `src/sim/vehicles/idm.ts#idmAccel` (function, line 86) — call by 1: `src/sim/vehicles/spawn.ts#spawnVehicleAt`

### src/sim/vehicles/kerbStops.ts

- `src/sim/vehicles/kerbStops.ts#assignOccupancy` (function, line 126) — call by 1: `src/sim/vehicles/spawn.ts#spawnVehicleAt`

### src/sim/vehicles/laneClosures.ts

- `src/sim/vehicles/laneClosures.ts#openLanes` (function, line 26) — call by 1: `src/sim/agents/lorryTraffic.ts#LorryTraffic.reset`

### src/sim/vehicles/spawn.ts

- `src/sim/vehicles/spawn.ts#birthSpeed` (function, line 280) — call by 1: `src/sim/vehicles/spawn.ts#spawnVehicleAt`
- `src/sim/vehicles/spawn.ts#trafficTarget` (function, line 32) — call by 1: `src/sim/ambient/ambient.ts#AmbientWorld.open`

### src/sim/vehicles/state.ts

- `src/sim/vehicles/state.ts#ConnectorClearance` (interface, line 23) — read by 2: `src/sim/crossings/permission.ts#indexReservations`, `src/sim/intersections/crossingSpans.ts#reservationCoversCrossing`
- `src/sim/vehicles/state.ts#createVehicle` (function, line 181) — call by 2: `src/sim/ambient/ambient.ts#AmbientWorld.release`, `src/sim/vehicles/spawn.ts#spawnVehicleAt`
- `src/sim/vehicles/state.ts#FreePose` (interface, line 155) — read by 6: `src/sim/agents/walk.ts#moved`, `src/sim/agents/walk.ts#parkedZone`, `src/sim/agents/walk.ts#ParkedZoneIndex.place`, `src/sim/ambient/ambient.ts#AmbientWorld.moverCells`, …
- `src/sim/vehicles/state.ts#FreePose` (interface, line 155) — typeRef by 5: `src/sim/agents/walk.ts#moved`, `src/sim/agents/walk.ts#parkedZone`, `src/sim/agents/walk.ts#ParkedZoneIndex.place`, `src/sim/ambient/ambient.ts#AmbientWorld.stand`, …
- `src/sim/vehicles/state.ts#snapshot` (function, line 166) — call by 1: `src/sim/vehicles/spawn.ts#spawnVehicleAt`
- `src/sim/vehicles/state.ts#StandingCar` (interface, line 147) — inherit by 1: `src/sim/ambient/ambient.ts#SceneryCar`
- `src/sim/vehicles/state.ts#StandingCar` (interface, line 147) — read by 5: `src/sim/agents/walk.ts#parkedZone`, `src/sim/agents/walk.ts#ParkedZoneIndex.follow`, `src/sim/agents/walk.ts#ParkedZoneIndex.keep`, `src/sim/agents/walk.ts#ParkedZoneIndex.sync`, …
- `src/sim/vehicles/state.ts#StandingCar` (interface, line 147) — typeRef by 14: `src/sim/agents/walk.ts#ParkedEntry`, `src/sim/agents/walk.ts#ParkedSource`, `src/sim/agents/walk.ts#parkedZone`, `src/sim/agents/walk.ts#ParkedZoneIndex.byCar`, …
- `src/sim/vehicles/state.ts#Vehicle` (interface, line 30) — read by 13: `src/sim/agents/walk.ts#gapOpen`, `src/sim/crossings/permission.ts#heldOnlyByPedestrians`, `src/sim/crossings/permission.ts#indexReservations`, `src/sim/crossings/permission.ts#pedGapAccepted`, …
- `src/sim/vehicles/state.ts#Vehicle` (interface, line 30) — typeRef by 15: `src/sim/crossings/permission.ts#heldOnlyByPedestrians`, `src/sim/crossings/permission.ts#Reservation`, `src/sim/crossings/permission.ts#vehicleBodyIntersectsCrossing`, `src/sim/intersections/crossingSpans.ts#reservationCoversCrossing`, …
- `src/sim/vehicles/state.ts#Vehicle` (interface, line 30) — write by 3: `src/sim/ambient/ambient.ts#AmbientWorld.release`, `src/sim/ambient/ambient.ts#AmbientWorld.traffic`, `src/sim/vehicles/spawn.ts#spawnVehicleAt`

### src/sim/world.ts

- `src/sim/world.ts#BODY_OFF_LANE` (variable, line 39) — read by 1: `src/sim/world.ts#SimWorld.someVehicleNear`
- `src/sim/world.ts#LaneletRuntime` (interface, line 78) — read by 2: `src/sim/meso/meso.ts#Meso.handoffDestination`, `src/sim/world.ts#SimWorld.someVehicleNear`
- `src/sim/world.ts#MesoTraffic` (interface, line 144) — call by 1: `src/sim/ambient/ambient.ts#AmbientWorld.parking`
- `src/sim/world.ts#NO_PEDESTRIANS` (variable, line 61) — implementation by 3: `src/sim/ambient/ambient.ts#AmbientWorld.edgePeople`, `src/sim/ambient/ambient.ts#AmbientWorld.openPeople`, `src/sim/vehicles/spawn.ts#spawnVehicleAt`
- `src/sim/world.ts#SimStreams` (interface, line 91) — read by 13: `src/sim/ambient/ambient.ts#AmbientWorld.across`, `src/sim/ambient/ambient.ts#AmbientWorld.company`, `src/sim/ambient/ambient.ts#AmbientWorld.edgePeople`, `src/sim/ambient/ambient.ts#AmbientWorld.keepWalking`, …
- `src/sim/world.ts#SimWorld` (class, line 153) — typeRef by 92: `src/sim/agents/walk.ts#addPlayerWalker`, `src/sim/agents/walk.ts#createAgentWalkEngine`, `src/sim/agents/walk.ts#crossingOf`, `src/sim/agents/walk.ts#ensureGraph`, …
- `src/sim/world.ts#SimWorld.ambient` (field, line 267) — read by 1: `src/sim/agents/walk.ts#stepWalkers`
- `src/sim/world.ts#SimWorld.asleep` (field, line 199) — read by 1: `src/sim/world.ts#SimWorld.signalAtCrossing`
- `src/sim/world.ts#SimWorld.bodiesIn` (method, line 1261) — call by 2: `src/sim/crossings/permission.ts#vehicleBodyOnCrossing`, `src/sim/vehicles/spawn.ts#spawnVehicleAt`
- `src/sim/world.ts#SimWorld.bodiesIn` (method, line 1261) — read by 2: `src/sim/crossings/permission.ts#vehicleBodyOnCrossing`, `src/sim/vehicles/spawn.ts#spawnVehicleAt`
- `src/sim/world.ts#SimWorld.city` (field, line 260) — read by 7: `src/sim/agents/walk.ts#stepWalkers`, `src/sim/ambient/ambient.ts#AmbientWorld.edgePeople`, `src/sim/ambient/ambient.ts#AmbientWorld.keepWalking`, `src/sim/ambient/ambient.ts#AmbientWorld.openPeople`, …
- `src/sim/world.ts#SimWorld.clock` (field, line 154) — read by 3: `src/sim/crossings/permission.ts#heldOnlyByPedestrians`, `src/sim/ambient/ambient.ts#AmbientWorld.release`, `src/sim/vehicles/spawn.ts#spawnVehicleAt`
- `src/sim/world.ts#SimWorld.connector` (method, line 371) — call by 3: `src/sim/crossings/permission.ts#crossingReservedByVehicle`, `src/sim/crossings/permission.ts#indexReservations`, `src/sim/crossings/permission.ts#turnOwedToVehicle`
- `src/sim/world.ts#SimWorld.constructor` (constructor, line 293) — read by 8: `src/sim/crossings/permission.ts#crossingReservedByVehicle`, `src/sim/intersections/crossingSpans.ts#CrossingSpans.measureJunction`, `src/sim/ambient/ambient.ts#AmbientWorld.behindWall`, `src/sim/ambient/ambient.ts#AmbientWorld.everyEnd`, …
- `src/sim/world.ts#SimWorld.controller` (method, line 395) — call by 1: `src/sim/crossings/permission.ts#mayEnterCrossing`
- `src/sim/world.ts#SimWorld.crossingSpans` (field, line 210) — read by 3: `src/sim/crossings/permission.ts#crossingReservedByVehicle`, `src/sim/crossings/permission.ts#turnOwedToVehicle`, `src/sim/intersections/crossingSpans.ts#pedestrianInSpan`
- `src/sim/world.ts#SimWorld.crossingStates` (field, line 204) — read by 2: `src/sim/agents/walk.ts#publish`, `src/sim/intersections/crossingSpans.ts#pedestrianInSpan`
- `src/sim/world.ts#SimWorld.crossingStates` (field, line 204) — write by 1: `src/sim/agents/walk.ts#publish`
- `src/sim/world.ts#SimWorld.edgeTraffic` (field, line 258) — read by 1: `src/sim/ambient/ambient.ts#AmbientWorld.open`
- `src/sim/world.ts#SimWorld.enterLanelet` (method, line 1184) — call by 1: `src/sim/vehicles/spawn.ts#spawnVehicleAt`
- `src/sim/world.ts#SimWorld.focus` (field, line 273) — read by 5: `src/sim/ambient/ambient.ts#AmbientWorld.people`, `src/sim/ambient/ambient.ts#AmbientWorld.seen`, `src/sim/ambient/ambient.ts#AmbientWorld.step`, `src/sim/ambient/ambient.ts#AmbientWorld.traffic`, …
- `src/sim/world.ts#SimWorld.followCity` (field, line 254) — read by 1: `src/sim/ambient/ambient.ts#AmbientWorld.peopleTarget`
- `src/sim/world.ts#SimWorld.graph` (field, line 155) — read by 7: `src/sim/crossings/permission.ts#mayEnterCrossing`, `src/sim/crossings/permission.ts#turnOwedToVehicle`, `src/sim/intersections/crossingSpans.ts#CrossingSpans.build`, `src/sim/intersections/crossingSpans.ts#CrossingSpans.measureJunction`, …
- `src/sim/world.ts#SimWorld.junctionHolders` (method, line 434) — call by 1: `src/sim/crossings/permission.ts#indexReservations`
- `src/sim/world.ts#SimWorld.laneHead` (method, line 1230) — call by 2: `src/sim/crossings/permission.ts#pedGapAccepted`, `src/sim/crossings/permission.ts#turnOwedToVehicle`
- `src/sim/world.ts#SimWorld.lanelet` (method, line 361) — call by 10: `src/sim/agents/walk.ts#createAgentWalkEngine`, `src/sim/crossings/permission.ts#indexReservations`, `src/sim/crossings/permission.ts#pedGapAccepted`, `src/sim/crossings/permission.ts#turnOwedToVehicle`, …
- `src/sim/world.ts#SimWorld.laneReaches` (field, line 237) — read by 1: `src/sim/meso/meso.ts#Meso.handoffDestination`
- `src/sim/world.ts#SimWorld.meso` (field, line 269) — read by 3: `src/sim/ambient/ambient.ts#AmbientWorld.open`, `src/sim/ambient/ambient.ts#AmbientWorld.parking`, `src/sim/ambient/ambient.ts#AmbientWorld.step`
- `src/sim/world.ts#SimWorld.nearLanes` (field, line 478) — read by 1: `src/sim/world.ts#SimWorld.someVehicleNear`
- `src/sim/world.ts#SimWorld.nextVehicleId` (field, line 229) — read by 2: `src/sim/ambient/ambient.ts#AmbientWorld.stand`, `src/sim/vehicles/spawn.ts#spawnVehicleAt`
- `src/sim/world.ts#SimWorld.nextVehicleId` (field, line 229) — write by 2: `src/sim/ambient/ambient.ts#AmbientWorld.stand`, `src/sim/vehicles/spawn.ts#spawnVehicleAt`
- `src/sim/world.ts#SimWorld.pedEngine` (field, line 182) — read by 8: `src/sim/ambient/ambient.ts#AmbientWorld.across`, `src/sim/ambient/ambient.ts#AmbientWorld.company`, `src/sim/ambient/ambient.ts#AmbientWorld.edgePeople`, `src/sim/ambient/ambient.ts#AmbientWorld.openPeople`, …
- `src/sim/world.ts#SimWorld.pedestrianIntensity` (field, line 249) — read by 1: `src/sim/ambient/ambient.ts#AmbientWorld.peopleTarget`
- `src/sim/world.ts#SimWorld.pedViewById` (field, line 208) — read by 1: `src/sim/agents/walk.ts#publish`
- `src/sim/world.ts#SimWorld.pedViews` (field, line 206) — read by 3: `src/sim/agents/walk.ts#publish`, `src/sim/ambient/ambient.ts#AmbientWorld.edgePeople`, `src/sim/ambient/ambient.ts#AmbientWorld.trimToCeilings`
- `src/sim/world.ts#SimWorld.populationShare` (field, line 271) — read by 2: `src/sim/ambient/ambient.ts#AmbientWorld.peopleTarget`, `src/sim/ambient/ambient.ts#AmbientWorld.trimToCeilings`
- `src/sim/world.ts#SimWorld.removeVehicle` (method, line 1286) — call by 4: `src/sim/ambient/ambient.ts#AmbientWorld.reset`, `src/sim/ambient/ambient.ts#AmbientWorld.traffic`, `src/sim/ambient/ambient.ts#AmbientWorld.trimToCeilings`, `src/sim/vehicles/spawn.ts#spawnVehicleAt`
- `src/sim/world.ts#SimWorld.rng` (getter, line 226) — call by 13: `src/sim/ambient/ambient.ts#AmbientWorld.across`, `src/sim/ambient/ambient.ts#AmbientWorld.company`, `src/sim/ambient/ambient.ts#AmbientWorld.edgePeople`, `src/sim/ambient/ambient.ts#AmbientWorld.keepWalking`, …
- `src/sim/world.ts#SimWorld.rt` (method, line 376) — call by 2: `src/sim/meso/meso.ts#Meso.handoffDestination`, `src/sim/world.ts#SimWorld.someVehicleNear`
- `src/sim/world.ts#SimWorld.sidewalks` (field, line 157) — read by 2: `src/sim/agents/walk.ts#crossingOf`, `src/sim/intersections/crossingSpans.ts#CrossingSpans.measureJunction`
- `src/sim/world.ts#SimWorld.signalPosts` (field, line 159) — read by 1: `src/sim/agents/walk.ts#stepWalkers`
- `src/sim/world.ts#SimWorld.topologyLog` (field, line 700) — read by 2: `src/sim/ambient/ambient.ts#AmbientWorld.listLanes`, `src/sim/ambient/ambient.ts#AmbientWorld.openingLanes`
- `src/sim/world.ts#SimWorld.trafficCount` (field, line 251) — read by 1: `src/sim/ambient/ambient.ts#AmbientWorld.trimToCeilings`
- `src/sim/world.ts#SimWorld.vehicles` (field, line 163) — read by 8: `src/sim/agents/walk.ts#gapOpen`, `src/sim/ambient/ambient.ts#AmbientWorld.open`, `src/sim/ambient/ambient.ts#AmbientWorld.reset`, `src/sim/ambient/ambient.ts#AmbientWorld.step`, …
- `src/sim/world.ts#SimWorld.vehicles` (field, line 163) — write by 1: `src/sim/vehicles/spawn.ts#spawnVehicleAt`
- `src/sim/world.ts#SimWorld.wakeSignal` (method, line 601) — call by 1: `src/sim/world.ts#SimWorld.signalAtCrossing`
- `src/sim/world.ts#SimWorld.walkwaysNow` (method, line 685) — call by 2: `src/sim/agents/walk.ts#ensureGraph`, `src/sim/ambient/ambient.ts#AmbientWorld.openPeople`
- `src/sim/world.ts#VersionedMap.set` (method, line 1349) — call by 1: `src/sim/vehicles/spawn.ts#spawnVehicleAt`

### src/world/approach.ts

- `src/world/approach.ts#CROSSWALK_DEPTH` (variable, line 7) — read by 3: `src/sim/intersections/crossingSpans.ts#measure`, `src/sim/intersections/crossingSpans.ts#PED_CROSSING_STOP_BUFFER`, `src/world/walkways.ts#Walkways.buildNode`

### src/world/approachGroups.ts

- `src/world/approachGroups.ts#ApproachGroup` (interface, line 5) — read by 1: `src/sim/peds/signalPostIndex.ts#SignalPostIndex.placeJunction`

### src/world/buildings/cameraSolids.ts

- `src/world/buildings/cameraSolids.ts#SolidFiling.constructor` (constructor, line 205) — callbackAssign by 2: `src/sim/ambient/ambientIndex.ts#BuildingMix.constructor`, `src/sim/ambient/ambientIndex.ts#SightWalls.constructor`

### src/world/buildings/decayWatch.ts

- `src/world/buildings/decayWatch.ts#createDecayWatch` (function, line 8) — callbackAssign by 2: `src/sim/ambient/ambientIndex.ts#BuildingMix.constructor`, `src/sim/ambient/ambientIndex.ts#SightWalls.constructor`

### src/world/buildings/geometry.ts

- `src/world/buildings/geometry.ts#localToWorld` (function, line 32) — call by 1: `src/sim/agents/lotDoors.ts#workOut`
- `src/world/buildings/geometry.ts#worldToLocal` (function, line 59) — call by 1: `src/sim/agents/lotDoors.ts#workOut`

### src/world/buildings/pads.ts

- `src/world/buildings/pads.ts#townPads` (function, line 184) — callbackAssign by 2: `src/sim/ambient/ambientIndex.ts#BuildingMix.constructor`, `src/sim/ambient/ambientIndex.ts#SightWalls.constructor`

### src/world/buildings/siteGrading.ts

- `src/world/buildings/siteGrading.ts#siteGrading` (function, line 33) — callbackAssign by 2: `src/sim/ambient/ambientIndex.ts#BuildingMix.constructor`, `src/sim/ambient/ambientIndex.ts#SightWalls.constructor`

### src/world/buildings/store.ts

- `src/world/buildings/store.ts#BuildingStore.all` (method, line 123) — call by 1: `src/sim/agents/trafficBuildings.ts#docBuildings.eachFootprints`

### src/world/buildings/streetDoors.ts

- `src/world/buildings/streetDoors.ts#StreetDoor` (interface, line 9) — read by 1: `src/sim/agents/lotDoors.ts#streetWay`

### src/world/buildings/types.ts

- `src/world/buildings/types.ts#Building` (interface, line 284) — read by 2: `src/sim/ambient/demand.ts#kindOf`, `src/sim/agents/trafficBuildings.ts#docBuildings.eachFootprints`
- `src/world/buildings/types.ts#Building` (interface, line 284) — typeRef by 1: `src/sim/ambient/demand.ts#kindOf`
- `src/world/buildings/types.ts#BuildingElement` (interface, line 259) — read by 1: `src/sim/agents/lotDoors.ts#workOut`
- `src/world/buildings/types.ts#BuildingId` (type, line 9) — typeRef by 3: `src/sim/ambient/ambientIndex.ts#SightWalls.sync`, `src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.eachFootprints`, `src/sim/agents/lotDoors.ts#DoorWay`
- `src/world/buildings/types.ts#Volume` (interface, line 150) — read by 2: `src/sim/agents/lotDoors.ts#workOut`, `src/sim/agents/lotDoors.ts#streetWay`

### src/world/cells.ts

- `src/world/cells.ts#CELL` (variable, line 9) — read by 1: `src/world/walkable.ts#WalkableSurface.cellOf`
- `src/world/cells.ts#cellAt` (function, line 23) — call by 1: `src/world/walkable.ts#WalkableSurface.cellOf`
- `src/world/cells.ts#CellFiling` (class, line 60) — construct by 1: `src/world/walkways.ts#Walkways.reads`
- `src/world/cells.ts#CellFiling.epoch` (field, line 69) — read by 1: `src/world/walkable.ts#WalkableSurface.cellOf`
- `src/world/cells.ts#CellFiling.file` (method, line 76) — call by 1: `src/world/walkways.ts#Walkways.buildNode`
- `src/world/cells.ts#CellFiling.idsMeeting` (method, line 131) — call by 4: `src/world/walkable.ts#WalkableSurface.cellOf`, `src/world/walkways.ts#ringsIn`, `src/world/walkways.ts#Walkways.plan`, `src/sim/ambient/ambient.ts#AmbientWorld.roadEnds`
- `src/world/cells.ts#CellFiling.idsMeetingAny` (method, line 169) — call by 1: `src/world/roads/footwayRise.ts#packFlush`
- `src/world/cells.ts#CellFiling.rectOf` (method, line 120) — call by 1: `src/world/roads/footwayRise.ts#packFlush`
- `src/world/cells.ts#CellFiling.remove` (method, line 107) — call by 1: `src/world/walkways.ts#Walkways.apply`
- `src/world/cells.ts#CellFiling.someAt` (method, line 157) — call by 1: `src/world/roads/footwayRise.ts#footwayRiseShare`
- `src/world/cells.ts#CellFiling.stamp` (method, line 193) — call by 1: `src/world/walkable.ts#WalkableSurface.cellOf`
- `src/world/cells.ts#cellRect` (function, line 17) — call by 2: `src/world/walkable.ts#WalkableSurface.cellOf`, `src/world/walkable.ts#WalkableSurface.forgetChanged`

### src/world/changes.ts

- `src/world/changes.ts#Change` (interface, line 35) — read by 2: `src/sim/ambient/ambientIndex.ts#buildingsChangedSince`, `src/sim/ambient/lotPlaces.ts#LotPlaces.sync`
- `src/world/changes.ts#ChangeJournal` (class, line 156) — typeRef by 4: `src/sim/ambient/ambientIndex.ts#BuildingMix.sync`, `src/sim/ambient/ambientIndex.ts#buildingsChangedSince`, `src/sim/ambient/ambientIndex.ts#SightWalls.sync`, `src/sim/ambient/lotPlaces.ts#LotPlaces.sync`
- `src/world/changes.ts#ChangeJournal.since` (method, line 309) — call by 2: `src/sim/ambient/ambientIndex.ts#buildingsChangedSince`, `src/sim/ambient/lotPlaces.ts#LotPlaces.sync`
- `src/world/changes.ts#ChangeJournal.version` (getter, line 198) — call by 4: `src/sim/ambient/ambientIndex.ts#BuildingMix.sync`, `src/sim/ambient/ambientIndex.ts#buildingsChangedSince`, `src/sim/ambient/ambientIndex.ts#SightWalls.sync`, `src/sim/ambient/lotPlaces.ts#LotPlaces.sync`
- `src/world/changes.ts#ChangeRect` (type, line 4) — typeRef by 6: `src/world/footwayPath.ts#FootwayNet`, `src/world/walkable.ts#clipToBox`, `src/world/walkable.ts#WalkableSurface.cellOf`, `src/world/walkways.ts#boxOf`, …

### src/world/conflictTable.ts

- `src/world/conflictTable.ts#BODY_ENVELOPE` (variable, line 45) — read by 1: `src/sim/intersections/crossingSpans.ts#REACH`
- `src/world/conflictTable.ts#BodyClass` (type, line 15) — typeRef by 2: `src/sim/meso/meso.ts#Meso.handoffDestination`, `src/sim/vehicles/spawn.ts#DestinationPicker`
- `src/world/conflictTable.ts#HEAVY` (variable, line 50) — read by 1: `src/sim/intersections/crossingSpans.ts#REACH`

### src/world/doc.ts

- `src/world/doc.ts#RoadDoc` (class, line 153) — typeRef by 12: `src/sim/peds/sidewalk.ts#SidewalkGraph.build`, `src/sim/peds/sidewalk.ts#SidewalkGraph.crossingsAt`, `src/sim/peds/sidewalk.ts#SidewalkGraph.footwaysOf`, `src/sim/peds/sidewalk.ts#SidewalkGraph.kerbsAt`, …
- `src/world/doc.ts#RoadDoc.buildings` (field, line 289) — read by 1: `src/sim/agents/trafficBuildings.ts#docBuildings.eachFootprints`
- `src/world/doc.ts#RoadDoc.changes` (field, line 262) — read by 3: `src/sim/ambient/ambient.ts#AmbientWorld.behindWall`, `src/sim/ambient/ambient.ts#AmbientWorld.mixAround`, `src/sim/ambient/ambient.ts#AmbientWorld.refreshPlaces`
- `src/world/doc.ts#RoadDoc.mapEdge` (method, line 703) — call by 1: `src/sim/ambient/ambient.ts#AmbientWorld.roadEnds`
- `src/world/doc.ts#RoadDoc.node` (method, line 416) — call by 11: `src/sim/crossings/permission.ts#crossingReservedByVehicle`, `src/sim/intersections/crossingSpans.ts#CrossingSpans.measureJunction`, `src/sim/peds/sidewalk.ts#SidewalkGraph.crossingsAt`, `src/sim/peds/sidewalk.ts#SidewalkGraph.kerbsAt`, …
- `src/world/doc.ts#RoadDoc.nodes` (field, line 154) — read by 3: `src/sim/peds/sidewalk.ts#SidewalkGraph.build`, `src/world/walkways.ts#Walkways.everything`, `src/sim/ambient/ambient.ts#AmbientWorld.everyEnd`
- `src/world/doc.ts#RoadDoc.requireNode` (method, line 424) — call by 3: `src/world/walkways.ts#cornersOf`, `src/world/walkways.ts#endsAt`, `src/world/walkways.ts#Walkways.buildNode`
- `src/world/doc.ts#RoadDoc.requireSegment` (method, line 430) — call by 5: `src/world/walkways.ts#endsAt`, `src/world/walkways.ts#footwayShapes`, `src/world/walkways.ts#Walkways.buildNode`, `src/world/walkways.ts#Walkways.cut`, …
- `src/world/doc.ts#RoadDoc.segment` (method, line 420) — call by 6: `src/sim/peds/sidewalk.ts#SidewalkGraph.crossingsAt`, `src/sim/peds/sidewalk.ts#SidewalkGraph.footwaysOf`, `src/sim/peds/sidewalk.ts#SidewalkGraph.kerbsAt`, `src/world/walkways.ts#deckOf`, …
- `src/world/doc.ts#RoadDoc.segments` (field, line 155) — read by 2: `src/sim/peds/sidewalk.ts#SidewalkGraph.build`, `src/world/walkways.ts#Walkways.everything`
- `src/world/doc.ts#RoadNode` (interface, line 89) — read by 13: `src/sim/crossings/permission.ts#crossingReservedByVehicle`, `src/sim/intersections/crossingSpans.ts#CrossingSpans.measureJunction`, `src/sim/peds/sidewalk.ts#SidewalkGraph.crossingsAt`, `src/sim/peds/sidewalk.ts#SidewalkGraph.kerbsAt`, …
- `src/world/doc.ts#RoadSegment` (interface, line 124) — read by 11: `src/sim/peds/sidewalk.ts#SidewalkGraph.crossingsAt`, `src/sim/peds/sidewalk.ts#SidewalkGraph.footwaysOf`, `src/sim/peds/sidewalk.ts#SidewalkGraph.kerbsAt`, `src/world/walkways.ts#deckOf`, …

### src/world/geometry.ts

- `src/world/geometry.ts#orientedPolyline` (function, line 33) — call by 3: `src/sim/peds/sidewalk.ts#SidewalkGraph.footwaysOf`, `src/sim/peds/sidewalk.ts#SidewalkGraph.kerbsAt`, `src/world/walkways.ts#Walkways.buildNode`
- `src/world/geometry.ts#PolylineCache` (class, line 66) — typeRef by 1: `src/world/junction/bulb.ts#bulbLine`

### src/world/ids.ts

- `src/world/ids.ts#NodeId` (type, line 7) — typeRef by 35: `src/sim/crossings/permission.ts#crossingReservedByVehicle`, `src/sim/crossings/permission.ts#indexReservations`, `src/sim/crossings/permission.ts#RESERVATIONS`, `src/sim/intersections/crossingSpans.ts#CrossingSpans.measureJunction`, …
- `src/world/ids.ts#SegmentId` (type, line 8) — typeRef by 27: `src/sim/agents/walk.ts#publish`, `src/sim/peds/sidewalk.ts#kerbId`, `src/sim/peds/sidewalk.ts#SidewalkEdge`, `src/sim/peds/sidewalk.ts#SidewalkGraph.footwaysOf`, …

### src/world/junction/bulb.ts

- `src/world/junction/bulb.ts#outline` (function, line 27) — call by 1: `src/world/junction/bulb.ts#bulbLine`
- `src/world/junction/bulb.ts#Outline` (interface, line 24) — read by 1: `src/world/junction/bulb.ts#bulbLine`

### src/world/junction/legs.ts

- `src/world/junction/legs.ts#buildLegs` (function, line 101) — call by 1: `src/world/junction/bulb.ts#bulbLine`

### src/world/junction/levels.ts

- `src/world/junction/levels.ts#JunctionLevels.get` (method, line 256) — call by 2: `src/world/walkways.ts#cornersOf`, `src/world/network.ts#Network.junctionAt`

### src/world/junction/types.ts

- `src/world/junction/types.ts#Corner` (interface, line 43) — read by 1: `src/world/walkways.ts#cornersOf`
- `src/world/junction/types.ts#Junction` (interface, line 60) — read by 2: `src/sim/peds/sidewalk.ts#clearOfJunction`, `src/world/walkways.ts#cornersOf`
- `src/world/junction/types.ts#Junction` (interface, line 60) — typeRef by 2: `src/world/walkable.ts#WalkableSurface.cellOf`, `src/world/network.ts#Network.junctionAt`
- `src/world/junction/types.ts#Leg` (interface, line 14) — read by 2: `src/world/walkways.ts#cornersOf`, `src/world/junction/bulb.ts#bulbLine`

### src/world/lanelets.ts

- `src/world/lanelets.ts#Connector` (interface, line 62) — read by 5: `src/sim/crossings/permission.ts#crossingReservedByVehicle`, `src/sim/crossings/permission.ts#indexReservations`, `src/sim/crossings/permission.ts#turnOwedToVehicle`, `src/sim/intersections/crossingSpans.ts#CrossingSpans.measureJunction`, …
- `src/world/lanelets.ts#Connector` (interface, line 62) — typeRef by 1: `src/sim/intersections/crossingSpans.ts#reservationCoversCrossing`
- `src/world/lanelets.ts#ConnectorId` (type, line 28) — typeRef by 6: `src/sim/intersections/crossingSpans.ts#CrossingSpans.measureJunction`, `src/sim/intersections/crossingSpans.ts#CrossingSpans.ofJunction`, `src/sim/intersections/crossingSpans.ts#CrossingSpans.span`, `src/sim/intersections/crossingSpans.ts#CrossingSpans.spans`, …
- `src/world/lanelets.ts#GraphChange` (interface, line 131) — read by 1: `src/sim/peds/sidewalk.ts#SidewalkGraph.update`
- `src/world/lanelets.ts#GraphChange` (interface, line 131) — typeRef by 1: `src/sim/peds/sidewalk.ts#SidewalkGraph.update`
- `src/world/lanelets.ts#JunctionTopology` (interface, line 78) — read by 4: `src/sim/crossings/permission.ts#mayEnterCrossing`, `src/sim/crossings/permission.ts#turnOwedToVehicle`, `src/sim/intersections/crossingSpans.ts#CrossingSpans.measureJunction`, `src/sim/peds/signalPostIndex.ts#SignalPostIndex.placeJunction`
- `src/world/lanelets.ts#Lanelet` (interface, line 31) — read by 12: `src/sim/agents/walk.ts#createAgentWalkEngine`, `src/sim/crossings/permission.ts#indexReservations`, `src/sim/crossings/permission.ts#pedGapAccepted`, `src/sim/crossings/permission.ts#turnOwedToVehicle`, …
- `src/world/lanelets.ts#LaneletGraph` (class, line 137) — typeRef by 4: `src/sim/peds/sidewalk.ts#SidewalkGraph.build`, `src/sim/peds/sidewalk.ts#SidewalkGraph.crossingsAt`, `src/sim/peds/sidewalk.ts#SidewalkGraph.update`, `src/sim/peds/signalPostIndex.ts#SignalPostWorld`
- `src/world/lanelets.ts#LaneletGraph.connectors` (field, line 139) — read by 1: `src/sim/intersections/crossingSpans.ts#CrossingSpans.measureJunction`
- `src/world/lanelets.ts#LaneletGraph.exitsOf` (method, line 742) — call by 1: `src/sim/vehicles/spawn.ts#spawnVehicleAt`
- `src/world/lanelets.ts#LaneletGraph.junctions` (field, line 140) — read by 5: `src/sim/crossings/permission.ts#mayEnterCrossing`, `src/sim/crossings/permission.ts#turnOwedToVehicle`, `src/sim/intersections/crossingSpans.ts#CrossingSpans.build`, `src/sim/intersections/crossingSpans.ts#CrossingSpans.measureJunction`, …
- `src/world/lanelets.ts#LaneletGraph.lanelets` (field, line 138) — read by 2: `src/sim/ambient/ambient.ts#AmbientWorld.listLanes`, `src/sim/meso/meso.ts#Meso.handoffDestination`
- `src/world/lanelets.ts#LaneletGraph.laneUsable` (method, line 760) — call by 1: `src/sim/vehicles/spawn.ts#spawnVehicleAt`
- `src/world/lanelets.ts#LaneletGraph.linksOfSegment` (method, line 738) — call by 1: `src/sim/peds/sidewalk.ts#SidewalkGraph.crossingsAt`
- `src/world/lanelets.ts#LaneletId` (type, line 27) — typeRef by 8: `src/sim/peds/sidewalk.ts#SidewalkEdge`, `src/sim/people/engine.ts#PeopleBridge`, `src/sim/ambient/ambient.ts#SceneryCar`, `src/sim/meso/meso.ts#Meso.handoffDestination`, …

### src/world/network.ts

- `src/world/network.ts#Network` (class, line 46) — typeRef by 21: `src/sim/peds/sidewalk.ts#clearOfJunction`, `src/sim/peds/sidewalk.ts#SidewalkGraph.build`, `src/sim/peds/sidewalk.ts#SidewalkGraph.crossingsAt`, `src/sim/peds/sidewalk.ts#SidewalkGraph.footwaysOf`, …
- `src/world/network.ts#Network.changesSince` (method, line 143) — call by 3: `src/world/walkable.ts#WalkableSurface.forgetChanged`, `src/world/walkways.ts#Walkways.plan`, `src/sim/ambient/ambient.ts#AmbientWorld.roadEnds`
- `src/world/network.ts#Network.constructor` (constructor, line 100) — read by 6: `src/world/walkways.ts#cornersOf`, `src/world/walkways.ts#footwayShapes`, `src/world/walkways.ts#ringsIn`, `src/world/walkways.ts#Walkways.apply`, …
- `src/world/network.ts#Network.crosswalkDistanceAt` (method, line 324) — call by 4: `src/sim/peds/sidewalk.ts#SidewalkGraph.crossingsAt`, `src/sim/peds/sidewalk.ts#SidewalkGraph.footwaysOf`, `src/sim/peds/sidewalk.ts#SidewalkGraph.kerbsAt`, `src/world/walkways.ts#Walkways.buildNode`
- `src/world/network.ts#Network.flushCells` (field, line 60) — read by 2: `src/world/roads/footwayRise.ts#footwayRiseShare`, `src/world/roads/footwayRise.ts#packFlush`
- `src/world/network.ts#Network.junctionCells` (field, line 58) — read by 2: `src/world/walkable.ts#WalkableSurface.cellOf`, `src/world/walkways.ts#ringsIn`
- `src/world/network.ts#Network.junctions` (field, line 54) — read by 4: `src/world/walkable.ts#WalkableSurface.cellOf`, `src/world/walkways.ts#cornersOf`, `src/world/walkways.ts#ringsIn`, `src/world/network.ts#Network.junctionAt`
- `src/world/network.ts#Network.mouthDistance` (method, line 288) — call by 2: `src/sim/peds/sidewalk.ts#SidewalkGraph.footwaysOf`, `src/sim/peds/sidewalk.ts#SidewalkGraph.kerbsAt`
- `src/world/network.ts#Network.polylines` (field, line 80) — read by 1: `src/world/walkways.ts#Walkways.buildNode`
- `src/world/network.ts#Network.ribbons` (field, line 55) — read by 4: `src/world/roads/footwayRise.ts#footwayRiseShare`, `src/world/roads/footwayRise.ts#packFlush`, `src/world/walkways.ts#footwayShapes`, `src/world/walkways.ts#Walkways.buildNode`
- `src/world/network.ts#Network.ribbonsMeeting` (method, line 62) — call by 3: `src/world/walkable.ts#WalkableSurface.cellOf`, `src/world/walkways.ts#ringsIn`, `src/sim/ambient/ambientIndex.ts#StreetLength.around`
- `src/world/network.ts#Network.segmentCells` (field, line 57) — read by 2: `src/world/walkable.ts#WalkableSurface.cellOf`, `src/sim/ambient/ambient.ts#AmbientWorld.roadEnds`
- `src/world/network.ts#Network.state` (getter, line 148) — call by 5: `src/world/walkable.ts#WalkableSurface.forgetChanged`, `src/world/walkways.ts#Walkways.apply`, `src/world/walkways.ts#Walkways.update`, `src/world/walkways.ts#Walkways.updateSteps`, …

### src/world/networkDelta.ts

- `src/world/networkDelta.ts#NetworkDelta` (interface, line 5) — read by 3: `src/world/walkable.ts#WalkableSurface.forgetChanged`, `src/world/walkways.ts#Walkways.plan`, `src/sim/ambient/ambient.ts#AmbientWorld.roadEnds`

### src/world/networkLength.ts

- `src/world/networkLength.ts#NetworkLength` (class, line 8) — construct by 1: `src/sim/ambient/ambientIndex.ts#StreetLength.whole`
- `src/world/networkLength.ts#NetworkLength.of` (method, line 14) — call by 1: `src/sim/ambient/ambientIndex.ts#StreetLength.wholeMap`

### src/world/rebuildWork.ts

- `src/world/rebuildWork.ts#rebuildWork` (variable, line 5) — read by 5: `src/sim/intersections/crossingSpans.ts#CrossingSpans.measureJunction`, `src/sim/peds/sidewalk.ts#SidewalkGraph.footwaysOf`, `src/sim/peds/sidewalk.ts#SidewalkGraph.kerbsAt`, `src/world/walkways.ts#Walkways.buildNode`, …
- `src/world/rebuildWork.ts#rebuildWork` (variable, line 5) — write by 5: `src/sim/intersections/crossingSpans.ts#CrossingSpans.measureJunction`, `src/sim/peds/sidewalk.ts#SidewalkGraph.footwaysOf`, `src/sim/peds/sidewalk.ts#SidewalkGraph.kerbsAt`, `src/world/walkways.ts#Walkways.buildNode`, …

### src/world/ribbons.ts

- `src/world/ribbons.ts#SegmentRibbon` (interface, line 25) — read by 6: `src/world/roads/footwayRise.ts#footwayRiseShare`, `src/world/roads/footwayRise.ts#packFlush`, `src/world/walkways.ts#footwayShapes`, `src/world/walkways.ts#Walkways.buildNode`, …
- `src/world/ribbons.ts#SegmentRibbon` (interface, line 25) — typeRef by 2: `src/sim/ambient/ambientIndex.ts#StreetLength.add`, `src/sim/ambient/ambientIndex.ts#StreetLength.pieces`

### src/world/roadTypes.ts

- `src/world/roadTypes.ts#CASING_BAND` (variable, line 95) — read by 1: `src/world/roads/footwayRise.ts#flushHolds`
- `src/world/roadTypes.ts#flushOn` (function, line 360) — call by 1: `src/world/roads/footwayRise.ts#flushHolds`
- `src/world/roadTypes.ts#FOOTWAY_RISE` (variable, line 93) — read by 1: `src/world/roads/footwayRise.ts#footwayRiseAt`
- `src/world/roadTypes.ts#footwayOn` (function, line 356) — call by 3: `src/sim/peds/sidewalk.ts#SidewalkGraph.footwaysOf`, `src/sim/peds/sidewalk.ts#SidewalkGraph.kerbsAt`, `src/world/roads/footwayRise.ts#flushHolds`
- `src/world/roadTypes.ts#Level` (enum, line 10) — reference by 8: `src/sim/peds/sidewalk.ts#clearOfJunction`, `src/world/footwayPath.ts#footwayPath`, `src/world/walkable.ts#WalkableSurface.cellOf`, `src/world/walkingLines.ts#LocalLines.at`, …
- `src/world/roadTypes.ts#roadProfile` (function, line 248) — call by 3: `src/sim/peds/sidewalk.ts#SidewalkGraph.crossingsAt`, `src/sim/peds/sidewalk.ts#SidewalkGraph.footwaysOf`, `src/sim/peds/sidewalk.ts#SidewalkGraph.kerbsAt`
- `src/world/roadTypes.ts#RoadType` (interface, line 29) — read by 5: `src/sim/peds/sidewalk.ts#SidewalkGraph.footwaysOf`, `src/sim/peds/sidewalk.ts#SidewalkGraph.kerbsAt`, `src/world/pedestrianAccess.ts#carriesPedestrians`, `src/world/roads/footwayRise.ts#flushHolds`, …
- `src/world/roadTypes.ts#RoadType` (interface, line 29) — typeRef by 4: `src/world/footwayPath.ts#FootwayRibbon`, `src/world/pedestrianAccess.ts#carriesPedestrians`, `src/world/roads/footwayRise.ts#flushHolds`, `src/world/roads/footwayRise.ts#FlushRoad`
- `src/world/roadTypes.ts#SegmentDirection` (type, line 7) — typeRef by 2: `src/world/footwayPath.ts#FootwayNet`, `src/world/footwayPath.ts#footwayPath`
- `src/world/roadTypes.ts#SurfaceLevel` (type, line 25) — typeRef by 6: `src/world/walkable.ts#WalkableSurface.cellOf`, `src/world/walkable.ts#WalkableSurface.cellOf.ringsOf`, `src/world/walkingLines.ts#CornerAsk`, `src/world/walkingLines.ts#LocalLines.polygons`, …

### src/world/section.ts

- `src/world/section.ts#Band` (interface, line 8) — read by 2: `src/world/walkways.ts#footwayShapes`, `src/world/section.ts#bandMid`
- `src/world/section.ts#Band` (interface, line 8) — typeRef by 1: `src/world/section.ts#bandMid`
- `src/world/section.ts#CrossSection` (interface, line 33) — read by 3: `src/world/footwayPath.ts#footwayPath`, `src/world/walkways.ts#footwayShapes`, `src/world/walkways.ts#Walkways.buildNode`
- `src/world/section.ts#sectionOf` (function, line 67) — call by 3: `src/world/footwayPath.ts#footwayPath`, `src/world/walkways.ts#footwayShapes`, `src/world/walkways.ts#Walkways.buildNode`
- `src/world/section.ts#SideZones` (interface, line 14) — read by 3: `src/world/footwayPath.ts#footwayPath`, `src/world/walkways.ts#footwayShapes`, `src/world/walkways.ts#Walkways.buildNode`

### src/world/signalPosts.ts

- `src/world/signalPosts.ts#SIGNAL_POST_RADIUS` (variable, line 13) — read by 1: `src/sim/agents/walk.ts#stepWalkers`
- `src/world/signalPosts.ts#SignalPost` (interface, line 15) — read by 3: `src/sim/agents/walk.ts#stepWalkers`, `src/sim/peds/signalPostIndex.ts#SignalPostIndex.file`, `src/sim/peds/signalPostIndex.ts#SignalPostIndex.lift`
- `src/world/signalPosts.ts#SignalPost` (interface, line 15) — typeRef by 5: `src/sim/peds/signalPostIndex.ts#SignalPostIndex.byNode`, `src/sim/peds/signalPostIndex.ts#SignalPostIndex.cells`, `src/sim/peds/signalPostIndex.ts#SignalPostIndex.file`, `src/sim/peds/signalPostIndex.ts#SignalPostIndex.found`, …
- `src/world/signalPosts.ts#signalPostPlace` (function, line 25) — call by 1: `src/sim/peds/signalPostIndex.ts#SignalPostIndex.placeJunction`
- `src/world/signalPosts.ts#signalPosts` (function, line 53) — call by 1: `src/sim/peds/signalPostIndex.ts#SignalPostIndex.placeAll`

### src/world/structures.ts

- `src/world/structures.ts#RoadStructure` (type, line 5) — typeRef by 12: `src/world/walkingLines.ts#CornerAsk`, `src/world/walkingLines.ts#CornerEnd`, `src/world/walkingLines.ts#LocalLines.at`, `src/world/walkingLines.ts#LocalLines.nearestOuter`, …

### src/world/surfaces.ts

- `src/world/surfaces.ts#levelRingsOf` (function, line 81) — call by 2: `src/world/walkable.ts#WalkableSurface.cellOf.ringsOf`, `src/world/walkways.ts#ringsIn`

### src/world/transit.ts

- `src/world/transit.ts#TransitLine` (interface, line 35) — read by 1: `src/sim/transit/transit.ts#TransitSim.trainZones`

### src/world/waiting.ts

- `src/world/waiting.ts#answered` (function, line 5) — call by 1: `src/world/walkways.ts#Walkways.updateSteps`
- `src/world/waiting.ts#WAITING` (variable, line 2) — typeRef by 1: `src/world/walkways.ts#Walkways.updateSteps`

## External APIs

- `es:Math` × 1105
- `es:Float32Array` × 351
- `es:Array.push` × 348
- `es:Array.length` × 335
- `es:Map.get` × 293
- `es:Math.max` × 278
- `es:Math.min` × 238
- `three:Vector3` × 198
- `es:Map` × 178
- `es:Promise` × 173
- `es:Math.floor` × 162
- `es:Map.set` × 155
- `es:Array.map` × 125
- `es:Infinity` × 120
- `es:Record` × 109
- `es:Set` × 101
- `es:ReadonlyArray.length` × 100
- `es:Map.delete` × 84
- `es:Float64Array` × 76
- `es:Set.add` × 76
- `three:BufferAttribute` × 73
- `three:Matrix4` × 71
- `es:Math.abs` × 70
- `es:Math.sin` × 67
- `es:Uint32Array` × 66
- `es:Math.hypot` × 63
- `three:Object3D` × 63
- `es:ReadonlyArray.map` × 62
- `three:Mesh.geometry` × 61
- `three:BufferGeometry.setAttribute` × 58
- `three:Texture` × 57
- `es:Math.cos` × 56
- `es:Int32Array` × 55
- `es:Uint8Array` × 55
- `three:BufferGeometry.userData` × 52
- `three:BufferGeometry` × 51
- `es:Object` × 50
- `three:BufferGeometry.getAttribute` × 49
- `three:Vector3.y` × 49
- `es:Map.clear` × 48
- `es:Set.has` × 48
- `es:ArrayBuffer` × 47
- `three:Vector3.x` × 46
- `es:Number` × 44
- `es:String.replace` × 44
- `three:Color` × 44
- `three:Quaternion` × 44
- `es:Readonly` × 43
- `es:Math.PI` × 42
- `three:BufferAttribute.count` × 41
- `three:Vector3.normalize` × 41
- `es:Float32Array.length` × 40
- `three:Vector3.z` × 39
- `three:SkinnedMesh` × 37
- `es:Math.round` × 36
- `es:RegExp.test` × 36
- `three:BufferAttribute.getX` × 36
- `three:Object3D.getWorldPosition` × 36
- `es:ArrayLike` × 35
- `es:Error` × 35
- `es:Uint16Array` × 35
- `es:Array.filter` × 34
- `es:Array.sort` × 34
- `three:Vector3.sub` × 34
- `es:Array` × 33
- `es:Array.indexOf` × 33
- `three:Object3D.name` × 33
- `three:SkinnedMesh.skeleton` × 33
- `three:BufferAttribute.array` × 31
- `three:DataTexture` × 31
- `es:Set.delete` × 30
- `three:Object3D.parent` × 30
- `es:Map.values` × 29
- `es:Map.size` × 28
- `three:Vector3.copy` × 28
- `es:WeakMap.get` × 27
- `three:Object3D.userData` × 27
- `dom:console` × 26
- `es:Float32Array.set` × 26
- `three:Vector3.clone` × 26
- `three:WebGLProgramParametersWithUniforms.uniforms` × 26
- `es:Math.atan2` × 25
- `es:Promise.then` × 25
- `es:NumberConstructor.isFinite` × 24
- `es:ObjectConstructor.entries` × 24
- `es:WeakMap.set` × 24
- `three:Object3D.getObjectByName` × 24
- `three:Texture.needsUpdate` × 24
- `three:WebGLProgramParameters.fragmentShader` × 24
- `es:Array.forEach` × 23
- `es:Int16Array` × 23
- `es:ReadonlyArray.filter` × 23
- `es:Set.size` × 23
- `three:Group` × 23
- `three:Object3D.updateMatrixWorld` × 23
- `es:Array.slice` × 22
- `es:Math.ceil` × 22
- `es:ReadonlyArray.indexOf` × 22
- `es:WeakMap` × 22
- `three:Color.b` × 22
- `three:Color.g` × 22
- `three:Color.r` × 22
- `three:uniforms.__index` × 22
- `three:Vector3.addScaledVector` × 22
- `dom:performance` × 21
- `dom:Performance.now` × 21
- `es:Uint32Array.length` × 21
- `three:InstancedMesh.count` × 21
- `three:InstancedMesh.instanceMatrix` × 21
- `three:Mesh.material` × 21
- `es:PromiseConstructor.all` × 20
- `es:Set.clear` × 20
- `three:BufferGeometry.index` × 20
- `three:InstancedMesh` × 20
- `dom:Console.error` × 19
- `es:Array.splice` × 19
- `vite:src` × 19
- `es:Float64Array.length` × 18
- `three:BufferAttribute.getY` × 18
- `three:NearestFilter` × 18
- `three:Object3D.quaternion` × 18
- `three:Object3D.traverse` × 18
- `three:Skeleton.bones` × 18
- `es:Float32Array.buffer` × 17
- `es:Float32Array.fill` × 17
- `es:Map.has` × 17
- `es:String.startsWith` × 17
- `three:RGBAFormat` × 17
- `dom:fetch` × 16
- `es:Array.some` × 16
- `es:Map.keys` × 16
- `es:Omit` × 16
- `three:Object3D.add` × 16
- `three:Texture.image` × 16
- `three:WebGLProgramParameters.vertexShader` × 16
- `es:ArrayConstructor.isArray` × 15
- `three:BufferAttribute.getZ` × 15
- `three:Matrix4.fromArray` × 15
- `three:Plane` × 15
- `three:Texture.dispose` × 15
- `es:Array.find` × 14
- `es:Array.pop` × 14
- `es:PromiseConstructor.resolve` × 14
- `es:ReadonlyArray.includes` × 14
- `es:ReadonlyMap` × 14
- `three:FloatType` × 14
- `three:Matrix4.elements` × 14
- `three:Quaternion.copy` × 14
- `three:Vector3.add` × 14
- `three:Vector3.lengthSq` × 14
- `three:Vector3.set` × 14
- `es:Int32Array.length` × 13
- `es:ReadonlySet` × 13
- `three:Object3D.visible` × 13
- `three:Vector3.applyMatrix4` × 13
- `three:Vector3.distanceTo` × 13
- `three:Vector3.multiplyScalar` × 13
- `dom:MessageEvent.data` × 12
- `dom:Worker` × 12
- `es:Number.toFixed` × 12
- `es:ObjectConstructor.keys` × 12
- `es:Promise.catch` × 12
- `es:Uint32ArrayConstructor.from` × 12
- `es:Uint8ClampedArray` × 12
- `three:BufferGeometry.boundingBox` × 12
- `three:InstancedBufferAttribute` × 12
- `three:Object3D.position` × 12
- `three:Object3D.scale` × 12
- `three:Quaternion.multiply` × 12
- `three:RenderTarget.dispose` × 12
- `dom:Response.ok` × 11
- `es:Array.findIndex` × 11
- `es:Math.imul` × 11
- `es:Math.sqrt` × 11
- `es:Partial` × 11
- `es:ReadonlyArray.forEach` × 11
- `three:BufferAttribute.setUsage` × 11
- `three:BufferGeometry.setIndex` × 11
- `three:DynamicDrawUsage` × 11
- `three:InstancedMesh.instanceColor` × 11
- `three:Object3D.castShadow` × 11
- `es:Array.fill` × 10
- `es:Array.includes` × 10
- `es:ArrayConstructor.from` × 10
- `es:Pick` × 10
- `es:ReadonlyMap.get` × 10
- `es:String` × 10
- `three:Bone` × 10
- `three:BufferGeometry.boundingSphere` × 10
- `three:BufferGeometry.dispose` × 10
- `three:BufferGeometry.getIndex` × 10
- `three:BufferGeometry.morphAttributes` × 10
- `three:GLTF.scene` × 10
- `three:MeshStandardMaterial` × 10
- `three:Object3D.getWorldQuaternion` × 10
- `three:Object3D.matrixWorld` × 10
- `three:Skeleton.boneInverses` × 10
- `three:Vector3.setY` × 10
- `three:WebGLRenderTarget` × 10
- `dom:OffscreenCanvas` × 9
- `dom:setTimeout` × 9
- `es:Float32ArrayConstructor.from` × 9
- `es:SharedArrayBuffer` × 9
- `es:Uint8Array.byteLength` × 9
- `three:BufferAttribute.getComponent` × 9
- `three:Material.dispose` × 9
- `three:Mesh.morphTargetInfluences` × 9
- `three:SRGBColorSpace` × 9
- `three:Texture.colorSpace` × 9
- `three:Texture.flipY` × 9
- `three:Vector3.fromArray` × 9
- `dom:createImageBitmap` × 8
- `dom:ImageData.data` × 8
- `dom:location` × 8
- `es:Array.every` × 8
- `es:Array.reduce` × 8
- `es:CallableFunction.call` × 8
- `es:Float32Array.byteLength` × 8
- `es:Float32Array.slice` × 8
- `es:NumberConstructor.isNaN` × 8
- `es:ReadonlyArray.some` × 8
- `es:Uint8Array.buffer` × 8
- `es:Uint8Array.length` × 8
- `meshoptimizer:MeshoptSimplifier` × 8
- `three:BoxGeometry` × 8
- `three:BufferAttribute.needsUpdate` × 8
- `three:DataTexture.magFilter` × 8
- `three:DataTexture.minFilter` × 8
- `three:Matrix4.clone` × 8
- `three:Matrix4.invert` × 8
- `three:Matrix4.multiply` × 8
- `three:Object3D.frustumCulled` × 8
- `three:RenderTarget.texture` × 8
- `three:Vector3.setFromMatrixPosition` × 8
- `three:Vector3.toArray` × 8
- `dom:Console.warn` × 7
- `dom:MessageEvent` × 7
- `dom:Response.status` × 7
- `es:ReadonlySet.has` × 7
- `es:String.slice` × 7
- `es:Uint32Array.buffer` × 7
- `es:Uint8Array.set` × 7
- `three:BufferGeometry.hasAttribute` × 7
- `three:DataTextureImageData.data` × 7
- `three:Material` × 7
- `three:Material.onBeforeCompile` × 7
- `three:MaterialProperties.alphaTest` × 7
- `three:Matrix4.makeTranslation` × 7
- `three:Matrix4.premultiply` × 7
- `three:Object3D.customDepthMaterial` × 7
- `three:Quaternion.invert` × 7
- `dom:Body.arrayBuffer` × 6
- `dom:Body.json` × 6
- `dom:window` × 6
- `es:Array.join` × 6
- `es:Float32Array.subarray` × 6
- `es:JSON` × 6
- `es:Math.exp` × 6
- `es:NonNullable` × 6
- `es:NumberConstructor.EPSILON` × 6
- `es:ObjectConstructor.fromEntries` × 6
- `es:ReadonlyArray.find` × 6
- `three:Box3.max` × 6
- `three:Box3.min` × 6
- `three:BufferAttribute.addUpdateRange` × 6
- `three:BufferAttribute.clearUpdateRanges` × 6
- `three:BufferGeometry.attributes` × 6
- `three:BufferGeometry.computeBoundingBox` × 6
- `three:BufferGeometry.computeBoundingSphere` × 6
- `three:BufferGeometry.computeVertexNormals` × 6
- `three:DataArrayTexture` × 6
- `three:DoubleSide` × 6
- `three:Material.customProgramCacheKey` × 6
- `three:Matrix4.multiplyMatrices` × 6
- `three:Object3D.matrix` × 6
- `three:Quaternion.setFromUnitVectors` × 6
- `three:RenderTarget.viewport` × 6
- `three:SkinnedMesh.bindMatrix` × 6
- `three:Texture.addUpdateRange` × 6
- `three:Texture.clearUpdateRanges` × 6
- `three:Texture.userData` × 6
- `three:WebGLRenderer` × 6
- `three:WebGLRenderer.autoClear` × 6
- `three:WebGLRenderer.setRenderTarget` × 6
- `vite:ImportMeta.env` × 6
- `dom:CanvasDrawImage.drawImage` × 5
- `dom:CanvasImageData.getImageData` × 5
- `dom:document` × 5
- `dom:HTMLCanvasElement.width` × 5
- `dom:OffscreenCanvas.getContext` × 5
- `es:Array.flatMap` × 5
- `es:Float32Array.copyWithin` × 5
- `es:Int8Array.buffer` × 5
- `es:Math.pow` × 5
- `es:Math.sign` × 5
- `es:ReadonlyArray.slice` × 5
- `three:BufferAttribute.itemSize` × 5
- `three:BufferAttribute.setXYZW` × 5
- `three:BufferGeometry.position` × 5
- `three:Float32BufferAttribute` × 5
- `three:Material.defines` × 5
- `three:Matrix4.compose` × 5
- `three:Matrix4.copy` × 5
- `three:MeshDepthMaterial` × 5
- `three:Object3D.receiveShadow` × 5
- `three:Quaternion.setFromAxisAngle` × 5
- `three:ShaderMaterial` × 5
- `three:UnsignedByteType` × 5
- `three:Vector3.dot` × 5
- `three:Vector3.lerp` × 5
- `dom:Body.blob` × 4
- `dom:CanvasRenderingContext2D.canvas` × 4
- `dom:HTMLCanvasElement.height` × 4
- `dom:ImageBitmap.height` × 4
- `dom:ImageBitmap.width` × 4
- `dom:Location.search` × 4
- `dom:Transferable` × 4
- `dom:URLSearchParams` × 4
- `dom:Worker.terminate` × 4
- `es:Array.entries` × 4
- `es:ArrayLike.length` × 4
- `es:DataView` × 4
- `es:Float32Array.map` × 4
- `es:Float64Array.buffer` × 4
- `es:Int16Array.length` × 4
- `es:Int32Array.fill` × 4
- `es:Int32ArrayConstructor.from` × 4
- `es:JSON.parse` × 4
- `es:ObjectConstructor.assign` × 4
- `es:ReturnType` × 4
- `es:String.includes` × 4
- `es:String.length` × 4
- `es:String.toLowerCase` × 4
- `three:BufferGeometry.groups` × 4
- `three:BufferGeometry.morphTargetsRelative` × 4
- `three:Color.clone` × 4
- `three:CylinderGeometry` × 4
- `three:GLSL3` × 4
- `three:InstancedMesh.morphTexture` × 4
- `three:Loader.loadAsync` × 4
- `three:MaterialProperties.alphaToCoverage` × 4
- `three:NoBlending` × 4
- `three:Object3D.clear` × 4
- `three:Object3D.remove` × 4
- `three:OrthographicCamera` × 4
- `three:Quaternion.clone` × 4
- `three:RGBADepthPacking` × 4
- `three:Scene` × 4
- `three:Skeleton.update` × 4
- `three:SkinnedMesh.bindMode` × 4
- `three:Vector3.cross` × 4
- `three:Vector3.crossVectors` × 4
- `three:Vector3.divideScalar` × 4
- `three:Vector3.length` × 4
- `vite:ImportMetaEnv.BASE_URL` × 4
- `dom:AbstractWorker.onerror` × 3
- `dom:Document.createElement` × 3
- `dom:ErrorEvent.message` × 3
- `dom:HTMLCanvasElement.getContext` × 3
- `dom:ImageBitmap.close` × 3
- `dom:MessageEventTarget.onmessage` × 3
- `dom:MessageEventTarget.onmessageerror` × 3
- `dom:Response` × 3
- `dom:Response.statusText` × 3
- `dom:Worker.postMessage` × 3
- `es:Array.reverse` × 3
- `es:ArrayBuffer.byteLength` × 3
- `es:Float64Array.fill` × 3
- `es:Float64Array.set` × 3
- `es:Int8Array.set` × 3
- `es:Map.entries` × 3
- `es:Math.random` × 3
- `es:NumberConstructor.NaN` × 3
- `es:ObjectConstructor.values` × 3
- `es:PromiseRejectedResult.status` × 3
- `es:String.endsWith` × 3
- `es:Uint32Array.fill` × 3
- `es:Uint8ClampedArray.length` × 3
- `meshoptimizer:MeshoptSimplifier.ready` × 3
- `project:public/models/people/skins/index.json:average` × 3
- `project:public/models/people/skins/index.json:index` × 3
- `project:public/models/people/skins/index.json:name` × 3
- `project:public/models/people/skins/index.json:skins` × 3
- `three:BufferAttribute.name` × 3
- `three:BufferGeometry.addGroup` × 3
- `three:GLTF` × 3
- `three:InstancedMesh.dispose` × 3
- `three:InstancedMesh.setMatrixAt` × 3
- `three:LinearFilter` × 3
- `three:LinearMipmapLinearFilter` × 3
- `three:MaterialProperties.userData` × 3
- `three:Matrix4.decompose` × 3
- `three:Matrix4.makeRotationAxis` × 3
- `three:Matrix4.makeScale` × 3
- `three:Mesh.getVertexPosition` × 3
- `three:Mesh.morphTargetDictionary` × 3
- `three:Object3D.onAfterRender` × 3
- `three:Object3D.updateWorldMatrix` × 3
- `three:Quaternion.fromArray` × 3
- `three:Quaternion.identity` × 3
- `three:Quaternion.slerp` × 3
- `three:Quaternion.w` × 3
- `three:Quaternion.x` × 3
- `three:Quaternion.y` × 3
- `three:Quaternion.z` × 3
- `three:Skeleton` × 3
- `three:SkinnedMesh.bind` × 3
- `three:Sphere` × 3
- `three:Texture.onUpdate` × 3
- `three:TextureLoader` × 3
- `three:Vector3.applyQuaternion` × 3
- `three:Vector3.fromBufferAttribute` × 3
- `three:Vector4.set` × 3
- `three:WebGLRenderer.getRenderTarget` × 3
- `three:WebGLRenderer.render` × 3
- `dom:CanvasImageData.putImageData` × 2
- `dom:CanvasImageSource` × 2
- `dom:CanvasRenderingContext2D` × 2
- `dom:Event.preventDefault` × 2
- `dom:ImageData` × 2
- `dom:Location.href` × 2
- `dom:navigator` × 2
- `dom:OffscreenCanvas.height` × 2
- `dom:OffscreenCanvas.width` × 2
- `dom:OffscreenCanvasRenderingContext2D` × 2
- `dom:requestAnimationFrame` × 2
- `dom:TextDecoder` × 2
- `dom:TextDecoder.decode` × 2
- `dom:TextEncoder` × 2
- `dom:TextEncoder.encode` × 2
- `dom:URL` × 2
- `dom:URL.href` × 2
- `dom:URLSearchParams.get` × 2
- `dom:URLSearchParams.has` × 2
- `es:Array.shift` × 2
- `es:ArrayBuffer.slice` × 2
- `es:DataView.getUint32` × 2
- `es:DataView.setUint32` × 2
- `es:Float32Array.byteOffset` × 2
- `es:Float64Array.subarray` × 2
- `es:Float64ArrayConstructor.from` × 2
- `es:Int16Array.fill` × 2
- `es:Int32Array.buffer` × 2
- `es:Int32Array.slice` × 2
- `es:JSON.stringify` × 2
- `es:Math.acos` × 2
- `es:NumberConstructor.isInteger` × 2
- `es:ReadonlyArray.join` × 2
- `es:ReadonlyArray.reduce` × 2
- `es:ReadonlyMap.has` × 2
- `es:RegExp` × 2
- `es:String.split` × 2
- `es:Uint16Array.length` × 2
- `es:Uint32Array.set` × 2
- `es:Uint32Array.slice` × 2
- `es:WeakMap.delete` × 2
- `meshoptimizer:MeshoptSimplifier.supported` × 2
- `project:public/models/people/skins/index.json:makeup` × 2
- `project:public/models/people/skins/index.json:origin` × 2
- `project:public/models/people/skins/index.json:sex` × 2
- `three:Bone.isBone` × 2
- `three:Box3` × 2
- `three:BufferAttribute.setComponent` × 2
- `three:Color.multiplyScalar` × 2
- `three:Color.setRGB` × 2
- `three:DataArrayTextureImageData.depth` × 2
- `three:DataTexture.flipY` × 2
- `three:DataTexture.generateMipmaps` × 2
- `three:DataTextureImageData.width` × 2
- `three:EventDispatcher.removeEventListener` × 2
- `three:GeometryGroup.count` × 2
- `three:GeometryGroup.materialIndex` × 2
- `three:GeometryGroup.start` × 2
- `three:Group.isGroup` × 2
- `three:InstancedMesh.boundingSphere` × 2
- `three:InstancedMesh.setMorphAt` × 2
- `three:IUniform.value` × 2
- `three:Matrix4.makeRotationFromQuaternion` × 2
- `three:Matrix4.toArray` × 2
- `three:mergeGeometries` × 2
- `three:Mesh` × 2
- `three:Object3D.onAfterShadow` × 2
- `three:Object3D.onBeforeShadow` × 2
- `three:Object3D.rotation` × 2
- `three:Plane.setFromNormalAndCoplanarPoint` × 2
- `three:Quaternion.multiplyQuaternions` × 2
- `three:Quaternion.toArray` × 2
- `three:RedFormat` × 2
- `three:RenderTarget.width` × 2
- `three:ShaderChunk` × 2
- `three:ShaderMaterialProperties.uniforms` × 2
- `three:Sphere.radius` × 2
- `three:Vector2` × 2
- `three:Vector3.transformDirection` × 2
- `three:WebGLRenderer.initRenderTarget` × 2
- `vite:ImportMetaEnv.DEV` × 2
- `clipper2-ts:EndType` × 1
- `clipper2-ts:EndType.Polygon` × 1
- `clipper2-ts:FillRule` × 1
- `clipper2-ts:FillRule.NonZero` × 1
- `clipper2-ts:inflatePathsD` × 1
- `clipper2-ts:JoinType` × 1
- `clipper2-ts:JoinType.Round` × 1
- `clipper2-ts:PathsD` × 1
- `clipper2-ts:unionD` × 1
- `dom:atob` × 1
- `dom:CanvasRect.clearRect` × 1
- `dom:CanvasState.restore` × 1
- `dom:CanvasState.save` × 1
- `dom:CanvasTransform.setTransform` × 1
- `dom:clearInterval` × 1
- `dom:clearTimeout` × 1
- `dom:HTMLCanvasElement.addEventListener` × 1
- `dom:localStorage` × 1
- `dom:NavigatorConcurrentHardware.hardwareConcurrency` × 1
- `dom:Storage.getItem` × 1
- `dom:WindowOrWorkerGlobalScope.setInterval` × 1
- `es:Array.flat` × 1
- `es:Array.keys` × 1
- `es:ArrayBufferConstructor.isView` × 1
- `es:ArrayBufferLike` × 1
- `es:CallableFunction.apply` × 1
- `es:Error.stack` × 1
- `es:Exclude` × 1
- `es:Float64Array.slice` × 1
- `es:Generator` × 1
- `es:Int16Array.buffer` × 1
- `es:Int16Array.slice` × 1
- `es:Int16Array.subarray` × 1
- `es:Int16ArrayConstructor.from` × 1
- `es:Int32Array.map` × 1
- `es:Int32Array.set` × 1
- `es:Int32Array.subarray` × 1
- `es:Int32ArrayConstructor.BYTES_PER_ELEMENT` × 1
- `es:Int8Array.length` × 1
- `es:Iterable` × 1
- `es:Iterator.next` × 1
- `es:IteratorYieldResult.value` × 1
- `es:Math.fround` × 1
- `es:Math.SQRT1_2` × 1
- `es:Math.tan` × 1
- `es:NaN` × 1
- `es:Object.constructor` × 1
- `es:ObjectConstructor.freeze` × 1
- `es:Parameters` × 1
- `es:Promise.finally` × 1
- `es:PromiseConstructor.allSettled` × 1
- `es:PromiseConstructor.reject` × 1
- `es:PromiseFulfilledResult.value` × 1
- `es:PromiseRejectedResult.reason` × 1
- `es:ReadonlyArray.entries` × 1
- `es:ReadonlyArray.every` × 1
- `es:ReadonlyArray.flatMap` × 1
- `es:ReadonlyArray.keys` × 1
- `es:String.charCodeAt` × 1
- `es:String.indexOf` × 1
- `es:String.lastIndexOf` × 1
- `es:String.padStart` × 1
- `es:String.replaceAll` × 1
- `es:String.trim` × 1
- `es:StringConstructor.fromCharCode` × 1
- `es:Uint16Array.buffer` × 1
- `es:Uint32Array.every` × 1
- `es:Uint8Array.slice` × 1
- `es:Uint8Array.subarray` × 1
- `es:Uint8ArrayConstructor.from` × 1
- `es:Uint8ClampedArray.buffer` × 1
- `es:Uint8ClampedArray.set` × 1
- `es:WeakSet` × 1
- `es:WeakSet.add` × 1
- `es:WeakSet.has` × 1
- `meshoptimizer:MeshoptSimplifier.simplify` × 1
- `meshoptimizer:MeshoptSimplifier.simplifySloppy` × 1
- `meshoptimizer:MeshoptSimplifier.simplifyWithAttributes` × 1
- `project:public/models/people/proxies/community.json:community` × 1
- `project:public/models/people/proxies/community.json:items` × 1
- `project:public/models/people/skins/index.json:age` × 1
- `three:Box3.getCenter` × 1
- `three:Box3.getSize` × 1
- `three:Box3.setFromPoints` × 1
- `three:BufferAttribute.normalized` × 1
- `three:BufferAttribute.setXYZ` × 1
- `three:BufferGeometry.deleteAttribute` × 1
- `three:BufferGeometry.rotateX` × 1
- `three:BufferGeometry.toNonIndexed` × 1
- `three:BufferGeometry.translate` × 1
- `three:CanvasTexture` × 1
- `three:clone` × 1
- `three:Color.multiply` × 1
- `three:Color.setHex` × 1
- `three:ConeGeometry` × 1
- `three:DataArrayTexture.addLayerUpdate` × 1
- `three:DataArrayTexture.generateMipmaps` × 1
- `three:DataArrayTexture.magFilter` × 1
- `three:DataArrayTexture.minFilter` × 1
- `three:DataArrayTexture.unpackAlignment` × 1
- `three:DataTexture.unpackAlignment` × 1
- `three:DataTextureImageData.height` × 1
- `three:Euler.set` × 1
- `three:EventDispatcher.addEventListener` × 1
- `three:GLTF.parser` × 1
- `three:GLTFLoader` × 1
- `three:GLTFParser.getDependency` × 1
- `three:InterleavedBufferAttribute.getX` × 1
- `three:InterleavedBufferAttribute.getY` × 1
- `three:InterleavedBufferAttribute.getZ` × 1
- `three:InterleavedBufferAttribute.name` × 1
- `three:Material.clone` × 1
- `three:MaterialLoader` × 1
- `three:MaterialLoader.parse` × 1
- `three:Matrix3` × 1
- `three:Matrix3.elements` × 1
- `three:Matrix3.setFromMatrix4` × 1
- `three:Matrix4.extractRotation` × 1
- `three:Matrix4.getMaxScaleOnAxis` × 1
- `three:Mesh.updateMorphTargets` × 1
- `three:MeshBasicMaterial` × 1
- `three:MeshDepthMaterialProperties.map` × 1
- `three:MeshStandardMaterialProperties.color` × 1
- `three:MeshStandardMaterialProperties.map` × 1
- `three:MeshStandardMaterialProperties.metalness` × 1
- `three:MeshStandardMaterialProperties.roughness` × 1
- `three:NoColorSpace` × 1
- `three:Object3D.children` × 1
- `three:Object3D.localToWorld` × 1
- `three:Object3D.matrixAutoUpdate` × 1
- `three:Object3D.updateMatrix` × 1
- `three:Object3D.worldToLocal` × 1
- `three:OrthographicCamera.isOrthographicCamera` × 1
- `three:OrthographicCamera.left` × 1
- `three:OrthographicCamera.right` × 1
- `three:OrthographicCamera.zoom` × 1
- `three:Plane.distanceToPoint` × 1
- `three:Quaternion.normalize` × 1
- `three:Quaternion.set` × 1
- `three:RenderTarget.height` × 1
- `three:ShaderChunk.lights_physical_pars_fragment` × 1
- `three:ShaderLib` × 1
- `three:ShaderLibShader.fragmentShader` × 1
- `three:ShaderLibShader.vertexShader` × 1
- `three:Skeleton.boneMatrices` × 1
- `three:Skeleton.dispose` × 1
- `three:SkinnedMesh.bindMatrixInverse` × 1
- `three:Sphere.center` × 1
- `three:Texture.anisotropy` × 1
- `three:Vector2.set` × 1
- `three:Vector3.addScalar` × 1
- `three:Vector3.addVectors` × 1
- `three:Vector3.angleTo` × 1
- `three:Vector3.negate` × 1
- `three:Vector3.setScalar` × 1
- `three:WebGLRenderer.compile` × 1
- `three:WebGLRenderer.copyTextureToTexture` × 1
- `three:WebGLRenderer.domElement` × 1
