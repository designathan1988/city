// The code of src/world/surfaceParts.ts that is the feature's (the rest of the file is shared).

// src/world/surfaceParts.ts:22-58 slice of src/world/surfaceParts.ts#surfaceParts (shared): the feature's statements; "…" marks code left out
export function surfaceParts(net: Network, node: NodeId): SurfaceSource {
  // … 18 lines
  for (const level of [Level.Asphalt, Level.Curb] as const) {
    const junction = net.junctions.get(node)?.get(level);
    for (const ring of junction?.rings ?? []) if (!ring.isEmpty) parts.push(ring);
    for (const segment of at?.incident ?? []) {
      // … 5 lines
      for (const farRing of net.junctions.get(far)?.get(level)?.rings ?? []) if (!farRing.isEmpty) parts.push(farRing);
    // … 4 lines
    }
  }
// … 1 lines
}
