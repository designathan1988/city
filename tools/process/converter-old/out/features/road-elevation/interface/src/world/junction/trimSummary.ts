// What the feature uses of src/world/junction/trimSummary.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/junction/trimSummary.ts:8-8 variable SUMMARY,SUMMARY_FLOOR
/** A summary's numbers a road (`trimSummary`): the road, its greatest trim at each level (NaN: no leg there; at the level's own index), the floor of its mouth (0: none). */
export const SUMMARY = 6, SUMMARY_FLOOR = 5;

// src/world/junction/trimSummary.ts:14-41 function trimSummary
/**
 * What `reconcileSegment` reads of a node's junction, by road: each level's greatest trim of the road's legs (at least 0), and the part
 * of its mouth no length budget may take (where two carriageways still overlap: the greatest reach along the road of a sharp corner's point).
 */
export function trimSummary(levels: JunctionLevels): number[];
