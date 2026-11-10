// What the feature uses of src/render/buildings/cellArrays.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/buildings/cellArrays.ts:80-80 variable FINISH_ATTRIBUTE
/** The finish id of a shell's part per instance, and of a ruin's piece per vertex (`finishes.ts`): its place in `FINISHES`, its layer of the finishes' maps. */
export const FINISH_ATTRIBUTE = 'shellFinish';

// src/render/buildings/cellArrays.ts:87-87 function finishId
/** A finish's id, as the shell's program reads it. */
export const finishId = (finish: Finish): number => …;

// src/render/buildings/cellArrays.ts:90-90 variable COLOUR_RANGE
/** The brightest linear colour a shell's paint holds: a light paint shaded up (1.1), the building's own shade (1.05) and the tone (1.03) stay under it. */
export const COLOUR_RANGE = 2;

// src/render/buildings/cellArrays.ts:92-92 variable PAINT_UNIT
/** The largest unsigned short: a paint's channel and its finish id over it. */
export const PAINT_UNIT = 65535;

// src/render/buildings/cellArrays.ts:94-94 variable NORMAL_UNIT
/** The largest signed short: a normal's and a wear's channel over it (WebGL 2: a normalised short reads back as c / 32767). */
export const NORMAL_UNIT = 32767;

// src/render/buildings/cellArrays.ts:102-102 variable SHELL_SHORTS
/** Numbers a shell vertex carries in its paint (`paint`): four shorts, rgb and its finish with its wear (`finishWear`). */
export const SHELL_SHORTS = 4;

// src/render/buildings/cellArrays.ts:104-104 variable NORMAL_SHORTS
/** Numbers a shell vertex carries in its normal (`normal`): two signed shorts, octahedral (`octNormal`). */
export const NORMAL_SHORTS = 2;

// src/render/buildings/cellArrays.ts:106-106 variable WEAR_STEPS
/** A wear's steps in the paint's fourth short (`finishWear`): its high byte. */
export const WEAR_STEPS = 255;
