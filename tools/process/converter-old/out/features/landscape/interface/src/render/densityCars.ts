// What the feature uses of src/render/densityCars.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/densityCars.ts:50-50 variable TILE_START
/** A tile's mesh starts with room for this many cars; it doubles with them. */
const TILE_START = 256;

// src/render/densityCars.ts:52-52 variable TILE_RADIUS
/** A tile's sphere: its half diagonal and the land's heights, with a car's room over the highest deck. */
const TILE_RADIUS = Math.hypot(TILE / 2, TILE / 2, (TERRAIN_MAX_HEIGHT - TERRAIN_MIN_HEIGHT) / 2 + m(80));
