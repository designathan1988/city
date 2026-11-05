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

// src/render/coverSweep.ts:38-38 variable FOREST_SPACING
/** A tree every twelve metres at the most where a wood is painted: the player wants few trees, not woods filling the map */
const FOREST_SPACING = m(12);

// src/render/coverSweep.ts:40-40 variable SCRUB_SPACING
/** Low scrub (`world/terrainPaint.ts` 'scrub'): close-set low-poly bushes of a metre to two and a half, no trees */
const SCRUB_SPACING = m(2.4);

// src/render/coverSweep.ts:42-42 variable ROCK_SPACING
/** The stones: where rocks were painted, one candidate a cell of ROCK_SPACING kept by the density */
const ROCK_SPACING = m(3.2);

// src/render/coverSweep.ts:45-45 variable SHORE_REACH
/** How far past the water's box its stones are looked for. */
const SHORE_REACH = m(12);

// src/render/coverSweep.ts:49-49 variable COVER_SWEEP_PAD
/** How far a dab's change of the covers reaches a place: a cover cell (8 m, `terrain.ts` FOREST_RES) and the widest jitter */
export const COVER_SWEEP_PAD = m(8) + FOREST_SPACING * 0.45;

// src/render/coverSweep.ts:52-56 function cellHash
/** A cell's hash in 0..1: the same wherever a placement asks it, so an edit elsewhere never moves a plant. */
export const cellHash = (a: number, b: number, salt: number): number =>;

// src/render/coverSweep.ts:59-73 function scrubClump
/** -1..1 value noise of about 25 m: where scrub gathers into thickets. */
function scrubClump(x: number, y: number): number;

// src/render/coverSweep.ts:76-83 function insidePlate
/** Whether lattice cell (i, j) of `spacing` lies whole inside one plate: the painted covers' lattice stops at each plate's edge. */
function insidePlate(i: number, j: number, spacing: number): boolean;

// src/render/coverSweep.ts:86-91 function latticeOver
/** The lattice cells of `spacing` (anchored at -TERRAIN_HALF) a rectangle reaches: [i0, i1, j0, j1]. */
function latticeOver(rect: ChangeRect, spacing: number): [number, number, number, number];

// src/render/coverSweep.ts:95-115 function forestIn
/** The painted woods' trees (`world/terrainPaint.ts` 'forest') standing in keep cell `key`. */
export function forestIn(key: number, field: CoverField): Candidate[];

// src/render/coverSweep.ts:118-135 function scrubIn
/** The low scrub ('scrub') standing in keep cell `key`. */
export function scrubIn(key: number, field: CoverField): Candidate[];

// src/render/coverSweep.ts:138-156 function rocksIn
/** The painted stones ('rocks') lying in keep cell `key`. */
export function rocksIn(key: number, field: CoverField): Candidate[];

// src/render/coverSweep.ts:159-159 interface WaterBox
/** The world box a plate's water covers (`PlateSurface.waterArea`, moved by the plate's centre). */
interface WaterBox { readonly minX: number; readonly maxX: number; readonly minY: number; readonly maxY: number }

// src/render/coverSweep.ts:162-183 function shoreIn
/** The places a stone may lie along the water - in the shallows, on the line and up the bank - in keep cell `key`. */
export function shoreIn(key: number, field: CoverField, water: WaterBox | null): ShoreCandidate[];
