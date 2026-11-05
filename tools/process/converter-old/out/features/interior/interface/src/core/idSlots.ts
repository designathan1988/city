// What the feature uses of src/core/idSlots.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/idSlots.ts:15-124 class IdSlots
export class IdSlots { … }

// src/core/idSlots.ts:38-40 getter IdSlots.size
/** How many ids hold a slot. */
  get size(): number;

// src/core/idSlots.ts:48-55 method IdSlots.get
/** The slot of an id; -1 when it has none. */
  get(id: number): number;

// src/core/idSlots.ts:58-72 method IdSlots.take
/** The slot of an id, handed one when it had none. */
  take(id: number): number;

// src/core/idSlots.ts:75-88 method IdSlots.delete
/** The id's slot handed back (its columns are the caller's to clear); false when it had none. */
  delete(id: number): boolean;

// src/core/idSlots.ts:91-94 method IdSlots.forEach
/** Every id with its slot. */
  forEach(visit: (id: number, slot: number) => void): void;

// src/core/idSlots.ts:97-103 method IdSlots.clear
/** Every id let go; the columns' capacity kept. */
  clear(): void;
