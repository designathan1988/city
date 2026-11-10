// What the feature uses of src/world/lotStore.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/lotStore.ts:96-98 method LotStore.orderOf
/** A lot's place in the order (`ORDER` above): a lot put last gets a number greater than any before. */
  orderOf(id: number): number | undefined;

// src/world/lotStore.ts:117-119 getter LotStore.keys
/** The land already cut into lots, in the order it was cut. */
  get keys(): readonly string[];

// src/world/lotStore.ts:270-279 method LotStore.load
/** The lots and keys of a map read (`RoadDoc.fromJSON`): the whole store, entries without ids. */
  load(lots: readonly Lot[], keys: readonly string[]): void;

// src/world/lotStore.ts:409-411 method LotStore.toJSON
/** Every lot, its corners copied (the file, `RoadDoc.serialized`). */
  toJSON(): Lot[];
