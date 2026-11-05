# Feature: src/world/clouds.ts, src/editor/cloudTool.ts

The program starts from `src/main.ts`, `src/peopleLab/main.ts`. A declaration is the feature's own when the program reaches it only through the feature; in a declaration of the program that names the feature, its statements are a slice (what names it, what depends on that, what that controls).

- Own: 25 declarations
- Slices of shared code: 16
- Code: 332 lines in 5 files (source/)
- Resources: 4 (4 entry, 0 style, 0 import; 4 only the feature uses)
- Uses: 14 shared declarations (interface/: 18 declarations in 5 files, the types they name included); hooks: 14; external APIs: 21

## Code

- src/editor/cloudTool.ts — whole, 93 lines
- src/main.ts — part, 56 lines
- src/render/postprocess.ts — part, 21 lines
- src/world/clouds.ts — whole, 111 lines
- src/world/doc.ts — part, 51 lines

## Slices of shared code

- `src/main.ts#<IfStatement>@1703` — 7 of 9 lines
- `src/main.ts#MAP_ACTIONS` — 11 of 20 lines
- `src/main.ts#cloudTool` — 14 of 14 lines
- `src/main.ts#currentGesture` — 4 of 18 lines
- `src/main.ts#endPointer` — 3 of 30 lines
- `src/main.ts#pointerDown` — 10 of 180 lines
- `src/main.ts#pointerMove` — 7 of 72 lines
- `src/render/postprocess.ts#PostChain` — 3 of 30 lines
- `src/render/postprocess.ts#createPostChain` — 4 of 493 lines
- `src/render/postprocess.ts#layClouds` — 14 of 16 lines
- `src/world/doc.ts#RoadDoc.addClouds` — 11 of 14 lines
- `src/world/doc.ts#RoadDoc.clouds` — 1 of 1 lines
- `src/world/doc.ts#RoadDoc.fromJSON` — 8 of 187 lines
- `src/world/doc.ts#RoadDoc.removeCloud` — 5 of 6 lines
- `src/world/doc.ts#listBox` — 8 of 15 lines
- `src/world/doc.ts#placedCloud` — 2 of 2 lines

## Resources

- feature entry `confirm.clearClouds` — src/ui/i18n/en.ts:526: `'confirm.clearClouds': 'Remove every cloud in the sky?'`
- feature entry `hint.cloud.full` — src/ui/i18n/en.ts:528: `'hint.cloud.full': 'The sky already holds the most clouds (24)'`
- feature entry `confirm.clearClouds` — src/ui/i18n/pt-BR.ts:521: `'confirm.clearClouds': 'Apagar todas as nuvens do céu?'`
- feature entry `hint.cloud.full` — src/ui/i18n/pt-BR.ts:523: `'hint.cloud.full': 'O céu já tem o máximo de nuvens (24)'`

## Where the program plugs the feature in

### src/editor/cloudTool.ts

- `src/editor/cloudTool.ts#CloudTool` (class, line 25) ← construct from `src/main.ts#cloudTool`
- `src/editor/cloudTool.ts#CloudTool.down` (method, line 38) ← call from `src/main.ts#pointerDown`
- `src/editor/cloudTool.ts#CloudTool.gesture` (method, line 34) ← call from `src/main.ts#currentGesture`
- `src/editor/cloudTool.ts#CloudTool.move` (method, line 74) ← call from `src/main.ts#pointerMove`
- `src/editor/cloudTool.ts#CloudTool.pointer` (getter, line 30) ← call from `src/main.ts#pointerMove`
- `src/editor/cloudTool.ts#CloudTool.up` (method, line 85) ← call from `src/main.ts#endPointer`

### src/world/clouds.ts

