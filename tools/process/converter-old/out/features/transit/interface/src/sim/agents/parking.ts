// What the feature uses of src/sim/agents/parking.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/agents/parking.ts:21-21 variable LANE_BESIDE_REACH
/** The parking bays of the city: where a resident's car stands when it is not on the road. */

/** How far from a point the lane beside it is looked for (`laneBeside`), u. */
export const LANE_BESIDE_REACH = LANE_REACH;

// src/sim/agents/parking.ts:26-29 interface BayWorld
/** DO NOT import `SimWorld`: what the bays read of a world (the simulation's, or a tile's own roads and buildings where it is baked, `sim/tileBays.ts`). */
export interface BayWorld extends KerbWorld, LaneGridWorld {
  /** A lane's runtime now (`SimWorld.rt`): whether it is a ghost. */
  rt(id: LaneletId): { readonly ghost: boolean };
}

// src/sim/agents/parking.ts:482-484 function laneBeside
/** The lane beside a point of a street (a bus stop): the nearest on the kerb side of that road, its point there. */
export function laneBeside(w: BayWorld, x: number, y: number, segment?: SegmentId): BayLane | null;
