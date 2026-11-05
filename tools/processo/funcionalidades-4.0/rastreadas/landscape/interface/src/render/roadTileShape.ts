// What the feature uses of src/render/roadTileShape.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/roadTileShape.ts:11-14 type TileSource
/** What each surface of a tile is made from. */
export type TileSource =
  | { readonly kind: 'band'; readonly band: 'verge' | 'footway' | 'kerb' | 'asphalt' | 'curbRamp' }
  | { readonly kind: 'median'; readonly part: 'kerb' | 'planting' | 'paved' }
  | { readonly kind: 'paint'; readonly color: string };

// src/render/roadTileShape.ts:25-31 interface TilePolys
/** The rings that reach a tile, by kind, in input order. */
export interface TilePolys {
  readonly levels: Record<'casing' | 'sidewalk' | 'curb' | 'asphalt', readonly Poly[]>;
  readonly medians: Record<'kerb' | 'planting' | 'paved', readonly Poly[]>;
  readonly ramps: readonly Poly[];
  readonly ribbons: readonly Poly[];
  readonly quads: ReadonlyMap<string, readonly Poly[]>;
}
