// What the feature uses of src/sim/transit/transit.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/transit/transit.ts:25-25 variable APPROACH
/** How far back from its first stop a bus is put on the road, to drive up to it. */
const APPROACH = m(25);

// src/sim/transit/transit.ts:29-29 variable AT_STOP,STOPPED
/** A bus is at its stop this near the stopping point, below this speed. */
const AT_STOP = m(2.5), STOPPED = m(0.3);

// src/sim/transit/transit.ts:31-31 variable TRAIN_AHEAD
/** The track ahead of a moving train kept clear by walkers. */
const TRAIN_AHEAD = m(30);

// src/sim/transit/transit.ts:33-33 variable AHEAD_TIME
/** Seconds of a train's run ahead of it kept clear of walkers: time to walk across the track and more. */
const AHEAD_TIME = 7;

// src/sim/transit/transit.ts:35-35 variable CROSSING_STOP
/** How far short of a level crossing a vehicle waits, u. */
export const CROSSING_STOP = m(5);

// src/sim/transit/transit.ts:37-37 variable KEEP_CLEAR
/** A crossing closer than this to a stop line has the traffic wait before it (a bus and room to spare). */
const KEEP_CLEAR = m(20);

// src/sim/transit/transit.ts:39-39 variable CROSSING_WARN
/** A level crossing closes when a train is this far from it. */
const CROSSING_WARN = m(160);

// src/sim/transit/transit.ts:40-40 variable NONE
const NONE: number[] = [];

// src/sim/transit/transit.ts:42-42 variable TRAIN_CARS,METRO_CARS,CAR_LENGTH
/** Cars of a train, length of one. */
const TRAIN_CARS = 4, METRO_CARS = 3, CAR_LENGTH = m(18);

// src/sim/transit/transit.ts:45-45 variable AWAY_MARGIN
/** How far past the view's reach a bus on the road is taken off it (`stepBus`): off and on again at the edge would be seen. */
const AWAY_MARGIN = m(200);

// src/sim/transit/transit.ts:51-53 function travelTime
/** Seconds a bus takes between two points `distance` apart, on streets of `limit`. */
function travelTime(distance: number, limit: number): number;

// src/sim/transit/transit.ts:55-64 interface Bus
interface Bus {
  readonly line: number;
  id: VehicleId | null;
  /** The index of the stop it is going to, and the way along the line (+1 out, -1 back). */
  next: number;
  dir: 1 | -1;
  dwell: number;
  /** Seconds before it is put on the road (spaced from the bus before). */
  wait: number;
}

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

// src/sim/transit/transit.ts:107-107 field TransitSim.stopLanes
/** Each bus stop's lane point (where a bus stands beside it). */
  private stopLanes = new Map<number, BayLane>();

// src/sim/transit/transit.ts:109-109 field TransitSim.rails
private rails: RailLine[] = [];

// src/sim/transit/transit.ts:117-153 method TransitSim.rebuild
/** @rebuild The lines and their vehicles made again from the map as it is. */
  private rebuild(w: SimWorld): void;

// src/sim/transit/transit.ts:156-175 method TransitSim.placeBuses
/** A bus line's buses: those it had kept where they are, each sent on again; more put at its terminal, up to its number. */
  private placeBuses(w: SimWorld, line: TransitLine, was: readonly Bus[], kept: Set<Bus>): void;

// src/sim/transit/transit.ts:236-236 field TransitSim.crossingsByLane
/** DO NOT walk every line's crossings for a lane: every crossing of the town, for each vehicle and each lane ahead. */
  private readonly crossingsByLane = new Map<string, { readonly rail: RailLine; readonly crossing: LevelCrossing }[]>();

// src/sim/transit/transit.ts:239-286 method TransitSim.followLanes
/** DO NOT make every line again for an edit of the roads */
  private followLanes(w: SimWorld): void;

// src/sim/transit/transit.ts:289-298 method TransitSim.step
/** One tick of every line. */
  step(w: SimWorld): void;

// src/sim/transit/transit.ts:305-348 method TransitSim.stepBus
private stepBus(w: SimWorld, bus: Bus): void;

// src/sim/transit/transit.ts:351-357 method TransitSim.stopPoint
/** Where a stop's bus stands (the point of its lane beside it) and the lane's speed; null when it has no lane. */
  private stopPoint(w: SimWorld, stopId: number): { x: number; y: number; speed: number } | null;

// src/sim/transit/transit.ts:369-385 method TransitSim.sendBus
/** A bus driven on to its next stop: the lane beside it as where it stops. */
  private sendBus(w: SimWorld, line: TransitLine, bus: Bus, v: Vehicle): void;

// src/sim/transit/transit.ts:387-409 method TransitSim.spawnBus
private spawnBus(w: SimWorld, line: TransitLine, bus: Bus): VehicleId | null;

// src/sim/transit/transit.ts:446-461 method TransitSim.pointAt
/** The point and heading at a distance along a line's loop. */
  private pointAt(rail: RailLine, s: number): Vec2 & { heading: number };

// src/sim/transit/transit.ts:554-557 function sameLane
/** Whether a stop's lane point is the same before and after. */
function sameLane(a: { lanelet: string; at: number; x: number; y: number } | undefined, b: { lanelet: string; at: number; x: number; y: number } | null): boolean;

// src/sim/transit/transit.ts:560-580 function levelCrossings
/** DO NOT go back to the loop over every lane. */
export function levelCrossings(w: SimWorld, points: readonly Vec2[], at: readonly number[]): LevelCrossing[];

// src/sim/transit/transit.ts:582-582 variable NO_CROSSINGS
const NO_CROSSINGS: readonly { readonly rail: RailLine; readonly crossing: LevelCrossing }[] = Object.freeze([]);

// src/sim/transit/transit.ts:592-599 function crossAt
/** Where segment a-b crosses segment p-q: the share along each, or null. */
function crossAt(a: Vec2, b: Vec2, p: Vec2, q: Vec2): { u: number; v: number } | null;
