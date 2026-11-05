// What the feature uses of src/render/buildings/cellArrays.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/buildings/cellArrays.ts:80-80 variable FINISH_ATTRIBUTE
/** The finish id of a shell's part per instance, and of a ruin's piece per vertex (`finishes.ts`): its place in `FINISHES`, its layer of the finishes' maps. */
export const FINISH_ATTRIBUTE = 'shellFinish';

// src/render/buildings/cellArrays.ts:87-87 function finishId
/** A finish's id, as the shell's program reads it. */
export const finishId = (finish: Finish): number => …;
