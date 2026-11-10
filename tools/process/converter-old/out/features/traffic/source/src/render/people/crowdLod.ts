// The code of src/render/people/crowdLod.ts that is the feature's (the rest of the file is shared).

// src/render/people/crowdLod.ts:17-25 function levelFor: the feature's own
export function levelFor(pixels: number, was: number): number {
  for (let level = 0; level < LEVELS; level++) {
    // Moving to a closer level than now needs the margin.
    const edge = LEVEL_PIXELS[level]!;
    const need = level < was || was < 0 ? edge * (1 + HYSTERESIS) : edge;
    if (pixels >= need) return level;
  }
  return -1;
}

// src/render/people/crowdLod.ts:53-55 function itemShownAt: the feature's own
export function itemShownAt(share: number, level: number): boolean {
  return share * LEVEL_TOP_PIXELS[level]! >= ITEM_LEAST_PIXELS;
}

// src/render/people/crowdLod.ts:67-78 function boneZone: the feature's own
export function boneZone(name: string): number {
  const n = name.toLowerCase();
  if (/finger|hand|wrist|metacarpal|thumb/.test(n)) return FarZone.hand;
  if (/forearm|lowerarm/.test(n)) return FarZone.forearm;
  if (/upperarm|shoulder/.test(n)) return FarZone.upperArm;
  if (/thigh|upperleg/.test(n)) return FarZone.thigh;
  if (/calf|lowerleg/.test(n)) return FarZone.calf;
  if (/foot|toe|ball/.test(n)) return FarZone.foot;
  if (/pelvis|hip/.test(n)) return FarZone.pelvis;
  if (/spine|clavicle|breast|chest|root|^bip01$/.test(n)) return FarZone.torso;
  return FarZone.face;
}

// src/render/people/crowdLod.ts:83-85 function headZone: the feature's own
export function headZone(y: number, z: number, head: HeadFrame): number {
  return y > head.eyeY + 0.35 * head.above || (z < head.eyeZ - 0.9 * head.above && y > head.eyeY - 0.6 * head.above) ? FarZone.hair : FarZone.face;
}

// src/render/people/crowdLod.ts:92-99 function positionsOf: the feature's own
function positionsOf(geometry: BufferGeometry): Float32Array {
  const position = geometry.getAttribute('position');
  const out = new Float32Array(position.count * 3);
  for (let i = 0; i < position.count; i++) {
    out[i * 3] = position.getX(i); out[i * 3 + 1] = position.getY(i); out[i * 3 + 2] = position.getZ(i);
  }
  return out;
}

// src/render/people/crowdLod.ts:102-107 function simplifiedIndex: the feature's own
function simplifiedIndex(geometry: BufferGeometry, triangles: number, indices?: Uint32Array): Uint32Array | null {
  const source = indices ?? (geometry.getIndex() ? Uint32Array.from(geometry.getIndex()!.array as ArrayLike<number>) : null);
  if (!source || !geometry.getAttribute('position')) return null;
  if (!Number.isFinite(triangles) || source.length <= triangles * 3) return source;
  return simplifyIndex(source, positionsOf(geometry), triangles);
}

// src/render/people/crowdLod.ts:110-118 function simplifiedIndexAway: the feature's own
export async function simplifiedIndexAway(geometry: BufferGeometry, triangles: number, indices?: Uint32Array): Promise<Uint32Array | null> {
  const source = indices ?? (geometry.getIndex() ? Uint32Array.from(geometry.getIndex()!.array as ArrayLike<number>) : null);
  if (!source || !geometry.getAttribute('position')) return null;
  if (!Number.isFinite(triangles) || source.length <= triangles * 3) return source;
  const pool = lodPool();
  if (!pool || !MeshoptSimplifier.supported) return simplifiedIndex(geometry, triangles, indices);
  // A copy goes: the worker takes what it is sent, and the caller may still use its own.
  return pool.index({ positions: positionsOf(geometry), index: source.slice(), triangles });
}

// src/render/people/crowdLod.ts:121-128 function filterTriangles: the feature's own
export function filterTriangles(index: ArrayLike<number>, keep: (v: number) => boolean): Uint32Array {
  const out: number[] = [];
  for (let t = 0; t + 2 < index.length; t += 3) {
    const a = index[t]!, b = index[t + 1]!, c = index[t + 2]!;
    if (keep(a) && keep(b) && keep(c)) out.push(a, b, c);
  }
  return Uint32Array.from(out);
}

