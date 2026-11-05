// What the feature uses of src/world/cityGen/streets.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/cityGen/streets.ts:262-268 interface StreetEdge
export interface StreetEdge {
  readonly a: number;
  readonly b: number;
  /** The points between the two nodes, in order from a to b. */
  readonly points: readonly Vec2[];
  readonly level: number;
}

// src/world/cityGen/streets.ts:270-273 interface StreetGraph
export interface StreetGraph {
  readonly nodes: readonly Vec2[];
  readonly edges: readonly StreetEdge[];
}
