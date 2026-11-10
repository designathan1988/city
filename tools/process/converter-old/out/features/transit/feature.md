# Feature: src/editor/transitTools.ts, src/render/transit.ts, src/sim/transit/transit.ts, src/ui/v2/transitLine.ts, src/world/transit.ts

The program starts from `src/main.ts`, `src/peopleLab/main.ts`. A declaration is the feature's own when the program reaches it only through the feature; in a declaration of the program that names the feature, its statements are a slice (what names it, what depends on that, what that controls).

- Own: 197 declarations
- Slices of shared code: 35
- Code: 2103 lines in 19 files (source/)
- Resources: 42 (33 entry, 9 style, 0 import; 35 only the feature uses)
- Uses: 123 shared declarations (interface/: 634 declarations in 165 files, the types they name included); hooks: 52; external APIs: 141

## Code

- src/editor/transitTools.ts — whole, 250 lines
- src/lab/catalog/layers.ts — part, 23 lines
- src/main.ts — part, 42 lines
- src/render/renderer.ts — part, 86 lines
- src/render/transit.ts — whole, 569 lines
- src/sim/agents/laneGrid.ts — part, 5 lines
- src/sim/agents/trafficBuildings.ts — part, 7 lines
- src/sim/agents/walk.ts — part, 8 lines
- src/sim/city/city.ts — part, 4 lines
- src/sim/host/simHost.ts — part, 5 lines
- src/sim/intersections/admission.ts — part, 9 lines
- src/sim/transit/transit.ts — whole, 600 lines
- src/sim/vehicles/obstacles.ts — part, 17 lines
- src/sim/world.ts — part, 6 lines
- src/ui/v2/shell.ts — part, 14 lines
- src/ui/v2/transitLine.ts — whole, 17 lines
- src/world/doc.ts — part, 41 lines
- src/world/footwayPath.ts — part, 31 lines
- src/world/transit.ts — whole, 369 lines

## Slices of shared code

- `src/lab/catalog/layers.ts#transitItems` — 16 of 25 lines
- `src/lab/catalog/layers.ts#transitItems.lay` — 7 of 8 lines
- `src/main.ts#<ExpressionStatement>@861` — 1 of 1 lines
- `src/main.ts#<IfStatement>@4036` — 4 of 196 lines
- `src/main.ts#doubleClick` — 3 of 6 lines
- `src/main.ts#drawOverlayScreen` — 4 of 469 lines
- `src/main.ts#gameKeys` — 3 of 77 lines
- `src/main.ts#mountInterface` — 5 of 88 lines
- `src/main.ts#pointerDown` — 7 of 180 lines
- `src/main.ts#pointerMove` — 5 of 72 lines
- `src/main.ts#simShowsMotion` — 3 of 3 lines
- `src/main.ts#transitEditor` — 7 of 7 lines
- `src/render/renderer.ts#createSceneRenderer` — 59 of 3774 lines
- `src/render/renderer.ts#createSceneRenderer.movingCasters` — 1 of 1 lines
- `src/render/renderer.ts#createSceneRenderer.switchedOff` — 16 of 16 lines
- `src/render/renderer.ts#createSceneRenderer.warmPreviewShaders` — 10 of 38 lines
- `src/sim/agents/trafficBuildings.ts#trafficMapOf` — 7 of 14 lines
- `src/sim/agents/walk.ts#stepWalkers` — 8 of 343 lines
- `src/sim/city/city.ts#City.step` — 3 of 6 lines
- `src/sim/city/city.ts#City.transit` — 1 of 1 lines
- `src/sim/host/simHost.ts#SimHost.publish` — 5 of 80 lines
- `src/sim/intersections/admission.ts#admit` — 9 of 117 lines
- `src/sim/vehicles/obstacles.ts#longitudinalConstraints` — 17 of 104 lines
- `src/ui/v2/shell.ts#ShellTransitTool` — 3 of 9 lines
- `src/ui/v2/shell.ts#mountShell.renderTransit` — 11 of 36 lines
- `src/world/doc.ts#RoadDoc.fromJSON` — 3 of 187 lines
- `src/world/doc.ts#RoadDoc.layOver` — 3 of 47 lines
- `src/world/doc.ts#RoadDoc.replaceContents` — 7 of 149 lines
- `src/world/doc.ts#RoadDoc.restoreValue` — 5 of 27 lines
- `src/world/doc.ts#RoadDoc.savedGlobals` — 5 of 18 lines
- `src/world/doc.ts#RoadDoc.setTransit` — 3 of 3 lines
- `src/world/doc.ts#RoadDoc.takeFork` — 3 of 49 lines
- `src/world/doc.ts#RoadDoc.transit` — 1 of 1 lines
- `src/world/doc.ts#RoadDoc.valueOf` — 5 of 15 lines
- `src/world/doc.ts#RoadDoc.worldPart` — 5 of 26 lines

## Resources

