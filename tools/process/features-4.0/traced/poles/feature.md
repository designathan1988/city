# Feature: src/editor/poles.ts, src/editor/poleRoutes.ts, src/render/poleLayer.ts

The program starts from `src/main.ts`, `src/peopleLab/main.ts`. A declaration is the feature's own when the program reaches it only through the feature; in a declaration of the program that names the feature, its statements are a slice (what names it, what depends on that, what that controls).

- Own: 198 declarations
- Slices of shared code: 33
- Code: 1870 lines in 12 files (source/)
- Resources: 6 (6 entry, 0 style, 0 import; 4 only the feature uses)
- Uses: 116 shared declarations (interface/: 129 declarations in 28 files, the types they name included); hooks: 64; external APIs: 140

## Code

- src/editor/poleRoutes.ts — whole, 199 lines
- src/editor/poles.ts — whole, 516 lines
- src/editor/roads/powerLine.ts — part, 16 lines
- src/main.ts — part, 81 lines
- src/render/poleBake.ts — part, 41 lines
- src/render/poleCells.ts — part, 86 lines
- src/render/poleLayer.ts — whole, 751 lines
- src/render/renderer.ts — part, 61 lines
- src/render/utilities.ts — part, 43 lines
- src/world/doc.ts — part, 3 lines
- src/world/poleLines.ts — part, 72 lines
- src/world/utilities.ts — part, 1 lines

## Slices of shared code

- `src/editor/roads/powerLine.ts#PlanRun` — 1 of 1 lines
- `src/editor/roads/powerLine.ts#layPowerLinesSteps` — 8 of 44 lines
- `src/editor/roads/powerLine.ts#runPlanner` — 7 of 7 lines
- `src/main.ts#<IfStatement>@4036` — 10 of 196 lines
- `src/main.ts#cancelGestures` — 3 of 13 lines
- `src/main.ts#currentGesture` — 4 of 18 lines
- `src/main.ts#drawOverlayScreen` — 3 of 469 lines
- `src/main.ts#drawPolePlan` — 24 of 50 lines
- `src/main.ts#endPointer` — 3 of 30 lines
- `src/main.ts#frame` — 6 of 102 lines
- `src/main.ts#framePolePlan` — 1 of 1 lines
- `src/main.ts#gestureInProgress` — 3 of 3 lines
- `src/main.ts#pointerDown` — 7 of 180 lines
- `src/main.ts#pointerMove` — 3 of 72 lines
- `src/main.ts#poleReach` — 3 of 3 lines
- `src/main.ts#poleTool` — 11 of 11 lines
- `src/render/poleCells.ts#HeldBlock` — 3 of 4 lines
- `src/render/poleCells.ts#PoleCells.build` — 17 of 38 lines
- `src/render/poleCells.ts#PoleCells.constructor` — 8 of 11 lines
- `src/render/poleCells.ts#PoleCells.derive` — 16 of 37 lines
- `src/render/poleCells.ts#PoleCells.dispose` — 3 of 4 lines
- `src/render/poleCells.ts#PoleCells.group` — 1 of 1 lines
- `src/render/poleCells.ts#PoleCells.live` — 1 of 1 lines
- `src/render/poleCells.ts#PoleCells.look` — 21 of 39 lines
- `src/render/poleCells.ts#PoleCells.release` — 5 of 5 lines
- `src/render/poleCells.ts#PoleCells.scale` — 1 of 1 lines
- `src/render/poleCells.ts#PoleCells.view` — 10 of 11 lines
- `src/render/renderer.ts#SceneHandle` — 3 of 170 lines
- `src/render/renderer.ts#createSceneRenderer` — 23 of 3774 lines
- `src/render/renderer.ts#createSceneRenderer.switchedOff` — 16 of 16 lines
- `src/render/renderer.ts#createSceneRenderer.syncUtilities` — 5 of 12 lines
- `src/render/renderer.ts#createSceneRenderer.warmRoadShaders` — 14 of 39 lines
- `src/render/utilities.ts#wireMaterials` — 5 of 31 lines

## Resources

- feature entry `hint.pole.offFootway` — src/ui/i18n/en.ts:366: `'hint.pole.offFootway': 'Poles on footways only'`
- feature entry `hint.pole.noPath` — src/ui/i18n/en.ts:367: `'hint.pole.noPath': 'No footway joins these points'`
- shared entry `hint.pole.removed` — src/ui/i18n/en.ts:906: `'hint.pole.removed': 'Pole removed'`
- feature entry `hint.pole.offFootway` — src/ui/i18n/pt-BR.ts:364: `'hint.pole.offFootway': 'Postes só nas calçadas'`
- feature entry `hint.pole.noPath` — src/ui/i18n/pt-BR.ts:365: `'hint.pole.noPath': 'Não há calçada ligando esses pontos'`
- shared entry `hint.pole.removed` — src/ui/i18n/pt-BR.ts:899: `'hint.pole.removed': 'Poste removido'`

## Where the program plugs the feature in

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

