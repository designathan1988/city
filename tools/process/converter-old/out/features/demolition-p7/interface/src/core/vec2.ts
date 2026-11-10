// What the feature uses of src/core/vec2.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/vec2.ts:4-7 interface Vec2
/** 2D vector helpers. */
export interface Vec2 {
  readonly x: number;
  readonly y: number;
}

// src/core/vec2.ts:9-9 function add
export const add = (a: Vec2, b: Vec2): Vec2 => …;

// src/core/vec2.ts:10-10 function sub
export const sub = (a: Vec2, b: Vec2): Vec2 => …;

// src/core/vec2.ts:11-11 function scale
export const scale = (a: Vec2, k: number): Vec2 => …;

// src/core/vec2.ts:14-14 function dot
export const dot = (a: Vec2, b: Vec2): number => …;

// src/core/vec2.ts:20-20 function dist
export const dist = (a: Vec2, b: Vec2): number => …;
