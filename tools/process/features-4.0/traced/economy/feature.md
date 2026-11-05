# Feature: src/world/economy.ts

The program starts from `src/main.ts`, `src/peopleLab/main.ts`. A declaration is the feature's own when the program reaches it only through the feature; in a declaration of the program that names the feature, its statements are a slice (what names it, what depends on that, what that controls).

- Own: 20 declarations
- Slices of shared code: 20
- Code: 281 lines in 9 files (source/)
- Resources: 9 (9 entry, 0 style, 0 import; 7 only the feature uses)
- Uses: 22 shared declarations (interface/: 53 declarations in 14 files, the types they name included); hooks: 9; external APIs: 21

## Code

- src/editor/commit.ts — part, 17 lines
- src/editor/editRules.ts — part, 5 lines
- src/editor/persistence.ts — part, 3 lines
- src/editor/roads/economy.ts — part, 10 lines
- src/editor/roadTool.ts — part, 16 lines
- src/main.ts — part, 5 lines
- src/ui/roads/catalogPanel.ts — part, 7 lines
- src/world/doc.ts — part, 48 lines
- src/world/economy.ts — whole, 170 lines

## Slices of shared code

- `src/editor/commit.ts#commitRoadPath` — 7 of 97 lines
- `src/editor/commit.ts#layCopy` — 6 of 11 lines
- `src/editor/commit.ts#moveNodeChecked` — 4 of 19 lines
- `src/editor/editRules.ts#guardRoadEdit` — 5 of 15 lines
- `src/editor/persistence.ts#isSerializedDoc` — 3 of 98 lines
- `src/editor/roadTool.ts#RoadTool.layPart` — 16 of 75 lines
- `src/editor/roads/economy.ts#roadsBefore` — 3 of 3 lines
- `src/editor/roads/economy.ts#settleRoadEdit` — 7 of 7 lines
- `src/main.ts#currentStatus` — 5 of 20 lines
- `src/ui/roads/catalogPanel.ts#catalogStrip` — 7 of 89 lines
- `src/world/doc.ts#RoadDoc.economy` — 1 of 1 lines
- `src/world/doc.ts#RoadDoc.fromJSON` — 4 of 187 lines
- `src/world/doc.ts#RoadDoc.layOver` — 3 of 47 lines
- `src/world/doc.ts#RoadDoc.replaceContents` — 7 of 149 lines
- `src/world/doc.ts#RoadDoc.restoreValue` — 5 of 27 lines
- `src/world/doc.ts#RoadDoc.savedGlobals` — 7 of 18 lines
- `src/world/doc.ts#RoadDoc.setBalance` — 4 of 5 lines
- `src/world/doc.ts#RoadDoc.takeFork` — 3 of 49 lines
- `src/world/doc.ts#RoadDoc.valueOf` — 5 of 15 lines
- `src/world/doc.ts#RoadDoc.worldPart` — 5 of 26 lines

## Resources

- feature entry `catalog.meta` — src/ui/i18n/en.ts:757: `'catalog.meta': "{width} m · {price}/m"`
- shared entry `hint.rule.funds` — src/ui/i18n/en.ts:887: `'hint.rule.funds': 'Refused: not enough money to build this. Demolishing roads gives back part of their price.'`
- feature entry `catalog.meta` — src/ui/i18n/pt-BR.ts:750: `'catalog.meta': "{width} m · {price}/m"`
- shared entry `hint.rule.funds` — src/ui/i18n/pt-BR.ts:880: `'hint.rule.funds': 'Recusado: não há dinheiro para construir isto. Demolir vias devolve parte do preço delas.'`
- feature entry `ground` — src/world/roads/tuning.ts:79: `ground: 1`
- feature entry `embankment` — src/world/roads/tuning.ts:79: `embankment: 1.4`
- feature entry `trench` — src/world/roads/tuning.ts:79: `trench: 1.8`
- feature entry `bridge` — src/world/roads/tuning.ts:79: `bridge: 2.5`
- feature entry `tunnel` — src/world/roads/tuning.ts:79: `tunnel: 6`

## Where the program plugs the feature in

### src/world/economy.ts

- `src/world/economy.ts#chargeFor` (function, line 160) ← call from `src/editor/roads/economy.ts#settleRoadEdit`
- `src/world/economy.ts#DEFAULT_ECONOMY` (variable, line 19) ← read from `src/world/doc.ts#RoadDoc.economy`, `src/world/doc.ts#RoadDoc.savedGlobals`, `src/world/doc.ts#RoadDoc.worldPart`
- `src/world/economy.ts#Economy` (interface, line 14) ← read from `src/editor/roads/economy.ts#roadsBefore`, `src/main.ts#currentStatus`, `src/world/doc.ts#RoadDoc.replaceContents`, `src/world/doc.ts#RoadDoc.savedGlobals`, …
- `src/world/economy.ts#Economy` (interface, line 14) ← typeRef from `src/world/doc.ts#RoadDoc.economy`, `src/world/doc.ts#RoadDoc.restoreValue`
- `src/world/economy.ts#pricePerUnit` (function, line 37) ← call from `src/ui/roads/catalogPanel.ts#catalogStrip`
- `src/world/economy.ts#readEconomy` (function, line 22) ← call from `src/editor/persistence.ts#isSerializedDoc`, `src/world/doc.ts#RoadDoc.fromJSON`
- `src/world/economy.ts#RoadCharge` (interface, line 150) ← read from `src/editor/commit.ts#commitRoadPath`, `src/editor/commit.ts#layCopy`, `src/editor/commit.ts#moveNodeChecked`, `src/editor/editRules.ts#guardRoadEdit`, …
- `src/world/economy.ts#RoadCharge` (interface, line 150) ← typeRef from `src/editor/roads/economy.ts#settleRoadEdit`
- `src/world/economy.ts#roadValue` (function, line 109) ← call from `src/editor/roads/economy.ts#roadsBefore`, `src/editor/roads/economy.ts#settleRoadEdit`

