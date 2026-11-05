// What the feature uses of src/world/nearestRibbon.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/nearestRibbon.ts:6-6 variable FIRST_REACH
/** The first box searched round a point, u; each search after it four times as wide. */
const FIRST_REACH = m(60);

// src/world/nearestRibbon.ts:8-8 variable LAST_REACH
/** Wider than any map: the last box holds every road. */
const LAST_REACH = m(200_000);

// src/world/nearestRibbon.ts:11-21 function nearestRibbon
/** DO NOT walk every road of the map for the street nearest a point: every lot drawn did, for each of its sides. */
export function nearestRibbon(net: Network, p: Vec2): { readonly ribbon: SegmentRibbon; readonly distance: number } | null;
