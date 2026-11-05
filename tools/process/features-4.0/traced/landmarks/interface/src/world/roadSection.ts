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

// src/world/roadSection.ts:52-52 type CentreLine
export type CentreLine = 'dashed' | 'solid' | 'double';

// src/world/roadSection.ts:56-56 type LaneUse
/** Who may drive a lane (`RoadSection.useForward`). */
export type LaneUse = 'all' | 'bus';

// src/world/roadSection.ts:59-59 type LaneLine
/** The line between two lanes of one direction (`RoadSection.linesForward`). */
export type LaneLine = 'dashed' | 'solid';
