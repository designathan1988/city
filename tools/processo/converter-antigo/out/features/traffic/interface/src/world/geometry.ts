// What the feature uses of src/world/geometry.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/geometry.ts:28-30 function segmentLine
/** A segment's centreline from its two ends and its curve (`segmentPolyline`): what a worker makes it from too (`junctionBuild.ts`). */
export function segmentLine(a: Vec2, b: Vec2, curve: CurveShape | null): Polyline;

// src/world/geometry.ts:33-40 function orientedPolyline
/** Centreline oriented so that it starts at `from`. */
export function orientedPolyline(
  doc: RoadDoc,
  seg: RoadSegment,
  from: NodeId,
): Polyline;
