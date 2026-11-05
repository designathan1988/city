// What the feature uses of src/render/networkFarRoadsBuild.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/networkFarRoadsBuild.ts:31-37 interface FarRoadCell
/** One cell's layer: positions in the scene (x, height, -y), a colour a vertex, the triangles. */
export interface FarRoadCell {
  readonly key: string;
  readonly layer: number;
  readonly positions: Float32Array;
  readonly colors: Float32Array;
  readonly indices: Uint32Array;
}
