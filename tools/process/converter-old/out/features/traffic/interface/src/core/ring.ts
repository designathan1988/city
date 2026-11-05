// What the feature uses of src/core/ring.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/ring.ts:27-202 class Ring
/** A closed boundary made of lines and exact arcs. */
export class Ring { … }

// src/core/ring.ts:84-86 getter Ring.isEmpty
get isEmpty(): boolean;

// src/core/ring.ts:89-95 method Ring.flatten
/** Vertices approximating the ring, first point NOT repeated at the end: a new list at each call (the caller may keep it). */
  flatten(): Vec2[];

// src/core/ring.ts:98-120 method Ring.flatXY
/** The vertices as x, y pairs (`flatten`), worked out at each call from the edges (the ring keeps none). */
  flatXY(): Float64Array;
