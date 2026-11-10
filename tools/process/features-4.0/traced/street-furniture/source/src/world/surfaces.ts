// The code of src/world/surfaces.ts that is the feature's (the rest of the file is shared).

// src/world/surfaces.ts:129-131 function levelPolygons: the feature's own
export function levelPolygons(net: SurfaceNet, level: SurfaceLevel, include?: SurfaceSegmentFilter): MultiPoly {
  return union(levelRings(net, level, include));
}
