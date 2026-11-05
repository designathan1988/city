# Feature: src/ui/v2/shell.ts, src/ui/v2/start.ts, src/ui/v2/compass.ts, src/ui/v2/transitLine.ts, src/ui/about.ts, src/ui/buildStamp.ts, src/ui/chrome.ts, src/ui/effectsPanel.ts, src/ui/graphicsPanel.ts, src/ui/healthPanel.ts, src/ui/healthWatch.ts, src/ui/inspector.ts, src/ui/labels.ts, src/ui/profilerHud.ts, src/ui/thunder.ts, src/ui/toolChoices.ts, src/ui/i18n/index.ts, src/view/camera.ts, src/view/cameraGestures.ts, src/view/cameraMotion.ts, src/view/cameraProfile.ts, src/view/viewport.ts, src/editor/history.ts, src/editor/persistence.ts, src/editor/saveStore.ts, src/editor/saveBinary.ts, src/editor/chunkedSave.ts, src/editor/gameKeys.ts, src/desktopSaves.ts, src/desktopShown.ts, src/render/quality.ts, src/render/optimizations.ts, src/render/optimizationsApply.ts, src/render/effectSwitches.ts, src/steamAchievements.ts, src/core/dynamicResolution.ts, src/render/revealGate.ts, src/editor/cookedCities.ts, src/render/isoViewport.ts, src/render/occlusion.ts, src/render/canyonLight.ts

The program starts from `src/main.ts`, `src/peopleLab/main.ts`. A declaration is the feature's own when the program reaches it only through the feature; in a declaration of the program that names the feature, its statements are a slice (what names it, what depends on that, what that controls).

- Own: 1259 declarations
- Slices of shared code: 268
- Code: 20391 lines in 114 files (source/)
- Resources: 817 (187 entry, 630 style, 0 import; 379 only the feature uses)
- Uses: 481 shared declarations (interface/: 1273 declarations in 254 files, the types they name included); hooks: 296; external APIs: 593

## Code

- src/actionsWiring.ts — part, 59 lines
- src/buildingsWiring.ts — part, 128 lines
- src/core/commands.ts — part, 8 lines
- src/core/dynamicResolution.ts — whole, 64 lines
- src/core/health.ts — part, 34 lines
- src/core/perfBudget.ts — part, 9 lines
- src/desktopSaves.ts — whole, 152 lines
- src/desktopShown.ts — whole, 20 lines
- src/editor/buildingTool.ts — part, 12 lines
- src/editor/chunkedSave.ts — whole, 801 lines
- src/editor/cookedCities.ts — whole, 53 lines
- src/editor/gameKeys.ts — whole, 149 lines
- src/editor/history.ts — whole, 193 lines
- src/editor/mapExport.ts — part, 16 lines
- src/editor/persistence.ts — whole, 663 lines
- src/editor/regionFrames.ts — whole, 69 lines
- src/editor/repair.ts — whole, 73 lines
- src/editor/save.worker.ts — part, 9 lines
- src/editor/saveBinary.ts — whole, 109 lines
- src/editor/saveStore.ts — whole, 157 lines
- src/editor/workerParts.ts — part, 37 lines
- src/effectsWiring.ts — part, 144 lines
- src/graphicsWiring.ts — part, 128 lines
- src/lab/catalog/city.ts — part, 12 lines
- src/lab/inspector/entity.ts — part, 7 lines
- src/lab/inspector/raycast.ts — part, 5 lines
- src/lab/inspector/related.ts — part, 24 lines
- src/lab/optimizations/index.ts — part, 72 lines
- src/lab/recorder/recording.ts — part, 14 lines
- src/lab/recorder/report.ts — part, 7 lines
- src/lab/tabs.ts — part, 6 lines
- src/lab/tests/benchmarks.ts — part, 15 lines
- src/lab/tests/items.ts — part, 17 lines
- src/lab/tests/measure.ts — part, 9 lines
- src/main.ts — part, 1425 lines
- src/render/agents.ts — part, 53 lines
- src/render/buildings/frameBudget.ts — part, 3 lines
- src/render/buildings/layer.ts — part, 17 lines
- src/render/buildingSwitches.ts — part, 31 lines
- src/render/canyonLight.ts — whole, 379 lines
- src/render/densityCars.ts — part, 3 lines
- src/render/effectSwitches.ts — whole, 65 lines
- src/render/environment.ts — part, 2 lines
- src/render/isoViewport.ts — whole, 461 lines
- src/render/occlusion.ts — whole, 261 lines
- src/render/optimizations.ts — whole, 377 lines
- src/render/optimizationsApply.ts — whole, 263 lines
- src/render/optimizationsQuality.ts — whole, 115 lines
- src/render/people/proceduralCrowd.ts — part, 6 lines
- src/render/postprocess.ts — part, 275 lines
- src/render/quality.ts — whole, 344 lines
- src/render/renderer.ts — part, 937 lines
- src/render/revealGate.ts — whole, 42 lines
- src/render/sunCascades.ts — part, 13 lines
- src/render/vehicleLevels.ts — part, 4 lines
- src/sim/host/simClient.ts — part, 15 lines
- src/steamAchievements.ts — whole, 25 lines
- src/ui/about.ts — whole, 25 lines
- src/ui/builder/workspace.ts — part, 69 lines
- src/ui/buildStamp.ts — whole, 47 lines
- src/ui/chrome.ts — whole, 16 lines
- src/ui/effectsPanel.ts — whole, 181 lines
- src/ui/graphicsPanel.ts — whole, 112 lines
- src/ui/healthPanel.ts — whole, 267 lines
- src/ui/healthWatch.ts — whole, 204 lines
- src/ui/i18n/en.ts — whole, 1866 lines
- src/ui/i18n/index.ts — whole, 142 lines
- src/ui/i18n/pt-BR.ts — whole, 1856 lines
- src/ui/inspector.ts — whole, 520 lines
- src/ui/labels.ts — whole, 21 lines
- src/ui/materialSwatch.ts — whole, 212 lines
- src/ui/overlay/buildingOverlay.ts — part, 14 lines
- src/ui/planSwatch.ts — whole, 88 lines
- src/ui/profilerHud.ts — whole, 101 lines
- src/ui/roads/catalogPanel.ts — whole, 187 lines
- src/ui/roads/connectorPanel.ts — whole, 177 lines
- src/ui/roads/crossSection.ts — whole, 318 lines
- src/ui/roads/cutWalls.ts — part, 5 lines
- src/ui/roads/furnitureChoice.ts — part, 5 lines
- src/ui/roads/junctionPanel.ts — whole, 326 lines
- src/ui/roads/keys.ts — part, 34 lines
- src/ui/roads/keysPanel.ts — whole, 100 lines
- src/ui/roads/money.ts — part, 9 lines
- src/ui/roads/profileEditor.ts — whole, 755 lines
- src/ui/roads/profilePanel.ts — whole, 84 lines
- src/ui/roads/profileSwatch.ts — whole, 137 lines
- src/ui/roads/templates.ts — whole, 54 lines
- src/ui/roadSectionEditor.ts — part, 114 lines
- src/ui/thunder.ts — whole, 74 lines
- src/ui/toolChoices.ts — whole, 234 lines
- src/ui/v2/compass.ts — whole, 118 lines
- src/ui/v2/shell.ts — whole, 2085 lines
- src/ui/v2/start.ts — whole, 77 lines
- src/ui/v2/transitLine.ts — whole, 17 lines
- src/view/camera.ts — whole, 82 lines
- src/view/cameraGestures.ts — whole, 334 lines
- src/view/cameraMotion.ts — whole, 196 lines
- src/view/cameraProfile.ts — whole, 58 lines
- src/view/viewport.ts — whole, 105 lines
- src/world/buildings/cameraSolids.ts — part, 48 lines
- src/world/buildings/lotLibrary.ts — part, 1 lines
- src/world/buildings/sections.ts — part, 3 lines
- src/world/buildings/serialize.ts — part, 1 lines
- src/world/buildings/store.ts — part, 3 lines
- src/world/doc.ts — part, 167 lines
- src/world/docBinary.ts — part, 36 lines
- src/world/docImage.ts — whole, 26 lines
- src/world/roads/catalog.ts — whole, 110 lines
- src/world/roads/connectors.ts — part, 16 lines
- src/world/roads/profile.ts — part, 2 lines
- src/world/roads/streetNames.ts — part, 52 lines
- src/world/roads/templates.ts — whole, 90 lines
- src/world/terrain.ts — part, 3 lines
- src/world/viewCells.ts — part, 16 lines

## Slices of shared code

- `src/actionsWiring.ts#ActionsDeps` — 3 of 23 lines
- `src/actionsWiring.ts#createActions.buildingsUnderRay` — 4 of 6 lines
- `src/actionsWiring.ts#createActions.rayOnBuilding` — 10 of 17 lines
- `src/actionsWiring.ts#createActions.shootAt` — 42 of 58 lines
- `src/buildingsWiring.ts#BuildingWiringDeps` — 4 of 17 lines
- `src/buildingsWiring.ts#createBuildingWiring` — 61 of 1136 lines
- `src/buildingsWiring.ts#createBuildingWiring.beforeDraw` — 11 of 27 lines
- `src/buildingsWiring.ts#createBuildingWiring.drawOverlay` — 18 of 40 lines
- `src/buildingsWiring.ts#createBuildingWiring.inspectorFields` — 8 of 95 lines
- `src/buildingsWiring.ts#createBuildingWiring.loadReference` — 9 of 17 lines
- `src/buildingsWiring.ts#createBuildingWiring.materialLine` — 3 of 4 lines
- `src/buildingsWiring.ts#createBuildingWiring.selectionName` — 7 of 10 lines
- `src/buildingsWiring.ts#createBuildingWiring.state` — 7 of 48 lines
- `src/editor/buildingTool.ts#BuildingTool.interiorPreview` — 12 of 17 lines
- `src/editor/mapExport.ts#exportToFile` — 9 of 17 lines
- `src/editor/mapExport.ts#mapFileBlob` — 7 of 7 lines
- `src/editor/save.worker.ts#SaveMessage` — 1 of 1 lines
- `src/editor/save.worker.ts#serve` — 8 of 12 lines
- `src/editor/workerParts.ts#backend` — 1 of 1 lines
- `src/editor/workerParts.ts#give` — 1 of 1 lines
- `src/editor/workerParts.ts#portParts` — 2 of 33 lines
- `src/editor/workerParts.ts#takeStoreSetup` — 3 of 8 lines
- `src/editor/workerParts.ts#workerParts` — 2 of 8 lines
- `src/effectsWiring.ts#BUILDING_CHOICES` — 5 of 5 lines
- `src/effectsWiring.ts#EffectsWiring` — 3 of 24 lines
- `src/effectsWiring.ts#Kept` — 4 of 6 lines
- `src/effectsWiring.ts#OptimizationControl` — 4 of 11 lines
- `src/effectsWiring.ts#OptimizationState` — 3 of 5 lines
- `src/effectsWiring.ts#createEffectsWiring` — 65 of 196 lines
- `src/effectsWiring.ts#createEffectsWiring.attach` — 3 of 8 lines
- `src/effectsWiring.ts#createEffectsWiring.isOn` — 1 of 1 lines
- `src/effectsWiring.ts#createEffectsWiring.saveOptimizations` — 7 of 8 lines
- `src/effectsWiring.ts#createEffectsWiring.set` — 6 of 13 lines
- `src/effectsWiring.ts#createEffectsWiring.setOptimizations` — 8 of 12 lines
- `src/effectsWiring.ts#createEffectsWiring.stats` — 16 of 27 lines
- `src/effectsWiring.ts#readKept` — 8 of 13 lines
- `src/effectsWiring.ts#readOptimizations` — 11 of 13 lines
- `src/graphicsWiring.ts#GraphicsWiring` — 4 of 16 lines
- `src/graphicsWiring.ts#QualityPreset` — 1 of 1 lines
- `src/graphicsWiring.ts#createGraphicsWiring` — 89 of 165 lines
- `src/graphicsWiring.ts#createGraphicsWiring.apply` — 3 of 8 lines
- `src/graphicsWiring.ts#createGraphicsWiring.choices` — 10 of 10 lines
- `src/graphicsWiring.ts#createGraphicsWiring.fullRatio` — 1 of 1 lines
- `src/graphicsWiring.ts#isQualityPreset` — 1 of 1 lines
- `src/graphicsWiring.ts#pick` — 5 of 7 lines
- `src/graphicsWiring.ts#settingsFor` — 14 of 14 lines
- `src/lab/catalog/city.ts#drawCallsByGroup` — 12 of 29 lines
- `src/lab/inspector/entity.ts#entitiesAt` — 7 of 36 lines
- `src/lab/inspector/raycast.ts#rayAtPixel` — 5 of 6 lines
- `src/lab/inspector/related.ts#BUILDING_ROOTS` — 7 of 7 lines
- `src/lab/inspector/related.ts#LAYER_BY_NAME` — 15 of 15 lines
- `src/lab/inspector/related.ts#buildingKindSwitch` — 2 of 10 lines
- `src/lab/optimizations/index.ts#createOptimizationTabs` — 5 of 189 lines
- `src/lab/optimizations/index.ts#createOptimizationTabs.drawList` — 12 of 32 lines
- `src/lab/optimizations/index.ts#createOptimizationTabs.rowElement` — 46 of 66 lines
- `src/lab/optimizations/index.ts#rowOf` — 9 of 10 lines
- `src/lab/recorder/recording.ts#startRecording.onFrame` — 14 of 44 lines
- `src/lab/recorder/report.ts#buildReport` — 7 of 161 lines
- `src/lab/tabs.ts#createLabTabs` — 6 of 21 lines
- `src/lab/tests/benchmarks.ts#stress` — 15 of 38 lines
- `src/lab/tests/items.ts#SWITCH_GROUPS` — 4 of 4 lines
- `src/lab/tests/items.ts#sweepItems` — 13 of 51 lines
- `src/lab/tests/measure.ts#holdScene` — 5 of 21 lines
- `src/lab/tests/measure.ts#holdScene.release` — 4 of 6 lines
- `src/main.ts#<ExpressionStatement>` — 1 of 1 lines
- `src/main.ts#<ExpressionStatement>@160` — 1 of 1 lines
- `src/main.ts#<ExpressionStatement>@1611` — 1 of 1 lines
- `src/main.ts#<ExpressionStatement>@1674` — 1 of 1 lines
- `src/main.ts#<ExpressionStatement>@1984` — 1 of 1 lines
- `src/main.ts#<ExpressionStatement>@1985` — 1 of 1 lines
- `src/main.ts#<ExpressionStatement>@1986` — 1 of 1 lines
- `src/main.ts#<ExpressionStatement>@2406` — 1 of 1 lines
- `src/main.ts#<ExpressionStatement>@2556` — 6 of 8 lines
- `src/main.ts#<ExpressionStatement>@2564` — 1 of 1 lines
- `src/main.ts#<ExpressionStatement>@2566` — 1 of 1 lines
- `src/main.ts#<ExpressionStatement>@2617` — 1 of 1 lines
- `src/main.ts#<ExpressionStatement>@385` — 1 of 1 lines
- `src/main.ts#<ExpressionStatement>@386` — 1 of 1 lines
- `src/main.ts#<ExpressionStatement>@387` — 1 of 1 lines
- `src/main.ts#<ExpressionStatement>@388` — 1 of 1 lines
- `src/main.ts#<ExpressionStatement>@389` — 1 of 1 lines
- `src/main.ts#<ExpressionStatement>@3901` — 3 of 3 lines
- `src/main.ts#<ExpressionStatement>@3904` — 1 of 1 lines
- `src/main.ts#<ExpressionStatement>@3908` — 6 of 6 lines
- `src/main.ts#<ExpressionStatement>@3923` — 8 of 8 lines
- `src/main.ts#<ExpressionStatement>@744` — 1 of 1 lines
- `src/main.ts#<ExpressionStatement>@745` — 1 of 1 lines
- `src/main.ts#<ExpressionStatement>@874` — 1 of 1 lines
- `src/main.ts#<IfStatement>@1703` — 9 of 9 lines
- `src/main.ts#<IfStatement>@210` — 7 of 9 lines
- `src/main.ts#<IfStatement>@320` — 7 of 9 lines
- `src/main.ts#<IfStatement>@391` — 1 of 1 lines
- `src/main.ts#<IfStatement>@4036` — 46 of 196 lines
- `src/main.ts#<TryStatement>@1681` — 8 of 9 lines
- `src/main.ts#CanvasInput` — 5 of 5 lines
- `src/main.ts#MAP_ACTIONS` — 9 of 20 lines
- `src/main.ts#SLIDERS` — 57 of 57 lines
- `src/main.ts#achievements` — 1 of 1 lines
- `src/main.ts#actions` — 5 of 16 lines
- `src/main.ts#afterRestore` — 4 of 11 lines
- `src/main.ts#applyAtmosphere` — 10 of 12 lines
- `src/main.ts#applySnapshot` — 7 of 14 lines
- `src/main.ts#barrierTool` — 7 of 14 lines
- `src/main.ts#bootSave` — 1 of 1 lines
- `src/main.ts#bootSettings` — 1 of 1 lines
- `src/main.ts#bootZoomBounds` — 3 of 3 lines
- `src/main.ts#brushSlider` — 2 of 2 lines
- `src/main.ts#buildSummary` — 7 of 9 lines
- `src/main.ts#buildings` — 24 of 47 lines
- `src/main.ts#camera` — 1 of 1 lines
- `src/main.ts#cameraCommand` — 11 of 14 lines
- `src/main.ts#cameraHand` — 7 of 7 lines
- `src/main.ts#cameraMotion` — 5 of 5 lines
- `src/main.ts#cancelGestures` — 3 of 13 lines
- `src/main.ts#captureLost` — 3 of 4 lines
- `src/main.ts#cityAhead` — 4 of 17 lines
- `src/main.ts#cityData` — 8 of 21 lines
- `src/main.ts#closeInspector` — 3 of 3 lines
- `src/main.ts#cloudTool` — 9 of 14 lines
- `src/main.ts#coalescedPath` — 2 of 4 lines
- `src/main.ts#comesBackFromLostContext` — 11 of 18 lines
- `src/main.ts#compass` — 1 of 1 lines
- `src/main.ts#currentGesture` — 4 of 18 lines
- `src/main.ts#currentStatus` — 6 of 20 lines
- `src/main.ts#dabSettings` — 10 of 15 lines
- `src/main.ts#desktopShown` — 5 of 5 lines
- `src/main.ts#desktopStores` — 1 of 1 lines
- `src/main.ts#drawOverlayScreen` — 119 of 469 lines
- `src/main.ts#drawOverlayScreen.at` — 1 of 1 lines
- `src/main.ts#drawPolePlan` — 7 of 50 lines
- `src/main.ts#drawStreetscapeHover` — 3 of 40 lines
- `src/main.ts#endPointer` — 19 of 30 lines
- `src/main.ts#endTerrainStroke` — 3 of 11 lines
- `src/main.ts#firstSurfaceAt.at` — 1 of 1 lines
- `src/main.ts#fitOpeningView` — 4 of 11 lines
- `src/main.ts#fitView` — 3 of 4 lines
- `src/main.ts#flashHint` — 5 of 5 lines
- `src/main.ts#frame` — 20 of 102 lines
- `src/main.ts#gameKeys` — 77 of 77 lines
- `src/main.ts#gameState` — 5 of 38 lines
- `src/main.ts#groundHeightUnder.point` — 1 of 1 lines
- `src/main.ts#groundOnScreen` — 14 of 19 lines
- `src/main.ts#growZones` — 4 of 10 lines
- `src/main.ts#growthAnswered` — 5 of 15 lines
- `src/main.ts#healthPanel` — 16 of 16 lines
- `src/main.ts#healthWatch` — 15 of 15 lines
- `src/main.ts#history` — 1 of 1 lines
- `src/main.ts#lab` — 1 of 1 lines
- `src/main.ts#laneChoices` — 5 of 9 lines
- `src/main.ts#listenHint` — 4 of 6 lines
- `src/main.ts#lotTool` — 3 of 14 lines
- `src/main.ts#mapSlider` — 2 of 14 lines
- `src/main.ts#mountInterface` — 85 of 88 lines
- `src/main.ts#mutateBuilt` — 8 of 36 lines
- `src/main.ts#networkBakeOf` — 7 of 17 lines
- `src/main.ts#newLargeWorld` — 3 of 5 lines
- `src/main.ts#newMap` — 7 of 13 lines
- `src/main.ts#newWorld` — 3 of 5 lines
- `src/main.ts#nextCitySeed` — 5 of 10 lines
- `src/main.ts#noteSimulationIssues` — 6 of 9 lines
- `src/main.ts#openCity` — 4 of 15 lines
- `src/main.ts#openImported` — 9 of 9 lines
- `src/main.ts#openMap` — 3 of 3 lines
- `src/main.ts#openSession` — 11 of 21 lines
- `src/main.ts#pageHidden` — 3 of 3 lines
- `src/main.ts#persistence` — 1 of 1 lines
- `src/main.ts#pointerDown` — 94 of 180 lines
- `src/main.ts#pointerMove` — 12 of 72 lines
- `src/main.ts#poleTool` — 5 of 11 lines
- `src/main.ts#redo` — 5 of 8 lines
- `src/main.ts#releaseHeld` — 3 of 5 lines
- `src/main.ts#restoreBootSave` — 14 of 29 lines
- `src/main.ts#restoreOrbit` — 3 of 3 lines
- `src/main.ts#restoreSettings` — 16 of 20 lines
- `src/main.ts#roadClasses` — 6 of 9 lines
- `src/main.ts#roadTool` — 8 of 29 lines
- `src/main.ts#saveOnClose` — 10 of 14 lines
- `src/main.ts#saveOnceShown` — 3 of 7 lines
- `src/main.ts#scene` — 1 of 1 lines
- `src/main.ts#screenReach` — 4 of 4 lines
- `src/main.ts#serializedDoc` — 5 of 8 lines
- `src/main.ts#sessionSettings` — 11 of 13 lines
- `src/main.ts#setDemand` — 4 of 5 lines
- `src/main.ts#setMapBiome` — 4 of 10 lines
- `src/main.ts#setMarkingStyle` — 5 of 9 lines
- `src/main.ts#setPaused` — 4 of 7 lines
- `src/main.ts#setSpeed` — 6 of 8 lines
- `src/main.ts#setZone` — 13 of 14 lines
- `src/main.ts#shell` — 1 of 1 lines
- `src/main.ts#showInspector` — 246 of 246 lines
- `src/main.ts#snapshotApplied` — 6 of 12 lines
- `src/main.ts#statusListeners` — 1 of 1 lines
- `src/main.ts#streetscapeTool` — 7 of 12 lines
- `src/main.ts#syncFlatCameraFromView` — 7 of 7 lines
- `src/main.ts#syncViewFromFlatCamera` — 10 of 10 lines
- `src/main.ts#terrainBrush` — 3 of 11 lines
- `src/main.ts#toggleCongestion` — 3 of 4 lines
- `src/main.ts#undo` — 5 of 9 lines
- `src/main.ts#updateCompass` — 3 of 3 lines
- `src/main.ts#updateHint` — 6 of 6 lines
- `src/main.ts#updateHistoryButtons` — 4 of 4 lines
- `src/main.ts#view` — 1 of 1 lines
- `src/main.ts#weatherSlider` — 2 of 2 lines
- `src/main.ts#wheel` — 5 of 16 lines
- `src/main.ts#wholeMapEdit` — 6 of 9 lines
- `src/main.ts#wireProfiler` — 3 of 43 lines
- `src/main.ts#worldAtScreen` — 12 of 13 lines
- `src/render/agents.ts#AgentRenderOptions` — 3 of 30 lines
- `src/render/agents.ts#VEHICLE_NEAR_CAP` — 1 of 1 lines
- `src/render/agents.ts#createAgentMeshes` — 4 of 2045 lines
- `src/render/agents.ts#createAgentMeshes.sync` — 38 of 421 lines
- `src/render/agents.ts#standingRange` — 7 of 14 lines
- `src/render/buildingSwitches.ts#BuildingSwitcher` — 4 of 9 lines
- `src/render/buildingSwitches.ts#KIND_SWITCH` — 10 of 10 lines
- `src/render/buildingSwitches.ts#KIND_SWITCHES` — 1 of 1 lines
- `src/render/buildingSwitches.ts#ROOT_SWITCH` — 8 of 8 lines
- `src/render/buildingSwitches.ts#createBuildingSwitcher.beforeRender` — 6 of 28 lines
- `src/render/buildingSwitches.ts#kindOf` — 2 of 2 lines
- `src/render/buildings/frameBudget.ts#FrameScale.fit` — 3 of 3 lines
- `src/render/buildings/layer.ts#createBuildingLayer.capTriangles` — 8 of 19 lines
- `src/render/buildings/layer.ts#createBuildingLayer.levelOfDetail` — 6 of 55 lines
- `src/render/buildings/layer.ts#createBuildingLayer.levels` — 3 of 15 lines
- `src/render/densityCars.ts#createDensityCars.update` — 3 of 21 lines
- `src/render/people/proceduralCrowd.ts#createProceduralCrowd` — 3 of 1581 lines
- `src/render/people/proceduralCrowd.ts#createProceduralCrowd.update` — 3 of 106 lines
- `src/render/postprocess.ts#PostChain` — 3 of 30 lines
- `src/render/postprocess.ts#createPostChain` — 259 of 493 lines
- `src/render/postprocess.ts#createPostChain.disposeAll` — 3 of 18 lines
- `src/render/postprocess.ts#createPostChain.followTown` — 3 of 3 lines
- `src/render/postprocess.ts#createPostChain.render` — 4 of 35 lines
- `src/render/postprocess.ts#createPostChain.setAtmosphere` — 3 of 12 lines
- `src/render/renderer.ts#RenderStats` — 3 of 19 lines
- `src/render/renderer.ts#SceneHandle` — 9 of 170 lines
- `src/render/renderer.ts#createSceneRenderer` — 799 of 3774 lines
- `src/render/renderer.ts#createSceneRenderer.applyGeometryBudget` — 6 of 8 lines
- `src/render/renderer.ts#createSceneRenderer.applyQuality` — 17 of 17 lines
- `src/render/renderer.ts#createSceneRenderer.budgetNow` — 1 of 1 lines
- `src/render/renderer.ts#createSceneRenderer.buildWorld` — 13 of 78 lines
- `src/render/renderer.ts#createSceneRenderer.compileChanged` — 8 of 18 lines
- `src/render/renderer.ts#createSceneRenderer.completeSolve` — 3 of 24 lines
- `src/render/renderer.ts#createSceneRenderer.fillStep` — 3 of 25 lines
- `src/render/renderer.ts#createSceneRenderer.governGeometry` — 11 of 18 lines
- `src/render/renderer.ts#createSceneRenderer.loadEffects` — 5 of 18 lines
- `src/render/renderer.ts#createSceneRenderer.natureSweep` — 29 of 89 lines
- `src/render/renderer.ts#createSceneRenderer.occlusionHidden` — 1 of 1 lines
- `src/render/renderer.ts#createSceneRenderer.pumpWorld` — 6 of 19 lines
- `src/render/renderer.ts#createSceneRenderer.strikeAt` — 4 of 7 lines
- `src/render/renderer.ts#createSceneRenderer.switchedOff` — 16 of 16 lines
- `src/render/renderer.ts#createSceneRenderer.watchRoads` — 3 of 27 lines
- `src/render/vehicleLevels.ts#ERROR_PIXELS` — 1 of 1 lines
- `src/render/vehicleLevels.ts#levelFor` — 3 of 5 lines
- `src/sim/host/simClient.ts#SimClient.frame` — 7 of 22 lines
- `src/sim/host/simClient.ts#SimClient.paused` — 1 of 1 lines
- `src/ui/builder/workspace.ts#initBuilderWorkspace` — 4 of 255 lines
- `src/ui/builder/workspace.ts#initBuilderWorkspace.renderInspector` — 56 of 150 lines
- `src/ui/overlay/buildingOverlay.ts#drawBuildingOverlay` — 14 of 182 lines
- `src/ui/roads/keys.ts#cameraKey` — 3 of 3 lines
- `src/ui/roads/keys.ts#roadKeyAction` — 3 of 4 lines
- `src/ui/roads/keys.ts#roadKeyParams` — 3 of 4 lines
- `src/ui/roads/money.ts#formatCost` — 3 of 3 lines
- `src/ui/roads/money.ts#formatMoney` — 3 of 4 lines
- `src/world/doc.ts#RoadDoc.nodeToWrite` — 3 of 6 lines
- `src/world/doc.ts#RoadDoc.note` — 7 of 8 lines
- `src/world/doc.ts#RoadDoc.noteList` — 4 of 4 lines
- `src/world/doc.ts#RoadDoc.replaceWhole` — 3 of 11 lines
- `src/world/doc.ts#RoadDoc.replaceWith` — 3 of 32 lines
- `src/world/doc.ts#RoadDoc.segmentToWrite` — 3 of 6 lines
- `src/world/viewCells.ts#ViewCells.trim` — 16 of 23 lines

## Resources

