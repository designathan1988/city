// What the feature uses of src/render/groundShape.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/groundShape.ts:14-28 interface PlateShapeAsk
/** THE GROUND'S CUT AND FILL WORKED OUT OFF THE PAGE (`groundShape.worker.ts`) */

/** One plate's part of a shaping pass: its corners asked, as its mesh stands them. */
export interface PlateShapeAsk {
  /** The plate's centre: its corners are in its own coordinates, the roads and platforms in the world's. */
  readonly cx: number;
  readonly cy: number;
  /** Corners a side. */
  readonly n: number;
  /** The regions shaped, four numbers each (x0, x1, y0, y1, corners, clamped to the plate), in the pass's order. */
  readonly regions: Int32Array;
  /** `whole`: `xy` and `natural` hold every corner of the plate */
  readonly whole: boolean;
  /** Each corner's x and y where the mesh stands it (`CornerGrid.x`, `y`). */
  readonly xy: Float64Array;
  /** Each corner's height before any road or platform shaped it. */
  readonly natural: Float64Array;
}

// src/render/groundShape.ts:31-35 interface GroundShapeAsk
/** A shaping pass asked of the worker: the roads and the platforms its corners read, and its plates. */
export interface GroundShapeAsk {
  readonly roads: RoadShapeCut | null;
  readonly pads: PadShapeCut | null;
  readonly plates: readonly PlateShapeAsk[];
}

// src/render/groundShape.ts:38-41 interface PlateShapeAnswer
/** The corners a plate's pass moved, in the order the pass met them, and their new heights. */
export interface PlateShapeAnswer {
  readonly corners: Int32Array;
  readonly heights: Float64Array;
}

// src/render/groundShape.ts:77-90 function cornerBoxes
/** The world boxes a pass's corners stand in, a cell round (`cell`, `half`: the plates' grid) */
export function cornerBoxes(plates: readonly PlateShapeAsk[], cell: number, half: number): [number, number, number, number][];

// src/render/groundShape.ts:93-102 class ShapeScratch
/** A worker's scratch for its passes, sized to a plate once: where each corner stands and its land, and the corners visited. */
export class ShapeScratch { … }

// src/render/groundShape.ts:104-104 function NO_BOUNDS
const NO_BOUNDS = (): never =>;

// src/render/groundShape.ts:107-151 function shapeGroundAsk
/** A shaping pass as `terrain.ts` `shapeToRoads` runs it (`shapeCorners`), for each plate asked. */
export function shapeGroundAsk(ask: GroundShapeAsk, scratch = new ShapeScratch()): PlateShapeAnswer[];

// src/render/groundShape.ts:174-184 interface NaturalPlate
/** A plate's natural ground as `naturalRenderedHeightAt` reads it: its corners and the diagonals they are drawn with. */
export interface NaturalPlate {
  /** The plate's key (`world/bounds.ts` `plateOf`) and centre. */
  readonly index: number;
  readonly cx: number;
  readonly cy: number;
  readonly n: number;
  readonly cell: number;
  readonly half: number;
  readonly natural: Float64Array;
  readonly diagonals: Uint8Array;
}

// src/render/groundShape.ts:187-193 interface BlockDigestAsk
/** Blocks of two solves (or one) digested off the page: each solve's roads near its blocks, and the ground under them. */
export interface BlockDigestAsk {
  readonly solves: readonly { readonly roads: RoadShapeCut; readonly blocks: Float64Array }[];
  readonly plates: readonly NaturalPlate[];
  readonly block: number;
  readonly cell: number;
  readonly half: number;
}

// src/render/groundShape.ts:224-230 interface PadsAsk
/** The platforms of some buildings asked off the page (`renderer.ts` `shapeBlocksSteps`): each building's record and its floor as the page keeps it (`floors.ts`), the paving round them and the ground it was solved on. */
export interface PadsAsk {
  readonly buildings: readonly { readonly record: RecordRef; readonly floor: number }[];
  /** The paving over a rectangle holding every bank (`world/paving.ts` `PavingSource.pack`). */
  readonly paving: PavingData;
  readonly ground: SharedGroundData;
  readonly apron: number;
}

// src/render/groundShape.ts:233-250 function padsOfAsk
/** Each building's platform as the page works it out (`siteGrading.ts` `refresh`: `padOf` on its kept floor), in the ask's order */
export function padsOfAsk(ask: PadsAsk): (Pad | null)[];

// src/render/groundShape.ts:255-255 type GroundJobAnswer
/** What the worker answers each. */
export type GroundJobAnswer = PlateShapeAnswer[] | Float64Array[] | (Pad | null)[];
