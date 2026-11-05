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

// src/sim/signals/plan.ts:370-371 function makeCrossingId
/** Crossing ids are `${nodeId}:${segmentId}`. */
export const makeCrossingId = (node: NodeId, segment: number): CrossingId => …;