- feature entry `primBox` — src/buildingsWiring.ts:1220: `primBox: 'box'`
- feature entry `primCylinder` — src/buildingsWiring.ts:1221: `primCylinder: 'cylinder'`
- feature entry `primOctagonal` — src/buildingsWiring.ts:1222: `primOctagonal: 'octagonal'`
- feature entry `primPrism` — src/buildingsWiring.ts:1223: `primPrism: 'prism'`
- feature entry `primWedge` — src/buildingsWiring.ts:1224: `primWedge: 'wedge'`
- feature entry `primPyramid` — src/buildingsWiring.ts:1225: `primPyramid: 'pyramid'`
- feature entry `primCone` — src/buildingsWiring.ts:1226: `primCone: 'cone'`
- feature entry `primCross` — src/buildingsWiring.ts:1227: `primCross: 'crossBlock'`
- shared entry `street` — src/core/perfBudget.ts:45: `street: { triangles: 16_000_000, drawCalls: 700 + 300, gpuMs: 12, fps: 60 }`
- shared entry `district` — src/core/perfBudget.ts:46: `district: { triangles: 16_000_000, drawCalls: 800 + 300, gpuMs: 12, fps: 60 }`
- shared entry `map` — src/core/perfBudget.ts:47: `map: { triangles: 8_000_000, drawCalls: 300 + 100, gpuMs: 12, fps: 50 }`
- feature entry `raise` — src/main.ts:384: `raise: 0`
- feature entry `lower` — src/main.ts:384: `lower: 0`
- feature entry `mesa` — src/main.ts:384: `mesa: 85`
- feature entry `canyon` — src/main.ts:384: `canyon: 80`
- feature entry `escarpment` — src/main.ts:384: `escarpment: 75`
- feature entry `clearTerrain` — src/main.ts:1799: `clearTerrain: () => wholeMapEdit('confirm.clearTerrain', () => doc.clearTerrain())`
- feature entry `clearGullies` — src/main.ts:1800: `clearGullies: () => { if (doc.gullyDabs.length > 0) wholeMapEdit('confirm.clearGullies', () => doc.clearGullies()); }`
- feature entry `clearFog` — src/main.ts:1801: `clearFog: () => { if (doc.fogDabs.length > 0) wholeMapEdit('confirm.clearFog', () => doc.clearFog()); }`
- feature entry `clearTrees` — src/main.ts:1802: `clearTrees: () => { if (doc.trees.length > 0 || doc.treeClearings.length > 0) wholeMapEdit('confirm.clearTrees', () => doc.clearTrees()); }`
- feature entry `clearElements` — src/main.ts:1805: `clearElements: () => { if (doc.elements.length > 0) wholeMapEdit('confirm.clearElements', () => doc.clearElements()); }`
- feature entry `clearClouds` — src/main.ts:1806: `clearClouds: () => { if (doc.clouds.length > 0) wholeMapEdit('confirm.clearClouds', () => { for (const cloud of [...doc.clouds]) doc.removeCloud(cloud.id); }); `
- feature entry `lambert` — src/render/buildingSwitches.ts:44: `lambert: new MeshLambertMaterial({ color: 0xa8a8a8 })`
- feature entry `basic` — src/render/buildingSwitches.ts:44: `basic: new MeshBasicMaterial({ color: 0x9a9a9a })`
- shared entry `renderer` — src/render/optimizationTypes.ts:9: `renderer: 'Renderer'`
- shared entry `culling` — src/render/optimizationTypes.ts:10: `culling: 'Culling'`
- shared entry `shadows` — src/render/optimizationTypes.ts:11: `shadows: 'Shadows'`
- shared entry `post` — src/render/optimizationTypes.ts:12: `post: 'Post-processing'`
- shared entry `textures` — src/render/optimizationTypes.ts:13: `textures: 'Textures'`
- shared entry `materials` — src/render/optimizationTypes.ts:14: `materials: 'Materials and shaders'`
- shared entry `buildings` — src/render/optimizationTypes.ts:15: `buildings: 'Buildings'`
- shared entry `terrain` — src/render/optimizationTypes.ts:16: `terrain: 'Terrain'`
- shared entry `water` — src/render/optimizationTypes.ts:17: `water: 'Water'`
- shared entry `roads` — src/render/optimizationTypes.ts:18: `roads: 'Roads'`
- shared entry `vegetation` — src/render/optimizationTypes.ts:19: `vegetation: 'Vegetation and street furniture'`
- shared entry `people` — src/render/optimizationTypes.ts:20: `people: 'People'`
- shared entry `vehicles` — src/render/optimizationTypes.ts:21: `vehicles: 'Vehicles'`
- shared entry `particles` — src/render/optimizationTypes.ts:22: `particles: 'Particles'`
- shared entry `simulation` — src/render/optimizationTypes.ts:23: `simulation: 'Simulation (CPU)'`
- shared entry `uploads` — src/render/optimizationTypes.ts:24: `uploads: 'GPU uploads and pacing'`
- shared entry `rect` — src/ui/builder/catalog.ts:126: `rect: 'rectangle'`
- shared entry `shapeL` — src/ui/builder/catalog.ts:127: `shapeL: 'l'`
- shared entry `shapeU` — src/ui/builder/catalog.ts:128: `shapeU: 'u'`
- shared entry `circle` — src/ui/builder/catalog.ts:129: `circle: 'circle'`
- shared entry `hexagon` — src/ui/builder/catalog.ts:130: `hexagon: 'hexagon'`
- shared entry `octagon` — src/ui/builder/catalog.ts:131: `octagon: 'octagon'`
- shared entry `chamfered` — src/ui/builder/catalog.ts:132: `chamfered: 'chamfered'`
- shared entry `shapeCross` — src/ui/builder/catalog.ts:133: `shapeCross: 'cross'`
- shared entry `shapeStepped` — src/ui/builder/catalog.ts:134: `shapeStepped: 'stepped'`
- feature entry `tierRect` — src/ui/builder/catalog.ts:139: `tierRect: 'rectangle'`
- feature entry `tierChamfered` — src/ui/builder/catalog.ts:140: `tierChamfered: 'chamfered'`
- feature entry `tierOctagon` — src/ui/builder/catalog.ts:141: `tierOctagon: 'octagon'`
- feature entry `tierHexagon` — src/ui/builder/catalog.ts:142: `tierHexagon: 'hexagon'`
- feature entry `tierCircle` — src/ui/builder/catalog.ts:143: `tierCircle: 'circle'`
- feature entry `tierL` — src/ui/builder/catalog.ts:144: `tierL: 'l'`
- feature entry `tierU` — src/ui/builder/catalog.ts:145: `tierU: 'u'`
- feature entry `tierCross` — src/ui/builder/catalog.ts:146: `tierCross: 'cross'`
- feature entry `tierStepped` — src/ui/builder/catalog.ts:147: `tierStepped: 'stepped'`
- feature entry `doubleDoor` — src/ui/builder/catalog.ts:152: `doubleDoor: 'doubleDoor'`
- feature entry `garageDoor` — src/ui/builder/catalog.ts:153: `garageDoor: 'garageDoor'`
- feature entry `frenchWindow` — src/ui/builder/catalog.ts:154: `frenchWindow: 'frenchWindow'`
- feature entry `bayWindow` — src/ui/builder/catalog.ts:155: `bayWindow: 'bayWindow'`
- feature entry `ribbon` — src/ui/builder/catalog.ts:156: `ribbon: 'ribbon'`
- feature entry `window` — src/ui/builder/catalog.ts:157: `window: 'window'`
- feature entry `sashWindow` — src/ui/builder/catalog.ts:158: `sashWindow: 'sashWindow'`
- feature entry `wideWindow` — src/ui/builder/catalog.ts:159: `wideWindow: 'wideWindow'`
- feature entry `balcony` — src/ui/builder/catalog.ts:160: `balcony: 'balcony'`
- feature entry `door` — src/ui/builder/catalog.ts:161: `door: 'door'`
- feature entry `shopfront` — src/ui/builder/catalog.ts:162: `shopfront: 'shopfront'`
- feature entry `loadingDoor` — src/ui/builder/catalog.ts:163: `loadingDoor: 'loadingDoor'`
- feature entry `pillarBay` — src/ui/builder/catalog.ts:164: `pillarBay: 'pillar'`
- feature entry `wallBay` — src/ui/builder/catalog.ts:165: `wallBay: 'wall'`
- feature entry `interiorView` — src/ui/builder/icons.ts:237: `interiorView: 'interiorView'`
- feature entry `floorDown` — src/ui/builder/icons.ts:238: `floorDown: 'floorDown'`
- feature entry `floorUp` — src/ui/builder/icons.ts:239: `floorUp: 'floorUp'`
- feature entry `coreLift` — src/ui/builder/icons.ts:240: `coreLift: 'coreLift'`
- feature entry `coreStair` — src/ui/builder/icons.ts:241: `coreStair: 'stair'`
- feature entry `coreBoth` — src/ui/builder/icons.ts:242: `coreBoth: 'coreLift'`
- feature entry `coreRemove` — src/ui/builder/icons.ts:243: `coreRemove: 'trash'`
- feature entry `furn_sofa` — src/ui/builder/icons.ts:244: `furn_sofa: 'furnSofa'`
- feature entry `furn_armchair` — src/ui/builder/icons.ts:245: `furn_armchair: 'furnSofa'`
- feature entry `furn_table` — src/ui/builder/icons.ts:246: `furn_table: 'furnTable'`
- feature entry `furn_chair` — src/ui/builder/icons.ts:247: `furn_chair: 'furnChair'`
- feature entry `furn_bed` — src/ui/builder/icons.ts:248: `furn_bed: 'furnBed'`
- feature entry `furn_singleBed` — src/ui/builder/icons.ts:249: `furn_singleBed: 'furnBed'`
- feature entry `furn_wardrobe` — src/ui/builder/icons.ts:250: `furn_wardrobe: 'furnShelf'`
- feature entry `furn_tv` — src/ui/builder/icons.ts:251: `furn_tv: 'furnTv'`
- feature entry `furn_desk` — src/ui/builder/icons.ts:252: `furn_desk: 'furnTable'`
- feature entry `furn_officeChair` — src/ui/builder/icons.ts:253: `furn_officeChair: 'furnChair'`
- feature entry `furn_bookshelf` — src/ui/builder/icons.ts:254: `furn_bookshelf: 'furnShelf'`
- feature entry `furn_shelf` — src/ui/builder/icons.ts:255: `furn_shelf: 'furnShelf'`
- feature entry `furn_counter` — src/ui/builder/icons.ts:256: `furn_counter: 'furnKitchen'`
- feature entry `furn_fridge` — src/ui/builder/icons.ts:257: `furn_fridge: 'furnKitchen'`
- feature entry `furn_stove` — src/ui/builder/icons.ts:258: `furn_stove: 'furnKitchen'`
- feature entry `furn_sink` — src/ui/builder/icons.ts:259: `furn_sink: 'furnKitchen'`
- feature entry `furn_bath` — src/ui/builder/icons.ts:260: `furn_bath: 'furnBath'`
- feature entry `furn_toilet` — src/ui/builder/icons.ts:261: `furn_toilet: 'furnBath'`
- feature entry `furn_plant` — src/ui/builder/icons.ts:262: `furn_plant: 'furnPlant'`
- feature entry `furn_ceilingLamp` — src/ui/builder/icons.ts:263: `furn_ceilingLamp: 'furnLamp'`
- feature entry `furn_floorLamp` — src/ui/builder/icons.ts:264: `furn_floorLamp: 'furnLamp'`
- feature entry `furn_tableLamp` — src/ui/builder/icons.ts:265: `furn_tableLamp: 'furnLamp'`
- feature entry `furnMove` — src/ui/builder/icons.ts:266: `furnMove: 'snap'`
- feature entry `furnTurn` — src/ui/builder/icons.ts:267: `furnTurn: 'redo'`
- feature entry `furnRemove` — src/ui/builder/icons.ts:268: `furnRemove: 'trash'`
- feature entry `tierRect` — src/ui/builder/icons.ts:269: `tierRect: 'rect'`
- feature entry `blockSolid` — src/ui/builder/icons.ts:270: `blockSolid: 'mass'`
- feature entry `primBox` — src/ui/builder/icons.ts:271: `primBox: 'mass'`
- feature entry `primCylinder` — src/ui/builder/icons.ts:272: `primCylinder: 'primCylinder'`
- feature entry `primOctagonal` — src/ui/builder/icons.ts:273: `primOctagonal: 'octagon'`
- feature entry `primPrism` — src/ui/builder/icons.ts:274: `primPrism: 'roofGable'`
- feature entry `primWedge` — src/ui/builder/icons.ts:275: `primWedge: 'roofShed'`
- feature entry `primPyramid` — src/ui/builder/icons.ts:276: `primPyramid: 'primPyramid'`
- feature entry `primCone` — src/ui/builder/icons.ts:277: `primCone: 'primCone'`
- feature entry `primCross` — src/ui/builder/icons.ts:278: `primCross: 'shapeCross'`
- feature entry `extrudeOut` — src/ui/builder/icons.ts:279: `extrudeOut: 'extrudeOut'`
- feature entry `extrudeIn` — src/ui/builder/icons.ts:280: `extrudeIn: 'extrudeIn'`
- feature entry `extrudeBlock` — src/ui/builder/icons.ts:281: `extrudeBlock: 'wing'`
- feature entry `insetFace` — src/ui/builder/icons.ts:282: `insetFace: 'inset'`
- feature entry `offsetOut` — src/ui/builder/icons.ts:283: `offsetOut: 'offsetOut'`
- feature entry `offsetIn` — src/ui/builder/icons.ts:284: `offsetIn: 'offsetIn'`
- feature entry `bevelAll` — src/ui/builder/icons.ts:285: `bevelAll: 'chamfered'`
- feature entry `pointMode` — src/ui/builder/icons.ts:286: `pointMode: 'pointMode'`
- feature entry `bevelCorner` — src/ui/builder/icons.ts:287: `bevelCorner: 'bevelCorner'`
- feature entry `blockVoid` — src/ui/builder/icons.ts:288: `blockVoid: 'blockVoid'`
- feature entry `blockIntersect` — src/ui/builder/icons.ts:289: `blockIntersect: 'blockIntersect'`
- feature entry `blockXor` — src/ui/builder/icons.ts:290: `blockXor: 'blockXor'`
- feature entry `copyBlock` — src/ui/builder/icons.ts:291: `copyBlock: 'duplicate'`
- feature entry `detachBlock` — src/ui/builder/icons.ts:292: `detachBlock: 'exit'`
- feature entry `centerBlock` — src/ui/builder/icons.ts:293: `centerBlock: 'snap'`
- feature entry `turnBlockLeft` — src/ui/builder/icons.ts:294: `turnBlockLeft: 'undo'`
- feature entry `turnBlockRight` — src/ui/builder/icons.ts:295: `turnBlockRight: 'redo'`
- feature entry `tierChamfered` — src/ui/builder/icons.ts:296: `tierChamfered: 'chamfered'`
- feature entry `tierOctagon` — src/ui/builder/icons.ts:297: `tierOctagon: 'octagon'`
- feature entry `tierHexagon` — src/ui/builder/icons.ts:298: `tierHexagon: 'hexagon'`
- feature entry `tierCircle` — src/ui/builder/icons.ts:299: `tierCircle: 'circle'`
- feature entry `tierL` — src/ui/builder/icons.ts:300: `tierL: 'shapeL'`
- feature entry `tierU` — src/ui/builder/icons.ts:301: `tierU: 'shapeU'`
- feature entry `tierCross` — src/ui/builder/icons.ts:302: `tierCross: 'shapeCross'`
- feature entry `tierStepped` — src/ui/builder/icons.ts:303: `tierStepped: 'shapeStepped'`
- feature entry `plaster` — src/world/buildings/materials.ts:31: `plaster: 0xe6e0d4`
- feature entry `stucco` — src/world/buildings/materials.ts:32: `stucco: 0xdcd4c4`
- feature entry `ceramic` — src/world/buildings/materials.ts:33: `ceramic: 0xcfd6da`
- feature entry `brick` — src/world/buildings/materials.ts:34: `brick: 0xa4563f`
- feature entry `stone` — src/world/buildings/materials.ts:35: `stone: 0xb7b2a6`
- feature entry `concrete` — src/world/buildings/materials.ts:36: `concrete: 0xb4b4b0`
- feature entry `wood` — src/world/buildings/materials.ts:37: `wood: 0x9c6b43`
- feature entry `metal` — src/world/buildings/materials.ts:38: `metal: 0x9aa3a8`
- feature entry `glass` — src/world/buildings/materials.ts:39: `glass: 0x9db6c2`
- feature entry `tile` — src/world/buildings/materials.ts:40: `tile: 0xbd6a52`
- feature entry `slate` — src/world/buildings/materials.ts:41: `slate: 0x585b5f`
- feature entry `panel` — src/world/buildings/materials.ts:42: `panel: 0x8f9ba5`
- feature entry `roofing` — src/world/buildings/materials.ts:43: `roofing: 0x7b4a37`
- feature entry `stones` — src/world/elements.ts:46: `stones: { density: 2, size: 1.2, variation: 60, spacing: 2, strength: 60, intensity: 50 }`
- feature entry `pebbles` — src/world/elements.ts:47: `pebbles: { density: 30, size: 0.3, variation: 50, spacing: 0.4, strength: 60, intensity: 50 }`
- feature entry `gravel` — src/world/elements.ts:48: `gravel: { density: 120, size: 0.12, variation: 40, spacing: 0.15, strength: 60, intensity: 50 }`
- feature entry `leaves` — src/world/elements.ts:49: `leaves: { density: 6, size: 1.6, variation: 40, spacing: 0.8, strength: 60, intensity: 50 }`
- feature entry `grass` — src/world/elements.ts:50: `grass: { density: 40, size: 0.5, variation: 40, spacing: 0.3, strength: 60, intensity: 50 }`
- feature entry `tallGrass` — src/world/elements.ts:51: `tallGrass: { density: 20, size: 0.9, variation: 40, spacing: 0.5, strength: 60, intensity: 50 }`
- feature entry `scrub` — src/world/elements.ts:52: `scrub: { density: 6, size: 1.2, variation: 50, spacing: 1, strength: 60, intensity: 50 }`
- feature entry `fern` — src/world/elements.ts:53: `fern: { density: 4, size: 1.1, variation: 40, spacing: 1, strength: 60, intensity: 50 }`
- feature entry `clover` — src/world/elements.ts:54: `clover: { density: 30, size: 0.25, variation: 40, spacing: 0.2, strength: 60, intensity: 50 }`
- feature entry `flowers` — src/world/elements.ts:55: `flowers: { density: 12, size: 0.5, variation: 40, spacing: 0.4, strength: 60, intensity: 50 }`
- feature entry `mushrooms` — src/world/elements.ts:56: `mushrooms: { density: 6, size: 0.25, variation: 50, spacing: 0.3, strength: 60, intensity: 50 }`
- feature entry `smoke` — src/world/elements.ts:57: `smoke: { density: 0.3, size: 3, variation: 30, spacing: 8, strength: 100, intensity: 60 }`
- feature entry `fire` — src/world/elements.ts:58: `fire: { density: 0.3, size: 2, variation: 30, spacing: 4, strength: 100, intensity: 60 }`
- feature entry `steam` — src/world/elements.ts:59: `steam: { density: 0.3, size: 2, variation: 30, spacing: 6, strength: 100, intensity: 50 }`
- feature entry `dust` — src/world/elements.ts:60: `dust: { density: 0.3, size: 4, variation: 30, spacing: 8, strength: 100, intensity: 40 }`
- feature entry `soot` — src/world/elements.ts:61: `soot: { density: 0.3, size: 3, variation: 30, spacing: 8, strength: 100, intensity: 50 }`
- feature entry `sparks` — src/world/elements.ts:62: `sparks: { density: 0.3, size: 1, variation: 30, spacing: 4, strength: 100, intensity: 50 }`
- feature entry `spray` — src/world/elements.ts:63: `spray: { density: 0.5, size: 2, variation: 30, spacing: 4, strength: 100, intensity: 50 }`
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
- shared entry `laneWidth` — src/world/roadSection.ts:65: `laneWidth: [m(2), m(6)]`
- shared entry `sidewalk` — src/world/roadSection.ts:66: `sidewalk: [m(1), m(12)]`
- shared entry `median` — src/world/roadSection.ts:67: `median: [0, m(8)]`
- shared entry `speedKmh` — src/world/roadSection.ts:68: `speedKmh: [10, 130]`
- shared entry `priority` — src/world/roadSection.ts:69: `priority: [0, 5]`
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
- shared style `lab-panel` — src/lab/lab.css:30: `.lab-panel:not(.wide) .lab-head { flex-wrap: wrap; }`
- shared style `lab-panel` — src/lab/lab.css:31: `.lab-panel:not(.wide) .lab-meters { order: 3; flex-basis: 100%; }`
- shared style `lab-panel` — src/lab/lab.css:32: `.lab-panel:not(.wide) .lab-meter { flex: 1 1 0; min-width: 64px; }`
- shared style `lab-panel` — src/lab/lab.css:33: `.lab-panel:not(.wide) .lab-title { flex: 1; }`
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
- shared style `lab-panel` — src/lab/lab.css:71: `.lab-panel:not(.wide) .lab-tabs { width: 46px; padding: 6px 4px; }`
- shared style `lab-panel` — src/lab/lab.css:72: `.lab-panel:not(.wide) .lab-tab { justify-content: center; padding: 7px 0 !important; }`
- shared style `lab-panel` — src/lab/lab.css:73: `.lab-panel:not(.wide) .lab-tab span { display: none; }`
- shared style `lab-panel` — src/lab/lab.css:74: `.lab-panel:not(.wide) .lab-tab-group { height: 1px; margin: 6px 4px; background: var(--lab-line2); color: transparent; overflow: hidden; }`
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
- shared style `lab-table` — src/lab/lab.css:121: `.lab-table td.num { text-align: right; }`
- shared style `lab-table` — src/lab/lab.css:122: `.lab-table td.bar { background: linear-gradient(to left, rgb(47 194 171 / 0.22) var(--bar), transparent var(--bar)); }`
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
- shared style `lab-inv-table` — src/lab/lab.css:152: `.lab-inv-table td.num { text-align: right; }`
- shared style `lab-inv-table` — src/lab/lab.css:153: `.lab-inv-table tr:hover td { background: rgb(47 194 171 / 0.12); cursor: pointer; }`
- shared style `lab-table` — src/lab/lab.css:154: `.lab-table td.lab-look { width: 24px; padding: 0 2px; }`
- shared style `lab-table` — src/lab/lab.css:155: `.lab-table td.lab-look button { padding: 2px 3px; border: 0; background: transparent; color: var(--lab-muted); opacity: 0.55; }`
- shared style `lab-table` — src/lab/lab.css:156: `.lab-table tbody tr:hover td.lab-look button { opacity: 1; color: var(--lab-accent); }`
- shared style `lab-table` — src/lab/lab.css:157: `.lab-table tbody tr { cursor: pointer; }`
- shared style `lab-investigate` — src/lab/lab.css:158: `.lab-investigate h3 { display: flex; align-items: center; gap: 6px; margin: 12px 0 6px; font-size: 0.92em; color: #a9bcc4; text-transform: uppercase; letter-spa`
- shared style `lab-investigate` — src/lab/lab.css:159: `.lab-investigate p { margin: 4px 0; }`
- feature style `opt-slider` — src/lab/optimizations/optimizations.css:11: `.opt-slider { width: 120px; }`
- feature style `opt-number` — src/lab/optimizations/optimizations.css:12: `.lab-panel .opt-number { width: 84px; }`
- feature style `opt-tag` — src/lab/optimizations/optimizations.css:15: `.lab-panel .opt-tag { padding: 0 6px; font-size: 0.86em; color: var(--lab-muted); background: var(--lab-bg3); border-color: var(--lab-line); border-radius: 9px;`
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
- shared style `icon` — src/peopleLab/peopleLab.css:26: `.icon { display: block; flex: none; }`
- shared style `panel` — src/peopleLab/peopleLab.css:40: `.panel { background: var(--panel); border-right: 1px solid var(--line); overflow-y: auto; padding: 14px 14px 14px; display: flex; flex-direction: column; scroll`
- shared style `panel` — src/peopleLab/peopleLab.css:41: `.panel h2 { margin: 0 0 10px; font-size: 15px; font-weight: 600; letter-spacing: -.01em; }`
- shared style `panel` — src/peopleLab/peopleLab.css:42: `.panel h3 { margin: 18px 0 8px; font-size: 11px; font-weight: 600; letter-spacing: .06em; text-transform: uppercase; color: var(--ink-3); }`
- shared style `panel` — src/peopleLab/peopleLab.css:43: `.panel h3:first-child { margin-top: 2px; }`
- shared style `row` — src/peopleLab/peopleLab.css:45: `.row { display: grid; grid-template-columns: 96px 1fr 38px; align-items: center; gap: 8px; min-height: 28px; color: var(--ink-2); }`
- shared style `row` — src/peopleLab/peopleLab.css:46: `.row output { text-align: right; color: var(--ink-3); font-variant-numeric: tabular-nums; font-size: 11px; }`
- shared style `grid` — src/peopleLab/peopleLab.css:61: `.grid { display: grid; grid-template-columns: repeat(3, 1fr); gap: 6px; }`
- shared style `tile` — src/peopleLab/peopleLab.css:62: `.tile { border: 1px solid var(--line); border-radius: var(--radius); background: var(--panel-2); padding: 9px 6px; cursor: pointer; display: flex; flex-directio`
- shared style `tile` — src/peopleLab/peopleLab.css:65: `.tile img { width: 56px; height: 56px; object-fit: contain; border-radius: 6px; }`
- shared style `tile` — src/peopleLab/peopleLab.css:66: `.tile:hover { border-color: #3d434b; color: var(--ink); }`
- shared style `tile` — src/peopleLab/peopleLab.css:67: `.tile.on { border-color: var(--accent); color: var(--ink); background: var(--accent-soft); }`
- feature style `tools` — src/peopleLab/peopleLab.css:68: `.tools { display: grid; grid-template-columns: repeat(4, 1fr); gap: 4px; }`
- shared style `tool` — src/peopleLab/peopleLab.css:69: `.tool { border: 1px solid transparent; border-radius: 7px; background: var(--panel-2); height: 52px; cursor: pointer; display: flex; flex-direction: column; ali`
- shared style `tool` — src/peopleLab/peopleLab.css:70: `.tool:hover { color: var(--ink); background: var(--panel-3); }`
- shared style `tool` — src/peopleLab/peopleLab.css:71: `.tool.on { color: var(--ink); border-color: var(--accent); background: var(--accent-soft); }`
- shared style `check` — src/peopleLab/peopleLab.css:76: `.check { display: flex; gap: 8px; align-items: center; color: var(--ink-2); margin: 6px 0; cursor: pointer; }`
- shared style `check` — src/peopleLab/peopleLab.css:77: `.check input { accent-color: var(--accent); }`
- shared style `fine` — src/peopleLab/peopleLab.css:84: `details.fine { margin: 2px 0 6px; }`
- shared style `fine` — src/peopleLab/peopleLab.css:85: `details.fine summary { cursor: pointer; font-size: 12px; color: var(--ink-3); padding: 6px 0; list-style: none; }`
- shared style `fine` — src/peopleLab/peopleLab.css:86: `details.fine summary::-webkit-details-marker { display: none; }`
- shared style `fine` — src/peopleLab/peopleLab.css:87: `details.fine summary::before { content: '+ '; }`
- shared style `fine` — src/peopleLab/peopleLab.css:88: `details.fine[open] summary::before { content: '\2013 '; }`
- shared style `fine` — src/peopleLab/peopleLab.css:89: `details.fine summary:hover { color: var(--ink); }`
- shared style `panel` — src/peopleLab/peopleLab.css:94: `.panel > * { flex-shrink: 0; }`
- shared style `panel` — src/peopleLab/peopleLab.css:101: `.panel .bar { display: flex; gap: 6px; align-items: center; margin-bottom: 6px; }`
- shared style `toggle` — src/peopleLab/peopleLab.css:104: `.toggle { width: 32px; height: 30px; display: grid; place-items: center; border: 1px solid var(--line); border-radius: 7px; background: var(--panel-2); color: v`
- shared style `toggle` — src/peopleLab/peopleLab.css:105: `.toggle.on { color: var(--ink); border-color: var(--accent); box-shadow: inset 0 0 0 1px var(--accent); }`
- shared style `top` — src/peopleLab/peopleLab.css:121: `.top { position: absolute; left: 14px; right: 14px; top: 12px; display: flex; align-items: center; gap: 10px; pointer-events: none; }`
- shared style `top` — src/peopleLab/peopleLab.css:122: `.top > * { pointer-events: auto; }`
- shared style `title` — src/peopleLab/peopleLab.css:123: `.title { white-space: nowrap; font-weight: 600; font-size: 14px; color: var(--ink); display: flex; align-items: center; gap: 8px; letter-spacing: -.01em; }`
- shared style `views` — src/peopleLab/peopleLab.css:126: `.views { position: absolute; bottom: 14px; left: 50%; transform: translateX(-50%); display: flex; gap: 2px; background: rgba(22, 24, 28, .94); border: 1px solid`
- shared style `views` — src/peopleLab/peopleLab.css:127: `.views button { border: 0; background: transparent; padding: 6px 12px; border-radius: 6px; cursor: pointer; font-weight: 500; color: var(--ink-2); }`
- shared style `views` — src/peopleLab/peopleLab.css:128: `.views button.on { background: var(--panel-3); color: var(--ink); }`
- shared style `tip` — src/peopleLab/peopleLab.css:132: `.tip { position: absolute; pointer-events: none; background: rgba(13, 15, 18, .92); border: 1px solid var(--line); color: var(--ink); font-size: 12px; line-heig`
- shared style `tip` — src/peopleLab/peopleLab.css:133: `.tip.on { display: block; }`
- shared style `tip` — src/peopleLab/peopleLab.css:134: `.tip b { font-weight: 600; }`
- shared style `side` — src/peopleLab/peopleLab.css:142: `.side { background: var(--panel); border-left: 1px solid var(--line); display: flex; flex-direction: column; padding: 14px; gap: 8px; }`
- shared style `side` — src/peopleLab/peopleLab.css:143: `.side h2 { margin: 0 0 2px; font-size: 11px; font-weight: 600; letter-spacing: .06em; text-transform: uppercase; color: var(--ink-3); }`
- shared style `gallery` — src/peopleLab/peopleLab.css:145: `.gallery { flex: 1; overflow-y: auto; display: grid; grid-template-columns: 1fr 1fr; gap: 6px; align-content: start; margin-top: 4px; }`
- shared style `card` — src/peopleLab/peopleLab.css:146: `.card { position: relative; border-radius: var(--radius); background: var(--panel-2); border: 1px solid var(--line); cursor: pointer; padding: 3px; text-align: `
- shared style `card` — src/peopleLab/peopleLab.css:147: `.card img { width: 100%; aspect-ratio: 1; object-fit: cover; border-radius: 6px; background: #2a2f37; display: block; margin-bottom: 3px; }`
- shared style `card` — src/peopleLab/peopleLab.css:148: `.card.on { border-color: var(--accent); color: var(--ink); }`
- shared style `card` — src/peopleLab/peopleLab.css:149: `.card .x { position: absolute; top: 5px; right: 5px; width: 18px; height: 18px; border-radius: 4px; border: 0; background: rgba(0,0,0,.55); color: #fff; cursor:`
- shared style `card` — src/peopleLab/peopleLab.css:150: `.card:hover .x { opacity: 1; }`
- shared style `gap` — src/peopleLab/peopleLab.css:163: `.gap { height: 12px; }`
- shared style `panel` — src/peopleLab/peopleLab.css:175: `.panel .loading { height: 3px; border-radius: 2px; background: linear-gradient(90deg, transparent, var(--accent), transparent); background-size: 50% 100%; backg`
- shared style `toggle` — src/peopleLab/peopleLab.css:178: `.toggle.wide { width: 100%; height: 40px; }`
- shared style `glass` — src/ui/app.css:57: `.glass { background: linear-gradient(145deg, var(--ui-hi), transparent 42%), var(--ui-bg); border: 1px solid var(--ui-border); border-radius: 14px; box-shadow: `
- shared style `inspect-select` — src/ui/app.css:96: `.inspect-select select option { color: #0d1512; background: #e8efe9; }`
- shared style `inspector` — src/ui/app.css:98: `.inspector { position: absolute; right: 18px; top: 372px; width: 244px; padding: 12px; transition: opacity 0.18s, transform 0.18s; }`
- shared style `inspector` — src/ui/app.css:99: `.inspector.hidden { opacity: 0; transform: translateX(12px); pointer-events: none; }`
- shared style `inspector-open` — src/ui/app.css:101: `.inspector-open .inspector { top: 96px; max-height: calc(100dvh - 150px); overflow: auto; }`
- shared style `inspect-actions` — src/ui/app.css:110: `.inspect-actions { display: grid; grid-template-columns: 1fr 1fr; gap: 6px; margin-top: 8px; }`
- shared style `inspect-actions` — src/ui/app.css:111: `.inspect-actions button { padding: 8px; border-radius: 9px; border: 1px solid var(--ui-border); background: transparent; color: var(--text); font-size: 11px; cu`
- shared style `inspect-actions` — src/ui/app.css:120: `.inspect-actions button.danger { color: var(--danger); border-color: rgba(255, 111, 99, 0.4); }`
- shared style `inspect-actions` — src/ui/app.css:121: `.inspect-actions button:hover:not(:disabled) { border-color: rgba(101, 229, 195, 0.45); background: rgba(101, 229, 195, 0.08); }`
- shared style `inspect-actions` — src/ui/app.css:122: `.inspect-actions button.danger:hover { border-color: var(--danger); background: rgba(255, 111, 99, 0.1); }`
- shared style `inspect-actions` — src/ui/app.css:123: `.inspect-actions button:disabled { opacity: 0.4; cursor: default; }`
- shared style `inspect-actions` — src/ui/app.css:124: `.inspect-actions button:last-child:nth-child(odd) { grid-column: 1 / -1; }`
- shared style `inspect-select` — src/ui/app.css:125: `.inspect-select { display: grid; gap: 4px; margin-top: 8px; color: var(--muted); font-size: 10px; }`
- shared style `inspect-select` — src/ui/app.css:126: `.inspect-select select { width: 100%; border: 1px solid var(--ui-border); border-radius: 8px; background: rgba(20, 28, 23, 0.7); color: var(--text); padding: 7p`
- shared style `inspect-range` — src/ui/app.css:127: `.inspect-range { display: grid; grid-template-columns: 1fr auto; align-items: center; gap: 2px 8px; margin-top: 9px; color: var(--muted); font-size: 11px; }`
- shared style `inspect-range` — src/ui/app.css:136: `.inspect-range input { grid-column: 1 / -1; width: 100%; margin: 2px 0 0; accent-color: var(--accent); }`
- shared style `inspect-range` — src/ui/app.css:137: `.inspect-range output { color: var(--text); font-variant-numeric: tabular-nums; }`
- shared style `inspect-range` — src/ui/app.css:138: `.inspect-range[data-disabled] { opacity: 0.45; }`
- feature style `about-dialog` — src/ui/app.css:156: `.about-dialog { max-width: min(560px, calc(100vw - 32px)); color: var(--text); padding: 18px 20px; border: 1px solid var(--ui-border); border-radius: 12px; }`
- feature style `about-dialog` — src/ui/app.css:157: `.about-dialog::backdrop { background: rgb(0 0 0 / 0.35); }`
- feature style `about-dialog` — src/ui/app.css:158: `.about-dialog h2 { margin: 0 0 10px; font-size: 16px; }`
- feature style `about-dialog` — src/ui/app.css:159: `.about-dialog .about-close { margin-top: 12px; padding: 6px 14px; border-radius: 8px; border: 1px solid var(--ui-border); background: transparent; color: var(--`
- feature style `options-dialog` — src/ui/app.css:161: `.options-dialog { width: min(560px, calc(100vw - 32px)); max-height: calc(100vh - 32px); overflow: auto; color: var(--text); padding: 18px 20px; border: 1px sol`
- feature style `options-dialog` — src/ui/app.css:162: `.options-dialog::backdrop { background: rgb(0 0 0 / 0.35); }`
- feature style `options-dialog` — src/ui/app.css:163: `.options-dialog h2 { margin: 0 0 12px; font-size: 16px; }`
- feature style `options-row` — src/ui/app.css:164: `.options-row { display: grid; gap: 6px; padding: 8px 0; border-top: 1px solid var(--ui-border); }`
- feature style `options-name` — src/ui/app.css:165: `.options-name { color: var(--muted); font-size: 12px; font-weight: 600; }`
- feature style `options-choices` — src/ui/app.css:166: `.options-choices { display: flex; flex-wrap: wrap; gap: 4px; }`
- feature style `options-choices` — src/ui/app.css:167: `.options-choices .v2-choice { color: var(--text); font: inherit; font-size: 12.5px; cursor: pointer; }`
- feature style `options-choices` — src/ui/app.css:168: `.options-choices .v2-choice.on { color: var(--v-accent-ink); }`
- feature style `options-note` — src/ui/app.css:169: `.options-note, .options-gpu { color: var(--muted); font-size: 11.5px; }`
- feature style `options-gpu` — src/ui/app.css:170: `.options-gpu { margin: 10px 0 0; overflow-wrap: anywhere; }`
- feature style `options-dialog` — src/ui/app.css:171: `.options-dialog .about-close { margin-top: 12px; padding: 6px 14px; border-radius: 8px; border: 1px solid var(--ui-border); background: transparent; color: var(`
- shared style `effects-panel` — src/ui/app.css:173: `.effects-panel { position: fixed; top: 64px; right: 12px; z-index: 70; width: min(380px, calc(100vw - 24px)); max-height: calc(100vh - 76px); overflow: auto; co`
- shared style `effects-panel` — src/ui/app.css:174: `.effects-panel[hidden] { display: none; }`
- shared style `effects-panel` — src/ui/app.css:175: `.effects-panel.embedded { position: static; width: auto; max-height: none; overflow: visible; background: none; border: 0; box-shadow: none; padding: 0; }`
- shared style `effects-panel` — src/ui/app.css:176: `.effects-panel.embedded .effects-close { display: none; }`
- shared style `effects-panel` — src/ui/app.css:177: `.effects-panel header { display: flex; align-items: center; gap: 6px; }`
- shared style `effects-panel` — src/ui/app.css:178: `.effects-panel h2 { margin: 0; font-size: 15px; flex: 1; }`
- feature style `v2-choice` — src/ui/app.css:179: `.effects-panel .v2-choice { color: var(--text); font: inherit; font-size: 11.5px; cursor: pointer; }`
- feature style `v2-choice` — src/ui/app.css:180: `.effects-panel .v2-choice.on { color: var(--v-accent-ink); }`
- shared style `effects-close` — src/ui/app.css:181: `.effects-close { border: 0; background: transparent; color: var(--text); font-size: 18px; cursor: pointer; padding: 0 4px; }`
- shared style `effects-stats` — src/ui/app.css:182: `.effects-stats { display: grid; grid-template-columns: 1fr auto; gap: 2px 10px; margin: 8px 0; padding: 8px; border: 1px solid var(--ui-border); border-radius: `
- shared style `effects-stats` — src/ui/app.css:183: `.effects-stats dt { color: var(--muted); }`
- shared style `effects-stats` — src/ui/app.css:184: `.effects-stats dd { margin: 0; text-align: right; }`
- shared style `effects-group` — src/ui/app.css:185: `.effects-group { margin: 8px 0 0; padding: 6px 8px; border: 1px solid var(--ui-border); border-radius: 8px; }`
- shared style `effects-group` — src/ui/app.css:186: `.effects-group legend { display: flex; align-items: center; gap: 4px; padding: 0 4px; color: var(--muted); font-weight: 600; }`
- shared style `effects-choice` — src/ui/app.css:187: `.effects-choice { display: grid; gap: 4px; padding: 4px 0; }`
- shared style `effects-item` — src/ui/app.css:188: `.effects-item { display: flex; align-items: center; gap: 6px; padding: 2px 0; cursor: pointer; }`
- feature style `about-dialog` — src/ui/app.css:189: `.about-dialog #aboutCredits { font-size: 12px; max-height: 50vh; overflow: auto; white-space: pre-wrap; }`
- shared style `inspector` — src/ui/app.css:193: `@media (max-width: 780px) { .inspector { left: 10px; right: 10px; top: 74px; width: auto; max-height: calc(100dvh - 210px); overflow: auto; } }`
- feature style `v2-shell` — src/ui/builder/workspace.css:84: `:root.v2-shell { --v2-text-s: 13px; --v2-text-m: 14px; --v2-text-l: 16px; --v2-top: 52px; }`
- feature style `v2-shell` — src/ui/builder/workspace.css:90: `:root.v2-shell #builder { font-size: var(--v2-text-m); }`
- shared style `rp-panel` — src/ui/roads/roads.css:3: `.rp-panel { position: fixed; left: 16px; right: 16px; bottom: calc(var(--v-dock-h, 54px) + 14px); z-index: 40; max-width: 1000px; margin: 0 auto; max-height: ca`
- feature style `v2-toolopts` — src/ui/roads/roads.css:15: `@media (min-width: 761px) { body:has(.v2-toolopts:not([hidden])) .rp-panel { left: calc(var(--v-opts-w, 300px) + 28px); } }`
- feature style `v2-side` — src/ui/roads/roads.css:16: `@media (min-width: 761px) { body:has(.v2-side:not([hidden])) .rp-panel { right: 364px; } }`
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
- shared style `rp-switch` — src/ui/roads/roads.css:107: `.rp-switch .track { position: relative; width: 34px; height: 18px; border-radius: 999px; background: rgba(255,255,255,.16); transition: background .15s; flex: n`
- shared style `rp-switch` — src/ui/roads/roads.css:109: `.rp-switch[aria-checked="true"] .track { background: rgba(79, 224, 191, 0.45); }`
- shared style `rp-panel` — src/ui/roads/roads.css:112: `.rp-panel .rp-issue { width: 100%; text-align: left; font: inherit; display: flex; gap: 8px; align-items: flex-start; padding: 8px 10px; border-radius: 8px; bac`
- shared style `rp-issue` — src/ui/roads/roads.css:113: `button.rp-issue:hover { background: rgba(255, 111, 97, 0.2); }`
- shared style `rp-issue` — src/ui/roads/roads.css:114: `.rp-issue svg { flex: none; margin-top: 1px; color: var(--v-danger, #ff6f61); }`
- shared style `rp-ok` — src/ui/roads/roads.css:115: `.rp-ok { display: flex; gap: 8px; align-items: center; color: #9be3c9; font-size: 12.5px; }`
- shared style `rp-foot` — src/ui/roads/roads.css:118: `.rp-foot { display: flex; flex-wrap: wrap; align-items: center; gap: 8px; }`
- shared style `rp-btn` — src/ui/roads/roads.css:119: `.rp-btn { display: inline-flex; align-items: center; gap: 6px; min-height: 34px; padding: 0 14px; border-radius: 999px; border: 1px solid var(--v-line); backgro`
- shared style `rp-btn` — src/ui/roads/roads.css:120: `.rp-btn:hover:not(:disabled) { border-color: var(--v-accent-line); background: var(--v-raise-2); }`
- shared style `rp-btn` — src/ui/roads/roads.css:121: `.rp-btn.primary { background: var(--v-accent); color: var(--v-accent-ink); border-color: var(--v-accent); }`
- shared style `rp-btn` — src/ui/roads/roads.css:122: `.rp-btn.primary:hover:not(:disabled) { filter: brightness(1.08); background: var(--v-accent); }`
- shared style `rp-btn` — src/ui/roads/roads.css:123: `.rp-btn.danger { color: var(--v-danger-ink); }`
- shared style `rp-field` — src/ui/roads/roads.css:124: `.rp-field { min-height: 32px; width: 180px; padding: 0 12px; border-radius: 999px; border: 1px solid var(--v-line); background: rgba(0,0,0,.25); color: var(--v-`
- shared style `rp-field` — src/ui/roads/roads.css:125: `.rp-field::placeholder { color: var(--v-muted); }`
- shared style `rp-field` — src/ui/roads/roads.css:126: `.rp-field:focus { border-color: var(--v-accent-line); }`
- shared style `rp-summary` — src/ui/roads/roads.css:129: `.rp-summary { display: grid; gap: 8px; margin: 10px 0; padding: 10px; border: 1px solid rgba(255,255,255,.12); border-radius: 10px; background: rgba(255,255,255`
- shared style `rp-summary-head` — src/ui/roads/roads.css:130: `.rp-summary-head { display: flex; align-items: baseline; gap: 8px; font-weight: 650; }`
- shared style `rp-summary-head` — src/ui/roads/roads.css:131: `.rp-summary-head .rp-sub { margin-left: auto; font-weight: 500; }`
- shared style `rp-summary-art` — src/ui/roads/roads.css:132: `.rp-summary-art { border-radius: 8px; overflow: hidden; background: linear-gradient(180deg, #2b4448 0%, #1d2f32 70%); }`
- shared style `rp-summary-art` — src/ui/roads/roads.css:133: `.rp-summary-art svg { display: block; width: 100%; height: auto; }`
- shared style `rp-summary` — src/ui/roads/roads.css:134: `.rp-summary .rp-btn { justify-self: start; }`
- feature style `v2-pill` — src/ui/roads/roads.css:137: `.v2-pill.rp-open { width: auto; max-width: 170px; padding: 0 10px 0 8px; font-size: 12.5px; font-weight: 600; }`
- feature style `v2-pill` — src/ui/roads/roads.css:138: `.v2-pill.rp-open > span { position: static; width: auto; height: auto; overflow: hidden; clip: auto; text-overflow: ellipsis; white-space: nowrap; }`
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
- feature style `v2-seg-b` — src/ui/roads/roads.css:162: `.v2-seg-b.rp-seg-text { width: auto; padding: 0 11px; font-size: 12.5px; font-weight: 600; }`
- feature style `v2-seg-b` — src/ui/roads/roads.css:163: `.v2-seg-b.rp-seg-text > span { position: static; width: auto; height: auto; overflow: visible; clip: auto; }`
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
- feature style `v2-strip` — src/ui/roads/roads.css:230: `.v2-strip:has(> .rc-catalog) { overflow: hidden; }`
- feature style `v2-strip` — src/ui/roads/roads.css:231: `.v2-strip > .rc-catalog { flex: 1 1 auto; min-width: 0; }`
- feature style `v2-drawer` — src/ui/roads/roads.css:232: `.v2-drawer:has(.rc-catalog) { width: min(860px, calc(100vw - 24px)); }`
- feature style `v2-seg` — src/ui/roads/roads.css:235: `.v2-seg.rp-seg-tight .v2-seg-b { padding: 0 7px; font-size: 13px; }`
- shared style `rp-summary-actions` — src/ui/roads/roads.css:238: `.rp-summary-actions { display: flex; flex-wrap: wrap; gap: 6px; }`
- feature style `v2` — src/ui/v2/shell.css:27: `.v2 { position: absolute; inset: 0; pointer-events: none; z-index: 5; color: var(--v-text); font: 13px/1.3 Inter, ui-sans-serif, "Segoe UI", Roboto, sans-serif;`
- feature style `v2` — src/ui/v2/shell.css:28: `.v2 * { box-sizing: border-box; }`
- feature style `v2` — src/ui/v2/shell.css:29: `.v2 button, .v2 select, .v2 input { font: inherit; color: inherit; }`
- feature style `v2` — src/ui/v2/shell.css:30: `.v2 button { cursor: pointer; }`
- feature style `v2` — src/ui/v2/shell.css:31: `.v2 button:disabled { opacity: 0.35; cursor: default; }`
- feature style `v2` — src/ui/v2/shell.css:32: `.v2 [hidden] { display: none !important; }`
- feature style `v2` — src/ui/v2/shell.css:33: `.v2 :focus-visible { outline: 2px solid var(--v-accent); outline-offset: 1px; }`
- feature style `v2-hud` — src/ui/v2/shell.css:34: `.v2-hud, .v2-dock, .v2-drawer, .v2-toolopts, .v2-pop, .v2-side, .v2-toast { pointer-events: auto; background: var(--v-bg); border: 1px solid var(--v-line); box-`
- feature style `v2-icon` — src/ui/v2/shell.css:41: `.v2-icon > span, .v2-pill > span, .v2-seg-b > span, .v2-tabs.compact .v2-tab.icon:not(.on) > span, .v2-tabs.tight .v2-tab.icon > span, .v2-choice.icon > span, .`
- feature style `v2-hud` — src/ui/v2/shell.css:47: `.v2-hud { position: absolute; top: 12px; left: 12px; right: 12px; height: var(--v-hud-h); display: grid; grid-template-columns: 1fr auto 1fr; align-items: cente`
- feature style `v2-city` — src/ui/v2/shell.css:52: `.v2-city { display: flex; align-items: center; gap: 12px; min-width: 0; }`
- feature style `v2-clock` — src/ui/v2/shell.css:53: `.v2-clock { font-size: 17px; font-variant-numeric: tabular-nums; letter-spacing: 0.02em; }`
- feature style `v2-money` — src/ui/v2/shell.css:54: `.v2-money { font-size: 15px; font-variant-numeric: tabular-nums; white-space: nowrap; }`
- feature style `v2-stats` — src/ui/v2/shell.css:55: `.v2-stats { color: var(--v-muted); font-size: 13px; white-space: nowrap; overflow: hidden; text-overflow: ellipsis; }`
- feature style `v2-seg` — src/ui/v2/shell.css:57: `.v2-seg, .v2-speed { display: flex; gap: 2px; padding: 3px; border-radius: 999px; background: var(--v-raise); border: 1px solid var(--v-line); }`
- feature style `v2-seg-b` — src/ui/v2/shell.css:58: `.v2-seg-b, .v2-speed-b { position: relative; display: grid; place-items: center; min-width: 34px; height: 28px; padding: 0 6px; border: 0; border-radius: 999px;`
- feature style `v2-seg-b` — src/ui/v2/shell.css:62: `.v2-seg-b:hover, .v2-speed-b:hover { color: var(--v-text); }`
- feature style `v2-seg-b` — src/ui/v2/shell.css:63: `.v2-seg-b.on, .v2-speed-b.on { background: var(--v-accent); color: var(--v-accent-ink); }`
- feature style `v2-warn` — src/ui/v2/shell.css:65: `.v2-warn { display: inline-flex; align-items: center; gap: 5px; height: 28px; padding: 0 9px; border: 1px solid rgba(255, 111, 97, 0.55); border-radius: 999px; `
- feature style `v2-warn` — src/ui/v2/shell.css:66: `.v2-warn > span { position: absolute; width: 1px; height: 1px; overflow: hidden; clip: rect(0 0 0 0); }`
- feature style `v2-warn` — src/ui/v2/shell.css:67: `.v2-warn:hover { border-color: var(--v-danger); }`
- feature style `v2-actions` — src/ui/v2/shell.css:68: `.v2-actions { display: flex; justify-content: flex-end; align-items: center; gap: 6px; }`
- feature style `v2-pill` — src/ui/v2/shell.css:69: `.v2-pill, .v2-icon { position: relative; display: inline-flex; align-items: center; justify-content: center; gap: 6px; width: 32px; height: 32px; padding: 0; fl`
- feature style `v2-pill` — src/ui/v2/shell.css:74: `.v2-pill:hover, .v2-icon:hover:not(:disabled) { background: var(--v-raise-2); border-color: var(--v-accent-line); }`
- feature style `v2-icon` — src/ui/v2/shell.css:75: `.v2-icon.on, .v2-pill.on { color: var(--v-accent); border-color: var(--v-accent); background: var(--v-accent-soft); }`
- feature style `v2-icon` — src/ui/v2/shell.css:76: `.v2-icon.danger { color: var(--v-danger-ink); }`
- feature style `v2-inside` — src/ui/v2/shell.css:78: `.v2-inside { display: flex; align-items: center; gap: 2px; padding-right: 4px; border-right: 1px solid var(--v-line); }`
- feature style `v2-inside-level` — src/ui/v2/shell.css:79: `.v2-inside-level { min-width: 22px; padding: 0 2px; font-size: 12px; font-weight: 700; text-align: center; }`
- feature style `v2-pop` — src/ui/v2/shell.css:82: `.v2-pop { position: absolute; z-index: 9; min-width: 280px; max-width: min(380px, calc(100vw - 24px)); max-height: calc(100dvh - 100px); overflow: auto; padding`
- feature style `v2-pop-body` — src/ui/v2/shell.css:83: `.v2-pop-body { display: grid; gap: 2px; }`
- feature style `v2-pop` — src/ui/v2/shell.css:84: `.v2-pop hr { width: 100%; height: 1px; margin: 4px 0; border: 0; background: var(--v-line); }`
- feature style `v2-row` — src/ui/v2/shell.css:85: `.v2-row { position: relative; display: flex; align-items: center; gap: 12px; width: 100%; min-height: 38px; padding: 6px 10px; border: 0; border-radius: var(--v`
- feature style `v2-row` — src/ui/v2/shell.css:86: `.v2-row:hover { background: var(--v-raise-2); }`
- feature style `v2-row` — src/ui/v2/shell.css:87: `.v2-row svg { flex: none; width: 18px; height: 18px; fill: none; stroke: currentColor; stroke-width: 1.8; color: var(--v-muted); }`
- feature style `v2-switch` — src/ui/v2/shell.css:89: `.v2-switch { padding-right: 52px; }`
- feature style `v2-switch` — src/ui/v2/shell.css:90: `.v2-switch::after { content: ""; position: absolute; right: 10px; top: 50%; width: 30px; height: 16px; margin-top: -8px; border-radius: 999px; background: rgba(`
- feature style `v2-switch` — src/ui/v2/shell.css:91: `.v2-switch::before { content: ""; position: absolute; right: 26px; top: 50%; z-index: 1; width: 12px; height: 12px; margin-top: -6px; border-radius: 50%; backgr`
- feature style `v2-switch` — src/ui/v2/shell.css:92: `.v2-switch.on::after { background: rgba(79, 224, 191, 0.45); }`
- feature style `v2-switch` — src/ui/v2/shell.css:93: `.v2-switch.on::before { transform: translateX(14px); background: var(--v-accent); }`
- feature style `v2-switch` — src/ui/v2/shell.css:94: `.v2-switch.on svg { color: var(--v-accent); }`
- feature style `v2-pop` — src/ui/v2/shell.css:95: `.v2-pop .v2-orow { padding: 2px 8px; }`
- feature style `v2-metrics` — src/ui/v2/shell.css:96: `.v2-metrics { display: grid; grid-template-columns: 1fr auto; gap: 4px 12px; margin: 4px 0 0; padding: 8px 10px 4px; border-top: 1px solid var(--v-line); font-s`
- feature style `v2-metrics` — src/ui/v2/shell.css:97: `.v2-metrics dt { color: var(--v-muted); }`
- feature style `v2-metrics` — src/ui/v2/shell.css:98: `.v2-metrics dd { margin: 0; text-align: right; font-variant-numeric: tabular-nums; }`
- feature style `v2-pop-body` — src/ui/v2/shell.css:99: `.v2-pop-body.icons { display: flex; gap: 2px; }`
- feature style `v2-pop` — src/ui/v2/shell.css:100: `.v2-pop:has(> .v2-pop-body.icons) { min-width: 0; padding: 4px; }`
- feature style `v2-pop-body` — src/ui/v2/shell.css:101: `.v2-pop-body.icons .v2-icon svg { width: 18px; height: 18px; fill: none; stroke: currentColor; stroke-width: 1.8; }`
- feature style `v2-pop-body` — src/ui/v2/shell.css:103: `.v2-pop-body.help { gap: 6px; grid-template-columns: auto 1fr; column-gap: 10px; }`
- feature style `v2-pop-body` — src/ui/v2/shell.css:104: `.v2-pop-body.help > * { grid-column: 1 / -1; }`
- feature style `v2-pop-body` — src/ui/v2/shell.css:105: `.v2-pop-body.help > .v2-keys { display: grid; grid-template-columns: subgrid; }`
- feature style `v2-pop-body` — src/ui/v2/shell.css:106: `.v2-pop-body.help > .v2-keys > .v2-subhead { grid-column: 1 / -1; }`
- feature style `v2-keys` — src/ui/v2/shell.css:107: `.v2-keys dl { display: grid; grid-template-columns: auto 1fr; gap: 4px 10px; margin: 4px 0 0; padding: 0 8px; align-items: baseline; }`
- feature style `v2-pop-body` — src/ui/v2/shell.css:108: `.v2-pop-body.help > .v2-keys > dl { grid-column: 1 / -1; grid-template-columns: subgrid; }`
- feature style `v2-keys` — src/ui/v2/shell.css:109: `.v2-keys dt { margin: 0; }`
- feature style `v2-keys` — src/ui/v2/shell.css:110: `.v2-keys dd { margin: 0; color: var(--v-muted); font-size: 12.5px; }`
- feature style `v2-keys` — src/ui/v2/shell.css:111: `.v2-keys kbd { display: inline-block; padding: 2px 6px; border: 1px solid var(--v-line); border-bottom-width: 2px; border-radius: 6px; background: var(--v-raise`
- feature style `v2-more` — src/ui/v2/shell.css:112: `.v2-more { padding: 0 8px; }`
- feature style `v2-more` — src/ui/v2/shell.css:113: `.v2-more summary { cursor: pointer; color: var(--v-accent); font-size: 12.5px; padding: 4px 0; }`
- feature style `v2-help` — src/ui/v2/shell.css:114: `.v2-help { padding: 4px 0; }`
- feature style `v2-help` — src/ui/v2/shell.css:115: `.v2-help strong { font-size: 13px; }`
- feature style `v2-help` — src/ui/v2/shell.css:116: `.v2-help p { margin: 2px 0 0; color: var(--v-muted); font-size: 12.5px; line-height: 1.45; }`
- feature style `v2-dock` — src/ui/v2/shell.css:119: `.v2-dock { position: absolute; left: 50%; bottom: 12px; transform: translateX(-50%); display: flex; gap: 2px; padding: 4px; border-radius: 14px; max-width: calc`
- feature style `v2-cat` — src/ui/v2/shell.css:124: `.v2-cat { position: relative; display: grid; grid-template-rows: 24px auto; justify-items: center; align-content: center; gap: 3px; min-width: 66px; height: 52p`
- feature style `v2-cat` — src/ui/v2/shell.css:129: `.v2-cat:hover { background: var(--v-raise); color: var(--v-text); }`
- feature style `v2-cat` — src/ui/v2/shell.css:130: `.v2-cat.on { background: var(--v-accent-soft); border-color: var(--v-accent); color: var(--v-accent); }`
- feature style `v2-cat` — src/ui/v2/shell.css:131: `.v2-cat.danger.on { background: rgba(255, 111, 97, 0.14); border-color: var(--v-danger); color: var(--v-danger); }`
- feature style `v2-cat` — src/ui/v2/shell.css:132: `.v2-cat svg { width: 22px; height: 22px; }`
- feature style `v2-cat` — src/ui/v2/shell.css:133: `.v2-cat kbd { display: none; }`
- feature style `v2-drawer` — src/ui/v2/shell.css:136: `.v2-drawer { position: absolute; left: 50%; bottom: calc(12px + var(--v-dock-h) + 10px); transform: translateX(-50%); width: max-content; min-width: 0; max-widt`
- feature style `v2-drawer` — src/ui/v2/shell.css:142: `.v2-drawer:has(> .v2-drawer-head > .v2-tabs[hidden]) { grid-template-areas: "body close"; }`
- feature style `v2-drawer-head` — src/ui/v2/shell.css:143: `.v2-drawer-head { grid-area: head; display: flex; align-items: center; gap: 6px; min-width: 0; padding: 4px 6px; border-bottom: 1px solid var(--v-line); }`
- feature style `v2-drawer-head` — src/ui/v2/shell.css:144: `.v2-drawer-head > .v2-tabs { min-width: 0; overflow-x: auto; scrollbar-width: thin; scrollbar-color: rgba(255, 255, 255, 0.18) transparent; }`
- feature style `v2-drawer-head` — src/ui/v2/shell.css:145: `.v2-drawer-head:has(> .v2-tabs[hidden]) { display: none; }`
- feature style `v2-drawer` — src/ui/v2/shell.css:146: `.v2-drawer > .v2-icon { grid-area: close; width: 34px; height: auto; align-self: stretch; margin: 0; border: 0; border-bottom: 1px solid var(--v-line); border-r`
- feature style `v2-drawer` — src/ui/v2/shell.css:147: `.v2-drawer:has(> .v2-drawer-head > .v2-tabs[hidden]) > .v2-icon { align-self: center; width: 26px; height: 26px; margin: 0 6px; border: 1px solid var(--v-line);`
- feature style `v2-drawer` — src/ui/v2/shell.css:148: `.v2-drawer > .v2-drawer-body { grid-area: body; display: flex; align-items: center; min-width: 0; }`
- feature style `v2-title` — src/ui/v2/shell.css:149: `.v2-title { display: none; }`
- feature style `v2-tabs` — src/ui/v2/shell.css:150: `.v2-tabs { display: flex; gap: 2px; align-items: center; }`
- feature style `v2-tab` — src/ui/v2/shell.css:151: `.v2-tab { height: 30px; padding: 0 10px; border: 1px solid transparent; border-radius: 8px; background: none; color: var(--v-muted); font-size: 12.5px; font-wei`
- feature style `v2-tab` — src/ui/v2/shell.css:153: `.v2-tab.icon { display: inline-flex; align-items: center; gap: 6px; position: relative; padding: 0 10px 0 8px; }`
- feature style `v2-tab` — src/ui/v2/shell.css:154: `.v2-tab.icon > svg { flex: none; }`
- feature style `v2-tabs` — src/ui/v2/shell.css:155: `.v2-tabs.compact .v2-tab.icon:not(.on), .v2-tabs.tight .v2-tab.icon { width: 34px; padding: 0; justify-content: center; }`
- feature style `v2-tab` — src/ui/v2/shell.css:156: `.v2-tab:hover:not(:disabled) { color: var(--v-text); background: var(--v-raise-2); }`
- feature style `v2-tab` — src/ui/v2/shell.css:157: `.v2-tab.on { color: var(--v-accent-ink); background: var(--v-accent); border-color: var(--v-accent); }`
- feature style `v2-hint` — src/ui/v2/shell.css:158: `.v2-hint { display: none !important; }`
- feature style `v2-strip` — src/ui/v2/shell.css:161: `.v2-strip { flex: 1; min-width: 0; display: flex; gap: 0; padding: 6px 8px; align-items: flex-start; overflow-x: auto; overflow-y: hidden; scrollbar-width: thin`
- feature style `v2-strip` — src/ui/v2/shell.css:166: `.v2-strip::-webkit-scrollbar { height: 6px; }`
- feature style `v2-strip` — src/ui/v2/shell.css:167: `.v2-strip::-webkit-scrollbar-thumb { background: rgba(255, 255, 255, 0.18); border-radius: 3px; }`
- feature style `v2-strip` — src/ui/v2/shell.css:168: `.v2-strip:empty { display: none; }`
- feature style `v2-drawer` — src/ui/v2/shell.css:170: `.v2-drawer [data-more="right"] { mask-image: linear-gradient(to right, #000 calc(100% - 64px), transparent); }`
- feature style `v2-drawer` — src/ui/v2/shell.css:171: `.v2-drawer [data-more="left"] { mask-image: linear-gradient(to left, #000 calc(100% - 64px), transparent); }`
- feature style `v2-drawer` — src/ui/v2/shell.css:172: `.v2-drawer [data-more="both"] { mask-image: linear-gradient(to right, transparent, #000 64px, #000 calc(100% - 64px), transparent); }`
- feature style `v2-section` — src/ui/v2/shell.css:173: `.v2-section { flex: none; display: grid; }`
- feature style `v2-section` — src/ui/v2/shell.css:175: `.v2-section + .v2-section { margin-left: 8px; padding-left: 8px; border-left: 1px solid var(--v-line); }`
- feature style `v2-section` — src/ui/v2/shell.css:176: `.v2-section:has(> .v2-section-items:empty) { display: none; }`
- feature style `v2-section-title` — src/ui/v2/shell.css:177: `.v2-section-title { display: none; }`
- feature style `v2-section-items` — src/ui/v2/shell.css:178: `.v2-section-items { display: grid; grid-auto-flow: column; grid-template-rows: auto; gap: 4px; }`
- feature style `v2-strip` — src/ui/v2/shell.css:180: `.v2-strip.two-rows { --v-card: 40px; }`
- feature style `v2-strip` — src/ui/v2/shell.css:181: `.v2-strip.two-rows .v2-section-items { grid-template-rows: repeat(2, auto); }`
- feature style `v2-card` — src/ui/v2/shell.css:184: `.v2-card { position: relative; display: grid; grid-template-rows: var(--v-card) auto; width: var(--v-card-w); padding: 0; overflow: hidden; border: 1px solid va`
- feature style `v2-card` — src/ui/v2/shell.css:194: `.v2-card:hover:not(:disabled) { border-color: rgba(79, 224, 191, 0.6); background: var(--v-raise-2); }`
- feature style `v2-card` — src/ui/v2/shell.css:195: `.v2-card.on { border-color: var(--v-accent); box-shadow: 0 0 0 2px var(--v-accent) inset; }`
- feature style `v2-strip` — src/ui/v2/shell.css:202: `.v2-strip .v2-card.model { --v-card: 54px; --v-card-w: 80px; }`
- feature style `v2-verb` — src/ui/v2/shell.css:212: `.v2-verb { position: relative; display: grid; place-items: center; width: 32px; height: 32px; padding: 0; border: 1px solid var(--v-line); border-radius: var(--`
- feature style `v2-verb` — src/ui/v2/shell.css:216: `.v2-verb svg { width: 16px; height: 16px; }`
- feature style `v2-verb` — src/ui/v2/shell.css:217: `.v2-verb:hover:not(:disabled) { border-color: rgba(79, 224, 191, 0.6); }`
- feature style `v2-verb` — src/ui/v2/shell.css:218: `.v2-verb.on { border-color: var(--v-accent); color: var(--v-accent); background: var(--v-accent-soft); }`
- feature style `v2-verb` — src/ui/v2/shell.css:219: `.v2-verb.danger { color: var(--v-danger-ink); }`
- feature style `v2-muted` — src/ui/v2/shell.css:220: `.v2-muted { margin: 0; color: var(--v-muted); font-size: 11.5px; }`
- feature style `v2-toolopts` — src/ui/v2/shell.css:223: `.v2-toolopts { position: absolute; left: 12px; bottom: 12px; width: var(--v-opts-w); max-width: calc(100vw - 24px); display: grid; gap: 2px; padding: 6px 8px; b`
- feature style `v2-options` — src/ui/v2/shell.css:229: `.v2-options { display: grid; gap: 2px; }`
- feature style `v2-orow` — src/ui/v2/shell.css:231: `.v2-orow { display: grid; grid-template-columns: 66px minmax(0, 1fr); align-items: center; gap: 8px; min-height: 34px; }`
- feature style `v2-orow-name` — src/ui/v2/shell.css:232: `.v2-orow-name { color: var(--v-muted); font-size: 11.5px; font-weight: 600; overflow: hidden; text-overflow: ellipsis; white-space: nowrap; }`
- feature style `v2-orow-ctrls` — src/ui/v2/shell.css:233: `.v2-orow-ctrls { display: flex; flex-wrap: wrap; align-items: center; gap: 4px; min-width: 0; }`
- feature style `v2-orow` — src/ui/v2/shell.css:234: `.v2-orow.range .v2-orow-ctrls { flex-wrap: nowrap; }`
- feature style `v2-range` — src/ui/v2/shell.css:235: `.v2-range { flex: 1; min-width: 0; height: 22px; margin: 0; accent-color: var(--v-accent); }`
- feature style `v2-range-out` — src/ui/v2/shell.css:236: `.v2-range-out { min-width: 48px; text-align: right; font-size: 12px; font-variant-numeric: tabular-nums; color: var(--v-text); white-space: nowrap; }`
- feature style `v2-subhead` — src/ui/v2/shell.css:238: `.v2-subhead { margin-top: 4px; padding: 6px 0 2px; border-top: 1px solid var(--v-line); color: var(--v-muted); font-size: 10.5px; font-weight: 700; letter-spaci`
- feature style `v2-toolopts` — src/ui/v2/shell.css:239: `.v2-toolopts > .v2-head-tools { display: flex; flex-wrap: wrap; gap: 4px; padding-top: 4px; border-top: 1px solid var(--v-line); }`
- feature style `v2-toolopts` — src/ui/v2/shell.css:240: `.v2-toolopts > .v2-head-tools:empty { display: none; }`
- feature style `v2-toolopts` — src/ui/v2/shell.css:241: `.v2-toolopts .v2-options[hidden] + .v2-head-tools { border-top: 0; padding-top: 0; }`
- feature style `v2-choices` — src/ui/v2/shell.css:243: `.v2-choices { display: flex; flex-wrap: wrap; gap: 2px; }`
- feature style `v2-choice` — src/ui/v2/shell.css:244: `.v2-choice { min-height: 30px; padding: 3px 8px; border: 1px solid var(--v-line); border-radius: var(--v-r-s); background: var(--v-raise); font-size: 12px; line`
- feature style `v2-choice` — src/ui/v2/shell.css:245: `.v2-choice:hover:not(:disabled) { border-color: var(--v-accent-line); }`
- feature style `v2-choice` — src/ui/v2/shell.css:246: `.v2-choice.on { color: var(--v-accent-ink); background: var(--v-accent); border-color: var(--v-accent); font-weight: 650; }`
- feature style `v2-choice` — src/ui/v2/shell.css:247: `.v2-choice.icon { position: relative; display: grid; place-items: center; width: 32px; height: 32px; min-height: 0; padding: 0; border-color: transparent; backg`
- feature style `v2-choice` — src/ui/v2/shell.css:248: `.v2-choice.icon:hover:not(:disabled) { color: var(--v-text); background: var(--v-raise-2); }`
- feature style `v2-choice` — src/ui/v2/shell.css:249: `.v2-choice.icon.on { color: var(--v-accent-ink); background: var(--v-accent); border-color: var(--v-accent); }`
- feature style `v2-stepper` — src/ui/v2/shell.css:251: `.v2-stepper { display: inline-grid; grid-template-columns: 26px minmax(36px, auto) 26px; align-items: center; gap: 2px; }`
- feature style `v2-stepper` — src/ui/v2/shell.css:252: `.v2-stepper .v2-icon { width: 26px; height: 26px; border-radius: 7px; }`
- feature style `v2-stepper-value` — src/ui/v2/shell.css:253: `.v2-stepper-value { padding: 0 2px; text-align: center; font-size: 12.5px; font-weight: 700; font-variant-numeric: tabular-nums; white-space: nowrap; }`
- feature style `v2-act` — src/ui/v2/shell.css:255: `.v2-act { position: relative; display: grid; place-items: center; width: 32px; height: 32px; padding: 0; border: 1px solid var(--v-line); border-radius: var(--v`
- feature style `v2-act` — src/ui/v2/shell.css:256: `.v2-act:hover { border-color: var(--v-accent-line); }`
- feature style `v2-act` — src/ui/v2/shell.css:257: `.v2-act.danger { color: var(--v-danger-ink); }`
- feature style `v2-act` — src/ui/v2/shell.css:258: `.v2-act.danger:hover { border-color: var(--v-danger); }`
- feature style `v2-group` — src/ui/v2/shell.css:260: `.v2-group { display: flex; flex-wrap: wrap; align-items: center; gap: 4px; padding: 3px 0; }`
- feature style `v2-group` — src/ui/v2/shell.css:261: `.v2-group + .v2-group { border-top: 1px solid var(--v-line); }`
- feature style `v2-search` — src/ui/v2/shell.css:262: `.v2-search { width: 100%; min-width: 0; height: 30px; padding: 0 10px; border: 1px solid var(--v-line); border-radius: var(--v-r-s); background: var(--v-bg-soli`
- feature style `v2-group` — src/ui/v2/shell.css:263: `.v2-group .v2-search { width: 132px; }`
- feature style `v2-search` — src/ui/v2/shell.css:264: `.v2-search:focus { outline: none; border-color: var(--v-accent); }`
- feature style `v2-swatches` — src/ui/v2/shell.css:265: `.v2-swatches { display: flex; flex-wrap: wrap; gap: 6px; }`
- feature style `v2-swatch` — src/ui/v2/shell.css:266: `.v2-swatch { width: 24px; height: 24px; padding: 0; border: 1px solid var(--v-line); border-radius: 7px; background: var(--c); }`
- feature style `v2-swatch` — src/ui/v2/shell.css:267: `.v2-swatch.on { outline: 2px solid var(--v-accent); outline-offset: 2px; }`
- feature style `v2-swatch` — src/ui/v2/shell.css:268: `.v2-swatch.custom { background: conic-gradient(#e66, #ec6, #6c6, #6ce, #66e, #c6e, #e66); -webkit-appearance: none; appearance: none; }`
- feature style `v2-swatch` — src/ui/v2/shell.css:269: `.v2-swatch.custom::-webkit-color-swatch { opacity: 0; }`
- feature style `v2-transit-line` — src/ui/v2/shell.css:272: `.v2-transit-line { display: flex; align-items: center; gap: 6px; min-width: 260px; padding: 4px 8px; border-radius: 8px; background: var(--v-raise); }`
- feature style `v2-transit-line` — src/ui/v2/shell.css:273: `.v2-transit-line > i { flex: none; width: 12px; height: 12px; border-radius: 3px; }`
- feature style `v2-transit-line` — src/ui/v2/shell.css:274: `.v2-transit-line > b { font-weight: 700; }`
- feature style `v2-transit-line` — src/ui/v2/shell.css:275: `.v2-transit-line > span:not(.v2-transit-count) { flex: 1; font-size: 12px; opacity: 0.75; }`
- feature style `v2-transit-count` — src/ui/v2/shell.css:276: `.v2-transit-count { min-width: 18px; text-align: center; font-weight: 700; }`
- feature style `v2-side` — src/ui/v2/shell.css:280: `.v2-side { position: absolute; right: 12px; top: calc(12px + var(--v-hud-h) + 10px); width: 340px; max-height: calc(100dvh - var(--v-hud-h) - var(--v-dock-h) - `
- feature style `v2-side` — src/ui/v2/shell.css:281: `.v2-side .inspector { position: static !important; display: block; flex: 1 1 auto; min-height: 0; overflow-y: auto; width: 100% !important; max-height: none !im`
- feature style `v2-side` — src/ui/v2/shell.css:282: `.v2-side .inspector[data-more-y="bottom"] { mask-image: linear-gradient(to bottom, #000 calc(100% - 48px), transparent); }`
- feature style `v2-side` — src/ui/v2/shell.css:283: `.v2-side .inspector[data-more-y="top"] { mask-image: linear-gradient(to top, #000 calc(100% - 48px), transparent); }`
- feature style `v2-side` — src/ui/v2/shell.css:284: `.v2-side .inspector[data-more-y="both"] { mask-image: linear-gradient(to bottom, transparent, #000 48px, #000 calc(100% - 48px), transparent); }`
- feature style `v2-side` — src/ui/v2/shell.css:286: `.v2-side .inspect-select { font-size: 11.5px; }`
- feature style `v2-side` — src/ui/v2/shell.css:287: `.v2-side .inspect-select input[type="number"], .v2-side .inspect-select select { width: 100%; height: 32px; padding: 0 8px; border: 1px solid var(--v-line); bor`
- feature style `v2-side` — src/ui/v2/shell.css:291: `.v2-side .inspect-select input[type="number"]:focus, .v2-side .inspect-select select:focus { outline: none; border-color: var(--v-accent); }`
- feature style `v2-tip` — src/ui/v2/shell.css:307: `.v2-tip { position: fixed; z-index: 20; pointer-events: none; padding: 4px 8px; border: 1px solid var(--v-line); border-radius: 6px; background: var(--v-bg-soli`
- feature style `v2-toast` — src/ui/v2/shell.css:308: `.v2-toast { position: absolute; left: 50%; top: calc(12px + var(--v-hud-h) + 10px); transform: translateX(-50%); z-index: 10; max-width: min(520px, calc(100vw -`
- feature style `v2-stats` — src/ui/v2/shell.css:312: `@media (max-width: 1100px) { .v2-stats { display: none; } }`
- feature style `v2` — src/ui/v2/shell.css:316: `@media (781px <= width < 1160px) { .v2 > .v2-dock { left: calc(24px + var(--v-opts-w)); transform: none; max-width: calc(100vw - var(--v-opts-w) - 36px); } }`
- feature style `v2-hud` — src/ui/v2/shell.css:322: `@media (max-width: 780px) { .v2-hud { top: 8px; left: 8px; right: 8px; grid-template-columns: 1fr auto; padding: 0 8px; } }`
- feature style `v2-speed` — src/ui/v2/shell.css:323: `@media (max-width: 780px) { .v2-speed, .v2-stats, .v2-sky { display: none; } }`
- feature style `v2-dock` — src/ui/v2/shell.css:324: `@media (max-width: 780px) { .v2-dock { left: 8px; right: 8px; bottom: 8px; transform: none; justify-content: space-between; } }`
- feature style `v2-cat` — src/ui/v2/shell.css:325: `@media (max-width: 780px) { .v2-cat { width: auto; flex: 1 0 52px; height: 58px; } }`
- feature style `v2-drawer` — src/ui/v2/shell.css:326: `@media (max-width: 780px) { .v2-drawer { left: 8px; right: 8px; width: auto; transform: none; bottom: calc(8px + var(--v-dock-h) + 8px); max-height: 52dvh; } }`
- feature style `v2-tabs` — src/ui/v2/shell.css:327: `@media (max-width: 780px) { .v2-tabs { flex-wrap: nowrap; overflow-x: auto; } }`
- feature style `v2-toolopts` — src/ui/v2/shell.css:328: `@media (max-width: 780px) { .v2-toolopts { left: 8px; bottom: calc(8px + var(--v-dock-h) + 8px + 160px); max-height: 30dvh; } }`
- feature style `v2-side` — src/ui/v2/shell.css:329: `@media (max-width: 780px) { .v2-side, .v2-shell #builder .bw-inspector { right: 8px; width: min(300px, calc(100vw - 16px)); } }`
- feature style `v2-dd` — src/ui/v2/shell.css:333: `.v2-dd { display: inline-flex; align-items: center; justify-content: space-between; gap: 6px; height: 30px; max-width: 100%; min-width: 0; padding: 0 8px 0 10px`
- feature style `v2-dd` — src/ui/v2/shell.css:337: `.v2-dd:hover, .v2-dd[aria-expanded="true"] { border-color: var(--v-accent-line); background: var(--v-raise-2); }`
- feature style `v2-dd-value` — src/ui/v2/shell.css:338: `.v2-dd-value { overflow: hidden; text-overflow: ellipsis; white-space: nowrap; }`
- feature style `v2-dd` — src/ui/v2/shell.css:339: `.v2-dd > svg { flex: none; color: var(--v-muted); }`
- feature style `v2-orow` — src/ui/v2/shell.css:340: `.v2-orow .v2-dd { flex: 1; }`
- feature style `v2-dd-list` — src/ui/v2/shell.css:341: `.v2-dd-list { position: fixed; z-index: 12; pointer-events: auto; display: grid; gap: 1px; max-height: min(320px, 60dvh); overflow-y: auto; padding: 4px; border`
- feature style `v2-dd-opt` — src/ui/v2/shell.css:346: `.v2-dd-opt { height: 30px; padding: 0 10px; border: 0; border-radius: 6px; background: none; color: var(--v-text); font-size: 12.5px; text-align: left; white-sp`
- feature style `v2-dd-opt` — src/ui/v2/shell.css:347: `.v2-dd-opt:hover { background: var(--v-raise-2); }`
- feature style `v2-dd-opt` — src/ui/v2/shell.css:348: `.v2-dd-opt.on { color: var(--v-accent); font-weight: 650; }`
- feature style `v2-pop-title` — src/ui/v2/shell.css:351: `.v2-pop-title { padding: 6px 10px 4px; color: var(--v-muted); font-size: 10.5px; font-weight: 700; letter-spacing: 0.06em; text-transform: uppercase; }`
- feature style `v2-note` — src/ui/v2/shell.css:354: `.v2-note { max-width: 520px; margin: 0; padding: 4px 6px; color: var(--v-muted); font-size: 12.5px; line-height: 1.4; white-space: normal; }`
- feature style `v2` — src/ui/v2/shell.css:360: `.v2 input[type="range"].v2-range { accent-color: var(--v-accent); color-scheme: dark; cursor: pointer; }`
- feature style `v2-compass` — src/ui/v2/shell.css:374: `.v2-compass { position: absolute; right: 16px; bottom: 16px; width: 52px; height: 52px; padding: 0; pointer-events: auto; touch-action: none; border-radius: 50%`
- feature style `v2-compass` — src/ui/v2/shell.css:380: `.v2-compass:hover { border-color: var(--v-accent-line); transform: scale(1.06); }`
- feature style `v2-compass` — src/ui/v2/shell.css:381: `.v2-compass.held { cursor: grabbing; border-color: var(--v-accent); }`
- feature style `v2-compass` — src/ui/v2/shell.css:382: `.v2-compass svg { display: block; width: 100%; height: 100%; }`
- feature style `v2-compass` — src/ui/v2/shell.css:387: `.v2-compass text { fill: #fff; font: 700 8px/1 Inter, ui-sans-serif, sans-serif; pointer-events: none; }`
- feature style `v2-compass` — src/ui/v2/shell.css:389: `@media (max-width: 780px) { .v2-compass { right: 8px; bottom: calc(8px + var(--v-dock-h) + 8px); width: 44px; height: 44px; } }`
- feature style `v2-start` — src/ui/v2/shell.css:393: `.v2-start { position: absolute; inset: 0; z-index: 30; display: grid; place-items: center; pointer-events: auto; background: radial-gradient(ellipse at 50% 40%,`
- feature style `v2-start-card` — src/ui/v2/shell.css:395: `.v2-start-card { display: grid; gap: 10px; width: min(380px, calc(100vw - 32px)); padding: 32px 28px 22px; border-radius: 18px; background: var(--v-bg); border:`
- feature style `v2-start-card` — src/ui/v2/shell.css:397: `.v2-start-card h1 { margin: 0; font-size: 40px; font-weight: 800; letter-spacing: 0.01em; }`
- feature style `v2-start-tag` — src/ui/v2/shell.css:398: `.v2-start-tag { margin: 0 0 14px; color: var(--v-muted); font-size: 14px; }`
- feature style `v2-start-b` — src/ui/v2/shell.css:399: `.v2-start-b { height: 46px; border: 1px solid var(--v-line); border-radius: 12px; background: var(--v-raise); font-size: 16px; font-weight: 600; }`
- feature style `v2-start-b` — src/ui/v2/shell.css:400: `.v2-start-b:hover { background: var(--v-raise-2); border-color: var(--v-accent-line); }`
- feature style `v2-start-b` — src/ui/v2/shell.css:401: `.v2-start-b.primary { background: var(--v-accent); color: var(--v-accent-ink); border-color: var(--v-accent); font-size: 17px; }`
- feature style `v2-start-b` — src/ui/v2/shell.css:402: `.v2-start-b.primary:hover { filter: brightness(1.08); }`
- feature style `v2-start-langs` — src/ui/v2/shell.css:403: `.v2-start-langs { display: flex; justify-content: center; gap: 6px; margin-top: 8px; }`
- feature style `v2-start-lang` — src/ui/v2/shell.css:404: `.v2-start-lang { padding: 6px 12px; border: 1px solid var(--v-line); border-radius: 999px; background: none; color: var(--v-muted); }`
- feature style `v2-start-lang` — src/ui/v2/shell.css:405: `.v2-start-lang.on { color: var(--v-accent-ink); background: var(--v-accent); border-color: var(--v-accent); }`
- feature style `v2-start-about` — src/ui/v2/shell.css:406: `.v2-start-about { justify-self: center; margin-top: 4px; border: 0; background: none; color: var(--v-muted); font-size: 13px; text-decoration: underline; }`

