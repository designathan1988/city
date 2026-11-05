// What the feature uses of src/sim/vehicles/driver.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/vehicles/driver.ts:7-18 interface DriverParams
/** The driver behind the wheel, as distinct from the vehicle around them. ## Why this exists Every sedan braked identically */

/** The subset of an archetype the longitudinal model actually reads. */
export interface DriverParams {
  /** Comfortable acceleration. */
  readonly a: number;
  /** Comfortable deceleration. */
  readonly b: number;
  /** Emergency deceleration, used by the safe-speed cap. */
  readonly bEmergency: number;
  /** Desired time headway, in seconds. */
  readonly T: number;
  /** Standstill gap. */
  readonly s0: number;
}

// src/sim/vehicles/driver.ts:20-37 interface Driver
export interface Driver extends DriverParams {
  /** -1 timid, 0 ordinary, +1 pushy. */
  readonly aggression: number;
  /** Multiplies the critical gap a yielding movement demands. */
  readonly gapFactor: number;
  /** MOBIL politeness: how much this driver weighs the cars they affect. */
  readonly politeness: number;
  /** MOBIL threshold: the gain an overtake has to be worth. */
  readonly laneThreshold: number;
  /** Amplitude of the slow wander in desired speed, as a fraction. */
  readonly driftAmplitude: number;
  /** Angular rate of that wander, in radians per second. */
  readonly driftRate: number;
  /** Phase of it, so no two drivers wander together. */
  readonly driftPhase: number;
  /** Seconds of being held up before this driver looks for another way round. */
  readonly patience: number;
}

// src/sim/vehicles/driver.ts:48-71 function makeDriver
/** Draws a driver. */
export function makeDriver(archetype: Archetype, sample: () => number): Driver;
