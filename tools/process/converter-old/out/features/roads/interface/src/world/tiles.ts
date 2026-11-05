// What the feature uses of src/world/tiles.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/tiles.ts:7-7 variable TILE
/** THE WORLD'S TILES: an unbounded grid of 1,024 m squares, the unit the world is generated, kept, loaded and drawn by (CLAUDE.md, "Streaming world") */

/** A tile's side. The tiles' edges lie on multiples of it, so a tile nests in a 2,048 m plate (`bounds.ts`) and holds 16 cells of 256 m. */
export const TILE = m(1_024);
