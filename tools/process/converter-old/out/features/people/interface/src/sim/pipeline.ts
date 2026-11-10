// What the feature uses of src/sim/pipeline.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/pipeline.ts:21-26 interface StepOptions
export interface StepOptions {
  readonly traffic?: boolean;
  readonly pedestrians?: boolean;
  /** Milliseconds spent per stage, ADDED to on every step when given. */
  readonly timings?: Map<string, number>;
}

// src/sim/pipeline.ts:29-128 function step
/** @perFrame One fixed simulation step. */
export function step(w: SimWorld, opts: StepOptions = {}): void;

// src/sim/pipeline.ts:173-175 function rebindPeds
/** @rebuild The pedestrian half of `rebindAgents`, after `SimWorld.rebuildWalkTopology`. */
export function rebindPeds(w: SimWorld): void;
