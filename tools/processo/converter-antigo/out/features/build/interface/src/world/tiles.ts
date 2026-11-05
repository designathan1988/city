// What the feature uses of src/world/tiles.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/tiles.ts:7-7 variable TILE
/** THE WORLD'S TILES: an unbounded grid of 1,024 m squares, the unit the world is generated, kept, loaded and drawn by (CLAUDE.md, "Streaming world") */

/** A tile's side. The tiles' edges lie on multiples of it, so a tile nests in a 2,048 m plate (`bounds.ts`) and holds 16 cells of 256 m. */
export const TILE = m(1_024);

// src/world/tiles.ts:10-10 variable TILE_REACH
/** Tiles each way from the origin a key can name (2^15): about 33,000 km, far past any map, so a key and an id stay exact doubles. */
const TILE_REACH = 1 << 15;

// src/world/tiles.ts:12-12 variable WORLD_REACH
/** How far from the origin a tile's key reaches, each way: the world's only edge (`bounds.ts` WORLD_HALF keeps a plate inside it). */
export const WORLD_REACH = TILE_REACH * TILE;

// src/world/tiles.ts:36-39 function tileKeyAt
/** The key of the tile a point lies in. */
export const tileKeyAt = (x: number, y: number): number =>;

// src/world/tiles.ts:48-48 function tileIdBase
/** The first id of a tile's range; the ids under `TILE_IDS` are a map's from before the tiles (kept as they were, never handed out again). */
export const tileIdBase = (key: number): number => …;

// src/world/tiles.ts:51-51 variable TILE_COUNTED
/** The ids a tile counts out one by one (its own records); the upper half of its range is its edge's nodes' (`edgeNodeId`). */
export const TILE_COUNTED = 1 << 19;

// src/world/tiles.ts:82-91 variable OWNER
export const OWNER = {
  /** A node: where it stands. */
  node: (n: { readonly x: number; readonly y: number }): number => tileKeyAt(n.x, n.y),
  /** A road: the tile of the middle of its ends (a border avenue on the edge: the tile whose low edge it is; a street crossing an edge: the side it mostly lies on); an end in another tile is kept with it as a borrowed node. */
  segment: (a: { readonly x: number; readonly y: number }, b: { readonly x: number; readonly y: number }): number => tileKeyAt((a.x + b.x) / 2, (a.y + b.y) / 2),
  /** A span: the tile of the middle of its poles, as a road's (its other pole, if across an edge, borrowed). */
  span: (a: { readonly x: number; readonly y: number }, b: { readonly x: number; readonly y: number }): number => tileKeyAt((a.x + b.x) / 2, (a.y + b.y) / 2),
  /** A building or a lot, a pole, a tree, a dab, an item: its anchor. */
  at: (p: { readonly x: number; readonly y: number }): number => tileKeyAt(p.x, p.y),
} as const;
