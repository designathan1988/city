// What the feature uses of src/world/terrainPaint.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/terrainPaint.ts:4-5 variable PAINT_KINDS
/** Ground the player paints over the terrain: sand, soil, meadow, snow, gravel, asphalt, concrete */

/** The paintable layers, in channel order: two RGBA textures, then grass, then the covers. */
export const PAINT_KINDS = ['sand', 'soil', 'meadow', 'snow', 'gravel', 'asphalt', 'concrete', 'grass', 'forest', 'scrub', 'flowers', 'rocks', 'granite', 'sandstone', 'basalt',
  'cerrado', 'atlantic', 'amazon', 'caatinga', 'pampa', 'pantanal'] as const;

// src/world/terrainPaint.ts:6-6 type PaintKind
export type PaintKind = (typeof PAINT_KINDS)[number];

// src/world/terrainPaint.ts:9-9 variable COVER_KINDS
/** The kinds painted as a density of things standing on the ground. */
export const COVER_KINDS = ['forest', 'scrub', 'flowers', 'rocks'] as const;

// src/world/terrainPaint.ts:10-10 type CoverKind
export type CoverKind = (typeof COVER_KINDS)[number];

// src/world/terrainPaint.ts:14-14 variable GEOLOGY_KINDS
/** The rocks the land can be made of, in the terrain shader's layer order. */
const GEOLOGY_KINDS = ['granite', 'sandstone', 'basalt'] as const;

// src/world/terrainPaint.ts:15-15 type GeologyKind
export type GeologyKind = (typeof GEOLOGY_KINDS)[number];

// src/world/terrainPaint.ts:19-19 variable BIOME_KINDS
/** The BIOMES (`world/ecology.ts`): painting one makes that ground the biome's own country */
export const BIOME_KINDS = ['cerrado', 'atlantic', 'amazon', 'caatinga', 'pampa', 'pantanal'] as const;

// src/world/terrainPaint.ts:20-20 type BiomeKind
export type BiomeKind = (typeof BIOME_KINDS)[number];

// src/world/terrainPaint.ts:23-30 interface PaintDab
export interface PaintDab {
  readonly kind: PaintKind;
  readonly x: number;
  readonly y: number;
  readonly radius: number;
  /** 0..1: how far one dab takes the ground to its layer at the centre. */
  readonly strength: number;
}
