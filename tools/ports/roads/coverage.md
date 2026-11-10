# Cobertura do rastreamento: roads

Uma linha por arquivo e por declaração própria de `feature.md` (2.504 declarações, 280 arquivos). Gerado por `coverage.py` a partir de `coverage_rules.json`.
Estados: **portado**, **já existia**, **não se aplica** (com o motivo), **não portado** (ainda falta, dito às claras) e **a portar** (antes de codificar).

| Estado | Arquivos | Declarações próprias |
|---|---|---|
| em parte | 10 | 233 |
| já existia | 1 | 11 |
| não portado | 98 | 1113 |
| não se aplica | 165 | 1045 |
| portado | 6 | 102 |
| total | 280 | 2504 |

## `src/actionsWiring.ts` (parte, 3 linhas) — **não se aplica**

Dono: cabos do 4.0. Só liga módulos; sem declaração própria.


## `src/buildingsWiring.ts` (parte, 3 linhas) — **não se aplica**

Dono: cabos do 4.0. Só liga módulos; sem declaração própria.


## `src/conflictTable.worker.ts` (parte, 14 linhas) — **não se aplica**

Dono: cabos do 4.0. Só liga módulos; sem declaração própria.


## `src/core/aabb.ts` (parte, 6 linhas) — **não se aplica**

Dono: `native/src/core`. Utilitários do 4.0 (índices, mapas, recortes) sem função própria para o jogador. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `expand` | — | não se aplica | `native/src/core` |

## `src/core/axisCut.ts` (inteiro, 232 linhas) — **não se aplica**

Dono: `native/src/core`. Utilitários do 4.0 (índices, mapas, recortes) sem função própria para o jogador. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `CLEAR` | No vertex is allowed nearer the cut than this, in world units. | não se aplica | `native/src/core` |
| `Chain` | A stretch of one ring on one side, from the crossing it enters by to the one it leaves by. | não se aplica | `native/src/core` |
| `Crossing` | A point where a ring crosses the cut line. | não se aplica | `native/src/core` |
| `NUDGE` | How far each retry moves the cut, and how many it makes. | não se aplica | `native/src/core` |
| `TRIES` | — | não se aplica | `native/src/core` |
| `clearLine` | The cut position nearest `at` that clears every vertex by `CLEAR`, or null. | não se aplica | `native/src/core` |
| `contains` | Even-odd point in ring. | não se aplica | `native/src/core` |
| `cutAtAxis` | The parts of `polygon` below and above the line `axis = at` | não se aplica | `native/src/core` |
| `join` | Joins one side's chains into closed rings along the cut. | não se aplica | `native/src/core` |
| `openRing` | The ring without a repeated closing point. | não se aplica | `native/src/core` |
| `sideOnly` | A polygon no ring of which reaches the line: all of it is on one side. | não se aplica | `native/src/core` |
| `signedArea` | Twice the signed area in the (across, along) frame; positive is anticlockwise there. | não se aplica | `native/src/core` |
| `worldWinding` | The clipper's winding, in WORLD axes: outer rings anticlockwise, holes clockwise. | não se aplica | `native/src/core` |

## `src/core/bezier.ts` (parte, 20 linhas) — **não se aplica**

Dono: `native/src/core`. Utilitários do 4.0 (índices, mapas, recortes) sem função própria para o jogador. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `shapeFromControl` | — | não se aplica | `native/src/core` |
| `splitQuad` | — | não se aplica | `native/src/core` |

## `src/core/clipper.ts` (parte, 12 linhas) — **não se aplica**

Dono: `native/src/core`. Utilitários do 4.0 (índices, mapas, recortes) sem função própria para o jogador. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `opened` | — | não se aplica | `native/src/core` |

## `src/core/fillet.ts` (parte, 7 linhas) — **não se aplica**

Dono: `native/src/core`. Utilitários do 4.0 (índices, mapas, recortes) sem função própria para o jogador. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `acuteSetback` | — | não se aplica | `native/src/core` |
| `curbRadius` | — | não se aplica | `native/src/core` |

## `src/core/forkMap.ts` (parte, 63 linhas) — **não se aplica**

Dono: `native/src/core`. Utilitários do 4.0 (índices, mapas, recortes) sem função própria para o jogador. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `ForkSet` | — | não se aplica | `native/src/core` |
| `ForkSet.applyTo` | Writes the fork's changes into `target`, a set holding what the set below held. | não se aplica | `native/src/core` |
| `ForkSet.map` | — | não se aplica | `native/src/core` |

## `src/core/intersect.ts` (parte, 12 linhas) — **não se aplica**

Dono: `native/src/core`. Utilitários do 4.0 (índices, mapas, recortes) sem função própria para o jogador. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `inForwardSlab` | — | não se aplica | `native/src/core` |

## `src/core/offset.ts` (inteiro, 148 linhas) — **não se aplica**

Dono: `native/src/core`. Utilitários do 4.0 (índices, mapas, recortes) sem função própria para o jogador. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `OffsetOptions` | — | não se aplica | `native/src/core` |
| `offsetPolyline` | Offsets an open polyline by a signed distance `d` along `perp(tangent)`. | não se aplica | `native/src/core` |
| `pruneSelfIntersections` | Removes loops from an open chain by walking forward and short-cutting across the first self-intersection found | não se aplica | `native/src/core` |
| `pushArc` | Appends a circular arc of `radius` about `c`, from angle `a0` to `a1`, sweeping the short way. | não se aplica | `native/src/core` |

## `src/core/recentCache.ts` (parte, 29 linhas) — **não se aplica**

Dono: `native/src/core`. Utilitários do 4.0 (índices, mapas, recortes) sem função própria para o jogador. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `RecentCache` | — | não se aplica | `native/src/core` |
| `RecentCache.constructor` | — | não se aplica | `native/src/core` |
| `RecentCache.get` | The value of `key`: the one held, moved to the newest, or made by `make` and held. | não se aplica | `native/src/core` |
| `RecentCache.held` | — | não se aplica | `native/src/core` |

## `src/core/regionIndex.ts` (parte, 520 linhas) — **não se aplica**

Dono: `native/src/core`. Utilitários do 4.0 (índices, mapas, recortes) sem função própria para o jogador. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `CLUSTER_GAP` | — | não se aplica | `native/src/core` |
| `COARSE` | — | não se aplica | `native/src/core` |
| `FillRule` | — | não se aplica | `native/src/core` |
| `Grid` | — | não se aplica | `native/src/core` |
| `Grid.cell` | — | não se aplica | `native/src/core` |
| `Grid.covers` | Whether a point lies in one of its cells. | não se aplica | `native/src/core` |
| `Grid.list` | — | não se aplica | `native/src/core` |
| `Grid.nx` | — | não se aplica | `native/src/core` |
| `Grid.ny` | — | não se aplica | `native/src/core` |
| `Grid.ref` | — | não se aplica | `native/src/core` |
| `Grid.rule` | — | não se aplica | `native/src/core` |
| `Grid.segs` | — | não se aplica | `native/src/core` |
| `Grid.start` | — | não se aplica | `native/src/core` |
| `Grid.winding` | Winding number at a point (evenodd: the crossing count); 0 off every grid. | não se aplica | `native/src/core` |
| `Grid.x0` | — | não se aplica | `native/src/core` |
| `Grid.y0` | — | não se aplica | `native/src/core` |
| `RegionIndex` | — | não se aplica | `native/src/core` |
| `RegionIndex.coarse` | — | não se aplica | `native/src/core` |
| `RegionIndex.contains` | — | não se aplica | `native/src/core` |
| `RegionIndex.fromRings` | From closed rings; each need not repeat its first point. | não se aplica | `native/src/core` |
| `RegionIndex.gridAt` | The grid whose cells hold a point, if any. | não se aplica | `native/src/core` |
| `RegionIndex.grids` | — | não se aplica | `native/src/core` |
| `RegionIndex.inside` | — | não se aplica | `native/src/core` |
| `RegionIndex.lookup` | The clusters' grids by coarse cell (`COARSE` cells a side), for point queries; null with one grid. | não se aplica | `native/src/core` |
| `RegionIndex.rule` | — | não se aplica | `native/src/core` |
| `RegionIndex.winding` | Winding number at a point (evenodd: the crossing count); 0 off every grid. | não se aplica | `native/src/core` |
| `clusters` | — | não se aplica | `native/src/core` |
| `clusters.find` | — | não se aplica | `native/src/core` |
| `clusters.join` | — | não se aplica | `native/src/core` |
| `clusters.overlap` | — | não se aplica | `native/src/core` |
| `coarseKey` | — | não se aplica | `native/src/core` |
| `hashBits` | — | não se aplica | `native/src/core` |
| `hashWords` | — | não se aplica | `native/src/core` |

## `src/core/rerootMap.ts` (inteiro, 105 linhas) — **não se aplica**

Dono: `native/src/core`. Utilitários do 4.0 (índices, mapas, recortes) sem função própria para o jogador. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `GONE` | A key absent from a version (a diff's entry). | não se aplica | `native/src/core` |
| `RerootMap` | One version of a map: the root holds the values; any other holds its differences from the version it points to. | não se aplica | `native/src/core` |
| `RerootMap.data` | Root: the values (never `GONE`). Otherwise: this version's value of each key where it differs from `up`'s. | não se aplica | `native/src/core` |
| `RerootMap.delete` | — | não se aplica | `native/src/core` |
| `RerootMap.derive` | A new version holding this one's values, to be written; this one keeps its values (read through the new one). | não se aplica | `native/src/core` |
| `RerootMap.entries` | — | não se aplica | `native/src/core` |
| `RerootMap.get` | `V` must not be `undefined`: an absent key reads as undefined. | não se aplica | `native/src/core` |
| `RerootMap.note` | The version derived from keeps the key's value before this version's first write of it. | não se aplica | `native/src/core` |
| `RerootMap.prev` | While this version is being written: the version it was derived from, told of each key before its first write. | não se aplica | `native/src/core` |
| `RerootMap.reroot` | Makes this version the root, reversing the differences on the way to it; every version keeps its values. | não se aplica | `native/src/core` |
| `RerootMap.seal` | No more writes: the version is what it holds now, and others may be derived from it. | não se aplica | `native/src/core` |
| `RerootMap.sealed` | — | não se aplica | `native/src/core` |
| `RerootMap.set` | — | não se aplica | `native/src/core` |
| `RerootMap.up` | The version this one is read through; null for the root. | não se aplica | `native/src/core` |
| `RerootMap.writable` | — | não se aplica | `native/src/core` |
| `Slot` | — | não se aplica | `native/src/core` |

## `src/core/ring.ts` (parte, 199 linhas) — **não se aplica**

Dono: `native/src/core`. Utilitários do 4.0 (índices, mapas, recortes) sem função própria para o jogador. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `Ring` | — | não se aplica | `native/src/core` |
| `Ring.edges` | The edges (new objects at each call: the ring keeps numbers). | não se aplica | `native/src/core` |
| `Ring.ensurePositive` | Returns this ring, or a reversed copy, whichever has positive area. | não se aplica | `native/src/core` |
| `Ring.fromPolygon` | Builds a ring of straight edges from a closed point list. | não se aplica | `native/src/core` |
| `Ring.fromRun` | A ring from its run (`run`), the run kept as it is. | não se aplica | `native/src/core` |
| `Ring.reversed` | The same boundary traversed the other way, with arcs kept exact. | não se aplica | `native/src/core` |
| `Ring.run` | The ring as its run of numbers and its edge count, for another thread (`Ring.fromRun`): the run itself, not a copy. | não se aplica | `native/src/core` |
| `Ring.signedArea` | Shoelace area of the flattened ring. Positive means correct winding. | não se aplica | `native/src/core` |
| `Ring.start` | The first vertex (a new point at each call). | não se aplica | `native/src/core` |
| `RingEdge` | — | não se aplica | `native/src/core` |
| `arcEdge` | — | não se aplica | `native/src/core` |
| `lineEdge` | — | não se aplica | `native/src/core` |

## `src/core/simplifyLine.ts` (inteiro, 25 linhas) — **não se aplica**

Dono: `native/src/core`. Utilitários do 4.0 (índices, mapas, recortes) sem função própria para o jogador. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `simplifyLine` | An open line with the points within `tol` of the line through the points kept either side of them taken out, its ends kept (Ramer-Douglas-Peucker: no  | não se aplica | `native/src/core` |

## `src/core/vec2.ts` (parte, 5 linhas) — **não se aplica**

Dono: `native/src/core`. Utilitários do 4.0 (índices, mapas, recortes) sem função própria para o jogador. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `fromAngle` | — | não se aplica | `native/src/core` |
| `neg` | — | não se aplica | `native/src/core` |

## `src/editor/barriers.ts` (parte, 34 linhas) — **não se aplica**

Dono: `street-furniture` (porte-6). Barreiras são de `street-furniture`.


## `src/editor/blocks.ts` (parte, 14 linhas) — **não se aplica**

Dono: `build` (porte-2). Quarteirões são de `build`.


## `src/editor/buildings.ts` (parte, 4 linhas) — **não se aplica**

Dono: `build` (porte-2). Prédios são de `build`.


## `src/editor/buildingSnap.ts` (parte, 65 linhas) — **não se aplica**

Dono: `build` (porte-2). Prédios são de `build`.


## `src/editor/bulldozer.ts` (parte, 45 linhas) — **não se aplica**

Dono: `demolition` (porte-4). Demolição é de `demolition`.


## `src/editor/cityGenerator.ts` (parte, 44 linhas) — **não portado**

Dono: `sim/commands`. O restante de `commit.ts` e `roadTop.ts`: reconciliação fina de nós e faixas. Classificado por pasta: não li cada declaração.


## `src/editor/commit.ts` (inteiro, 1009 linhas) — **em parte**

Dono: `sim/commands/road_commands.cpp`, `road_path.cpp`, `road_move.cpp`. Assentar, cruzar, mover e dividir existem; reconciliação fina de nós, colapso de nós e parte das recusas não foram conferidas uma a uma.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `AUTO_TUNNEL_COVER` | Cover over a road at grade past which it is bored as a tunnel instead of cut | em parte | `sim/commands/road_commands.cpp`, `road_path.cpp`, `road_move.cpp` |
| `CROSSING_CLEARANCE` | Vertical room required before two crossing carriageways can pass independently. | em parte | `sim/commands/road_commands.cpp`, `road_path.cpp`, `road_move.cpp` |
| `CROSS_PAD` | Slack round a piece's box when looking for crossings in it: only ever lets more pairs through to `segSeg`, which decides. | em parte | `sim/commands/road_commands.cpp`, `road_path.cpp`, `road_move.cpp` |
| `DraftResult` | — | em parte | `sim/commands/road_commands.cpp`, `road_path.cpp`, `road_move.cpp` |
| `DraftStop` | — | em parte | `sim/commands/road_commands.cpp`, `road_path.cpp`, `road_move.cpp` |
| `ExistingCut` | — | em parte | `sim/commands/road_commands.cpp`, `road_path.cpp`, `road_move.cpp` |
| `HEIGHT_JOIN_EPS` | Distinct decks at the same map point remain separate networks. | em parte | `sim/commands/road_commands.cpp`, `road_path.cpp`, `road_move.cpp` |
| `InteriorCut` | — | em parte | `sim/commands/road_commands.cpp`, `road_path.cpp`, `road_move.cpp` |
| `JOINT_STRAIGHT` | How far a node may stand off the straight line of its two roads and still be a joint of one straight road. | em parte | `sim/commands/road_commands.cpp`, `road_path.cpp`, `road_move.cpp` |
| `MERGE_EPS` | Two nodes closer than this are the same node. | em parte | `sim/commands/road_commands.cpp`, `road_path.cpp`, `road_move.cpp` |
| `MIN_DRAFT_LENGTH` | Shortest road the editor will create. | em parte | `sim/commands/road_commands.cpp`, `road_path.cpp`, `road_move.cpp` |
| `STATION_STEP` | Spacing of a dry run's stations (`DraftResult.stations`). | em parte | `sim/commands/road_commands.cpp`, `road_path.cpp`, `road_move.cpp` |
| `TaggedCut` | — | em parte | `sim/commands/road_commands.cpp`, `road_path.cpp`, `road_move.cpp` |
| `alreadyJoined` | — | em parte | `sim/commands/road_commands.cpp`, `road_path.cpp`, `road_move.cpp` |
| `boreDeepCuts` | — | em parte | `sim/commands/road_commands.cpp`, `road_path.cpp`, `road_move.cpp` |
| `commitDraft` | Commits a drafted road atomically, splitting anything it crosses. | em parte | `sim/commands/road_commands.cpp`, `road_path.cpp`, `road_move.cpp` |
| `commitDraftInPlace` | — | em parte | `sim/commands/road_commands.cpp`, `road_path.cpp`, `road_move.cpp` |
| `commitRoadPath` | Commits one freehand gesture as one undoable, atomic road. | em parte | `sim/commands/road_commands.cpp`, `road_path.cpp`, `road_move.cpp` |
| `commitRoadPath.timed` | — | em parte | `sim/commands/road_commands.cpp`, `road_path.cpp`, `road_move.cpp` |
| `curveShapeForRange` | — | em parte | `sim/commands/road_commands.cpp`, `road_path.cpp`, `road_move.cpp` |
| `draftStations` | The stations of the road a gesture laid, on the copy it was laid on: the new segments that lie on the drawn pieces | em parte | `sim/commands/road_commands.cpp`, `road_path.cpp`, `road_move.cpp` |
| `dryRunKey` | The road asked, as a key: its ends, class, pieces (joined), lanes, parking, and whether the ground was handed in. | em parte | `sim/commands/road_commands.cpp`, `road_path.cpp`, `road_move.cpp` |
| `dryRunsReused` | Roads laid from their dry run's copy, for the tests. | em parte | `sim/commands/road_commands.cpp`, `road_path.cpp`, `road_move.cpp` |
| `duplicateSegment` | Creates a disconnected parallel copy of a road for rapid layout iteration. | em parte | `sim/commands/road_commands.cpp`, `road_path.cpp`, `road_move.cpp` |
| `joinSegments` | Joins two compatible straight segments meeting at an otherwise unused node. | em parte | `sim/commands/road_commands.cpp`, `road_path.cpp`, `road_move.cpp` |
| `joinShortPieces` | A piece too short to be a road is folded into the next one (the last into the one before) | em parte | `sim/commands/road_commands.cpp`, `road_path.cpp`, `road_move.cpp` |
| `lastDryRun` | — | em parte | `sim/commands/road_commands.cpp`, `road_path.cpp`, `road_move.cpp` |
| `layCopy` | The road worked out on the copy, paid for on it (so the live map takes the balance with the roads and undo gives it back) and taken back. | em parte | `sim/commands/road_commands.cpp`, `road_path.cpp`, `road_move.cpp` |
| `materializeEndpoints` | Materializes both anchors together so two cuts on one segment stay valid. | em parte | `sim/commands/road_commands.cpp`, `road_path.cpp`, `road_move.cpp` |
| `materializeEndpoints.queue` | — | em parte | `sim/commands/road_commands.cpp`, `road_path.cpp`, `road_move.cpp` |
| `materializeFree` | The node a free anchor lands on. | em parte | `sim/commands/road_commands.cpp`, `road_path.cpp`, `road_move.cpp` |
| `moveNodeChecked` | A node dropped by the Move tool at `to`, reconciled with the network | em parte | `sim/commands/road_commands.cpp`, `road_path.cpp`, `road_move.cpp` |
| `normalizeDraftStops` | — | em parte | `sim/commands/road_commands.cpp`, `road_path.cpp`, `road_move.cpp` |
| `onTerrain` | The terrain's index as last built, and the last solve on it (no ground handed in): a road edit does not move the land, so the next reuses both. | em parte | `sim/commands/road_commands.cpp`, `road_path.cpp`, `road_move.cpp` |
| `parameterAtArc` | — | em parte | `sim/commands/road_commands.cpp`, `road_path.cpp`, `road_move.cpp` |
| `quadRange` | Returns the de Casteljau control triple over the original [t0,t1] range. | em parte | `sim/commands/road_commands.cpp`, `road_path.cpp`, `road_move.cpp` |
| `reconcileMovedNode` | Makes the topology agree with a node the Move tool has just dropped. | em parte | `sim/commands/road_commands.cpp`, `road_path.cpp`, `road_move.cpp` |
| `segmentOffsetAt` | — | em parte | `sim/commands/road_commands.cpp`, `road_path.cpp`, `road_move.cpp` |
| `slideOntoCrossing` | Brings an existing node the draft crossed near onto the crossing, along its own road | em parte | `sim/commands/road_commands.cpp`, `road_path.cpp`, `road_move.cpp` |
| `slideOntoCrossing.straight` | — | em parte | `sim/commands/road_commands.cpp`, `road_path.cpp`, `road_move.cpp` |
| `solveOver` | A solve of `net` over `ground`: from the game's last on the same natural ground, or from the last on the same index | em parte | `sim/commands/road_commands.cpp`, `road_path.cpp`, `road_move.cpp` |
| `splitSegment` | Splits a segment at an arc position, returning the new or endpoint node. | em parte | `sim/commands/road_commands.cpp`, `road_path.cpp`, `road_move.cpp` |
| `splitSegmentAtCuts` | Splits one original segment at any number of positions. | em parte | `sim/commands/road_commands.cpp`, `road_path.cpp`, `road_move.cpp` |
| `terrainIndexOf` | — | em parte | `sim/commands/road_commands.cpp`, `road_path.cpp`, `road_move.cpp` |
| `validPoint` | — | em parte | `sim/commands/road_commands.cpp`, `road_path.cpp`, `road_move.cpp` |

## `src/editor/editRules.ts` (inteiro, 285 linhas) — **em parte**

Dono: `sim/commands/road_plan.cpp`, `road_rules.cpp`. Ângulo e trecho apertado. Rampa e vão ficam em `road-elevation`; a prévia não avisa do trecho apertado

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `GAP_NOISE` | A node's smallest leg gap may shrink this much before it counts as made sharper | em parte | `sim/commands/road_plan.cpp`, `road_rules.cpp` |
| `GRADE_RUN` | — | em parte | `sim/commands/road_plan.cpp`, `road_rules.cpp` |
| `GRADE_STEP` | Spacing of the deck samples a grade is read from, and the run it is measured over (0.4 m). | em parte | `sim/commands/road_plan.cpp`, `road_rules.cpp` |
| `MAX_BUILT_GRADE` | The steepest grade a road is refused beyond: Baldwin Street, Dunedin, the steepest street in the world at 34.8 % | em parte | `sim/commands/road_plan.cpp`, `road_rules.cpp` |
| `OVERLAP_SLACK` | Overlap shallower than this is a kerb grazing a kerb, not a road on a road (0.2 m). | em parte | `sim/commands/road_plan.cpp`, `road_rules.cpp` |
| `PASS_CLEARANCE` | Vertical room for one road to pass over another | em parte | `sim/commands/road_plan.cpp`, `road_rules.cpp` |
| `RoadEditRefusal` | What a road edit may not leave behind, judged on the network it builds. | em parte | `sim/commands/road_plan.cpp`, `road_rules.cpp` |
| `RoadState` | — | em parte | `sim/commands/road_plan.cpp`, `road_rules.cpp` |
| `SAME_LEVEL` | Two decks within this height of each other are at one level (`commit.ts` `HEIGHT_JOIN_EPS`). | em parte | `sim/commands/road_plan.cpp`, `road_rules.cpp` |
| `SAME_PLACE` | A point within this of an old centreline lay on that road before the edit. | em parte | `sim/commands/road_plan.cpp`, `road_rules.cpp` |
| `SAMPLE_STEP` | Spacing of the samples along a changed road (0.8 m). | em parte | `sim/commands/road_plan.cpp`, `road_rules.cpp` |
| `SQUEEZE_SLACK` | A squeeze may deepen this much (0.4 m) before it counts as made worse. | em parte | `sim/commands/road_plan.cpp`, `road_rules.cpp` |
| `changedSegments` | Segments of `after` that are new, or whose shape, class, structure or end points changed. | em parte | `sim/commands/road_plan.cpp`, `road_rules.cpp` |
| `deckAt` | The deck's authored height along a segment at arc length `s` (as `commit.ts` `segmentOffsetAt`). | em parte | `sim/commands/road_plan.cpp`, `road_rules.cpp` |
| `flatSolves` | The roads' heights before and after, over flat land, each solved once and only when a rule asks | em parte | `sim/commands/road_plan.cpp`, `road_rules.cpp` |
| `flatSolves.after` | — | em parte | `sim/commands/road_plan.cpp`, `road_rules.cpp` |
| `flatSolves.beforeSolve` | — | em parte | `sim/commands/road_plan.cpp`, `road_rules.cpp` |
| `grade` | The steepest grade of a segment's deck as the game solves it (rise over run, over `GRADE_RUN`) | em parte | `sim/commands/road_plan.cpp`, `road_rules.cpp` |
| `guardRoadEdit` | DO NOT copy the map to judge an edit against (`doc.clone()` and the whole network, every in-place edit) | em parte | `sim/commands/road_plan.cpp`, `road_rules.cpp` |
| `overlapReach` | How close two roads' centrelines may come before one's carriageway lies on the other's carriageway or footway. | em parte | `sim/commands/road_plan.cpp`, `road_rules.cpp` |
| `overlappedBefore` | The same two places already overlapped before the edit | em parte | `sim/commands/road_plan.cpp`, `road_rules.cpp` |
| `overlapsAnother` | Whether a changed segment's roadway lies on another road's carriageway | em parte | `sim/commands/road_plan.cpp`, `road_rules.cpp` |
| `refuseRoadEdit` | The reason to refuse turning `before` into `after`, or null when the edit may stand. | em parte | `sim/commands/road_plan.cpp`, `road_rules.cpp` |
| `refuseRoadEdit.deck` | — | em parte | `sim/commands/road_plan.cpp`, `road_rules.cpp` |
| `sameEnd` | — | em parte | `sim/commands/road_plan.cpp`, `road_rules.cpp` |
| `sameSegment` | Whether a segment's shape, class, structure and end points are the same in both documents (no text written per road). | em parte | `sim/commands/road_plan.cpp`, `road_rules.cpp` |
| `segmentsThrough` | DO NOT walk `state.doc.segments`: this runs for every sample of every changed road. | em parte | `sim/commands/road_plan.cpp`, `road_rules.cpp` |
| `tooSteep` | Whether the edit leaves a ramp steeper than any street, on a road it laid or on one meeting it | em parte | `sim/commands/road_plan.cpp`, `road_rules.cpp` |
| `withinSharedPlate` | The other road's closest point lies on its own junction plate at a node the two share. | em parte | `sim/commands/road_plan.cpp`, `road_rules.cpp` |
| `withinSharedPlate.shares` | — | em parte | `sim/commands/road_plan.cpp`, `road_rules.cpp` |
| `written` | The marks of `after` that may differ from `before`'s: those its working copy (`Network.fork`) wrote | em parte | `sim/commands/road_plan.cpp`, `road_rules.cpp` |

## `src/editor/growthFeed.ts` (parte, 131 linhas) — **não se aplica**

Dono: `build` (porte-2). Crescimento é de `build`.


## `src/editor/growthReplica.ts` (parte, 19 linhas) — **não se aplica**

Dono: `build` (porte-2). Crescimento é de `build`.


## `src/editor/history.ts` (parte, 10 linhas) — **não portado**

Dono: `sim/commands`. O restante de `commit.ts` e `roadTop.ts`: reconciliação fina de nós e faixas. Classificado por pasta: não li cada declaração.


## `src/editor/lotTool.ts` (parte, 47 linhas) — **não se aplica**

Dono: `build` (porte-2). Lotes são de `build`.


## `src/editor/nodeMover.ts` (inteiro, 121 linhas) — **portado**

Dono: `project/game/tools/move_tool.gd`, `sim/commands/road_move.cpp`. Prévia é a faixa, não a via reconstruída; o kit de rua segue pelo perfil

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `NodeMover` | DO NOT keep the whole document (`toJSON` at the press, `replaceFromJSON` at the release) | portado | `project/game/tools/move_tool.gd`, `sim/commands/road_move.cpp` |
| `NodeMover.cancel` | The drag cut short: the node and its roads as they were before it. | portado | `project/game/tools/move_tool.gd`, `sim/commands/road_move.cpp` |
| `NodeMover.constructor` | — | portado | `project/game/tools/move_tool.gd`, `sim/commands/road_move.cpp` |
| `NodeMover.drag` | — | portado | `project/game/tools/move_tool.gd`, `sim/commands/road_move.cpp` |
| `NodeMover.dragging` | — | portado | `project/game/tools/move_tool.gd`, `sim/commands/road_move.cpp` |
| `NodeMover.drop` | The pointer let go: the move recorded as one undo step (`commit` false: dropped). | portado | `project/game/tools/move_tool.gd`, `sim/commands/road_move.cpp` |
| `NodeMover.gesture` | The gesture in progress, as a dictionary key (the interface translates it), or null. | portado | `project/game/tools/move_tool.gd`, `sim/commands/road_move.cpp` |
| `NodeMover.grab` | A press on `node`: the drag begins (false when there is no such node). | portado | `project/game/tools/move_tool.gd`, `sim/commands/road_move.cpp` |
| `NodeMover.lastPreview` | — | portado | `project/game/tools/move_tool.gd`, `sim/commands/road_move.cpp` |
| `NodeMover.move` | @perInput The pointer moved: the node follows it, live. | portado | `project/game/tools/move_tool.gd`, `sim/commands/road_move.cpp` |
| `NodeMover.previewDue` | Whether the network may be rebuilt for the preview at `now` (ms): no sooner than the last rebuild's own cost allows. | portado | `project/game/tools/move_tool.gd`, `sim/commands/road_move.cpp` |
| `NodeMover.putBack` | The node back where the drag found it, each of its roads with the curve it had then; the network after them. | portado | `project/game/tools/move_tool.gd`, `sim/commands/road_move.cpp` |
| `NodeMoverHost` | What the node mover reads of the game and does to it. | portado | `project/game/tools/move_tool.gd`, `sim/commands/road_move.cpp` |
| `PREVIEW_MIN_MS` | Shortest interval between geometry rebuilds while a node is dragged | portado | `project/game/tools/move_tool.gd`, `sim/commands/road_move.cpp` |

## `src/editor/persistence.ts` (parte, 15 linhas) — **não portado**

Dono: `sim/commands`. O restante de `commit.ts` e `roadTop.ts`: reconciliação fina de nós e faixas. Classificado por pasta: não li cada declaração.


## `src/editor/poleRoutes.ts` (parte, 25 linhas) — **não se aplica**

Dono: `street-furniture` (porte-6). Postes são de `street-furniture`.


## `src/editor/poles.ts` (parte, 122 linhas) — **não se aplica**

Dono: `street-furniture` (porte-6). Postes são de `street-furniture`.


## `src/editor/repair.ts` (parte, 38 linhas) — **não portado**

Dono: `sim/commands`. O restante de `commit.ts` e `roadTop.ts`: reconciliação fina de nós e faixas. Classificado por pasta: não li cada declaração.


## `src/editor/roadParking.ts` (inteiro, 41 linhas) — **portado**

Dono: `sim/roads/road_variants.cpp`, `project/ui/road_options.gd`. Os seis conjuntos de vagas

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `<TryStatement>` | — | portado | `sim/roads/road_variants.cpp`, `project/ui/road_options.gd` |
| `KEY` | — | portado | `sim/roads/road_variants.cpp`, `project/ui/road_options.gd` |
| `ROAD_PARKING_PRESETS` | The parking a new road is drawn with, chosen in the road tool's options as Cities: Skylines II offers road variants | portado | `sim/roads/road_variants.cpp`, `project/ui/road_options.gd` |
| `RoadParkingPreset` | — | portado | `sim/roads/road_variants.cpp`, `project/ui/road_options.gd` |
| `preset` | — | portado | `sim/roads/road_variants.cpp`, `project/ui/road_options.gd` |
| `roadParking` | The parking of a road drawn with the current preset, or undefined for none. | portado | `sim/roads/road_variants.cpp`, `project/ui/road_options.gd` |
| `roadParkingPreset` | — | portado | `sim/roads/road_variants.cpp`, `project/ui/road_options.gd` |
| `setRoadParkingPreset` | — | portado | `sim/roads/road_variants.cpp`, `project/ui/road_options.gd` |

## `src/editor/roadPath.ts` (inteiro, 109 linhas) — **portado**

Dono: `native/src/sim/commands/road_path.cpp`. Simplificação Ramer-Douglas-Peucker e cadeia de quadráticas tangentes, sem a altura por ponto, que entra com `road-elevation`. Peça menor que a via mínima passa a dobra à seguinte (tratado aqui; no 4.0 é `joinShortPieces`).

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `MAX_CONTROLS` | — | portado | `native/src/sim/commands/road_path.cpp` |
| `MIN_CONTROL_SPACING` | — | portado | `native/src/sim/commands/road_path.cpp` |
| `RoadPathPiece` | One quadratic in a continuous road gesture. Neighbouring pieces share an end. | portado | `native/src/sim/commands/road_path.cpp` |
| `RoadPathPoint` | An authored point. Heights are offsets from the designed ground in world units. | portado | `native/src/sim/commands/road_path.cpp` |
| `distanceToChord` | — | portado | `native/src/sim/commands/road_path.cpp` |
| `midpoint` | — | portado | `native/src/sim/commands/road_path.cpp` |
| `roadPathFromGesture` | Converts a freehand gesture into a chain of tangent-continuous quadratics. | portado | `native/src/sim/commands/road_path.cpp` |
| `simplify` | Keeps the path's turns and every requested height change without storing pointer noise. | portado | `native/src/sim/commands/road_path.cpp` |
| `simplify.visit` | — | portado | `native/src/sim/commands/road_path.cpp` |

## `src/editor/roads/economy.ts` (inteiro, 23 linhas) — **não portado**

Dono: `sim/commands`. Falta conferir. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `RoadsBefore` | Paying for a road edit (`world/economy.ts`) | não portado | `sim/commands` |
| `roadsBefore` | What the roads are worth and the money in hand, before an edit. | não portado | `sim/commands` |
| `settleRoadEdit` | The charge for the edit `after` holds against `before`, written into `after` when it can be paid | não portado | `sim/commands` |

## `src/editor/roads/powerLine.ts` (inteiro, 95 linhas) — **não se aplica**

Dono: `street-furniture` (porte-6). Também é entrada de `street-furniture`, que porta mobiliário, travessias, retornos e rede.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `BATCH` | Streets in one call past which they are planned over shared tiles (`poleRunPlanner`). | não se aplica | `street-furniture` (porte-6) |
| `BURIED` | The deepest a footway may lie under the designed ground and still carry poles: deeper, a pole stands in the bore and only its top shows above ground. | não se aplica | `street-furniture` (porte-6) |
| `END_SETBACK` | THE POWER LINE OF A NEW STREET | não se aplica | `street-furniture` (porte-6) |
| `MIN_RUN` | — | não se aplica | `street-furniture` (porte-6) |
| `PlanRun` | How one street's run is planned: its two ends, with the generator's reach, spacing and lamps. | não se aplica | `street-furniture` (porte-6) |
| `SNAP_REACH` | — | não se aplica | `street-furniture` (porte-6) |
| `STREETS_A_STEP` | Segments strung between two yields of `layPowerLinesSteps`. | não se aplica | `street-furniture` (porte-6) |
| `layPowerLines` | — | não se aplica | `street-furniture` (porte-6) |
| `layPowerLinesSteps` | — | não se aplica | `street-furniture` (porte-6) |
| `layPowerLinesSteps.join` | — | não se aplica | `street-furniture` (porte-6) |
| `openStretch` | The share of a road (0 at a, 1 at b) whose deck is not buried, its height running from its ends' authored offsets: null when it all is (a tunnel drawn | não se aplica | `street-furniture` (porte-6) |
| `runPlanner` | The planning of a call's runs. | não se aplica | `street-furniture` (porte-6) |

## `src/editor/roads/profile.ts` (inteiro, 35 linhas) — **não portado**

Dono: `sim/commands`. Falta conferir. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `ApplyResult` | Applying a profile to roads already built (: "apply without demolishing") | não portado | `sim/commands` |
| `applyProfileTo` | — | não portado | `sim/commands` |

## `src/editor/roads/streetKit.ts` (inteiro, 91 linhas) — **não se aplica**

Dono: `street-furniture` (porte-6). Também é entrada de `street-furniture`, que porta mobiliário, travessias, retornos e rede.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `BEYOND` | Past the outer edge of the footway, what still belongs to the street (a pole or a tree pit at its edge). | não se aplica | `street-furniture` (porte-6) |
| `Kit` | What stood on the streets before the edit: their poles and pieces, and the set to lay again. | não se aplica | `street-furniture` (porte-6) |
| `WITHIN` | Inside the kerb, what still counts as the footway's (a lamp leaning on the kerb face). | não se aplica | `street-furniture` (porte-6) |
| `carried` | The pieces a street is furnished with (`furnitureSets.ts`): what an edit carries; a sign or a name plate stays where it was put. | não se aplica | `street-furniture` (porte-6) |
| `carryStreetKit` | Runs a road edit of `segments` (`edit`, true when it changed the map), then lays their power line and furniture again on the roads it left (`after`: t | não se aplica | `street-furniture` (porte-6) |
| `kitOf` | The poles and pieces standing on `segments`' footways, each street's own (nearer to it than to any other). | não se aplica | `street-furniture` (porte-6) |
| `kitOf.owned` | — | não se aplica | `street-furniture` (porte-6) |
| `nearestStreet` | The street nearest to `p` among `net`'s, and how far and where along it `p` is. | não se aplica | `street-furniture` (porte-6) |
| `removeWithStreetKit` | Runs an edit that takes `segments` away (`edit`, true when it changed the map), and their power line and furniture with them: nothing of a street left | não se aplica | `street-furniture` (porte-6) |

## `src/editor/roadTool.ts` (inteiro, 600 linhas) — **em parte**

Dono: `project/game/tools/road_tool.gd`. Arrastar, encadear, curva com dobra, livre, prévia e obra atômica portados. Faltam altura (PgUp/PgDn), largura, vagas, mobiliário e nomes ao assentar, muro de corte, piscar da via nova e a via desenhada como prévia até o mundo ficar pronto.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `CurvePending` | A curve with its end put down, waiting for the click that sets its bend. | portado | `project/game/tools/road_tool.gd` + `road_path.cpp` |
| `DENSE_SAMPLES` | — | portado | `project/game/tools/road_tool.gd` + `road_path.cpp` |
| `DraftKey` | A draft's numbers as it is judged (`RoadTool.draftKey`): the same array while none of them moved. | não se aplica | o plano roda em C++ a cada movimento; a espera existia para o ensaio caro do 4.0. Ainda sem medição do custo com 48 peças |
| `FURNITURE_CELL` | The side of `furnitureFor`'s cells (`world/roads/furnitureSets.ts` CELL). | em parte | `project/game/tools/road_tool.gd` |
| `MOST_SAMPLES` | — | portado | `project/game/tools/road_tool.gd` + `road_path.cpp` |
| `REFUSALS` | — | em parte | `project/game/tools/road_tool.gd` |
| `RefusalReason` | A refusal the preview names: the editing rules', or a crossing at the wrong height. | em parte | `project/game/tools/road_tool.gd` |
| `RoadDraft` | A road being drawn: a drag, or a stretch chained from the last road's end. | portado | `project/game/tools/road_tool.gd` + `road_path.cpp` |
| `RoadTool` | THE ROAD TOOL: a road drawn by a drag | portado | `project/game/tools/road_tool.gd` + `road_path.cpp` |
| `RoadTool.anchorHeight` | Height of an authored connection; open ground takes the height being drawn at. | não portado | `road-elevation` (próxima desta sessão) |
| `RoadTool.atHeight` | A nearby road at another height is a crossing, not an accidental junction. | não portado | `road-elevation` (próxima desta sessão) |
| `RoadTool.bend` | A press at `world` while a curve waits for its bend: the curve is laid bent there. True when there was one. | portado | `project/game/tools/road_tool.gd` + `road_path.cpp` |
| `RoadTool.bending` | A curve's end is down and its bend follows the pointer. | portado | `project/game/tools/road_tool.gd` + `road_path.cpp` |
| `RoadTool.cancel` | — | portado | `project/game/tools/road_tool.gd` + `road_path.cpp` |
| `RoadTool.chain` | The end of the last stretch, while the road is being chained. | portado | `project/game/tools/road_tool.gd` + `road_path.cpp` |
| `RoadTool.chainHeight` | — | não portado | `road-elevation` (próxima desta sessão) |
| `RoadTool.chainPreview` | — | em parte | `project/game/tools/road_tool.gd` |
| `RoadTool.commit` | Lays the road `d` ends at (`chosenEnd`, or what is under its end), one undo step; false when refused. | portado | `project/game/tools/road_tool.gd` + `road_path.cpp` |
| `RoadTool.constructor` | — | em parte | `project/game/tools/road_tool.gd` |
| `RoadTool.cost` | What the road in hand would cost (`world/economy.ts`), judged with the verdict on the same dry run, or null until it is | portado | `project/game/tools/road_tool.gd` + `road_path.cpp` |
| `RoadTool.curveOf` | The curve of a draft: through its control point, or bent as far as the drag went off the chord. | portado | `project/game/tools/road_tool.gd` + `road_path.cpp` |
| `RoadTool.curvePending` | — | portado | `project/game/tools/road_tool.gd` + `road_path.cpp` |
| `RoadTool.down` | A press at `world`, `anchor` what is under it: a stroke begins (from the chain's end when chaining). | portado | `project/game/tools/road_tool.gd` + `road_path.cpp` |
| `RoadTool.draft` | The drag in progress. | portado | `project/game/tools/road_tool.gd` + `road_path.cpp` |
| `RoadTool.draftKey` | DO NOT key the draft on the whole map's revision (every tile that came in judged it again) nor make its key a text each frame: its numbers, compared a | não se aplica | o plano roda em C++ a cada movimento; a espera existia para o ensaio caro do 4.0. Ainda sem medição do custo com 48 peças |
| `RoadTool.dropCurve` | The plan is no longer a curve: a curve waiting for its bend is dropped. | portado | `project/game/tools/road_tool.gd` + `road_path.cpp` |
| `RoadTool.ending` | The end a draft is laid to (`chosenEnd`, or what is under its end), its height and its pieces. | em parte | `project/game/tools/road_tool.gd` |
| `RoadTool.gesture` | — | em parte | `project/game/tools/road_tool.gd` |
| `RoadTool.heightChosen` | A height chosen from outside the tool (the console). | não portado | `road-elevation` (próxima desta sessão) |
| `RoadTool.heightEdited` | The height was set by hand since the stroke began (a start on a road does not take that road's height then). | não portado | `road-elevation` (próxima desta sessão) |
| `RoadTool.hover` | @perInput The pointer over the map with no drag: the next stretch of a chain, previewed. | portado | `project/game/tools/road_tool.gd` + `road_path.cpp` |
| `RoadTool.inProgress` | — | portado | `project/game/tools/road_tool.gd` + `road_path.cpp` |
| `RoadTool.judge` | — | não se aplica | o plano roda em C++ a cada movimento; a espera existia para o ensaio caro do 4.0. Ainda sem medição do custo com 48 peças |
| `RoadTool.judgeAt` | When the draft in hand will have rested long enough to be judged (`verdict`); null: nothing waiting. | não se aplica | o plano roda em C++ a cada movimento; a espera existia para o ensaio caro do 4.0. Ainda sem medição do custo com 48 peças |
| `RoadTool.judged` | — | não se aplica | o plano roda em C++ a cada movimento; a espera existia para o ensaio caro do 4.0. Ainda sem medição do custo com 48 peças |
| `RoadTool.lastKey` | — | não se aplica | o plano roda em C++ a cada movimento; a espera existia para o ensaio caro do 4.0. Ainda sem medição do custo com 48 peças |
| `RoadTool.layPart` | One part of a road laid from `start` to `end`, one undo step (joined into `group`'s); `last`: the gesture ends with it. False when refused. | portado | `project/game/tools/road_tool.gd` + `road_path.cpp` |
| `RoadTool.mapAround` | What the dry run reads of the map moved near the draft: a serial, moved by a change of the diary whose place meets the draft's box (or the balance, or | não se aplica | o plano roda em C++ a cada movimento; a espera existia para o ensaio caro do 4.0. Ainda sem medição do custo com 48 peças |
| `RoadTool.mapSeen` | — | não se aplica | o plano roda em C++ a cada movimento; a espera existia para o ensaio caro do 4.0. Ainda sem medição do custo com 48 peças |
| `RoadTool.mapSerial` | — | não se aplica | o plano roda em C++ a cada movimento; a espera existia para o ensaio caro do 4.0. Ainda sem medição do custo com 48 peças |
| `RoadTool.move` | @perInput The pointer moved; true when the curve's bend or the drag took it. | portado | `project/game/tools/road_tool.gd` + `road_path.cpp` |
| `RoadTool.pieces` | The pieces a draft is laid as: one straight or curved, or the freehand path's. | portado | `project/game/tools/road_tool.gd` + `road_path.cpp` |
| `RoadTool.pointerAt` | The pointer over the map, on the screen. | não portado | `road-elevation` (próxima desta sessão) |
| `RoadTool.pointerScreen` | The pointer on the screen while the tool is in hand (the height step carries the stroke from it). | não portado | `road-elevation` (próxima desta sessão) |
| `RoadTool.preview` | What the overlay draws: the curve waiting for its bend, the drag, the chain's next stretch | portado | `project/game/tools/road_tool.gd` + `road_path.cpp` |
| `RoadTool.reset` | A new map: drawn at the land's own height again. | não portado | `road-elevation` (próxima desta sessão) |
| `RoadTool.settling` | The road just committed, drawn as previewed until the world with it is built. | em parte | `project/game/tools/road_tool.gd` |
| `RoadTool.shift` | How far a drag's stroke is carried, in plan, by the heights changed during it | não portado | `road-elevation` (próxima desta sessão) |
| `RoadTool.stations` | The road in hand as it would be built: the dry run's stations | não portado | `road-elevation` (próxima desta sessão) |
| `RoadTool.stepHeight` | PageUp/PageDown or the height buttons: the road is drawn a metre higher or lower, landing on whole metres | não portado | `road-elevation` (próxima desta sessão) |
| `RoadTool.up` | @perInput The pointer let go: a click starts a chain, a curve waits for its bend, a drag is laid (`commit` false: dropped). | portado | `project/game/tools/road_tool.gd` + `road_path.cpp` |
| `RoadTool.verdict` | Why the road in hand would be refused (`editRules.ts`, or a crossing at the wrong height), or null | portado | `project/game/tools/road_tool.gd` + `road_path.cpp` |
| `RoadToolHost` | What the road tool reads of the game and does to it. | em parte | `project/game/tools/road_tool.gd` |
| `VERDICT_DELAY` | How long a draft rests before the preview judges it (`RoadTool.verdict`), ms. | não se aplica | o plano roda em C++ a cada movimento; a espera existia para o ensaio caro do 4.0. Ainda sem medição do custo com 48 peças |
| `isRefusal` | — | em parte | `project/game/tools/road_tool.gd` |
| `landscapeAlong` | The placed items the furniture of `laid` can meet | em parte | `project/game/tools/road_tool.gd` |
| `pushSample` | — | portado | `project/game/tools/road_tool.gd` + `road_path.cpp` |

## `src/editor/roadTop.ts` (inteiro, 94 linhas) — **não se aplica**

Dono: funcionalidade `road-elevation`. Depende da altura solucionada das vias.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `KINDS` | DO NOT walk `doc.nodes` on a road edit for the top: every street drawn read every node of the city again | não se aplica | funcionalidade `road-elevation` |
| `RoadTop` | — | não se aplica | funcionalidade `road-elevation` |
| `RoadTop.byHeight` | How many cells have their top at each whole height (`Math.ceil`). | não se aplica | funcionalidade `road-elevation` |
| `RoadTop.cellTop` | The highest node standing in cell `key`: the nodes of the roads that meet it, only those inside it. | não se aplica | funcionalidade `road-elevation` |
| `RoadTop.cells` | — | não se aplica | funcionalidade `road-elevation` |
| `RoadTop.constructor` | `heightAt`: the solved road height at a point (`SceneHandle.elevationAt`). | não se aplica | funcionalidade `road-elevation` |
| `RoadTop.constructor.count` | — | não se aplica | funcionalidade `road-elevation` |
| `RoadTop.height` | The highest road node's height (rounded up to a whole unit); -Infinity with no roads. | não se aplica | funcionalidade `road-elevation` |
| `RoadTop.lastComputes` | What the last `height` computed: cells (for the tests). | não se aplica | funcionalidade `road-elevation` |
| `RoadTop.source` | — | não se aplica | funcionalidade `road-elevation` |
| `RoadTop.top` | — | não se aplica | funcionalidade `road-elevation` |
| `Source` | — | não se aplica | funcionalidade `road-elevation` |
| `allCells` | @rebuild The diary cannot say what moved (first look, a map loaded, a whole solve): every node's cell. | não se aplica | funcionalidade `road-elevation` |
| `dirtyCells` | The cells a change's rectangles reach; a change of the whole map, every node's cell. | não se aplica | funcionalidade `road-elevation` |

## `src/editor/roundabout.ts` (inteiro, 86 linhas) — **não portado**

Dono: `sim/commands`. O restante de `commit.ts` e `roadTop.ts`: reconciliação fina de nós e faixas. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `ROUNDABOUT_APPROACH_LENGTH` | — | não portado | `sim/commands` |
| `ROUNDABOUT_MAX_RADIUS` | — | não portado | `sim/commands` |
| `ROUNDABOUT_MIN_RADIUS` | — | não portado | `sim/commands` |
| `RoundaboutResult` | — | não portado | `sim/commands` |
| `commitRoundabout` | Places a single-lane roundabout on vacant ground. | não portado | `sim/commands` |
| `commitRoundabout.point` | — | não portado | `sim/commands` |
| `roundaboutRefusal` | Why a roundabout of `radius` round `centre` cannot be placed, or null when it can: what the click would answer, asked before it (the preview). | não portado | `sim/commands` |

## `src/editor/smallCityMap.ts` (parte, 39 linhas) — **não portado**

Dono: `sim/commands`. O restante de `commit.ts` e `roadTop.ts`: reconciliação fina de nós e faixas. Classificado por pasta: não li cada declaração.


## `src/editor/snap.ts` (inteiro, 425 linhas) — **em parte**

Dono: `native/src/sim/commands/road_snap.cpp`. Rede, ângulos, grade, guias, pouso em via e opções portados; o filtro por altura espera `road-elevation`; a grade de células espera a camada Grade (`interface`).

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `<TryStatement>` | — | em parte | `native/src/sim/commands/road_snap.cpp` |
| `Anchor` | — | portado | `native/src/sim/commands/road_snap.cpp` |
| `AnchorKind` | — | portado | `native/src/sim/commands/road_snap.cpp` |
| `CONE` | Angular snap cone, in radians. | portado | `native/src/sim/commands/road_snap.cpp` |
| `GRID_HEADINGS` | The grid's eight headings: its two lines both ways, then the diagonals between them. | portado | `native/src/sim/commands/road_snap.cpp` |
| `LENGTH_STEP` | Length quantum, in world units. | não se aplica | nenhum: a ferramenta de via passa lengthStep 0, este ramo nunca roda nela |
| `LENGTH_TOLERANCE` | — | não se aplica | nenhum: a ferramenta de via passa lengthStep 0, este ramo nunca roda nela |
| `ROAD_SNAP_KEY` | — | portado | `project/game/settings/settings.gd` |
| `RoadSnap` | The road tool's snapping, as Cities: Skylines II lays it out: one switch for all of it, and each kind on its own. | portado | `project/game/settings/settings.gd` |
| `SnapOptions` | — | portado | `native/src/sim/commands/road_snap.cpp` |
| `SnapResult` | — | portado | `native/src/sim/commands/road_snap.cpp` |
| `anchorForHeight` | A nearby road at another height is a crossing, not an accidental junction. | não portado | `road-elevation` (próxima desta sessão) |
| `anchorHeightOffset` | Height of an authored connection, relative to the designed ground, or `fallback` for open ground. | não portado | `road-elevation` (próxima desta sessão) |
| `findAnchor` | @perInput Finds what the pointer is over: an existing node, a point on an existing road, or open ground. | portado | `native/src/sim/commands/road_snap.cpp` |
| `gridSnapOffset` | Where the snapped points sit within a cell: on its lines (0), or - for a road an odd number of cells wide | não portado | `interface` (camada Grade) |
| `gridSnapStep` | The grid the road snap lands on: the 1 m subdivision, or - while the grid is drawn (the road panel's Grid) | não portado | `interface` (camada Grade) |
| `onGridAlong` | The end of a road moved onto the 1 m grid. | portado | `native/src/sim/commands/road_snap.cpp` |
| `rayHitsPolyline` | Where a ray from `from` along `dir` first crosses a polyline, or `null`. | portado | `native/src/sim/commands/road_snap.cpp` |
| `ribbonsNear` | DO NOT walk `net.ribbons` / `doc.segments` instead, and DO NOT go through `Network.ribbonsMeeting`: its order table | já existia | `RoadNetwork::for_segments_near` |
| `roadSnap` | — | portado | `project/game/settings/settings.gd` |
| `roadSnapState` | — | portado | `project/game/settings/settings.gd` |
| `setGridSnapStep` | — | não portado | `interface` (camada Grade) |
| `setRoadSnap` | — | portado | `project/game/settings/settings.gd` |
| `snapEndpoint` | @perInput Runs once a pointer move (the snap under the cursor): nothing it reaches may walk the map. | portado | `native/src/sim/commands/road_snap.cpp` |
| `snapHeading` | The heading a drag is allowed to take, and why. | portado | `native/src/sim/commands/road_snap.cpp` |
| `snapRoadEndpoint` | @perInput Free road drawing follows the pointer exactly, snapping only to compatible networks. | portado | `native/src/sim/commands/road_snap.cpp` |
| `snapRoadStart` | Where a road starts: a junction or a road it is drawn from keeps its exact point | portado | `native/src/sim/commands/road_snap.cpp` |

## `src/editor/streetObjects.ts` (inteiro, 106 linhas) — **não se aplica**

Dono: `street-furniture` (porte-6). Também é entrada de `street-furniture`, que porta mobiliário, travessias, retornos e rede.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `PedestrianCrossingKind` | — | não se aplica | `street-furniture` (porte-6) |
| `PedestrianCrossingResult` | — | não se aplica | `street-furniture` (porte-6) |
| `UturnResult` | — | não se aplica | `street-furniture` (porte-6) |
| `commitPedestrianCrossing` | Inserts one crossing whose centre is at the selected arc distance (midpoint by default). | não se aplica | `street-furniture` (porte-6) |
| `commitUturn` | Puts a U-turn through the median at the selected arc distance (the middle by default, retorno) | não se aplica | `street-furniture` (porte-6) |

## `src/editor/streetscapeTool.ts` (inteiro, 155 linhas) — **não se aplica**

Dono: `landscape` (porte-3). Também é entrada de `landscape`.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `MEADOW_CLEAR` | How far a meadow keeps from another (`snapLandscape`), and more than any gap between two items. | não se aplica | `landscape` (porte-3) |
| `ROW_SPACING` | The spacing a row of each kind is laid at: the catalogue's (`world/roads/furnitureCatalog.ts`). | não se aplica | `landscape` (porte-3) |
| `StreetscapeTool` | THE LANDSCAPING TOOL: benches, bins, street lights, trees, signs... placed on the footways | não se aplica | `landscape` (porte-3) |
| `StreetscapeTool.constructor` | — | não se aplica | `landscape` (porte-3) |
| `StreetscapeTool.down` | @perInput Runs once an input on one thing (a press, a release, a click): nothing it reaches may walk the map. | não se aplica | `landscape` (porte-3) |
| `StreetscapeTool.hover` | Where the chosen item would go under the pointer: a ring, red where it cannot. | não se aplica | `landscape` (porte-3) |
| `StreetscapeTool.move` | @perInput `pressed`: the button still down, dragging a row out from the item just placed. | não se aplica | `landscape` (porte-3) |
| `StreetscapeTool.reach` | Pick radius for a placed item and the reach of the footway snap, world units. | não se aplica | `landscape` (porte-3) |
| `StreetscapeTool.row` | A row being dragged out from the item just placed (placement in line) | não se aplica | `landscape` (porte-3) |
| `StreetscapeTool.up` | The button let go: the row dragged out is laid, one undo step. | não se aplica | `landscape` (porte-3) |
| `StreetscapeToolHost` | What the landscaping tool reads of the game and does to it. | não se aplica | `landscape` (porte-3) |
| `itemsAround` | DO NOT hand it `doc.landscape.values()` | não se aplica | `landscape` (porte-3) |
| `rowPieces` | The pieces after `from` along its footway, toward the pointer, each where the tool would accept it. | não se aplica | `landscape` (porte-3) |

## `src/editor/transitTools.ts` (parte, 7 linhas) — **não se aplica**

Dono: `transit` (porte-5). Transporte é de `transit`.


## `src/jobs/jobPool.ts` (parte, 4 linhas) — **não se aplica**

Dono: `core/job_pool`. O jogo tem o próprio pool.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `JobSystem.firstLanes` | — | não se aplica | `core/job_pool` |
| `laneFirst` | — | não se aplica | `core/job_pool` |

## `src/lab/catalog/city.ts` (parte, 3 linhas) — **não se aplica**

Dono: nenhum. Laboratório de catálogo do 4.0: ferramenta de desenvolvimento, não é jogo.


## `src/lab/catalog/layers.ts` (parte, 47 linhas) — **não se aplica**

Dono: nenhum. Laboratório de catálogo do 4.0: ferramenta de desenvolvimento, não é jogo.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `farNetworkItems.flat` | — | não se aplica | nenhum |

## `src/lab/catalog/layout.ts` (parte, 39 linhas) — **não se aplica**

Dono: nenhum. Laboratório de catálogo do 4.0: ferramenta de desenvolvimento, não é jogo.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `surfacesOf` | — | não se aplica | nenhum |
| `surfacesOf.flat` | — | não se aplica | nenhum |

## `src/main.ts` (parte, 1107 linhas) — **não se aplica**

Dono: cabos do 4.0. Só liga módulos; sem declaração própria.


## `src/networkBake.worker.ts` (parte, 6 linhas) — **não se aplica**

Dono: cabos do 4.0. Só liga módulos; sem declaração própria.


## `src/render/agents.ts` (parte, 23 linhas) — **não se aplica**

Dono: `traffic` (porte-5). Agentes são de `traffic`.


## `src/render/bake/roadRecipes.ts` (inteiro, 218 linhas) — **não se aplica**

Dono: `native/src/render/roads`. Infraestrutura de render do 4.0 (ladrilhos, workers, WebGPU, residência). O jogo desenha por chunk com MeshBuilder e JobPool: porta-se o que se vê, não a máquina. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `ASPHALT_BASE` | — | não se aplica | `native/src/render/roads` |
| `ASPHALT_SIZE` | — | não se aplica | `native/src/render/roads` |
| `ASPHALT_TILE` | — | não se aplica | `native/src/render/roads` |
| `DECK_TILE` | — | não se aplica | `native/src/render/roads` |
| `FOOTWAY_TILE` | — | não se aplica | `native/src/render/roads` |
| `KERB_TILE` | — | não se aplica | `native/src/render/roads` |
| `RAISED_ASPHALT_BASE` | — | não se aplica | `native/src/render/roads` |
| `VERGE_TILE` | — | não se aplica | `native/src/render/roads` |
| `asphaltRecipe` | The asphalt; `raised` its paler tone for raised crossings, whose colour alone is baked (it shares the road's normal and roughness). | não se aplica | `native/src/render/roads` |
| `asphaltRecipe.shade` | — | não se aplica | `native/src/render/roads` |
| `cellHash` | A small integer hash, for per-slab variation that is stable and tiles. | não se aplica | `native/src/render/roads` |
| `deckRecipe` | — | não se aplica | `native/src/render/roads` |
| `deckRecipe.shade` | — | não se aplica | `native/src/render/roads` |
| `footwayRecipe` | The footway: interlocking concrete pavers in basketweave (it was large precast slabs; the player found them ugly). | não se aplica | `native/src/render/roads` |
| `footwayRecipe.shade` | — | não se aplica | `native/src/render/roads` |
| `kerbRecipe` | The kerb: precast concrete, smooth, a shade lighter and cooler than the footway so the kerb line reads as its own edge | não se aplica | `native/src/render/roads` |
| `kerbRecipe.shade` | — | não se aplica | `native/src/render/roads` |
| `roadRecipes` | Every street surface by its key; `asphalt-raised` bakes its colour only (`mapOnly`): it shares the road's normal and roughness. | não se aplica | `native/src/render/roads` |
| `vergeRecipe` | — | não se aplica | `native/src/render/roads` |
| `vergeRecipe.shade` | — | não se aplica | `native/src/render/roads` |

## `src/render/buildings/cellPool.ts` (parte, 14 linhas) — **não se aplica**

Dono: `build` (porte-2). Fatia dos prédios.


## `src/render/buildings/cellWork.ts` (parte, 12 linhas) — **não se aplica**

Dono: `build` (porte-2). Fatia dos prédios.


## `src/render/buildings/finishes.ts` (parte, 44 linhas) — **não se aplica**

Dono: `build` (porte-2). Fatia dos prédios.


## `src/render/densityCars.ts` (parte, 57 linhas) — **não se aplica**

Dono: `traffic` (porte-5). Carros de densidade são de `traffic`.


## `src/render/farWoods.ts` (parte, 7 linhas) — **não portado**

Dono: `native/src/render/roads`. Falta conferir um a um o que sobra do render de vias. Classificado por pasta: não li cada declaração.


## `src/render/flatAtlas.ts` (parte, 26 linhas) — **não portado**

Dono: `native/src/render/roads`. Falta conferir um a um o que sobra do render de vias. Classificado por pasta: não li cada declaração.


## `src/render/grass.ts` (inteiro, 98 linhas) — **não portado**

Dono: `native/src/render/roads`. Falta conferir um a um o que sobra do render de vias. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `CLUMP_REACH` | The largest clump's radius (`grassClump`: one to three metres). | não portado | `native/src/render/roads` |
| `CLUMP_READ_REACH` | How far round its item a clump reads the ground and the roads: its radius and the slope's probe. | não portado | `native/src/render/roads` |
| `Clump` | A clump's grass: its tufts and its wildflowers. | não portado | `native/src/render/roads` |
| `FLOWER_COLOURS` | — | não portado | `native/src/render/roads` |
| `FLOWER_MIN` | — | não portado | `native/src/render/roads` |
| `FLOWER_RANGE` | — | não portado | `native/src/render/roads` |
| `NONE` | — | não portado | `native/src/render/roads` |
| `SLOPE_PROBE` | How far round a clump's centre the slope is read (`grassClump`). | não portado | `native/src/render/roads` |
| `TUFT_MIN` | Real turf height, root to tip. | não portado | `native/src/render/roads` |
| `TUFT_RANGE` | — | não portado | `native/src/render/roads` |
| `Tuft` | — | não portado | `native/src/render/roads` |
| `grassClump` | The clump at a placed item, from its own generator (seeded by its id, so an edit elsewhere moves none of it) | não portado | `native/src/render/roads` |

## `src/render/groundShape.ts` (parte, 77 linhas) — **não portado**

Dono: `native/src/render/roads`. Falta conferir um a um o que sobra do render de vias. Classificado por pasta: não li cada declaração.


## `src/render/isoViewport.ts` (parte, 5 linhas) — **não portado**

Dono: `native/src/render/roads`. Falta conferir um a um o que sobra do render de vias. Classificado por pasta: não li cada declaração.


## `src/render/ktxParse.d.ts` (parte, 57 linhas) — **não portado**

Dono: `native/src/render/roads`. Falta conferir um a um o que sobra do render de vias. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `KHR_DF_CHANNEL_RGBSDA_ALPHA` | — | não portado | `native/src/render/roads` |
| `KHR_DF_CHANNEL_RGBSDA_BLUE` | — | não portado | `native/src/render/roads` |
| `KHR_DF_CHANNEL_RGBSDA_GREEN` | — | não portado | `native/src/render/roads` |
| `KHR_DF_CHANNEL_RGBSDA_RED` | — | não portado | `native/src/render/roads` |
| `KHR_DF_MODEL_RGBSDA` | — | não portado | `native/src/render/roads` |
| `KHR_DF_PRIMARIES_BT709` | — | não portado | `native/src/render/roads` |
| `KHR_DF_SAMPLE_DATATYPE_LINEAR` | — | não portado | `native/src/render/roads` |
| `KHR_DF_TRANSFER_LINEAR` | — | não portado | `native/src/render/roads` |
| `KHR_DF_TRANSFER_SRGB` | — | não portado | `native/src/render/roads` |
| `KHR_SUPERCOMPRESSION_NONE` | — | não portado | `native/src/render/roads` |
| `KHR_SUPERCOMPRESSION_ZSTD` | — | não portado | `native/src/render/roads` |
| `KTX2Container` | — | não portado | `native/src/render/roads` |
| `KTX2DataFormatDescriptorBasicFormat` | — | não portado | `native/src/render/roads` |
| `KTX2Level` | — | não portado | `native/src/render/roads` |
| `KTX2Sample` | — | não portado | `native/src/render/roads` |
| `VK_FORMAT_R8G8B8A8_SRGB` | — | não portado | `native/src/render/roads` |
| `VK_FORMAT_R8G8B8A8_UNORM` | — | não portado | `native/src/render/roads` |
| `VK_FORMAT_UNDEFINED` | — | não portado | `native/src/render/roads` |
| `read` | — | não portado | `native/src/render/roads` |

## `src/render/markings.ts` (inteiro, 62 linhas) — **não portado**

Dono: `native/src/render/roads`. Falta conferir um a um o que sobra do render de vias. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `paintCatalogue` | DO NOT make a paint material where a frame reaches: a colour first painted made its material in the frame of the edit. | não portado | `native/src/render/roads` |
| `paintMaterial` | Paint of one colour: lit like the asphalt under it, drawn over it. | não portado | `native/src/render/roads` |

## `src/render/materials.ts` (parte, 82 linhas) — **não portado**

Dono: `native/src/render/roads`. Falta conferir um a um o que sobra do render de vias. Classificado por pasta: não li cada declaração.


## `src/render/mesh/detailLayer.ts` (parte, 6 linhas) — **não se aplica**

Dono: `native/src/render/roads`. Infraestrutura de render do 4.0 (ladrilhos, workers, WebGPU, residência). O jogo desenha por chunk com MeshBuilder e JobPool: porta-se o que se vê, não a máquina. Classificado por pasta: não li cada declaração.


## `src/render/mesh/farBlockBatches.ts` (parte, 13 linhas) — **não se aplica**

Dono: `native/src/render/roads`. Infraestrutura de render do 4.0 (ladrilhos, workers, WebGPU, residência). O jogo desenha por chunk com MeshBuilder e JobPool: porta-se o que se vê, não a máquina. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `FarBlockBatches.adopt` | — | não se aplica | `native/src/render/roads` |
| `batchedFar` | — | não se aplica | `native/src/render/roads` |

## `src/render/mesh/roadEdges.ts` (inteiro, 53 linhas) — **não se aplica**

Dono: `native/src/render/roads`. Infraestrutura de render do 4.0 (ladrilhos, workers, WebGPU, residência). O jogo desenha por chunk com MeshBuilder e JobPool: porta-se o que se vê, não a máquina. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `SECTION` | Where a material says its shader reads the `roadSection` attribute: no other surface is given it (`surfaceTiles.ts`). | não se aplica | `native/src/render/roads` |
| `applyRoadEdges` | — | não se aplica | `native/src/render/roads` |
| `marksRoadSection` | Gives `material`'s surfaces the `roadSection` attribute: its shader reads it. | não se aplica | `native/src/render/roads` |
| `readsRoadSection` | — | não se aplica | `native/src/render/roads` |

## `src/render/mesh/roadLod.ts` (inteiro, 79 linhas) — **não se aplica**

Dono: `native/src/render/roads`. Infraestrutura de render do 4.0 (ladrilhos, workers, WebGPU, residência). O jogo desenha por chunk com MeshBuilder e JobPool: porta-se o que se vê, não a máquina. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `BlockLevels` | What a block keeps of its two levels (`mesh.userData.lod`). | não se aplica | `native/src/render/roads` |
| `LOD_KEY` | — | não se aplica | `native/src/render/roads` |
| `coarseGeometry` | A coarse block's geometry from its arrays. | não se aplica | `native/src/render/roads` |
| `giveBackArrays` | The arrays of a mesh's attributes and index let go (made again by `refillArrays`). | não se aplica | `native/src/render/roads` |
| `giveBackArrays.letGo` | — | não se aplica | `native/src/render/roads` |
| `levelsOf` | — | não se aplica | `native/src/render/roads` |
| `makeFar` | The block's far level shown, its full mesh's buffers given back. | não se aplica | `native/src/render/roads` |

## `src/render/mesh/roadLodPure.ts` (inteiro, 82 linhas) — **não se aplica**

Dono: `native/src/render/roads`. Infraestrutura de render do 4.0 (ladrilhos, workers, WebGPU, residência). O jogo desenha por chunk com MeshBuilder e JobPool: porta-se o que se vê, não a máquina. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `BlockArrays` | A block's arrays, as `tileArrays` makes them: `color` and `roadSection` only where the surface's material reads them. | não se aplica | `native/src/render/roads` |
| `COARSE_ERROR` | What a coarse block may deviate by from the block, metres (absolute: a 1 % error of a 250 m block is 2.5 m). | não se aplica | `native/src/render/roads` |
| `COARSE_MIN_TRIANGLES` | A block with fewer triangles than this is not simplified: its far level would save nothing. | não se aplica | `native/src/render/roads` |
| `COARSE_RATIO` | The share of the triangles aimed at: the simplifier stops sooner when the error is reached. | não se aplica | `native/src/render/roads` |
| `blockBuffers` | A block's typed arrays, to transfer them to or from a worker. | não se aplica | `native/src/render/roads` |
| `coarseArrays` | The coarse arrays of a block, or null when it is too small to be worth it or the simplifier cannot run. | não se aplica | `native/src/render/roads` |
| `coarseArrays.take` | — | não se aplica | `native/src/render/roads` |

## `src/render/mesh/roadResidency.ts` (inteiro, 353 linhas) — **não se aplica**

Dono: `native/src/render/roads`. Infraestrutura de render do 4.0 (ladrilhos, workers, WebGPU, residência). O jogo desenha por chunk com MeshBuilder e JobPool: porta-se o que se vê, não a máquina. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `BRING_PER_LOOK` | Blocks brought near in one look: each is made again from its tiles, a few milliseconds. | não se aplica | `native/src/render/roads` |
| `Cell` | A road cell as one look sees it. | não se aplica | `native/src/render/roads` |
| `CellCover` | The network's far level as the residency sees it (`networkFarRoads.ts`). | não se aplica | `native/src/render/roads` |
| `DROPPED_FARTHER` | A dropped cell ranks as if this much farther than it is: a cell at the budget's edge is not let go and made again as the eye moves by a few metres. | não se aplica | `native/src/render/roads` |
| `FAR_MARGIN` | A block goes far when it is this much farther than `reach`, so one near the line is not asked twice. | não se aplica | `native/src/render/roads` |
| `NEAR_PER_FAR` | Bytes a block's full level holds for its far level's, before it was ever near. | não se aplica | `native/src/render/roads` |
| `RESTORING_AT_ONCE` | Cells made again at once (`restoreCell`): each a few blocks simplified by the workers. | não se aplica | `native/src/render/roads` |
| `RoadResidency` | Near: wanted within the reach and meeting the view's frustum (Cesium's traversal visits only the tiles in the frustum); past the budget, the near bloc | não se aplica | `native/src/render/roads` |
| `VIEW_MARGIN` | How far round a block's box the view meets it, m: a block just past the edge is near before the eye pans onto it. | não se aplica | `native/src/render/roads` |
| `distance` | The block's distance to the camera in the ground plane, scene units (x, z = -y). | não se aplica | `native/src/render/roads` |
| `dropCell` | Every block of a cell let go to the network's far level: the far levels' geometry given back (made again by `restoreCell`), a block with none (too sma | não se aplica | `native/src/render/roads` |
| `fitFarLevels` | The far levels under `residency.farBudget`: the cells it holds keep them, the rest go to the network's far level (see `RoadResidency`). | não se aplica | `native/src/render/roads` |
| `fitFarLevels.farther` | — | não se aplica | `native/src/render/roads` |
| `forgetRoadCells` | The residency's cells forgotten (another map opened: its blocks and the network's level went with the last one). | não se aplica | `native/src/render/roads` |
| `fullBytes` | The bytes a block's full level holds on the GPU (its arrays, or what they were once uploaded, `gpuOnly.ts`), kept as its last measure. | não se aplica | `native/src/render/roads` |
| `isNear` | Whether a block at (`x`, `y`) in world units is within `reach` of the camera: the level a block is made at. | não se aplica | `native/src/render/roads` |
| `makeNear` | The block's full mesh brought back and shown; the coarse one is hidden. | não se aplica | `native/src/render/roads` |
| `parkCells` | The view's cells drawn by the network's far level (`networkFrom`) and the others given back to their blocks; returns the cells changed. DO NOT draw a  | não se aplica | `native/src/render/roads` |
| `refillNow` | The block's full mesh made again on the page from its tiles' rings (`refillArrays`) and shown. | não se aplica | `native/src/render/roads` |
| `restoreCell` | A dropped cell's blocks' far levels made again off the page, shown together in place of the network's level once all are back. | não se aplica | `native/src/render/roads` |
| `showFull` | The block's full mesh shown, the coarse one hidden. | não se aplica | `native/src/render/roads` |
| `updateRoadLevels` | The levels brought up to the camera at (`focusX`, `focusY`) in world units, under `residency`'s budgets; returns the blocks and cells changed. | não se aplica | `native/src/render/roads` |

## `src/render/mesh/stream.ts` (inteiro, 55 linhas) — **não se aplica**

Dono: `native/src/render/roads`. Infraestrutura de render do 4.0 (ladrilhos, workers, WebGPU, residência). O jogo desenha por chunk com MeshBuilder e JobPool: porta-se o que se vê, não a máquina. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `Stream` | A STREAM OF NUMBERS UNDER CONSTRUCTION IN A TYPED ARRAY grown by doubling, read through `data` (replaced as it grows) up to `length` - DO NOT accumula | não se aplica | `native/src/render/roads` |
| `Stream.clear` | Emptied, its array kept for the next use. | não se aplica | `native/src/render/roads` |
| `Stream.constructor` | — | não se aplica | `native/src/render/roads` |
| `Stream.data` | — | não se aplica | `native/src/render/roads` |
| `Stream.length` | — | não se aplica | `native/src/render/roads` |
| `Stream.push1` | — | não se aplica | `native/src/render/roads` |
| `Stream.push2` | — | não se aplica | `native/src/render/roads` |
| `Stream.push3` | — | não se aplica | `native/src/render/roads` |
| `Stream.room` | Room for `n` more, the array doubled until it fits. | não se aplica | `native/src/render/roads` |
| `Stream.view` | What was written, sharing the array (`data` until the next push). | não se aplica | `native/src/render/roads` |
| `doubles` | A stream of doubles: the numbers exactly as a number[] holds them. | não se aplica | `native/src/render/roads` |
| `indices` | A stream of vertex indices. | não se aplica | `native/src/render/roads` |

## `src/render/mesh/surfaceMesh.ts` (inteiro, 48 linhas) — **não se aplica**

Dono: `native/src/render/roads`. Infraestrutura de render do 4.0 (ladrilhos, workers, WebGPU, residência). O jogo desenha por chunk com MeshBuilder e JobPool: porta-se o que se vê, não a máquina. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `SurfaceMeshOptions` | — | não se aplica | `native/src/render/roads` |
| `disposeMesh` | — | não se aplica | `native/src/render/roads` |
| `finish` | — | não se aplica | `native/src/render/roads` |

## `src/render/mesh/surfaceTiles.ts` (inteiro, 242 linhas) — **não se aplica**

Dono: `native/src/render/roads`. Infraestrutura de render do 4.0 (ladrilhos, workers, WebGPU, residência). O jogo desenha por chunk com MeshBuilder e JobPool: porta-se o que se vê, não a máquina. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `BlockMember` | One surface of a block: its tiles, and its own block's edge, locked when it is simplified (`coarseArrays`). | não se aplica | `native/src/render/roads` |
| `FAR_AGAIN` | Where a far level keeps how its arrays are made again from its tiles on the page (`attachFarLevel`, `restoreFarLevel`). | não se aplica | `native/src/render/roads` |
| `MemberSize` | A surface's share of a block: its tiles' vertices and indices (`memberSizes`). | não se aplica | `native/src/render/roads` |
| `SurfaceReads` | The attributes a surface's material reads besides position, normal and uv: no other is sent to the GPU. | não se aplica | `native/src/render/roads` |
| `attachFarLevel` | The block's far level (`roadLod.ts`) as it arrives from its worker; `members`, each surface it draws (one, or several drawn alike), each simplified on | não se aplica | `native/src/render/roads` |
| `blockArraysOf` | A block's arrays from its tiles as kept (the store's), for the attributes its material reads: a copy end to end, nothing cut (`roadResidency.ts` makeN | não se aplica | `native/src/render/roads` |
| `blockBox` | The box round a block's tiles' rectangles, [minX, minY, maxX, maxY]. | não se aplica | `native/src/render/roads` |
| `cloneBlock` | A block's arrays copied: what a worker is handed to simplify while the block keeps its own. | não se aplica | `native/src/render/roads` |
| `concat` | Arrays end to end, with where each ends: one buffer to hand over instead of one an array. | não se aplica | `native/src/render/roads` |
| `emptyLike` | A mesh's arrays at their sizes, all zero (every triangle of no area): what a far level the tiles no longer simplify to is given back, never the arrays | não se aplica | `native/src/render/roads` |
| `joinBlocks` | Blocks' arrays end to end, as one mesh's (the indices shifted). | não se aplica | `native/src/render/roads` |
| `joinBlocks.join` | — | não se aplica | `native/src/render/roads` |
| `memberSizes` | Each surface's share of a block, from its tiles (in the order they were merged): what `splitBlock` cuts by, without the tiles. | não se aplica | `native/src/render/roads` |
| `mergeTiles` | DO NOT pass a source that returns `parts`: the closure would hold every tile's arrays for as long as the mesh lives. | não se aplica | `native/src/render/roads` |
| `readsOf` | What `material` reads: the vertex colour only with `vertexColors` (three defines USE_COLOR from it), the section only with the road edges. | não se aplica | `native/src/render/roads` |
| `restoreFarLevel` | A far level let go (`roadResidency.ts` dropCell) made again: its geometry replaced by one of `arrays` (made off the page, `BlockLevels.remake`), or, n | não se aplica | `native/src/render/roads` |
| `splitBlock` | A block's arrays cut back into its surfaces' (`members`: each surface's share, in the order they were merged), each a copy: what a block of several su | não se aplica | `native/src/render/roads` |
| `tileArrays` | A surface's tiles, in order, as one mesh's arrays (the indices shifted), with only what its material reads; null for none. | não se aplica | `native/src/render/roads` |
| `tilesOnGpu` | — | não se aplica | `native/src/render/roads` |

## `src/render/mesh/tileMesh.ts` (parte, 265 linhas) — **não se aplica**

Dono: `native/src/render/roads`. Infraestrutura de render do 4.0 (ladrilhos, workers, WebGPU, residência). O jogo desenha por chunk com MeshBuilder e JobPool: porta-se o que se vê, não a máquina. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `Context` | — | não se aplica | `native/src/render/roads` |
| `HeightFn` | — | não se aplica | `native/src/render/roads` |
| `Streams` | — | não se aplica | `native/src/render/roads` |
| `TileMeshOptions` | — | não se aplica | `native/src/render/roads` |
| `TintFn` | — | não se aplica | `native/src/render/roads` |
| `UvFn` | — | não se aplica | `native/src/render/roads` |
| `UvFrameFn` | — | não se aplica | `native/src/render/roads` |
| `boxOf` | — | não se aplica | `native/src/render/roads` |
| `copyCorner` | — | não se aplica | `native/src/render/roads` |
| `meshPiece` | — | não se aplica | `native/src/render/roads` |
| `meshSkirts` | — | não se aplica | `native/src/render/roads` |
| `meshTileShape` | — | não se aplica | `native/src/render/roads` |
| `smoothTopNormals` | — | não se aplica | `native/src/render/roads` |
| `streamArrays` | — | não se aplica | `native/src/render/roads` |
| `streams` | — | não se aplica | `native/src/render/roads` |
| `uvConsistent` | — | não se aplica | `native/src/render/roads` |
| `uvEdgeConsistent` | — | não se aplica | `native/src/render/roads` |
| `weldVertices` | — | não se aplica | `native/src/render/roads` |
| `weldVertices.bits` | — | não se aplica | `native/src/render/roads` |
| `weldVertices.same` | — | não se aplica | `native/src/render/roads` |
| `weldVertices.take` | — | não se aplica | `native/src/render/roads` |

## `src/render/mesh/tileShape.ts` (inteiro, 578 linhas) — **não se aplica**

Dono: `native/src/render/roads`. Infraestrutura de render do 4.0 (ladrilhos, workers, WebGPU, residência). O jogo desenha por chunk com MeshBuilder e JobPool: porta-se o que se vê, não a máquina. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `Builder` | A piece's vertices and triangles as it is refined, in typed streams (`stream.ts`). | não se aplica | `native/src/render/roads` |
| `CUT_SLACK` | How far a cut may land from the line it was asked for | não se aplica | `native/src/render/roads` |
| `EdgeOwners` | The triangles that own each edge, for `flipToDelaunay`: open addressing over the edge's key (`edgeKey`), each slot its owners' count and the first two | não se aplica | `native/src/render/roads` |
| `EdgeOwners.add` | `tri` among the owners of the edge at `at`, last. | não se aplica | `native/src/render/roads` |
| `EdgeOwners.allocate` | — | não se aplica | `native/src/render/roads` |
| `EdgeOwners.clear` | The edge at `at` owned by none. | não se aplica | `native/src/render/roads` |
| `EdgeOwners.counts` | Owners per slot; -1: a slot no edge took. | não se aplica | `native/src/render/roads` |
| `EdgeOwners.first` | — | não se aplica | `native/src/render/roads` |
| `EdgeOwners.grow` | Twice the slots, every edge filed again in the order it came. | não se aplica | `native/src/render/roads` |
| `EdgeOwners.keys` | — | não se aplica | `native/src/render/roads` |
| `EdgeOwners.many` | The lists of the edges three or more triangles own, by slot. | não se aplica | `native/src/render/roads` |
| `EdgeOwners.order` | The slots taken, in the order their edges were first asked. | não se aplica | `native/src/render/roads` |
| `EdgeOwners.pair` | The edge at `at` owned by these two alone. | não se aplica | `native/src/render/roads` |
| `EdgeOwners.replace` | The edge at `at` owned by `tri` first, then by its owners less `gone`, in their order: `[tri, ...owners.filter((x) => x !== gone)]`. | não se aplica | `native/src/render/roads` |
| `EdgeOwners.reset` | Emptied for a piece of about `edges` edges. | não se aplica | `native/src/render/roads` |
| `EdgeOwners.scratch` | An edge's owners while `replace` writes them again. | não se aplica | `native/src/render/roads` |
| `EdgeOwners.second` | — | não se aplica | `native/src/render/roads` |
| `EdgeOwners.slot` | The slot of `key`, taken for it where it has none (count 0: owned by none). | não se aplica | `native/src/render/roads` |
| `EdgeOwners.taken` | — | não se aplica | `native/src/render/roads` |
| `Flips` | What a surface's pieces flip in (`surfaceShape`, one a surface): the edges' owners and the edges still to look at, grown and reused piece after piece. | não se aplica | `native/src/render/roads` |
| `Flips.owners` | — | não se aplica | `native/src/render/roads` |
| `Flips.stack` | — | não se aplica | `native/src/render/roads` |
| `PieceShape` | One compact piece's triangulation: its vertices, its triangles as clipped and refined, and whether they run clockwise. | não se aplica | `native/src/render/roads` |
| `SkirtRing` | One ring of an outline's skirt: its edges [ax, ay, bx, by, ...] in order, with its polygon's winding. | não se aplica | `native/src/render/roads` |
| `SurfaceShape` | A surface's shape: its pieces and, for a surface with a bottom, its skirts. | não se aplica | `native/src/render/roads` |
| `TileRect` | An axis-aligned rectangle a surface has been cut to: [minX, minY, maxX, maxY]. | não se aplica | `native/src/render/roads` |
| `addVertex` | — | não se aplica | `native/src/render/roads` |
| `alongCut` | Whether an edge runs along one of the lines a tile was cut on. | não se aplica | `native/src/render/roads` |
| `bounds` | Bounding box of a polygon's outer ring, [minX, minY, maxX, maxY]. | não se aplica | `native/src/render/roads` |
| `clipHalves` | The two sides of a cut, by intersection with a rectangle either side of it. | não se aplica | `native/src/render/roads` |
| `clipHalves.rect` | — | não se aplica | `native/src/render/roads` |
| `clipToRect` | The parts of `polygons` inside a rectangle, by four straight cuts (see `cutAtAxis`). | não se aplica | `native/src/render/roads` |
| `densify` | — | não se aplica | `native/src/render/roads` |
| `edgeKey` | — | não se aplica | `native/src/render/roads` |
| `edgeLength` | — | não se aplica | `native/src/render/roads` |
| `emit` | — | não se aplica | `native/src/render/roads` |
| `flipToDelaunay` | — | não se aplica | `native/src/render/roads` |
| `flipToDelaunay.inCircle` | — | não se aplica | `native/src/render/roads` |
| `flipToDelaunay.orient` | — | não se aplica | `native/src/render/roads` |
| `flipToDelaunay.push` | — | não se aplica | `native/src/render/roads` |
| `flipToDelaunay.put` | — | não se aplica | `native/src/render/roads` |
| `flipToDelaunay.third` | — | não se aplica | `native/src/render/roads` |
| `midpoint` | — | não se aplica | `native/src/render/roads` |
| `pieceShape` | One compact piece: ear clipping and refinement. | não se aplica | `native/src/render/roads` |
| `refine` | — | não se aplica | `native/src/render/roads` |
| `refine.long` | — | não se aplica | `native/src/render/roads` |
| `ringArea` | Twice the signed area of a ring of [x, y] pairs; positive means CCW. | não se aplica | `native/src/render/roads` |
| `skirtShape` | The edges the walls hang from, from the ORIGINAL outlines, never from the pieces | não se aplica | `native/src/render/roads` |
| `splitToSpan` | Cuts a polygon down until no piece is wider than `span` in either axis. ## Why this is here | não se aplica | `native/src/render/roads` |
| `surfaceShape` | The shape of `polygons` (one compact piece at a time, `splitToSpan`), and the skirts along their outline when `skirts` | não se aplica | `native/src/render/roads` |
| `trianglesArea` | Twice the summed signed area of triangles over a flat [x,y,...] list; positive means CCW. | não se aplica | `native/src/render/roads` |

## `src/render/networkFarRoads.ts` (inteiro, 281 linhas) — **não se aplica**

Dono: `native/src/render/roads`. Infraestrutura de render do 4.0 (ladrilhos, workers, WebGPU, residência). O jogo desenha por chunk com MeshBuilder e JobPool: porta-se o que se vê, não a máquina. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `ASPHALT_BY_CLASS` | — | não se aplica | `native/src/render/roads` |
| `ASPHALT_REFERENCE_COLOUR` | The class's tint over the reference the asphalt texture is authored against (`roadSurfaces.ts` CLASS_TINTS), times the texture's mean. | não se aplica | `native/src/render/roads` |
| `CASING` | — | não se aplica | `native/src/render/roads` |
| `COLOUR_STEPS` | A vertex's colour as stored (`Uint16Array` normalized: 0-65535 read as 0-1, three's `BufferAttribute.normalized`): a third of a float's bytes twice ov | não se aplica | `native/src/render/roads` |
| `FarRoadHeights` | What the outlines' heights are read from: the opening's solve (`RoadElevation`), the roads it left for later, the ground. | não se aplica | `native/src/render/roads` |
| `NetworkFarRoads` | The whole network's far roads (see the file's comment), KEPT FOR GOOD AS THE ROADS' TOP LEVEL (`mesh/roadResidency.ts`): a cell is shown where no bloc | não se aplica | `native/src/render/roads` |
| `NetworkFarRoads.against` | The cells against the rule (`settledLevel`): shown ones once settled, hidden ones before. | não se aplica | `native/src/render/roads` |
| `NetworkFarRoads.asked` | — | não se aplica | `native/src/render/roads` |
| `NetworkFarRoads.cellKeys` | The cells drawing roads, by key (`x,y` on the cells' grid). | não se aplica | `native/src/render/roads` |
| `NetworkFarRoads.cells` | Each cell's triangles, by its key (`x,y` on the cells' grid): what the probes count. | não se aplica | `native/src/render/roads` |
| `NetworkFarRoads.clear` | The level let go: another map opened. | não se aplica | `native/src/render/roads` |
| `NetworkFarRoads.clearMeshes` | — | não se aplica | `native/src/render/roads` |
| `NetworkFarRoads.constructor` | — | não se aplica | `native/src/render/roads` |
| `NetworkFarRoads.covers` | Whether the level standing draws the cell's roads as the blocks do: made of the roads drawn, with roads there, none of it left to the detailed surface | não se aplica | `native/src/render/roads` |
| `NetworkFarRoads.dispose` | — | não se aplica | `native/src/render/roads` |
| `NetworkFarRoads.group` | — | não se aplica | `native/src/render/roads` |
| `NetworkFarRoads.hide` | A cell hidden: the detailed blocks of its square stand (the opening's fill landed, or a cell made again, `roadResidency.ts`). | não se aplica | `native/src/render/roads` |
| `NetworkFarRoads.made` | The level standing: the request it answers (`asked`), its cells' side and origin, the area it left to the detailed surfaces. Null: none landed. | não se aplica | `native/src/render/roads` |
| `NetworkFarRoads.materials` | — | não se aplica | `native/src/render/roads` |
| `NetworkFarRoads.meshesOf` | Each cell's instances in the layers' batches, by its key. | não se aplica | `native/src/render/roads` |
| `NetworkFarRoads.outdate` | The roads drawn changed (a build of a road edit landed, `renderer.ts`): every level asked so far stands for none of them. | não se aplica | `native/src/render/roads` |
| `NetworkFarRoads.outdated` | Whether the level standing is not of the roads drawn (none, made before they changed, or cut by the opening's view): made again when a cell must be le | não se aplica | `native/src/render/roads` |
| `NetworkFarRoads.pending` | — | não se aplica | `native/src/render/roads` |
| `NetworkFarRoads.place` | — | não se aplica | `native/src/render/roads` |
| `NetworkFarRoads.request` | The level made for the network as it stands (replacing any when it lands), the area `exclude` left to the detailed surfaces. | não se aplica | `native/src/render/roads` |
| `NetworkFarRoads.request.failed` | — | não se aplica | `native/src/render/roads` |
| `NetworkFarRoads.request.landed` | — | não se aplica | `native/src/render/roads` |
| `NetworkFarRoads.settle` | The blocks draw every cell now (the fill landed): the level hidden but where `shown` (the cells let go) asks. | não se aplica | `native/src/render/roads` |
| `NetworkFarRoads.settled` | Whether the blocks draw every cell, the level shown only where it is asked to (`show`). | não se aplica | `native/src/render/roads` |
| `NetworkFarRoads.settledLevel` | Whether a cell is hidden unless shown (`settle`: the blocks draw every cell) or shown unless hidden (the opening's, till the fill lands). | não se aplica | `native/src/render/roads` |
| `NetworkFarRoads.show` | A cell shown: its blocks' far levels let go (`roadResidency.ts` dropCell). | não se aplica | `native/src/render/roads` |
| `NetworkFarRoads.shown` | — | não se aplica | `native/src/render/roads` |
| `NetworkFarRoads.staleThrough` | The last request made before the roads drawn changed (`outdate`): a level answering it or one before stands for none of them. | não se aplica | `native/src/render/roads` |
| `NetworkFarRoads.triangleCount` | — | não se aplica | `native/src/render/roads` |
| `NetworkFarRoads.triangles` | — | não se aplica | `native/src/render/roads` |
| `NetworkFarRoads.waiting` | A request not answered yet: the opening waits for it (`pending`). | não se aplica | `native/src/render/roads` |
| `NetworkFarRoads.worker` | — | não se aplica | `native/src/render/roads` |
| `farRoadInput` | @rebuild The outlines of the network, their heights read on the page (`heights`, `groundAt` for the roads the solve left): what the worker fills. Once | não se aplica | `native/src/render/roads` |
| `farRoadInput.add` | — | não se aplica | `native/src/render/roads` |
| `farRoadInput.onGround` | — | não se aplica | `native/src/render/roads` |
| `farRoadInput.roadAt` | — | não se aplica | `native/src/render/roads` |
| `layerMaterial` | One material a layer: the colours the vertices', pulled over the ground it lies on (the casing under the carriageway, as `roadStandIn.ts` draws them). | não se aplica | `native/src/render/roads` |

## `src/render/networkFarRoads.worker.ts` (parte, 14 linhas) — **não se aplica**

Dono: `native/src/render/roads`. Infraestrutura de render do 4.0 (ladrilhos, workers, WebGPU, residência). O jogo desenha por chunk com MeshBuilder e JobPool: porta-se o que se vê, não a máquina. Classificado por pasta: não li cada declaração.


## `src/render/networkFarRoadsBuild.ts` (inteiro, 188 linhas) — **não se aplica**

Dono: `native/src/render/roads`. Infraestrutura de render do 4.0 (ladrilhos, workers, WebGPU, residência). O jogo desenha por chunk com MeshBuilder e JobPool: porta-se o que se vê, não a máquina. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `Builder` | — | não se aplica | `native/src/render/roads` |
| `CellOutlines` | Each cell's outlines, by layer: the casing's rings, and the carriageway's by colour (in the order the colours first came). | não se aplica | `native/src/render/roads` |
| `EdgeHeights` | The outlines' edges filed by bucket: a point's height is read on the nearest edge, interpolated along it (a clipped vertex is not one of the outline's | não se aplica | `native/src/render/roads` |
| `EdgeHeights.at` | The height of the edge nearest (x, y), where its foot falls along it; 0 where no edge is near. | não se aplica | `native/src/render/roads` |
| `EdgeHeights.buckets` | — | não se aplica | `native/src/render/roads` |
| `EdgeHeights.constructor` | — | não se aplica | `native/src/render/roads` |
| `EdgeHeights.key` | — | não se aplica | `native/src/render/roads` |
| `FarRoadCell` | One cell's layer: positions in the scene (x, height, -y), a colour a vertex, the triangles. | não se aplica | `native/src/render/roads` |
| `FarRoadInput` | What the page hands over: each outline's vertices (x, y, height), how many each has, its layer (0 the casing, 1 the carriageway) and its colour (linea | não se aplica | `native/src/render/roads` |
| `HEIGHT_BUCKET` | The side of the buckets the outlines' edges are filed in to read a height (`EdgeHeights`), world units: wider than half the widest road, so a cut acro | não se aplica | `native/src/render/roads` |
| `buildFarRoadCells` | The cells' layers, each disjoint from the other and clipped to its cell, the excluded area cut out (see the file's comment). | não se aplica | `native/src/render/roads` |
| `fill` | Each polygon of `shape` triangulated (its holes kept), laid at the heights read and painted `colour`, into `b`. | não se aplica | `native/src/render/roads` |

## `src/render/paintQuads.ts` (inteiro, 119 linhas) — **não portado**

Dono: `native/src/render/roads`. Falta conferir um a um o que sobra do render de vias. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `PAINT_COLOURS` | — | não portado | `native/src/render/roads` |
| `PAINT_RISE` | How far paint stands above the asphalt. Enough to win the depth test. | não portado | `native/src/render/roads` |
| `QuadSink` | Where a stroke's quads go, one ring each, with its colour (`roadSurfaceInputs.ts`). | não portado | `native/src/render/roads` |
| `STOP_PAINT` | The stop lines' and the zebras' paint. | não portado | `native/src/render/roads` |
| `ZEBRA_PAINT` | — | não portado | `native/src/render/roads` |
| `barQuad` | A stop line or a zebra's bar as one quad. | não portado | `native/src/render/roads` |
| `quad` | — | não portado | `native/src/render/roads` |
| `roadStrokes` | A road's own paint along it (`segmentMarkings`), from its mouth to its stop lines or crossings. | não portado | `native/src/render/roads` |
| `roadStrokes.cut` | — | não portado | `native/src/render/roads` |
| `strokeQuads` | One stroke as quads, dashed as its pattern says. | não portado | `native/src/render/roads` |

## `src/render/platePaint.ts` (inteiro, 37 linhas) — **não portado**

Dono: `native/src/render/roads`. Falta conferir um a um o que sobra do render de vias. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `dabInRects` | — | não portado | `native/src/render/roads` |
| `dabKey` | A dab's identity on a plate: a tile come back brings its dabs as new objects of the same paint (a dab a player repeats exactly, at the same place and  | não portado | `native/src/render/roads` |
| `plateDabs` | THE DABS A PLATE LAYS brought up to the document's (`reaching`: those reaching the plate now). What was laid stays as laid: a tile that left the page  | não portado | `native/src/render/roads` |
| `sameList` | — | não portado | `native/src/render/roads` |

## `src/render/poleBake.ts` (parte, 6 linhas) — **não se aplica**

Dono: `street-furniture` (porte-6). Postes e fios são de `street-furniture`.


## `src/render/poleCells.ts` (parte, 25 linhas) — **não se aplica**

Dono: `street-furniture` (porte-6). Postes e fios são de `street-furniture`.


## `src/render/poleLayer.ts` (parte, 23 linhas) — **não se aplica**

Dono: `street-furniture` (porte-6). Postes e fios são de `street-furniture`.


## `src/render/propCells.ts` (parte, 4 linhas) — **não portado**

Dono: `native/src/render/roads`. Falta conferir um a um o que sobra do render de vias. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `createPropLayer.forgetGrowing` | — | não portado | `native/src/render/roads` |

## `src/render/propGeometry.ts` (parte, 34 linhas) — **não portado**

Dono: `native/src/render/roads`. Falta conferir um a um o que sobra do render de vias. Classificado por pasta: não li cada declaração.


## `src/render/renderer.ts` (parte, 1236 linhas) — **não portado**

Dono: `native/src/render/roads`. Falta conferir um a um o que sobra do render de vias. Classificado por pasta: não li cada declaração.


## `src/render/roadProgramStandIns.ts` (inteiro, 59 linhas) — **não se aplica**

Dono: `native/src/render/roads`. Infraestrutura de render do 4.0 (ladrilhos, workers, WebGPU, residência). O jogo desenha por chunk com MeshBuilder e JobPool: porta-se o que se vê, não a máquina. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `LOOK_SOURCES` | Every surface a block of the roads can draw, for its programs' stand-ins: each band, each median part, each paint colour of the catalogue. | não se aplica | `native/src/render/roads` |
| `noSection` | Each vertex's place in no road's section (`tileMesh.ts`: a surface without one). | não se aplica | `native/src/render/roads` |
| `roadProgramStandIns` | THE ROADS' PROGRAMS AMONG THE WORLD'S, never drawn: one invisible triangle per look of a road surface (`surfaceLook`), raised and not (a raised surfac | não se aplica | `native/src/render/roads` |
| `roadProgramStandIns.dispose` | — | não se aplica | `native/src/render/roads` |

## `src/render/roadsRead.ts` (inteiro, 13 linhas) — **não se aplica**

Dono: `native/src/render/roads`. Infraestrutura de render do 4.0 (ladrilhos, workers, WebGPU, residência). O jogo desenha por chunk com MeshBuilder e JobPool: porta-se o que se vê, não a máquina. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `RoadsRead` | THE ROADS THE NEXT SOLVE READS THE GROUND UNDER, AS LAST WORKED OUT (`elevation.ts` roadsToRead), and what for (`key`: what they were worked out of).  | não se aplica | `native/src/render/roads` |
| `RoadsRead.kept` | — | não se aplica | `native/src/render/roads` |
| `RoadsRead.of` | — | não se aplica | `native/src/render/roads` |

## `src/render/roadStandIn.ts` (inteiro, 135 linhas) — **não se aplica**

Dono: `native/src/render/roads`. Infraestrutura de render do 4.0 (ladrilhos, workers, WebGPU, residência). O jogo desenha por chunk com MeshBuilder e JobPool: porta-se o que se vê, não a máquina. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `RoadStandIn` | THE ROADS JUST BUILT, BY THE COMMIT THEY CAME WITH, until the world's build of that commit lands. | não se aplica | `native/src/render/roads` |
| `RoadStandIn.asphalt` | — | não se aplica | `native/src/render/roads` |
| `RoadStandIn.byRevision` | Each commit's stand-in meshes by the network revision it made: let go once a build of that revision landed (`landed`). DO NOT keep only the last commi | não se aplica | `native/src/render/roads` |
| `RoadStandIn.casing` | — | não se aplica | `native/src/render/roads` |
| `RoadStandIn.clear` | Every stand-in let go (another map, the diary read from nothing). | não se aplica | `native/src/render/roads` |
| `RoadStandIn.constructor` | — | não se aplica | `native/src/render/roads` |
| `RoadStandIn.dispose` | — | não se aplica | `native/src/render/roads` |
| `RoadStandIn.group` | — | não se aplica | `native/src/render/roads` |
| `RoadStandIn.landed` | A build of the world landed, of network revision `revision`: every commit it holds (made at that revision or before) let go, the later ones kept. | não se aplica | `native/src/render/roads` |
| `RoadStandIn.revisions` | The commits whose stand-ins wait for the world's build (`show`). | não se aplica | `native/src/render/roads` |
| `RoadStandIn.show` | The roads `segments` drawn now (the commit's network solved, of network revision `revision`): their outlines, and the junctions at their ends whose ev | não se aplica | `native/src/render/roads` |
| `STREET_MEANS` | THE MEAN LINEAR ALBEDO OF EACH BAKED STREET TEXTURE (`bake/roadRecipes.ts`), every texel as stored (sRGB bytes) and read (linear): constants, worked o | não se aplica | `native/src/render/roads` |
| `StandInHeights` | What the stand-in reads of the roads' heights (`RoadElevation`). | não se aplica | `native/src/render/roads` |
| `fill` | A closed outline (x, y pairs) triangulated and laid at `height`: positions, normals up, indices appended. | não se aplica | `native/src/render/roads` |
| `meshOf` | A mesh of the outlines laid, or null for none. | não se aplica | `native/src/render/roads` |
| `standInMaterial` | One material each, made once: the mean of the street's texture (`STREET_MEANS`, linear, as the far level paints it; a light slab stood out over the as | não se aplica | `native/src/render/roads` |

## `src/render/roadStations.ts` (inteiro, 84 linhas) — **não se aplica**

Dono: `native/src/render/roads`. Infraestrutura de render do 4.0 (ladrilhos, workers, WebGPU, residência). O jogo desenha por chunk com MeshBuilder e JobPool: porta-se o que se vê, não a máquina. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `Hint` | A road's ground as read, with the plates its box read (each as the share held it: a plate made or let go since is read again) and the terrain's revisi | não se aplica | `native/src/render/roads` |
| `OFF_PAGE_FROM` | Fewer roads than this are read on the page (an edit's few): a job's round trip would cost more than their reads. | não se aplica | `native/src/render/roads` |
| `RoadStations` | — | não se aplica | `native/src/render/roads` |
| `RoadStations.asked` | — | não se aplica | `native/src/render/roads` |
| `RoadStations.constructor` | `landed`: a frame asked for once an answer is in (the solve runs again then). | não se aplica | `native/src/render/roads` |
| `RoadStations.current` | The hints for the solve (`buildRoadElevation` checks each road's line and casing again). | não se aplica | `native/src/render/roads` |
| `RoadStations.hints` | — | não se aplica | `native/src/render/roads` |
| `RoadStations.off` | The job system failed it once: read on the page from then on. | não se aplica | `native/src/render/roads` |
| `RoadStations.used` | After a solve: what it read is in the solve now (kept there for the next, `stationsFrom`). | não se aplica | `native/src/render/roads` |
| `RoadStations.waiting` | Whether the solve must wait for the ground under `roads` (each read off the page unless its hint stands); asked now if not yet. False: solve now, with | não se aplica | `native/src/render/roads` |
| `RoadStations.waiting.stands` | — | não se aplica | `native/src/render/roads` |
| `platesUnder` | The plates a road's casing reads, with the buffer the share holds for each now. | não se aplica | `native/src/render/roads` |
| `sameXY` | — | não se aplica | `native/src/render/roads` |

## `src/render/roadStations.worker.ts` (parte, 8 linhas) — **não se aplica**

Dono: `native/src/render/roads`. Infraestrutura de render do 4.0 (ladrilhos, workers, WebGPU, residência). O jogo desenha por chunk com MeshBuilder e JobPool: porta-se o que se vê, não a máquina. Classificado por pasta: não li cada declaração.


## `src/render/roadSurfaceEntries.ts` (inteiro, 200 linhas) — **não se aplica**

Dono: `native/src/render/roads`. Infraestrutura de render do 4.0 (ladrilhos, workers, WebGPU, residência). O jogo desenha por chunk com MeshBuilder e JobPool: porta-se o que se vê, não a máquina. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `CellHold` | The items a cell of the residency found when it was derived: let go with it unless another held cell meets them. | não se aplica | `native/src/render/roads` |
| `DETAILS` | The junctions' stop lines, zebras, islands and legends | não se aplica | `native/src/render/roads` |
| `ENTRY_BYTES` | What an entry's input holds besides its points, and each point (a two-number array in a ring), bytes: an estimate for the budget, set against the heap | não se aplica | `native/src/render/roads` |
| `Entry` | One input of an item, as a pass files it: its kind (`slot`), its pass, and where a walk of the map meets it - `phase` | não se aplica | `native/src/render/roads` |
| `InputResidency` | How the inputs are held round the view (`viewCells.ts`): what the cells derived round the tiles visited may hold on the CPU, and how far round the eye | não se aplica | `native/src/render/roads` |
| `LEVEL_OF` | — | não se aplica | `native/src/render/roads` |
| `LEVEL_SLOTS` | — | não se aplica | `native/src/render/roads` |
| `MEDIAN_SLOTS` | — | não se aplica | `native/src/render/roads` |
| `NODE_PAD` | How far past its roads' boxes and its plates a node's inputs may lie (its dropped kerbs, its paint). | não se aplica | `native/src/render/roads` |
| `NodeItem` | — | não se aplica | `native/src/render/roads` |
| `PAINT_PREFIX` | — | não se aplica | `native/src/render/roads` |
| `PHASE` | The phases of the walk the inputs are ordered by, as `roadSurfaces.ts` once walked the map | não se aplica | `native/src/render/roads` |
| `POINT_BYTES` | — | não se aplica | `native/src/render/roads` |
| `PassInputs` | One pass's tiles, by key. | não se aplica | `native/src/render/roads` |
| `ROAD_INPUT_RESIDENCY` | The game's residency (`renderer.ts`): the cells round the roads' focus kept, where the player edits (a tile farther is derived when a build visits it, | não se aplica | `native/src/render/roads` |
| `Reach` | Everything that reaches a tile of a pass, by kind, in input order. | não se aplica | `native/src/render/roads` |
| `SegmentItem` | — | não se aplica | `native/src/render/roads` |
| `Superset` | Where an item not derived now may put inputs: the tiles its box reaches in each pass it may draw in. | não se aplica | `native/src/render/roads` |
| `SurfacePass` | A pass of the surfaces as the build walks it: a structural level, or an alignment raised by hand. | não se aplica | `native/src/render/roads` |
| `TILE` | World size of one tile. | não se aplica | `native/src/render/roads` |
| `TILE_BIAS` | — | não se aplica | `native/src/render/roads` |
| `TILE_BYTES` | Each tile an entry is filed in: its place in the tile's set. | não se aplica | `native/src/render/roads` |
| `TILE_ORIGIN` | Where the tile grid starts. | não se aplica | `native/src/render/roads` |
| `TILE_REACH` | How far round a tile a ring may reach and still be read by it. | não se aplica | `native/src/render/roads` |
| `TileInputs` | A tile of one pass: its entries by slot, and what it reads once put together. | não se aplica | `native/src/render/roads` |
| `WAKE_PAD` | How far past a tile its visit derives the items round it (`wakeTile`): a junction's plate and a corner's dropped kerb past its roads' boxes, the footw | não se aplica | `native/src/render/roads` |
| `alignmentPass` | — | não se aplica | `native/src/render/roads` |
| `byWalk` | — | não se aplica | `native/src/render/roads` |
| `entryBytes` | What an item's entries hold on the CPU, bytes (an estimate, `ENTRY_BYTES`). | não se aplica | `native/src/render/roads` |
| `entryOf` | — | não se aplica | `native/src/render/roads` |
| `growRect` | — | não se aplica | `native/src/render/roads` |
| `isGroundPass` | A pass at ground level: the ground's, or an alignment raised by hand (drawn from the ground's structure). | não se aplica | `native/src/render/roads` |
| `medianSlot` | — | não se aplica | `native/src/render/roads` |
| `paintSlot` | — | não se aplica | `native/src/render/roads` |
| `surfaceInputWork` | How many items had their inputs made, for the scale tests: an edit far away makes few. | não se aplica | `native/src/render/roads` |
| `tileKey` | A tile's key from its column and row. | não se aplica | `native/src/render/roads` |
| `tileRectOf` | A tile's rectangle from its key. | não se aplica | `native/src/render/roads` |
| `tilesOver` | The tiles a box reaches, by the rule an entry is filed by (`entryOf`). | não se aplica | `native/src/render/roads` |
| `unite` | — | não se aplica | `native/src/render/roads` |

## `src/render/roadSurfaceInputs.ts` (inteiro, 776 linhas) — **não se aplica**

Dono: `native/src/render/roads`. Infraestrutura de render do 4.0 (ladrilhos, workers, WebGPU, residência). O jogo desenha por chunk com MeshBuilder e JobPool: porta-se o que se vê, não a máquina. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `EDGE_PAD` | How far round a footway's cell its outline is read (`footwayEdges`): twice the batter's reach. | não se aplica | `native/src/render/roads` |
| `PARKING_REACH` | How far from a building's box a street's parking may read its gates (`parkingLayout.ts` `entrancesNear`). | não se aplica | `native/src/render/roads` |
| `SurfaceInputs` | The inputs of every pass, kept between builds and brought up to the network by its change (`refresh`). | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.births` | Passes made so far: each pass's birth number. | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.buildings` | The buildings' entries read (a street's parking keeps clear of their gates, `parkingLayout.ts`). | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.cells` | The cells the inputs are held in round the view; null: every item derived for good. | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.chains` | — | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.colour` | — | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.constructor` | `residency`: the inputs held round the view, cell by cell (`viewCells.ts`), each item derived only when a tile it reaches is visited; none: every item | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.count` | The document's roads in each pass. | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.deriveCell` | A cell of the residency derived: every road and node whose box meets it derived (silently: the same inputs they had), and found. | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.details` | The junctions' paint (`DETAILS`), by tile and slot. | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.dirtyOf` | A pass's birth and the tiles whose inputs changed since the build last settled them; null when the pass is not there. | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.dormant` | An item made not derived: its superset's tiles counted in its passes (`PassInputs.dormant`) before its entries go, so no pass is left empty on the way | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.dormantNodes` | — | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.dormantSegments` | The items not derived now, filed by their boxes: what a visited tile derives (`wakeTile`). | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.file` | `silent`: entries derived again as they were (`wakeTile`), or let go while what they drew still stands (`dormant`): what the tiles read is put togethe | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.fileNode` | A node filed as not derived: the passes of its roads, its box's tiles. | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.fileSegment` | A road filed as not derived (held round the view): its pass counted, its box's tiles filed (`Superset`). | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.footwayEdges` | DO NOT merge every footway of the pass for it | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.hasTile` | Whether a pass has a tile at `key`: inputs filed there, or an item not derived now that may put some (`wakeTile` derives them). | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.held` | Whether a box meets a cell held now (other than `except`). | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.keepNear` | The inputs brought back to the view after a build (`roadSurfaces.ts`): the cells round the eye wanted (`most` of them derived ahead), every item deriv | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.letGoNode` | — | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.letGoSegment` | — | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.manual` | The roads with an end raised by hand: drawn in an alignment of their own. | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.met` | Every paint colour met (a handful: the catalogue's, `PAINT_COLOURS`, and any it lacks). | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.net` | — | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.nodeBox` | The box a node's inputs may lie in: its roads' boxes and its plates', grown by `NODE_PAD`; null for a node with neither. | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.nodePasses` | The passes a node's inputs may be drawn in: its roads'. | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.nodes` | — | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.ownerOf` | The pass a junction's plates are drawn in: that of its lowest road that is not a tunnel | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.paintColours` | Every colour a pass may paint in: the catalogue's, then any other met | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.paintedColours` | The colours the roads and junctions have been painted in. | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.passFor` | A pass's inputs, made if it has none yet. | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.passList` | The passes of the surfaces: every structural level, then each alignment raised by hand, by its lowest road. | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.passOf` | The pass a road is drawn in: its alignment when raised by hand, else its structural level. | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.passes` | — | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.placeNode` | A node's inputs made again and filed: its plates, taper, island, dropped kerbs and junction paint. | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.placeNode.add` | — | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.placeNode.bars` | — | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.placeNode.strokes` | — | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.placeNode.strokes.sink` | — | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.placeSegment` | A road's inputs made again and filed. `silent`: derived again as it was (held round the view, `wakeTile`): no tile made dirty. | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.present` | Whether some road of the document is drawn in a pass. | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.raisedByHand` | Whether a road has an end raised by hand: it is drawn from its own deck, in an alignment. | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.reachOf` | What a tile of a pass reads, put together when something in it changed | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.reachOf.addAll` | — | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.reachOf.collect` | — | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.reachOf.list` | — | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.reassign` | The alignments of `ids` made again | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.rebuildAll` | @rebuild Every item made again: the network could not say what changed (a load, the first build). Held round the view, every item is only filed by its | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.refresh` | @rebuild Brings the inputs up to `net`: the items its change since the last refresh names, and the roads round them, made again | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.refresh.ends` | — | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.releaseCell` | A cell let go: the items it found that no other held cell meets made not derived again. | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.resetting` | Set while every item is let go at once (`rebuildAll`): the cells let go with them make nothing dormant. | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.residentBytes` | What the derived items' inputs hold, bytes (an estimate, `entryBytes`). | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.residentNodes` | — | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.residentSegments` | The items derived now (held round the view only). | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.rootOf` | Each such road's alignment, by its lowest id; and each alignment's roads, in id order. | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.segmentBox` | The box a road's inputs lie in: its ribbon's (`ribbonBounds`, every level), or null when it has none. | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.segments` | — | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.settle` | The build that took these tiles of a pass finished: they are no longer dirty (a pass made again since keeps its own). | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.style` | — | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.tileKeys` | The keys of a pass's tiles, in order. | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.token` | The network's state the inputs stand for (`Network.state`). | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.touchDetails` | The junctions' paint in a tile changed: every pass at ground level reads that tile again. | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.undormant` | A dormant item's superset taken out of its passes (it is derived now, or gone); `dirty`: its tiles visited by the next build (what it drew there may h | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.unfile` | — | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.visits` | A tile a build visits: the items reaching it derived (`wakeTile`), then whether the pass has it. | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.wakeRect` | The cells a rectangle reaches derived (`deriveCell`), none of them let go for another. | não se aplica | `native/src/render/roads` |
| `SurfaceInputs.wakeTile` | The items that may put inputs in tile `key` derived, with the cells round it (`WAKE_PAD`): what a build visiting the tile reads is whole. Nothing when | não se aplica | `native/src/render/roads` |

## `src/render/roadSurfacePass.ts` (inteiro, 143 linhas) — **não se aplica**

Dono: `native/src/render/roads`. Infraestrutura de render do 4.0 (ladrilhos, workers, WebGPU, residência). O jogo desenha por chunk com MeshBuilder e JobPool: porta-se o que se vê, não a máquina. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `GROUND_MAX_EDGE` | Longest triangle edge on a road at grade. | não se aplica | `native/src/render/roads` |
| `MeshedSurface` | A surface of a pass as a tile is made of it: what it is cut from and how it is meshed. | não se aplica | `native/src/render/roads` |
| `PassLook` | The page's constants a pass is drawn with: its materials' texture scales, whether the ground verge wears the terrain's material, the carriageway's tin | não se aplica | `native/src/render/roads` |
| `RAISED_MAX_EDGE` | A raised deck follows nothing, so it needs vertices only for its shading. | não se aplica | `native/src/render/roads` |
| `followsGround` | Whether a road at grade has ground to follow: on a new map's level ground (`RELIEF_FLAT`, no stamp) it follows nothing either, and is laid as a deck i | não se aplica | `native/src/render/roads` |
| `lookOf` | The road a surface reads beside the field, as a pass sees it (`RoadLook`): its paving, widths and mouths. | não se aplica | `native/src/render/roads` |
| `passDesc` | The pass as its surfaces read it, plain data: the same on the page and in a worker (`roadSurfaceReads.ts`). | não se aplica | `native/src/render/roads` |
| `passRaised` | Whether a pass is drawn as a raised deck: an alignment lifted by hand where some point of its roads stands a structure's lift over the ground; a struc | não se aplica | `native/src/render/roads` |
| `passReads` | What a pass's surfaces read where the network is at hand (the page's, or a worker's own): the solved field, the ground as drawn, each road's look and  | não se aplica | `native/src/render/roads` |
| `passReads.edgesAt` | — | não se aplica | `native/src/render/roads` |
| `passReads.footwayShare` | — | não se aplica | `native/src/render/roads` |
| `passReads.lineOf` | — | não se aplica | `native/src/render/roads` |
| `passReads.look` | — | não se aplica | `native/src/render/roads` |
| `passReads.walkingRise` | — | não se aplica | `native/src/render/roads` |
| `polysOf` | A tile's rings without their digests: what its shapes are cut from. | não se aplica | `native/src/render/roads` |
| `polysOf.polys` | — | não se aplica | `native/src/render/roads` |
| `tileBundle` | Every surface of one tile, from the rings that reach it, each vertex read here (`meshTileShape`): by surface name, in the order of `specs`. | não se aplica | `native/src/render/roads` |

## `src/render/roadSurfaceReads.ts` (inteiro, 360 linhas) — **não se aplica**

Dono: `native/src/render/roads`. Infraestrutura de render do 4.0 (ladrilhos, workers, WebGPU, residência). O jogo desenha por chunk com MeshBuilder e JobPool: porta-se o que se vê, não a máquina. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `FOOTWAY_EDGE_CELL` | The grid the footways' outer edges are filed by for the verge's fall (`fromFootway`). | não se aplica | `native/src/render/roads` |
| `FOOTWAY_PANEL_TILES` | Slabs in a footway panel along a bend: a joint every this many tiles (`surfaceFrameAt`). | não se aplica | `native/src/render/roads` |
| `KERB_RISE` | Height of the kerb face above the carriageway. | não se aplica | `native/src/render/roads` |
| `MEDIAN_KERB` | Height of the median island's kerb above the carriageway: the footway's kerb (15 cm). | não se aplica | `native/src/render/roads` |
| `MEDIAN_PLANTING` | Height of the planting inside it: the soil a hair over the kerb's top, so the two are never one plane. | não se aplica | `native/src/render/roads` |
| `PassDesc` | A road pass as its surfaces are read (`roadSurfaceSteps`): plain data, handed to a worker as it is. | não se aplica | `native/src/render/roads` |
| `RAMP_MAX_EDGE` | A dropped kerb's triangles: its slope is 1.8 m long, its flares 1.5 m. | não se aplica | `native/src/render/roads` |
| `RoadLook` | What a surface reads of a road beside the field: its paving, widths and its carriageway's span between its mouths. | não se aplica | `native/src/render/roads` |
| `SurfaceRead` | One surface of a pass: what its shape is cut from and what each vertex reads. | não se aplica | `native/src/render/roads` |
| `SurfaceReads` | Everything a pass's surfaces read besides the point: on the page the network's, in a worker the roads handed to it. | não se aplica | `native/src/render/roads` |
| `TERRAIN_UV` | World size of one UV unit on the ground verge when it wears the terrain's material (and a retaining wall's backfill, `structures.ts`). | não se aplica | `native/src/render/roads` |
| `VERGE_DROP` | Depth of the verge below the carriageway, where the grass starts. | não se aplica | `native/src/render/roads` |
| `VERGE_LIFT` | The verge meets the drawn terrain this far BELOW it | não se aplica | `native/src/render/roads` |
| `VERGE_SKIRT` | Drop of the verge skirt into the ground. | não se aplica | `native/src/render/roads` |
| `footwayEdgeCell` | The key of a cell of that grid. | não se aplica | `native/src/render/roads` |
| `offset` | — | não se aplica | `native/src/render/roads` |
| `passSurfaces` | The surfaces of a pass, outermost first, then its paint in the network's order. | não se aplica | `native/src/render/roads` |
| `passSurfaces.asphaltSection` | A point's place in the carriageway's cross-section, for the marks of its use (`mesh/roadEdges.ts`) | não se aplica | `native/src/render/roads` |
| `passSurfaces.asphaltTint` | The carriageway's tint, taken from the class of the nearest road and normalised against the colour the asphalt texture | não se aplica | `native/src/render/roads` |
| `passSurfaces.footwaySection` | A point's place across the footway: metres from the kerb's back, and the footway's width to the lot. | não se aplica | `native/src/render/roads` |
| `passSurfaces.footwayTint` | The footway's tone: its side's material on the road nearest the point. | não se aplica | `native/src/render/roads` |
| `passSurfaces.footwayTop` | Where the footway stands: a kerb's rise over the carriageway, or level with it where it is laid flush. | não se aplica | `native/src/render/roads` |
| `passSurfaces.frameFor` | — | não se aplica | `native/src/render/roads` |
| `passSurfaces.fromFootway` | — | não se aplica | `native/src/render/roads` |
| `passSurfaces.segmentAt` | The segment of the pass a point belongs to: the one whose centreline is nearest. | não se aplica | `native/src/render/roads` |
| `passSurfaces.terrainUv` | — | não se aplica | `native/src/render/roads` |
| `passSurfaces.uvFor` | Road-framed UVs, with every triangle kept inside one road's frame. | não se aplica | `native/src/render/roads` |
| `passSurfaces.uvFor.uv` | — | não se aplica | `native/src/render/roads` |
| `passSurfaces.vergeTop` | — | não se aplica | `native/src/render/roads` |
| `tileSpecOf` | What a surface's shape is made from (`roadTileShape.ts`). | não se aplica | `native/src/render/roads` |

## `src/render/roadSurfaceRings.ts` (inteiro, 78 linhas) — **não se aplica**

Dono: `native/src/render/roads`. Infraestrutura de render do 4.0 (ladrilhos, workers, WebGPU, residência). O jogo desenha por chunk com MeshBuilder e JobPool: porta-se o que se vê, não a máquina. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `Input` | One ring of input, with its box and its digest, measured once when its item is made. | não se aplica | `native/src/render/roads` |
| `inputOf` | — | não se aplica | `native/src/render/roads` |
| `medianOf` | A road's central reservation, as two real polygons rather than as paint | não se aplica | `native/src/render/roads` |
| `medianOf.back` | — | não se aplica | `native/src/render/roads` |
| `strip` | A closed ribbon `half` wide either side of a centreline. | não se aplica | `native/src/render/roads` |

## `src/render/roadSurfaces.ts` (inteiro, 824 linhas) — **não se aplica**

Dono: `native/src/render/roads`. Infraestrutura de render do 4.0 (ladrilhos, workers, WebGPU, residência). O jogo desenha por chunk com MeshBuilder e JobPool: porta-se o que se vê, não a máquina. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `CARRIAGEWAY_TINT` | A carriageway's tone by what it is paved with; asphalt keeps its class's. | não se aplica | `native/src/render/roads` |
| `CHUNK` | Tiles a side of one mesh: eight tiles of 76.8 m, a mesh a surface over 614 m of roads. An edit re-merges its block's tiles, never builds them again (k | não se aplica | `native/src/render/roads` |
| `CLASS_TINT` | Linear-space tints per road class, so the asphalt mesh can vary by class. | não se aplica | `native/src/render/roads` |
| `CLASS_TINTS` | The carriageway's tints as a pass reads them (`PassDesc`): numbers, made once here with three's colour management. | não se aplica | `native/src/render/roads` |
| `DrawGroup` | Surfaces drawn alike - one material, order and shadows - drawn as one mesh a block (`drawGroups`): its name, theirs joined by `+`. | não se aplica | `native/src/render/roads` |
| `IN_FLIGHT` | Batches of tiles asked of the workers ahead of the one being taken (`buildAll`): each worker's next share waits in its queue. | não se aplica | `native/src/render/roads` |
| `KeptTile` | A tile kept between builds: the names of the surfaces it has. Its digest is its key. | não se aplica | `native/src/render/roads` |
| `PAVING_TINTS` | — | não se aplica | `native/src/render/roads` |
| `PassState` | A pass of the surfaces as the last build left it (`roadSurfaceSteps`): the constants it was built with, its inputs' birth, its tiles by key (no arrays | não se aplica | `native/src/render/roads` |
| `PassTile` | One tile of a pass in a build: its digest and block, its surfaces and arrays once known. DO NOT keep its rings with it: read from the inputs when it i | não se aplica | `native/src/render/roads` |
| `REFERENCE_TINT` | — | não se aplica | `native/src/render/roads` |
| `RoadSurfaces` | — | não se aplica | `native/src/render/roads` |
| `SHAPE_BATCH` | Tiles a batch asked of the workers at once (`roadTilePool.ts`) | não se aplica | `native/src/render/roads` |
| `Source` | What each surface of a tile is made from (`roadTileShape.ts`). | não se aplica | `native/src/render/roads` |
| `StoredTiles` | The tiles kept off the page (`derivedCache.ts`) | não se aplica | `native/src/render/roads` |
| `SurfaceReuse` | What one build of the surfaces keeps for the next, so an edit rebuilds only the tiles it reaches. | não se aplica | `native/src/render/roads` |
| `SurfaceSpec` | One surface of a structural level: how it is meshed, and from what. | não se aplica | `native/src/render/roads` |
| `TINT_REFERENCE` | The tint the asphalt material is authored against, so 1.0 means "as baked". | não se aplica | `native/src/render/roads` |
| `TileBundle` | Every surface of one tile, by mesh name. | não se aplica | `native/src/render/roads` |
| `addBlock` | A block into the group with its far level, which waits hidden beside it (`mesh/roadLod.ts`). | não se aplica | `native/src/render/roads` |
| `againstReference` | A colour against the reference, channel by channel: the multiplier a vertex is tinted with. | não se aplica | `native/src/render/roads` |
| `buildRoadSurfaces` | — | não se aplica | `native/src/render/roads` |
| `disposeSurfaceReuse` | Frees what a `SurfaceReuse` holds on the GPU. | não se aplica | `native/src/render/roads` |
| `drawGroups` | DO NOT draw a mesh a surface where surfaces are drawn alike (the footway and its ramps, a kerb and a median's kerb: one material each): a draw call a  | não se aplica | `native/src/render/roads` |
| `dress` | A surface as the page draws it: what it reads (`passSurfaces`) with its material and shadows (`surfaceLook`). | não se aplica | `native/src/render/roads` |
| `dropSurfaceBlocks` | — | não se aplica | `native/src/render/roads` |
| `passLook` | The page's constants the roads are drawn with (`PassLook`): what a road bake in a worker is handed to draw them alike (`nearRoadTiles.ts`). | não se aplica | `native/src/render/roads` |
| `ringsReader` | A pass's rings by tile key (`surfaceSource`, the build), made here: DO NOT make it in the build's pass loop, whose closures share one context (V8): ke | não se aplica | `native/src/render/roads` |
| `roadSurfaceSteps` | — | não se aplica | `native/src/render/roads` |
| `roadSurfaceSteps.adopt` | Puts the blocks kept from the build before into `group`, at the swap. | não se aplica | `native/src/render/roads` |
| `roadSurfaceSteps.ask` | Asks the tiler for `SHAPE_BATCH` tiles from `from` on; the answer lands in the record it returns. | não se aplica | `native/src/render/roads` |
| `roadSurfaceSteps.askNext` | — | não se aplica | `native/src/render/roads` |
| `roadSurfaceSteps.dispose` | — | não se aplica | `native/src/render/roads` |
| `roadSurfaceSteps.paintFor` | — | não se aplica | `native/src/render/roads` |
| `roadSurfaceSteps.reading` | — | não se aplica | `native/src/render/roads` |
| `roadSurfaceSteps.retire` | — | não se aplica | `native/src/render/roads` |
| `sameTiles` | Whether a block's tiles are the ones its mesh was merged from, by digest. | não se aplica | `native/src/render/roads` |
| `storedKey` | The name a tile is kept under between sessions: its pass, its place and its digest (a new digest at the same place replaces the old). | não se aplica | `native/src/render/roads` |
| `surfaceLook` | How a surface is drawn: its material, whether it casts a shadow, its order among the roads' meshes (the paint over the deck) - the region's (`dress`)  | não se aplica | `native/src/render/roads` |
| `surfaceSource` | One surface's tiles in a block, built again from their rings (a lost WebGL context, `mesh/gpuOnly.ts`), read again off the inputs then (`ringsOf`): DO | não se aplica | `native/src/render/roads` |
| `tileSignature` | A block's tiles by digest, surface by surface of its group (a surface after the first marked by minus its place: digests are never negative). | não se aplica | `native/src/render/roads` |

## `src/render/roadTile.worker.ts` (parte, 59 linhas) — **não se aplica**

Dono: `native/src/render/roads`. Infraestrutura de render do 4.0 (ladrilhos, workers, WebGPU, residência). O jogo desenha por chunk com MeshBuilder e JobPool: porta-se o que se vê, não a máquina. Classificado por pasta: não li cada declaração.


## `src/render/roadTileMesh.ts` (inteiro, 173 linhas) — **não se aplica**

Dono: `native/src/render/roads`. Infraestrutura de render do 4.0 (ladrilhos, workers, WebGPU, residência). O jogo desenha por chunk com MeshBuilder e JobPool: porta-se o que se vê, não a máquina. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `MeshAsk` | What a share of a pass's tiles is made from. | não se aplica | `native/src/render/roads` |
| `SurfaceData` | Everything a share of a pass's tiles reads besides the ground: the roads near them, as typed arrays and small records. | não se aplica | `native/src/render/roads` |
| `TileMesher` | One share of a pass's tiles, made whole: `need` the ground's heights at the points it reads, `give` them, `run` | não se aplica | `native/src/render/roads` |
| `TileMesher.constructor` | — | não se aplica | `native/src/render/roads` |
| `TileMesher.give` | The ground's heights at `points` (x, y each), as the page reads it. | não se aplica | `native/src/render/roads` |
| `TileMesher.heights` | — | não se aplica | `native/src/render/roads` |
| `TileMesher.made` | — | não se aplica | `native/src/render/roads` |
| `TileMesher.missing` | — | não se aplica | `native/src/render/roads` |
| `TileMesher.need` | The points of the surfaces that read the ground at every vertex (the verge on the ground), each once: x, y. | não se aplica | `native/src/render/roads` |
| `TileMesher.need.take` | — | não se aplica | `native/src/render/roads` |
| `TileMesher.ran` | — | não se aplica | `native/src/render/roads` |
| `TileMesher.reads` | — | não se aplica | `native/src/render/roads` |
| `TileMesher.renew` | The reads made afresh: a field that read NaN for a point missing remembers it (`roadField.ts` `last`). | não se aplica | `native/src/render/roads` |
| `TileMesher.run` | Meshes every tile not made yet: the tiles, or the points still missing (`give` them, then `run` again). | não se aplica | `native/src/render/roads` |
| `TileMesher.shapes` | — | não se aplica | `native/src/render/roads` |
| `TileMesher.specs` | The surfaces and the points they found missing; made again after a round that missed some (its field remembered NaN). | não se aplica | `native/src/render/roads` |
| `TileMeshes` | A tile's surfaces, by name, in the pass's order, as handed back. | não se aplica | `native/src/render/roads` |
| `readsOf` | The reads of `SurfaceData`, the ground from `heights` (a point missing pushed onto `missing`, read NaN). | não se aplica | `native/src/render/roads` |
| `readsOf.footwayEdges` | — | não se aplica | `native/src/render/roads` |
| `readsOf.lineOf` | — | não se aplica | `native/src/render/roads` |
| `readsOf.look` | — | não se aplica | `native/src/render/roads` |
| `readsOf.terrainAt` | — | não se aplica | `native/src/render/roads` |
| `readsOf.walkingRise` | — | não se aplica | `native/src/render/roads` |
| `tileBuffers` | Every buffer of the tiles' arrays, to hand back (MDN, "Transferable objects"). | não se aplica | `native/src/render/roads` |

## `src/render/roadTilePool.ts` (inteiro, 208 linhas) — **não se aplica**

Dono: `native/src/render/roads`. Infraestrutura de render do 4.0 (ladrilhos, workers, WebGPU, residência). O jogo desenha por chunk com MeshBuilder e JobPool: porta-se o que se vê, não a máquina. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `Lane` | DO NOT take a silent worker for a dead one: it shares the job pool's workers and the CPU share (a failure comes as an error). | não se aplica | `native/src/render/roads` |
| `MOST_LANES` | Most workers the pool opens: on the desktop, workers of the job processes (`jobPool.ts` PROCESS_LANES). | não se aplica | `native/src/render/roads` |
| `RoadTilePool` | — | não se aplica | `native/src/render/roads` |
| `RoadTilePool.broken` | — | não se aplica | `native/src/render/roads` |
| `RoadTilePool.check` | Called while waiting: reports a worker with tiles waiting silent too long (queued behind other work or without CPU, still waited for). | não se aplica | `native/src/render/roads` |
| `RoadTilePool.coarseLanes` | The far levels' lanes (`coarsen`), on the page's own workers. DO NOT make them on the tiles' (`jobPool.ts` PROCESS_LANES, the desktop's job processes) | não se aplica | `native/src/render/roads` |
| `RoadTilePool.coarsen` | A block's far level (`mesh/roadLodPure.ts`) made off the page: `arrays` are handed over | não se aplica | `native/src/render/roads` |
| `RoadTilePool.constructor` | — | não se aplica | `native/src/render/roads` |
| `RoadTilePool.dispose` | Ends the workers, every job still waiting rejected. | não se aplica | `native/src/render/roads` |
| `RoadTilePool.end` | — | não se aplica | `native/src/render/roads` |
| `RoadTilePool.fail` | The pool fails: every job waiting on any worker rejected (built on the page), every worker ended. | não se aplica | `native/src/render/roads` |
| `RoadTilePool.hidden` | — | não se aplica | `native/src/render/roads` |
| `RoadTilePool.lanes` | — | não se aplica | `native/src/render/roads` |
| `RoadTilePool.nextId` | — | não se aplica | `native/src/render/roads` |
| `RoadTilePool.nextLane` | — | não se aplica | `native/src/render/roads` |
| `RoadTilePool.open` | — | não se aplica | `native/src/render/roads` |
| `RoadTilePool.open.live` | — | não se aplica | `native/src/render/roads` |
| `RoadTilePool.start` | — | não se aplica | `native/src/render/roads` |
| `RoadTilePool.startCoarse` | — | não se aplica | `native/src/render/roads` |
| `RoadTilePool.tiles` | — | não se aplica | `native/src/render/roads` |
| `RoadTilePool.unseen` | No frame is seen (the opening's curtain, `renderer.ts`): the far levels go to the tiles' lanes, the page's copies paid behind it. DO NOT put them on t | não se aplica | `native/src/render/roads` |
| `RoadTilePool.unseen@60` | — | não se aplica | `native/src/render/roads` |
| `RoadTiler` | What makes a road surface's tiles off the page (`roadSurfaceSteps`). | não se aplica | `native/src/render/roads` |
| `SILENCE_MS` | Silence past which a worker with tiles waiting is reported (it beats every 500 ms while it works, when it runs). | não se aplica | `native/src/render/roads` |
| `TileJob` | Tiles of a road pass to make off the page, and what the page answers for them. | não se aplica | `native/src/render/roads` |

## `src/render/roadTileShape.ts` (inteiro, 84 linhas) — **não se aplica**

Dono: `native/src/render/roads`. Infraestrutura de render do 4.0 (ladrilhos, workers, WebGPU, residência). O jogo desenha por chunk com MeshBuilder e JobPool: porta-se o que se vê, não a máquina. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `SLIVER` | A band thinner than twice this is a sliver of two outlines nearly meeting, not a surface. | não se aplica | `native/src/render/roads` |
| `TilePolys` | The rings that reach a tile, by kind, in input order. | não se aplica | `native/src/render/roads` |
| `TileSource` | What each surface of a tile is made from. | não se aplica | `native/src/render/roads` |
| `TileSpec` | One surface of a tile, as its shape is made: its name, source, finest edge and whether it hangs skirts. | não se aplica | `native/src/render/roads` |
| `tileShapes` | Each surface's shape on the tile, in the order of `specs`; a surface with nothing on the tile left out. | não se aplica | `native/src/render/roads` |
| `tileShapes.merged` | — | não se aplica | `native/src/render/roads` |

## `src/render/roadTones.ts` (inteiro, 20 linhas) — **não portado**

Dono: `native/src/render/roads`. Falta conferir um a um o que sobra do render de vias. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `ASPHALT_REFERENCE` | The colour the asphalt texture is authored against (sRGB): a class's colour (`RoadType.color`) over it is the tint its carriageway is drawn with. | não portado | `native/src/render/roads` |
| `CARRIAGEWAY_COLOUR` | A carriageway's colour by what it is paved with (sRGB); asphalt keeps its class's. | não portado | `native/src/render/roads` |
| `FOOTWAY_TINT` | A footway's tone over the baked pavers, by material, linear multipliers. | não portado | `native/src/render/roads` |

## `src/render/scenery.ts` (parte, 3 linhas) — **não portado**

Dono: `native/src/render/roads`. Falta conferir um a um o que sobra do render de vias. Classificado por pasta: não li cada declaração.


## `src/render/signals.ts` (parte, 92 linhas) — **não se aplica**

Dono: `street-furniture` (porte-6). Placas são de `street-furniture`.


## `src/render/signLayer.ts` (parte, 90 linhas) — **não se aplica**

Dono: `street-furniture` (porte-6). Placas são de `street-furniture`.


## `src/render/signs.ts` (parte, 20 linhas) — **não se aplica**

Dono: `street-furniture` (porte-6). Placas são de `street-furniture`.


## `src/render/streetFurnitureLayer.ts` (inteiro, 471 linhas) — **não se aplica**

Dono: `street-furniture` (porte-6). Também é entrada de `street-furniture`, que porta mobiliário, travessias, retornos e rede.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `DECK_REACH` | Its deck's height read at its point. | não se aplica | `street-furniture` (porte-6) |
| `FURNITURE_MODELS` | The furniture's stores, in the order the scenery's meshes were made. | não se aplica | `street-furniture` (porte-6) |
| `FURNITURE_SPREAD` | How far from its item a placed thing may stand: a lamp's pool of light out over the widest road. | não se aplica | `street-furniture` (porte-6) |
| `FurnitureModel` | — | não se aplica | `street-furniture` (porte-6) |
| `FurniturePlace` | A placed item as it stands: its instances, where it is (its filing), when it was planted. | não se aplica | `street-furniture` (porte-6) |
| `FurnitureSource` | What the layer is refreshed from: the diary it reads, and the roads and their heights it places on. | não se aplica | `street-furniture` (porte-6) |
| `LampLens` | Where a lamp's lens stands on its post (`SceneryKit.lens`). | não se aplica | `street-furniture` (porte-6) |
| `MEADOW_STORES` | — | não se aplica | `street-furniture` (porte-6) |
| `MODEL` | — | não se aplica | `street-furniture` (porte-6) |
| `MeadowPlace` | A placed item's grass: its clump's tufts and flowers as one run. | não se aplica | `street-furniture` (porte-6) |
| `PROP_FAR_PX` | Pixels across under which a small prop is drawn as its box (`farBox`). | não se aplica | `street-furniture` (porte-6) |
| `Placed` | One instance of a placed item: its store, where, how turned and sized, its tint. | não se aplica | `street-furniture` (porte-6) |
| `PlacedProduct` | Placed items of some kinds as a product of the diary, key by key (the item's id) | não se aplica | `street-furniture` (porte-6) |
| `PlacedProduct.allKeys` | @rebuild The document's items of the product's kinds: read only when everything changed (a load, the diary forgotten). | não se aplica | `street-furniture` (porte-6) |
| `PlacedProduct.byId` | — | não se aplica | `street-furniture` (porte-6) |
| `PlacedProduct.changed` | — | não se aplica | `street-furniture` (porte-6) |
| `PlacedProduct.compute` | — | não se aplica | `street-furniture` (porte-6) |
| `PlacedProduct.constructor` | — | não se aplica | `street-furniture` (porte-6) |
| `PlacedProduct.dirty` | — | não se aplica | `street-furniture` (porte-6) |
| `PlacedProduct.filing` | The items holding a value, by place: the ones a change reaches, gone from the document or not. | não se aplica | `street-furniture` (porte-6) |
| `PlacedProduct.kinds` | — | não se aplica | `street-furniture` (porte-6) |
| `ROAD_REACH` | How far a placed item stands from what moves it: a road within reach of its footway and of the crossings near it. | não se aplica | `street-furniture` (porte-6) |
| `STREET_TREE_MIN` | Street trees are pollarded smaller than a tree in open ground. | não se aplica | `street-furniture` (porte-6) |
| `STREET_TREE_RANGE` | — | não se aplica | `street-furniture` (porte-6) |
| `StreetFurnitureLayer` | The street furniture held round the view (`propCells.ts`), the long grass kept item by item. | não se aplica | `street-furniture` (porte-6) |
| `bushStore` | — | não se aplica | `street-furniture` (porte-6) |
| `createStreetFurniture` | — | não se aplica | `street-furniture` (porte-6) |
| `createStreetFurniture.cull` | — | não se aplica | `street-furniture` (porte-6) |
| `createStreetFurniture.dispose` | — | não se aplica | `street-furniture` (porte-6) |
| `createStreetFurniture.setMap` | — | não se aplica | `street-furniture` (porte-6) |
| `createStreetFurniture.sync` | Brings the furniture and the long grass up to the diary (and the hour): the furniture's cells an entry reached dropped, placed again as the view asks; | não se aplica | `street-furniture` (porte-6) |
| `createStreetFurniture.warm` | A mesh of each of its stores, as it draws them | não se aplica | `street-furniture` (porte-6) |
| `farBox` | A prop seen a few pixels across: the box round it, every attribute of the model at its mean (its tone, its finish), 12 triangles. | não se aplica | `street-furniture` (porte-6) |
| `furnitureAbout` | The furniture's things standing about a rectangle at the city's time `now` (the hour's start): each item near it placed (`furniturePlace`), a plant si | não se aplica | `street-furniture` (porte-6) |
| `furniturePlace` | Where a placed item's instances stand, or undefined when it draws nothing. | não se aplica | `street-furniture` (porte-6) |
| `furniturePlace.put` | — | não se aplica | `street-furniture` (porte-6) |
| `furnitureProduct` | The furniture's product: where each placed item stands (a product of the diary), proved against one made from nothing. | não se aplica | `street-furniture` (porte-6) |
| `furnitureStores` | The meshes of the furniture and the street plants: the stores, and where each plant's body and cards are. | não se aplica | `street-furniture` (porte-6) |
| `furnitureStores.plant` | A street plant: its body and its leaf cards near, its picture far (`plantImpostors.ts`), chosen by its size on the screen. | não se aplica | `street-furniture` (porte-6) |
| `furnitureStores.prop` | `far`: the prop's own far model in place of its box (a lamp, whose box would be a block nine metres tall). | não se aplica | `street-furniture` (porte-6) |
| `isFurniture` | The items the furniture draws (not the long grass, the signs, the street names). | não se aplica | `street-furniture` (porte-6) |
| `meadowInto` | — | não se aplica | `street-furniture` (porte-6) |
| `meadowInto.add` | — | não se aplica | `street-furniture` (porte-6) |
| `meadowProduct` | The long grass's product: each placed clump's run, laid on the ground as drawn. | não se aplica | `street-furniture` (porte-6) |
| `placeInto` | A placed item's instances at the city's time (the hour's start), put in a run of the furniture's stores: a plant sized by its growth then. | não se aplica | `street-furniture` (porte-6) |
| `radiusOf` | A model's bounding sphere's radius (computed once here when its maker did not). | não se aplica | `street-furniture` (porte-6) |
| `sameColour` | — | não se aplica | `street-furniture` (porte-6) |
| `samePlace` | — | não se aplica | `street-furniture` (porte-6) |
| `stillGrowing` | Whether a plant planted then is still growing at `minutes` (unknown while the clock is). | não se aplica | `street-furniture` (porte-6) |
| `streetSpecies` | Picks a species for a street tree: mostly planes, some flowering ipês. | não se aplica | `street-furniture` (porte-6) |
| `treeStore` | Each street plant's store (`furnitureStores`): the props', then the trees' by species, then the bushes'. | não se aplica | `street-furniture` (porte-6) |

## `src/render/structureCells.ts` (parte, 28 linhas) — **não se aplica**

Dono: funcionalidade `road-elevation`. Pilares, muros e portais.


## `src/render/structures.ts` (parte, 291 linhas) — **não se aplica**

Dono: funcionalidade `road-elevation`. Pilares, muros e portais.


## `src/render/surfaceBakeClient.ts` (inteiro, 122 linhas) — **não se aplica**

Dono: `native/src/render/roads`. Infraestrutura de render do 4.0 (ladrilhos, workers, WebGPU, residência). O jogo desenha por chunk com MeshBuilder e JobPool: porta-se o que se vê, não a máquina. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `BAKE_HASH` | The fingerprint of the recipes' code (`surface-bake-plugin.ts`): a new bake is a new URL, never an old file from the cache | não se aplica | `native/src/render/roads` |
| `FOLDER` | — | não se aplica | `native/src/render/roads` |
| `Manifest` | — | não se aplica | `native/src/render/roads` |
| `ManifestEntry` | — | não se aplica | `native/src/render/roads` |
| `SurfaceBakeResult` | — | não se aplica | `native/src/render/roads` |
| `TRANSCODER` | Basis Universal's transcoder from three's addons, served by the plugin at a fixed path (Vite's prebundled three cannot find it beside the loader). | não se aplica | `native/src/render/roads` |
| `__SURFACE_BAKE_HASH__` | — | não se aplica | `native/src/render/roads` |
| `primeSurfaceBake` | Install the textures before the first material is made; a texture whose texels were let go reads its file again when it must be uploaded anew. | não se aplica | `native/src/render/roads` |
| `rawLevels` | Each level's texels of an uncompressed file (`rgba8`), Zstandard undone. | não se aplica | `native/src/render/roads` |
| `readFiles` | Reads the named files; UASTC through three's KTX2Loader, whose transcoding workers are let go once they are done. | não se aplica | `native/src/render/roads` |
| `readFiles.zstd` | — | não se aplica | `native/src/render/roads` |
| `rgbaTexture` | An RGBA8 file as a texture of its own mip levels: three uploads an RGBA "compressed" texture level by level (WebGLTextures, `texSubImage2D`/`3D`), whi | não se aplica | `native/src/render/roads` |
| `startSurfaceBake` | Reads every baked surface while the main thread builds the town. | não se aplica | `native/src/render/roads` |
| `transcodeTargets` | What the GPU can take a UASTC file as (BC7, ASTC, ETC2...), asked of a throwaway WebGL2 context: the game's renderer is made after the textures are re | não se aplica | `native/src/render/roads` |

## `src/render/surfaceTextures.ts` (inteiro, 105 linhas) — **não se aplica**

Dono: `native/src/render/roads`. Infraestrutura de render do 4.0 (ladrilhos, workers, WebGPU, residência). O jogo desenha por chunk com MeshBuilder e JobPool: porta-se o que se vê, não a máquina. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `SurfaceBake` | — | não se aplica | `native/src/render/roads` |
| `SurfaceTextureSet` | The page's one set of baked textures: what it read, and where to read let-go texels again. | não se aplica | `native/src/render/roads` |
| `SurfaceTextureSet.byName` | — | não se aplica | `native/src/render/roads` |
| `SurfaceTextureSet.letTexelsGo` | DO NOT keep a texture's texels once uploaded: some 250 MB of the page's heap were copies of what the GPU holds. | não se aplica | `native/src/render/roads` |
| `SurfaceTextureSet.source` | The build's files, fetched anew (`surfaceBakeClient.ts`); null keeps the texels. | não se aplica | `native/src/render/roads` |
| `SurfaceTextureSet.want` | These textures' texels wanted again (to be uploaded anew); once until they come. | não se aplica | `native/src/render/roads` |
| `SurfaceTextureSet.wanted` | Textures whose texels were let go and are wanted again, until they come. | não se aplica | `native/src/render/roads` |
| `disposeSurfaceTextures` | — | não se aplica | `native/src/render/roads` |
| `hasTexels` | Every mip level's texels, or none once let go. | não se aplica | `native/src/render/roads` |
| `primeSurfaceTextures` | The textures read from the build's files (`surfaceBakeClient.ts`), and where to read them again. | não se aplica | `native/src/render/roads` |
| `refillSurfaceTexels` | Texels read again: each let-go texture given its own and uploaded anew (it lets them go again after). | não se aplica | `native/src/render/roads` |
| `requireSurfaceMaps` | A surface's three maps, which its material's shader needs. | não se aplica | `native/src/render/roads` |
| `surfaceMaps` | A surface's colour, normal and roughness maps; undefined where none were read (a worker, a test). | não se aplica | `native/src/render/roads` |
| `surfaceTexture` | A baked texture by name (`bake/catalogue.ts`); the shaders need it, so a missing one is an error. | não se aplica | `native/src/render/roads` |
| `surfaceTextureOrNone` | A baked texture by name, its anisotropy at least `anisotropy`; undefined where none was read (a worker, a test). | não se aplica | `native/src/render/roads` |
| `surfaces` | — | não se aplica | `native/src/render/roads` |
| `wantAllTexels` | Every baked texture let go, its texels needed again (the WebGL context restored). | não se aplica | `native/src/render/roads` |

## `src/render/terrain.ts` (parte, 17 linhas) — **não portado**

Dono: `native/src/render/roads`. Falta conferir um a um o que sobra do render de vias. Classificado por pasta: não li cada declaração.


## `src/render/utilities.ts` (parte, 11 linhas) — **não portado**

Dono: `native/src/render/roads`. Falta conferir um a um o que sobra do render de vias. Classificado por pasta: não li cada declaração.


## `src/sim/agents/bay.ts` (parte, 3 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.


## `src/sim/agents/kerbBays.ts` (parte, 88 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.


## `src/sim/agents/laneGrid.ts` (parte, 95 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.


## `src/sim/agents/lorryDocks.ts` (parte, 31 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.


## `src/sim/agents/lorryTraffic.ts` (parte, 47 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.


## `src/sim/agents/lotBays.ts` (parte, 100 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.


## `src/sim/agents/lotTraffic.ts` (parte, 12 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.


## `src/sim/agents/parking.ts` (parte, 16 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.


## `src/sim/agents/walk.ts` (parte, 32 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.


## `src/sim/ambient/ambient.ts` (parte, 71 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.


## `src/sim/ambient/ambientIndex.ts` (parte, 30 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.


## `src/sim/crossings/permission.ts` (parte, 54 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.


## `src/sim/drive/tactical.ts` (parte, 84 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.


## `src/sim/host/protocol.ts` (parte, 19 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.


## `src/sim/host/simAnswers.ts` (parte, 49 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.


## `src/sim/host/simClient.ts` (parte, 10 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `SimClient.ask` | — | não se aplica | `native/src/sim/traffic` (funcionalidade `traffic`, porte-5) |

## `src/sim/host/simHost.ts` (parte, 50 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.


## `src/sim/intersections/admission.ts` (parte, 524 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.


## `src/sim/intersections/claims.ts` (parte, 63 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.


## `src/sim/intersections/crossingSpans.ts` (parte, 83 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.


## `src/sim/intersections/cycles.ts` (parte, 163 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.


## `src/sim/intersections/spillback.ts` (parte, 41 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.


## `src/sim/invariants.ts` (parte, 103 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.


## `src/sim/meso/meso.ts` (parte, 19 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.


## `src/sim/peds/sidewalk.ts` (parte, 93 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.


## `src/sim/peds/signalPostIndex.ts` (parte, 11 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.


## `src/sim/people/engine.ts` (parte, 4 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.


## `src/sim/pipeline.ts` (parte, 161 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.


## `src/sim/pose.ts` (parte, 16 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.


## `src/sim/roads/controlAdvisor.ts` (parte, 14 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.


## `src/sim/roads/flowStats.ts` (parte, 15 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.


## `src/sim/roads/greenWave.ts` (parte, 60 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.


## `src/sim/routing/destination.ts` (parte, 46 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.


## `src/sim/routing/laneReach.ts` (parte, 219 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.


## `src/sim/routing/lowerBound.ts` (parte, 14 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.


## `src/sim/routing/routeIndex.ts` (parte, 20 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.


## `src/sim/routing/router.ts` (parte, 224 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.


## `src/sim/routing/searchScratch.ts` (parte, 12 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.


## `src/sim/signals/fsm.ts` (parte, 119 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.


## `src/sim/signals/plan.ts` (parte, 217 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.


## `src/sim/topologyCatchUp.ts` (parte, 18 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.


## `src/sim/transit/transit.ts` (parte, 34 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.


## `src/sim/vehicles/curvature.ts` (parte, 56 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.


## `src/sim/vehicles/cycleLane.ts` (parte, 15 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.


## `src/sim/vehicles/integrate.ts` (parte, 150 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.


## `src/sim/vehicles/kerbStops.ts` (parte, 100 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.


## `src/sim/vehicles/laneChange.ts` (parte, 316 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.


## `src/sim/vehicles/laneClosures.ts` (parte, 42 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.


## `src/sim/vehicles/leaderIndex.ts` (parte, 86 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.


## `src/sim/vehicles/obstacles.ts` (parte, 124 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.


## `src/sim/vehicles/spawn.ts` (parte, 104 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.


## `src/sim/vehicles/state.ts` (parte, 30 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.


## `src/sim/vehicleSignals.ts` (parte, 35 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.


## `src/sim/world.ts` (parte, 418 linhas) — **não se aplica**

Dono: `native/src/sim/traffic` (funcionalidade `traffic`, porte-5). Fatia da simulação de veículos e pedestres que lê a via; a sessão `traffic` a porta. A via entrega as faixas pelo grafo já existente.


## `src/ui/inspector.ts` (parte, 155 linhas) — **não se aplica**

Dono: `interface` (porte-4). Inspetor é de `interface`.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `junctionHost.setControl` | — | não se aplica | `interface` (porte-4) |
| `junctionHost.setHeight` | — | não se aplica | `interface` (porte-4) |
| `junctionHost.setMovementBlocked` | — | não se aplica | `interface` (porte-4) |
| `junctionHost.setRules` | — | não se aplica | `interface` (porte-4) |
| `junctionHost.setSignal` | — | não se aplica | `interface` (porte-4) |
| `junctionHost.setSignals` | — | não se aplica | `interface` (porte-4) |

## `src/ui/labels.ts` (parte, 8 linhas) — **não portado**

Dono: `project/ui`. Figura do perfil e editor de seção do inspetor. Classificado por pasta: não li cada declaração.


## `src/ui/overlay/congestion.ts` (parte, 44 linhas) — **não se aplica**

Dono: `interface` (porte-4). Camada de congestionamento já desenhada por `overlays`.


## `src/ui/roads/catalogPanel.ts` (inteiro, 187 linhas) — **em parte**

Dono: `project/ui/road_catalog.gd`. Abas e cartões. Personalizar e Meus modelos não

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `BuiltStrip` | The strip as last built and what it was built from: the same inputs keep it, only the picked card changes. | em parte | `project/ui/road_catalog.gd` |
| `CatalogHost` | THE ROAD CATALOGUE in the road tool's drawer (the player's order of 2026-10-09) | em parte | `project/ui/road_catalog.gd` |
| `allCatalogRoads` | Every road the catalogue offers, the player's own under "mine". | em parte | `project/ui/road_catalog.gd` |
| `catalogStrip` | — | em parte | `project/ui/road_catalog.gd` |
| `customButton` | — | em parte | `project/ui/road_catalog.gd` |
| `markPicked` | — | em parte | `project/ui/road_catalog.gd` |
| `nameOf` | — | em parte | `project/ui/road_catalog.gd` |
| `pickRoad` | Picks a road: the class first (colours, rules), then its profile, unless it is the class's own. | em parte | `project/ui/road_catalog.gd` |
| `pickedRoad` | The road the tool draws with now: the picked catalogue road, or the class in hand's own. | em parte | `project/ui/road_catalog.gd` |
| `sameRoad` | — | em parte | `project/ui/road_catalog.gd` |
| `strip` | The tab shown and the strip last built. | em parte | `project/ui/road_catalog.gd` |

## `src/ui/roads/connectorPanel.ts` (inteiro, 177 linhas) — **não portado**

Dono: `project/ui`. Editor de perfil, corte da seção, painel de cruzamento, teclas e conectores não foram portados. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `ConnectorPanelHost` | THE LANE CONNECTOR EDITOR of a junction, in the inspector | não portado | `project/ui` |
| `Picked` | — | não portado | `project/ui` |
| `draw` | — | não portado | `project/ui` |
| `draw.at` | — | não portado | `project/ui` |
| `draw.keyOf` | — | não portado | `project/ui` |
| `draw.press` | — | não portado | `project/ui` |
| `draw.view` | — | não portado | `project/ui` |
| `mountConnectorPanel` | — | não portado | `project/ui` |
| `panel` | The panel open now, the lane picked, and the lane end the keyboard is on (kept past a rebuild of the inspector: its panel made again, the old one's fo | não portado | `project/ui` |
| `refreshConnectorPanel` | Redraws the open panel when the lane graph or the node's links changed. | não portado | `project/ui` |
| `turnOf` | How the camera turns the junction: the screen direction of the world's x and y axes at its first lane end. | não portado | `project/ui` |
| `turnOf.dir` | — | não portado | `project/ui` |

## `src/ui/roads/crossSection.ts` (inteiro, 318 linhas) — **não portado**

Dono: `project/ui`. Editor de perfil, corte da seção, painel de cruzamento, teclas e conectores não foram portados. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `COLOUR` | — | não portado | `project/ui` |
| `ICON_Y` | The row of signs (a lane's arrow, the P, the bicycle), clear of the end labels. | não portado | `project/ui` |
| `SectionDrawing` | A road's CROSS-SECTION drawn as a street section, the way Streetmix illustrates a street | não portado | `project/ui` |
| `SectionOptions` | — | não portado | `project/ui` |
| `arrow` | The direction a lane runs, as the arrow painted in it (towards B: right). | não portado | `project/ui` |
| `bicycleGlyph` | A bicycle pictogram, 14 pixels wide, centred on (cx, cy). | não portado | `project/ui` |
| `busRear` | A bus seen from behind (or the front), `w` pixels wide, standing on `base`: what a bus lane carries. | não portado | `project/ui` |
| `carRear` | A car seen from behind (or from the front: headlights and a grille), `w` pixels wide, standing on `base`. | não portado | `project/ui` |
| `carRear.v` | — | não portado | `project/ui` |
| `carriageway` | — | não portado | `project/ui` |
| `cyclist` | A cyclist from behind, `k` pixels tall, on `base`. | não portado | `project/ui` |
| `decoration` | What stands on each element, to the road's metre scale: people, cars, a tree, a cyclist, a lane's arrow, paving. | não portado | `project/ui` |
| `decoration.car` | — | não portado | `project/ui` |
| `drawSection` | — | não portado | `project/ui` |
| `elementColour` | The colour an element is drawn in (the swatch beside its name in the editor). | não portado | `project/ui` |
| `esc` | — | não portado | `project/ui` |
| `laneLine` | The line painted between two neighbours: between directions, between lanes of one, at the carriageway's edge. | não portado | `project/ui` |
| `laneLine.road` | — | não portado | `project/ui` |
| `metresText` | A width in metres as the panels write it: whole metres plain, else one decimal in the interface language's notation. | não portado | `project/ui` |
| `person` | Somebody walking, `k` pixels tall, standing on `top`. | não portado | `project/ui` |
| `r1` | — | não portado | `project/ui` |
| `raised` | — | não portado | `project/ui` |
| `surface` | — | não portado | `project/ui` |
| `tree` | A street tree: crown radius `r`, `h` pixels tall, on `top`. | não portado | `project/ui` |

## `src/ui/roads/cutWalls.ts` (inteiro, 18 linhas) — **não portado**

Dono: `project/ui`. Editor de perfil, corte da seção, painel de cruzamento, teclas e conectores não foram portados. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `STORAGE_KEY` | Whether the road tool lays new roads with retaining walls in their cuttings | não portado | `project/ui` |
| `chosen` | — | não portado | `project/ui` |
| `cutWallsChosen` | — | não portado | `project/ui` |
| `read` | — | não portado | `project/ui` |
| `setCutWalls` | — | não portado | `project/ui` |

## `src/ui/roads/drawProfile.ts` (inteiro, 19 linhas) — **não portado**

Dono: `project/ui`. Editor de perfil, corte da seção, painel de cruzamento, teclas e conectores não foram portados. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `DrawProfile` | The profile the road tool lays new roads with, chosen in the profile editor | não portado | `project/ui` |
| `chosen` | — | não portado | `project/ui` |
| `drawProfile` | — | não portado | `project/ui` |
| `setDrawProfile` | — | não portado | `project/ui` |

## `src/ui/roads/furnitureChoice.ts` (inteiro, 23 linhas) — **não portado**

Dono: `project/ui`. Editor de perfil, corte da seção, painel de cruzamento, teclas e conectores não foram portados. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `STORAGE_KEY` | The furniture the road tool lays new roads with (`world/roads/furnitureSets.ts`) | não portado | `project/ui` |
| `chosen` | — | não portado | `project/ui` |
| `furnitureChosen` | — | não portado | `project/ui` |
| `read` | — | não portado | `project/ui` |
| `setFurnitureChosen` | — | não portado | `project/ui` |

## `src/ui/roads/junctionPanel.ts` (inteiro, 326 linhas) — **não portado**

Dono: `project/ui`. Editor de perfil, corte da seção, painel de cruzamento, teclas e conectores não foram portados. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `CONTROLS` | — | não portado | `project/ui` |
| `JunctionPanelHost` | THE JUNCTION PANEL, in the inspector: the junction's height, its control as buttons | não portado | `project/ui` |
| `RULES` | — | não portado | `project/ui` |
| `button` | — | não portado | `project/ui` |
| `compassOf` | — | não portado | `project/ui` |
| `div` | — | não portado | `project/ui` |
| `flowOf` | A leg's flow, vehicles an hour, from the junction's answer (`SimClient.junction`); 0 before the first. | não portado | `project/ui` |
| `flowsShown` | — | não portado | `project/ui` |
| `fmt` | — | não portado | `project/ui` |
| `head` | — | não portado | `project/ui` |
| `mountJunctionPanel` | — | não portado | `project/ui` |
| `mountJunctionPanel.set` | — | não portado | `project/ui` |
| `moveChip` | — | não portado | `project/ui` |
| `refreshJunctionFlows` | Rewrites the flows in place (the inspector's refresh), without rebuilding the panel. | não portado | `project/ui` |
| `row` | — | não portado | `project/ui` |
| `rowOf` | — | não portado | `project/ui` |
| `screenOf` | The way a leg points from its junction AS THE PLAYER SEES IT | não portado | `project/ui` |
| `segmented` | — | não portado | `project/ui` |
| `stepper` | — | não portado | `project/ui` |
| `toggle` | — | não portado | `project/ui` |
| `withoutFixed` | — | não portado | `project/ui` |

## `src/ui/roads/keys.ts` (inteiro, 111 linhas) — **não portado**

Dono: `project/ui`. Editor de perfil, corte da seção, painel de cruzamento, teclas e conectores não foram portados. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `CODES_OF_KEY` | The `KeyboardEvent.code`s that type a key on a US keyboard: the camera's keys are codes | não portado | `project/ui` |
| `DEFAULTS` | — | não portado | `project/ui` |
| `RESERVED` | Keys the game keeps for itself: not given to the road tool. | não portado | `project/ui` |
| `ROAD_KEY_ACTIONS` | — | não portado | `project/ui` |
| `RoadKeyAction` | THE ROAD TOOL'S KEYS, rebindable (read by `main.ts`) | não portado | `project/ui` |
| `STORAGE_KEY` | — | não portado | `project/ui` |
| `bindableKey` | Whether `key` may be given to a road action. | não portado | `project/ui` |
| `bindings` | — | não portado | `project/ui` |
| `cameraKey` | DO NOT list the camera's keys here by hand. | não portado | `project/ui` |
| `codesOfKey` | — | não portado | `project/ui` |
| `keyLabel` | A key as the interface writes it: "Page Up", "V", "F2". | não portado | `project/ui` |
| `lentByTheCamera` | Page Up and Page Down tilt the camera, and are the road's own raise and lower by default | não portado | `project/ui` |
| `listeners` | — | não portado | `project/ui` |
| `load` | — | não portado | `project/ui` |
| `normaliseKey` | A key as stored: letters lower case, everything else as `KeyboardEvent.key` names it. | não portado | `project/ui` |
| `onRoadKeysChange` | — | não portado | `project/ui` |
| `paramsOf` | — | não portado | `project/ui` |
| `reserved` | — | não portado | `project/ui` |
| `resetRoadKeys` | — | não portado | `project/ui` |
| `roadKey` | — | não portado | `project/ui` |
| `roadKeyAction` | The road action a key press asks for, or null. | não portado | `project/ui` |
| `roadKeyParams` | The keys as placeholders for the interface's sentences (`{heightUp}`, `{heightDown}`, `{alignmentKey}`). | não portado | `project/ui` |
| `save` | — | não portado | `project/ui` |
| `setRoadKey` | Gives `key` to `action`; the action that had it takes `action`'s old key. False for a reserved key. | não portado | `project/ui` |

## `src/ui/roads/keysPanel.ts` (inteiro, 100 linhas) — **não portado**

Dono: `project/ui`. Editor de perfil, corte da seção, painel de cruzamento, teclas e conectores não foram portados. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `closeRoadKeys` | — | não portado | `project/ui` |
| `open` | The open panel, and what lets go of its key listener when it closes (MDN, addEventListener `signal`). | não portado | `project/ui` |
| `openRoadKeys` | — | não portado | `project/ui` |
| `openRoadKeys.onKey` | — | não portado | `project/ui` |
| `openRoadKeys.render` | — | não portado | `project/ui` |
| `roadKeysOpen` | — | não portado | `project/ui` |

## `src/ui/roads/money.ts` (inteiro, 19 linhas) — **não portado**

Dono: `project/ui`. Editor de perfil, corte da seção, painel de cruzamento, teclas e conectores não foram portados. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `balanceTip` | The balance's tooltip, with the demolition share the game uses. | não portado | `project/ui` |
| `formatCost` | What a road in hand costs, for the preview: its price, or what a demolition-like edit gives back. | não portado | `project/ui` |
| `formatMoney` | Money as the player reads it (`world/economy.ts`) | não portado | `project/ui` |

## `src/ui/roads/profileEditor.ts` (inteiro, 755 linhas) — **não portado**

Dono: `project/ui`. Editor de perfil não existe no jogo.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `EdgeDrag` | — | não portado | `project/ui` |
| `ICON` | — | não portado | `project/ui` |
| `MoveDrag` | — | não portado | `project/ui` |
| `PALETTE` | — | não portado | `project/ui` |
| `PaletteKind` | — | não portado | `project/ui` |
| `ProfileEditorOptions` | THE PROFILE EDITOR: the road's cross-section drawn as a street section | não portado | `project/ui` |
| `WIDTH_LIMITS` | Limits of each element's width, whole metres; null for a fixed width. | não portado | `project/ui` |
| `button` | — | não portado | `project/ui` |
| `closeProfileEditor` | Closes the editor if it is open. | não portado | `project/ui` |
| `div` | — | não portado | `project/ui` |
| `icon` | — | não portado | `project/ui` |
| `iconButton` | — | não portado | `project/ui` |
| `insertionIndex` | Where the palette puts a new element: where the road can carry it | não portado | `project/ui` |
| `insertionIndex.clamp` | — | não portado | `project/ui` |
| `insertionIndex.firstIndex` | — | não portado | `project/ui` |
| `insertionIndex.kerbBand` | — | não portado | `project/ui` |
| `insertionIndex.lastIndex` | — | não portado | `project/ui` |
| `moved` | The elements with the one at `from` moved to `to`. | não portado | `project/ui` |
| `newElement` | A new element of each kind, as the palette adds it; a lane as wide as the road's others. | não portado | `project/ui` |
| `openEditor` | — | não portado | `project/ui` |
| `openProfileEditor` | — | não portado | `project/ui` |
| `openProfileEditor.drawStage` | — | não portado | `project/ui` |
| `openProfileEditor.endDrag` | — | não portado | `project/ui` |
| `openProfileEditor.issueLine` | — | não portado | `project/ui` |
| `openProfileEditor.issues` | — | não portado | `project/ui` |
| `openProfileEditor.moveSelected` | — | não portado | `project/ui` |
| `openProfileEditor.place` | — | não portado | `project/ui` |
| `openProfileEditor.removeSelected` | — | não portado | `project/ui` |
| `openProfileEditor.renderAll` | — | não portado | `project/ui` |
| `openProfileEditor.renderCards` | — | não portado | `project/ui` |
| `openProfileEditor.renderCards.card` | — | não portado | `project/ui` |
| `openProfileEditor.renderFoot` | — | não portado | `project/ui` |
| `openProfileEditor.renderProps` | — | não portado | `project/ui` |
| `openProfileEditor.replace` | — | não portado | `project/ui` |
| `openProfileEditor.sameAs` | — | não portado | `project/ui` |
| `openProfileEditor.save` | — | não portado | `project/ui` |
| `openProfileEditor.stageSize` | — | não portado | `project/ui` |
| `openProfileEditor.svgX` | — | não portado | `project/ui` |
| `profileEditorOpen` | Whether the editor is open (the shell keeps its tool while it is). | não portado | `project/ui` |
| `row` | — | não portado | `project/ui` |
| `segmented` | — | não portado | `project/ui` |
| `stepper` | — | não portado | `project/ui` |
| `toggle` | — | não portado | `project/ui` |
| `withWidth` | The element at `index` made `metres` wide. | não portado | `project/ui` |

## `src/ui/roads/profilePanel.ts` (inteiro, 84 linhas) — **não portado**

Dono: `project/ui`. Editor de perfil, corte da seção, painel de cruzamento, teclas e conectores não foram portados. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `mountProfilePanel` | THE ROAD'S PROFILE in the inspector | não portado | `project/ui` |
| `mountProfilePanel.hold` | — | não portado | `project/ui` |

## `src/ui/roads/profileSwatch.ts` (inteiro, 137 linhas) — **não portado**

Dono: `project/ui`. Editor de perfil, corte da seção, painel de cruzamento, teclas e conectores não foram portados. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `ASPHALT` | — | não portado | `project/ui` |
| `BUS` | — | não portado | `project/ui` |
| `CARS` | — | não portado | `project/ui` |
| `CYCLE` | — | não portado | `project/ui` |
| `FOOTWAY` | — | não portado | `project/ui` |
| `GRASS` | — | não portado | `project/ui` |
| `KERB` | — | não portado | `project/ui` |
| `MEDIAN_GRASS` | — | não portado | `project/ui` |
| `WHITE` | — | não portado | `project/ui` |
| `YELLOW` | — | não portado | `project/ui` |
| `profileSwatch` | — | não portado | `project/ui` |

## `src/ui/roads/templates.ts` (inteiro, 54 linhas) — **não portado**

Dono: `project/ui`. Editor de perfil, corte da seção, painel de cruzamento, teclas e conectores não foram portados. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `deleteTemplate` | Deletes one template, the rest as they were; false when nothing could be written. | não portado | `project/ui` |
| `inThisUnit` | A stored template's widths in this build's unit; absent `unit`: 0.4 m, every template before 2026-10-10. | não portado | `project/ui` |
| `kept` | The player's own road templates, kept in this browser for every map, as Cities | não portado | `project/ui` |
| `readTemplate` | A stored entry as a template in this build's unit, or null when this build cannot read it (it stays stored). | não portado | `project/ui` |
| `readable` | An entry as read, for matching it by name or id; an entry that cannot be read matches nothing and stays. | não portado | `project/ui` |
| `saveTemplate` | Saves a template under a name, replacing one of the same name; null when it cannot be built or was not kept. | não portado | `project/ui` |
| `savedTemplates` | The same list while nothing was saved or deleted, so the catalogue can tell it is unchanged. | não portado | `project/ui` |
| `withUnit` | A template as stored: with the world unit its widths are in. | não portado | `project/ui` |

## `src/ui/roadSectionEditor.ts` (inteiro, 129 linhas) — **não portado**

Dono: `project/ui`. Figura do perfil e editor de seção do inspetor. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `freeRoadsEnabled` | Editing is opt-in; saved road dimensions remain readable without the flag. | não portado | `project/ui` |
| `mountRoadSectionEditor` | One change on release, so a drag creates one exact undo step. | não portado | `project/ui` |
| `mountRoadSectionEditor.draw` | — | não portado | `project/ui` |
| `mountRoadSectionEditor.draw.band` | — | não portado | `project/ui` |
| `turnsExpanded` | — | não portado | `project/ui` |

## `src/ui/roadSwatch.ts` (inteiro, 98 linhas) — **não portado**

Dono: `project/ui`. Figura do perfil e editor de seção do inspetor. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `CACHE` | Drawn once per class, profile and size. | não portado | `project/ui` |
| `CASING` | — | não portado | `project/ui` |
| `CASING_COLOR` | — | não portado | `project/ui` |
| `FOOTWAY_COLOR` | — | não portado | `project/ui` |
| `GRASS_BOTTOM` | — | não portado | `project/ui` |
| `GRASS_TOP` | — | não portado | `project/ui` |
| `MARGIN` | — | não portado | `project/ui` |
| `MAX_SPAN` | World units a tile's full height covers: the widest class, plus margin. | não portado | `project/ui` |
| `MEDIAN_GREEN` | — | não portado | `project/ui` |
| `dashed` | Paint dashed along a line, so it reads as a marking and not as an edge. | não portado | `project/ui` |
| `roadSwatch` | — | não portado | `project/ui` |
| `roadSwatch.band` | — | não portado | `project/ui` |

## `src/ui/v2/shell.ts` (parte, 52 linhas) — **não se aplica**

Dono: `interface` (porte-4). Casca da interface é de `interface`; a gaveta de vias entra por `project/ui/hud_drawer.gd`.


## `src/view/cameraMotion.ts` (parte, 3 linhas) — **não se aplica**

Dono: `interface` (porte-4). Câmera é de `interface`.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `CameraMotion.owns` | — | não se aplica | `interface` (porte-4) |

## `src/world/approach.ts` (inteiro, 44 linhas) — **não portado**

Dono: `sim/roads`, `sim/traffic`. Rede, faixas e campos do 4.0: parte tem dono equivalente no jogo, nada conferido declaração a declaração. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `APPROACH_DEPTH` | How much further back than the junction mouth the approach zone reaches. | não portado | `sim/roads`, `sim/traffic` |
| `CROSSWALK_CAP` | Ceiling on the crossing's distance from a node. | não portado | `sim/roads`, `sim/traffic` |
| `CROSSWALK_DEPTH` | Depth of the zebra along the direction of travel: 3 m, the Brazilian minimum for an urban crossing | não portado | `sim/roads`, `sim/traffic` |
| `CROSSWALK_SETBACK` | Clear space between the junction mouth and the near edge of the zebra. | não portado | `sim/roads`, `sim/traffic` |
| `LEG_TRIM_CAP` | Ceiling on one leg's own trim guess, before the solver reconciles legs. | não portado | `sim/roads`, `sim/traffic` |
| `MIN_DRIVABLE_RESERVE` | Total length a segment must reserve for drivable road plus both approaches. | não portado | `sim/roads`, `sim/traffic` |
| `MIN_LINK_LENGTH` | Drivable length every link must retain between two approach zones. | não portado | `sim/roads`, `sim/traffic` |
| `RESERVE_CAP` | Ceiling on the drivable reserve a short segment is asked to keep. | não portado | `sim/roads`, `sim/traffic` |
| `STOP_BAR_SETBACK` | Clear space between the far edge of the zebra and the stop line. | não portado | `sim/roads`, `sim/traffic` |
| `STOP_BAR_WIDTH` | Painted width of the stop bar itself. | não portado | `sim/roads`, `sim/traffic` |
| `STOP_LINE_CAP` | Ceiling on the stop line's distance from a node. | não portado | `sim/roads`, `sim/traffic` |
| `crosswalkDistance` | Distance from the node to the centre of the crossing. | não portado | `sim/roads`, `sim/traffic` |
| `stopLineDistance` | Distance from the node to the stop line, i.e. the end of a link lanelet. | não portado | `sim/roads`, `sim/traffic` |

## `src/world/approachGroups.ts` (inteiro, 70 linhas) — **não portado**

Dono: `sim/roads`, `sim/traffic`. Rede, faixas e campos do 4.0: parte tem dono equivalente no jogo, nada conferido declaração a declaração. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `ApproachGroup` | A set of approach legs that can safely receive green together. | não portado | `sim/roads`, `sim/traffic` |
| `Cluster` | — | não portado | `sim/roads`, `sim/traffic` |
| `GROUP_MERGE_DEG` | Angular tolerance for merging two legs into one approach group. | não portado | `sim/roads`, `sim/traffic` |
| `computeApproachGroups` | Clusters approach legs into signal groups. | não portado | `sim/roads`, `sim/traffic` |

## `src/world/buildings/validate.ts` (parte, 24 linhas) — **não se aplica**

Dono: `build` (porte-2). Fatia dos prédios.


## `src/world/carriageway.ts` (inteiro, 44 linhas) — **não portado**

Dono: `sim/roads`, `sim/traffic`. Rede, faixas e campos do 4.0: parte tem dono equivalente no jogo, nada conferido declaração a declaração. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `onCarriageway` | DO NOT go back to a grid of every road's centre line made again at each network revision | não portado | `sim/roads`, `sim/traffic` |
| `onRoadGround` | Whether the point is on a street's ground: its carriageway, its footways, or a junction's plate (a tunnel's road is under the ground, not on it). What | não portado | `sim/roads`, `sim/traffic` |
| `ringPoints` | A junction's footway ring as points, once per ring (a junction solved again is a new one). | não portado | `sim/roads`, `sim/traffic` |

## `src/world/cells.ts` (parte, 7 linhas) — **não portado**

Dono: `sim/roads`, `sim/traffic`. Rede, faixas e campos do 4.0: parte tem dono equivalente no jogo, nada conferido declaração a declaração. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `CellFiling.takeBack` | — | não portado | `sim/roads`, `sim/traffic` |

## `src/world/changes.ts` (parte, 10 linhas) — **não portado**

Dono: `sim/roads`, `sim/traffic`. Rede, faixas e campos do 4.0: parte tem dono equivalente no jogo, nada conferido declaração a declaração. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `ChangeJournal.idsSince` | — | não portado | `sim/roads`, `sim/traffic` |

## `src/world/cityGen/field.ts` (inteiro, 65 linhas) — **não portado**

Dono: `sim/citygen`. O jogo gera a própria cidade; `streets.ts` e `plan.ts`, que o prompt pede, não foram comparados nem portados. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `BasisField` | — | não portado | `sim/citygen` |
| `FieldNoise` | — | não portado | `sim/citygen` |
| `TensorField` | — | não portado | `sim/citygen` |
| `TensorField.constructor` | — | não portado | `sim/citygen` |
| `TensorField.major` | The way the major streets run at a point (unit), or null where the field vanishes. | não portado | `sim/citygen` |
| `TensorField.minor` | The way the minor streets run: across the major ones. | não portado | `sim/citygen` |
| `TensorField.strength` | How much the field holds here, 0 where no basis field reaches. | não portado | `sim/citygen` |
| `TensorField.tensor` | The tensor at a point, as (a, b) of [[a, b], [b, -a]]. | não portado | `sim/citygen` |

## `src/world/cityGen/largeCity.ts` (parte, 3 linhas) — **não portado**

Dono: `sim/citygen`. O jogo gera a própria cidade; `streets.ts` e `plan.ts`, que o prompt pede, não foram comparados nem portados. Classificado por pasta: não li cada declaração.


## `src/world/cityGen/mapSize.ts` (inteiro, 10 linhas) — **não portado**

Dono: `sim/citygen`. O jogo gera a própria cidade; `streets.ts` e `plan.ts`, que o prompt pede, não foram comparados nem portados. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `MAP_HALF` | The map's half side, world units. | não portado | `sim/citygen` |
| `MAP_TILES` | Set at build time (`vite.config.ts`, ROADCRAFT_MAP_TILES: the page and every worker read the same); 18 otherwise (~18.4 km a side; the edge on the 2 k | não portado | `sim/citygen` |
| `__MAP_TILES__` | THE MAP'S SIZE, apart from the whole map's rules (`wholeMap.ts`): what the plan reads of it (`plan.ts`) without importing the rules that read the plan | não portado | `sim/citygen` |

## `src/world/cityGen/plan.ts` (inteiro, 157 linhas) — **não portado**

Dono: `native/src/sim/citygen`. Não comparado com a geração do jogo; o prompt pede.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `AVENUE_EVERY` | Every this many blocks an avenue, every other one between a collector. | não portado | `native/src/sim/citygen` |
| `BLOCK_LONG` | Block sizes of the 'blocks' style, in metres: a rectangular grid laid straight, no field traced | não portado | `native/src/sim/citygen` |
| `BLOCK_SHORT` | — | não portado | `native/src/sim/citygen` |
| `CityOptions` | — | não portado | `native/src/sim/citygen` |
| `CityPlan` | — | não portado | `native/src/sim/citygen` |
| `CitySize` | — | não portado | `native/src/sim/citygen` |
| `CityStyle` | — | não portado | `native/src/sim/citygen` |
| `DEFAULT_CITY` | — | não portado | `native/src/sim/citygen` |
| `LEVEL_ROAD` | Road classes the levels are laid as (`roadTypes.ts` ids). | não portado | `native/src/sim/citygen` |
| `RADIUS` | — | não portado | `native/src/sim/citygen` |
| `ROUND_REACH` | How far a round city may reach from the origin: the map's own half side (`wholeMap.ts` MAP_HALF, 5 120 m on the 10-tile map). | não portado | `native/src/sim/citygen` |
| `blockGrid` | A rectangular street grid over the city's square, axis-aligned, avenues every few blocks. | não portado | `native/src/sim/citygen` |
| `blockGrid.id` | — | não portado | `native/src/sim/citygen` |
| `blockGrid.levelOf` | — | não portado | `native/src/sim/citygen` |
| `planCity` | — | não portado | `native/src/sim/citygen` |
| `planCity.grid` | — | não portado | `native/src/sim/citygen` |
| `planCity.inside` | — | não portado | `native/src/sim/citygen` |
| `planCity.radial` | — | não portado | `native/src/sim/citygen` |
| `planCity.zoneAt` | What the land at a point is for. | não portado | `native/src/sim/citygen` |

## `src/world/cityGen/smallCity.ts` (parte, 5 linhas) — **não portado**

Dono: `sim/citygen`. O jogo gera a própria cidade; `streets.ts` e `plan.ts`, que o prompt pede, não foram comparados nem portados. Classificado por pasta: não li cada declaração.


## `src/world/cityGen/squares.ts` (parte, 6 linhas) — **não portado**

Dono: `sim/citygen`. O jogo gera a própria cidade; `streets.ts` e `plan.ts`, que o prompt pede, não foram comparados nem portados. Classificado por pasta: não li cada declaração.


## `src/world/cityGen/streets.ts` (inteiro, 484 linhas) — **não portado**

Dono: `native/src/sim/citygen/road_gen*.cpp`. Não comparado com a geração do jogo; o prompt pede.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `AnchoredGraph` | The planar street graph, and each anchor's node in it (-1: no street reached it). | não portado | `native/src/sim/citygen/road_gen*.cpp` |
| `ForcedSeed` | A street that must start at `p`, on the family of the field running nearest `along`; failing that, a straight street along `along` to the first street | não portado | `native/src/sim/citygen/road_gen*.cpp` |
| `GraphOptions` | — | não portado | `native/src/sim/citygen/road_gen*.cpp` |
| `RANKS` | Room for a street's ranks either side of its first stretch (ends carried on add a few). | não portado | `native/src/sim/citygen/road_gen*.cpp` |
| `SampleGrid` | Points of streamlines, hashed by cell, for "anything of mine within d of here?". | não portado | `native/src/sim/citygen/road_gen*.cpp` |
| `SampleGrid.add` | — | não portado | `native/src/sim/citygen/road_gen*.cpp` |
| `SampleGrid.cells` | — | não portado | `native/src/sim/citygen/road_gen*.cpp` |
| `SampleGrid.constructor` | — | não portado | `native/src/sim/citygen/road_gen*.cpp` |
| `SampleGrid.key` | — | não portado | `native/src/sim/citygen/road_gen*.cpp` |
| `SampleGrid.near` | — | não portado | `native/src/sim/citygen/road_gen*.cpp` |
| `StreetEdge` | — | não portado | `native/src/sim/citygen/road_gen*.cpp` |
| `StreetGraph` | — | não portado | `native/src/sim/citygen/road_gen*.cpp` |
| `StreetLevel` | — | não portado | `native/src/sim/citygen/road_gen*.cpp` |
| `StreetLine` | — | não portado | `native/src/sim/citygen/road_gen*.cpp` |
| `Stretch` | A street's stretch as filed: its ends, its street, and its place in the order a walk of every street meets it. | não portado | `native/src/sim/citygen/road_gen*.cpp` |
| `StretchGrid` | The streets' stretches by cell, kept as ends are carried on (`traceStreets`) | não portado | `native/src/sim/citygen/road_gen*.cpp` |
| `StretchGrid.addBack` | — | não portado | `native/src/sim/citygen/road_gen*.cpp` |
| `StretchGrid.addFront` | — | não portado | `native/src/sim/citygen/road_gen*.cpp` |
| `StretchGrid.addLine` | — | não portado | `native/src/sim/citygen/road_gen*.cpp` |
| `StretchGrid.back` | — | não portado | `native/src/sim/citygen/road_gen*.cpp` |
| `StretchGrid.cells` | — | não portado | `native/src/sim/citygen/road_gen*.cpp` |
| `StretchGrid.constructor` | — | não portado | `native/src/sim/citygen/road_gen*.cpp` |
| `StretchGrid.file` | — | não portado | `native/src/sim/citygen/road_gen*.cpp` |
| `StretchGrid.front` | — | não portado | `native/src/sim/citygen/road_gen*.cpp` |
| `StretchGrid.near` | — | não portado | `native/src/sim/citygen/road_gen*.cpp` |
| `castTo` | The first street a ray from `p` along `dir` meets within `reach` (not its own last stretch). | não portado | `native/src/sim/citygen/road_gen*.cpp` |
| `key` | — | não portado | `native/src/sim/citygen/road_gen*.cpp` |
| `oriented` | A unit way, flipped to go on the way it was going. | não portado | `native/src/sim/citygen/road_gen*.cpp` |
| `planarize` | Cuts the streamlines into a planar street graph. | não portado | `native/src/sim/citygen/road_gen*.cpp` |
| `planarize.leaving` | — | não portado | `native/src/sim/citygen/road_gen*.cpp` |
| `planarize.length` | — | não portado | `native/src/sim/citygen/road_gen*.cpp` |
| `planarize.nodeAt` | — | não portado | `native/src/sim/citygen/road_gen*.cpp` |
| `rayHit` | Parameter along p-q where it crosses a-b, or null. | não portado | `native/src/sim/citygen/road_gen*.cpp` |
| `segCross` | — | não portado | `native/src/sim/citygen/road_gen*.cpp` |
| `simplify` | — | não portado | `native/src/sim/citygen/road_gen*.cpp` |
| `traceStreets` | Traces every level; returns the streamlines, coarsest level first. | não portado | `native/src/sim/citygen/road_gen*.cpp` |
| `traceStreets.dirOf` | — | não portado | `native/src/sim/citygen/road_gen*.cpp` |
| `traceStreets.familyAlong` | — | não portado | `native/src/sim/citygen/road_gen*.cpp` |
| `traceStreets.trace` | — | não portado | `native/src/sim/citygen/road_gen*.cpp` |

## `src/world/cityGen/wholeMap.ts` (inteiro, 248 linhas) — **não portado**

Dono: `sim/citygen`. O jogo gera a própria cidade; `streets.ts` e `plan.ts`, que o prompt pede, não foram comparados nem portados. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `AVENUE_GATES` | Share of the gates that are avenues: a straight avenue across the world every few kilometres. | não portado | `sim/citygen` |
| `CITY_JITTER` | A city's first downtown, this far from its cell's middle at most (the cell at the origin: the player's start, near the middle). | não portado | `sim/citygen` |
| `CITY_SHARE` | Share of district cells holding a city at density 0 and 1 (the cell at the origin always does). | não portado | `sim/citygen` |
| `CORNER` | No gate this near a tile's corner: the corner is the two border avenues' junction. | não portado | `sim/citygen` |
| `DISTRICT` | A district cell's side: 8 tiles, so its edges are tiles' edges; cell (0, 0) is centred on the origin. | não portado | `sim/citygen` |
| `DOWNTOWN` | Heat at or over which a place is downtown, urban, a suburb; under the last, countryside. | não portado | `sim/citygen` |
| `District` | What one district cell holds: its centres and its industry. | não portado | `sim/citygen` |
| `EDGE_INSET` | How far in from the map's edge the avenues closing it run: half an avenue with its footways, so nothing of them stands past the ground, which ends at  | não portado | `sim/citygen` |
| `GATE_MAX` | — | não portado | `sim/citygen` |
| `GATE_MIN` | Distance between two gates of a border, at least and at most. | não portado | `sim/citygen` |
| `Gate` | A street's crossing of a border avenue; `level` 0 an avenue, 1 a collector (`LEVEL_ROAD`). | não portado | `sim/citygen` |
| `Hub` | A centre: how strongly it draws and how far; a downtown or a neighbourhood's. | não portado | `sim/citygen` |
| `MAP_FLOOR` | The least heat on the map: built up everywhere (suburbs at the least), the downtowns and the centres over it (the city over the whole map). | não portado | `sim/citygen` |
| `NEAR` | Cells each way whose centres can reach a place: a city's centres stand within ~7.3 km of its cell's middle and pull a radius of 3 km at most, so a cel | não portado | `sim/citygen` |
| `ORIGIN_JITTER` | — | não portado | `sim/citygen` |
| `PLAN_VERSION` | Bumped when what the plan puts where changes (the map's edge, the city's spread): a world's stored tiles and far bakes are kept under it, so a city of | não portado | `sim/citygen` |
| `SUBURB` | — | não portado | `sim/citygen` |
| `SUB_CENTRES` | Centres of neighbourhoods besides a city's downtowns. | não portado | `sim/citygen` |
| `TileCharacter` | — | não portado | `sim/citygen` |
| `TileKind` | What a tile is: how built up, and how its streets are laid. | não portado | `sim/citygen` |
| `TileStyle` | A tile's street style: a generated city's (`CityStyle`), or the countryside's (its border avenues and avenue gates only). | não portado | `sim/citygen` |
| `URBAN` | — | não portado | `sim/citygen` |
| `VILLAGE_SHARE` | Share of the other cells holding a village: one neighbourhood centre in the countryside. | não portado | `sim/citygen` |
| `WHOLE_MAP_DEFAULTS` | What the whole map asks when the player asks only for it: a style per tile, three centres, some parks. | não portado | `sim/citygen` |
| `WORKS` | Industrial districts of a city, and their reach. | não portado | `sim/citygen` |
| `WORKS_REACH` | — | não portado | `sim/citygen` |
| `WholeMapOptions` | The whole map's options: a generated city's (`CityOptions`) without a size; style 'mixed' picks a style per tile. | não portado | `sim/citygen` |
| `WholeMapPlan` | — | não portado | `sim/citygen` |
| `borderLine` | The line of border `k` along either axis, for any integer `k`: the tiles' edges (`tiles.ts`), exact multiples of `TILE`. | não portado | `sim/citygen` |
| `districtOf` | District cell (`cx`, `cy`): a city (downtowns, neighbourhood centres round them, industry away from them), a village, or open country - a pure functio | não portado | `sim/citygen` |
| `gatesOf` | The gates of row or column `k`, any integer (`axis` 0: across the borders x = const of row k, at these y; 1: across y = const of column k, at these x) | não portado | `sim/citygen` |
| `mapLine` | Where border `k`'s avenue runs: on the border, or `EDGE_INSET` inside the map on its edges (west and south at -MAP_TILES / 2, east and north at MAP_TI | não portado | `sim/citygen` |
| `mix` | A hash of a few integers: the seed of a rule's own stream. | não portado | `sim/citygen` |
| `onMap` | Whether a point is on the map. | não portado | `sim/citygen` |
| `planWholeMap` | — | não portado | `sim/citygen` |
| `planWholeMap.blockAt` | — | não portado | `sim/citygen` |
| `planWholeMap.centresNear` | The downtowns of the district cells round a place. | não portado | `sim/citygen` |
| `planWholeMap.heatAt` | How built up a place is: about 1 at a downtown, 0 in the countryside. | não portado | `sim/citygen` |
| `planWholeMap.nearWorks` | — | não portado | `sim/citygen` |
| `planWholeMap.tile` | — | não portado | `sim/citygen` |
| `planWholeMap.zoneAt` | What the land at a point is for (`CityPlan.zoneAt`). | não portado | `sim/citygen` |
| `tileMiddle` | The tile's middle. | não portado | `sim/citygen` |
| `tileOnMap` | Whether tile (i, j) is on the map (-5..4 each way at 10). | não portado | `sim/citygen` |
| `worldKey` | The key a world is stored and baked under: the seed, the map's size and the plan's version. | não portado | `sim/citygen` |

## `src/world/conflictPoints.ts` (parte, 236 linhas) — **não se aplica**

Dono: `traffic` (porte-5). Pontos de conflito pertencem à admissão de cruzamento.


## `src/world/conflictTable.ts` (parte, 3 linhas) — **não se aplica**

Dono: `traffic` (porte-5). Pontos de conflito pertencem à admissão de cruzamento.


## `src/world/curbRamps.ts` (inteiro, 204 linhas) — **não portado**

Dono: `sim/roads`, `sim/traffic`. Rede, faixas e campos do 4.0: parte tem dono equivalente no jogo, nada conferido declaração a declaração. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `FLARE_RUN` | The steepest flare: 1 in 10. | não portado | `sim/roads`, `sim/traffic` |
| `RAMP_REACH` | How far a ramp's cells reach past the road it lies on: its foot is on the carriageway's edge, within the ribbon's box (`ribbonBounds`), and its cells  | não portado | `sim/roads`, `sim/traffic` |
| `RAMP_RUN` | The steepest ramp: 1 in 12. | não portado | `sim/roads`, `sim/traffic` |
| `REGION` | The side of the regions the ramps are worked out by (`ensureRegion`). DO NOT work out every ramp of the map at its first ask (the map's load: 0.6 s of | não portado | `sim/roads`, `sim/traffic` |
| `RampState` | DO NOT build every ramp of the map again on a revision | não portado | `sim/roads`, `sim/traffic` |
| `UNMARKED_WIDTH` | The width of the unmarked crossing at a road's end (`walkways.ts`). | não portado | `sim/roads`, `sim/traffic` |
| `bring` | — | não portado | `sim/roads`, `sim/traffic` |
| `bring.ends` | — | não portado | `sim/roads`, `sim/traffic` |
| `cache` | — | não portado | `sim/roads`, `sim/traffic` |
| `ensureRect` | Every region the grid's cells over `rect` lie in (a cell lies in one region: `REGION` is a whole number of them), its ramps worked out (`ensureRegion` | não portado | `sim/roads`, `sim/traffic` |
| `ensureRegion` | Every ramp that reaches the region at (`rx`, `ry`) worked out and filed: the ramps of both ends of every road whose box meets it, widened by `RAMP_REA | não portado | `sim/roads`, `sim/traffic` |
| `followRamps` | The network rebuilt or taken back (`Network.afterRebuild`, registered with its ramps' state): its ramps, where they are kept, brought up to it in that | não portado | `sim/roads`, `sim/traffic` |
| `forgetAll` | Every ramp let go, when the network cannot say what changed (the first ask, a load): each region worked out again when asked (`ensureRegion`). | não portado | `sim/roads`, `sim/traffic` |
| `place` | A node's ramps worked out again and filed, its old ones taken out of their cells. | não portado | `sim/roads`, `sim/traffic` |
| `rampOutline` | A ramp's outline, its flares included, for cutting the footway and the kerb (rings of [x, y]). | não portado | `sim/roads`, `sim/traffic` |
| `rampOutline.at` | — | não portado | `sim/roads`, `sim/traffic` |
| `rampState` | — | não portado | `sim/roads`, `sim/traffic` |
| `rampsAt` | The ramps a node's zebras (and a road's end there) have. | não portado | `sim/roads`, `sim/traffic` |
| `rampsAt.pair` | A ramp each side of the street, `at` along it from the node, `half` its half width. | não portado | `sim/roads`, `sim/traffic` |
| `rampsMeeting` | The ramps filed in the cells `rect` reaches (each once): every ramp `walkingRise` reads at a point of it. | não portado | `sim/roads`, `sim/traffic` |
| `rampsOfNode` | The dropped kerbs of one node, as it stands (`rampState` brought up to the network first) | não portado | `sim/roads`, `sim/traffic` |
| `regionKey` | A region's key (`REGION`). | não portado | `sim/roads`, `sim/traffic` |
| `walkingRise` | How high the walking surface stands at (x, y), as a share of the footway's rise: a read of the ramps as kept (asked by every walker drawn every frame) | não portado | `sim/roads`, `sim/traffic` |

## `src/world/doc.ts` (parte, 370 linhas) — **não portado**

Dono: `sim/roads`, `sim/traffic`. Rede, faixas e campos do 4.0: parte tem dono equivalente no jogo, nada conferido declaração a declaração. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `RoadDoc.addedSegments` | — | não portado | `sim/roads`, `sim/traffic` |
| `RoadDoc.carryCrossing` | — | não portado | `sim/roads`, `sim/traffic` |
| `RoadDoc.carryMovements` | — | não portado | `sim/roads`, `sim/traffic` |
| `RoadDoc.fork` | — | não portado | `sim/roads`, `sim/traffic` |
| `RoadDoc.forkOrigin` | — | não portado | `sim/roads`, `sim/traffic` |
| `RoadDoc.moveNode` | — | não portado | `sim/roads`, `sim/traffic` |
| `RoadDoc.removeElementItems` | — | não portado | `sim/roads`, `sim/traffic` |
| `RoadDoc.roadAreas` | — | não portado | `sim/roads`, `sim/traffic` |
| `RoadDoc.roadIdsChangedFrom` | — | não portado | `sim/roads`, `sim/traffic` |
| `RoadDoc.setBalance` | — | não portado | `sim/roads`, `sim/traffic` |
| `RoadDoc.setNodeCrossing` | — | não portado | `sim/roads`, `sim/traffic` |
| `RoadDoc.setNodeUturn` | — | não portado | `sim/roads`, `sim/traffic` |
| `RoadDoc.setSegmentCutWalls` | — | não portado | `sim/roads`, `sim/traffic` |
| `RoadDoc.setSegmentStructure` | — | não portado | `sim/roads`, `sim/traffic` |
| `RoadDoc.streetNamesInUse` | — | não portado | `sim/roads`, `sim/traffic` |
| `RoadDoc.takeBack` | — | não portado | `sim/roads`, `sim/traffic` |
| `RoadDoc.trafficRevision` | — | não portado | `sim/roads`, `sim/traffic` |

## `src/world/economy.ts` (parte, 112 linhas) — **já existia**

Dono: `road_value`, dono `economy`. Preço por metro confere; devolução parcial ao encurtar é do porte de economia

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `RoadCharge` | — | já existia | `road_value`, dono `economy` |
| `buildLengths` | — | já existia | `road_value`, dono `economy` |
| `buildLengths.share` | Share of the road whose height lies in [lo, hi). | já existia | `road_value`, dono `economy` |
| `chargeFor` | — | já existia | `road_value`, dono `economy` |
| `priceAll` | — | já existia | `road_value`, dono `economy` |
| `priceOf` | — | já existia | `road_value`, dono `economy` |
| `pricePerUnit` | — | já existia | `road_value`, dono `economy` |
| `roadValue` | — | já existia | `road_value`, dono `economy` |
| `segmentCost` | — | já existia | `road_value`, dono `economy` |
| `segmentPrice` | — | já existia | `road_value`, dono `economy` |

## `src/world/elevation.ts` (inteiro, 1541 linhas) — **não se aplica**

Dono: funcionalidade `road-elevation`. Greide.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `BATTER` | Run per unit of rise on a cut or fill batter — a 1:2.5 slope. | não se aplica | funcionalidade `road-elevation` |
| `BESIDE_DROP` | How far under the carriageway the ground beside a road is laid | não se aplica | funcionalidade `road-elevation` |
| `CURVE_PEAK` | Peak-to-mean slope ratio of the smoothstep used for vertical curves. | não se aplica | funcionalidade `road-elevation` |
| `CUT_SHOULDER` | The same batter for a tunnel's approach cutting, and much narrower. | não se aplica | funcionalidade `road-elevation` |
| `CUT_WALL_REACH` | How far past a road's casing a cutting held by retaining walls is dug (`RoadSegment.cutWalls`) | não se aplica | funcionalidade `road-elevation` |
| `DECK_GRADE` | Steepest grade a deck may take to clear a rise under it. | não se aplica | funcionalidade `road-elevation` |
| `DECK_TIE` | Steepest gradient with which a deck is carried up to a higher deck it meets in the air. | não se aplica | funcionalidade `road-elevation` |
| `DILATE` | Stations either side of one that must also be cleared (chord protection). | não se aplica | funcionalidade `road-elevation` |
| `FLAT_GROUND` | Level ground everywhere: the editing rules solve on it | não se aplica | funcionalidade `road-elevation` |
| `GROUND_GRADE` | Steepest gradient of a road at grade (rise / run), from `roads/tuning.ts`. | não se aplica | funcionalidade `road-elevation` |
| `GROUND_ONLY` | — | não se aplica | funcionalidade `road-elevation` |
| `LATERAL` | Lateral samples across the casing when reading the ground under a road. | não se aplica | funcionalidade `road-elevation` |
| `LIFT_OFF` | — | não se aplica | funcionalidade `road-elevation` |
| `LIFT_ON` | Lift above the ground at which a structure stops shaping it: piers, not fill. | não se aplica | funcionalidade `road-elevation` |
| `MAX_AUTHORED_GRADE` | Maximum authored height change per unit of alignment before a ramp needs more run. | não se aplica | funcionalidade `road-elevation` |
| `MAX_SOLVE_PASSES` | Most rounds of the profile solve. | não se aplica | funcionalidade `road-elevation` |
| `MAX_STATIONS` | Hard cap on stations per segment, so a 4 km road does not allocate 1000. | não se aplica | funcionalidade `road-elevation` |
| `NO_PROFILES` | — | não se aplica | funcionalidade `road-elevation` |
| `NO_SHAPE` | — | não se aplica | funcionalidade `road-elevation` |
| `NodeHeights` | The nodes' heights as the solvers read them. | não se aplica | funcionalidade `road-elevation` |
| `ORDER_CELL` | The side of the squares the roads left are taken in (`pendingOrder`). | não se aplica | funcionalidade `road-elevation` |
| `PLATE_BLEND` | How far past a junction plate the ground floor stops constraining a ramp. | não se aplica | funcionalidade `road-elevation` |
| `PLATE_MARGIN` | Extra reach past the junction trim that the junction plate has to cover. | não se aplica | funcionalidade `road-elevation` |
| `PLATE_TANGENT_REACH` | Decay length of the grade correction just outside a flat junction plate. | não se aplica | funcionalidade `road-elevation` |
| `PLATE_TANGENT_SHARE` | Cancels enough of the incoming grade to round the join without bending the whole hill. | não se aplica | funcionalidade `road-elevation` |
| `Profile` | — | não se aplica | funcionalidade `road-elevation` |
| `ProfileIndex` | THE PROFILES BY THE CELLS THEIR BOXES COVER (`INDEX_CELL`), persistent: a solve files the roads it made, not every road | não se aplica | funcionalidade `road-elevation` |
| `ProfileIndex.add` | — | não se aplica | funcionalidade `road-elevation` |
| `ProfileIndex.cells` | The cells a box covers, or null when it covers too many to be filed in them. | não se aplica | funcionalidade `road-elevation` |
| `ProfileIndex.constructor` | — | não se aplica | funcionalidade `road-elevation` |
| `ProfileIndex.derive` | The next version, to be written; this one keeps answering as it did. | não se aplica | funcionalidade `road-elevation` |
| `ProfileIndex.digest` | A digest of everything a height, frame or road query can read at any point of a rectangle, the terrain aside | não se aplica | funcionalidade `road-elevation` |
| `ProfileIndex.empty` | — | não se aplica | funcionalidade `road-elevation` |
| `ProfileIndex.meeting` | Every profile `near` can return for a point of the rectangles, and `also`, in id order; with the cells read. | não se aplica | funcionalidade `road-elevation` |
| `ProfileIndex.near` | — | não se aplica | funcionalidade `road-elevation` |
| `ProfileIndex.oversized` | — | não se aplica | funcionalidade `road-elevation` |
| `ProfileIndex.remove` | — | não se aplica | funcionalidade `road-elevation` |
| `ProfileIndex.seal` | — | não se aplica | funcionalidade `road-elevation` |
| `RAMP_CREST` | Softness of the crest where a ramp meets its deck (a height: 3.2 m of run at the ramp's grade). | não se aplica | funcionalidade `road-elevation` |
| `RAMP_CURVE` | Length of the vertical curve at the foot of a ramp, where the grade builds up from level, in world units (8 m). | não se aplica | funcionalidade `road-elevation` |
| `RAMP_GRADE` | Grade of the straight part of a ramp between a raised deck and a road at grade: sixteen per cent. | não se aplica | funcionalidade `road-elevation` |
| `RAMP_RELAX` | The share of a ramp over which the natural ground stops being a floor. | não se aplica | funcionalidade `road-elevation` |
| `RoadElevation` | The solved field: its reads (`roadField.ts`) and what the ground and the cache ask of it. | não se aplica | funcionalidade `road-elevation` |
| `RoadShapeCut` | The roads a shaping of the ground can read inside some rectangles | não se aplica | funcionalidade `road-elevation` |
| `SHAPE_DROP` | How far below the road SURFACE the ground beside it is pulled. | não se aplica | funcionalidade `road-elevation` |
| `SHAPE_INNER` | Extra width past the casing over which the ground is held at road level. | não se aplica | funcionalidade `road-elevation` |
| `SHAPE_SHOULDER` | Width of the embankment or cutting that carries the ground back to its natural height. | não se aplica | funcionalidade `road-elevation` |
| `SHAPE_SHOULDER_MAX` | Cap on the batter, so one deep cut cannot reshape a quarter of the map. | não se aplica | funcionalidade `road-elevation` |
| `SMOOTH_REACH` | How far along a road its grade line is averaged, in world units. | não se aplica | funcionalidade `road-elevation` |
| `STATION` | Station spacing along a profile, in world units. | não se aplica | funcionalidade `road-elevation` |
| `SURFACE_READ_REACH` | How far from a point ON a road any profile can still change what a query there answers | não se aplica | funcionalidade `road-elevation` |
| `SettledNode` | What a node settled with: kept whole while its inputs (`key`) are the same. | não se aplica | funcionalidade `road-elevation` |
| `ShapingRoad` | What the ground's shaping reads of a road (`roadShapeAt`): a solve's own profile, or its copy off the page. | não se aplica | funcionalidade `road-elevation` |
| `SolveMemo` | What a solve keeps for the next one on the same ground (`previous`): persistent, each solve a version of the one before. | não se aplica | funcionalidade `road-elevation` |
| `SolveScope` | THE ROADS A SOLVE TAKES NOW, THE OTHERS LEFT FOR LATER (`buildRoadElevation` `scope`): an opening solves the roads round its view and shows the town,  | não se aplica | funcionalidade `road-elevation` |
| `StationHint` | A road's ground read off the page (`roadStations`, the job system's `roadStations`), for the solve that reads it next: its line and casing as read, so | não se aplica | funcionalidade `road-elevation` |
| `Summary` | A road's summary (`RoadElevation.summaries`): its whole solved profile as a digest, with its box. | não se aplica | funcionalidade `road-elevation` |
| `TIE_REACH` | How far past its junction plate a profile is tied back to the node height. | não se aplica | funcionalidade `road-elevation` |
| `VERTICAL_CURVE_REACH` | Reach of the vertical curves rounded into a road wherever its grade changes (see `roundGradeBreaks`), in world units | não se aplica | funcionalidade `road-elevation` |
| `buildRoadElevation` | Solves the height of every road surface in the network: from nothing, or from a solve before it, where the network changed. | não se aplica | funcionalidade `road-elevation` |
| `buildRoadElevation.capOf` | — | não se aplica | funcionalidade `road-elevation` |
| `buildRoadElevation.count` | — | não se aplica | funcionalidade `road-elevation` |
| `buildRoadElevation.gradeOf` | — | não se aplica | funcionalidade `road-elevation` |
| `buildRoadElevation.incident` | — | não se aplica | funcionalidade `road-elevation` |
| `buildRoadElevation.keyOf` | A node's inputs as a digest (no text per node of the map): where it stands, its authored height, its roads. | não se aplica | funcionalidade `road-elevation` |
| `buildRoadElevation.localDigest` | What a profile can give any point of a rectangle | não se aplica | funcionalidade `road-elevation` |
| `buildRoadElevation.mine` | A profile made this solve's own before it is written: from its grade line (`fresh`), or with the heights it had. | não se aplica | funcionalidade `road-elevation` |
| `buildRoadElevation.profileOf` | — | não se aplica | funcionalidade `road-elevation` |
| `buildRoadElevation.quantum` | — | não se aplica | funcionalidade `road-elevation` |
| `buildRoadElevation.roadsAt` | — | não se aplica | funcionalidade `road-elevation` |
| `buildRoadElevation.settledAt` | A node's figures: solved here, or as it settled in the solve reused (its inputs did not change). | não se aplica | funcionalidade `road-elevation` |
| `buildRoadElevation.solve` | — | não se aplica | funcionalidade `road-elevation` |
| `buildRoadElevation.take` | — | não se aplica | funcionalidade `road-elevation` |
| `buildRoadElevation.wantedOf` | Free height a sunken span wants, before the ramps are fitted. | não se aplica | funcionalidade `road-elevation` |
| `descentEnvelope` | — | não se aplica | funcionalidade `road-elevation` |
| `dilate` | — | não se aplica | funcionalidade `road-elevation` |
| `ease` | — | não se aplica | funcionalidade `road-elevation` |
| `elevationWork` | The work of the solves, counted (`tests/scale`): profiles made and nodes whose inputs were read. | não se aplica | funcionalidade `road-elevation` |
| `everyRoad` | @rebuild Every road of the network, by id: what a scoped solve from nothing leaves but those it takes (once, at an opening). | não se aplica | funcionalidade `road-elevation` |
| `flatSolves` | Each network's last solve on level ground: the next one on it, or on a working copy of it (`Network.fork`), starts there. | não se aplica | funcionalidade `road-elevation` |
| `flatSolvesOf` | The solves on level ground a solve of `net` may start from: its own last, then those of the networks it is a copy of. | não se aplica | funcionalidade `road-elevation` |
| `flattenPlates` | — | não se aplica | funcionalidade `road-elevation` |
| `gradeEnvelope` | — | não se aplica | funcionalidade `road-elevation` |
| `lookDigest` | A digest of what a road looks like beyond its widths (materials, flush footways); 0 for the class's own. | não se aplica | funcionalidade `road-elevation` |
| `memos` | — | não se aplica | funcionalidade `road-elevation` |
| `noGround` | — | não se aplica | funcionalidade `road-elevation` |
| `pendingOrder` | The roads a scoped solve left, in the order to take them: a square of `ORDER_CELL` at a time, the squares nearest (`x`, `y`) first, each square's road | não se aplica | funcionalidade `road-elevation` |
| `pinPlates` | — | não se aplica | funcionalidade `road-elevation` |
| `rampLength` | Plate edge to deck, for a ramp that climbs `rise`: sag, straight and crest. | não se aplica | funcionalidade `road-elevation` |
| `rampRise` | Height a ramp has gained `d` units past the plate edge | não se aplica | funcionalidade `road-elevation` |
| `readStations` | The ground under a road: the dilated ceiling and the designed grade line (see the stations step). | não se aplica | funcionalidade `road-elevation` |
| `roadShapeAt` | DO NOT give either side its own copy of it. | não se aplica | funcionalidade `road-elevation` |
| `roadShaper` | The roads of a cut (`RoadElevation.shapeCut`) read as the solve reads its own: `shapeAt` | não se aplica | funcionalidade `road-elevation` |
| `roadShaper.shapeAt` | How far the ground should be pulled towards the road at a point, and to what height. | não se aplica | funcionalidade `road-elevation` |
| `roadStations` | The ground under a road at its stations, read as the solve reads it (`readStations`): what a worker reads off the shared ground for the page. | não se aplica | funcionalidade `road-elevation` |
| `roadsToRead` | The roads the next solve of `net` from `previous` reads the ground under (new, moved or widened, or on ground that moved: those `buildRoadElevation` c | não se aplica | funcionalidade `road-elevation` |
| `roundGradeBreaks` | — | não se aplica | funcionalidade `road-elevation` |
| `roundPlateJoins` | — | não se aplica | funcionalidade `road-elevation` |
| `roundPlateJoins.steep` | — | não se aplica | funcionalidade `road-elevation` |
| `sameLine` | — | não se aplica | funcionalidade `road-elevation` |
| `sameProfile` | Whether a profile is built from the same inputs as the one solved before it. | não se aplica | funcionalidade `road-elevation` |
| `sameXY` | — | não se aplica | funcionalidade `road-elevation` |
| `scopeIds` | The roads a scope names, those of the network only. | não se aplica | funcionalidade `road-elevation` |
| `shapeHit` | Scratch of `roadShapeAt`'s nearest-point reads. | não se aplica | funcionalidade `road-elevation` |
| `slopeLimit` | — | não se aplica | funcionalidade `road-elevation` |
| `smoothMin` | The smaller of two heights with the corner between them rounded over a band `k` high (a quadratic smooth minimum). | não se aplica | funcionalidade `road-elevation` |
| `smoothProfile` | — | não se aplica | funcionalidade `road-elevation` |
| `solveGround` | A road at grade: it follows the ground, flat over its junction plates. | não se aplica | funcionalidade `road-elevation` |
| `solveGround.baseAt` | — | não se aplica | funcionalidade `road-elevation` |
| `solveGround.tieFor` | — | não se aplica | funcionalidade `road-elevation` |
| `solveRaised` | A raised structure: a level deck between two short ramps. | não se aplica | funcionalidade `road-elevation` |
| `solveSunken` | A tunnel: a flat floor below the ground, with a ramp up to each end. | não se aplica | funcionalidade `road-elevation` |
| `solveVariable` | A continuous authored vertical alignment, including ground, bridge and bore. | não se aplica | funcionalidade `road-elevation` |
| `stationIndex` | — | não se aplica | funcionalidade `road-elevation` |
| `stationsAt` | Where each station of a profile lies (`h[i]` at arc `i * step`), measured once per profile (a profile is shared by solves). | não se aplica | funcionalidade `road-elevation` |
| `withProfile` | The profiles in id order with `profile` put in its place (replacing the one of its id): a new list, the old one kept. | não se aplica | funcionalidade `road-elevation` |

## `src/world/footwayPath.ts` (inteiro, 62 linhas) — **não portado**

Dono: `sim/roads`, `sim/traffic`. Rede, faixas e campos do 4.0: parte tem dono equivalente no jogo, nada conferido declaração a declaração. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `FootwayNet` | — | não portado | `sim/roads`, `sim/traffic` |
| `FootwayRibbon` | DO NOT import `network.ts` or `doc.ts` here | não portado | `sim/roads`, `sim/traffic` |
| `REJECT_STEP` | How far apart a footway is sampled when its nearest point is turned down, world units. | não portado | `sim/roads`, `sim/traffic` |
| `footwayPath` | One side's footway of a road, as the walking network lays it: down the middle of that side's through zone, along a to b | não portado | `sim/roads`, `sim/traffic` |
| `nearestFootway` | DO NOT build the walking network of the whole map to find the footway at one point. | não portado | `sim/roads`, `sim/traffic` |

## `src/world/geometry.ts` (inteiro, 101 linhas) — **não portado**

Dono: `sim/roads`, `sim/traffic`. Rede, faixas e campos do 4.0: parte tem dono equivalente no jogo, nada conferido declaração a declaração. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `LegFrame` | A cross-section frame taken at some distance from one end of a segment. | não portado | `sim/roads`, `sim/traffic` |
| `PolylineCache` | Cheap cache of per-segment polylines, rebuilt only for dirty segments. | não portado | `sim/roads`, `sim/traffic` |
| `PolylineCache.clear` | — | não portado | `sim/roads`, `sim/traffic` |
| `PolylineCache.forkFrom` | A working copy over `other` (`Network.fork`): reads through it, keeps only the polylines it makes or drops. | não portado | `sim/roads`, `sim/traffic` |
| `PolylineCache.get` | The segment's centreline. | não portado | `sim/roads`, `sim/traffic` |
| `PolylineCache.invalidate` | — | não portado | `sim/roads`, `sim/traffic` |
| `PolylineCache.map` | — | não portado | `sim/roads`, `sim/traffic` |
| `PolylineCache.size` | — | não portado | `sim/roads`, `sim/traffic` |
| `PolylineCache.takeBack` | A working copy's changes (`forkFrom`) written back. | não portado | `sim/roads`, `sim/traffic` |
| `farNode` | The other endpoint of `seg`. | não portado | `sim/roads`, `sim/traffic` |
| `frameFromNode` | Frame at `distance` measured from `nodeId` along `seg`. | não portado | `sim/roads`, `sim/traffic` |
| `orientedPolyline` | Centreline oriented so that it starts at `from`. | não portado | `sim/roads`, `sim/traffic` |
| `segmentLine` | A segment's centreline from its two ends and its curve (`segmentPolyline`): what a worker makes it from too (`junctionBuild.ts`). | não portado | `sim/roads`, `sim/traffic` |
| `segmentPolyline` | Flattened centreline of a segment, oriented a -> b. | não portado | `sim/roads`, `sim/traffic` |
| `segmentStartsAt` | — | não portado | `sim/roads`, `sim/traffic` |

## `src/world/grid.ts` (inteiro, 25 linhas) — **não portado**

Dono: `sim/roads`, `sim/traffic`. Rede, faixas e campos do 4.0: parte tem dono equivalente no jogo, nada conferido declaração a declaração. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `GRID_CELL` | One cell of the grid, 10 m. | não portado | `sim/roads`, `sim/traffic` |
| `GRID_STEP` | One subdivision of a cell, 1 m. | não portado | `sim/roads`, `sim/traffic` |
| `onGrid` | Whether a point is on the grid (`snapToGrid` leaves it where it is). | não portado | `sim/roads`, `sim/traffic` |
| `onGridLength` | The nearest whole number of subdivisions to a length, never below `min` of them. | não portado | `sim/roads`, `sim/traffic` |
| `snapToGrid` | The nearest grid point (a whole number of subdivisions on both axes). | não portado | `sim/roads`, `sim/traffic` |

## `src/world/growth/zoning.ts` (parte, 5 linhas) — **não se aplica**

Dono: `build` (porte-2). Crescimento é de `build`.


## `src/world/gullies.ts` (inteiro, 24 linhas) — **não se aplica**

Dono: `landscape` (porte-3). Também é entrada de `landscape`.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `DEFAULT_GULLY_AUTO` | How much of the steep land carries gullies of itself, 0..1, when the map says nothing. | não se aplica | `landscape` (porte-3) |
| `GullyDab` | — | não se aplica | `landscape` (porte-3) |
| `MAX_GULLY_DABS` | A map keeps at most this many gully dabs; the oldest go first. | não se aplica | `landscape` (porte-3) |
| `readGullyDab` | — | não se aplica | `landscape` (porte-3) |
| `readGullyDab.finite` | — | não se aplica | `landscape` (porte-3) |

## `src/world/junction/build.ts` (inteiro, 299 linhas) — **não portado**

Dono: `sim/roads/junction`. O jogo tem braços, recuos, esquinas e balões próprios; falta conferir transições e derivados um a um. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `BuildOptions` | — | não portado | `sim/roads/junction` |
| `CHAIN_BEND` | Largest bend at a node of two legs that is still one road with no junction, radians. | não portado | `sim/roads/junction` |
| `CHAIN_BEND_COS` | Two legs of the same class meeting at less than this angle need no junction. | não portado | `sim/roads/junction` |
| `MERGE_BEND` | Widest angle between a ramp and the carriageway it joins or leaves that is still a gore | não portado | `sim/roads/junction` |
| `REFRAME_EPS` | A mouth moved further than this after its leg was framed is framed again, world units. | não portado | `sim/roads/junction` |
| `SEAM_RUN` | How far each leg is given to a change of structure on one road, units. | não portado | `sim/roads/junction` |
| `SLAB_BUMP_CAP` | Runaway guard for the slab-separation loop, as a multiple of the widest leg. | não portado | `sim/roads/junction` |
| `SurfaceMode` | How a node behaves geometrically. | não portado | `sim/roads/junction` |
| `bendAtNode` | How far the road turns AT a node of two legs: the angle between the two roads' own directions where they meet, radians. | não portado | `sim/roads/junction` |
| `buildJunction` | Builds one surface level of one junction. | não portado | `sim/roads/junction` |
| `buildJunction.bySegment` | Re-keys this pass's positional trims so the next pass cannot mis-index them. | não portado | `sim/roads/junction` |
| `buildJunction.capTrims` | — | não portado | `sim/roads/junction` |
| `buildJunction.scaleOf` | — | não portado | `sim/roads/junction` |
| `outlinesMeet` | Whether two roads meeting at a node run on into each other at every level with the same outline | não portado | `sim/roads/junction` |
| `outlinesMeet.minus` | — | não portado | `sim/roads/junction` |
| `outlinesMeet.plus` | — | não portado | `sim/roads/junction` |
| `structureSeam` | Whether the two roads at a two-leg node differ only in their structure. | não portado | `sim/roads/junction` |
| `surfaceMode` | Classifies a node. | não portado | `sim/roads/junction` |

## `src/world/junction/bulb.ts` (inteiro, 136 linhas) — **não portado**

Dono: `sim/roads/junction`. O jogo tem braços, recuos, esquinas e balões próprios; falta conferir transições e derivados um a um. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `BULB_RADIUS` | A TURNING CIRCLE AT A ROAD'S END: the player marks an end as one | não portado | `sim/roads/junction` |
| `BULB_RETURN` | The kerb's return from the road's edge into the circle (Silverton, OR: a 25 ft curb radius at a bulb's transition) | não portado | `sim/roads/junction` |
| `BULB_TURN_RADIUS` | The radius traffic turns round it at | não portado | `sim/roads/junction` |
| `Outline` | A bulb's outline at some lateral reach, in its leg's frame at the node. | não portado | `sim/roads/junction` |
| `buildBulb` | The circle's outline at one level from its node, the leg cut where its kerb returns begin. | não portado | `sim/roads/junction` |
| `buildBulb.at` | — | não portado | `sim/roads/junction` |
| `bulbClearing` | DO NOT walk every junction of the map, and every pole and element of the map for each, after every edit | não portado | `sim/roads/junction` |
| `bulbLine` | A line round a bulb `offset` from its road's centreline on both sides | não portado | `sim/roads/junction` |
| `clearBulbsOf` | `bulbClearing`'s work: the circles `delta` names, or every junction when it is null. | não portado | `sim/roads/junction` |
| `clearBulbsOf.candidates` | @rebuild Every junction only when the network cannot say what changed (the first clearing, a load). | não portado | `sim/roads/junction` |
| `outline` | The outline `grow` outside the carriageway's | não portado | `sim/roads/junction` |
| `outline.along` | — | não portado | `sim/roads/junction` |
| `outline.at` | — | não portado | `sim/roads/junction` |
| `outline.touch` | — | não portado | `sim/roads/junction` |

## `src/world/junction/corners.ts` (inteiro, 205 linhas) — **não portado**

Dono: `sim/roads/junction`. O jogo tem braços, recuos, esquinas e balões próprios; falta conferir transições e derivados um a um. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `CornerData` | A corner as plain data (`cornerData`, `cornerOf`): what a message's clone keeps of it (`NetworkBake`). | não portado | `sim/roads/junction` |
| `MODES` | The corners' modes by number, for `packCorners`. | não portado | `sim/roads/junction` |
| `PSI,TRIM_I,TRIM_J,XX,XY,TAX,TAY,TBX,TBY,CX,CY,R,RUN` | — | não portado | `sim/roads/junction` |
| `PackedCorner` | DO NOT keep a corner as an object of points (its point, its fillet's three: ~300 B a corner, 2026-10-21): its numbers in one run, its points made when | não portado | `sim/roads/junction` |
| `PackedCorner.constructor` | — | não portado | `sim/roads/junction` |
| `PackedCorner.fillet` | — | não portado | `sim/roads/junction` |
| `PackedCorner.psi` | — | não portado | `sim/roads/junction` |
| `PackedCorner.trimI` | — | não portado | `sim/roads/junction` |
| `PackedCorner.trimJ` | — | não portado | `sim/roads/junction` |
| `PackedCorner.x` | — | não portado | `sim/roads/junction` |
| `collinear` | — | não portado | `sim/roads/junction` |
| `computeCorners` | Resolves every corner of a junction. | não portado | `sim/roads/junction` |
| `cornerData` | A corner's fields as plain data (`CornerData`). | não portado | `sim/roads/junction` |
| `cornerOf` | A corner kept compact (`PackedCorner`) from its fields. | não portado | `sim/roads/junction` |
| `packCorners` | Corners as one run of numbers (`packLegs`): each its legs, mode, whether it has a point, its run's length, then its run (`PackedCorner`). | não portado | `sim/roads/junction` |
| `project` | Distance along `leg` at which `p` sits, measured from the leg's ray origin. | não portado | `sim/roads/junction` |
| `resolveCorner` | — | não portado | `sim/roads/junction` |
| `resolveCorner.cap` | — | não portado | `sim/roads/junction` |
| `unpackCorners` | The corners of `packCorners`. | não portado | `sim/roads/junction` |

## `src/world/junction/derive.ts` (inteiro, 203 linhas) — **não portado**

Dono: `sim/roads/junction`. O jogo tem braços, recuos, esquinas e balões próprios; falta conferir transições e derivados um a um. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `<ImportDeclaration>` | — | não portado | `sim/roads/junction` |
| `<ImportDeclaration>@11` | — | não portado | `sim/roads/junction` |
| `<ImportDeclaration>@12` | — | não portado | `sim/roads/junction` |
| `BLEND_STEPS` | Points along a return whose two sides have different widths. | não portado | `sim/roads/junction` |
| `JUNCTION_KIT` | The solver's functions a node's levels are made again with (`levels.ts` reads none of them itself, `JunctionKit`). | não portado | `sim/roads/junction` |
| `SQUARE_MIN` | Below this wedge, or above its supplement, the footway's back is rounded, not squared. | não portado | `sim/roads/junction` |
| `SQUARE_MIN_WIDTH` | Narrowest footway a squared back corner may leave beside the kerb return. | não portado | `sim/roads/junction` |
| `SQUARE_REACH` | How far past the kerb return a squared back corner may reach, world units. | não portado | `sim/roads/junction` |
| `baseTrims` | The carriageway's trims by road, the last leg of a road standing (a road that comes back to its node). | não portado | `sim/roads/junction` |
| `derivable` | Whether an outer level can be carried out from `base` (`deriveJunctionLevel`) at all. | não portado | `sim/roads/junction` |
| `deriveFromLegs` | One outer level of `base` from its legs at that level (`buildLegs` framed at the carriageway's trims, or the same legs kept as numbers, `junction/leve | não portado | `sim/roads/junction` |
| `deriveFromLegs.push` | — | não portado | `sim/roads/junction` |
| `deriveJunctionLevel` | — | não portado | `sim/roads/junction` |
| `deriveTransitionFromLegs` | One surface of a transition, as an offset of the carriageway's. | não portado | `sim/roads/junction` |

## `src/world/junction/legs.ts` (inteiro, 165 linhas) — **não portado**

Dono: `sim/roads/junction`. O jogo tem braços, recuos, esquinas e balões próprios; falta conferir transições e derivados um a um. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `DIRECTIONS` | A road's directions by number, for `packLegs`. | não portado | `sim/roads/junction` |
| `LEG_PACK` | How many numbers `packLegs` gives a leg: its road, far node, side, type, direction, approach and profile (an index into the roads), then its run (`Pac | não portado | `sim/roads/junction` |
| `LegBuildOptions` | — | não portado | `sim/roads/junction` |
| `LegData` | A leg as plain data, every field its own (`legData`, `legOf`): what a message's clone keeps of it (`NetworkBake`). | não portado | `sim/roads/junction` |
| `OX,OY,DX,DY,NX,NY,ANG,HW,HW_LEFT,HW_RIGHT,HW_SIDEWALK,LENGTH,SHIFT` | — | não portado | `sim/roads/junction` |
| `PackedLeg` | DO NOT keep a leg as an object of points and numbers (three points and seven boxed numbers, ~300 B a leg, four levels of every junction: 2026-10-21):  | não portado | `sim/roads/junction` |
| `PackedLeg.ang` | — | não portado | `sim/roads/junction` |
| `PackedLeg.constructor` | — | não portado | `sim/roads/junction` |
| `PackedLeg.dir` | — | não portado | `sim/roads/junction` |
| `PackedLeg.hw` | — | não portado | `sim/roads/junction` |
| `PackedLeg.hwLeft` | — | não portado | `sim/roads/junction` |
| `PackedLeg.hwRight` | — | não portado | `sim/roads/junction` |
| `PackedLeg.hwSidewalk` | — | não portado | `sim/roads/junction` |
| `PackedLeg.length` | — | não portado | `sim/roads/junction` |
| `PackedLeg.nrm` | — | não portado | `sim/roads/junction` |
| `PackedLeg.origin` | — | não portado | `sim/roads/junction` |
| `PackedLeg.shift` | — | não portado | `sim/roads/junction` |
| `buildLegs` | Builds the legs of a node, sorted counter-clockwise by outgoing angle. | não portado | `sim/roads/junction` |
| `legData` | A leg's fields as plain data (`LegData`). | não portado | `sim/roads/junction` |
| `legOf` | A leg kept compact (`PackedLeg`) from its fields. | não portado | `sim/roads/junction` |
| `mouthLeft` | The mouth corner on the `+nrm` side of a leg, at distance `t`. | não portado | `sim/roads/junction` |
| `mouthRight` | The mouth corner on the `-nrm` side of a leg, at distance `t`. | não portado | `sim/roads/junction` |
| `packLegs` | Legs as one run of numbers, their profiles in `roads` (each once: a message's clone keeps one copy of an object it meets twice): DO NOT hand a thread  | não portado | `sim/roads/junction` |
| `unpackLegs` | The legs of `packLegs`. | não portado | `sim/roads/junction` |

## `src/world/junction/levels.ts` (inteiro, 345 linhas) — **não se aplica**

Dono: funcionalidade `road-elevation`. Níveis de cruzamento.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `BASE,OX,OY,DX,DY,NX,NY,ANG,LENGTH` | — | não se aplica | funcionalidade `road-elevation` |
| `EMPTY_RINGS` | — | não se aplica | funcionalidade `road-elevation` |
| `FRAME` | A frame's numbers in `JunctionLevels.frames`: the carriageway's leg it is (its index), then its origin, direction, normal, angle and road length. | não se aplica | funcionalidade `road-elevation` |
| `HELD_LEVELS` | How many outer levels made again are held, per level: the junctions of the cells being drawn and walked. | não se aplica | funcionalidade `road-elevation` |
| `HELD_RINGS` | How many carriageways' tongues and rings made again are held. | não se aplica | funcionalidade `road-elevation` |
| `JunctionData` | A junction as runs of numbers, for another thread (`NetworkBake`): its legs and corners packed (`packLegs`, `packCorners`), its rings as runs. | não se aplica | funcionalidade `road-elevation` |
| `JunctionLevels` | A node's junction at each surface level, read as the map of them it stands for (in `SURFACE_LEVELS` order). | não se aplica | funcionalidade `road-elevation` |
| `JunctionLevels.EMPTY` | — | não se aplica | funcionalidade `road-elevation` |
| `JunctionLevels.[Symbol.iterator]` | — | não se aplica | funcionalidade `road-elevation` |
| `JunctionLevels.constructor` | — | não se aplica | funcionalidade `road-elevation` |
| `JunctionLevels.data` | — | não se aplica | funcionalidade `road-elevation` |
| `JunctionLevels.entries` | — | não se aplica | funcionalidade `road-elevation` |
| `JunctionLevels.forEach` | — | não se aplica | funcionalidade `road-elevation` |
| `JunctionLevels.forEachTrim` | Each level's legs' roads and trims, in level order (`networkBuild.ts` reconciles a road's ends from them), without making a level. | não se aplica | funcionalidade `road-elevation` |
| `JunctionLevels.fromData` | — | não se aplica | funcionalidade `road-elevation` |
| `JunctionLevels.get` | — | não se aplica | funcionalidade `road-elevation` |
| `JunctionLevels.has` | — | não se aplica | funcionalidade `road-elevation` |
| `JunctionLevels.keys` | — | não se aplica | funcionalidade `road-elevation` |
| `JunctionLevels.legsAt` | A level's legs, without making the level (`elevation.ts` reads the casing's widths at every junction). | não se aplica | funcionalidade `road-elevation` |
| `JunctionLevels.make` | A derived level made again from the frames and the carriageway (`deriveFromLegs`). | não se aplica | funcionalidade `road-elevation` |
| `JunctionLevels.of` | — | não se aplica | funcionalidade `road-elevation` |
| `JunctionLevels.ringAt` | A level's outline, without making the level when it is kept (the carriageway's, the casing's). | não se aplica | funcionalidade `road-elevation` |
| `JunctionLevels.size` | — | não se aplica | funcionalidade `road-elevation` |
| `JunctionLevels.values` | — | não se aplica | funcionalidade `road-elevation` |
| `JunctionLevelsData` | A node's levels as plain data (`JunctionLevels.data`, `fromData`). | não se aplica | funcionalidade `road-elevation` |
| `LeanJunction` | A junction whose tongues and rings are made when asked (`fill`), or kept as they came when they are of another form (a turning circle, a taper, a merg | não se aplica | funcionalidade `road-elevation` |
| `LeanJunction.constructor` | — | não se aplica | funcionalidade `road-elevation` |
| `LeanJunction.rings` | — | não se aplica | funcionalidade `road-elevation` |
| `LeanJunction.tongues` | — | não se aplica | funcionalidade `road-elevation` |
| `RingRun` | A ring as its run of numbers (`Ring.run`): what a message's clone keeps of it (a class's methods are not cloned, `structuredClone`). | não se aplica | funcionalidade `road-elevation` |
| `bit` | — | não se aplica | funcionalidade `road-elevation` |
| `boundsOf` | Every ring a node's junctions put on any level; null for none. | não se aplica | funcionalidade `road-elevation` |
| `fill` | The tongues and rings `buildJunctionRing` and `deriveJunctionLevel` lay round a junction's outline: one tongue a leg cut back, a cap where every leg l | não se aplica | funcionalidade `road-elevation` |
| `filled` | Whether a junction's tongues and rings are the ones `fill` makes from it (checked by identity and count, never assumed). | não se aplica | funcionalidade `road-elevation` |
| `framesOf` | The frames of `legs` (a derived level's), each with the carriageway's leg of its road; null when a road has none there. | não se aplica | funcionalidade `road-elevation` |
| `junctionData` | — | não se aplica | funcionalidade `road-elevation` |
| `junctionOf` | — | não se aplica | funcionalidade `road-elevation` |
| `leanJunction` | A junction kept small (`LeanJunction`). | não se aplica | funcionalidade `road-elevation` |
| `legAt` | A leg at `level` from a frame of `frames` (`JunctionLevels.frames`) and the carriageway's leg of the same road: `buildLegs`'s numbers, the same. | não se aplica | funcionalidade `road-elevation` |
| `legsFrom` | — | não se aplica | funcionalidade `road-elevation` |
| `made` | — | não se aplica | funcionalidade `road-elevation` |
| `madeAt` | — | não se aplica | funcionalidade `road-elevation` |
| `madeCasing` | The outer levels made again (`JunctionLevels.get`), one cache a level. | não se aplica | funcionalidade `road-elevation` |
| `madeCurb` | — | não se aplica | funcionalidade `road-elevation` |
| `madeRings` | A carriageway's tongues and rings made again from its legs, trims and outline (`LeanJunction`). | não se aplica | funcionalidade `road-elevation` |
| `madeSidewalk` | — | não se aplica | funcionalidade `road-elevation` |
| `runOf` | — | não se aplica | funcionalidade `road-elevation` |
| `sameLeg` | Whether two legs carry the same numbers and road. | não se aplica | funcionalidade `road-elevation` |
| `sameLegs` | — | não se aplica | funcionalidade `road-elevation` |

## `src/world/junction/mouthOverlap.ts` (inteiro, 26 linhas) — **não portado**

Dono: `sim/roads/junction`. O jogo tem braços, recuos, esquinas e balões próprios; falta conferir transições e derivados um a um. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `REACH` | How far the strip reaches either side of the mouth, along the road, world units. | não portado | `sim/roads/junction` |
| `SHORTEST` | Shortest strip worth adding, world units. | não portado | `sim/roads/junction` |
| `STEP` | Spacing of the strip's cross-sections, world units. | não portado | `sim/roads/junction` |
| `mouthOverlapStrip` | The strip of a curved road's full width across one of its junction mouths. | não portado | `sim/roads/junction` |

## `src/world/junction/polygon.ts` (inteiro, 239 linhas) — **não portado**

Dono: `sim/roads/junction`. O jogo tem braços, recuos, esquinas e balões próprios; falta conferir transições e derivados um a um. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `CAP_STEPS` | Points on the circle approximating a node cap. | não portado | `sim/roads/junction` |
| `JunctionRing` | — | não portado | `sim/roads/junction` |
| `RING_REACH_FACTOR` | Absolute reach of a junction ring: three times the widest road at the node. | não portado | `sim/roads/junction` |
| `arcReach` | Farthest point of a circular arc from `centre`. | não portado | `sim/roads/junction` |
| `buildJunctionRing` | Assembles the junction boundary for one surface level. | não portado | `sim/roads/junction` |
| `buildJunctionRing.push` | — | não portado | `sim/roads/junction` |
| `findSlabViolations` | Checks that no leg's mouth corner falls inside a non-adjacent leg's carriageway. | não portado | `sim/roads/junction` |
| `legTongue` | Rectangle covering a leg's carriageway from the node out to its mouth. | não portado | `sim/roads/junction` |
| `nodeCap` | A rounded cap over the node, for junctions whose legs all leave on one side. | não portado | `sim/roads/junction` |
| `nodeCentre` | The node itself. `leg.origin` is back-projected so every leg agrees on it. | não portado | `sim/roads/junction` |
| `ringReach` | Farthest a vertex of this junction may sit from the node. | não portado | `sim/roads/junction` |

## `src/world/junction/transition.ts` (inteiro, 120 linhas) — **não portado**

Dono: `sim/roads/junction`. O jogo tem braços, recuos, esquinas e balões próprios; falta conferir transições e derivados um a um. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `SAMPLE_STEP` | Taper length between two cross-sections when the outline is sampled. | não portado | `sim/roads/junction` |
| `TAPER_MIN` | Shortest taper worth drawing, end to end, in world units. | não portado | `sim/roads/junction` |
| `TAPER_RATIO` | Run of a taper per unit of lateral change of the kerb line: 1 in 8. | não portado | `sim/roads/junction` |
| `TRANSITION_BEND` | Largest bend at a node of two legs that is still one road running on: no zebra and no stop line | não portado | `sim/roads/junction` |
| `TransitionAxis` | The centreline of a transition, from the mouth of the first leg to the mouth of the second | não portado | `sim/roads/junction` |
| `TransitionAxis.constructor` | — | não portado | `sim/roads/junction` |
| `TransitionAxis.length` | — | não portado | `sim/roads/junction` |
| `TransitionAxis.normal` | Unit LEFT normal of the direction of travel from the first leg to the second. | não portado | `sim/roads/junction` |
| `TransitionAxis.offset` | Points `offset(u)` to the left of the axis, one per sample. | não portado | `sim/roads/junction` |
| `TransitionAxis.p0` | — | não portado | `sim/roads/junction` |
| `TransitionAxis.p1` | — | não portado | `sim/roads/junction` |
| `TransitionAxis.p2` | — | não portado | `sim/roads/junction` |
| `TransitionAxis.p3` | — | não portado | `sim/roads/junction` |
| `TransitionAxis.point` | — | não portado | `sim/roads/junction` |
| `TransitionAxis.samples` | Parameters of the cross-sections an outline or a stroke is sampled at. | não portado | `sim/roads/junction` |
| `isTransition` | Whether two legs meeting at a node form a straight-through change of width. | não portado | `sim/roads/junction` |
| `legWidthStep` | The largest change of half-width from one leg into the other, SIDE BY SIDE | não portado | `sim/roads/junction` |
| `legWidthStep.other` | — | não portado | `sim/roads/junction` |
| `taperEase` | Ease of the cross-section from the first leg's to the second's. | não portado | `sim/roads/junction` |
| `transitionRing` | The outline of one surface level of a transition: both kerb lines of the taper, from mouth to mouth. | não portado | `sim/roads/junction` |
| `transitionRing.halfLeft` | — | não portado | `sim/roads/junction` |
| `transitionRing.halfRight` | — | não portado | `sim/roads/junction` |
| `transitionRunFor` | How far each leg is given over to the taper, from the node. | não portado | `sim/roads/junction` |
| `widthStep` | The largest change of half-width between two profiles, over every level and the median. | não portado | `sim/roads/junction` |

## `src/world/junction/trim.ts` (inteiro, 55 linhas) — **não portado**

Dono: `sim/roads/junction`. O jogo tem braços, recuos, esquinas e balões próprios; falta conferir transições e derivados um a um. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `ClampResult` | — | não portado | `sim/roads/junction` |
| `SegmentTrimBudget` | — | não portado | `sim/roads/junction` |
| `clampSegmentTrims` | Reconciles the two ends of a segment against its own length. | não portado | `sim/roads/junction` |
| `computeTrims` | THE trim solver. | não portado | `sim/roads/junction` |

## `src/world/junction/trimSummary.ts` (inteiro, 42 linhas) — **não portado**

Dono: `sim/roads/junction`. O jogo tem braços, recuos, esquinas e balões próprios; falta conferir transições e derivados um a um. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `SUMMARY,SUMMARY_FLOOR` | — | não portado | `sim/roads/junction` |
| `trimSummary` | What `reconcileSegment` reads of a node's junction, by road: each level's greatest trim of the road's legs (at least 0), and the part of its mouth no  | não portado | `sim/roads/junction` |
| `trimSummary.slot` | — | não portado | `sim/roads/junction` |

## `src/world/junction/types.ts` (inteiro, 95 linhas) — **não portado**

Dono: `sim/roads/junction`. O jogo tem braços, recuos, esquinas e balões próprios; falta conferir transições e derivados um a um. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `Corner` | — | não portado | `sim/roads/junction` |
| `CornerMode` | — | não portado | `sim/roads/junction` |
| `Junction` | — | não portado | `sim/roads/junction` |
| `JunctionKit` | What `levels.ts` makes a node's levels again with, handed in by who solves them (`networkBuild.ts`): the solver's own functions, the same. | não portado | `sim/roads/junction` |
| `Leg` | One approach arm of a junction, resolved at a specific surface level. | não portado | `sim/roads/junction` |

## `src/world/junctionBuild.ts` (inteiro, 416 linhas) — **não portado**

Dono: `sim/roads`, `sim/traffic`. Rede, faixas e campos do 4.0: parte tem dono equivalente no jogo, nada conferido declaração a declaração. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `CARRIED_MARGIN` | A pair of legs clearly straighter than every other pair by at least this much. | não portado | `sim/roads`, `sim/traffic` |
| `CARRIED_MAX_BEND` | A pair bending more than this is a corner between two roads, not one road. | não portado | `sim/roads`, `sim/traffic` |
| `GROUPING_LOOKAHEAD_FRACTION` | How far along a leg to look when deciding which approaches share an axis. | não portado | `sim/roads`, `sim/traffic` |
| `GROUPING_LOOKAHEAD_MAX` | — | não portado | `sim/roads`, `sim/traffic` |
| `JunctionInput` | Everything one junction's connectors are made of, but its surface (`junctionMovements`). | não portado | `sim/roads`, `sim/traffic` |
| `JunctionLane` | A link lane arriving at or leaving the junction (`LaneletGraph.buildSegmentLinks`), its centre made (`LaneAsk` as sent). | não portado | `sim/roads`, `sim/traffic` |
| `JunctionMade` | One junction's movements, in the order made, with its approach groups and its control. | não portado | `sim/roads`, `sim/traffic` |
| `MadeConnector` | A movement through the junction as `junctionMovements` made it. | não portado | `sim/roads`, `sim/traffic` |
| `STRAIGHT_BEND` | Below this bend a movement is already classified as straight on. | não portado | `sim/roads`, `sim/traffic` |
| `buildJunctionOf` | One junction built from what the page sent (`JunctionAsk`) | não portado | `sim/roads`, `sim/traffic` |
| `buildJunctionOf.lane` | — | não portado | `sim/roads`, `sim/traffic` |
| `buildJunctionOf.line` | — | não portado | `sim/roads`, `sim/traffic` |
| `buildJunctionOf.ring` | — | não portado | `sim/roads`, `sim/traffic` |
| `buildJunctionOf.surfaceOf` | — | não portado | `sim/roads`, `sim/traffic` |
| `carriedPair` | The two legs that are ONE ROAD carried across a node, when that road bends. | não portado | `sim/roads`, `sim/traffic` |
| `classifyTurn` | Classifies a movement from its heading change. | não portado | `sim/roads`, `sim/traffic` |
| `connectorId` | — | não portado | `sim/roads`, `sim/traffic` |
| `endDirection` | — | não portado | `sim/roads`, `sim/traffic` |
| `fallbackTurnRank` | — | não portado | `sim/roads`, `sim/traffic` |
| `isJunction` | Whether a node with these lanes is a junction at all: two roads meet, or a road ends in a turning circle; traffic arrives and leaves. | não portado | `sim/roads`, `sim/traffic` |
| `junctionAsk` | A junction's input and surface as a worker is sent them (`JunctionAsk`), with the turns of its build before. | não portado | `sim/roads`, `sim/traffic` |
| `junctionAsk.lane` | — | não portado | `sim/roads`, `sim/traffic` |
| `junctionAsk.leg` | — | não portado | `sim/roads`, `sim/traffic` |
| `junctionKey` | The key a junction's connectors are kept under, from its surface's key (`JunctionSurface.key`). | não portado | `sim/roads`, `sim/traffic` |
| `junctionMovements` | The connectors of one junction (`isJunction`), shaped across its surface | não portado | `sim/roads`, `sim/traffic` |
| `junctionMovements.addConnector` | — | não portado | `sim/roads`, `sim/traffic` |
| `junctionMovements.addConnector.shape` | — | não portado | `sim/roads`, `sim/traffic` |
| `junctionMovements.leg` | — | não portado | `sim/roads`, `sim/traffic` |
| `laneIsPlausible` | Whether a movement from this lane to that one is one a driver would make. | não portado | `sim/roads`, `sim/traffic` |
| `legLine` | A leg's centreline oriented to start at `from` (`geometry.ts` `orientedPolyline`, from the leg's own numbers). | não portado | `sim/roads`, `sim/traffic` |
| `shouldSignalise` | — | não portado | `sim/roads`, `sim/traffic` |
| `smoothedDirectionFromNode` | The direction of a leg once a kink at the node is smoothed away. | não portado | `sim/roads`, `sim/traffic` |
| `tangentAtStart` | The immediate tangent where a lane leaves its node. | não portado | `sim/roads`, `sim/traffic` |
| `turnSpeedFactor` | — | não portado | `sim/roads`, `sim/traffic` |

## `src/world/junctionTurns.ts` (inteiro, 84 linhas) — **não portado**

Dono: `sim/roads`, `sim/traffic`. Rede, faixas e campos do 4.0: parte tem dono equivalente no jogo, nada conferido declaração a declaração. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `JunctionTurns` | DO NOT go back to one memo for the whole map with a ceiling that empties it (`MEMO_LIMIT`, then `clear`) | não portado | `sim/roads`, `sim/traffic` |
| `JunctionTurns.constructor` | `before`: the same junction's turns of the build before, read where a movement is unchanged. | não portado | `sim/roads`, `sim/traffic` |
| `JunctionTurns.defer` | A turn left to be chosen off the page: what it is chosen from, asked once a key; the junction built again after. | não portado | `sim/roads`, `sim/traffic` |
| `JunctionTurns.deferred` | How many turns this build left to be chosen off the page (`defer`): a movement that left one is shaped again once chosen. | não portado | `sim/roads`, `sim/traffic` |
| `JunctionTurns.deferring` | Whether turns not known are left to be chosen off the page (`defer`) instead of here. | não portado | `sim/roads`, `sim/traffic` |
| `JunctionTurns.entries` | The turns it holds, in the order found: what a worker is sent and answers (`junctionBuild.ts`). | não portado | `sim/roads`, `sim/traffic` |
| `JunctionTurns.find` | The turn chosen for `key`: this build's, else the build before's, else the plan's (then this build's too). | não portado | `sim/roads`, `sim/traffic` |
| `JunctionTurns.keep` | — | não portado | `sim/roads`, `sim/traffic` |
| `JunctionTurns.made` | — | não portado | `sim/roads`, `sim/traffic` |
| `JunctionTurns.of` | A settled junction's turns from `entries` (a worker's answer, or what it is sent). | não portado | `sim/roads`, `sim/traffic` |
| `JunctionTurns.provisional` | The junctions built with a stand-in, and the turns they had. | não portado | `sim/roads`, `sim/traffic` |
| `JunctionTurns.settle` | Done building: the build before is let go, so what this one did not ask for goes with it. | não portado | `sim/roads`, `sim/traffic` |
| `TurnPlan` | DO NOT sweep a generated city's turns on the page: 25 s of one frame on the whole map (2026-10-15). | não portado | `sim/roads`, `sim/traffic` |
| `TurnPlan.asks` | — | não portado | `sim/roads`, `sim/traffic` |
| `TurnPlan.chosen` | — | não portado | `sim/roads`, `sim/traffic` |
| `TurnPlan.provisional` | The junctions built with a stand-in, and the turns they had. | não portado | `sim/roads`, `sim/traffic` |
| `TurnPlan.recording` | — | não portado | `sim/roads`, `sim/traffic` |
| `TurnPlan.surfaces` | Each junction surface the asks stand on, once a key. DO NOT copy its rings into every ask: a junction has dozens of turns. | não portado | `sim/roads`, `sim/traffic` |

## `src/world/landscape.ts` (parte, 259 linhas) — **não se aplica**

Dono: `landscape` (porte-3). Itens da paisagem são de `landscape`.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `SnapLocal` | — | não se aplica | `landscape` (porte-3) |
| `crossingAccessTester` | — | não se aplica | `landscape` (porte-3) |
| `crossingAccesses` | — | não se aplica | `landscape` (porte-3) |
| `crossingAccessesNow` | — | não se aplica | `landscape` (porte-3) |
| `depthFor` | — | não se aplica | `landscape` (porte-3) |
| `medianAt` | — | não se aplica | `landscape` (porte-3) |
| `medianNose` | — | não se aplica | `landscape` (porte-3) |
| `nearCrossingAccess` | — | não se aplica | `landscape` (porte-3) |
| `onAnyRoad` | — | não se aplica | `landscape` (porte-3) |
| `onCrossingAccess` | — | não se aplica | `landscape` (porte-3) |
| `plantGrowth` | — | não se aplica | `landscape` (porte-3) |
| `plantedMedian` | — | não se aplica | `landscape` (porte-3) |
| `snapLandscape` | — | não se aplica | `landscape` (porte-3) |
| `snapLandscape.others` | — | não se aplica | `landscape` (porte-3) |

## `src/world/lanelets.ts` (inteiro, 846 linhas) — **não portado**

Dono: `sim/roads`, `sim/traffic`. Rede, faixas e campos do 4.0: parte tem dono equivalente no jogo, nada conferido declaração a declaração. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `Connector` | — | não portado | `sim/roads`, `sim/traffic` |
| `ConnectorId` | — | não portado | `sim/roads`, `sim/traffic` |
| `GraphChange` | What one `LaneletGraph.update` built again: the junctions, the lanelets that came or went, the segments. | não portado | `sim/roads`, `sim/traffic` |
| `JunctionCache` | — | não portado | `sim/roads`, `sim/traffic` |
| `JunctionTopology` | — | não portado | `sim/roads`, `sim/traffic` |
| `Lanelet` | The atomic unit of vehicle space: a directed, single-lane centreline. | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph` | — | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.adopt` | DO NOT seed this graph's caches from `from` and `update` it | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.brought` | The graph now stands for the network as it is. | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.build` | — | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.buildJunction` | The connectors of one node (`buildJunctions`): kept whole when its key is known, built otherwise (`junctionBuild.ts`). | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.buildJunction.surfaceOf` | — | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.buildJunctions` | — | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.buildJunctions.before` | — | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.buildLinks` | — | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.buildSegmentLinks` | The link lanelets of one segment, both directions (`buildLinks`), under the keys they were built from. | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.buildSteps` | — | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.bySegment` | The link lanelets of each segment, both directions, in the order they were built (the order of `lanelets`). | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.changeTargets` | The lanes of the same carriageway a vehicle of this shape may change into from `id`: across dashed lines only | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.changes` | — | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.connectors` | — | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.current` | Whether this graph stands for the network as it is now, finished: no junction on stand-in turns (`adopt`). | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.exits` | Outgoing connectors of each link lanelet. | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.exitsOf` | — | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.findChangeTargets` | — | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.findSiblings` | — | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.forget` | The next `update` builds the whole graph (a world reset: what read the graph forgot it all). | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.inbound` | Inbound link lanelets, in deterministic order. | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.junctionCache` | What each junction built, by the surface and lanelets it was built from. | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.junctionConnectors` | @perInput The connectors of the junction at `node` ALONE, made as the whole graph's build makes them (`buildJunction`) | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.junctionInput` | What a junction's connectors are made of (`junctionBuild.ts`), read off the document and this graph's links | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.junctionInput.lane` | — | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.junctionKeyOf` | The key each junction's cache entry is under (`buildJunction`): found by its node, never by walking the cache. | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.junctions` | — | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.junctionsOffPage` | — | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.laneUsable` | Whether a vehicle of this shape may drive a lane (a bus lane is buses only). | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.lanelet` | — | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.laneletKeys` | The key each lanelet of this build was made under (`buildLinks`). | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.lanelets` | The connector lanelets and the connectors, in creation order. | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.letGo` | DO NOT gather every lanelet's key to find out: every edit read the whole map's. | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.linkCache` | The link lanelets of each (segment, direction), by the geometry and trims they were built from | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.linksOfSegment` | DO NOT walk `lanelets` to find one road's lanes (the inspector did, every 0.4 s, for the whole map). | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.linksOnly` | @rebuild The link lanelets of every segment ALONE (no junction), made as the whole graph's build makes them: what a tile's bays bake reads (`sim/tileB | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.netState` | The network's state this graph was last brought up to (`update`). | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.outbound` | Link lanelets leaving each node. | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.pending` | — | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.place` | A junction's movements (`junctionMovements`, here or in a worker) made this graph's | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.reach` | What a change of the network reaches (`update`, `adopt`): the segments to build again | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.redoProvisional` | Only those junctions: DO NOT build the graph again for them. | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.reshape` | — | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.revision` | — | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.siblingLanes` | The other lanes of the same carriageway, adjacent ones first. | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.siblings` | — | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.takeKnown` | The junction at `nodeId` handed back whole from the cache when its key is known (a junction an edit did not reach: objects and all). | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.takeOut` | DO NOT walk the junction cache to find them: every edit read every junction of the map. | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.takeWhole` | Every link, junction and connector of `from`, and its caches, as this graph's own (`adopt` of a graph built whole). | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.takeWhole.copy` | — | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.takeWhole.lists` | — | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.turnPlan` | The turns of the next build chosen off the page (`SimWorld.prepareVehicleTopology`) | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.update` | DO NOT build the whole graph for an edit | não portado | `sim/roads`, `sim/traffic` |
| `LaneletGraph.updateSteps` | — | não portado | `sim/roads`, `sim/traffic` |
| `LaneletId` | — | não portado | `sim/roads`, `sim/traffic` |
| `PendingJunction` | A junction built on turns still to be chosen off the page (`TurnPlan`), kept until they are | não portado | `sim/roads`, `sim/traffic` |
| `TakenOut` | What `LaneletGraph.takeOut` took out, for `letGo`: the lanelets that went, the junctions' entries and keys, the links' keys. | não portado | `sim/roads`, `sim/traffic` |
| `allowsDirection` | — | não portado | `sim/roads`, `sim/traffic` |
| `clipToEndSections` | A lane's centre between the cross-sections at its two ends. | não portado | `sim/roads`, `sim/traffic` |
| `drop` | `value` taken out of the list at `key`; the list gone when empty. | não portado | `sim/roads`, `sim/traffic` |
| `laneletId` | — | não portado | `sim/roads`, `sim/traffic` |
| `laneletWork` | The work of the lane graph's builds, counted (`tests/scale/edit.spec.ts`): segments and nodes looked at. | não portado | `sim/roads`, `sim/traffic` |
| `push` | — | não portado | `sim/roads`, `sim/traffic` |

## `src/world/legAngles.ts` (inteiro, 45 linhas) — **portado**

Dono: `sim/commands/road_plan.cpp sharp_at`. 25° em todo pouso

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `MIN_LEG_ANGLE` | How sharply two roads may meet at one node. | portado | `sim/commands/road_plan.cpp sharp_at` |
| `legAngles` | Outgoing directions of a node's legs, in radians, sorted. | portado | `sim/commands/road_plan.cpp sharp_at` |
| `smallestGap` | Smallest angle between any two of these legs, in radians. | portado | `sim/commands/road_plan.cpp sharp_at` |

## `src/world/lots.ts` (parte, 191 linhas) — **não se aplica**

Dono: `build` (porte-2). Lotes são de `build`.


## `src/world/markings.ts` (inteiro, 469 linhas) — **em parte**

Dono: `render/roads/road_markings.cpp`. Linhas e faixas. Setas (o jogo não define regra de conversão), legendas e dê a preferência não

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `Bar` | — | em parte | `render/roads/road_markings.cpp` |
| `DASH` | Dash pattern used for every broken line, in world units. | em parte | `render/roads/road_markings.cpp` |
| `DASH_PERIOD` | — | em parte | `render/roads/road_markings.cpp` |
| `LEGEND_EVERY` | — | em parte | `render/roads/road_markings.cpp` |
| `LEGEND_FIRST` | How far from each end of a stretch a bus lane's legend is first painted, and then every so far. | em parte | `render/roads/road_markings.cpp` |
| `LEGEND_GAP` | Clear floor between a stop line and the legend before it. | em parte | `render/roads/road_markings.cpp` |
| `StrokeSpec` | — | em parte | `render/roads/road_markings.cpp` |
| `ZEBRA_BAR` | Zebra bar width and gap (0.5 m each), and the clear margin kept at each kerb. | em parte | `render/roads/road_markings.cpp` |
| `ZEBRA_EDGE` | — | em parte | `render/roads/road_markings.cpp` |
| `ZEBRA_GAP` | — | em parte | `render/roads/road_markings.cpp` |
| `approachLegends` | What a junction's rule paints on each leg before its stop line (MBST vol. IV): "PARE" | em parte | `render/roads/road_markings.cpp` |
| `approachLegendsAt` | `approachLegends` of one node, onto `out`: what the surfaces paint of it again when an edit reaches it. | em parte | `render/roads/road_markings.cpp` |
| `busLegends` | The legend along a bus lane MBST vol. IV, 4.5.1 and 8.3: a legend may be painted along the | em parte | `render/roads/road_markings.cpp` |
| `crosswalkBars` | Zebra bars for one approach. | em parte | `render/roads/road_markings.cpp` |
| `junctionDetail` | Collects stop bars and crosswalks for every junction approach | em parte | `render/roads/road_markings.cpp` |
| `junctionDetailAt` | The stop bars and crosswalks of one junction's approaches, onto `stops` and `zebras` (`junctionDetail`). | em parte | `render/roads/road_markings.cpp` |
| `laneArrowMarkings` | Painted turn arrows follow the same authored rule as the connector graph. | em parte | `render/roads/road_markings.cpp` |
| `midBlockDetailAt` | Paint for the mid-block crossings (`RoadNode.crossing`). | em parte | `render/roads/road_markings.cpp` |
| `miniRoundaboutAt` | A mini-roundabout's painted island | em parte | `render/roads/road_markings.cpp` |
| `paintInLane` | — | em parte | `render/roads/road_markings.cpp` |
| `paintInLane.at` | — | em parte | `render/roads/road_markings.cpp` |
| `segmentMarkings` | Lane markings for one segment. | em parte | `render/roads/road_markings.cpp` |
| `segmentMarkings.centreLine` | — | em parte | `render/roads/road_markings.cpp` |
| `segmentMarkings.divider` | — | em parte | `render/roads/road_markings.cpp` |
| `stopBar` | Stop bar for one approach, spanning only the lanes entering the junction. | em parte | `render/roads/road_markings.cpp` |
| `transitionMarkings` | The paint across a taper where one road carries on at another width. | em parte | `render/roads/road_markings.cpp` |
| `transitionMarkings.mix` | — | em parte | `render/roads/road_markings.cpp` |
| `transitionMarkings.solid` | — | em parte | `render/roads/road_markings.cpp` |
| `zebraBars` | The stripes of one crosswalk centred `mid` along a frame given per distance. | em parte | `render/roads/road_markings.cpp` |
| `zebraBars.across` | — | em parte | `render/roads/road_markings.cpp` |

## `src/world/nearestRibbon.ts` (inteiro, 22 linhas) — **não portado**

Dono: `sim/roads`, `sim/traffic`. Rede, faixas e campos do 4.0: parte tem dono equivalente no jogo, nada conferido declaração a declaração. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `FIRST_REACH` | The first box searched round a point, u; each search after it four times as wide. | não portado | `sim/roads`, `sim/traffic` |
| `LAST_REACH` | Wider than any map: the last box holds every road. | não portado | `sim/roads`, `sim/traffic` |
| `nearestRibbon` | DO NOT walk every road of the map for the street nearest a point: every lot drawn did, for each of its sides. | não portado | `sim/roads`, `sim/traffic` |

## `src/world/network.ts` (inteiro, 381 linhas) — **não portado**

Dono: `sim/roads`, `sim/traffic`. Rede, faixas e campos do 4.0: parte tem dono equivalente no jogo, nada conferido declaração a declaração. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `CrossingLanding` | A crossing's landing on the footways (`Network.crossingLandingAt`): kept clear of everything. | não portado | `sim/roads`, `sim/traffic` |
| `Network` | — | não portado | `sim/roads`, `sim/traffic` |
| `Network.afterRebuild` | `follow` told every rebuild and take-back, in that step (what is kept from the network by its owner: the network never imports it). | não portado | `sim/roads`, `sim/traffic` |
| `Network.build` | The incremental build: what each piece was made from (`networkBuild.ts`). | não portado | `sim/roads`, `sim/traffic` |
| `Network.changesSince` | What changed since the network was in `state` (a token it handed out, `state`), or null: everything | não portado | `sim/roads`, `sim/traffic` |
| `Network.circulating` | Whether `seg` is one of two one-way pieces carrying one flow through a priority node. | não portado | `sim/roads`, `sim/traffic` |
| `Network.circulating.arrives` | — | não portado | `sim/roads`, `sim/traffic` |
| `Network.circulating.leaves` | — | não portado | `sim/roads`, `sim/traffic` |
| `Network.clear` | Forgets every derived piece: the next rebuild builds the whole map. | não portado | `sim/roads`, `sim/traffic` |
| `Network.constructor` | — | não portado | `sim/roads`, `sim/traffic` |
| `Network.continues` | Whether a node is one road carrying on rather than an intersection | não portado | `sim/roads`, `sim/traffic` |
| `Network.crossingDistances` | DO NOT clear it on every rebuild: every edit then fitted every crossing of the map again for whoever asked next. | não portado | `sim/roads`, `sim/traffic` |
| `Network.crossingLandingAt` | Where the crossing of `seg` at `node` lands on the footways | não portado | `sim/roads`, `sim/traffic` |
| `Network.crossingLandings` | DO NOT work a landing out again for each asker. | não portado | `sim/roads`, `sim/traffic` |
| `Network.crossingSurface` | The footway the crossings are fitted to, built once per network | não portado | `sim/roads`, `sim/traffic` |
| `Network.crossingWalkable` | The footway the crossings are fitted to, by cell (`walkable.ts`): kept across rebuilds. | não portado | `sim/roads`, `sim/traffic` |
| `Network.crosswalkDistanceAt` | — | não portado | `sim/roads`, `sim/traffic` |
| `Network.crosswalkDistanceAt.fits` | — | não portado | `sim/roads`, `sim/traffic` |
| `Network.crosswalkDistanceAt.keep` | — | não portado | `sim/roads`, `sim/traffic` |
| `Network.exportBake` | The junctions and ribbons of `nodes` and `segments` as this build solved them, for another thread's build (`NetworkBuild.exportBake`). | não portado | `sim/roads`, `sim/traffic` |
| `Network.flushCells` | The ribbons with a footway laid flush on a side, filed like `segmentCells` | não portado | `sim/roads`, `sim/traffic` |
| `Network.followers` | What follows the network in the step that rebuilds or takes it back (`afterRebuild`): the dropped kerbs (`curbRamps.ts`). | não portado | `sim/roads`, `sim/traffic` |
| `Network.forgetCrossings` | Drops the crossing distances a change of the network may have moved | não portado | `sim/roads`, `sim/traffic` |
| `Network.fork` | — | não portado | `sim/roads`, `sim/traffic` |
| `Network.forkedFrom` | The network this one is a working copy of (`fork`), its maps laid over that one's; null for a network of its own. | não portado | `sim/roads`, `sim/traffic` |
| `Network.impossible` | Nodes the editor would refuse to create, with the gap that condemns them, in radians. | não portado | `sim/roads`, `sim/traffic` |
| `Network.isBuilt` | Whether the network is rebuilt up to its document: its derived maps answer for what the document holds. | não portado | `sim/roads`, `sim/traffic` |
| `Network.isJunction` | — | não portado | `sim/roads`, `sim/traffic` |
| `Network.junctionAt` | — | não portado | `sim/roads`, `sim/traffic` |
| `Network.junctionCells` | — | não portado | `sim/roads`, `sim/traffic` |
| `Network.junctionNodes` | Nodes that carry a junction surface. | não portado | `sim/roads`, `sim/traffic` |
| `Network.junctions` | The junctions and ribbons by id, in the order the rebuilds filed them, never id order: a reader that needs id order sorts what it takes (`CellFiling.i | não portado | `sim/roads`, `sim/traffic` |
| `Network.log` | The states this network has been in and what changed between them (`changesSince`). | não portado | `sim/roads`, `sim/traffic` |
| `Network.midBlockCrossing` | The node's authored crossing, while the node still joins exactly two roads. | não portado | `sim/roads`, `sim/traffic` |
| `Network.mouthDistance` | Distance from a node to this segment's junction mouth, at asphalt level. | não portado | `sim/roads`, `sim/traffic` |
| `Network.offerBake` | A tile's junctions and ribbons solved off the page, taken by the next rebuild where the inputs are the same (`NetworkBuild.takeBakes`). | não portado | `sim/roads`, `sim/traffic` |
| `Network.plateReach` | How far along each leg a junction's flat plate reaches, keyed `node:segment` | não portado | `sim/roads`, `sim/traffic` |
| `Network.poleLines` | What the pole lines keep of this network (`poleLines.ts` `PoleLineState`) | não portado | `sim/roads`, `sim/traffic` |
| `Network.polylines` | — | não portado | `sim/roads`, `sim/traffic` |
| `Network.rebuild` | — | não portado | `sim/roads`, `sim/traffic` |
| `Network.revision` | Matches `RoadDoc.revision` at the time of the last rebuild. | não portado | `sim/roads`, `sim/traffic` |
| `Network.ribbons` | — | não portado | `sim/roads`, `sim/traffic` |
| `Network.ribbonsMeeting` | DO NOT go back to a rank of every ribbon made again at each revision (`ribbonOrder`) | não portado | `sim/roads`, `sim/traffic` |
| `Network.segmentCells` | The ribbons and the junctions filed by the cells their surfaces reach (`world/cells.ts`) | não portado | `sim/roads`, `sim/traffic` |
| `Network.squeezed` | Junctions whose legs are too short to separate their lanes, with how far | não portado | `sim/roads`, `sim/traffic` |
| `Network.state` | The current state's token, for `changesSince`. | não portado | `sim/roads`, `sim/traffic` |
| `Network.stopLineDistance` | Distance from a node at which vehicles must stop on the given segment. | não portado | `sim/roads`, `sim/traffic` |
| `Network.takeBack` | A working network (`fork`) taken back once its document was (`RoadDoc.takeBack`): only what it wrote, then its state. | não portado | `sim/roads`, `sim/traffic` |
| `Network.trafficRevision` | Road plan revision; vertical-only edits leave it unchanged. | não portado | `sim/roads`, `sim/traffic` |
| `Network.trafficToken` | The road plan as a token for the simulation's frame bookkeeping: the value, never a product key. | não portado | `sim/roads`, `sim/traffic` |
| `Network.transitions` | Nodes where one road carries on at another width, built as a taper rather than as a junction (`junction/transition.ts`). | não portado | `sim/roads`, `sim/traffic` |
| `Network.trims` | — | não portado | `sim/roads`, `sim/traffic` |

## `src/world/networkBuild.ts` (inteiro, 798 linhas) — **não portado**

Dono: `sim/roads`, `sim/traffic`. Rede, faixas e campos do 4.0: parte tem dono equivalente no jogo, nada conferido declaração a declaração. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `BakedMemo` | A pass's junctions as their data (`JunctionLevels.data`: the carriageway, the frames its outer levels are made from, what is kept whole; null: let go, | não portado | `sim/roads`, `sim/traffic` |
| `BakedNode` | — | não portado | `sim/roads`, `sim/traffic` |
| `BakedRibbon` | A ribbon as what it is made from (`ribbons.ts` LeanRibbon): its road, its whole centreline as its flat array (`Polyline.xy`) and its box; its levels m | não portado | `sim/roads`, `sim/traffic` |
| `BakedSegment` | — | não portado | `sim/roads`, `sim/traffic` |
| `MemoEntry` | A junction solved for one pass, with what the pass handed it (`junctionKey`), the plate it reaches, how many levels it has and what the reconciling of | não portado | `sim/roads`, `sim/traffic` |
| `Network` | What the build writes: the network's own derived maps | não portado | `sim/roads`, `sim/traffic` |
| `NetworkBake` | A TILE'S JUNCTIONS AND RIBBONS SOLVED OFF THE PAGE (`editor/tileStore.ts` prepareTile, the save worker or the generator's): each node's passes with th | não portado | `sim/roads`, `sim/traffic` |
| `NetworkBuild` | THE NETWORK'S BUILD, INCREMENTAL | não portado | `sim/roads`, `sim/traffic` |
| `NetworkBuild.active` | — | não portado | `sim/roads`, `sim/traffic` |
| `NetworkBuild.addEnds` | — | não portado | `sim/roads`, `sim/traffic` |
| `NetworkBuild.bakes` | The tiles' bakes offered since the last rebuild (`offerBake`), taken by it (`takeBakes`). | não portado | `sim/roads`, `sim/traffic` |
| `NetworkBuild.buildRibbon` | A road's ribbon: the one it had when its road, its trims and the bends at its ends are the same. | não portado | `sim/roads`, `sim/traffic` |
| `NetworkBuild.constructor` | — | não portado | `sim/roads`, `sim/traffic` |
| `NetworkBuild.empty` | Whether a full rebuild must follow (never built, or forgotten). | não portado | `sim/roads`, `sim/traffic` |
| `NetworkBuild.entry1` | The junction a node has after each pass (null: none): the last pass that built one. | não portado | `sim/roads`, `sim/traffic` |
| `NetworkBuild.entry2` | — | não portado | `sim/roads`, `sim/traffic` |
| `NetworkBuild.entry3` | — | não portado | `sim/roads`, `sim/traffic` |
| `NetworkBuild.exportBake` | The junctions and ribbons of `nodes` and `segments` as this build solved them, for another thread's build (`NetworkBake`). | não portado | `sim/roads`, `sim/traffic` |
| `NetworkBuild.finalTrims` | The reconciled trims lowered to what the final junctions actually used | não portado | `sim/roads`, `sim/traffic` |
| `NetworkBuild.findChanges` | The segments and nodes whose sources differ from what the last rebuild recorded, and the nodes whose record does | não portado | `sim/roads`, `sim/traffic` |
| `NetworkBuild.forget` | Forgets everything: the next rebuild builds every piece (a map loaded). | não portado | `sim/roads`, `sim/traffic` |
| `NetworkBuild.forkFrom` | DO NOT copy every map (`copyFrom`, twice per road drawn): a working copy over `other`'s state that keeps only what it writes | não portado | `sim/roads`, `sim/traffic` |
| `NetworkBuild.j1` | — | não portado | `sim/roads`, `sim/traffic` |
| `NetworkBuild.j2` | — | não portado | `sim/roads`, `sim/traffic` |
| `NetworkBuild.j3` | — | não portado | `sim/roads`, `sim/traffic` |
| `NetworkBuild.junctionKey` | What a pass hands a node's junction beyond the node and its roads | não portado | `sim/roads`, `sim/traffic` |
| `NetworkBuild.letGoEarlierPasses` | The passes' junctions a later pass of the node solved again, let go (their summaries and plates kept, `MemoEntry`). | não portado | `sim/roads`, `sim/traffic` |
| `NetworkBuild.levels3` | The junction a node has after the last pass, whole: a pass let go and fallen back to solved again, the same (its inputs are the ones it was solved fro | não portado | `sim/roads`, `sim/traffic` |
| `NetworkBuild.nodeIncident` | — | não portado | `sim/roads`, `sim/traffic` |
| `NetworkBuild.nodePlaces` | — | não portado | `sim/roads`, `sim/traffic` |
| `NetworkBuild.nodeRecord` | — | não portado | `sim/roads`, `sim/traffic` |
| `NetworkBuild.nodeSource` | — | não portado | `sim/roads`, `sim/traffic` |
| `NetworkBuild.offerBake` | A tile's junctions and ribbons solved off the page, for the next rebuild (the tile's records just loaded). | não portado | `sim/roads`, `sim/traffic` |
| `NetworkBuild.plateKeys` | The plate keys each road wrote (`node:segment`), to take them out when it changes. | não portado | `sim/roads`, `sim/traffic` |
| `NetworkBuild.plateRaw` | — | não portado | `sim/roads`, `sim/traffic` |
| `NetworkBuild.pressured` | — | não portado | `sim/roads`, `sim/traffic` |
| `NetworkBuild.rebuild` | @rebuild Brings the network up to the document; returns what changed (`null`: everything, a build from nothing). | não portado | `sim/roads`, `sim/traffic` |
| `NetworkBuild.reconcileSegment` | One segment's two ends reconciled against its length, per level, from the junctions at its ends | não portado | `sim/roads`, `sim/traffic` |
| `NetworkBuild.ribbonMemo` | — | não portado | `sim/roads`, `sim/traffic` |
| `NetworkBuild.scale1` | — | não portado | `sim/roads`, `sim/traffic` |
| `NetworkBuild.scale2` | — | não portado | `sim/roads`, `sim/traffic` |
| `NetworkBuild.seen` | The diary's serial the last rebuild read up to; -1: never built. | não portado | `sim/roads`, `sim/traffic` |
| `NetworkBuild.segmentEnds` | — | não portado | `sim/roads`, `sim/traffic` |
| `NetworkBuild.segmentSource` | — | não portado | `sim/roads`, `sim/traffic` |
| `NetworkBuild.solveNode` | One node's junction for one pass: the entry made for the same key before when there is one | não portado | `sim/roads`, `sim/traffic` |
| `NetworkBuild.solveNode.build` | — | não portado | `sim/roads`, `sim/traffic` |
| `NetworkBuild.squeeze` | How far the worst mouth of a junction is cut short of where two of its legs' carriageways stop overlapping | não portado | `sim/roads`, `sim/traffic` |
| `NetworkBuild.takeBack` | A working copy's state (`forkFrom`) written into this one, up to its document as it stands now: only what the copy wrote. | não portado | `sim/roads`, `sim/traffic` |
| `NetworkBuild.takeBakes` | The bakes offered: a changed node whose inputs are a bake's takes its passes as solved there (DO NOT solve a tile's thousand junctions on the page at  | não portado | `sim/roads`, `sim/traffic` |
| `NetworkBuild.trimCapsAt` | Final per-leg caps for one junction after segment-end reconciliation. | não portado | `sim/roads`, `sim/traffic` |
| `NetworkBuild.trims2` | — | não portado | `sim/roads`, `sim/traffic` |
| `NetworkBuild.writePlate` | A road's plate reaches (`Network.plateReach`), reconciled against its length the way the trims are | não portado | `sim/roads`, `sim/traffic` |
| `RibbonMemo` | — | não portado | `sim/roads`, `sim/traffic` |
| `SQUEEZE_NOISE` | A mouth cut this little short of where its carriageway separates from the neighbouring leg's is the solver's own | não portado | `sim/roads`, `sim/traffic` |
| `memoData` | — | não portado | `sim/roads`, `sim/traffic` |
| `memoEntry` | — | não portado | `sim/roads`, `sim/traffic` |
| `memoOf` | — | não portado | `sim/roads`, `sim/traffic` |
| `nodeBakeKey` | What a node's junctions are solved from, whichever thread solves them: its record and its roads' (where their ends stand too), the roads in id order ( | não portado | `sim/roads`, `sim/traffic` |
| `nodeRecordKey` | The whole record of a node: what the traffic reads of it besides the geometry (lane links, signal settings, approach rules). | não portado | `sim/roads`, `sim/traffic` |
| `nodeSourceDigest` | — | não portado | `sim/roads`, `sim/traffic` |
| `nodeSourceKey` | Everything of a node a junction there is built from: its place, height, roads by id, control, crossing and end. | não portado | `sim/roads`, `sim/traffic` |
| `pointRect` | A point's rectangle, for filing a node by where it stands. | não portado | `sim/roads`, `sim/traffic` |
| `ribbonFromBake` | — | não portado | `sim/roads`, `sim/traffic` |
| `samePlate` | — | não portado | `sim/roads`, `sim/traffic` |
| `segmentSourceKey` | Everything a segment's polyline, ribbon and junction legs are built from: its record and where its two ends stand. | não portado | `sim/roads`, `sim/traffic` |

## `src/world/networkDelta.ts` (inteiro, 123 linhas) — **não portado**

Dono: `sim/roads`, `sim/traffic`. Rede, faixas e campos do 4.0: parte tem dono equivalente no jogo, nada conferido declaração a declaração. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `DeltaEdge` | One rebuild, or several merged: from one state of the network to another, and what changed; `null`, everything | não portado | `sim/roads`, `sim/traffic` |
| `DeltaLog` | DO NOT keep the last N rebuilds only | não portado | `sim/roads`, `sim/traffic` |
| `DeltaLog.advance` | Records a rebuild that moved the network to a new state. | não portado | `sim/roads`, `sim/traffic` |
| `DeltaLog.edges` | — | não portado | `sim/roads`, `sim/traffic` |
| `DeltaLog.merge` | Merges away the states no reader holds: collected, or not `held` | não portado | `sim/roads`, `sim/traffic` |
| `DeltaLog.merge.alive` | — | não portado | `sim/roads`, `sim/traffic` |
| `DeltaLog.merged` | How long the history was after its last merge (`advance`). | não portado | `sim/roads`, `sim/traffic` |
| `DeltaLog.since` | Everything that changed between the state `from` and the current one, or `null` when that is not known | não portado | `sim/roads`, `sim/traffic` |
| `DeltaLog.state` | The current state. A fresh object per rebuild: only its identity is read. | não portado | `sim/roads`, `sim/traffic` |
| `DeltaLog.stateRef` | The current state's one reference: every edge touching a state shares it, so a state gone is still known as one. | não portado | `sim/roads`, `sim/traffic` |
| `DeltaLog.takeFrom` | Takes another log's current state and every rebuild it remembers (`Network.adopt`). | não portado | `sim/roads`, `sim/traffic` |
| `MERGE_FROM` | Edges at which the history is first merged; past it, again whenever it has doubled since. | não portado | `sim/roads`, `sim/traffic` |
| `NetworkDelta` | What one rebuild of the network changed (`Network.rebuild`) | não portado | `sim/roads`, `sim/traffic` |
| `union` | Two changes as one: every road, node and rectangle of both; `null` (everything) if either is. | não portado | `sim/roads`, `sim/traffic` |

## `src/world/networkLength.ts` (inteiro, 45 linhas) — **não portado**

Dono: `sim/roads`, `sim/traffic`. Rede, faixas e campos do 4.0: parte tem dono equivalente no jogo, nada conferido declaração a declaração. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `NetworkLength` | DO NOT sum every ribbon of the map at each revision: every road drawn walked the town's roads. | não portado | `sim/roads`, `sim/traffic` |
| `NetworkLength.bySegment` | — | não portado | `sim/roads`, `sim/traffic` |
| `NetworkLength.net` | — | não portado | `sim/roads`, `sim/traffic` |
| `NetworkLength.of` | — | não portado | `sim/roads`, `sim/traffic` |
| `NetworkLength.put` | — | não portado | `sim/roads`, `sim/traffic` |
| `NetworkLength.seen` | — | não portado | `sim/roads`, `sim/traffic` |
| `NetworkLength.sumAll` | @rebuild Every road summed again. | não portado | `sim/roads`, `sim/traffic` |
| `NetworkLength.total` | — | não portado | `sim/roads`, `sim/traffic` |
| `SCALE` | Lengths are summed in millionths of a unit, as integers: exact, in whatever order an edit adds and takes them. | não portado | `sim/roads`, `sim/traffic` |

## `src/world/parking.ts` (inteiro, 70 linhas) — **portado**

Dono: `sim/roads/road_variants.cpp`. Vaga como faixa da seção, 2 m; desenho das vagas é de `sim/traffic` e do porte de tráfego.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `CORNER_CLEAR` | No bay within this distance of a junction's kerb line (CTB art. 181 I). | portado | `sim/roads/road_variants.cpp` |
| `CROSSING_CLEAR` | No bay within this distance of a pedestrian crossing. | portado | `sim/roads/road_variants.cpp` |
| `PARKING_DEPTH` | Depth of a parking lane from the kerb face, world units. | portado | `sim/roads/road_variants.cpp` |
| `PARKING_KINDS` | A band by the kerb is parking (`parallel`) or a CYCLE LANE (`cycle`): the ciclofaixa of the Brazilian code | portado | `sim/roads/road_variants.cpp` |
| `PARKING_PITCH` | Distance along the kerb between bay lines, world units. | portado | `sim/roads/road_variants.cpp` |
| `ParkingKind` | — | portado | `sim/roads/road_variants.cpp` |
| `SegmentParking` | Parking on the two sides of a segment, left and right of its a -> b direction. | portado | `sim/roads/road_variants.cpp` |
| `flipParking` | The same parking seen from the other end of the segment. | portado | `sim/roads/road_variants.cpp` |
| `isParkingKind` | — | portado | `sim/roads/road_variants.cpp` |
| `normalizeParking` | Reads a stored value, or undefined when there is no parking at all. | portado | `sim/roads/road_variants.cpp` |
| `parkingAllowed` | Roads without footways (highways, ramps) do not park at all; every street does. | portado | `sim/roads/road_variants.cpp` |
| `readKind` | A stored kind: the kinds across the kerb an earlier build offered park parallel now. | portado | `sim/roads/road_variants.cpp` |
| `sameParking` | — | portado | `sim/roads/road_variants.cpp` |

## `src/world/parkingLayout.ts` (inteiro, 258 linhas) — **não portado**

Dono: `sim/roads`, `sim/traffic`. Rede, faixas e campos do 4.0: parte tem dono equivalente no jogo, nada conferido declaração a declaração. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `BAY_LINE` | — | não portado | `sim/roads`, `sim/traffic` |
| `CYCLE_RED` | The red of a ciclofaixa's paving. | não portado | `sim/roads`, `sim/traffic` |
| `ENTRANCE_MARGIN` | Kept clear either side of a lot's frontage for its entrance. | não portado | `sim/roads`, `sim/traffic` |
| `MER_DASH` | The MER's dashes along the parking lane: 1 m of paint, 1 m clear. | não portado | `sim/roads`, `sim/traffic` |
| `NO_PARKING_LINE` | — | não portado | `sim/roads`, `sim/traffic` |
| `PARKING_PAINT` | Every colour the parking's lines are painted in (`render/markings.ts` `PAINT_COLOURS`). | não portado | `sim/roads`, `sim/traffic` |
| `ParkingBay` | — | não portado | `sim/roads`, `sim/traffic` |
| `ParkingLayout` | — | não portado | `sim/roads`, `sim/traffic` |
| `ParkingLine` | — | não portado | `sim/roads`, `sim/traffic` |
| `RUN_STEP` | Step of the paint along a run, so a line follows a curve. | não portado | `sim/roads`, `sim/traffic` |
| `STRAIGHT` | A point of a run's line nearer than this to the line through its neighbours carries no shape: left out. | não portado | `sim/roads`, `sim/traffic` |
| `WHITE` | — | não portado | `sim/roads`, `sim/traffic` |
| `YELLOW` | — | não portado | `sim/roads`, `sim/traffic` |
| `endClear` | How far from a node no bay may start: the corner clearance, or clear of the crossing. | não portado | `sim/roads`, `sim/traffic` |
| `entrancesNear` | The entrances of the buildings within reach of a street (`frontagesOn`'s reach). | não portado | `sim/roads`, `sim/traffic` |
| `frontagesOn` | Intervals along `line` in front of a lot entrance on one side. | não portado | `sim/roads`, `sim/traffic` |
| `gatesNear` | The entrances of `gates` (`gatesOf`) a street of half width `half` along `line` may read: those `entrancesNear` would hand it, for a road bake without | não portado | `sim/roads`, `sim/traffic` |
| `parkingDigest` | DO NOT read anything in `parkingOf` without adding it here: what one street's bays are laid out from besides the entrances it is handed, into `digest` | não portado | `sim/roads`, `sim/traffic` |
| `parkingLayout` | DO NOT hand every entrance of the map to every street: each street with parking tested every gate of the city. | não portado | `sim/roads`, `sim/traffic` |
| `parkingLinesOf` | The paint of one street's parking (`parkingLayout`); `entrances`: the gates within its reach (`gatesNear`), read from the document's buildings when no | não portado | `sim/roads`, `sim/traffic` |
| `parkingOf` | — | não portado | `sim/roads`, `sim/traffic` |
| `parksAlong` | Whether a street has parking laid out at all (`parkingOf`): only then do the buildings near it move its paint. | não portado | `sim/roads`, `sim/traffic` |
| `stations` | Stations from `lo` to `hi`, one a metre and both ends. | não portado | `sim/roads`, `sim/traffic` |
| `subtract` | `from` with every interval of `cut` removed. | não portado | `sim/roads`, `sim/traffic` |

## `src/world/paving.ts` (inteiro, 64 linhas) — **não portado**

Dono: `sim/roads`, `sim/traffic`. Rede, faixas e campos do 4.0: parte tem dono equivalente no jogo, nada conferido declaração a declaração. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `PavingData` | What the paving reads near a rectangle, to hand a worker: the solved profiles there | não portado | `sim/roads`, `sim/traffic` |
| `PavingSource` | The paving's height as the page reads it, and what to hand a worker for a rectangle of it. | não portado | `sim/roads`, `sim/traffic` |
| `SAMPLE` | The road read under a point, kept (`roadAtInto`): DO NOT make a sample per read, a building's floor reads thousands (2026-10-16: 70 KB of garbage a fl | não portado | `sim/roads`, `sim/traffic` |
| `pavedHeightFrom` | The paving's height at (x, y) over a field of roads at grade; `riseAt` the footway's rise there. | não portado | `sim/roads`, `sim/traffic` |
| `pavingReader` | The paving's height over `PavingData`; undefined outside its rectangle, or where the field needs the ground and `naturalAt` is not given | não portado | `sim/roads`, `sim/traffic` |
| `pavingReader.inside` | — | não portado | `sim/roads`, `sim/traffic` |
| `pavingReader.riseAt` | — | não portado | `sim/roads`, `sim/traffic` |

## `src/world/pedestrianAccess.ts` (parte, 3 linhas) — **não portado**

Dono: `sim/roads`, `sim/traffic`. Rede, faixas e campos do 4.0: parte tem dono equivalente no jogo, nada conferido declaração a declaração. Classificado por pasta: não li cada declaração.


## `src/world/poleLines.ts` (parte, 187 linhas) — **não se aplica**

Dono: `street-furniture` (porte-6). Linhas de postes são de `street-furniture`.


## `src/world/rampRise.ts` (inteiro, 95 linhas) — **não se aplica**

Dono: funcionalidade `road-elevation`. Depende da altura solucionada das vias.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `CurbRamp` | A dropped kerb: at each end of every zebra the footway ramps down to the carriageway (`curbRamps.ts`). | não se aplica | funcionalidade `road-elevation` |
| `CurbRampData` | A ramp as plain data, every field its own (`rampData`): what a message's clone keeps of it (a road tile's job, `roadTileMesh.ts`). | não se aplica | funcionalidade `road-elevation` |
| `PackedRamp` | DO NOT keep a ramp as an object of nine boxed numbers (~170 B a ramp, two a zebra, 2026-10-21): its numbers in one run, read through getters (a class: | não se aplica | funcionalidade `road-elevation` |
| `PackedRamp.constructor` | — | não se aplica | funcionalidade `road-elevation` |
| `PackedRamp.flare` | Length of each flared side along the street. | não se aplica | funcionalidade `road-elevation` |
| `PackedRamp.half` | Half the ramp's width along the street (half the zebra's). | não se aplica | funcionalidade `road-elevation` |
| `PackedRamp.nx` | Across the street, from the carriageway into the footway. | não se aplica | funcionalidade `road-elevation` |
| `PackedRamp.ny` | — | não se aplica | funcionalidade `road-elevation` |
| `PackedRamp.run` | How far into the footway the slope runs, to the footway's full height. | não se aplica | funcionalidade `road-elevation` |
| `PackedRamp.tx` | Along the street. | não se aplica | funcionalidade `road-elevation` |
| `PackedRamp.ty` | — | não se aplica | funcionalidade `road-elevation` |
| `PackedRamp.x` | The middle of the ramp's foot, on the kerb face (the carriageway's edge). | não se aplica | funcionalidade `road-elevation` |
| `PackedRamp.y` | — | não se aplica | funcionalidade `road-elevation` |
| `RAMP_CELL` | Bucket size of the lookup grid. | não se aplica | funcionalidade `road-elevation` |
| `ROAD_REACH` | How far onto the carriageway a ramp still answers, so a walker's step off it is level. | não se aplica | funcionalidade `road-elevation` |
| `rampCellKey` | A grid cell's key as a number: `walkingRise` is asked for every walker drawn every frame | não se aplica | funcionalidade `road-elevation` |
| `rampCells` | Calls `visit` with the key of every cell a ramp reaches. | não se aplica | funcionalidade `road-elevation` |
| `rampData` | A ramp's fields as plain data (`CurbRampData`). | não se aplica | funcionalidade `road-elevation` |
| `rampGrid` | The ramps filed by the cells they reach. | não se aplica | funcionalidade `road-elevation` |
| `rampOf` | A ramp kept compact (`PackedRamp`) from its fields. | não se aplica | funcionalidade `road-elevation` |
| `rampRiseIn` | The walking surface's share of the footway's rise at (x, y), from the ramps filed in its cell of `grid` (`walkingRise`). | não se aplica | funcionalidade `road-elevation` |

## `src/world/ribbons.ts` (inteiro, 212 linhas) — **não portado**

Dono: `sim/roads`, `sim/traffic`. Rede, faixas e campos do 4.0: parte tem dono equivalente no jogo, nada conferido declaração a declaração. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `HELD_RIBBONS` | How many ribbons' levels made again are held (`LeanRibbon`): the roads of the cells being drawn and walked. | não portado | `sim/roads`, `sim/traffic` |
| `LeanRibbon` | A ROAD'S RIBBON KEPT SMALL: its road, its whole centreline, its trims and the bends at its ends, and the box of everything it puts on any level (`ribb | não portado | `sim/roads`, `sim/traffic` |
| `LeanRibbon.centre` | Centreline trimmed for this level, in world space. | não portado | `sim/roads`, `sim/traffic` |
| `LeanRibbon.constructor` | — | não portado | `sim/roads`, `sim/traffic` |
| `LeanRibbon.rings` | Closed carriageway outline for this level. | não portado | `sim/roads`, `sim/traffic` |
| `RibbonParts` | — | não portado | `sim/roads`, `sim/traffic` |
| `SegmentRibbon` | — | não portado | `sim/roads`, `sim/traffic` |
| `SegmentTrims` | Per-level trim distances at both ends of a segment. | não portado | `sim/roads`, `sim/traffic` |
| `bendAt` | How far the road turns at a node joining exactly two segments, radians (0 straight on); 0 anywhere else. | não portado | `sim/roads`, `sim/traffic` |
| `bendAt.away` | — | não portado | `sim/roads`, `sim/traffic` |
| `boundsOf` | — | não portado | `sim/roads`, `sim/traffic` |
| `junctionBounds` | Every ring a node's junctions put on any level; null for none. | não portado | `sim/roads`, `sim/traffic` |
| `leanRibbon` | A ribbon kept small (`LeanRibbon`) from its fields; `bounds` worked out from its parts when not given. | não portado | `sim/roads`, `sim/traffic` |
| `madeParts` | The ribbons' levels made again (`LeanRibbon`), the latest asked held. | não portado | `sim/roads`, `sim/traffic` |
| `makeRibbon` | A road's ribbon from its road, its trims and the bends at its two ends. | não portado | `sim/roads`, `sim/traffic` |
| `overlapUntrimmedEnds` | — | não portado | `sim/roads`, `sim/traffic` |
| `overlapUntrimmedEnds.push` | — | não portado | `sim/roads`, `sim/traffic` |
| `partsOf` | — | não portado | `sim/roads`, `sim/traffic` |
| `ribbonBounds` | Everything a ribbon puts on any level (`surfaces.ts` levelRings) | não portado | `sim/roads`, `sim/traffic` |
| `ribbonParts` | Each level's trimmed centreline and outline (`makeRibbon`). | não portado | `sim/roads`, `sim/traffic` |
| `ribbonRing` | Closes a trimmed centreline into a carriageway outline | não portado | `sim/roads`, `sim/traffic` |

## `src/world/roadField.ts` (inteiro, 751 linhas) — **não portado**

Dono: `sim/roads`, `sim/traffic`. Rede, faixas e campos do 4.0: parte tem dono equivalente no jogo, nada conferido declaração a declaração. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `BLEND_SKIP` | A road this much farther than the nearest one weighs under the blend's 1e-4 cut for sure (exp(-9.26) < 9.6e-5) | não portado | `sim/roads`, `sim/traffic` |
| `BLEND_TAU` | Softness of the blend between neighbouring profiles, in world units. | não portado | `sim/roads`, `sim/traffic` |
| `BOX_SLACK` | Rounding between a box's distance and a line's (`closestInto` measures it its own way): a road this close to the cut is measured. | não portado | `sim/roads`, `sim/traffic` |
| `EMPTY` | — | não portado | `sim/roads`, `sim/traffic` |
| `FieldData` | The profiles a batch of road tiles reads, as typed arrays handed to a worker (MDN, "Transferable objects") | não portado | `sim/roads`, `sim/traffic` |
| `FieldProfile` | What a query reads of one road's solved profile. | não portado | `sim/roads`, `sim/traffic` |
| `HashScratch` | A coordinate's bits, to hash it: one per table. | não portado | `sim/roads`, `sim/traffic` |
| `INDEX_CELL` | Side of the spatial index's cells, world units. | não portado | `sim/roads`, `sim/traffic` |
| `NearIndex` | What the field's reads ask of an index: the roads a point may read. | não portado | `sim/roads`, `sim/traffic` |
| `OVERSIZED_CELLS` | A box over more cells than this is filed apart, read by every query (a road across the whole map would fill the grid). | não portado | `sim/roads`, `sim/traffic` |
| `PROFILE_FADE` | How far out the road's influence fades to nothing. | não portado | `sim/roads`, `sim/traffic` |
| `PROFILE_REACH` | How far from a road its own profile is the whole answer. | não portado | `sim/roads`, `sim/traffic` |
| `PointHeights` | Heights of the ground at exact points, handed over by the page (a road tile worker's terrain, `render/roadTileMesh.ts`) | não portado | `sim/roads`, `sim/traffic` |
| `PointHeights.add` | Whether (x, y) is held; adds it with `height` when it is not. | não portado | `sim/roads`, `sim/traffic` |
| `PointHeights.count` | — | não portado | `sim/roads`, `sim/traffic` |
| `PointHeights.get` | The height at (x, y), or undefined when none was given. | não portado | `sim/roads`, `sim/traffic` |
| `PointHeights.hash` | — | não portado | `sim/roads`, `sim/traffic` |
| `PointHeights.hs` | — | não portado | `sim/roads`, `sim/traffic` |
| `PointHeights.size` | — | não portado | `sim/roads`, `sim/traffic` |
| `PointHeights.slots` | — | não portado | `sim/roads`, `sim/traffic` |
| `PointHeights.xs` | — | não portado | `sim/roads`, `sim/traffic` |
| `PointHeights.ys` | — | não portado | `sim/roads`, `sim/traffic` |
| `PointMemo` | What the height field found at each point of one piece of work | não portado | `sim/roads`, `sim/traffic` |
| `PointMemo.clear` | Forgets every point; keeps the arrays, sized for the largest piece of work so far. | não portado | `sim/roads`, `sim/traffic` |
| `PointMemo.distance` | — | não portado | `sim/roads`, `sim/traffic` |
| `PointMemo.find` | The entry of (x, y), or -1. | não portado | `sim/roads`, `sim/traffic` |
| `PointMemo.grow` | — | não portado | `sim/roads`, `sim/traffic` |
| `PointMemo.growEntries` | — | não portado | `sim/roads`, `sim/traffic` |
| `PointMemo.growEntries.f64` | — | não portado | `sim/roads`, `sim/traffic` |
| `PointMemo.hash` | — | não portado | `sim/roads`, `sim/traffic` |
| `PointMemo.height` | — | não portado | `sim/roads`, `sim/traffic` |
| `PointMemo.manual` | — | não portado | `sim/roads`, `sim/traffic` |
| `PointMemo.put` | — | não portado | `sim/roads`, `sim/traffic` |
| `PointMemo.roads` | — | não portado | `sim/roads`, `sim/traffic` |
| `PointMemo.s` | — | não portado | `sim/roads`, `sim/traffic` |
| `PointMemo.size` | — | não portado | `sim/roads`, `sim/traffic` |
| `PointMemo.slots` | — | não portado | `sim/roads`, `sim/traffic` |
| `PointMemo.structures` | — | não portado | `sim/roads`, `sim/traffic` |
| `PointMemo.xs` | — | não portado | `sim/roads`, `sim/traffic` |
| `PointMemo.ys` | — | não portado | `sim/roads`, `sim/traffic` |
| `RoadReads` | The reads of the field a road surface makes per vertex (`render/roadSurfaceReads.ts`). | não portado | `sim/roads`, `sim/traffic` |
| `RoadSample` | — | não portado | `sim/roads`, `sim/traffic` |
| `RoadSampleOut` | `RoadSample` as `roadAtInto` writes it, one kept by its reader: every field written at each read. | não portado | `sim/roads`, `sim/traffic` |
| `STRIDE` | Figures per profile in `FieldData.numbers`. | não portado | `sim/roads`, `sim/traffic` |
| `SpatialIndex` | A uniform grid over the segments' influence boxes. | não portado | `sim/roads`, `sim/traffic` |
| `SpatialIndex.all` | — | não portado | `sim/roads`, `sim/traffic` |
| `SpatialIndex.buckets` | — | não portado | `sim/roads`, `sim/traffic` |
| `SpatialIndex.cell` | — | não portado | `sim/roads`, `sim/traffic` |
| `SpatialIndex.constructor` | — | não portado | `sim/roads`, `sim/traffic` |
| `SpatialIndex.digest` | Digest of every profile `near` can return for a point of the rectangle, in its order; a profile `of` gives null for is left out. | não portado | `sim/roads`, `sim/traffic` |
| `SpatialIndex.isOversized` | By a set: asked for every profile of the build (`profileSummaries`). | não portado | `sim/roads`, `sim/traffic` |
| `SpatialIndex.joined` | — | não portado | `sim/roads`, `sim/traffic` |
| `SpatialIndex.mark` | — | não portado | `sim/roads`, `sim/traffic` |
| `SpatialIndex.marks` | — | não portado | `sim/roads`, `sim/traffic` |
| `SpatialIndex.meeting` | Every profile `near` can return for a point of the rectangles, and `also`, in the order the index was built from | não portado | `sim/roads`, `sim/traffic` |
| `SpatialIndex.meeting.take` | — | não portado | `sim/roads`, `sim/traffic` |
| `SpatialIndex.near` | — | não portado | `sim/roads`, `sim/traffic` |
| `SpatialIndex.oversized` | — | não portado | `sim/roads`, `sim/traffic` |
| `SpatialIndex.oversizedSet` | — | não portado | `sim/roads`, `sim/traffic` |
| `SpatialIndex.ranks` | Each profile's place in the list the index was built from, and a mark per place (`meeting`). | não portado | `sim/roads`, `sim/traffic` |
| `boxDistance` | The distance from a point to a box (0 inside it): never more than to anything in the box. | não portado | `sim/roads`, `sim/traffic` |
| `cellKeyOf` | — | não portado | `sim/roads`, `sim/traffic` |
| `fieldOf` | The field over `FieldData`, answering for its cells alone. | não portado | `sim/roads`, `sim/traffic` |
| `hashScratch` | — | não portado | `sim/roads`, `sim/traffic` |
| `heightAtArc` | Linear read of a profile at an arc position — continuous, never stepped. | não portado | `sim/roads`, `sim/traffic` |
| `packField` | `profiles` (in the field's own order) as `FieldData`, its arrays fresh: the buffers to hand over in `transfer`. | não portado | `sim/roads`, `sim/traffic` |
| `roadField` | The reads of a solved field over `profiles` (`index` built over them, in that order). | não portado | `sim/roads`, `sim/traffic` |
| `roadField.blend` | The height at a point from the `found` candidates just measured. | não portado | `sim/roads`, `sim/traffic` |
| `roadField.frameInto` | — | não portado | `sim/roads`, `sim/traffic` |
| `roadField.nearest` | The road nearest a point, out of the structural levels asked for; its arc and distance in `last`. | não portado | `sim/roads`, `sim/traffic` |
| `roadField.onSegment` | Deck height on one specific segment — what an agent riding it stands on. | não portado | `sim/roads`, `sim/traffic` |
| `roadField.query` | — | não portado | `sim/roads`, `sim/traffic` |
| `roadField.remember` | — | não portado | `sim/roads`, `sim/traffic` |
| `roadField.remembering` | Runs `work` with every point it reads remembered, so a point asked again | não portado | `sim/roads`, `sim/traffic` |
| `roadField.remembers` | — | não portado | `sim/roads`, `sim/traffic` |
| `roadField.roadAt` | Everything a surface needs to know about the road nearest a point. | não portado | `sim/roads`, `sim/traffic` |
| `roadField.roadAtInto` | `roadAt` written into `out`, nothing made (the paving read under every building's footprint, `paving.ts`); `sidewalkLeft`/`Right` only when they diffe | não portado | `sim/roads`, `sim/traffic` |
| `roadField.sampleProfile` | — | não portado | `sim/roads`, `sim/traffic` |
| `roadField.surfaceFrameAt` | Texture coordinates of (x, y) in the frame of the road nearest to | não portado | `sim/roads`, `sim/traffic` |
| `roadField.surfaceFrameInto` | `surfaceFrameAt` written into `out`, nothing made (asked for every vertex of a road surface, `roadSurfaceReads.ts`). | não portado | `sim/roads`, `sim/traffic` |
| `roadSampleOut` | A sample to hand `roadAtInto`. | não portado | `sim/roads`, `sim/traffic` |
| `slotOf` | The slot of (x, y) in an open-addressed table of `mask + 1` slots, probing over `xs`/`ys` (entries + 1 in `slots`). | não portado | `sim/roads`, `sim/traffic` |
| `smoothstep` | 0 below `a`, 1 above `b`, smooth in between. | não portado | `sim/roads`, `sim/traffic` |
| `unpackField` | The profiles of `FieldData`, in its order. | não portado | `sim/roads`, `sim/traffic` |

## `src/world/roads/buildMode.ts` (inteiro, 125 linhas) — **não se aplica**

Dono: funcionalidade `road-elevation`. Depende da altura solucionada das vias.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `BuildMode` | HOW A ROAD IS BUILT at each point along it | não se aplica | funcionalidade `road-elevation` |
| `BuildRun` | A stretch built one way. | não se aplica | funcionalidade `road-elevation` |
| `RoadStation` | A point of a road being built: where, how far along, its deck, the ground under it, and how. | não se aplica | funcionalidade `road-elevation` |
| `buildModeAt` | The way of building at one point: `lift` is the deck over the natural ground (world units). | não se aplica | funcionalidade `road-elevation` |
| `buildRuns` | The stations as stretches of one way of building, in order. | não se aplica | funcionalidade `road-elevation` |
| `orderAlong` | The segments of a road laid from `startAt`, in order and each with its direction | não se aplica | funcionalidade `road-elevation` |
| `stationsAlong` | The stations of a road every `step` units along it | não se aplica | funcionalidade `road-elevation` |

## `src/world/roads/catalog.ts` (inteiro, 110 linhas) — **portado**

Dono: `project/data/catalog/road_profiles*.json`, `tools/assets/make_road_profiles.py`. Os 31 perfis do catálogo, com larguras, prioridades, velocidades e ordem dos cartões. Meus modelos não

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `CATALOG_CATEGORIES` | — | portado | `project/data/catalog/road_profiles*.json`, `tools/assets/make_road_profiles.py` |
| `CatalogCategory` | THE ROAD CATALOGUE (the player's order of 2026-10-09) | portado | `project/data/catalog/road_profiles*.json`, `tools/assets/make_road_profiles.py` |
| `CatalogRoad` | — | portado | `project/data/catalog/road_profiles*.json`, `tools/assets/make_road_profiles.py` |
| `Lane` | — | portado | `project/data/catalog/road_profiles*.json`, `tools/assets/make_road_profiles.py` |
| `Shape` | — | portado | `project/data/catalog/road_profiles*.json`, `tools/assets/make_road_profiles.py` |
| `catalogRoads` | The ready roads, by category; the classes in theirs. | portado | `project/data/catalog/road_profiles*.json`, `tools/assets/make_road_profiles.py` |
| `catalogRoads.cls` | — | portado | `project/data/catalog/road_profiles*.json`, `tools/assets/make_road_profiles.py` |
| `road` | — | portado | `project/data/catalog/road_profiles*.json`, `tools/assets/make_road_profiles.py` |
| `shapeProfile` | A profile from a short description, left edge to right edge looking from A to B. | portado | `project/data/catalog/road_profiles*.json`, `tools/assets/make_road_profiles.py` |
| `shapeProfile.kerb` | — | portado | `project/data/catalog/road_profiles*.json`, `tools/assets/make_road_profiles.py` |
| `shapeProfile.lane` | — | portado | `project/data/catalog/road_profiles*.json`, `tools/assets/make_road_profiles.py` |
| `type` | — | portado | `project/data/catalog/road_profiles*.json`, `tools/assets/make_road_profiles.py` |

## `src/world/roads/connectors.ts` (inteiro, 84 linhas) — **não portado**

Dono: `sim/roads`, `sim/commands`. Regras por acesso, conectores, nomes de rua, modo de construção: não portados. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `LaneEnds` | A junction's lanes as the connector editor draws them: `NodeLanes`, each lane's end at the junction and its next point, the graph's revision. | não portado | `sim/roads`, `sim/commands` |
| `LaneLink` | LANE CONNECTORS BY HAND | não portado | `sim/roads`, `sim/commands` |
| `MAX_LINKS` | — | não portado | `sim/roads`, `sim/commands` |
| `NodeLanes` | The arriving and leaving lanes a connector editor offers, and the connections now built (`lanelets.ts`). | não portado | `sim/roads`, `sim/commands` |
| `linkKey` | — | não portado | `sim/roads`, `sim/commands` |
| `linksDigest` | What a node's connections are, for its key: '' when it has none of its own. | não portado | `sim/roads`, `sim/commands` |
| `linksOf` | The links of one arriving lane. | não portado | `sim/roads`, `sim/commands` |
| `movementKey` | Stable segment-pair key for a movement through a junction (`RoadNode.blockedMovements`). | não portado | `sim/roads`, `sim/commands` |
| `normalizeLaneLinks` | A stored list of links, or undefined when it is not one (or empty). | não portado | `sim/roads`, `sim/commands` |
| `sortLinks` | Links in one order, so two lists that say the same thing are the same text. | não portado | `sim/roads`, `sim/commands` |
| `toggleLaneLink` | The node's links after the player toggles one connection | não portado | `sim/roads`, `sim/commands` |
| `toggleLaneLink.arrives` | — | não portado | `sim/roads`, `sim/commands` |
| `toggleLaneLink.leaves` | — | não portado | `sim/roads`, `sim/commands` |

## `src/world/roads/derivedSigns.ts` (inteiro, 84 linhas) — **não portado**

Dono: `sim/roads`, `sim/commands`. Regras por acesso, conectores, nomes de rua, modo de construção: não portados. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `APPROACH_REACH` | How far back from the junction mouth a placed plate still belongs to that approach. | não portado | `sim/roads`, `sim/commands` |
| `DerivedSign` | THE SIGNS THE RULES PUT UP | não portado | `sim/roads`, `sim/commands` |
| `SPEED_MIN_LENGTH` | A stretch shorter than this gets no speed plate of its own. | não portado | `sim/roads`, `sim/commands` |
| `SPEED_PAST_MOUTH` | Past the junction mouth, where a speed plate stands. | não portado | `sim/roads`, `sim/commands` |
| `signApproach` | The junction approach a stop or give-way plate put at `at` would govern ("placa = regra") | não portado | `sim/roads`, `sim/commands` |
| `signsAtNode` | The signs the rules put up at one junction | não portado | `sim/roads`, `sim/commands` |

## `src/world/roads/footwayRise.ts` (inteiro, 92 linhas) — **não portado**

Dono: `native/src/sim/roads`. Calçada nivelada.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `FlushData` | The roads with a footway laid flush whose filed box (`Network.flushCells`) meets some rectangles | não portado | `native/src/sim/roads` |
| `FlushRoad` | What `flushHolds` reads of a road: its widths and which footway is flush. | não portado | `native/src/sim/roads` |
| `flushHolds` | Whether (x, y) is on a footway of this road laid flush. | não portado | `native/src/sim/roads` |
| `flushShareOf` | `footwayRiseShare` over `FlushData`: the same answer at every point of the rectangles it was packed for. | não portado | `native/src/sim/roads` |
| `footwayRiseAt` | The footway's height over the carriageway at (x, y), world units. | não portado | `native/src/sim/roads` |
| `footwayRiseShare` | The share of `FOOTWAY_RISE` the footway at (x, y) stands at: 1 kerbed, 0 flush. | não portado | `native/src/sim/roads` |
| `frame` | The frame a read's side is taken from (`flushHolds`): one, never kept. | não portado | `native/src/sim/roads` |
| `hit` | — | não portado | `native/src/sim/roads` |
| `packFlush` | `FlushData` for `rects`, its arrays fresh: the buffers to hand over in `transfer`. | não portado | `native/src/sim/roads` |

## `src/world/roads/furnitureCatalog.ts` (inteiro, 29 linhas) — **não se aplica**

Dono: `street-furniture` (porte-6). Também é entrada de `street-furniture`, que porta mobiliário, travessias, retornos e rede.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `FURNITURE_CATALOG` | — | não se aplica | `street-furniture` (porte-6) |
| `FurnitureEntry` | THE STREET FURNITURE CATALOGUE, by data | não se aplica | `street-furniture` (porte-6) |
| `furnitureEntry` | — | não se aplica | `street-furniture` (porte-6) |

## `src/world/roads/furnitureSets.ts` (inteiro, 165 linhas) — **não se aplica**

Dono: `street-furniture` (porte-6). Também é entrada de `street-furniture`, que porta mobiliário, travessias, retornos e rede.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `CELL` | The cell of the nearby-items grid, world units. | não se aplica | `street-furniture` (porte-6) |
| `FURNITURE_SETS` | — | não se aplica | `street-furniture` (porte-6) |
| `FurniturePlacement` | — | não se aplica | `street-furniture` (porte-6) |
| `FurnitureSet` | THE FURNITURE A NEW ROAD IS BUILT WITH | não se aplica | `street-furniture` (porte-6) |
| `NBR9050_CLEAR_WALK` | NBR 9050 6.12.3: the least clear walking strip. | não se aplica | `street-furniture` (porte-6) |
| `NEAR_CELL` | The cell a street's items are filed in for each piece's look round it (`SnapLocal.around`). | não se aplica | `street-furniture` (porte-6) |
| `Pattern` | — | não se aplica | `street-furniture` (porte-6) |
| `SETS` | Each set's patterns, from the catalogue (`furnitureCatalog.ts`), in its order. | não se aplica | `street-furniture` (porte-6) |
| `furnitureFor` | The pieces of `set` along the given segments, each where the landscaping tool would accept it among `existing` | não se aplica | `street-furniture` (porte-6) |
| `furnitureFor.cellOf` | — | não se aplica | `street-furniture` (porte-6) |
| `furnitureFor.file` | — | não se aplica | `street-furniture` (porte-6) |
| `isFurnitureSet` | — | não se aplica | `street-furniture` (porte-6) |
| `nearGrid` | A street's items filed by place: a piece looks at those round it, never the street's every item. | não se aplica | `street-furniture` (porte-6) |
| `nearGrid.around` | — | não se aplica | `street-furniture` (porte-6) |
| `nearGrid.file` | — | não se aplica | `street-furniture` (porte-6) |
| `patternsOf` | — | não se aplica | `street-furniture` (porte-6) |

## `src/world/roads/laneEnds.ts` (inteiro, 42 linhas) — **não portado**

Dono: `sim/roads`, `sim/commands`. Regras por acesso, conectores, nomes de rua, modo de construção: não portados. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `laneEnds` | `nodeLanes` with each lane's end at the junction (`LaneEnds`), for the connector editor. | não portado | `sim/roads`, `sim/commands` |
| `nodeLanes` | What the node's lanes and built connections are, read from the lane graph. | não portado | `sim/roads`, `sim/commands` |
| `nodeLanes.lane` | — | não portado | `sim/roads`, `sim/commands` |

## `src/world/roads/markingStyle.ts` (inteiro, 87 linhas) — **em parte**

Dono: `streets.json markings`, `sim/roads/profiles.cpp`. Cores da tinta e linha central por classe. Estilos regionais e legendas (ÔNIBUS, PARE) não

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `CLASSIC_DASH` | — | em parte | `streets.json markings`, `sim/roads/profiles.cpp` |
| `GLYPHS` | The letters of the legends, stroked: each a list of polylines in a box one unit wide | em parte | `streets.json markings`, `sim/roads/profiles.cpp` |
| `MarkingStyle` | — | em parte | `streets.json markings`, `sim/roads/profiles.cpp` |
| `MarkingStyleId` | THE REGIONAL STANDARD OF ROAD PAINT, chosen for the map: - 'br', Brasil/CONTRAN | em parte | `streets.json markings`, `sim/roads/profiles.cpp` |
| `STYLE_PAINT` | The centre line colours the regional standards paint (`render/markings.ts` `PAINT_COLOURS`). | em parte | `streets.json markings`, `sim/roads/profiles.cpp` |
| `WHITE` | — | em parte | `streets.json markings`, `sim/roads/profiles.cpp` |
| `YELLOW` | — | em parte | `streets.json markings`, `sim/roads/profiles.cpp` |
| `YIELD_SYMBOL` | The give-way symbol on the floor (MBST vol. IV, "SIP" / Vienna "shark's tooth") | em parte | `streets.json markings`, `sim/roads/profiles.cpp` |
| `isMarkingStyle` | — | em parte | `streets.json markings`, `sim/roads/profiles.cpp` |
| `markingStyle` | — | em parte | `streets.json markings`, `sim/roads/profiles.cpp` |
| `ring` | — | em parte | `streets.json markings`, `sim/roads/profiles.cpp` |
| `wordStrokes` | A word's strokes in the unit box per letter, letters side by side with a gap of `gap` letter widths. | em parte | `streets.json markings`, `sim/roads/profiles.cpp` |

## `src/world/roads/profile.ts` (inteiro, 268 linhas) — **em parte**

Dono: `sim/roads/road_variants.cpp`, `sim/roads/profiles.cpp`. Perfil como lista de faixas já existia; validação do editor (`profileProblems`) e seção por segmento não.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `AppliedProfile` | The segment's own fields for a profile (`profileProblems` must be empty). | em parte | `sim/roads/road_variants.cpp`, `sim/roads/profiles.cpp` |
| `ProfileElement` | THE ROAD PROFILE | em parte | `sim/roads/road_variants.cpp`, `sim/roads/profiles.cpp` |
| `ProfileIssue` | A problem and the elements it is about (by index; empty: the road as a whole). | em parte | `sim/roads/road_variants.cpp`, `sim/roads/profiles.cpp` |
| `ProfileProblem` | Why a profile cannot be built as it stands; a translation key under `profile.problem.`. | em parte | `sim/roads/road_variants.cpp`, `sim/roads/profiles.cpp` |
| `ProfileSource` | What a segment is made of, as the profile reads it. | em parte | `sim/roads/road_variants.cpp`, `sim/roads/profiles.cpp` |
| `RoadProfileSpec` | — | em parte | `sim/roads/road_variants.cpp`, `sim/roads/profiles.cpp` |
| `applyProfile` | The segment's fields for a profile. | em parte | `sim/roads/road_variants.cpp`, `sim/roads/profiles.cpp` |
| `applyProfile.same` | — | em parte | `sim/roads/road_variants.cpp`, `sim/roads/profiles.cpp` |
| `authoredFields` | — | em parte | `sim/roads/road_variants.cpp`, `sim/roads/profiles.cpp` |
| `authoredFields.kindAt` | — | em parte | `sim/roads/road_variants.cpp`, `sim/roads/profiles.cpp` |
| `authoredFields.uses` | — | em parte | `sim/roads/road_variants.cpp`, `sim/roads/profiles.cpp` |
| `profileIssues` | `profileProblems` with the elements each problem is about, so an editor can name the problem on the element that has it | em parte | `sim/roads/road_variants.cpp`, `sim/roads/profiles.cpp` |
| `profileIssues.add` | — | em parte | `sim/roads/road_variants.cpp`, `sim/roads/profiles.cpp` |
| `profileOf` | The profile of a segment as it stands (its class, lanes, direction, section, parking). | em parte | `sim/roads/road_variants.cpp`, `sim/roads/profiles.cpp` |
| `profileOf.band` | — | em parte | `sim/roads/road_variants.cpp`, `sim/roads/profiles.cpp` |
| `profileOf.footway` | — | em parte | `sim/roads/road_variants.cpp`, `sim/roads/profiles.cpp` |
| `profileOf.lane` | — | em parte | `sim/roads/road_variants.cpp`, `sim/roads/profiles.cpp` |
| `profileProblems` | What stops a profile from being built, empty when nothing does. | em parte | `sim/roads/road_variants.cpp`, `sim/roads/profiles.cpp` |
| `profileRoad` | The road a profile builds, as every consumer reads it. | em parte | `sim/roads/road_variants.cpp`, `sim/roads/profiles.cpp` |
| `profileWidth` | Total width of a profile, footways included, world units. | em parte | `sim/roads/road_variants.cpp`, `sim/roads/profiles.cpp` |
| `sameFlat` | Whether two flat records hold the same values, an absent field the same as `undefined`. | em parte | `sim/roads/road_variants.cpp`, `sim/roads/profiles.cpp` |
| `sameProfile` | DO NOT compare profiles by `JSON.stringify` | em parte | `sim/roads/road_variants.cpp`, `sim/roads/profiles.cpp` |

## `src/world/roads/rules.ts` (inteiro, 143 linhas) — **não portado**

Dono: `sim/roads`, `sim/commands`. Regras por acesso, conectores, nomes de rua, modo de construção: não portados. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `APPROACH_RULES` | — | não portado | `sim/roads`, `sim/commands` |
| `ApproachRule` | THE RULES OF A JUNCTION: who goes first, as the source; the signs are derived from it (V6). | não portado | `sim/roads`, `sim/commands` |
| `ApproachRuleEntry` | — | não portado | `sim/roads`, `sim/commands` |
| `ApproachSign` | The sign a leg's driver meets at a junction | não portado | `sim/roads`, `sim/commands` |
| `GREEN_LIMITS` | — | não portado | `sim/roads`, `sim/commands` |
| `OFFSET_LIMIT` | — | não portado | `sim/roads`, `sim/commands` |
| `SignalSettings` | A signal's settings; absent: adaptive, the game's own timings. | não portado | `sim/roads`, `sim/commands` |
| `approachSign` | — | não portado | `sim/roads`, `sim/commands` |
| `arrivesAt` | Whether traffic on a segment arrives at the node (not a one-way leaving it). | não portado | `sim/roads`, `sim/commands` |
| `isHighwayClass` | Whether a road is a "rodovia" for CTB art. 29, III, a (the highway class). | não portado | `sim/roads`, `sim/commands` |
| `legRule` | The rule of one leg of a junction with signs ('priority' control) | não portado | `sim/roads`, `sim/commands` |
| `mainRoadLegs` | The road a junction is ON: the pair of legs whose lesser rank is the highest, the straighter pair on a tie. | não portado | `sim/roads`, `sim/commands` |
| `normalizeApproachRules` | — | não portado | `sim/roads`, `sim/commands` |
| `normalizeSignalSettings` | — | não portado | `sim/roads`, `sim/commands` |
| `roadRank` | A road's rank at a junction: its authored priority, else its class (the order `admission.ts` uses). | não portado | `sim/roads`, `sim/commands` |
| `rulesDigest` | — | não portado | `sim/roads`, `sim/commands` |
| `signalDigest` | — | não portado | `sim/roads`, `sim/commands` |

## `src/world/roads/streetChain.ts` (inteiro, 69 linhas) — **não portado**

Dono: `sim/roads`, `sim/commands`. Regras por acesso, conectores, nomes de rua, modo de construção: não portados. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `ChainPiece` | — | não portado | `sim/roads`, `sim/commands` |
| `PROBE` | — | não portado | `sim/roads`, `sim/commands` |
| `STRAIGHT` | THE WHOLE STREET a segment belongs to (bulk edit) | não portado | `sim/roads`, `sim/commands` |
| `flipProfile` | A profile seen from the other end: its elements in the other order, each lane's direction swapped. | não portado | `sim/roads`, `sim/commands` |
| `streetChain` | — | não portado | `sim/roads`, `sim/commands` |
| `streetChain.leaving` | — | não portado | `sim/roads`, `sim/commands` |
| `streetChain.walk` | — | não portado | `sim/roads`, `sim/commands` |

## `src/world/roads/streetNames.ts` (inteiro, 142 linhas) — **não portado**

Dono: `sim/roads`, `sim/commands`. Regras por acesso, conectores, nomes de rua, modo de construção: não portados. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `NAMES` | The names new streets are given: the ones every Brazilian town has. | não portado | `sim/roads`, `sim/commands` |
| `NAME_REACH` | How far from a street its name plate may stand and still name it. | não portado | `sim/roads`, `sim/commands` |
| `StreetNumbers` | A segment's place in its street's numbering: the numbers along it and which side is even. | não portado | `sim/roads`, `sim/commands` |
| `nameFor` | Names for the streets of `segments` that have none | não portado | `sim/roads`, `sim/commands` |
| `nameItemSegment` | DO NOT walk every ribbon for each plate: the names of a city were plates × roads. | não portado | `sim/roads`, `sim/commands` |
| `platesNear` | The street name plates that may name a road of `chain` (those within reach of one of its ribbons), in id order. | não portado | `sim/roads`, `sim/commands` |
| `prefixFor` | "Avenida" for the wide classes, "Rua" for the rest. | não portado | `sim/roads`, `sim/commands` |
| `streetNameOf` | The name of the street a segment runs in, or null: the first plate (by id) that names one of its roads. | não portado | `sim/roads`, `sim/commands` |
| `streetNumbers` | — | não portado | `sim/roads`, `sim/commands` |

## `src/world/roads/templates.ts` (inteiro, 90 linhas) — **não portado**

Dono: `native/src/sim/roads/profiles.cpp`. Modelos do jogador: Meus modelos não existe.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `KINDS` | — | não portado | `native/src/sim/roads/profiles.cpp` |
| `RoadTemplate` | ROAD TEMPLATES: a profile with a name, to lay or apply again. | não portado | `native/src/sim/roads/profiles.cpp` |
| `classDirection` | The direction a class is drawn in (`editor/commit.ts`): one-lane classes one way. | não portado | `native/src/sim/roads/profiles.cpp` |
| `classTemplates` | The classes as templates, in the order of the road tool. | não portado | `native/src/sim/roads/profiles.cpp` |
| `normalizeTemplate` | A stored template, or null when it is not one the game can build. | não portado | `native/src/sim/roads/profiles.cpp` |
| `readElement` | — | não portado | `native/src/sim/roads/profiles.cpp` |

## `src/world/roads/tuning.ts` (inteiro, 86 linhas) — **não portado**

Dono: `sim/roads`, `sim/commands`. Regras por acesso, conectores, nomes de rua, modo de construção: não portados. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `ROAD_TUNING` | THE ROAD TUNING: every figure the road system is tuned by, in one place. | não portado | `sim/roads`, `sim/commands` |

## `src/world/roadSection.ts` (inteiro, 271 linhas) — **em parte**

Dono: `sim/roads/road_variants.cpp`. Largura pela seção (`sectionForWidth`) e limites portados; seção por segmento não.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `CARRIAGEWAY_MATERIALS` | What an element of the cross-section is paved with. | em parte | `sim/roads/road_variants.cpp` |
| `CENTRE_LINES` | — | em parte | `sim/roads/road_variants.cpp` |
| `CarriagewayMaterial` | — | em parte | `sim/roads/road_variants.cpp` |
| `CentreLine` | — | em parte | `sim/roads/road_variants.cpp` |
| `FOOTWAY_MATERIALS` | — | em parte | `sim/roads/road_variants.cpp` |
| `FootwayMaterial` | — | em parte | `sim/roads/road_variants.cpp` |
| `LANE_LINES` | — | em parte | `sim/roads/road_variants.cpp` |
| `LANE_LIST_KEYS` | — | em parte | `sim/roads/road_variants.cpp` |
| `LANE_TURN_RULES` | — | em parte | `sim/roads/road_variants.cpp` |
| `LANE_USES` | — | em parte | `sim/roads/road_variants.cpp` |
| `LaneLine` | The line between two lanes of one direction (`RoadSection.linesForward`). | em parte | `sim/roads/road_variants.cpp` |
| `LaneTurnRule` | — | em parte | `sim/roads/road_variants.cpp` |
| `LaneUse` | Who may drive a lane (`RoadSection.useForward`). | em parte | `sim/roads/road_variants.cpp` |
| `MATERIAL_FIELDS` | Every field of `SectionMaterials`: a field added there and left out here does not compile. | em parte | `sim/roads/road_variants.cpp` |
| `MEDIAN_MATERIALS` | — | em parte | `sim/roads/road_variants.cpp` |
| `MedianMaterial` | — | em parte | `sim/roads/road_variants.cpp` |
| `ROAD_SECTION_LIMITS` | Physical edit bounds, shared by persistence and the section editor. | em parte | `sim/roads/road_variants.cpp` |
| `RoadSection` | Authored road section. | em parte | `sim/roads/road_variants.cpp` |
| `SectionMaterials` | — | em parte | `sim/roads/road_variants.cpp` |
| `cloneRoadSection` | A snapshot must never share editable turn arrays with its document. | em parte | `sim/roads/road_variants.cpp` |
| `flipSection` | The same section seen from the other end of its road | em parte | `sim/roads/road_variants.cpp` |
| `laneLineCrossable` | Whether a lane change across the line between lane `index` and lane `index + 1` of one direction is allowed | em parte | `sim/roads/road_variants.cpp` |
| `laneTurnAllowed` | Explicit arrows restrict every connector, including merge and U-turn fallbacks. | em parte | `sim/roads/road_variants.cpp` |
| `laneUse` | Who may drive lane `index` of one direction of a section. | em parte | `sim/roads/road_variants.cpp` |
| `normalizeMaterials` | Stored materials: undefined when none are set, null when one is not a material. | em parte | `sim/roads/road_variants.cpp` |
| `normalizeMaterials.pick` | — | em parte | `sim/roads/road_variants.cpp` |
| `normalizeRoadSection` | Rejects incomplete/non-finite input; clamps finite values at the document boundary. | em parte | `sim/roads/road_variants.cpp` |
| `sameMaterials` | DO NOT compare them as JSON text: the text follows the order the keys were written in | em parte | `sim/roads/road_variants.cpp` |
| `sameRoadSection` | — | em parte | `sim/roads/road_variants.cpp` |
| `sameRoadSectionIgnoringArrows` | Whether two sections are the same road once their lane arrows are set aside. | em parte | `sim/roads/road_variants.cpp` |
| `sectionForPiece` | The section of one piece of a split road. | em parte | `sim/roads/road_variants.cpp` |
| `sectionForWidth` | The section that gives a road of class profile `rt` a chosen TOTAL width | em parte | `sim/roads/road_variants.cpp` |
| `sectionSidewalk` | The section's footway on one side of a -> b. | em parte | `sim/roads/road_variants.cpp` |

## `src/world/roadTypes.ts` (inteiro, 428 linhas) — **em parte**

Dono: `RoadClass` em `sim/catalog/profile_types.h`, `sim/roads/road_variants.cpp`, `streets.json asphalt_by_class`. Classes, tom por classe, faixas e largura por opção. Seção por segmento (`RoadSection`) não: variantes são perfis

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `CASING_BAND` | Extra half-width of the casing beyond the footway edge: the grass verge that carries the footway's edge down | em parte | `RoadClass` em `sim/catalog/profile_types.h`, `sim/roads/road_variants.cpp`, `streets.json asphalt_by_class` |
| `CURB_BAND` | The kerb stone's width on top, beyond the carriageway edge: 15 cm, as a precast kerb. | em parte | `RoadClass` em `sim/catalog/profile_types.h`, `sim/roads/road_variants.cpp`, `streets.json asphalt_by_class` |
| `FOOTWAY_RISE` | Height of the footway above the carriageway, world units. | em parte | `RoadClass` em `sim/catalog/profile_types.h`, `sim/roads/road_variants.cpp`, `streets.json asphalt_by_class` |
| `LANE_LINE` | The line between two lanes running the SAME way: always white. | em parte | `RoadClass` em `sim/catalog/profile_types.h`, `sim/roads/road_variants.cpp`, `streets.json asphalt_by_class` |
| `LAST_UPGRADE_CLASS` | Street classes form the upgrade ladder; highway and access ramps are separate choices. | em parte | `RoadClass` em `sim/catalog/profile_types.h`, `sim/roads/road_variants.cpp`, `streets.json asphalt_by_class` |
| `Level` | Surface levels, in painting order. | em parte | `RoadClass` em `sim/catalog/profile_types.h`, `sim/roads/road_variants.cpp`, `streets.json asphalt_by_class` |
| `MAX_TRAVEL_LANES` | — | em parte | `RoadClass` em `sim/catalog/profile_types.h`, `sim/roads/road_variants.cpp`, `streets.json asphalt_by_class` |
| `MIN_TRAVEL_LANES` | Supported physical lane-count range for an individually configured road. | em parte | `RoadClass` em `sim/catalog/profile_types.h`, `sim/roads/road_variants.cpp`, `streets.json asphalt_by_class` |
| `MarkingStyle` | — | em parte | `RoadClass` em `sim/catalog/profile_types.h`, `sim/roads/road_variants.cpp`, `streets.json asphalt_by_class` |
| `NO_PART` | A segment with no section, or no parking: one key object for all of them (`roadProfile`'s memo). | em parte | `RoadClass` em `sim/catalog/profile_types.h`, `sim/roads/road_variants.cpp`, `streets.json asphalt_by_class` |
| `ROAD_TYPES` | — | em parte | `RoadClass` em `sim/catalog/profile_types.h`, `sim/roads/road_variants.cpp`, `streets.json asphalt_by_class` |
| `RoadSide` | One side of a road, left or right of a -> b. | em parte | `RoadClass` em `sim/catalog/profile_types.h`, `sim/roads/road_variants.cpp`, `streets.json asphalt_by_class` |
| `RoadType` | — | em parte | `RoadClass` em `sim/catalog/profile_types.h`, `sim/roads/road_variants.cpp`, `streets.json asphalt_by_class` |
| `SURFACE_LEVELS` | Levels that carry a filled surface built from roads and junctions alike. | em parte | `RoadClass` em `sim/catalog/profile_types.h`, `sim/roads/road_variants.cpp`, `streets.json asphalt_by_class` |
| `SegmentDirection` | Legal driving directions, relative to the stored `a -> b` orientation (`RoadSegment.direction`; `doc.ts` re-exports it). | em parte | `RoadClass` em `sim/catalog/profile_types.h`, `sim/roads/road_variants.cpp`, `streets.json asphalt_by_class` |
| `SurfaceLevel` | — | em parte | `RoadClass` em `sim/catalog/profile_types.h`, `sim/roads/road_variants.cpp`, `streets.json asphalt_by_class` |
| `casingHalf` | — | em parte | `RoadClass` em `sim/catalog/profile_types.h`, `sim/roads/road_variants.cpp`, `streets.json asphalt_by_class` |
| `flushOn` | Whether the footway on one side is level with the carriageway (no kerb). | em parte | `RoadClass` em `sim/catalog/profile_types.h`, `sim/roads/road_variants.cpp`, `streets.json asphalt_by_class` |
| `footwayOn` | The footway on one side of a -> b, kerb included. | em parte | `RoadClass` em `sim/catalog/profile_types.h`, `sim/roads/road_variants.cpp`, `streets.json asphalt_by_class` |
| `halfWidth` | Half-width of a road type at a given surface level. | em parte | `RoadClass` em `sim/catalog/profile_types.h`, `sim/roads/road_variants.cpp`, `streets.json asphalt_by_class` |
| `laneOffset` | Signed lateral offset of a lane's centreline from the road centreline. | em parte | `RoadClass` em `sim/catalog/profile_types.h`, `sim/roads/road_variants.cpp`, `streets.json asphalt_by_class` |
| `laneWidth` | Width of one travel lane: the carriageway less its median and parking lanes. | em parte | `RoadClass` em `sim/catalog/profile_types.h`, `sim/roads/road_variants.cpp`, `streets.json asphalt_by_class` |
| `lanesPerDirection` | — | em parte | `RoadClass` em `sim/catalog/profile_types.h`, `sim/roads/road_variants.cpp`, `streets.json asphalt_by_class` |
| `markingColor` | The colour of a painted line. | em parte | `RoadClass` em `sim/catalog/profile_types.h`, `sim/roads/road_variants.cpp`, `streets.json asphalt_by_class` |
| `profilesOf` | Each segment's profile, by its section and parking (records replaced on an edit, never changed) and its class, lanes and direction. | em parte | `RoadClass` em `sim/catalog/profile_types.h`, `sim/roads/road_variants.cpp`, `streets.json asphalt_by_class` |
| `resolveProfile` | — | em parte | `RoadClass` em `sim/catalog/profile_types.h`, `sim/roads/road_variants.cpp`, `streets.json asphalt_by_class` |
| `resolveProfile.fit` | — | em parte | `RoadClass` em `sim/catalog/profile_types.h`, `sim/roads/road_variants.cpp`, `streets.json asphalt_by_class` |
| `roadProfile` | Resolves the physical road profile for a segment: DO NOT build it again for every leg of every pass of a junction (`junction/build.ts`, `derive.ts`, ` | em parte | `RoadClass` em `sim/catalog/profile_types.h`, `sim/roads/road_variants.cpp`, `streets.json asphalt_by_class` |
| `roadType` | — | em parte | `RoadClass` em `sim/catalog/profile_types.h`, `sim/roads/road_variants.cpp`, `streets.json asphalt_by_class` |
| `sectionFromProfile` | Materializes the current profile for the first authored edit without changing old maps. | em parte | `RoadClass` em `sim/catalog/profile_types.h`, `sim/roads/road_variants.cpp`, `streets.json asphalt_by_class` |
| `sideHalfWidth` | Half-width of one side of a road at a surface level, left or right of a -> b | em parte | `RoadClass` em `sim/catalog/profile_types.h`, `sim/roads/road_variants.cpp`, `streets.json asphalt_by_class` |
| `sidewalkHalf` | — | em parte | `RoadClass` em `sim/catalog/profile_types.h`, `sim/roads/road_variants.cpp`, `streets.json asphalt_by_class` |
| `symmetric` | Whether the road's outline is the plain one: the same footway both sides, each kerbed | em parte | `RoadClass` em `sim/catalog/profile_types.h`, `sim/roads/road_variants.cpp`, `streets.json asphalt_by_class` |
| `travelLanes` | Number of lanes a vehicle may use in one travelling direction. | em parte | `RoadClass` em `sim/catalog/profile_types.h`, `sim/roads/road_variants.cpp`, `streets.json asphalt_by_class` |
| `travelProfile` | — | em parte | `RoadClass` em `sim/catalog/profile_types.h`, `sim/roads/road_variants.cpp`, `streets.json asphalt_by_class` |
| `travelShift` | Lateral offset of the travel way's centre from the road centreline, in the a -> b frame (positive: left). | em parte | `RoadClass` em `sim/catalog/profile_types.h`, `sim/roads/road_variants.cpp`, `streets.json asphalt_by_class` |
| `travelWidth` | Width of the travel lanes and median together, between the parking lanes. | em parte | `RoadClass` em `sim/catalog/profile_types.h`, `sim/roads/road_variants.cpp`, `streets.json asphalt_by_class` |

## `src/world/section.ts` (inteiro, 106 linhas) — **não portado**

Dono: `sim/roads`, `sim/traffic`. Rede, faixas e campos do 4.0: parte tem dono equivalente no jogo, nada conferido declaração a declaração. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `BENCH_ZONE` | Depth a bench (0.52 m seat) or a bin (0.66 m across) takes beside the kerb. | não portado | `sim/roads`, `sim/traffic` |
| `Band` | A band across one side of the road: from `inner` to `outer`, measured from the centreline. | não portado | `sim/roads`, `sim/traffic` |
| `CrossSection` | — | não portado | `sim/roads`, `sim/traffic` |
| `EDGE_ZONE` | Depth of the edge zone from the kerb face, kerb stone included (36 cm). | não portado | `sim/roads`, `sim/traffic` |
| `LAMP_ZONE` | Depth a lamp column (and a hydrant) takes beside the kerb. | não portado | `sim/roads`, `sim/traffic` |
| `MIN_FRONTAGE` | A frontage zone narrower than this is no use to anything: it is given to the through zone. | não portado | `sim/roads`, `sim/traffic` |
| `MIN_THROUGH` | Narrowest through zone a footway keeps, whatever else it holds. | não portado | `sim/roads`, `sim/traffic` |
| `SideZones` | One side of the road beyond the carriageway edge. | não portado | `sim/roads`, `sim/traffic` |
| `THROUGH_GOAL` | Through zone a footway is given before any of it goes to a frontage zone (NACTO: 1.5-2.5 m). | não portado | `sim/roads`, `sim/traffic` |
| `TREE_KERB_SETBACK` | — | não portado | `sim/roads`, `sim/traffic` |
| `TREE_MIN_FOOTWAY` | A footway at least this wide (`RoadType.sidewalk`, kerb included) has street trees. | não portado | `sim/roads`, `sim/traffic` |
| `TREE_PIT` | Tree pit side and its setback from the kerb face; the street-furniture layout uses the same numbers. | não portado | `sim/roads`, `sim/traffic` |
| `TravelLane` | — | não portado | `sim/roads`, `sim/traffic` |
| `bandMid` | Centre of a band. | não portado | `sim/roads`, `sim/traffic` |
| `sectionOf` | The cross-section of a resolved road profile (`roadProfile`). | não portado | `sim/roads`, `sim/traffic` |
| `sideZones` | One side's footway, `sidewalk` wide (kerb included), cut into zones. | não portado | `sim/roads`, `sim/traffic` |
| `zonesOn` | The zones of one side, left or right of a -> b. | não portado | `sim/roads`, `sim/traffic` |

## `src/world/signalPosts.ts` (parte, 34 linhas) — **não se aplica**

Dono: `street-furniture` (porte-6). Postes de semáforo são de `street-furniture`.


## `src/world/streetFurniture.ts` (inteiro, 124 linhas) — **não se aplica**

Dono: `street-furniture` (porte-6). Também é entrada de `street-furniture`, que porta mobiliário, travessias, retornos e rede.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `FURNITURE_OF` | Each placed kind as the furniture the renderer and the walkers know. | não se aplica | `street-furniture` (porte-6) |
| `FurnitureItem` | — | não se aplica | `street-furniture` (porte-6) |
| `FurnitureKind` | — | não se aplica | `street-furniture` (porte-6) |
| `STAND_REACH` | How far off its footway's band a stored item may have drifted and still stand on it. | não se aplica | `street-furniture` (porte-6) |
| `furnitureOf` | One placed item as the furniture standing where it was put: on the footway under it, or | não se aplica | `street-furniture` (porte-6) |
| `hash01` | — | não se aplica | `street-furniture` (porte-6) |
| `standing` | The furniture an item makes on the footway or the median under it, before the crossings are asked. | não se aplica | `street-furniture` (porte-6) |
| `streetFurniture` | Everything standing on the footways: the items the player placed with the landscaping tool | não se aplica | `street-furniture` (porte-6) |

## `src/world/structures.ts` (parte, 11 linhas) — **não portado**

Dono: `sim/roads`, `sim/traffic`. Rede, faixas e campos do 4.0: parte tem dono equivalente no jogo, nada conferido declaração a declaração. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `isSunken` | — | não portado | `sim/roads`, `sim/traffic` |

## `src/world/surfaceParts.ts` (inteiro, 59 linhas) — **não portado**

Dono: `sim/roads`, `sim/traffic`. Rede, faixas e campos do 4.0: parte tem dono equivalente no jogo, nada conferido declaração a declaração. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `MouthStrip` | A strip a curved road's drawn surface lays across one of its mouths (`junction/mouthOverlap.ts`), still to be laid: its full centreline, its half widt | não portado | `sim/roads`, `sim/traffic` |
| `SurfaceSource` | What the drivable surface round a node is made of, in the order `turnPaths.ts` `JunctionSurface` adds them | não portado | `sim/roads`, `sim/traffic` |
| `surfaceParts` | The pieces of the drivable surface round `node` - its junction plate and its legs - read off the network, nothing made | não portado | `sim/roads`, `sim/traffic` |
| `surfaceParts.road` | — | não portado | `sim/roads`, `sim/traffic` |

## `src/world/surfaces.ts` (inteiro, 151 linhas) — **não portado**

Dono: `sim/roads`, `sim/traffic`. Rede, faixas e campos do 4.0: parte tem dono equivalente no jogo, nada conferido declaração a declaração. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `Bands` | Bands, each the part of one level not covered by the level inside it. | não portado | `sim/roads`, `sim/traffic` |
| `SurfaceJunction` | — | não portado | `sim/roads`, `sim/traffic` |
| `SurfaceNet` | — | não portado | `sim/roads`, `sim/traffic` |
| `SurfaceRibbon` | — | não portado | `sim/roads`, `sim/traffic` |
| `SurfaceSegmentFilter` | — | não portado | `sim/roads`, `sim/traffic` |
| `Surfaces` | Every surface of the network, one entry per level, outermost first. | não portado | `sim/roads`, `sim/traffic` |
| `bands` | — | não portado | `sim/roads`, `sim/traffic` |
| `levelPolygons` | One merged polygon set per surface level. | não portado | `sim/roads`, `sim/traffic` |
| `levelRings` | Every ring of one level, as clipper input, before they are merged (`levelPolygons`). | não portado | `sim/roads`, `sim/traffic` |
| `levelRingsOf` | `levelRings` of the given ribbons and junctions only: the rings round one place | não portado | `sim/roads`, `sim/traffic` |
| `ownsJunction` | Whether this pass draws the junction at a node. | não portado | `sim/roads`, `sim/traffic` |
| `surfaces` | — | não portado | `sim/roads`, `sim/traffic` |

## `src/world/turnDigest.ts` (parte, 24 linhas) — **não se aplica**

Dono: `traffic` (porte-5). Caminhos de conversão pertencem ao grafo de faixas de `sim/traffic`.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `TurnDigest` | — | não se aplica | `traffic` (porte-5) |
| `TurnDigest.a` | — | não se aplica | `traffic` (porte-5) |
| `TurnDigest.add` | — | não se aplica | `traffic` (porte-5) |
| `TurnDigest.b` | — | não se aplica | `traffic` (porte-5) |
| `TurnDigest.point` | — | não se aplica | `traffic` (porte-5) |
| `TurnDigest.value` | — | não se aplica | `traffic` (porte-5) |

## `src/world/turnPaths.ts` (parte, 136 linhas) — **não se aplica**

Dono: `traffic` (porte-5). Caminhos de conversão pertencem ao grafo de faixas de `sim/traffic`.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `JunctionSurface.empty` | — | não se aplica | `traffic` (porte-5) |
| `JunctionSurface.flatRings` | — | não se aplica | `traffic` (porte-5) |
| `JunctionSurface.key` | — | não se aplica | `traffic` (porte-5) |
| `TurnPathResult` | — | não se aplica | `traffic` (porte-5) |
| `bulbTurnPath` | — | não se aplica | `traffic` (porte-5) |
| `bulbTurnPath.angleAt` | — | não se aplica | `traffic` (porte-5) |
| `bulbTurnPath.cubic` | — | não se aplica | `traffic` (porte-5) |
| `bulbTurnPath.on` | — | não se aplica | `traffic` (porte-5) |
| `bulbTurnPath.tangent` | — | não se aplica | `traffic` (porte-5) |
| `bulbTurnPath.wrap` | — | não se aplica | `traffic` (porte-5) |
| `laneEndKey` | — | não se aplica | `traffic` (porte-5) |
| `makeChoice` | — | não se aplica | `traffic` (porte-5) |
| `medianUturnPath` | — | não se aplica | `traffic` (porte-5) |
| `turnFits` | — | não se aplica | `traffic` (porte-5) |
| `turnPath` | — | não se aplica | `traffic` (porte-5) |
| `unflat` | — | não se aplica | `traffic` (porte-5) |

## `src/world/utilities.ts` (parte, 3 linhas) — **não se aplica**

Dono: `street-furniture` (porte-6). Rede elétrica é de `street-furniture`.


## `src/world/viewCells.ts` (parte, 18 linhas) — **não portado**

Dono: `sim/roads`, `sim/traffic`. Rede, faixas e campos do 4.0: parte tem dono equivalente no jogo, nada conferido declaração a declaração. Classificado por pasta: não li cada declaração.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `ViewCells.ensureMany` | — | não portado | `sim/roads`, `sim/traffic` |
| `ViewCells.forget` | — | não portado | `sim/roads`, `sim/traffic` |

## `src/world/walkable.ts` (parte, 121 linhas) — **não se aplica**

Dono: `traffic` (porte-5). Caminhos a pé são da simulação de pedestres.

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `KEPT_CELLS` | — | não se aplica | `traffic` (porte-5) |
| `WalkableSurface` | — | não se aplica | `traffic` (porte-5) |
| `WalkableSurface.cellOf` | — | não se aplica | `traffic` (porte-5) |
| `WalkableSurface.cellOf.ringsOf` | — | não se aplica | `traffic` (porte-5) |
| `WalkableSurface.constructor` | — | não se aplica | `traffic` (porte-5) |
| `WalkableSurface.footway` | On the footway or its kerb. | não se aplica | `traffic` (porte-5) |
| `WalkableSurface.forgetChanged` | DO NOT keep a cell's rings after its roads changed or went (a tile let go): its stamp would not match again, and the grids of every cell the walkers e | não se aplica | `traffic` (porte-5) |
| `WalkableSurface.keep` | A cell made kept, the cells asked longest ago let go past `KEPT_CELLS`. | não se aplica | `traffic` (porte-5) |
| `WalkableSurface.lastKey` | The cell asked last (`cellOf`): its place in the order stands while it is asked again. | não se aplica | `traffic` (porte-5) |
| `WalkableSurface.made` | Each cell made, its stamp, and the two filings and their epochs when it was last found to match (`cellOf`). | não se aplica | `traffic` (porte-5) |
| `WalkableSurface.seen` | The network's state the cells kept were checked against (`forgetChanged`). | não se aplica | `traffic` (porte-5) |
| `clipToBox` | — | não se aplica | `traffic` (porte-5) |

## `src/world/walkingLines.ts` (parte, 19 linhas) — **não se aplica**

Dono: `traffic` (porte-5). Caminhos a pé são da simulação de pedestres.


## `src/world/walkways.ts` (parte, 177 linhas) — **não se aplica**

Dono: `traffic` (porte-5). Caminhos a pé são da simulação de pedestres.


## `src/world/zoneGrid.ts` (parte, 75 linhas) — **não portado**

Dono: `sim/roads`, `sim/traffic`. Rede, faixas e campos do 4.0: parte tem dono equivalente no jogo, nada conferido declaração a declaração. Classificado por pasta: não li cada declaração.

