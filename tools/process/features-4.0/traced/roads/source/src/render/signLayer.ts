// The code of src/render/signLayer.ts that is the feature's (the rest of the file is shared).

// src/render/signLayer.ts:34-38 slice of src/render/signLayer.ts#SignSource (shared): the feature's statements; "…" marks code left out
export interface SignSource {
  // … 1 lines
  readonly net: Network;
  readonly elevation: RoadElevation;
}

// src/render/signLayer.ts:49-54 slice of src/render/signLayer.ts#ribbonBox (shared): the feature's statements; "…" marks code left out
function ribbonBox(net: Network, id: SegmentId): ChangeRect | null {
  const ribbon = net.ribbons.get(id);
  if (!ribbon) return null;
  const b = ribbon.full.bbox, pad = ribbon.road.width / 2 + ribbon.road.sidewalk + m(1);
  return [b.minX - pad, b.minY - pad, b.maxX + pad, b.maxY + pad];
}

// src/render/signLayer.ts:73-79 slice of src/render/signLayer.ts#SignProduct.allKeys (shared): the feature's statements; "…" marks code left out
  allKeys(source: SignSource): Iterable<number> {
    // … 1 lines
    for (const id of source.net.doc.nodes.keys()) out.push(nodeKey(id));
    for (const item of source.net.doc.landscape.values()) if (isSignItem(item)) out.push(itemKey(item.id));
  // … 2 lines
  }

// src/render/signLayer.ts:81-110 slice of src/render/signLayer.ts#SignProduct.dirty (shared): the feature's statements; "…" marks code left out
  dirty(change: Change, out: Set<number>, source: SignSource): void {
    // … 6 lines
    if (change.kind === 'landscape') {
      // … 1 lines
      if (change.ids) {
        for (const id of change.ids) {
          const item = net.doc.landscape.get(id);
          if (!item || isSignItem(item)) out.add(itemKey(id));
        }
      // … 1 lines
      }
      // … 2 lines
      for (const item of net.doc.landscapeInAny(change.rects.map((rect) => grow(rect, ITEM_READS)))) if (isSignItem(item)) out.add(itemKey(item.id));
    // … 1 lines
    }
    // … 3 lines
    for (const id of net.segmentCells.idsMeetingAny(change.rects)) {
      const seg = net.doc.segment(id);
      if (seg) { out.add(nodeKey(seg.a)); out.add(nodeKey(seg.b)); }
    }
    for (const item of net.doc.landscapeInAny(change.rects.map((rect) => grow(rect, ITEM_READS)))) if (isSignItem(item)) out.add(itemKey(item.id));
  }

// src/render/signLayer.ts:137-152 slice of src/render/signLayer.ts#junctionSigns (shared): the feature's statements; "…" marks code left out
function junctionSigns({ net, elevation }: SignSource, node: NodeId): SignValue | undefined {
  const at = net.doc.node(node);
  if (!at) return undefined;
  // … 1 lines
  for (const sign of signsAtNode(net, node)) {
    const ground = elevation.onSegment(sign.segment, sign.x, sign.y) + footwayRiseAt(net, sign.x, sign.y);
    stands.push({ x: sign.x, y: sign.y, ground, type: sign.type, text: sign.text, yaw: Math.atan2(sign.facing.x, -sign.facing.y) });
  }
  // … 1 lines
  let reads: ChangeRect = around(at.x, at.y, ITEM_READS);
  for (const seg of at.incident) {
    const box = ribbonBox(net, seg);
    if (box) reads = unite(reads, box);
  }
  return { stands, reads };
}

// src/render/signLayer.ts:155-196 slice of src/render/signLayer.ts#itemSigns (shared): the feature's statements; "…" marks code left out
function itemSigns({ net, elevation }: SignSource, id: number): SignValue | undefined {
  const item = net.doc.landscape.get(id);
  if (!item || !isSignItem(item)) return undefined;
  // … 1 lines
  let reads: ChangeRect = around(item.x, item.y, ITEM_READS);
  if (item.kind === 'sign') {
    const hit = footwayAt(net, item, m(0.6));
    if (!hit) return { stands, reads };
    const ground = elevation.onSegment(hit.segment, item.x, item.y) + footwayRiseAt(net, item.x, item.y);
    // A traffic sign faces the traffic coming towards it on its side of the street.
    const facing = { x: -hit.frame.t.x * hit.side, y: -hit.frame.t.y * hit.side };
    const type = item.signType ?? 'stop';
    const yaw = type === 'street' || type === 'direction' || type === 'info'
      ? Math.atan2(-hit.frame.n.x * hit.side, hit.frame.n.y * hit.side) // read from the street
      : Math.atan2(facing.x, -facing.y);
    stands.push({ x: item.x, y: item.y, ground, type, text: item.text ?? '', yaw });
    return { stands, reads };
  }
  // … 1 lines
  for (const seg of streetChain(net, item)) {
    const segment = net.doc.segment(seg), ribbon = net.ribbons.get(seg);
    if (!segment || !ribbon) continue;
    const box = ribbonBox(net, seg);
    if (box) reads = unite(reads, box);
    const section = sectionOf(ribbon.road, segment.direction);
    for (const [node, sign] of [[segment.a, 1], [segment.b, -1]] as const) {
      // On the right-hand footway seen from the corner: the road's right from `a`, its left from `b`.
      const zone = (sign > 0 ? section.right : section.left).furnishing;
      const out = zone.inner + Math.min(LAMP_ZONE, Math.max(zone.outer - zone.inner, m(0.2))) / 2;
      if ((net.doc.node(node)?.incident.length ?? 0) < 2) continue;
      const s = sign > 0 ? net.mouthDistance(seg, node) + m(1.5) : ribbon.full.length - net.mouthDistance(seg, node) - m(1.5);
      if (s <= 0 || s >= ribbon.full.length) continue;
      const f = ribbon.full.sampleAt(s);
      const side = sign > 0 ? -1 : 1;
      const x = f.p.x + f.n.x * out * side, y = f.p.y + f.n.y * out * side;
      const ground = elevation.onSegment(seg, x, y) + footwayRiseAt(net, x, y);
      // Parallel to the street, readable from both sides.
      stands.push({ x, y, ground, type: 'street', text: item.text!, yaw: Math.atan2(f.n.x, -f.n.y) });
    }
  }
  return { stands, reads };
}

// src/render/signLayer.ts:219-232 slice of src/render/signLayer.ts#SignLayerKept (shared): the feature's statements; "…" marks code left out
export interface SignLayerKept {
  // … 2 lines
  sync(net: Network, elevation: RoadElevation): number;
// … 9 lines
}

// src/render/signLayer.ts:394-408 slice of src/render/signLayer.ts#createSignLayer.sync (shared): the feature's statements; "…" marks code left out
    sync(net, elevation) {
      // … 1 lines
      const source: SignSource = { changes: net.doc.changes, net, elevation };
    // … 11 lines
    },
