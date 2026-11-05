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
