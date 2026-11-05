// What the feature uses of src/editor/bulldozer.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/bulldozer.ts:28-36 function removeRoads
/** Roads taken away as the player demolishes them, by any tool: their power line and furniture with them, a share of their price back (`editor/roads/economy.ts`). */
export function removeRoads(doc: RoadDoc, net: Network, ids: readonly SegmentId[]): boolean;

// src/editor/bulldozer.ts:42-150 class Bulldozer
/** THE BULLDOZER: a click removes the one thing under the pointer */
export class Bulldozer { … }
