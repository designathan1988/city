// What the feature uses of src/world/bounds.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/bounds.ts:9-9 variable PLATE_SIZE
/** THE WORLD'S PLATES: an unbounded grid of 2,048 m squares of ground, plate (pi, pj) centred on (pi, pj) x PLATE_SIZE (CLAUDE.md, "Streaming world") */

/** A plate's side: it holds 2 x 2 tiles of 1,024 m (`tiles.ts`), their edges on its centre lines and its own. */
export const PLATE_SIZE = m(2_048);
