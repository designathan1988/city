// What the feature uses of src/world/geometry.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/geometry.ts:21-25 function segmentPolyline
/** Flattened centreline of a segment, oriented a -> b. */
export function segmentPolyline(doc: RoadDoc, seg: RoadSegment): Polyline;

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

// src/world/geometry.ts:67-67 field PolylineCache.map
private map = new Map<SegmentId, Polyline>();

// src/world/geometry.ts:70-77 method PolylineCache.get
/** The segment's centreline. */
  get(doc: RoadDoc, id: SegmentId): Polyline;
