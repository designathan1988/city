// The code of src/render/roadTile.worker.ts that is the feature's (the rest of the file is shared).

// src/render/roadTile.worker.ts:10-13 slice of src/render/roadTile.worker.ts#Message (shared): the feature's statements; "…" marks code left out
type Message =
  | { readonly id: number; readonly ask: MeshAsk & { readonly keep?: KeepAsk } }
  | { readonly id: number; readonly coarse: { readonly arrays: BlockArrays; readonly lock: readonly [number, number, number, number] } }
  | { readonly id: number; readonly heights: Float64Array };

// src/render/roadTile.worker.ts:22-38 slice of src/render/roadTile.worker.ts#keepTiles (shared): the feature's statements; "…" marks code left out
export async function keepTiles(tiles: readonly TileMeshes[], keep: { readonly prefix: string; readonly keys: readonly (string | null)[] }, budget: WriteBudget = writes): Promise<number[]> {
  // … 3 lines
  for (let i = 0; i < tiles.length; i++) {
    const key = keep.keys[i];
    if (!key) continue;
    try {
      await keepDerivedQueuedMany([[keep.prefix + key, new Map(tiles[i]!)]], budget);
      kept.push(i);
    } catch (error) {
      // Not kept: the page keeps it as it did before (`roadSurfaces.ts` keep).
      console.error('[roads] a road tile could not be kept by its worker', error);
    }
  }
// … 1 lines
}

// src/render/roadTile.worker.ts:42-99 slice of src/render/roadTile.worker.ts#handler (shared): the feature's statements; "…" marks code left out
function handler(post: JobScope): (event: MessageEvent<Message>) => void {
  // … 1 lines
  const waiting = new Map<number, { readonly mesher: TileMesher; readonly points: Float64Array; readonly keep: KeepAsk | undefined }>();
  return (event) => {
    // … 7 lines
    try {
      if ('coarse' in data) {
        // … 1 lines
        const made = coarseArrays(data.coarse.arrays, data.coarse.lock);
        post.postMessage({ id, coarse: made }, made ? blockBuffers(made) : []);
      // … 1 lines
      }
      let mesher: TileMesher;
      // … 1 lines
      if ('ask' in data) {
        mesher = new TileMesher(data.ask);
        // … 1 lines
        const points = mesher.need();
        if (points.length > 0) {
          waiting.set(id, { mesher, points, keep });
          post.postMessage({ id, need: points });
          return;
        }
      } else {
        const job = waiting.get(id);
        if (!job) return;
        waiting.delete(id);
        mesher = job.mesher;
        keep = job.keep;
        mesher.give(job.points, data.heights);
      }
      const done = mesher.run(beat);
      if ('need' in done) {
        waiting.set(id, { mesher, points: done.need, keep });
        post.postMessage({ id, need: done.need });
        return;
      }
      const tiles = done.tiles;
      // … 1 lines
      if (keep) {
        void keepTiles(tiles, keep).then((kept) => post.postMessage({ id, tiles, kept }, tileBuffers(tiles)));
        return;
      }
      post.postMessage({ id, tiles }, tileBuffers(tiles));
    } catch (error) {
      // … 2 lines
      waiting.delete(id);
    // … 1 lines
    }
  };
}
