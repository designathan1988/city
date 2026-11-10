// What the feature uses of src/render/structures.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/structures.ts:88-88 variable WALL_THICK
/** A retaining wall's thickness, its coping over the ground it holds, its footing under the road. */
const WALL_THICK = m(0.32);

// src/render/structures.ts:89-89 variable WALL_COPING
const WALL_COPING = m(0.18);

// src/render/structures.ts:90-90 variable WALL_FOOT
const WALL_FOOT = m(0.24);

// src/render/structures.ts:95-152 function retainingWalls
/** THE RETAINING WALLS of a cutting (`RoadSegment.cutWalls`) */
function retainingWalls(
  net: Network, walled: readonly SegmentRibbon[], elevation: RoadElevation, terrainAt: (x: number, y: number) => number,
  faces: Extrusion, lids: Extrusion, faceTile: number, lidTile: number,
): void;

// src/render/structures.ts:265-346 class Extrusion
/** Flat-shaded quads, indexed, built in world coordinates and mapped into three's frame once. */
class Extrusion { … }
