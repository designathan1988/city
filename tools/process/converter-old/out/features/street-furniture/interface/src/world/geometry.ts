// What the feature uses of src/world/geometry.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/geometry.ts:33-40 function orientedPolyline
/** Centreline oriented so that it starts at `from`. */
export function orientedPolyline(
  doc: RoadDoc,
  seg: RoadSegment,
  from: NodeId,
): Polyline;

// src/world/geometry.ts:66-100 class PolylineCache
/** Cheap cache of per-segment polylines, rebuilt only for dirty segments. */
export class PolylineCache { … }

// src/world/geometry.ts:70-77 method PolylineCache.get
/** The segment's centreline. */
  get(doc: RoadDoc, id: SegmentId): Polyline;