### src/render/poleLayer.ts

- `src/render/poleLayer.ts#Block` (interface, line 506) ← read from `src/render/poleCells.ts#PoleCells.build`, `src/render/poleCells.ts#PoleCells.derive`, `src/render/poleCells.ts#PoleCells.release`, `src/render/poleCells.ts#PoleCells.view`
- `src/render/poleLayer.ts#Block` (interface, line 506) ← typeRef from `src/render/poleCells.ts#HeldBlock`, `src/render/poleCells.ts#PoleCells.build`, `src/render/poleCells.ts#PoleCells.live`, `src/render/poleCells.ts#PoleCells.release`
- `src/render/poleLayer.ts#Block` (interface, line 506) ← write from `src/render/poleCells.ts#PoleCells.build`
- `src/render/poleLayer.ts#BLOCK` (variable, line 503) ← read from `src/render/poleCells.ts#PoleCells.build`, `src/render/poleCells.ts#PoleCells.constructor`, `src/render/poleCells.ts#PoleCells.look`
- `src/render/poleLayer.ts#buildPolePreview` (function, line 729) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/poleLayer.ts#buildPolePreview` (function, line 729) ← implementation from `src/render/poleBake.ts#framePole`
- `src/render/poleLayer.ts#buildUtilities` (function, line 711) ← call from `src/render/renderer.ts#createSceneRenderer.warmRoadShaders`
- `src/render/poleLayer.ts#docPoles` (function, line 66) ← call from `src/render/poleCells.ts#PoleCells.build`
- `src/render/poleLayer.ts#docPoles` (function, line 66) ← implementation from `src/render/poleBake.ts#framePole`
- `src/render/poleLayer.ts#PartStore` (class, line 153) ← construct from `src/render/poleCells.ts#PoleCells.build`
- `src/render/poleLayer.ts#PartStore.add` (method, line 226) ← call from `src/render/poleCells.ts#PoleCells.build`
- `src/render/poleLayer.ts#PartStore.box` (getter, line 279) ← call from `src/render/poleCells.ts#PoleCells.view`
- `src/render/poleLayer.ts#PartStore.count` (field, line 156) ← read from `src/render/poleCells.ts#PoleCells.view`
- `src/render/poleLayer.ts#PartStore.dispose` (method, line 317) ← call from `src/render/poleCells.ts#PoleCells.release`
- `src/render/poleLayer.ts#PartStore.flush` (method, line 253) ← call from `src/render/poleCells.ts#PoleCells.build`
- `src/render/poleLayer.ts#PartStore.meshOf` (method, line 178) ← implementation from `src/render/shadowCasters.ts#castCastersInto`, `src/render/shadowCasters.ts#castShadowKey`
- `src/render/poleLayer.ts#PartStore.seal` (method, line 301) ← call from `src/render/poleCells.ts#PoleCells.derive`
- `src/render/poleLayer.ts#PartStore.view` (method, line 284) ← call from `src/render/poleCells.ts#PoleCells.view`
- `src/render/poleLayer.ts#POLE_REACH` (variable, line 69) ← read from `src/render/poleCells.ts#PoleCells.constructor`
- `src/render/poleLayer.ts#PoleLayer.dispose` (method, line 697) ← call from `src/render/renderer.ts#createSceneRenderer.warmRoadShaders`
- `src/render/poleLayer.ts#PoleLayer.dispose` (method, line 697) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/poleLayer.ts#PoleLayer.group` (field, line 514) ← read from `src/render/renderer.ts#createSceneRenderer.warmRoadShaders`
- `src/render/poleLayer.ts#PoleLayer.place` (method, line 611) ← indirectCall from `src/render/poleBake.ts#framePole`, `src/render/utilities.ts#placePole`
- `src/render/poleLayer.ts#PolePreviewInput` (interface, line 724) ← read from `src/render/renderer.ts#createSceneRenderer`
- `src/render/poleLayer.ts#PolePreviewInput` (interface, line 724) ← typeRef from `src/render/renderer.ts#SceneHandle`
- `src/render/poleLayer.ts#screen` (variable, line 85) ← read from `src/render/poleCells.ts#PoleCells.view`
- `src/render/poleLayer.ts#ScreenScale` (class, line 108) ← construct from `src/render/poleCells.ts#PoleCells.scale`
- `src/render/poleLayer.ts#ScreenScale.atBox` (method, line 134) ← call from `src/render/poleCells.ts#PoleCells.view`
- `src/render/poleLayer.ts#ScreenScale.set` (method, line 117) ← call from `src/render/poleCells.ts#PoleCells.view`
- `src/render/poleLayer.ts#ViewedRoot` (class, line 492) ← construct from `src/render/poleCells.ts#PoleCells.constructor`
- `src/render/poleLayer.ts#ViewedRoot` (class, line 492) ← typeRef from `src/render/poleCells.ts#PoleCells.group`
- `src/render/poleLayer.ts#WireStore` (class, line 327) ← construct from `src/render/poleCells.ts#PoleCells.build`
- `src/render/poleLayer.ts#WireStore.add` (method, line 399) ← call from `src/render/poleCells.ts#PoleCells.build`
- `src/render/poleLayer.ts#WireStore.box` (getter, line 445) ← call from `src/render/poleCells.ts#PoleCells.view`
- `src/render/poleLayer.ts#WireStore.count` (field, line 332) ← read from `src/render/poleCells.ts#PoleCells.view`
- `src/render/poleLayer.ts#WireStore.dispose` (method, line 483) ← call from `src/render/poleCells.ts#PoleCells.release`
- `src/render/poleLayer.ts#WireStore.drawn` (getter, line 479) ← call from `src/render/poleCells.ts#PoleCells.derive`
- `src/render/poleLayer.ts#WireStore.flush` (method, line 422) ← call from `src/render/poleCells.ts#PoleCells.build`
- `src/render/poleLayer.ts#WireStore.instances` (getter, line 474) ← call from `src/render/poleCells.ts#PoleCells.derive`
- `src/render/poleLayer.ts#WireStore.makeMesh` (method, line 365) ← implementation from `src/render/shadowCasters.ts#castCastersInto`
- `src/render/poleLayer.ts#WireStore.seal` (method, line 464) ← call from `src/render/poleCells.ts#PoleCells.derive`
- `src/render/poleLayer.ts#WireStore.view` (method, line 450) ← call from `src/render/poleCells.ts#PoleCells.view`

