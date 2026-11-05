// What the feature uses of src/render/roadTilePool.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/roadTilePool.ts:28-39 interface TileJob
/** Tiles of a road pass to make off the page, and what the page answers for them. */
export interface TileJob {
  readonly pass: PassDesc;
  readonly tiles: readonly { readonly rect: TileRect; readonly into: TilePolys }[];
  /** The roads a share of the tiles reads, its arrays made afresh to be handed over. */
  dataFor(rects: readonly TileRect[]): { data: SurfaceData; transfer: ArrayBuffer[] };
  /** The ground as drawn, read here at the points a worker asks. */
  terrainAt(x: number, y: number): number;
  /** Where the workers keep the tiles they make for a later session (`derivedCache.ts`): the store's prefix and each tile's key, in the tiles' order (null: not to keep). */
  readonly keep?: { readonly prefix: string; readonly keys: readonly (string | null)[] };
  /** A tile (by its place in `tiles`) kept by its worker before its arrays came here: the page keeps nothing of it. */
  onKept?(index: number): void;
}

// src/render/roadTilePool.ts:42-49 interface RoadTiler
/** What makes a road surface's tiles off the page (`roadSurfaceSteps`). */
export interface RoadTiler {
  /** Each tile's surfaces, finished, in the order asked; rejected when they cannot be made off the page. */
  tiles(job: TileJob): Promise<Map<string, Tile>[]>;
  /** Called while waiting: reports a worker with tiles waiting silent too long (queued behind other work or without CPU, still waited for). */
  check(now?: number): void;
  /** A block's far level (`mesh/roadLodPure.ts`) made off the page: `arrays` are handed over */
  coarsen?(arrays: BlockArrays, lock: readonly [number, number, number, number]): Promise<BlockArrays | null>;
}
