# Feature: src/editor/poles.ts, src/editor/poleRoutes.ts, src/editor/barriers.ts, src/editor/streetObjects.ts, src/world/streetFurniture.ts, src/world/signalPosts.ts, src/world/poleIndex.ts, src/world/poleLines.ts, src/world/utilities.ts, src/world/barriers.ts, src/render/poleLayer.ts, src/render/poleBake.ts, src/render/poleCells.ts, src/render/signs.ts, src/render/signLayer.ts, src/render/signals.ts, src/render/streetFurnitureLayer.ts, src/render/utilities.ts, src/render/barriers.ts, src/render/propCells.ts, src/render/propGeometry.ts, src/world/roads/derivedSigns.ts, src/world/roads/furnitureCatalog.ts, src/world/roads/furnitureSets.ts, src/ui/roads/furnitureChoice.ts, src/editor/streetscapeTool.ts, src/render/lightLevels.ts

The program starts from `src/main.ts`, `src/peopleLab/main.ts`. A declaration is the feature's own when the program reaches it only through the feature; in a declaration of the program that names the feature, its statements are a slice (what names it, what depends on that, what that controls).

- Own: 843 declarations
- Slices of shared code: 138
- Code: 9347 lines in 63 files (source/)
- Resources: 262 (91 entry, 171 style, 0 import; 75 only the feature uses)
- Uses: 364 shared declarations (interface/: 1005 declarations in 203 files, the types they name included); hooks: 192; external APIs: 366

## Code

- src/actionsWiring.ts — part, 35 lines
- src/editor/barriers.ts — whole, 175 lines
- src/editor/bulldozer.ts — part, 16 lines
- src/editor/chunkedSave.ts — part, 14 lines
- src/editor/cityGenerator.ts — part, 5 lines
- src/editor/poleRoutes.ts — whole, 199 lines
- src/editor/poles.ts — whole, 516 lines
- src/editor/roads/powerLine.ts — part, 18 lines
- src/editor/roads/streetKit.ts — part, 11 lines
- src/editor/roadTool.ts — part, 19 lines
- src/editor/streetObjects.ts — whole, 106 lines
- src/editor/streetscapeTool.ts — whole, 155 lines
- src/lab/catalog/layers.ts — part, 15 lines
- src/main.ts — part, 232 lines
- src/render/agents.ts — part, 7 lines
- src/render/barriers.ts — whole, 258 lines
- src/render/buildings/kit.ts — part, 15 lines
- src/render/cellInstances.ts — part, 66 lines
- src/render/flatAtlas.ts — part, 4 lines
- src/render/gardens.ts — part, 78 lines
- src/render/grass.ts — whole, 98 lines
- src/render/instanceRuns.ts — part, 4 lines
- src/render/lightLevels.ts — whole, 62 lines
- src/render/plantImpostors.ts — part, 20 lines
- src/render/playEffects.ts — part, 35 lines
- src/render/poleBake.ts — whole, 88 lines
- src/render/poleCells.ts — whole, 239 lines
- src/render/poleLayer.ts — whole, 751 lines
- src/render/propCells.ts — whole, 619 lines
- src/render/propGeometry.ts — whole, 453 lines
- src/render/renderer.ts — part, 148 lines
- src/render/scenery.ts — part, 68 lines
- src/render/signals.ts — whole, 731 lines
- src/render/signLayer.ts — whole, 441 lines
- src/render/signs.ts — whole, 466 lines
- src/render/streetFurnitureLayer.ts — whole, 471 lines
- src/render/utilities.ts — whole, 588 lines
- src/render/wind.ts — part, 52 lines
- src/sim/agents/walk.ts — part, 10 lines
- src/sim/peds/signalPostIndex.ts — part, 31 lines
- src/ui/roads/furnitureChoice.ts — whole, 23 lines
- src/ui/toolChoices.ts — part, 7 lines
- src/ui/v2/shell.ts — part, 29 lines
- src/world/barriers.ts — whole, 28 lines
- src/world/doc.ts — part, 244 lines
- src/world/junction/bulb.ts — part, 7 lines
- src/world/landscape.ts — part, 227 lines
- src/world/lanelets.ts — part, 9 lines
- src/world/network.ts — part, 18 lines
- src/world/poleIndex.ts — whole, 122 lines
- src/world/poleLines.ts — whole, 574 lines
- src/world/rasterGrid.ts — part, 7 lines
- src/world/roads/derivedSigns.ts — whole, 84 lines
- src/world/roads/furnitureCatalog.ts — whole, 29 lines
- src/world/roads/furnitureSets.ts — whole, 165 lines
- src/world/section.ts — part, 1 lines
- src/world/signalPosts.ts — whole, 77 lines
- src/world/solids.ts — part, 25 lines
- src/world/streetFurniture.ts — whole, 124 lines
- src/world/surfaces.ts — part, 3 lines
- src/world/utilities.ts — whole, 162 lines
- src/world/viewCells.ts — part, 59 lines
- src/world/zoneGrid.ts — part, 4 lines

## Slices of shared code

- `src/actionsWiring.ts#createActions.explodeAt` — 35 of 158 lines
- `src/editor/bulldozer.ts#Bulldozer.boxed` — 9 of 25 lines
- `src/editor/bulldozer.ts#Bulldozer.click` — 7 of 29 lines
- `src/editor/chunkedSave.ts#KEYED` — 14 of 35 lines
- `src/editor/cityGenerator.ts#greenCity` — 5 of 17 lines
- `src/editor/roadTool.ts#RoadTool.layPart` — 7 of 75 lines
- `src/editor/roadTool.ts#RoadToolHost` — 12 of 36 lines
- `src/editor/roads/powerLine.ts#PlanRun` — 1 of 1 lines
- `src/editor/roads/powerLine.ts#layPowerLinesSteps` — 10 of 44 lines
- `src/editor/roads/powerLine.ts#runPlanner` — 7 of 7 lines
- `src/editor/roads/streetKit.ts#carried` — 1 of 1 lines
- `src/editor/roads/streetKit.ts#carryStreetKit` — 5 of 18 lines
- `src/editor/roads/streetKit.ts#kitOf` — 5 of 19 lines
- `src/lab/catalog/layers.ts#signItems.one` — 8 of 9 lines
- `src/lab/catalog/layers.ts#signalItems` — 7 of 17 lines
- `src/main.ts#<IfStatement>@4036` — 18 of 196 lines
- `src/main.ts#barrierTool` — 14 of 14 lines
- `src/main.ts#bulldozer` — 3 of 10 lines
- `src/main.ts#cancelGestures` — 4 of 13 lines
- `src/main.ts#currentGesture` — 6 of 18 lines
- `src/main.ts#drawBarrierPlan` — 17 of 20 lines
- `src/main.ts#drawOverlayScreen` — 3 of 469 lines
- `src/main.ts#drawPolePlan` — 24 of 50 lines
- `src/main.ts#drawStreetscapeHover` — 33 of 40 lines
- `src/main.ts#endPointer` — 7 of 30 lines
- `src/main.ts#frame` — 6 of 102 lines
- `src/main.ts#framePolePlan` — 1 of 1 lines
- `src/main.ts#gameKeys` — 3 of 77 lines
- `src/main.ts#gameState` — 3 of 38 lines
- `src/main.ts#gestureInProgress` — 3 of 3 lines
- `src/main.ts#mountInterface` — 5 of 88 lines
- `src/main.ts#pointerDown` — 13 of 180 lines
- `src/main.ts#pointerMove` — 7 of 72 lines
- `src/main.ts#poleReach` — 3 of 3 lines
- `src/main.ts#poleTool` — 11 of 11 lines
- `src/main.ts#roadTool` — 5 of 29 lines
- `src/main.ts#showInspector` — 31 of 246 lines
- `src/main.ts#streetscapeTool` — 12 of 12 lines
- `src/render/agents.ts#LAMP_GAIN` — 7 of 7 lines
- `src/render/buildings/kit.ts#createBuildingKit` — 7 of 256 lines
- `src/render/buildings/kit.ts#createBuildingKit.setNight` — 8 of 21 lines
- `src/render/cellInstances.ts#createCellInstances` — 8 of 685 lines
- `src/render/cellInstances.ts#createCellInstances.cull` — 8 of 56 lines
- `src/render/cellInstances.ts#createCellInstances.makeMesh` — 5 of 29 lines
- `src/render/cellInstances.ts#screenHalf` — 1 of 1 lines
- `src/render/flatAtlas.ts#createFlatAtlas.readWholeContent` — 4 of 19 lines
- `src/render/gardens.ts#GardenCells` — 3 of 12 lines
- `src/render/gardens.ts#GardenPlant` — 3 of 17 lines
- `src/render/gardens.ts#KIND_KEYS` — 1 of 1 lines
- `src/render/gardens.ts#KIND_OF` — 1 of 1 lines
- `src/render/gardens.ts#createGardenCells` — 14 of 29 lines
- `src/render/gardens.ts#createGardenCells.cull` — 1 of 1 lines
- `src/render/gardens.ts#createGardenCells.dispose` — 3 of 4 lines
- `src/render/gardens.ts#createGardenCells.follow` — 1 of 1 lines
- `src/render/gardens.ts#createGardenCells.setMap` — 1 of 1 lines
- `src/render/gardens.ts#createGardenCells.source` — 3 of 6 lines
- `src/render/gardens.ts#gardenKinds` — 6 of 19 lines
- `src/render/gardens.ts#gardenPack` — 38 of 80 lines
- `src/render/gardens.ts#gardenPictures` — 1 of 1 lines
- `src/render/gardens.ts#gardenSpecies` — 2 of 6 lines
- `src/render/plantImpostors.ts#PlantModels` — 5 of 5 lines
- `src/render/plantImpostors.ts#SceneryPictures` — 4 of 7 lines
- `src/render/plantImpostors.ts#sceneryPictures` — 9 of 25 lines
- `src/render/plantImpostors.ts#sceneryPictures.bushAt` — 1 of 1 lines
- `src/render/plantImpostors.ts#sceneryPictures.treeAt` — 1 of 1 lines
- `src/render/playEffects.ts#createPlayEffects.explode` — 35 of 143 lines
- `src/render/renderer.ts#SceneHandle` — 3 of 170 lines
- `src/render/renderer.ts#createSceneRenderer` — 93 of 3774 lines
- `src/render/renderer.ts#createSceneRenderer.rebuildUtilities` — 3 of 7 lines
- `src/render/renderer.ts#createSceneRenderer.switchedOff` — 16 of 16 lines
- `src/render/renderer.ts#createSceneRenderer.syncUtilities` — 9 of 12 lines
- `src/render/renderer.ts#createSceneRenderer.warmRoadShaders` — 20 of 39 lines
- `src/render/renderer.ts#createSceneRenderer.worldSteps` — 4 of 105 lines
- `src/render/scenery.ts#SceneryKit` — 6 of 42 lines
- `src/render/scenery.ts#createSceneryKit` — 50 of 138 lines
- `src/render/scenery.ts#plantSamples` — 12 of 30 lines
- `src/sim/agents/walk.ts#stepWalkers` — 10 of 343 lines
- `src/sim/peds/signalPostIndex.ts#SignalPostIndex.byNode` — 1 of 1 lines
- `src/sim/peds/signalPostIndex.ts#SignalPostIndex.cells` — 1 of 1 lines
- `src/sim/peds/signalPostIndex.ts#SignalPostIndex.file` — 7 of 7 lines
- `src/sim/peds/signalPostIndex.ts#SignalPostIndex.found` — 1 of 1 lines
- `src/sim/peds/signalPostIndex.ts#SignalPostIndex.lift` — 10 of 13 lines
- `src/sim/peds/signalPostIndex.ts#SignalPostIndex.near` — 2 of 12 lines
- `src/sim/peds/signalPostIndex.ts#SignalPostIndex.placeAll` — 3 of 3 lines
- `src/sim/peds/signalPostIndex.ts#SignalPostIndex.placeJunction` — 6 of 10 lines
- `src/ui/toolChoices.ts#lamps` — 1 of 1 lines
- `src/ui/toolChoices.ts#poleLampMode` — 2 of 3 lines
- `src/ui/toolChoices.ts#setPoleLampMode` — 4 of 4 lines
- `src/ui/v2/shell.ts#mountShell.renderLandscape` — 18 of 145 lines
- `src/ui/v2/shell.ts#mountShell.renderRoads` — 11 of 116 lines
- `src/world/doc.ts#RoadDoc.addBarrier` — 6 of 8 lines
- `src/world/doc.ts#RoadDoc.addPole` — 6 of 8 lines
- `src/world/doc.ts#RoadDoc.addPoleSpan` — 11 of 12 lines
- `src/world/doc.ts#RoadDoc.barrierCells` — 1 of 1 lines
- `src/world/doc.ts#RoadDoc.barrierMap` — 1 of 1 lines
- `src/world/doc.ts#RoadDoc.barrierNear` — 14 of 15 lines
- `src/world/doc.ts#RoadDoc.barriers` — 1 of 1 lines
- `src/world/doc.ts#RoadDoc.barriersIn` — 3 of 3 lines
- `src/world/doc.ts#RoadDoc.clearBarrierRecords` — 4 of 5 lines
- `src/world/doc.ts#RoadDoc.clearPoleRecords` — 4 of 7 lines
- `src/world/doc.ts#RoadDoc.deleteBarrierRecord` — 3 of 5 lines
- `src/world/doc.ts#RoadDoc.deletePoleRecord` — 3 of 5 lines
- `src/world/doc.ts#RoadDoc.deleteSpanRecord` — 3 of 7 lines
- `src/world/doc.ts#RoadDoc.fromJSON` — 8 of 187 lines
- `src/world/doc.ts#RoadDoc.layOver` — 4 of 47 lines
- `src/world/doc.ts#RoadDoc.ownIndex` — 10 of 14 lines
- `src/world/doc.ts#RoadDoc.poleIndex` — 1 of 1 lines
- `src/world/doc.ts#RoadDoc.poleMap` — 1 of 1 lines
- `src/world/doc.ts#RoadDoc.poleNear` — 13 of 14 lines
- `src/world/doc.ts#RoadDoc.poleSpans` — 1 of 1 lines
- `src/world/doc.ts#RoadDoc.poles` — 1 of 1 lines
- `src/world/doc.ts#RoadDoc.polesIn` — 3 of 3 lines
- `src/world/doc.ts#RoadDoc.putBarrierRecord` — 4 of 5 lines
- `src/world/doc.ts#RoadDoc.putPoleRecord` — 4 of 5 lines
- `src/world/doc.ts#RoadDoc.putSpanRecord` — 7 of 9 lines
- `src/world/doc.ts#RoadDoc.removeBarrier` — 5 of 7 lines
- `src/world/doc.ts#RoadDoc.removePole` — 12 of 15 lines
- `src/world/doc.ts#RoadDoc.replaceContents` — 10 of 149 lines
- `src/world/doc.ts#RoadDoc.restore` — 11 of 105 lines
- `src/world/doc.ts#RoadDoc.restore.itemRects` — 10 of 11 lines
- `src/world/doc.ts#RoadDoc.serialized` — 7 of 35 lines
- `src/world/doc.ts#RoadDoc.spanMap` — 1 of 1 lines
- `src/world/doc.ts#RoadDoc.spansAt` — 3 of 3 lines
- `src/world/doc.ts#RoadDoc.takeFork` — 5 of 49 lines
- `src/world/doc.ts#barrierRecord` — 3 of 6 lines
- `src/world/doc.ts#itemDifferences` — 21 of 33 lines
- `src/world/doc.ts#sameBarriers` — 10 of 11 lines
- `src/world/doc.ts#savedBarrier` — 1 of 1 lines
- `src/world/doc.ts#savedPole` — 1 of 1 lines
- `src/world/doc.ts#savedSpan` — 1 of 1 lines
- `src/world/junction/bulb.ts#clearBulbsOf` — 7 of 32 lines
- `src/world/solids.ts#createSolids` — 8 of 47 lines
- `src/world/solids.ts#solidsOfItem` — 17 of 38 lines
- `src/world/viewCells.ts#ViewCells.drop` — 4 of 8 lines
- `src/world/viewCells.ts#ViewCells.make` — 4 of 11 lines
- `src/world/viewCells.ts#ViewCells.trim` — 19 of 23 lines
- `src/world/zoneGrid.ts#markPaved` — 3 of 5 lines
- `src/world/zoneGrid.ts#pavedTester.onPlate` — 1 of 1 lines

## Resources

