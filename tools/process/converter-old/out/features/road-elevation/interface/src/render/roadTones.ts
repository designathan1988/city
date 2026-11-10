// What the feature uses of src/render/roadTones.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/roadTones.ts:6-6 variable ASPHALT_REFERENCE
/** THE STREET SURFACES' TONES over their baked textures, one source for the near roads (`roadSurfaces.ts`, `roadSurfaceReads.ts`) and the far ones (`farRoadTiles.ts`) */

/** The colour the asphalt texture is authored against (sRGB): a class's colour (`RoadType.color`) over it is the tint its carriageway is drawn with. */
export const ASPHALT_REFERENCE = 0x3a3d3f;

// src/render/roadTones.ts:15-19 variable FOOTWAY_TINT
/** A footway's tone over the baked pavers, by material, linear multipliers. */
export const FOOTWAY_TINT: Readonly<Record<FootwayMaterial, readonly [number, number, number]>> = {
  pavers: [1, 1, 1],
  concrete: [1.08, 1.08, 1.1],
  stone: [1, 0.84, 0.66],
};
