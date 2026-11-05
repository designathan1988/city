// What the feature uses of src/world/elements.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/elements.ts:3-3 variable PROP_KINDS
/** ELEMENTS: the small things a scene is dressed with, laid by a brush */

const PROP_KINDS = ['stones', 'pebbles', 'gravel', 'leaves', 'grass', 'tallGrass', 'scrub', 'fern', 'clover', 'flowers', 'mushrooms'] as const;

// src/world/elements.ts:4-4 variable EFFECT_KINDS
export const EFFECT_KINDS = ['smoke', 'fire', 'steam', 'dust', 'soot', 'sparks', 'spray'] as const;

// src/world/elements.ts:5-5 variable ELEMENT_KINDS
const ELEMENT_KINDS = [...PROP_KINDS, ...EFFECT_KINDS] as const;

// src/world/elements.ts:6-6 type ElementKind
export type ElementKind = (typeof ELEMENT_KINDS)[number];

// src/world/elements.ts:7-7 type EffectKind
export type EffectKind = (typeof EFFECT_KINDS)[number];

// src/world/elements.ts:12-23 interface ElementItem
export interface ElementItem {
  readonly kind: ElementKind;
  readonly x: number;
  readonly y: number;
  /** Across, world units (a prop's size; an effect's spread). */
  readonly size: number;
  readonly yaw: number;
  /** 0..1: which model variant, its tint. */
  readonly seed: number;
  /** Effects only: 0..1, how much it gives off. */
  readonly intensity?: number;
}
