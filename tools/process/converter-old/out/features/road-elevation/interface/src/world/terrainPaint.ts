// What the feature uses of src/world/terrainPaint.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/terrainPaint.ts:14-14 variable GEOLOGY_KINDS
/** The rocks the land can be made of, in the terrain shader's layer order. */
const GEOLOGY_KINDS = ['granite', 'sandstone', 'basalt'] as const;

// src/world/terrainPaint.ts:15-15 type GeologyKind
export type GeologyKind = (typeof GEOLOGY_KINDS)[number];
