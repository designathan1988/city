# Feature: src/world/economy.ts, src/editor/roads/economy.ts, src/ui/roads/money.ts, src/world/roads/buildMode.ts

The program starts from `src/main.ts`, `src/peopleLab/main.ts`. A declaration is the feature's own when the program reaches it only through the feature; in a declaration of the program that names the feature, its statements are a slice (what names it, what depends on that, what that controls).

- Own: 34 declarations
- Slices of shared code: 32
- Code: 617 lines in 14 files (source/)
- Resources: 39 (39 entry, 0 style, 0 import; 37 only the feature uses)
- Uses: 45 shared declarations (interface/: 221 declarations in 48 files, the types they name included); hooks: 20; external APIs: 30

## Code

- src/editor/bulldozer.ts — part, 6 lines
- src/editor/commit.ts — part, 27 lines
- src/editor/editRules.ts — part, 6 lines
- src/editor/persistence.ts — part, 3 lines
- src/editor/roads/economy.ts — whole, 23 lines
- src/editor/roadTool.ts — part, 35 lines
- src/main.ts — part, 103 lines
- src/render/structures.ts — part, 40 lines
- src/ui/roads/catalogPanel.ts — part, 7 lines
- src/ui/roads/money.ts — whole, 19 lines
- src/ui/v2/shell.ts — part, 4 lines
- src/world/doc.ts — part, 49 lines
- src/world/economy.ts — whole, 170 lines
- src/world/roads/buildMode.ts — whole, 125 lines

## Slices of shared code

- `src/editor/bulldozer.ts#removeRoads` — 6 of 9 lines
- `src/editor/commit.ts#DraftResult` — 3 of 14 lines
- `src/editor/commit.ts#commitRoadPath` — 8 of 97 lines
- `src/editor/commit.ts#draftStations` — 4 of 24 lines
- `src/editor/commit.ts#layCopy` — 7 of 11 lines
- `src/editor/commit.ts#moveNodeChecked` — 5 of 19 lines
- `src/editor/editRules.ts#guardRoadEdit` — 6 of 15 lines
- `src/editor/persistence.ts#isSerializedDoc` — 3 of 98 lines
- `src/editor/roadTool.ts#RoadTool.cost` — 3 of 4 lines
- `src/editor/roadTool.ts#RoadTool.judge` — 3 of 13 lines
- `src/editor/roadTool.ts#RoadTool.judged` — 2 of 2 lines
- `src/editor/roadTool.ts#RoadTool.layPart` — 17 of 75 lines
- `src/editor/roadTool.ts#RoadTool.stations` — 3 of 4 lines
- `src/editor/roadTool.ts#RoadTool.verdict` — 7 of 14 lines
- `src/main.ts#BUILD_FILL` — 7 of 7 lines
- `src/main.ts#buildSummary` — 9 of 9 lines
- `src/main.ts#currentStatus` — 5 of 20 lines
- `src/main.ts#drawBuildModes` — 38 of 39 lines
- `src/main.ts#drawOverlayScreen` — 36 of 469 lines
- `src/main.ts#drawTunnelStretches` — 8 of 15 lines
- `src/render/structures.ts#retainingWalls` — 40 of 58 lines
- `src/ui/roads/catalogPanel.ts#catalogStrip` — 7 of 89 lines
- `src/ui/v2/shell.ts#mountShell.syncMoney` — 4 of 6 lines
- `src/world/doc.ts#RoadDoc.economy` — 1 of 1 lines
- `src/world/doc.ts#RoadDoc.fromJSON` — 4 of 187 lines
- `src/world/doc.ts#RoadDoc.layOver` — 3 of 47 lines
- `src/world/doc.ts#RoadDoc.replaceContents` — 7 of 149 lines
- `src/world/doc.ts#RoadDoc.restoreValue` — 5 of 27 lines
- `src/world/doc.ts#RoadDoc.savedGlobals` — 7 of 18 lines
- `src/world/doc.ts#RoadDoc.takeFork` — 3 of 49 lines
- `src/world/doc.ts#RoadDoc.valueOf` — 5 of 15 lines
- `src/world/doc.ts#RoadDoc.worldPart` — 5 of 26 lines

