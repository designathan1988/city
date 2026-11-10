// What the feature uses of src/world/cityGen/wholeMap.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/cityGen/wholeMap.ts:34-34 type WholeMapOptions
/** The whole map's options: a generated city's (`CityOptions`) without a size; style 'mixed' picks a style per tile. */
export type WholeMapOptions = Omit<CityOptions, 'size' | 'radius'>;

// src/world/cityGen/wholeMap.ts:43-43 variable EDGE_INSET
/** How far in from the map's edge the avenues closing it run: half an avenue with its footways, so nothing of them stands past the ground, which ends at the edge (`flatAtlas.ts` slab). */
export const EDGE_INSET = halfWidth(roadType(Math.max(0, ROAD_TYPES.findIndex((t) => t.id === 'avenue'))), Level.Sidewalk);
