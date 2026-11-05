// What the feature uses of src/world/bounds.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/bounds.ts:9-9 variable PLATE_SIZE
/** THE WORLD'S PLATES: an unbounded grid of 2,048 m squares of ground, plate (pi, pj) centred on (pi, pj) x PLATE_SIZE (CLAUDE.md, "Streaming world") */

/** A plate's side: it holds 2 x 2 tiles of 1,024 m (`tiles.ts`), their edges on its centre lines and its own. */
export const PLATE_SIZE = m(2_048);

// src/world/bounds.ts:23-28 interface PlateRegion
/** One square plate of ground: its key, its centre and half its side. */
export interface PlateRegion {
  readonly key: number;
  readonly cx: number;
  readonly cy: number;
  readonly half: number;
}

// src/world/bounds.ts:51-51 function plateOf
/** The key of the plate a point lies in, anywhere (no map to hold it to: the world's reach alone). */
export const plateOf = (x: number, y: number): number => …;

// src/world/bounds.ts:54-57 function plateRegion
/** A plate's square by its key. */
export function plateRegion(key: number): PlateRegion;

// src/world/bounds.ts:81-81 variable WORLD_HALF
/** How far from the origin anything stands, each way: a plate inside the tiles' reach, so every point's tile and plate have a key. */
export const WORLD_HALF = WORLD_REACH - PLATE_SIZE;

// src/world/bounds.ts:88-91 function clampToWorld
/** The nearest point to `p` within the world's reach: only a file's broken number lies past it. */
export const clampToWorld = (p: Vec2, margin = 0): Vec2 => …;
