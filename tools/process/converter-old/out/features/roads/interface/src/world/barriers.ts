// What the feature uses of src/world/barriers.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/barriers.ts:2-2 type BarrierKind
/** Walls, fences and hedges drawn along a path, as roads are */
export type BarrierKind = 'fence' | 'wall' | 'hedge' | 'guardrail' | 'railing';

// src/world/barriers.ts:7-7 variable KERB_BARRIERS
/** Kinds that stand at the KERB, on the footway's road edge, rather than along its back: a road guardrail */
export const KERB_BARRIERS: ReadonlySet<BarrierKind> = new Set<BarrierKind>(['guardrail', 'railing']);
