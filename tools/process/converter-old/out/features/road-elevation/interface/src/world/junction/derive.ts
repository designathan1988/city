// What the feature uses of src/world/junction/derive.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/junction/derive.ts:11-11 statement <ImportDeclaration>
import { type Leg, buildLegs, legOf, mouthLeft, mouthRight, packLegs, unpackLegs } from './legs';

// src/world/junction/derive.ts:12-12 statement <ImportDeclaration>
import { type Corner, computeCorners, cornerOf, packCorners, unpackCorners } from './corners';

// src/world/junction/derive.ts:14-14 statement <ImportDeclaration>
import { legTongue, nodeCap, nodeCentre } from './polygon';

// src/world/junction/derive.ts:47-51 function baseTrims
/** The carriageway's trims by road, the last leg of a road standing (a road that comes back to its node). */
export function baseTrims(base: Junction): Map<SegmentId, number>;

// src/world/junction/derive.ts:57-187 function deriveFromLegs
/**
 * One outer level of `base` from its legs at that level (`buildLegs` framed at the carriageway's trims, or the same legs kept as
 * numbers, `junction/levels.ts`): reads no document, so a level let go is made again, the same, from what the junction keeps.
 */
export function deriveFromLegs(base: Junction, level: SurfaceLevel, legs: readonly Leg[]): Junction | null;

// src/world/junction/derive.ts:202-202 variable JUNCTION_KIT
/** The solver's functions a node's levels are made again with (`levels.ts` reads none of them itself, `JunctionKit`). */
export const JUNCTION_KIT: JunctionKit = { legOf, packLegs, unpackLegs, packCorners, unpackCorners, nodeCentre, legTongue, nodeCap, baseTrims, deriveFromLegs };
