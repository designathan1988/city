// The code of src/render/buildings/buildingMesh.ts that is the feature's (the rest of the file is shared).

// src/render/buildings/buildingMesh.ts:2151-2151 slice of src/render/buildings/buildingMesh.ts#LAWN_LATTICE (shared): the feature's statements; "…" marks code left out
const LAWN_LATTICE = { cell: TERRAIN_CELL, half: TERRAIN_HALF } as const;

// src/render/buildings/buildingMesh.ts:2154-2332 slice of src/render/buildings/buildingMesh.ts#emitLots (shared): the feature's statements; "…" marks code left out
function emitLots(b: Building, lots: readonly Volume[], withParts: boolean, groundAt: GroundAt, shell: Shell,
  // … 1 lines
  onLot?: (el: BuildingElement) => boolean, entrances?: readonly Entrance[]): LotGround | undefined {
  // … 124 lines
  {
    // … 9 lines
    for (const el of b.elements ?? []) {
      // … 7 lines
      const ground = lawn.length === points.length
        ? highestGroundUnder(points.slice(0, 4).map(([px, py]) => { const w = e.L(px, py, 0); return [w[0], w[1]] as const; }), groundAt, LAWN_LATTICE)
        : Math.max(...lawn.map(([px, py]) => at(px, py)));
      // … 1 lines
      if (!seat) seats.set(g, { base: ground, lowest: lowest.get(g)! });
      else seat.base = Math.max(seat.base, ground);
    }
  }
// … 25 lines
}
