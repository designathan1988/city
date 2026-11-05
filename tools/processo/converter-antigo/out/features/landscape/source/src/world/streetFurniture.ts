// The code of src/world/streetFurniture.ts that is the feature's (the rest of the file is shared).

// src/world/streetFurniture.ts:48-58 slice of src/world/streetFurniture.ts#FURNITURE_OF (shared): the feature's statements; "…" marks code left out
const FURNITURE_OF: Readonly<Partial<Record<LandscapeKind, FurnitureKind>>> = {
  tree: 'streetTree',
  shrub: 'shrub',
  bench: 'bench',
  bin: 'bin',
  lamp: 'lamp',
  hydrant: 'hydrant',
  postbox: 'postbox',
  phone: 'phone',
  drain: 'drain',
};

// src/world/streetFurniture.ts:71-81 slice of src/world/streetFurniture.ts#furnitureOf (shared): the feature's statements; "…" marks code left out
export function furnitureOf(
  // … 1 lines
  placed: LandscapeItem,
  onAccess: (segment: SegmentId, x: number, y: number, radius: number) => boolean = (segment, x, y, radius) => nearCrossingAccess(net, segment, x, y, radius),
): FurnitureItem | null {
  // … 1 lines
  const kind = FURNITURE_OF[placed.kind];
  if (!kind) return null;
  const item = standing(net, placed, kind);
  return item && !onAccess(item.segment, item.x, item.y, item.radius) ? item : null;
}

// src/world/streetFurniture.ts:84-111 slice of src/world/streetFurniture.ts#standing (shared): the feature's statements; "…" marks code left out
function standing(net: Network, placed: LandscapeItem, kind: FurnitureKind): FurnitureItem | null {
  const hit = footwayAt(net, placed, STAND_REACH);
  if (!hit) {
    // A tree or shrub in a planted median: nobody walks there.
    const median = kind === 'streetTree' || kind === 'shrub' ? medianAt(net, placed, m(0.3)) : null;
    if (!median) return null;
    return { kind, x: placed.x, y: placed.y, segment: median.segment,
      along: { x: median.frame.t.x, y: median.frame.t.y },
      outward: { x: median.frame.n.x, y: median.frame.n.y },
      on: 'median', radius: LANDSCAPE_RADIUS[placed.kind], seed: hash01(placed.id, 0x5eed),
      ...(placed.planted !== undefined ? { planted: placed.planted } : {}) };
  }
  const outward = { x: hit.frame.n.x * hit.side, y: hit.frame.n.y * hit.side };
  const base = {
    // … 1 lines
    x: placed.x,
    y: placed.y,
    segment: hit.segment,
    along: { x: hit.frame.t.x, y: hit.frame.t.y },
    // … 2 lines
    radius: LANDSCAPE_RADIUS[placed.kind],
    seed: hash01(placed.id, 0x5eed),
    ...(placed.planted !== undefined ? { planted: placed.planted } : {}),
  };
// … 2 lines
}

// src/world/streetFurniture.ts:114-123 slice of src/world/streetFurniture.ts#streetFurniture (shared): the feature's statements; "…" marks code left out
export function streetFurniture(net: Network): FurnitureItem[] {
  // … 1 lines
  const onAccess = crossingAccessTester(net, crossingAccesses(net));
  // … 1 lines
  for (const placed of net.doc.landscape.values()) {
    const item = furnitureOf(net, placed, onAccess);
    if (item) items.push(item);
  }
// … 1 lines
}