// src/render/people/crowdLod.ts:131-134 interface CardSelection: the feature's own
export interface CardSelection {
  readonly index: Uint32Array;
  readonly widen: number;
}

// src/render/people/crowdLod.ts:137-186 function cardSelections: the feature's own
export function cardSelections(geometry: BufferGeometry, budgets: readonly number[]): { cardOf: Int32Array; selections: (CardSelection | null)[] } {
  const index = geometry.getIndex();
  const position = geometry.getAttribute('position');
  const count = position.count;
  const cardOf = new Int32Array(count).fill(-1);
  if (!index) return { cardOf, selections: budgets.map(() => null) };
  const tris = index.count / 3;
  // Cards: the triangles joined by shared vertices (union-find).
  const parent = new Int32Array(count).map((_, i) => i);
  const find = (i: number): number => { while (parent[i] !== i) { parent[i] = parent[parent[i]!]!; i = parent[i]!; } return i; };
  for (let t = 0; t < tris; t++) {
    const a = find(index.getX(t * 3)), b = find(index.getX(t * 3 + 1));
    parent[b] = a;
    parent[find(index.getX(t * 3 + 2))] = a;
  }
  const roots = new Map<number, number>();
  for (let v = 0; v < count; v++) {
    const r = find(v);
    let id = roots.get(r);
    if (id === undefined) { id = roots.size; roots.set(r, id); }
    cardOf[v] = id;
  }
  const cards: { tris: number[]; area: number }[] = Array.from({ length: roots.size }, () => ({ tris: [], area: 0 }));
  let total = 0;
  for (let t = 0; t < tris; t++) {
    const i0 = index.getX(t * 3), i1 = index.getX(t * 3 + 1), i2 = index.getX(t * 3 + 2);
    const ax = position.getX(i1) - position.getX(i0), ay = position.getY(i1) - position.getY(i0), az = position.getZ(i1) - position.getZ(i0);
    const bx = position.getX(i2) - position.getX(i0), by = position.getY(i2) - position.getY(i0), bz = position.getZ(i2) - position.getZ(i0);
    const area = 0.5 * Math.hypot(ay * bz - az * by, az * bx - ax * bz, ax * by - ay * bx);
    const card = cards[cardOf[i0]!]!;
    card.tris.push(t);
    card.area += area;
    total += area;
  }
  const sorted = [...cards].sort((p, q) => q.area - p.area);
  const selections = budgets.map((budget): CardSelection | null => {
    if (budget <= 0) return null;
    if (!Number.isFinite(budget) || tris <= budget) return { index: Uint32Array.from(index.array as ArrayLike<number>), widen: 1 };
    const out: number[] = [];
    let kept = 0, area = 0;
    for (const card of sorted) {
      if (kept + card.tris.length > budget && kept > 0) break;
      for (const t of card.tris) out.push(index.getX(t * 3), index.getX(t * 3 + 1), index.getX(t * 3 + 2));
      kept += card.tris.length;
      area += card.area;
    }
    return { index: Uint32Array.from(out), widen: Math.min(1.6, Math.sqrt(total / Math.max(1e-9, area))) };
  });
  return { cardOf, selections };
}

// src/render/people/crowdLod.ts:189-210 function cardCentres: the feature's own
export function cardCentres(geometry: BufferGeometry, cardOf: Int32Array): Float32Array {
  const position = geometry.getAttribute('position');
  let cards = 0;
  for (let v = 0; v < cardOf.length; v++) cards = Math.max(cards, cardOf[v]! + 1);
  const sum = new Float64Array(cards * 4);
  for (let v = 0; v < position.count; v++) {
    const c = cardOf[v]!;
    if (c < 0) continue;
    sum[c * 4] = sum[c * 4]! + position.getX(v);
    sum[c * 4 + 1] = sum[c * 4 + 1]! + position.getY(v);
    sum[c * 4 + 2] = sum[c * 4 + 2]! + position.getZ(v);
    sum[c * 4 + 3] = sum[c * 4 + 3]! + 1;
  }
  const out = new Float32Array(position.count * 3);
  for (let v = 0; v < position.count; v++) {
    const c = cardOf[v]!;
    if (c < 0) { out[v * 3] = position.getX(v); out[v * 3 + 1] = position.getY(v); out[v * 3 + 2] = position.getZ(v); continue; }
    const n = Math.max(1, sum[c * 4 + 3]!);
    out[v * 3] = sum[c * 4]! / n; out[v * 3 + 1] = sum[c * 4 + 1]! / n; out[v * 3 + 2] = sum[c * 4 + 2]! / n;
  }
  return out;
}

