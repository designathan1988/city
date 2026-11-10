// What the feature uses of src/world/cityGen/mapSize.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/cityGen/mapSize.ts:5-5 variable __MAP_TILES__
/** THE MAP'S SIZE, apart from the whole map's rules (`wholeMap.ts`): what the plan reads of it (`plan.ts`) without importing the rules that read
 *  the plan's own types back - DO NOT define them in `wholeMap.ts` again: `plan.ts` and `wholeMap.ts` imported each other. */
declare const __MAP_TILES__: number | undefined;

// src/world/cityGen/mapSize.ts:7-7 variable MAP_TILES
/** Set at build time (`vite.config.ts`, ROADCRAFT_MAP_TILES: the page and every worker read the same); 18 otherwise (~18.4 km a side; the edge on the 2 km plates' edges, so 10, 14, 18... tiles). */
export const MAP_TILES = typeof __MAP_TILES__ === 'number' ? __MAP_TILES__ : 18;

// src/world/cityGen/mapSize.ts:9-9 variable MAP_HALF
/** The map's half side, world units. */
export const MAP_HALF = (MAP_TILES / 2) * TILE;
