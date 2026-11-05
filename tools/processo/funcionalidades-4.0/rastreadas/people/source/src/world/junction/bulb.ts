// The code of src/world/junction/bulb.ts that is the feature's (the rest of the file is shared).

// src/world/junction/bulb.ts:81-91 function bulbLine: the feature's own
export function bulbLine(doc: RoadDoc, cache: PolylineCache, nodeId: NodeId, offset: number): Vec2[] | null {
  const node = doc.node(nodeId);
  if (!node || node.incident.length !== 1 || node.end !== 'bulb') return null;
  const asphalt = buildLegs(doc, cache, nodeId, Level.Asphalt)[0];
  if (!asphalt) return null;
  const shape = outline({ x: node.x, y: node.y }, asphalt.dir, asphalt.nrm, offset, offset, Math.max(0, offset - asphalt.hw));
  if (!shape) return null;
  // The ring runs right mouth, left mouth, round, back to the right: drop the mouth's line.
  const flat = shape.ring.flatten();
  return [...flat.slice(1), flat[0]!];
}
