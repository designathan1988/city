// What the feature uses of src/render/terrainGrid.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/terrainGrid.ts:6-6 variable TERRAIN_SIZE
/** THE TERRAIN PLATE'S GRID: its numbers alone, for the workers (`buildings/buildingMesh.ts`) that must not load the page's terrain (`terrain.ts`) and its pools */

/** Side of one terrain plate, in world units: the map is laid in square plates */
export const TERRAIN_SIZE = PLATE_SIZE;

// src/render/terrainGrid.ts:8-8 variable TERRAIN_SEGMENTS
/** Cells per side: 320 cells of 6.4 m (`world/bounds.ts` PLATE_SIZE), fine enough for a brush. */
export const TERRAIN_SEGMENTS = 320;

// src/render/terrainGrid.ts:10-10 variable TERRAIN_CELL
/** World size of one terrain quad. */
export const TERRAIN_CELL = TERRAIN_SIZE / TERRAIN_SEGMENTS;
