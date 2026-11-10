// What the feature uses of src/world/junction/legs.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/junction/legs.ts:35-35 getter PackedLeg.origin
get origin(): Vec2;

// src/world/junction/legs.ts:36-36 getter PackedLeg.dir
get dir(): Vec2;

// src/world/junction/legs.ts:37-37 getter PackedLeg.nrm
get nrm(): Vec2;

// src/world/junction/legs.ts:38-38 getter PackedLeg.ang
get ang(): number;

// src/world/junction/legs.ts:39-39 getter PackedLeg.hw
get hw(): number;

// src/world/junction/legs.ts:40-40 getter PackedLeg.hwLeft
get hwLeft(): number;

// src/world/junction/legs.ts:41-41 getter PackedLeg.hwRight
get hwRight(): number;

// src/world/junction/legs.ts:42-42 getter PackedLeg.hwSidewalk
get hwSidewalk(): number;

// src/world/junction/legs.ts:43-43 getter PackedLeg.length
get length(): number;

// src/world/junction/legs.ts:44-44 getter PackedLeg.shift
get shift(): number;

// src/world/junction/legs.ts:48-53 function legOf
/** A leg kept compact (`PackedLeg`) from its fields. */
export function legOf(d: Leg): Leg;

// src/world/junction/legs.ts:65-74 function packLegs
/**
 * Legs as one run of numbers, their profiles in `roads` (each once: a message's clone keeps one copy of an object it meets twice):
 * DO NOT hand a thread a leg as an object of points - the page took 0.4 s to read back the junctions of the city of 70,000 buildings
 * solved off it (2026-10-22).
 */
export function packLegs(legs: readonly Leg[], roads: RoadType[]): number[];

// src/world/junction/legs.ts:77-84 function unpackLegs
/** The legs of `packLegs`. */
export function unpackLegs(nums: readonly number[], roads: readonly RoadType[]): Leg[];

// src/world/junction/legs.ts:95-98 interface LegBuildOptions
export interface LegBuildOptions {
  /** Per-leg trim guesses, keyed by SEGMENT. */
  readonly trims?: ReadonlyMap<SegmentId, number>;
}

// src/world/junction/legs.ts:101-156 function buildLegs
/** Builds the legs of a node, sorted counter-clockwise by outgoing angle. */
export function buildLegs(
  doc: RoadDoc,
  cache: PolylineCache,
  nodeId: NodeId,
  level: SurfaceLevel,
  opts: LegBuildOptions = {},
): Leg[];

// src/world/junction/legs.ts:159-160 function mouthRight
/** The mouth corner on the `-nrm` side of a leg, at distance `t`. */
export const mouthRight = (leg: Leg, t: number): Vec2 => …;

// src/world/junction/legs.ts:163-164 function mouthLeft
/** The mouth corner on the `+nrm` side of a leg, at distance `t`. */
export const mouthLeft = (leg: Leg, t: number): Vec2 => …;
