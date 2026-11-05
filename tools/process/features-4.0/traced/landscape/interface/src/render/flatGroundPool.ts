// What the feature uses of src/render/flatGroundPool.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/flatGroundPool.ts:7-21 interface Job
/** THE MAP'S LAND, WORKED OUT OFF THE PAGE. */

interface Job {
  readonly key: number;
  readonly relief: ReliefVersion;
  /** Where the job lies, for its order (the nearest the view first): world x, y. */
  readonly at: { readonly x: number; readonly y: number };
  readonly cx: number;
  readonly cy: number;
  readonly half: number;
  readonly cell: number;
  readonly side: number;
  /** The terrain's base level, added to the land in the mesh (`TERRAIN_BASE`). */
  readonly base: number;
  readonly done: (mesh: PlateMesh) => void;
  readonly failed: () => void;
}

// src/render/flatGroundPool.ts:86-91 function readFlatGround
/** Reads plate `key`'s corners in a worker, and builds its first mesh from them (`plateMesh.ts`): `done` with it */
export function readFlatGround(job: Job): void;

// src/render/flatGroundPool.ts:94-97 function cancelFlatGround
/** Drops plate `key` from the queue (no longer wanted); a job already running finishes. */
export function cancelFlatGround(key: number): void;