## Shared declarations the feature uses

### src/core/heap.ts

- `src/core/heap.ts#Heap` (class, line 3) — construct by 1: `src/editor/poleRoutes.ts#route`
- `src/core/heap.ts#Heap.pop` (method, line 24) — call by 1: `src/editor/poleRoutes.ts#route`
- `src/core/heap.ts#Heap.push` (method, line 14) — call by 2: `src/editor/poleRoutes.ts#route`, `src/editor/poleRoutes.ts#route.relax`
- `src/core/heap.ts#Heap.size` (getter, line 10) — call by 1: `src/editor/poleRoutes.ts#route`

### src/core/polyline.ts

- `src/core/polyline.ts#Frame` (interface, line 7) — read by 1: `src/editor/poleRoutes.ts#buildRouteGraph`
- `src/core/polyline.ts#Polyline.cum` (getter, line 81) — call by 2: `src/editor/poleRoutes.ts#buildRouteGraph`, `src/world/poleLines.ts#nearestOnContour`
- `src/core/polyline.ts#Polyline.length` (getter, line 123) — call by 1: `src/editor/poleRoutes.ts#buildRouteGraph`
- `src/core/polyline.ts#Polyline.n` (field, line 40) — read by 1: `src/editor/poleRoutes.ts#buildRouteGraph`
- `src/core/polyline.ts#Polyline.sampleAt` (method, line 152) — call by 1: `src/editor/poleRoutes.ts#buildRouteGraph`
- `src/core/polyline.ts#Polyline.xy` (field, line 38) — read by 2: `src/editor/poleRoutes.ts#buildRouteGraph`, `src/world/poleLines.ts#nearestOnContour`

### src/core/scalar.ts

- `src/core/scalar.ts#EPS` (variable, line 6) — read by 1: `src/world/poleLines.ts#nearestOnContour`

### src/core/units.ts

- `src/core/units.ts#m` (function, line 6) — call by 17: `src/editor/poles.ts#CLOSED_PAD`, `src/editor/poles.ts#computePoleRun.clear`, `src/editor/poles.ts#CORNER_TOLERANCE`, `src/editor/poles.ts#inLanding`, …

### src/core/vec2.ts

- `src/core/vec2.ts#dist` (function, line 20) — call by 4: `src/editor/poles.ts#computePoleRun`, `src/editor/poles.ts#corners`, `src/editor/poles.ts#routeRun`, `src/editor/poleRoutes.ts#route`
- `src/core/vec2.ts#Vec2` (interface, line 4) — read by 15: `src/editor/poles.ts#commitPoleRunPoles`, `src/editor/poles.ts#computePoleRun.at`, `src/editor/poles.ts#corners`, `src/editor/poles.ts#inLanding`, …
- `src/core/vec2.ts#Vec2` (interface, line 4) — typeRef by 27: `src/editor/poles.ts#computePoleRun`, `src/editor/poles.ts#computePoleRun.at`, `src/editor/poles.ts#corners`, `src/editor/poles.ts#endOff`, …

### src/main.ts

- `src/main.ts#poleTool` (variable, line 488) — implementation by 7: `src/editor/poles.ts#PoleTool.down`, `src/editor/poles.ts#PoleTool.down.remove`, `src/editor/poles.ts#PoleTool.marks`, `src/editor/poles.ts#PoleTool.move`, …

### src/render/poleBake.ts

