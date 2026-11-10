// What the feature uses of src/world/terrainPaint.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/terrainPaint.ts:4-5 variable PAINT_KINDS
/** Ground the player paints over the terrain: sand, soil, meadow, snow, gravel, asphalt, concrete */

/** The paintable layers, in channel order: two RGBA textures, then grass, then the covers. */
export const PAINT_KINDS = ['sand', 'soil', 'meadow', 'snow', 'gravel', 'asphalt', 'concrete', 'grass', 'forest', 'scrub', 'flowers', 'rocks', 'granite', 'sandstone', 'basalt',
  'cerrado', 'atlantic', 'amazon', 'caatinga', 'pampa', 'pantanal'] as const;

// src/world/terrainPaint.ts:6-6 type PaintKind
export type PaintKind = (typeof PAINT_KINDS)[number];
