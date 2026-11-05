// What the feature uses of src/render/roadTile.worker.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/roadTile.worker.ts:9-9 type KeepAsk
/** One worker of the roads' surface tiles (`roadTilePool.ts`): each tile of a share shaped and meshed whole */
/** Where a share's tiles are kept (`roadTilePool.ts` TileJob.keep). */
type KeepAsk = { readonly prefix: string; readonly keys: readonly (string | null)[] };

// src/render/roadTile.worker.ts:10-13 type Message
type Message =
  | { readonly id: number; readonly ask: MeshAsk & { readonly keep?: KeepAsk } }
  | { readonly id: number; readonly coarse: { readonly arrays: BlockArrays; readonly lock: readonly [number, number, number, number] } }
  | { readonly id: number; readonly heights: Float64Array };

// src/render/roadTile.worker.ts:18-18 variable WRITES_MOST
/** The tiles' writes this worker has handed the store and not seen committed, bytes: past it the next tile waits for them, its answer held (DO NOT post them to the page to keep: past the page's ceiling a tile was dropped and made again next time). */
const WRITES_MOST = 48 * 2 ** 20;

// src/render/roadTile.worker.ts:19-19 variable writes
const writes = new WriteBudget(WRITES_MOST);

// src/render/roadTile.worker.ts:22-38 function keepTiles
/** Each tile of `tiles` with a key kept in the store before its arrays leave (IndexedDB clones the value at `put`); the places of those kept. */
export async function keepTiles(tiles: readonly TileMeshes[], keep: { readonly prefix: string; readonly keys: readonly (string | null)[] }, budget: WriteBudget = writes): Promise<number[]>;

// src/render/roadTile.worker.ts:42-99 function handler
/** The worker's answer to the page's messages, holding the shares that wait on the page for the ground's heights. */
function handler(post: JobScope): (event: MessageEvent<Message>) => void;

// src/render/roadTile.worker.ts:49-52 function handler.beat
const beat = (): void =>;
