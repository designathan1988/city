// What the feature uses of src/world/roadSection.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/roadSection.ts:3-3 variable LANE_TURN_RULES
export const LANE_TURN_RULES = ['all', 'left', 'through', 'right', 'leftThrough', 'throughRight'] as const;

// src/world/roadSection.ts:4-4 type LaneTurnRule
export type LaneTurnRule = (typeof LANE_TURN_RULES)[number];

// src/world/roadSection.ts:7-7 variable CARRIAGEWAY_MATERIALS
/** What an element of the cross-section is paved with. */
export const CARRIAGEWAY_MATERIALS = ['asphalt', 'concrete', 'cobble'] as const;

// src/world/roadSection.ts:8-8 variable FOOTWAY_MATERIALS
export const FOOTWAY_MATERIALS = ['pavers', 'concrete', 'stone'] as const;

// src/world/roadSection.ts:9-9 variable MEDIAN_MATERIALS
export const MEDIAN_MATERIALS = ['grass', 'concrete', 'pavers'] as const;

// src/world/roadSection.ts:10-10 type CarriagewayMaterial
export type CarriagewayMaterial = (typeof CARRIAGEWAY_MATERIALS)[number];

// src/world/roadSection.ts:11-11 type FootwayMaterial
export type FootwayMaterial = (typeof FOOTWAY_MATERIALS)[number];

// src/world/roadSection.ts:12-12 type MedianMaterial
export type MedianMaterial = (typeof MEDIAN_MATERIALS)[number];

// src/world/roadSection.ts:14-20 interface SectionMaterials
export interface SectionMaterials {
  readonly carriageway?: CarriagewayMaterial;
  /** Left and right of a -> b. */
  readonly footwayLeft?: FootwayMaterial;
  readonly footwayRight?: FootwayMaterial;
  readonly median?: MedianMaterial;
}

// src/world/roadSection.ts:23-50 interface RoadSection
/** Authored road section. */
export interface RoadSection {
  readonly laneWidth: number;
  readonly sidewalk: number;
  /** The footway left of a -> b, when it differs from the right one. */
  readonly sidewalkLeft?: number;
  readonly sidewalkRight?: number;
  /** A footway laid level with the carriageway (a shared surface): no kerb stone, no rise. */
  readonly flushLeft?: boolean;
  readonly flushRight?: boolean;
  /** A central reservation painted on the carriageway instead of kerbed and raised. */
  readonly medianFlush?: boolean;
  readonly materials?: SectionMaterials;
  readonly median: number;
  readonly speedKmh: number;
  readonly priority: number;
  /** Lane order matches laneIndex, inner to outer, in the stored a -> b direction. */
  readonly turnsForward?: readonly LaneTurnRule[];
  /** Lane order in the stored b -> a direction. Missing indexes retain default movements. */
  readonly turnsBackward?: readonly LaneTurnRule[];
  /** Who may drive each lane, by laneIndex (inner to outer) in each direction */
  readonly useForward?: readonly LaneUse[];
  readonly useBackward?: readonly LaneUse[];
  /** The line between lane k and lane k + 1 of each direction: dashed (a lane change allowed, LMS-2) or solid */
  readonly linesForward?: readonly LaneLine[];
  readonly linesBackward?: readonly LaneLine[];
  /** The line between the two directions (MBST vol. IV): dashed (LFO-2, overtaking allowed), solid */
  readonly centreLine?: CentreLine;
}

// src/world/roadSection.ts:52-52 type CentreLine
export type CentreLine = 'dashed' | 'solid' | 'double';

// src/world/roadSection.ts:56-56 type LaneUse
/** Who may drive a lane (`RoadSection.useForward`). */
export type LaneUse = 'all' | 'bus';

// src/world/roadSection.ts:59-59 type LaneLine
/** The line between two lanes of one direction (`RoadSection.linesForward`). */
export type LaneLine = 'dashed' | 'solid';

// src/world/roadSection.ts:64-70 variable ROAD_SECTION_LIMITS
/** Physical edit bounds, shared by persistence and the section editor. */
export const ROAD_SECTION_LIMITS = {
  laneWidth: [m(2), m(6)],
  sidewalk: [m(1), m(12)],
  median: [0, m(8)],
  speedKmh: [10, 130],
  priority: [0, 5],
} as const;

// src/world/roadSection.ts:262-264 function laneUse
/** Who may drive lane `index` of one direction of a section. */
export function laneUse(section: RoadSection | undefined, forward: boolean, index: number): LaneUse;
