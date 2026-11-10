// What the feature uses of src/world/roads/streetNames.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/roads/streetNames.ts:31-31 variable NAME_REACH
/** How far from a street its name plate may stand and still name it. */
const NAME_REACH = m(30);

// src/world/roads/streetNames.ts:44-54 function platesNear
/** The street name plates that may name a road of `chain` (those within reach of one of its ribbons), in id order. */
function platesNear(net: Network, chain: readonly SegmentId[]): LandscapeItem[];

// src/world/roads/streetNames.ts:57-66 function streetNameOf
/** The name of the street a segment runs in, or null: the first plate (by id) that names one of its roads. */
export function streetNameOf(net: Network, segment: SegmentId): string | null;