- `src/render/poleBake.ts#framePole` (function, line 75) — call by 1: `src/render/poleLayer.ts#PoleLayer.place`
- `src/render/poleBake.ts#PoleSource` (interface, line 10) — call by 2: `src/render/poleLayer.ts#PoleLayer.place`, `src/render/poleLayer.ts#PoleLayer.redo`
- `src/render/poleBake.ts#PoleSource` (interface, line 10) — read by 2: `src/render/poleLayer.ts#PoleLayer.fill`, `src/render/poleLayer.ts#PoleLayer.redo`
- `src/render/poleBake.ts#PoleSource` (interface, line 10) — typeRef by 6: `src/render/poleLayer.ts#buildUtilities`, `src/render/poleLayer.ts#docPoles`, `src/render/poleLayer.ts#PoleLayer.fill`, `src/render/poleLayer.ts#PoleLayer.follow`, …
- `src/render/poleBake.ts#WIRE_BLOCK` (variable, line 53) — read by 2: `src/render/poleLayer.ts#WireStore.BLOCK`, `src/render/poleBake.ts#growWires`
- `src/render/poleBake.ts#wireEnd` (function, line 86) — call by 1: `src/render/poleLayer.ts#PoleLayer.string`

### src/render/scenery.ts

- `src/render/scenery.ts#SceneryKit` (interface, line 131) — typeRef by 3: `src/render/poleLayer.ts#buildPolePreview`, `src/render/poleLayer.ts#buildUtilities`, `src/render/poleLayer.ts#PoleLayer.constructor`

### src/render/shadowCasters.ts

- `src/render/shadowCasters.ts#castBySize` (function, line 26) — call by 2: `src/render/poleLayer.ts#PartStore.meshOf`, `src/render/poleLayer.ts#WireStore.makeMesh`

### src/render/utilities.ts

- `src/render/utilities.ts#PartLevel` (interface, line 99) — read by 3: `src/render/poleLayer.ts#PartStore.levelFor`, `src/render/poleLayer.ts#PartStore.meshOf`, `src/render/poleLayer.ts#PartStore.view`
- `src/render/utilities.ts#PARTS` (variable, line 587) — read by 1: `src/render/utilities.ts#wireParts`
- `src/render/utilities.ts#Placement` (interface, line 79) — read by 2: `src/render/poleBake.ts#growPart`, `src/render/utilities.ts#writePlacement`
- `src/render/utilities.ts#Placement` (interface, line 79) — typeRef by 5: `src/render/poleLayer.ts#PartStore.add`, `src/render/poleLayer.ts#PartStore.write`, `src/render/poleLayer.ts#PoleLayer.scratch`, `src/render/poleBake.ts#growPart`, …
- `src/render/utilities.ts#placePole` (function, line 155) — call by 1: `src/render/poleLayer.ts#PoleLayer.place`
- `src/render/utilities.ts#placing` (variable, line 239) — read by 1: `src/render/utilities.ts#writePlacement`
- `src/render/utilities.ts#POLE_PART_COUNT` (variable, line 96) — read by 2: `src/render/poleLayer.ts#PoleLayer.place`, `src/render/poleLayer.ts#PoleLayer.scratch`
- `src/render/utilities.ts#PolePartSpec` (interface, line 105) — call by 1: `src/render/poleLayer.ts#PartStore.view`
- `src/render/utilities.ts#PolePartSpec` (interface, line 105) — read by 5: `src/render/poleLayer.ts#PartStore.constructor`, `src/render/poleLayer.ts#PartStore.flush`, `src/render/poleLayer.ts#PartStore.levelFor`, `src/render/poleLayer.ts#PartStore.meshOf`, …
- `src/render/utilities.ts#PolePartSpec` (interface, line 105) — typeRef by 2: `src/render/poleLayer.ts#PartStore.constructor`, `src/render/poleLayer.ts#PoleLayer.specs`
- `src/render/utilities.ts#polePartSpecs` (function, line 123) — call by 1: `src/render/poleLayer.ts#PoleLayer.constructor`
- `src/render/utilities.ts#polePartSpecs` (function, line 123) — callbackAssign by 1: `src/render/poleLayer.ts#PartStore.view`
- `src/render/utilities.ts#polePartSpecs` (function, line 123) — implementation by 1: `src/render/poleLayer.ts#PartStore.view`
- `src/render/utilities.ts#polePartSpecs.light` (function, line 127) — callbackAssign by 1: `src/render/poleLayer.ts#PartStore.view`
- `src/render/utilities.ts#polePartSpecs.light` (function, line 127) — implementation by 1: `src/render/poleLayer.ts#PartStore.view`
- `src/render/utilities.ts#polePartSpecs.solid` (function, line 125) — callbackAssign by 1: `src/render/poleLayer.ts#PartStore.view`
- `src/render/utilities.ts#polePartSpecs.solid` (function, line 125) — implementation by 1: `src/render/poleLayer.ts#PartStore.view`
- `src/render/utilities.ts#SPAN_WIRES` (variable, line 250) — read by 2: `src/render/poleLayer.ts#WireStore.flush`, `src/render/poleLayer.ts#WireStore.geometry`
- `src/render/utilities.ts#spanWires` (function, line 255) — call by 1: `src/render/poleLayer.ts#WireStore.add`
- `src/render/utilities.ts#spanWires` (function, line 255) — typeRef by 1: `src/render/poleLayer.ts#WireStore.add`
- `src/render/utilities.ts#Utilities` (interface, line 73) — implements by 1: `src/render/poleLayer.ts#PoleLayer`
- `src/render/utilities.ts#Utilities` (interface, line 73) — typeRef by 1: `src/render/poleLayer.ts#buildPolePreview`
- `src/render/utilities.ts#UtilityParts` (interface, line 452) — read by 1: `src/render/utilities.ts#wireParts`
- `src/render/utilities.ts#WIRE_FADE` (variable, line 68) — read by 1: `src/render/utilities.ts#wireLevel`
- `src/render/utilities.ts#WIRE_HALF_WIDTH` (variable, line 279) — read by 1: `src/render/poleLayer.ts#WireStore.flush`
- `src/render/utilities.ts#WIRE_STRIDE` (variable, line 252) — read by 2: `src/render/poleLayer.ts#WireStore.reserve`, `src/render/poleBake.ts#growWires`
- `src/render/utilities.ts#WIRE_TEMPLATES` (variable, line 328) — read by 2: `src/render/utilities.ts#wireLevel`, `src/render/utilities.ts#wireGeometry`
- `src/render/utilities.ts#WIRE_TUBE_PIXELS` (variable, line 66) — read by 1: `src/render/utilities.ts#wireLevel`
- `src/render/utilities.ts#WIRE_WIDTH` (variable, line 64) — read by 2: `src/render/poleLayer.ts#WireStore.makeMesh`, `src/render/utilities.ts#wireLevel`
- `src/render/utilities.ts#wireScreen` (variable, line 365) — read by 1: `src/render/poleLayer.ts#PoleLayer.view`
- `src/render/utilities.ts#wireScreen` (variable, line 365) — write by 1: `src/render/poleLayer.ts#PoleLayer.view`
- `src/render/utilities.ts#WireTemplate` (interface, line 282) — read by 2: `src/render/utilities.ts#wireLevel`, `src/render/utilities.ts#wireGeometry`

