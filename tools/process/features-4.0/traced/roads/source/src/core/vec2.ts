// The code of src/core/vec2.ts that is the feature's (the rest of the file is shared).

// src/core/vec2.ts:12-12 function neg: the feature's own
export const neg = (a: Vec2): Vec2 => ({ x: -a.x, y: -a.y });

// src/core/vec2.ts:37-40 function fromAngle: the feature's own
export const fromAngle = (a: number, r = 1): Vec2 => ({
  x: Math.cos(a) * r,
  y: Math.sin(a) * r,
});