- feature entry `transit.mode.bus` — src/ui/i18n/en.ts:1839: `'transit.mode.bus': 'Bus'`
- feature entry `transit.mode.train` — src/ui/i18n/en.ts:1840: `'transit.mode.train': 'Train'`
- feature entry `transit.mode.metro` — src/ui/i18n/en.ts:1841: `'transit.mode.metro': 'Metro'`
- feature entry `transit.stops` — src/ui/i18n/en.ts:1842: `'transit.stops': '{n} stops'`
- feature entry `transit.fewer` — src/ui/i18n/en.ts:1843: `'transit.fewer': 'Fewer vehicles'`
- feature entry `transit.more` — src/ui/i18n/en.ts:1844: `'transit.more': 'More vehicles'`
- feature entry `transit.remove` — src/ui/i18n/en.ts:1846: `'transit.remove': 'Delete the line'`
- feature entry `hint.transit.removed` — src/ui/i18n/en.ts:1852: `'hint.transit.removed': 'Removed.'`
- feature entry `hint.transit.noStreet` — src/ui/i18n/en.ts:1853: `'hint.transit.noStreet': 'Put the stop near a street with a footway.'`
- feature entry `hint.transit.taken` — src/ui/i18n/en.ts:1854: `'hint.transit.taken': 'There is a stop here already.'`
- feature entry `hint.transit.noStop` — src/ui/i18n/en.ts:1855: `'hint.transit.noStop': 'Click a stop or a station.'`
- feature entry `hint.transit.noTrack` — src/ui/i18n/en.ts:1856: `'hint.transit.noTrack': 'Click on a track.'`
- feature entry `hint.transit.trackShort` — src/ui/i18n/en.ts:1857: `'hint.transit.trackShort': 'A track needs at least two points.'`
- feature entry `hint.transit.trackOnRoad` — src/ui/i18n/en.ts:1858: `'hint.transit.trackOnRoad': 'A train track crosses streets but does not run down them (use the metro under them).'`
- feature entry `hint.transit.lineShort` — src/ui/i18n/en.ts:1859: `'hint.transit.lineShort': 'A line needs at least two stops.'`
- feature entry `hint.transit.lineMade` — src/ui/i18n/en.ts:1860: `'hint.transit.lineMade': 'Line made.'`
- feature entry `transit.mode.bus` — src/ui/i18n/pt-BR.ts:1829: `'transit.mode.bus': 'Ônibus'`
- feature entry `transit.mode.train` — src/ui/i18n/pt-BR.ts:1830: `'transit.mode.train': 'Trem'`
- feature entry `transit.mode.metro` — src/ui/i18n/pt-BR.ts:1831: `'transit.mode.metro': 'Metrô'`
- feature entry `transit.stops` — src/ui/i18n/pt-BR.ts:1832: `'transit.stops': '{n} paradas'`
- feature entry `transit.fewer` — src/ui/i18n/pt-BR.ts:1833: `'transit.fewer': 'Menos veículos'`
- feature entry `transit.more` — src/ui/i18n/pt-BR.ts:1834: `'transit.more': 'Mais veículos'`
- feature entry `transit.remove` — src/ui/i18n/pt-BR.ts:1836: `'transit.remove': 'Apagar a linha'`
- feature entry `hint.transit.removed` — src/ui/i18n/pt-BR.ts:1842: `'hint.transit.removed': 'Removido.'`
- feature entry `hint.transit.noStreet` — src/ui/i18n/pt-BR.ts:1843: `'hint.transit.noStreet': 'Ponha o ponto perto de uma rua com calçada.'`
- feature entry `hint.transit.taken` — src/ui/i18n/pt-BR.ts:1844: `'hint.transit.taken': 'Já há uma parada aqui.'`
- feature entry `hint.transit.noStop` — src/ui/i18n/pt-BR.ts:1845: `'hint.transit.noStop': 'Clique numa parada ou estação.'`
- feature entry `hint.transit.noTrack` — src/ui/i18n/pt-BR.ts:1846: `'hint.transit.noTrack': 'Clique sobre um trilho.'`
- feature entry `hint.transit.trackShort` — src/ui/i18n/pt-BR.ts:1847: `'hint.transit.trackShort': 'Um trilho precisa de pelo menos dois pontos.'`
- feature entry `hint.transit.trackOnRoad` — src/ui/i18n/pt-BR.ts:1848: `'hint.transit.trackOnRoad': 'Trilho de trem cruza ruas, mas não corre por cima delas (use o metrô por baixo).'`
- feature entry `hint.transit.lineShort` — src/ui/i18n/pt-BR.ts:1849: `'hint.transit.lineShort': 'Uma linha precisa de pelo menos duas paradas.'`
- feature entry `hint.transit.lineMade` — src/ui/i18n/pt-BR.ts:1850: `'hint.transit.lineMade': 'Linha criada.'`
- shared entry `demolish` — src/ui/v2/shell.ts:79: `demolish: '<path d="m5 9 8-5 5 8-8 5Z"/><path d="m7 15 5 5m4-8 3 5"/>'`
- shared style `glass` — src/ui/app.css:57: `.glass { background: linear-gradient(145deg, var(--ui-hi), transparent 42%), var(--ui-bg); border: 1px solid var(--ui-border); border-radius: 14px; box-shadow: `
- shared style `v2-icon` — src/ui/v2/shell.css:41: `.v2-icon > span, .v2-pill > span, .v2-seg-b > span, .v2-tabs.compact .v2-tab.icon:not(.on) > span, .v2-tabs.tight .v2-tab.icon > span, .v2-choice.icon > span, .`
- shared style `v2-icon` — src/ui/v2/shell.css:69: `.v2-pill, .v2-icon { position: relative; display: inline-flex; align-items: center; justify-content: center; gap: 6px; width: 32px; height: 32px; padding: 0; fl`
- shared style `v2-icon` — src/ui/v2/shell.css:74: `.v2-pill:hover, .v2-icon:hover:not(:disabled) { background: var(--v-raise-2); border-color: var(--v-accent-line); }`
- shared style `v2-icon` — src/ui/v2/shell.css:75: `.v2-icon.on, .v2-pill.on { color: var(--v-accent); border-color: var(--v-accent); background: var(--v-accent-soft); }`
- shared style `v2-icon` — src/ui/v2/shell.css:76: `.v2-icon.danger { color: var(--v-danger-ink); }`
- feature style `v2-transit-line` — src/ui/v2/shell.css:272: `.v2-transit-line { display: flex; align-items: center; gap: 6px; min-width: 260px; padding: 4px 8px; border-radius: 8px; background: var(--v-raise); }`
- feature style `v2-transit-line` — src/ui/v2/shell.css:273: `.v2-transit-line > i { flex: none; width: 12px; height: 12px; border-radius: 3px; }`
- feature style `v2-transit-line` — src/ui/v2/shell.css:274: `.v2-transit-line > b { font-weight: 700; }`

## Where the program plugs the feature in

### src/editor/transitTools.ts

- `src/editor/transitTools.ts#setTransitTool` (function, line 249) ← call from `src/main.ts#<ExpressionStatement>@861`
- `src/editor/transitTools.ts#transitTool` (function, line 248) ← reference from `src/main.ts#mountInterface`
- `src/editor/transitTools.ts#TransitTool` (class, line 32) ← construct from `src/main.ts#transitEditor`
- `src/editor/transitTools.ts#TransitTool.click` (method, line 86) ← call from `src/main.ts#pointerDown`
- `src/editor/transitTools.ts#TransitTool.draw` (method, line 188) ← call from `src/main.ts#drawOverlayScreen`
- `src/editor/transitTools.ts#TransitTool.key` (method, line 142) ← call from `src/main.ts#doubleClick`, `src/main.ts#gameKeys`
- `src/editor/transitTools.ts#TransitTool.move` (method, line 139) ← call from `src/main.ts#pointerMove`
- `src/editor/transitTools.ts#TransitTool.preview` (method, line 176) ← call from `src/main.ts#drawOverlayScreen`

