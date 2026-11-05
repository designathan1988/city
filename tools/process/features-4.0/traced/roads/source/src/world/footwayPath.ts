// The code of src/world/footwayPath.ts that is the feature's (the rest of the file is shared).

// src/world/footwayPath.ts:11-15 slice of src/world/footwayPath.ts#FootwayRibbon (shared): the feature's statements; "…" marks code left out
export interface FootwayRibbon {
  // … 1 lines
  readonly road: RoadType;
// … 1 lines
}

// src/world/footwayPath.ts:16-19 slice of src/world/footwayPath.ts#FootwayNet (shared): the feature's statements; "…" marks code left out
export interface FootwayNet {
  readonly doc: { segment(id: SegmentId): { readonly direction: SegmentDirection } | undefined };
// … 1 lines
}

// src/world/footwayPath.ts:22-29 slice of src/world/footwayPath.ts#footwayPath (shared): the feature's statements; "…" marks code left out
export function footwayPath(ribbon: FootwayRibbon, direction: SegmentDirection, side: 1 | -1): Polyline | null {
  const section = sectionOf(ribbon.road, direction);
  if (!section.walkable) return null;
  const centre = ribbon.centre[Level.Sidewalk];
  if (!centre || centre.n < 2 || centre.length < 1e-6) return null;
  // … 1 lines
  return Polyline.fromPoints(offsetPolyline(centre.toPoints(), side * bandMid((side > 0 ? section.left : section.right).through)));
}
