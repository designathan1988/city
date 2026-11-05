// What the feature uses of src/world/landscape.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/landscape.ts:13-13 variable LANDSCAPE_KINDS
/** Street landscaping the PLAYER places */

export const LANDSCAPE_KINDS = ['tree', 'shrub', 'bench', 'bin', 'lamp', 'hydrant', 'postbox', 'phone', 'drain', 'meadow', 'sign', 'streetname'] as const;

// src/world/landscape.ts:14-14 type LandscapeKind
export type LandscapeKind = (typeof LANDSCAPE_KINDS)[number];

// src/world/landscape.ts:16-27 interface LandscapeItem
export interface LandscapeItem {
  readonly id: number;
  readonly kind: LandscapeKind;
  readonly x: number;
  readonly y: number;
  /** A sign's type (`SIGN_TYPES`). */
  readonly signType?: SignType;
  /** What a sign says, or a street's name. */
  readonly text?: string;
  /** When a plant was planted, in city minutes (`City.minutes`, `sim/city/city.ts`); absent: fully grown. */
  readonly planted?: number;
}

// src/world/landscape.ts:30-30 variable GROW_MINUTES
/** City minutes a planted tree or shrub takes to reach its full size: three days. */
export const GROW_MINUTES = 3 * 1440;

// src/world/landscape.ts:32-32 variable PLANTED_SIZE
/** The size a plant is planted at, as a share of its full size. */
const PLANTED_SIZE = 0.3;

// src/world/landscape.ts:42-42 variable SIGN_TYPES
/** The signs the one sign tool makes (the player's order of 2026-10-05) */
export const SIGN_TYPES = ['stop', 'yield', 'speed', 'noParking', 'noEntry', 'pedestrian', 'school', 'direction', 'street', 'info'] as const;

// src/world/landscape.ts:43-43 type SignType
export type SignType = (typeof SIGN_TYPES)[number];

// src/world/landscape.ts:57-70 variable LANDSCAPE_RADIUS
/** Plan radius of each kind, for spacing and for anyone walking round it. */
export const LANDSCAPE_RADIUS: Readonly<Record<LandscapeKind, number>> = {
  tree: TREE_PIT / 2,
  shrub: m(0.35),
  bench: Math.hypot(m(0.9), m(0.26)),
  bin: m(0.33),
  lamp: m(0.13),
  hydrant: m(0.16),
  postbox: m(0.33),
  phone: m(0.45),
  drain: m(0.5),
  meadow: m(2),
  sign: m(0.15),
  streetname: m(1),
};

// src/world/landscape.ts:73-73 variable ITEM_GAP
/** The least clear distance kept between two placed items, besides their radii. */
const ITEM_GAP = m(0.4);

// src/world/landscape.ts:76-86 interface FootwayHit
/** Where a point lies on the footways: the road, the station along it and the side. */
export interface FootwayHit {
  readonly segment: SegmentId;
  readonly road: RoadType;
  /** Station along the ribbon's full centreline. */
  readonly s: number;
  /** +1 left of the segment's a -> b direction, -1 right. */
  readonly side: 1 | -1;
  /** Distance of the point from the centreline. */
  readonly across: number;
  readonly frame: { readonly p: Vec2; readonly t: Vec2; readonly n: Vec2 };
}

// src/world/landscape.ts:89-91 function ribbonsNear
/** The ribbons a point within `reach` of a road's surface can be on */
function ribbonsNear(net: Network, at: Vec2, reach: number): Iterable<SegmentRibbon>;

// src/world/landscape.ts:94-124 function footwayAt
/** The footway a point stands on, or, with `reach`, the nearest one within that distance of the footway's own band. */
export function footwayAt(net: Network, at: Vec2, reach = 0): FootwayHit | null;

// src/world/landscape.ts:127-133 interface MedianHit
/** A planted median a point stands on: trees and shrubs go there too. */
export interface MedianHit {
  readonly segment: SegmentId;
  readonly s: number;
  /** The median's centre at the station. */
  readonly centre: Vec2;
  readonly frame: { readonly p: Vec2; readonly t: Vec2; readonly n: Vec2 };
}

// src/world/landscape.ts:136-136 variable MEDIAN_TREE_MIN
/** Narrowest median a tree is planted in: its pit and a kerb's width each side. */
const MEDIAN_TREE_MIN = m(1.5);

// src/world/landscape.ts:145-145 variable MEDIAN_NOSE_GAP
/** Room left between a crossing's far edge and a median island's nose, world units. */
const MEDIAN_NOSE_GAP = m(0.5);

// src/world/landscape.ts:158-158 variable MEDIAN_UTURN_OPENING
/** How far each side of a U-turn's node its median opening runs: a car's half circle and half its length. */
export const MEDIAN_UTURN_OPENING = m(10);

// src/world/landscape.ts:224-224 type LandscapeRefusal
type LandscapeRefusal = 'offFootway' | 'narrow' | 'occupied' | 'crossing';

// src/world/landscape.ts:227-234 interface CrossingAccess
/** The landing of a crossing on the footways: kept clear of everything. */
export interface CrossingAccess {
  readonly x: number;
  readonly y: number;
  readonly tx: number;
  readonly ty: number;
  readonly across: number;
  readonly structure: string;
}

// src/world/landscape.ts:237-237 variable accessMemo
/** Every crossing's approach, including its landing on each footway. */
const accessMemo = new WeakMap<Network, { revision: number; trafficRevision: number; list: CrossingAccess[] }>();

// src/world/landscape.ts:251-262 function crossingAccessesAt
/** DO NOT work a landing out here again. */
export function crossingAccessesAt(net: Network, nodes: Iterable<NodeId>): CrossingAccess[];

// src/world/landscape.ts:278-278 variable ACCESS_REACH
/** How far from a point a crossing's landing can reach it, besides the item's own radius */
export const ACCESS_REACH = m(40);

// src/world/landscape.ts:293-293 variable ACCESS_CELL
/** The side of the cells `crossingAccessTester` files the landings in. */
const ACCESS_CELL = m(32);

// src/world/landscape.ts:315-317 type LandscapeSnap
export type LandscapeSnap =
  | { readonly ok: true; readonly at: Vec2; readonly hit: FootwayHit | null; readonly median?: MedianHit }
  | { readonly ok: false; readonly at: Vec2; readonly reason: LandscapeRefusal };

// src/world/landscape.ts:395-395 variable MAX_ITEM_RADIUS
/** The largest radius of a placed item (`LANDSCAPE_RADIUS`). */
/** The widest item's radius: what a search round a point grows by to meet every item reaching it. */
export const MAX_ITEM_RADIUS = Math.max(...Object.values(LANDSCAPE_RADIUS));

// src/world/landscape.ts:398-401 function landscapeNearIn
/** DO NOT hand `doc.landscape.values()` to `landscapeNear` for one point: every item of the map, every move. */
export function landscapeNearIn(doc: { landscapeIn(rect: readonly [number, number, number, number]): LandscapeItem[] }, at: Vec2, radius: number): LandscapeItem | null;

// src/world/landscape.ts:412-414 function apart
/** How far `a` is from `b`. */
function apart(a: Vec2, b: Vec2): number;
