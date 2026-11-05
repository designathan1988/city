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

// src/world/roads/profile.ts:43-92 function profileOf
/** The profile of a segment as it stands (its class, lanes, direction, section, parking). */
export function profileOf(source: ProfileSource): RoadProfileSpec;

// src/world/roads/profile.ts:95-96 type ProfileProblem
/** Why a profile cannot be built as it stands; a translation key under `profile.problem.`. */
export type ProfileProblem =
  | 'footways' | 'noLanes' | 'laneWidths' | 'laneBalance' | 'order' | 'median' | 'parkingSide' | 'width' | 'class' | 'busOnly';

// src/world/roads/profile.ts:99-101 function profileProblems
/** What stops a profile from being built, empty when nothing does. */
export function profileProblems(profile: RoadProfileSpec, typeIndex: number): ProfileProblem[];

// src/world/roads/profile.ts:104-107 interface ProfileIssue
/** A problem and the elements it is about (by index; empty: the road as a whole). */
export interface ProfileIssue {
  readonly problem: ProfileProblem;
  readonly elements: readonly number[];
}

// src/world/roads/profile.ts:110-165 function profileIssues
/** `profileProblems` with the elements each problem is about, so an editor can name the problem on the element that has it */
export function profileIssues(profile: RoadProfileSpec, typeIndex: number): ProfileIssue[];

// src/world/roads/profile.ts:177-182 function sameProfile
/** DO NOT compare profiles by `JSON.stringify` */
export function sameProfile(a: RoadProfileSpec, b: RoadProfileSpec): boolean;

// src/world/roads/profile.ts:260-263 function profileRoad
/** The road a profile builds, as every consumer reads it. */
export function profileRoad(profile: RoadProfileSpec, typeIndex: number): RoadType;