## Where the program plugs the feature in

### src/core/dynamicResolution.ts

- `src/core/dynamicResolution.ts#DynamicResolution` (class, line 16) ← construct from `src/graphicsWiring.ts#createGraphicsWiring`
- `src/core/dynamicResolution.ts#DynamicResolution.reset` (method, line 58) ← call from `src/graphicsWiring.ts#createGraphicsWiring`, `src/graphicsWiring.ts#createGraphicsWiring.apply`
- `src/core/dynamicResolution.ts#DynamicResolution.sample` (method, line 30) ← call from `src/graphicsWiring.ts#createGraphicsWiring`

### src/desktopSaves.ts

- `src/desktopSaves.ts#desktopSaves` (function, line 35) ← call from `src/main.ts#desktopStores`
- `src/desktopSaves.ts#desktopSaves` (function, line 35) ← implementation from `src/main.ts#<IfStatement>@210`
- `src/desktopSaves.ts#desktopSaves.onLost` (method, line 113) ← implementation from `src/main.ts#<IfStatement>@210`
- `src/desktopSaves.ts#DesktopStores` (interface, line 27) ← call from `src/main.ts#<IfStatement>@210`
- `src/desktopSaves.ts#DesktopStores` (interface, line 27) ← read from `src/main.ts#persistence`

