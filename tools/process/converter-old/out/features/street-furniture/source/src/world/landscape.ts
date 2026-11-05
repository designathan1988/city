// The code of src/world/landscape.ts that is the feature's (the rest of the file is shared).

// src/world/landscape.ts:35-39 function plantGrowth: the feature's own
export function plantGrowth(planted: number | undefined, now: number): number {
  if (planted === undefined || !Number.isFinite(now)) return 1;
  const t = Math.max(0, Math.min(1, (now - planted) / GROW_MINUTES));
  return PLANTED_SIZE + (1 - PLANTED_SIZE) * (t * t * (3 - 2 * t));
}

// src/world/landscape.ts:89-91 function ribbonsNear: the feature's own
function ribbonsNear(net: Network, at: Vec2, reach: number): Iterable<SegmentRibbon> {
  return net.ribbonsMeeting([at.x - reach, at.y - reach, at.x + reach, at.y + reach]);
}

// src/world/landscape.ts:94-124 function footwayAt: the feature's own
export function footwayAt(net: Network, at: Vec2, reach = 0): FootwayHit | null {
  let best: FootwayHit | null = null;
  let bestMiss = Infinity;
  const hit = { s: 0, distance: 0 };
  for (const ribbon of ribbonsNear(net, at, reach)) {
    const road = ribbon.road;
    if (!carriesPedestrians(road) || road.sidewalk <= 0) continue;
    const segment = net.doc.segment(ribbon.id);
    if (!segment) continue;
    const box = ribbon.full.bbox;
    const pad = road.width / 2 + road.sidewalk + reach;
    if (at.x < box.minX - pad || at.x > box.maxX + pad || at.y < box.minY - pad || at.y > box.maxY + pad) continue;
    ribbon.full.closestInto(at.x, at.y, hit);
    // The side the point is on, and that side's own footway (an asymmetric road).
    const atFrame = ribbon.full.sampleAt(hit.s);
    const onLeft = (at.x - atFrame.p.x) * atFrame.n.x + (at.y - atFrame.p.y) * atFrame.n.y >= 0;
    const zones = zonesOn(sectionOf(road, segment.direction), onLeft ? 'left' : 'right');
    const inner = zones.curb.outer;
    const outer = zones.frontage.outer;
    const length = ribbon.full.length;
    // Not past the mouths: the corner of a junction belongs to no one leg.
    const lo = net.mouthDistance(ribbon.id, segment.a);
    const hi = length - net.mouthDistance(ribbon.id, segment.b);
    if (hit.s < lo - 1e-6 || hit.s > hi + 1e-6) continue;
    const miss = hit.distance < inner ? inner - hit.distance : hit.distance > outer ? hit.distance - outer : 0;
    if (miss > reach || miss >= bestMiss) continue;
    bestMiss = miss;
    best = { segment: ribbon.id, road, s: hit.s, side: onLeft ? 1 : -1, across: hit.distance, frame: atFrame };
  }
  return best;
}

// src/world/landscape.ts:139-142 function plantedMedian: the feature's own
export function plantedMedian(road: RoadType): boolean {
  const material = road.materials?.median;
  return road.median >= MEDIAN_TREE_MIN - 1e-6 && !road.medianFlush && (material === undefined || material === 'grass');
}

// src/world/landscape.ts:161-184 function medianAt: the feature's own
export function medianAt(net: Network, at: Vec2, reach = 0): MedianHit | null {
  const hit = { s: 0, distance: 0 };
  for (const ribbon of ribbonsNear(net, at, reach)) {
    const road = ribbon.road;
    if (!plantedMedian(road)) continue;
    const segment = net.doc.segment(ribbon.id);
    if (!segment || segment.direction !== 'both') continue;
    const box = ribbon.full.bbox;
    const pad = road.width / 2 + reach;
    if (at.x < box.minX - pad || at.x > box.maxX + pad || at.y < box.minY - pad || at.y > box.maxY + pad) continue;
    ribbon.full.closestInto(at.x, at.y, hit);
    const lo = Math.max(net.mouthDistance(ribbon.id, segment.a), medianNose(net, ribbon.id, segment.a));
    const hi = ribbon.full.length - Math.max(net.mouthDistance(ribbon.id, segment.b), medianNose(net, ribbon.id, segment.b));
    if (hit.s < lo || hit.s > hi) continue;
    const frame = ribbon.full.sampleAt(hit.s);
    // The median is centred on the travel way's own centre (`travelShift`).
    const shift = travelShift(road);
    const centre = { x: frame.p.x + frame.n.x * shift, y: frame.p.y + frame.n.y * shift };
    const off = Math.abs((at.x - centre.x) * frame.n.x + (at.y - centre.y) * frame.n.y);
    if (off > road.median / 2 + reach) continue;
    return { segment: ribbon.id, s: hit.s, centre, frame };
  }
  return null;
}

