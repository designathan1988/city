// What the feature uses of src/sim/agents/hybridAStar.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/agents/hybridAStar.ts:20-20 interface AxlePose
/** A pose of the rear axle: where it is and where the body points. */
export interface AxlePose { readonly x: number; readonly y: number; readonly angle: number }

// src/sim/agents/hybridAStar.ts:23-23 interface Waypoint
/** A pose on the way, and whether it is reached backing. */
export interface Waypoint extends AxlePose { readonly reverse: boolean }
