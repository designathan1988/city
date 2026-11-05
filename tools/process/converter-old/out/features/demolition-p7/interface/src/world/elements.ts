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

// src/world/elements.ts:45-64 variable DEFAULT_BRUSH
/** Each kind's own starting brush: a stone is not a blade of grass. */
export const DEFAULT_BRUSH: Readonly<Record<ElementKind, ElementBrush>> = {
  stones: { density: 2, size: 1.2, variation: 60, spacing: 2, strength: 60, intensity: 50 },
  pebbles: { density: 30, size: 0.3, variation: 50, spacing: 0.4, strength: 60, intensity: 50 },
  gravel: { density: 120, size: 0.12, variation: 40, spacing: 0.15, strength: 60, intensity: 50 },
  leaves: { density: 6, size: 1.6, variation: 40, spacing: 0.8, strength: 60, intensity: 50 },
  grass: { density: 40, size: 0.5, variation: 40, spacing: 0.3, strength: 60, intensity: 50 },
  tallGrass: { density: 20, size: 0.9, variation: 40, spacing: 0.5, strength: 60, intensity: 50 },
  scrub: { density: 6, size: 1.2, variation: 50, spacing: 1, strength: 60, intensity: 50 },
  fern: { density: 4, size: 1.1, variation: 40, spacing: 1, strength: 60, intensity: 50 },
  clover: { density: 30, size: 0.25, variation: 40, spacing: 0.2, strength: 60, intensity: 50 },
  flowers: { density: 12, size: 0.5, variation: 40, spacing: 0.4, strength: 60, intensity: 50 },
  mushrooms: { density: 6, size: 0.25, variation: 50, spacing: 0.3, strength: 60, intensity: 50 },
  smoke: { density: 0.3, size: 3, variation: 30, spacing: 8, strength: 100, intensity: 60 },
  fire: { density: 0.3, size: 2, variation: 30, spacing: 4, strength: 100, intensity: 60 },
  steam: { density: 0.3, size: 2, variation: 30, spacing: 6, strength: 100, intensity: 50 },
  dust: { density: 0.3, size: 4, variation: 30, spacing: 8, strength: 100, intensity: 40 },
  soot: { density: 0.3, size: 3, variation: 30, spacing: 8, strength: 100, intensity: 50 },
  sparks: { density: 0.3, size: 1, variation: 30, spacing: 4, strength: 100, intensity: 50 },
  spray: { density: 0.5, size: 2, variation: 30, spacing: 4, strength: 100, intensity: 50 },
};
