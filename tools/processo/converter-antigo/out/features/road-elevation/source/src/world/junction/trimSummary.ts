// The code of src/world/junction/trimSummary.ts that is the feature's (the rest of the file is shared).

// src/world/junction/trimSummary.ts:14-41 slice of src/world/junction/trimSummary.ts#trimSummary (shared): the feature's statements; "…" marks code left out
export function trimSummary(levels: JunctionLevels): number[] {
  // … 6 lines
  levels.forEachTrim((level, seg, trim) => {
    const k = slot(seg) + level;
    out[k] = Math.max(Number.isNaN(out[k]!) ? 0 : out[k]!, trim);
  });
  const junction = levels.get(Level.Asphalt);
  if (junction && !junction.transition) {
    for (const corner of junction.corners) {
      const x = corner.x;
      if (!x || corner.psi >= Math.PI / 2) continue;
      for (const index of [corner.i, corner.j]) {
        const leg = junction.legs[index];
        if (!leg) continue;
        const along = (x.x - leg.origin.x) * leg.dir.x + (x.y - leg.origin.y) * leg.dir.y;
        if (!(along > 0)) continue;
        const k = slot(leg.seg) + SUMMARY_FLOOR;
        out[k] = Math.max(out[k]!, along);
      }
    }
  }
// … 1 lines
}
