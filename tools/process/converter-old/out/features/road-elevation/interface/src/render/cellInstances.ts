// What the feature uses of src/render/cellInstances.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/cellInstances.ts:111-116 interface Pack
/** A thing's instances of one kind: placements (`stride` floats each), tints, each one's largest scale. */
export interface Pack {
  readonly data: Float32Array;
  readonly colours: Float32Array | null;
  readonly scales: Float32Array;
  readonly count: number;
}