### src/render/transit.ts

- `src/render/transit.ts#buildTrackPreview` (function, line 563) ← call from `src/render/renderer.ts#createSceneRenderer.warmPreviewShaders`
- `src/render/transit.ts#createTrackPreview` (function, line 505) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/transit.ts#createTrackPreview.dispose` (method, line 555) ← implementation from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.warmPreviewShaders`
- `src/render/transit.ts#createTrackPreview.set` (method, line 522) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/transit.ts#createTransit` (function, line 323) ← call from `src/lab/catalog/layers.ts#transitItems.lay`, `src/render/renderer.ts#createSceneRenderer`
- `src/render/transit.ts#createTransit` (function, line 323) ← implementation from `src/lab/catalog/layers.ts#transitItems.lay`, `src/render/renderer.ts#createSceneRenderer`
- `src/render/transit.ts#createTransit` (function, line 323) ← read from `src/render/renderer.ts#createSceneRenderer`
- `src/render/transit.ts#createTransit.cull` (method, line 414) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/transit.ts#createTransit.dispose` (method, line 459) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/transit.ts#createTransit.setXray` (method, line 398) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/transit.ts#createTransit.sync` (method, line 399) ← implementation from `src/lab/catalog/layers.ts#transitItems.lay`, `src/render/renderer.ts#createSceneRenderer`
- `src/render/transit.ts#createTransit.update` (method, line 420) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/transit.ts#TrackPreview` (interface, line 487) ← call from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.warmPreviewShaders`
- `src/render/transit.ts#TrackPreview` (interface, line 487) ← read from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.warmPreviewShaders`
- `src/render/transit.ts#TrackPreview` (interface, line 487) ← typeRef from `src/render/renderer.ts#createSceneRenderer`
- `src/render/transit.ts#TransitMeshes` (interface, line 31) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/transit.ts#TransitMeshes` (interface, line 31) ← read from `src/lab/catalog/layers.ts#transitItems.lay`, `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.movingCasters`, `src/render/renderer.ts#createSceneRenderer.switchedOff`
- `src/render/transit.ts#TransitProduct.allKeys` (method, line 247) ← implementation from `src/world/derived.ts#Derived.refresh`
- `src/render/transit.ts#TransitProduct.changed` (method, line 308) ← implementation from `src/world/derived.ts#Derived.refresh`
- `src/render/transit.ts#TransitProduct.compute` (method, line 271) ← implementation from `src/world/derived.ts#Derived.refresh`
- `src/render/transit.ts#TransitProduct.dirty` (method, line 255) ← implementation from `src/world/derived.ts#Derived.refresh`
- `src/render/transit.ts#TransitProduct.equals` (method, line 304) ← implementation from `src/world/derived.ts#Derived.refresh`

### src/sim/transit/transit.ts

- `src/sim/transit/transit.ts#CROSSING_STOP` (variable, line 35) ← read from `src/sim/intersections/admission.ts#admit`, `src/sim/vehicles/obstacles.ts#longitudinalConstraints`
- `src/sim/transit/transit.ts#TrainView` (interface, line 93) ← read from `src/sim/host/simHost.ts#SimHost.publish`
- `src/sim/transit/transit.ts#TransitSim` (class, line 101) ← construct from `src/sim/city/city.ts#City.transit`
- `src/sim/transit/transit.ts#TransitSim.closedOn` (method, line 479) ← call from `src/sim/vehicles/obstacles.ts#longitudinalConstraints`
- `src/sim/transit/transit.ts#TransitSim.crossingNearEnd` (method, line 464) ← call from `src/sim/intersections/admission.ts#admit`, `src/sim/vehicles/obstacles.ts#longitudinalConstraints`
- `src/sim/transit/transit.ts#TransitSim.step` (method, line 289) ← call from `src/sim/city/city.ts#City.step`
- `src/sim/transit/transit.ts#TransitSim.trains` (method, line 534) ← call from `src/sim/host/simHost.ts#SimHost.publish`
- `src/sim/transit/transit.ts#TransitSim.trainZones` (method, line 493) ← call from `src/sim/agents/walk.ts#stepWalkers`
- `src/sim/transit/transit.ts#TransitSim.trainZones` (method, line 493) ← read from `src/sim/agents/walk.ts#stepWalkers`

### src/ui/v2/transitLine.ts

- `src/ui/v2/transitLine.ts#transitLineRow` (function, line 5) ← call from `src/ui/v2/shell.ts#mountShell.renderTransit`

### src/world/transit.ts

- `src/world/transit.ts#addLine` (function, line 134) ← call from `src/lab/catalog/layers.ts#transitItems`
- `src/world/transit.ts#addLine` (function, line 134) ← read from `src/lab/catalog/layers.ts#transitItems`
- `src/world/transit.ts#addStop` (function, line 107) ← call from `src/lab/catalog/layers.ts#transitItems`
- `src/world/transit.ts#addStop` (function, line 107) ← read from `src/lab/catalog/layers.ts#transitItems`
- `src/world/transit.ts#addTrack` (function, line 122) ← call from `src/lab/catalog/layers.ts#transitItems`
- `src/world/transit.ts#addTrack` (function, line 122) ← read from `src/lab/catalog/layers.ts#transitItems`
- `src/world/transit.ts#emptyTransit` (function, line 53) ← call from `src/lab/catalog/layers.ts#transitItems`, `src/world/doc.ts#RoadDoc.transit`
- `src/world/transit.ts#hasTransit` (function, line 103) ← call from `src/main.ts#simShowsMotion`, `src/sim/agents/trafficBuildings.ts#trafficMapOf`, `src/world/doc.ts#RoadDoc.savedGlobals`, `src/world/doc.ts#RoadDoc.worldPart`
- `src/world/transit.ts#normalizeTransit` (function, line 63) ← call from `src/world/doc.ts#RoadDoc.fromJSON`
- `src/world/transit.ts#TransitData` (interface, line 46) ← typeRef from `src/world/doc.ts#RoadDoc.replaceContents`, `src/world/doc.ts#RoadDoc.restoreValue`, `src/world/doc.ts#RoadDoc.setTransit`, `src/world/doc.ts#RoadDoc.transit`, …
- `src/world/transit.ts#TransitLine` (interface, line 35) ← read from `src/ui/v2/shell.ts#mountShell.renderTransit`
- `src/world/transit.ts#TransitLine` (interface, line 35) ← typeRef from `src/ui/v2/shell.ts#ShellTransitTool`

