// The code of src/render/vehicleLevels.ts that is the feature's (the rest of the file is shared).

// src/render/vehicleLevels.ts:12-12 slice of src/render/vehicleLevels.ts#ERROR_PIXELS (shared): the feature's statements; "…" marks code left out
export const ERROR_PIXELS = VEHICLE_ERROR_PIXELS;

// src/render/vehicleLevels.ts:23-27 slice of src/render/vehicleLevels.ts#levelFor (shared): the feature's statements; "…" marks code left out
export function levelFor(pixelsPerUnit: number): number {
  // … 1 lines
  while (level < LEVEL_ERRORS.length && LEVEL_ERRORS[level]! * pixelsPerUnit <= ERROR_PIXELS) level++;
// … 1 lines
}
