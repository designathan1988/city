// The code of src/world/walkways.ts that is the feature's (the rest of the file is shared).

// src/world/walkways.ts:32-54 slice of src/world/walkways.ts#Walkway (shared): the feature's statements; "…" marks code left out
export interface Walkway {
  // … 18 lines
  readonly structure: RoadStructure;
// … 2 lines
}

// src/world/walkways.ts:60-66 slice of src/world/walkways.ts#deckOf (shared): the feature's statements; "…" marks code left out
export function deckOf(doc: RoadDoc, id: SegmentId): RoadStructure {
// … 5 lines
}

// src/world/walkways.ts:223-239 slice of src/world/walkways.ts#FootwayShape (shared): the feature's statements; "…" marks code left out
interface FootwayShape {
  // … 8 lines
  readonly structure: RoadStructure;
// … 6 lines
}

// src/world/walkways.ts:630-662 slice of src/world/walkways.ts#cornersOf (shared): the feature's statements; "…" marks code left out
function cornersOf(net: Network, nodeId: NodeId, here: readonly FootwayEnd[]): { readonly from: FootwayEnd; readonly to: FootwayEnd; readonly kerb: Vec2 | null }[] {
  // … 4 lines
  const junction = byLevel?.get(Level.Sidewalk);
  if (junction && junction.legs.length >= 2 && !junction.transition) {
    // Each leg's footway joins the next leg's round the node, in angular order, whatever the angle between them
    const pairs = junction.corners.length ? junction.corners
      : junction.legs.map((_, i) => ({ i, j: (i + 1) % junction.legs.length }));
    for (const corner of pairs) {
      const li = junction.legs[corner.i]!, lj = junction.legs[corner.j]!;
      // Leg i's LEFT boundary feeds the corner, leg j's RIGHT boundary leaves it.
      const from = sideOf(here, li.seg, node, li.nrm, 1);
      const to = sideOf(here, lj.seg, node, lj.nrm, -1);
      if (!from || !to || from === to) continue;
      const kerb = byLevel?.get(Level.Curb)?.corners.find((c) => c.i === corner.i && c.j === corner.j)?.fillet ?? null;
      out.push({ from, to, kerb: kerb?.c ?? null });
    }
  } else if (node.incident.length === 2) {
    // A node of two roads drawn as a taper or a bend, or not drawn at all
    const [s0, s1] = node.incident as [SegmentId, SegmentId];
    for (const e0 of here.filter((e) => e.way.segment === s0)) {
      let best: FootwayEnd | null = null, bestD = Infinity;
      for (const e1 of here.filter((e) => e.way.segment === s1)) {
        const d = Math.hypot(e1.p.x - e0.p.x, e1.p.y - e0.p.y);
        if (d < bestD) { best = e1; bestD = d; }
      }
      if (best) out.push({ from: e0, to: best, kerb: null });
    }
  }
// … 1 lines
}
