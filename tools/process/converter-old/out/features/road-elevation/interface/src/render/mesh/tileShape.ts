// What the feature uses of src/render/mesh/tileShape.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/mesh/tileShape.ts:12-16 interface PieceShape
/** THE SHAPE OF A SURFACE, WITHOUT ITS HEIGHTS */

/** One compact piece's triangulation: its vertices, its triangles as clipped and refined, and whether they run clockwise. */
export interface PieceShape {
  readonly xy: Float64Array;
  readonly tris: Uint32Array;
  readonly flip: boolean;
}

// src/render/mesh/tileShape.ts:18-21 interface SkirtRing
/** One ring of an outline's skirt: its edges [ax, ay, bx, by, ...] in order, with its polygon's winding. */
export interface SkirtRing {
  readonly flip: boolean;
  readonly edges: Float64Array;
}

// src/render/mesh/tileShape.ts:23-26 interface SurfaceShape
/** A surface's shape: its pieces and, for a surface with a bottom, its skirts. */
export interface SurfaceShape {
  readonly pieces: readonly PieceShape[];
  readonly skirts: readonly SkirtRing[] | null;
}

// src/render/mesh/tileShape.ts:504-504 type TileRect
/** An axis-aligned rectangle a surface has been cut to: [minX, minY, maxX, maxY]. */
export type TileRect = readonly [number, number, number, number];
