// What the feature uses of src/world/barriers.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/barriers.ts:2-2 type BarrierKind
/** Walls, fences and hedges drawn along a path, as roads are */
export type BarrierKind = 'fence' | 'wall' | 'hedge' | 'guardrail' | 'railing';

// src/world/barriers.ts:9-14 interface Barrier
export interface Barrier {
  readonly id: number;
  readonly kind: BarrierKind;
  /** The path, in world units on the ground; two points or more. */
  readonly points: readonly { readonly x: number; readonly y: number }[];
}
