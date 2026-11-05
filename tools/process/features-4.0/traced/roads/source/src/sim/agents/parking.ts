// The code of src/sim/agents/parking.ts that is the feature's (the rest of the file is shared).

// src/sim/agents/parking.ts:26-29 slice of src/sim/agents/parking.ts#BayWorld (shared): the feature's statements; "…" marks code left out
export interface BayWorld extends KerbWorld, LaneGridWorld {
  // … 1 lines
  rt(id: LaneletId): { readonly ghost: boolean };
}

// src/sim/agents/parking.ts:189-198 slice of src/sim/agents/parking.ts#kerbReads.linksKey (shared): the feature's statements; "…" marks code left out
    linksKey: (segment) => {
      // … 3 lines
      for (const id of [...w.graph.linksOfSegment(segment)].sort(byText)) {
        const lane = w.graph.lanelet(id);
        if (lane) digest.add(grid.digest(lane)).add(w.rt(id).ghost ? 1 : 0);
      }
    // … 1 lines
    },

// src/sim/agents/parking.ts:374-416 slice of src/sim/agents/parking.ts#lookAtLot (shared): the feature's statements; "…" marks code left out
function lookAtLot(w: BayWorld, book: BayBook, b: TrafficBuilding, rects: readonly ChangeRect[] | null, walls: Walls, asked: Asked[] | null): void {
  // … 12 lines
  const near = grid.near(cx, cy, Math.hypot(box.maxX - box.minX, box.maxY - box.minY) / 2).filter((lane) => lane.kind === 'link');
  for (const lane of near) digest.add(grid.digest(lane)).add(w.rt(lane.id).ghost ? 1 : 0).add(bothWays(w, lane.segment) ? 1 : 0);
  // … 22 lines
  asked.push({ id: b.id, sources, ask: {
    // … 1 lines
    lanes: near.map((lane) => ({ lane, ghost: w.rt(lane.id).ghost, bothWays: bothWays(w, lane.segment) })),
  // … 1 lines
  } });
}