## Shared declarations the feature uses

### src/core/changeLog.ts

- `src/core/changeLog.ts#ChangeLog.since` (method, line 59) — call by 1: `src/sim/agents/laneGrid.ts#laneChangesSince`
- `src/core/changeLog.ts#ChangeLog.state` (getter, line 21) — call by 1: `src/sim/agents/laneGrid.ts#laneChangesSince`

### src/core/heap.ts

- `src/core/heap.ts#Heap` (class, line 3) — construct by 1: `src/world/transit.ts#railPath`
- `src/core/heap.ts#Heap.peekKey` (method, line 12) — call by 1: `src/world/transit.ts#railPath`
- `src/core/heap.ts#Heap.pop` (method, line 24) — call by 1: `src/world/transit.ts#railPath`
- `src/core/heap.ts#Heap.push` (method, line 14) — call by 1: `src/world/transit.ts#railPath`
- `src/core/heap.ts#Heap.size` (getter, line 10) — call by 1: `src/world/transit.ts#railPath`

### src/core/identityMemo.ts

- `src/core/identityMemo.ts#identityMemo` (function, line 2) — call by 2: `src/world/transit.ts#stopIndex`, `src/world/transit.ts#trackCells`

### src/core/polygon.ts

- `src/core/polygon.ts#pointInPolygon` (function, line 40) — call by 1: `src/editor/transitTools.ts#TransitTool.built`

### src/core/polyline.ts

- `src/core/polyline.ts#Frame` (interface, line 7) — read by 2: `src/sim/transit/transit.ts#TransitSim.stopPoint`, `src/world/footwayPath.ts#nearestFootway`
- `src/core/polyline.ts#Polyline.closestPoint` (method, line 330) — call by 2: `src/sim/transit/transit.ts#levelCrossings`, `src/world/footwayPath.ts#nearestFootway`
- `src/core/polyline.ts#Polyline.closestPoint` (method, line 330) — read by 2: `src/sim/transit/transit.ts#levelCrossings`, `src/world/footwayPath.ts#nearestFootway`
- `src/core/polyline.ts#Polyline.length` (getter, line 123) — call by 1: `src/world/footwayPath.ts#nearestFootway`
- `src/core/polyline.ts#Polyline.n` (field, line 40) — read by 1: `src/sim/transit/transit.ts#levelCrossings`
- `src/core/polyline.ts#Polyline.point` (method, line 69) — call by 1: `src/sim/transit/transit.ts#levelCrossings`
- `src/core/polyline.ts#Polyline.sampleAt` (method, line 152) — call by 2: `src/sim/transit/transit.ts#TransitSim.stopPoint`, `src/world/footwayPath.ts#nearestFootway`

### src/core/rng.ts

- `src/core/rng.ts#Rng.float` (method, line 57) — call by 1: `src/sim/transit/transit.ts#TransitSim.spawnBus`
- `src/core/rng.ts#Rng.range` (method, line 62) — call by 1: `src/sim/transit/transit.ts#TransitSim.spawnBus`

### src/core/units.ts

- `src/core/units.ts#m` (function, line 6) — call by 39: `src/editor/transitTools.ts#PICK`, `src/editor/transitTools.ts#STOP_REACH`, `src/editor/transitTools.ts#TransitTool.built`, `src/editor/transitTools.ts#TransitTool.click`, …

### src/core/vec2.ts

- `src/core/vec2.ts#Vec2` (interface, line 4) — read by 27: `src/editor/transitTools.ts#TransitTool.built`, `src/editor/transitTools.ts#TransitTool.draw`, `src/editor/transitTools.ts#TransitTool.finishTrack`, `src/editor/transitTools.ts#TransitTool.snapTrack`, …
- `src/core/vec2.ts#Vec2` (interface, line 4) — typeRef by 32: `src/editor/transitTools.ts#TransitTool.built`, `src/editor/transitTools.ts#TransitTool.click`, `src/editor/transitTools.ts#TransitTool.cursor`, `src/editor/transitTools.ts#TransitTool.draw`, …

### src/main.ts

- `src/main.ts#drawOverlayScreen.at` (function, line 2960) — indirectCall by 1: `src/editor/transitTools.ts#TransitTool.draw`
- `src/main.ts#transitEditor` (variable, line 854) — callbackAssign by 11: `src/editor/transitTools.ts#TransitTool.built`, `src/editor/transitTools.ts#TransitTool.click`, `src/editor/transitTools.ts#TransitTool.data`, `src/editor/transitTools.ts#TransitTool.finishLine`, …
- `src/main.ts#transitEditor` (variable, line 854) — implementation by 11: `src/editor/transitTools.ts#TransitTool.built`, `src/editor/transitTools.ts#TransitTool.click`, `src/editor/transitTools.ts#TransitTool.data`, `src/editor/transitTools.ts#TransitTool.finishLine`, …

### src/render/instanceRuns.ts

