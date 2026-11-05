// What the feature uses of src/sim/intersections/admission.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/intersections/admission.ts:22-22 type RowClass
export type RowClass = 'signalGreen' | 'priority' | 'stop' | 'yield' | 'none';

// src/sim/intersections/admission.ts:24-31 interface Request
interface Request {
  readonly v: Vehicle;
  readonly conn: Connector;
  /** Distance from the vehicle's nose to the stop line. */
  readonly d: number;
  readonly requestTick: number;
  readonly row: RowClass;
}

// src/sim/intersections/admission.ts:98-214 function admit
function admit(w: SimWorld, banker: BankerPass): void;

// src/sim/intersections/admission.ts:612-666 class BankerPass
/** The Banker's tests' view of one admission pass (`stepAdmission` makes one and drops it at the end). */
class BankerPass { … }

// src/sim/intersections/admission.ts:979-983 function stoppedForWalker
/** True when the vehicle stands for somebody on foot between its nose and its stop line */
function stoppedForWalker(v: Vehicle, toLine: number): boolean;

// src/sim/intersections/admission.ts:986-991 function crossingBusy
/** True when a pedestrian is on, or about to step onto, the stretch of a zebra this movement drives over (`CrossingSpans`). */
function crossingBusy(w: SimWorld, conn: Connector): boolean;

// src/sim/intersections/admission.ts:995-1001 function crossingLegs
function crossingLegs(w: SimWorld, node: NodeId): readonly { readonly segment: SegmentId; readonly id: string }[];

// src/sim/intersections/admission.ts:1005-1005 variable CLEAR_MARGIN
/** Allowance over the estimated time for a front to reach a zebra it turns across. */
const CLEAR_MARGIN = 1.2;

// src/sim/intersections/admission.ts:1008-1014 function travelTime
/** Seconds to cover `d` from `v0`, accelerating at `a` up to `top`. */
function travelTime(d: number, v0: number, top: number, a: number): number;

// src/sim/intersections/admission.ts:1017-1044 function crossingReachedFirst
/** Whether somebody already on a zebra this movement crosses will reach the stretch it drives over before its body has */
function crossingReachedFirst(w: SimWorld, r: Request): boolean;

// src/sim/intersections/admission.ts:1047-1066 function aheadOnMovement
/** The latest time, with `CLEAR_MARGIN`, at which a vehicle already admitted onto this movement */
function aheadOnMovement(w: SimWorld, r: Request, along: number, top: number): number;

// src/sim/intersections/admission.ts:1069-1100 function pedestrianHasPriority
/** Whether somebody waiting at a kerb has the right to cross in front of this movement before it is admitted */
function pedestrianHasPriority(w: SimWorld, r: Request): boolean;