### src/desktopShown.ts

- `src/desktopShown.ts#DesktopShown` (interface, line 2) ← read from `src/main.ts#pageHidden`
- `src/desktopShown.ts#watchDesktopShown` (function, line 8) ← call from `src/main.ts#desktopShown`

### src/editor/cookedCities.ts

- `src/editor/cookedCities.ts#COOKED_CITY_SEEDS` (variable, line 6) ← read from `src/main.ts#nextCitySeed`
- `src/editor/cookedCities.ts#CookedCity` (interface, line 19) ← read from `src/main.ts#cityAhead`, `src/main.ts#cityData`
- `src/editor/cookedCities.ts#readCookedCity` (function, line 33) ← call from `src/main.ts#cityAhead`, `src/main.ts#cityData`

### src/editor/gameKeys.ts

- `src/editor/gameKeys.ts#createGameKeys` (function, line 69) ← call from `src/main.ts#gameKeys`
- `src/editor/gameKeys.ts#createGameKeys` (function, line 69) ← callbackAssign from `src/core/commands.ts#CommandRegistry.press`
- `src/editor/gameKeys.ts#createGameKeys` (function, line 69) ← implementation from `src/core/commands.ts#CommandRegistry.press`
- `src/editor/gameKeys.ts#createGameKeys.command` (function, line 72) ← callbackAssign from `src/core/commands.ts#CommandRegistry.run`
- `src/editor/gameKeys.ts#createGameKeys.command` (function, line 72) ← implementation from `src/core/commands.ts#CommandRegistry.run`

### src/editor/history.ts

- `src/editor/history.ts#followNetwork` (function, line 159) ← call from `src/main.ts#afterRestore`, `src/main.ts#mutateBuilt`, `src/main.ts#openSession`
- `src/editor/history.ts#History` (class, line 10) ← construct from `src/main.ts#history`
- `src/editor/history.ts#History` (class, line 10) ← typeRef from `src/buildingsWiring.ts#BuildingWiringDeps`
- `src/editor/history.ts#History.begin` (method, line 39) ← call from `src/buildingsWiring.ts#createBuildingWiring`, `src/main.ts#<IfStatement>@4036`, `src/main.ts#cloudTool`, `src/main.ts#mutateBuilt`, …
- `src/editor/history.ts#History.cancel` (method, line 52) ← call from `src/main.ts#openSession`
- `src/editor/history.ts#History.canRedo` (getter, line 34) ← call from `src/buildingsWiring.ts#createBuildingWiring.state`, `src/main.ts#updateHistoryButtons`
- `src/editor/history.ts#History.canUndo` (getter, line 30) ← call from `src/buildingsWiring.ts#createBuildingWiring.state`, `src/main.ts#updateHistoryButtons`
- `src/editor/history.ts#History.end` (method, line 46) ← call from `src/buildingsWiring.ts#createBuildingWiring`, `src/main.ts#endPointer`, `src/main.ts#endTerrainStroke`, `src/main.ts#mutateBuilt`, …
- `src/editor/history.ts#History.grouped` (method, line 60) ← call from `src/main.ts#roadTool`
- `src/editor/history.ts#History.redo` (method, line 88) ← call from `src/main.ts#redo`
- `src/editor/history.ts#History.undo` (method, line 66) ← call from `src/main.ts#undo`
- `src/editor/history.ts#openedGenerated` (function, line 171) ← call from `src/main.ts#networkBakeOf`
- `src/editor/history.ts#restoreInto` (function, line 176) ← call from `src/main.ts#applySnapshot`, `src/main.ts#restoreBootSave`
- `src/editor/history.ts#restoreSnapshot` (function, line 188) ← call from `src/main.ts#applySnapshot`
- `src/editor/history.ts#serialize` (function, line 164) ← call from `src/main.ts#serializedDoc`

### src/editor/persistence.ts

- `src/editor/persistence.ts#importFromFile` (function, line 436) ← call from `src/main.ts#<IfStatement>@4036`, `src/main.ts#openMap`
- `src/editor/persistence.ts#ImportResult` (type, line 430) ← read from `src/main.ts#openImported`
- `src/editor/persistence.ts#ImportResult` (type, line 430) ← typeRef from `src/main.ts#openImported`, `src/main.ts#openSession`
- `src/editor/persistence.ts#MAX_INTENSITY` (variable, line 395) ← read from `src/main.ts#SLIDERS`
- `src/editor/persistence.ts#Persistence` (class, line 61) ← construct from `src/main.ts#persistence`
- `src/editor/persistence.ts#Persistence` (class, line 61) ← typeRef from `src/main.ts#restoreBootSave`
- `src/editor/persistence.ts#Persistence.adoptLoaded` (method, line 80) ← call from `src/main.ts#restoreBootSave`
- `src/editor/persistence.ts#Persistence.documentText` (field, line 103) ← callbackAssign from `src/main.ts#<ExpressionStatement>@874`
- `src/editor/persistence.ts#Persistence.documentText` (field, line 103) ← implementation from `src/main.ts#<ExpressionStatement>@874`
- `src/editor/persistence.ts#Persistence.documentText` (field, line 103) ← write from `src/main.ts#<ExpressionStatement>@874`
- `src/editor/persistence.ts#Persistence.flush` (method, line 266) ← call from `src/main.ts#<ExpressionStatement>@3901`, `src/main.ts#<IfStatement>@4036`, `src/main.ts#comesBackFromLostContext`, `src/main.ts#desktopShown`
- `src/editor/persistence.ts#Persistence.flush` (method, line 266) ← schedule from `src/main.ts#comesBackFromLostContext`
- `src/editor/persistence.ts#Persistence.loadSessionAsync` (method, line 166) ← call from `src/main.ts#bootSave`
- `src/editor/persistence.ts#Persistence.onSaveFailed` (field, line 68) ← callbackAssign from `src/main.ts#<ExpressionStatement>@3908`
- `src/editor/persistence.ts#Persistence.onSaveFailed` (field, line 68) ← implementation from `src/main.ts#<ExpressionStatement>@3908`
- `src/editor/persistence.ts#Persistence.onSaveFailed` (field, line 68) ← write from `src/main.ts#<ExpressionStatement>@3908`
- `src/editor/persistence.ts#Persistence.quarantineStored` (method, line 319) ← call from `src/main.ts#restoreBootSave`
- `src/editor/persistence.ts#Persistence.readSettings` (method, line 253) ← call from `src/main.ts#bootSettings`
- `src/editor/persistence.ts#Persistence.saveSession` (method, line 110) ← call from `src/main.ts#<ExpressionStatement>@3904`, `src/main.ts#saveOnClose`
- `src/editor/persistence.ts#Persistence.saveSessionSoon` (method, line 203) ← call from `src/main.ts#<IfStatement>@1703`, `src/main.ts#<IfStatement>@4036`, `src/main.ts#afterRestore`, `src/main.ts#buildings`, …
- `src/editor/persistence.ts#Persistence.saveSessionSoon` (method, line 203) ← schedule from `src/main.ts#comesBackFromLostContext`
- `src/editor/persistence.ts#Persistence.saveSettingsSoon` (method, line 227) ← call from `src/main.ts#cameraCommand`, `src/main.ts#cameraHand`, `src/main.ts#endPointer`, `src/main.ts#frame`, …
- `src/editor/persistence.ts#Persistence.written` (method, line 109) ← call from `src/main.ts#<IfStatement>@4036`, `src/main.ts#saveOnClose`
- `src/editor/persistence.ts#SavedSession` (interface, line 55) ← read from `src/main.ts#openSession`, `src/main.ts#restoreBootSave`
- `src/editor/persistence.ts#SavedSettings` (interface, line 28) ← read from `src/main.ts#<ExpressionStatement>@385`, `src/main.ts#<ExpressionStatement>@386`, `src/main.ts#<ExpressionStatement>@387`, `src/main.ts#<ExpressionStatement>@388`, …
- `src/editor/persistence.ts#SavedSettings` (interface, line 28) ← typeRef from `src/editor/mapExport.ts#exportToFile`, `src/editor/mapExport.ts#mapFileBlob`, `src/main.ts#restoreBootSave`, `src/main.ts#restoreOrbit`, …
- `src/editor/persistence.ts#withUnit` (function, line 53) ← call from `src/editor/mapExport.ts#mapFileBlob`

### src/editor/saveBinary.ts

- `src/editor/saveBinary.ts#FILE_MAGIC` (variable, line 12) ← read from `src/editor/mapExport.ts#mapFileBlob`
- `src/editor/saveBinary.ts#FILE_SETTINGS,FILE_DOC` (variable, line 14) ← read from `src/editor/mapExport.ts#mapFileBlob`
- `src/editor/saveBinary.ts#framePieces` (function, line 25) ← call from `src/editor/mapExport.ts#mapFileBlob`
- `src/editor/saveBinary.ts#jsonBytes` (function, line 80) ← call from `src/editor/mapExport.ts#mapFileBlob`

### src/editor/saveStore.ts

- `src/editor/saveStore.ts#PartStore` (interface, line 6) ← typeRef from `src/editor/workerParts.ts#backend`, `src/editor/workerParts.ts#give`, `src/editor/workerParts.ts#portParts`, `src/editor/workerParts.ts#takeStoreSetup`, …
- `src/editor/saveStore.ts#SaveBatch` (interface, line 19) ← typeRef from `src/editor/save.worker.ts#SaveMessage`
- `src/editor/saveStore.ts#writeBatch` (function, line 39) ← call from `src/editor/save.worker.ts#serve`

### src/render/canyonLight.ts

- `src/render/canyonLight.ts#CanyonLight` (interface, line 37) ← call from `src/render/postprocess.ts#createPostChain`, `src/render/postprocess.ts#createPostChain.disposeAll`, `src/render/postprocess.ts#createPostChain.followTown`, `src/render/postprocess.ts#createPostChain.setAtmosphere`
- `src/render/canyonLight.ts#CanyonLight` (interface, line 37) ← read from `src/render/postprocess.ts#createPostChain`, `src/render/postprocess.ts#createPostChain.render`
- `src/render/canyonLight.ts#createCanyonLight` (function, line 198) ← call from `src/render/postprocess.ts#createPostChain`
- `src/render/canyonLight.ts#createCanyonLight.dispose` (method, line 372) ← implementation from `src/render/postprocess.ts#createPostChain.disposeAll`
- `src/render/canyonLight.ts#createCanyonLight.follow` (method, line 325) ← implementation from `src/render/postprocess.ts#createPostChain.followTown`
- `src/render/canyonLight.ts#createCanyonLight.setOcclusion` (method, line 361) ← implementation from `src/render/postprocess.ts#createPostChain`
- `src/render/canyonLight.ts#createCanyonLight.setSun` (method, line 366) ← implementation from `src/render/postprocess.ts#createPostChain.setAtmosphere`

### src/render/effectSwitches.ts

- `src/render/effectSwitches.ts#ALL_EFFECTS_ON` (variable, line 24) ← read from `src/effectsWiring.ts#readKept`, `src/main.ts#<IfStatement>@4036`, `src/render/postprocess.ts#createPostChain`, `src/render/renderer.ts#createSceneRenderer`
- `src/render/effectSwitches.ts#BUILDING_SWITCHES` (variable, line 13) ← read from `src/effectsWiring.ts#createEffectsWiring`, `src/lab/catalog/city.ts#drawCallsByGroup`, `src/lab/tests/items.ts#SWITCH_GROUPS`
- `src/render/effectSwitches.ts#BUILDING_SWITCHES` (variable, line 13) ← typeRef from `src/lab/inspector/related.ts#BUILDING_ROOTS`, `src/lab/inspector/related.ts#buildingKindSwitch`
- `src/render/effectSwitches.ts#BuildingChoices` (interface, line 29) ← read from `src/render/buildingSwitches.ts#createBuildingSwitcher.beforeRender`, `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.applyQuality`, `src/render/renderer.ts#createSceneRenderer.governGeometry`
- `src/render/effectSwitches.ts#BuildingChoices` (interface, line 29) ← typeRef from `src/effectsWiring.ts#BUILDING_CHOICES`, `src/effectsWiring.ts#Kept`, `src/effectsWiring.ts#readKept`, `src/main.ts#<IfStatement>@4036`, …
- `src/render/effectSwitches.ts#BuildingSwitch` (type, line 19) ← typeRef from `src/render/buildingSwitches.ts#KIND_SWITCH`, `src/render/buildingSwitches.ts#KIND_SWITCHES`, `src/render/buildingSwitches.ts#kindOf`, `src/render/buildingSwitches.ts#ROOT_SWITCH`
- `src/render/effectSwitches.ts#createFrameHider` (function, line 49) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/effectSwitches.ts#createFrameHider.hide` (method, line 52) ← implementation from `src/render/buildingSwitches.ts#createBuildingSwitcher.beforeRender`, `src/render/renderer.ts#createSceneRenderer`
- `src/render/effectSwitches.ts#createFrameHider.restore` (method, line 59) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/effectSwitches.ts#DEFAULT_BUILDING_CHOICES` (variable, line 37) ← read from `src/effectsWiring.ts#createEffectsWiring`, `src/effectsWiring.ts#readKept`, `src/main.ts#<IfStatement>@4036`, `src/render/renderer.ts#createSceneRenderer`
- `src/render/effectSwitches.ts#EffectSwitch` (type, line 20) ← typeRef from `src/effectsWiring.ts#createEffectsWiring`, `src/effectsWiring.ts#createEffectsWiring.isOn`
- `src/render/effectSwitches.ts#EffectSwitches` (type, line 21) ← typeRef from `src/effectsWiring.ts#Kept`, `src/main.ts#<IfStatement>@4036`, `src/render/buildingSwitches.ts#BuildingSwitcher`, `src/render/renderer.ts#createSceneRenderer`, …
- `src/render/effectSwitches.ts#FrameHider` (interface, line 44) ← call from `src/render/buildingSwitches.ts#createBuildingSwitcher.beforeRender`, `src/render/renderer.ts#createSceneRenderer`
- `src/render/effectSwitches.ts#FrameHider` (interface, line 44) ← typeRef from `src/render/buildingSwitches.ts#BuildingSwitcher`
- `src/render/effectSwitches.ts#isEffectSwitch` (function, line 39) ← call from `src/effectsWiring.ts#createEffectsWiring.isOn`, `src/effectsWiring.ts#createEffectsWiring.set`, `src/effectsWiring.ts#readKept`
- `src/render/effectSwitches.ts#LAYER_SWITCHES` (variable, line 10) ← read from `src/effectsWiring.ts#createEffectsWiring`, `src/lab/catalog/city.ts#drawCallsByGroup`, `src/lab/tests/items.ts#SWITCH_GROUPS`
- `src/render/effectSwitches.ts#LAYER_SWITCHES` (variable, line 10) ← typeRef from `src/lab/inspector/related.ts#LAYER_BY_NAME`
- `src/render/effectSwitches.ts#LIGHT_SWITCHES` (variable, line 8) ← read from `src/effectsWiring.ts#createEffectsWiring`, `src/lab/tests/items.ts#SWITCH_GROUPS`
- `src/render/effectSwitches.ts#POST_SWITCHES` (variable, line 6) ← read from `src/effectsWiring.ts#createEffectsWiring`, `src/lab/tests/items.ts#SWITCH_GROUPS`
- `src/render/effectSwitches.ts#PostSwitches` (type, line 22) ← typeRef from `src/render/postprocess.ts#createPostChain`, `src/render/postprocess.ts#PostChain`

### src/render/isoViewport.ts

- `src/render/isoViewport.ts#createIsoRig` (function, line 78) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/isoViewport.ts#createIsoRig` (function, line 78) ← implementation from `src/actionsWiring.ts#createActions.buildingsUnderRay`, `src/actionsWiring.ts#createActions.rayOnBuilding`, `src/actionsWiring.ts#createActions.shootAt`, `src/buildingsWiring.ts#createBuildingWiring`, …
- `src/render/isoViewport.ts#createIsoRig.resize` (method, line 454) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/isoViewport.ts#createIsoRig.setGround` (method, line 424) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/isoViewport.ts#createIsoRig.setPerspective` (method, line 416) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/isoViewport.ts#createIsoRig.setSolids` (method, line 449) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/isoViewport.ts#createIsoRig.settle` (method, line 428) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/isoViewport.ts#DEFAULT_AZIMUTH` (variable, line 27) ← read from `src/main.ts#cameraCommand`, `src/main.ts#gameKeys`, `src/main.ts#restoreOrbit`
- `src/render/isoViewport.ts#DEFAULT_ELEVATION` (variable, line 28) ← read from `src/main.ts#cameraCommand`, `src/main.ts#gameKeys`, `src/main.ts#restoreOrbit`
- `src/render/isoViewport.ts#IsoRig` (interface, line 48) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/isoViewport.ts#IsoRig` (interface, line 48) ← read from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.applyQuality`, `src/render/renderer.ts#createSceneRenderer.buildWorld`, `src/render/renderer.ts#createSceneRenderer.compileChanged`, …
- `src/render/isoViewport.ts#isoZoomBounds` (function, line 22) ← call from `src/main.ts#bootZoomBounds`
- `src/render/isoViewport.ts#PERSPECTIVE_FOV` (variable, line 35) ← read from `src/render/renderer.ts#createSceneRenderer`

### src/render/occlusion.ts

- `src/render/occlusion.ts#OcclusionBuffer` (class, line 36) ← construct from `src/render/renderer.ts#createSceneRenderer`
- `src/render/occlusion.ts#OcclusionBuffer.build` (method, line 57) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/occlusion.ts#OcclusionBuffer.clear` (method, line 90) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/occlusion.ts#OcclusionBuffer.hidden` (method, line 95) ← call from `src/render/renderer.ts#createSceneRenderer.occlusionHidden`
- `src/render/occlusion.ts#OcclusionBuffer.stats` (field, line 54) ← read from `src/render/renderer.ts#createSceneRenderer`
- `src/render/occlusion.ts#OcclusionStats` (interface, line 25) ← typeRef from `src/render/renderer.ts#SceneHandle`

### src/render/optimizations.ts

- `src/render/optimizations.ts#affectsTarget` (function, line 368) ← call from `src/lab/optimizations/index.ts#createOptimizationTabs`
- `src/render/optimizations.ts#DEFAULT_OPTIMIZATIONS` (variable, line 289) ← read from `src/effectsWiring.ts#createEffectsWiring`, `src/effectsWiring.ts#createEffectsWiring.attach`, `src/effectsWiring.ts#createEffectsWiring.saveOptimizations`, `src/effectsWiring.ts#readOptimizations`, …
- `src/render/optimizations.ts#OptId` (type, line 279) ← typeRef from `src/effectsWiring.ts#createEffectsWiring`, `src/effectsWiring.ts#createEffectsWiring.attach`, `src/effectsWiring.ts#createEffectsWiring.saveOptimizations`, `src/effectsWiring.ts#createEffectsWiring.setOptimizations`, …
- `src/render/optimizations.ts#optimizationChoice` (function, line 296) ← call from `src/effectsWiring.ts#createEffectsWiring`, `src/effectsWiring.ts#readOptimizations`
- `src/render/optimizations.ts#Optimizations` (type, line 287) ← typeRef from `src/effectsWiring.ts#OptimizationControl`, `src/effectsWiring.ts#readOptimizations`, `src/main.ts#<IfStatement>@4036`, `src/render/renderer.ts#createSceneRenderer`, …
- `src/render/optimizations.ts#OPTIMIZATIONS` (variable, line 282) ← read from `src/effectsWiring.ts#createEffectsWiring`
- `src/render/optimizations.ts#optimizationSpec` (function, line 293) ← call from `src/effectsWiring.ts#createEffectsWiring`
- `src/render/optimizations.ts#OptimizationSpec` (interface, line 20) ← read from `src/effectsWiring.ts#createEffectsWiring`, `src/lab/optimizations/index.ts#createOptimizationTabs.drawList`, `src/lab/optimizations/index.ts#createOptimizationTabs.rowElement`, `src/lab/optimizations/index.ts#rowOf`
- `src/render/optimizations.ts#OptimizationSpec` (interface, line 20) ← typeRef from `src/effectsWiring.ts#OptimizationState`
- `src/render/optimizations.ts#OptTarget` (interface, line 346) ← typeRef from `src/lab/optimizations/index.ts#createOptimizationTabs`
- `src/render/optimizations.ts#qualityOptionsDiffer` (function, line 314) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/optimizations.ts#withOptimizations` (function, line 319) ← call from `src/render/renderer.ts#createSceneRenderer`

### src/render/optimizationsApply.ts

- `src/render/optimizationsApply.ts#createOptimizer` (function, line 78) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/optimizationsApply.ts#createOptimizer.afterRender` (method, line 249) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/optimizationsApply.ts#createOptimizer.beforeRender` (method, line 237) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/optimizationsApply.ts#createOptimizer.dispose` (method, line 258) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/optimizationsApply.ts#createOptimizer.postSwitches` (method, line 194) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/optimizationsApply.ts#createOptimizer.reliefDue` (method, line 235) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/optimizationsApply.ts#createOptimizer.report` (method, line 257) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/optimizationsApply.ts#createOptimizer.set` (method, line 199) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/optimizationsApply.ts#createOptimizer.shadowDue` (method, line 234) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/optimizationsApply.ts#createOptimizer.uploadsDue` (method, line 236) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/optimizationsApply.ts#Optimizer` (interface, line 47) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/optimizationsApply.ts#Optimizer` (interface, line 47) ← read from `src/render/renderer.ts#createSceneRenderer`

### src/render/quality.ts

- `src/render/quality.ts#GEOMETRY_BUDGETS` (variable, line 110) ← read from `src/render/renderer.ts#createSceneRenderer.budgetNow`
- `src/render/quality.ts#GeometryBudget` (interface, line 101) ← read from `src/render/renderer.ts#createSceneRenderer.applyGeometryBudget`, `src/render/renderer.ts#createSceneRenderer.governGeometry`
- `src/render/quality.ts#GeometryBudget` (interface, line 101) ← typeRef from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.budgetNow`, `src/render/renderer.ts#SceneHandle`
- `src/render/quality.ts#HAIR_STRAND_PEOPLE` (variable, line 10) ← read from `src/render/people/proceduralCrowd.ts#createProceduralCrowd`, `src/render/people/proceduralCrowd.ts#createProceduralCrowd.update`
- `src/render/quality.ts#isQualityLevel` (function, line 268) ← call from `src/graphicsWiring.ts#isQualityPreset`, `src/main.ts#lab`
- `src/render/quality.ts#QUALITY` (variable, line 122) ← read from `src/graphicsWiring.ts#settingsFor`, `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.applyQuality`
- `src/render/quality.ts#QUALITY_LEVELS` (variable, line 266) ← read from `src/main.ts#mountInterface`
- `src/render/quality.ts#QualityGovernor` (class, line 287) ← construct from `src/graphicsWiring.ts#createGraphicsWiring`
- `src/render/quality.ts#QualityGovernor.sample` (method, line 318) ← call from `src/graphicsWiring.ts#createGraphicsWiring`
- `src/render/quality.ts#QualityGovernor.set` (method, line 309) ← call from `src/graphicsWiring.ts#createGraphicsWiring`
- `src/render/quality.ts#QualityLevel` (type, line 5) ← typeRef from `src/graphicsWiring.ts#createGraphicsWiring`, `src/graphicsWiring.ts#GraphicsWiring`, `src/graphicsWiring.ts#QualityPreset`, `src/graphicsWiring.ts#settingsFor`, …
- `src/render/quality.ts#QualitySettings` (interface, line 12) ← read from `src/graphicsWiring.ts#createGraphicsWiring.choices`, `src/graphicsWiring.ts#createGraphicsWiring.fullRatio`, `src/graphicsWiring.ts#settingsFor`, `src/render/agents.ts#createAgentMeshes.sync`, …
- `src/render/quality.ts#QualitySettings` (interface, line 12) ← typeRef from `src/graphicsWiring.ts#GraphicsWiring`, `src/graphicsWiring.ts#settingsFor`, `src/render/agents.ts#AgentRenderOptions`, `src/render/postprocess.ts#createPostChain`, …
- `src/render/quality.ts#VEHICLE_ERROR_PIXELS` (variable, line 8) ← read from `src/render/vehicleLevels.ts#ERROR_PIXELS`
- `src/render/quality.ts#VEHICLE_WHOLE_CAP` (variable, line 9) ← read from `src/render/agents.ts#VEHICLE_NEAR_CAP`

### src/render/revealGate.ts

- `src/render/revealGate.ts#RevealGate` (class, line 9) ← construct from `src/render/renderer.ts#createSceneRenderer`
- `src/render/revealGate.ts#RevealGate.begin` (method, line 16) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/revealGate.ts#RevealGate.opened` (field, line 11) ← read from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.pumpWorld`, `src/render/renderer.ts#createSceneRenderer.watchRoads`
- `src/render/revealGate.ts#RevealGate.step` (method, line 27) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/revealGate.ts#RevealGate.worked` (getter, line 23) ← call from `src/render/renderer.ts#createSceneRenderer`

### src/steamAchievements.ts

- `src/steamAchievements.ts#createSteamAchievements` (function, line 10) ← call from `src/main.ts#achievements`, `src/main.ts#frame`
- `src/steamAchievements.ts#createSteamAchievements` (function, line 10) ← implementation from `src/main.ts#frame`
- `src/steamAchievements.ts#createSteamAchievements.roads` (method, line 15) ← implementation from `src/main.ts#frame`

### src/ui/about.ts

- `src/ui/about.ts#mountAbout` (function, line 5) ← call from `src/main.ts#<ExpressionStatement>@1986`
- `src/ui/about.ts#openAbout` (function, line 17) ← reference from `src/main.ts#mountInterface`

### src/ui/buildStamp.ts

- `src/ui/buildStamp.ts#formatBuildStamp` (function, line 20) ← call from `src/lab/recorder/report.ts#buildReport`

### src/ui/chrome.ts

- `src/ui/chrome.ts#focusCameFromKeyboard` (function, line 6) ← call from `src/main.ts#<ExpressionStatement>@2556`, `src/main.ts#gameKeys`
- `src/ui/chrome.ts#trackFocusModality` (function, line 11) ← call from `src/main.ts#<ExpressionStatement>@1984`

### src/ui/effectsPanel.ts

- `src/ui/effectsPanel.ts#createEffectsPanel` (function, line 49) ← call from `src/effectsWiring.ts#createEffectsWiring`
- `src/ui/effectsPanel.ts#createEffectsPanel` (function, line 49) ← implementation from `src/lab/tabs.ts#createLabTabs`
- `src/ui/effectsPanel.ts#EffectsPanel` (interface, line 36) ← call from `src/lab/tabs.ts#createLabTabs`
- `src/ui/effectsPanel.ts#EffectsPanel` (interface, line 36) ← typeRef from `src/effectsWiring.ts#EffectsWiring`
- `src/ui/effectsPanel.ts#EffectsPanelGroup` (interface, line 14) ← typeRef from `src/effectsWiring.ts#createEffectsWiring`

