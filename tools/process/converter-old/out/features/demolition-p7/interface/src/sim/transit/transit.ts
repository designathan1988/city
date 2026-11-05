// What the feature uses of src/sim/transit/transit.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/transit/transit.ts:31-31 variable TRAIN_AHEAD
/** The track ahead of a moving train kept clear by walkers. */
const TRAIN_AHEAD = m(30);

// src/sim/transit/transit.ts:33-33 variable AHEAD_TIME
/** Seconds of a train's run ahead of it kept clear of walkers: time to walk across the track and more. */
const AHEAD_TIME = 7;

// src/sim/transit/transit.ts:42-42 variable TRAIN_CARS,METRO_CARS,CAR_LENGTH
/** Cars of a train, length of one. */
const TRAIN_CARS = 4, METRO_CARS = 3, CAR_LENGTH = m(18);

// src/sim/transit/transit.ts:66-74 interface Train
interface Train {
  readonly line: number;
  readonly key: string;
  s: number;
  v: number;
  /** Index of the station it is going to on the loop. */
  next: number;
  dwell: number;
}

// src/sim/transit/transit.ts:77-77 interface LevelCrossing
/** Where a train track crosses a lane of a road at grade: the lane, how far along it, how far along the line's loop. */
interface LevelCrossing { readonly lanelet: string; readonly at: number; readonly loopS: number }

// src/sim/transit/transit.ts:79-90 interface RailLine
interface RailLine {
  readonly line: TransitLine;
  /** Where it crosses the roads' lanes: made again when a lane near it changes (`followLanes`). */
  crossings: LevelCrossing[];
  /** The loop out and back: points and the distance along it of each. */
  readonly points: Vec2[];
  readonly at: number[];
  readonly length: number;
  /** Each station on the loop: the stop's id, its distance along it, and whether the loop turns back there */
  readonly stations: { readonly stop: number; readonly s: number; readonly turn: boolean }[];
  readonly trains: Train[];
}

// src/sim/transit/transit.ts:101-548 class TransitSim
export class TransitSim { … }

// src/sim/transit/transit.ts:109-109 field TransitSim.rails
private rails: RailLine[] = [];

// src/sim/transit/transit.ts:446-461 method TransitSim.pointAt
/** The point and heading at a distance along a line's loop. */
  private pointAt(rail: RailLine, s: number): Vec2 & { heading: number };
