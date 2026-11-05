// What the feature uses of src/render/terrainShaping.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/terrainShaping.ts:7-7 type TerrainRegion
/** WHICH CORNERS OF A PLATE'S GROUND A SHAPING PASS ASKS */

/** A box of grid corners, inclusive: x0..x1 across, y0..y1 down. */
export type TerrainRegion = readonly [number, number, number, number];
