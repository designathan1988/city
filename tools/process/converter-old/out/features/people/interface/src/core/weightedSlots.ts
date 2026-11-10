// What the feature uses of src/core/weightedSlots.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/weightedSlots.ts:4-118 class WeightedSlots
/** SLOTS IN ORDER, EACH EMPTY OR OF ONE OF A FEW CLASSES, AND THE DRAWS OF A WEIGHTED LIST OVER THEM WITHOUT WALKING IT */

/** DO NOT walk a list of thousands to draw one by weight: a Fenwick tree of counts per class, its prefix of weights read in log steps */
export class WeightedSlots { … }

// src/core/weightedSlots.ts:87-92 method WeightedSlots.clear
/** Every slot taken back: the next `push` is slot 0 (the room is kept). */
  clear(): void;
