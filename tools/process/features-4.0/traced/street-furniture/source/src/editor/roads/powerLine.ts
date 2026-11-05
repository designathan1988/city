// The code of src/editor/roads/powerLine.ts that is the feature's (the rest of the file is shared).

// src/editor/roads/powerLine.ts:25-68 slice of src/editor/roads/powerLine.ts#layPowerLinesSteps (shared): the feature's statements; "…" marks code left out
export function* layPowerLinesSteps(doc: RoadDoc, net: Network, segments: Iterable<SegmentId>, plan?: PlanRun): Generator<void, number> {
  // … 13 lines
  for (const id of ids) {
    // … 18 lines
    const start = open[0] === 0 && coming && !coming.leaving && doc.node(segment.a)?.incident.length === 2 ? doc.poles.get(coming.pole) : undefined;
    const from = start ?? at(s0);
    const poles = commitPoleRunPoles(doc, planRun(doc, from, at(s1)));
    if (!poles) continue;
    // … 2 lines
    if (open[0] === 0) join(segment.a, poles[0]!, true);
    if (open[1] === 1) join(segment.b, poles[poles.length - 1]!, false);
  }
// … 1 lines
}

// src/editor/roads/powerLine.ts:79-79 slice of src/editor/roads/powerLine.ts#PlanRun (shared): the feature's statements; "…" marks code left out
export type PlanRun = (doc: RoadDoc, from: Vec2, to: Vec2) => PoleRunPlan;

// src/editor/roads/powerLine.ts:85-91 slice of src/editor/roads/powerLine.ts#runPlanner (shared): the feature's statements; "…" marks code left out
function runPlanner(net: Network, streets: number): PlanRun {
  if (streets > BATCH) {
    const planner = poleRunPlanner(net);
    return (doc, from, to) => planner.plan(doc, from, to, SNAP_REACH, undefined, 'all');
  }
  return (doc, from, to) => planPoleRun(doc, net, from, to, SNAP_REACH, undefined, 'all');
}
