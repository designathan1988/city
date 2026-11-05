// What the feature uses of src/core/heap.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/heap.ts:3-64 class Heap
/** DO NOT write another heap beside it: the copies drifted */
/** Its items, priorities and arrival order in typed columns (outside the JS heap, no array element boxed): a heap may hold one entry a building of the city (`decayWatch.ts`). */
export class Heap { … }

// src/core/heap.ts:10-10 getter Heap.size
get size(): number;

// src/core/heap.ts:12-12 method Heap.peekKey
/** The smallest priority waiting (Infinity when empty), without taking it. */
  peekKey(): number;

// src/core/heap.ts:14-23 method Heap.push
push(item: number, key: number): void;

// src/core/heap.ts:24-41 method Heap.pop
pop(): number;
