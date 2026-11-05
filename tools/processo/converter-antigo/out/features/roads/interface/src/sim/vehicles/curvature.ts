// What the feature uses of src/sim/vehicles/curvature.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/vehicles/curvature.ts:12-12 variable SAMPLE
/** How fast a driver is willing to take the road ahead, from its curvature. */

/** Spacing of the curvature profile along a lanelet. */
const SAMPLE = m(2);

// src/sim/vehicles/curvature.ts:14-14 variable CHORD
/** Half-length of the chord the curvature is measured over. */
const CHORD = m(3);

// src/sim/vehicles/curvature.ts:23-23 variable profiles
const profiles = new WeakMap<Lanelet, Float32Array>();

// src/sim/vehicles/curvature.ts:25-25 variable peaks
/** Each profile's sharpest sample: below `STRAIGHT` the whole lanelet is straight. */
const peaks = new WeakMap<Lanelet, number>();

// src/sim/vehicles/curvature.ts:27-27 variable STRAIGHT
/** Curvature taken as none at all. */
const STRAIGHT = 1e-6;

// src/sim/vehicles/curvature.ts:30-49 function profileOf
/** Curvature, per world unit, at each `SAMPLE` along the lanelet. */
function profileOf(lane: Lanelet): Float32Array;

// src/sim/vehicles/curvature.ts:52-55 function straight
/** Whether a lanelet has no bend anywhere (its profile made first if need be). */
function straight(lane: Lanelet): boolean;

// src/sim/vehicles/curvature.ts:64-70 function bendSpeed
/** Fastest speed at which a bend of curvature `k` is taken within the driver's speed-dependent comfort */
function bendSpeed(share: number, k: number): number;

// src/sim/vehicles/curvature.ts:73-81 function slowestBend
/** The speed this vehicle takes the sharpest bend of a lanelet at, within its comfort. */
export function slowestBend(v: Vehicle, lane: Lanelet): number;

// src/sim/vehicles/curvature.ts:84-126 function curveSpeedCap
/** Highest speed at which this vehicle can drive on now and still take every bend within its look-ahead at a comfortable */
export function curveSpeedCap(w: SimWorld, v: Vehicle): number;
