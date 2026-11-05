// What the feature uses of src/editor/smallCityMap.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/smallCityMap.ts:18-18 variable GROWTH_SEED
/** The stream the city's buildings are grown from. */
const GROWTH_SEED = 0x5eed;

// src/editor/smallCityMap.ts:97-113 function growLots
/** Every open lot of the city grown, on the flat ground of a new map and the paving its streets lay on it; `grew`: told after each building. */
function growLots(doc: RoadDoc, net: Network, grew: (() => void) | null): void;
