// The code of src/world/section.ts that is the feature's (the rest of the file is shared).

// src/world/section.ts:67-77 slice of src/world/section.ts#sectionOf (shared): the feature's statements; "…" marks code left out
export function sectionOf(rt: RoadType, direction: SegmentDirection = 'both'): CrossSection {
  // … 2 lines
  const walkable = carriesPedestrians(rt);
  const right = sideZones(rt, footwayOn(rt, 'right'), flushOn(rt, 'right'), walkable);
  const left = symmetric(rt) ? right : sideZones(rt, footwayOn(rt, 'left'), flushOn(rt, 'left'), walkable);
  // … 3 lines
  return { carriageway: half, median: rt.median, lanes, side: right, left, right, walkable };
}

// src/world/section.ts:105-105 function bandMid: the feature's own
export const bandMid = (b: Band): number => (b.inner + b.outer) / 2;