- `src/render/instanceRuns.ts#createInstanceRuns` (function, line 201) — call by 1: `src/render/transit.ts#createTransit`
- `src/render/instanceRuns.ts#createInstanceRuns.cull` (method, line 214) — implementation by 1: `src/render/transit.ts#createTransit.cull`
- `src/render/instanceRuns.ts#createInstanceRuns.dispose` (method, line 215) — implementation by 1: `src/render/transit.ts#createTransit.dispose`
- `src/render/instanceRuns.ts#createInstanceRuns.update` (method, line 209) — implementation by 1: `src/render/transit.ts#createTransit.sync`
- `src/render/instanceRuns.ts#InstanceRuns` (interface, line 170) — call by 3: `src/render/transit.ts#createTransit.cull`, `src/render/transit.ts#createTransit.dispose`, `src/render/transit.ts#createTransit.sync`
- `src/render/instanceRuns.ts#InstanceRuns` (interface, line 170) — read by 3: `src/render/transit.ts#createTransit`, `src/render/transit.ts#createTransit.cull`, `src/render/transit.ts#createTransit.setXray`
- `src/render/instanceRuns.ts#Run` (type, line 100) — typeRef by 3: `src/render/transit.ts#createTransit.sync`, `src/render/transit.ts#TransitPlace`, `src/render/transit.ts#TransitProduct.compute.runOf`
- `src/render/instanceRuns.ts#RunBuilder` (class, line 111) — construct by 1: `src/render/transit.ts#TransitProduct.compute.runOf`
- `src/render/instanceRuns.ts#RunBuilder.build` (method, line 137) — call by 1: `src/render/transit.ts#TransitProduct.compute.runOf`
- `src/render/instanceRuns.ts#RunBuilder.turned` (method, line 132) — call by 1: `src/render/transit.ts#TransitProduct.compute.runOf`
- `src/render/instanceRuns.ts#RunStore` (interface, line 69) — read by 1: `src/render/transit.ts#TransitProduct.compute.runOf`
- `src/render/instanceRuns.ts#RunStore` (interface, line 69) — typeRef by 5: `src/render/transit.ts#createTransit`, `src/render/transit.ts#createTransit.shown`, `src/render/transit.ts#createTransit.through`, `src/render/transit.ts#TransitProduct.compute.runOf`, …
- `src/render/instanceRuns.ts#sameRun` (function, line 152) — call by 1: `src/render/transit.ts#TransitProduct.equals`

### src/render/pinned.ts

- `src/render/pinned.ts#pinned` (function, line 5) — call by 3: `src/render/transit.ts#createTrackPreview`, `src/render/transit.ts#createTrackPreview.set`, `src/render/transit.ts#createTransit`

### src/sim/agents/bay.ts

- `src/sim/agents/bay.ts#BayLane` (interface, line 7) — read by 3: `src/sim/transit/transit.ts#TransitSim.sendBus`, `src/sim/transit/transit.ts#TransitSim.spawnBus`, `src/sim/transit/transit.ts#TransitSim.stopPoint`
- `src/sim/agents/bay.ts#BayLane` (interface, line 7) — typeRef by 1: `src/sim/transit/transit.ts#TransitSim.stopLanes`

### src/sim/agents/laneGrid.ts

- `src/sim/agents/laneGrid.ts#laneGrid` (function, line 167) — call by 1: `src/sim/agents/laneGrid.ts#laneChangesSince`
- `src/sim/agents/laneGrid.ts#LaneGrid.changes` (field, line 35) — read by 1: `src/sim/agents/laneGrid.ts#laneChangesSince`
- `src/sim/agents/laneGrid.ts#LaneGridWorld` (interface, line 160) — typeRef by 1: `src/sim/agents/laneGrid.ts#laneChangesSince`
- `src/sim/agents/laneGrid.ts#lanesInBox` (function, line 183) — call by 1: `src/sim/transit/transit.ts#levelCrossings`

### src/sim/agents/parking.ts

- `src/sim/agents/parking.ts#LANE_BESIDE_REACH` (variable, line 21) — read by 1: `src/sim/transit/transit.ts#TransitSim.followLanes`
- `src/sim/agents/parking.ts#laneBeside` (function, line 482) — call by 2: `src/sim/transit/transit.ts#TransitSim.followLanes`, `src/sim/transit/transit.ts#TransitSim.rebuild`

### src/sim/clock.ts

- `src/sim/clock.ts#SimClock.tick` (field, line 7) — read by 1: `src/sim/transit/transit.ts#TransitSim.spawnBus`

### src/sim/dt.ts

- `src/sim/dt.ts#DT` (variable, line 2) — read by 2: `src/sim/transit/transit.ts#TransitSim.stepBus`, `src/sim/transit/transit.ts#TransitSim.stepTrain`

### src/sim/host/protocol.ts

- `src/sim/host/protocol.ts#TrainCar` (interface, line 83) — read by 1: `src/render/transit.ts#createTransit.update`
- `src/sim/host/protocol.ts#TrainState` (interface, line 84) — read by 1: `src/render/transit.ts#createTransit.update`
- `src/sim/host/protocol.ts#TrainState` (interface, line 84) — typeRef by 1: `src/render/transit.ts#TransitMeshes`

### src/sim/params.ts

- `src/sim/params.ts#DRIVER_NOISE` (variable, line 131) — read by 1: `src/sim/transit/transit.ts#TransitSim.spawnBus`

### src/sim/pose.ts

- `src/sim/pose.ts#Pose` (interface, line 11) — read by 1: `src/sim/transit/transit.ts#TransitSim.stepBus`
- `src/sim/pose.ts#vehiclePose` (function, line 46) — call by 1: `src/sim/transit/transit.ts#TransitSim.stepBus`

### src/sim/routing/router.ts

- `src/sim/routing/router.ts#planFrom` (function, line 107) — call by 2: `src/sim/transit/transit.ts#TransitSim.sendBus`, `src/sim/transit/transit.ts#TransitSim.spawnBus`

### src/sim/vehicles/archetypes.ts

- `src/sim/vehicles/archetypes.ts#Archetype` (interface, line 12) — read by 2: `src/sim/transit/transit.ts#TransitSim.sendBus`, `src/sim/transit/transit.ts#TransitSim.spawnBus`
- `src/sim/vehicles/archetypes.ts#ARCHETYPES` (variable, line 50) — read by 1: `src/sim/transit/transit.ts#TransitSim.spawnBus`

### src/sim/vehicles/driver.ts

- `src/sim/vehicles/driver.ts#makeDriver` (function, line 48) — call by 1: `src/sim/transit/transit.ts#TransitSim.spawnBus`

### src/sim/vehicles/state.ts

