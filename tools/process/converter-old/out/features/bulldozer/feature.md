# Feature: src/editor/bulldozer.ts

The program starts from `src/main.ts`, `src/peopleLab/main.ts`. A declaration is the feature's own when the program reaches it only through the feature; in a declaration of the program that names the feature, its statements are a slice (what names it, what depends on that, what that controls).

- Own: 17 declarations
- Slices of shared code: 10
- Code: 238 lines in 3 files (source/)
- Resources: 6 (6 entry, 0 style, 0 import; 0 only the feature uses)
- Uses: 40 shared declarations (interface/: 122 declarations in 28 files, the types they name included); hooks: 9; external APIs: 9

## Code

- src/editor/bulldozer.ts — whole, 151 lines
- src/editor/roads/streetKit.ts — part, 9 lines
- src/main.ts — part, 78 lines

## Slices of shared code

- `src/main.ts#bulldozer` — 10 of 10 lines
- `src/main.ts#cancelGestures` — 3 of 13 lines
- `src/main.ts#currentGesture` — 4 of 18 lines
- `src/main.ts#drawOverlayScreen` — 26 of 469 lines
- `src/main.ts#endPointer` — 3 of 30 lines
- `src/main.ts#gameKeys` — 5 of 77 lines
- `src/main.ts#gestureInProgress` — 3 of 3 lines
- `src/main.ts#pointerDown` — 7 of 180 lines
- `src/main.ts#pointerMove` — 3 of 72 lines
- `src/main.ts#showInspector` — 14 of 246 lines

## Resources

- shared entry `hint.lot.deleted` — src/ui/i18n/en.ts:40: `'hint.lot.deleted': 'Deleted.'`
- shared entry `hint.streetscape.removed` — src/ui/i18n/en.ts:335: `'hint.streetscape.removed': 'Item removed'`
- shared entry `hint.pole.removed` — src/ui/i18n/en.ts:906: `'hint.pole.removed': 'Pole removed'`
- shared entry `hint.lot.deleted` — src/ui/i18n/pt-BR.ts:40: `'hint.lot.deleted': 'Apagado.'`
- shared entry `hint.streetscape.removed` — src/ui/i18n/pt-BR.ts:333: `'hint.streetscape.removed': 'Item removido'`
- shared entry `hint.pole.removed` — src/ui/i18n/pt-BR.ts:899: `'hint.pole.removed': 'Poste removido'`

## Where the program plugs the feature in

### src/editor/bulldozer.ts

- `src/editor/bulldozer.ts#Bulldozer` (class, line 42) ← construct from `src/main.ts#bulldozer`
- `src/editor/bulldozer.ts#Bulldozer.box` (method, line 87) ← call from `src/main.ts#drawOverlayScreen`
- `src/editor/bulldozer.ts#Bulldozer.cancel` (method, line 56) ← call from `src/main.ts#cancelGestures`
- `src/editor/bulldozer.ts#Bulldozer.down` (method, line 61) ← call from `src/main.ts#pointerDown`
- `src/editor/bulldozer.ts#Bulldozer.gesture` (method, line 48) ← call from `src/main.ts#currentGesture`
- `src/editor/bulldozer.ts#Bulldozer.inProgress` (method, line 52) ← call from `src/main.ts#gestureInProgress`
- `src/editor/bulldozer.ts#Bulldozer.move` (method, line 66) ← call from `src/main.ts#pointerMove`
- `src/editor/bulldozer.ts#Bulldozer.up` (method, line 75) ← call from `src/main.ts#endPointer`
- `src/editor/bulldozer.ts#removeRoads` (function, line 28) ← call from `src/main.ts#gameKeys`, `src/main.ts#showInspector`

## Shared declarations the feature uses

### src/core/polyline.ts

- `src/core/polyline.ts#Frame` (interface, line 7) — read by 1: `src/editor/bulldozer.ts#Bulldozer.boxed`
- `src/core/polyline.ts#Polyline.length` (getter, line 123) — call by 1: `src/editor/bulldozer.ts#Bulldozer.boxed`
- `src/core/polyline.ts#Polyline.sampleAt` (method, line 152) — call by 1: `src/editor/bulldozer.ts#Bulldozer.boxed`

