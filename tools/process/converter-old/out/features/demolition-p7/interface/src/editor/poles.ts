// What the feature uses of src/editor/poles.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/poles.ts:40-40 type PoleSnapKind
type PoleSnapKind = 'pole' | 'footway' | 'free';

// src/editor/poles.ts:42-47 interface PoleSnap
export interface PoleSnap {
  readonly at: Vec2;
  readonly kind: PoleSnapKind;
  /** The existing pole this snapped onto, when `kind` is `pole`. */
  readonly pole?: PoleId;
}

// src/editor/poles.ts:65-70 interface PlannedPole
interface PlannedPole {
  readonly at: Vec2;
  /** The pole already standing here, which the run will reuse. */
  readonly existing: PoleId | null;
  readonly lamp: boolean;
}

// src/editor/poles.ts:72-72 type PoleRunRefusal
type PoleRunRefusal = 'offFootway' | 'noPath';

// src/editor/poles.ts:74-80 interface PoleRunPlan
export interface PoleRunPlan {
  readonly from: PoleSnap;
  readonly to: PoleSnap;
  readonly poles: readonly PlannedPole[];
  /** Set when nothing would be built, and why. */
  readonly refused?: PoleRunRefusal;
}

// src/editor/poles.ts:407-515 class PoleTool
/** THE POLE TOOL: a distribution line traced as straight stretches, each a drag */
export class PoleTool { … }