### src/ui/graphicsPanel.ts

- `src/ui/graphicsPanel.ts#createGraphicsPanel` (function, line 35) ← call from `src/graphicsWiring.ts#createGraphicsWiring`
- `src/ui/graphicsPanel.ts#createGraphicsPanel` (function, line 35) ← implementation from `src/graphicsWiring.ts#createGraphicsWiring`
- `src/ui/graphicsPanel.ts#createGraphicsPanel.open` (method, line 106) ← implementation from `src/graphicsWiring.ts#createGraphicsWiring`
- `src/ui/graphicsPanel.ts#GraphicsChoices` (interface, line 7) ← read from `src/graphicsWiring.ts#createGraphicsWiring`, `src/graphicsWiring.ts#pick`
- `src/ui/graphicsPanel.ts#GraphicsChoices` (interface, line 7) ← typeRef from `src/graphicsWiring.ts#createGraphicsWiring.choices`, `src/graphicsWiring.ts#pick`

### src/ui/healthPanel.ts

- `src/ui/healthPanel.ts#mountHealthPanel` (function, line 57) ← call from `src/main.ts#gameKeys`, `src/main.ts#healthPanel`
- `src/ui/healthPanel.ts#mountHealthPanel` (function, line 57) ← implementation from `src/main.ts#gameKeys`
- `src/ui/healthPanel.ts#mountHealthPanel.close` (function, line 244) ← implementation from `src/main.ts#gameKeys`
- `src/ui/healthPanel.ts#mountHealthPanel.toggle` (function, line 230) ← implementation from `src/main.ts#gameKeys`

### src/ui/healthWatch.ts

- `src/ui/healthWatch.ts#HealthWatch` (interface, line 13) ← call from `src/main.ts#frame`
- `src/ui/healthWatch.ts#watchHealth` (function, line 61) ← call from `src/main.ts#healthWatch`
- `src/ui/healthWatch.ts#watchHealth.frameEnded` (method, line 199) ← implementation from `src/main.ts#frame`

### src/ui/i18n/index.ts

- `src/ui/i18n/index.ts#applyTranslations` (function, line 114) ← call from `src/main.ts#<ExpressionStatement>@1611`, `src/ui/builder/workspace.ts#initBuilderWorkspace`
- `src/ui/i18n/index.ts#formatDecimal` (function, line 79) ← call from `src/buildingsWiring.ts#createBuildingWiring.drawOverlay`, `src/buildingsWiring.ts#createBuildingWiring.inspectorFields`, `src/buildingsWiring.ts#createBuildingWiring.loadReference`, `src/main.ts#drawOverlayScreen`, …
- `src/ui/i18n/index.ts#initLanguage` (function, line 136) ← call from `src/main.ts#<ExpressionStatement>@160`
- `src/ui/i18n/index.ts#language` (function, line 50) ← call from `src/ui/builder/workspace.ts#initBuilderWorkspace.renderInspector`, `src/ui/roads/money.ts#formatMoney`
- `src/ui/i18n/index.ts#onLanguageChange` (function, line 108) ← call from `src/main.ts#<ExpressionStatement>@3923`, `src/ui/builder/workspace.ts#initBuilderWorkspace`
- `src/ui/i18n/index.ts#parseDecimal` (function, line 90) ← call from `src/ui/builder/workspace.ts#initBuilderWorkspace.renderInspector`
- `src/ui/i18n/index.ts#plural` (function, line 73) ← call from `src/buildingsWiring.ts#createBuildingWiring.drawOverlay`, `src/main.ts#laneChoices`
- `src/ui/i18n/index.ts#setGlobalParams` (function, line 68) ← call from `src/main.ts#<ExpressionStatement>`
- `src/ui/i18n/index.ts#t` (function, line 55) ← call from `src/buildingsWiring.ts#createBuildingWiring`, `src/buildingsWiring.ts#createBuildingWiring.drawOverlay`, `src/buildingsWiring.ts#createBuildingWiring.inspectorFields`, `src/buildingsWiring.ts#createBuildingWiring.materialLine`, …

### src/ui/inspector.ts

- `src/ui/inspector.ts#closeInspector` (function, line 160) ← call from `src/main.ts#closeInspector`
- `src/ui/inspector.ts#onInspectorChange` (function, line 156) ← call from `src/main.ts#<ExpressionStatement>@2406`
- `src/ui/inspector.ts#openInspector` (function, line 86) ← call from `src/main.ts#showInspector`
- `src/ui/inspector.ts#refreshInspector` (function, line 99) ← call from `src/main.ts#<ExpressionStatement>@3923`, `src/main.ts#frame`, `src/main.ts#mutateBuilt`

### src/ui/labels.ts

- `src/ui/labels.ts#peopleCountLabel` (function, line 20) ← call from `src/main.ts#currentStatus`
- `src/ui/labels.ts#roadTypeDescription` (function, line 9) ← call from `src/main.ts#roadClasses`
- `src/ui/labels.ts#roadTypeName` (function, line 6) ← call from `src/main.ts#roadClasses`
- `src/ui/labels.ts#vehicleCountLabel` (function, line 19) ← call from `src/main.ts#currentStatus`

### src/ui/profilerHud.ts

- `src/ui/profilerHud.ts#mountProfilerHud` (function, line 4) ← call from `src/main.ts#wireProfiler`

### src/ui/thunder.ts

- `src/ui/thunder.ts#playThunder` (function, line 31) ← call from `src/main.ts#<ExpressionStatement>@1674`

### src/ui/toolChoices.ts

- `src/ui/toolChoices.ts#blockGridChoice` (variable, line 208) ← read from `src/main.ts#drawOverlayScreen`, `src/main.ts#pointerDown`
- `src/ui/toolChoices.ts#blockGridChoice` (variable, line 208) ← write from `src/main.ts#pointerDown`
- `src/ui/toolChoices.ts#cloudBrush` (function, line 150) ← call from `src/main.ts#cloudTool`, `src/main.ts#MAP_ACTIONS`, `src/main.ts#SLIDERS`
- `src/ui/toolChoices.ts#CloudBrush` (interface, line 135) ← read from `src/main.ts#MAP_ACTIONS`, `src/main.ts#SLIDERS`
- `src/ui/toolChoices.ts#cloudMode` (function, line 128) ← call from `src/main.ts#cloudTool`
- `src/ui/toolChoices.ts#elementBrush` (function, line 113) ← call from `src/main.ts#dabSettings`, `src/main.ts#SLIDERS`
- `src/ui/toolChoices.ts#elementKind` (function, line 110) ← call from `src/main.ts#dabSettings`
- `src/ui/toolChoices.ts#elementMode` (function, line 111) ← call from `src/main.ts#dabSettings`
- `src/ui/toolChoices.ts#fogBrush` (function, line 174) ← call from `src/main.ts#dabSettings`, `src/main.ts#SLIDERS`
- `src/ui/toolChoices.ts#FogBrush` (interface, line 159) ← read from `src/main.ts#dabSettings`, `src/main.ts#SLIDERS`
- `src/ui/toolChoices.ts#fogErase` (function, line 67) ← call from `src/main.ts#dabSettings`
- `src/ui/toolChoices.ts#gullyErase` (function, line 95) ← call from `src/main.ts#dabSettings`
- `src/ui/toolChoices.ts#onRoadGridChange` (function, line 233) ← call from `src/main.ts#<ExpressionStatement>@1985`
- `src/ui/toolChoices.ts#paintKind` (function, line 186) ← call from `src/main.ts#dabSettings`
- `src/ui/toolChoices.ts#poleLampMode` (function, line 42) ← call from `src/main.ts#poleTool`
- `src/ui/toolChoices.ts#poleToolMode` (function, line 56) ← call from `src/main.ts#drawPolePlan`, `src/main.ts#poleTool`
- `src/ui/toolChoices.ts#roadGridShown` (function, line 226) ← call from `src/main.ts#drawOverlayScreen`, `src/main.ts#roadTool`
- `src/ui/toolChoices.ts#roadWidth` (function, line 199) ← call from `src/main.ts#drawOverlayScreen`, `src/main.ts#roadTool`
- `src/ui/toolChoices.ts#setCloudBrush` (function, line 153) ← call from `src/main.ts#SLIDERS`
- `src/ui/toolChoices.ts#setElementBrush` (function, line 116) ← call from `src/main.ts#SLIDERS`
- `src/ui/toolChoices.ts#setFogBrush` (function, line 177) ← call from `src/main.ts#SLIDERS`
- `src/ui/toolChoices.ts#setTreeBrush` (function, line 88) ← call from `src/main.ts#SLIDERS`
- `src/ui/toolChoices.ts#signChoice` (variable, line 211) ← read from `src/main.ts#streetscapeTool`
- `src/ui/toolChoices.ts#streetscapeKind` (function, line 30) ← call from `src/main.ts#drawStreetscapeHover`, `src/main.ts#streetscapeTool`
- `src/ui/toolChoices.ts#strikeChoice` (variable, line 214) ← read from `src/main.ts#actions`, `src/main.ts#frame`, `src/main.ts#pointerDown`
- `src/ui/toolChoices.ts#treeBrush` (function, line 87) ← call from `src/main.ts#dabSettings`, `src/main.ts#SLIDERS`
- `src/ui/toolChoices.ts#treeKind` (function, line 85) ← call from `src/main.ts#dabSettings`
- `src/ui/toolChoices.ts#treeMode` (function, line 83) ← call from `src/main.ts#dabSettings`
- `src/ui/toolChoices.ts#zoneColoursShown` (function, line 219) ← call from `src/main.ts#drawOverlayScreen`

### src/ui/v2/compass.ts

- `src/ui/v2/compass.ts#Compass` (interface, line 42) ← call from `src/main.ts#updateCompass`
- `src/ui/v2/compass.ts#Compass` (interface, line 42) ← typeRef from `src/main.ts#compass`
- `src/ui/v2/compass.ts#mountCompass` (function, line 53) ← call from `src/main.ts#mountInterface`
- `src/ui/v2/compass.ts#mountCompass.point` (method, line 110) ← implementation from `src/main.ts#updateCompass`
- `src/ui/v2/compass.ts#northOnScreen` (function, line 10) ← call from `src/main.ts#updateCompass`
- `src/ui/v2/compass.ts#northTurn` (function, line 31) ← call from `src/main.ts#mountInterface`

### src/ui/v2/shell.ts

- `src/ui/v2/shell.ts#mountShell` (function, line 446) ← call from `src/main.ts#mountInterface`
- `src/ui/v2/shell.ts#mountShell.openTool` (method, line 2071) ← implementation from `src/main.ts#showInspector`
- `src/ui/v2/shell.ts#mountShell.toolKeyed` (method, line 2076) ← implementation from `src/main.ts#gameKeys`
- `src/ui/v2/shell.ts#ShellHandle` (interface, line 437) ← call from `src/main.ts#gameKeys`, `src/main.ts#showInspector`
- `src/ui/v2/shell.ts#ShellHandle` (interface, line 437) ← typeRef from `src/main.ts#shell`
- `src/ui/v2/shell.ts#ShellRoadClass` (interface, line 354) ← typeRef from `src/main.ts#roadClasses`
- `src/ui/v2/shell.ts#ShellSlider` (interface, line 313) ← typeRef from `src/main.ts#brushSlider`, `src/main.ts#mapSlider`, `src/main.ts#SLIDERS`, `src/main.ts#weatherSlider`
- `src/ui/v2/shell.ts#ShellStatus` (interface, line 323) ← typeRef from `src/main.ts#currentStatus`, `src/main.ts#statusListeners`
- `src/ui/v2/shell.ts#ShellZonePatch` (interface, line 345) ← read from `src/main.ts#setZone`
- `src/ui/v2/shell.ts#ShellZonePatch` (interface, line 345) ← typeRef from `src/main.ts#setZone`

### src/ui/v2/start.ts

- `src/ui/v2/start.ts#mountStart` (function, line 15) ← call from `src/main.ts#mountInterface`

### src/view/camera.ts

- `src/view/camera.ts#Camera` (class, line 11) ← construct from `src/main.ts#camera`
- `src/view/camera.ts#Camera.fit` (method, line 69) ← call from `src/main.ts#<IfStatement>@320`, `src/main.ts#fitOpeningView`, `src/main.ts#fitView`
- `src/view/camera.ts#Camera.px` (method, line 78) ← call from `src/main.ts#roadTool`
- `src/view/camera.ts#Camera.x` (field, line 13) ← read from `src/main.ts#scene`, `src/main.ts#syncViewFromFlatCamera`
- `src/view/camera.ts#Camera.x` (field, line 13) ← write from `src/main.ts#<IfStatement>@320`, `src/main.ts#<IfStatement>@4036`, `src/main.ts#restoreSettings`, `src/main.ts#syncFlatCameraFromView`
- `src/view/camera.ts#Camera.y` (field, line 14) ← read from `src/main.ts#scene`, `src/main.ts#syncViewFromFlatCamera`
- `src/view/camera.ts#Camera.y` (field, line 14) ← write from `src/main.ts#<IfStatement>@320`, `src/main.ts#<IfStatement>@4036`, `src/main.ts#restoreSettings`, `src/main.ts#syncFlatCameraFromView`
- `src/view/camera.ts#Camera.zoom` (field, line 16) ← read from `src/main.ts#<IfStatement>@4036`, `src/main.ts#scene`, `src/main.ts#syncViewFromFlatCamera`
- `src/view/camera.ts#Camera.zoom` (field, line 16) ← write from `src/main.ts#<IfStatement>@320`, `src/main.ts#<IfStatement>@4036`, `src/main.ts#restoreSettings`, `src/main.ts#syncFlatCameraFromView`

### src/view/cameraGestures.ts

- `src/view/cameraGestures.ts#CameraGestures` (class, line 52) ← construct from `src/main.ts#cameraHand`
- `src/view/cameraGestures.ts#CameraGestures.active` (getter, line 69) ← call from `src/main.ts#frame`, `src/main.ts#pointerMove`
- `src/view/cameraGestures.ts#CameraGestures.advance` (method, line 176) ← call from `src/main.ts#frame`
- `src/view/cameraGestures.ts#CameraGestures.cancel` (method, line 321) ← call from `src/main.ts#cancelGestures`
- `src/view/cameraGestures.ts#CameraGestures.coasting` (getter, line 102) ← call from `src/main.ts#frame`
- `src/view/cameraGestures.ts#CameraGestures.gesture` (method, line 87) ← call from `src/main.ts#currentGesture`
- `src/view/cameraGestures.ts#CameraGestures.has` (method, line 82) ← call from `src/main.ts#captureLost`
- `src/view/cameraGestures.ts#CameraGestures.move` (method, line 246) ← call from `src/main.ts#pointerMove`
- `src/view/cameraGestures.ts#CameraGestures.press` (method, line 95) ← call from `src/main.ts#pointerDown`
- `src/view/cameraGestures.ts#CameraGestures.release` (method, line 303) ← call from `src/main.ts#endPointer`
- `src/view/cameraGestures.ts#CameraGestures.releaseAll` (method, line 328) ← call from `src/main.ts#releaseHeld`
- `src/view/cameraGestures.ts#CameraGestures.startOrbit` (method, line 230) ← call from `src/main.ts#pointerDown`
- `src/view/cameraGestures.ts#CameraGestures.startPan` (method, line 236) ← call from `src/main.ts#pointerDown`
- `src/view/cameraGestures.ts#CameraGestures.startPinch` (method, line 216) ← call from `src/main.ts#pointerDown`
- `src/view/cameraGestures.ts#CameraGestures.touches` (getter, line 78) ← call from `src/main.ts#pointerDown`
- `src/view/cameraGestures.ts#PointerSample` (interface, line 39) ← typeRef from `src/main.ts#CanvasInput`, `src/main.ts#coalescedPath`, `src/main.ts#pointerMove`

### src/view/cameraMotion.ts

- `src/view/cameraMotion.ts#CameraMotion` (class, line 50) ← construct from `src/main.ts#cameraMotion`
- `src/view/cameraMotion.ts#CameraMotion` (class, line 50) ← reference from `src/ui/roads/keys.ts#cameraKey`
- `src/view/cameraMotion.ts#CameraMotion.aim` (method, line 119) ← call from `src/main.ts#mountInterface`
- `src/view/cameraMotion.ts#CameraMotion.moving` (getter, line 136) ← call from `src/main.ts#frame`
- `src/view/cameraMotion.ts#CameraMotion.owns` (method, line 96) ← call from `src/ui/roads/keys.ts#cameraKey`
- `src/view/cameraMotion.ts#CameraMotion.press` (method, line 74) ← call from `src/main.ts#gameKeys`
- `src/view/cameraMotion.ts#CameraMotion.release` (method, line 84) ← call from `src/main.ts#<ExpressionStatement>@2564`
- `src/view/cameraMotion.ts#CameraMotion.releaseAll` (method, line 90) ← call from `src/main.ts#<ExpressionStatement>@2566`
- `src/view/cameraMotion.ts#CameraMotion.step` (method, line 142) ← call from `src/main.ts#frame`
- `src/view/cameraMotion.ts#CameraMotion.turn` (method, line 113) ← call from `src/main.ts#gameKeys`
- `src/view/cameraMotion.ts#CameraMotion.wheel` (method, line 101) ← call from `src/main.ts#wheel`

### src/view/viewport.ts

- `src/view/viewport.ts#flatViewport` (function, line 52) ← call from `src/main.ts#view`
- `src/view/viewport.ts#flatViewport.moveTo` (method, line 90) ← implementation from `src/main.ts#<IfStatement>@4036`, `src/main.ts#buildings`, `src/main.ts#mountInterface`, `src/main.ts#syncViewFromFlatCamera`
- `src/view/viewport.ts#flatViewport.orbit` (method, line 79) ← implementation from `src/main.ts#cameraCommand`, `src/main.ts#mountInterface`
- `src/view/viewport.ts#flatViewport.panTo` (method, line 60) ← implementation from `src/main.ts#buildings`
- `src/view/viewport.ts#flatViewport.rotate` (method, line 76) ← implementation from `src/buildingsWiring.ts#createBuildingWiring`
- `src/view/viewport.ts#flatViewport.setOrbit` (method, line 80) ← implementation from `src/buildingsWiring.ts#createBuildingWiring`, `src/main.ts#<IfStatement>@4036`, `src/main.ts#cameraCommand`, `src/main.ts#fitOpeningView`, …
- `src/view/viewport.ts#flatViewport.toScreen` (method, line 58) ← implementation from `src/buildingsWiring.ts#createBuildingWiring`, `src/main.ts#buildings`, `src/main.ts#drawOverlayScreen`, `src/main.ts#drawOverlayScreen.at`, …
- `src/view/viewport.ts#flatViewport.toWorld` (method, line 55) ← implementation from `src/actionsWiring.ts#createActions.shootAt`, `src/buildingsWiring.ts#createBuildingWiring.beforeDraw`, `src/lab/inspector/entity.ts#entitiesAt`, `src/main.ts#worldAtScreen`
- `src/view/viewport.ts#flatViewport.toWorldAt` (method, line 57) ← implementation from `src/actionsWiring.ts#createActions.buildingsUnderRay`, `src/actionsWiring.ts#createActions.rayOnBuilding`, `src/actionsWiring.ts#createActions.shootAt`, `src/buildingsWiring.ts#createBuildingWiring`, …
- `src/view/viewport.ts#flatViewport.zoomAt` (method, line 65) ← implementation from `src/main.ts#buildings`, `src/main.ts#syncViewFromFlatCamera`
- `src/view/viewport.ts#Viewport` (interface, line 8) ← call from `src/actionsWiring.ts#createActions.buildingsUnderRay`, `src/actionsWiring.ts#createActions.rayOnBuilding`, `src/actionsWiring.ts#createActions.shootAt`, `src/buildingsWiring.ts#createBuildingWiring`, …
- `src/view/viewport.ts#Viewport` (interface, line 8) ← read from `src/buildingsWiring.ts#createBuildingWiring`, `src/lab/inspector/raycast.ts#rayAtPixel`, `src/lab/recorder/recording.ts#startRecording.onFrame`, `src/main.ts#<IfStatement>@4036`, …
- `src/view/viewport.ts#Viewport` (interface, line 8) ← typeRef from `src/actionsWiring.ts#ActionsDeps`, `src/buildingsWiring.ts#BuildingWiringDeps`, `src/main.ts#view`, `src/render/renderer.ts#SceneHandle`

## Shared declarations the feature uses

### src/buildingsWiring.ts

- `src/buildingsWiring.ts#createBuildingWiring` (function, line 81) — implementation by 5: `src/ui/v2/shell.ts#mountShell.renderBuild`, `src/ui/v2/shell.ts#mountShell.renderModels`, `src/ui/v2/shell.ts#mountShell.renderSection`, `src/ui/v2/shell.ts#mountShell.seeInside`, …
- `src/buildingsWiring.ts#createBuildingWiring.requestThumbnails` (function, line 483) — implementation by 1: `src/ui/v2/shell.ts#mountShell.ask`

### src/core/aabb.ts

- `src/core/aabb.ts#Aabb` (interface, line 3) — read by 1: `src/view/camera.ts#Camera.fit`
- `src/core/aabb.ts#Aabb` (interface, line 3) — typeRef by 2: `src/view/camera.ts#Camera.fit`, `src/view/camera.ts#Camera.viewBounds`

### src/core/bezier.ts

- `src/core/bezier.ts#CurveShape` (interface, line 5) — read by 1: `src/ui/inspector.ts#renderSegment`
- `src/core/bezier.ts#CurveShape` (interface, line 5) — typeRef by 1: `src/ui/inspector.ts#InspectorActions`

### src/core/commands.ts

- `src/core/commands.ts#Command` (interface, line 5) — read by 1: `src/core/commands.ts#CommandRegistry.register`
- `src/core/commands.ts#Command` (interface, line 5) — typeRef by 1: `src/core/commands.ts#CommandRegistry.register`
- `src/core/commands.ts#CommandRegistry` (class, line 39) — construct by 1: `src/editor/gameKeys.ts#createGameKeys`
- `src/core/commands.ts#CommandRegistry` (class, line 39) — typeRef by 3: `src/editor/gameKeys.ts#createGameKeys`, `src/core/commands.ts#CommandRegistry.register`, `src/core/commands.ts#CommandRegistry.bind`
- `src/core/commands.ts#CommandRegistry.commands` (field, line 40) — read by 2: `src/core/commands.ts#CommandRegistry.register`, `src/core/commands.ts#CommandRegistry.bind`
- `src/core/commands.ts#CommandRegistry.commands` (field, line 40) — write by 1: `src/core/commands.ts#CommandRegistry.register`
- `src/core/commands.ts#CommandRegistry.keys` (field, line 41) — read by 1: `src/core/commands.ts#CommandRegistry.bind`
- `src/core/commands.ts#CommandRegistry.keys` (field, line 41) — write by 1: `src/core/commands.ts#CommandRegistry.bind`
- `src/core/commands.ts#CommandResult` (type, line 3) — typeRef by 4: `src/editor/gameKeys.ts#createGameKeys.command`, `src/editor/gameKeys.ts#done`, `src/editor/gameKeys.ts#held`, `src/editor/gameKeys.ts#used`
- `src/core/commands.ts#KeyRule` (interface, line 13) — read by 1: `src/core/commands.ts#CommandRegistry.bind`
- `src/core/commands.ts#KeyRule` (interface, line 13) — typeRef by 2: `src/editor/gameKeys.ts#createGameKeys`, `src/core/commands.ts#CommandRegistry.bind`

### src/core/health.ts

- `src/core/health.ts#FrameRecord` (interface, line 132) — read by 2: `src/core/health.ts#FrameTimer.between`, `src/core/health.ts#systemsOf`
- `src/core/health.ts#FrameRecord` (interface, line 132) — typeRef by 2: `src/core/health.ts#FrameTimer.between`, `src/core/health.ts#systemsOf`
- `src/core/health.ts#FrameTimer` (class, line 144) — typeRef by 1: `src/ui/healthWatch.ts#watchHealth`
- `src/core/health.ts#FrameTimer.frames` (field, line 148) — read by 1: `src/core/health.ts#FrameTimer.between`
- `src/core/health.ts#HealthEntry` (interface, line 10) — read by 4: `src/ui/healthPanel.ts#clock`, `src/ui/healthPanel.ts#mountHealthPanel.renderHealth`, `src/core/health.ts#HealthLog.restore`, `src/core/health.ts#HealthLog.status`
- `src/core/health.ts#HealthEntry` (interface, line 10) — typeRef by 4: `src/ui/healthPanel.ts#clock`, `src/ui/healthWatch.ts#watchHealth`, `src/core/health.ts#HealthLog.toJSON`, `src/core/health.ts#HealthLog.restore`
- `src/core/health.ts#HealthLog` (class, line 39) — typeRef by 3: `src/ui/healthPanel.ts#HealthPanelSources`, `src/ui/healthWatch.ts#watchHealth`, `src/ui/healthWatch.ts#watchHealth.write`
- `src/core/health.ts#HealthLog.count` (field, line 42) — read by 1: `src/core/health.ts#HealthLog.restore`
- `src/core/health.ts#HealthLog.count` (field, line 42) — write by 1: `src/core/health.ts#HealthLog.restore`
- `src/core/health.ts#HealthLog.latest` (method, line 84) — call by 4: `src/ui/healthPanel.ts#mountHealthPanel`, `src/ui/healthPanel.ts#mountHealthPanel.renderHealth`, `src/core/health.ts#HealthLog.toJSON`, `src/core/health.ts#HealthLog.status`
- `src/core/health.ts#HealthLog.record` (method, line 56) — call by 3: `src/ui/healthWatch.ts#watchHealth`, `src/ui/healthWatch.ts#watchHealth.slowFrame`, `src/ui/healthWatch.ts#watchHealth.write`
- `src/core/health.ts#HealthLog.ring` (field, line 41) — read by 1: `src/core/health.ts#HealthLog.restore`
- `src/core/health.ts#HealthLog.ring` (field, line 41) — write by 1: `src/core/health.ts#HealthLog.restore`
- `src/core/health.ts#HealthLog.serial` (field, line 40) — read by 1: `src/core/health.ts#HealthLog.restore`
- `src/core/health.ts#HealthLog.serial` (field, line 40) — write by 1: `src/core/health.ts#HealthLog.restore`
- `src/core/health.ts#HealthLog.touched` (field, line 50) — read by 2: `src/core/health.ts#HealthLog.changes`, `src/core/health.ts#HealthLog.restore`
- `src/core/health.ts#HealthLog.touched` (field, line 50) — write by 1: `src/core/health.ts#HealthLog.restore`
- `src/core/health.ts#KEPT` (variable, line 35) — read by 2: `src/core/health.ts#HealthLog.restore`, `src/core/health.ts#HealthLog.status`
- `src/core/health.ts#LONG_FRAME_MS` (variable, line 123) — read by 2: `src/ui/healthWatch.ts#watchHealth`, `src/ui/healthWatch.ts#watchHealth.frameEnded`

### src/core/hitch.ts

- `src/core/hitch.ts#measureHitch` (function, line 2) — call by 1: `src/editor/history.ts#History.close`

### src/core/keptList.ts

- `src/core/keptList.ts#KeptList` (class, line 2) — construct by 1: `src/ui/roads/templates.ts#kept`
- `src/core/keptList.ts#KeptList.readShared` (method, line 30) — call by 1: `src/ui/roads/templates.ts#savedTemplates`
- `src/core/keptList.ts#KeptList.update` (method, line 45) — call by 2: `src/ui/roads/templates.ts#saveTemplate`, `src/ui/roads/templates.ts#deleteTemplate`

### src/core/perfBudget.ts

- `src/core/perfBudget.ts#Budget` (interface, line 3) — read by 1: `src/ui/healthWatch.ts#watchHealth`
- `src/core/perfBudget.ts#Budget` (interface, line 3) — typeRef by 1: `src/core/perfBudget.ts#stepBudget`
- `src/core/perfBudget.ts#STEP_BUDGETS` (variable, line 15) — read by 1: `src/core/perfBudget.ts#stepBudget`

### src/core/plain.ts

- `src/core/plain.ts#samePlain` (function, line 2) — call by 3: `src/world/doc.ts#RoadDoc.savedGlobals`, `src/world/doc.ts#RoadDoc.endEdit`, `src/world/doc.ts#memberDiffs`

### src/core/polyline.ts

- `src/core/polyline.ts#Polyline.closestPoint` (method, line 330) — call by 1: `src/editor/repair.ts#nearestCandidate`
- `src/core/polyline.ts#Polyline.closestPoint` (method, line 330) — read by 1: `src/editor/repair.ts#nearestCandidate`
- `src/core/polyline.ts#Polyline.length` (getter, line 123) — call by 4: `src/ui/inspector.ts#renderSegment`, `src/ui/inspector.ts#segmentStats`, `src/editor/repair.ts#nearestCandidate`, `src/world/roads/streetNames.ts#streetNumbers`

### src/core/profiler.ts

- `src/core/profiler.ts#ProductTiming` (interface, line 58) — read by 1: `src/ui/profilerHud.ts#render`
- `src/core/profiler.ts#Profiler` (class, line 74) — typeRef by 1: `src/ui/profilerHud.ts#mountProfilerHud`
- `src/core/profiler.ts#Profiler.closed` (field, line 231) — read by 1: `src/ui/profilerHud.ts#mountProfilerHud`
- `src/core/profiler.ts#Profiler.enabled` (getter, line 78) — call by 2: `src/ui/profilerHud.ts#mountProfilerHud`, `src/ui/profilerHud.ts#mountProfilerHud.toggle`
- `src/core/profiler.ts#Profiler.enabled@81` (setter, line 81) — call by 2: `src/ui/profilerHud.ts#mountProfilerHud`, `src/ui/profilerHud.ts#mountProfilerHud.toggle`
- `src/core/profiler.ts#Profiler.view` (method, line 234) — call by 1: `src/ui/profilerHud.ts#mountProfilerHud`
- `src/core/profiler.ts#ProfilerView` (interface, line 258) — read by 4: `src/ui/profilerHud.ts#render`, `src/ui/profilerHud.ts#render.e`, `src/ui/profilerHud.ts#render.n`, `src/ui/profilerHud.ts#render.peak`
- `src/core/profiler.ts#ProfilerView` (interface, line 258) — typeRef by 1: `src/ui/profilerHud.ts#render`

### src/core/scalar.ts

- `src/core/scalar.ts#clamp` (function, line 58) — call by 4: `src/view/camera.ts#Camera.fit`, `src/view/camera.ts#Camera.zoomAt`, `src/view/cameraGestures.ts#CameraGestures.move`, `src/view/cameraProfile.ts#blend`

### src/core/units.ts

- `src/core/units.ts#m` (function, line 6) — call by 27: `src/view/cameraProfile.ts#EYE_HEIGHT`, `src/view/cameraProfile.ts#eyeLift`, `src/view/cameraProfile.ts#fieldOfView`, `src/view/cameraProfile.ts#minTilt`, …
- `src/core/units.ts#METERS_PER_UNIT` (variable, line 2) — read by 16: `src/ui/inspector.ts#curveText`, `src/ui/inspector.ts#meters`, `src/ui/inspector.ts#nodeStats`, `src/ui/inspector.ts#segmentStats`, …
- `src/core/units.ts#perM` (function, line 9) — call by 6: `src/view/camera.ts#Camera.fit`, `src/view/camera.ts#Camera.zoom`, `src/view/camera.ts#MAX_ZOOM`, `src/view/camera.ts#MIN_ZOOM`, …
- `src/core/units.ts#UNITS_PER_METER` (variable, line 3) — read by 7: `src/ui/v2/shell.ts#mountShell.renderRoads`, `src/ui/inspector.ts#renderSegment`, `src/ui/roadSectionEditor.ts#mountRoadSectionEditor`, `src/ui/roads/profileEditor.ts#newElement`, …

### src/core/vec2.ts

- `src/core/vec2.ts#Vec2` (interface, line 4) — read by 17: `src/ui/v2/compass.ts#groundAngleToNorth`, `src/ui/v2/compass.ts#northOnScreen`, `src/ui/v2/compass.ts#toNorth`, `src/view/camera.ts#Camera.worldToScreen`, …
- `src/core/vec2.ts#Vec2` (interface, line 4) — typeRef by 22: `src/ui/v2/compass.ts#toNorth`, `src/view/camera.ts#Camera.screenToWorld`, `src/view/camera.ts#Camera.worldToScreen`, `src/view/cameraGestures.ts#CameraGestures.anchor`, …

### src/editor/commit.ts

- `src/editor/commit.ts#splitSegment` (function, line 662) — call by 1: `src/editor/repair.ts#repairNearConnections`

### src/editor/snap.ts

- `src/editor/snap.ts#ribbonsNear` (function, line 74) — call by 1: `src/editor/repair.ts#nearestCandidate`

### src/editor/toolMachine.ts

- `src/editor/toolMachine.ts#Tool` (type, line 4) — typeRef by 3: `src/editor/gameKeys.ts#createGameKeys.tool`, `src/editor/gameKeys.ts#GameKeyDeps`, `src/editor/gameKeys.ts#TOOL_LETTERS`

### src/editor/workerParts.ts

- `src/editor/workerParts.ts#Answer` (interface, line 16) — read by 2: `src/editor/workerParts.ts#portParts.get`, `src/editor/workerParts.ts#portParts.write`
- `src/editor/workerParts.ts#Answer` (interface, line 16) — typeRef by 1: `src/editor/workerParts.ts#portParts.call`
- `src/editor/workerParts.ts#backend` (variable, line 19) — read by 1: `src/editor/workerParts.ts#workerParts.open`
- `src/editor/workerParts.ts#backend` (variable, line 19) — write by 1: `src/editor/workerParts.ts#workerParts.open`
- `src/editor/workerParts.ts#portParts` (function, line 45) — read by 1: `src/editor/workerParts.ts#portParts.call`
- `src/editor/workerParts.ts#portParts` (function, line 45) — write by 1: `src/editor/workerParts.ts#portParts.call`

### src/effectsWiring.ts

- `src/effectsWiring.ts#createEffectsWiring` (function, line 108) — implementation by 1: `src/ui/effectsPanel.ts#createEffectsPanel.render`
- `src/effectsWiring.ts#createEffectsWiring.stats` (function, line 193) — implementation by 1: `src/ui/effectsPanel.ts#createEffectsPanel.renderStats`

### src/graphicsWiring.ts

- `src/graphicsWiring.ts#createGraphicsWiring` (function, line 118) — implementation by 1: `src/ui/graphicsPanel.ts#createGraphicsPanel.render`

### src/jobs/jobPool.ts

- `src/jobs/jobPool.ts#jobPort` (function, line 669) — call by 1: `src/editor/persistence.ts#defaultParts`

### src/lab/catalog/layers.ts

- `src/lab/catalog/layers.ts#signalItems` (function, line 118) — implementation by 1: `src/ui/inspector.ts#nodeStats`

### src/main.ts