- `src/sim/vehicles/state.ts#createVehicle` (function, line 181) — call by 1: `src/sim/transit/transit.ts#TransitSim.spawnBus`
- `src/sim/vehicles/state.ts#snapshot` (function, line 166) — call by 1: `src/sim/transit/transit.ts#TransitSim.spawnBus`
- `src/sim/vehicles/state.ts#Vehicle` (interface, line 30) — read by 3: `src/sim/transit/transit.ts#TransitSim.sendBus`, `src/sim/transit/transit.ts#TransitSim.spawnBus`, `src/sim/transit/transit.ts#TransitSim.stepBus`
- `src/sim/vehicles/state.ts#Vehicle` (interface, line 30) — typeRef by 1: `src/sim/transit/transit.ts#TransitSim.sendBus`
- `src/sim/vehicles/state.ts#Vehicle` (interface, line 30) — write by 4: `src/sim/transit/transit.ts#TransitSim.placeBuses`, `src/sim/transit/transit.ts#TransitSim.sendBus`, `src/sim/transit/transit.ts#TransitSim.spawnBus`, `src/sim/transit/transit.ts#TransitSim.stepBus`

### src/sim/vehicles/vehicleId.ts

- `src/sim/vehicles/vehicleId.ts#VehicleId` (type, line 2) — typeRef by 2: `src/sim/transit/transit.ts#Bus`, `src/sim/transit/transit.ts#TransitSim.spawnBus`

### src/sim/world.ts

- `src/sim/world.ts#FOCUS_WINDOW` (variable, line 37) — read by 1: `src/sim/world.ts#SimWorld.inFocusReach`
- `src/sim/world.ts#SimStreams` (interface, line 91) — read by 1: `src/sim/transit/transit.ts#TransitSim.spawnBus`
- `src/sim/world.ts#SimWorld` (class, line 153) — typeRef by 11: `src/sim/transit/transit.ts#levelCrossings`, `src/sim/transit/transit.ts#TransitSim.followLanes`, `src/sim/transit/transit.ts#TransitSim.passStopAway`, `src/sim/transit/transit.ts#TransitSim.placeBuses`, …
- `src/sim/world.ts#SimWorld.bodiesIn` (method, line 1261) — call by 1: `src/sim/transit/transit.ts#TransitSim.spawnBus`
- `src/sim/world.ts#SimWorld.bodiesIn` (method, line 1261) — read by 1: `src/sim/transit/transit.ts#TransitSim.spawnBus`
- `src/sim/world.ts#SimWorld.clock` (field, line 154) — read by 1: `src/sim/transit/transit.ts#TransitSim.spawnBus`
- `src/sim/world.ts#SimWorld.constructor` (constructor, line 293) — read by 3: `src/sim/transit/transit.ts#TransitSim.placeRails`, `src/sim/transit/transit.ts#TransitSim.rebuild`, `src/sim/transit/transit.ts#TransitSim.step`
- `src/sim/world.ts#SimWorld.enterLanelet` (method, line 1184) — call by 1: `src/sim/transit/transit.ts#TransitSim.spawnBus`
- `src/sim/world.ts#SimWorld.focus` (field, line 273) — read by 1: `src/sim/world.ts#SimWorld.inFocusReach`
- `src/sim/world.ts#SimWorld.lanelet` (method, line 361) — call by 2: `src/sim/transit/transit.ts#TransitSim.spawnBus`, `src/sim/transit/transit.ts#TransitSim.stopPoint`
- `src/sim/world.ts#SimWorld.microWindow` (field, line 174) — read by 1: `src/sim/world.ts#SimWorld.inFocusReach`
- `src/sim/world.ts#SimWorld.nextVehicleId` (field, line 229) — read by 1: `src/sim/transit/transit.ts#TransitSim.spawnBus`
- `src/sim/world.ts#SimWorld.nextVehicleId` (field, line 229) — write by 1: `src/sim/transit/transit.ts#TransitSim.spawnBus`
- `src/sim/world.ts#SimWorld.removeVehicle` (method, line 1286) — call by 3: `src/sim/transit/transit.ts#TransitSim.followLanes`, `src/sim/transit/transit.ts#TransitSim.rebuild`, `src/sim/transit/transit.ts#TransitSim.stepBus`
- `src/sim/world.ts#SimWorld.rng` (getter, line 226) — call by 1: `src/sim/transit/transit.ts#TransitSim.spawnBus`
- `src/sim/world.ts#SimWorld.vehicles` (field, line 163) — read by 5: `src/sim/transit/transit.ts#TransitSim.followLanes`, `src/sim/transit/transit.ts#TransitSim.placeBuses`, `src/sim/transit/transit.ts#TransitSim.rebuild`, `src/sim/transit/transit.ts#TransitSim.spawnBus`, …
- `src/sim/world.ts#SimWorld.vehicles` (field, line 163) — write by 1: `src/sim/transit/transit.ts#TransitSim.spawnBus`
- `src/sim/world.ts#VersionedMap.set` (method, line 1349) — call by 1: `src/sim/transit/transit.ts#TransitSim.spawnBus`

### src/ui/i18n/index.ts

- `src/ui/i18n/index.ts#t` (function, line 55) — call by 1: `src/ui/v2/transitLine.ts#transitLineRow`

### src/world/buildings/geometry.ts

- `src/world/buildings/geometry.ts#volumeCorners` (function, line 153) — call by 1: `src/editor/transitTools.ts#TransitTool.built`

### src/world/buildings/store.ts

- `src/world/buildings/store.ts#BuildingStore.near` (method, line 133) — call by 1: `src/editor/transitTools.ts#TransitTool.built`

### src/world/buildings/types.ts

- `src/world/buildings/types.ts#Building` (interface, line 284) — read by 1: `src/editor/transitTools.ts#TransitTool.built`
- `src/world/buildings/types.ts#Volume` (interface, line 150) — read by 1: `src/editor/transitTools.ts#TransitTool.built`

### src/world/carriageway.ts

- `src/world/carriageway.ts#onCarriageway` (function, line 9) — call by 1: `src/editor/transitTools.ts#TransitTool.onRoad`

### src/world/cells.ts

