// What the feature uses of src/editor/poles.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/poles.ts:17-17 variable POLE_PICK_PIXELS
/** Where a pole line goes: on the footway, just behind the kerb, and nowhere else */

/** Pick radius for an existing pole, in screen pixels. */
export const POLE_PICK_PIXELS = 34;

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

// src/editor/poles.ts:229-237 function inLanding
/** Whether a point stands in a crossing's landing (any street's: a corner belongs to none). */
function inLanding(accesses: readonly CrossingAccess[], p: Vec2): boolean;

// src/editor/poles.ts:240-258 function planPoleRun
/** Everything the run WOULD build, from the two raw ends of the gesture. */
export function planPoleRun(
  doc: RoadDoc,
  net: Network,
  rawFrom: Vec2,
  rawTo: Vec2,
  reach: number,
  spacing = DEFAULT_POLE_SPACING,
  lamps: PoleLampMode = 'alternate',
): PoleRunPlan;

// src/editor/poles.ts:368-370 function commitPoleRun
/** Applies a plan to the document. */
export function commitPoleRun(doc: RoadDoc, plan: PoleRunPlan): boolean;

// src/editor/poles.ts:407-515 class PoleTool
/** THE POLE TOOL: a distribution line traced as straight stretches, each a drag */
export class PoleTool { … }
