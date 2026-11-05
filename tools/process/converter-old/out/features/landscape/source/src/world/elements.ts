// The code of src/world/elements.ts that is the feature's (the rest of the file is shared).

// src/world/elements.ts:7-7 type EffectKind: the feature's own
export type EffectKind = (typeof EFFECT_KINDS)[number];

// src/world/elements.ts:10-10 function isEffectKind: the feature's own
export const isEffectKind = (k: ElementKind): k is EffectKind => (EFFECT_KINDS as readonly string[]).includes(k);

// src/world/elements.ts:78-101 function scatter: the feature's own
export function scatter(kind: ElementKind, brush: ElementBrush, x: number, y: number, radius: number, unitsPerMetre: number,
  random: () => number, nearby: readonly ElementItem[]): ElementItem[] {
  const area = Math.PI * (radius / unitsPerMetre) ** 2;
  const wanted = (brush.density / 100) * area * (brush.strength / 100);
  // A fraction left over is laid by chance, so a light brush still lays some.
  let count = Math.floor(wanted) + (random() < wanted % 1 ? 1 : 0);
  count = Math.min(count, 400);
  const spacing = brush.spacing * unitsPerMetre;
  const out: ElementItem[] = [];
  const same = nearby.filter((e) => e.kind === kind);
  for (let i = 0; i < count * 3 && out.length < count; i++) {
    const r = radius * Math.sqrt(random());
    const a = random() * Math.PI * 2;
    const px = x + Math.cos(a) * r, py = y + Math.sin(a) * r;
    const free = (e: { x: number; y: number }): boolean => Math.hypot(e.x - px, e.y - py) >= spacing;
    if (!same.every(free) || !out.every(free)) continue;
    const vary = (brush.variation / 100) * (random() * 2 - 1);
    out.push({
      kind, x: px, y: py, size: brush.size * unitsPerMetre * Math.max(0.15, 1 + vary), yaw: random() * Math.PI * 2, seed: random(),
      ...(isEffectKind(kind) ? { intensity: brush.intensity / 100 } : {}),
    });
  }
  return out;
}
