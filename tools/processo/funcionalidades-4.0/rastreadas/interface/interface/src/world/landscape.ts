// What the feature uses of src/world/landscape.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/landscape.ts:13-13 variable LANDSCAPE_KINDS
/** Street landscaping the PLAYER places */

export const LANDSCAPE_KINDS = ['tree', 'shrub', 'bench', 'bin', 'lamp', 'hydrant', 'postbox', 'phone', 'drain', 'meadow', 'sign', 'streetname'] as const;

// src/world/landscape.ts:14-14 type LandscapeKind
export type LandscapeKind = (typeof LANDSCAPE_KINDS)[number];

// src/world/landscape.ts:16-27 interface LandscapeItem
export interface LandscapeItem {
  readonly id: number;
  readonly kind: LandscapeKind;
  readonly x: number;
  readonly y: number;
  /** A sign's type (`SIGN_TYPES`). */
  readonly signType?: SignType;
  /** What a sign says, or a street's name. */
  readonly text?: string;
  /** When a plant was planted, in city minutes (`City.minutes`, `sim/city/city.ts`); absent: fully grown. */
  readonly planted?: number;
}

// src/world/landscape.ts:42-42 variable SIGN_TYPES
/** The signs the one sign tool makes (the player's order of 2026-10-05) */
export const SIGN_TYPES = ['stop', 'yield', 'speed', 'noParking', 'noEntry', 'pedestrian', 'school', 'direction', 'street', 'info'] as const;

// src/world/landscape.ts:43-43 type SignType
export type SignType = (typeof SIGN_TYPES)[number];

// src/world/landscape.ts:48-48 variable SIGN_HAS_TEXT
/** Signs whose plate shows the player's text. */
export const SIGN_HAS_TEXT: ReadonlySet<SignType> = new Set<SignType>(['speed', 'direction', 'street', 'info']);

// src/world/landscape.ts:50-50 variable SIGN_TEXT_MAX
/** The longest text a sign or a street name keeps. */
export const SIGN_TEXT_MAX = 40;

// src/world/landscape.ts:57-70 variable LANDSCAPE_RADIUS
/** Plan radius of each kind, for spacing and for anyone walking round it. */
export const LANDSCAPE_RADIUS: Readonly<Record<LandscapeKind, number>> = {
  tree: TREE_PIT / 2,
  shrub: m(0.35),
  bench: Math.hypot(m(0.9), m(0.26)),
  bin: m(0.33),
  lamp: m(0.13),
  hydrant: m(0.16),
  postbox: m(0.33),
  phone: m(0.45),
  drain: m(0.5),
  meadow: m(2),
  sign: m(0.15),
  streetname: m(1),
};
