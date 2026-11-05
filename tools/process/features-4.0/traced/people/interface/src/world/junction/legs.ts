// What the feature uses of src/world/junction/legs.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/junction/legs.ts:95-98 interface LegBuildOptions
export interface LegBuildOptions {
  /** Per-leg trim guesses, keyed by SEGMENT. */
  readonly trims?: ReadonlyMap<SegmentId, number>;
}

// src/world/junction/legs.ts:101-156 function buildLegs
/** Builds the legs of a node, sorted counter-clockwise by outgoing angle. */
export function buildLegs(
  doc: RoadDoc,
  cache: PolylineCache,
  nodeId: NodeId,
  level: SurfaceLevel,
  opts: LegBuildOptions = {},
): Leg[];
