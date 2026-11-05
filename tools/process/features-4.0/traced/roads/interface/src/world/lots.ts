// What the feature uses of src/world/lots.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/lots.ts:28-28 variable LOT_DEPTH
/** Lots: the land cut into plots the player zones and buildings grow on, as a town's cadastre is. - A closed block */

/** Depth of a lot along an open street. */
export const LOT_DEPTH = m(32);

// src/world/lots.ts:30-30 variable LOT_FRONTAGE
/** The frontage a lot aims at: a block's rows and a street's strip are cut into equal lots of about this width */
const LOT_FRONTAGE = m(22);

// src/world/lots.ts:33-33 variable MIN_LOT
const MIN_LOT = m(10);

// src/world/lots.ts:34-34 variable RASTER
const RASTER = m(2);

// src/world/lots.ts:39-44 function lotArea
export function lotArea(l: Pick<Lot, 'corners'>): number;

// src/world/lots.ts:128-128 interface Candidate
interface Candidate { key: string; corners: Vec2[] }

// src/world/lots.ts:131-135 function facingCorners
/** The corners in the order a building reads its lot: front-start to front-end with the back on the LEFT of that direction */
function facingCorners<T extends readonly Vec2[]>(c: T): Vec2[];

// src/world/lots.ts:138-145 interface LotPlan
/** New lots for land with none yet, and the lots the paving now covers. */
export interface LotPlan {
  add: Candidate[];
  /** The land not cut into lots yet, by key (a key the document knows is left out). */
  keys: string[];
  drop: number[];
  /** Where the land of each key lies (`keys[i]`'s at `keyRects[i]`): a key new to the document is written in the diary there */
  keyRects?: ChangeRect[];
}

// src/world/lots.ts:148-148 variable planners
/** Each network's planner, following that network's changes and its document's lots (`LotPlanner`). */
const planners = new WeakMap<Network, LotPlanner>();

// src/world/lots.ts:151-155 function planLots
/** DO NOT plan the whole map again after an edit: the land the diaries say changed is planned again, the rest kept */
export function planLots(doc: RoadDoc, net: Network): LotPlan;

// src/world/lots.ts:158-160 function forgetLotPlans
/** A network's planner let go (`planLots` makes it again when asked): a city generated once (`editor/smallCityMap.ts`), whose land is planned once while its network lives on for the growth - its rasters and blocks held for nothing (17 MB of the city of 25,000 buildings, 2026-10-22). */
export function forgetLotPlans(net: Network): void;

// src/world/lots.ts:166-166 variable TILE
/** Side of a tile of the land's raster, in raster cells (128 m). */
const TILE = 64;

// src/world/lots.ts:167-167 variable TILE_SIDE
const TILE_SIDE = TILE * RASTER;

// src/world/lots.ts:171-171 variable PAD
/** How far round a street's box its land is asked whether it is paved: a lot's depth and a street's width more. */
const PAD = LOT_DEPTH + m(20);

// src/world/lots.ts:173-173 variable STREET_GROW
/** How far past a street's filed bounds its strip reads the land and the lots (`streetPlan`'s reach, with a margin). */
const STREET_GROW = LOT_DEPTH + m(6);

// src/world/lots.ts:175-175 variable SMALL_PIECE
/** How far a piece of land too small for a block reaches at most: fewer cells than a block's least area holds. */
const SMALL_PIECE = (MIN_LOT * MIN_LOT * 2) / RASTER;

// src/world/lots.ts:188-192 function forTiles
/** Calls `visit` with every tile a rectangle reaches. */
function forTiles(rect: ChangeRect, visit: (key: number) => void): void;

// src/world/lots.ts:203-204 function sameRect
const sameRect = (a: ChangeRect | undefined, b: ChangeRect | undefined): boolean => …;

// src/world/lots.ts:209-210 function plannedStreet
/** Whether a street's land is planned: a ground street with footways. */
const plannedStreet = (doc: RoadDoc, r: SegmentRibbon): boolean => …;

// src/world/lots.ts:252-279 method LandPaving.tile
/** A tile's paved cells: only the cells within a planned street's box are asked, the rest is land. */
  private tile(ti: number, tj: number, key: number): Uint8Array | null;

// src/world/lots.ts:295-383 class LandLabels
/** The land's pieces for one plan, labelled where asked: each piece flooded once, from the first of its cells asked */
class LandLabels { … }

// src/world/lots.ts:390-390 type PlanOrder
/** A place in the plan's order: blocks by their first cell, row by row, then streets by id (the order the whole map is planned in). */
type PlanOrder = readonly [number, number, number];

// src/world/lots.ts:394-394 interface PlannedLot
/** A lot the plan proposes, with its box. */
interface PlannedLot { readonly key: string; readonly corners: Vec2[]; readonly box: ChangeRect }

// src/world/lots.ts:397-406 interface Planned
/** What a block or a street plans: its land's keys and the lots it proposes. */
interface Planned {
  /** Where what it plans reads the lots and the plan. */
  readonly reach: ChangeRect;
  /** A block's piece of land: its first cell and the rectangle of its cells; null for a street. */
  readonly piece: { readonly ci: number; readonly cj: number; readonly rect: ChangeRect } | null;
  readonly segment: SegmentId | null;
  readonly keys: readonly string[];
  readonly keyRects: readonly ChangeRect[];
  readonly made: readonly PlannedLot[];
}

// src/world/lots.ts:421-421 interface PlanWork
/** A block or a street to plan: a block by its piece (or, null, by its first cell asked again), a street by its id. */
interface PlanWork { readonly slot: string; readonly order: PlanOrder; block: number | null; readonly segment: SegmentId | null }

