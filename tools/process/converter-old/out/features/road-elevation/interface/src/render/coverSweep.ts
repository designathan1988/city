// What the feature uses of src/render/coverSweep.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/coverSweep.ts:14-21 interface Candidate
/** THE PAINTED COVERS' AND THE RIVERS' PLACES, ONE KEEP CELL AT A TIME (`cellKeeps.ts`) */

/** A place a plant or a stone may stand (`CandidateKeep` keeps those that do). */
export interface Candidate {
  readonly x: number;
  readonly y: number;
  readonly size: number;
  readonly yaw: number;
  readonly seed: number;
  readonly species: ForestSpecies;
}

// src/render/coverSweep.ts:38-38 variable FOREST_SPACING
/** A tree every twelve metres at the most where a wood is painted: the player wants few trees, not woods filling the map */
const FOREST_SPACING = m(12);

// src/render/coverSweep.ts:49-49 variable COVER_SWEEP_PAD
/** How far a dab's change of the covers reaches a place: a cover cell (8 m, `terrain.ts` FOREST_RES) and the widest jitter */
export const COVER_SWEEP_PAD = m(8) + FOREST_SPACING * 0.45;
