// The code of src/editor/roads/profile.ts that is the feature's (the rest of the file is shared).

// src/editor/roads/profile.ts:13-34 slice of src/editor/roads/profile.ts#applyProfileTo (shared): the feature's statements; "…" marks code left out
export function applyProfileTo(doc: RoadDoc, ids: readonly SegmentId[], profile: RoadProfileSpec, type?: number): ApplyResult {
  // … 1 lines
  for (const id of ids) {
    // … 4 lines
    if (problems.length) return { changed, problems };
    // … 9 lines
    changed ||= before.type !== after.type || before.lanes !== after.lanes || before.direction !== after.direction ||
      !sameRoadSection(before.section, after.section) || !sameParking(before.parking, after.parking);
  }
  return { changed, problems: [] };
}