- `src/main.ts#<ExpressionStatement>@3908` (statement, line 3908) — callbackAssign by 1: `src/editor/persistence.ts#Persistence.saveSession`
- `src/main.ts#<ExpressionStatement>@874` (statement, line 874) — callbackAssign by 1: `src/editor/persistence.ts#Persistence.saveSession`
- `src/main.ts#brushSlider` (function, line 1670) — implementation by 2: `src/ui/v2/shell.ts#mountShell.range`, `src/ui/v2/shell.ts#mountShell.range.show`
- `src/main.ts#cameraHand` (variable, line 553) — implementation by 6: `src/view/cameraGestures.ts#CameraGestures.advance`, `src/view/cameraGestures.ts#CameraGestures.anchor`, `src/view/cameraGestures.ts#CameraGestures.move`, `src/view/cameraGestures.ts#CameraGestures.startPan`, …
- `src/main.ts#cameraMotion` (variable, line 563) — implementation by 2: `src/view/cameraMotion.ts#CameraMotion.step`, `src/view/cameraMotion.ts#CameraMotion.wheel`
- `src/main.ts#doc` (variable, line 189) — read by 2: `src/main.ts#mapSlider.end`, `src/main.ts#mapSlider.set`
- `src/main.ts#gameKeys` (variable, line 2479) — callbackAssign by 1: `src/editor/gameKeys.ts#createGameKeys`
- `src/main.ts#gameKeys` (variable, line 2479) — implementation by 2: `src/editor/gameKeys.ts#createGameKeys`, `src/editor/gameKeys.ts#createGameKeys.tool`
- `src/main.ts#healthPanel` (variable, line 3987) — callbackAssign by 2: `src/ui/healthPanel.ts#mountHealthPanel`, `src/ui/healthPanel.ts#mountHealthPanel.render`
- `src/main.ts#healthPanel` (variable, line 3987) — implementation by 2: `src/ui/healthPanel.ts#mountHealthPanel`, `src/ui/healthPanel.ts#mountHealthPanel.render`
- `src/main.ts#history` (variable, line 201) — read by 2: `src/main.ts#mapSlider.end`, `src/main.ts#mapSlider.set`
- `src/main.ts#laneChoices` (function, line 1577) — implementation by 1: `src/ui/v2/shell.ts#mountShell.renderRoads`
- `src/main.ts#mapBiome` (function, line 1774) — implementation by 1: `src/ui/v2/shell.ts#mountShell.renderLandscape`
- `src/main.ts#mapSlider` (function, line 1655) — call by 1: `src/main.ts#mapSlider.set`
- `src/main.ts#mapSlider` (function, line 1655) — implementation by 1: `src/ui/v2/shell.ts#mountShell.range.show`
- `src/main.ts#mapSlider` (function, line 1655) — read by 2: `src/main.ts#mapSlider.end`, `src/main.ts#mapSlider.set`
- `src/main.ts#mapSlider` (function, line 1655) — write by 2: `src/main.ts#mapSlider.end`, `src/main.ts#mapSlider.set`
- `src/main.ts#mountInterface` (function, line 1859) — implementation by 13: `src/ui/v2/shell.ts#mountShell`, `src/ui/v2/shell.ts#mountShell.fillPop`, `src/ui/v2/shell.ts#mountShell.mapAction`, `src/ui/v2/shell.ts#mountShell.openTool`, …
- `src/main.ts#newLargeWorld` (function, line 2035) — implementation by 1: `src/ui/v2/shell.ts#mountShell.popBody`
- `src/main.ts#newMap` (function, line 2015) — implementation by 2: `src/ui/v2/shell.ts#mountShell.popBody`, `src/ui/v2/start.ts#mountStart.fill`
- `src/main.ts#newWorld` (function, line 2029) — implementation by 1: `src/ui/v2/shell.ts#mountShell.popBody`
- `src/main.ts#persistence` (variable, line 205) — read by 1: `src/main.ts#mapSlider.set`
- `src/main.ts#redo` (function, line 2344) — implementation by 2: `src/ui/v2/shell.ts#mountShell`, `src/editor/gameKeys.ts#createGameKeys`
- `src/main.ts#requestDraw` (function, line 2589) — call by 1: `src/main.ts#mapSlider.set`
- `src/main.ts#requestDraw` (function, line 2589) — implementation by 1: `src/editor/gameKeys.ts#createGameKeys`
- `src/main.ts#roadClasses` (function, line 1558) — implementation by 1: `src/ui/v2/shell.ts#mountShell.renderRoads`
- `src/main.ts#saveMap` (function, line 2257) — implementation by 2: `src/ui/v2/shell.ts#mountShell.popBody`, `src/editor/gameKeys.ts#createGameKeys`
- `src/main.ts#selectRoadType` (function, line 1596) — implementation by 1: `src/ui/v2/shell.ts#mountShell.renderRoads`
- `src/main.ts#sessionSettings` (function, line 443) — indirectCall by 1: `src/editor/persistence.ts#Persistence.saveSessionSoon`
- `src/main.ts#sessionSettings` (function, line 443) — reference by 1: `src/main.ts#mapSlider.set`
- `src/main.ts#setDemand` (function, line 1974) — implementation by 1: `src/ui/v2/shell.ts#mountShell.popBody`
- `src/main.ts#setLanes` (function, line 1586) — implementation by 1: `src/ui/v2/shell.ts#mountShell.renderRoads`
- `src/main.ts#setMapBiome` (function, line 1777) — implementation by 1: `src/ui/v2/shell.ts#mountShell.renderLandscape.run`
- `src/main.ts#setMarkingStyle` (function, line 3933) — implementation by 1: `src/ui/v2/shell.ts#mountShell.popBody`
- `src/main.ts#setQuality` (function, line 3944) — implementation by 1: `src/ui/v2/shell.ts#mountShell.popBody`
- `src/main.ts#setSky` (function, line 2244) — implementation by 1: `src/ui/v2/shell.ts#mountShell`
- `src/main.ts#setSpeed` (function, line 1964) — implementation by 1: `src/ui/v2/shell.ts#mountShell`
- `src/main.ts#setZone` (function, line 1617) — implementation by 1: `src/ui/v2/shell.ts#mountShell.renderZones`
- `src/main.ts#showInspector` (function, line 3573) — callbackAssign by 10: `src/ui/inspector.ts#junctionHost`, `src/ui/inspector.ts#junctionHost.setControl`, `src/ui/inspector.ts#junctionHost.setHeight`, `src/ui/inspector.ts#junctionHost.setMovementBlocked`, …
- `src/main.ts#showInspector` (function, line 3573) — implementation by 10: `src/ui/inspector.ts#junctionHost`, `src/ui/inspector.ts#junctionHost.setControl`, `src/ui/inspector.ts#junctionHost.setHeight`, `src/ui/inspector.ts#junctionHost.setMovementBlocked`, …
- `src/main.ts#SLIDERS` (variable, line 1715) — implementation by 2: `src/ui/v2/shell.ts#mountShell.range`, `src/ui/v2/shell.ts#mountShell.range.show`
- `src/main.ts#toggleCongestion` (function, line 1980) — implementation by 1: `src/ui/v2/shell.ts#mountShell`
- `src/main.ts#undo` (function, line 2335) — implementation by 2: `src/ui/v2/shell.ts#mountShell`, `src/editor/gameKeys.ts#createGameKeys`
- `src/main.ts#updateHistoryButtons` (function, line 2366) — call by 2: `src/main.ts#mapSlider.end`, `src/main.ts#mapSlider.set`

### src/render/depthConvention.ts

- `src/render/depthConvention.ts#DEPTH_GLSL` (variable, line 6) — read by 1: `src/render/canyonLight.ts#FRAGMENT`

### src/render/environment.ts

- `src/render/environment.ts#createEnvironment` (function, line 118) — read by 2: `src/render/environment.ts#createEnvironment.remakeShadowMaps`, `src/render/environment.ts#createEnvironment.setFarCascadeEvery`
- `src/render/environment.ts#SceneEnvironment` (interface, line 36) — call by 2: `src/render/optimizationsApply.ts#createOptimizer`, `src/render/optimizationsApply.ts#createOptimizer.set`
- `src/render/environment.ts#SceneEnvironment` (interface, line 36) — typeRef by 1: `src/render/optimizationsApply.ts#OptimizerDeps`

### src/render/optimizationTypes.ts

- `src/render/optimizationTypes.ts#OptChoice` (interface, line 27) — read by 4: `src/render/optimizations.ts#DEFAULT_OPTIMIZATIONS`, `src/render/optimizations.ts#isDefaultOptimizations`, `src/render/optimizations.ts#optimizationChoice`, `src/render/optimizationsQuality.ts#qualityFieldValue`
- `src/render/optimizationTypes.ts#OptChoice` (interface, line 27) — typeRef by 5: `src/render/optimizations.ts#BaseSpec`, `src/render/optimizations.ts#OptimizationSpec`, `src/render/optimizations.ts#TIER`, `src/render/optimizationsQuality.ts#QUALITY_FIELD_OPTIONS`, …
- `src/render/optimizationTypes.ts#OptSystem` (type, line 5) — typeRef by 4: `src/render/optimizations.ts#BaseSpec`, `src/render/optimizations.ts#OptimizationSpec`, `src/render/optimizationsQuality.ts#QualityOptionSpec`, `src/render/optimizationsQuality.ts#FieldSpec`
- `src/render/optimizationTypes.ts#OptValue` (type, line 3) — typeRef by 6: `src/render/optimizations.ts#DEFAULT_OPTIMIZATIONS`, `src/render/optimizations.ts#optimizationChoice`, `src/render/optimizations.ts#Optimizations`, `src/render/optimizations.ts#withOptimizations.num`, …

### src/render/renderer.ts

- `src/render/renderer.ts#createSceneRenderer` (function, line 432) — implementation by 1: `src/render/optimizationsApply.ts#createOptimizer.beforeRender`
- `src/render/renderer.ts#createSceneRenderer.occluderDrawn` (function, line 1464) — indirectCall by 1: `src/render/occlusion.ts#OcclusionBuffer.build`

### src/render/sunCascades.ts

- `src/render/sunCascades.ts#createSunCascades` (function, line 206) — read by 2: `src/render/sunCascades.ts#createSunCascades.setFarEvery`, `src/render/sunCascades.ts#createSunCascades.remakeMaps`
- `src/render/sunCascades.ts#createSunCascades` (function, line 206) — write by 2: `src/render/sunCascades.ts#createSunCascades.setFarEvery`, `src/render/sunCascades.ts#createSunCascades.remakeMaps`
- `src/render/sunCascades.ts#SunCascades` (interface, line 123) — call by 2: `src/render/environment.ts#createEnvironment.remakeShadowMaps`, `src/render/environment.ts#createEnvironment.setFarCascadeEvery`

### src/sim/host/protocol.ts

- `src/sim/host/protocol.ts#SignalLayout` (interface, line 70) — read by 1: `src/ui/inspector.ts#nodeStats`
- `src/sim/host/protocol.ts#SimAsk` (type, line 144) — typeRef by 1: `src/sim/host/simClient.ts#SimClient.ask`

### src/sim/host/simAnswers.ts

- `src/sim/host/simAnswers.ts#JunctionAnswer` (interface, line 24) — read by 5: `src/ui/inspector.ts#nodeStats`, `src/ui/inspector.ts#renderCurrent`, `src/ui/roads/junctionPanel.ts#mountJunctionPanel`, `src/ui/roads/junctionPanel.ts#mountJunctionPanel.set`, …
- `src/sim/host/simAnswers.ts#JunctionAnswer` (interface, line 24) — typeRef by 1: `src/sim/host/simClient.ts#SimClient.junction`
- `src/sim/host/simAnswers.ts#TrafficCount` (interface, line 14) — read by 2: `src/ui/inspector.ts#nodeStats`, `src/ui/inspector.ts#segmentStats`

### src/sim/host/simClient.ts

- `src/sim/host/simClient.ts#SignalFeed` (interface, line 58) — call by 1: `src/ui/inspector.ts#nodeStats`
- `src/sim/host/simClient.ts#SimClient` (class, line 84) — typeRef by 9: `src/ui/inspector.ts#InspectorContext`, `src/ui/inspector.ts#junctionHost`, `src/ui/inspector.ts#nodeStats`, `src/ui/inspector.ts#openInspector`, …
- `src/sim/host/simClient.ts#SimClient.asked` (method, line 373) — call by 2: `src/ui/inspector.ts#renderNode`, `src/sim/host/simClient.ts#SimClient.junction`
- `src/sim/host/simClient.ts#SimClient.nextAsk` (field, line 104) — read by 1: `src/sim/host/simClient.ts#SimClient.ask`
- `src/sim/host/simClient.ts#SimClient.nextAsk` (field, line 104) — write by 1: `src/sim/host/simClient.ts#SimClient.ask`
- `src/sim/host/simClient.ts#SimClient.pending` (field, line 103) — read by 1: `src/sim/host/simClient.ts#SimClient.ask`
- `src/sim/host/simClient.ts#SimClient.pending` (field, line 103) — write by 1: `src/sim/host/simClient.ts#SimClient.ask`
- `src/sim/host/simClient.ts#SimClient.segmentTraffic` (method, line 400) — call by 1: `src/ui/inspector.ts#segmentStats`
- `src/sim/host/simClient.ts#SimClient.send` (method, line 423) — call by 1: `src/sim/host/simClient.ts#SimClient.ask`
- `src/sim/host/simClient.ts#SimClient.signals` (field, line 411) — implementation by 1: `src/ui/inspector.ts#nodeStats`
- `src/sim/host/simClient.ts#SimClient.signals` (field, line 411) — read by 1: `src/ui/inspector.ts#nodeStats`
- `src/sim/host/simClient.ts#SimClient.topologyRevision` (getter, line 201) — call by 1: `src/ui/inspector.ts#renderCurrent`

### src/ui/builder/catalog.ts

- `src/ui/builder/catalog.ts#BUILDER_TAB_SPECS` (variable, line 36) — read by 1: `src/ui/v2/shell.ts#mountShell.renderBuild`
- `src/ui/builder/catalog.ts#BuilderCategoryId` (type, line 3) — typeRef by 1: `src/ui/v2/shell.ts#mountShell.renderBuild`
- `src/ui/builder/catalog.ts#BuilderSection` (interface, line 17) — read by 2: `src/ui/v2/shell.ts#mountShell.renderBuild`, `src/ui/v2/shell.ts#mountShell.renderSection`
- `src/ui/builder/catalog.ts#BuilderSection` (interface, line 17) — typeRef by 1: `src/ui/v2/shell.ts#mountShell.renderSection`
- `src/ui/builder/catalog.ts#BuilderTabSpec` (interface, line 26) — read by 1: `src/ui/v2/shell.ts#mountShell.renderBuild`
- `src/ui/builder/catalog.ts#BuilderToolSpec` (interface, line 7) — read by 4: `src/ui/v2/shell.ts#mountShell.renderBuild`, `src/ui/v2/shell.ts#mountShell.renderSection`, `src/ui/v2/shell.ts#mountShell.toolButton`, `src/ui/v2/shell.ts#mountShell.toolButton.run`
- `src/ui/builder/catalog.ts#BuilderToolSpec` (interface, line 7) — typeRef by 1: `src/ui/v2/shell.ts#mountShell.toolButton`
- `src/ui/builder/catalog.ts#DRAW_ACTIONS` (variable, line 169) — read by 1: `src/ui/v2/shell.ts#mountShell.renderSection`
- `src/ui/builder/catalog.ts#DRAW_SHAPES` (variable, line 125) — read by 2: `src/ui/v2/shell.ts#mountShell.picture`, `src/ui/v2/shell.ts#mountShell.renderSection`
- `src/ui/builder/catalog.ts#FACADE_SCOPES` (variable, line 172) — read by 1: `src/ui/v2/shell.ts#mountShell.renderSection`
- `src/ui/builder/catalog.ts#PlanShapeId` (type, line 122) — typeRef by 2: `src/ui/planSwatch.ts#planSwatch`, `src/ui/planSwatch.ts#OUTLINES`
- `src/ui/builder/catalog.ts#TIER_SHAPES` (variable, line 138) — read by 2: `src/ui/v2/shell.ts#mountShell.picture`, `src/ui/v2/shell.ts#mountShell.renderSection`

### src/ui/builder/icons.ts

- `src/ui/builder/icons.ts#builderIconSvg` (function, line 313) — call by 6: `src/ui/v2/shell.ts#mountShell`, `src/ui/v2/shell.ts#mountShell.BARRIER_ICON`, `src/ui/v2/shell.ts#mountShell.renderBuild`, `src/ui/v2/shell.ts#mountShell.renderModels.fill`, …

### src/ui/builder/workspace.ts

- `src/ui/builder/workspace.ts#BuilderActions` (interface, line 52) — call by 6: `src/ui/v2/shell.ts#mountShell.ask`, `src/ui/v2/shell.ts#mountShell.renderBuild`, `src/ui/v2/shell.ts#mountShell.renderModels`, `src/ui/v2/shell.ts#mountShell.renderSection`, …
- `src/ui/builder/workspace.ts#BuilderState` (interface, line 17) — read by 7: `src/ui/v2/shell.ts#mountShell.render`, `src/ui/v2/shell.ts#mountShell.renderBuild`, `src/ui/v2/shell.ts#mountShell.renderModels`, `src/ui/v2/shell.ts#mountShell.renderSection`, …
- `src/ui/builder/workspace.ts#BuilderState` (interface, line 17) — typeRef by 5: `src/ui/v2/shell.ts#mountShell`, `src/ui/v2/shell.ts#mountShell.renderModels`, `src/ui/v2/shell.ts#mountShell.renderSection`, `src/ui/v2/shell.ts#mountShell.sigOf`, …
- `src/ui/builder/workspace.ts#BuilderWorkspace` (interface, line 88) — call by 5: `src/ui/v2/shell.ts#mountShell`, `src/ui/v2/shell.ts#mountShell.ask`, `src/ui/v2/shell.ts#mountShell.picture`, `src/ui/v2/shell.ts#mountShell.renderModels.fill`, …
- `src/ui/builder/workspace.ts#BuilderWorkspace` (interface, line 88) — read by 2: `src/ui/v2/shell.ts#mountShell`, `src/ui/v2/shell.ts#mountShell.syncInside`
- `src/ui/builder/workspace.ts#BuilderWorkspace` (interface, line 88) — typeRef by 1: `src/ui/v2/shell.ts#ShellDeps`
- `src/ui/builder/workspace.ts#initBuilderWorkspace` (function, line 117) — read by 4: `src/ui/builder/workspace.ts#initBuilderWorkspace.thumbnail`, `src/ui/builder/workspace.ts#initBuilderWorkspace.inside`, `src/ui/builder/workspace.ts#initBuilderWorkspace.onInside`, `src/ui/builder/workspace.ts#initBuilderWorkspace.subscribe`
- `src/ui/builder/workspace.ts#initBuilderWorkspace` (function, line 117) — write by 2: `src/ui/builder/workspace.ts#initBuilderWorkspace.onInside`, `src/ui/builder/workspace.ts#initBuilderWorkspace.subscribe`
- `src/ui/builder/workspace.ts#SNAP_MODES` (variable, line 108) — read by 1: `src/ui/v2/shell.ts#mountShell.renderBuild`

### src/ui/roads/cutWalls.ts

- `src/ui/roads/cutWalls.ts#chosen` (variable, line 3) — read by 1: `src/ui/roads/cutWalls.ts#setCutWalls`
- `src/ui/roads/cutWalls.ts#chosen` (variable, line 3) — write by 1: `src/ui/roads/cutWalls.ts#setCutWalls`
- `src/ui/roads/cutWalls.ts#cutWallsChosen` (function, line 9) — call by 1: `src/ui/v2/shell.ts#mountShell.renderRoads`
- `src/ui/roads/cutWalls.ts#STORAGE_KEY` (variable, line 2) — read by 1: `src/ui/roads/cutWalls.ts#setCutWalls`

### src/ui/roads/drawProfile.ts

- `src/ui/roads/drawProfile.ts#drawProfile` (function, line 12) — call by 3: `src/ui/v2/shell.ts#mountShell.renderRoads`, `src/ui/roads/catalogPanel.ts#catalogStrip`, `src/ui/roads/catalogPanel.ts#pickedRoad`
- `src/ui/roads/drawProfile.ts#DrawProfile` (interface, line 4) — read by 1: `src/ui/roads/catalogPanel.ts#catalogStrip`
- `src/ui/roads/drawProfile.ts#DrawProfile` (interface, line 4) — typeRef by 6: `src/ui/inspector.ts#InspectorActions`, `src/ui/inspector.ts#renderSegment`, `src/ui/roads/profilePanel.ts#mountProfilePanel`, `src/ui/roads/profilePanel.ts#mountProfilePanel.hold`, …
- `src/ui/roads/drawProfile.ts#setDrawProfile` (function, line 16) — call by 3: `src/ui/roads/profilePanel.ts#mountProfilePanel.hold`, `src/ui/roads/catalogPanel.ts#customButton`, `src/ui/roads/catalogPanel.ts#pickRoad`

### src/ui/roads/furnitureChoice.ts

- `src/ui/roads/furnitureChoice.ts#chosen` (variable, line 5) — read by 1: `src/ui/roads/furnitureChoice.ts#setFurnitureChosen`
- `src/ui/roads/furnitureChoice.ts#chosen` (variable, line 5) — write by 1: `src/ui/roads/furnitureChoice.ts#setFurnitureChosen`
- `src/ui/roads/furnitureChoice.ts#furnitureChosen` (function, line 14) — call by 1: `src/ui/v2/shell.ts#mountShell.renderRoads`
- `src/ui/roads/furnitureChoice.ts#STORAGE_KEY` (variable, line 4) — read by 1: `src/ui/roads/furnitureChoice.ts#setFurnitureChosen`

### src/ui/roads/keys.ts

- `src/ui/roads/keys.ts#bindings` (variable, line 35) — read by 3: `src/ui/roads/keys.ts#roadKey`, `src/ui/roads/keys.ts#save`, `src/ui/roads/keys.ts#setRoadKey`
- `src/ui/roads/keys.ts#bindings` (variable, line 35) — write by 2: `src/ui/roads/keys.ts#resetRoadKeys`, `src/ui/roads/keys.ts#setRoadKey`
- `src/ui/roads/keys.ts#DEFAULTS` (variable, line 8) — read by 1: `src/ui/roads/keys.ts#resetRoadKeys`
- `src/ui/roads/keys.ts#keyLabel` (function, line 97) — call by 3: `src/ui/v2/shell.ts#mountShell.renderRoads`, `src/ui/roads/keysPanel.ts#openRoadKeys.onKey`, `src/ui/roads/keysPanel.ts#openRoadKeys.render`
- `src/ui/roads/keys.ts#listeners` (variable, line 36) — read by 1: `src/ui/roads/keys.ts#save`
- `src/ui/roads/keys.ts#normaliseKey` (function, line 31) — call by 2: `src/ui/roads/keys.ts#bindableKey`, `src/ui/roads/keys.ts#setRoadKey`
- `src/ui/roads/keys.ts#onRoadKeysChange` (function, line 91) — call by 1: `src/ui/v2/shell.ts#mountShell`
- `src/ui/roads/keys.ts#reserved` (function, line 28) — call by 1: `src/ui/roads/keys.ts#bindableKey`
- `src/ui/roads/keys.ts#ROAD_KEY_ACTIONS` (variable, line 6) — read by 2: `src/ui/roads/keysPanel.ts#openRoadKeys.render`, `src/ui/roads/keys.ts#setRoadKey`
- `src/ui/roads/keys.ts#RoadKeyAction` (type, line 5) — typeRef by 3: `src/ui/roads/keys.ts#roadKey`, `src/ui/roads/keysPanel.ts#openRoadKeys`, `src/ui/roads/keys.ts#setRoadKey`
- `src/ui/roads/keys.ts#STORAGE_KEY` (variable, line 9) — read by 1: `src/ui/roads/keys.ts#save`

### src/ui/roads/money.ts

- `src/ui/roads/money.ts#formatMoney` (function, line 5) — call by 2: `src/ui/v2/shell.ts#mountShell.syncMoney`, `src/ui/roads/catalogPanel.ts#catalogStrip`

### src/ui/roadSectionEditor.ts

- `src/ui/roadSectionEditor.ts#freeRoadsEnabled` (function, line 10) — call by 1: `src/ui/inspector.ts#renderSegment`
- `src/ui/roadSectionEditor.ts#turnsExpanded` (variable, line 12) — read by 1: `src/ui/roadSectionEditor.ts#mountRoadSectionEditor`
- `src/ui/roadSectionEditor.ts#turnsExpanded` (variable, line 12) — write by 1: `src/ui/roadSectionEditor.ts#mountRoadSectionEditor`

### src/world/barriers.ts

- `src/world/barriers.ts#Barrier` (interface, line 9) — read by 1: `src/editor/chunkedSave.ts#KEYED`
- `src/world/barriers.ts#Barrier` (interface, line 9) — typeRef by 1: `src/editor/chunkedSave.ts#KEYED`
- `src/world/barriers.ts#BARRIER_KINDS` (variable, line 4) — read by 1: `src/ui/v2/shell.ts#mountShell.renderLandscape`

### src/world/buildings/blueprints.ts

- `src/world/buildings/blueprints.ts#Blueprint` (interface, line 23) — read by 3: `src/ui/v2/shell.ts#mountShell.renderBuild`, `src/ui/v2/shell.ts#mountShell.renderModels`, `src/ui/v2/shell.ts#mountShell.sigOf`
- `src/world/buildings/blueprints.ts#BLUEPRINTS` (variable, line 306) — read by 2: `src/ui/v2/shell.ts#mountShell.renderBuild`, `src/ui/v2/shell.ts#mountShell.renderModels`

### src/world/buildings/cameraSolids.ts

- `src/world/buildings/cameraSolids.ts#at` (function, line 78) — call by 2: `src/world/buildings/cameraSolids.ts#SolidFiling.visitCells`, `src/world/buildings/cameraSolids.ts#SolidFiling.forEachOccluder`
- `src/world/buildings/cameraSolids.ts#CameraSolids` (interface, line 80) — call by 2: `src/render/occlusion.ts#OcclusionBuffer.build`, `src/render/canyonLight.ts#createCanyonLight.follow`
- `src/world/buildings/cameraSolids.ts#CameraSolids` (interface, line 80) — typeRef by 2: `src/render/occlusion.ts#OcclusionBuffer.build`, `src/render/canyonLight.ts#CanyonLight`
- `src/world/buildings/cameraSolids.ts#CELL` (variable, line 14) — read by 2: `src/world/buildings/cameraSolids.ts#SolidFiling.visitCells`, `src/world/buildings/cameraSolids.ts#SolidFiling.forEachOccluder`
- `src/world/buildings/cameraSolids.ts#cellKey` (function, line 177) — call by 2: `src/world/buildings/cameraSolids.ts#SolidFiling.visitCells`, `src/world/buildings/cameraSolids.ts#SolidFiling.forEachOccluder`
- `src/world/buildings/cameraSolids.ts#CLEARANCE` (variable, line 10) — read by 1: `src/world/buildings/cameraSolids.ts#SolidFiling.visitCells`
- `src/world/buildings/cameraSolids.ts#OccluderVisit` (type, line 94) — typeRef by 1: `src/world/buildings/cameraSolids.ts#SolidFiling.forEachOccluder`
- `src/world/buildings/cameraSolids.ts#PAGE` (variable, line 26) — read by 1: `src/world/buildings/cameraSolids.ts#SolidPages.visitOnce`
- `src/world/buildings/cameraSolids.ts#PAGE_BITS` (variable, line 25) — read by 1: `src/world/buildings/cameraSolids.ts#SolidPages.visitOnce`
- `src/world/buildings/cameraSolids.ts#Rect` (type, line 102) — typeRef by 3: `src/render/canyonLight.ts#createCanyonLight.clearCells`, `src/render/canyonLight.ts#createCanyonLight.follow`, `src/world/buildings/cameraSolids.ts#SolidFiling.forEachInAny`
- `src/world/buildings/cameraSolids.ts#S_BX,S_BY,S_COS,S_SIN,S_X0,S_Y0,S_X1,S_Y1,S_FOOT,S_TOP` (variable, line 18) — read by 2: `src/world/buildings/cameraSolids.ts#SolidFiling.visitCells`, `src/world/buildings/cameraSolids.ts#SolidFiling.forEachOccluder`
- `src/world/buildings/cameraSolids.ts#S_WALL0,S_WALL1,S_INSET` (variable, line 20) — read by 1: `src/world/buildings/cameraSolids.ts#SolidFiling.forEachOccluder`
- `src/world/buildings/cameraSolids.ts#SolidFiling.cells` (field, line 200) — read by 2: `src/world/buildings/cameraSolids.ts#SolidFiling.visitCells`, `src/world/buildings/cameraSolids.ts#SolidFiling.forEachOccluder`
- `src/world/buildings/cameraSolids.ts#SolidFiling.pages` (field, line 199) — read by 4: `src/world/buildings/cameraSolids.ts#SolidFiling.forEachIn`, `src/world/buildings/cameraSolids.ts#SolidFiling.forEachInAny`, `src/world/buildings/cameraSolids.ts#SolidFiling.visitCells`, `src/world/buildings/cameraSolids.ts#SolidFiling.forEachOccluder`
- `src/world/buildings/cameraSolids.ts#SolidPages.page` (method, line 65) — call by 2: `src/world/buildings/cameraSolids.ts#SolidFiling.visitCells`, `src/world/buildings/cameraSolids.ts#SolidFiling.forEachOccluder`
- `src/world/buildings/cameraSolids.ts#SolidPages.stamps` (field, line 31) — read by 2: `src/world/buildings/cameraSolids.ts#SolidPages.walk`, `src/world/buildings/cameraSolids.ts#SolidPages.visitOnce`
- `src/world/buildings/cameraSolids.ts#SolidVisit` (type, line 97) — typeRef by 4: `src/render/canyonLight.ts#createCanyonLight.layVolume`, `src/world/buildings/cameraSolids.ts#SolidFiling.forEachIn`, `src/world/buildings/cameraSolids.ts#SolidFiling.forEachInAny`, `src/world/buildings/cameraSolids.ts#SolidFiling.visitCells`

### src/world/buildings/cityBuildings.ts

- `src/world/buildings/cityBuildings.ts#CITY_BUILDINGS` (variable, line 197) — read by 2: `src/ui/v2/shell.ts#mountShell.renderBuild`, `src/ui/v2/shell.ts#mountShell.renderModels`
- `src/world/buildings/cityBuildings.ts#CityBuilding` (interface, line 26) — read by 2: `src/ui/v2/shell.ts#mountShell.renderBuild`, `src/ui/v2/shell.ts#mountShell.renderModels`

### src/world/buildings/lotLibrary.ts

- `src/world/buildings/lotLibrary.ts#LOT_NAMES` (variable, line 9) — read by 2: `src/ui/v2/shell.ts#mountShell.renderBuild`, `src/ui/v2/shell.ts#mountShell.renderModels`

### src/world/buildings/materials.ts

- `src/world/buildings/materials.ts#Finish` (type, line 7) — typeRef by 3: `src/ui/materialSwatch.ts#materialSwatch`, `src/ui/materialSwatch.ts#BASE`, `src/ui/materialSwatch.ts#draw`
- `src/world/buildings/materials.ts#FINISH_COLOUR` (variable, line 30) — read by 1: `src/ui/materialSwatch.ts#BASE`
- `src/world/buildings/materials.ts#FINISHES` (variable, line 6) — read by 2: `src/ui/v2/shell.ts#mountShell.renderSection`, `src/ui/materialSwatch.ts#BASE`
- `src/world/buildings/materials.ts#MaterialSpec` (interface, line 9) — read by 1: `src/ui/v2/shell.ts#mountShell.renderSection`
- `src/world/buildings/materials.ts#STYLES` (variable, line 260) — read by 1: `src/ui/v2/shell.ts#mountShell.renderSection`

### src/world/buildings/recordTable.ts

- `src/world/buildings/recordTable.ts#allocate` (function, line 17) — call by 1: `src/world/doc.ts#RoadDoc.fromBinary`
- `src/world/buildings/recordTable.ts#encodeRecords` (function, line 108) — call by 1: `src/editor/regionFrames.ts#squareFrame`
- `src/world/buildings/recordTable.ts#RecordTable` (class, line 302) — construct by 1: `src/editor/history.ts#openedGenerated`
- `src/world/buildings/recordTable.ts#RecordTable.constructor` (constructor, line 312) — read by 1: `src/editor/regionFrames.ts#squareFrame`
- `src/world/buildings/recordTable.ts#RecordTable.rows` (field, line 303) — read by 1: `src/editor/history.ts#openedGenerated`
- `src/world/buildings/recordTable.ts#tableRecords` (function, line 484) — call by 1: `src/editor/regionFrames.ts#readSquareFrame`

### src/world/buildings/sections.ts

- `src/world/buildings/sections.ts#BuildingSections` (interface, line 7) — read by 1: `src/world/buildings/sections.ts#sectionsBytes`
- `src/world/buildings/sections.ts#BuildingSections` (interface, line 7) — typeRef by 1: `src/world/buildings/sections.ts#sectionsBytes`

### src/world/buildings/store.ts

- `src/world/buildings/store.ts#BuildingStore.all` (method, line 123) — call by 1: `src/editor/chunkedSave.ts#KEYED`
- `src/world/buildings/store.ts#BuildingStore.get` (method, line 115) — call by 2: `src/editor/chunkedSave.ts#KEYED`, `src/world/doc.ts#RoadDoc.recordOf`
- `src/world/buildings/store.ts#BuildingStore.items` (field, line 34) — read by 1: `src/world/buildings/store.ts#BuildingStore.toText`
- `src/world/buildings/store.ts#BuildingStore.size` (getter, line 99) — call by 1: `src/world/doc.ts#RoadDoc.toText`

### src/world/buildings/types.ts

- `src/world/buildings/types.ts#Building` (interface, line 284) — read by 1: `src/editor/regionFrames.ts#wearFrame`
- `src/world/buildings/types.ts#Building` (interface, line 284) — typeRef by 4: `src/editor/chunkedSave.ts#KEYED`, `src/editor/chunkedSave.ts#TiledSaves.write`, `src/editor/regionFrames.ts#squareFrame`, `src/editor/regionFrames.ts#wearFrame`
- `src/world/buildings/types.ts#BuildingId` (type, line 9) — typeRef by 2: `src/editor/chunkedSave.ts#KEYED`, `src/world/doc.ts#RoadDoc.recordOf`
- `src/world/buildings/types.ts#FACADE_PATTERNS` (variable, line 42) — read by 3: `src/ui/v2/shell.ts#mountShell`, `src/ui/v2/shell.ts#mountShell.renderBuild`, `src/ui/v2/shell.ts#mountShell.renderSection`

### src/world/cells.ts

- `src/world/cells.ts#CELL` (variable, line 9) — read by 1: `src/editor/chunkedSave.ts#DENSE_SQUARE`

### src/world/changes.ts

- `src/world/changes.ts#Change` (interface, line 35) — read by 2: `src/editor/chunkedSave.ts#TiledSaves.note`, `src/editor/chunkedSave.ts#TiledSaves.worn`
- `src/world/changes.ts#Change` (interface, line 35) — typeRef by 2: `src/editor/chunkedSave.ts#TiledSaves.note`, `src/editor/chunkedSave.ts#TiledSaves.worn`
- `src/world/changes.ts#ChangeKind` (type, line 27) — typeRef by 3: `src/editor/chunkedSave.ts#GLOBAL_KINDS`, `src/editor/chunkedSave.ts#KeyedSpec`, `src/editor/chunkedSave.ts#ListSpec`
- `src/world/changes.ts#ChangeRect` (type, line 4) — typeRef by 5: `src/editor/chunkedSave.ts#KeyedState.dirtyOver`, `src/editor/chunkedSave.ts#ListState.rects`, `src/editor/chunkedSave.ts#meets`, `src/editor/chunkedSave.ts#squareRect`, …
- `src/world/changes.ts#DOC_CHANGE_KINDS` (variable, line 24) — read by 1: `src/editor/chunkedSave.ts#TiledSaves.cursor`

