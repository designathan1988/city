// The code of src/world/landscape.ts that is the feature's (the rest of the file is shared).

// src/world/landscape.ts:35-39 function plantGrowth: the feature's own
export function plantGrowth(planted: number | undefined, now: number): number {
  if (planted === undefined || !Number.isFinite(now)) return 1;
  const t = Math.max(0, Math.min(1, (now - planted) / GROW_MINUTES));
  return PLANTED_SIZE + (1 - PLANTED_SIZE) * (t * t * (3 - 2 * t));
}

// src/world/landscape.ts:238-245 function crossingAccesses: the feature's own
export function crossingAccesses(net: Network): CrossingAccess[] {
  // Asked once per item placed: the city generator places thousands (V7).
  const known = accessMemo.get(net);
  if (known && known.revision === net.revision && known.trafficRevision === net.trafficRevision) return known.list;
  const list = crossingAccessesNow(net);
  accessMemo.set(net, { revision: net.revision, trafficRevision: net.trafficRevision, list });
  return list;
}

// src/world/landscape.ts:246-248 function crossingAccessesNow: the feature's own
function crossingAccessesNow(net: Network): CrossingAccess[] {
  return crossingAccessesAt(net, net.doc.nodes.keys());
}

// src/world/landscape.ts:296-313 function crossingAccessTester: the feature's own
export function crossingAccessTester(net: Network, accesses: readonly CrossingAccess[]): (segment: SegmentId, x: number, y: number, radius: number) => boolean {
  const cells = new Map<string, CrossingAccess[]>();
  let reach = 0;
  for (const a of accesses) {
    reach = Math.max(reach, Math.hypot(CROSSWALK_DEPTH / 2 + m(0.3), a.across));
    const key = `${Math.floor(a.x / ACCESS_CELL)},${Math.floor(a.y / ACCESS_CELL)}`;
    const list = cells.get(key);
    if (list) list.push(a); else cells.set(key, [a]);
  }
  return (segment, x, y, radius) => {
    const r = reach + radius * Math.SQRT2;
    const near: CrossingAccess[] = [];
    for (let cx = Math.floor((x - r) / ACCESS_CELL); cx <= Math.floor((x + r) / ACCESS_CELL); cx++) {
      for (let cy = Math.floor((y - r) / ACCESS_CELL); cy <= Math.floor((y + r) / ACCESS_CELL); cy++) near.push(...(cells.get(`${cx},${cy}`) ?? []));
    }
    return near.length > 0 && onCrossingAccess(net, near, segment, x, y, radius);
  };
}