- `src/world/cells.ts#CellFiling` (class, line 60) — construct by 3: `src/render/transit.ts#TransitProduct.filing`, `src/sim/transit/transit.ts#TransitSim.busStops`, `src/sim/transit/transit.ts#TransitSim.rebuild`
- `src/world/cells.ts#CellFiling.file` (method, line 76) — call by 2: `src/render/transit.ts#TransitProduct.changed`, `src/sim/transit/transit.ts#TransitSim.rebuild`
- `src/world/cells.ts#CellFiling.ids` (method, line 126) — call by 1: `src/render/transit.ts#TransitProduct.allKeys`
- `src/world/cells.ts#CellFiling.idsMeetingAny` (method, line 169) — call by 2: `src/render/transit.ts#TransitProduct.dirty`, `src/sim/transit/transit.ts#TransitSim.followLanes`
- `src/world/cells.ts#CellFiling.remove` (method, line 107) — call by 1: `src/render/transit.ts#TransitProduct.changed`

### src/world/changes.ts

- `src/world/changes.ts#Change` (interface, line 35) — read by 2: `src/render/transit.ts#TransitProduct.dirty`, `src/render/transit.ts#TransitProduct.dirty.meets`
- `src/world/changes.ts#Change` (interface, line 35) — typeRef by 1: `src/render/transit.ts#TransitProduct.dirty`
- `src/world/changes.ts#ChangeJournal` (class, line 156) — typeRef by 1: `src/render/transit.ts#TransitSource`
- `src/world/changes.ts#ChangeJournal.serialOf` (method, line 203) — call by 1: `src/world/doc.ts#RoadDoc.transitRevision`
- `src/world/changes.ts#ChangeKind` (type, line 27) — typeRef by 1: `src/render/transit.ts#TransitProduct.kinds`
- `src/world/changes.ts#ChangeRect` (type, line 4) — typeRef by 6: `src/render/transit.ts#pointsBox`, `src/render/transit.ts#TransitPlace`, `src/render/transit.ts#TransitProduct.compute`, `src/sim/transit/transit.ts#TransitSim.followLanes`, …

### src/world/derived.ts

- `src/world/derived.ts#Derived` (class, line 71) — construct by 1: `src/render/transit.ts#createTransit`
- `src/world/derived.ts#Derived.get` (method, line 80) — call by 1: `src/render/transit.ts#createTransit.sync`
- `src/world/derived.ts#Derived.refresh` (method, line 93) — call by 1: `src/render/transit.ts#createTransit.sync`
- `src/world/derived.ts#Derived.work` (field, line 74) — read by 1: `src/render/transit.ts#createTransit.sync`
- `src/world/derived.ts#DerivedSpec` (interface, line 40) — implements by 1: `src/render/transit.ts#TransitProduct`
- `src/world/derived.ts#DerivedWork` (interface, line 60) — read by 1: `src/render/transit.ts#createTransit.sync`

### src/world/doc.ts

- `src/world/doc.ts#RoadDoc` (class, line 153) — typeRef by 6: `src/editor/transitTools.ts#TransitToolDeps`, `src/render/transit.ts#createTransit`, `src/render/transit.ts#stopParts`, `src/render/transit.ts#TransitSource`, …
- `src/world/doc.ts#RoadDoc.buildings` (field, line 289) — read by 1: `src/editor/transitTools.ts#TransitTool.built`
- `src/world/doc.ts#RoadDoc.changes` (field, line 262) — read by 2: `src/render/transit.ts#createTransit.sync`, `src/world/doc.ts#RoadDoc.transitRevision`
- `src/world/doc.ts#RoadDoc.nodes` (field, line 154) — read by 1: `src/render/transit.ts#stopParts`
- `src/world/doc.ts#RoadDoc.segments` (field, line 155) — read by 1: `src/render/transit.ts#stopParts`
- `src/world/doc.ts#RoadDoc.transit` (field, line 187) — read by 7: `src/editor/transitTools.ts#TransitTool.data`, `src/render/transit.ts#stopParts`, `src/render/transit.ts#TransitProduct.allKeys`, `src/render/transit.ts#TransitProduct.compute`, …
- `src/world/doc.ts#RoadNode` (interface, line 89) — read by 1: `src/render/transit.ts#stopParts`
- `src/world/doc.ts#RoadSegment` (interface, line 124) — read by 1: `src/render/transit.ts#stopParts`

### src/world/footwayPath.ts

- `src/world/footwayPath.ts#footwayPath` (function, line 22) — call by 1: `src/world/footwayPath.ts#nearestFootway`
- `src/world/footwayPath.ts#FootwayRibbon` (interface, line 11) — read by 1: `src/world/footwayPath.ts#nearestFootway`
- `src/world/footwayPath.ts#FootwayRibbon` (interface, line 11) — typeRef by 1: `src/world/footwayPath.ts#FootwayNet`
- `src/world/footwayPath.ts#REJECT_STEP` (variable, line 32) — read by 1: `src/world/footwayPath.ts#nearestFootway`

### src/world/ids.ts

- `src/world/ids.ts#SegmentId` (type, line 8) — typeRef by 4: `src/world/transit.ts#normalizeTransit`, `src/world/transit.ts#TransitStop`, `src/world/footwayPath.ts#nearestFootway`, `src/world/footwayPath.ts#FootwayNet`

### src/world/lanelets.ts

- `src/world/lanelets.ts#Lanelet` (interface, line 31) — read by 3: `src/sim/transit/transit.ts#levelCrossings`, `src/sim/transit/transit.ts#TransitSim.spawnBus`, `src/sim/transit/transit.ts#TransitSim.stopPoint`

### src/world/network.ts

- `src/world/network.ts#Network` (class, line 46) — typeRef by 1: `src/editor/transitTools.ts#TransitToolDeps`

### src/world/roadTypes.ts

- `src/world/roadTypes.ts#SegmentDirection` (type, line 7) — typeRef by 1: `src/world/footwayPath.ts#FootwayNet`

### src/world/terrain.ts

- `src/world/terrain.ts#TERRAIN_MAX_HEIGHT` (variable, line 39) — read by 2: `src/render/transit.ts#createTransit`, `src/render/transit.ts#createTransit.update`
- `src/world/terrain.ts#TERRAIN_MIN_HEIGHT` (variable, line 38) — read by 2: `src/render/transit.ts#createTransit`, `src/render/transit.ts#createTransit.update`