// src/world/landscape.ts:187-222 function depthFor: the feature's own
function depthFor(kind: LandscapeKind, road: RoadType, direction: 'both' | 'aToB' | 'bToA', side: RoadSide = 'right'): number | null {
  const zones = zonesOn(sectionOf(road, direction), side);
  const zone = zones.furnishing;
  const depth = zone.outer - zone.inner;
  if (depth <= 0) return null;
  switch (kind) {
    case 'lamp':
    case 'hydrant':
    case 'sign':
      return zone.inner + Math.min(LAMP_ZONE, depth) / 2;
    // A telephone booth is no column: placed as one, half a lamp's depth off the kerb, its box stood 0.27 m over it.
    case 'phone':
      if (depth < 2 * LANDSCAPE_RADIUS.phone - 1e-9) return null;
      return zone.inner + LANDSCAPE_RADIUS.phone;
    // A kerb inlet ("boca de lobo"): its mouth is IN the kerb, its grate in the gutter in front
    case 'drain':
      return zone.inner + m(0.05);
    // Long grass is placed on open ground and a street's name on the street itself (`snapLandscape`), never by depth.
    case 'meadow':
    case 'streetname':
      return null;
    case 'tree': {
      // A pit beside the kerb, so long as the walkers keep their through width behind it: a 2 m footway takes one
      const footway = zones.frontage.outer - zone.inner;
      if (footway - TREE_KERB_SETBACK - TREE_PIT < MIN_THROUGH - 1e-6) return null;
      return zone.inner + TREE_KERB_SETBACK + TREE_PIT / 2;
    }
    case 'shrub':
      return zone.inner + depth / 2;
    case 'bench':
    case 'bin':
    case 'postbox':
      if (depth < BENCH_ZONE - 1e-9) return null;
      return zone.inner + BENCH_ZONE / 2;
  }
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

// src/world/landscape.ts:251-262 function crossingAccessesAt: the feature's own
export function crossingAccessesAt(net: Network, nodes: Iterable<NodeId>): CrossingAccess[] {
  const out: CrossingAccess[] = [];
  for (const nodeId of nodes) {
    const node = net.doc.node(nodeId);
    if (!node || node.incident.length < 2) continue;
    for (const segmentId of node.incident) {
      const landing = net.crossingLandingAt(segmentId, nodeId);
      if (landing) out.push(landing);
    }
  }
  return out;
}

// src/world/landscape.ts:265-275 function onCrossingAccess: the feature's own
export function onCrossingAccess(
  net: Network, accesses: readonly CrossingAccess[], segment: SegmentId, x: number, y: number, radius: number,
): boolean {
  const structure = net.doc.segment(segment)?.structure ?? 'ground';
  return accesses.some((access) => {
    if (structure !== access.structure) return false;
    const dx = x - access.x, dy = y - access.y;
    return Math.abs(dx * access.tx + dy * access.ty) < CROSSWALK_DEPTH / 2 + m(0.3) + radius &&
      Math.abs(-dx * access.ty + dy * access.tx) < access.across + radius;
  });
}

// src/world/landscape.ts:281-290 function nearCrossingAccess: the feature's own
export function nearCrossingAccess(net: Network, segment: SegmentId, x: number, y: number, radius: number): boolean {
  const r = ACCESS_REACH + radius;
  const nodes = new Set<NodeId>();
  for (const ribbon of net.ribbonsMeeting([x - r, y - r, x + r, y + r])) {
    const s = net.doc.segment(ribbon.id);
    if (s) { nodes.add(s.a); nodes.add(s.b); }
  }
  if (nodes.size === 0) return false;
  return onCrossingAccess(net, crossingAccessesAt(net, [...nodes].sort((a, b) => a - b)), segment, x, y, radius);
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

// src/world/landscape.ts:320-323 interface SnapLocal: the feature's own
export interface SnapLocal {
  readonly onAccess?: (segment: SegmentId, x: number, y: number, radius: number) => boolean;
  readonly around?: (at: Vec2, radius: number) => Iterable<LandscapeItem>;
}

// src/world/landscape.ts:326-377 function snapLandscape: the feature's own
export function snapLandscape(
  net: Network,
  items: Iterable<LandscapeItem>,
  kind: LandscapeKind,
  at: Vec2,
  reach: number,
  local?: SnapLocal,
): LandscapeSnap {
  if (kind === 'streetname') {
    // A street's name is put on the street: its signs stand at the corners.
    if (!onAnyRoad(net, at)) return { ok: false, at, reason: 'offFootway' };
    return { ok: true, at, hit: null };
  }
  if (kind === 'meadow') {
    // A clump of long grass goes on open ground, never on a street.
    if (footwayAt(net, at, m(1)) || onAnyRoad(net, at)) return { ok: false, at, reason: 'offFootway' };
    for (const other of items) {
      if (other.kind === 'meadow' && apart(other, at) < m(2.5)) return { ok: false, at, reason: 'occupied' };
    }
    return { ok: true, at, hit: null };
  }
  const hit = footwayAt(net, at, reach);
  // A tree or a shrub in a planted median, on its centre line (V7).
  const median = !hit && (kind === 'tree' || kind === 'shrub') ? medianAt(net, at, Math.min(reach, m(1))) : null;
  // The items that can stand in its way: within its radius, the widest item's and the gap.
  const others = (p: Vec2, radius: number): Iterable<LandscapeItem> => local?.around ? local.around(p, radius + MAX_ITEM_RADIUS + ITEM_GAP) : items;
  if (median) {
    const radius = LANDSCAPE_RADIUS[kind];
    for (const other of others(median.centre, radius)) {
      const clear = radius + LANDSCAPE_RADIUS[other.kind] + ITEM_GAP;
      if (apart(other, median.centre) < clear) return { ok: false, at: median.centre, reason: 'occupied' };
    }
    return { ok: true, at: median.centre, hit: null, median };
  }
  if (!hit) return { ok: false, at, reason: 'offFootway' };
  const segment = net.doc.requireSegment(hit.segment);
  const depth = depthFor(kind, hit.road, segment.direction, hit.side > 0 ? 'left' : 'right');
  if (depth === null) return { ok: false, at, reason: 'narrow' };
  const placed = {
    x: hit.frame.p.x + hit.frame.n.x * depth * hit.side,
    y: hit.frame.p.y + hit.frame.n.y * depth * hit.side,
  };
  const radius = LANDSCAPE_RADIUS[kind];
  if (local?.onAccess ? local.onAccess(hit.segment, placed.x, placed.y, radius) : nearCrossingAccess(net, hit.segment, placed.x, placed.y, radius)) {
    return { ok: false, at: placed, reason: 'crossing' };
  }
  for (const other of others(placed, radius)) {
    const clear = radius + LANDSCAPE_RADIUS[other.kind] + ITEM_GAP;
    if (apart(other, placed) < clear) return { ok: false, at: placed, reason: 'occupied' };
  }
  return { ok: true, at: placed, hit };
}

// src/world/landscape.ts:404-409 function onAnyRoad: the feature's own
function onAnyRoad(net: Network, at: Vec2): boolean {
  for (const ribbon of ribbonsNear(net, at, m(1))) {
    if (ribbon.full.distanceTo(at) < ribbon.road.width / 2 + ribbon.road.sidewalk + m(1)) return true;
  }
  return false;
}
