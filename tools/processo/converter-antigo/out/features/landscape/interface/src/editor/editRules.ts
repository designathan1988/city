// What the feature uses of src/editor/editRules.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/editRules.ts:17-17 type RoadEditRefusal
/** What a road edit may not leave behind, judged on the network it builds. */
export type RoadEditRefusal = 'sharp' | 'squeezed' | 'overlap' | 'steep' | 'funds';

// src/editor/editRules.ts:19-23 interface RoadState
export interface RoadState {
  readonly doc: RoadDoc;
  /** Built for `doc` (`revision` current). */
  readonly net: Network;
}

// src/editor/editRules.ts:30-30 variable MAX_BUILT_GRADE
/** The steepest grade a road is refused beyond: Baldwin Street, Dunedin, the steepest street in the world at 34.8 % */
const MAX_BUILT_GRADE = ROAD_TUNING.grade.refuse;

// src/editor/editRules.ts:63-69 function flatSolves
/** The roads' heights before and after, over flat land, each solved once and only when a rule asks */
function flatSolves(before: RoadState, after: RoadState): { before(): RoadElevation; after(): RoadElevation };

// src/editor/editRules.ts:138-138 variable GRADE_STEP
/** Spacing of the deck samples a grade is read from, and the run it is measured over (0.4 m). */
const GRADE_STEP = m(0.4);

// src/editor/editRules.ts:142-164 function grade
/** The steepest grade of a segment's deck as the game solves it (rise over run, over `GRADE_RUN`) */
function grade(state: RoadState, id: SegmentId, solved: () => RoadElevation): number;

// src/editor/editRules.ts:167-185 function tooSteep
/** Whether the edit leaves a ramp steeper than any street, on a road it laid or on one meeting it */
function tooSteep(before: RoadState, after: RoadState, changed: readonly SegmentId[],
  solves: { before(): RoadElevation; after(): RoadElevation }): boolean;
