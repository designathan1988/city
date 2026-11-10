// What the feature uses of src/render/partLevelsWork.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/partLevelsWork.ts:20-20 variable LEVEL_ERRORS
/** A VEHICLE PART'S LEVELS AS ARRAYS (`vehicleLevels.ts`): welded, simplified once per error and, for box assemblies, their normals creased again; all of it in the levels' worker (`people/lodWorker.ts` 'part') */
/** DO NOT weld a part's vertices on the page (`mergeVertices`, `toCreasedNormals`: every vertex of every part of every vehicle while the map opens). */

/** Errors of a vehicle part's coarser levels, in the part's own units (metres; a wheel's diameters), finest first. */
export const LEVEL_ERRORS = [0.0125, 0.025, 0.05, 0.1, 0.2, 0.4] as const;