### src/world/cityGen/mapSize.ts

- `src/world/cityGen/mapSize.ts#MAP_HALF` (variable, line 9) — read by 1: `src/render/isoViewport.ts#createIsoRig.apply`

### src/world/derived.ts

- `src/world/derived.ts#JournalCursor` (class, line 6) — construct by 1: `src/editor/chunkedSave.ts#TiledSaves.cursor`
- `src/world/derived.ts#JournalCursor.read` (method, line 19) — call by 2: `src/editor/chunkedSave.ts#TiledSaves.adopt`, `src/editor/chunkedSave.ts#TiledSaves.take`

### src/world/doc.ts

- `src/world/doc.ts#AFTER_BUILDINGS` (variable, line 3527) — read by 1: `src/world/doc.ts#RoadDoc.toText`
- `src/world/doc.ts#copyRecord` (function, line 3617) — call by 1: `src/world/doc.ts#RoadDoc.endEdit`
- `src/world/doc.ts#EditRecorder` (interface, line 3562) — read by 2: `src/world/doc.ts#RoadDoc.endEdit`, `src/world/doc.ts#RoadDoc.beginEdit`
- `src/world/doc.ts#EditRecorder` (interface, line 3562) — typeRef by 1: `src/world/doc.ts#RoadDoc.beginEdit`
- `src/world/doc.ts#EditRecorder` (interface, line 3562) — write by 1: `src/world/doc.ts#RoadDoc.beginEdit`
- `src/world/doc.ts#GLOBAL_PARTS` (variable, line 3630) — read by 6: `src/editor/chunkedSave.ts#isTiledManifest`, `src/editor/chunkedSave.ts#TiledSaves.everything`, `src/editor/chunkedSave.ts#TiledSaves.forget`, `src/editor/chunkedSave.ts#TiledSaves.globals`, …
- `src/world/doc.ts#GLOBAL_PARTS` (variable, line 3630) — typeRef by 1: `src/world/doc.ts#GlobalPart`
- `src/world/doc.ts#JunctionControl` (type, line 77) — typeRef by 5: `src/ui/inspector.ts#InspectorActions`, `src/ui/inspector.ts#junctionHost.setControl`, `src/editor/persistence.ts#isJunctionControl`, `src/ui/roads/junctionPanel.ts#CONTROLS`, …
- `src/world/doc.ts#ListPart` (type, line 2942) — typeRef by 2: `src/world/doc.ts#DocPatch`, `src/world/docImage.ts#PARTS_OF_SAVED`
- `src/world/doc.ts#MemberDiff` (class, line 3573) — construct by 1: `src/world/doc.ts#memberDiffs`
- `src/world/doc.ts#MemberDiff` (class, line 3573) — reference by 3: `src/world/doc.ts#memberDiffs`, `src/world/doc.ts#patchBytes`, `src/world/doc.ts#MemberDiff.keyOrder`
- `src/world/doc.ts#MemberDiff` (class, line 3573) — typeRef by 1: `src/world/doc.ts#memberDiffs`
- `src/world/doc.ts#MemberDiff.orders` (field, line 3577) — read by 1: `src/world/doc.ts#MemberDiff.keyOrder`
- `src/world/doc.ts#MemberDiff.orders` (field, line 3577) — write by 1: `src/world/doc.ts#MemberDiff.keyOrder`
- `src/world/doc.ts#MemberDiff.ORDERS_KEPT` (field, line 3578) — read by 1: `src/world/doc.ts#MemberDiff.keyOrder`
- `src/world/doc.ts#NodeCrossing` (interface, line 119) — read by 1: `src/ui/inspector.ts#renderNode`
- `src/world/doc.ts#NodeCrossingKind` (type, line 118) — typeRef by 1: `src/ui/inspector.ts#InspectorActions`
- `src/world/doc.ts#RecordPart` (type, line 2941) — typeRef by 3: `src/world/doc.ts#DocPatch`, `src/world/docImage.ts#PARTS_OF_SAVED`, `src/world/doc.ts#RoadDoc.recordOf`
- `src/world/doc.ts#Restoration` (interface, line 3169) — typeRef by 1: `src/world/docImage.ts#sideOf`
- `src/world/doc.ts#RoadDoc` (class, line 153) — reference by 2: `src/editor/persistence.ts#readMapFile`, `src/world/doc.ts#RoadDoc.fromBinary`
- `src/world/doc.ts#RoadDoc` (class, line 153) — typeRef by 46: `src/ui/inspector.ts#endSelect`, `src/ui/inspector.ts#InspectorContext`, `src/ui/inspector.ts#junctionHost`, `src/ui/inspector.ts#nodeStats`, …
- `src/world/doc.ts#RoadDoc.barrierMap` (field, line 169) — read by 1: `src/world/doc.ts#RoadDoc.recordOf`
- `src/world/doc.ts#RoadDoc.barriers` (getter, line 170) — call by 1: `src/editor/chunkedSave.ts#KEYED`
- `src/world/doc.ts#RoadDoc.buildings` (field, line 289) — read by 3: `src/editor/chunkedSave.ts#KEYED`, `src/world/doc.ts#RoadDoc.toText`, `src/world/doc.ts#RoadDoc.recordOf`
- `src/world/doc.ts#RoadDoc.changes` (field, line 262) — read by 2: `src/editor/chunkedSave.ts#TiledSaves.adopt`, `src/editor/chunkedSave.ts#TiledSaves.take`
- `src/world/doc.ts#RoadDoc.clouds` (field, line 216) — read by 1: `src/editor/chunkedSave.ts#LISTS`
- `src/world/doc.ts#RoadDoc.degree` (method, line 686) — call by 1: `src/ui/inspector.ts#endSelect`
- `src/world/doc.ts#RoadDoc.economy` (field, line 220) — read by 1: `src/world/doc.ts#RoadDoc.savedGlobals`
- `src/world/doc.ts#RoadDoc.elements` (field, line 243) — read by 1: `src/editor/chunkedSave.ts#LISTS`
- `src/world/doc.ts#RoadDoc.fogDabs` (field, line 210) — read by 1: `src/editor/chunkedSave.ts#LISTS`
- `src/world/doc.ts#RoadDoc.fogSettings` (field, line 211) — read by 1: `src/world/doc.ts#RoadDoc.savedGlobals`
- `src/world/doc.ts#RoadDoc.fromImage` (method, line 2571) — call by 1: `src/world/doc.ts#RoadDoc.fromBinary`
- `src/world/doc.ts#RoadDoc.gullyAuto` (field, line 214) — read by 1: `src/world/doc.ts#RoadDoc.savedGlobals`
- `src/world/doc.ts#RoadDoc.gullyDabs` (field, line 213) — read by 1: `src/editor/chunkedSave.ts#LISTS`
- `src/world/doc.ts#RoadDoc.image` (method, line 2566) — call by 1: `src/editor/history.ts#History.undo`
- `src/world/doc.ts#RoadDoc.landscape` (getter, line 178) — call by 1: `src/editor/chunkedSave.ts#KEYED`
- `src/world/doc.ts#RoadDoc.landscapeMap` (field, line 177) — read by 1: `src/world/doc.ts#RoadDoc.recordOf`
- `src/world/doc.ts#RoadDoc.listen` (method, line 2200) — call by 2: `src/world/doc.ts#RoadDoc.endEdit`, `src/world/doc.ts#RoadDoc.beginEdit`
- `src/world/doc.ts#RoadDoc.listOf` (method, line 2340) — call by 1: `src/world/doc.ts#RoadDoc.endEdit`
- `src/world/doc.ts#RoadDoc.lotStore` (field, line 297) — read by 3: `src/editor/chunkedSave.ts#KEYED`, `src/world/doc.ts#RoadDoc.savedGlobals`, `src/world/doc.ts#RoadDoc.recordOf`
- `src/world/doc.ts#RoadDoc.markingStyle` (field, line 204) — read by 1: `src/world/doc.ts#RoadDoc.savedGlobals`
- `src/world/doc.ts#RoadDoc.mergeNodes` (method, line 820) — call by 1: `src/editor/repair.ts#repairNearConnections`
- `src/world/doc.ts#RoadDoc.nature` (field, line 206) — read by 1: `src/world/doc.ts#RoadDoc.savedGlobals`
- `src/world/doc.ts#RoadDoc.node` (method, line 416) — call by 9: `src/ui/inspector.ts#endSelect`, `src/ui/inspector.ts#nodeStats`, `src/ui/inspector.ts#renderNode`, `src/ui/inspector.ts#renderSegment`, …
- `src/world/doc.ts#RoadDoc.nodes` (field, line 154) — read by 4: `src/editor/chunkedSave.ts#KEYED`, `src/editor/chunkedSave.ts#nodeOf`, `src/editor/repair.ts#nearestCandidate`, `src/world/doc.ts#RoadDoc.recordOf`
- `src/world/doc.ts#RoadDoc.people` (field, line 346) — read by 1: `src/world/doc.ts#RoadDoc.savedGlobals`
- `src/world/doc.ts#RoadDoc.poleMap` (field, line 161) — read by 1: `src/world/doc.ts#RoadDoc.recordOf`
- `src/world/doc.ts#RoadDoc.poles` (getter, line 164) — call by 1: `src/editor/chunkedSave.ts#KEYED`
- `src/world/doc.ts#RoadDoc.poleSpans` (getter, line 165) — call by 1: `src/editor/chunkedSave.ts#KEYED`
- `src/world/doc.ts#RoadDoc.recorder` (field, line 2130) — read by 1: `src/world/doc.ts#RoadDoc.endEdit`
- `src/world/doc.ts#RoadDoc.recorder` (field, line 2130) — write by 2: `src/world/doc.ts#RoadDoc.endEdit`, `src/world/doc.ts#RoadDoc.beginEdit`
- `src/world/doc.ts#RoadDoc.replaceFromJSON` (method, line 2053) — call by 2: `src/editor/history.ts#restoreInto`, `src/editor/history.ts#restoreSnapshot`
- `src/world/doc.ts#RoadDoc.restore` (method, line 2233) — call by 3: `src/editor/history.ts#History.cancel`, `src/editor/history.ts#History.redo`, `src/editor/history.ts#History.undo`
- `src/world/doc.ts#RoadDoc.revision` (getter, line 371) — call by 3: `src/ui/inspector.ts#renderCurrent`, `src/editor/history.ts#followNetwork`, `src/editor/history.ts#restoreSnapshot`
- `src/world/doc.ts#RoadDoc.segment` (method, line 420) — call by 6: `src/ui/inspector.ts#renderSegment`, `src/ui/inspector.ts#segmentStats`, `src/editor/repair.ts#nearestCandidate`, `src/world/roads/streetNames.ts#streetNumbers`, …
- `src/world/doc.ts#RoadDoc.segments` (field, line 155) — read by 2: `src/editor/chunkedSave.ts#KEYED`, `src/world/doc.ts#RoadDoc.recordOf`
- `src/world/doc.ts#RoadDoc.serialized` (method, line 2636) — call by 1: `src/world/doc.ts#RoadDoc.toText`
- `src/world/doc.ts#RoadDoc.spanMap` (field, line 162) — read by 1: `src/world/doc.ts#RoadDoc.recordOf`
- `src/world/doc.ts#RoadDoc.spansAt` (method, line 455) — call by 1: `src/editor/chunkedSave.ts#KEYED`
- `src/world/doc.ts#RoadDoc.terrainPaint` (field, line 208) — read by 1: `src/editor/chunkedSave.ts#LISTS`
- `src/world/doc.ts#RoadDoc.terrainRelief` (field, line 202) — read by 1: `src/world/doc.ts#RoadDoc.savedGlobals`
- `src/world/doc.ts#RoadDoc.terrainStamps` (field, line 200) — read by 1: `src/editor/chunkedSave.ts#LISTS`
- `src/world/doc.ts#RoadDoc.toJSON` (method, line 2556) — call by 2: `src/editor/persistence.ts#Persistence.saveSession`, `src/editor/persistence.ts#readMapFile`
- `src/world/doc.ts#RoadDoc.transit` (field, line 187) — read by 1: `src/world/doc.ts#RoadDoc.savedGlobals`
- `src/world/doc.ts#RoadDoc.treeClearings` (field, line 241) — read by 1: `src/editor/chunkedSave.ts#LISTS`
- `src/world/doc.ts#RoadDoc.trees` (field, line 222) — read by 1: `src/editor/chunkedSave.ts#LISTS`
- `src/world/doc.ts#RoadDoc.valueOf` (method, line 2358) — call by 2: `src/world/doc.ts#RoadDoc.endEdit`, `src/world/doc.ts#RoadDoc.beginEdit`
- `src/world/doc.ts#RoadDoc.weather` (field, line 218) — read by 1: `src/world/doc.ts#RoadDoc.savedGlobals`
- `src/world/doc.ts#RoadDoc.zoneMarks` (field, line 295) — read by 1: `src/editor/chunkedSave.ts#LISTS`
- `src/world/doc.ts#RoadDoc.zones` (field, line 292) — read by 1: `src/editor/chunkedSave.ts#LISTS`
- `src/world/doc.ts#RoadEnd` (type, line 116) — typeRef by 1: `src/ui/inspector.ts#InspectorActions`
- `src/world/doc.ts#RoadNode` (interface, line 89) — read by 10: `src/ui/inspector.ts#endSelect`, `src/ui/inspector.ts#nodeStats`, `src/ui/inspector.ts#renderNode`, `src/ui/inspector.ts#renderSegment`, …
- `src/world/doc.ts#RoadNode` (interface, line 89) — typeRef by 2: `src/editor/chunkedSave.ts#KEYED`, `src/editor/chunkedSave.ts#nodeOf`
- `src/world/doc.ts#RoadSegment` (interface, line 124) — read by 6: `src/ui/inspector.ts#renderSegment`, `src/ui/inspector.ts#segmentStats`, `src/editor/chunkedSave.ts#KEYED`, `src/editor/repair.ts#nearestCandidate`, …
- `src/world/doc.ts#RoadSegment` (interface, line 124) — typeRef by 2: `src/editor/chunkedSave.ts#KEYED`, `src/ui/roads/profilePanel.ts#mountProfilePanel`
- `src/world/doc.ts#savedBarrier` (function, line 3695) — read by 1: `src/editor/chunkedSave.ts#KEYED`
- `src/world/doc.ts#savedLandscape` (function, line 3692) — read by 1: `src/editor/chunkedSave.ts#KEYED`
- `src/world/doc.ts#savedNode` (function, line 3650) — reference by 1: `src/editor/chunkedSave.ts#KEYED`
- `src/world/doc.ts#savedPole` (function, line 3690) — read by 1: `src/editor/chunkedSave.ts#KEYED`
- `src/world/doc.ts#savedSegment` (function, line 3672) — reference by 1: `src/editor/chunkedSave.ts#KEYED`
- `src/world/doc.ts#savedSpan` (function, line 3691) — read by 1: `src/editor/chunkedSave.ts#KEYED`
- `src/world/doc.ts#SerializedDoc` (interface, line 3181) — read by 1: `src/editor/history.ts#openedGenerated`
- `src/world/doc.ts#SerializedDoc` (interface, line 3181) — typeRef by 12: `src/editor/history.ts#openedGenerated`, `src/editor/history.ts#restoreInto`, `src/editor/history.ts#restoreSnapshot`, `src/editor/persistence.ts#isSerializedDoc`, …
- `src/world/doc.ts#VALUE_PARTS` (variable, line 3570) — read by 1: `src/world/doc.ts#RoadDoc.beginEdit`
- `src/world/doc.ts#ValuePart` (type, line 2944) — typeRef by 2: `src/world/doc.ts#DocPatch`, `src/world/docImage.ts#PARTS_OF_SAVED`

### src/world/docBinary.ts

- `src/world/docBinary.ts#align8` (function, line 65) — call by 1: `src/world/docBinary.ts#unpackImage`
- `src/world/docBinary.ts#decoder` (variable, line 23) — read by 1: `src/world/docBinary.ts#unpackImage`
- `src/world/docBinary.ts#DocImage` (interface, line 14) — read by 1: `src/world/docBinary.ts#imageBytes`
- `src/world/docBinary.ts#DocImage` (interface, line 14) — typeRef by 4: `src/world/docBinary.ts#unpackImage`, `src/world/doc.ts#DocPatch`, `src/world/doc.ts#wholeBytes`, `src/world/docBinary.ts#imageBytes`
- `src/world/docBinary.ts#DocPart` (interface, line 6) — read by 1: `src/world/docBinary.ts#imageBytes`
- `src/world/docBinary.ts#DocPart` (interface, line 6) — typeRef by 1: `src/world/docBinary.ts#unpackImage`
- `src/world/docBinary.ts#FORMAT` (variable, line 60) — read by 1: `src/world/docBinary.ts#isDocBinary`
- `src/world/docBinary.ts#HEAD,ENTRY` (variable, line 64) — read by 2: `src/world/docBinary.ts#unpackImage`, `src/world/docBinary.ts#isDocBinary`
- `src/world/docBinary.ts#MAGIC` (variable, line 59) — read by 1: `src/world/docBinary.ts#isDocBinary`
- `src/world/docBinary.ts#S_PART,S_RUN,S_BASE,S_GONE,S_REPLACED,S_ADDED` (variable, line 62) — read by 1: `src/world/docBinary.ts#unpackImage`

### src/world/ecology.ts

- `src/world/ecology.ts#isNatureSettings` (function, line 18) — call by 1: `src/editor/persistence.ts#isSerializedDoc`

### src/world/economy.ts

- `src/world/economy.ts#DEFAULT_ECONOMY` (variable, line 19) — read by 1: `src/world/doc.ts#RoadDoc.savedGlobals`
- `src/world/economy.ts#Economy` (interface, line 14) — read by 1: `src/world/doc.ts#RoadDoc.savedGlobals`
- `src/world/economy.ts#pricePerUnit` (function, line 37) — call by 1: `src/ui/roads/catalogPanel.ts#catalogStrip`
- `src/world/economy.ts#readEconomy` (function, line 22) — call by 1: `src/editor/persistence.ts#isSerializedDoc`

### src/world/elements.ts

- `src/world/elements.ts#DEFAULT_BRUSH` (variable, line 45) — read by 1: `src/ui/toolChoices.ts#elementBrush`
- `src/world/elements.ts#EFFECT_KINDS` (variable, line 4) — read by 2: `src/ui/v2/shell.ts#mountShell.isEffect`, `src/ui/v2/shell.ts#mountShell.renderLandscape`
- `src/world/elements.ts#ElementBrush` (interface, line 29) — typeRef by 3: `src/ui/toolChoices.ts#elementBrush`, `src/ui/toolChoices.ts#elementBrushes`, `src/ui/toolChoices.ts#setElementBrush`
- `src/world/elements.ts#ElementKind` (type, line 6) — typeRef by 8: `src/ui/v2/shell.ts#mountShell`, `src/ui/v2/shell.ts#mountShell.openLandTab`, `src/ui/v2/shell.ts#mountShell.useElement`, `src/ui/toolChoices.ts#elementBrush`, …

### src/world/geometry.ts

- `src/world/geometry.ts#PolylineCache.get` (method, line 70) — call by 4: `src/ui/inspector.ts#renderSegment`, `src/ui/inspector.ts#segmentStats`, `src/editor/repair.ts#nearestCandidate`, `src/world/roads/streetNames.ts#streetNumbers`

### src/world/grid.ts

- `src/world/grid.ts#onGridLength` (function, line 11) — call by 1: `src/ui/roadSectionEditor.ts#mountRoadSectionEditor`

### src/world/ids.ts

- `src/world/ids.ts#NodeId` (type, line 7) — typeRef by 16: `src/ui/inspector.ts#endSelect`, `src/ui/inspector.ts#InspectorActions`, `src/ui/inspector.ts#InspectorSelection`, `src/ui/inspector.ts#junctionHost`, …
- `src/world/ids.ts#PoleId` (type, line 10) — typeRef by 2: `src/editor/chunkedSave.ts#KEYED`, `src/world/doc.ts#RoadDoc.recordOf`
- `src/world/ids.ts#SegmentId` (type, line 8) — typeRef by 16: `src/ui/inspector.ts#InspectorActions`, `src/ui/inspector.ts#InspectorSelection`, `src/ui/inspector.ts#renderSegment`, `src/ui/inspector.ts#segmentStats`, …
- `src/world/ids.ts#SpanId` (type, line 12) — typeRef by 2: `src/editor/chunkedSave.ts#KEYED`, `src/world/doc.ts#RoadDoc.recordOf`

### src/world/junction/build.ts

- `src/world/junction/build.ts#surfaceMode` (function, line 48) — call by 1: `src/ui/inspector.ts#renderNode`

### src/world/landscape.ts

- `src/world/landscape.ts#LANDSCAPE_KINDS` (variable, line 13) — read by 2: `src/ui/v2/shell.ts#mountShell.renderLandscape`, `src/ui/toolChoices.ts#kind`
- `src/world/landscape.ts#LandscapeItem` (interface, line 16) — read by 1: `src/world/roads/streetNames.ts#streetNameOf`
- `src/world/landscape.ts#LandscapeItem` (interface, line 16) — typeRef by 1: `src/editor/chunkedSave.ts#KEYED`
- `src/world/landscape.ts#LandscapeKind` (type, line 14) — typeRef by 3: `src/ui/toolChoices.ts#kind`, `src/ui/toolChoices.ts#setStreetscapeKind`, `src/ui/toolChoices.ts#streetscapeKind`
- `src/world/landscape.ts#SIGN_HAS_TEXT` (variable, line 48) — read by 1: `src/ui/v2/shell.ts#mountShell.renderLandscape`
- `src/world/landscape.ts#SIGN_TEXT_MAX` (variable, line 50) — read by 1: `src/ui/v2/shell.ts#mountShell.textInput`
- `src/world/landscape.ts#SIGN_TYPES` (variable, line 42) — read by 1: `src/ui/v2/shell.ts#mountShell.renderLandscape`
- `src/world/landscape.ts#SignType` (type, line 43) — typeRef by 1: `src/ui/toolChoices.ts#signChoice`

### src/world/lotRecord.ts

- `src/world/lotRecord.ts#Lot` (interface, line 6) — read by 1: `src/editor/chunkedSave.ts#KEYED`
- `src/world/lotRecord.ts#Lot` (interface, line 6) — typeRef by 1: `src/editor/chunkedSave.ts#KEYED`

### src/world/lotStore.ts

- `src/world/lotStore.ts#LotStore.all` (method, line 91) — call by 1: `src/editor/chunkedSave.ts#KEYED`
- `src/world/lotStore.ts#LotStore.get` (method, line 82) — call by 2: `src/editor/chunkedSave.ts#KEYED`, `src/world/doc.ts#RoadDoc.recordOf`
- `src/world/lotStore.ts#LotStore.keys` (getter, line 117) — call by 1: `src/world/doc.ts#RoadDoc.savedGlobals`
- `src/world/lotStore.ts#LotStore.orderOf` (method, line 96) — call by 1: `src/editor/chunkedSave.ts#KEYED`

### src/world/network.ts

- `src/world/network.ts#Network` (class, line 46) — typeRef by 12: `src/ui/inspector.ts#InspectorContext`, `src/ui/inspector.ts#openInspector`, `src/ui/inspector.ts#renderNode`, `src/ui/inspector.ts#renderSegment`, …
- `src/world/network.ts#Network.constructor` (constructor, line 100) — read by 1: `src/world/roads/streetNames.ts#streetNameOf`
- `src/world/network.ts#Network.impossible` (field, line 72) — read by 1: `src/ui/inspector.ts#renderNode`
- `src/world/network.ts#Network.mouthDistance` (method, line 288) — call by 1: `src/ui/inspector.ts#renderNode`
- `src/world/network.ts#Network.polylines` (field, line 80) — read by 5: `src/ui/inspector.ts#renderNode`, `src/ui/inspector.ts#renderSegment`, `src/ui/inspector.ts#segmentStats`, `src/editor/repair.ts#nearestCandidate`, …
- `src/world/network.ts#Network.rebuild` (method, line 119) — call by 4: `src/editor/history.ts#followNetwork`, `src/editor/history.ts#restoreInto`, `src/editor/history.ts#restoreSnapshot`, `src/editor/repair.ts#repairNearConnections`
- `src/world/network.ts#Network.revision` (field, line 87) — read by 2: `src/editor/history.ts#followNetwork`, `src/editor/history.ts#restoreSnapshot`

### src/world/parking.ts

- `src/world/parking.ts#PARKING_DEPTH` (variable, line 16) — read by 2: `src/ui/roads/profileEditor.ts#newElement`, `src/world/roads/catalog.ts#shapeProfile.kerb`
- `src/world/parking.ts#PARKING_KINDS` (variable, line 6) — read by 1: `src/ui/inspector.ts#parkingSelect`
- `src/world/parking.ts#parkingAllowed` (function, line 66) — call by 1: `src/ui/inspector.ts#parkingSelect`
- `src/world/parking.ts#ParkingKind` (type, line 7) — typeRef by 2: `src/ui/inspector.ts#parkingSelect`, `src/ui/inspector.ts#renderSegment.setParking`
- `src/world/parking.ts#SegmentParking` (interface, line 10) — read by 1: `src/ui/inspector.ts#renderSegment`
- `src/world/parking.ts#SegmentParking` (interface, line 10) — typeRef by 1: `src/ui/inspector.ts#InspectorActions`

### src/world/rescale.ts

- `src/world/rescale.ts#unitFactor` (function, line 10) — call by 2: `src/editor/persistence.ts#cameraInUnit`, `src/ui/roads/templates.ts#inThisUnit`

### src/world/ribbons.ts

- `src/world/ribbons.ts#SegmentRibbon` (interface, line 25) — read by 1: `src/editor/repair.ts#nearestCandidate`

### src/world/roads/connectors.ts

- `src/world/roads/connectors.ts#LaneEnds` (interface, line 80) — read by 2: `src/ui/roads/connectorPanel.ts#draw`, `src/ui/roads/connectorPanel.ts#turnOf`
- `src/world/roads/connectors.ts#LaneEnds` (interface, line 80) — typeRef by 2: `src/ui/roads/connectorPanel.ts#ConnectorPanelHost`, `src/ui/roads/connectorPanel.ts#turnOf`
- `src/world/roads/connectors.ts#LaneLink` (interface, line 5) — read by 4: `src/ui/roads/connectorPanel.ts#draw`, `src/world/roads/connectors.ts#toggleLaneLink`, `src/world/roads/connectors.ts#toggleLaneLink.arrives`, `src/world/roads/connectors.ts#toggleLaneLink.leaves`
- `src/world/roads/connectors.ts#LaneLink` (interface, line 5) — typeRef by 5: `src/ui/inspector.ts#InspectorActions`, `src/ui/roads/connectorPanel.ts#ConnectorPanelHost`, `src/world/roads/connectors.ts#toggleLaneLink`, `src/world/roads/connectors.ts#toggleLaneLink.arrives`, …
- `src/world/roads/connectors.ts#linkKey` (function, line 38) — call by 1: `src/world/roads/connectors.ts#toggleLaneLink`
- `src/world/roads/connectors.ts#linkKey` (function, line 38) — read by 1: `src/ui/roads/connectorPanel.ts#draw`
- `src/world/roads/connectors.ts#linksOf` (function, line 46) — call by 1: `src/world/roads/connectors.ts#toggleLaneLink`
- `src/world/roads/connectors.ts#movementKey` (function, line 36) — call by 2: `src/ui/roads/junctionPanel.ts#mountJunctionPanel`, `src/ui/roads/junctionPanel.ts#moveChip`
- `src/world/roads/connectors.ts#NodeLanes` (interface, line 54) — read by 4: `src/ui/roads/connectorPanel.ts#draw`, `src/world/roads/connectors.ts#toggleLaneLink`, `src/world/roads/connectors.ts#toggleLaneLink.arrives`, `src/world/roads/connectors.ts#toggleLaneLink.leaves`
- `src/world/roads/connectors.ts#NodeLanes` (interface, line 54) — typeRef by 1: `src/world/roads/connectors.ts#toggleLaneLink`
- `src/world/roads/connectors.ts#sortLinks` (function, line 41) — call by 1: `src/world/roads/connectors.ts#toggleLaneLink`

### src/world/roads/furnitureSets.ts

- `src/world/roads/furnitureSets.ts#FURNITURE_SETS` (variable, line 14) — read by 1: `src/ui/v2/shell.ts#mountShell.renderRoads`
- `src/world/roads/furnitureSets.ts#FurnitureSet` (type, line 13) — typeRef by 1: `src/ui/roads/furnitureChoice.ts#setFurnitureChosen`

### src/world/roads/profile.ts

- `src/world/roads/profile.ts#ProfileElement` (type, line 11) — read by 16: `src/ui/roads/profilePanel.ts#mountProfilePanel`, `src/ui/roads/crossSection.ts#drawSection`, `src/ui/roads/profileEditor.ts#openProfileEditor`, `src/world/roads/profile.ts#profileWidth`, …
- `src/world/roads/profile.ts#ProfileElement` (type, line 11) — typeRef by 16: `src/ui/roads/profileEditor.ts#WIDTH_LIMITS`, `src/ui/roads/profileEditor.ts#newElement`, `src/world/roads/templates.ts#normalizeTemplate`, `src/world/roads/templates.ts#readElement`, …
- `src/world/roads/profile.ts#ProfileIssue` (interface, line 104) — read by 3: `src/ui/roads/profileEditor.ts#openProfileEditor.drawStage`, `src/ui/roads/profileEditor.ts#openProfileEditor.renderProps`, `src/ui/roads/profileEditor.ts#openProfileEditor.issueLine`
- `src/world/roads/profile.ts#ProfileIssue` (interface, line 104) — typeRef by 2: `src/ui/roads/profileEditor.ts#openProfileEditor.issues`, `src/ui/roads/profileEditor.ts#openProfileEditor.issueLine`
- `src/world/roads/profile.ts#profileIssues` (function, line 110) — call by 1: `src/ui/roads/profileEditor.ts#openProfileEditor.issues`
- `src/world/roads/profile.ts#profileOf` (function, line 43) — call by 2: `src/ui/roads/profilePanel.ts#mountProfilePanel`, `src/world/roads/templates.ts#classTemplates`
- `src/world/roads/profile.ts#profileProblems` (function, line 99) — call by 1: `src/world/roads/templates.ts#normalizeTemplate`
- `src/world/roads/profile.ts#profileRoad` (function, line 260) — call by 1: `src/ui/roads/catalogPanel.ts#catalogStrip`
- `src/world/roads/profile.ts#RoadProfileSpec` (interface, line 23) — read by 18: `src/ui/roads/profilePanel.ts#mountProfilePanel`, `src/ui/roads/crossSection.ts#drawSection`, `src/ui/roads/profileEditor.ts#openProfileEditor`, `src/world/roads/profile.ts#profileWidth`, …
- `src/world/roads/profile.ts#RoadProfileSpec` (interface, line 23) — typeRef by 22: `src/ui/inspector.ts#InspectorActions`, `src/ui/roads/profilePanel.ts#mountProfilePanel`, `src/ui/roads/crossSection.ts#drawSection`, `src/ui/roads/profileEditor.ts#openProfileEditor`, …
- `src/world/roads/profile.ts#sameProfile` (function, line 177) — call by 2: `src/ui/roads/profileEditor.ts#openProfileEditor.sameAs`, `src/ui/roads/catalogPanel.ts#sameRoad`

### src/world/roads/rules.ts

- `src/world/roads/rules.ts#ApproachRule` (type, line 8) — typeRef by 1: `src/ui/roads/junctionPanel.ts#RULES`
- `src/world/roads/rules.ts#ApproachRuleEntry` (interface, line 11) — read by 1: `src/ui/roads/junctionPanel.ts#mountJunctionPanel`
- `src/world/roads/rules.ts#ApproachRuleEntry` (interface, line 11) — typeRef by 2: `src/ui/inspector.ts#InspectorActions`, `src/ui/roads/junctionPanel.ts#JunctionPanelHost`
- `src/world/roads/rules.ts#GREEN_LIMITS` (variable, line 28) — read by 1: `src/ui/roads/junctionPanel.ts#mountJunctionPanel.set`
- `src/world/roads/rules.ts#legRule` (function, line 106) — call by 1: `src/ui/roads/junctionPanel.ts#mountJunctionPanel`
- `src/world/roads/rules.ts#mainRoadLegs` (function, line 73) — call by 1: `src/ui/roads/junctionPanel.ts#mountJunctionPanel`
- `src/world/roads/rules.ts#OFFSET_LIMIT` (variable, line 29) — read by 1: `src/ui/roads/junctionPanel.ts#mountJunctionPanel`
- `src/world/roads/rules.ts#SignalSettings` (interface, line 17) — read by 3: `src/ui/roads/junctionPanel.ts#mountJunctionPanel`, `src/ui/roads/junctionPanel.ts#mountJunctionPanel.set`, `src/ui/roads/junctionPanel.ts#withoutFixed`
- `src/world/roads/rules.ts#SignalSettings` (interface, line 17) — typeRef by 4: `src/ui/inspector.ts#InspectorActions`, `src/ui/roads/junctionPanel.ts#mountJunctionPanel`, `src/ui/roads/junctionPanel.ts#JunctionPanelHost`, `src/ui/roads/junctionPanel.ts#withoutFixed`

### src/world/roads/streetChain.ts

- `src/world/roads/streetChain.ts#ChainPiece` (interface, line 11) — read by 2: `src/world/roads/streetNames.ts#streetNameOf`, `src/world/roads/streetNames.ts#streetNumbers`
- `src/world/roads/streetChain.ts#streetChain` (function, line 17) — call by 2: `src/world/roads/streetNames.ts#streetNameOf`, `src/world/roads/streetNames.ts#streetNumbers`

### src/world/roads/streetNames.ts

- `src/world/roads/streetNames.ts#nameItemSegment` (function, line 34) — call by 1: `src/world/roads/streetNames.ts#streetNameOf`
- `src/world/roads/streetNames.ts#platesNear` (function, line 44) — call by 1: `src/world/roads/streetNames.ts#streetNameOf`

### src/world/roads/tuning.ts

- `src/world/roads/tuning.ts#ROAD_TUNING` (variable, line 4) — read by 1: `src/ui/roads/money.ts#balanceTip`

### src/world/roadSection.ts

