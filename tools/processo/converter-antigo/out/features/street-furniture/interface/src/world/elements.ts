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

// src/world/elements.ts:29-42 interface ElementBrush
/** The brush's settings, in the player's units. */
export interface ElementBrush {
  /** Instances per 100 m² at full strength. */
  readonly density: number;
  /** An instance's size, metres. */
  readonly size: number;
  /** 0..100: how far sizes vary about it. */
  readonly variation: number;
  /** Least distance between two instances of the same kind, metres. */
  readonly spacing: number;
  /** 0..100: how much of the density one dab lays. */
  readonly strength: number;
  /** 0..100: effects only, how much each gives off. */
  readonly intensity: number;
}
