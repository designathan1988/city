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

// src/sim/vehicles/laneClosures.ts:15-15 variable CLOSED
const CLOSED = new WeakMap<SimWorld, Map<LaneletId, Closure[]>>();

// src/sim/vehicles/laneClosures.ts:16-16 variable NONE
const NONE: readonly Closure[] = [];

// src/sim/vehicles/laneClosures.ts:36-38 function closedOn
/** The stretches closed on a lane now. */
export function closedOn(w: SimWorld, lanelet: LaneletId): readonly Closure[];

// src/sim/vehicles/laneClosures.ts:41-44 function anyClosed
/** Whether any lane is closed at all (the commonest answer is no, and costs one look). */
export function anyClosed(w: SimWorld): boolean;

// src/sim/vehicles/laneClosures.ts:47-47 variable CLEAR_MARGIN
/** Kept between a closed stretch and the nearest body on it or past it. */
const CLEAR_MARGIN = m(2);

// src/sim/vehicles/laneClosures.ts:50-75 function laneClear
/** Whether a stretch of lane may be closed now */
export function laneClear(w: SimWorld, lanelet: LaneletId, from: number, to: number, stopShort: number, margin = CLEAR_MARGIN): boolean;

// src/sim/vehicles/laneClosures.ts:78-89 function lanesInto
/** The lanes a vehicle enters `id` from: a connector's own approach, or the connectors into a link at its junction. */
function lanesInto(w: SimWorld, id: LaneletId): LaneletId[];