## Resources

- feature entry `rule.short.sharp` — src/ui/i18n/en.ts:726: `'rule.short.sharp': 'angle too sharp'`
- feature entry `rule.short.squeezed` — src/ui/i18n/en.ts:727: `'rule.short.squeezed': 'stretch too short'`
- feature entry `rule.short.overlap` — src/ui/i18n/en.ts:728: `'rule.short.overlap': 'on another road'`
- feature entry `rule.short.steep` — src/ui/i18n/en.ts:729: `'rule.short.steep': 'too steep'`
- feature entry `rule.short.clearance` — src/ui/i18n/en.ts:730: `'rule.short.clearance': 'no clearance'`
- feature entry `rule.short.funds` — src/ui/i18n/en.ts:731: `'rule.short.funds': 'not enough money'`
- feature entry `buildMode.ground` — src/ui/i18n/en.ts:732: `'buildMode.ground': 'at grade'`
- feature entry `buildMode.embankment` — src/ui/i18n/en.ts:733: `'buildMode.embankment': 'embankment'`
- feature entry `buildMode.cutting` — src/ui/i18n/en.ts:734: `'buildMode.cutting': 'cutting'`
- feature entry `buildMode.bridge` — src/ui/i18n/en.ts:735: `'buildMode.bridge': 'bridge'`
- feature entry `buildMode.tunnel` — src/ui/i18n/en.ts:736: `'buildMode.tunnel': 'tunnel'`
- feature entry `buildMode.cuttingWalls` — src/ui/i18n/en.ts:737: `'buildMode.cuttingWalls': 'walled cutting'`
- feature entry `catalog.meta` — src/ui/i18n/en.ts:757: `'catalog.meta': "{width} m · {price}/m"`
- shared entry `hint.rule.funds` — src/ui/i18n/en.ts:887: `'hint.rule.funds': 'Refused: not enough money to build this. Demolishing roads gives back part of their price.'`
- feature entry `economy.money` — src/ui/i18n/en.ts:888: `'economy.money': '$ {value}'`
- feature entry `economy.balance` — src/ui/i18n/en.ts:889: `'economy.balance': 'Money in hand. Roads cost by width and length; bridges and tunnels more. Demolishing a road gives back {share}% of its price.'`
- feature entry `economy.refund` — src/ui/i18n/en.ts:890: `'economy.refund': 'refund {value}'`
- feature entry `rule.short.sharp` — src/ui/i18n/pt-BR.ts:719: `'rule.short.sharp': 'ângulo fechado'`
- feature entry `rule.short.squeezed` — src/ui/i18n/pt-BR.ts:720: `'rule.short.squeezed': 'trecho curto demais'`
- feature entry `rule.short.overlap` — src/ui/i18n/pt-BR.ts:721: `'rule.short.overlap': 'sobre outra via'`
- feature entry `rule.short.steep` — src/ui/i18n/pt-BR.ts:722: `'rule.short.steep': 'íngreme demais'`
- feature entry `rule.short.clearance` — src/ui/i18n/pt-BR.ts:723: `'rule.short.clearance': 'altura sem vão'`
- feature entry `rule.short.funds` — src/ui/i18n/pt-BR.ts:724: `'rule.short.funds': 'dinheiro insuficiente'`
- feature entry `buildMode.ground` — src/ui/i18n/pt-BR.ts:725: `'buildMode.ground': 'no chão'`
- feature entry `buildMode.embankment` — src/ui/i18n/pt-BR.ts:726: `'buildMode.embankment': 'aterro'`
- feature entry `buildMode.cutting` — src/ui/i18n/pt-BR.ts:727: `'buildMode.cutting': 'trincheira'`
- feature entry `buildMode.bridge` — src/ui/i18n/pt-BR.ts:728: `'buildMode.bridge': 'ponte'`
- feature entry `buildMode.tunnel` — src/ui/i18n/pt-BR.ts:729: `'buildMode.tunnel': 'túnel'`
- feature entry `buildMode.cuttingWalls` — src/ui/i18n/pt-BR.ts:730: `'buildMode.cuttingWalls': 'trincheira com muro'`
- feature entry `catalog.meta` — src/ui/i18n/pt-BR.ts:750: `'catalog.meta': "{width} m · {price}/m"`
- shared entry `hint.rule.funds` — src/ui/i18n/pt-BR.ts:880: `'hint.rule.funds': 'Recusado: não há dinheiro para construir isto. Demolir vias devolve parte do preço delas.'`
- feature entry `economy.money` — src/ui/i18n/pt-BR.ts:881: `'economy.money': 'R$ {value}'`
- feature entry `economy.balance` — src/ui/i18n/pt-BR.ts:882: `'economy.balance': 'Dinheiro em caixa. Vias custam pela largura e pelo comprimento; pontes e túneis custam mais. Demolir uma via devolve {share}% do preço dela.`
- feature entry `economy.refund` — src/ui/i18n/pt-BR.ts:883: `'economy.refund': 'devolve {value}'`
- feature entry `ground` — src/world/roads/tuning.ts:79: `ground: 1`
- feature entry `embankment` — src/world/roads/tuning.ts:79: `embankment: 1.4`
- feature entry `trench` — src/world/roads/tuning.ts:79: `trench: 1.8`
- feature entry `bridge` — src/world/roads/tuning.ts:79: `bridge: 2.5`
- feature entry `tunnel` — src/world/roads/tuning.ts:79: `tunnel: 6`

