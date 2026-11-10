// The code of src/render/signLayer.ts that is the feature's (the rest of the file is shared).

// src/render/signLayer.ts:19-26 slice of src/render/signLayer.ts#Stand (shared): the feature's statements; "…" marks code left out
interface Stand {
  // … 3 lines
  readonly type: SignType;
// … 2 lines
}

// src/render/signLayer.ts:34-38 slice of src/render/signLayer.ts#SignSource (shared): the feature's statements; "…" marks code left out
export interface SignSource {
  // … 2 lines
  readonly elevation: RoadElevation;
}

// src/render/signLayer.ts:56-56 slice of src/render/signLayer.ts#isSignItem (shared): the feature's statements; "…" marks code left out
const isSignItem = (item: LandscapeItem): boolean => item.kind === 'sign' || (item.kind === 'streetname' && !!item.text);

// src/render/signLayer.ts:73-79 slice of src/render/signLayer.ts#SignProduct.allKeys (shared): the feature's statements; "…" marks code left out
  allKeys(source: SignSource): Iterable<number> {
    // … 2 lines
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
    // … 7 lines
    for (const item of net.doc.landscapeInAny(change.rects.map((rect) => grow(rect, ITEM_READS)))) if (isSignItem(item)) out.add(itemKey(item.id));
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

// src/render/signLayer.ts:214-217 slice of src/render/signLayer.ts#Standing (shared): the feature's statements; "…" marks code left out
interface Standing {
  // … 1 lines
  readonly plates: readonly { readonly store: PlateStore; readonly slot: number; readonly type: SignType; readonly text: string }[];
}

// src/render/signLayer.ts:219-232 slice of src/render/signLayer.ts#SignLayerKept (shared): the feature's statements; "…" marks code left out
export interface SignLayerKept {
  // … 2 lines
  sync(net: Network, elevation: RoadElevation): number;
// … 9 lines
}

// src/render/signLayer.ts:322-345 slice of src/render/signLayer.ts#createSignLayer.putQuad (shared): the feature's statements; "…" marks code left out
  const putQuad = (placed: Placed, face: { x: number; y: number; yaw: number }, ground: number, type: SignType): { store: PlateStore; slot: number } => {
    const store = storeOf(placed.sheet);
    let slot = store.free.pop();
    if (slot === undefined) {
      if (store.used === store.capacity) sized(store, Math.ceil(store.capacity * 1.5) + 512);
      slot = store.used++;
      store.mesh.geometry.setDrawRange(0, store.used * 6);
    }
    const size = PLATE[type];
    const g = store.mesh.geometry;
    const position = (g.getAttribute('position') as BufferAttribute).array as Float32Array;
    const normal = (g.getAttribute('normal') as BufferAttribute).array as Float32Array;
    // The quad of the plane of 1 x 1, turned and sized as `buildSigns` turned it.
    matrix.makeRotationY(face.yaw).scale(scale.set(m(size.w), m(size.h), 1)).setPosition(face.x, ground + m(size.z), -face.y);
    QUAD_ORDER.forEach((i, k) => {
      corner.set(QUAD[i]![0], QUAD[i]![1], 0).applyMatrix4(matrix);
      position[slot * QUAD_3 + k * 3] = corner.x; position[slot * QUAD_3 + k * 3 + 1] = corner.y; position[slot * QUAD_3 + k * 3 + 2] = corner.z;
      normal[slot * QUAD_3 + k * 3] = Math.sin(face.yaw); normal[slot * QUAD_3 + k * 3 + 1] = 0; normal[slot * QUAD_3 + k * 3 + 2] = Math.cos(face.yaw);
    });
    store.cells[slot] = placed;
    writeUv(store, slot, placed);
    touch(store, slot);
    return { store, slot };
  };
