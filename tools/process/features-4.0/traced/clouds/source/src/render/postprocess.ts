// The code of src/render/postprocess.ts that is the feature's (the rest of the file is shared).

// src/render/postprocess.ts:28-57 slice of src/render/postprocess.ts#PostChain (shared): the feature's statements; "…" marks code left out
export interface PostChain {
  // … 20 lines
  setPlacedClouds(clouds: readonly PlacedCloud[]): void;
// … 7 lines
}

// src/render/postprocess.ts:75-567 slice of src/render/postprocess.ts#createPostChain (shared): the feature's statements; "…" marks code left out
export function createPostChain(
// … 5 lines
): PostChain {
  // … 206 lines
  let placedClouds: readonly PlacedCloud[] = [];
// … 278 lines
}

// src/render/postprocess.ts:642-657 slice of src/render/postprocess.ts#layClouds (shared): the feature's statements; "…" marks code left out
function layClouds(bounds: Vector4[], puffs: Vector4[], lives: number[], bases: number[], shows: number[], placed: readonly PlacedCloud[], drift: { x: number; y: number },
  about: { x: number; y: number }): number {
  // … 1 lines
  for (const cloud of placed) {
    if (slot >= MAX_CLOUDS) break;
    // Where the wind has carried it, its repeat round the view, thinning away near the edge it wraps round.
    const at = driftedCloud(cloud, drift, about);
    const height = cloud.height;
    layOne(slot, 1_000_003 + cloud.id * 7919, at.x, -at.y, height, cloud.size, cloud.yaw, cloud.density, 1, bounds, puffs, lives);
    shows[slot] = at.show;
    // Each its own flat base, at its own height
    bases[slot] = height;
    slot++;
  }
// … 1 lines
}