## Shared declarations the feature uses

### src/core/polyline.ts

- `src/core/polyline.ts#Polyline.length` (getter, line 123) — call by 1: `src/world/economy.ts#segmentPrice`

### src/core/units.ts

- `src/core/units.ts#METERS_PER_UNIT` (variable, line 2) — read by 1: `src/world/economy.ts#PER_SQUARE_UNIT`

### src/world/changes.ts

- `src/world/changes.ts#Change` (interface, line 35) — read by 1: `src/world/economy.ts#roadValue`
- `src/world/changes.ts#ChangeJournal` (class, line 156) — typeRef by 1: `src/world/economy.ts#Ledger`
- `src/world/changes.ts#ChangeJournal.since` (method, line 309) — call by 1: `src/world/economy.ts#roadValue`
- `src/world/changes.ts#ChangeJournal.version` (getter, line 198) — call by 3: `src/world/economy.ts#priceAll`, `src/world/economy.ts#roadValue`, `src/world/doc.ts#RoadDoc.forkOrigin`

### src/world/doc.ts

- `src/world/doc.ts#RoadDoc` (class, line 153) — typeRef by 6: `src/world/economy.ts#ledgers`, `src/world/economy.ts#priceAll`, `src/world/economy.ts#roadValue`, `src/world/economy.ts#segmentCost`, …
- `src/world/doc.ts#RoadDoc.changes` (field, line 262) — read by 3: `src/world/economy.ts#priceAll`, `src/world/economy.ts#roadValue`, `src/world/doc.ts#RoadDoc.forkOrigin`
- `src/world/doc.ts#RoadDoc.forkedFrom` (field, line 264) — read by 1: `src/world/doc.ts#RoadDoc.forkOrigin`
- `src/world/doc.ts#RoadDoc.node` (method, line 416) — call by 2: `src/world/economy.ts#roadValue`, `src/world/economy.ts#segmentCost`
- `src/world/doc.ts#RoadDoc.segment` (method, line 420) — call by 1: `src/world/economy.ts#roadValue`
- `src/world/doc.ts#RoadDoc.segments` (field, line 155) — read by 1: `src/world/economy.ts#priceAll`
- `src/world/doc.ts#RoadNode` (interface, line 89) — read by 2: `src/world/economy.ts#roadValue`, `src/world/economy.ts#segmentCost`
- `src/world/doc.ts#RoadSegment` (interface, line 124) — read by 2: `src/world/economy.ts#priceAll`, `src/world/economy.ts#segmentCost`
- `src/world/doc.ts#RoadSegment` (interface, line 124) — typeRef by 2: `src/world/economy.ts#segmentCost`, `src/world/economy.ts#segmentPrice`

### src/world/geometry.ts

- `src/world/geometry.ts#segmentPolyline` (function, line 21) — call by 1: `src/world/economy.ts#segmentPrice`

### src/world/ids.ts

- `src/world/ids.ts#SegmentId` (type, line 8) — typeRef by 4: `src/world/economy.ts#Ledger`, `src/world/economy.ts#priceAll`, `src/world/economy.ts#priceOf`, `src/world/economy.ts#roadValue`

### src/world/roads/tuning.ts

- `src/world/roads/tuning.ts#ROAD_TUNING` (variable, line 4) — read by 6: `src/world/economy.ts#buildLengths`, `src/world/economy.ts#chargeFor`, `src/world/economy.ts#DEFAULT_ECONOMY`, `src/world/economy.ts#E`, …

### src/world/roadTypes.ts

- `src/world/roadTypes.ts#roadProfile` (function, line 248) — call by 1: `src/world/economy.ts#segmentCost`
- `src/world/roadTypes.ts#RoadType` (interface, line 29) — read by 1: `src/world/economy.ts#pricePerUnit`
- `src/world/roadTypes.ts#RoadType` (interface, line 29) — typeRef by 1: `src/world/economy.ts#pricePerUnit`

### src/world/structures.ts

- `src/world/structures.ts#RoadStructure` (type, line 5) — typeRef by 1: `src/world/economy.ts#buildLengths`

## External APIs

- `es:Math` × 9
- `es:Math.max` × 4
- `es:Map` × 3
- `es:WeakMap.set` × 3
- `es:Infinity` × 2
- `es:Map.set` × 2
- `es:Math.min` × 2
- `es:Math.round` × 2
- `es:Record` × 2
- `es:Set.add` × 2
- `es:WeakMap.get` × 2
- `es:Array` × 1
- `es:Array.some` × 1
- `es:ArrayConstructor.isArray` × 1
- `es:Map.get` × 1
- `es:Map.values` × 1
- `es:Math.abs` × 1
- `es:Number` × 1
- `es:NumberConstructor.isFinite` × 1
- `es:Set` × 1
- `es:WeakMap` × 1
