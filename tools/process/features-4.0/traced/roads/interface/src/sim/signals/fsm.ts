// What the feature uses of src/sim/signals/fsm.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/signals/fsm.ts:15-15 type SubPhase
type SubPhase = 'GREEN' | 'AMBER' | 'ALL_RED';

// src/sim/signals/fsm.ts:18-18 interface SignalClock
/** The signals' passes (stage 1, `pipeline.ts`), one a tick: the clock every controller's time is read against, asleep or not. */
export interface SignalClock { passes: number }

// src/sim/signals/fsm.ts:21-39 interface SignalController
/** Per-junction signal state. */
export interface SignalController {
  readonly node: NodeId;
  plan: SignalPlan;
  stageIndex: number;
  sub: SubPhase;
  /** Seconds spent in the current sub-phase: `base` plus a step a pass of `clock` since `since` (an accessor; written, it starts again from now). */
  elapsed: number;
  base: number;
  since: number;
  readonly clock: SignalClock;
  /** Tick at which each group last received green. */
  lastServed: Map<GroupId, number>;
  /** Tick at which each stage last ended its green. */
  stageServed: Map<number, number>;
  /** Coordination offset ALREADY APPLIED, in seconds. */
  readonly offsetApplied: number;
  /** Set when a plan failed validation and a fallback was substituted. */
  degraded: boolean;
}

// src/sim/signals/fsm.ts:41-68 interface SignalDeps
export interface SignalDeps {
  readonly tick: () => number;
  readonly connectorsOf: (id: string) => Connector | undefined;
  /** The speed limit of an approach lane, u/s: what each stage's yellow is timed for (`plan.ts` `withAmbers`). */
  readonly approachSpeed?: (lane: string) => number;
  /** Movements that physically conflict with a connector (swept zones). */
  readonly conflictsOf?: (id: string) => readonly string[];
  /** Whether `conflictsOf` knows a junction's movements: where it does not (out of the micro window), the plan pairs opposing approaches by their geometry. */
  readonly conflictsKnown?: (node: NodeId) => boolean;
  /** Pedestrians still inside a crossing that this stage released. */
  readonly pedestriansCrossing: (node: NodeId, crossings: readonly CrossingId[]) => boolean;
  /** Whether any vehicle is waiting on the given groups (and movements). */
  readonly demandOn: (node: NodeId, groups: readonly GroupId[], movements?: readonly string[]) => boolean;
  /** Weighted demand; see `SimWorld.signalDemand`. */
  readonly demand?: (node: NodeId, groups: readonly GroupId[], movements?: readonly string[]) => { active: number; score: number };
  /** Longest time anybody has waited at a kerb for one of these crossings, seconds. */
  readonly pedestrianWait?: (node: NodeId, crossings: readonly CrossingId[]) => number;
  /** Whether a pedestrian is waiting to start one of these crossings. */
  readonly pedestrianDemandOn: (node: NodeId, crossings: readonly CrossingId[]) => boolean;
  /** Whether a physical compact-box holder needs this group next. */
  readonly reservationDemandOn: (node: NodeId, groups: readonly GroupId[]) => boolean;
  /** The player's settings of a junction's signal: fixed time, greens, offset, bus priority. */
  readonly settings?: (node: NodeId) => SignalSettings | undefined;
  /** Whether a bus is coming to the line of one of these groups, close enough to be served (V5). */
  readonly busNear?: (node: NodeId, groups: readonly GroupId[]) => boolean;
  /** Doing less (`SimWorld.doLess`): nobody near the junction */
  readonly quiet?: (c: SignalController) => boolean;
}

// src/sim/signals/fsm.ts:71-71 variable BUS_EXTENSION
/** How long a green is held past its maximum for a bus about to reach the line (green extension), seconds */
const BUS_EXTENSION = 10;

// src/sim/signals/fsm.ts:74-74 variable signalWork
/** Greens that rested with nobody near (`SignalDeps.quiet`): a count for the tests and the budgets. */
export const signalWork = { rested: 0 };

// src/sim/signals/fsm.ts:76-100 function createController
export function createController(
  junction: JunctionTopology,
  crossings: readonly CrossingId[],
  deps: SignalDeps,
  offset: number,
  clock: SignalClock,
): SignalController;

// src/sim/signals/fsm.ts:109-111 function timedController
/** A controller from its fields, its `elapsed` kept by its clock (`base` seconds at pass `since`). */
export function timedController(fields: Omit<SignalController, 'elapsed'>): SignalController;

// src/sim/signals/fsm.ts:114-140 function seekPhase
/** Where in its own cycle a plan sits `seconds` after the start of stage 0. */
function seekPhase(
  plan: SignalPlan,
  seconds: number,
): { stageIndex: number; sub: SubPhase; elapsed: number };

// src/sim/signals/fsm.ts:142-162 function safePlan
function safePlan(
  junction: JunctionTopology,
  crossings: readonly CrossingId[],
  deps: SignalDeps,
): { plan: SignalPlan; degraded: boolean };

// src/sim/signals/fsm.ts:168-168 variable PED_HOLD_LIMIT
/** Longest a green is held past its maximum for people still on its crossings, seconds. */
const PED_HOLD_LIMIT = 30;

// src/sim/signals/fsm.ts:171-265 function stepController
/** One tick of a controller, after its clock's pass (`SignalClock`): the time it read has moved on a step. */
export function stepController(c: SignalController, deps: SignalDeps): boolean;

// src/sim/signals/fsm.ts:268-319 function pickNextStage
/** Chooses the next stage without skipping any vehicle or pedestrian service. */
function pickNextStage(c: SignalController, deps: SignalDeps): number;

// src/sim/signals/fsm.ts:363-402 function rebuildController
/** Rebuilds a controller's plan after the network changed, carrying state over. */
export function rebuildController(
  c: SignalController,
  junction: JunctionTopology,
  crossings: readonly CrossingId[],
  deps: SignalDeps,
): void;

// src/sim/signals/fsm.ts:410-412 function fixedCycle
/** A fixed-time plan's cycle with the player's greens, seconds (V5). */
export function fixedCycle(plan: SignalPlan, settings: SignalSettings | undefined): number;

// src/sim/signals/fsm.ts:415-430 function seekFixed
/** Puts a fixed-time signal where its cycle stands at `seconds` of the common clock plus its offset */
export function seekFixed(c: SignalController, settings: SignalSettings, seconds: number): void;
