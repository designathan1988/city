// What the feature uses of src/render/roadSurfaces.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/roadSurfaces.ts:45-79 interface SurfaceReuse
/** The road network's surface bands */

/** What one build of the surfaces keeps for the next, so an edit rebuilds only the tiles it reaches. */
export interface SurfaceReuse {
  /** DO NOT keep a tile's arrays here */
  readonly tiles: Map<string, Map<number, KeptTile>>;
  /** What each tile's ground and heights digested to at the last build (`dependsOn`), by pass and tile key: a tile no changed block reaches keeps it (DO NOT digest every tile of the region at every build: ~6,600 digests, 700 ms on the page at each tile a streamed world put in, 2026-10-16). */
  readonly depends?: Map<string, Map<number, number>>;
  /** A digest of everything the height field and the texture frames read in a rectangle */
  readonly dependsOn: (minX: number, minY: number, maxX: number, maxY: number) => number;
  /** The paint materials, by colour: made once, not on every rebuild. */
  readonly paint: Map<string, Material>;
  /** The meshes of each block of tiles */
  readonly chunks: Map<string, { readonly parts: readonly number[]; readonly mesh: Mesh }>;
  /** Block meshes replaced but maybe still drawn: freed by the owner once the new roads are in. */
  readonly retired?: Mesh[];
  /** Whether a block at (x, y), world units, is near the camera: made at full detail, its far level hidden */
  readonly near?: (x: number, y: number) => boolean;
  /** Whether a tile of the roads is built here at all (the opening's view, `renderer.ts` openingWithin): a tile left out is built by a later build. None: every tile. */
  within?: (rect: TileRect) => boolean;
  /** Where `within`'s answer changed since the last build finished; null: it cannot say (every tile visited). */
  withinMoved?: (readonly [number, number, number, number])[] | null;
  /** Each pass as the last build left it (`PassState`): a build visits only what changed. None: every tile at every build. */
  readonly passes?: Map<string, PassState>;
  /** Tiles kept from earlier builds and sessions (`derivedCache.ts`), by `storedKey`: what is kept is known, a tile read only when a build uses it */
  readonly stored?: StoredTiles;
  /** The passes' inputs, kept road by road and node by node between builds (`roadSurfaceInputs.ts`) */
  readonly inputs?: SurfaceInputs;
  /** Where the eye is when a build ends: the inputs held round the view (`InputResidency`) are brought back to it. None: they stay as the build left them. */
  readonly inputsEye?: () => { readonly x: number; readonly y: number } | null;
  /** Each tile built afresh, to be kept for a later session */
  readonly keep?: (key: string, bundle: TileBundle) => void;
  /** The store's prefix the road tile workers keep the tiles they make under (none: the page keeps them, `keep`); `kept` tells the page a key is kept. */
  readonly workersKeep?: string | null;
  readonly kept?: (key: string) => void;
  /** The batches the blocks' far levels are drawn in, one a look (`farBlockBatches.ts`). None: each far level a mesh of its own. */
  readonly farBatches?: FarBlockBatches;
}

// src/render/roadSurfaces.ts:92-97 interface StoredTiles
/** The tiles kept off the page (`derivedCache.ts`) */
export interface StoredTiles {
  /** Whether a tile is kept. */
  has(key: string): boolean;
  /** The kept tiles of these keys, read from the store: never the whole of it. */
  fetch(keys: readonly string[]): Promise<ReadonlyMap<string, TileBundle>>;
}

// src/render/roadSurfaces.ts:118-118 type TileBundle
/** Every surface of one tile, by mesh name. */
export type TileBundle = ReadonlyMap<string, Tile>;

// src/render/roadSurfaces.ts:120-120 type KeptTile
/** A tile kept between builds: the names of the surfaces it has. Its digest is its key. */
type KeptTile = readonly string[];

// src/render/roadSurfaces.ts:125-135 interface PassTile
/** One tile of a pass in a build: its digest and block, its surfaces and arrays once known. DO NOT keep its rings with it: read from the
 *  inputs when it is built (`SurfaceInputs.reachOf`, held round the view), a tile kept in the pass's state held them for good, and the
 *  fill after a large city's opening every tile of the map's until it was over. */
interface PassTile {
  /** Its key among the pass's tiles (`roadSurfaceInputs.ts` tileKey). */
  readonly key: number;
  readonly value: number;
  /** Its name in the store (`storedKey`). */
  readonly stored: string;
  readonly chunk: string;
  readonly rect: TileRect;
  surfaces: KeptTile | null;
  bundle: TileBundle | null;
}

// src/render/roadSurfaces.ts:138-146 interface PassState
/** A pass of the surfaces as the last build left it (`roadSurfaceSteps`): the constants it was built with, its inputs' birth, its tiles by key (no arrays), the tiles of each block and, for each surface, its blocks by the first tile holding it (the order a fresh build draws them in). */
export interface PassState {
  readonly salt: number;
  readonly born: number;
  readonly tiles: Map<number, PassTile>;
  readonly chunkTiles: Map<string, Set<number>>;
  readonly blocks: Map<string, Map<string, number>>;
  /** Blocks whose tiles changed and that a build replaced halfway did not merge yet: merged by the next. */
  readonly pending: Set<string>;
}
