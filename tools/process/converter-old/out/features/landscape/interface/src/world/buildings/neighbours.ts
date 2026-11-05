// What the feature uses of src/world/buildings/neighbours.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildings/neighbours.ts:12-15 interface Obstacle
/** A STANDING BUILDING AS ITS NEIGHBOURS READ IT: what a building checked or grown beside it asks of it (`validate.ts` `validateOn`, `world/growth/zoning.ts`) - where it stands, what it puts on the ground, what it looks like, how tall it is - and nothing else of the record. On the page a neighbour is a stored record read through (`neighbourOf`), each answer worked out once per record; off the page (`editor/growthReplica.ts`) it is the same answers worked out once from the record sent, the record itself let go: the growth's worker keeps what the checks read, not a second copy of every record of the town (`settledNeighbour`). Both are worked out by the same functions here, so a check reads the same numbers on either side. */

/** A ring a building puts on the ground (world units) and its box. */
export interface Obstacle {
  readonly ring: readonly Vec2[];
  readonly box: Aabb;
}

// src/world/buildings/neighbours.ts:17-35 interface Neighbour
export interface Neighbour {
  readonly id: BuildingId;
  /** Its anchor (`Building.x`, `y`): the front middle of its lot when grown. */
  readonly x: number;
  readonly y: number;
  /** Its box as the store files it (`storedBounds`). */
  readonly bounds: Aabb;
  /** Its box grown by the deepest a projection may be (`storedBounds(b, MAX_PROJECTION)`). */
  readonly reach: Aabb;
  /** What it puts on the ground: its footprints, then its projections, then its ground elements. */
  readonly obstacles: readonly Obstacle[];
  /** How many of `obstacles` are footprints, and how many projections follow them. */
  readonly footprints: number;
  readonly projections: number;
  /** What it looks like (`procedural.ts` `bodySignature`). */
  readonly signature: Signature;
  /** Its height in storeys: the top of its tallest built block (0: none). */
  readonly storeys: number;
}

// src/world/buildings/neighbours.ts:38-40 interface NeighbourSource
/** Where a neighbour is asked for: the buildings by place (`BuildingStore.near`, a superset). */
export interface NeighbourSource {
  near(minX: number, minY: number, maxX: number, maxY: number): readonly Neighbour[];
}
