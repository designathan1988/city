// The code of src/world/approach.ts that is the feature's (the rest of the file is shared).

// src/world/approach.ts:18-19 function stopLineDistance: the feature's own
export const stopLineDistance = (trim: number): number =>
  trim + CROSSWALK_SETBACK + CROSSWALK_DEPTH + STOP_BAR_SETBACK;
