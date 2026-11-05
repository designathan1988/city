// What the feature uses of src/world/buildings/cameraSolids.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildings/cameraSolids.ts:10-10 variable CLEARANCE
/** THE LOWEST THE CAMERA'S EYE MAY STAND OVER THE BUILDINGS, a continuous function of the map point */

/** How far over a roof the eye keeps, world units. */
const CLEARANCE = m(1);

// src/world/buildings/cameraSolids.ts:14-14 variable CELL
/** The grid the volumes are filed in, world units a cell. */
const CELL = m(40);

// src/world/buildings/cameraSolids.ts:18-18 variable S_BX,S_BY,S_COS,S_SIN,S_X0,S_Y0,S_X1,S_Y1,S_FOOT,S_TOP
const S_BX = 0, S_BY = 1, S_COS = 2, S_SIN = 3, S_X0 = 4, S_Y0 = 5, S_X1 = 6, S_Y1 = 7, S_FOOT = 8, S_TOP = 9;

// src/world/buildings/cameraSolids.ts:20-20 variable S_WALL0,S_WALL1,S_INSET
/** What the volume surely hides (`forEachOccluder`): the bottom and top of its walls, world height, its plan inset by `S_INSET`; NaN when it hides nothing for sure (a building with voids, a volume of its own outline). */
const S_WALL0 = 10, S_WALL1 = 11, S_INSET = 12;

// src/world/buildings/cameraSolids.ts:25-25 variable PAGE_BITS
const PAGE_BITS = 12;

// src/world/buildings/cameraSolids.ts:26-26 variable PAGE
const PAGE = 1 << PAGE_BITS;

// src/world/buildings/cameraSolids.ts:28-75 class SolidPages
class SolidPages { … }

// src/world/buildings/cameraSolids.ts:31-31 field SolidPages.stamps
/** Each slot's last visit (`visitOnce`): a volume filed in several cells is visited once a walk. */
  private stamps: Uint32Array[] = [];

// src/world/buildings/cameraSolids.ts:65-67 method SolidPages.page
/** The page holding `slot`, read at `at(slot) + S_*`. */
  page(slot: number): Float64Array;

// src/world/buildings/cameraSolids.ts:78-78 function at
/** Where a slot's numbers start in its page. */
const at = (slot: number): number => …;

// src/world/buildings/cameraSolids.ts:80-91 interface CameraSolids
export interface CameraSolids {
  /** The lowest the eye may stand at (x, y), world height; -Infinity away from every building. */
  floorAt(x: number, y: number): number;
  /** The roof over (x, y) when the point is inside a solid volume's plan (its highest, no margin round it), world height */
  roofAt(x: number, y: number): number;
  /** Every solid volume filed in the cells over a map rectangle, once, as its building's frame */
  forEachIn(x0: number, y0: number, x1: number, y1: number, visit: SolidVisit): void;
  /** Every solid volume filed in the cells over any of the rectangles, once for all of them. */
  forEachInAny(rects: readonly Rect[], visit: SolidVisit): void;
  /** Every volume over a map rectangle that surely hides what stands behind it (`render/occlusion.ts`): the box of its walls drawn in by its reliefs, never its roof. */
  forEachOccluder(x0: number, y0: number, x1: number, y1: number, visit: OccluderVisit): void;
}

// src/world/buildings/cameraSolids.ts:94-94 type OccluderVisit
/** An occluder as its building's frame: the building's centre and turn, the box in that frame, the walls' bottom and top (world height). */
export type OccluderVisit = (bx: number, by: number, cos: number, sin: number, lx0: number, ly0: number, lx1: number, ly1: number, bottom: number, top: number) => void;

// src/world/buildings/cameraSolids.ts:97-97 type SolidVisit
/** A solid volume as its building's frame: the building's centre and turn, the volume's box in that frame, its top. */
export type SolidVisit = (bx: number, by: number, cos: number, sin: number, lx0: number, ly0: number, lx1: number, ly1: number, top: number) => void;

// src/world/buildings/cameraSolids.ts:102-102 type Rect
/** A world rectangle: [minX, minY, maxX, maxY]. */
export type Rect = readonly [number, number, number, number];

// src/world/buildings/cameraSolids.ts:177-177 function cellKey
const cellKey = (cx: number, cy: number): number => …;

// src/world/buildings/cameraSolids.ts:199-199 field SolidFiling.pages
private readonly pages = new SolidPages();

// src/world/buildings/cameraSolids.ts:200-200 field SolidFiling.cells
private readonly cells = new Map<number, number[]>();
