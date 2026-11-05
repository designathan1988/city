// What the feature uses of src/render/plantImpostors.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/plantImpostors.ts:255-274 interface PlantImpostors
export interface PlantImpostors {
  /** Each model's quad: four corners, its sphere and its block on the atlas in their attributes. */
  readonly quads: readonly BufferGeometry[];
  /** The atlas's colour and normal pictures, its blocks across and down, and each model's sphere (one unit tall): what the far woods draw from too (`farForest.ts`). */
  readonly albedo: Texture;
  readonly normals: Texture;
  readonly blocks: { readonly x: number; readonly y: number };
  readonly spheres: readonly { readonly centre: Vector3; readonly radius: number }[];
  /** Each model's sphere radius, one unit tall: the size a plant's level is chosen by (`cellInstances.ts`). */
  readonly reach: readonly number[];
  readonly material: MeshStandardMaterial;
  readonly depth: MeshDepthMaterial;
  /** The view the instances were culled for and half the screen's height, device pixels: what the pictures thin by. */
  setView(view: Matrix4, halfHeight: number): void;
  /** Whether the pictures are taken (`takeWaitingPictures`): until then the plants are drawn whole. */
  ready(): boolean;
  /** A mesh drawn with the pictures: the shadow map's size read before its shadow. */
  attach(mesh: InstancedMesh): void;
  dispose(): void;
}
