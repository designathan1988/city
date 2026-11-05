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

// src/editor/editRules.ts:26-26 variable GAP_NOISE
/** A node's smallest leg gap may shrink this much before it counts as made sharper */
const GAP_NOISE = COARSE_EPS;

// src/editor/editRules.ts:28-28 variable SQUEEZE_SLACK
/** A squeeze may deepen this much (0.4 m) before it counts as made worse. */
const SQUEEZE_SLACK = m(0.4);

// src/editor/editRules.ts:30-30 variable MAX_BUILT_GRADE
/** The steepest grade a road is refused beyond: Baldwin Street, Dunedin, the steepest street in the world at 34.8 % */
const MAX_BUILT_GRADE = ROAD_TUNING.grade.refuse;

// src/editor/editRules.ts:32-32 variable PASS_CLEARANCE
/** Vertical room for one road to pass over another */
const PASS_CLEARANCE = ROAD_TUNING.clearance.elevated;

// src/editor/editRules.ts:34-34 variable SAME_LEVEL
/** Two decks within this height of each other are at one level (`commit.ts` `HEIGHT_JOIN_EPS`). */
const SAME_LEVEL = m(0.3);

// src/editor/editRules.ts:36-36 variable SAMPLE_STEP
/** Spacing of the samples along a changed road (0.8 m). */
const SAMPLE_STEP = m(0.8);

// src/editor/editRules.ts:38-38 variable OVERLAP_SLACK
/** Overlap shallower than this is a kerb grazing a kerb, not a road on a road (0.2 m). */
const OVERLAP_SLACK = m(0.2);

// src/editor/editRules.ts:40-40 variable SAME_PLACE
/** A point within this of an old centreline lay on that road before the edit. */
const SAME_PLACE = 1;

// src/editor/editRules.ts:43-60 function refuseRoadEdit
/** The reason to refuse turning `before` into `after`, or null when the edit may stand. */
export function refuseRoadEdit(before: RoadState, after: RoadState): RoadEditRefusal | null;

// src/editor/editRules.ts:63-69 function flatSolves
/** The roads' heights before and after, over flat land, each solved once and only when a rule asks */
function flatSolves(before: RoadState, after: RoadState): { before(): RoadElevation; after(): RoadElevation };

// src/editor/editRules.ts:72-78 function written
/** The marks of `after` that may differ from `before`'s: those its working copy (`Network.fork`) wrote */
function written(before: Network, after: Network, name: 'impossible' | 'squeezed'): Iterable<[NodeId, number]>;

// src/editor/editRules.ts:81-95 function guardRoadEdit
/** DO NOT copy the map to judge an edit against (`doc.clone()` and the whole network, every in-place edit) */
export function guardRoadEdit(doc: RoadDoc, net: Network, edit: (doc: RoadDoc, net: Network) => boolean):
  { readonly changed: boolean; readonly refused: RoadEditRefusal | null };

// src/editor/editRules.ts:130-135 function deckAt
/** The deck's authored height along a segment at arc length `s` (as `commit.ts` `segmentOffsetAt`). */
function deckAt(doc: RoadDoc, seg: RoadSegment, s: number, length: number): number;

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

// src/editor/editRules.ts:188-236 function overlapsAnother
/** Whether a changed segment's roadway lies on another road's carriageway */
function overlapsAnother(before: RoadState, after: RoadState, id: SegmentId,
  deck: (id: SegmentId, p: Vec2) => number): boolean;

// src/editor/editRules.ts:239-245 function withinSharedPlate
/** The other road's closest point lies on its own junction plate at a node the two share. */
function withinSharedPlate(after: RoadState, seg: RoadSegment, other: RoadSegment, otherId: SegmentId, s: number, length: number): boolean;

// src/editor/editRules.ts:248-258 function overlappedBefore
/** The same two places already overlapped before the edit */
function overlappedBefore(before: RoadState, p: Vec2, ph: number, q: Vec2, qh: number): boolean;

// src/editor/editRules.ts:261-267 function overlapReach
/** How close two roads' centrelines may come before one's carriageway lies on the other's carriageway or footway. */
function overlapReach(net: Network, a: SegmentId, b: SegmentId): number;

// src/editor/editRules.ts:270-284 function segmentsThrough
/** DO NOT walk `state.doc.segments`: this runs for every sample of every changed road. */
function segmentsThrough(state: RoadState, p: Vec2, height: number): SegmentId[];
