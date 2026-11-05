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

// src/render/coverSweep.ts:24-28 interface ShoreCandidate
/** A stone of a river's bed or bank: kept by its height over the water, read on the ground as drawn. */
export interface ShoreCandidate extends Candidate {
  readonly level: number;
  /** Its cell's draw against the odds of its height over the water. */
  readonly draw: number;
}

// src/render/coverSweep.ts:31-35 interface CoverField
/** What a sweep reads of the ground: the painted covers' densities (0..1) and the water's level at a point. */
export interface CoverField {
  forestAt(x: number, y: number): number;
  coverAt(kind: 'scrub' | 'rocks', x: number, y: number): number;
  shoreLevelAt(x: number, y: number): number | null;
}

// src/render/coverSweep.ts:52-56 function cellHash
/** A cell's hash in 0..1: the same wherever a placement asks it, so an edit elsewhere never moves a plant. */
export const cellHash = (a: number, b: number, salt: number): number =>;
