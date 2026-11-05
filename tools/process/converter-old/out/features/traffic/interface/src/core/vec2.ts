// What the feature uses of src/core/vec2.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/vec2.ts:4-7 interface Vec2
/** 2D vector helpers. */
export interface Vec2 {
  readonly x: number;
  readonly y: number;
}

// src/core/vec2.ts:10-10 function sub
export const sub = (a: Vec2, b: Vec2): Vec2 => …;

// src/core/vec2.ts:14-14 function dot
export const dot = (a: Vec2, b: Vec2): number => …;

// src/core/vec2.ts:19-19 function len
export const len = (a: Vec2): number => …;

// src/core/vec2.ts:23-23 function perp
/** The left-hand normal: a quarter turn counter-clockwise. */
export const perp = (a: Vec2): Vec2 => …;

// src/core/vec2.ts:25-28 function normalize
export function normalize(a: Vec2): Vec2;

// src/core/vec2.ts:35-35 function angleOf
export const angleOf = (a: Vec2): number => …;

// src/core/vec2.ts:43-46 function addScaled
/** `a + b * k` without an intermediate allocation at the call site. */
export const addScaled = (a: Vec2, b: Vec2, k: number): Vec2 => …;