### src/core/vec2.ts

- `src/core/vec2.ts#Vec2` (interface, line 4) — read by 4: `src/editor/bulldozer.ts#Bulldozer.box`, `src/editor/bulldozer.ts#Bulldozer.boxed`, `src/editor/bulldozer.ts#Bulldozer.boxed.inside`, `src/editor/bulldozer.ts#Bulldozer.up`
- `src/core/vec2.ts#Vec2` (interface, line 4) — typeRef by 8: `src/editor/bulldozer.ts#Bulldozer.box`, `src/editor/bulldozer.ts#Bulldozer.boxed`, `src/editor/bulldozer.ts#Bulldozer.boxed.inside`, `src/editor/bulldozer.ts#Bulldozer.click`, …

### src/editor/roads/economy.ts

- `src/editor/roads/economy.ts#roadsBefore` (function, line 11) — call by 1: `src/editor/bulldozer.ts#removeRoads`
- `src/editor/roads/economy.ts#settleRoadEdit` (function, line 16) — call by 1: `src/editor/bulldozer.ts#removeRoads`

### src/editor/roads/streetKit.ts

- `src/editor/roads/streetKit.ts#Kit` (interface, line 21) — read by 1: `src/editor/roads/streetKit.ts#removeWithStreetKit`
- `src/editor/roads/streetKit.ts#kitOf` (function, line 41) — call by 1: `src/editor/roads/streetKit.ts#removeWithStreetKit`

### src/editor/snap.ts

- `src/editor/snap.ts#Anchor` (interface, line 16) — read by 1: `src/editor/bulldozer.ts#Bulldozer.click`
- `src/editor/snap.ts#Anchor` (interface, line 16) — typeRef by 3: `src/editor/bulldozer.ts#Bulldozer.click`, `src/editor/bulldozer.ts#Bulldozer.down`, `src/editor/bulldozer.ts#Bulldozer.drag`
- `src/editor/snap.ts#ribbonsNear` (function, line 74) — call by 1: `src/editor/bulldozer.ts#Bulldozer.boxed`

### src/main.ts

- `src/main.ts#bulldozer` (variable, line 1105) — implementation by 4: `src/editor/bulldozer.ts#Bulldozer.boxed`, `src/editor/bulldozer.ts#Bulldozer.click`, `src/editor/bulldozer.ts#Bulldozer.move`, `src/editor/bulldozer.ts#Bulldozer.up`

### src/world/barriers.ts

- `src/world/barriers.ts#Barrier` (interface, line 9) — read by 1: `src/editor/bulldozer.ts#Bulldozer.boxed`

### src/world/buildings/store.ts

- `src/world/buildings/store.ts#BuildingStore.near` (method, line 133) — call by 1: `src/editor/bulldozer.ts#Bulldozer.boxed`
- `src/world/buildings/store.ts#BuildingStore.remove` (method, line 246) — call by 1: `src/editor/bulldozer.ts#Bulldozer.boxed`

### src/world/buildings/types.ts

- `src/world/buildings/types.ts#Building` (interface, line 284) — read by 1: `src/editor/bulldozer.ts#Bulldozer.boxed`

### src/world/carriageway.ts

- `src/world/carriageway.ts#onCarriageway` (function, line 9) — call by 1: `src/editor/bulldozer.ts#Bulldozer.click`

### src/world/doc.ts

