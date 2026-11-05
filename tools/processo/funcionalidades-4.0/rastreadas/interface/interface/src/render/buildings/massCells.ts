// What the feature uses of src/render/buildings/massCells.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/buildings/massCells.ts:99-99 type CellDrawn
/** What draws a cell of buildings now (`MassCells.census`): its detail, its masses, its tile's or plate's, the base's; 'empty' where none of its buildings stands from afar (lots of grass alone), null where nothing does (a hole). */
export type CellDrawn = 'detail' | 'mass' | 'tile' | 'plate' | 'base' | 'empty' | null;
