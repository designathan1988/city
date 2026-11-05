// What the feature uses of src/core/plain.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/plain.ts:2-23 function samePlain
/** Whether two plain values (numbers, strings, booleans, null, arrays, objects) are the same; a key holding `undefined` is no key. */
export function samePlain(a: unknown, b: unknown): boolean;

// src/core/plain.ts:26-49 function sameText
/** Whether two plain values write the same text (`JSON.stringify`), keys in the same order, without writing it; stricter only for numbers JSON writes as null. */
export function sameText(a: unknown, b: unknown): boolean;
