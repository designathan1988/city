// The code of src/core/scalar.ts that is the feature's (the rest of the file is shared).

// src/core/scalar.ts:56-56 function steadyRatio: the feature's own
export const steadyRatio = (a: number, b: number): number => Math.round((a / b) * 1e6) / 1e6;
