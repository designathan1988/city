// The code of src/render/networkFarRoads.ts that is the feature's (the rest of the file is shared).

// src/render/networkFarRoads.ts:48-83 slice of src/render/networkFarRoads.ts#farRoadInput (shared): the feature's statements; "…" marks code left out
export function farRoadInput(net: Network, heights: FarRoadHeights, groundAt: (x: number, y: number) => number, cell: number, origin: number,
  exclude: readonly [number, number, number, number] | null): { input: FarRoadInput; transfer: ArrayBuffer[] } {
  // … 18 lines
  for (const [id, levels] of net.junctions) {
    // … 5 lines
    const casing = levels.ringAt(Level.Casing), asphalt = levels.ringAt(Level.Asphalt);
    if (casing) add(casing.flatten(), 0, CASING, height);
    if (asphalt) {
      const types = node.incident.map((s) => net.ribbons.get(s)?.typeIndex ?? -1).filter((t) => t >= 0);
      add(asphalt.flatten(), 1, ASPHALT_BY_CLASS[types.length ? Math.max(...types) : 0] ?? STREET_MEANS.asphalt, height);
    }
  }
// … 2 lines
}

// src/render/networkFarRoads.ts:60-60 slice of src/render/networkFarRoads.ts#farRoadInput.onGround (shared): the feature's statements; "…" marks code left out
  const onGround = (x: number, y: number): number => groundAt(x, y) + ROAD_GROUND_CLEARANCE;