### src/world/approach.ts

- `src/world/approach.ts#CROSSWALK_DEPTH` (variable, line 7) — read by 1: `src/editor/poles.ts#inLanding`

### src/world/cells.ts

- `src/world/cells.ts#CELL` (variable, line 9) — read by 3: `src/editor/poles.ts#poleRunPlanner.tileAt`, `src/editor/poles.ts#routeRun`, `src/editor/poles.ts#TILE`
- `src/world/cells.ts#CellFiling.idsMeeting` (method, line 131) — call by 1: `src/editor/poleRoutes.ts#buildRouteGraph`

### src/world/changes.ts

- `src/world/changes.ts#Change` (interface, line 35) — read by 1: `src/render/poleLayer.ts#PoleLayer.follow`
- `src/world/changes.ts#Change` (interface, line 35) — typeRef by 1: `src/render/poleLayer.ts#PoleLayer.follow`
- `src/world/changes.ts#ChangeJournal.serialOf` (method, line 203) — call by 1: `src/editor/poles.ts#planPoleRun`
- `src/world/changes.ts#ChangeRect` (type, line 4) — typeRef by 8: `src/editor/poles.ts#poleRunPlanner`, `src/editor/poles.ts#poleRunPlanner.inner`, `src/editor/poles.ts#poleRunPlanner.tileAt`, `src/editor/poles.ts#routeGraph`, …

### src/world/doc.ts

- `src/world/doc.ts#RoadDoc` (class, line 153) — typeRef by 12: `src/editor/poles.ts#commitPoleRun`, `src/editor/poles.ts#commitPoleRunPoles`, `src/editor/poles.ts#computePoleRun`, `src/editor/poles.ts#lastPlan`, …
- `src/world/doc.ts#RoadDoc.addPole` (method, line 440) — call by 1: `src/editor/poles.ts#commitPoleRunPoles`
- `src/world/doc.ts#RoadDoc.addPoleSpan` (method, line 498) — call by 1: `src/editor/poles.ts#commitPoleRunPoles`
- `src/world/doc.ts#RoadDoc.changes` (field, line 262) — read by 1: `src/editor/poles.ts#planPoleRun`
- `src/world/doc.ts#RoadDoc.poleNear` (method, line 671) — call by 4: `src/editor/poles.ts#computePoleRun`, `src/editor/poles.ts#PoleTool.down`, `src/editor/poles.ts#PoleTool.marks`, `src/editor/poles.ts#snapOn`
- `src/world/doc.ts#RoadDoc.poles` (getter, line 164) — call by 2: `src/render/poleLayer.ts#docPoles`, `src/world/doc.ts#RoadDoc.pole`
- `src/world/doc.ts#RoadDoc.polesIn` (method, line 450) — call by 1: `src/render/poleLayer.ts#PoleLayer.follow`
- `src/world/doc.ts#RoadDoc.poleSpans` (getter, line 165) — call by 1: `src/render/poleLayer.ts#docPoles`
- `src/world/doc.ts#RoadDoc.removePole` (method, line 654) — call by 1: `src/editor/poles.ts#PoleTool.down.remove`
- `src/world/doc.ts#RoadDoc.spansAt` (method, line 455) — call by 1: `src/render/poleLayer.ts#docPoles`

