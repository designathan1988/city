// What the feature uses of src/world/streetFurniture.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/streetFurniture.ts:11-20 type FurnitureKind
/** Where every piece of street furniture stands. */

export type FurnitureKind =
  | 'lamp'
  | 'bin'
  | 'bench'
  | 'hydrant'
  | 'postbox'
  | 'phone'
  | 'drain'
  | 'streetTree'
  | 'shrub';

// src/world/streetFurniture.ts:22-45 interface FurnitureItem
export interface FurnitureItem {
  readonly kind: FurnitureKind;
  readonly x: number;
  readonly y: number;
  readonly segment: SegmentId;
  /** Unit tangent of the road at the item. */
  readonly along: Vec2;
  /** Unit normal pointing AWAY from the carriageway, towards the buildings. */
  readonly outward: Vec2;
  /** The surface the item stands on; the renderer adds the kerb's rise (a kerbed median stands as high as a footway). */
  readonly on: 'footway' | 'median';
  /** Plan footprint, for anything that must walk around it. */
  readonly radius: number;
  /** Set for a long thing (a bench): its half extent along `along`. */
  readonly halfLength?: number;
  /** Set for a long thing: its half extent along `outward`. */
  readonly halfWidth?: number;
  /** Set for a seat: the way somebody sitting on it faces. */
  readonly faces?: Vec2;
  /** A per-item number in [0, 1), stable across rebuilds, for variety. */
  readonly seed: number;
  /** When it was planted, city minutes (plants only); absent: fully grown. */
  readonly planted?: number;
}

// src/world/streetFurniture.ts:48-58 variable FURNITURE_OF
/** Each placed kind as the furniture the renderer and the walkers know. */
const FURNITURE_OF: Readonly<Partial<Record<LandscapeKind, FurnitureKind>>> = {
  tree: 'streetTree',
  shrub: 'shrub',
  bench: 'bench',
  bin: 'bin',
  lamp: 'lamp',
  hydrant: 'hydrant',
  postbox: 'postbox',
  phone: 'phone',
  drain: 'drain',
};

// src/world/streetFurniture.ts:61-61 variable STAND_REACH
/** How far off its footway's band a stored item may have drifted and still stand on it. */
const STAND_REACH = m(0.6);

// src/world/streetFurniture.ts:63-68 function hash01
function hash01(a: number, b: number): number;

// src/world/streetFurniture.ts:71-81 function furnitureOf
/** One placed item as the furniture standing where it was put: on the footway under it, or */
export function furnitureOf(
  net: Network,
  placed: LandscapeItem,
  onAccess: (segment: SegmentId, x: number, y: number, radius: number) => boolean = (segment, x, y, radius) => nearCrossingAccess(net, segment, x, y, radius),
): FurnitureItem | null;

// src/world/streetFurniture.ts:84-111 function standing
/** The furniture an item makes on the footway or the median under it, before the crossings are asked. */
function standing(net: Network, placed: LandscapeItem, kind: FurnitureKind): FurnitureItem | null;

// src/world/streetFurniture.ts:114-123 function streetFurniture
/** Everything standing on the footways: the items the player placed with the landscaping tool */
export function streetFurniture(net: Network): FurnitureItem[];
