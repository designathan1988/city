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
