// What the feature uses of src/world/walkingLines.ts (shared with the program): declarations as the source writes them, bodies left out.

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

// src/world/walkingLines.ts:381-383 interface CornerMeasurer
/** Rounds many nodes' corners somewhere else */
export interface CornerMeasurer {
  roundCorners?(asks: readonly CornerAsk[]): Promise<readonly (readonly (Float64Array | null)[])[]>;
}