- feature entry `barrier.kind.fence` — src/ui/i18n/en.ts:161: `'barrier.kind.fence': 'Fence'`
- feature entry `barrier.kind.wall` — src/ui/i18n/en.ts:162: `'barrier.kind.wall': 'Wall'`
- feature entry `barrier.kind.hedge` — src/ui/i18n/en.ts:163: `'barrier.kind.hedge': 'Hedge'`
- feature entry `barrier.kind.guardrail` — src/ui/i18n/en.ts:164: `'barrier.kind.guardrail': 'Guardrail'`
- feature entry `barrier.kind.railing` — src/ui/i18n/en.ts:165: `'barrier.kind.railing': 'Railing'`
- feature entry `roadFurniture.complete` — src/ui/i18n/en.ts:295: `'roadFurniture.complete': 'Complete'`
- feature entry `roadFurniture.basic` — src/ui/i18n/en.ts:296: `'roadFurniture.basic': 'Basic'`
- feature entry `roadFurniture.none` — src/ui/i18n/en.ts:297: `'roadFurniture.none': 'None'`
- feature entry `hint.streetscape.rule.stop` — src/ui/i18n/en.ts:298: `'hint.streetscape.rule.stop': 'Stop sign: this road now stops at the junction'`
- feature entry `hint.streetscape.rule.yield` — src/ui/i18n/en.ts:299: `'hint.streetscape.rule.yield': 'Give-way sign: this road now gives way at the junction'`
- feature entry `hint.streetscape.row` — src/ui/i18n/en.ts:334: `'hint.streetscape.row': 'Row laid along the footway'`
- shared entry `hint.streetscape.removed` — src/ui/i18n/en.ts:335: `'hint.streetscape.removed': 'Item removed'`
- feature entry `hint.streetscape.offFootway` — src/ui/i18n/en.ts:336: `'hint.streetscape.offFootway': 'Footways only'`
- feature entry `hint.streetscape.narrow` — src/ui/i18n/en.ts:337: `'hint.streetscape.narrow': 'The footway is too narrow for that'`
- feature entry `hint.streetscape.occupied` — src/ui/i18n/en.ts:338: `'hint.streetscape.occupied': 'Something is already here'`
- feature entry `hint.streetscape.crossing` — src/ui/i18n/en.ts:339: `'hint.streetscape.crossing': 'Not in a crossing landing'`
- feature entry `hint.pole.offFootway` — src/ui/i18n/en.ts:366: `'hint.pole.offFootway': 'Poles on footways only'`
- feature entry `hint.pole.noPath` — src/ui/i18n/en.ts:367: `'hint.pole.noPath': 'No footway joins these points'`
- feature entry `pole.lamps` — src/ui/i18n/en.ts:368: `'pole.lamps': 'Lights on the poles'`
- feature entry `pole.lamps.none` — src/ui/i18n/en.ts:369: `'pole.lamps.none': 'No lights'`
- feature entry `pole.lamps.alternate` — src/ui/i18n/en.ts:370: `'pole.lamps.alternate': 'A light on every other pole'`
- feature entry `pole.lamps.all` — src/ui/i18n/en.ts:371: `'pole.lamps.all': 'A light on every pole'`
- feature entry `hint.crossing.tooShort` — src/ui/i18n/en.ts:640: `'hint.crossing.tooShort': 'A crossing needs room for a queue of cars on both sides.'`
- feature entry `hint.crossing.unsupported` — src/ui/i18n/en.ts:641: `'hint.crossing.unsupported': 'This road carries no pedestrians.'`
- feature entry `hint.crossing.geometry` — src/ui/i18n/en.ts:642: `'hint.crossing.geometry': 'A crossing cannot be placed here.'`
- feature entry `hint.uturn.placed` — src/ui/i18n/en.ts:643: `'hint.uturn.placed': 'U-turn opened in the median.'`
- feature entry `hint.uturn.noMedian` — src/ui/i18n/en.ts:644: `'hint.uturn.noMedian': 'A U-turn needs a two-way road with a central median.'`
- feature entry `hint.uturn.tooShort` — src/ui/i18n/en.ts:645: `'hint.uturn.tooShort': 'A U-turn needs room for a queue of cars on both sides.'`
- feature entry `hint.uturn.narrow` — src/ui/i18n/en.ts:646: `'hint.uturn.narrow': 'Median too narrow: a car turns round in 12.8 m from lane to lane. Widen the median in the road profile.'`
- feature entry `hint.uturn.geometry` — src/ui/i18n/en.ts:647: `'hint.uturn.geometry': 'A U-turn cannot be opened here.'`
- shared entry `hint.pole.removed` — src/ui/i18n/en.ts:906: `'hint.pole.removed': 'Pole removed'`
- feature entry `hint.barrier.built` — src/ui/i18n/en.ts:914: `'hint.barrier.built': 'Built'`
- feature entry `hint.barrier.removed` — src/ui/i18n/en.ts:915: `'hint.barrier.removed': 'Removed'`
- feature entry `hint.barrier.road` — src/ui/i18n/en.ts:916: `'hint.barrier.road': 'A wall or fence cannot stand on a road or its pavement'`
- feature entry `hint.barrier.short` — src/ui/i18n/en.ts:917: `'hint.barrier.short': 'Too short to build'`
- feature entry `hint.barrier.nothing` — src/ui/i18n/en.ts:918: `'hint.barrier.nothing': 'No wall, fence or hedge here to remove'`
- feature entry `v2.row.lights` — src/ui/i18n/en.ts:1345: `'v2.row.lights': 'Lights'`
- feature entry `barrier.kind.fence` — src/ui/i18n/pt-BR.ts:160: `'barrier.kind.fence': 'Cerca'`
- feature entry `barrier.kind.wall` — src/ui/i18n/pt-BR.ts:161: `'barrier.kind.wall': 'Muro'`
- feature entry `barrier.kind.hedge` — src/ui/i18n/pt-BR.ts:162: `'barrier.kind.hedge': 'Sebe'`
- feature entry `barrier.kind.guardrail` — src/ui/i18n/pt-BR.ts:163: `'barrier.kind.guardrail': 'Defensa (guard-rail)'`
- feature entry `barrier.kind.railing` — src/ui/i18n/pt-BR.ts:164: `'barrier.kind.railing': 'Gradil urbano'`
- feature entry `roadFurniture.complete` — src/ui/i18n/pt-BR.ts:294: `'roadFurniture.complete': 'Completo'`
- feature entry `roadFurniture.basic` — src/ui/i18n/pt-BR.ts:295: `'roadFurniture.basic': 'Básico'`
- feature entry `roadFurniture.none` — src/ui/i18n/pt-BR.ts:296: `'roadFurniture.none': 'Nenhum'`
- feature entry `hint.streetscape.rule.stop` — src/ui/i18n/pt-BR.ts:297: `'hint.streetscape.rule.stop': 'Placa de Pare: esta via agora para no cruzamento'`
- feature entry `hint.streetscape.rule.yield` — src/ui/i18n/pt-BR.ts:298: `'hint.streetscape.rule.yield': 'Placa de Dê a preferência: esta via agora dá a preferência no cruzamento'`
- feature entry `hint.streetscape.row` — src/ui/i18n/pt-BR.ts:332: `'hint.streetscape.row': 'Fileira posta ao longo da calçada'`
- shared entry `hint.streetscape.removed` — src/ui/i18n/pt-BR.ts:333: `'hint.streetscape.removed': 'Item removido'`
- feature entry `hint.streetscape.offFootway` — src/ui/i18n/pt-BR.ts:334: `'hint.streetscape.offFootway': 'Só nas calçadas'`
- feature entry `hint.streetscape.narrow` — src/ui/i18n/pt-BR.ts:335: `'hint.streetscape.narrow': 'A calçada é estreita demais para isso'`
- feature entry `hint.streetscape.occupied` — src/ui/i18n/pt-BR.ts:336: `'hint.streetscape.occupied': 'Já há algo aqui'`
- feature entry `hint.streetscape.crossing` — src/ui/i18n/pt-BR.ts:337: `'hint.streetscape.crossing': 'Não na saída de uma faixa de pedestres'`
- feature entry `hint.pole.offFootway` — src/ui/i18n/pt-BR.ts:364: `'hint.pole.offFootway': 'Postes só nas calçadas'`
- feature entry `hint.pole.noPath` — src/ui/i18n/pt-BR.ts:365: `'hint.pole.noPath': 'Não há calçada ligando esses pontos'`
- feature entry `pole.lamps` — src/ui/i18n/pt-BR.ts:366: `'pole.lamps': 'Iluminação nos postes'`
- feature entry `pole.lamps.none` — src/ui/i18n/pt-BR.ts:367: `'pole.lamps.none': 'Sem luz'`
- feature entry `pole.lamps.alternate` — src/ui/i18n/pt-BR.ts:368: `'pole.lamps.alternate': 'Luz em postes alternados'`
- feature entry `pole.lamps.all` — src/ui/i18n/pt-BR.ts:369: `'pole.lamps.all': 'Luz em todos os postes'`
- feature entry `hint.crossing.tooShort` — src/ui/i18n/pt-BR.ts:634: `'hint.crossing.tooShort': 'A faixa precisa de espaço para uma fila de carros dos dois lados.'`
- feature entry `hint.crossing.unsupported` — src/ui/i18n/pt-BR.ts:635: `'hint.crossing.unsupported': 'Esta via não tem pedestres.'`
- feature entry `hint.crossing.geometry` — src/ui/i18n/pt-BR.ts:636: `'hint.crossing.geometry': 'Não é possível colocar uma faixa aqui.'`
- feature entry `hint.uturn.placed` — src/ui/i18n/pt-BR.ts:637: `'hint.uturn.placed': 'Retorno aberto no canteiro.'`
- feature entry `hint.uturn.noMedian` — src/ui/i18n/pt-BR.ts:638: `'hint.uturn.noMedian': 'O retorno precisa de uma via de mão dupla com canteiro central.'`
- feature entry `hint.uturn.tooShort` — src/ui/i18n/pt-BR.ts:639: `'hint.uturn.tooShort': 'O retorno precisa de espaço para uma fila de carros dos dois lados.'`
- feature entry `hint.uturn.narrow` — src/ui/i18n/pt-BR.ts:640: `'hint.uturn.narrow': 'Canteiro estreito demais: um carro dá a volta em 12,8 m de uma faixa à outra. Alargue o canteiro no perfil da via.'`
- feature entry `hint.uturn.geometry` — src/ui/i18n/pt-BR.ts:641: `'hint.uturn.geometry': 'Não é possível abrir um retorno aqui.'`
- shared entry `hint.pole.removed` — src/ui/i18n/pt-BR.ts:899: `'hint.pole.removed': 'Poste removido'`
- feature entry `hint.barrier.built` — src/ui/i18n/pt-BR.ts:907: `'hint.barrier.built': 'Construído'`
- feature entry `hint.barrier.removed` — src/ui/i18n/pt-BR.ts:908: `'hint.barrier.removed': 'Removido'`
- feature entry `hint.barrier.road` — src/ui/i18n/pt-BR.ts:909: `'hint.barrier.road': 'Muro e cerca não podem ficar sobre a via nem sobre a calçada'`
- feature entry `hint.barrier.short` — src/ui/i18n/pt-BR.ts:910: `'hint.barrier.short': 'Curto demais para construir'`
- feature entry `hint.barrier.nothing` — src/ui/i18n/pt-BR.ts:911: `'hint.barrier.nothing': 'Não há muro, cerca nem sebe aqui para remover'`
- feature entry `v2.row.lights` — src/ui/i18n/pt-BR.ts:1335: `'v2.row.lights': 'Luzes'`
- feature entry `guardrail` — src/ui/v2/shell.ts:187: `guardrail: '<path d="M5 18v-8M12 18v-8M19 18v-8"/><path d="M3 9h18v4H3Z"/><path d="M3 11h18"/>'`
- feature entry `railing` — src/ui/v2/shell.ts:188: `railing: '<path d="M4 20V6M20 20V6"/><path d="M4 7h16M4 18h16"/><path d="M8 7v11M12 7v11M16 7v11"/>'`
- feature entry `pl_none` — src/ui/v2/shell.ts:193: `pl_none: '<path d="M8 21V4"/><path d="M4 7h8"/><path d="M15 9l5 5M20 9l-5 5"/>'`
- feature entry `pl_alternate` — src/ui/v2/shell.ts:194: `pl_alternate: '<path d="M6 21V5h4M18 21V5"/><path d="M9 5l1 2H8Z"/><path d="M9 9l-.5 2" stroke-dasharray="1.5 1.5"/>'`
- feature entry `pl_all` — src/ui/v2/shell.ts:195: `pl_all: '<path d="M6 21V5h4M18 21V5h-4"/><path d="M9 5l1 2H8ZM15 5l-1 2h2Z"/><path d="M9 9l-.5 2M15 9l.5 2" stroke-dasharray="1.5 1.5"/>'`
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
- shared style `rail` — src/peopleLab/peopleLab.css:29: `.rail { display: flex; flex-direction: column; align-items: center; gap: 2px; padding: 10px 0; background: #121417; border-right: 1px solid var(--line); }`
- shared style `rail` — src/peopleLab/peopleLab.css:30: `.rail button { position: relative; width: 40px; height: 40px; border: 0; border-radius: 8px; background: transparent; color: var(--ink-3); cursor: pointer; disp`
- shared style `rail` — src/peopleLab/peopleLab.css:31: `.rail button:hover { color: var(--ink); background: var(--panel-2); }`
- shared style `rail` — src/peopleLab/peopleLab.css:32: `.rail button.on { color: var(--ink); background: var(--panel-3); }`
- shared style `rail` — src/peopleLab/peopleLab.css:33: `.rail button.on::before { content: ''; position: absolute; left: -8px; top: 9px; bottom: 9px; width: 2px; border-radius: 2px; background: var(--accent); }`
- shared style `panel` — src/peopleLab/peopleLab.css:40: `.panel { background: var(--panel); border-right: 1px solid var(--line); overflow-y: auto; padding: 14px 14px 14px; display: flex; flex-direction: column; scroll`
- shared style `panel` — src/peopleLab/peopleLab.css:41: `.panel h2 { margin: 0 0 10px; font-size: 15px; font-weight: 600; letter-spacing: -.01em; }`
- shared style `panel` — src/peopleLab/peopleLab.css:42: `.panel h3 { margin: 18px 0 8px; font-size: 11px; font-weight: 600; letter-spacing: .06em; text-transform: uppercase; color: var(--ink-3); }`
- shared style `panel` — src/peopleLab/peopleLab.css:43: `.panel h3:first-child { margin-top: 2px; }`
- shared style `row` — src/peopleLab/peopleLab.css:45: `.row { display: grid; grid-template-columns: 96px 1fr 38px; align-items: center; gap: 8px; min-height: 28px; color: var(--ink-2); }`
- shared style `row` — src/peopleLab/peopleLab.css:46: `.row output { text-align: right; color: var(--ink-3); font-variant-numeric: tabular-nums; font-size: 11px; }`
- shared style `pair` — src/peopleLab/peopleLab.css:83: `.pair { display: grid; grid-template-columns: 1fr 1fr; gap: 6px; }`
- shared style `pics` — src/peopleLab/peopleLab.css:92: `.pics { display: grid; grid-template-columns: repeat(3, 1fr); gap: 6px; margin-bottom: 4px; align-content: start; }`
- shared style `panel` — src/peopleLab/peopleLab.css:94: `.panel > * { flex-shrink: 0; }`
- shared style `panel` — src/peopleLab/peopleLab.css:95: `.panel > .picker { flex: 1 1 0; min-height: 150px; display: flex; flex-direction: column; overflow: hidden; margin-bottom: 8px; }`
- shared style `picker` — src/peopleLab/peopleLab.css:96: `.picker .pics { margin: 0; }`
- shared style `picker-search` — src/peopleLab/peopleLab.css:97: `.picker-search { display: none; align-items: center; gap: 6px; margin-bottom: 6px; background: var(--panel-2); color: var(--ink-3); border: 1px solid var(--line`
- shared style `picker-search` — src/peopleLab/peopleLab.css:98: `.picker-search.on { display: flex; }`
- shared style `picker-search` — src/peopleLab/peopleLab.css:99: `.picker-search input { flex: 1; min-width: 0; background: transparent; border: 0; outline: 0; color: var(--ink); font: inherit; font-size: 12px; padding: 5px 0;`
- shared style `picker-arrow` — src/peopleLab/peopleLab.css:100: `.picker-arrow { display: grid; place-items: center; }`
- shared style `picker-bar` — src/peopleLab/peopleLab.css:106: `.picker-bar { height: 30px; margin-top: auto; display: flex; align-items: center; justify-content: center; gap: 10px; color: var(--ink-3); font-size: 12px; font`
- shared style `picker-bar` — src/peopleLab/peopleLab.css:107: `.picker-bar.one { visibility: hidden; }`
- shared style `picker-arrow` — src/peopleLab/peopleLab.css:108: `.picker-arrow { width: 28px; height: 24px; border-radius: 6px; border: 1px solid var(--line); background: var(--panel-2); color: var(--ink); cursor: pointer; fo`
- shared style `picker-arrow` — src/peopleLab/peopleLab.css:109: `.picker-arrow:disabled { opacity: .35; cursor: default; }`
- shared style `pic` — src/peopleLab/peopleLab.css:110: `.pic { border: 1px solid var(--line); border-radius: var(--radius); background: linear-gradient(180deg, #2b3038, #1f2228); padding: 0; cursor: pointer; overflow`
- shared style `pic` — src/peopleLab/peopleLab.css:112: `.pic img { width: 100%; aspect-ratio: 1; object-fit: cover; display: block; }`
- shared style `pic` — src/peopleLab/peopleLab.css:114: `.pic span { padding: 4px 5px 5px; white-space: nowrap; overflow: hidden; text-overflow: ellipsis; text-align: center; }`
- shared style `pic` — src/peopleLab/peopleLab.css:115: `.pic:hover { border-color: #444b54; color: var(--ink); }`
- shared style `pic` — src/peopleLab/peopleLab.css:116: `.pic.on { border-color: var(--accent); color: var(--ink); box-shadow: inset 0 0 0 1px var(--accent); }`
- shared style `glass` — src/ui/app.css:57: `.glass { background: linear-gradient(145deg, var(--ui-hi), transparent 42%), var(--ui-bg); border: 1px solid var(--ui-border); border-radius: 14px; box-shadow: `
- shared style `panel-title` — src/ui/app.css:77: `.panel-title { display: flex; align-items: center; justify-content: space-between; font-size: 10px; letter-spacing: 0.12em; color: var(--muted); margin-bottom: `
- shared style `panel-title` — src/ui/app.css:86: `.panel-title kbd { font: inherit; border: 1px solid var(--ui-border); border-radius: 5px; padding: 1px 5px; }`
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
- shared style `rp-card-art` — src/ui/roads/roads.css:44: `.rp-card-art { display: block; height: 34px; border-radius: 6px; overflow: hidden; background: linear-gradient(#26383a, #1a2729); }`
- shared style `rp-card-art` — src/ui/roads/roads.css:45: `.rp-card-art svg { display: block; width: 100%; height: 100%; }`
- shared style `rp-card-name` — src/ui/roads/roads.css:46: `.rp-card-name { font-size: 11.5px; font-weight: 600; white-space: nowrap; overflow: hidden; text-overflow: ellipsis; }`
- shared style `rp-panel` — src/ui/roads/roads.css:47: `.rp-panel .rp-card-x { position: absolute; top: 3px; right: 3px; width: 20px; height: 20px; display: grid; place-items: center; border: 0; border-radius: 6px; b`
- shared style `rp-card-x` — src/ui/roads/roads.css:48: `.rp-card-x svg { width: 12px; height: 12px; fill: none; stroke: currentColor; stroke-width: 2.4; stroke-linecap: round; }`
- shared style `rp-panel` — src/ui/roads/roads.css:49: `.rp-panel .rp-card-x:hover { color: var(--v-danger-ink, #ffb3aa); }`
- shared style `rp-card-blank` — src/ui/roads/roads.css:50: `.rp-card-blank { display: grid; place-items: center; height: 100%; color: var(--v-accent); background: var(--v-accent-soft, rgba(79,224,191,.14)); }`
- shared style `rp-card-blank` — src/ui/roads/roads.css:51: `.rp-card-blank svg { width: 20px; height: 20px; fill: none; stroke: currentColor; stroke-width: 2; stroke-linecap: round; }`
- shared style `rp-stage` — src/ui/roads/roads.css:54: `.rp-stage { position: relative; border-radius: 10px; overflow: hidden; background: linear-gradient(180deg, #2b4448 0%, #1d2f32 70%); border: 1px solid var(--v-l`
- shared style `rp-end` — src/ui/roads/roads.css:64: `.rp-end { position: absolute; top: 6px; padding: 2px 7px; border-radius: 999px; background: rgba(0,0,0,.32); color: #c9d6d3; font-size: 11px; pointer-events: no`
- shared style `rp-end-l` — src/ui/roads/roads.css:65: `.rp-end-l { left: 8px; }`
- shared style `rp-end-r` — src/ui/roads/roads.css:65: `.rp-end-r { right: 8px; }`
- shared style `rp-picture` — src/ui/roads/roads.css:66: `.rp-picture { min-height: 120px; }`
- shared style `rp-empty` — src/ui/roads/roads.css:67: `.rp-empty { display: grid; place-items: center; align-content: center; gap: 8px; min-height: 160px; padding: 20px; color: var(--v-muted); text-align: center; fo`
- shared style `rp-empty` — src/ui/roads/roads.css:68: `.rp-empty svg { width: 34px; height: 34px; fill: none; stroke: currentColor; stroke-width: 1.5; }`
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
- shared style `rp-keys` — src/ui/roads/roads.css:142: `.rp-keys { position: fixed; left: 330px; bottom: 80px; z-index: 41; width: 264px; display: grid; gap: 8px; padding: 12px; background: rgba(14, 22, 24, 0.97); co`
- shared style `rp-keys` — src/ui/roads/roads.css:148: `.rp-keys * { box-sizing: border-box; }`
- shared style `rp-keys` — src/ui/roads/roads.css:149: `.rp-keys button { font: inherit; color: inherit; cursor: pointer; }`
- shared style `rp-keys-head` — src/ui/roads/roads.css:150: `.rp-keys-head { display: flex; align-items: center; justify-content: space-between; }`
- shared style `rp-keys` — src/ui/roads/roads.css:151: `.rp-keys .rp-icon { display: grid; place-items: center; width: 28px; height: 28px; border: 0; border-radius: 8px; background: none; color: var(--v-muted); }`
- shared style `rp-keys` — src/ui/roads/roads.css:152: `.rp-keys .rp-icon svg { width: 16px; height: 16px; fill: none; stroke: currentColor; stroke-width: 2; stroke-linecap: round; }`
- shared style `rp-keys-row` — src/ui/roads/roads.css:153: `.rp-keys-row { display: flex; align-items: center; justify-content: space-between; gap: 8px; min-height: 32px; }`
- shared style `rp-key` — src/ui/roads/roads.css:154: `.rp-key { min-width: 86px; min-height: 30px; padding: 0 10px; border-radius: 8px; border: 1px solid var(--v-line); border-bottom-width: 3px; background: var(--v`
- shared style `rp-key` — src/ui/roads/roads.css:155: `.rp-key:hover { border-color: var(--v-accent-line, rgba(79,224,191,.5)); }`
- shared style `rp-keys` — src/ui/roads/roads.css:157: `.rp-keys .rp-btn { justify-self: start; display: inline-flex; align-items: center; min-height: 30px; padding: 0 12px; border-radius: 999px; border: 1px solid va`
- shared style `rp-keys` — src/ui/roads/roads.css:158: `.rp-keys .rp-note { color: var(--v-danger-ink, #ffb3aa); font-size: 12px; }`
- shared style `rp-keys` — src/ui/roads/roads.css:159: `.rp-keys :focus-visible { outline: 2px solid var(--v-accent, #4fe0bf); outline-offset: 1px; }`
- shared style `v2-seg-b` — src/ui/roads/roads.css:162: `.v2-seg-b.rp-seg-text { width: auto; padding: 0 11px; font-size: 12.5px; font-weight: 600; }`
- shared style `v2-seg-b` — src/ui/roads/roads.css:163: `.v2-seg-b.rp-seg-text > span { position: static; width: auto; height: auto; overflow: visible; clip: auto; }`
- shared style `rp-connectors` — src/ui/roads/roads.css:177: `.rp-connectors .rp-btn { justify-self: start; display: inline-flex; align-items: center; min-height: 30px; padding: 0 12px; border-radius: 999px; border: 1px so`
- shared style `rp-junction` — src/ui/roads/roads.css:180: `.rp-junction { display: grid; gap: 8px; margin: 10px 0; color: var(--v-text, #eef4f2); font: 13px/1.3 Inter, ui-sans-serif, "Segoe UI", Roboto, sans-serif; }`
- shared style `rp-junction` — src/ui/roads/roads.css:181: `.rp-junction * { box-sizing: border-box; }`
- shared style `rp-junction` — src/ui/roads/roads.css:182: `.rp-junction button { font: inherit; color: inherit; cursor: pointer; }`
- shared style `rp-junction` — src/ui/roads/roads.css:183: `.rp-junction button:disabled { opacity: 0.35; cursor: default; }`
- shared style `rp-junction` — src/ui/roads/roads.css:184: `.rp-junction :focus-visible { outline: 2px solid var(--v-accent, #4fe0bf); outline-offset: 1px; }`
- shared style `rp-junction` — src/ui/roads/roads.css:185: `.rp-junction .rp-block { display: grid; gap: 8px; padding: 10px; border: 1px solid rgba(255,255,255,.12); border-radius: 10px; background: rgba(255,255,255,.04)`
- shared style `rp-junction` — src/ui/roads/roads.css:186: `.rp-junction .rp-block-head { font-weight: 650; }`
- shared style `rp-junction` — src/ui/roads/roads.css:187: `.rp-junction .rp-row { display: grid; grid-template-columns: 84px minmax(0, 1fr); align-items: center; gap: 8px; min-height: 30px; }`
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
- shared style `rc-art` — src/ui/roads/roads.css:221: `.rc-art { display: block; width: 124px; height: 64px; border-radius: 8px; border: 2px solid transparent; box-shadow: 0 1px 0 rgba(0,0,0,.35); transition: border`
- shared style `rc-card-main` — src/ui/roads/roads.css:222: `.rc-card-main:hover .rc-art { border-color: rgba(255,255,255,.45); transform: translateY(-1px); }`
- shared style `rc-card` — src/ui/roads/roads.css:223: `.rc-card.on .rc-art { border-color: var(--v-accent, #4fe0bf); }`
- shared style `rc-name` — src/ui/roads/roads.css:224: `.rc-name { padding: 0 2px; font-weight: 600; font-size: 11px; line-height: 1.2; min-height: 3.6em; display: -webkit-box; -webkit-line-clamp: 3; -webkit-box-orie`
- shared style `rc-card` — src/ui/roads/roads.css:225: `.rc-card.on .rc-name { color: var(--v-accent, #4fe0bf); }`
- shared style `rc-meta` — src/ui/roads/roads.css:226: `.rc-meta { padding: 0 2px; font-size: 10.5px; color: var(--v-muted, #a3b4b0); font-variant-numeric: tabular-nums; white-space: nowrap; }`
- shared style `rc-custom` — src/ui/roads/roads.css:227: `.rc-custom { position: absolute; top: 4px; right: 4px; display: grid; place-items: center; width: 24px; height: 24px; padding: 0; border-radius: 7px; border: 0;`
- shared style `rc-custom` — src/ui/roads/roads.css:228: `.rc-custom:hover { background: var(--v-accent, #4fe0bf); color: var(--v-accent-ink, #062520); }`
- shared style `rc-empty` — src/ui/roads/roads.css:229: `.rc-empty { padding: 10px 4px; color: var(--v-muted, #a3b4b0); max-width: 360px; }`
- shared style `rp-summary-actions` — src/ui/roads/roads.css:238: `.rp-summary-actions { display: flex; flex-wrap: wrap; gap: 6px; }`
- shared style `v2-seg-b` — src/ui/v2/shell.css:41: `.v2-icon > span, .v2-pill > span, .v2-seg-b > span, .v2-tabs.compact .v2-tab.icon:not(.on) > span, .v2-tabs.tight .v2-tab.icon > span, .v2-choice.icon > span, .`
- shared style `v2-seg-b` — src/ui/v2/shell.css:58: `.v2-seg-b, .v2-speed-b { position: relative; display: grid; place-items: center; min-width: 34px; height: 28px; padding: 0 6px; border: 0; border-radius: 999px;`
- shared style `v2-seg-b` — src/ui/v2/shell.css:62: `.v2-seg-b:hover, .v2-speed-b:hover { color: var(--v-text); }`
- shared style `v2-seg-b` — src/ui/v2/shell.css:63: `.v2-seg-b.on, .v2-speed-b.on { background: var(--v-accent); color: var(--v-accent-ink); }`

