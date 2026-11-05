// What the feature uses of src/render/propCells.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/propCells.ts:48-48 variable PROP_CELL
/**
 * THE PROPS HELD ROUND THE VIEW, NOT FOR THE MAP: the gardens' plants, the street furniture and its plants (`gardens.ts`,
 * `streetFurnitureLayer.ts`), worked out cell by cell from the document as the view asks for them (`viewCells.ts`), never for the whole map.
 *
 * - NEAR, the square of world cells round the eye where a plant may be big enough on the screen to be drawn whole (`PICTURE_PX`):
 *   each cell's things placed on the CPU (`place`, a pure function of the cell and the document) and kept in the cells' instances
 *   (`cellInstances.ts`), culled and levelled thing by thing as before. Held under a CPU budget, least recently used out first.
 * - FAR, everything else the view reaches, to the horizon: blocks of 4 x 4 cells, each the same things placed the same way, written
 *   once into GPU buffers (a picture batch for the plants, `plantImpostors.ts`, and a batch per kind for the rest) whose CPU arrays go
 *   once they are on the GPU (`mesh/gpuOnly.ts`). The GPU thins and sizes the pictures itself (`impostorThin`), as it does the near
 *   cells' far pictures: the horizon shows the same trees at the same places. Held under a GPU budget.
 * - A far batch's thing standing in the near square is not drawn (its pose's cell against `NEAR_SQUARE`, `instanceRuns.ts`): the near
 *   cells draw it. The square only covers the cells whose near things are all in: nothing is drawn twice, nothing missing.
 *
 * DO NOT go back to placing every garden, bench and street tree of the map on the page: 102,600 garden plants of the 8,100-building city
 * held 29 MB and the furniture 46 MB (2026-10-21), ~2 GB at 200,000 buildings.
 */

/** The near cells' side: the world's grid (`cells.ts`). */
export const PROP_CELL = CELL;

// src/render/propCells.ts:53-53 variable MOST_NEAR
/** The most cells the near square reaches each way from the eye's: a 4K screen, a tall tree. */
const MOST_NEAR = 8;

// src/render/propCells.ts:67-84 interface PropView
/** What a view reaches, on the ground, in world units (x east, y north). */
export interface PropView {
  /** Projection times view. */
  readonly view: Matrix4;
  readonly frustum: Frustum;
  /** Pixels a world unit spans one unit before the eye (perspective) or anywhere (orthographic): half the screen's height in device pixels times the projection's scale. */
  readonly perUnitAtOne: number;
  readonly ortho: boolean;
  /** Where distances are measured from: the eye over the ground (perspective) or the footprint's middle (orthographic). */
  readonly x: number;
  readonly y: number;
  /** How far the view reaches on the ground from there: the far plane's farthest corner, or the footprint's radius. */
  readonly reach: number;
  /** Perspective: how much farther than its depth a point at the frustum's corner is (the cull measures a size by the depth, `cellInstances.ts`), and the eye's height over the ground. */
  readonly slant: number;
  readonly height: number;
  /** The ground's height under the view (three's y), for the far blocks' boxes. */
  readonly ground: number;
}

// src/render/propCells.ts:119-124 function reachFor
/** How far from the view's point things of `size` across are at least `px` pixels: the view's reach, or -1 for none (orthographic, too small). */
function reachFor(v: PropView, size: number, px: number): number;

// src/render/propCells.ts:127-127 interface PropSquare
/** The near square a view asks for: its middle cell and how many cells it reaches each way (-1: none). */
export interface PropSquare { readonly cx: number; readonly cy: number; readonly k: number }

// src/render/propCells.ts:130-134 function nearSquareOf
/** The square round the view's point holding every plant `largest` across (world units) big enough to be drawn whole there (`PICTURE_PX`, less the band it keeps its level in). */
export function nearSquareOf(v: PropView, largest: number): PropSquare;

// src/render/propCells.ts:137-141 interface PropPlacement
/** The things a layer stands about a rectangle (at least every one whose pose stands in it), by kind, poses in `YAW_POSE`. */
export interface PropPlacement {
  readonly packs: ReadonlyMap<number, Pack>;
  /** Whether any of them changes with the city's hour (a plant still growing). */
  readonly growing: boolean;
}

