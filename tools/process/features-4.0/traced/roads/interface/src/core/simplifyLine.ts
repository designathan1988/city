// What the feature uses of src/core/simplifyLine.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/simplifyLine.ts:5-24 function simplifyLine
/** An open line with the points within `tol` of the line through the points kept either side of them taken out, its ends kept
 *  (Ramer-Douglas-Peucker: no point dropped lies farther than `tol` from the line drawn). */
export function simplifyLine(points: readonly Vec2[], tol: number): Vec2[];