## Where the program plugs the feature in

### src/editor/barriers.ts

- `src/editor/barriers.ts#BarrierTool` (class, line 76) ← construct from `src/main.ts#barrierTool`
- `src/editor/barriers.ts#BarrierTool.cancel` (method, line 91) ← call from `src/main.ts#cancelGestures`
- `src/editor/barriers.ts#BarrierTool.down` (method, line 96) ← call from `src/main.ts#pointerDown`
- `src/editor/barriers.ts#BarrierTool.gesture` (method, line 87) ← call from `src/main.ts#currentGesture`
- `src/editor/barriers.ts#BarrierTool.key` (method, line 123) ← call from `src/main.ts#gameKeys`
- `src/editor/barriers.ts#BarrierTool.move` (method, line 117) ← call from `src/main.ts#pointerMove`
- `src/editor/barriers.ts#BarrierTool.plan` (method, line 155) ← call from `src/main.ts#drawBarrierPlan`
- `src/editor/barriers.ts#BarrierTool.plan` (method, line 155) ← read from `src/main.ts#drawBarrierPlan`

### src/editor/poles.ts

- `src/editor/poles.ts#commitPoleRun` (function, line 368) ← call from `src/main.ts#<IfStatement>@4036`
- `src/editor/poles.ts#commitPoleRunPoles` (function, line 373) ← call from `src/editor/roads/powerLine.ts#layPowerLinesSteps`
- `src/editor/poles.ts#PlannedPole` (interface, line 65) ← read from `src/main.ts#drawPolePlan`, `src/main.ts#frame`
- `src/editor/poles.ts#planPoleRun` (function, line 240) ← call from `src/editor/roads/powerLine.ts#runPlanner`, `src/main.ts#<IfStatement>@4036`
- `src/editor/poles.ts#POLE_PICK_PIXELS` (variable, line 17) ← read from `src/main.ts#<IfStatement>@4036`
- `src/editor/poles.ts#PoleRunPlan` (interface, line 74) ← read from `src/main.ts#drawPolePlan`, `src/main.ts#frame`
- `src/editor/poles.ts#PoleRunPlan` (interface, line 74) ← typeRef from `src/editor/roads/powerLine.ts#PlanRun`, `src/main.ts#drawPolePlan`, `src/main.ts#framePolePlan`
- `src/editor/poles.ts#poleRunPlanner` (function, line 160) ← call from `src/editor/roads/powerLine.ts#runPlanner`
- `src/editor/poles.ts#PoleRunPlanner` (interface, line 155) ← call from `src/editor/roads/powerLine.ts#runPlanner`
- `src/editor/poles.ts#poleRunPlanner.plan` (method, line 177) ← implementation from `src/editor/roads/powerLine.ts#runPlanner`
- `src/editor/poles.ts#PoleSnap` (interface, line 42) ← read from `src/main.ts#drawPolePlan`
- `src/editor/poles.ts#PoleTool` (class, line 407) ← construct from `src/main.ts#poleTool`
- `src/editor/poles.ts#PoleTool.cancel` (method, line 431) ← call from `src/main.ts#cancelGestures`
- `src/editor/poles.ts#PoleTool.down` (method, line 451) ← call from `src/main.ts#pointerDown`
- `src/editor/poles.ts#PoleTool.gesture` (method, line 423) ← call from `src/main.ts#currentGesture`
- `src/editor/poles.ts#PoleTool.inProgress` (method, line 427) ← call from `src/main.ts#gestureInProgress`
- `src/editor/poles.ts#PoleTool.marks` (method, line 506) ← call from `src/main.ts#drawPolePlan`
- `src/editor/poles.ts#PoleTool.marks` (method, line 506) ← read from `src/main.ts#drawPolePlan`
- `src/editor/poles.ts#PoleTool.move` (method, line 473) ← call from `src/main.ts#pointerMove`
- `src/editor/poles.ts#PoleTool.plan` (method, line 437) ← call from `src/main.ts#frame`
- `src/editor/poles.ts#PoleTool.reach` (method, line 419) ← call from `src/main.ts#poleReach`
- `src/editor/poles.ts#PoleTool.up` (method, line 488) ← call from `src/main.ts#endPointer`

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

### src/render/barriers.ts

- `src/render/barriers.ts#BarrierProduct.allKeys` (method, line 51) ← implementation from `src/world/derived.ts#Derived.refresh`
- `src/render/barriers.ts#BarrierProduct.changed` (method, line 83) ← implementation from `src/world/derived.ts#Derived.refresh`
- `src/render/barriers.ts#BarrierProduct.compute` (method, line 68) ← implementation from `src/world/derived.ts#Derived.refresh`
- `src/render/barriers.ts#BarrierProduct.dirty` (method, line 58) ← implementation from `src/world/derived.ts#Derived.refresh`
- `src/render/barriers.ts#BarrierProduct.equals` (method, line 79) ← implementation from `src/world/derived.ts#Derived.refresh`
- `src/render/barriers.ts#Barriers` (interface, line 12) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/barriers.ts#Barriers` (interface, line 12) ← read from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.switchedOff`
- `src/render/barriers.ts#createBarriers` (function, line 104) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/barriers.ts#createBarriers.cull` (method, line 128) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/barriers.ts#createBarriers.dispose` (method, line 132) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/barriers.ts#createBarriers.sync` (method, line 116) ← implementation from `src/render/renderer.ts#createSceneRenderer`

### src/render/lightLevels.ts

- `src/render/lightLevels.ts#lightPoolTexture` (function, line 39) ← call from `src/render/buildings/kit.ts#createBuildingKit`, `src/render/scenery.ts#createSceneryKit`
- `src/render/lightLevels.ts#luminanceOf` (function, line 28) ← call from `src/render/buildings/kit.ts#createBuildingKit.setNight`
- `src/render/lightLevels.ts#NIGHT_LUMINANCE` (variable, line 9) ← read from `src/render/agents.ts#LAMP_GAIN`, `src/render/buildings/kit.ts#createBuildingKit.setNight`
- `src/render/lightLevels.ts#nightGain` (function, line 34) ← call from `src/render/agents.ts#LAMP_GAIN`

### src/render/poleCells.ts

- `src/render/poleCells.ts#PoleCells` (class, line 49) ← construct from `src/render/renderer.ts#createSceneRenderer.syncUtilities`
- `src/render/poleCells.ts#PoleCells` (class, line 49) ← typeRef from `src/render/renderer.ts#createSceneRenderer`
- `src/render/poleCells.ts#PoleCells.constructor` (constructor, line 66) ← implementation from `src/world/viewCells.ts#ViewCells.drop`, `src/world/viewCells.ts#ViewCells.make`
- `src/render/poleCells.ts#PoleCells.dispose` (method, line 234) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/poleCells.ts#PoleCells.dispose` (method, line 234) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/poleCells.ts#PoleCells.follow` (method, line 91) ← call from `src/render/renderer.ts#createSceneRenderer.syncUtilities`
- `src/render/poleCells.ts#PoleCells.group` (field, line 50) ← read from `src/render/renderer.ts#createSceneRenderer.switchedOff`, `src/render/renderer.ts#createSceneRenderer.syncUtilities`
- `src/render/poleCells.ts#PoleCells.look` (method, line 96) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/poleCells.ts#PoleCells.source` (method, line 85) ← call from `src/render/renderer.ts#createSceneRenderer.syncUtilities`

### src/render/poleLayer.ts

- `src/render/poleLayer.ts#buildPolePreview` (function, line 729) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/poleLayer.ts#buildUtilities` (function, line 711) ← call from `src/render/renderer.ts#createSceneRenderer.warmRoadShaders`
- `src/render/poleLayer.ts#PartStore.meshOf` (method, line 178) ← implementation from `src/render/shadowCasters.ts#castCastersInto`, `src/render/shadowCasters.ts#castShadowKey`
- `src/render/poleLayer.ts#PoleLayer.dispose` (method, line 697) ← call from `src/render/renderer.ts#createSceneRenderer.warmRoadShaders`
- `src/render/poleLayer.ts#PoleLayer.dispose` (method, line 697) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/poleLayer.ts#PoleLayer.group` (field, line 514) ← read from `src/render/renderer.ts#createSceneRenderer.warmRoadShaders`
- `src/render/poleLayer.ts#PolePreviewInput` (interface, line 724) ← read from `src/render/renderer.ts#createSceneRenderer`
- `src/render/poleLayer.ts#PolePreviewInput` (interface, line 724) ← typeRef from `src/render/renderer.ts#SceneHandle`
- `src/render/poleLayer.ts#WireStore.makeMesh` (method, line 365) ← implementation from `src/render/shadowCasters.ts#castCastersInto`

### src/render/propCells.ts

- `src/render/propCells.ts#createPropLayer` (function, line 254) ← call from `src/render/gardens.ts#createGardenCells`
- `src/render/propCells.ts#createPropLayer` (function, line 254) ← implementation from `src/world/viewCells.ts#ViewCells.drop`, `src/world/viewCells.ts#ViewCells.make`
- `src/render/propCells.ts#createPropLayer.batches` (function, line 310) ← callbackAssign from `src/render/cellInstances.ts#createCellInstances.makeMesh`
- `src/render/propCells.ts#createPropLayer.batches` (function, line 310) ← implementation from `src/render/cellInstances.ts#createCellInstances.makeMesh`, `src/render/shadowCasters.ts#castCastersInto`
- `src/render/propCells.ts#createPropLayer.batches.own` (function, line 324) ← indirectCall from `src/render/mesh/gpuOnly.ts#gpuOnlyAttributes`
- `src/render/propCells.ts#createPropLayer.cull` (method, line 591) ← implementation from `src/render/gardens.ts#createGardenCells.cull`
- `src/render/propCells.ts#createPropLayer.dispose` (method, line 596) ← implementation from `src/render/gardens.ts#createGardenCells.dispose`
- `src/render/propCells.ts#createPropLayer.follow` (method, line 511) ← implementation from `src/render/gardens.ts#createGardenCells.follow`
- `src/render/propCells.ts#createPropLayer.setMap` (method, line 590) ← implementation from `src/render/gardens.ts#createGardenCells.setMap`
- `src/render/propCells.ts#lookProps` (function, line 609) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/propCells.ts#PropLayer` (interface, line 187) ← call from `src/render/gardens.ts#createGardenCells.cull`, `src/render/gardens.ts#createGardenCells.dispose`, `src/render/gardens.ts#createGardenCells.follow`, `src/render/gardens.ts#createGardenCells.setMap`
- `src/render/propCells.ts#PropLayer` (interface, line 187) ← read from `src/render/gardens.ts#createGardenCells`, `src/render/gardens.ts#createGardenCells.source`, `src/render/renderer.ts#createSceneRenderer`
- `src/render/propCells.ts#PropLayer` (interface, line 187) ← typeRef from `src/render/gardens.ts#GardenCells`
- `src/render/propCells.ts#PropLayer` (interface, line 187) ← write from `src/render/gardens.ts#createGardenCells.source`
- `src/render/propCells.ts#propView` (function, line 89) ← call from `src/render/renderer.ts#createSceneRenderer`

### src/render/propGeometry.ts

- `src/render/propGeometry.ts#benchGeometry` (function, line 302) ← call from `src/render/scenery.ts#createSceneryKit`
- `src/render/propGeometry.ts#binGeometry` (function, line 330) ← call from `src/render/scenery.ts#createSceneryKit`
- `src/render/propGeometry.ts#BUSH_KINDS` (variable, line 158) ← read from `src/render/gardens.ts#gardenKinds`, `src/render/gardens.ts#gardenPack`, `src/render/gardens.ts#gardenPictures`, `src/render/gardens.ts#KIND_KEYS`, …
- `src/render/propGeometry.ts#BushKind` (type, line 157) ← typeRef from `src/render/plantImpostors.ts#PlantModels`, `src/render/plantImpostors.ts#SceneryPictures`, `src/render/scenery.ts#createSceneryKit`, `src/render/scenery.ts#SceneryKit`
- `src/render/propGeometry.ts#bushParts` (function, line 161) ← call from `src/render/scenery.ts#createSceneryKit`
- `src/render/propGeometry.ts#drainGeometry` (function, line 405) ← call from `src/render/scenery.ts#createSceneryKit`
- `src/render/propGeometry.ts#grassTuftGeometry` (function, line 166) ← call from `src/render/scenery.ts#createSceneryKit`
- `src/render/propGeometry.ts#hydrantGeometry` (function, line 350) ← call from `src/render/scenery.ts#createSceneryKit`
- `src/render/propGeometry.ts#lampFarGeometry` (function, line 268) ← call from `src/render/scenery.ts#createSceneryKit`
- `src/render/propGeometry.ts#lampGeometry` (function, line 240) ← call from `src/render/scenery.ts#createSceneryKit`
- `src/render/propGeometry.ts#lampLensGeometry` (function, line 286) ← call from `src/render/scenery.ts#createSceneryKit`
- `src/render/propGeometry.ts#phoneGeometry` (function, line 384) ← call from `src/render/scenery.ts#createSceneryKit`
- `src/render/propGeometry.ts#postboxGeometry` (function, line 366) ← call from `src/render/scenery.ts#createSceneryKit`
- `src/render/propGeometry.ts#TREE_SPECIES` (variable, line 145) ← read from `src/render/gardens.ts#gardenKinds`, `src/render/gardens.ts#gardenPack`, `src/render/gardens.ts#gardenPictures`, `src/render/gardens.ts#KIND_KEYS`, …
- `src/render/propGeometry.ts#treeParts` (function, line 153) ← call from `src/render/scenery.ts#createSceneryKit`
- `src/render/propGeometry.ts#treePitGeometry` (function, line 427) ← call from `src/render/scenery.ts#createSceneryKit`
- `src/render/propGeometry.ts#TreeSpecies` (type, line 144) ← typeRef from `src/render/gardens.ts#gardenPack`, `src/render/gardens.ts#GardenPlant`, `src/render/gardens.ts#gardenSpecies`, `src/render/plantImpostors.ts#PlantModels`, …
- `src/render/propGeometry.ts#trianglesOf` (function, line 450) ← call from `src/render/cellInstances.ts#createCellInstances`
- `src/render/propGeometry.ts#wildflowerGeometry` (function, line 221) ← call from `src/render/scenery.ts#createSceneryKit`

### src/render/signals.ts

- `src/render/signals.ts#createSignalHeads` (function, line 105) ← call from `src/lab/catalog/layers.ts#signalItems`, `src/render/renderer.ts#createSceneRenderer`
- `src/render/signals.ts#createSignalHeads.dispose` (method, line 708) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/signals.ts#createSignalHeads.sync` (method, line 662) ← implementation from `src/lab/catalog/layers.ts#signalItems`, `src/render/renderer.ts#createSceneRenderer`
- `src/render/signals.ts#SignalBatch.makeMesh` (method, line 377) ← implementation from `src/render/shadowCasters.ts#castCastersInto`
- `src/render/signals.ts#SignalHeads` (interface, line 60) ← call from `src/lab/catalog/layers.ts#signalItems`, `src/render/renderer.ts#createSceneRenderer`
- `src/render/signals.ts#SignalHeads` (interface, line 60) ← read from `src/lab/catalog/layers.ts#signalItems`, `src/render/renderer.ts#createSceneRenderer.switchedOff`
- `src/render/signals.ts#SignalHeads` (interface, line 60) ← typeRef from `src/render/renderer.ts#createSceneRenderer`

### src/render/signLayer.ts

- `src/render/signLayer.ts#createSignLayer` (function, line 239) ← call from `src/lab/catalog/layers.ts#signItems.one`, `src/render/renderer.ts#createSceneRenderer`
- `src/render/signLayer.ts#createSignLayer.cull` (method, line 426) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/signLayer.ts#createSignLayer.dispose` (method, line 429) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/signLayer.ts#createSignLayer.refill` (method, line 409) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/signLayer.ts#createSignLayer.sync` (method, line 394) ← implementation from `src/lab/catalog/layers.ts#signItems.one`, `src/render/renderer.ts#createSceneRenderer`
- `src/render/signLayer.ts#createSignLayer.useCopier` (method, line 412) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/signLayer.ts#createSignLayer.warm` (method, line 415) ← implementation from `src/render/renderer.ts#createSceneRenderer.warmRoadShaders`
- `src/render/signLayer.ts#SignLayerKept` (interface, line 219) ← call from `src/lab/catalog/layers.ts#signItems.one`, `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.warmRoadShaders`
- `src/render/signLayer.ts#SignLayerKept` (interface, line 219) ← read from `src/lab/catalog/layers.ts#signItems.one`, `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.switchedOff`
- `src/render/signLayer.ts#SignProduct.allKeys` (method, line 73) ← implementation from `src/world/derived.ts#Derived.refresh`
- `src/render/signLayer.ts#SignProduct.changed` (method, line 125) ← implementation from `src/world/derived.ts#Derived.refresh`
- `src/render/signLayer.ts#SignProduct.compute` (method, line 112) ← implementation from `src/world/derived.ts#Derived.refresh`
- `src/render/signLayer.ts#SignProduct.dirty` (method, line 81) ← implementation from `src/world/derived.ts#Derived.refresh`
- `src/render/signLayer.ts#SignProduct.equals` (method, line 117) ← implementation from `src/world/derived.ts#Derived.refresh`

### src/render/streetFurnitureLayer.ts

- `src/render/streetFurnitureLayer.ts#createStreetFurniture` (function, line 400) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/streetFurnitureLayer.ts#createStreetFurniture.cull` (method, line 460) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/streetFurnitureLayer.ts#createStreetFurniture.dispose` (method, line 464) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/streetFurnitureLayer.ts#createStreetFurniture.setMap` (method, line 454) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/streetFurnitureLayer.ts#createStreetFurniture.sync` (method, line 430) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/streetFurnitureLayer.ts#createStreetFurniture.warm` (method, line 457) ← implementation from `src/render/renderer.ts#createSceneRenderer.warmRoadShaders`
- `src/render/streetFurnitureLayer.ts#furnitureStores.plant` (function, line 126) ← callbackAssign from `src/render/cellInstances.ts#createCellInstances.cull`, `src/render/cellInstances.ts#createCellInstances.makeMesh`
- `src/render/streetFurnitureLayer.ts#furnitureStores.plant` (function, line 126) ← implementation from `src/render/cellInstances.ts#createCellInstances.cull`, `src/render/cellInstances.ts#createCellInstances.makeMesh`
- `src/render/streetFurnitureLayer.ts#PlacedProduct.allKeys` (method, line 263) ← implementation from `src/world/derived.ts#Derived.refresh`
- `src/render/streetFurnitureLayer.ts#PlacedProduct.changed` (method, line 295) ← implementation from `src/world/derived.ts#Derived.refresh`
- `src/render/streetFurnitureLayer.ts#PlacedProduct.compute` (method, line 290) ← implementation from `src/world/derived.ts#Derived.refresh`
- `src/render/streetFurnitureLayer.ts#PlacedProduct.dirty` (method, line 270) ← implementation from `src/world/derived.ts#Derived.refresh`
- `src/render/streetFurnitureLayer.ts#StreetFurnitureLayer` (interface, line 358) ← call from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.warmRoadShaders`
- `src/render/streetFurnitureLayer.ts#StreetFurnitureLayer` (interface, line 358) ← read from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.switchedOff`

