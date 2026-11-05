// What the feature uses of src/world/roads/profile.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/roads/profile.ts:11-21 type ProfileElement
/** THE ROAD PROFILE */
export type ProfileElement =
  | { readonly kind: 'footway'; readonly width: number; readonly material?: FootwayMaterial; readonly flush?: boolean }
  | { readonly kind: 'parking' | 'cycle'; readonly width: number }
  | {
    readonly kind: 'lane'; readonly width: number; readonly dir: 'forward' | 'backward';
    /** Buses only (`RoadSection.useForward`). */
    readonly use?: 'bus';
    /** The line on its right, to the next lane of its direction: solid (no lane change) instead of dashed. */
    readonly line?: 'solid';
  }
  | { readonly kind: 'median'; readonly width: number; readonly material?: MedianMaterial; readonly flush?: boolean };

// src/world/roads/profile.ts:23-31 interface RoadProfileSpec
export interface RoadProfileSpec {
  /** Left edge to right edge, looking from `a` to `b`. */
  readonly elements: readonly ProfileElement[];
  readonly speedKmh: number;
  readonly priority: number;
  readonly carriageway?: CarriagewayMaterial;
  /** The line between the two directions (V6); absent: the road's own. */
  readonly centreLine?: CentreLine;
}

// src/world/roads/profile.ts:177-182 function sameProfile
/** DO NOT compare profiles by `JSON.stringify` */
export function sameProfile(a: RoadProfileSpec, b: RoadProfileSpec): boolean;

// src/world/roads/profile.ts:260-263 function profileRoad
/** The road a profile builds, as every consumer reads it. */
export function profileRoad(profile: RoadProfileSpec, typeIndex: number): RoadType;

// src/world/roads/profile.ts:266-267 function profileWidth
/** Total width of a profile, footways included, world units. */
export const profileWidth = (profile: RoadProfileSpec): number => …;
