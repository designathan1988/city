// What the feature uses of src/render/buildings/furnitureKit.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/buildings/furnitureKit.ts:17-17 type RGB
/** The furniture people use inside buildings, as models */

type RGB = readonly [number, number, number];

// src/render/buildings/furnitureKit.ts:19-19 function hex
const hex = (h: number): RGB => …;

// src/render/buildings/furnitureKit.ts:21-21 variable OAK
const OAK = hex(0x9a6a43);

// src/render/buildings/furnitureKit.ts:22-22 variable WALNUT
const WALNUT = hex(0x5e3d26);

// src/render/buildings/furnitureKit.ts:23-23 variable LINEN
const LINEN = hex(0xf2efe8);

// src/render/buildings/furnitureKit.ts:24-24 variable DUVET
const DUVET = hex(0x7d93ad);

// src/render/buildings/furnitureKit.ts:25-25 variable FABRIC
const FABRIC = hex(0x6f7f8f);

// src/render/buildings/furnitureKit.ts:26-26 variable CUSHION
const CUSHION = hex(0x84929f);

// src/render/buildings/furnitureKit.ts:27-27 variable BLACK
const BLACK = hex(0x1e2125);

// src/render/buildings/furnitureKit.ts:28-28 variable STEEL
const STEEL = hex(0xb3b9bd);

// src/render/buildings/furnitureKit.ts:29-29 variable CHROME
const CHROME = hex(0xd9dde0);

// src/render/buildings/furnitureKit.ts:30-30 variable WHITE
const WHITE = hex(0xf6f6f3);

// src/render/buildings/furnitureKit.ts:31-31 variable CERAMIC
const CERAMIC = hex(0xeef0ef);

// src/render/buildings/furnitureKit.ts:32-32 variable LEAF
const LEAF = hex(0x4c8a3a);

// src/render/buildings/furnitureKit.ts:33-33 variable LEAF_DARK
const LEAF_DARK = hex(0x386d2b);

// src/render/buildings/furnitureKit.ts:34-34 variable POT
const POT = hex(0xb4673f);

// src/render/buildings/furnitureKit.ts:35-35 variable RED
const RED = hex(0xa22b2b);

// src/render/buildings/furnitureKit.ts:36-36 variable SCREEN
const SCREEN = hex(0x0f1418);

// src/render/buildings/furnitureKit.ts:37-37 variable BOARD
const BOARD = hex(0x2c4a3a);

// src/render/buildings/furnitureKit.ts:38-38 variable BOOKS
const BOOKS: readonly RGB[] = [hex(0x8b2c2c), hex(0x2c4f8b), hex(0x2e6b3f), hex(0xc49a3c), hex(0x5a3b6b), hex(0xd8d2c0)];

// src/render/buildings/furnitureKit.ts:39-39 variable GOODS
const GOODS: readonly RGB[] = [hex(0xd9483b), hex(0xf2b33d), hex(0x3d8ad9), hex(0x5fb34a), hex(0xe8e3d6), hex(0x8b5a2b)];

// src/render/buildings/furnitureKit.ts:52-52 variable ROUND_P,ROUND_B,ROUND_X,ROUND_Y
/** The rounding a box's fragments shade by (`ROUND_GLSL`): each vertex's place in its box's own frame, the box's half size and radius (w 0: none), its axes. */
const ROUND_P = 'aRoundP', ROUND_B = 'aRoundB', ROUND_X = 'aRoundX', ROUND_Y = 'aRoundY';

// src/render/buildings/furnitureKit.ts:56-73 function rbox
/** A box with rounded edges, its bottom at `y`: a plain box (12 triangles), its edges rounded in the shading (`createFurnitureMaterial`).
 *  DO NOT model the rounding: three's RoundedBoxGeometry was 300 triangles a box, a bookshelf of books 55,200 (the audit, 2026-10-22). */
function rbox(w: number, h: number, d: number, x: number, y: number, z: number, c: RGB, r = 0.03): BufferGeometry;

// src/render/buildings/furnitureKit.ts:76-76 function box
/** A box with sharp edges, its bottom at `y`: wood, steel, books, treads, boards (no rounding to shade: their edges are sharp). */
const box = (w: number, h: number, d: number, x: number, y: number, z: number, c: RGB): BufferGeometry => …;

// src/render/buildings/furnitureKit.ts:79-91 function turnY
/** A part turned about the up axis, its rounding's axes turned with it (three turns only its positions and normals). */
function turnY(g: BufferGeometry, angle: number): BufferGeometry;

// src/render/buildings/furnitureKit.ts:94-95 function cyl
/** An upright cylinder, its bottom at `y`. */
const cyl = (r: number, h: number, x: number, y: number, z: number, c: RGB, top = r, seg = 12): BufferGeometry => …;

// src/render/buildings/furnitureKit.ts:97-98 function ball
const ball = (r: number, x: number, y: number, z: number, c: RGB): BufferGeometry => …;

// src/render/buildings/furnitureKit.ts:101-105 function legs
/** Four legs under a top `w` x `d`, inset `inset`, `h` high. */
function legs(w: number, d: number, h: number, c: RGB, inset = 0.06, r = 0.022): BufferGeometry[];

// src/render/buildings/furnitureKit.ts:107-336 function build
function build(kind: FurnitureKind): BufferGeometry[];

// src/render/buildings/furnitureKit.ts:338-338 variable SHADE
const SHADE = hex(0xfff0d0);

// src/render/buildings/furnitureKit.ts:339-339 variable BULB
const BULB = hex(0xfffbe8);

// src/render/buildings/furnitureKit.ts:361-384 function createFurnitureGeometries
/** One geometry per furniture kind, merged, vertex-coloured. */
export function createFurnitureGeometries(): Record<FurnitureKind, BufferGeometry>;

// src/render/buildings/furnitureKit.ts:442-462 function furnitureTones
/** Each kind's colour as the eye takes it in from across a room */
export function furnitureTones(geometry: Readonly<Record<FurnitureKind, BufferGeometry>>): Float32Array;