### src/render/utilities.ts

- `src/render/utilities.ts#poleGroundAt` (function, line 524) ← call from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.rebuildUtilities`, `src/render/renderer.ts#createSceneRenderer.warmRoadShaders`, `src/render/renderer.ts#createSceneRenderer.worldSteps`
- `src/render/utilities.ts#Utilities` (interface, line 73) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/utilities.ts#Utilities` (interface, line 73) ← read from `src/render/renderer.ts#createSceneRenderer`
- `src/render/utilities.ts#Utilities` (interface, line 73) ← typeRef from `src/render/renderer.ts#createSceneRenderer`

### src/ui/roads/furnitureChoice.ts

- `src/ui/roads/furnitureChoice.ts#furnitureChosen` (function, line 14) ← call from `src/main.ts#roadTool`, `src/ui/v2/shell.ts#mountShell.renderRoads`
- `src/ui/roads/furnitureChoice.ts#setFurnitureChosen` (function, line 18) ← call from `src/ui/v2/shell.ts#mountShell.renderRoads`

### src/world/barriers.ts

- `src/world/barriers.ts#Barrier` (interface, line 9) ← read from `src/editor/bulldozer.ts#Bulldozer.boxed`, `src/editor/chunkedSave.ts#KEYED`, `src/main.ts#barrierTool`, `src/render/flatAtlas.ts#createFlatAtlas.readWholeContent`, …
- `src/world/barriers.ts#Barrier` (interface, line 9) ← typeRef from `src/editor/chunkedSave.ts#KEYED`, `src/world/doc.ts#barrierRecord`, `src/world/doc.ts#RoadDoc.addBarrier`, `src/world/doc.ts#RoadDoc.barrierCells`, …
- `src/world/barriers.ts#BARRIER_KINDS` (variable, line 4) ← read from `src/main.ts#mountInterface`, `src/ui/v2/shell.ts#mountShell.renderLandscape`
- `src/world/barriers.ts#BARRIER_SIZE` (variable, line 17) ← read from `src/world/solids.ts#solidsOfItem`
- `src/world/barriers.ts#BarrierKind` (type, line 2) ← typeRef from `src/main.ts#gameState`, `src/main.ts#mountInterface`, `src/world/doc.ts#RoadDoc.addBarrier`
- `src/world/barriers.ts#isBarrierKind` (function, line 25) ← call from `src/world/doc.ts#barrierRecord`

### src/world/poleIndex.ts

- `src/world/poleIndex.ts#PoleIndex` (class, line 25) ← construct from `src/world/doc.ts#RoadDoc.clearPoleRecords`, `src/world/doc.ts#RoadDoc.poleIndex`
- `src/world/poleIndex.ts#PoleIndex.clear` (method, line 45) ← call from `src/world/doc.ts#RoadDoc.clearPoleRecords`
- `src/world/poleIndex.ts#PoleIndex.deletePole` (method, line 63) ← call from `src/world/doc.ts#RoadDoc.deletePoleRecord`
- `src/world/poleIndex.ts#PoleIndex.deleteSpan` (method, line 85) ← call from `src/world/doc.ts#RoadDoc.deleteSpanRecord`, `src/world/doc.ts#RoadDoc.putSpanRecord`
- `src/world/poleIndex.ts#PoleIndex.near` (method, line 100) ← call from `src/world/doc.ts#RoadDoc.polesIn`
- `src/world/poleIndex.ts#PoleIndex.over` (method, line 32) ← call from `src/world/doc.ts#RoadDoc.ownIndex`
- `src/world/poleIndex.ts#PoleIndex.putPole` (method, line 52) ← call from `src/world/doc.ts#RoadDoc.putPoleRecord`
- `src/world/poleIndex.ts#PoleIndex.putSpan` (method, line 77) ← call from `src/world/doc.ts#RoadDoc.putSpanRecord`
- `src/world/poleIndex.ts#PoleIndex.spansAt` (method, line 95) ← call from `src/world/doc.ts#RoadDoc.spansAt`

### src/world/poleLines.ts

- `src/world/poleLines.ts#junctionRings` (function, line 355) ← indirectCall from `src/world/surfaces.ts#levelRingsOf`
- `src/world/poleLines.ts#markPlates` (function, line 384) ← call from `src/world/zoneGrid.ts#markPaved`
- `src/world/poleLines.ts#onPaving` (function, line 481) ← call from `src/world/zoneGrid.ts#pavedTester.onPlate`
- `src/world/poleLines.ts#poleLinesOf` (function, line 230) ← indirectCall from `src/world/surfaces.ts#levelRingsOf`
- `src/world/poleLines.ts#ribbonRings` (function, line 341) ← indirectCall from `src/world/surfaces.ts#levelRingsOf`

### src/world/roads/furnitureCatalog.ts

- `src/world/roads/furnitureCatalog.ts#FURNITURE_CATALOG` (variable, line 17) ← read from `src/editor/roads/streetKit.ts#carried`
- `src/world/roads/furnitureCatalog.ts#FurnitureEntry` (interface, line 6) ← read from `src/editor/roads/streetKit.ts#carried`

### src/world/roads/furnitureSets.ts

- `src/world/roads/furnitureSets.ts#FURNITURE_SETS` (variable, line 14) ← read from `src/ui/v2/shell.ts#mountShell.renderRoads`
- `src/world/roads/furnitureSets.ts#furnitureFor` (function, line 71) ← call from `src/editor/cityGenerator.ts#greenCity`, `src/editor/roads/streetKit.ts#carryStreetKit`, `src/editor/roadTool.ts#RoadTool.layPart`
- `src/world/roads/furnitureSets.ts#FurniturePlacement` (interface, line 65) ← read from `src/editor/cityGenerator.ts#greenCity`, `src/editor/roads/streetKit.ts#carryStreetKit`, `src/editor/roadTool.ts#RoadTool.layPart`
- `src/world/roads/furnitureSets.ts#FurnitureSet` (type, line 13) ← typeRef from `src/editor/roads/streetKit.ts#carryStreetKit`, `src/editor/roadTool.ts#RoadToolHost`

### src/world/signalPosts.ts

- `src/world/signalPosts.ts#SIGNAL_POST_RADIUS` (variable, line 13) ← read from `src/sim/agents/walk.ts#stepWalkers`
- `src/world/signalPosts.ts#SignalPost` (interface, line 15) ← read from `src/actionsWiring.ts#createActions.explodeAt`, `src/sim/agents/walk.ts#stepWalkers`, `src/sim/peds/signalPostIndex.ts#SignalPostIndex.file`, `src/sim/peds/signalPostIndex.ts#SignalPostIndex.lift`
- `src/world/signalPosts.ts#SignalPost` (interface, line 15) ← typeRef from `src/sim/peds/signalPostIndex.ts#SignalPostIndex.byNode`, `src/sim/peds/signalPostIndex.ts#SignalPostIndex.cells`, `src/sim/peds/signalPostIndex.ts#SignalPostIndex.file`, `src/sim/peds/signalPostIndex.ts#SignalPostIndex.found`, …
- `src/world/signalPosts.ts#signalPostPlace` (function, line 25) ← call from `src/sim/peds/signalPostIndex.ts#SignalPostIndex.placeJunction`
- `src/world/signalPosts.ts#signalPosts` (function, line 53) ← call from `src/actionsWiring.ts#createActions.explodeAt`, `src/sim/peds/signalPostIndex.ts#SignalPostIndex.placeAll`

### src/world/streetFurniture.ts

- `src/world/streetFurniture.ts#FurnitureKind` (type, line 11) ← typeRef from `src/render/scenery.ts#SceneryKit`

### src/world/utilities.ts

- `src/world/utilities.ts#POLE_ARM_DROP` (variable, line 16) ← read from `src/render/playEffects.ts#createPlayEffects.explode`
- `src/world/utilities.ts#POLE_ARM_HALF` (variable, line 14) ← read from `src/render/playEffects.ts#createPlayEffects.explode`
- `src/world/utilities.ts#POLE_HEIGHT` (variable, line 9) ← read from `src/render/playEffects.ts#createPlayEffects.explode`
- `src/world/utilities.ts#POLE_LAMP_MODES` (variable, line 72) ← read from `src/ui/toolChoices.ts#lamps`, `src/ui/v2/shell.ts#mountShell.renderLandscape`
- `src/world/utilities.ts#POLE_LAMP_REACH` (variable, line 24) ← read from `src/render/playEffects.ts#createPlayEffects.explode`
- `src/world/utilities.ts#PoleLampMode` (type, line 73) ← typeRef from `src/ui/toolChoices.ts#lamps`, `src/ui/toolChoices.ts#poleLampMode`, `src/ui/toolChoices.ts#setPoleLampMode`
- `src/world/utilities.ts#UtilityPole` (interface, line 43) ← read from `src/actionsWiring.ts#createActions.explodeAt`, `src/editor/bulldozer.ts#Bulldozer.boxed`, `src/editor/bulldozer.ts#Bulldozer.click`, `src/editor/chunkedSave.ts#KEYED`, …
- `src/world/utilities.ts#UtilityPole` (interface, line 43) ← typeRef from `src/editor/chunkedSave.ts#KEYED`, `src/world/doc.ts#RoadDoc.addPole`, `src/world/doc.ts#RoadDoc.poleMap`, `src/world/doc.ts#RoadDoc.poleNear`, …
- `src/world/utilities.ts#UtilitySpan` (interface, line 51) ← read from `src/actionsWiring.ts#createActions.explodeAt`, `src/editor/chunkedSave.ts#KEYED`, `src/world/doc.ts#itemDifferences`, `src/world/doc.ts#RoadDoc.addPoleSpan`, …
- `src/world/utilities.ts#UtilitySpan` (interface, line 51) ← typeRef from `src/editor/chunkedSave.ts#KEYED`, `src/world/doc.ts#itemDifferences`, `src/world/doc.ts#RoadDoc.addPoleSpan`, `src/world/doc.ts#RoadDoc.poleSpans`, …

## Shared declarations the feature uses

### src/core/aabb.ts

- `src/core/aabb.ts#Aabb` (interface, line 3) — read by 4: `src/render/signLayer.ts#ribbonBox`, `src/world/roads/furnitureSets.ts#furnitureFor`, `src/world/landscape.ts#footwayAt`, `src/world/landscape.ts#medianAt`

### src/core/clipper.ts

- `src/core/clipper.ts#CLIPPER_SCALE` (variable, line 14) — read by 4: `src/world/poleLines.ts#insideLevels`, `src/world/poleLines.ts#kerbward`, `src/world/poleLines.ts#markPlates`, `src/world/poleLines.ts#windingRings`
- `src/core/clipper.ts#MultiPoly` (type, line 9) — typeRef by 6: `src/world/poleLines.ts#boxesOf`, `src/world/poleLines.ts#contoursOf`, `src/world/poleLines.ts#PoleLines`, `src/world/poleLines.ts#ringBoxes`, …
- `src/core/clipper.ts#union` (function, line 143) — call by 2: `src/world/poleLines.ts#poleLinesOf`, `src/world/surfaces.ts#levelPolygons`

### src/core/forkMap.ts

- `src/core/forkMap.ts#ForkMap` (class, line 2) — construct by 1: `src/world/poleIndex.ts#PoleIndex.over`
- `src/core/forkMap.ts#ForkMap` (class, line 2) — reference by 1: `src/world/poleIndex.ts#PoleIndex.own`
- `src/core/forkMap.ts#ForkMap.writable` (method, line 37) — call by 1: `src/world/poleIndex.ts#PoleIndex.own`

### src/core/heap.ts

- `src/core/heap.ts#Heap` (class, line 3) — construct by 1: `src/editor/poleRoutes.ts#route`
- `src/core/heap.ts#Heap.pop` (method, line 24) — call by 1: `src/editor/poleRoutes.ts#route`
- `src/core/heap.ts#Heap.push` (method, line 14) — call by 2: `src/editor/poleRoutes.ts#route`, `src/editor/poleRoutes.ts#route.relax`
- `src/core/heap.ts#Heap.size` (getter, line 10) — call by 1: `src/editor/poleRoutes.ts#route`

### src/core/polyline.ts

- `src/core/polyline.ts#Frame` (interface, line 7) — read by 13: `src/editor/poleRoutes.ts#buildRouteGraph`, `src/editor/barriers.ts#snapBarrierPoint`, `src/editor/streetObjects.ts#commitPedestrianCrossing`, `src/editor/streetObjects.ts#commitUturn`, …
- `src/core/polyline.ts#Polyline` (class, line 36) — reference by 1: `src/world/poleLines.ts#contoursOf`
- `src/core/polyline.ts#Polyline` (class, line 36) — typeRef by 4: `src/world/poleLines.ts#contourEdges`, `src/world/poleLines.ts#contoursOf`, `src/world/poleLines.ts#PoleLines`, `src/world/lanelets.ts#LaneletGraph.junctionConnectors`
- `src/core/polyline.ts#Polyline.bbox` (getter, line 116) — call by 4: `src/render/signLayer.ts#ribbonBox`, `src/world/roads/furnitureSets.ts#furnitureFor`, `src/world/landscape.ts#footwayAt`, `src/world/landscape.ts#medianAt`
- `src/core/polyline.ts#Polyline.closestInto` (method, line 347) — call by 2: `src/world/landscape.ts#footwayAt`, `src/world/landscape.ts#medianAt`
- `src/core/polyline.ts#Polyline.closestPoint` (method, line 330) — call by 3: `src/editor/barriers.ts#onCarriageway`, `src/editor/barriers.ts#snapBarrierPoint`, `src/world/roads/derivedSigns.ts#signApproach`
- `src/core/polyline.ts#Polyline.closestPoint` (method, line 330) — read by 3: `src/editor/barriers.ts#onCarriageway`, `src/editor/barriers.ts#snapBarrierPoint`, `src/world/roads/derivedSigns.ts#signApproach`
- `src/core/polyline.ts#Polyline.cum` (getter, line 81) — call by 2: `src/editor/poleRoutes.ts#buildRouteGraph`, `src/world/poleLines.ts#nearestOnContour`
- `src/core/polyline.ts#Polyline.distanceTo` (method, line 359) — call by 2: `src/render/signs.ts#streetChain`, `src/world/landscape.ts#onAnyRoad`
- `src/core/polyline.ts#Polyline.fromPoints` (method, line 55) — call by 1: `src/world/poleLines.ts#contoursOf`
- `src/core/polyline.ts#Polyline.length` (getter, line 123) — call by 12: `src/editor/poleRoutes.ts#buildRouteGraph`, `src/editor/streetObjects.ts#commitPedestrianCrossing`, `src/editor/streetObjects.ts#commitUturn`, `src/world/signalPosts.ts#signalPostPlace`, …
- `src/core/polyline.ts#Polyline.n` (field, line 40) — read by 2: `src/editor/poleRoutes.ts#buildRouteGraph`, `src/world/poleLines.ts#contourEdges`
- `src/core/polyline.ts#Polyline.reversed` (method, line 214) — call by 2: `src/world/signalPosts.ts#signalPostPlace`, `src/world/roads/derivedSigns.ts#signsAtNode`
- `src/core/polyline.ts#Polyline.sampleAt` (method, line 152) — call by 13: `src/editor/poleRoutes.ts#buildRouteGraph`, `src/editor/barriers.ts#snapBarrierPoint`, `src/editor/streetObjects.ts#commitPedestrianCrossing`, `src/editor/streetObjects.ts#commitUturn`, …
- `src/core/polyline.ts#Polyline.xy` (field, line 38) — read by 3: `src/editor/poleRoutes.ts#buildRouteGraph`, `src/world/poleLines.ts#contourEdges`, `src/world/poleLines.ts#nearestOnContour`

### src/core/rng.ts

- `src/core/rng.ts#Rng` (class, line 6) — construct by 3: `src/render/streetFurnitureLayer.ts#furniturePlace`, `src/render/propGeometry.ts#grassTuftGeometry`, `src/render/grass.ts#grassClump`
- `src/core/rng.ts#Rng.float` (method, line 57) — call by 3: `src/render/streetFurnitureLayer.ts#furniturePlace`, `src/render/propGeometry.ts#grassTuftGeometry`, `src/render/grass.ts#grassClump`

### src/core/scalar.ts

- `src/core/scalar.ts#EPS` (variable, line 6) — read by 1: `src/world/poleLines.ts#nearestOnContour`

### src/core/units.ts

- `src/core/units.ts#kmh` (function, line 19) — call by 1: `src/world/roads/derivedSigns.ts#signsAtNode`
- `src/core/units.ts#m` (function, line 6) — call by 129: `src/editor/poles.ts#CLOSED_PAD`, `src/editor/poles.ts#computePoleRun.clear`, `src/editor/poles.ts#CORNER_TOLERANCE`, `src/editor/poles.ts#inLanding`, …
- `src/core/units.ts#perM` (function, line 9) — call by 2: `src/render/wind.ts#wireWindGlsl`, `src/render/wind.ts#installStructureSway`

### src/core/vec2.ts

- `src/core/vec2.ts#addScaled` (function, line 43) — call by 1: `src/world/signalPosts.ts#signalPostPlace`
- `src/core/vec2.ts#angleOf` (function, line 35) — call by 3: `src/world/signalPosts.ts#signalPostPlace`, `src/render/streetFurnitureLayer.ts#furniturePlace`, `src/render/utilities.ts#placePole`
- `src/core/vec2.ts#dist` (function, line 20) — call by 4: `src/editor/poles.ts#computePoleRun`, `src/editor/poles.ts#corners`, `src/editor/poles.ts#routeRun`, `src/editor/poleRoutes.ts#route`
- `src/core/vec2.ts#perp` (function, line 23) — call by 1: `src/world/signalPosts.ts#signalPostPlace`
- `src/core/vec2.ts#Vec2` (interface, line 4) — read by 46: `src/editor/poles.ts#commitPoleRunPoles`, `src/editor/poles.ts#computePoleRun.at`, `src/editor/poles.ts#corners`, `src/editor/poles.ts#inLanding`, …
- `src/core/vec2.ts#Vec2` (interface, line 4) — typeRef by 65: `src/editor/poles.ts#computePoleRun`, `src/editor/poles.ts#computePoleRun.at`, `src/editor/poles.ts#corners`, `src/editor/poles.ts#endOff`, …

### src/editor/commit.ts

- `src/editor/commit.ts#splitSegment` (function, line 662) — call by 2: `src/editor/streetObjects.ts#commitPedestrianCrossing`, `src/editor/streetObjects.ts#commitUturn`

### src/editor/snap.ts

- `src/editor/snap.ts#ribbonsNear` (function, line 74) — call by 3: `src/editor/barriers.ts#onCarriageway`, `src/editor/barriers.ts#snapBarrierPoint`, `src/editor/streetscapeTool.ts#itemsAround`

### src/lab/catalog/layers.ts

- `src/lab/catalog/layers.ts#signalItems` (function, line 118) — implementation by 3: `src/render/signals.ts#createSignalHeads.junctionOf`, `src/render/signals.ts#createSignalHeads.placeAll`, `src/render/signals.ts#createSignalHeads.sync`

### src/lab/catalog/layout.ts

- `src/lab/catalog/layout.ts#layoutItems.flat` (function, line 166) — indirectCall by 1: `src/render/utilities.ts#poleGroundAt`

### src/main.ts

- `src/main.ts#barrierTool` (variable, line 515) — implementation by 5: `src/editor/barriers.ts#BarrierTool.down`, `src/editor/barriers.ts#BarrierTool.finish`, `src/editor/barriers.ts#BarrierTool.key`, `src/editor/barriers.ts#BarrierTool.move`, …
- `src/main.ts#poleTool` (variable, line 488) — implementation by 7: `src/editor/poles.ts#PoleTool.down`, `src/editor/poles.ts#PoleTool.down.remove`, `src/editor/poles.ts#PoleTool.marks`, `src/editor/poles.ts#PoleTool.move`, …
- `src/main.ts#streetscapeTool` (variable, line 502) — implementation by 4: `src/editor/streetscapeTool.ts#StreetscapeTool.down`, `src/editor/streetscapeTool.ts#StreetscapeTool.move`, `src/editor/streetscapeTool.ts#StreetscapeTool.reach`, `src/editor/streetscapeTool.ts#StreetscapeTool.up`

### src/render/cellInstances.ts

