// The code of src/render/structureCells.ts that is the feature's (the rest of the file is shared).

// src/render/structureCells.ts:16-19 slice of src/render/structureCells.ts#cellOfRibbon (shared): the feature's statements; "…" marks code left out
const cellOfRibbon = (ribbon: SegmentRibbon): number => {
  const b = ribbon.full.bbox;
  return cellKey(Math.floor((b.minX + b.maxX) / 2 / CELL), Math.floor((b.minY + b.maxY) / 2 / CELL));
};

// src/render/structureCells.ts:28-35 slice of src/render/structureCells.ts#StructureCells (shared): the feature's statements; "…" marks code left out
export interface StructureCells {
  // … 3 lines
  prepare(net: Network, elevation: RoadElevation, terrainAt: (x: number, y: number) => number,
    blocks: readonly (readonly [number, number, number, number])[] | null): { readonly fresh: readonly Group[]; apply(): void };
// … 1 lines
}

// src/render/structureCells.ts:48-55 slice of src/render/structureCells.ts#createStructureCells.ribbonsOf (shared): the feature's statements; "…" marks code left out
  const ribbonsOf = (net: Network, key: number): SegmentRibbon[] => {
    const out: SegmentRibbon[] = [];
    for (const id of net.segmentCells.idsMeeting(cellRect(key))) {
      const ribbon = net.ribbons.get(id);
      if (ribbon && cellOfRibbon(ribbon) === key) out.push(ribbon);
    }
    return out.sort((a, b) => a.id - b.id);
  };

// src/render/structureCells.ts:60-105 slice of src/render/structureCells.ts#createStructureCells.prepare (shared): the feature's statements; "…" marks code left out
    prepare(net, elevation, terrainAt, blocks) {
      // … 2 lines
      if (reached === null) {
        // … 1 lines
        for (const ribbon of net.ribbons.values()) dirty.add(cellOfRibbon(ribbon));
      // … 1 lines
      } else {
        for (const b of reached) {
          for (const id of net.segmentCells.idsMeeting([b[0] - SPAN_REACH, b[1] - SPAN_REACH, b[2] + SPAN_REACH, b[3] + SPAN_REACH])) {
            const ribbon = net.ribbons.get(id);
            if (ribbon) dirty.add(cellOfRibbon(ribbon));
          }
        // … 1 lines
        }
      }
    // … 29 lines
    },
