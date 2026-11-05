// What the feature uses of src/world/walkingLines.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/walkingLines.ts:27-27 variable JOIN
/** Ends of two footways at one road node closer than this are one point, u. */
export const JOIN = m(0.02);

// src/world/walkingLines.ts:74-149 class LocalLines
/** DO NOT merge the whole map's kerbs and paving for a corner (`levelPolygons`) */
export class LocalLines { … }

// src/world/walkingLines.ts:81-83 method LocalLines.polygons
/** The merged rings of a level on one deck, of the pieces in the box. */
  private polygons(level: SurfaceLevel, deck: RoadStructure): MultiPoly;

// src/world/walkingLines.ts:86-94 method LocalLines.paving
/** Everything paved for walking or driving on one deck, out to the footway's outer edge. */
  paving(deck: RoadStructure): MultiPoly;

// src/world/walkingLines.ts:137-148 method LocalLines.at
at(deck: RoadStructure, inset: number): Polyline[];

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

// src/world/walkingLines.ts:294-371 function roundCorner
/** The walking line round from one footway end to another, as drawn, or null; `avoid`: the other footway ends at the node. */
export function roundCorner(from: CornerEnd, to: CornerEnd, avoid: readonly Vec2[], lines: () => LocalLines): Polyline | null;

// src/world/walkingLines.ts:375-378 interface CornerAsk
/** The corners of one road node to round (`roundCorner`), as plain numbers for a worker (`conflictPool.ts`) */
export interface CornerAsk {
  readonly rings: readonly { readonly level: SurfaceLevel; readonly deck: RoadStructure; readonly polys: readonly (readonly Float64Array[])[] }[];
  readonly corners: readonly { readonly from: CornerEnd; readonly to: CornerEnd; readonly avoid: readonly Vec2[] }[];
}

// src/world/walkingLines.ts:381-383 interface CornerMeasurer
/** Rounds many nodes' corners somewhere else */
export interface CornerMeasurer {
  roundCorners?(asks: readonly CornerAsk[]): Promise<readonly (readonly (Float64Array | null)[])[]>;
}

// src/world/walkingLines.ts:386-392 function flatRings
/** A node's rings as an ask carries them: each ring's points as one flat x, y list. */
export function flatRings(polys: MultiPoly): Float64Array[][];
