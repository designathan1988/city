// What the feature uses of src/core/plain.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/plain.ts:26-49 function sameText
/** Whether two plain values write the same text (`JSON.stringify`), keys in the same order, without writing it; stricter only for numbers JSON writes as null. */
export function sameText(a: unknown, b: unknown): boolean;