- `src/render/cellInstances.ts#bufferSize` (variable, line 304) — read by 1: `src/render/cellInstances.ts#watchScreen`
- `src/render/cellInstances.ts#CellInstances` (interface, line 124) — call by 4: `src/render/propCells.ts#createPropLayer.cull`, `src/render/propCells.ts#createPropLayer.dispose`, `src/render/propCells.ts#createPropLayer.setMap`, `src/render/propCells.ts#createPropLayer.show`
- `src/render/cellInstances.ts#CellInstances` (interface, line 124) — read by 1: `src/render/propCells.ts#createPropLayer`
- `src/render/cellInstances.ts#CellInstances` (interface, line 124) — typeRef by 1: `src/render/propCells.ts#createPropLayer`
- `src/render/cellInstances.ts#createCellInstances` (function, line 447) — call by 1: `src/render/propCells.ts#createPropLayer`
- `src/render/cellInstances.ts#createCellInstances` (function, line 447) — read by 1: `src/render/cellInstances.ts#createCellInstances.setMap`
- `src/render/cellInstances.ts#createCellInstances` (function, line 447) — write by 1: `src/render/cellInstances.ts#createCellInstances.setMap`
- `src/render/cellInstances.ts#createCellInstances.cull` (method, line 1067) — implementation by 1: `src/render/propCells.ts#createPropLayer.cull`
- `src/render/cellInstances.ts#createCellInstances.dispose` (method, line 1123) — implementation by 1: `src/render/propCells.ts#createPropLayer.dispose`
- `src/render/cellInstances.ts#createCellInstances.update` (method, line 1010) — implementation by 1: `src/render/propCells.ts#createPropLayer.show`
- `src/render/cellInstances.ts#dressedMesh` (function, line 243) — call by 2: `src/render/cellInstances.ts#batchMesh`, `src/render/cellInstances.ts#warmMesh`
- `src/render/cellInstances.ts#HYSTERESIS` (variable, line 281) — read by 2: `src/render/propCells.ts#createPropLayer.show`, `src/render/propCells.ts#nearSquareOf`
- `src/render/cellInstances.ts#KeptFrustum` (type, line 61) — read by 1: `src/render/poleCells.ts#PoleCells.look`
- `src/render/cellInstances.ts#KeptFrustum` (type, line 61) — typeRef by 1: `src/render/poleCells.ts#PoleCells.look`
- `src/render/cellInstances.ts#Kind` (interface, line 401) — read by 1: `src/render/cellInstances.ts#createCellInstances.setMap`
- `src/render/cellInstances.ts#Kind` (interface, line 401) — write by 1: `src/render/cellInstances.ts#createCellInstances.setMap`
- `src/render/cellInstances.ts#KindLevels` (interface, line 70) — read by 3: `src/render/propCells.ts#createPropLayer`, `src/render/propCells.ts#createPropLayer.note`, `src/render/cellInstances.ts#createCellInstances.setMap`
- `src/render/cellInstances.ts#KindMesh` (interface, line 36) — call by 1: `src/render/cellInstances.ts#batchMesh`
- `src/render/cellInstances.ts#KindMesh` (interface, line 36) — read by 3: `src/render/propCells.ts#createPropLayer.batches`, `src/render/cellInstances.ts#createCellInstances.setMap`, `src/render/cellInstances.ts#warmMesh`
- `src/render/cellInstances.ts#KindMesh` (interface, line 36) — typeRef by 3: `src/render/propCells.ts#createPropLayer.batches`, `src/render/cellInstances.ts#batchMesh`, `src/render/cellInstances.ts#warmMesh`
- `src/render/cellInstances.ts#KindSpec` (interface, line 88) — read by 5: `src/render/propCells.ts#createPropLayer`, `src/render/propCells.ts#createPropLayer.batches`, `src/render/propCells.ts#createPropLayer.note`, `src/render/cellInstances.ts#createCellInstances.setMap`, …
- `src/render/cellInstances.ts#KindSpec` (interface, line 88) — typeRef by 1: `src/render/propCells.ts#PropLayerSpec`
- `src/render/cellInstances.ts#letGo` (function, line 236) — call by 1: `src/render/cellInstances.ts#releaseBatchMesh`
- `src/render/cellInstances.ts#MeshState` (interface, line 389) — read by 1: `src/render/cellInstances.ts#createCellInstances.setMap`
- `src/render/cellInstances.ts#Pack` (interface, line 111) — read by 5: `src/render/propCells.ts#createPropLayer.batches`, `src/render/propCells.ts#createPropLayer.batches.boundsOf`, `src/render/propCells.ts#createPropLayer.note`, `src/render/propCells.ts#packBytes`, …
- `src/render/cellInstances.ts#Pack` (interface, line 111) — typeRef by 9: `src/render/propCells.ts#createPropLayer.batches`, `src/render/propCells.ts#createPropLayer.batches.boundsOf`, `src/render/propCells.ts#createPropLayer.batches.make`, `src/render/propCells.ts#createPropLayer.note`, …
- `src/render/cellInstances.ts#posedGeometry` (function, line 221) — call by 2: `src/render/cellInstances.ts#batchMesh`, `src/render/cellInstances.ts#warmMesh`
- `src/render/cellInstances.ts#screen` (variable, line 291) — read by 1: `src/render/cellInstances.ts#watchScreen`
- `src/render/cellInstances.ts#screen` (variable, line 291) — write by 1: `src/render/cellInstances.ts#watchScreen`
- `src/render/cellInstances.ts#screenHalf` (function, line 307) — call by 1: `src/render/propCells.ts#createPropLayer.cull`
- `src/render/cellInstances.ts#SlotFormat` (interface, line 96) — read by 3: `src/render/propCells.ts#createPropLayer.batches`, `src/render/propCells.ts#within`, `src/render/cellInstances.ts#warmMesh`
- `src/render/cellInstances.ts#SlotFormat` (interface, line 96) — typeRef by 2: `src/render/cellInstances.ts#batchMesh`, `src/render/cellInstances.ts#warmMesh`

### src/render/gardens.ts

- `src/render/gardens.ts#createGardenCells` (function, line 246) — implementation by 2: `src/render/propCells.ts#createPropLayer`, `src/render/propCells.ts#createPropLayer.batches.make`
- `src/render/gardens.ts#gardenKinds.kind` (function, line 171) — callbackAssign by 1: `src/render/cellInstances.ts#batchMesh`
- `src/render/gardens.ts#gardenKinds.kind` (function, line 171) — implementation by 1: `src/render/cellInstances.ts#batchMesh`

### src/render/gpu/backendChoice.ts

- `src/render/gpu/backendChoice.ts#chosenBackend` (function, line 5) — call by 1: `src/render/signs.ts#sheetTexture`

### src/render/instanceRuns.ts

- `src/render/instanceRuns.ts#createInstanceRuns` (function, line 201) — call by 3: `src/render/signLayer.ts#createSignLayer`, `src/render/streetFurnitureLayer.ts#createStreetFurniture`, `src/render/barriers.ts#createBarriers`
- `src/render/instanceRuns.ts#createInstanceRuns.cull` (method, line 214) — implementation by 3: `src/render/signLayer.ts#createSignLayer.cull`, `src/render/streetFurnitureLayer.ts#createStreetFurniture.cull`, `src/render/barriers.ts#createBarriers.cull`
- `src/render/instanceRuns.ts#createInstanceRuns.dispose` (method, line 215) — implementation by 3: `src/render/signLayer.ts#createSignLayer.dispose`, `src/render/streetFurnitureLayer.ts#createStreetFurniture.dispose`, `src/render/barriers.ts#createBarriers.dispose`
- `src/render/instanceRuns.ts#createInstanceRuns.update` (method, line 209) — implementation by 3: `src/render/signLayer.ts#createSignLayer.sync`, `src/render/streetFurnitureLayer.ts#createStreetFurniture.sync`, `src/render/barriers.ts#createBarriers.sync`
- `src/render/instanceRuns.ts#InstanceRuns` (interface, line 170) — call by 9: `src/render/signLayer.ts#createSignLayer.cull`, `src/render/signLayer.ts#createSignLayer.dispose`, `src/render/signLayer.ts#createSignLayer.sync`, `src/render/streetFurnitureLayer.ts#createStreetFurniture.cull`, …
- `src/render/instanceRuns.ts#InstanceRuns` (interface, line 170) — read by 3: `src/render/signLayer.ts#createSignLayer`, `src/render/streetFurnitureLayer.ts#createStreetFurniture`, `src/render/barriers.ts#createBarriers`
- `src/render/instanceRuns.ts#kindOf` (function, line 185) — call by 1: `src/render/instanceRuns.ts#warmMeshes`
- `src/render/instanceRuns.ts#kindOf` (function, line 185) — read by 1: `src/render/streetFurnitureLayer.ts#createStreetFurniture`
- `src/render/instanceRuns.ts#NEAR_CODE_SPAN` (variable, line 36) — read by 1: `src/render/instanceRuns.ts#nearCode`
- `src/render/instanceRuns.ts#NEAR_SQUARE` (variable, line 33) — read by 1: `src/render/propCells.ts#lookProps`
- `src/render/instanceRuns.ts#NEAR_SQUARE` (variable, line 33) — write by 1: `src/render/propCells.ts#lookProps`
- `src/render/instanceRuns.ts#PoseFormat` (type, line 16) — typeRef by 1: `src/render/instanceRuns.ts#warmMeshes`
- `src/render/instanceRuns.ts#Run` (type, line 100) — typeRef by 10: `src/render/signLayer.ts#createSignLayer.letGo`, `src/render/signLayer.ts#createSignLayer.stand`, `src/render/signLayer.ts#createSignLayer.sync`, `src/render/signLayer.ts#createSignLayer.take`, …
- `src/render/instanceRuns.ts#RunBuilder` (class, line 111) — construct by 4: `src/render/signLayer.ts#createSignLayer.standOf`, `src/render/streetFurnitureLayer.ts#furnitureAbout`, `src/render/streetFurnitureLayer.ts#meadowProduct`, `src/render/barriers.ts#BarrierProduct.compute`
- `src/render/instanceRuns.ts#RunBuilder` (class, line 111) — typeRef by 3: `src/render/streetFurnitureLayer.ts#meadowInto`, `src/render/streetFurnitureLayer.ts#placeInto`, `src/render/barriers.ts#barrierInto`
- `src/render/instanceRuns.ts#RunBuilder.build` (method, line 137) — call by 4: `src/render/signLayer.ts#createSignLayer.standOf`, `src/render/streetFurnitureLayer.ts#furnitureAbout`, `src/render/streetFurnitureLayer.ts#meadowProduct`, `src/render/barriers.ts#BarrierProduct.compute`
- `src/render/instanceRuns.ts#RunBuilder.turned` (method, line 132) — call by 4: `src/render/signLayer.ts#createSignLayer.standOf`, `src/render/streetFurnitureLayer.ts#meadowInto.add`, `src/render/streetFurnitureLayer.ts#placeInto`, `src/render/barriers.ts#barrierInto`
- `src/render/instanceRuns.ts#RunStore` (interface, line 69) — read by 1: `src/render/instanceRuns.ts#warmMeshes`
- `src/render/instanceRuns.ts#RunStore` (interface, line 69) — typeRef by 10: `src/render/signLayer.ts#POST_STORES`, `src/render/streetFurnitureLayer.ts#furnitureAbout`, `src/render/streetFurnitureLayer.ts#furnitureStores`, `src/render/streetFurnitureLayer.ts#furnitureStores.plant`, …
- `src/render/instanceRuns.ts#sameRun` (function, line 152) — call by 2: `src/render/streetFurnitureLayer.ts#meadowProduct`, `src/render/barriers.ts#BarrierProduct.equals`
- `src/render/instanceRuns.ts#YAW_POSE` (variable, line 42) — read by 4: `src/render/propCells.ts#createPropLayer`, `src/render/propCells.ts#createPropLayer.batches`, `src/render/propCells.ts#within`, `src/render/instanceRuns.ts#warmMeshes`

### src/render/lowPolyTrees.ts

- `src/render/lowPolyTrees.ts#LowPolyKind` (type, line 31) — typeRef by 1: `src/render/propGeometry.ts#TREE_KIND`
- `src/render/lowPolyTrees.ts#LowPolyParts` (interface, line 34) — typeRef by 2: `src/render/propGeometry.ts#bushParts`, `src/render/propGeometry.ts#treeParts`
- `src/render/lowPolyTrees.ts#lowPolyTreeParts` (function, line 461) — call by 2: `src/render/propGeometry.ts#bushParts`, `src/render/propGeometry.ts#treeParts`

### src/render/mesh/cylinder.ts

- `src/render/mesh/cylinder.ts#cylinderWith` (function, line 16) — call by 9: `src/render/signs.ts#POST`, `src/render/signals.ts#createSignalHeads`, `src/render/utilities.ts#madeUtilityParts`, `src/render/utilities.ts#mastShape`, …

### src/render/mesh/gpuOnly.ts

- `src/render/mesh/gpuOnly.ts#gpuOnlyAttributes` (function, line 22) — call by 2: `src/render/poleCells.ts#PoleCells.derive`, `src/render/propCells.ts#createPropLayer.batches.own`

### src/render/mesh/hiddenFaces.ts

- `src/render/mesh/hiddenFaces.ts#dropHiddenFaces` (function, line 22) — call by 1: `src/render/propGeometry.ts#merge`

### src/render/plantImpostors.ts

- `src/render/plantImpostors.ts#createPlantImpostors.attach` (method, line 444) — implementation by 2: `src/render/streetFurnitureLayer.ts#furnitureStores.plant`, `src/render/propCells.ts#createPropLayer.batches`
- `src/render/plantImpostors.ts#createPlantImpostors.ready` (method, line 443) — implementation by 2: `src/render/streetFurnitureLayer.ts#furnitureStores.plant`, `src/render/propCells.ts#createPropLayer.show`
- `src/render/plantImpostors.ts#createPlantImpostors.setView` (method, line 439) — implementation by 2: `src/render/streetFurnitureLayer.ts#furnitureStores.plant`, `src/render/propCells.ts#createPropLayer.cull`
- `src/render/plantImpostors.ts#PICTURE_PX` (variable, line 467) — read by 2: `src/render/streetFurnitureLayer.ts#furnitureStores.plant`, `src/render/propCells.ts#nearSquareOf`
- `src/render/plantImpostors.ts#PlantImpostors` (interface, line 255) — call by 3: `src/render/streetFurnitureLayer.ts#furnitureStores.plant`, `src/render/propCells.ts#createPropLayer.cull`, `src/render/propCells.ts#createPropLayer.show`
- `src/render/plantImpostors.ts#PlantImpostors` (interface, line 255) — read by 3: `src/render/streetFurnitureLayer.ts#furnitureStores.plant`, `src/render/propCells.ts#createPropLayer.batches`, `src/render/propCells.ts#createPropLayer.note`
- `src/render/plantImpostors.ts#PlantImpostors` (interface, line 255) — reference by 2: `src/render/streetFurnitureLayer.ts#furnitureStores.plant`, `src/render/propCells.ts#createPropLayer.batches`
- `src/render/plantImpostors.ts#PlantImpostors` (interface, line 255) — typeRef by 1: `src/render/propCells.ts#PropLayerSpec`
- `src/render/plantImpostors.ts#sceneryPictures` (function, line 487) — call by 1: `src/render/streetFurnitureLayer.ts#createStreetFurniture`
- `src/render/plantImpostors.ts#SceneryPictures` (interface, line 470) — call by 3: `src/render/streetFurnitureLayer.ts#createStreetFurniture`, `src/render/streetFurnitureLayer.ts#createStreetFurniture.dispose`, `src/render/streetFurnitureLayer.ts#furnitureStores`
- `src/render/plantImpostors.ts#SceneryPictures` (interface, line 470) — read by 2: `src/render/streetFurnitureLayer.ts#createStreetFurniture`, `src/render/streetFurnitureLayer.ts#furnitureStores`
- `src/render/plantImpostors.ts#SceneryPictures` (interface, line 470) — typeRef by 1: `src/render/streetFurnitureLayer.ts#furnitureStores`
- `src/render/plantImpostors.ts#sceneryPictures.bushAt` (method, line 502) — implementation by 2: `src/render/streetFurnitureLayer.ts#createStreetFurniture`, `src/render/streetFurnitureLayer.ts#furnitureStores`
- `src/render/plantImpostors.ts#sceneryPictures.release` (method, line 503) — implementation by 1: `src/render/streetFurnitureLayer.ts#createStreetFurniture.dispose`
- `src/render/plantImpostors.ts#sceneryPictures.treeAt` (method, line 501) — implementation by 2: `src/render/streetFurnitureLayer.ts#createStreetFurniture`, `src/render/streetFurnitureLayer.ts#furnitureStores`

### src/render/renderer.ts

- `src/render/renderer.ts#createSceneRenderer.deckHeight` (function, line 1257) — indirectCall by 1: `src/render/signals.ts#createSignalHeads.junctionOf`
- `src/render/renderer.ts#createSceneRenderer.warmRoadShaders` (function, line 2506) — indirectCall by 1: `src/render/utilities.ts#poleGroundAt`

### src/render/scenery.ts

- `src/render/scenery.ts#foliageTint` (function, line 373) — call by 1: `src/render/streetFurnitureLayer.ts#furniturePlace`
- `src/render/scenery.ts#LAMP_BEAM_RADIUS` (variable, line 66) — read by 2: `src/render/streetFurnitureLayer.ts#furniturePlace`, `src/render/utilities.ts#placePole`
- `src/render/scenery.ts#LAMP_POOL_RADIUS` (variable, line 61) — read by 2: `src/render/streetFurnitureLayer.ts#furniturePlace`, `src/render/utilities.ts#placePole`
- `src/render/scenery.ts#POOL_ALONG` (variable, line 63) — read by 1: `src/render/streetFurnitureLayer.ts#furniturePlace`
- `src/render/scenery.ts#SceneryKit` (interface, line 131) — read by 5: `src/render/streetFurnitureLayer.ts#createStreetFurniture`, `src/render/streetFurnitureLayer.ts#furnitureProduct`, `src/render/streetFurnitureLayer.ts#furnitureStores`, `src/render/streetFurnitureLayer.ts#MEADOW_STORES`, …
- `src/render/scenery.ts#SceneryKit` (interface, line 131) — typeRef by 10: `src/render/poleLayer.ts#buildPolePreview`, `src/render/poleLayer.ts#buildUtilities`, `src/render/poleLayer.ts#PoleLayer.constructor`, `src/render/poleCells.ts#PoleCells.constructor`, …
- `src/render/scenery.ts#SHADOW_REACH` (variable, line 363) — read by 1: `src/render/streetFurnitureLayer.ts#createStreetFurniture`

### src/render/shadowCasters.ts

- `src/render/shadowCasters.ts#castBySize` (function, line 26) — call by 4: `src/render/poleLayer.ts#PartStore.meshOf`, `src/render/poleLayer.ts#WireStore.makeMesh`, `src/render/signals.ts#SignalBatch.makeMesh`, `src/render/propCells.ts#createPropLayer.batches`
- `src/render/shadowCasters.ts#casterTexel` (variable, line 22) — read by 1: `src/render/signals.ts#createSignalHeads`

### src/render/stillTree.ts

- `src/render/stillTree.ts#holdStill` (function, line 13) — call by 1: `src/render/signals.ts#createSignalHeads`

### src/render/wind.ts

- `src/render/wind.ts#markSway` (function, line 103) — call by 1: `src/render/wind.ts#installStructureSway`
- `src/render/wind.ts#windUniforms` (variable, line 13) — read by 2: `src/render/utilities.ts#wireMaterials`, `src/render/wind.ts#installStructureSway`
- `src/render/wind.ts#WIRE_WIND_UNIFORMS_GLSL` (variable, line 144) — read by 1: `src/render/utilities.ts#wireMaterials`

### src/sim/host/protocol.ts

- `src/sim/host/protocol.ts#SignalLayout` (interface, line 70) — read by 1: `src/render/signals.ts#createSignalHeads.junctionOf`

### src/sim/host/simClient.ts

- `src/sim/host/simClient.ts#SignalFeed` (interface, line 58) — call by 3: `src/render/signals.ts#createSignalHeads.junctionOf`, `src/render/signals.ts#createSignalHeads.placeAll`, `src/render/signals.ts#createSignalHeads.sync`
- `src/sim/host/simClient.ts#SignalFeed` (interface, line 58) — read by 1: `src/render/signals.ts#createSignalHeads.sync`
- `src/sim/host/simClient.ts#SignalFeed` (interface, line 58) — typeRef by 4: `src/render/signals.ts#createSignalHeads.junctionOf`, `src/render/signals.ts#createSignalHeads.placeAll`, `src/render/signals.ts#createSignalHeads.placeSome`, `src/render/signals.ts#SignalHeads`
- `src/sim/host/simClient.ts#SimClient.signals` (field, line 411) — implementation by 3: `src/render/signals.ts#createSignalHeads.junctionOf`, `src/render/signals.ts#createSignalHeads.placeAll`, `src/render/signals.ts#createSignalHeads.sync`

### src/sim/signals/query.ts

- `src/sim/signals/query.ts#SignalState` (type, line 5) — typeRef by 2: `src/render/signals.ts#createSignalHeads`, `src/render/signals.ts#Head`

### src/world/approach.ts

- `src/world/approach.ts#CROSSWALK_DEPTH` (variable, line 7) — read by 4: `src/editor/poles.ts#inLanding`, `src/editor/streetObjects.ts#commitPedestrianCrossing`, `src/world/landscape.ts#onCrossingAccess`, `src/world/landscape.ts#crossingAccessTester`
- `src/world/approach.ts#crosswalkDistance` (function, line 14) — call by 1: `src/world/signalPosts.ts#signalPostPlace`
- `src/world/approach.ts#MIN_LINK_LENGTH` (variable, line 26) — read by 2: `src/editor/streetObjects.ts#commitPedestrianCrossing`, `src/editor/streetObjects.ts#commitUturn`
- `src/world/approach.ts#STOP_BAR_SETBACK` (variable, line 9) — read by 1: `src/editor/streetObjects.ts#commitPedestrianCrossing`

