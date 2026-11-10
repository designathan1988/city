// What the feature uses of src/sim/routing/lowerBound.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/routing/lowerBound.ts:6-6 variable FASTEST_LIMIT
/** DO NOT work out the fastest lane of the graph here */
const FASTEST_LIMIT = kmh(ROAD_SECTION_LIMITS.speedKmh[1]);

// src/sim/routing/lowerBound.ts:9-11 interface LaneLookup
/** DO NOT import `SimWorld`: the world reaches this file, and the type import closed an import cycle */
export interface LaneLookup {
  lanelet(id: LaneletId): Lanelet | undefined;
}

// src/sim/routing/lowerBound.ts:30-33 function goalEnd
/** The end of the goal lane, for `laneLowerBound`; null when there is no goal lane. */
export function goalEnd(w: LaneLookup, goal: LaneletId): { readonly x: number; readonly y: number } | null;

// src/sim/routing/lowerBound.ts:36-41 function laneLowerBound
/** The bound to the goal ending at `end` from the end of lane `id`, seconds (`tripLowerBound` works each out once a search). */
export function laneLowerBound(w: LaneLookup, id: LaneletId, end: { readonly x: number; readonly y: number }): number;
