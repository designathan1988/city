// What the feature uses of src/render/propCells.ts (shared with the program): declarations as the source writes them, bodies left out.

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

// src/render/propCells.ts:127-127 interface PropSquare
/** The near square a view asks for: its middle cell and how many cells it reaches each way (-1: none). */
export interface PropSquare { readonly cx: number; readonly cy: number; readonly k: number }

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

// src/render/propCells.ts:609-618 function lookProps
/**
 * The layers brought to a view, one square for all (the far batches of every layer read one `NEAR_SQUARE`): each layer's rings
 * placed within the frame's time, the square drawn near the one all have in, then each layer's far blocks.
 */
export function lookProps(layers: readonly PropLayer[], view: PropView): PropSquare;
