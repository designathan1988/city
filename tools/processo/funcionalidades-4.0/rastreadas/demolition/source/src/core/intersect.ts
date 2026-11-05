// The code of src/core/intersect.ts that is the feature's (the rest of the file is shared).

// src/core/intersect.ts:39-54 function closestOnSegment: the feature's own
export function closestOnSegment(
  p: Vec2,
  a: Vec2,
  b: Vec2,
): { point: Vec2; t: number; distSq: number } {
  const ab = sub(b, a);
  const l2 = lenSq(ab);
  if (l2 < EPS) {
    const ap0 = sub(p, a);
    return { point: a, t: 0, distSq: lenSq(ap0) };
  }
  let t = dot(sub(p, a), ab) / l2;
  t = t < 0 ? 0 : t > 1 ? 1 : t;
  const point = add(a, scale(ab, t));
  return { point, t, distSq: lenSq(sub(p, point)) };
}