### src/world/buildings/validate.ts

- `src/world/buildings/validate.ts#ROAD_CLEARANCE` (variable, line 117) — read by 1: `src/editor/barriers.ts#snapBarrierPoint`
- `src/world/buildings/validate.ts#touchesRoad` (function, line 256) — call by 1: `src/editor/barriers.ts#barrierProblem`

### src/world/cells.ts

- `src/world/cells.ts#CELL` (variable, line 9) — read by 4: `src/editor/poles.ts#poleRunPlanner.tileAt`, `src/editor/poles.ts#routeRun`, `src/editor/poles.ts#TILE`, `src/render/propCells.ts#PROP_CELL`
- `src/world/cells.ts#cellAt` (function, line 23) — call by 1: `src/world/poleLines.ts#poleLinesAt`
- `src/world/cells.ts#CellFiling` (class, line 60) — construct by 3: `src/render/signLayer.ts#SignProduct.filing`, `src/render/streetFurnitureLayer.ts#PlacedProduct.filing`, `src/render/barriers.ts#BarrierProduct.filing`
- `src/world/cells.ts#CellFiling.epoch` (field, line 69) — read by 1: `src/world/poleLines.ts#PoleLineState.letGoStale`
- `src/world/cells.ts#CellFiling.file` (method, line 76) — call by 3: `src/render/signLayer.ts#SignProduct.changed`, `src/render/streetFurnitureLayer.ts#PlacedProduct.changed`, `src/render/barriers.ts#BarrierProduct.changed`
- `src/world/cells.ts#CellFiling.ids` (method, line 126) — call by 3: `src/render/signLayer.ts#SignProduct.allKeys`, `src/render/streetFurnitureLayer.ts#PlacedProduct.allKeys`, `src/render/barriers.ts#BarrierProduct.allKeys`
- `src/world/cells.ts#CellFiling.idsMeeting` (method, line 131) — call by 5: `src/editor/poleRoutes.ts#buildRouteGraph`, `src/world/poleLines.ts#localRings`, `src/world/poleLines.ts#markPlates`, `src/world/poleLines.ts#poleLinesOf`, …
- `src/world/cells.ts#CellFiling.idsMeetingAny` (method, line 169) — call by 4: `src/render/signLayer.ts#SignProduct.dirty`, `src/render/streetFurnitureLayer.ts#PlacedProduct.dirty`, `src/render/barriers.ts#BarrierProduct.dirty`, `src/world/doc.ts#RoadDoc.barriersInAny`
- `src/world/cells.ts#CellFiling.rectOf` (method, line 120) — call by 1: `src/world/poleLines.ts#markPlates`
- `src/world/cells.ts#CellFiling.remove` (method, line 107) — call by 3: `src/render/signLayer.ts#SignProduct.changed`, `src/render/streetFurnitureLayer.ts#PlacedProduct.changed`, `src/render/barriers.ts#BarrierProduct.changed`
- `src/world/cells.ts#CellFiling.someAt` (method, line 157) — call by 1: `src/world/poleLines.ts#insideLevels`
- `src/world/cells.ts#CellFiling.stamp` (method, line 193) — call by 1: `src/world/poleLines.ts#regionStamp`
- `src/world/cells.ts#cellRect` (function, line 17) — call by 1: `src/world/poleLines.ts#poleLinesAt`
- `src/world/cells.ts#grow` (function, line 33) — call by 4: `src/world/poleLines.ts#poleLinesOf`, `src/world/poleLines.ts#regionStamp`, `src/render/streetFurnitureLayer.ts#furnitureAbout`, `src/render/streetFurnitureLayer.ts#PlacedProduct.dirty`

### src/world/changes.ts

- `src/world/changes.ts#Change` (interface, line 35) — read by 5: `src/render/poleLayer.ts#PoleLayer.follow`, `src/render/signLayer.ts#SignProduct.dirty`, `src/render/signals.ts#createSignalHeads.sync`, `src/render/streetFurnitureLayer.ts#PlacedProduct.dirty`, …
- `src/world/changes.ts#Change` (interface, line 35) — typeRef by 4: `src/render/poleLayer.ts#PoleLayer.follow`, `src/render/signLayer.ts#SignProduct.dirty`, `src/render/streetFurnitureLayer.ts#PlacedProduct.dirty`, `src/render/barriers.ts#BarrierProduct.dirty`
- `src/world/changes.ts#ChangeJournal` (class, line 156) — typeRef by 5: `src/render/poleCells.ts#PoleCells.follow`, `src/render/signLayer.ts#SignSource`, `src/render/streetFurnitureLayer.ts#FurnitureSource`, `src/render/barriers.ts#BarrierSource`, …
- `src/world/changes.ts#ChangeJournal.serialOf` (method, line 203) — call by 1: `src/editor/poles.ts#planPoleRun`
- `src/world/changes.ts#ChangeKind` (type, line 27) — typeRef by 5: `src/render/signLayer.ts#SignProduct.kinds`, `src/render/streetFurnitureLayer.ts#PlacedProduct.constructor`, `src/render/streetFurnitureLayer.ts#PlacedProduct.kinds`, `src/render/barriers.ts#BarrierProduct.kinds`, …
- `src/world/changes.ts#ChangeRect` (type, line 4) — typeRef by 34: `src/editor/poles.ts#poleRunPlanner`, `src/editor/poles.ts#poleRunPlanner.inner`, `src/editor/poles.ts#poleRunPlanner.tileAt`, `src/editor/poles.ts#routeGraph`, …

### src/world/derived.ts

- `src/world/derived.ts#Derived` (class, line 71) — construct by 4: `src/render/signLayer.ts#signProduct`, `src/render/streetFurnitureLayer.ts#furnitureProduct`, `src/render/streetFurnitureLayer.ts#meadowProduct`, `src/render/barriers.ts#createBarriers`
- `src/world/derived.ts#Derived` (class, line 71) — typeRef by 3: `src/render/signLayer.ts#signProduct`, `src/render/streetFurnitureLayer.ts#furnitureProduct`, `src/render/streetFurnitureLayer.ts#meadowProduct`
- `src/world/derived.ts#Derived.get` (method, line 80) — call by 3: `src/render/signLayer.ts#createSignLayer.sync`, `src/render/streetFurnitureLayer.ts#createStreetFurniture.sync`, `src/render/barriers.ts#createBarriers.sync`
- `src/world/derived.ts#Derived.refresh` (method, line 93) — call by 3: `src/render/signLayer.ts#createSignLayer.sync`, `src/render/streetFurnitureLayer.ts#createStreetFurniture.sync`, `src/render/barriers.ts#createBarriers.sync`
- `src/world/derived.ts#Derived.work` (field, line 74) — read by 3: `src/render/signLayer.ts#createSignLayer.sync`, `src/render/streetFurnitureLayer.ts#createStreetFurniture.sync`, `src/render/barriers.ts#createBarriers.sync`
- `src/world/derived.ts#DerivedSpec` (interface, line 40) — implements by 3: `src/render/signLayer.ts#SignProduct`, `src/render/streetFurnitureLayer.ts#PlacedProduct`, `src/render/barriers.ts#BarrierProduct`
- `src/world/derived.ts#DerivedWork` (interface, line 60) — read by 3: `src/render/signLayer.ts#createSignLayer.sync`, `src/render/streetFurnitureLayer.ts#createStreetFurniture.sync`, `src/render/barriers.ts#createBarriers.sync`
- `src/world/derived.ts#JournalCursor` (class, line 6) — construct by 1: `src/render/signals.ts#createSignalHeads`
- `src/world/derived.ts#JournalCursor.read` (method, line 19) — call by 1: `src/render/signals.ts#createSignalHeads.sync`

### src/world/doc.ts

- `src/world/doc.ts#NodeCrossingKind` (type, line 118) — typeRef by 1: `src/world/doc.ts#RoadDoc.setNodeCrossing`
- `src/world/doc.ts#RoadDoc` (class, line 153) — typeRef by 18: `src/editor/poles.ts#commitPoleRun`, `src/editor/poles.ts#commitPoleRunPoles`, `src/editor/poles.ts#computePoleRun`, `src/editor/poles.ts#lastPlan`, …
- `src/world/doc.ts#RoadDoc.addLandscape` (method, line 576) — call by 2: `src/editor/streetscapeTool.ts#StreetscapeTool.down`, `src/editor/streetscapeTool.ts#StreetscapeTool.up`
- `src/world/doc.ts#RoadDoc.addPole` (method, line 440) — call by 1: `src/editor/poles.ts#commitPoleRunPoles`
- `src/world/doc.ts#RoadDoc.addPoleSpan` (method, line 498) — call by 1: `src/editor/poles.ts#commitPoleRunPoles`
- `src/world/doc.ts#RoadDoc.barrierCells` (field, line 172) — read by 1: `src/world/doc.ts#RoadDoc.barriersInAny`
- `src/world/doc.ts#RoadDoc.barrierMap` (field, line 169) — read by 1: `src/world/doc.ts#RoadDoc.barriersInAny`
- `src/world/doc.ts#RoadDoc.barriers` (getter, line 170) — call by 2: `src/render/barriers.ts#BarrierProduct.allKeys`, `src/render/barriers.ts#BarrierProduct.compute`
- `src/world/doc.ts#RoadDoc.changes` (field, line 262) — read by 5: `src/editor/poles.ts#planPoleRun`, `src/render/signLayer.ts#createSignLayer.sync`, `src/render/signals.ts#createSignalHeads.sync`, `src/render/streetFurnitureLayer.ts#createStreetFurniture.sync`, …
- `src/world/doc.ts#RoadDoc.editableNode` (method, line 691) — call by 2: `src/world/doc.ts#RoadDoc.setNodeUturn`, `src/world/doc.ts#RoadDoc.setNodeCrossing`
- `src/world/doc.ts#RoadDoc.fork` (method, line 1827) — call by 2: `src/editor/streetObjects.ts#commitPedestrianCrossing`, `src/editor/streetObjects.ts#commitUturn`
- `src/world/doc.ts#RoadDoc.landscape` (getter, line 178) — call by 7: `src/world/streetFurniture.ts#streetFurniture`, `src/render/signLayer.ts#itemSigns`, `src/render/signLayer.ts#SignProduct.allKeys`, `src/render/signLayer.ts#SignProduct.dirty`, …
- `src/world/doc.ts#RoadDoc.landscapeAt` (field, line 615) — callbackAssign by 1: `src/world/doc.ts#RoadDoc.landscapeInAny`
- `src/world/doc.ts#RoadDoc.landscapeAt` (field, line 615) — implementation by 1: `src/world/doc.ts#RoadDoc.landscapeInAny`
- `src/world/doc.ts#RoadDoc.landscapeAt` (field, line 615) — read by 1: `src/world/doc.ts#RoadDoc.landscapeInAny`
- `src/world/doc.ts#RoadDoc.landscapeCells` (field, line 180) — read by 1: `src/world/doc.ts#RoadDoc.landscapeInAny`
- `src/world/doc.ts#RoadDoc.landscapeIn` (method, line 605) — call by 3: `src/render/streetFurnitureLayer.ts#furnitureAbout`, `src/world/roads/furnitureSets.ts#furnitureFor`, `src/editor/streetscapeTool.ts#itemsAround`
- `src/world/doc.ts#RoadDoc.landscapeMap` (field, line 177) — read by 1: `src/world/doc.ts#RoadDoc.landscapeInAny`
- `src/world/doc.ts#RoadDoc.markingStyle` (field, line 204) — read by 1: `src/world/roads/derivedSigns.ts#signsAtNode`
- `src/world/doc.ts#RoadDoc.markNode` (method, line 1407) — call by 2: `src/world/doc.ts#RoadDoc.setNodeUturn`, `src/world/doc.ts#RoadDoc.setNodeCrossing`
- `src/world/doc.ts#RoadDoc.node` (method, line 416) — call by 12: `src/world/signalPosts.ts#signalPostPlace`, `src/world/signalPosts.ts#signalPosts`, `src/render/signs.ts#streetChain.dirAt`, `src/render/signs.ts#streetChain.walk`, …
- `src/world/doc.ts#RoadDoc.nodes` (field, line 154) — read by 2: `src/render/signLayer.ts#SignProduct.allKeys`, `src/world/landscape.ts#crossingAccessesNow`
- `src/world/doc.ts#RoadDoc.nodeToWrite` (method, line 1953) — call by 2: `src/world/doc.ts#RoadDoc.setNodeUturn`, `src/world/doc.ts#RoadDoc.setNodeCrossing`
- `src/world/doc.ts#RoadDoc.poleNear` (method, line 671) — call by 4: `src/editor/poles.ts#computePoleRun`, `src/editor/poles.ts#PoleTool.down`, `src/editor/poles.ts#PoleTool.marks`, `src/editor/poles.ts#snapOn`
- `src/world/doc.ts#RoadDoc.poles` (getter, line 164) — call by 3: `src/render/poleLayer.ts#docPoles`, `src/render/poleCells.ts#PoleCells.build`, `src/world/doc.ts#RoadDoc.pole`
- `src/world/doc.ts#RoadDoc.polesIn` (method, line 450) — call by 3: `src/render/poleLayer.ts#PoleLayer.follow`, `src/render/poleCells.ts#PoleCells.build`, `src/world/roads/furnitureSets.ts#furnitureFor`
- `src/world/doc.ts#RoadDoc.poleSpans` (getter, line 165) — call by 1: `src/render/poleLayer.ts#docPoles`
- `src/world/doc.ts#RoadDoc.removeLandscape` (method, line 596) — call by 1: `src/editor/streetscapeTool.ts#StreetscapeTool.down`
- `src/world/doc.ts#RoadDoc.removePole` (method, line 654) — call by 1: `src/editor/poles.ts#PoleTool.down.remove`
- `src/world/doc.ts#RoadDoc.requireNode` (method, line 424) — call by 2: `src/editor/streetObjects.ts#commitPedestrianCrossing`, `src/editor/streetObjects.ts#commitUturn`
- `src/world/doc.ts#RoadDoc.requireSegment` (method, line 430) — call by 5: `src/editor/streetObjects.ts#commitPedestrianCrossing`, `src/editor/streetObjects.ts#commitUturn`, `src/world/roads/furnitureSets.ts#furnitureFor`, `src/world/landscape.ts#snapLandscape`, …
- `src/world/doc.ts#RoadDoc.segment` (method, line 420) — call by 20: `src/editor/barriers.ts#snapBarrierPoint`, `src/editor/streetObjects.ts#commitPedestrianCrossing`, `src/editor/streetObjects.ts#commitUturn`, `src/world/signalPosts.ts#signalPostPlace`, …
- `src/world/doc.ts#RoadDoc.setNodeApproachRules` (method, line 1027) — call by 1: `src/editor/streetscapeTool.ts#StreetscapeTool.down`
- `src/world/doc.ts#RoadDoc.setNodeControl` (method, line 1347) — call by 1: `src/editor/streetscapeTool.ts#StreetscapeTool.down`
- `src/world/doc.ts#RoadDoc.spansAt` (method, line 455) — call by 2: `src/render/poleLayer.ts#docPoles`, `src/render/poleCells.ts#PoleCells.build`
- `src/world/doc.ts#RoadDoc.takeBack` (method, line 1847) — call by 2: `src/editor/streetObjects.ts#commitPedestrianCrossing`, `src/editor/streetObjects.ts#commitUturn`
- `src/world/doc.ts#RoadNode` (interface, line 89) — read by 16: `src/editor/streetObjects.ts#commitPedestrianCrossing`, `src/editor/streetObjects.ts#commitUturn`, `src/world/signalPosts.ts#signalPostPlace`, `src/world/signalPosts.ts#signalPosts`, …
- `src/world/doc.ts#RoadNode` (interface, line 89) — write by 2: `src/world/doc.ts#RoadDoc.setNodeUturn`, `src/world/doc.ts#RoadDoc.setNodeCrossing`
- `src/world/doc.ts#RoadSegment` (interface, line 124) — read by 21: `src/editor/barriers.ts#snapBarrierPoint`, `src/editor/streetObjects.ts#commitPedestrianCrossing`, `src/editor/streetObjects.ts#commitUturn`, `src/world/signalPosts.ts#signalPostPlace`, …

### src/world/elevation.ts

- `src/world/elevation.ts#buildRoadElevation` (function, line 279) — implementation by 5: `src/render/signLayer.ts#itemSigns`, `src/render/signLayer.ts#junctionSigns`, `src/render/streetFurnitureLayer.ts#furniturePlace`, `src/render/utilities.ts#poleGroundAt`, …
- `src/world/elevation.ts#GROUND_ONLY` (variable, line 208) — read by 1: `src/render/utilities.ts#poleGroundAt`
- `src/world/elevation.ts#RoadElevation` (interface, line 135) — typeRef by 8: `src/render/signLayer.ts#SignLayerKept`, `src/render/signLayer.ts#SignSource`, `src/render/streetFurnitureLayer.ts#furniturePlace`, `src/render/streetFurnitureLayer.ts#FurnitureSource`, …
- `src/world/elevation.ts#roadShaper` (function, line 888) — implementation by 2: `src/render/utilities.ts#poleGroundAt`, `src/render/grass.ts#grassClump`

### src/world/geometry.ts

- `src/world/geometry.ts#orientedPolyline` (function, line 33) — call by 1: `src/world/network.ts#Network.crossingLandingAt`
- `src/world/geometry.ts#PolylineCache.get` (method, line 70) — call by 5: `src/editor/streetObjects.ts#commitPedestrianCrossing`, `src/editor/streetObjects.ts#commitUturn`, `src/world/signalPosts.ts#signalPostPlace`, `src/world/roads/derivedSigns.ts#signApproach`, …

### src/world/ids.ts

- `src/world/ids.ts#NodeId` (type, line 7) — typeRef by 27: `src/editor/streetObjects.ts#PedestrianCrossingResult`, `src/editor/streetObjects.ts#UturnResult`, `src/world/signalPosts.ts#SignalJunctions`, `src/world/signalPosts.ts#SignalPost`, …
- `src/world/ids.ts#PoleId` (type, line 10) — typeRef by 28: `src/editor/poles.ts#commitPoleRunPoles`, `src/editor/poles.ts#PlannedPole`, `src/editor/poles.ts#PoleSnap`, `src/world/poleIndex.ts#PoleIndex.cellOfPole`, …
- `src/world/ids.ts#SegmentId` (type, line 8) — typeRef by 26: `src/editor/streetObjects.ts#commitPedestrianCrossing`, `src/editor/streetObjects.ts#commitUturn`, `src/editor/streetObjects.ts#PedestrianCrossingResult`, `src/world/streetFurniture.ts#FurnitureItem`, …
- `src/world/ids.ts#SpanId` (type, line 12) — typeRef by 13: `src/world/poleIndex.ts#PoleIndex.spansAt`, `src/world/poleIndex.ts#PoleIndex.spansOfPole`, `src/world/utilities.ts#poleArms`, `src/world/utilities.ts#PoleArms`, …

### src/world/junction/types.ts

- `src/world/junction/types.ts#Junction` (interface, line 60) — typeRef by 2: `src/world/poleLines.ts#junctionRings`, `src/world/poleLines.ts#poleLinesOf`

### src/world/landscape.ts