## External APIs

- `es:Math` × 130
- `es:Array.push` × 75
- `es:Array.length` × 46
- `es:Map.get` × 46
- `es:Math.max` × 29
- `es:ReadonlyArray.length` × 26
- `es:Math.min` × 24
- `es:Map` × 23
- `es:Math.hypot` × 18
- `es:Map.set` × 17
- `es:Math.floor` × 17
- `es:Infinity` × 13
- `es:Math.cos` × 10
- `es:Math.sin` × 10
- `three:InstancedMesh.instanceMatrix` × 10
- `es:ReadonlyArray.map` × 9
- `es:Record` × 9
- `es:Array.filter` × 8
- `es:Array.map` × 8
- `es:Math.atan2` × 8
- `es:Set` × 7
- `es:Set.add` × 7
- `es:Math.PI` × 6
- `es:ReadonlyArray.filter` × 6
- `es:ReadonlyArray.find` × 6
- `three:Object3D.visible` × 6
- `three:Vector3.set` × 6
- `es:Array` × 5
- `es:ArrayConstructor.isArray` × 5
- `three:InstancedMesh.count` × 5
- `three:InstancedMesh.instanceColor` × 5
- `three:MeshStandardMaterial` × 5
- `dom:CanvasDrawPath.beginPath` × 4
- `dom:document` × 4
- `dom:Document.createElement` × 4
- `es:Array.sort` × 4
- `es:Math.abs` × 4
- `es:NaN` × 4
- `es:ReadonlyArray.forEach` × 4
- `es:Set.has` × 4
- `three:Color` × 4
- `three:Group` × 4
- `three:Object3D.add` × 4
- `three:Object3D.position` × 4
- `three:Object3D.rotation` × 4
- `three:Object3D.scale` × 4
- `three:Sphere.center` × 4
- `dom:CanvasDrawPath.stroke` × 3
- `dom:CanvasFillStrokeStyles.fillStyle` × 3
- `dom:CanvasFillStrokeStyles.strokeStyle` × 3
- `dom:CanvasPathDrawingStyles.lineWidth` × 3
- `es:Array.forEach` × 3
- `es:Array.pop` × 3
- `es:Map.clear` × 3
- `es:Map.delete` × 3
- `es:Map.values` × 3
- `es:Number` × 3
- `es:NumberConstructor.isFinite` × 3
- `three:BufferAttribute.addUpdateRange` × 3
- `three:BufferAttribute.clearUpdateRanges` × 3
- `three:BufferAttribute.needsUpdate` × 3
- `three:InstancedMesh` × 3
- `three:InstancedMesh.dispose` × 3
- `three:Object3D.frustumCulled` × 3
- `three:Object3D.name` × 3
- `three:Object3D.renderOrder` × 3
- `dom:CanvasCompositing.globalAlpha` × 2
- `dom:CanvasDrawPath.fill` × 2
- `dom:CanvasPath.arc` × 2
- `dom:CanvasPath.lineTo` × 2
- `dom:CanvasPath.moveTo` × 2
- `dom:CanvasState.restore` × 2
- `dom:Element.textContent` × 2
- `es:Array.every` × 2
- `es:Array.reverse` × 2
- `es:Iterable` × 2
- `es:Math.round` × 2
- `es:Object` × 2
- `es:ReadonlyArray.some` × 2
- `three:BoxGeometry` × 2
- `three:BufferAttribute.count` × 2
- `three:BufferAttribute.setUsage` × 2
- `three:BufferGeometry.dispose` × 2
- `three:DynamicDrawUsage` × 2
- `three:Euler.set` × 2
- `three:Frustum` × 2
- `three:Frustum.intersectsSphere` × 2
- `three:InstancedMesh.setMatrixAt` × 2
- `three:MaterialProperties.userData` × 2
- `three:Mesh.material` × 2
- `three:Object3D` × 2
- `three:Object3D.matrix` × 2
- `three:Object3D.updateMatrix` × 2
- `three:Sphere.radius` × 2
- `dom:CanvasPath.rect` × 1
- `dom:CanvasPathDrawingStyles.lineCap` × 1
- `dom:CanvasPathDrawingStyles.lineJoin` × 1
- `dom:CanvasRenderingContext2D` × 1
- `dom:CanvasState.save` × 1
- `dom:CanvasText.fillText` × 1
- `dom:CanvasTextDrawingStyles.font` × 1
- `dom:CanvasTextDrawingStyles.textAlign` × 1
- `dom:CanvasTextDrawingStyles.textBaseline` × 1
- `dom:CSSStyleDeclaration.background` × 1
- `dom:Element.className` × 1
- `dom:ElementCSSInlineStyle.style` × 1
- `dom:HTMLDivElement` × 1
- `dom:ParentNode.append` × 1
- `es:Array.findIndex` × 1
- `es:Array.slice` × 1
- `es:Float32Array` × 1
- `es:Float32Array.fill` × 1
- `es:Float64Array` × 1
- `es:Float64Array.fill` × 1
- `es:Int32Array` × 1
- `es:Int32Array.fill` × 1
- `es:Map.has` × 1
- `es:Math.ceil` × 1
- `es:Math.sqrt` × 1
- `es:NonNullable` × 1
- `es:ObjectConstructor.freeze` × 1
- `es:ObjectConstructor.fromEntries` × 1
- `es:Omit` × 1
- `es:ReadonlyArray.every` × 1
- `es:ReadonlyArray.findIndex` × 1
- `es:ReadonlyArray.flat` × 1
- `es:ReadonlyArray.includes` × 1
- `es:RegExp.test` × 1
- `es:Set.size` × 1
- `es:Uint8Array` × 1
- `three:Color.copy` × 1
- `three:Color.set` × 1
- `three:Frustum.copy` × 1
- `three:InstancedBufferAttribute` × 1
- `three:InstancedMesh.setColorAt` × 1
- `three:MaterialProperties.depthTest` × 1
- `three:Matrix4` × 1
- `three:Object3D.castShadow` × 1
- `three:Object3D.remove` × 1
- `three:Sphere` × 1
- `three:Vector3` × 1
