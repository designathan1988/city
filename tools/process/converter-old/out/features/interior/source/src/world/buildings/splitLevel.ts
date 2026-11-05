// The code of src/world/buildings/splitLevel.ts that is the feature's (the rest of the file is shared).

// src/world/buildings/splitLevel.ts:243-243 function liftStep: the feature's own
const liftStep = (b: Building): number => levelHeight(b, 0) / 2;

// src/world/buildings/splitLevel.ts:246-250 function snapLift: the feature's own
export function snapLift(b: Building, lift: number): number {
  const step = liftStep(b);
  const max = Math.floor(MAX_LIFT / step + 1e-9) * step;
  return Math.max(-max, Math.min(max, Math.round(lift / step) * step));
}

// src/world/buildings/splitLevel.ts:253-280 function opSetLift: the feature's own
export function opSetLift(b: Building, id: number, lift: number): boolean {
  const v = volumeById(b, id);
  if (!v || v.open || !Number.isFinite(lift)) return false;
  const target = Math.max(-MAX_LIFT, Math.min(MAX_LIFT, lift));
  const was = volumeLift(v);
  const dz = target - was;
  if (Math.abs(dz) < EPS) return false;
  // What stands on it: blocks on its roof at its floor, and on theirs.
  const moved: Volume[] = [v];
  for (let i = 0; i < moved.length; i++) {
    const under = moved[i]!;
    for (const o of b.volumes) {
      if (o.open || moved.includes(o) || o.base !== volumeTop(under)) continue;
      if (Math.abs(volumeLift(o) - volumeLift(under)) > EPS || !planOverlap(o, under)) continue;
      moved.push(o);
    }
  }
  // The parts hung on the faces of the blocks that move, found before they do.
  const all = b.volumes.filter((o) => !o.open && isMass(o));
  const ids = new Set(moved.map((o) => o.id));
  const hung = (b.elements ?? []).filter((el) => ON_FACADE.has(el.kind) && ids.has(ownerOf(el, all)?.id ?? -1));
  for (const o of moved) {
    const next = volumeLift(o) + dz;
    if (Math.abs(next) < EPS) delete o.lift; else o.lift = next;
  }
  for (const el of hung) el.z += dz;
  return true;
}
