// What the feature uses of src/world/buildings/cameraSolids.ts (shared with the program): declarations as the source writes them, bodies left out.

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
