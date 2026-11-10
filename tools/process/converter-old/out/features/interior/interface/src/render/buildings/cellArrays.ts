// What the feature uses of src/render/buildings/cellArrays.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/buildings/cellArrays.ts:16-16 variable COLOURED_PARTS
/** A BATCH OF BUILDINGS AS ARRAYS: what `buildingMesh.ts` `meshesOf` wraps in meshes */

/** Parts drawn with a colour per instance: awnings, the window frames and balcony railings in their building's trim (the materials are white), the shell's parts in their paint. */
export const COLOURED_PARTS: ReadonlySet<PartKind> = Object.freeze(new Set<PartKind>(['awning', 'frame', 'railing', 'fin', 'louvre', 'brise', ...FACADE_PROP_KINDS, ...SHELL_PART_KINDS]));

// src/render/buildings/cellArrays.ts:19-23 interface PartBanding
/** What filing a kind reads of its screen (`kit.ts` `PartScreen`), sent with each job (`kitBoxes`): its first coarser level's error, its box (the kit's `Axes`), its finest level's triangles. Here, not in `partFiling.ts`: the batches' boxes carry it, and the filing reads the batches. */
export interface PartBanding {
  readonly first: readonly [number, number, number] | null;
  readonly extent: readonly [number, number, number];
  readonly triangles: number;
}

// src/render/buildings/cellArrays.ts:24-24 type PartBandings
export type PartBandings = Readonly<Record<PartKind, PartBanding>>;

// src/render/buildings/cellArrays.ts:27-35 interface CellSelection
/** What a batch takes: the part kinds, whether the shells and the furniture; the preview's look (`ghost`) or the faded one (`dim`). */
export interface CellSelection {
  readonly parts: readonly PartKind[];
  readonly shells: boolean;
  readonly furniture: boolean;
  readonly ghost: boolean;
  readonly dim: boolean;
  /** The far level's cell (its column and row, `BATCH_CELL`): its shells' places quantized on the cell's grid (`ShellGrid`); none: in floats (the detail's millimetre steps). */
  readonly grid?: readonly [number, number];
}

// src/render/buildings/cellArrays.ts:38-45 interface ShellGrid
/** A cell's grid its far level's shells are laid on (`cellShellGrid`): one origin and one step for every piece of the cell, so a corner two pieces share is one point of it, bit for bit. */
export interface ShellGrid {
  /** The grid's origin, three's axes. */
  readonly x: number;
  readonly y: number;
  readonly z: number;
  /** Metres a step: every axis alike (the mesh's scale is uniform, so its normals stay as they are). */
  readonly step: number;
}

// src/render/buildings/cellArrays.ts:74-74 type Bounds
/** A box and a sphere: minX, minY, minZ, maxX, maxY, maxZ, then centre x, y, z and radius (three's `Box3` and `Sphere`). */
export type Bounds = Float64Array;

// src/render/buildings/cellArrays.ts:77-77 variable SHELL_MESH
/** The name of a batch's one shell mesh (`buildingMesh.ts` `meshesOf`), as `fade.ts` and the memory counters find it. */
export const SHELL_MESH = 'building-shell';

// src/render/buildings/cellArrays.ts:80-80 variable FINISH_ATTRIBUTE
/** The finish id of a shell's part per instance, and of a ruin's piece per vertex (`finishes.ts`): its place in `FINISHES`, its layer of the finishes' maps. */
export const FINISH_ATTRIBUTE = 'shellFinish';

// src/render/buildings/cellArrays.ts:135-150 interface ShellArrays
/** A piece of a batch's shells of every finish, one geometry drawn in one call: 24 bytes a vertex, 44 on the far level's runs of bays with openings */
interface ShellArrays {
  /** Floats, or on its cell's grid (`place`): four unsigned shorts, read normalised and put in place by the mesh's matrix. */
  readonly position: Float32Array | Uint16Array;
  readonly place?: ShellGrid;
  /** The normal, octahedral in two signed shorts (`octNormal`): the uv is worked out from it and the place (`shellParts.ts` `shellUv`), so it needs more than a byte. */
  readonly normal: Int16Array;
  /** The paint's linear rgb (tone and grime in it) over `COLOUR_RANGE`, and the finish id with the wear (`finishWear`, `PAINT_ATTRIBUTE`), unsigned shorts. */
  readonly paint: Uint16Array;
  /** 16 bits: a piece's vertices fit them (`SHELL_PIECE`), but for a single building larger than that. */
  readonly index: Uint16Array | Uint32Array;
  readonly bounds: Bounds;
  /** The far level's runs of bays with openings only (`RunChunk`): each vertex's bay width and storey height (`FACADE_FLOATS`), its grid (`FACADE_GRID` shorts) and its opening (`FACADE_BYTES`, `facadeLevel.ts`). */
  readonly facade?: Uint16Array;
  readonly grid?: Int16Array;
  readonly opening?: Uint8Array;
}

