// What the feature uses of src/render/mesh/tileMesh.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/mesh/tileMesh.ts:7-7 type HeightFn
/** ONE TILE OF A ROAD SURFACE MESHED FROM ITS SHAPE (`tileShape.ts`) */

export type HeightFn = (x: number, y: number) => number;

// src/render/mesh/tileMesh.ts:9-9 type UvFn
/** `u` runs across the surface and `v` along it, both in world units. */
export type UvFn = (x: number, y: number, out: [number, number]) => void;

// src/render/mesh/tileMesh.ts:11-11 type UvFrameFn
/** The UV of (x, y), in the frame of whichever road is nearest to (pickX, pickY) rather than to the point itself. */
export type UvFrameFn = (x: number, y: number, pickX: number, pickY: number, out: [number, number]) => void;

// src/render/mesh/tileMesh.ts:13-13 type TintFn
/** Writes a linear RGB tint for one vertex. */
export type TintFn = (x: number, y: number, out: [number, number, number]) => void;

// src/render/mesh/tileMesh.ts:16-31 interface TileMeshOptions
/** What a tile's mesh reads per vertex (`SurfaceMeshOptions` without its polygons and material). */
export interface TileMeshOptions {
  /** Height of the visible top face. */
  readonly top: HeightFn;
  /** Height of the bottom face of the skirt drawn down from the boundary. */
  readonly bottom?: HeightFn;
  readonly uv: UvFn;
  /** Keeps every triangle inside ONE texture frame. */
  readonly uvFrame?: UvFrameFn;
  readonly uvWorld?: number;
  /** Per-vertex tint, multiplied into the material's colour. */
  readonly tint?: TintFn;
  /** Per-vertex place in the road's cross-section, for the material's marks along its edges */
  readonly section?: TintFn;
  /** Extra UV scale for the skirt, so its texture is not stretched. */
  readonly skirtUvScale?: number;
}

// src/render/mesh/tileMesh.ts:158-169 interface Tile
/** One tile's worth of a surface: its top faces and the skirts along its part of the outline */
export interface Tile {
  readonly positions: Float32Array;
  readonly normals: Float32Array;
  readonly uvs: Float32Array;
  /** Empty for a surface with no tint (one colour, white) or no section (none: 0, -1, 0): filled where a material reads it (`surfaceTiles.ts` tileArrays). */
  readonly colors: Float32Array;
  readonly sections: Float32Array;
  /** Local to the tile. */
  readonly indices: Uint32Array;
  /** The tile's box, [minX, minY, minZ, maxX, maxY, maxZ] of its positions, made with it (in the worker) */
  readonly box?: Float32Array;
}

// src/render/mesh/tileMesh.ts:172-181 function meshTileShape
/** One tile of a surface from its shape (`tileShape.ts`, made here or by a worker, `roadTile.worker.ts`) */
export function meshTileShape(shape: SurfaceShape, options: TileMeshOptions): Tile;
