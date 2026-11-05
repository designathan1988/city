// What the feature uses of src/sim/signals/plan.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/signals/plan.ts:7-7 type GroupId
export type GroupId = number;

// src/sim/signals/plan.ts:8-8 type CrossingId
export type CrossingId = string;

// src/sim/signals/plan.ts:10-24 interface Stage
interface Stage {
  readonly greenGroups: readonly GroupId[];
  /** Crossings showing WALK during this stage. */
  readonly pedWalk: readonly CrossingId[];
  readonly minGreen: number;
  readonly maxGreen: number;
  readonly targetGreen: number;
  readonly amber: number;
  readonly allRed: number;
  readonly exclusivePed: boolean;
  /** Movements green in this stage that no conflicting movement from another approach can meet */
  readonly protectedMovements: readonly string[];
  /** When set, this stage exists only to serve these movements */
  readonly demandMovements?: readonly string[];
}

// src/sim/signals/plan.ts:26-31 interface SignalPlan
export interface SignalPlan {
  readonly stages: readonly Stage[];
  readonly cycle: number;
  readonly groups: readonly GroupId[];
  readonly crossings: readonly CrossingId[];
}

// src/sim/signals/plan.ts:36-46 function buildSignalPlan
/** Builds a fixed-time plan for one junction and validates it before returning. */
export function buildSignalPlan(
  junction: JunctionTopology,
  crossings: readonly CrossingId[],
  connectorsOf: (id: string) => Connector | undefined,
  conflictsOf?: (id: string) => readonly string[],
  approachSpeed?: (lane: string) => number,
): SignalPlan;

// src/sim/signals/plan.ts:58-75 function withAmbers
/** Each stage's yellow from the fastest approach it releases, by the ITE kinematic formula Y = t + V / 2a (level ground) */
function withAmbers(plan: SignalPlan, junction: JunctionTopology, connectorsOf: (id: string) => Connector | undefined,
  approachSpeed: (lane: string) => number): SignalPlan;

// src/sim/signals/plan.ts:78-126 function annotate
/** Marks every stage's protected movements from the real conflict matrix and adds an exclusive stage for any approach whose */
function annotate(
  plan: SignalPlan,
  junction: JunctionTopology,
  connectorsOf: (id: string) => Connector | undefined,
  conflictsOf?: (id: string) => readonly string[],
): SignalPlan;

// src/sim/signals/plan.ts:128-128 variable EXCLUSIVE_MAX_FACTOR
const EXCLUSIVE_MAX_FACTOR = 1.6;

// src/sim/signals/plan.ts:131-131 variable TURN_RANK
/** Who gives way to whom between two green movements that cross. */
const TURN_RANK: Record<Connector['turn'], number> = { through: 3, right: 2, left: 1, uturn: 0 };

// src/sim/signals/plan.ts:133-222 function buildStages
function buildStages(
  junction: JunctionTopology,
  crossings: readonly CrossingId[],
  connectorsOf: (id: string) => Connector | undefined,
  conflictsOf?: (id: string) => readonly string[],
): SignalPlan;

// src/sim/signals/plan.ts:225-278 function conflictFreeStages
/** Groups that may take green together WITHOUT any two of their movements crossing, merging or sweeping into one another. */
function conflictFreeStages(
  junction: JunctionTopology,
  connectorsOf: (id: string) => Connector | undefined,
  conflictsOf: (id: string) => readonly string[],
): GroupId[][];

// src/sim/signals/plan.ts:281-281 variable OPPOSED_TOLERANCE
/** Groups that may take green together, as one entry per stage. */
const OPPOSED_TOLERANCE = (35 * Math.PI) / 180;

// src/sim/signals/plan.ts:283-315 function pairOpposingGroups
function pairOpposingGroups(junction: JunctionTopology): GroupId[][];

// src/sim/signals/plan.ts:318-323 function normaliseAngle
/** Wraps an angle into (-PI, PI]. */
const normaliseAngle = (a: number): number =>;

// src/sim/signals/plan.ts:325-330 interface StageInput
interface StageInput {
  readonly green: readonly GroupId[];
  readonly pedWalk: readonly CrossingId[];
  readonly target: number;
  readonly exclusivePed?: boolean;
}

// src/sim/signals/plan.ts:332-344 function stage
function stage(input: StageInput): Stage;

// src/sim/signals/plan.ts:347-364 function crossingsCompatibleWith
/** A crossing over leg L may show WALK during a stage if and only if no connector green in that stage DEPARTS from L. */
export function crossingsCompatibleWith(
  junction: JunctionTopology,
  greenGroups: readonly GroupId[],
  crossings: readonly CrossingId[],
  connectorsOf: (id: string) => Connector | undefined,
): CrossingId[];

// src/sim/signals/plan.ts:367-367 function mustYieldToPeds
/** True when this connector must yield to pedestrians already in the crossing over its destination leg. */
const mustYieldToPeds = (c: Connector): boolean => …;

// src/sim/signals/plan.ts:370-371 function makeCrossingId
/** Crossing ids are `${nodeId}:${segmentId}`. */
export const makeCrossingId = (node: NodeId, segment: number): CrossingId => …;

// src/sim/signals/plan.ts:376-413 function validate
function validate(plan: SignalPlan): SignalPlan;

// src/sim/signals/plan.ts:416-433 function roundRobinPlan
/** Fallback plan used when validation fails in a production build. */
export function roundRobinPlan(groups: readonly GroupId[], crossings: readonly CrossingId[]): SignalPlan;
