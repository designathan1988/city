// What the feature uses of src/render/mesh/surfaceTiles.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/mesh/surfaceTiles.ts:55-89 function mergeTiles
/** DO NOT pass a source that returns `parts`: the closure would hold every tile's arrays for as long as the mesh lives. */
export function mergeTiles(parts: readonly Tile[], options: Pick<SurfaceMeshOptions,
  'name' | 'material' | 'castShadow' | 'receiveShadow'>, source: () => readonly Tile[],
  far?: { readonly lock: readonly [number, number, number, number]; readonly near: boolean; readonly start?: (mesh: Mesh, arrays: BlockArrays) => void;
    /** The other surfaces' blocks' boxes, for a block of several (`BlockLevels.also`). */
    readonly also?: readonly (readonly [number, number, number, number])[];
    /** The road block's cell (`BlockLevels.cell`). */
    readonly cell?: string }): Mesh | null;

// src/render/mesh/surfaceTiles.ts:92-98 function cloneBlock
/** A block's arrays copied: what a worker is handed to simplify while the block keeps its own. */
export function cloneBlock(arrays: BlockArrays): BlockArrays;

// src/render/mesh/surfaceTiles.ts:101-101 interface MemberSize
/** A surface's share of a block: its tiles' vertices and indices (`memberSizes`). */
export interface MemberSize { readonly vertices: number; readonly count: number }

// src/render/mesh/surfaceTiles.ts:104-110 function memberSizes
/** Each surface's share of a block, from its tiles (in the order they were merged): what `splitBlock` cuts by, without the tiles. */
export function memberSizes(members: readonly (readonly Tile[])[]): MemberSize[];

// src/render/mesh/surfaceTiles.ts:114-129 function splitBlock
/** A block's arrays cut back into its surfaces' (`members`: each surface's share, in the order they were merged), each a copy: what a block
 *  of several surfaces drawn alike hands its workers to simplify, a surface at a time, as each was simplified when it was a block of its own. */
export function splitBlock(arrays: BlockArrays, members: readonly MemberSize[]): BlockArrays[];

// src/render/mesh/surfaceTiles.ts:132-154 function joinBlocks
/** Blocks' arrays end to end, as one mesh's (the indices shifted). */
export function joinBlocks(parts: readonly BlockArrays[]): BlockArrays;

// src/render/mesh/surfaceTiles.ts:157-160 interface BlockMember
/** One surface of a block: its tiles, and its own block's edge, locked when it is simplified (`coarseArrays`). */
export interface BlockMember {
  readonly source: () => readonly Tile[];
  readonly lock: readonly [number, number, number, number];
}

// src/render/mesh/surfaceTiles.ts:163-181 function attachFarLevel
/** The block's far level (`roadLod.ts`) as it arrives from its worker; `members`, each surface it draws (one, or several drawn alike), each simplified on its own. */
export function attachFarLevel(mesh: Mesh, coarse: BlockArrays, options: Pick<SurfaceMeshOptions,
  'name' | 'material' | 'castShadow' | 'receiveShadow'>, members: readonly BlockMember[], near: boolean): Mesh;

// src/render/mesh/surfaceTiles.ts:215-217 function blockArraysOf
/** A block's arrays from its tiles as kept (the store's), for the attributes its material reads: a copy end to end, nothing cut (`roadResidency.ts` makeNear). */
export function blockArraysOf(tiles: readonly Tile[], material: Material): BlockArrays | null;

// src/render/mesh/surfaceTiles.ts:220-223 function tilesOnGpu
/** A merged mesh's arrays let go once on the GPU, and made again from `source`'s tiles when asked (`gpuOnly`). */
export function tilesOnGpu(mesh: Mesh, source: () => readonly Tile[]): void;

// src/render/mesh/surfaceTiles.ts:226-230 function blockBox
/** The box round a block's tiles' rectangles, [minX, minY, maxX, maxY]. */
export function blockBox(rects: readonly TileRect[]): [number, number, number, number];

// src/render/mesh/surfaceTiles.ts:233-241 function concat
/** Arrays end to end, with where each ends: one buffer to hand over instead of one an array. */
export function concat(parts: readonly ArrayLike<number>[]): { lineEnds: Uint32Array; lines: Float64Array };
