// What the feature uses of src/world/zones.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/zones.ts:2-2 variable ZONE_USES
/** Authored land-use area. Buildings generated from it remain ordinary editable buildings. */
const ZONE_USES = ['residential', 'commercial', 'industrial'] as const;

// src/world/zones.ts:3-3 variable ZONE_DENSITIES
const ZONE_DENSITIES = ['low', 'medium', 'high'] as const;

// src/world/zones.ts:4-4 type ZoneUse
export type ZoneUse = (typeof ZONE_USES)[number];

// src/world/zones.ts:5-5 type ZoneDensity
export type ZoneDensity = (typeof ZONE_DENSITIES)[number];

// src/world/zones.ts:7-17 interface Zone
export interface Zone {
  readonly id: number;
  readonly use: ZoneUse;
  readonly density: ZoneDensity;
  readonly x0: number;
  readonly y0: number;
  readonly x1: number;
  readonly y1: number;
  readonly seed: number;
  readonly buildingIds: readonly number[];
}

// src/world/zones.ts:26-33 interface ZoneMark
/** One zoned cell of the street grid */
export interface ZoneMark {
  readonly x: number;
  readonly y: number;
  readonly use: ZoneUse;
  readonly density: ZoneDensity;
  /** The building that grew on it, while that building stands. */
  readonly building?: number;
}
