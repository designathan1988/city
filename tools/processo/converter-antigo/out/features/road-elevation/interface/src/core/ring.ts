// What the feature uses of src/core/ring.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/ring.ts:27-202 class Ring
/** A closed boundary made of lines and exact arcs. */
export class Ring { … }

// src/core/ring.ts:33-33 field Ring.data
/**
   * DO NOT keep a ring as objects - an edge an object, a point an object, its flattened vertices a list of points (~600 B a ring, 2 KB
   * a building of the network's junctions and roads, 2026-10-21): one run of doubles (`AREA`..`EDGES`, a plain array of numbers: no
   * typed array's ~200 B of buffer a ring), the edges and the vertices made from it when asked for.
   */
  private readonly data: number[];

// src/core/ring.ts:34-34 field Ring.count
private readonly count: number;

// src/core/ring.ts:84-86 getter Ring.isEmpty
get isEmpty(): boolean;

// src/core/ring.ts:123-139 getter Ring.bbox
/** The box of the flattened ring (a new object at each call; the numbers are kept). */
  get bbox(): Aabb;
