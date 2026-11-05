// The code of src/render/buildings/frameBudget.ts that is the feature's (the rest of the file is shared).

// src/render/buildings/frameBudget.ts:11-20 function letGoPassed: the feature's own
export function letGoPassed(heldFar: Set<string>, heldCoarse: Set<string>, kept: Iterable<{ readonly key: string; readonly fine: boolean }>, away: (key: string) => number): void {
  let nearest = -1, finest = -1;
  for (const { key, fine } of kept) {
    const d = away(key);
    if (d > nearest) nearest = d;
    if (fine && d > finest) finest = d;
  }
  for (const key of heldFar) if (away(key) < nearest) heldFar.delete(key);
  for (const key of heldCoarse) if (away(key) < finest) heldCoarse.delete(key);
}

// src/render/buildings/frameBudget.ts:23-37 function letBackFitting: the feature's own
export function letBackFitting(heldFar: Set<string>, heldCoarse: Set<string>, drawn: number, budget: number, rank: (key: string) => number, more: (key: string, far: boolean) => number): void {
  // Each key ranked once, not twice per comparison (PRD-30): the same stable order.
  const keys = new Set(heldCoarse);
  for (const key of heldFar) keys.add(key);
  const ranked: { key: string; rank: number }[] = [];
  for (const key of keys) ranked.push({ key, rank: rank(key) });
  ranked.sort((a, b) => a.rank - b.rank);
  for (const { key } of ranked) {
    const far = heldFar.has(key);
    const add = more(key, far);
    if (drawn + add > budget) continue;
    drawn += add;
    if (far) { heldFar.delete(key); heldCoarse.add(key); } else heldCoarse.delete(key);
  }
}

// src/render/buildings/frameBudget.ts:41-54 function fitErrorScale: the feature's own
export function fitErrorScale(scale: number, floor: number, budget: number, count: (scale: number) => number): number {
  let k = Math.max(floor, Math.min(MOST_ERROR_SCALE, scale));
  let n = count(k);
  while (n > budget && k < MOST_ERROR_SCALE) {
    k = Math.min(MOST_ERROR_SCALE, k * ERROR_STEP);
    n = count(k);
  }
  if (n <= budget * SLACK && k > floor) {
    const finer = Math.max(floor, k / ERROR_STEP);
    // `count` chooses as it counts: the scale kept is the one chosen last.
    if (count(finer) <= budget) k = finer; else count(k);
  }
  return k;
}

// src/render/buildings/frameBudget.ts:57-64 class FrameScale: the feature's own
export class FrameScale {
  budget = Infinity;
  scale = 1;
  /** The levels chosen at the scale that fits the budget: `count` chooses at a scale and says their triangles. */
  fit(count: (scale: number) => number): void {
    this.scale = fitErrorScale(this.scale, 1, this.budget, count);
  }
}

// src/render/buildings/frameBudget.ts:70-142 class LandedMeasure: the feature's own
export class LandedMeasure<K extends string> {
  private readonly landed = new Map<string, number>();
  private readonly sums = new Map<K, { sum: number; n: number }>();
  /** Each landed level's weight when it landed (`weightOf`), and per kind the levels' sum over their weights' sum. */
  private readonly weights = new Map<string, number>();
  private readonly ratios = new Map<K, { measured: number; weight: number }>();
  /** Called whenever what it answers may change (`layer.ts` levelOfDetail chooses the levels again then). */
  changed: () => void = () => {};
  /** What a cell weighs whatever its level (`layer.ts`: its masses' triangles, always at hand), 0 when unknown: a level never landed, or let go, is reckoned at its weight times what the landed ones measured per unit of theirs. DO NOT reckon it at a dense cell's `typical`: a suburb's cell let go came back as 1.3 M triangles, never fitted the budget again and stood in its masses for good. */
  weightOf: (key: string) => number = () => 0;
  constructor(private readonly typical: Readonly<Record<K, number>>) {}
  set(kind: K, key: string, triangles: number): void {
    const at = `${kind}|${key}`;
    const s = this.sums.get(kind) ?? { sum: 0, n: 0 };
    const was = this.landed.get(at);
    if (was === triangles) return;
    this.changed();
    if (was !== undefined) s.sum -= was; else s.n++;
    s.sum += triangles;
    this.sums.set(kind, s);
    this.landed.set(at, triangles);
    this.unweigh(kind, at, was);
    const weight = this.weightOf(key);
    if (weight > 0) {
      const r = this.ratios.get(kind) ?? { measured: 0, weight: 0 };
      r.measured += triangles; r.weight += weight;
      this.ratios.set(kind, r);
      this.weights.set(at, weight);
    }
  }
  /** The cell's levels of the other kinds that landed, with what they measured (none when the typical figures are not numbers). */
  private *othersOf(kind: K, key: string): Generator<[K, number]> {
    for (const other of Object.keys(this.typical) as K[]) {
      if (other === kind || !(this.typical[other] > 0) || !(this.typical[kind] > 0)) continue;
      const measured = this.landed.get(`${other}|${key}`);
      if (measured !== undefined) yield [other, measured];
    }
  }
  private unweigh(kind: K, at: string, was: number | undefined): void {
    const weight = this.weights.get(at);
    if (weight === undefined || was === undefined) return;
    const r = this.ratios.get(kind)!;
    r.measured -= was; r.weight -= weight;
    this.weights.delete(at);
  }
  /** A cell gone from the town (its tile left the active region): its measures let go, out of the means too (DO NOT keep every cell the camera ever passed: the books grew with the road walked, not with the view). */
  forget(key: string): void {
    for (const [kind, s] of this.sums) {
      const at = `${kind}|${key}`;
      const was = this.landed.get(at);
      if (was === undefined) continue;
      this.changed();
      this.unweigh(kind, at, was);
      this.landed.delete(at);
      s.sum -= was;
      s.n--;
    }
  }
  /** How many levels are measured (`BuildingLayer.books`). */
  get size(): number {
    return this.landed.size;
  }
  of(kind: K, key: string): number {
    const got = this.landed.get(`${kind}|${key}`);
    if (got !== undefined) return got;
    const weight = this.weightOf(key), r = this.ratios.get(kind);
    if (weight > 0 && r && r.weight > 0) return (weight * r.measured) / r.weight;
    // The same cell's other level measured: this one in the typical proportion between the two. DO NOT reckon a cell's facade level at a dense cell's 600 k when its detail measured 3 k: stepping it down to that level looked dearer than its detail, and it went to its masses.
    for (const [other, measured] of this.othersOf(kind, key)) return (measured * this.typical[kind]) / this.typical[other];
    const s = this.sums.get(kind);
    return s?.n ? s.sum / s.n : this.typical[kind];
  }
}