### src/world/ids.ts

- `src/world/ids.ts#PoleId` (type, line 10) — typeRef by 15: `src/editor/poles.ts#commitPoleRunPoles`, `src/editor/poles.ts#PlannedPole`, `src/editor/poles.ts#PoleSnap`, `src/render/poleLayer.ts#Block`, …
- `src/world/ids.ts#SpanId` (type, line 12) — typeRef by 6: `src/render/poleLayer.ts#buildPolePreview`, `src/render/poleLayer.ts#PoleLayer.redo`, `src/render/poleLayer.ts#PoleLayer.spans`, `src/render/poleLayer.ts#PoleLayer.unstring`, …

### src/world/landscape.ts

- `src/world/landscape.ts#CrossingAccess` (interface, line 227) — read by 1: `src/editor/poles.ts#inLanding`
- `src/world/landscape.ts#CrossingAccess` (interface, line 227) — typeRef by 1: `src/editor/poles.ts#inLanding`

### src/world/network.ts

- `src/world/network.ts#Network` (class, line 46) — typeRef by 19: `src/editor/poles.ts#computePoleRun`, `src/editor/poles.ts#lastPlan`, `src/editor/poles.ts#planPoleRun`, `src/editor/poles.ts#planPoleRunWhole`, …
- `src/world/network.ts#Network.constructor` (constructor, line 100) — read by 1: `src/render/poleLayer.ts#buildUtilities`
- `src/world/network.ts#Network.revision` (field, line 87) — read by 1: `src/editor/poles.ts#planPoleRun`
- `src/world/network.ts#Network.ribbons` (field, line 55) — read by 1: `src/editor/poleRoutes.ts#buildRouteGraph`
- `src/world/network.ts#Network.segmentCells` (field, line 57) — read by 1: `src/editor/poleRoutes.ts#buildRouteGraph`

### src/world/poleIndex.ts

- `src/world/poleIndex.ts#PoleIndex` (class, line 25) — construct by 1: `src/render/poleLayer.ts#PoleLayer.index`
- `src/world/poleIndex.ts#PoleIndex.deletePole` (method, line 63) — call by 1: `src/render/poleLayer.ts#PoleLayer.unplace`
- `src/world/poleIndex.ts#PoleIndex.deleteSpan` (method, line 85) — call by 1: `src/render/poleLayer.ts#PoleLayer.unstring`
- `src/world/poleIndex.ts#PoleIndex.near` (method, line 100) — call by 1: `src/render/poleLayer.ts#PoleLayer.polesIn`
- `src/world/poleIndex.ts#PoleIndex.putPole` (method, line 52) — call by 1: `src/render/poleLayer.ts#PoleLayer.place`
- `src/world/poleIndex.ts#PoleIndex.putSpan` (method, line 77) — call by 1: `src/render/poleLayer.ts#PoleLayer.string`
- `src/world/poleIndex.ts#PoleIndex.spansAt` (method, line 95) — call by 1: `src/render/poleLayer.ts#PoleLayer.redo`

### src/world/poleLines.ts

- `src/world/poleLines.ts#ContourEdges` (interface, line 40) — read by 1: `src/world/poleLines.ts#scratchOf`
- `src/world/poleLines.ts#ContourScratch` (interface, line 73) — read by 2: `src/world/poleLines.ts#nearestOnContour`, `src/world/poleLines.ts#scratchOf`
- `src/world/poleLines.ts#ContourScratch` (interface, line 73) — typeRef by 1: `src/world/poleLines.ts#scratchOf`
- `src/world/poleLines.ts#ContourScratch` (interface, line 73) — write by 2: `src/world/poleLines.ts#nearestOnContour`, `src/world/poleLines.ts#scratchOf`
- `src/world/poleLines.ts#EDGE_BUCKET` (variable, line 42) — read by 1: `src/world/poleLines.ts#nearestOnContour`
- `src/world/poleLines.ts#edgeBucket` (function, line 43) — call by 1: `src/world/poleLines.ts#nearestOnContour`
- `src/world/poleLines.ts#insideLevels` (function, line 318) — call by 1: `src/world/poleLines.ts#onFootway`
- `src/world/poleLines.ts#insideLevels` (function, line 318) — read by 1: `src/world/poleLines.ts#onFootway`
- `src/world/poleLines.ts#POLE_KERB_INSET` (variable, line 26) — read by 1: `src/editor/poleRoutes.ts#buildRouteGraph`
- `src/world/poleLines.ts#PoleLines` (interface, line 28) — read by 2: `src/world/poleLines.ts#nearestOnContour`, `src/world/poleLines.ts#scratchOf`
- `src/world/poleLines.ts#PoleLines` (interface, line 28) — typeRef by 11: `src/editor/poles.ts#graphs`, `src/editor/poles.ts#placeOf`, `src/editor/poles.ts#poleRunPlanner`, `src/editor/poles.ts#poleRunPlanner.tileAt`, …
- `src/world/poleLines.ts#poleLinesAt` (function, line 260) — call by 2: `src/editor/poles.ts#regional`, `src/editor/poles.ts#snapPole`
- `src/world/poleLines.ts#poleLinesIn` (function, line 224) — call by 1: `src/editor/poles.ts#routeRun`
- `src/world/poleLines.ts#poleLinesOf` (function, line 230) — call by 1: `src/editor/poles.ts#poleRunPlanner.tileAt`