## Where the program plugs the feature in

### src/editor/roads/economy.ts

- `src/editor/roads/economy.ts#roadsBefore` (function, line 11) ← call from `src/editor/bulldozer.ts#removeRoads`, `src/editor/commit.ts#commitRoadPath`, `src/editor/commit.ts#layCopy`, `src/editor/commit.ts#moveNodeChecked`, …
- `src/editor/roads/economy.ts#settleRoadEdit` (function, line 16) ← call from `src/editor/bulldozer.ts#removeRoads`, `src/editor/commit.ts#commitRoadPath`, `src/editor/commit.ts#layCopy`, `src/editor/commit.ts#moveNodeChecked`, …

### src/ui/roads/money.ts

- `src/ui/roads/money.ts#balanceTip` (function, line 16) ← call from `src/ui/v2/shell.ts#mountShell.syncMoney`
- `src/ui/roads/money.ts#formatCost` (function, line 11) ← call from `src/main.ts#drawOverlayScreen`
- `src/ui/roads/money.ts#formatMoney` (function, line 5) ← call from `src/ui/roads/catalogPanel.ts#catalogStrip`, `src/ui/v2/shell.ts#mountShell.syncMoney`

### src/world/economy.ts

- `src/world/economy.ts#DEFAULT_ECONOMY` (variable, line 19) ← read from `src/world/doc.ts#RoadDoc.economy`, `src/world/doc.ts#RoadDoc.savedGlobals`, `src/world/doc.ts#RoadDoc.worldPart`
- `src/world/economy.ts#Economy` (interface, line 14) ← read from `src/main.ts#currentStatus`, `src/world/doc.ts#RoadDoc.replaceContents`, `src/world/doc.ts#RoadDoc.savedGlobals`, `src/world/doc.ts#RoadDoc.worldPart`
- `src/world/economy.ts#Economy` (interface, line 14) ← typeRef from `src/world/doc.ts#RoadDoc.economy`, `src/world/doc.ts#RoadDoc.restoreValue`
- `src/world/economy.ts#pricePerUnit` (function, line 37) ← call from `src/ui/roads/catalogPanel.ts#catalogStrip`
- `src/world/economy.ts#readEconomy` (function, line 22) ← call from `src/editor/persistence.ts#isSerializedDoc`, `src/world/doc.ts#RoadDoc.fromJSON`
- `src/world/economy.ts#RoadCharge` (interface, line 150) ← read from `src/editor/commit.ts#commitRoadPath`, `src/editor/commit.ts#layCopy`, `src/editor/commit.ts#moveNodeChecked`, `src/editor/editRules.ts#guardRoadEdit`, …