- `src/world/roadSection.ts#CARRIAGEWAY_MATERIALS` (variable, line 7) — read by 2: `src/world/roads/templates.ts#normalizeTemplate`, `src/ui/roads/profileEditor.ts#openProfileEditor.renderProps`
- `src/world/roadSection.ts#CarriagewayMaterial` (type, line 10) — typeRef by 3: `src/world/roads/templates.ts#normalizeTemplate`, `src/ui/roads/profileEditor.ts#openProfileEditor.renderProps`, `src/world/roads/catalog.ts#Shape`
- `src/world/roadSection.ts#CENTRE_LINES` (variable, line 53) — read by 2: `src/world/roads/templates.ts#normalizeTemplate`, `src/ui/roads/profileEditor.ts#openProfileEditor.renderProps`
- `src/world/roadSection.ts#CentreLine` (type, line 52) — typeRef by 2: `src/world/roads/templates.ts#normalizeTemplate`, `src/ui/roads/profileEditor.ts#openProfileEditor.renderProps`
- `src/world/roadSection.ts#FOOTWAY_MATERIALS` (variable, line 8) — read by 2: `src/world/roads/templates.ts#readElement`, `src/ui/roads/profileEditor.ts#openProfileEditor.renderProps`
- `src/world/roadSection.ts#FootwayMaterial` (type, line 11) — typeRef by 2: `src/world/roads/templates.ts#readElement`, `src/ui/roads/profileEditor.ts#openProfileEditor.renderProps`
- `src/world/roadSection.ts#LANE_TURN_RULES` (variable, line 3) — read by 1: `src/ui/roadSectionEditor.ts#mountRoadSectionEditor`
- `src/world/roadSection.ts#LaneTurnRule` (type, line 4) — typeRef by 1: `src/ui/roadSectionEditor.ts#mountRoadSectionEditor`
- `src/world/roadSection.ts#MEDIAN_MATERIALS` (variable, line 9) — read by 2: `src/world/roads/templates.ts#readElement`, `src/ui/roads/profileEditor.ts#openProfileEditor.renderProps`
- `src/world/roadSection.ts#MedianMaterial` (type, line 12) — typeRef by 1: `src/ui/roads/profileEditor.ts#openProfileEditor.renderProps`
- `src/world/roadSection.ts#normalizeRoadSection` (function, line 73) — call by 1: `src/editor/persistence.ts#isSerializedDoc`
- `src/world/roadSection.ts#ROAD_SECTION_LIMITS` (variable, line 64) — read by 4: `src/ui/roadSectionEditor.ts#mountRoadSectionEditor`, `src/ui/roads/profileEditor.ts#WIDTH_LIMITS`, `src/world/roads/templates.ts#normalizeTemplate`, `src/ui/roads/profileEditor.ts#openProfileEditor.renderProps`
- `src/world/roadSection.ts#ROAD_SECTION_LIMITS` (variable, line 64) — typeRef by 1: `src/ui/roadSectionEditor.ts#mountRoadSectionEditor`
- `src/world/roadSection.ts#RoadSection` (interface, line 23) — read by 1: `src/ui/roadSectionEditor.ts#mountRoadSectionEditor.draw`
- `src/world/roadSection.ts#RoadSection` (interface, line 23) — typeRef by 2: `src/ui/inspector.ts#InspectorActions`, `src/ui/roadSectionEditor.ts#mountRoadSectionEditor`
- `src/world/roadSection.ts#RoadSection` (interface, line 23) — write by 1: `src/ui/roadSectionEditor.ts#mountRoadSectionEditor`

### src/world/roadTypes.ts

- `src/world/roadTypes.ts#casingHalf` (function, line 384) — call by 1: `src/editor/repair.ts#nearestCandidate`
- `src/world/roadTypes.ts#laneWidth` (function, line 395) — call by 1: `src/ui/roadSectionEditor.ts#mountRoadSectionEditor`
- `src/world/roadTypes.ts#LAST_UPGRADE_CLASS` (variable, line 233) — read by 1: `src/ui/inspector.ts#renderSegment`
- `src/world/roadTypes.ts#ROAD_TYPES` (variable, line 97) — read by 7: `src/ui/inspector.ts#renderSegment`, `src/editor/persistence.ts#isSerializedDoc`, `src/world/roads/templates.ts#normalizeTemplate`, `src/world/roads/templates.ts#classTemplates`, …
- `src/world/roadTypes.ts#roadProfile` (function, line 248) — call by 2: `src/ui/inspector.ts#renderSegment`, `src/ui/inspector.ts#segmentStats`
- `src/world/roadTypes.ts#RoadType` (interface, line 29) — read by 9: `src/ui/inspector.ts#renderSegment`, `src/ui/inspector.ts#segmentStats`, `src/ui/labels.ts#roadTypeDescription`, `src/ui/labels.ts#roadTypeName`, …
- `src/world/roadTypes.ts#RoadType` (interface, line 29) — typeRef by 4: `src/ui/inspector.ts#parkingSelect`, `src/ui/labels.ts#roadTypeDescription`, `src/ui/labels.ts#roadTypeName`, `src/ui/roadSectionEditor.ts#mountRoadSectionEditor`
- `src/world/roadTypes.ts#SegmentDirection` (type, line 7) — typeRef by 7: `src/ui/inspector.ts#directionOptions`, `src/ui/inspector.ts#InspectorActions`, `src/ui/inspector.ts#laneOptions`, `src/ui/inspector.ts#renderSegment`, …
- `src/world/roadTypes.ts#travelLanes` (function, line 391) — call by 1: `src/ui/inspector.ts#segmentStats`

### src/world/structures.ts

- `src/world/structures.ts#migrateStructure` (function, line 11) — call by 1: `src/editor/persistence.ts#isSerializedDoc`
- `src/world/structures.ts#roadStructure` (function, line 55) — call by 1: `src/editor/repair.ts#nearestCandidate`
- `src/world/structures.ts#RoadStructureSpec` (interface, line 17) — read by 1: `src/editor/repair.ts#nearestCandidate`

### src/world/terrain.ts

- `src/world/terrain.ts#readRelief` (function, line 92) — call by 1: `src/editor/persistence.ts#isSerializedDoc`
- `src/world/terrain.ts#RELIEF_FLAT` (variable, line 90) — read by 1: `src/editor/history.ts#openedGenerated`
- `src/world/terrain.ts#RELIEF_LEGACY` (variable, line 88) — read by 1: `src/world/doc.ts#RoadDoc.savedGlobals`
- `src/world/terrain.ts#TerrainMode` (type, line 4) — typeRef by 1: `src/world/terrain.ts#isTerrainMode`
- `src/world/terrain.ts#TerrainProfile` (type, line 29) — typeRef by 1: `src/world/terrain.ts#isTerrainProfile`

### src/world/terrainPaint.ts

- `src/world/terrainPaint.ts#PAINT_KINDS` (variable, line 4) — read by 2: `src/ui/v2/shell.ts#mountShell.renderLandscape`, `src/ui/toolChoices.ts#paint`
- `src/world/terrainPaint.ts#PaintKind` (type, line 6) — typeRef by 5: `src/ui/v2/shell.ts#mountShell.renderLandscape`, `src/ui/v2/shell.ts#PAINT_SWATCH`, `src/ui/toolChoices.ts#paint`, `src/ui/toolChoices.ts#paintKind`, …

### src/world/transit.ts

- `src/world/transit.ts#hasTransit` (function, line 103) — call by 1: `src/world/doc.ts#RoadDoc.savedGlobals`
- `src/world/transit.ts#TransitLine` (interface, line 35) — read by 2: `src/ui/v2/shell.ts#mountShell.renderTransit`, `src/ui/v2/transitLine.ts#transitLineRow`
- `src/world/transit.ts#TransitLine` (interface, line 35) — typeRef by 2: `src/ui/v2/shell.ts#ShellTransitTool`, `src/ui/v2/transitLine.ts#transitLineRow`

### src/world/trees.ts

- `src/world/trees.ts#DEFAULT_TREE_BRUSH` (variable, line 38) — read by 1: `src/ui/toolChoices.ts#treeBrushNow`
- `src/world/trees.ts#TREE_KINDS` (variable, line 4) — read by 1: `src/ui/v2/shell.ts#mountShell.renderLandscape`
- `src/world/trees.ts#TreeBrush` (interface, line 27) — typeRef by 3: `src/ui/toolChoices.ts#setTreeBrush`, `src/ui/toolChoices.ts#treeBrush`, `src/ui/toolChoices.ts#treeBrushNow`
- `src/world/trees.ts#TreeKind` (type, line 5) — typeRef by 3: `src/ui/toolChoices.ts#setTreeKind`, `src/ui/toolChoices.ts#treeKind`, `src/ui/toolChoices.ts#treeKindNow`

### src/world/utilities.ts

- `src/world/utilities.ts#POLE_LAMP_MODES` (variable, line 72) — read by 2: `src/ui/v2/shell.ts#mountShell.renderLandscape`, `src/ui/toolChoices.ts#lamps`
- `src/world/utilities.ts#PoleLampMode` (type, line 73) — typeRef by 3: `src/ui/toolChoices.ts#lamps`, `src/ui/toolChoices.ts#poleLampMode`, `src/ui/toolChoices.ts#setPoleLampMode`
- `src/world/utilities.ts#UtilityPole` (interface, line 43) — read by 1: `src/editor/chunkedSave.ts#KEYED`
- `src/world/utilities.ts#UtilityPole` (interface, line 43) — typeRef by 1: `src/editor/chunkedSave.ts#KEYED`
- `src/world/utilities.ts#UtilitySpan` (interface, line 51) — read by 1: `src/editor/chunkedSave.ts#KEYED`
- `src/world/utilities.ts#UtilitySpan` (interface, line 51) — typeRef by 1: `src/editor/chunkedSave.ts#KEYED`

### src/world/weather.ts

- `src/world/weather.ts#DEFAULT_WEATHER` (variable, line 22) — read by 1: `src/world/doc.ts#RoadDoc.savedGlobals`

## External APIs

- `es:Math` × 433
- `dom:document` × 204
- `dom:ParentNode.append` × 159
- `dom:Document.createElement` × 144
- `dom:Node.appendChild` × 138
- `es:Array.push` × 138
- `dom:Element.textContent` × 135
- `es:Math.max` × 116
- `dom:HTMLOrSVGElement.dataset` × 113
- `dom:Element.setAttribute` × 101
- `dom:Element.className` × 93
- `es:Array.length` × 91
- `dom:HTMLElement` × 86
- `es:Promise` × 83
- `es:Math.min` × 82
- `es:String` × 78
- `es:Math.round` × 74
- `es:Record` × 74
- `es:ReadonlyArray.map` × 70
- `es:ReadonlyArray.length` × 67
- `es:Map.get` × 56
- `es:Number` × 56
- `dom:HTMLElement.hidden` × 51
- `es:Set` × 51
- `dom:GlobalEventHandlers.onclick` × 50
- `dom:window` × 49
- `es:Array.map` × 47
- `es:Map` × 47
- `three:Object3D.position` × 47
- `es:Uint8Array` × 45
- `dom:HTMLButtonElement.type` × 43
- `es:Map.set` × 43
- `es:Set.add` × 40
- `es:JSON` × 39
- `dom:HTMLButtonElement` × 38
- `dom:KeyboardEvent.key` × 38
- `dom:ElementCSSInlineStyle.style` × 37
- `dom:ParentNode.querySelector` × 37
- `dom:Element.innerHTML` × 36
- `es:Array` × 34
- `es:Math.floor` × 34
- `dom:CanvasFillStrokeStyles.fillStyle` × 32
- `es:Math.abs` × 32
- `three:Vector3.x` × 32
- `dom:Element.classList` × 31
- `dom:HTMLElement.title` × 31
- `three:Vector3.z` × 31
- `dom:Document.getElementById` × 30
- `three:Vector3.y` × 30
- `dom:localStorage` × 29
- `es:Array.join` × 29
- `es:ArrayConstructor.isArray` × 29
- `es:Math.PI` × 27
- `es:Readonly` × 27
- `dom:CanvasRect.fillRect` × 26
- `dom:Blob` × 25
- `es:Set.has` × 25
- `dom:performance` × 22
- `es:Partial` × 22
- `es:String.slice` × 22
- `es:Array.filter` × 21
- `es:Number.toFixed` × 21
- `es:Object` × 21
- `dom:console` × 20
- `dom:HTMLInputElement.value` × 20
- `es:Infinity` × 20
- `es:JSON.stringify` × 20
- `dom:ParentNode.querySelectorAll` × 19
- `es:JSON.parse` × 19
- `es:ReadonlyArray.includes` × 19
- `es:ReadonlyArray.filter` × 18
- `dom:Window.addEventListener` × 17
- `es:Uint8Array.byteLength` × 17
- `dom:Performance.now` × 16
- `dom:Storage.setItem` × 16
- `es:NumberConstructor.isFinite` × 16
- `dom:HTMLDivElement.addEventListener` × 15
- `es:Promise.then` × 15
- `es:ReadonlyArray.some` × 15
- `es:String.length` × 15
- `dom:Console.error` × 14
- `dom:GlobalEventHandlers.onchange` × 14
- `es:ArrayBuffer` × 14
- `es:String.replace` × 14
- `es:String.startsWith` × 14
- `three:IUniform.value` × 14
- `dom:DOMTokenList.add` × 13
- `dom:Element.getBoundingClientRect` × 13
- `dom:HTMLButtonElement.disabled` × 13
- `dom:ParentNode.replaceChildren` × 13
- `dom:Storage.getItem` × 13
- `es:Error` × 13
- `es:ReadonlyArray.find` × 13
- `dom:CanvasPath.lineTo` × 12
- `dom:Event.preventDefault` × 12
- `dom:Event.stopPropagation` × 12
- `es:ArrayBufferLike` × 12
- `es:Map.values` × 12
- `es:Math.random` × 12
- `three:Texture` × 12
- `dom:CanvasDrawPath.beginPath` × 11
- `dom:CanvasGradient.addColorStop` × 11
- `dom:HTMLDivElement` × 11
- `dom:HTMLOrSVGElement.focus` × 11
- `dom:PointerEvent.pointerId` × 11
- `dom:setTimeout` × 11
- `es:Float64Array` × 11
- `dom:Event.target` × 10
- `dom:HTMLButtonElement.addEventListener` × 10
- `dom:IDBRequest.result` × 10
- `es:Array.sort` × 10
- `es:Map.delete` × 10
- `es:Map.size` × 10
- `es:Math.exp` × 10
- `es:Math.hypot` × 10
- `es:String.split` × 10
- `es:WeakMap.get` × 10
- `dom:Document.body` × 9
- `dom:Element.clientWidth` × 9
- `dom:HTMLSelectElement.value` × 9
- `dom:KeyboardEvent` × 9
- `dom:MessageEvent.data` × 9
- `dom:MessagePort` × 9
- `dom:Node.contains` × 9
- `es:Date` × 9
- `es:DateConstructor.now` × 9
- `es:ReadonlyArray.forEach` × 9
- `es:ReadonlyMap.get` × 9
- `es:Set.size` × 9
- `es:String.padStart` × 9
- `es:Uint8Array.buffer` × 9
- `three:Matrix4` × 9
- `three:Vector3` × 9
- `dom:Blob.arrayBuffer` × 8
- `dom:CanvasDrawPath.fill` × 8
- `dom:DOMRectReadOnly.top` × 8
- `dom:DOMTokenList.toggle` × 8
- `dom:HTMLInputElement.type` × 8
- `dom:PerformanceEntry.duration` × 8
- `dom:PerformanceEntry.startTime` × 8
- `dom:Window.innerHeight` × 8
- `es:Math.atan2` × 8
- `es:Math.sin` × 8
- `es:String.trim` × 8
- `three:Material` × 8
- `dom:Blob.text` × 7
- `dom:CanvasFillStrokeStyles.strokeStyle` × 7
- `dom:CanvasPath.moveTo` × 7
- `dom:clearTimeout` × 7
- `dom:CSSStyleDeclaration.left` × 7
- `dom:DOMRect.width` × 7
- `dom:Element.getAttribute` × 7
- `dom:Element.scrollWidth` × 7
- `dom:HTMLInputElement` × 7
- `dom:MouseEvent.clientX` × 7
- `dom:navigator` × 7
- `dom:Window.innerWidth` × 7
- `es:Array.slice` × 7
- `es:DataView.getUint32` × 7
- `es:Map.clear` × 7
- `es:Map.has` × 7
- `es:Math.cos` × 7
- `es:ReadonlyArray.slice` × 7
- `es:ReadonlyMap` × 7
- `es:Uint8Array.slice` × 7
- `es:WeakMap.set` × 7
- `dom:ChildNode.remove` × 6
- `dom:DecompressionStream` × 6
- `dom:DOMRectReadOnly.bottom` × 6
- `dom:DOMTokenList.remove` × 6
- `dom:Element.closest` × 6
- `dom:Element.id` × 6
- `dom:Element.scrollLeft` × 6
- `dom:GlobalEventHandlers.oninput` × 6
- `dom:HTMLSelectElement` × 6
- `dom:ParentNode.childElementCount` × 6
- `dom:Response` × 6
- `es:Array.find` × 6
- `es:DataView.setUint32` × 6
- `es:Float32Array` × 6
- `es:ObjectConstructor.keys` × 6
- `es:PromiseConstructor.all` × 6
- `es:Set.delete` × 6
- `three:DirectionalLight.shadow` × 6
- `dom:CanvasDrawPath.stroke` × 5
- `dom:CharacterData.data` × 5
- `dom:CSSStyleDeclaration.bottom` × 5
- `dom:CSSStyleDeclaration.cssText` × 5
- `dom:CSSStyleDeclaration.top` × 5
- `dom:DocumentOrShadowRoot.activeElement` × 5
- `dom:DOMRectReadOnly.left` × 5
- `dom:Element` × 5
- `dom:ErrorEvent.message` × 5
- `dom:GainNode.gain` × 5
- `dom:KeyboardEvent.code` × 5
- `dom:Node.isConnected` × 5
- `dom:NodeListOf.forEach` × 5
- `dom:Performance.mark` × 5
- `dom:PerformanceObserver` × 5
- `dom:ReadableStream.pipeThrough` × 5
- `dom:TextDecoder.decode` × 5
- `dom:Worker` × 5
- `es:Array.every` × 5
- `es:Array.includes` × 5
- `es:Array.shift` × 5
- `es:ArrayConstructor.from` × 5
- `es:Intl` × 5
- `es:Math.ceil` × 5
- `es:Number.toString` × 5
- `es:ObjectConstructor.fromEntries` × 5
- `es:PromiseConstructor.resolve` × 5
- `es:ReadonlyArray.reduce` × 5
- `es:Uint8Array.fill` × 5
- `three:Raycaster.ray` × 5
- `three:Vector3.distanceTo` × 5
- `dom:Blob.stream` × 4
- `dom:CanvasFillStrokeStyles.createLinearGradient` × 4
- `dom:CanvasPath.closePath` × 4
- `dom:CompressionStream` × 4
- `dom:Console.warn` × 4
- `dom:DOMRectReadOnly.right` × 4
- `dom:DOMTokenList.contains` × 4
- `dom:fetch` × 4
- `dom:HTMLElement.offsetWidth` × 4
- `dom:IDBDatabase` × 4
- `dom:PerformanceObserverEntryList.getEntries` × 4
- `dom:requestAnimationFrame` × 4
- `dom:TextDecoder` × 4
- `dom:TextEncoder` × 4
- `dom:TextEncoder.encode` × 4
- `dom:WindowLocalStorage.localStorage` × 4
- `es:Array.pop` × 4
- `es:Array.reduce` × 4
- `es:Array.some` × 4
- `es:Array.splice` × 4
- `es:ArrayBuffer.byteLength` × 4
- `es:Intl.NumberFormat` × 4
- `es:Map.keys` × 4
- `es:NaN` × 4
- `es:NumberConstructor.isInteger` × 4
- `es:Parameters` × 4
- `es:Promise.catch` × 4
- `es:ReadonlyArray.indexOf` × 4
- `es:ReadonlyMap.values` × 4
- `es:ReadonlySet` × 4
- `es:ReturnType` × 4
- `es:String.toLocaleLowerCase` × 4
- `es:Uint8Array.byteOffset` × 4
- `es:WeakMap` × 4
- `three:MaterialProperties.precision` × 4
- `three:Mesh.material` × 4
- `three:Object3D` × 4
- `three:Object3D.visible` × 4
- `three:PerspectiveCamera.near` × 4
- `three:Plane.constant` × 4
- `three:Vector2` × 4
- `three:Vector2.set` × 4
- `three:Vector3.lerp` × 4
- `three:Vector3.set` × 4
- `dom:AudioNode.connect` × 3
- `dom:AudioParam.setTargetAtTime` × 3
- `dom:CanvasPath.arc` × 3
- `dom:CanvasPathDrawingStyles.lineWidth` × 3
- `dom:CanvasTransform.scale` × 3
- `dom:CSSStyleDeclaration.display` × 3
- `dom:Document.createTextNode` × 3
- `dom:Document.documentElement` × 3
- `dom:GlobalEventHandlers.onkeydown` × 3
- `dom:HTMLCanvasElement.getContext` × 3
- `dom:HTMLCanvasElement.height` × 3
- `dom:HTMLCanvasElement.toDataURL` × 3
- `dom:HTMLCanvasElement.width` × 3
- `dom:HTMLInputElement.placeholder` × 3
- `dom:HTMLOutputElement.value` × 3
- `dom:IDBDatabase.transaction` × 3
- `dom:IDBObjectStore` × 3
- `dom:IDBObjectStore.get` × 3
- `dom:IDBObjectStore.put` × 3
- `dom:IDBRequest` × 3
- `dom:IDBTransaction.objectStore` × 3
- `dom:indexedDB` × 3
- `dom:KeyboardEvent.shiftKey` × 3
- `dom:MessageEvent` × 3
- `dom:MessageEvent.source` × 3
- `dom:MouseEvent.clientY` × 3
- `dom:Navigator.clipboard` × 3
- `dom:Node` × 3
- `dom:ParentNode.children` × 3
- `dom:PerformanceEntry.name` × 3
- `dom:PerformanceObserver.observe` × 3
- `dom:setInterval` × 3
- `dom:structuredClone` × 3
- `es:Boolean` × 3
- `es:DataView` × 3
- `es:Error.stack` × 3
- `es:Float64Array.length` × 3
- `es:Math.log` × 3
- `es:Math.tan` × 3
- `es:ObjectConstructor.freeze` × 3
- `es:ObjectConstructor.hasOwn` × 3
- `es:ReadonlySet.has` × 3
- `es:RegExp.exec` × 3
- `es:String.indexOf` × 3
- `es:String.toLowerCase` × 3
- `es:Uint32Array` × 3
- `three:LightShadow.map` × 3
- `three:Matrix4.copy` × 3
- `three:Matrix4.elements` × 3
- `three:Mesh` × 3
- `three:NearestFilter` × 3
- `three:Object3D.frustumCulled` × 3
- `three:Object3D.matrixWorld` × 3
- `three:OrthographicCamera` × 3
- `three:PerspectiveCamera` × 3
- `three:Raycaster.setFromCamera` × 3
- `three:ShaderPass` × 3
- `three:Texture.anisotropy` × 3
- `three:Texture.needsUpdate` × 3
- `dom:AbortController` × 2
- `dom:AudioBuffer` × 2
- `dom:AudioContext` × 2
- `dom:BaseAudioContext.sampleRate` × 2
- `dom:Body.arrayBuffer` × 2
- `dom:Body.blob` × 2
- `dom:Body.body` × 2
- `dom:Body.json` × 2
- `dom:Clipboard.writeText` × 2
- `dom:CSSStyleDeclaration.background` × 2
- `dom:CSSStyleDeclaration.minWidth` × 2
- `dom:CSSStyleDeclaration.right` × 2
- `dom:CSSStyleDeclaration.setProperty` × 2
- `dom:Document.addEventListener` × 2
- `dom:Element.removeAttribute` × 2
- `dom:Element.scrollIntoView` × 2
- `dom:Element.scrollTop` × 2
- `dom:Element.setPointerCapture` × 2
- `dom:ErrorEvent.colno` × 2
- `dom:ErrorEvent.filename` × 2
- `dom:ErrorEvent.lineno` × 2
- `dom:FocusEvent.relatedTarget` × 2
- `dom:HTMLCanvasElement.addEventListener` × 2
- `dom:HTMLDetailsElement.open` × 2
- `dom:HTMLDialogElement.open` × 2
- `dom:HTMLDialogElement.showModal` × 2
- `dom:HTMLElement.addEventListener` × 2
- `dom:HTMLElement.click` × 2
- `dom:HTMLElement.lang` × 2
- `dom:HTMLElementTagNameMap` × 2
- `dom:HTMLInputElement.checked` × 2
- `dom:HTMLInputElement.files` × 2
- `dom:HTMLInputElement.max` × 2
- `dom:HTMLInputElement.maxLength` × 2
- `dom:HTMLInputElement.min` × 2
- `dom:HTMLInputElement.step` × 2
- `dom:HTMLOutputElement` × 2
- `dom:IDBObjectStore.delete` × 2
- `dom:IDBObjectStore.getAllKeys` × 2
- `dom:IDBRequest.onerror` × 2
- `dom:IDBRequest.onsuccess` × 2
- `dom:IDBTransaction.onabort` × 2
- `dom:IDBTransaction.oncomplete` × 2
- `dom:IDBTransaction.onerror` × 2
- `dom:IDBTransactionMode` × 2
- `dom:MessageEventTarget.onmessage` × 2
- `dom:MessagePort.postMessage` × 2
- `dom:MouseEvent.button` × 2
- `dom:MutationObserver` × 2
- `dom:MutationObserver.observe` × 2
- `dom:NavigatorLanguage.languages` × 2
- `dom:PerformanceObserver.supportedEntryTypes` × 2
- `dom:PointerEvent` × 2
- `dom:ResizeObserver` × 2
- `dom:ResizeObserver.observe` × 2
- `dom:Response.ok` × 2
- `dom:Storage.removeItem` × 2
- `dom:WheelEvent.deltaY` × 2
- `dom:Window.removeEventListener` × 2
- `dom:WindowOrWorkerGlobalScope.clearInterval` × 2
- `dom:WindowOrWorkerGlobalScope.setInterval` × 2
- `dom:Worker.addEventListener` × 2
- `es:Array.flatMap` × 2
- `es:Array.forEach` × 2
- `es:Array.indexOf` × 2
- `es:CallableFunction.bind` × 2
- `es:Error.message` × 2
- `es:Error.name` × 2
- `es:Float32Array.set` × 2
- `es:Float32Array.sort` × 2
- `es:Iterable` × 2
- `es:NumberConstructor.NaN` × 2
- `es:NumberFormat.format` × 2
- `es:ObjectConstructor.entries` × 2
- `es:parseInt` × 2
- `es:ReadonlyArray.every` × 2
- `es:ReadonlyArray.findIndex` × 2
- `es:ReadonlyArray.flatMap` × 2
- `es:Set.clear` × 2
- `es:String.includes` × 2
- `es:Uint8Array.set` × 2
- `three:Camera` × 2
- `three:Camera.matrixWorldInverse` × 2
- `three:Camera.projectionMatrix` × 2
- `three:Material.dispose` × 2
- `three:Material.needsUpdate` × 2
- `three:Mesh.isMesh` × 2
- `three:Object3D.traverse` × 2
- `three:Object3D.up` × 2
- `three:Object3D.updateMatrixWorld` × 2
- `three:Pass.render` × 2
- `three:PerspectiveCamera.fov` × 2
- `three:Ray.direction` × 2
- `three:Ray.origin` × 2
- `three:Texture.clearUpdateRanges` × 2
- `three:Texture.minFilter` × 2
- `three:Vector4.set` × 2
- `three:WebGLRenderer.extensions` × 2
- `dom:AbortController.abort` × 1
- `dom:AbortController.signal` × 1
- `dom:AbstractWorker.onerror` × 1
- `dom:AudioBuffer.getChannelData` × 1
- `dom:AudioBufferSourceNode.buffer` × 1
- `dom:AudioBufferSourceNode.start` × 1
- `dom:AudioContext.resume` × 1
- `dom:AudioParam.exponentialRampToValueAtTime` × 1
- `dom:AudioParam.setValueAtTime` × 1
- `dom:AudioParam.value` × 1
- `dom:BaseAudioContext.createBiquadFilter` × 1
- `dom:BaseAudioContext.createBuffer` × 1
- `dom:BaseAudioContext.createBufferSource` × 1
- `dom:BaseAudioContext.createGain` × 1
- `dom:BaseAudioContext.currentTime` × 1
- `dom:BaseAudioContext.destination` × 1
- `dom:BaseAudioContext.state` × 1
- `dom:BiquadFilterNode.frequency` × 1
- `dom:BiquadFilterNode.type` × 1
- `dom:Body.text` × 1
- `dom:CanvasPath.ellipse` × 1
- `dom:CanvasRect.strokeRect` × 1
- `dom:CanvasRenderingContext2D` × 1
- `dom:Console.log` × 1
- `dom:CSSStyleDeclaration.gap` × 1
- `dom:CSSStyleDeclaration.maxWidth` × 1
- `dom:CSSStyleDeclaration.transform` × 1
- `dom:CSSStyleDeclaration.width` × 1
- `dom:Document.head` × 1
- `dom:Document.hidden` × 1
- `dom:Document.title` × 1
- `dom:DOMMatrix2DInit` × 1
- `dom:DOMRect.height` × 1
- `dom:Element.clientHeight` × 1
- `dom:Element.hasPointerCapture` × 1
- `dom:Element.insertAdjacentHTML` × 1
- `dom:Element.releasePointerCapture` × 1
- `dom:Element.scrollHeight` × 1
- `dom:ErrorEvent.error` × 1
- `dom:Event.type` × 1
- `dom:FileList.length` × 1
- `dom:GlobalEventHandlers.oncancel` × 1
- `dom:GlobalEventHandlers.ontoggle` × 1
- `dom:HTMLCanvasElement` × 1
- `dom:HTMLDialogElement` × 1
- `dom:HTMLElement.innerText` × 1
- `dom:HTMLElement.offsetHeight` × 1
- `dom:HTMLFormElement.method` × 1
- `dom:HTMLImageElement` × 1
- `dom:HTMLImageElement.src` × 1
- `dom:HTMLInputElement.accept` × 1
- `dom:HTMLInputElement.addEventListener` × 1
- `dom:HTMLInputElement.disabled` × 1
- `dom:HTMLInputElement.setSelectionRange` × 1
- `dom:HTMLOptionElement.value` × 1
- `dom:HTMLOrSVGElement.tabIndex` × 1
- `dom:HTMLTitleElement` × 1
- `dom:IDBDatabase.createObjectStore` × 1
- `dom:IDBFactory.open` × 1
- `dom:IDBKeyRange` × 1
- `dom:IDBKeyRange.bound` × 1
- `dom:IDBObjectStore.getAll` × 1
- `dom:IDBOpenDBRequest.onblocked` × 1
- `dom:IDBOpenDBRequest.onupgradeneeded` × 1
- `dom:KeyboardEvent.repeat` × 1
- `dom:MessageEvent.ports` × 1
- `dom:MessageEventTarget.onmessageerror` × 1
- `dom:MouseEvent.relatedTarget` × 1
- `dom:NavigatorLanguage.language` × 1
- `dom:Node.insertBefore` × 1
- `dom:Node.parentElement` × 1
- `dom:NodeList.length` × 1
- `dom:ParentNode` × 1
- `dom:Performance.clearMeasures` × 1
- `dom:PerformanceEntry` × 1
- `dom:PromiseRejectionEvent.reason` × 1
- `dom:queueMicrotask` × 1
- `dom:ResizeObserver.disconnect` × 1
- `dom:SVGElement` × 1
- `dom:SVGGElement` × 1
- `dom:URL` × 1
- `dom:WebGL2RenderingContext` × 1
- `dom:WebGL2RenderingContextBase.TEXTURE_2D_ARRAY` × 1
- `dom:WebGL2RenderingContextBase.TEXTURE_3D` × 1
- `dom:WebGLRenderingContextBase.texParameterf` × 1
- `dom:WebGLRenderingContextBase.TEXTURE_2D` × 1
- `dom:WebGLRenderingContextBase.TEXTURE_CUBE_MAP` × 1
- `dom:WebGLTexture` × 1
- `dom:WheelEvent.deltaX` × 1
- `dom:Worker.postMessage` × 1
- `dom:WorkerOptions` × 1
- `es:Array.fill` × 1
- `es:Array.reverse` × 1
- `es:Array.unshift` × 1
- `es:CallableFunction.call` × 1
- `es:DateTimeFormat.format` × 1
- `es:Extract` × 1
- `es:Float32Array.fill` × 1
- `es:Float64Array.buffer` × 1
- `es:Float64Array.byteLength` × 1
- `es:Float64ArrayConstructor.from` × 1
- `es:Intl.DateTimeFormat` × 1
- `es:Map.entries` × 1
- `es:Math.log10` × 1
- `es:Math.sqrt` × 1
- `es:Number.toPrecision` × 1
- `es:NumberConstructor.isNaN` × 1
- `es:NumberConstructor.isSafeInteger` × 1
- `es:Object.hasOwnProperty` × 1
- `es:ObjectConstructor.assign` × 1
- `es:ObjectConstructor.prototype` × 1
- `es:Pick` × 1
- `es:RegExp.test` × 1
- `es:String.endsWith` × 1
- `es:String.padEnd` × 1
- `es:Uint32Array.fill` × 1
- `es:Uint8Array.subarray` × 1
- `three:BasicShadowMap` × 1
- `three:Camera.projectionMatrixInverse` × 1
- `three:DataTexture` × 1
- `three:DataTexture.generateMipmaps` × 1
- `three:DataTexture.magFilter` × 1
- `three:DataTexture.minFilter` × 1
- `three:DirectionalLight` × 1
- `three:LightShadow.bias` × 1
- `three:LightShadow.matrix` × 1
- `three:LightShadow.normalBias` × 1
- `three:LinearMipmapLinearFilter` × 1
- `three:Matrix4.multiplyMatrices` × 1
- `three:MeshLambertMaterial` × 1
- `three:NearestMipmapLinearFilter` × 1
- `three:Object3D.castShadow` × 1
- `three:Object3D.lookAt` × 1
- `three:OrthographicCamera.bottom` × 1
- `three:OrthographicCamera.far` × 1
- `three:OrthographicCamera.left` × 1
- `three:OrthographicCamera.right` × 1
- `three:OrthographicCamera.top` × 1
- `three:Pass.dispose` × 1
- `three:PCFShadowMap` × 1
- `three:PerspectiveCamera.aspect` × 1
- `three:PerspectiveCamera.far` × 1
- `three:PerspectiveCamera.updateProjectionMatrix` × 1
- `three:Plane` × 1
- `three:Ray.distanceToPlane` × 1
- `three:Ray.intersectPlane` × 1
- `three:Raycaster` × 1
- `three:RenderTarget.depthTexture` × 1
- `three:RenderTarget.dispose` × 1
- `three:RGBAFormat` × 1
- `three:Scene` × 1
- `three:ShaderMaterial` × 1
- `three:ShaderMaterialProperties.uniforms` × 1
- `three:Texture.addUpdateRange` × 1
- `three:Texture.dispose` × 1
- `three:Texture.magFilter` × 1
- `three:UnsignedByteType` × 1
- `three:Vector3.copy` × 1
- `three:Vector3.normalize` × 1
- `three:Vector3.project` × 1
- `three:Vector3.setFromMatrixPosition` × 1
- `three:Vector4` × 1
- `three:WebGLCapabilities.getMaxAnisotropy` × 1
- `three:WebGLExtensions.get` × 1
- `three:WebGLExtensions.has` × 1
- `three:WebGLProperties.get` × 1
- `three:WebGLRenderer` × 1
- `three:WebGLRenderer.capabilities` × 1
- `three:WebGLRenderer.getContext` × 1
- `three:WebGLRenderer.properties` × 1
- `three:WebGLRenderer.setOpaqueSort` × 1
- `three:WebGLRenderer.shadowMap` × 1
- `three:WebGLRenderer.state` × 1
- `three:WebGLShadowMap.type` × 1
- `three:WebGLState.bindTexture` × 1
- `vite:ImportMeta.env` × 1
- `vite:ImportMetaEnv.DEV` × 1
