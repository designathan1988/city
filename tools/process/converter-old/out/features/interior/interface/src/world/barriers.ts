// What the feature uses of src/world/barriers.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/barriers.ts:2-2 type BarrierKind
/** Walls, fences and hedges drawn along a path, as roads are */
export type BarrierKind = 'fence' | 'wall' | 'hedge' | 'guardrail' | 'railing';

// src/world/barriers.ts:4-4 variable BARRIER_KINDS
export const BARRIER_KINDS: readonly BarrierKind[] = ['fence', 'wall', 'hedge', 'guardrail', 'railing'];