### src/world/roads/buildMode.ts

- `src/world/roads/buildMode.ts#BuildMode` (type, line 10) ← typeRef from `src/main.ts#BUILD_FILL`, `src/main.ts#buildSummary`, `src/main.ts#drawBuildModes`, `src/main.ts#drawOverlayScreen`, …
- `src/world/roads/buildMode.ts#buildModeAt` (function, line 13) ← call from `src/render/structures.ts#retainingWalls`
- `src/world/roads/buildMode.ts#BuildRun` (interface, line 37) ← read from `src/main.ts#buildSummary`
- `src/world/roads/buildMode.ts#buildRuns` (function, line 110) ← call from `src/main.ts#buildSummary`
- `src/world/roads/buildMode.ts#buildRuns` (function, line 110) ← typeRef from `src/main.ts#buildSummary`
- `src/world/roads/buildMode.ts#orderAlong` (function, line 44) ← call from `src/editor/commit.ts#draftStations`
- `src/world/roads/buildMode.ts#RoadStation` (interface, line 24) ← read from `src/main.ts#drawOverlayScreen`
- `src/world/roads/buildMode.ts#RoadStation` (interface, line 24) ← typeRef from `src/editor/commit.ts#DraftResult`, `src/editor/commit.ts#draftStations`, `src/editor/roadTool.ts#RoadTool.judged`, `src/editor/roadTool.ts#RoadTool.stations`
- `src/world/roads/buildMode.ts#stationsAlong` (function, line 82) ← call from `src/editor/commit.ts#draftStations`

## Shared declarations the feature uses

### src/core/polyline.ts

- `src/core/polyline.ts#Frame` (interface, line 7) — read by 1: `src/world/roads/buildMode.ts#stationsAlong`
- `src/core/polyline.ts#Polyline.length` (getter, line 123) — call by 2: `src/world/economy.ts#segmentPrice`, `src/world/roads/buildMode.ts#stationsAlong`
- `src/core/polyline.ts#Polyline.sampleAt` (method, line 152) — call by 1: `src/world/roads/buildMode.ts#stationsAlong`

### src/core/units.ts

- `src/core/units.ts#METERS_PER_UNIT` (variable, line 2) — read by 1: `src/world/economy.ts#PER_SQUARE_UNIT`

### src/core/vec2.ts

- `src/core/vec2.ts#Vec2` (interface, line 4) — read by 2: `src/world/roads/buildMode.ts#orderAlong`, `src/world/roads/buildMode.ts#stationsAlong`
- `src/core/vec2.ts#Vec2` (interface, line 4) — typeRef by 1: `src/world/roads/buildMode.ts#orderAlong`

### src/editor/commit.ts

- `src/editor/commit.ts#draftStations` (function, line 254) — indirectCall by 1: `src/world/roads/buildMode.ts#stationsAlong`

### src/ui/i18n/index.ts

- `src/ui/i18n/index.ts#formatDecimal` (function, line 79) — call by 1: `src/ui/roads/money.ts#formatMoney`
- `src/ui/i18n/index.ts#language` (function, line 50) — call by 1: `src/ui/roads/money.ts#formatMoney`
- `src/ui/i18n/index.ts#t` (function, line 55) — call by 3: `src/ui/roads/money.ts#balanceTip`, `src/ui/roads/money.ts#formatCost`, `src/ui/roads/money.ts#formatMoney`

