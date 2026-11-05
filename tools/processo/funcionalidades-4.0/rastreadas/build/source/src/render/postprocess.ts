// The code of src/render/postprocess.ts that is the feature's (the rest of the file is shared).

// src/render/postprocess.ts:28-57 slice of src/render/postprocess.ts#PostChain (shared): the feature's statements; "…" marks code left out
export interface PostChain {
  // … 14 lines
  followTown(cx: number, cy: number, solids: (() => CameraSolids) | null, changed: readonly (readonly [number, number, number, number])[] | null): void;
// … 13 lines
}
