// What the feature uses of src/world/buildings/blocks.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildings/blocks.ts:11-11 variable resolveBlocks
/** A building is blocks put together like bricks, and the blocks are never changed by being put together. */
/** Each record resolved, once (a stored record is never changed in place, `BuildingStore` OWNERSHIP) */
export const resolveBlocks: (b: Building) => Building = identityMemo(resolve);

// src/world/buildings/blocks.ts:13-80 function resolve
function resolve(b: Building): Building;