### src/world/changes.ts

- `src/world/changes.ts#Change` (interface, line 35) — read by 1: `src/world/economy.ts#roadValue`
- `src/world/changes.ts#ChangeJournal` (class, line 156) — typeRef by 1: `src/world/economy.ts#Ledger`
- `src/world/changes.ts#ChangeJournal.record` (method, line 263) — call by 1: `src/world/doc.ts#RoadDoc.setBalance`
- `src/world/changes.ts#ChangeJournal.since` (method, line 309) — call by 1: `src/world/economy.ts#roadValue`
- `src/world/changes.ts#ChangeJournal.version` (getter, line 198) — call by 3: `src/world/economy.ts#priceAll`, `src/world/economy.ts#roadValue`, `src/world/doc.ts#RoadDoc.forkOrigin`

### src/world/doc.ts

- `src/world/doc.ts#RoadDoc` (class, line 153) — typeRef by 9: `src/world/economy.ts#ledgers`, `src/world/economy.ts#priceAll`, `src/world/economy.ts#roadValue`, `src/world/economy.ts#segmentCost`, …
- `src/world/doc.ts#RoadDoc.changes` (field, line 262) — read by 4: `src/world/economy.ts#priceAll`, `src/world/economy.ts#roadValue`, `src/world/doc.ts#RoadDoc.setBalance`, `src/world/doc.ts#RoadDoc.forkOrigin`
- `src/world/doc.ts#RoadDoc.economy` (field, line 220) — read by 2: `src/editor/roads/economy.ts#roadsBefore`, `src/world/doc.ts#RoadDoc.setBalance`
- `src/world/doc.ts#RoadDoc.economy` (field, line 220) — write by 1: `src/world/doc.ts#RoadDoc.setBalance`
- `src/world/doc.ts#RoadDoc.forkedFrom` (field, line 264) — read by 1: `src/world/doc.ts#RoadDoc.forkOrigin`
- `src/world/doc.ts#RoadDoc.node` (method, line 416) — call by 3: `src/world/economy.ts#roadValue`, `src/world/economy.ts#segmentCost`, `src/world/roads/buildMode.ts#orderAlong`
- `src/world/doc.ts#RoadDoc.segment` (method, line 420) — call by 3: `src/world/economy.ts#roadValue`, `src/world/roads/buildMode.ts#orderAlong`, `src/world/roads/buildMode.ts#stationsAlong`
- `src/world/doc.ts#RoadDoc.segments` (field, line 155) — read by 1: `src/world/economy.ts#priceAll`
- `src/world/doc.ts#RoadNode` (interface, line 89) — read by 3: `src/world/economy.ts#roadValue`, `src/world/economy.ts#segmentCost`, `src/world/roads/buildMode.ts#orderAlong`
- `src/world/doc.ts#RoadSegment` (interface, line 124) — read by 4: `src/world/economy.ts#priceAll`, `src/world/economy.ts#segmentCost`, `src/world/roads/buildMode.ts#orderAlong`, `src/world/roads/buildMode.ts#stationsAlong`
- `src/world/doc.ts#RoadSegment` (interface, line 124) — typeRef by 2: `src/world/economy.ts#segmentCost`, `src/world/economy.ts#segmentPrice`

### src/world/elevation.ts

- `src/world/elevation.ts#buildRoadElevation` (function, line 279) — implementation by 1: `src/world/roads/buildMode.ts#stationsAlong`
- `src/world/elevation.ts#RoadElevation` (interface, line 135) — typeRef by 1: `src/world/roads/buildMode.ts#stationsAlong`

### src/world/geometry.ts

- `src/world/geometry.ts#segmentPolyline` (function, line 21) — call by 1: `src/world/economy.ts#segmentPrice`

### src/world/ids.ts

