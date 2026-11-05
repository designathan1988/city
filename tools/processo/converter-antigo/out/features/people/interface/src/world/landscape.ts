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

// src/world/landscape.ts:42-42 variable SIGN_TYPES
/** The signs the one sign tool makes (the player's order of 2026-10-05) */
export const SIGN_TYPES = ['stop', 'yield', 'speed', 'noParking', 'noEntry', 'pedestrian', 'school', 'direction', 'street', 'info'] as const;

// src/world/landscape.ts:43-43 type SignType
export type SignType = (typeof SIGN_TYPES)[number];

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
