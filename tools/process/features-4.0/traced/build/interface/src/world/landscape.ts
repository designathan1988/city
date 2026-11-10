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
