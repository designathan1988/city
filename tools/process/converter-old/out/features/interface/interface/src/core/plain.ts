// What the feature uses of src/core/plain.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/plain.ts:2-23 function samePlain
/** Whether two plain values (numbers, strings, booleans, null, arrays, objects) are the same; a key holding `undefined` is no key. */
export function samePlain(a: unknown, b: unknown): boolean;
