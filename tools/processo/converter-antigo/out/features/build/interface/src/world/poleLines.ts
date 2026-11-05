// What the feature uses of src/world/poleLines.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/poleLines.ts:150-156 function polesInclude
/** The streets whose kerb lines carry poles: ground streets with footways (the two levels read; `surfaces` unions all four). */
function polesInclude(net: Network): (id: SegmentId) => boolean;

// src/world/poleLines.ts:265-265 interface WindingRing
/** A ring as Clipper takes it in (`core/clipper.ts` `input`): its vertices on Clipper's grid */
interface WindingRing { readonly xy: Float64Array; readonly x0: number; readonly y0: number; readonly x1: number; readonly y1: number }

// src/world/poleLines.ts:315-315 interface FootRings
/** The footway and kerb rings of a street or a junction (`NetworkLines.rings`): made again with the object the network builds for it. */
interface FootRings { readonly paving: WindingRing[]; readonly kerbed: WindingRing[] }

// src/world/poleLines.ts:318-338 function insideLevels
/** DO NOT answer a point from a region's union again: the answer is local. */
function insideLevels(net: Network, p: Vec2): { readonly paving: boolean; readonly kerbed: boolean };

// src/world/poleLines.ts:341-352 function ribbonRings
/** The footway and kerb rings of a street, made once per ribbon the network built. */
function ribbonRings(net: Network, ribbon: SegmentRibbon, include: (id: SegmentId) => boolean): FootRings;

// src/world/poleLines.ts:355-366 function junctionRings
/** The footway and kerb rings of a junction, made once per junction the network solved. */
function junctionRings(net: Network, id: NodeId, byLevel: ReadonlyMap<SurfaceLevel, Junction>, include: (id: SegmentId) => boolean): FootRings;
