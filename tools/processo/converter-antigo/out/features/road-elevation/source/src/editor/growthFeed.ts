// The code of src/editor/growthFeed.ts that is the feature's (the rest of the file is shared).

// src/editor/growthFeed.ts:344-353 slice of src/editor/growthFeed.ts#junctionPiece (shared): the feature's statements; "…" marks code left out
function junctionPiece(net: Network, node: NodeId): JunctionPiece | null {
  const junction = net.junctions.get(node)?.get(Level.Curb);
  // … 1 lines
  if (!junction || !rect) return null;
  const flat = junction.ring.flatten();
  const ring = new Float64Array(flat.length * 2);
  flat.forEach((p, k) => { ring[k * 2] = p.x; ring[k * 2 + 1] = p.y; });
  const box = junction.ring.bbox;
  return { node, rect, ring, bbox: { minX: box.minX, minY: box.minY, maxX: box.maxX, maxY: box.maxY }, empty: junction.ring.isEmpty };
}
