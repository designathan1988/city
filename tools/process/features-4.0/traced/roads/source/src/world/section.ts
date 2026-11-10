// The code of src/world/section.ts that is the feature's (the rest of the file is shared).

// src/world/section.ts:67-77 slice of src/world/section.ts#sectionOf (shared): the feature's statements; "…" marks code left out
export function sectionOf(rt: RoadType, direction: SegmentDirection = 'both'): CrossSection {
  const half = rt.width / 2;
  // … 1 lines
  const walkable = carriesPedestrians(rt);
  const right = sideZones(rt, footwayOn(rt, 'right'), flushOn(rt, 'right'), walkable);
  const left = symmetric(rt) ? right : sideZones(rt, footwayOn(rt, 'left'), flushOn(rt, 'left'), walkable);
  const count = travelLanes(rt, direction);
  // … 1 lines
  for (let i = 0; i < count; i++) lanes.push({ offset: laneOffset(rt, i, direction), width: laneWidth(rt), index: i });
  return { carriageway: half, median: rt.median, lanes, side: right, left, right, walkable };
}

// src/world/section.ts:80-80 slice of src/world/section.ts#zonesOn (shared): the feature's statements; "…" marks code left out
export const zonesOn = (section: CrossSection, side: RoadSide): SideZones => (side === 'left' ? section.left : section.right);

// src/world/section.ts:83-102 slice of src/world/section.ts#sideZones (shared): the feature's statements; "…" marks code left out
function sideZones(rt: RoadType, sidewalk: number, flush: boolean, walkable: boolean): SideZones {
  const half = rt.width / 2;
  const edge = half + sidewalk;
  // … 1 lines
  const curb: Band = { inner: half, outer: half + (flush ? 0 : CURB_BAND) };
  // … 1 lines
  const edgeZone = Math.max(curb.outer, half + EDGE_ZONE);
  const footway = Math.max(0, edge - edgeZone);
  // … 2 lines
  const seats = footway - BENCH_ZONE >= MIN_THROUGH ? BENCH_ZONE : 0;
  const furnishingDepth = walkable ? Math.min(Math.max(LAMP_ZONE, trees, seats), Math.max(0, footway - MIN_THROUGH)) : 0;
  // … 1 lines
  const furnishing: Band = { inner: curb.outer, outer: edgeZone + furnishingDepth };
  // … 1 lines
  const spare = footway - furnishingDepth - THROUGH_GOAL;
  const frontage = spare >= MIN_FRONTAGE ? spare : 0;
  const through: Band = { inner: furnishing.outer, outer: edge - frontage };
  return { curb, furnishing, through, frontage: { inner: edge - frontage, outer: edge } };
}
