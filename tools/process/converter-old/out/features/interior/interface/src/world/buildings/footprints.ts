// What the feature uses of src/world/buildings/footprints.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildings/footprints.ts:10-15 function validOutline
/** Polygon outlines are normalized to the volume's bounding rectangle. */
export function validOutline(points: readonly Vec2[]): boolean;

// src/world/buildings/footprints.ts:17-20 function localFootprint
export function localFootprint(v: Volume): Vec2[];

// src/world/buildings/footprints.ts:22-22 function volumeSides
export const volumeSides = (v: Volume): FaceId[] => …;

// src/world/buildings/footprints.ts:24-24 interface EdgeFrame
export interface EdgeFrame { x: number; y: number; tx: number; ty: number; nx: number; ny: number; length: number }

// src/world/buildings/footprints.ts:31-38 function edgeFrame
/** DO NOT make a side's frame (and the block's ring) again each time it is asked, by side and storey: kept on the block, worked out again only when its place, size or outline changed; frozen, read only. */
export function edgeFrame(v: Volume, side: number): EdgeFrame;

// src/world/buildings/footprints.ts:51-51 function asPolygon
export const asPolygon = (ring: readonly Vec2[]): clipping.Polygon => …;

// src/world/buildings/footprints.ts:59-59 variable AREA_EPS
/** Two footprints overlapping by less than this area (a square of some 1.3 mm) only touch */
export const AREA_EPS = 1e-5 * TUNED_UNIT * TUNED_UNIT;

// src/world/buildings/footprints.ts:63-67 function overlapArea
/** The area two rings share. DO NOT clip two rings whose boxes are apart (a building's boundary piece against every ring of its neighbours, each a polygon clipping: 11.7 s of a tile maker's 89 s, 2026-10-16): no area in common, exactly. */
/** DO NOT sweep two convex rings with the general clipper (Martinez, `polygon-clipping`): nearly every block, lot and part is a rectangle, and the share of two convex rings is one ring clipped by the other's edges (Sutherland and Hodgman 1974). Only a ring that is not convex goes to the sweep. */
export function overlapArea(a: readonly Vec2[], b: readonly Vec2[]): number;

// src/world/buildings/footprints.ts:107-119 function coveredArea
/** The area of `ring` that lies over `supports`, the supports' own overlaps counted once: summed one by one where `ring` and every support are convex and no two supports overlap, else swept. */
function coveredArea(ring: readonly Vec2[], supports: readonly Vec2[][], area: number): number;

// src/world/buildings/footprints.ts:144-152 function containsPoint
export function containsPoint(v: Volume, p: Vec2): boolean;

// src/world/buildings/footprints.ts:154-157 function roofPartFits
export function roofPartFits(v: Volume, part: Pick<RoofDetail, 'x' | 'y' | 'w' | 'd' | 'rotation'>): boolean;

// src/world/buildings/footprints.ts:159-167 function roofDetailRing
export function roofDetailRing(part: Pick<RoofDetail, 'x' | 'y' | 'w' | 'd' | 'rotation'>): Vec2[];

// src/world/buildings/footprints.ts:170-181 function offsetRing
/** Small miter offsets for selection outlines and ground clearance. */
export function offsetRing(ring: readonly Vec2[], distance: number): Vec2[];
