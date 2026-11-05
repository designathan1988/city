// What the feature uses of src/world/lotStore.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/lotStore.ts:33-451 class LotStore
/** DO NOT hand the lots out as an array the callers write into, nor search one by id or by place */
export class LotStore { … }

// src/world/lotStore.ts:82-84 method LotStore.get
get(id: number): Lot | undefined;

// src/world/lotStore.ts:91-93 method LotStore.all
/** Every lot, in the lots' order: what saves, copies or makes the whole map again reads; nothing that asks about a place. */
  all(): IterableIterator<Lot>;

// src/world/lotStore.ts:96-98 method LotStore.orderOf
/** A lot's place in the order (`ORDER` above): a lot put last gets a number greater than any before. */
  orderOf(id: number): number | undefined;

// src/world/lotStore.ts:117-119 getter LotStore.keys
/** The land already cut into lots, in the order it was cut. */
  get keys(): readonly string[];