### src/world/ribbons.ts

- `src/world/ribbons.ts#SegmentRibbon` (interface, line 25) — read by 1: `src/editor/poleRoutes.ts#buildRouteGraph`

### src/world/roadTypes.ts

- `src/world/roadTypes.ts#CURB_BAND` (variable, line 90) — read by 1: `src/editor/poleRoutes.ts#buildRouteGraph`
- `src/world/roadTypes.ts#Level` (enum, line 10) — reference by 1: `src/editor/poleRoutes.ts#buildRouteGraph`
- `src/world/roadTypes.ts#RoadType` (interface, line 29) — read by 1: `src/editor/poleRoutes.ts#buildRouteGraph`

### src/world/utilities.ts

- `src/world/utilities.ts#DEFAULT_POLE_SPACING` (variable, line 31) — read by 3: `src/editor/poles.ts#planPoleRun`, `src/editor/poles.ts#planPoleRunWhole`, `src/editor/poles.ts#poleRunPlanner.plan`
- `src/world/utilities.ts#MAX_POLE_SPACING` (variable, line 33) — read by 2: `src/editor/poles.ts#computePoleRun`, `src/editor/poleRoutes.ts#buildRouteGraph`
- `src/world/utilities.ts#MIN_POLE_SPACING` (variable, line 32) — read by 1: `src/editor/poles.ts#computePoleRun`
- `src/world/utilities.ts#PoleArms` (interface, line 79) — typeRef by 1: `src/render/poleLayer.ts#PoleState`
- `src/world/utilities.ts#PoleLampMode` (type, line 73) — typeRef by 7: `src/editor/poles.ts#computePoleRun`, `src/editor/poles.ts#lampFor`, `src/editor/poles.ts#lastPlan`, `src/editor/poles.ts#planPoleRun`, …
- `src/world/utilities.ts#UtilityPole` (interface, line 43) — read by 9: `src/editor/poles.ts#commitPoleRunPoles`, `src/editor/poles.ts#computePoleRun`, `src/editor/poles.ts#PoleTool.down.remove`, `src/editor/poles.ts#PoleTool.marks`, …
- `src/world/utilities.ts#UtilityPole` (interface, line 43) — typeRef by 3: `src/render/poleLayer.ts#buildPolePreview`, `src/render/poleLayer.ts#PoleLayer.place`, `src/world/doc.ts#RoadDoc.pole`
- `src/world/utilities.ts#UtilitySpan` (interface, line 51) — read by 4: `src/render/poleLayer.ts#buildPolePreview`, `src/render/poleLayer.ts#PoleLayer.redo`, `src/render/poleLayer.ts#PoleLayer.string`, `src/render/poleLayer.ts#WireStore.add`
- `src/world/utilities.ts#UtilitySpan` (interface, line 51) — typeRef by 5: `src/render/poleLayer.ts#buildPolePreview`, `src/render/poleLayer.ts#PoleLayer.redo`, `src/render/poleLayer.ts#PoleLayer.spans`, `src/render/poleLayer.ts#PoleLayer.string`, …

## External APIs