- `src/world/landscape.ts#ACCESS_CELL` (variable, line 293) — read by 1: `src/world/landscape.ts#crossingAccessTester`
- `src/world/landscape.ts#ACCESS_REACH` (variable, line 278) — read by 3: `src/render/streetFurnitureLayer.ts#ROAD_REACH`, `src/world/roads/furnitureSets.ts#furnitureFor`, `src/world/landscape.ts#nearCrossingAccess`
- `src/world/landscape.ts#accessMemo` (variable, line 237) — read by 1: `src/world/landscape.ts#crossingAccesses`
- `src/world/landscape.ts#accessMemo` (variable, line 237) — write by 1: `src/world/landscape.ts#crossingAccesses`
- `src/world/landscape.ts#apart` (function, line 412) — call by 1: `src/world/landscape.ts#snapLandscape`
- `src/world/landscape.ts#CrossingAccess` (interface, line 227) — read by 3: `src/editor/poles.ts#inLanding`, `src/world/landscape.ts#onCrossingAccess`, `src/world/landscape.ts#crossingAccessTester`
- `src/world/landscape.ts#CrossingAccess` (interface, line 227) — typeRef by 7: `src/editor/poles.ts#inLanding`, `src/world/poleLines.ts#PoleLines`, `src/world/landscape.ts#crossingAccessesAt`, `src/world/landscape.ts#onCrossingAccess`, …
- `src/world/landscape.ts#FootwayHit` (interface, line 76) — read by 6: `src/world/streetFurniture.ts#standing`, `src/render/signLayer.ts#itemSigns`, `src/world/roads/furnitureSets.ts#furnitureFor`, `src/editor/streetscapeTool.ts#rowPieces`, …
- `src/world/landscape.ts#FootwayHit` (interface, line 76) — typeRef by 1: `src/world/landscape.ts#footwayAt`
- `src/world/landscape.ts#GROW_MINUTES` (variable, line 30) — read by 2: `src/render/streetFurnitureLayer.ts#stillGrowing`, `src/world/landscape.ts#plantGrowth`
- `src/world/landscape.ts#ITEM_GAP` (variable, line 73) — read by 2: `src/world/landscape.ts#snapLandscape`, `src/world/landscape.ts#snapLandscape.others`
- `src/world/landscape.ts#LANDSCAPE_RADIUS` (variable, line 57) — read by 3: `src/world/streetFurniture.ts#standing`, `src/world/landscape.ts#snapLandscape`, `src/world/landscape.ts#depthFor`
- `src/world/landscape.ts#LandscapeItem` (interface, line 16) — read by 16: `src/world/streetFurniture.ts#furnitureOf`, `src/world/streetFurniture.ts#standing`, `src/render/signLayer.ts#isSignItem`, `src/render/signLayer.ts#itemSigns`, …
- `src/world/landscape.ts#LandscapeItem` (interface, line 16) — typeRef by 17: `src/world/streetFurniture.ts#furnitureOf`, `src/world/streetFurniture.ts#standing`, `src/render/signLayer.ts#isSignItem`, `src/render/streetFurnitureLayer.ts#furniturePlace`, …
- `src/world/landscape.ts#LandscapeKind` (type, line 14) — typeRef by 11: `src/world/streetFurniture.ts#FURNITURE_OF`, `src/world/roads/furnitureCatalog.ts#furnitureEntry`, `src/world/roads/furnitureCatalog.ts#FurnitureEntry`, `src/world/roads/furnitureSets.ts#FurniturePlacement`, …
- `src/world/landscape.ts#landscapeNearIn` (function, line 398) — call by 1: `src/editor/streetscapeTool.ts#StreetscapeTool.down`
- `src/world/landscape.ts#LandscapeSnap` (type, line 315) — read by 4: `src/world/roads/furnitureSets.ts#furnitureFor`, `src/editor/streetscapeTool.ts#rowPieces`, `src/editor/streetscapeTool.ts#StreetscapeTool.down`, `src/editor/streetscapeTool.ts#StreetscapeTool.up`
- `src/world/landscape.ts#LandscapeSnap` (type, line 315) — typeRef by 4: `src/editor/streetscapeTool.ts#rowPieces`, `src/editor/streetscapeTool.ts#StreetscapeTool.hover`, `src/editor/streetscapeTool.ts#StreetscapeTool.row`, `src/world/landscape.ts#snapLandscape`
- `src/world/landscape.ts#MAX_ITEM_RADIUS` (variable, line 395) — read by 4: `src/render/streetFurnitureLayer.ts#ROAD_REACH`, `src/world/roads/furnitureSets.ts#furnitureFor`, `src/editor/streetscapeTool.ts#itemsAround`, `src/world/landscape.ts#snapLandscape.others`
- `src/world/landscape.ts#MEDIAN_TREE_MIN` (variable, line 136) — read by 1: `src/world/landscape.ts#plantedMedian`
- `src/world/landscape.ts#MEDIAN_UTURN_OPENING` (variable, line 158) — read by 1: `src/editor/streetObjects.ts#commitUturn`
- `src/world/landscape.ts#MedianHit` (interface, line 127) — read by 3: `src/world/streetFurniture.ts#standing`, `src/world/roads/furnitureSets.ts#furnitureFor`, `src/world/landscape.ts#snapLandscape`
- `src/world/landscape.ts#MedianHit` (interface, line 127) — typeRef by 1: `src/world/landscape.ts#medianAt`
- `src/world/landscape.ts#medianNose` (function, line 148) — call by 1: `src/world/landscape.ts#medianAt`
- `src/world/landscape.ts#PLANTED_SIZE` (variable, line 32) — read by 1: `src/world/landscape.ts#plantGrowth`
- `src/world/landscape.ts#SignType` (type, line 43) — typeRef by 12: `src/render/signs.ts#onTopOfPost`, `src/render/signs.ts#PLATE`, `src/render/signs.ts#plateCanvas`, `src/render/signs.ts#plateFaces`, …

### src/world/lanelets.ts

- `src/world/lanelets.ts#Connector` (interface, line 62) — read by 2: `src/editor/streetObjects.ts#commitUturn`, `src/world/lanelets.ts#LaneletGraph.junctionConnectors`
- `src/world/lanelets.ts#Connector` (interface, line 62) — typeRef by 1: `src/world/lanelets.ts#LaneletGraph.junctionConnectors`
- `src/world/lanelets.ts#ConnectorId` (type, line 28) — typeRef by 1: `src/world/lanelets.ts#LaneletGraph.junctionConnectors`
- `src/world/lanelets.ts#JunctionTopology` (interface, line 78) — read by 1: `src/world/lanelets.ts#LaneletGraph.junctionConnectors`
- `src/world/lanelets.ts#Lanelet` (interface, line 31) — read by 1: `src/world/lanelets.ts#LaneletGraph.junctionConnectors`
- `src/world/lanelets.ts#LaneletGraph` (class, line 137) — construct by 1: `src/world/lanelets.ts#LaneletGraph.junctionConnectors`
- `src/world/lanelets.ts#LaneletGraph` (class, line 137) — reference by 1: `src/editor/streetObjects.ts#commitUturn`
- `src/world/lanelets.ts#LaneletGraph.buildJunction` (method, line 696) — call by 1: `src/world/lanelets.ts#LaneletGraph.junctionConnectors`
- `src/world/lanelets.ts#LaneletGraph.buildSegmentLinks` (method, line 447) — call by 1: `src/world/lanelets.ts#LaneletGraph.junctionConnectors`
- `src/world/lanelets.ts#LaneletGraph.connectors` (field, line 139) — read by 1: `src/world/lanelets.ts#LaneletGraph.junctionConnectors`
- `src/world/lanelets.ts#LaneletGraph.junctions` (field, line 140) — read by 1: `src/world/lanelets.ts#LaneletGraph.junctionConnectors`
- `src/world/lanelets.ts#LaneletGraph.lanelets` (field, line 138) — read by 1: `src/world/lanelets.ts#LaneletGraph.junctionConnectors`

### src/world/network.ts

- `src/world/network.ts#CrossingLanding` (interface, line 37) — typeRef by 1: `src/world/network.ts#Network.crossingLandingAt`
- `src/world/network.ts#Network` (class, line 46) — typeRef by 79: `src/editor/poles.ts#computePoleRun`, `src/editor/poles.ts#lastPlan`, `src/editor/poles.ts#planPoleRun`, `src/editor/poles.ts#planPoleRunWhole`, …
- `src/world/network.ts#Network.constructor` (constructor, line 100) — read by 33: `src/editor/barriers.ts#snapBarrierPoint`, `src/world/streetFurniture.ts#streetFurniture`, `src/world/signalPosts.ts#signalPostPlace`, `src/world/signalPosts.ts#signalPosts`, …
- `src/world/network.ts#Network.crossingLandings` (field, line 52) — read by 1: `src/world/network.ts#Network.crossingLandingAt`
- `src/world/network.ts#Network.crossingLandings` (field, line 52) — write by 1: `src/world/network.ts#Network.crossingLandingAt`
- `src/world/network.ts#Network.crosswalkDistanceAt` (method, line 324) — call by 2: `src/editor/streetObjects.ts#commitPedestrianCrossing`, `src/world/network.ts#Network.crossingLandingAt`
- `src/world/network.ts#Network.fork` (method, line 174) — call by 2: `src/editor/streetObjects.ts#commitPedestrianCrossing`, `src/editor/streetObjects.ts#commitUturn`
- `src/world/network.ts#Network.impossible` (field, line 72) — read by 1: `src/editor/streetObjects.ts#commitPedestrianCrossing`
- `src/world/network.ts#Network.junctionCells` (field, line 58) — read by 6: `src/world/poleLines.ts#insideLevels`, `src/world/poleLines.ts#localRings`, `src/world/poleLines.ts#markPlates`, `src/world/poleLines.ts#poleLinesOf`, …
- `src/world/network.ts#Network.junctions` (field, line 54) — read by 4: `src/world/poleLines.ts#insideLevels`, `src/world/poleLines.ts#localRings`, `src/world/poleLines.ts#markPlates`, `src/world/poleLines.ts#poleLinesOf`
- `src/world/network.ts#Network.mouthDistance` (method, line 288) — call by 7: `src/world/signalPosts.ts#signalPostPlace`, `src/render/signLayer.ts#itemSigns`, `src/world/roads/derivedSigns.ts#signApproach`, `src/world/roads/derivedSigns.ts#signsAtNode`, …
- `src/world/network.ts#Network.polylines` (field, line 80) — read by 5: `src/editor/streetObjects.ts#commitPedestrianCrossing`, `src/editor/streetObjects.ts#commitUturn`, `src/world/signalPosts.ts#signalPostPlace`, `src/world/roads/derivedSigns.ts#signApproach`, …
- `src/world/network.ts#Network.rebuild` (method, line 119) — call by 2: `src/editor/streetObjects.ts#commitPedestrianCrossing`, `src/editor/streetObjects.ts#commitUturn`
- `src/world/network.ts#Network.revision` (field, line 87) — read by 2: `src/editor/poles.ts#planPoleRun`, `src/world/landscape.ts#crossingAccesses`
- `src/world/network.ts#Network.ribbons` (field, line 55) — read by 12: `src/editor/poleRoutes.ts#buildRouteGraph`, `src/world/poleLines.ts#insideLevels`, `src/world/poleLines.ts#localRings`, `src/world/poleLines.ts#markPlates`, …
- `src/world/network.ts#Network.ribbonsMeeting` (method, line 62) — call by 5: `src/world/signalPosts.ts#signalPosts`, `src/render/signs.ts#streetChain`, `src/world/roads/furnitureSets.ts#furnitureFor`, `src/world/landscape.ts#nearCrossingAccess`, …
- `src/world/network.ts#Network.segmentCells` (field, line 57) — read by 9: `src/editor/poleRoutes.ts#buildRouteGraph`, `src/world/poleLines.ts#insideLevels`, `src/world/poleLines.ts#localRings`, `src/world/poleLines.ts#markPlates`, …
- `src/world/network.ts#Network.stopLineDistance` (method, line 298) — call by 4: `src/editor/streetObjects.ts#commitPedestrianCrossing`, `src/editor/streetObjects.ts#commitUturn`, `src/world/signalPosts.ts#signalPostPlace`, `src/world/roads/derivedSigns.ts#signsAtNode`
- `src/world/network.ts#Network.takeBack` (method, line 198) — call by 2: `src/editor/streetObjects.ts#commitPedestrianCrossing`, `src/editor/streetObjects.ts#commitUturn`
- `src/world/network.ts#Network.trafficRevision` (field, line 89) — read by 1: `src/world/landscape.ts#crossingAccesses`

### src/world/pedestrianAccess.ts

- `src/world/pedestrianAccess.ts#carriesPedestrians` (function, line 4) — call by 3: `src/world/poleLines.ts#polesInclude`, `src/world/roads/furnitureSets.ts#furnitureFor`, `src/world/landscape.ts#footwayAt`

### src/world/pointFiling.ts

- `src/world/pointFiling.ts#PointFiling.idsMeetingAny` (method, line 72) — call by 1: `src/world/doc.ts#RoadDoc.landscapeInAny`

### src/world/rasterGrid.ts

- `src/world/rasterGrid.ts#gridRect` (function, line 22) — call by 1: `src/world/poleLines.ts#markPlates`
- `src/world/rasterGrid.ts#NEAR` (variable, line 19) — read by 1: `src/world/poleLines.ts#markPlates`
- `src/world/rasterGrid.ts#PAVED` (variable, line 17) — read by 1: `src/world/poleLines.ts#markPlates`
- `src/world/rasterGrid.ts#RasterGrid` (interface, line 6) — typeRef by 1: `src/world/poleLines.ts#markPlates`

### src/world/ribbons.ts

- `src/world/ribbons.ts#SegmentRibbon` (interface, line 25) — read by 17: `src/editor/poleRoutes.ts#buildRouteGraph`, `src/editor/barriers.ts#onCarriageway`, `src/editor/barriers.ts#snapBarrierPoint`, `src/world/signalPosts.ts#signalPosts`, …
- `src/world/ribbons.ts#SegmentRibbon` (interface, line 25) — typeRef by 3: `src/world/poleLines.ts#poleLinesOf`, `src/world/poleLines.ts#ribbonRings`, `src/world/landscape.ts#ribbonsNear`

### src/world/roadField.ts

- `src/world/roadField.ts#roadField` (function, line 417) — implementation by 1: `src/render/utilities.ts#poleGroundAt`
- `src/world/roadField.ts#roadField.onSegment` (method, line 601) — implementation by 3: `src/render/signLayer.ts#itemSigns`, `src/render/signLayer.ts#junctionSigns`, `src/render/streetFurnitureLayer.ts#furniturePlace`
- `src/world/roadField.ts#roadField.roadAt` (method, line 624) — implementation by 2: `src/render/utilities.ts#poleGroundAt`, `src/render/grass.ts#grassClump`
- `src/world/roadField.ts#RoadReads` (interface, line 96) — call by 5: `src/render/signLayer.ts#itemSigns`, `src/render/signLayer.ts#junctionSigns`, `src/render/streetFurnitureLayer.ts#furniturePlace`, `src/render/utilities.ts#poleGroundAt`, …
- `src/world/roadField.ts#RoadSample` (interface, line 57) — read by 2: `src/render/utilities.ts#poleGroundAt`, `src/render/grass.ts#grassClump`

### src/world/roads/footwayRise.ts

- `src/world/roads/footwayRise.ts#footwayRiseAt` (function, line 33) — call by 4: `src/render/signLayer.ts#itemSigns`, `src/render/signLayer.ts#junctionSigns`, `src/render/streetFurnitureLayer.ts#furniturePlace`, `src/render/utilities.ts#poleGroundAt`

### src/world/roads/rules.ts

- `src/world/roads/rules.ts#ApproachRuleEntry` (interface, line 11) — read by 1: `src/editor/streetscapeTool.ts#StreetscapeTool.down`
- `src/world/roads/rules.ts#approachSign` (function, line 122) — call by 1: `src/world/roads/derivedSigns.ts#signsAtNode`
- `src/world/roads/rules.ts#arrivesAt` (function, line 137) — call by 2: `src/world/roads/derivedSigns.ts#signApproach`, `src/world/roads/derivedSigns.ts#signsAtNode`

### src/world/roads/tuning.ts

- `src/world/roads/tuning.ts#ROAD_TUNING` (variable, line 4) — read by 3: `src/world/utilities.ts#DEFAULT_POLE_SPACING`, `src/world/utilities.ts#MAX_POLE_SPACING`, `src/world/utilities.ts#MIN_POLE_SPACING`

### src/world/roadSection.ts

- `src/world/roadSection.ts#SectionMaterials` (interface, line 14) — read by 1: `src/world/landscape.ts#plantedMedian`

### src/world/roadTypes.ts

- `src/world/roadTypes.ts#CURB_BAND` (variable, line 90) — read by 4: `src/editor/poleRoutes.ts#buildRouteGraph`, `src/editor/barriers.ts#onCarriageway`, `src/editor/barriers.ts#snapBarrierPoint`, `src/render/propGeometry.ts#drainGeometry`
- `src/world/roadTypes.ts#FOOTWAY_RISE` (variable, line 93) — read by 3: `src/render/streetFurnitureLayer.ts#furniturePlace`, `src/render/utilities.ts#poleGroundAt`, `src/render/propGeometry.ts#drainGeometry`
- `src/world/roadTypes.ts#halfWidth` (function, line 339) — call by 2: `src/editor/barriers.ts#snapBarrierPoint`, `src/editor/streetscapeTool.ts#itemsAround`
- `src/world/roadTypes.ts#Level` (enum, line 10) — reference by 7: `src/editor/poleRoutes.ts#buildRouteGraph`, `src/editor/barriers.ts#snapBarrierPoint`, `src/world/poleLines.ts#junctionRings`, `src/world/poleLines.ts#poleLines`, …
- `src/world/roadTypes.ts#roadProfile` (function, line 248) — call by 5: `src/editor/streetObjects.ts#commitPedestrianCrossing`, `src/editor/streetObjects.ts#commitUturn`, `src/world/signalPosts.ts#signalPostPlace`, `src/world/roads/derivedSigns.ts#signsAtNode`, …
- `src/world/roadTypes.ts#RoadSide` (type, line 353) — typeRef by 3: `src/world/roads/furnitureSets.ts#furnitureFor`, `src/world/landscape.ts#depthFor`, `src/world/section.ts#zonesOn`
- `src/world/roadTypes.ts#RoadType` (interface, line 29) — read by 16: `src/editor/poleRoutes.ts#buildRouteGraph`, `src/editor/barriers.ts#onCarriageway`, `src/editor/barriers.ts#snapBarrierPoint`, `src/editor/streetObjects.ts#commitPedestrianCrossing`, …
- `src/world/roadTypes.ts#RoadType` (interface, line 29) — typeRef by 2: `src/world/landscape.ts#depthFor`, `src/world/landscape.ts#plantedMedian`
- `src/world/roadTypes.ts#SurfaceLevel` (type, line 25) — typeRef by 3: `src/world/poleLines.ts#junctionRings`, `src/world/poleLines.ts#poleLinesOf`, `src/world/surfaces.ts#levelPolygons`
- `src/world/roadTypes.ts#travelShift` (function, line 403) — call by 2: `src/world/roads/furnitureSets.ts#furnitureFor`, `src/world/landscape.ts#medianAt`

### src/world/section.ts

- `src/world/section.ts#Band` (interface, line 8) — read by 4: `src/render/signLayer.ts#itemSigns`, `src/world/roads/furnitureSets.ts#furnitureFor`, `src/world/landscape.ts#depthFor`, `src/world/landscape.ts#footwayAt`
- `src/world/section.ts#BENCH_ZONE` (variable, line 56) — read by 3: `src/world/roads/furnitureCatalog.ts#FURNITURE_CATALOG`, `src/world/roads/furnitureSets.ts#furnitureFor`, `src/world/landscape.ts#depthFor`
- `src/world/section.ts#CrossSection` (interface, line 33) — read by 3: `src/editor/streetObjects.ts#commitPedestrianCrossing`, `src/render/signLayer.ts#itemSigns`, `src/world/section.ts#zonesOn`
- `src/world/section.ts#CrossSection` (interface, line 33) — typeRef by 1: `src/world/section.ts#zonesOn`
- `src/world/section.ts#LAMP_ZONE` (variable, line 54) — read by 4: `src/world/poleLines.ts#POLE_KERB_INSET`, `src/render/signLayer.ts#itemSigns`, `src/world/roads/furnitureCatalog.ts#FURNITURE_CATALOG`, `src/world/landscape.ts#depthFor`
- `src/world/section.ts#MIN_THROUGH` (variable, line 60) — read by 1: `src/world/landscape.ts#depthFor`
- `src/world/section.ts#sectionOf` (function, line 67) — call by 5: `src/editor/streetObjects.ts#commitPedestrianCrossing`, `src/render/signLayer.ts#itemSigns`, `src/world/roads/furnitureSets.ts#furnitureFor`, `src/world/landscape.ts#depthFor`, …
- `src/world/section.ts#SideZones` (interface, line 14) — read by 4: `src/render/signLayer.ts#itemSigns`, `src/world/roads/furnitureSets.ts#furnitureFor`, `src/world/landscape.ts#depthFor`, `src/world/landscape.ts#footwayAt`
- `src/world/section.ts#SideZones` (interface, line 14) — typeRef by 1: `src/world/section.ts#zonesOn`
- `src/world/section.ts#TREE_KERB_SETBACK` (variable, line 50) — read by 2: `src/world/roads/furnitureCatalog.ts#FURNITURE_CATALOG`, `src/world/landscape.ts#depthFor`
- `src/world/section.ts#TREE_PIT` (variable, line 49) — read by 3: `src/render/propGeometry.ts#treePitGeometry`, `src/world/roads/furnitureCatalog.ts#FURNITURE_CATALOG`, `src/world/landscape.ts#depthFor`

### src/world/surfaces.ts

- `src/world/surfaces.ts#levelRings` (function, line 73) — call by 1: `src/world/surfaces.ts#levelPolygons`
- `src/world/surfaces.ts#levelRingsOf` (function, line 81) — call by 3: `src/world/poleLines.ts#junctionRings`, `src/world/poleLines.ts#poleLinesOf`, `src/world/poleLines.ts#ribbonRings`
- `src/world/surfaces.ts#SurfaceNet` (interface, line 21) — typeRef by 1: `src/world/surfaces.ts#levelPolygons`
- `src/world/surfaces.ts#SurfaceSegmentFilter` (type, line 9) — typeRef by 1: `src/world/surfaces.ts#levelPolygons`

### src/world/viewCells.ts

