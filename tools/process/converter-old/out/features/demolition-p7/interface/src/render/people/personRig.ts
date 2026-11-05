// What the feature uses of src/render/people/personRig.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/people/personRig.ts:110-116 interface WornPiece
/** A MakeHuman item fitted to a rigged body, as a separate skinned piece. */
export interface WornPiece {
  /** Vertices in the body's bind posture, metres. */
  readonly positions: Float32Array;
  /** Bones in the skeleton's order, four a vertex, and their weights. */
  readonly joints: Uint16Array;
  readonly weights: Float32Array;
}

// src/render/people/personRig.ts:145-150 interface RigDeltas
/** A body's shape and posture as `deltasAt` reads them: its positions (decimetres), its lowest body vertex, and each bone's posture as a 3 x 3 (column-major, nine a bone). */
export interface RigDeltas {
  readonly positions: Float32Array;
  readonly lowestDm: number;
  readonly bodyRange: readonly (readonly [number, number])[];
  readonly linear: Float64Array;
}
