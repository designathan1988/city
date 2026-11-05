// What the feature uses of src/world/rescale.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/rescale.ts:10-11 function unitFactor
/** What a stored length is worth in this build's units, for a map saved with `unit`. */
export const unitFactor = (unit: unknown): number => …;

// src/world/rescale.ts:13-13 type Loose
type Loose = Record<string, unknown>;

// src/world/rescale.ts:14-14 function isRecord
const isRecord = (v: unknown): v is Loose => …;

// src/world/rescale.ts:15-15 function finite
const finite = (v: unknown): v is number => …;

// src/world/rescale.ts:18-18 type PlaceFn
/** Positions of the map: the transform a document's points go through. */
export type PlaceFn = (x: number, y: number) => { x: number; y: number };

// src/world/rescale.ts:35-35 function list
const list = <T>(v: readonly T[] | undefined, f: (item: T) => T): T[] | undefined => …;

// src/world/rescale.ts:38-75 function rescaleBuilding
/** A stored building (any schema) in the new unit: its place moved, its measures scaled. */
export function rescaleBuilding(raw: unknown, factor: number, place: PlaceFn | null): unknown;

// src/world/rescale.ts:78-132 function rescaleSerializedDoc
/** A serialized document in this build's unit. */
export function rescaleSerializedDoc(data: SerializedDoc): SerializedDoc;
