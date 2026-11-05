// The code of src/editor/editRules.ts that is the feature's (the rest of the file is shared).

// src/editor/editRules.ts:30-30 slice of src/editor/editRules.ts#MAX_BUILT_GRADE (shared): the feature's statements; "…" marks code left out
const MAX_BUILT_GRADE = ROAD_TUNING.grade.refuse;

// src/editor/editRules.ts:32-32 slice of src/editor/editRules.ts#PASS_CLEARANCE (shared): the feature's statements; "…" marks code left out
const PASS_CLEARANCE = ROAD_TUNING.clearance.elevated;

// src/editor/editRules.ts:57-57 slice of src/editor/editRules.ts#refuseRoadEdit.deck (shared): the feature's statements; "…" marks code left out
  const deck = (id: SegmentId, p: Vec2): number => solves.after().onSegment(id, p.x, p.y);

// src/editor/editRules.ts:63-69 slice of src/editor/editRules.ts#flatSolves (shared): the feature's statements; "…" marks code left out
function flatSolves(before: RoadState, after: RoadState): { before(): RoadElevation; after(): RoadElevation } {
  let solvedBefore: RoadElevation | null = null;
  let solvedAfter: RoadElevation | null = null;
// … 3 lines
}

// src/editor/editRules.ts:66-66 slice of src/editor/editRules.ts#flatSolves.beforeSolve (shared): the feature's statements; "…" marks code left out
  const beforeSolve = (): RoadElevation => (solvedBefore ??= buildRoadElevation(before.net, FLAT_GROUND));

// src/editor/editRules.ts:68-68 slice of src/editor/editRules.ts#flatSolves.after (shared): the feature's statements; "…" marks code left out
  return { before: beforeSolve, after: () => (solvedAfter ??= buildRoadElevation(after.net, FLAT_GROUND, beforeSolve())) };

// src/editor/editRules.ts:130-135 slice of src/editor/editRules.ts#deckAt (shared): the feature's statements; "…" marks code left out
function deckAt(doc: RoadDoc, seg: RoadSegment, s: number, length: number): number {
  // … 3 lines
  return a + (b - a) * t + roadStructure(seg.structure).clearance;
}

// src/editor/editRules.ts:142-164 slice of src/editor/editRules.ts#grade (shared): the feature's statements; "…" marks code left out
function grade(state: RoadState, id: SegmentId, solved: () => RoadElevation): number {
  // … 8 lines
  const elevation = solved();
  // … 1 lines
  for (let s = 0; s <= line.length; s += GRADE_STEP) {
    // … 1 lines
    heights.push(elevation.onSegment(id, p.x, p.y));
  }
// … 7 lines
}

// src/editor/editRules.ts:167-185 slice of src/editor/editRules.ts#tooSteep (shared): the feature's statements; "…" marks code left out
function tooSteep(before: RoadState, after: RoadState, changed: readonly SegmentId[],
  solves: { before(): RoadElevation; after(): RoadElevation }): boolean {
  // … 8 lines
  const afterElevation = solves.after, beforeElevation = solves.before;
  for (const id of touched) {
    const now = grade(after, id, afterElevation);
    if (now <= MAX_BUILT_GRADE) continue;
    if (!changed.includes(id) && before.doc.segment(id) && grade(before, id, beforeElevation) >= now - 1e-6) continue;
  // … 1 lines
  }
// … 1 lines
}

// src/editor/editRules.ts:188-236 slice of src/editor/editRules.ts#overlapsAnother (shared): the feature's statements; "…" marks code left out
function overlapsAnother(before: RoadState, after: RoadState, id: SegmentId,
  deck: (id: SegmentId, p: Vec2) => number): boolean {
  // … 28 lines
  for (let k = 0; k <= steps; k++) {
    // … 3 lines
    for (const { id: otherId, seg: other, limit } of others) {
      // … 7 lines
      if (Math.abs(deck(id, p) - deck(otherId, hit.point)) >= PASS_CLEARANCE) continue;
    // … 2 lines
    }
  }
// … 1 lines
}
