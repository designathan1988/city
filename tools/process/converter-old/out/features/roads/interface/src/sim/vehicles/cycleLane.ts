// What the feature uses of src/sim/vehicles/cycleLane.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/vehicles/cycleLane.ts:8-8 variable CYCLE_EASE
/** Bicycles in a cycle lane (ciclofaixa, `world/parking.ts` kind `cycle`). */

/** Distance at each end of a link over which a bicycle eases into and out of the cycle lane. */
const CYCLE_EASE = m(8);

// src/sim/vehicles/cycleLane.ts:10-10 function isBicycle
const isBicycle = (v: Vehicle): boolean => …;

// src/sim/vehicles/cycleLane.ts:13-19 function inside
/** 0 at the ends of the link, 1 along its middle. */
function inside(lane: Lanelet, centre: number): number;

// src/sim/vehicles/cycleLane.ts:22-25 function cycleShift
/** Sideways shift of a bicycle's body at front arc `s` on `lane` (0 for anything else). */
export function cycleShift(v: Vehicle, lane: Lanelet | undefined, s: number): number;

// src/sim/vehicles/cycleLane.ts:28-32 function outOfTheWay
/** True when `lead` rides in the cycle lane beside `follower`'s path, out of its way. */
export function outOfTheWay(follower: Vehicle, lead: Vehicle, lane: Lanelet | undefined): boolean;
