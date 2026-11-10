// What the feature uses of src/sim/vehicles/idm.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/vehicles/idm.ts:6-16 interface Obstacle
/** Anything a vehicle must not run into */
export interface Obstacle {
  /** Bumper-to-bumper distance ahead. May be clamped to a small positive. */
  readonly gap: number;
  /** Speed of the obstacle; zero for anything stationary. */
  readonly speed: number;
  readonly kind: ObstacleKind;
  /** Excluded from the hard safe-speed cap when false. */
  readonly hard?: boolean | undefined;
  /** Its acceleration, when it is a vehicle: what it is doing, not just how fast. */
  readonly accel?: number | undefined;
}

// src/sim/vehicles/idm.ts:18-27 type ObstacleKind
export type ObstacleKind =
  | 'vehicle'
  | 'signal'
  | 'conflict'
  | 'pedestrian'
  | 'spillback'
  | 'yield'
  | 'endOfRoute'
  | 'curvature'
  | 'kerbStop';

// src/sim/vehicles/idm.ts:30-36 interface MutableObstacle
/** An obstacle the set owns and may write again (its fields as the set reads them). */
export interface MutableObstacle {
  gap: number;
  speed: number;
  kind: ObstacleKind;
  hard?: boolean | undefined;
  accel?: number | undefined;
}

// src/sim/vehicles/idm.ts:38-44 interface ConstraintSet
export interface ConstraintSet {
  obstacles: Obstacle[];
  /** The obstacle written at each place this tick, kept from the ticks before: an obstacle a vehicle does not carry a tick (a leader, a signal) is written in the same object, never a new one. */
  readonly pool: MutableObstacle[];
  /** How many of `pool` this tick's obstacles take (`resetConstraints` puts it back to none). */
  used: number;
}

// src/sim/vehicles/idm.ts:63-68 function addObstacle
/** An obstacle written into one of the set's slots and appended: the fields it does not carry left undefined (not the last tick's). */
export function addObstacle(c: ConstraintSet, gap: number, speed: number, kind: ObstacleKind, accel?: number, hard?: boolean): void;

// src/sim/vehicles/idm.ts:86-97 function idmAccel
/** Intelligent Driver Model acceleration for one obstacle. */
export function idmAccel(p: DriverParams, v: number, v0: number, o: Obstacle): number;

// src/sim/vehicles/idm.ts:108-109 function canStopComfortably
/** True when the vehicle can still stop before `d` without harsh braking. */
export const canStopComfortably = (p: DriverParams, v: number, d: number): boolean => …;
