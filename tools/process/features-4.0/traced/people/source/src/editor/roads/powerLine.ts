// The code of src/editor/roads/powerLine.ts that is the feature's (the rest of the file is shared).

// src/editor/roads/powerLine.ts:25-68 slice of src/editor/roads/powerLine.ts#layPowerLinesSteps (shared): the feature's statements; "…" marks code left out
export function* layPowerLinesSteps(doc: RoadDoc, net: Network, segments: Iterable<SegmentId>, plan?: PlanRun): Generator<void, number> {
  // … 13 lines
  for (const id of ids) {
    // … 2 lines
    if (!segment || !ribbon || !carriesPedestrians(ribbon.road) || ribbon.road.sidewalk <= 0) continue;
  // … 23 lines
  }
// … 1 lines
}