// src/world/lots.ts:429-429 function streetWork
const streetWork = (id: SegmentId): PlanWork => …;

// src/world/lots.ts:441-855 class LotPlanner
/** DO NOT plan the whole map for each edit: the plan kept by block and by street, each planned again where the network or the lots changed */
class LotPlanner { … }

// src/world/lots.ts:464-472 method LotPlanner.update
/** The plan for the land as it is now: planned again only where the network and the lots changed since the last. */
  update(): LotPlan;

// src/world/lots.ts:475-503 method LotPlanner.planAll
/** @rebuild The whole map planned: the first plan, a map loaded, or a change the diaries cannot place. */
  private planAll(): void;

// src/world/lots.ts:506-566 method LotPlanner.planChanged
/** The blocks and streets the network's changes and the lots' entries reach, planned again in order */
  private planChanged(delta: NetworkDelta, entries: readonly Change[]): void;

// src/world/lots.ts:569-610 method LotPlanner.run
/** Plans each block and street of `list` in order; `follow`: what a changed one reaches later in the order is planned again too */
  private run(list: PlanWork[], labels: LandLabels, paved: (p: Vec2) => boolean, follow: boolean): void;

// src/world/lots.ts:779-854 method LotPlanner.streetPlan
/** Open land along a street: a continuous strip each side, cut in equal lots (no snap afterwards: each lot is cut to the land) */
  private streetPlan(segId: SegmentId, order: PlanOrder, labels: LandLabels, paved: (p: Vec2) => boolean): Planned | null;

// src/world/lots.ts:858-858 variable LAND_CLEARANCE
/** How far a planned lot stands back from the paving: a hair, so what grows on it is never on the footway */
const LAND_CLEARANCE = m(0.08);

// src/world/lots.ts:861-868 function landLot
/** A proposed lot (four corners, the front first) cut back to the land: the paving taken off */
function landLot(net: Network, corners: readonly Vec2[]): Vec2[] | null;

// src/world/lots.ts:871-881 function freeSpans
/** The parts of `from..to` not under any of `taken`, each at least `min` long. */
function freeSpans(from: number, to: number, taken: readonly (readonly [number, number])[], min: number): [number, number][];

// src/world/lots.ts:884-892 function cutSpans
/** Each span cut into equal parts as near `target` long as a whole number of them allows. */
function cutSpans(spans: readonly (readonly [number, number])[], target: number): [number, number][];

// src/world/lots.ts:895-903 function applyLots
/** Stores a plan: the covered lots removed, the new ones added, the land's keys kept. */
export function applyLots(doc: RoadDoc, plan: LotPlan): boolean;

// src/world/lots.ts:925-940 function withFront
/** A piece of a cut lot with its front set: the side lying along the old front, or else the side nearest it */
function withFront(piece: readonly Vec2[], front: readonly [Vec2, Vec2]): Vec2[];

// src/world/lots.ts:1044-1060 function pavingNear
/** DO NOT go back to the union of the whole map's footways, made again at each network revision */
function pavingNear(net: Network, box: ChangeRect, grow = 0): MultiPoly | null;

// src/world/lots.ts:1063-1063 variable PAVING_MARGIN
/** Room round a lot's box the paving is read over: past the clearance, so the cut inside the box is the whole map's. */
const PAVING_MARGIN = m(2);

// src/world/lots.ts:1066-1066 variable CROSSED_AREA
/** Area a road takes off a lot under which it counts as crossing it: less is the paving's own outline round the lot, not a road through it. */
const CROSSED_AREA = m(1) * m(1);

// src/world/lots.ts:1069-1098 function cutLotsByRoads
/** A ROAD EDIT CUTS THE LOTS IT CROSSES (`main.ts` mutateBuilt, beside the buildings it razes): each lot the changed paving (`rects`; null, every lot) takes land from is replaced by the pieces of it left on land, each a lot of its zone where it is big enough, the rest let go - DO NOT leave a lot whole over a new road (its zone painted over the asphalt and nothing grown there, 2026-10-18). In the edit's own undo step. The lots cut. */
export function cutLotsByRoads(doc: RoadDoc, net: Network, rects: readonly ChangeRect[] | null): number;

// src/world/lots.ts:1101-1117 function onLand
/** A lot drawn over the street, cut back to the land: what falls on the paving is taken off, so the lot */
export function onLand(net: Network, points: readonly Vec2[], square = true, clearance = 0): Vec2[] | null;

// src/world/lots.ts:1120-1143 function squareCorners
/** A lot cut back to the footways, its corners made square */
function squareCorners(ring: Vec2[]): Vec2[];

// src/world/lots.ts:1267-1267 variable STRAIGHT_BACK
/** An outline edge at least this long is a straight back of a footway; shorter ones are a corner's curve. */
const STRAIGHT_BACK = m(2);

// src/world/lots.ts:1270-1279 function footwayEdgesNear
/** The outline of the paving a building may stand against round a box: the union of the footways (`pavingNear`) */
function footwayEdgesNear(net: Network, box: ChangeRect): number[];

// src/world/lots.ts:1282-1328 function lotSnapper
/** The lot snap: a point near a street is put on the outer edge of its footway as drawn */
export function lotSnapper(doc: RoadDoc, net: Network, lots?: readonly Pick<Lot, 'id' | 'corners'>[], apart?: Vec2): (p: Vec2, reach: number, skip?: number) => { p: Vec2; kind: 'corner' | 'street' | 'lot' | null };