// src/render/buildings/cellArrays.ts:152-163 interface InstanceArrays
interface InstanceArrays {
  readonly count: number;
  readonly poses: Float32Array;
  /** Each instance's tint, linear rgb in half floats (`HALF_FLOAT`): 6 bytes, not 12. */
  readonly instanceColor?: Uint16Array;
  /** Window panes: each one's light slot (-1: none) and the room behind it. */
  readonly litSlot?: Float32Array;
  readonly rooms?: Uint16Array;
  /** The shell's parts: each one's finish id, wear and ground (`shellParts.ts`). */
  readonly looks?: Float32Array;
  readonly bounds: Bounds;
}

// src/render/buildings/cellArrays.ts:165-170 interface CellArrays
export interface CellArrays {
  /** Every finish's shells in pieces of whole buildings whose vertices fit 16-bit indices, then the far level's runs of bays with openings in pieces of their own; none when the batch takes none or has none. */
  readonly shells: readonly ShellArrays[];
  readonly parts: Readonly<Partial<Record<PartKind, InstanceArrays>>>;
  readonly furniture: Readonly<Partial<Record<FurnitureKind, InstanceArrays>>>;
}

// src/render/buildings/cellArrays.ts:173-178 interface KitBoxes
/** Each part's and each piece of furniture's geometry box (minX, minY, minZ, maxX, maxY, maxZ), as the kit draws them. */
export interface KitBoxes {
  readonly parts: Readonly<Record<PartKind, readonly number[]>>;
  readonly furniture: Readonly<Partial<Record<FurnitureKind, readonly number[]>>>;
  /** What filing a cell's parts by band reads of each kind's screen, as the kit has them now (`partFiling.ts`). */
  readonly bandings?: PartBandings;
}

// src/render/buildings/cellArrays.ts:182-217 function cellArrays
/** The batch of `chunks` that `selection` takes, as arrays. */
/** `groups`: each chunk's square (`cellWork.ts` gives the shells quadrants of two detail cells), its shells then pieced square by square: each piece's box that square's, so three leaves out of the view and out of each sun cascade the squares outside them (a piece of buildings from across the whole cell was drawn whole for a corner of it in view). */
export function cellArrays(chunks: readonly BuildingChunk[], selection: CellSelection, boxes: KitBoxes, groups?: readonly string[]): CellArrays;

// src/render/buildings/cellArrays.ts:227-246 interface CellMass
/** A cell's buildings from afar as one mesh's arrays (`massCells.ts`) */
export interface CellMass {
  /** Floats, or on its square's grid (`place`, `massGrid`): four unsigned shorts, read normalised and put in place by the mesh's matrix. */
  readonly position: Float32Array | Uint16Array;
  readonly place?: ShellGrid;
  /** Four signed bytes a vertex over 127 (x, y, z, and one left over: no 3-byte format in Direct3D 11): lighting only, no uv worked out from it (the shells' are shorts, `normal`). */
  readonly normal: Int8Array;
  /** Linear rgb and a fourth left over, half floats (`HALF_FLOAT`, `massCells.ts` `cellMesh`): 11 bits of mantissa, more than a colour shows. */
  readonly color: Uint16Array;
  /** Along its face and its windows' first storey's foot, m: the long and the high, full floats (`MASS_FACADE`). */
  readonly facade: Float32Array;
  /** Its bay's width and storey's height and its glass share, and a fourth left over, half floats (`MASS_GRID`). */
  readonly grid: Uint16Array;
  /** Its rooms' seed (the lot of each lit window, `massingMaterial.ts`) and the office flag over it: a full float, a seed's fraction kept whole. */
  readonly seed: Float32Array;
  readonly index: Uint16Array | Uint32Array;
  readonly bounds: Bounds;
  readonly top: number;
  /** The tallest thing its level left out, m (`MassingChunk.error`): 0 for a cell's, what a tile's or a plate's choice reads (`massCells.ts`). */
  readonly error: number;
}

// src/render/buildings/cellArrays.ts:325-325 variable MASS_FACADE
/** A mass vertex's numbers: its facade's full floats, its grid's half floats (`CellMass`): 44 bytes a vertex with its place, normal, colour and seed (52 in full floats). */
export const MASS_FACADE = 2;

// src/render/buildings/cellArrays.ts:326-326 variable MASS_GRID
export const MASS_GRID = 4;

// src/render/buildings/cellArrays.ts:459-482 function partArrays
/** A batch's own instance arrays from its chunks, in order: the matrices, the colours of a coloured kind */
function partArrays(chunks: readonly BuildingChunk[], kind: PartKind, count: number, glassy: boolean, coloured: boolean, looked: boolean): {
  poses: Float32Array; instanceColor?: Uint16Array; litSlot?: Float32Array; rooms?: Uint16Array; looks?: Float32Array;
};

// src/render/buildings/cellArrays.ts:485-495 function furniturePoses
/** The furniture of one kind of every chunk, its poses in chunk order. */
function furniturePoses(chunks: readonly BuildingChunk[], kind: FurnitureKind, count: number): Float32Array;

// src/render/buildings/cellArrays.ts:545-550 function instanceBounds
/** An instanced batch's box and sphere as three's `InstancedMesh.computeBoundingBox` works the box out */
/** DO NOT end the loop over the instances with the box's sums in the same function: entered on the stack (OSR) on its first long batch, its code never saw the sums run and left on them for every batch after (138 times a job), each batch's start run unoptimised, every number boxed. */
function instanceBounds(poses: Float32Array, count: number, box: readonly number[]): Bounds;
