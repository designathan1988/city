// The code of src/world/junction/bulb.ts that is the feature's (the rest of the file is shared).

// src/world/junction/bulb.ts:104-135 slice of src/world/junction/bulb.ts#clearBulbsOf (shared): the feature's statements; "…" marks code left out
function clearBulbsOf(doc: RoadDoc, net: Network, delta: ReturnType<Network['changesSince']>): number {
  // … 4 lines
  for (const nodeId of candidates()) {
    // … 2 lines
    const ring = net.junctions.get(nodeId)?.get(Level.Asphalt)?.ring;
    if (!ring || ring.isEmpty) continue;
    bulbs.push({ node, poly: ring.flatten(), box: [node.x - reach, node.y - reach, node.x + reach, node.y + reach] });
  }
// … 19 lines
}
