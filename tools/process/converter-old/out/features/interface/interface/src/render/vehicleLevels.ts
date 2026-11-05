// What the feature uses of src/render/vehicleLevels.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/vehicleLevels.ts:12-12 variable ERROR_PIXELS
/** A level is drawn only while its error on the screen is under this: the swap is not seen (meshoptimizer README, `ErrorAbsolute`). */
export const ERROR_PIXELS = VEHICLE_ERROR_PIXELS;

// src/render/vehicleLevels.ts:23-27 function levelFor
/** The coarsest level whose error stays under `ERROR_PIXELS` for a part drawn at `pixelsPerUnit` (pixels a unit of its own geometry). */
export function levelFor(pixelsPerUnit: number): number;
