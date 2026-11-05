// What the feature uses of src/core/idSlots.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/idSlots.ts:3-3 variable EMPTY,FULL,GONE
/** DENSE SLOTS FOR SPARSE NUMERIC IDS, ON TYPED ARRAYS: an id's slot found by open addressing (linear probing over a power-of-two table, deleted keys tombstoned), its data kept in the caller's columns at that slot. No JS object or `Map` entry an id: ~20 bytes of array an id, outside the JS heap (report "Arquitetura para cidades grandes", "colunas binárias"). */

const EMPTY = 0, FULL = 1, GONE = 2;

// src/core/idSlots.ts:15-124 class IdSlots
export class IdSlots { … }

// src/core/idSlots.ts:16-16 field IdSlots.keys
private keys: Float64Array;

// src/core/idSlots.ts:17-17 field IdSlots.values
private values: Int32Array;

// src/core/idSlots.ts:18-18 field IdSlots.states
private states: Uint8Array;

// src/core/idSlots.ts:20-20 field IdSlots.live
private live = 0;

// src/core/idSlots.ts:26-26 field IdSlots.onGrow
/** Told the slots now needed (`capacity`) whenever it grows: the caller grows its columns to it. */
  onGrow: ((capacity: number) => void) | null = null;

// src/core/idSlots.ts:27-27 field IdSlots.capacityNow
private capacityNow = 0;

// src/core/idSlots.ts:48-55 method IdSlots.get
/** The slot of an id; -1 when it has none. */
  get(id: number): number;

// src/core/idSlots.ts:58-72 method IdSlots.take
/** The slot of an id, handed one when it had none. */
  take(id: number): number;

// src/core/idSlots.ts:75-88 method IdSlots.delete
/** The id's slot handed back (its columns are the caller's to clear); false when it had none. */
  delete(id: number): boolean;

// src/core/idSlots.ts:97-103 method IdSlots.clear
/** Every id let go; the columns' capacity kept. */
  clear(): void;

// src/core/idSlots.ts:127-132 function grown
/** A column grown to `capacity`, its values kept. */
export function grown<T extends Float64Array | Float32Array | Int32Array | Uint8Array>(column: T, capacity: number): T;
