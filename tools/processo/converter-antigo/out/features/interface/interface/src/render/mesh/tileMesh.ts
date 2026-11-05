// What the feature uses of src/render/mesh/tileMesh.ts (shared with the program): declarations as the source writes them, bodies left out.

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