// src/render/propCells.ts:143-163 interface PropLayerSpec
export interface PropLayerSpec {
  readonly name: string;
  /** The placing's version: a product made by another is made again. */
  readonly version: number;
  /** The kinds drawn, as the cells' instances draw them near. */
  readonly kinds: readonly KindSpec[];
  /** How far off the view a thing still throws its shadow into it. */
  readonly margin: number;
  /** The diary's kinds the placing reads, and how far past a change's rectangle a thing it moved may stand. */
  readonly changes: readonly ChangeKind[];
  readonly pad: number;
  /** The plants' pictures, and each kind's picture in them (null: a kind drawn far by its own meshes). */
  readonly pictures: PlantImpostors | null;
  readonly pictureOf: readonly (number | null)[];
  /** The things standing about a rectangle, from the document as it is: a pure function of the rectangle and the document. */
  place(rect: ChangeRect): PropPlacement;
  /** Budgets: the near cells' CPU bytes, the far blocks' CPU bytes before their upload and their GPU bytes. */
  readonly nearBytes?: number;
  readonly farBytes?: number;
  readonly gpuBytes?: number;
}

// src/render/propCells.ts:166-169 interface NearCell
/** A near cell's things, kept to be put in the cells' instances while the cell is in the square. */
interface NearCell {
  readonly packs: ReadonlyMap<number, Pack>;
  readonly growing: boolean;
}

// src/render/propCells.ts:172-182 interface FarBlock
/** A far block's batches: their meshes, each one's shadow chooser, whether it grows with the hour, its arrays not yet on the GPU. */
interface FarBlock {
  /** Its meshes' parent: shown only while the view wants the block (a block held for later draws nothing). */
  readonly group: Group;
  /** Its plants' pictures' batch, under `group`: shown once the pictures are taken. */
  readonly pictured: Group;
  readonly meshes: InstancedMesh[];
  readonly uncast: (() => void)[];
  readonly growing: boolean;
  waiting: number;
  readonly gpu: number;
}

// src/render/propCells.ts:185-185 enum FarClass
/** The far classes: the plants' pictures and the things drawn at every size reach the horizon; the small things drawn down to a pixel (`KindLevels.least`) only where they are. */
const enum FarClass { wide = 0, small = 1 }

// src/render/propCells.ts:187-212 interface PropLayer
export interface PropLayer {
  /** The near cells' meshes and the far batches. */
  readonly group: Group;
  readonly meshes: readonly InstancedMesh[];
  /** Triangles of the near meshes, every live instance counted. */
  readonly triangles: number;
  /** Bytes held on the CPU: the near cells' things, the far blocks' arrays not yet on the GPU, the near instances' arrays. */
  readonly cpuBytes: number;
  readonly near: ViewCells<NearCell>;
  readonly far: readonly ViewCells<FarBlock>[];
  /** The largest plant it placed so far, world units across (its picture's sphere times its scale): what the near square reaches for. */
  readonly largestPicture: number;
  /** The cells and blocks the view wants that are not placed yet. */
  readonly busy: boolean;
  /** The diary's changes since the last look: the cells and blocks they reached dropped, placed again as the view asks. */
  follow(journal: ChangeJournal): void;
  /** The cells and blocks with a thing that grows with the hour dropped (a new city hour). */
  forgetGrowing(): void;
  /** The near cells of the square's rings placed (within the frame's time), up to `k`: the rings in. */
  ringsIn(square: PropSquare, deadline: number): number;
  /** The square drawn near: the cells within `k` of its middle put in the cells' instances, the rest taken out; then the far blocks the view wants placed (within the frame's time). */
  show(square: PropSquare, view: PropView, deadline: number): void;
  setMap(map: boolean): void;
  cull(frustum: Frustum, view: Matrix4): void;
  dispose(): void;
}

// src/render/propCells.ts:254-603 function createPropLayer
/** A layer of things held round the view. */
export function createPropLayer(spec: PropLayerSpec): PropLayer;

// src/render/propCells.ts:609-618 function lookProps
/**
 * The layers brought to a view, one square for all (the far batches of every layer read one `NEAR_SQUARE`): each layer's rings
 * placed within the frame's time, the square drawn near the one all have in, then each layer's far blocks.
 */
export function lookProps(layers: readonly PropLayer[], view: PropView): PropSquare;