- `src/world/viewCells.ts#CellKey` (type, line 23) — typeRef by 17: `src/render/poleCells.ts#PoleCells.derive`, `src/render/poleCells.ts#PoleCells.look`, `src/render/poleCells.ts#PoleCells.wantedKeys`, `src/render/poleCells.ts#PoleCells.wantedOrder`, …
- `src/world/viewCells.ts#CellProduct` (interface, line 30) — read by 2: `src/world/viewCells.ts#ViewCells.peek`, `src/world/viewCells.ts#ViewCells.account`
- `src/world/viewCells.ts#CellStage` (variable, line 26) — read by 1: `src/world/viewCells.ts#ViewCells.account`
- `src/world/viewCells.ts#CellStage@27` (type, line 27) — reference by 1: `src/world/viewCells.ts#ViewCells.account`
- `src/world/viewCells.ts#Held` (interface, line 68) — read by 2: `src/world/viewCells.ts#ViewCells.peek`, `src/world/viewCells.ts#ViewCells.account`
- `src/world/viewCells.ts#Held` (interface, line 68) — write by 1: `src/world/viewCells.ts#ViewCells.account`
- `src/world/viewCells.ts#ViewCells` (class, line 95) — construct by 2: `src/render/poleCells.ts#PoleCells.constructor`, `src/render/propCells.ts#createPropLayer`
- `src/world/viewCells.ts#ViewCells` (class, line 95) — typeRef by 4: `src/render/poleCells.ts#PoleCells.cells`, `src/render/propCells.ts#createPropLayer`, `src/render/propCells.ts#PropLayer`, `src/world/viewCells.ts#ViewCells.peek`
- `src/world/viewCells.ts#ViewCells.cellAt` (method, line 117) — call by 1: `src/render/poleCells.ts#PoleCells.look`
- `src/world/viewCells.ts#ViewCells.clear` (method, line 271) — call by 4: `src/render/poleCells.ts#PoleCells.dispose`, `src/render/poleCells.ts#PoleCells.source`, `src/render/streetFurnitureLayer.ts#createStreetFurniture.sync`, `src/render/propCells.ts#createPropLayer.dispose`
- `src/world/viewCells.ts#ViewCells.constructor` (constructor, line 111) — read by 1: `src/world/viewCells.ts#ViewCells.peek`
- `src/world/viewCells.ts#ViewCells.cpuBytes` (field, line 105) — read by 2: `src/render/propCells.ts#createPropLayer`, `src/world/viewCells.ts#ViewCells.account`
- `src/world/viewCells.ts#ViewCells.cpuBytes` (field, line 105) — write by 1: `src/world/viewCells.ts#ViewCells.account`
- `src/world/viewCells.ts#ViewCells.drop` (method, line 328) — call by 1: `src/world/viewCells.ts#ViewCells.forget`
- `src/world/viewCells.ts#ViewCells.ensure` (method, line 161) — call by 3: `src/render/poleCells.ts#PoleCells.look`, `src/render/propCells.ts#createPropLayer.ringsIn`, `src/render/propCells.ts#createPropLayer.show`
- `src/world/viewCells.ts#ViewCells.follow` (method, line 279) — call by 2: `src/render/poleCells.ts#PoleCells.follow`, `src/render/propCells.ts#createPropLayer.follow`
- `src/world/viewCells.ts#ViewCells.gpuBytes` (field, line 106) — read by 1: `src/world/viewCells.ts#ViewCells.account`
- `src/world/viewCells.ts#ViewCells.gpuBytes` (field, line 106) — write by 1: `src/world/viewCells.ts#ViewCells.account`
- `src/world/viewCells.ts#ViewCells.has` (method, line 148) — call by 3: `src/render/poleCells.ts#PoleCells.look`, `src/render/propCells.ts#createPropLayer.ringsIn`, `src/render/propCells.ts#createPropLayer.show`
- `src/world/viewCells.ts#ViewCells.held` (field, line 98) — read by 3: `src/world/viewCells.ts#ViewCells.peek`, `src/world/viewCells.ts#ViewCells.forget`, `src/world/viewCells.ts#ViewCells.account`
- `src/world/viewCells.ts#ViewCells.touch` (method, line 338) — call by 1: `src/world/viewCells.ts#ViewCells.peek`
- `src/world/viewCells.ts#ViewCells.trim` (method, line 292) — call by 3: `src/render/poleCells.ts#PoleCells.look`, `src/render/propCells.ts#createPropLayer.ringsIn`, `src/render/propCells.ts#createPropLayer.show`
- `src/world/viewCells.ts#ViewCells.version` (field, line 109) — read by 1: `src/world/viewCells.ts#ViewCells.forget`
- `src/world/viewCells.ts#ViewCells.version` (field, line 109) — write by 1: `src/world/viewCells.ts#ViewCells.forget`
- `src/world/viewCells.ts#ViewCells.wanted` (field, line 102) — read by 1: `src/world/viewCells.ts#ViewCells.want`
- `src/world/viewCells.ts#ViewCells.wantedKeys` (getter, line 242) — call by 2: `src/render/poleCells.ts#PoleCells.look`, `src/render/propCells.ts#createPropLayer.show`
- `src/world/viewCells.ts#ViewCells.wantedSet` (field, line 103) — read by 1: `src/world/viewCells.ts#ViewCells.want`
- `src/world/viewCells.ts#ViewCells.wantedSet` (field, line 103) — write by 1: `src/world/viewCells.ts#ViewCells.want`
- `src/world/viewCells.ts#ViewCells.work` (field, line 107) — read by 2: `src/render/streetFurnitureLayer.ts#createStreetFurniture.sync`, `src/world/viewCells.ts#ViewCells.forget`
- `src/world/viewCells.ts#ViewCellsSpec` (interface, line 37) — read by 1: `src/world/viewCells.ts#ViewCells.peek`
- `src/world/viewCells.ts#ViewCellsWork` (interface, line 62) — read by 2: `src/render/streetFurnitureLayer.ts#createStreetFurniture.sync`, `src/world/viewCells.ts#ViewCells.forget`
- `src/world/viewCells.ts#ViewCellsWork` (interface, line 62) — write by 1: `src/world/viewCells.ts#ViewCells.forget`

### src/world/walkways.ts

- `src/world/walkways.ts#deckOf` (function, line 60) — call by 1: `src/world/poleLines.ts#polesInclude`

## External APIs

- `es:Math` × 403
- `es:Array.push` × 158
- `es:Array.length` × 122
- `es:Math.max` × 103
- `es:Map.get` × 86
- `es:Float32Array` × 77
- `es:Math.floor` × 72
- `es:Math.min` × 59
- `es:Map` × 57
- `es:Map.set` × 48
- `es:Math.PI` × 43
- `three:BufferGeometry` × 41
- `es:Set.add` × 39
- `es:ReadonlyArray.length` × 37
- `three:BufferAttribute` × 35
- `es:Infinity` × 32
- `three:BoxGeometry` × 32
- `three:Color` × 30
- `es:Math.hypot` × 27
- `es:Set` × 27
- `es:Math.abs` × 26
- `es:Map.delete` × 25
- `es:Record` × 25
- `three:BufferGeometry.setAttribute` × 25
- `three:Vector3` × 24
- `three:Matrix4` × 23
- `three:MeshStandardMaterial` × 23
- `es:String.replace` × 21
- `three:InstancedMesh` × 21
- `es:Array.map` × 20
- `es:Float32Array.set` × 20
- `es:Int32Array` × 20
- `three:BufferGeometry.getAttribute` × 20
- `es:ReadonlyMap` × 19
- `three:Vector3.x` × 19
- `three:Vector3.z` × 19
- `es:ReadonlyMap.get` × 18
- `three:Vector3.set` × 18
- `dom:CanvasFillStrokeStyles.fillStyle` × 17
- `three:BufferAttribute.array` × 17
- `three:Vector3.y` × 17
- `es:Float64Array` × 16
- `es:Iterable` × 16
- `es:Array.sort` × 15
- `es:Map.clear` × 15
- `es:Math.round` × 15
- `es:Object` × 15
- `es:ReadonlyArray.map` × 15
- `dom:CanvasPath.lineTo` × 14
- `es:Map.values` × 14
- `es:Math.sin` × 14
- `three:Group` × 14
- `three:Mesh.geometry` × 14
- `three:Sphere.center` × 14
- `three:Sphere.radius` × 14
- `three:CylinderGeometry` × 13
- `three:Object3D.name` × 13
- `dom:CanvasDrawPath.beginPath` × 12
- `three:BufferAttribute.count` × 12
- `three:BufferGeometry.boundingSphere` × 12
- `three:Sphere` × 12
- `three:WebGLProgramParameters.vertexShader` × 12
- `dom:HTMLCanvasElement.height` × 11
- `es:Array.forEach` × 11
- `three:BufferGeometry.dispose` × 11
- `three:InstancedMesh.instanceMatrix` × 11
- `three:Object3D.add` × 11
- `es:Float32Array.subarray` × 10
- `es:Math.ceil` × 10
- `es:Math.cos` × 10
- `three:CanvasTexture` × 10
- `three:InstancedBufferAttribute` × 10
- `dom:HTMLCanvasElement.width` × 9
- `es:Float64Array.length` × 9
- `es:Number.toFixed` × 9
- `three:BufferGeometry.setIndex` × 9
- `three:Color.b` × 9
- `three:Color.g` × 9
- `three:Color.r` × 9
- `three:Material` × 9
- `three:Object3D.visible` × 9
- `dom:CanvasDrawPath.fill` × 8
- `dom:CanvasPath.moveTo` × 8
- `es:Math.atan2` × 8
- `es:ReadonlyArray.forEach` × 8
- `es:ReadonlyMap.values` × 8
- `three:BufferGeometry.computeBoundingSphere` × 8
- `three:Camera` × 8
- `three:Frustum` × 8
- `three:Object3D.remove` × 8
- `es:Number` × 7
- `es:ReadonlyArray.some` × 7
- `three:BufferAttribute.needsUpdate` × 7
- `three:BufferGeometry.index` × 7
- `three:InstancedInterleavedBuffer` × 7
- `three:InstancedMesh.count` × 7
- `three:InstancedMesh.instanceColor` × 7
- `three:Material.onBeforeCompile` × 7
- `three:Matrix4.elements` × 7
- `three:Mesh` × 7
- `three:MeshDepthMaterial` × 7
- `dom:CanvasRect.fillRect` × 6
- `dom:CanvasText.fillText` × 6
- `dom:document` × 6
- `es:Array` × 6
- `es:Array.filter` × 6
- `es:Float32Array.fill` × 6
- `es:Readonly` × 6
- `three:Texture.colorSpace` × 6
- `dom:HTMLCanvasElement` × 5
- `dom:performance` × 5
- `dom:Performance.now` × 5
- `es:ArrayConstructor.from` × 5
- `es:Float32Array.byteLength` × 5
- `es:Float32Array.copyWithin` × 5
- `es:Float32Array.length` × 5
- `es:Map.has` × 5
- `es:Map.keys` × 5
- `es:Math.imul` × 5
- `es:Math.sqrt` × 5
- `es:NumberConstructor.isFinite` × 5
- `es:ObjectConstructor.keys` × 5
- `es:ReadonlyMap.has` × 5
- `es:Set.delete` × 5
- `es:Set.has` × 5
- `es:Uint8Array` × 5
- `es:WeakMap.get` × 5
- `es:WeakMap.set` × 5
- `three:BufferAttribute.clearUpdateRanges` × 5
- `three:BufferGeometry.attributes` × 5
- `three:BufferGeometry.deleteAttribute` × 5
- `three:InstancedBufferGeometry` × 5
- `three:InstancedMesh.boundingSphere` × 5
- `three:InstancedMesh.dispose` × 5
- `three:InterleavedBuffer` × 5
- `three:Material.customProgramCacheKey` × 5
- `three:Object3D` × 5
- `three:Object3D.castShadow` × 5
- `three:Object3D.clear` × 5
- `three:Texture.dispose` × 5
- `dom:CanvasDrawPath.stroke` × 4
- `dom:CanvasFillStrokeStyles.strokeStyle` × 4
- `dom:CanvasPath.closePath` × 4
- `dom:CanvasPathDrawingStyles.lineWidth` × 4
- `dom:Document.createElement` × 4
- `dom:HTMLCanvasElement.getContext` × 4
- `es:Array.splice` × 4
- `es:Float32ArrayConstructor.from` × 4
- `es:Float64ArrayConstructor.from` × 4
- `es:Int32Array.fill` × 4
- `es:ObjectConstructor.fromEntries` × 4
- `es:ReadonlyArray.includes` × 4
- `es:Set.size` × 4
- `three:Box3.max` × 4
- `three:Box3.min` × 4
- `three:BufferAttribute.setUsage` × 4
- `three:DoubleSide` × 4
- `three:DynamicDrawUsage` × 4
- `three:Material.dispose` × 4
- `three:Object3D.onBeforeRender` × 4
- `three:OrthographicCamera` × 4
- `three:RGBADepthPacking` × 4
- `three:SRGBColorSpace` × 4
- `three:WebGLProgramParameters.fragmentShader` × 4
- `dom:CanvasPath.arc` × 3
- `dom:CanvasTextDrawingStyles.font` × 3
- `es:Array.find` × 3
- `es:Array.indexOf` × 3
- `es:Array.pop` × 3
- `es:ArrayLike` × 3
- `es:CallableFunction.bind` × 3
- `es:Parameters` × 3
- `es:Partial` × 3
- `es:Pick` × 3
- `es:ReadonlyArray.filter` × 3
- `es:ReadonlyArray.find` × 3
- `es:ReadonlyArray.reduce` × 3
- `es:WeakMap` × 3
- `three:Box3` × 3
- `three:BufferGeometry.computeBoundingBox` × 3
- `three:BufferGeometry.rotateX` × 3
- `three:Camera.projectionMatrix` × 3
- `three:DataTexture` × 3
- `three:InterleavedBufferAttribute` × 3
- `three:Matrix4.multiplyMatrices` × 3
- `three:Matrix4.setPosition` × 3
- `three:MeshBasicMaterialProperties.color` × 3
- `three:Object3D.frustumCulled` × 3
- `three:Object3D.userData` × 3
- `three:Texture.needsUpdate` × 3
- `three:Texture.userData` × 3
- `three:Vector2.y` × 3
- `three:Vector3.applyMatrix4` × 3
- `three:Vector3.fromBufferAttribute` × 3
- `three:Vector3.length` × 3
- `three:WebGLProgramParametersWithUniforms.uniforms` × 3
- `dom:CanvasDrawImage.drawImage` × 2
- `dom:CanvasGradient.addColorStop` × 2
- `dom:localStorage` × 2
- `es:Array.includes` × 2
- `es:CallableFunction.call` × 2
- `es:Int32Array.slice` × 2
- `es:Math.SQRT2` × 2
- `es:NumberConstructor.NaN` × 2
- `es:ObjectConstructor.assign` × 2
- `es:ObjectConstructor.entries` × 2
- `es:ReadonlyArray.every` × 2
- `es:ReadonlyArray.indexOf` × 2
- `es:ReadonlyMap.keys` × 2
- `es:ReadonlySet.has` × 2
- `es:String.length` × 2
- `es:Uint16Array` × 2
- `es:Uint16Array.set` × 2
- `es:Uint32Array` × 2
- `three:BufferAttribute.addUpdateRange` × 2
- `three:BufferAttribute.onUpload` × 2
- `three:BufferAttribute.onUploadCallback` × 2
- `three:BufferGeometry.applyMatrix4` × 2
- `three:BufferGeometry.boundingBox` × 2
- `three:BufferGeometry.setDrawRange` × 2
- `three:Color.clone` × 2
- `three:Color.multiplyScalar` × 2
- `three:Float32BufferAttribute` × 2
- `three:Frustum.intersectsBox` × 2
- `three:GeometryGroup.start` × 2
- `three:InstancedBufferGeometry.instanceCount` × 2
- `three:InterleavedBuffer.needsUpdate` × 2
- `three:Matrix4.compose` × 2
- `three:Matrix4.makeRotationY` × 2
- `three:Matrix4.toArray` × 2
- `three:mergeGeometries` × 2
- `three:Object3D.customDepthMaterial` × 2
- `three:Object3D.matrixWorld` × 2
- `three:Object3D.onBeforeShadow` × 2
- `three:Object3D.position` × 2
- `three:Object3D.receiveShadow` × 2
- `three:Object3D.rotation` × 2
- `three:Object3D.scale` × 2
- `three:OrthographicCamera.isOrthographicCamera` × 2
- `three:PlaneGeometry` × 2
- `three:Quaternion` × 2
- `three:SphereGeometry` × 2
- `three:Texture.repeat` × 2
- `three:WebGLRenderer` × 2
- `three:WebGLRenderer.getDrawingBufferSize` × 2
- `clipper2-ts:EndType` × 1
- `clipper2-ts:EndType.Polygon` × 1
- `clipper2-ts:FillRule` × 1
- `clipper2-ts:FillRule.NonZero` × 1
- `clipper2-ts:inflatePathsD` × 1
- `clipper2-ts:JoinType` × 1
- `clipper2-ts:JoinType.Round` × 1
- `clipper2-ts:PathsD` × 1
- `clipper2-ts:unionD` × 1
- `dom:CanvasFillStrokeStyles.createRadialGradient` × 1
- `dom:CanvasPath.ellipse` × 1
- `dom:CanvasPathDrawingStyles.lineCap` × 1
- `dom:CanvasRect.clearRect` × 1
- `dom:CanvasState.restore` × 1
- `dom:CanvasState.save` × 1
- `dom:CanvasText.measureText` × 1
- `dom:CanvasTextDrawingStyles.textAlign` × 1
- `dom:CanvasTextDrawingStyles.textBaseline` × 1
- `dom:CanvasTransform.rotate` × 1
- `dom:CanvasTransform.translate` × 1
- `dom:Storage.getItem` × 1
- `dom:Storage.setItem` × 1
- `dom:TextMetrics.width` × 1
- `es:Array.fill` × 1
- `es:Array.findIndex` × 1
- `es:Array.flatMap` × 1
- `es:Array.reduce` × 1
- `es:Array.reverse` × 1
- `es:Array.shift` × 1
- `es:Array.slice` × 1
- `es:Array.some` × 1
- `es:Error` × 1
- `es:Exclude` × 1
- `es:Float64Array.fill` × 1
- `es:Function.length` × 1
- `es:Generator` × 1
- `es:Int32Array.length` × 1
- `es:Int32ArrayConstructor.from` × 1
- `es:IterableIterator` × 1
- `es:Map.size` × 1
- `es:Math.acos` × 1
- `es:Math.log10` × 1
- `es:Math.sign` × 1
- `es:Math.tan` × 1
- `es:ObjectConstructor.is` × 1
- `es:ObjectConstructor.values` × 1
- `es:Omit` × 1
- `es:ReadonlyArray.flatMap` × 1
- `es:ReadonlyMap.size` × 1
- `es:ReadonlySet` × 1
- `es:Set.clear` × 1
- `es:String` × 1
- `es:String.charCodeAt` × 1
- `es:Uint32Array.fill` × 1
- `es:Uint8Array.forEach` × 1
- `three:Box3.expandByPoint` × 1
- `three:Box3.getBoundingSphere` × 1
- `three:Box3.getCenter` × 1
- `three:Box3.getSize` × 1
- `three:Box3.intersectsBox` × 1
- `three:BufferAttribute.getComponent` × 1
- `three:BufferAttribute.getX` × 1
- `three:BufferAttribute.itemSize` × 1
- `three:BufferAttribute.normalized` × 1
- `three:BufferGeometry.clearGroups` × 1
- `three:BufferGeometry.clone` × 1
- `three:BufferGeometry.groups` × 1
- `three:BufferGeometry.toNonIndexed` × 1
- `three:BufferGeometry.translate` × 1
- `three:Camera.matrixWorldInverse` × 1
- `three:Color.setHex` × 1
- `three:DataTexture.magFilter` × 1
- `three:DataTexture.minFilter` × 1
- `three:Euler` × 1
- `three:Euler.set` × 1
- `three:Frustum.intersectsSphere` × 1
- `three:GeometryGroup.count` × 1
- `three:GeometryGroup.materialIndex` × 1
- `three:IcosahedronGeometry` × 1
- `three:InterleavedBuffer.addUpdateRange` × 1
- `three:InterleavedBuffer.clearUpdateRanges` × 1
- `three:LinearFilter` × 1
- `three:LOD` × 1
- `three:Material.needsUpdate` × 1
- `three:Matrix4.copy` × 1
- `three:Matrix4.invert` × 1
- `three:Matrix4.makeRotationZ` × 1
- `three:Matrix4.scale` × 1
- `three:Mesh.material` × 1
- `three:MeshBasicMaterial` × 1
- `three:MeshStandardMaterialProperties.map` × 1
- `three:Object3D.matrix` × 1
- `three:Object3D.renderOrder` × 1
- `three:Object3D.updateMatrix` × 1
- `three:OrthographicCamera.left` × 1
- `three:OrthographicCamera.right` × 1
- `three:OrthographicCamera.zoom` × 1
- `three:PerspectiveCamera` × 1
- `three:PerspectiveCamera.far` × 1
- `three:Quaternion.setFromAxisAngle` × 1
- `three:Quaternion.setFromEuler` × 1
- `three:RenderTarget.width` × 1
- `three:RepeatWrapping` × 1
- `three:RGBAFormat` × 1
- `three:Scene` × 1
- `three:ShaderChunk` × 1
- `three:ShaderChunk.project_vertex` × 1
- `three:Texture` × 1
- `three:Texture.anisotropy` × 1
- `three:Texture.image` × 1
- `three:Texture.onUpdate` × 1
- `three:Texture.wrapS` × 1
- `three:Texture.wrapT` × 1
- `three:uniforms.__index` × 1
- `three:Vector2` × 1
- `three:Vector2.set` × 1
- `three:Vector3.add` × 1
- `three:Vector3.distanceTo` × 1
- `three:Vector3.multiplyScalar` × 1
- `three:Vector3.normalize` × 1
- `three:WebGLRenderer.getRenderTarget` × 1
