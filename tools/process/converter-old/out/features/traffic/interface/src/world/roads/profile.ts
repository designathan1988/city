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

// src/world/roads/profile.ts:34-40 interface ProfileSource
/** What a segment is made of, as the profile reads it. */
export interface ProfileSource {
  readonly type: number;
  readonly lanes: number | null;
  readonly direction: SegmentDirection;
  readonly section?: RoadSection;
  readonly parking?: SegmentParking;
}

// src/world/roads/profile.ts:95-96 type ProfileProblem
/** Why a profile cannot be built as it stands; a translation key under `profile.problem.`. */
export type ProfileProblem =
  | 'footways' | 'noLanes' | 'laneWidths' | 'laneBalance' | 'order' | 'median' | 'parkingSide' | 'width' | 'class' | 'busOnly';

// src/world/roads/profile.ts:185-192 interface AppliedProfile
/** The segment's own fields for a profile (`profileProblems` must be empty). */
export interface AppliedProfile {
  /** Null: the class's own lane count. */
  readonly lanes: number | null;
  readonly direction: SegmentDirection;
  /** Absent: the class's own section, exactly (its lanes need not be whole metres). */
  readonly section: RoadSection | undefined;
  readonly parking: SegmentParking | undefined;
}
