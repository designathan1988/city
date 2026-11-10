// The code of src/world/paving.ts that is the feature's (the rest of the file is shared).

// src/world/paving.ts:27-31 slice of src/world/paving.ts#PavingData (shared): the feature's statements; "…" marks code left out
export interface PavingData {
  // … 2 lines
  readonly flush: FlushData | null;
}

// src/world/paving.ts:41-63 slice of src/world/paving.ts#pavingReader (shared): the feature's statements; "…" marks code left out
export function pavingReader(data: PavingData, naturalAt?: (x: number, y: number) => number): (x: number, y: number) => number | undefined {
  // … 9 lines
  const share = data.flush ? flushShareOf(data.flush) : null;
// … 11 lines
}
