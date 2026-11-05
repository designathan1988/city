// What the feature uses of src/world/paving.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/paving.ts:27-31 interface PavingData
/** What the paving reads near a rectangle, to hand a worker: the solved profiles there */
export interface PavingData {
  readonly rect: readonly [number, number, number, number];
  readonly field: FieldData | null;
  readonly flush: FlushData | null;
}

// src/world/paving.ts:34-38 interface PavingSource
/** The paving's height as the page reads it, and what to hand a worker for a rectangle of it. */
export interface PavingSource {
  (x: number, y: number): number;
  /** Null when it cannot be handed over (a field that cannot pack its roads): the page is asked. */
  readonly pack?: (rect: readonly [number, number, number, number]) => { data: PavingData; transfer: ArrayBuffer[] } | null;
}

// src/world/paving.ts:41-63 function pavingReader
/** The paving's height over `PavingData`; undefined outside its rectangle, or where the field needs the ground and `naturalAt` is not given */
export function pavingReader(data: PavingData, naturalAt?: (x: number, y: number) => number): (x: number, y: number) => number | undefined;
