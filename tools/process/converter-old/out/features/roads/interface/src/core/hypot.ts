// What the feature uses of src/core/hypot.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/hypot.ts:7-24 function hypot2
/** THE LENGTH OF A 2D OR 3D VECTOR, BIT FOR BIT `Math.hypot`, WITH NO NUMBER BOXED OUT OF A BUILT-IN CALL */
// V8's `Math.hypot` is a built-in (`src/builtins/math.tq` MathHypot) the optimising compiler calls, not inlines: its arguments go
// in an array and its result comes back boxed, one heap number a call. This is its algorithm in plain JS, which V8 inlines:
// the absolute values over the largest, their squares summed with Kahan's compensation, the root times the largest.

/** `Math.hypot(dx, dy)` to the bit (Infinity before NaN, then 0 when both are 0, as the built-in). */
export function hypot2(dx: number, dy: number): number;
