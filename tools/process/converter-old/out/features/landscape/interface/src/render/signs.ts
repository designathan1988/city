// What the feature uses of src/render/signs.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/signs.ts:20-31 variable PLATE
/** Signs on the streets: the ones placed with the sign tool */

/** Plate size by type, metres, and its centre's height above the footway. */
export const PLATE: Readonly<Record<SignType, { w: number; h: number; z: number }>> = {
  stop: { w: 0.75, h: 0.75, z: 2.2 },
  yield: { w: 0.8, h: 0.7, z: 2.2 },
  speed: { w: 0.6, h: 0.6, z: 2.2 },
  noParking: { w: 0.6, h: 0.6, z: 2.2 },
  noEntry: { w: 0.6, h: 0.6, z: 2.2 },
  pedestrian: { w: 0.7, h: 0.7, z: 2.2 },
  school: { w: 0.7, h: 0.7, z: 2.2 },
  direction: { w: 1.6, h: 0.5, z: 2.4 },
  street: { w: 1.0, h: 0.28, z: 2.6 },
  info: { w: 1.0, h: 0.6, z: 2.2 },
};

// src/render/signs.ts:34-34 variable POST_RADIUS
/** The post's radius at its top (`buildSigns` post). */
const POST_RADIUS = m(0.04);

// src/render/signs.ts:36-36 variable FACE_OUT
/** How far a plate's face stands out from the post's axis: the post's radius and a bracket's 2 cm. */
export const FACE_OUT = POST_RADIUS + m(0.02);

// src/render/signs.ts:38-38 variable BLADE_HALF
/** Half the thickness of a street's name plate: its two blades back to back, on top of the post. */
export const BLADE_HALF = m(0.005);

// src/render/signs.ts:41-41 function onTopOfPost
/** Whether the plate stands on top of its post (a street's name, two blades) rather than on its front. */
export const onTopOfPost = (type: SignType): boolean => …;

// src/render/signs.ts:44-48 function plateFaces
/** The faces of a plate whose face looks along world (sin yaw, -cos yaw) */
export function plateFaces(type: SignType, x: number, y: number, yaw: number): { x: number; y: number; yaw: number }[];

// src/render/signs.ts:62-148 function plateCanvas
function plateCanvas(type: SignType, text: string): HTMLCanvasElement;

// src/render/signs.ts:151-194 function streetChain
/** The segments a street's name runs along: the one under the point, carried straight on through its nodes. */
export function streetChain(net: Network, at: Vec2): SegmentId[];

// src/render/signs.ts:198-198 variable CELL
/** DO NOT let a sheet grow past `PAGE`: add a sheet. */
/** A cell is 256 wide (a plate canvas's width), with a gutter so the mipmaps do not bleed into the neighbours. */
const CELL = 256;

// src/render/signs.ts:205-205 variable CELL_HEIGHTS
/** The heights a cell comes in: a street's or a direction's name plate */
const CELL_HEIGHTS = [80, CELL] as const;

// src/render/signs.ts:208-228 interface Sheet
export interface Sheet {
  texture: CanvasTexture;
  /** One material per sheet, made with the sheet and kept while it is (never per build of the signs). */
  readonly material: MeshStandardMaterial;
  readonly cellHeight: number;
  rows: number;
  readonly free: number[];
  /** Cells holding a plate. */
  used: number;
  /** The sheet's size in pixels: its canvas is let go once on the GPU (`SignSheets.flush`). */
  width: number;
  height: number;
  /** Each cell's plate (type and words): what the sheet is painted again from, never a picture kept. */
  readonly plates: Map<number, { readonly type: SignType; readonly text: string }>;
  /** Plates placed since it was last painted. */
  dirty: boolean;
  /** To be painted whole (new, grown, its context lost): its cells cannot be sent one by one onto a texture never sent. */
  whole: boolean;
  /** Cells let go since it was last painted: blank again at the next flush, as a whole painting leaves them. */
  readonly blank: number[];
}

// src/render/signs.ts:231-231 type SheetCopier
/** Copies a picture onto a sheet's texture on the GPU, its corner at `x`, `y` (three's `copyTextureToTexture`, which makes the sheet's mipmaps again, r186). */
export type SheetCopier = (picture: CanvasTexture, sheet: CanvasTexture, x: number, y: number) => void;

// src/render/signs.ts:233-233 interface Placed
/** A plate on a sheet: its cell, the height its picture takes in it, and the last build that read it. */
export interface Placed { readonly sheet: Sheet; readonly cell: number; readonly h: number; build: number }

// src/render/signs.ts:281-286 method SignSheets.take
/** A plate's cell, held for one more standing plate (`drop` lets it go). */
  take(type: SignType, text: string): Placed;

// src/render/signs.ts:289-309 method SignSheets.drop
/** One standing plate fewer reads this picture; the last one frees its cell, and a sheet left empty is let go. */
  drop(type: SignType, text: string): void;

// src/render/signs.ts:317-333 method SignSheets.plateCell
/** The cell of a plate on its sheet (painted there at the next `flush`), and the height its picture takes. */
  plateCell(type: SignType, text: string): Placed;

// src/render/signs.ts:464-464 variable QUAD
/** The corners of three's plane of 1 x 1 (`PlaneGeometry`), and its two triangles. */
export const QUAD = [[-0.5, 0.5], [0.5, 0.5], [-0.5, -0.5], [0.5, -0.5]] as const;

// src/render/signs.ts:465-465 variable QUAD_ORDER
export const QUAD_ORDER = [0, 2, 1, 2, 3, 1] as const;
