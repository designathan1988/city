// What the feature uses of src/sim/intersections/spillback.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/intersections/spillback.ts:9-62 function hasDownstreamStorage
/** Don't-block-the-box, measured from real occupancy. */
/** `strict`: a movement that gives way */
export function hasDownstreamStorage(w: SimWorld, v: Vehicle, conn: Connector, strict = false): boolean;

// src/sim/intersections/spillback.ts:65-77 function restingRear
/** Where the rear of the last vehicle on a lane will come to rest. */
function restingRear(w: SimWorld, order: readonly number[]): number;

// src/sim/intersections/spillback.ts:90-93 function endsAtSignal
/** Whether a lane ends at a signalised junction. */
function endsAtSignal(w: SimWorld, node: NodeId | undefined): boolean;
