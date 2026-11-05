// What the feature uses of src/sim/agents/lorryTraffic.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/agents/lorryTraffic.ts:28-28 variable CALL_REACH
/** Farthest a passing lorry is called from, and how long it has to reach the gate. */
const CALL_REACH = m(400);

// src/sim/agents/lorryTraffic.ts:34-34 variable LANE_HALF
/** Body points within this of a lane's centre close it (half a wide lane and a hand). */
const LANE_HALF = m(2);

// src/sim/agents/lorryTraffic.ts:36-36 variable CLOSURE_MARGIN
/** Kept clear before and after the stretch a lorry sweeps. */
const CLOSURE_MARGIN = m(1.5);

// src/sim/agents/lorryTraffic.ts:38-38 variable JOIN_SPEED
/** Speed a lorry joins its lane at. */
const JOIN_SPEED = m(1.5);

// src/sim/agents/lorryTraffic.ts:42-42 type Phase
type Phase = 'coming' | 'waiting' | 'arriving' | 'docked' | 'leaving' | 'joining';

// src/sim/agents/lorryTraffic.ts:44-67 interface Lorry
interface Lorry {
  readonly id: VehicleId;
  readonly building: BuildingId;
  readonly dock: number;
  phase: Phase;
  /** The lorry as drawn off the road; null while it drives in the traffic. */
  body: Vehicle | null;
  shunt: Shunt | null;
  /** The two ways, in the world. */
  into: LorryWay;
  away: LorryWay;
  /** The lane it stops on before the gate and joins past it. */
  readonly lane: LaneletId;
  readonly stopAt: number;
  readonly joinAt: number;
  /** The stretches of lane its street part sweeps, coming and going. */
  readonly closeIn: Closure[];
  readonly closeOut: Closure[];
  /** Lanes closed now. */
  closed: boolean;
  t: number;
  stay: number;
  waited: number;
}

// src/sim/agents/lorryTraffic.ts:218-262 method LorryTraffic.call
/** A yard with no lorry, a dock of it drawn; planned, it is sent the nearest passing lorry. */
  private call(w: SimWorld): void;

// src/sim/agents/lorryTraffic.ts:349-367 method LorryTraffic.join
private join(w: SimWorld, lorry: Lorry): void;

// src/sim/agents/lorryTraffic.ts:429-452 function closures
/** The stretches of lane a street part sweeps */
export function closures(w: SimWorld, poses: readonly Waypoint[], owner: number, rear: number, length: number, width: number): Closure[];