- `src/world/ids.ts#NodeId` (type, line 7) — typeRef by 1: `src/world/roads/buildMode.ts#orderAlong`
- `src/world/ids.ts#SegmentId` (type, line 8) — typeRef by 6: `src/world/economy.ts#Ledger`, `src/world/economy.ts#priceAll`, `src/world/economy.ts#priceOf`, `src/world/economy.ts#roadValue`, …

### src/world/network.ts

- `src/world/network.ts#Network` (class, line 46) — typeRef by 1: `src/world/roads/buildMode.ts#stationsAlong`
- `src/world/network.ts#Network.constructor` (constructor, line 100) — read by 1: `src/world/roads/buildMode.ts#stationsAlong`
- `src/world/network.ts#Network.ribbons` (field, line 55) — read by 1: `src/world/roads/buildMode.ts#stationsAlong`

### src/world/ribbons.ts

- `src/world/ribbons.ts#SegmentRibbon` (interface, line 25) — read by 1: `src/world/roads/buildMode.ts#stationsAlong`

### src/world/roadField.ts

- `src/world/roadField.ts#roadField.onSegment` (method, line 601) — implementation by 1: `src/world/roads/buildMode.ts#stationsAlong`
- `src/world/roadField.ts#RoadReads` (interface, line 96) — call by 1: `src/world/roads/buildMode.ts#stationsAlong`

### src/world/roads/tuning.ts

- `src/world/roads/tuning.ts#ROAD_TUNING` (variable, line 4) — read by 8: `src/world/economy.ts#buildLengths`, `src/world/economy.ts#chargeFor`, `src/world/economy.ts#DEFAULT_ECONOMY`, `src/world/economy.ts#E`, …

### src/world/roadTypes.ts

- `src/world/roadTypes.ts#roadProfile` (function, line 248) — call by 1: `src/world/economy.ts#segmentCost`
- `src/world/roadTypes.ts#RoadType` (interface, line 29) — read by 1: `src/world/economy.ts#pricePerUnit`
- `src/world/roadTypes.ts#RoadType` (interface, line 29) — typeRef by 1: `src/world/economy.ts#pricePerUnit`

### src/world/structures.ts

- `src/world/structures.ts#isRaised` (function, line 68) — call by 1: `src/world/roads/buildMode.ts#buildModeAt`
- `src/world/structures.ts#RAISED_LIFT` (variable, line 65) — read by 1: `src/world/roads/buildMode.ts#buildModeAt`
- `src/world/structures.ts#RoadStructure` (type, line 5) — typeRef by 2: `src/world/economy.ts#buildLengths`, `src/world/roads/buildMode.ts#buildModeAt`
- `src/world/structures.ts#TUNNEL_BORE` (variable, line 38) — read by 1: `src/world/roads/buildMode.ts#buildModeAt`

## External APIs

- `es:Math` × 14
- `es:Math.max` × 5
- `es:Array.length` × 4
- `es:Array.push` × 4
- `es:Map` × 4
- `es:Map.get` × 4
- `es:Math.round` × 4
- `es:Infinity` × 3
- `es:Map.set` × 3
- `es:Set.add` × 3
- `es:WeakMap.set` × 3
- `es:Math.min` × 2
- `es:Number` × 2
- `es:NumberConstructor.isFinite` × 2
- `es:ReadonlyArray.length` × 2
- `es:Record` × 2
- `es:Set` × 2
- `es:WeakMap.get` × 2
- `es:Array` × 1
- `es:Array.find` × 1
- `es:Array.some` × 1
- `es:ArrayConstructor.isArray` × 1
- `es:Map.values` × 1
- `es:Math.abs` × 1
- `es:Math.ceil` × 1
- `es:Math.hypot` × 1
- `es:ReadonlySet` × 1
- `es:ReadonlySet.size` × 1
- `es:Set.has` × 1
- `es:WeakMap` × 1
