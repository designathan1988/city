// What the feature uses of src/render/people/cookPack.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/people/cookPack.ts:3-3 type Typed
/** A cooked record on disk: a JSON header of its fields, then each typed array's bytes, 8-byte aligned. */

type Typed = Float32Array | Float64Array | Int32Array | Uint32Array | Int16Array | Uint16Array | Uint8Array;

// src/render/people/cookPack.ts:4-4 type PackValue
export type PackValue = Typed | number | string | boolean | readonly number[] | null;

// src/render/people/cookPack.ts:7-7 variable KINDS
const KINDS = { Float32Array, Float64Array, Int32Array, Uint32Array, Int16Array, Uint16Array, Uint8Array } as const;

// src/render/people/cookPack.ts:8-8 type KindName
type KindName = keyof typeof KINDS;
