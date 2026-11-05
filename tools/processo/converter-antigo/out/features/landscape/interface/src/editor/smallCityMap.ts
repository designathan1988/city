// What the feature uses of src/editor/smallCityMap.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/smallCityMap.ts:18-18 variable GROWTH_SEED
/** The stream the city's buildings are grown from. */
const GROWTH_SEED = 0x5eed;

// src/editor/smallCityMap.ts:21-23 function buildSmallCityMap
/** THE SMALL CITY OF ROADCRAFT 2.0, WHOLE, AS ONE MAP (the player, 2026-10-18: no city by tiles). Planned as 2.0 planned it (`smallCity.ts`: straight blocks, medium size), then laid out on one document: its footways, lots, green and zones, and every lot grown, on the flat ground of a new map. */
export function buildSmallCityMap(seed: number): SerializedDoc;

// src/editor/smallCityMap.ts:97-113 function growLots
/** Every open lot of the city grown, on the flat ground of a new map and the paving its streets lay on it; `grew`: told after each building. */
function growLots(doc: RoadDoc, net: Network, grew: (() => void) | null): void;