- `src/world/doc.ts#RoadDoc` (class, line 153) — typeRef by 3: `src/editor/bulldozer.ts#BulldozerHost`, `src/editor/bulldozer.ts#removeRoads`, `src/editor/roads/streetKit.ts#removeWithStreetKit`
- `src/world/doc.ts#RoadDoc.barriersIn` (method, line 535) — call by 1: `src/editor/bulldozer.ts#Bulldozer.boxed`
- `src/world/doc.ts#RoadDoc.buildings` (field, line 289) — read by 1: `src/editor/bulldozer.ts#Bulldozer.boxed`
- `src/world/doc.ts#RoadDoc.landscapeIn` (method, line 605) — call by 1: `src/editor/bulldozer.ts#Bulldozer.boxed`
- `src/world/doc.ts#RoadDoc.lotsIn` (method, line 321) — call by 1: `src/editor/bulldozer.ts#Bulldozer.boxed`
- `src/world/doc.ts#RoadDoc.poleNear` (method, line 671) — call by 1: `src/editor/bulldozer.ts#Bulldozer.click`
- `src/world/doc.ts#RoadDoc.polesIn` (method, line 450) — call by 1: `src/editor/bulldozer.ts#Bulldozer.boxed`
- `src/world/doc.ts#RoadDoc.pruneOrphanNodes` (method, line 1386) — call by 1: `src/editor/bulldozer.ts#removeRoads`
- `src/world/doc.ts#RoadDoc.removeBarrier` (method, line 521) — call by 1: `src/editor/bulldozer.ts#Bulldozer.boxed`
- `src/world/doc.ts#RoadDoc.removeLandscape` (method, line 596) — call by 3: `src/editor/bulldozer.ts#Bulldozer.boxed`, `src/editor/bulldozer.ts#Bulldozer.click`, `src/editor/roads/streetKit.ts#removeWithStreetKit`
- `src/world/doc.ts#RoadDoc.removePole` (method, line 654) — call by 3: `src/editor/bulldozer.ts#Bulldozer.boxed`, `src/editor/bulldozer.ts#Bulldozer.click`, `src/editor/roads/streetKit.ts#removeWithStreetKit`
- `src/world/doc.ts#RoadDoc.removeSegment` (method, line 767) — call by 1: `src/editor/bulldozer.ts#removeRoads`

### src/world/ids.ts

- `src/world/ids.ts#SegmentId` (type, line 8) — typeRef by 2: `src/editor/bulldozer.ts#removeRoads`, `src/editor/roads/streetKit.ts#removeWithStreetKit`

### src/world/landscape.ts

- `src/world/landscape.ts#LandscapeItem` (interface, line 16) — read by 2: `src/editor/bulldozer.ts#Bulldozer.boxed`, `src/editor/bulldozer.ts#Bulldozer.click`
- `src/world/landscape.ts#landscapeNearIn` (function, line 398) — call by 1: `src/editor/bulldozer.ts#Bulldozer.click`

### src/world/lotRecord.ts

- `src/world/lotRecord.ts#Lot` (interface, line 6) — read by 1: `src/editor/bulldozer.ts#Bulldozer.boxed`

### src/world/lotRings.ts

- `src/world/lotRings.ts#lotCentre` (function, line 56) — call by 1: `src/editor/bulldozer.ts#Bulldozer.boxed`

### src/world/lots.ts

- `src/world/lots.ts#deleteLot` (function, line 1027) — call by 1: `src/editor/bulldozer.ts#Bulldozer.boxed`

### src/world/network.ts

- `src/world/network.ts#Network` (class, line 46) — typeRef by 3: `src/editor/bulldozer.ts#BulldozerHost`, `src/editor/bulldozer.ts#removeRoads`, `src/editor/roads/streetKit.ts#removeWithStreetKit`
- `src/world/network.ts#Network.rebuild` (method, line 119) — call by 1: `src/editor/roads/streetKit.ts#removeWithStreetKit`

### src/world/ribbons.ts

- `src/world/ribbons.ts#SegmentRibbon` (interface, line 25) — read by 1: `src/editor/bulldozer.ts#Bulldozer.boxed`

### src/world/utilities.ts

- `src/world/utilities.ts#UtilityPole` (interface, line 43) — read by 2: `src/editor/bulldozer.ts#Bulldozer.boxed`, `src/editor/bulldozer.ts#Bulldozer.click`

## External APIs

- `es:Array.length` × 7
- `es:Array.filter` × 6
- `es:Array.map` × 6
- `es:Math` × 6
- `es:Math.hypot` × 2
- `es:Math.max` × 2
- `es:Math.min` × 2
- `es:Array.sort` × 1
- `es:ReadonlyArray.some` × 1
