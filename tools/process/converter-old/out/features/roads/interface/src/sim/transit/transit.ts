// What the feature uses of src/sim/transit/transit.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/transit/transit.ts:35-35 variable CROSSING_STOP
/** How far short of a level crossing a vehicle waits, u. */
export const CROSSING_STOP = m(5);

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

// src/sim/transit/transit.ts:77-77 interface LevelCrossing
/** Where a train track crosses a lane of a road at grade: the lane, how far along it, how far along the line's loop. */
interface LevelCrossing { readonly lanelet: string; readonly at: number; readonly loopS: number }

// src/sim/transit/transit.ts:351-357 method TransitSim.stopPoint
/** Where a stop's bus stands (the point of its lane beside it) and the lane's speed; null when it has no lane. */
  private stopPoint(w: SimWorld, stopId: number): { x: number; y: number; speed: number } | null;

// src/sim/transit/transit.ts:387-409 method TransitSim.spawnBus
private spawnBus(w: SimWorld, line: TransitLine, bus: Bus): VehicleId | null;

// src/sim/transit/transit.ts:560-580 function levelCrossings
/** DO NOT go back to the loop over every lane. */
export function levelCrossings(w: SimWorld, points: readonly Vec2[], at: readonly number[]): LevelCrossing[];

// src/sim/transit/transit.ts:592-599 function crossAt
/** Where segment a-b crosses segment p-q: the share along each, or null. */
function crossAt(a: Vec2, b: Vec2, p: Vec2, q: Vec2): { u: number; v: number } | null;
