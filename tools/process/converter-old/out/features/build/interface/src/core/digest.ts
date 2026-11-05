// What the feature uses of src/core/digest.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/digest.ts:6-78 class Digest
export class Digest { … }

// src/core/digest.ts:11-16 method Digest.add
/** Adds one number. */
  add(value: number): this;

// src/core/digest.ts:25-28 method Digest.addText
/** Adds a string, character by character. */
  addText(text: string): this;

// src/core/digest.ts:31-56 method Digest.addPlain
/** DO NOT key a record by `JSON.stringify` kept beside it */
  addPlain(value: unknown, skip?: (key: string) => boolean): this;

// src/core/digest.ts:59-70 method Digest.value
/** The digest so far, as an integer below 2^53. */
  value(): number;