- `src/world/clouds.ts#CLOUD_SPAN` (variable, line 6) ← read from `src/main.ts#<IfStatement>@1703`, `src/main.ts#MAP_ACTIONS`
- `src/world/clouds.ts#driftedCloud` (function, line 101) ← call from `src/render/postprocess.ts#layClouds`
- `src/world/clouds.ts#driftedCloud` (function, line 101) ← read from `src/render/postprocess.ts#layClouds`
- `src/world/clouds.ts#MAX_PLACED_CLOUDS` (variable, line 24) ← read from `src/world/doc.ts#RoadDoc.addClouds`, `src/world/doc.ts#RoadDoc.fromJSON`
- `src/world/clouds.ts#PlacedCloud` (interface, line 8) ← read from `src/main.ts#MAP_ACTIONS`, `src/render/postprocess.ts#layClouds`, `src/world/doc.ts#listBox`, `src/world/doc.ts#placedCloud`, …
- `src/world/clouds.ts#PlacedCloud` (interface, line 8) ← typeRef from `src/render/postprocess.ts#createPostChain`, `src/render/postprocess.ts#layClouds`, `src/render/postprocess.ts#PostChain`, `src/world/doc.ts#listBox`, …
- `src/world/clouds.ts#readCloud` (function, line 36) ← call from `src/world/doc.ts#RoadDoc.fromJSON`
- `src/world/clouds.ts#scatterClouds` (function, line 51) ← call from `src/main.ts#<IfStatement>@1703`, `src/main.ts#MAP_ACTIONS`

## Shared declarations the feature uses

### src/core/units.ts

- `src/core/units.ts#m` (function, line 6) — call by 3: `src/world/clouds.ts#CLOUD_LIMITS`, `src/world/clouds.ts#CLOUD_SPAN`, `src/world/clouds.ts#readCloud`
- `src/core/units.ts#UNITS_PER_METER` (variable, line 3) — read by 1: `src/editor/cloudTool.ts#CloudTool.down`

### src/core/vec2.ts

- `src/core/vec2.ts#Vec2` (interface, line 4) — read by 2: `src/editor/cloudTool.ts#CloudTool.down`, `src/editor/cloudTool.ts#CloudTool.move`
- `src/core/vec2.ts#Vec2` (interface, line 4) — typeRef by 1: `src/editor/cloudTool.ts#CloudToolHost`

### src/main.ts

- `src/main.ts#cloudTool` (variable, line 1137) — implementation by 2: `src/editor/cloudTool.ts#CloudTool.down`, `src/editor/cloudTool.ts#CloudTool.move`

### src/world/changes.ts

- `src/world/changes.ts#ChangeJournal.record` (method, line 263) — call by 2: `src/world/doc.ts#RoadDoc.updateCloud`, `src/world/doc.ts#RoadDoc.addCloud`
- `src/world/changes.ts#rectAround` (function, line 147) — call by 2: `src/world/doc.ts#RoadDoc.updateCloud`, `src/world/doc.ts#RoadDoc.addCloud`

### src/world/doc.ts

- `src/world/doc.ts#placedCloud` (function, line 3314) — call by 1: `src/world/doc.ts#RoadDoc.addCloud`
- `src/world/doc.ts#RoadDoc` (class, line 153) — typeRef by 1: `src/editor/cloudTool.ts#CloudToolHost`
- `src/world/doc.ts#RoadDoc.changes` (field, line 262) — read by 2: `src/world/doc.ts#RoadDoc.updateCloud`, `src/world/doc.ts#RoadDoc.addCloud`
- `src/world/doc.ts#RoadDoc.clouds` (field, line 216) — read by 4: `src/editor/cloudTool.ts#CloudTool.down`, `src/editor/cloudTool.ts#CloudTool.move`, `src/world/doc.ts#RoadDoc.updateCloud`, `src/world/doc.ts#RoadDoc.addCloud`
- `src/world/doc.ts#RoadDoc.clouds` (field, line 216) — write by 2: `src/world/doc.ts#RoadDoc.updateCloud`, `src/world/doc.ts#RoadDoc.addCloud`
- `src/world/doc.ts#RoadDoc.ownList` (method, line 1969) — call by 2: `src/world/doc.ts#RoadDoc.updateCloud`, `src/world/doc.ts#RoadDoc.addCloud`
- `src/world/doc.ts#RoadDoc.removeCloud` (method, line 1297) — call by 1: `src/editor/cloudTool.ts#CloudTool.down`

## External APIs

- `es:Math` × 15
- `es:Math.max` × 5
- `es:Omit` × 4
- `es:Array.length` × 2
- `es:Array.push` × 2
- `es:Math.abs` × 2
- `es:Math.hypot` × 2
- `es:Math.min` × 2
- `es:Math.PI` × 2
- `es:Partial` × 2
- `es:Array.find` × 1
- `es:Array.findIndex` × 1
- `es:Array.map` × 1
- `es:Array.reduce` × 1
- `es:Array.some` × 1
- `es:Infinity` × 1
- `es:Math.round` × 1
- `es:Math.sqrt` × 1
- `es:Number` × 1
- `es:NumberConstructor.isFinite` × 1
- `es:Record` × 1