- `es:Math` × 68
- `es:Array.length` × 36
- `es:Array.push` × 29
- `es:Math.max` × 21
- `es:Map.get` × 16
- `es:Infinity` × 15
- `es:Math.min` × 14
- `es:Float32Array` × 12
- `es:Int32Array` × 12
- `es:Math.floor` × 11
- `es:ReadonlyArray.length` × 11
- `es:Float64Array` × 10
- `es:Math.abs` × 9
- `es:Map` × 8
- `es:Map.set` × 8
- `es:Set` × 8
- `es:Set.add` × 8
- `three:Vector3.x` × 8
- `three:Vector3.y` × 8
- `three:Vector3.z` × 8
- `three:InstancedMesh.instanceMatrix` × 7
- `es:Math.hypot` × 6
- `three:BufferAttribute.array` × 6
- `three:Camera` × 6
- `three:Sphere.center` × 6
- `es:Map.values` × 5
- `three:BufferGeometry.setAttribute` × 5
- `three:InstancedBufferGeometry` × 5
- `three:Vector3.set` × 5
- `es:Array.forEach` × 4
- `es:Array.sort` × 4
- `three:InstancedMesh` × 4
- `es:ArrayLike` × 3
- `es:Float64ArrayConstructor.from` × 3
- `es:Math.ceil` × 3
- `es:ReadonlyMap.get` × 3
- `es:Set.has` × 3
- `es:Uint8Array` × 3
- `three:InstancedInterleavedBuffer` × 3
- `three:InstancedMesh.boundingSphere` × 3
- `three:InterleavedBufferAttribute` × 3
- `three:Mesh` × 3
- `three:Mesh.geometry` × 3
- `three:Object3D` × 3
- `three:Object3D.name` × 3
- `three:Object3D.remove` × 3
- `three:Object3D.visible` × 3
- `three:OrthographicCamera` × 3
- `three:Sphere` × 3
- `three:Sphere.radius` × 3
- `three:Vector3` × 3
- `es:Array` × 2
- `es:Array.map` × 2
- `es:ArrayConstructor.from` × 2
- `es:Float32Array.copyWithin` × 2
- `es:Float32Array.set` × 2
- `es:Float32Array.subarray` × 2
- `es:Map.delete` × 2
- `es:Math.round` × 2
- `es:Math.sqrt` × 2
- `es:Parameters` × 2
- `es:ReadonlyArray.forEach` × 2
- `es:ReadonlyArray.map` × 2
- `es:ReadonlyMap.has` × 2
- `es:ReadonlyMap.values` × 2
- `es:Set.size` × 2
- `three:BufferAttribute.count` × 2
- `three:BufferAttribute.needsUpdate` × 2
- `three:BufferGeometry.boundingSphere` × 2
- `three:BufferGeometry.deleteAttribute` × 2
- `three:BufferGeometry.setIndex` × 2
- `three:InstancedBufferGeometry.instanceCount` × 2
- `three:InstancedMesh.count` × 2
- `three:InstancedMesh.dispose` × 2
- `three:InterleavedBuffer.needsUpdate` × 2
- `three:Matrix4.elements` × 2
- `three:Object3D.add` × 2
- `three:Object3D.castShadow` × 2
- `three:Object3D.onBeforeRender` × 2
- `three:Object3D.onBeforeShadow` × 2
- `three:Object3D.position` × 2
- `three:Object3D.rotation` × 2
- `three:Object3D.scale` × 2
- `three:Vector2.y` × 2
- `three:WebGLRenderer` × 2
- `es:Array.filter` × 1
- `es:Array.indexOf` × 1
- `es:Array.pop` × 1
- `es:Array.reverse` × 1
- `es:Array.slice` × 1
- `es:Float64Array.fill` × 1
- `es:Float64Array.length` × 1
- `es:Int32Array.fill` × 1
- `es:Int32Array.slice` × 1
- `es:Int32ArrayConstructor.from` × 1
- `es:Iterable` × 1
- `es:IterableIterator` × 1
- `es:Map.clear` × 1
- `es:Map.has` × 1
- `es:Map.keys` × 1
- `es:Pick` × 1
- `es:ReadonlyArray.some` × 1
- `es:Set.delete` × 1
- `es:Uint32Array.fill` × 1
- `es:Uint8Array.forEach` × 1
- `es:WeakMap` × 1
- `es:WeakMap.get` × 1
- `es:WeakMap.set` × 1
- `three:BufferAttribute` × 1
- `three:BufferAttribute.addUpdateRange` × 1
- `three:BufferAttribute.clearUpdateRanges` × 1
- `three:BufferGeometry.dispose` × 1
- `three:BufferGeometry.getAttribute` × 1
- `three:BufferGeometry.index` × 1
- `three:Camera.projectionMatrix` × 1
- `three:Euler.set` × 1
- `three:InterleavedBuffer` × 1
- `three:InterleavedBuffer.addUpdateRange` × 1
- `three:InterleavedBuffer.clearUpdateRanges` × 1
- `three:LOD` × 1
- `three:Matrix4.toArray` × 1
- `three:MeshDepthMaterial` × 1
- `three:MeshStandardMaterial` × 1
- `three:Object3D.clear` × 1
- `three:Object3D.customDepthMaterial` × 1
- `three:Object3D.matrix` × 1
- `three:Object3D.matrixWorld` × 1
- `three:Object3D.receiveShadow` × 1
- `three:Object3D.renderOrder` × 1
- `three:Object3D.updateMatrix` × 1
- `three:OrthographicCamera.isOrthographicCamera` × 1
- `three:OrthographicCamera.left` × 1
- `three:OrthographicCamera.right` × 1
- `three:OrthographicCamera.zoom` × 1
- `three:RenderTarget.width` × 1
- `three:Vector2` × 1
- `three:Vector3.length` × 1
- `three:Vector3.normalize` × 1
- `three:WebGLRenderer.getDrawingBufferSize` × 1
- `three:WebGLRenderer.getRenderTarget` × 1
