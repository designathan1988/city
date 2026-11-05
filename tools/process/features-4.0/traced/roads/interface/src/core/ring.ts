// What the feature uses of src/core/ring.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/ring.ts:20-20 variable AREA,MIN_X,MIN_Y,MAX_X,MAX_Y,START,EDGES
/** Where a ring's numbers stand in its run (`Ring.data`): its area and box (NaN until first asked), its start, then its edges. */
const AREA = 0, MIN_X = 1, MIN_Y = 2, MAX_X = 3, MAX_Y = 4, START = 5, EDGES = 7;

// src/core/ring.ts:22-22 variable LINE,ARC_CCW,ARC_CW
/** An edge's code, first of its numbers: a line (then its end), an arc either way (then its end, centre, radius and two angles). */
const LINE = 0, ARC_CCW = 1, ARC_CW = 2;

// src/core/ring.ts:24-24 variable LINE_SIZE,ARC_SIZE
/** How many numbers an edge of each kind takes. */
const LINE_SIZE = 3, ARC_SIZE = 8;

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

// src/core/ring.ts:89-95 method Ring.flatten
/** Vertices approximating the ring, first point NOT repeated at the end: a new list at each call (the caller may keep it). */
  flatten(): Vec2[];

// src/core/ring.ts:98-120 method Ring.flatXY
/** The vertices as x, y pairs (`flatten`), worked out at each call from the edges (the ring keeps none). */
  flatXY(): Float64Array;

// src/core/ring.ts:123-139 getter Ring.bbox
/** The box of the flattened ring (a new object at each call; the numbers are kept). */
  get bbox(): Aabb;
