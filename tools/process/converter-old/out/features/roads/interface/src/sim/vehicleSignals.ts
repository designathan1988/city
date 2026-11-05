// What the feature uses of src/sim/vehicleSignals.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/vehicleSignals.ts:10-10 type IndicatorSide
/** What a vehicle is signalling and how its wheels sit, read straight off the simulation. */

/** +1 indicating left, -1 right, 0 none — in the vehicle's own frame. */
export type IndicatorSide = -1 | 0 | 1;

// src/sim/vehicleSignals.ts:15-15 variable TURN_SIGNAL_TIME
/** A driver signals a turn this long, or this far, before the stop line. */
const TURN_SIGNAL_TIME = 4;

// src/sim/vehicleSignals.ts:16-16 variable TURN_SIGNAL_MIN
const TURN_SIGNAL_MIN = m(25);

// src/sim/vehicleSignals.ts:18-18 variable TURN_SIGNAL_UNTIL
/** And keeps signalling until this share of the turn is driven. */
const TURN_SIGNAL_UNTIL = 0.6;

// src/sim/vehicleSignals.ts:27-27 variable HERE
/** Which way a vehicle is signalling. */
/** What `indicatorSide` reads of the two lanes, made once. */
const HERE = newFrame();

// src/sim/vehicleSignals.ts:28-28 variable THERE
const THERE = newFrame();

// src/sim/vehicleSignals.ts:29-29 variable HIT
const HIT = { s: 0, distance: 0 };

// src/sim/vehicleSignals.ts:31-65 function indicatorSide
export function indicatorSide(w: SimWorld, v: Vehicle): IndicatorSide;

// src/sim/vehicleSignals.ts:67-71 function turnSide
function turnSide(turn: string | undefined): IndicatorSide;

// src/sim/vehicleSignals.ts:92-92 variable BEFORE
/** Curvature of the path the vehicle is driving under its front axle, per world unit, positive turning left */
/** The two tangents `pathCurvature` reads: made once (every vehicle drawn, every frame). */
const BEFORE = newFrame();

// src/sim/vehicleSignals.ts:93-93 variable AFTER
const AFTER = newFrame();

// src/sim/vehicleSignals.ts:95-112 function pathCurvature
export function pathCurvature(w: SimWorld, v: Vehicle): number;
