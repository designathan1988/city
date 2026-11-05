// The code of src/world/roads/streetNames.ts that is the feature's (the rest of the file is shared).

// src/world/roads/streetNames.ts:57-66 function streetNameOf: the feature's own
export function streetNameOf(net: Network, segment: SegmentId): string | null {
  const ids = streetChain(net.doc, segment).map((p) => p.id);
  const chain = new Set(ids);
  for (const item of platesNear(net, ids)) {
    if (!item.text) continue;
    const seg = nameItemSegment(net, item);
    if (seg !== null && chain.has(seg)) return item.text;
  }
  return null;
}

// src/world/roads/streetNames.ts:99-105 interface StreetNumbers: the feature's own
export interface StreetNumbers {
  /** Metres from the street's start where the segment begins and ends. */
  readonly from: number;
  readonly to: number;
  /** Even numbers on the segment's own right (a -> b) when true. */
  readonly evenOnRight: boolean;
}

// src/world/roads/streetNames.ts:107-141 function streetNumbers: the feature's own
export function streetNumbers(doc: RoadDoc, net: Network, segment: SegmentId, zero: Vec2 = { x: 0, y: 0 }): StreetNumbers | null {
  const chain = streetChain(doc, segment);
  if (!chain.length) return null;
  // The ends of the street: the nodes only one of its segments meets.
  const count = new Map<NodeId, number>();
  for (const p of chain) {
    const s = doc.segment(p.id);
    if (!s) continue;
    for (const n of [s.a, s.b]) count.set(n, (count.get(n) ?? 0) + 1);
  }
  const ends = [...count.entries()].filter(([, c]) => c === 1).map(([n]) => n);
  let start: NodeId | undefined;
  let bestD = Infinity;
  for (const n of ends) {
    const node = doc.node(n);
    if (!node) continue;
    const d = Math.hypot(node.x - zero.x, node.y - zero.y);
    if (d < bestD || (d === bestD && start !== undefined && n < start)) { bestD = d; start = n; }
  }
  if (start === undefined) start = doc.segment(chain[0]!.id)!.a; // a loop: from the first segment
  // Walk from the start, adding up lengths.
  const remaining = new Set(chain.map((p) => p.id));
  let node = start, along = 0;
  for (let guard = 0; guard < chain.length + 1 && remaining.size; guard++) {
    const next = [...remaining].find((id) => { const s = doc.segment(id); return s && (s.a === node || s.b === node); });
    if (next === undefined) break;
    remaining.delete(next);
    const s = doc.segment(next)!;
    const length = net.polylines.get(doc, next).length * METERS_PER_UNIT;
    if (next === segment) return { from: Math.round(along), to: Math.round(along + length), evenOnRight: s.a === node };
    along += length;
    node = s.a === node ? s.b : s.a;
  }
  return null;
}
