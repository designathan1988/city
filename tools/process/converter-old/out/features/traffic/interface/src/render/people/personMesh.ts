// What the feature uses of src/render/people/personMesh.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/people/personMesh.ts:7-19 interface PersonMeshData
/** A person as a mesh: the morphed MakeHuman body and the helper shells that morph with it */

export interface PersonMeshData {
  readonly vertexCount: number;
  /** Quads (a, b, c, d), d === c for a triangle. */
  readonly faces: Uint16Array;
  readonly uvs?: Float32Array;
  readonly faceUvs?: Uint16Array;
  readonly faceGroup: Uint8Array;
  readonly faceGroups: readonly string[];
  readonly vertexGroups: Readonly<Record<string, readonly (readonly [number, number])[]>>;
  readonly joints: Uint8Array;
  readonly weights: Uint16Array;
  readonly boneNames: readonly string[];
}
