// What the feature uses of src/sim/vehicles/laneClosures.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/vehicles/laneClosures.ts:6-13 interface Closure
/** Stretches of lane closed for a while by something off the lanes standing on them */
export interface Closure {
  readonly lanelet: LaneletId;
  /** The stretch closed, arc positions along the lane. */
  readonly from: number;
  readonly to: number;
  /** Who closed it. */
  readonly owner: number;
}

// src/sim/vehicles/laneClosures.ts:26-33 function openLanes
/** Every stretch `owner` closed, opened again. */
export function openLanes(w: SimWorld, owner: number): void;
