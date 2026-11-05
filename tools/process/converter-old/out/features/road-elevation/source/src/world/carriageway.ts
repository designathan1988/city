// The code of src/world/carriageway.ts that is the feature's (the rest of the file is shared).

// src/world/carriageway.ts:27-43 slice of src/world/carriageway.ts#onRoadGround (shared): the feature's statements; "…" marks code left out
export function onRoadGround(net: Network, p: Vec2): boolean {
  // … 1 lines
  return net.segmentCells.someAt(p.x, p.y, (id) => {
  // … 6 lines
  }) || net.junctionCells.someAt(p.x, p.y, (node) => {
    const ring = net.junctions.get(node)?.get(Level.Sidewalk)?.ring;
    if (!ring || ring.isEmpty) return false;
    const bb = ring.bbox;
    if (p.x < bb.minX || p.x > bb.maxX || p.y < bb.minY || p.y > bb.maxY) return false;
    return pointInPolygon(p, ringPoints(ring));
  });
}
