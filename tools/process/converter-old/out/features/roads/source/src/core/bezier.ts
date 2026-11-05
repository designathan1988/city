// The code of src/core/bezier.ts that is the feature's (the rest of the file is shared).

// src/core/bezier.ts:21-29 function shapeFromControl: the feature's own
export function shapeFromControl(a: Vec2, b: Vec2, c: Vec2): CurveShape {
  const chord = sub(b, a);
  const L = len(chord);
  if (L < EPS) return { t: 0.5, h: 0 };
  const u = scale(chord, 1 / L);
  const n = perp(u);
  const w = sub(c, a);
  return { t: (w.x * u.x + w.y * u.y) / L, h: w.x * n.x + w.y * n.y };
}

// src/core/bezier.ts:70-80 function splitQuad: the feature's own
export function splitQuad(
  a: Vec2,
  c: Vec2,
  b: Vec2,
  t: number,
): { left: [Vec2, Vec2, Vec2]; right: [Vec2, Vec2, Vec2] } {
  const ac = { x: a.x + (c.x - a.x) * t, y: a.y + (c.y - a.y) * t };
  const cb = { x: c.x + (b.x - c.x) * t, y: c.y + (b.y - c.y) * t };
  const mid = { x: ac.x + (cb.x - ac.x) * t, y: ac.y + (cb.y - ac.y) * t };
  return { left: [a, ac, mid], right: [mid, cb, b] };
}
