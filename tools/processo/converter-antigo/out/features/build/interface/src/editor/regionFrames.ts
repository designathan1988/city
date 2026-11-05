// What the feature uses of src/editor/regionFrames.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/regionFrames.ts:10-10 variable K_JSON,K_TABLE,K_ORDERS,K_PAIRS
/** THE FRAMES OF A MAP SAVED AS BINARY REGIONS (`chunkedSave.ts` manifest 6): a square's records (the buildings their record table, `encodeRecords`; the other layers their records' JSON) and their places in the order as doubles, a square's wears as doubles, a part kept outside the squares and the manifest as JSON - each one frame known by its magic word (`saveBinary.ts`) */

/** The sections: records as JSON or as a record table, their places in the order; a value's JSON; numbers in pairs. */
const K_JSON = 1, K_TABLE = 2, K_ORDERS = 3, K_PAIRS = 1;

// src/editor/regionFrames.ts:13-13 interface SquareRecords
/** A stored square: its records (`r`) and, for a list or the lots, their places in the order (`o`). */
export interface SquareRecords { readonly r: readonly unknown[]; readonly o?: readonly number[] }

// src/editor/regionFrames.ts:16-21 function squareFrame
/** A square's frame: buildings as their record table (no text of them), other records as their JSON, the order as doubles. */
export function squareFrame(buildings: boolean, records: readonly unknown[], orders: readonly number[] | null | undefined): ArrayBuffer;

// src/editor/regionFrames.ts:24-33 function readSquareFrame
/** A square's frame read back (`squareFrame`); null when the bytes are not one. */
export function readSquareFrame(bytes: Uint8Array): SquareRecords | null;

// src/editor/regionFrames.ts:36-40 function wearFrame
/** A square's wears: each building's id and decay (0: none), as doubles in pairs. */
export function wearFrame(records: readonly Building[]): ArrayBuffer;