// src/render/people/crowdLod.ts:216-225 interface FarPart: the feature's own
export interface FarPart {
  readonly geometry: BufferGeometry;
  readonly indices: Uint32Array;
  readonly region: number;
  readonly triangles: number;
  /** A vertex's own colour (linear RGB), multiplied by the person's in the shader; white without. */
  readonly colour?: (v: number) => readonly [number, number, number];
  /** A vertex's zone (`FarZone`), in place of the part's region. */
  readonly zoneOf?: (v: number) => number;
}

// src/render/people/crowdLod.ts:228-270 function farMesh: the feature's own
export function farMesh(parts: readonly FarPart[], ankle: number): BufferGeometry | null {
  const keep = ['position', 'normal', 'skinIndex', 'skinWeight', 'aRefs', 'aRefW'];
  const pieces: BufferGeometry[] = [];
  for (const part of parts) {
    const index = simplifiedIndex(part.geometry, part.triangles, part.indices);
    if (!index || !index.length) continue;
    // Only the vertices the simplified index uses, renumbered.
    const remap = new Map<number, number>();
    const order: number[] = [];
    const out: number[] = [];
    for (const v of index) {
      let n = remap.get(v);
      if (n === undefined) { n = order.length; remap.set(v, n); order.push(v); }
      out.push(n);
    }
    const g = new BufferGeometry();
    for (const name of keep) {
      const attribute = part.geometry.getAttribute(name);
      if (!attribute) return null;
      const size = attribute.itemSize;
      // Made in its own type and kept by the attribute (`BufferAttribute`): the typed subclasses copy the array they are given.
      const values = name === 'skinIndex' ? new Uint16Array(order.length * size) : new Float32Array(order.length * size);
      order.forEach((v, i) => { for (let c = 0; c < size; c++) values[i * size + c] = attribute.getComponent(v, c); });
      g.setAttribute(name, new BufferAttribute(values, size));
    }
    const position = g.getAttribute('position');
    const far = new Float32Array(order.length * 4);
    order.forEach((v, i) => {
      const [r, gr, b] = part.colour?.(v) ?? [1, 1, 1];
      far[i * 4] = r; far[i * 4 + 1] = gr; far[i * 4 + 2] = b;
      far[i * 4 + 3] = part.zoneOf ? part.zoneOf(v) : part.region === REGION.skin && position.getY(i) < ankle ? REGION.shoes : part.region;
    });
    g.setAttribute('aFar', new BufferAttribute(far, 4));
    g.setIndex(out);
    pieces.push(g);
  }
  if (!pieces.length) return null;
  const merged = mergeGeometries(pieces, false);
  for (const p of pieces) p.dispose();
  if (!merged) return null;
  merged.computeBoundingSphere();
  return merged;
}

// src/render/people/crowdLod.ts:274-287 function sortSkinWeights: the feature's own
export function sortSkinWeights(geometry: BufferGeometry): void {
  const index = geometry.getAttribute('skinIndex');
  const weight = geometry.getAttribute('skinWeight');
  if (!index || !weight || sortedWeights.has(weight)) return;
  const pairs: [number, number][] = [[0, 0], [0, 0], [0, 0], [0, 0]];
  for (let v = 0; v < index.count; v++) {
    for (let k = 0; k < 4; k++) { pairs[k]![0] = index.getComponent(v, k); pairs[k]![1] = weight.getComponent(v, k); }
    pairs.sort((a, b) => b[1] - a[1]);
    for (let k = 0; k < 4; k++) { index.setComponent(v, k, pairs[k]![0]); weight.setComponent(v, k, pairs[k]![1]); }
  }
  index.needsUpdate = true;
  weight.needsUpdate = true;
  sortedWeights.add(weight);
}
