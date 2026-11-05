// What the feature uses of src/world/walkingLines.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/walkingLines.ts:81-83 method LocalLines.polygons
/** The merged rings of a level on one deck, of the pieces in the box. */
  private polygons(level: SurfaceLevel, deck: RoadStructure): MultiPoly;

// src/world/walkingLines.ts:86-94 method LocalLines.paving
/** Everything paved for walking or driving on one deck, out to the footway's outer edge. */
  paving(deck: RoadStructure): MultiPoly;

// src/world/walkingLines.ts:100-115 method LocalLines.onPaving
/** Whether a point is on the paving of a deck. */
  onPaving(deck: RoadStructure, p: Vec2): boolean;

// src/world/walkingLines.ts:119-123 method LocalLines.nearestOuter
/** The nearest point of the footway's outer edge on one deck, as the nearest of `outer`'s lines' `closestPoint`s */
  nearestOuter(deck: RoadStructure, p: Vec2): { point: Vec2; distance: number } | null;

// src/world/walkingLines.ts:126-135 method LocalLines.outer
/** The footway's outer edge on one deck: the outline of the paving, as drawn. */
  outer(deck: RoadStructure): Polyline[];

// src/world/walkingLines.ts:137-148 method LocalLines.at
at(deck: RoadStructure, inset: number): Polyline[];

// src/world/walkingLines.ts:152-220 class NearestEdge
/** The pieces of some lines on a grid: the nearest point among all of them, reading only the cells near it. */
class NearestEdge { … }

// src/world/walkingLines.ts:224-224 interface PavingRing
interface PavingRing { readonly points: Vec2[]; readonly minX: number; readonly minY: number; readonly maxX: number; readonly maxY: number }

// src/world/walkingLines.ts:225-230 function pavingRing
function pavingRing(ring: readonly (readonly number[])[]): PavingRing;

// src/world/walkingLines.ts:232-234 function inRing
/** `pointInPolygon`, skipped when the point is outside the ring's box. */
function inRing(p: Vec2, r: PavingRing): boolean;

// src/world/walkingLines.ts:282-282 type RingsOf
/** The rings of a level on one deck round one node, before they are merged (`levelRingsOf` of the pieces in its box). */
export type RingsOf = (level: SurfaceLevel, deck: RoadStructure) => MultiPoly;

// src/world/walkingLines.ts:285-291 interface CornerEnd
/** One footway's end at a road node, as a corner reads it: where, walking into the node which way, its inset and width, its deck. */
export interface CornerEnd {
  readonly p: Vec2;
  readonly into: Vec2;
  readonly inset: number;
  readonly width: number;
  readonly structure: RoadStructure;
}

// src/world/walkingLines.ts:375-378 interface CornerAsk
/** The corners of one road node to round (`roundCorner`), as plain numbers for a worker (`conflictPool.ts`) */
export interface CornerAsk {
  readonly rings: readonly { readonly level: SurfaceLevel; readonly deck: RoadStructure; readonly polys: readonly (readonly Float64Array[])[] }[];
  readonly corners: readonly { readonly from: CornerEnd; readonly to: CornerEnd; readonly avoid: readonly Vec2[] }[];
}
