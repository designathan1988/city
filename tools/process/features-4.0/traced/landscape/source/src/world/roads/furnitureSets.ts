// The code of src/world/roads/furnitureSets.ts that is the feature's (the rest of the file is shared).

// src/world/roads/furnitureSets.ts:20-28 slice of src/world/roads/furnitureSets.ts#Pattern (shared): the feature's statements; "…" marks code left out
interface Pattern {
  readonly kind: LandscapeKind;
// … 6 lines
}

// src/world/roads/furnitureSets.ts:47-63 slice of src/world/roads/furnitureSets.ts#nearGrid (shared): the feature's statements; "…" marks code left out
function nearGrid(): { file(item: LandscapeItem): void; around(at: Vec2, radius: number): LandscapeItem[] } {
  const cells = new Map<string, LandscapeItem[]>();
// … 14 lines
}

// src/world/roads/furnitureSets.ts:50-54 slice of src/world/roads/furnitureSets.ts#nearGrid.file (shared): the feature's statements; "…" marks code left out
    file(item) {
      const key = `${Math.floor(item.x / NEAR_CELL)},${Math.floor(item.y / NEAR_CELL)}`;
      const list = cells.get(key);
      if (list) list.push(item); else cells.set(key, [item]);
    },

// src/world/roads/furnitureSets.ts:55-61 slice of src/world/roads/furnitureSets.ts#nearGrid.around (shared): the feature's statements; "…" marks code left out
    around(at, radius) {
      const out: LandscapeItem[] = [];
      for (let cx = Math.floor((at.x - radius) / NEAR_CELL); cx <= Math.floor((at.x + radius) / NEAR_CELL); cx++) {
        for (let cy = Math.floor((at.y - radius) / NEAR_CELL); cy <= Math.floor((at.y + radius) / NEAR_CELL); cy++) out.push(...(cells.get(`${cx},${cy}`) ?? []));
      }
      return out;
    },

// src/world/roads/furnitureSets.ts:65-68 slice of src/world/roads/furnitureSets.ts#FurniturePlacement (shared): the feature's statements; "…" marks code left out
export interface FurniturePlacement {
  readonly kind: LandscapeKind;
// … 1 lines
}

// src/world/roads/furnitureSets.ts:71-164 slice of src/world/roads/furnitureSets.ts#furnitureFor (shared): the feature's statements; "…" marks code left out
export function furnitureFor(net: Network, segments: Iterable<SegmentId>, set: FurnitureSet,
  // … 1 lines
  existing: Iterable<LandscapeItem> | null,
  // … 3 lines
  trees = true): FurniturePlacement[] {
  // … 3 lines
  const grid = new Map<string, LandscapeItem[]>();
  // … 7 lines
  for (const item of existing ?? []) file(item);
  // … 6 lines
  for (const id of streets) {
    // … 1 lines
    const r = ribbon.road.width / 2 + ribbon.road.sidewalk + m(4) + ACCESS_REACH + MAX_ITEM_RADIUS;
    for (const near of net.ribbonsMeeting([box.minX - r, box.minY - r, box.maxX + r, box.maxY + r])) {
      const s = net.doc.segment(near.id);
      if (s) { nodes.add(s.a); nodes.add(s.b); }
    }
  }
  const onAccess = crossingAccessTester(net, crossingAccessesAt(net, [...nodes].sort((a, b) => a - b)));
  // … 1 lines
  for (const id of streets) {
    // … 2 lines
    const items: LandscapeItem[] = [];
    for (let cx = Math.floor((box.minX - pad) / CELL); cx <= Math.floor((box.maxX + pad) / CELL); cx++) {
      for (let cy = Math.floor((box.minY - pad) / CELL); cy <= Math.floor((box.maxY + pad) / CELL); cy++) items.push(...(grid.get(`${cx},${cy}`) ?? []));
    }
    // … 1 lines
    if (existing === null) items.push(...net.doc.landscapeIn(area));
    // … 1 lines
    for (const pole of net.doc.polesIn(area)) items.push({ id: -1_000_000 - pole.id, kind: 'lamp', x: pole.x, y: pole.y });
    // … 1 lines
    for (const item of items) near.file(item);
    const local: SnapLocal = { onAccess, around: near.around };
    const keep = (item: LandscapeItem): void => { items.push(item); near.file(item); file(item); };
    // … 5 lines
    const medianTrees = trees && set === 'complete' && segment.direction === 'both' && plantedMedian(ribbon.road);
    if (medianTrees) {
      const shift = travelShift(ribbon.road);
      for (let s = startS + m(8); s <= endS - m(6); s += m(10)) {
        const f = line.sampleAt(s);
        const snap = snapLandscape(net, items, 'tree', { x: f.p.x + f.n.x * shift, y: f.p.y + f.n.y * shift }, m(0.6), local);
        if (!snap.ok || !snap.median || snap.median.segment !== id) continue;
        out.push({ kind: 'tree', at: snap.at });
        keep({ id: nextId--, kind: 'tree', x: snap.at.x, y: snap.at.y });
      }
    }
    for (const side of ['right', 'left'] as const satisfies readonly RoadSide[]) {
      // … 8 lines
      for (const pattern of SETS[set]) {
        // … 2 lines
        if (pattern.kind === 'tree' && (medianTrees || !trees)) continue;
        // … 2 lines
        for (let s = startS + pattern.first + (pattern.kind === 'lamp' ? stagger : 0); s <= endS - m(4); s += pattern.every) {
          // … 2 lines
          const snap = snapLandscape(net, items, pattern.kind, at, m(0.6), local);
          if (!snap.ok || snap.hit?.segment !== id) continue;
          out.push({ kind: pattern.kind, at: snap.at });
          keep({ id: nextId--, kind: pattern.kind, x: snap.at.x, y: snap.at.y });
        }
      }
    }
  }
// … 1 lines
}

// src/world/roads/furnitureSets.ts:83-88 slice of src/world/roads/furnitureSets.ts#furnitureFor.file (shared): the feature's statements; "…" marks code left out
  const file = (item: LandscapeItem): void => {
    const key = cellOf(item.x, item.y);
    let list = grid.get(key);
    if (!list) grid.set(key, list = []);
    list.push(item);
  };
