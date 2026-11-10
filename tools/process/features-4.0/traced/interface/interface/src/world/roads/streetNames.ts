// What the feature uses of src/world/roads/streetNames.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/roads/streetNames.ts:34-41 function nameItemSegment
/** DO NOT walk every ribbon for each plate: the names of a city were plates × roads. */
function nameItemSegment(net: Network, item: Vec2): SegmentId | null;

// src/world/roads/streetNames.ts:44-54 function platesNear
/** The street name plates that may name a road of `chain` (those within reach of one of its ribbons), in id order. */
function platesNear(net: Network, chain: readonly SegmentId[]): LandscapeItem[];
