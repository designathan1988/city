// The code of src/world/heading.ts that is the feature's (the rest of the file is shared).

// src/world/heading.ts:8-13 function chordHeading: the feature's own
export function chordHeading(behind: Vec2, ahead: Vec2, fallback: Vec2): Vec2 {
  const dx = ahead.x - behind.x;
  const dy = ahead.y - behind.y;
  const d = Math.hypot(dx, dy);
  return d < 1e-6 ? fallback : { x: dx / d, y: dy / d };
}
